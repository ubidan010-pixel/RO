#include "precompiled_gameplay.h"

#ifndef _ITF_SOFTPLATFORM_H_
#include "gameplay/components/Environment/SoftPlatform.h"
#endif //_ITF_SOFTPLATFORM_H_

#ifndef _ITF_PHYSSHAPES_H_
#include "engine/physics/PhysShapes.h"
#endif //_ITF_PHYSSHAPES_H_

#ifndef _ITF_PHYSBODY_H_
#include "engine/physics/PhysBody.h"
#endif //_ITF_PHYSBODY_H_

#ifndef _ITF_PHYSCONSTRAINT_H_
#include "engine/physics/PhysConstraint.h"
#endif //_ITF_PHYSCONSTRAINT_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_PHYSPHANTOM
#include "engine/physics/PhysPhantom.h"
#endif //_ITF_PHYSPHANTOM

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

namespace ITF
{

SoftPlatform::SoftPlatform()
: m_owner(NULL)
, m_depth(0.f)
, m_gravity(0.f,-40.f)
, m_phantom(NULL)
, m_polyline(NULL)
, m_movingPolyForce(0.f)
, m_weightMultiplier(1.f)
, m_landSpeedMultiplier(1.f)
, m_hitForceMultiplier(1.f)
, m_impulseMultiplier(1.f)
, m_registered(bfalse)
{

}

SoftPlatform::~SoftPlatform()
{
    clear();
    setOwner(NULL);
}

void SoftPlatform::clear()
{
    FixedArray <ObjectRef,32> detachActors;

    UsersList::iterator userIt = m_users.begin();

    while ( userIt != m_users.end() )
    {
        User& user = *userIt;

        detachActors.push_back(user.m_actor);

        if ( detachActors.full() )
        {
            break;
        }

        ++userIt;
    }

    HangingList::iterator hangingIt = m_hanging.begin();

    while ( hangingIt != m_hanging.end() )
    {
        Hanging& hanger = *hangingIt;

        detachActors.push_back(hanger.m_actor);

        if ( detachActors.full() )
        {
            break;
        }

        ++hangingIt;
    }

    EventDetach detachEvent;

    for ( u32 i = 0; i < detachActors.size(); i++ )
    {
        Actor* actor = AIUtils::getActor(detachActors[i]);

        if ( actor )
        {
            actor->onEvent(&detachEvent);
        }
    }

    u32 numBodies = m_bodies.size();
    u32 numConstraints = m_constraints.size();

    for ( u32 i = 0; i < numBodies; i++ )
    {
        BodyData& data = m_bodies[i];

        if ( data.m_body )
        {
            if ( m_registered )
            {
                PHYSWORLD->removeBody(data.m_body);
            }

            SF_DEL(data.m_shape);
            PHYSWORLD->deallocBody(data.m_body);
        }
    }

    for ( u32 i = 0; i < numConstraints; i++ )
    {
        ConstraintData& data = m_constraints[i];

        if ( data.m_constraint )
        {
            if ( m_registered )
            {
                PHYSWORLD->removeConstraint(data.m_constraint);
            }

            PHYSWORLD->deallocConstraint(data.m_constraint);
        }
    }

    m_bodies.clear();
    m_constraints.clear();

    if ( m_phantom )
    {
        if ( m_registered )
        {
            PHYSWORLD->removePhantom(m_phantom);
        }

        PHYSWORLD->deallocPhantom(m_phantom);
    }

    if ( m_polyline )
    {
        SF_DEL(m_polyline->m_physShape);
        SF_DEL(m_polyline);
    }
}

void SoftPlatform::setOwner( Actor* _owner )
{
    if ( m_owner != _owner )
    {
        if ( m_owner )
        {
            m_owner->unregisterEvent(ITF_GET_STRINGID_CRC(EventStickOnPolyline,471231076),this);
            m_owner->unregisterEvent(ITF_GET_STRINGID_CRC(EventStickOnPolylineChangeEdge,4222832239),this);
            m_owner->unregisterEvent(ITF_GET_STRINGID_CRC(EventHanging,2826410601),this);
            m_owner->unregisterEvent(ITF_GET_STRINGID_CRC(EventHangUpdate,3049416002),this);
            m_owner->unregisterEvent(ITF_GET_STRINGID_CRC(HitStim,2121727796),this);
            m_owner->unregisterEvent(ITF_GET_STRINGID_CRC(EventRopeSwingImpulse,807862862),this);
            m_owner->unregisterEvent(ITF_GET_STRINGID_CRC(EventBouncedOnPolyline,1421560006),this);
        }

        m_owner = _owner;

        if ( m_owner )
        {
            ACTOR_REGISTER_EVENT_COMPONENT(m_owner,ITF_GET_STRINGID_CRC(EventStickOnPolyline,471231076),this);
            ACTOR_REGISTER_EVENT_COMPONENT(m_owner,ITF_GET_STRINGID_CRC(EventStickOnPolylineChangeEdge,4222832239),this);
            ACTOR_REGISTER_EVENT_COMPONENT(m_owner,ITF_GET_STRINGID_CRC(EventHanging,2826410601),this);
            ACTOR_REGISTER_EVENT_COMPONENT(m_owner,ITF_GET_STRINGID_CRC(EventHangUpdate,3049416002),this);
            ACTOR_REGISTER_EVENT_COMPONENT(m_owner,ITF_GET_STRINGID_CRC(HitStim,2121727796),this);
            ACTOR_REGISTER_EVENT_COMPONENT(m_owner,ITF_GET_STRINGID_CRC(EventRopeSwingImpulse,807862862),this);
            ACTOR_REGISTER_EVENT_COMPONENT(m_owner,ITF_GET_STRINGID_CRC(EventBouncedOnPolyline,1421560006),this);
        }
    }
}

void SoftPlatform::init( const SoftPlatformInitData& _initData )
{
    clear();

    Vec2d bonePos = Vec2d::Zero;
    PhysBodyInitCommonInfo bodyInfo;

    SET_OWNER(bodyInfo, m_owner->getRef());
    bodyInfo.m_userData = m_owner->getRef();
    bodyInfo.m_collisionGroup = ECOLLISIONGROUP_NONE;
    bodyInfo.m_collisionFilter = ECOLLISIONFILTER_NONE;
    bodyInfo.m_angle = 0.f;
    bodyInfo.m_weight = 1.f;

    u32 numBodies = _initData.m_bodies.size();

    m_bodies.resize(numBodies);

    for ( u32 i = 0; i < numBodies; i++ )
    {
        const BodyInitData& bodyInitData = _initData.m_bodies[i];
        BodyData& bodyData = m_bodies[i];

        bodyInfo.m_pos = bodyInitData.m_pos;
        bodyInfo.m_static = bodyInitData.m_static;

        bodyData.m_shape = newAlloc(mId_Phys,PhysShapeCircle(0.f));
        bodyData.m_body = PHYSWORLD->allocBody(bodyData.m_shape,bodyInfo);
        bodyData.m_body->setGravityMultiplier(bodyInitData.m_gravityMultiplier);
        bodyData.m_body->setWindMultiplier(bodyInitData.m_windMultiplier);
        bodyData.m_constraintIndex = U32_INVALID;

        if ( m_registered )
        {
            PHYSWORLD->insertBody(bodyData.m_body,_initData.m_depth);
        }
    }

    PhysConstraintInitCommonInfo constraintInfo;

    u32 numConstraints = _initData.m_constraints.size();

    m_constraints.resize(numConstraints);

    for ( u32 i = 0; i < numConstraints; i++ )
    {
        const ConstraintInitData& constraintInitData = _initData.m_constraints[i];
        ConstraintData& constraintData = m_constraints[i];

        constraintData.m_bodyAIndex = constraintInitData.m_bodyAIndex;
        constraintData.m_bodyBIndex = constraintInitData.m_bodyBIndex;

        if ( constraintData.m_bodyAIndex >= m_bodies.size() ||
             constraintData.m_bodyBIndex >= m_bodies.size() )
        {
            constraintData.m_constraint = NULL;
            continue;
        }

        BodyData& bodyDataA = m_bodies[constraintInitData.m_bodyAIndex];
        BodyData& bodyDataB = m_bodies[constraintInitData.m_bodyBIndex];
        PhysBody* bodyA = bodyDataA.m_body;
        PhysBody* bodyB = bodyDataB.m_body;

        if ( bodyDataA.m_constraintIndex == U32_INVALID )
        {
            bodyDataA.m_constraintIndex = i;
        }

        f32 bodyDist = ( bodyB->getPos() - bodyA->getPos() ).norm();

        constraintInfo.m_bodyA = bodyA;
        constraintInfo.m_bodyB = bodyB;
        constraintInfo.m_length = bodyDist;
        constraintInfo.m_minAngle = constraintInitData.m_minAngle;
        constraintInfo.m_maxAngle = constraintInitData.m_maxAngle;
        constraintInfo.m_minLength = constraintInitData.m_minLength;
        constraintInfo.m_maxLength = constraintInitData.m_maxLength;
        constraintInfo.m_stiff = constraintInitData.m_stiff;
        constraintInfo.m_damp = constraintInitData.m_damp;
        constraintInfo.m_limitAngle = constraintInitData.m_limitAngle;
        constraintInfo.m_relaxLength = constraintInitData.m_relaxLength;
        constraintData.m_constraint = PHYSWORLD->allocConstraint(constraintInfo);

        if ( m_registered )
        {
            PHYSWORLD->insertConstraint(constraintData.m_constraint);
        }
    }

    if ( _initData.m_phantom || _initData.m_movingPolyForce )
    {
        m_polyline = newAlloc(mId_Gameplay, PolyLine);
        m_polyline->m_physShape = newAlloc(mId_Phys,PhysShapePolyline(m_polyline));
        m_polyline->m_isCollisionActive = bfalse;
        m_polyline->setOwnerActor(m_owner);

        for ( u32 i = 0; i < numBodies; i++ )
        {
            m_polyline->addPoint(m_bodies[i].m_body->getPos());
        }

        m_polyline->setGameMaterial(_initData.m_gameMaterial);

        PhysPhantomInitCommonInfo initInfo;

        SET_OWNER(initInfo, m_owner->getRef());
        initInfo.m_shape = m_polyline->m_physShape;
        initInfo.m_pos = m_owner->get2DPos();
        initInfo.m_angle = m_polyline->getAngle();
        initInfo.m_userData = m_polyline->getRef();
        initInfo.m_collisionGroup = ECOLLISIONGROUP_POLYLINE;

        m_phantom = PHYSWORLD->allocPhantom(initInfo);
        m_phantom->updateAABB();

        if ( m_registered )
        {
            PHYSWORLD->insertPhantom(m_phantom,_initData.m_depth);
        }
    }

    m_depth = _initData.m_depth;
    m_movingPolyForce = _initData.m_movingPolyForce;
    m_weightMultiplier = _initData.m_weightMultiplier;
    m_landSpeedMultiplier = _initData.m_landSpeedMultiplier;
    m_hitForceMultiplier = _initData.m_hitForceMultiplier;
    m_impulseMultiplier = _initData.m_impulseMultiplier;
}

void SoftPlatform::registerPhysics( bbool _val )
{
    if ( _val != m_registered )
    {
        if ( _val )
        {
            u32 numBodies = m_bodies.size();
            u32 numConstraints = m_constraints.size();

            for ( u32 i = 0; i < numBodies; i++ )
            {
                BodyData& body = m_bodies[i];

                if ( body.m_body )
                {
                    PHYSWORLD->insertBody(body.m_body,m_depth);
                }
            }

            for ( u32 i = 0; i < numConstraints; i++ )
            {
                ConstraintData& constraint = m_constraints[i];

                if ( constraint.m_constraint )
                {
                    PHYSWORLD->insertConstraint(constraint.m_constraint);
                }
            }

            if ( m_phantom )
            {
                PHYSWORLD->insertPhantom(m_phantom,m_depth);
            }
        }
        else
        {
            u32 numBodies = m_bodies.size();
            u32 numConstraints = m_constraints.size();

            for ( u32 i = 0; i < numBodies; i++ )
            {
                BodyData& body = m_bodies[i];

                if ( body.m_body )
                {
                    PHYSWORLD->removeBody(body.m_body);
                }
            }

            for ( u32 i = 0; i < numConstraints; i++ )
            {
                ConstraintData& constraint = m_constraints[i];

                if ( constraint.m_constraint )
                {
                    PHYSWORLD->removeConstraint(constraint.m_constraint);
                }
            }

            if ( m_phantom )
            {
                PHYSWORLD->removePhantom(m_phantom);
            }
        }

        m_registered = _val;

        if ( !m_registered )
        {
            FixedArray <ObjectRef,32> detachActors;

            UsersList::iterator userIt = m_users.begin();

            while ( userIt != m_users.end() )
            {
                User& user = *userIt;

                detachActors.push_back(user.m_actor);

                if ( detachActors.full() )
                {
                    break;
                }

                ++userIt;
            }

            HangingList::iterator hangingIt = m_hanging.begin();

            while ( hangingIt != m_hanging.end() )
            {
                Hanging& hanger = *hangingIt;

                detachActors.push_back(hanger.m_actor);

                if ( detachActors.full() )
                {
                    break;
                }

                ++hangingIt;
            }

            EventDetach detachEvent;

            for ( u32 i = 0; i < detachActors.size(); i++ )
            {
                Actor* actor = AIUtils::getActor(detachActors[i]);

                if ( actor )
                {
                    actor->onEvent(&detachEvent);
                }
            }
        }
    }
}

void SoftPlatform::setDepth( f32 _z, bbool _updateBodies )
{
    m_depth = _z;

    if ( _updateBodies )
    {
        u32 numBodies = m_bodies.size();

        for ( u32 i = 0; i < numBodies; i++ )
        {
            BodyData& bodyData = m_bodies[i];

            bodyData.m_body->setPos3D(bodyData.m_body->getPos().to3d(_z));
        }

        if ( m_phantom )
        {
            m_phantom->setPos3D(m_owner->getPos());
        }
    }
}

void SoftPlatform::update( f32 _dt )
{
    m_gravity = PHYSWORLD->getGravity(m_owner->get2DPos(),m_depth);

    if ( m_polyline )
    {
        updatePolyline(_dt);
    }

    addForces(_dt);
}

void SoftPlatform::updatePolyline( f32 _dt )
{
    u32 numBodies = m_bodies.size();

    for ( u32 i = 0; i < numBodies; i++ )
    {
        m_polyline->setPosAt(m_bodies[i].m_body->getPos(),i);
    }

    m_phantom->setPos3D(m_owner->getPos());
    m_phantom->setRecomputeAABB();
    m_phantom->updateAABB();

    m_polyline->update(_dt);
}

void SoftPlatform::addForces( f32 _dt )
{
    addUserForces();
    addHangingForces(_dt);
    addMovingPolylineForces(_dt);
}

void SoftPlatform::addUserForces()
{
    for ( UsersList::const_iterator it = m_users.begin(); it != m_users.end(); ++it )
    {
        const User& user = *it;

        Actor* actor = AIUtils::getActor(user.m_actor);

        if ( actor )
        {
            BodyData& body = m_bodies[user.m_bodyIndex];

            if ( body.m_constraintIndex != U32_INVALID )
            {
                ConstraintData& constraint = m_constraints[body.m_constraintIndex];
                BodyData& bodyDataB = m_bodies[constraint.m_bodyBIndex];
                Vec2d dir = bodyDataB.m_body->getPos() -
                    body.m_body->getPos();

                dir.normalize();

                Vec2d bodyPos = body.m_body->getPos();
                Vec2d vecToPos = actor->get2DPos() - bodyPos;
                f32 dotP = dir.dot(vecToPos);
                Vec2d pos = bodyPos + (dir*dotP);
                Vec2d force = m_gravity*user.m_weight*m_weightMultiplier;

                applyForce(user.m_bodyIndex,force,pos);
            }
        }
    }
}

void SoftPlatform::addHangingForces( f32 _dt )
{
    for ( HangingList::iterator it = m_hanging.begin(); it != m_hanging.end(); ++it )
    {
        Hanging& hanging = *it;

        BodyData& body = m_bodies[hanging.m_bodyIndex];

        if ( body.m_constraintIndex != U32_INVALID )
        {
            ConstraintData& constraint = m_constraints[body.m_constraintIndex];
            BodyData& bodyDataB = m_bodies[constraint.m_bodyBIndex];
            Vec2d dir = bodyDataB.m_body->getPos() -
                body.m_body->getPos();

            Vec2d force = m_gravity*hanging.m_weight*m_weightMultiplier;
            Vec2d pos = body.m_body->getPos() + (dir*hanging.m_t);
            applyForce(hanging.m_bodyIndex,force,pos);
        }
    }
}

void SoftPlatform::addMovingPolylineForces( f32 _dt )
{
    if ( !m_phantom || m_movingPolyForce == 0.f )
    {
        return;
    }

    PhysContactsContainer contacts;

    PHYSWORLD->collidePhantoms(m_phantom->getPos(),m_phantom->getPos(),m_phantom->getAngle(),m_phantom->getShape(),m_depth,ECOLLISIONFILTER_CHARACTERS,contacts);

    for ( u32 i = 0; i < contacts.size(); i++ )
    {
        const SCollidableContact& contact = contacts[i];

        if ( contact.m_edgeIndex == U32_INVALID )
        {
            continue;
        }

        BaseObject* obj = GETOBJECT(contact.m_collidableUserData);

        if ( !obj )
        {
            continue;
        }

        Actor* actor = obj->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));

