#include "precompiled_gameplay.h"

#ifndef _ITF_SWINGSOFTPLATFORM_H_
#include "gameplay/components/Environment/SwingSoftPlatform.h"
#endif //_ITF_SWINGSOFTPLATFORM_H_

#ifndef _ITF_PHYSBODY_H_
#include "engine/physics/PhysBody.h"
#endif //_ITF_PHYSBODY_H_

#ifndef _ITF_GAMEPLAYEVENTS_H_
#include "gameplay/GameplayEvents.h"
#endif //_ITF_GAMEPLAYEVENTS_H_

#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

#ifndef _ITF_PHYSCONSTRAINT_H_
#include "engine/physics/PhysConstraint.h"
#endif //_ITF_PHYSCONSTRAINT_H_

namespace ITF
{

SwingSoftPlatform::SwingSoftPlatform()
: m_numSwingForces(0)
, m_swingingForces(Vec2d::Zero)
, m_swingingForcesLastBodyIndex(U32_INVALID)
, m_firstBodyIndex(U32_INVALID)
, m_lastBodyIndex(U32_INVALID)
, m_stiffBehavior(bfalse)
, m_dragCounter(0.f)
, m_dragForce(0.f)
, m_nonStiffSwingSpeedFriction(0.95f)
, m_nonStiffGravityShift(0.f)
, m_stiffSwingSpeedFriction(0.95f)
, m_stiffGravityMultiplier(5.f)
, m_stiffImpulseMultiplier(2.5f)
, m_landDragDuration(0.5f)
, m_landDragMultiplier(10.f)
, m_maxSwingAngle(bfalse,MTH_PIBY2)
, m_canUseSmall(btrue)
, m_canUseNormal(btrue)
, m_repositionWithBones(btrue)
, m_allReduced(bfalse)
{

}

SwingSoftPlatform::~SwingSoftPlatform()
{

}

void SwingSoftPlatform::setOwner( Actor* _owner )
{
    if ( m_owner != _owner )
    {
        if ( m_owner )
        {
            m_owner->unregisterEvent(ITF_GET_STRINGID_CRC(EventGetPolylineConstrainedPosition,3189215538),this);
        }
    }

    Super::setOwner(_owner);

    if ( m_owner )
    {
        ACTOR_REGISTER_EVENT_COMPONENT(m_owner,ITF_GET_STRINGID_CRC(EventGetPolylineConstrainedPosition,3189215538),this);
    }
}

void SwingSoftPlatform::init( const SoftPlatformInitData& _initData )
{
    Super::init(_initData);

    u32 numBodies = m_bodies.size();
    u32 numConstraints = m_constraints.size();

    m_swingBodyData.resize(numBodies);

    for ( u32 bodyIndex = 0; bodyIndex < numBodies; bodyIndex++ )
    {
        SwingBodyData& swingBodyData = m_swingBodyData[bodyIndex];

        swingBodyData.m_gravityMultiplier = _initData.m_bodies[bodyIndex].m_gravityMultiplier;
        swingBodyData.m_windMultiplier = _initData.m_bodies[bodyIndex].m_windMultiplier;

        if ( _initData.m_bodies[bodyIndex].m_static )
        {
            m_firstBodyIndex = bodyIndex;

            if ( m_lastBodyIndex != U32_INVALID )
            {
                break;
            }
        }
        else
        {
            u32 usedInConstraints = 0;

            for ( u32 constraintIndex = 0; constraintIndex < numConstraints; constraintIndex++ )
            {
                const ConstraintData& constraint = m_constraints[constraintIndex];

                if ( constraint.m_bodyAIndex == bodyIndex ||
                     constraint.m_bodyBIndex == bodyIndex )
                {
                    usedInConstraints++;

                    if ( usedInConstraints > 1 )
                    {
                        break;
                    }
                }
            }

            if ( usedInConstraints == 1 )
            {
                m_lastBodyIndex = bodyIndex;

                if ( m_firstBodyIndex != U32_INVALID )
                {
                    break;
                }
            }
        }
    }
}

void SwingSoftPlatform::update( f32 _dt )
{
    orderHangers();

    Super::update(_dt);

    m_swingingForces = Vec2d::Zero;
    m_swingingForcesLastBodyIndex = U32_INVALID;
    m_numSwingForces = 0;
    m_allReduced = bfalse;
}

const SwingSoftPlatform::ConstraintData* SwingSoftPlatform::getBodyConstraint( u32 _bodyIndex ) const
{
    u32 numConstraints = m_constraints.size();

    for ( u32 i = 0; i < numConstraints; i++ )
    {
        const ConstraintData& constraint = m_constraints[i];

        if ( constraint.m_bodyAIndex == _bodyIndex )
        {
            return &constraint;
        }
    }
    
    return NULL;
}

void SwingSoftPlatform::orderHangers()
{
    FixedArray <Hanging,32> hangersOrders;

    for ( HangingList::const_iterator itHanger = m_hanging.begin(); itHanger != m_hanging.end(); ++itHanger )
    {
        const Hanging& hangActor = *itHanger;
        u32 numHangingOrderSize = hangersOrders.size();
        bbool inserted = bfalse;

        for ( u32 orderedIndex = 0; orderedIndex < numHangingOrderSize; orderedIndex++ )
        {
            const Hanging& hangActorCheck = hangersOrders[orderedIndex];

            if ( hangActor.m_bodyIndex > hangActorCheck.m_bodyIndex )
            {
                hangersOrders.insert(hangActor,orderedIndex);
                inserted = btrue;
                break;
            }
            else if ( hangActorCheck.m_bodyIndex == hangActor.m_bodyIndex )
            {
                if ( hangActor.m_t > hangActorCheck.m_t )
                {
                    hangersOrders.insert(hangActor,orderedIndex);
                    inserted = btrue;
                    break;
                }
            }
        }

        if ( !inserted )
        {
            hangersOrders.push_back(hangActor);
        }
    }

    m_hangersOrdered.clear();

    if ( m_repositionWithBones )
    {
        u32 numHangOrdered = hangersOrders.size();
        u32 lastBodyIndex = m_bodies.size() > 2 ? m_lastBodyIndex - 2 : m_lastBodyIndex - 1;
        f32 lastT = 1.f;
        OrderedHangingInfo newHangingInfo;

        for ( u32 i = 0; i < numHangOrdered; i++ )
        {
            const Hanging& hangActor = hangersOrders[i];

            newHangingInfo.m_actor = hangActor.m_actor;
            newHangingInfo.m_bodyIndex = lastBodyIndex;
            newHangingInfo.m_t = lastT;

            m_hangersOrdered.push_back(newHangingInfo);

            if ( lastT == 1.f )
            {
                lastT = 0.f;
            }
            else if ( lastBodyIndex > 0 )
            {
                lastBodyIndex--;
            }
        }
    }
    else
    {
        u32 numHangOrdered = hangersOrders.size();
        u32 lastBodyIndex = m_bodies.size() > 2 ? m_lastBodyIndex - 2 : m_lastBodyIndex - 1;
        f32 lastT = 1.f;
        OrderedHangingInfo newHangingInfo;
        static const f32 s_radiusCheck = 0.5f;

        for ( u32 hangIndex = 0; hangIndex < numHangOrdered; hangIndex++ )
        {
            const Hanging& hangActor = hangersOrders[hangIndex];

            newHangingInfo.m_actor = hangActor.m_actor;
            newHangingInfo.m_bodyIndex = lastBodyIndex;
            newHangingInfo.m_t = lastT;

            m_hangersOrdered.push_back(newHangingInfo);

            const ConstraintData* constraintData = getBodyConstraint(lastBodyIndex);
            
            if ( constraintData->m_constraint )
            {
                f32 length = constraintData->m_constraint->getLength();
                f32 currentPos = length * lastT;

                currentPos -= s_radiusCheck;

                lastT = currentPos / length;

                if ( lastT < 0.f )
                {
                    if ( lastBodyIndex )
                    {
                        lastBodyIndex--;
                    }
                    else
                    {
                        lastT = 0.f;
                    }

                    while ( lastBodyIndex < m_bodies.size() )
                    {
                        const ConstraintData* constraintData = getBodyConstraint(lastBodyIndex);

                        if ( constraintData && constraintData->m_constraint )
                        {
                            currentPos = ( length * lastT ) + currentPos;

                            lastT = currentPos / length;

                            if ( lastT < 0.f )
                            {
                                if ( lastBodyIndex )
                                {
                                    lastBodyIndex--;
                                }
                                else
                                {
                                    lastT = 0.f;
                                    break;
                                }
                            }
                            else
                            {
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
}

void SwingSoftPlatform::updateShiftGravity( f32 _dt )
{
    u32 numBodies = m_bodies.size();
    Vec2d gravity = m_gravity.Rotate(m_nonStiffGravityShift);

    for ( u32 i = 0; i < numBodies; i++  )
    {
        PhysBody* body = m_bodies[i].m_body;
        body->setGravityMultiplier(0.f);
        body->addForce(gravity*m_swingBodyData[i].m_gravityMultiplier);
    }
}

void SwingSoftPlatform::addForces( f32 _dt )
{
    if ( !m_stiffBehavior )
    {
        u32 numBodies = m_bodies.size();

        for ( u32 i = 0; i < numBodies; i++  )
        {
            Vec2d speed = m_bodies[i].m_body->getSpeed()*m_nonStiffSwingSpeedFriction;
            m_bodies[i].m_body->setSpeed(speed);
        }

        m_dragCounter = 0.f;

        if ( m_nonStiffGravityShift )
        {
            updateShiftGravity(_dt);
        }

        Super::addForces(_dt);
        return;
    }

    if ( m_lastBodyIndex == U32_INVALID || m_firstBodyIndex == U32_INVALID )
    {
        m_dragCounter = 0.f;
        return;
    }

    bbool swinging = m_swingingForces != Vec2d::Zero && m_numSwingForces;

    if ( swinging )
    {
        m_dragCounter = 0.f;

        u32 numBodies = m_bodies.size();

        for ( u32 i = 0; i < numBodies; i++  )
        {
            Vec2d speed = m_bodies[i].m_body->getSpeed()*m_stiffSwingSpeedFriction;
            m_bodies[i].m_body->setSpeed(speed);
        }

        u32 forcesBodyIndex = m_swingingForcesLastBodyIndex != U32_INVALID ? m_swingingForcesLastBodyIndex : m_lastBodyIndex;
        BodyData& firstBody = m_bodies[m_firstBodyIndex];
        BodyData& lastBody = m_bodies[forcesBodyIndex];

        Vec2d mainGravityDir = m_gravity;

        mainGravityDir.normalize();

        Vec2d swingForce = m_swingingForces/static_cast<f32>(m_numSwingForces);

        if ( m_numSwingForces > 1 )
        {
            swingForce *= 1.2f; 
        }

        f32 impulse = mainGravityDir.getPerpendicular().dot(m_swingingForces);
        f32 signImpulse = getSign(impulse);

        f32 gravitySize = m_gravity.norm()*m_stiffGravityMultiplier;

        Vec2d gravityDir = ( lastBody.m_body->getPos() - firstBody.m_body->getPos() );
        gravityDir.normalize();

        f32 signCurrentPos = getSign(gravityDir.cross(mainGravityDir));
        f32 multiplier;
        f32 dotProd = Clamp(gravityDir.dot(mainGravityDir),-1.f,1.f);
        f32 angle = fabsf(acosf(dotProd));

        if( signImpulse == signCurrentPos )
        {
            multiplier = 1.f;
        }
        else
        {
            if ( angle < m_maxSwingAngle.ToRadians() )
            {
                multiplier = 1.f - ( angle / m_maxSwingAngle.ToRadians() );
            }
            else
            {
                f32 dif = angle - m_maxSwingAngle.ToRadians();

                multiplier = - (1.f - ( dif / m_maxSwingAngle.ToRadians() ));
            }
        }

        Vec2d externalForce = gravityDir*gravitySize;
        Vec2d totalImpulse = gravityDir.getPerpendicular()*impulse*multiplier*m_stiffImpulseMultiplier;

        lastBody.m_body->addForce(externalForce);
        lastBody.m_body->addForce(totalImpulse);

        for ( u32 i = forcesBodyIndex + 1; i <= m_lastBodyIndex; i++ )
        {
            BodyData& bodyData = m_bodies[i];

            bodyData.m_body->setGravityMultiplier(1.f);
        }
    }
    else
    {
        if ( m_dragCounter )
        {
            updateDragForce(_dt);
        }
    }
    
    if ( !swinging || m_allReduced )
    {
        BodyData& firstBody = m_bodies[m_firstBodyIndex];
        BodyData& lastBody = m_bodies[m_lastBodyIndex];

        Vec2d mainGravityDir = m_gravity;
        mainGravityDir.normalize();
        Vec2d gravityDir = ( lastBody.m_body->getPos() - firstBody.m_body->getPos() );
        f32 gravitySize = m_gravity.norm()*m_stiffGravityMultiplier;
        gravityDir.normalize();
        f32 dotProd = Clamp(gravityDir.dot(mainGravityDir),-1.f,1.f);
        f32 angle = fabsf(acosf(dotProd));
        f32 halfAngle = m_maxSwingAngle.ToRadians()*0.5f;
        f32 multiplier = 1.f;

        if ( angle > halfAngle )
        {
            multiplier = 1.f + ( (angle-halfAngle) / halfAngle );
            multiplier = Clamp(multiplier,1.f,2.f);
        }

        lastBody.m_body->addForce(m_gravity*m_stiffGravityMultiplier*multiplier);
        lastBody.m_body->addForce(gravityDir*gravitySize);

        for ( u32 i = m_firstBodyIndex; i <= m_lastBodyIndex; i++ )
        {
            BodyData& bodyData = m_bodies[i];

            bodyData.m_body->setGravityMultiplier(0.f);
        }
    }
}

void SwingSoftPlatform::updateDragForce( f32 _dt )
{
    BodyData& firstBody = m_bodies[m_firstBodyIndex];
    BodyData& lastBody = m_bodies[m_lastBodyIndex];
    f32 t = m_dragCounter/m_landDragDuration;
    f32 signImpulse = getSign(m_dragForce);
    Vec2d mainGravityDir = m_gravity;
    mainGravityDir.normalize();
    Vec2d gravityDir = ( lastBody.m_body->getPos() - firstBody.m_body->getPos() );
    gravityDir.normalize();

    f32 signCurrentPos = getSign(gravityDir.cross(mainGravityDir));
    f32 multiplier;
    f32 dotProd = Clamp(gravityDir.dot(mainGravityDir),-1.f,1.f);
    f32 angle = fabsf(acosf(dotProd));

    if( signImpulse == signCurrentPos )
    {
        multiplier = 1.f;
    }
    else
    {
        if ( angle < m_maxSwingAngle.ToRadians() )
        {
            multiplier = 1.f - ( angle / m_maxSwingAngle.ToRadians() );
        }
        else
        {
            f32 dif = angle - m_maxSwingAngle.ToRadians();

            multiplier = - (1.f - ( dif / m_maxSwingAngle.ToRadians() ));
        }
    }

    Vec2d dragForce = gravityDir.getPerpendicular()*m_dragForce*t*multiplier;

    lastBody.m_body->addForce(dragForce);

    m_dragCounter = Max(m_dragCounter-_dt,0.f);
}

void SwingSoftPlatform::setStiffBehavior( bbool _val )
{
    if ( m_stiffBehavior == _val )
    {
        return;
    }

    m_stiffBehavior = _val;

    u32 numBodies = m_bodies.size();

    for ( u32 i = 0; i < numBodies; i++ )
    {
        BodyData& bodyData = m_bodies[i];
        SwingBodyData& swingData = m_swingBodyData[i];

        if ( m_stiffBehavior )
        {
            bodyData.m_body->setGravityMultiplier(0.f);
            bodyData.m_body->setWindMultiplier(0.f);
        }
        else
        {
            bodyData.m_body->setGravityMultiplier(swingData.m_gravityMultiplier);
            bodyData.m_body->setWindMultiplier(swingData.m_windMultiplier);
        }
    }
}

void SwingSoftPlatform::onEvent( Event * _event )
{
    Super::onEvent(_event);

    if ( EventGetPolylineConstrainedPosition* constainedEvent = _event->DynamicCast<EventGetPolylineConstrainedPosition>(ITF_GET_STRINGID_CRC(EventGetPolylineConstrainedPosition,3189215538)) )
    {
        processGetConstrainedPosition(constainedEvent);
    }
}

void SwingSoftPlatform::processHangingNew( class EventHanging* _event )
{
    Super::processHangingNew(_event);

    if ( m_polyline && m_polyline->getRef() == _event->getPolyline() )
    {
        if ( _event->isHanging() )
        {
            BodyData& firstBody = m_bodies[m_firstBodyIndex];
            BodyData& lastBody = m_bodies[m_lastBodyIndex];
            Vec2d gravityDir = ( lastBody.m_body->getPos() - firstBody.m_body->getPos() );

            gravityDir.normalize();

            f32 speed = gravityDir.getPerpendicular().dot(_event->getSpeed());

            m_dragForce = speed * m_landDragMultiplier;

            if ( fabs(m_dragForce) > MTH_EPSILON )
            {
                m_dragCounter = m_landDragDuration;
            }

            if ( (_event->getCharacterSize() == CHARACTERSIZE_NORMAL && m_canUseNormal ) ||
                 (_event->getCharacterSize() == CHARACTERSIZE_SMALL && m_canUseSmall ) )
            {
                setStiffBehavior(btrue);
            }
        }
        else
        {
            setStiffBehavior(!m_hanging.empty());
        }
    }
}

void SwingSoftPlatform::processGetConstrainedPosition( EventGetPolylineConstrainedPosition* _event )
{
    if ( m_lastBodyIndex == U32_INVALID || m_firstBodyIndex == U32_INVALID )
    {
        return;
    }

    if ( !m_polyline || _event->getPolyline() != m_polyline->getRef() )
    {
        return;
    }

    u32 numHangers = m_hangersOrdered.size();

    for ( u32 i = 0; i < numHangers; i++ )
    {
        const OrderedHangingInfo& hangInfo = m_hangersOrdered[i];

        if ( hangInfo.m_actor == _event->getSender() )
        {
            _event->setEdgeIndex(hangInfo.m_bodyIndex);
            _event->setT(hangInfo.m_t);
            break;
        }
    }
}

void SwingSoftPlatform::processRopeSwingImpulse( EventRopeSwingImpulse* _event )
{
    if ( !m_stiffBehavior )
    {
        Super::processRopeSwingImpulse(_event);
        return;
    }

    u32 numHangOrdered = m_hangersOrdered.size();

    for ( u32 i = 0; i < numHangOrdered; i++ )
    {
        const OrderedHangingInfo& hangInfo = m_hangersOrdered[i];

        if ( _event->getSender() == hangInfo.m_actor )
        {
            u32 bodyIndex;

            if ( hangInfo.m_t < 0.5f )
            {
                bodyIndex = hangInfo.m_bodyIndex;
            }
            else
            {
                bodyIndex = hangInfo.m_bodyIndex + 1;
            }

            if ( m_swingingForcesLastBodyIndex == U32_INVALID || hangInfo.m_bodyIndex > m_swingingForcesLastBodyIndex )
            {
                m_swingingForcesLastBodyIndex = bodyIndex;
            }

            break;
        }
    }

    if ( m_numSwingForces == 0 )
    {
        m_allReduced = _event->getCharacterSize() == CHARACTERSIZE_SMALL;
    }
    else
    {
        m_allReduced &= _event->getCharacterSize() == CHARACTERSIZE_SMALL;
    }

    m_swingingForces += _event->getImpulse();
    m_numSwingForces++;
}

}