        if ( !actor )
        {
            continue;
        }

        EventQueryPhysicsData physicsData;

        actor->onEvent(&physicsData);

        if ( physicsData.getWorldSpeed() != Vec2d::Zero )
        {
            applyForce(contact.m_edgeIndex,physicsData.getWorldSpeed()*m_movingPolyForce,actor->get2DPos());
        }
    }
}

void SoftPlatform::applyForce( u32 _bodyIndex, const Vec2d& _force, const Vec2d& _pos )
{
    BodyData& bodyDataA = m_bodies[_bodyIndex];

    bodyDataA.m_body->addForce(_force);

    // Try to find if this physic body is linked to another. If it is we will apply different forces
    // to get some torque
    if ( bodyDataA.m_constraintIndex != U32_INVALID )
    {
        ConstraintData& constraint = m_constraints[bodyDataA.m_constraintIndex];
        BodyData& bodyDataB = m_bodies[constraint.m_bodyBIndex];
        Vec2d vecToPos = _pos - bodyDataA.m_body->getPos();
        Vec2d vecToBody = bodyDataB.m_body->getPos() -
            bodyDataA.m_body->getPos();
        Vec2d dirToPos = vecToPos;
        Vec2d dirToBody = vecToBody;

        dirToPos.normalize();
        dirToBody.normalize();

        f32 dotP = 1.f - f32_Abs(dirToPos.dot(dirToBody));
        f32 forceVal = _force.norm() * dotP;

        bodyDataB.m_body->addForce((-vecToPos)*forceVal);
    }

    if ( m_owner->getBinding()->isBound() )
    {
        Actor* parentActor = AIUtils::getActor(m_owner->getBinding()->getParent());

        if ( parentActor )
        {
            EventForceOnActor forceEvent(m_owner->getRef(),_pos,_force);
            parentActor->onEvent(&forceEvent);
        }
    }
}

void SoftPlatform::applyImpulse( u32 _bodyIndex, const Vec2d& _impulse, f32 _t )
{
    BodyData& bodyDataA = m_bodies[_bodyIndex];
    PhysBody* body = bodyDataA.m_body;

    body->addForce(_impulse * f32_Abs(1.f-_t));

    if ( bodyDataA.m_constraintIndex != U32_INVALID )
    {
        ConstraintData& constraint = m_constraints[bodyDataA.m_constraintIndex];
        BodyData& bodyDataB = m_bodies[constraint.m_bodyBIndex];

        body = bodyDataB.m_body;
        body->addForce(_impulse * f32_Abs(_t));
    }
}

void SoftPlatform::onEvent( Event* _event)
{
    if ( EventStickOnPolyline* stickEvent = _event->DynamicCast<EventStickOnPolyline>(ITF_GET_STRINGID_CRC(EventStickOnPolyline,471231076)) )
    {
        processNewUser(stickEvent);
    }
    else if ( EventStickOnPolylineChangeEdge* edgeChange = _event->DynamicCast<EventStickOnPolylineChangeEdge>(ITF_GET_STRINGID_CRC(EventStickOnPolylineChangeEdge,4222832239)) )
    {
        processUserEdgeChange(edgeChange);
    }
    else if ( EventHanging* hanging = _event->DynamicCast<EventHanging>(ITF_GET_STRINGID_CRC(EventHanging,2826410601)) )
    {
        processHangingNew(hanging);
    }
    else if ( EventHangUpdate* hangUpdate = _event->DynamicCast<EventHangUpdate>(ITF_GET_STRINGID_CRC(EventHangUpdate,3049416002)) )
    {
        processHangingUpdate(hangUpdate);
    }
    else if ( HitStim* hit = _event->DynamicCast<HitStim>(ITF_GET_STRINGID_CRC(HitStim,2121727796)) )
    {
        processHitStim(hit);
    }
    else if ( EventRopeSwingImpulse* polyImpulse = _event->DynamicCast<EventRopeSwingImpulse>(ITF_GET_STRINGID_CRC(EventRopeSwingImpulse,807862862)) )
    {
        processRopeSwingImpulse(polyImpulse);
    }
    else if ( EventBouncedOnPolyline* bounceEvent = _event->DynamicCast<EventBouncedOnPolyline>(ITF_GET_STRINGID_CRC(EventBouncedOnPolyline,1421560006)) )
    {
        processBounceEvent(bounceEvent);
    }
}

void SoftPlatform::processNewUser( EventStickOnPolyline* _event )
{
    if ( m_polyline && m_polyline->getRef() == _event->getPolyline() )
    {
        if ( _event->isSticked() )
        {
            User newUser;

            newUser.m_actor = _event->getActor();
            newUser.m_bodyIndex = _event->getEdge();
            newUser.m_weight = _event->getWeight();

            m_users.push_back(newUser);

            Vec2d force = _event->getSpeed()*m_landSpeedMultiplier*_event->getWeight()*m_weightMultiplier;

            applyForce(newUser.m_bodyIndex,force,_event->getPos());
        }
        else
        {
            for ( UsersList::iterator it = m_users.begin(); it != m_users.end(); ++it )
            {
                User& user = *it;

                if ( user.m_actor == _event->getActor() )
                {
                    m_users.erase(it);
                    break;
                }
            }
        }
    }
}

void SoftPlatform::processUserEdgeChange( EventStickOnPolylineChangeEdge* _event )
{
    if ( m_polyline && m_polyline->getRef() == _event->getPolyline() )
    {
        for ( UsersList::iterator it = m_users.begin(); it != m_users.end(); ++it )
        {
            User& user = *it;

            if ( user.m_actor == _event->getActor() )
            {
                user.m_bodyIndex = _event->getEdge();
                break;
            }
        }
    }
}

void SoftPlatform::processHangingNew( EventHanging* _event )
{
    if ( m_polyline && m_polyline->getRef() == _event->getPolyline() )
    {
        if ( _event->isHanging() )
        {
            Hanging newHanging;

            newHanging.m_actor = _event->getActor();
            newHanging.m_bodyIndex = _event->getEdge();
            newHanging.m_t = _event->getT();
            newHanging.m_weight = _event->getWeight();

            Actor* hanger = AIUtils::getActor(_event->getActor());

            if ( hanger )
            {
                Vec2d force = _event->getSpeed()*m_landSpeedMultiplier*_event->getWeight()*m_weightMultiplier;

                applyForce(newHanging.m_bodyIndex,force,hanger->get2DPos());
            }

            m_hanging.push_back(newHanging);
        }
        else
        {
            for ( HangingList::iterator it = m_hanging.begin(); it != m_hanging.end(); ++it )
            {
                Hanging& hang = *it;

                if ( hang.m_actor == _event->getActor() )
                {
                    m_hanging.erase(it);
                    break;
                }
            }
        }
    }
}

void SoftPlatform::processHangingUpdate( EventHangUpdate* _event )
{
    for ( HangingList::iterator it = m_hanging.begin(); it != m_hanging.end(); ++it )
    {
        Hanging& hang = *it;

        if ( hang.m_actor == _event->getActor() )
        {
            hang.m_bodyIndex = _event->getEdge();
            hang.m_t = _event->getT();
            hang.m_weight = _event->getWeight();
            break;
        }
    }
}

void SoftPlatform::processHitStim( HitStim* _event )
{
    if ( m_polyline )
    {
        const PhysContactsContainer& contacts = _event->getContacts();

        for ( u32 index = 0; index < contacts.size(); index++ )
        {
            const SCollidableContact& contact = contacts[index];

            if ( contact.m_collidableUserData == m_polyline->getRef() )
            {
                Vec2d force = _event->getDirection() * ( _event->getLevel() + 1.f ) * m_hitForceMultiplier;

                applyForce(contact.m_edgeIndex,force,_event->getPos());
                break;
            }
        }
    }
}

void SoftPlatform::processRopeSwingImpulse( EventRopeSwingImpulse* _event )
{
    if ( m_polyline && _event->getPolyline() == m_polyline->getRef() && _event->getEdgeIndex() != U32_INVALID )
    {
        Vec2d impulse = _event->getImpulse() * (1.f/_event->getWeight()) * m_impulseMultiplier;

        applyImpulse(_event->getEdgeIndex(),impulse,_event->getT());

        if ( m_owner->getBinding()->isBound() )
        {
            Actor* parentActor = AIUtils::getActor(m_owner->getBinding()->getParent());

            if ( parentActor )
            {
                EventForceOnActor forceEvent(m_owner->getRef(),m_bodies[_event->getEdgeIndex()].m_body->getPos(),impulse);
                parentActor->onEvent(&forceEvent);
            }
        }
    }
}

void SoftPlatform::processBounceEvent( EventBouncedOnPolyline* _event )
{
    if ( m_polyline && _event->getPoly() == m_polyline->getRef() && _event->getEdgeIndex() != U32_INVALID )
    {
        PhysBody* body = m_bodies[_event->getEdgeIndex()].m_body;
        Vec2d force = (-_event->getDir()) * m_impulseMultiplier;
        body->addForce(force);
    }
}

#ifdef ITF_SUPPORT_EDITOR
void SoftPlatform::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
{
    if (_flags != ActorComponent::DrawEditFlag_All)
        return;

    if ( m_polyline )
    {
        _drawInterface->drawPolyLine(m_polyline,btrue);
    }

    u32 numBodies = m_bodies.size();

    for ( u32 i = 0; i < numBodies; i++ )
    {
        PhysBody* body = m_bodies[i].m_body;

        if ( body )
        {
            GFX_ADAPTER->drawDBGCircle(body->getPos().to3d(m_depth), 0.05f, 0.f, 1.f, 0.f);
        }
    }

    u32 numConstraints = m_constraints.size();

    for ( u32 i = 0; i < numConstraints; i++ )
    {
        PhysConstraint* constraint = m_constraints[i].m_constraint;

        if ( constraint )
        {
            if ( constraint->getLimitAngle() )
            {
                const PhysBody* bodyA = constraint->getBodyA();
                const Angle minAngle = Angle(bfalse,bodyA->getAngle()) + constraint->getMinAngle();
                const Angle maxAngle = Angle(bfalse,bodyA->getAngle()) + constraint->getMaxAngle();
                const Vec2d minLimit = minAngle.ToVector() * 0.5f;
                const Vec2d maxLimit = maxAngle.ToVector() * 0.5f;

                GFX_ADAPTER->drawDBG3DLine(bodyA->getPos().to3d(m_depth), (bodyA->getPos() + minLimit).to3d(m_depth), 1.f,0.f,0.f,0.f);
                GFX_ADAPTER->drawDBG3DLine(bodyA->getPos().to3d(m_depth), (bodyA->getPos() + maxLimit).to3d(m_depth), 1.f,0.f,0.f,0.f);
            }
        }
    }
}
#endif // ITF_SUPPORT_EDITOR

}

