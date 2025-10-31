
#include "precompiled_engine.h"

#ifndef _ITF_PHYSISLAND_H_
#include "engine/physics/PhysIsland.h"
#endif //_ITF_PHYSISLAND_H_

#ifndef _ITF_PHYSBODY_H_
#include "engine/physics/PhysBody.h"
#endif //_ITF_PHYSBODY_H_

#ifndef _ITF_PHYSPHANTOM
#include "engine/physics/PhysPhantom.h"
#endif //_ITF_PHYSPHANTOM

#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_

#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

namespace ITF
{

PhysIsland::PhysIsland( IslandId _id )
: m_firstBodyUpdate(NULL)
, m_lastBodyUpdate(NULL)
, m_firstBody(NULL)
, m_lastBody(NULL)
, m_firstPhantom(NULL)
, m_lastPhantom(NULL)
, m_id(_id)
{

}

PhysIsland::~PhysIsland()
{
    ITF_ASSERT(m_firstBody==NULL);
    ITF_ASSERT(m_firstPhantom==NULL);
}

void PhysIsland::update( f32 _delta )
{
    // Update bounding boxes, reset contacts
    for ( PhysBody* b = m_firstBody; b != NULL; b = static_cast<PhysBody*>(b->m_nextCollideable) )
    {
        b->startFrame();
    }

    for ( PhysBody* b = m_firstBodyUpdate; b != NULL; b = static_cast<PhysBody*>(b->m_nextUpdate) )
    {
        b->startFrame();
    }

    // Apply forces, move bodies
    for ( PhysBody* b = m_firstBodyUpdate; b != NULL; b = b->m_nextUpdate )
    {
        b->update(_delta);
    }
}

void PhysIsland::postUpdate( f32 _deltaTime )
{
    for ( PhysBody* b = m_firstBody; b != NULL; b = static_cast<PhysBody*>(b->m_nextCollideable) )
    {
        b->postUpdate(_deltaTime);
    }

    for ( PhysBody* b = m_firstBodyUpdate; b != NULL; b = static_cast<PhysBody*>(b->m_nextUpdate) )
    {
        b->postUpdate(_deltaTime);
    }

    for ( PhysPhantom* p = m_firstPhantom; p != NULL; p = static_cast<PhysPhantom*>(p->m_nextCollideable) )
    {
        p->synchronizePos();
    }
}

void PhysIsland::insertBody( PhysBody* _body )
{
    ITF_ASSERT(_body->m_nextCollideable==NULL);
    ITF_ASSERT(_body->m_prevCollideable==NULL);
    ITF_ASSERT(_body->m_nextUpdate==NULL);
    ITF_ASSERT(_body->m_prevUpdate==NULL);

    _body->setIsland(this);

    if ( _body->getCollisionGroup() != ECOLLISIONGROUP_NONE )
    {
        if ( m_lastBody )
        {
            ITF_ASSERT(m_lastBody->m_nextCollideable==NULL);
            m_lastBody->m_nextCollideable = _body;
            _body->m_prevCollideable = m_lastBody;
            m_lastBody = _body;
        }
        else
        {
            m_firstBody = _body;
            m_lastBody = _body;
        }
    }

    if ( !_body->isStatic() )
    {
        if ( m_lastBodyUpdate )
        {
            ITF_ASSERT(m_lastBodyUpdate->m_nextUpdate==NULL);
            m_lastBodyUpdate->m_nextUpdate = _body;
            _body->m_prevUpdate = m_lastBodyUpdate;
            m_lastBodyUpdate = _body;
        }
        else
        {
            m_firstBodyUpdate = _body;
            m_lastBodyUpdate = _body;
        }
    }
}

void PhysIsland::removeBody( PhysBody* _body )
{
    if ( _body->m_prevCollideable )
    {
        _body->m_prevCollideable->m_nextCollideable = _body->m_nextCollideable;
    }

    if ( _body->m_nextCollideable )
    {
        _body->m_nextCollideable->m_prevCollideable = _body->m_prevCollideable;
    }

    if ( m_lastBody == _body )
    {
        m_lastBody = static_cast<PhysBody*>(_body->m_prevCollideable);
    }

    if ( m_firstBody == _body )
    {
        m_firstBody = static_cast<PhysBody*>(_body->m_nextCollideable);
    }

    if ( !_body->isStatic() )
    {
        if ( _body->m_prevUpdate )
        {
            _body->m_prevUpdate->m_nextUpdate = _body->m_nextUpdate;
        }

        if ( _body->m_nextUpdate )
        {
            _body->m_nextUpdate->m_prevUpdate = _body->m_prevUpdate;
        }

        if ( m_lastBodyUpdate == _body )
        {
            m_lastBodyUpdate = _body->m_prevUpdate;
        }

        if ( m_firstBodyUpdate == _body )
        {
            m_firstBodyUpdate = _body->m_nextUpdate;
        }

        _body->m_nextUpdate = NULL;
        _body->m_prevUpdate = NULL;
    }

    _body->m_nextCollideable = NULL;
    _body->m_prevCollideable = NULL;
    _body->setIsland(NULL);

}

void PhysIsland::insertPhantom( PhysPhantom* _phantom )
{
    ITF_ASSERT(_phantom->m_nextCollideable==NULL);
    ITF_ASSERT(_phantom->m_prevCollideable==NULL);

    _phantom->setIsland(this);

    if ( m_lastPhantom )
    {
        ITF_ASSERT(m_lastPhantom->m_nextCollideable==NULL);
        m_lastPhantom->m_nextCollideable = _phantom;
        _phantom->m_prevCollideable = m_lastPhantom;
        m_lastPhantom = _phantom;
    }
    else
    {
        m_firstPhantom = _phantom;
        m_lastPhantom = _phantom;
    }
}

void PhysIsland::removePhantom( PhysPhantom* _phantom )
{
    if ( _phantom->m_prevCollideable )
    {
        _phantom->m_prevCollideable->m_nextCollideable = _phantom->m_nextCollideable;
    }

    if ( _phantom->m_nextCollideable )
    {
        _phantom->m_nextCollideable->m_prevCollideable = _phantom->m_prevCollideable;
    }

    if ( m_lastPhantom == _phantom )
    {
        m_lastPhantom = static_cast<PhysPhantom*>(_phantom->m_prevCollideable);
    }

    if ( m_firstPhantom == _phantom )
    {
        m_firstPhantom = static_cast<PhysPhantom*>(_phantom->m_nextCollideable);
    }

    _phantom->m_nextCollideable = NULL;
    _phantom->m_prevCollideable = NULL;
    _phantom->setIsland(NULL);
}

void PhysIsland::insertGravityModifier( ActorForceModifier* _modifier )
{
    m_gravityModifiers.push_back(_modifier);
}

void PhysIsland::removeGravityModifier( ActorForceModifier* _modifier )
{
    for ( ActorForceModifiersList::iterator it = m_gravityModifiers.begin(); it != m_gravityModifiers.end(); ++it )
    {
        if ( *it == _modifier )
        {
            m_gravityModifiers.erase(it);
            return;
        }
    }
}

bbool PhysIsland::getGravityModifier( const Vec2d& _pos, Vec2d& _gravity, Vec2d& _speedMultiplier ) const
{
    for ( ActorForceModifiersList::const_iterator it = m_gravityModifiers.begin(); it != m_gravityModifiers.end(); ++it )
    {
        const ActorForceModifier* actorMod = *it;
        BaseObject* obj = GETOBJECT(actorMod->m_actor);

        if ( obj )
        {
            Actor* actor = obj->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));

            ITF_ASSERT(actor);

            Vec2d actorPos2D = actor->get2DPos();
            f32 angle = actor->getAngle();

            if ( actorMod->m_modifier->checkForce(actorPos2D,angle,_pos,Vec2d::Zero,_gravity,_speedMultiplier) )
            {
                return btrue;
            }
        }
    }

    return bfalse;
}

void PhysIsland::insertWindForce( ActorForceModifier* _modifier )
{
    m_windForces.push_back(_modifier);
}

void PhysIsland::removeWindForce( ActorForceModifier* _modifier )
{
    for ( ActorForceModifiersList::iterator it = m_windForces.begin(); it != m_windForces.end(); ++it )
    {
        if ( *it == _modifier )
        {
            m_windForces.erase(it);
            return;
        }
    }
}

void PhysIsland::getWindForces( const Vec2d& _pos, const Vec2d& _speed, PhysActorForceArray& _res ) const
{
    Vec2d f, speedMultiplier;
    PhysActorForce newForce;

    for ( ActorForceModifiersList::const_iterator it = m_windForces.begin(); it != m_windForces.end(); ++it )
    {
        const ActorForceModifier* actorMod = *it;

        BaseObject* obj = GETOBJECT(actorMod->m_actor);

        if ( obj )
        {
            Actor* actor = obj->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));

            ITF_ASSERT(actor);

            Vec2d actorPos2D = actor->get2DPos();
            f32 angle = actor->getAngle();

            if ( actorMod->m_modifier->checkForce(actorPos2D,angle,_pos,_speed,f,speedMultiplier) )
            {
                newForce.m_actor = actorMod->m_actor;
                newForce.m_force = f;
                newForce.m_speedMultiplier = speedMultiplier;
                _res.push_back(newForce);
            }
        }
    }
}

void PhysIsland::iterateSystem( ObjectRef _userData, bbool _end )
{
    for ( PhysBody* b = m_firstBodyUpdate; b != NULL; b = static_cast<PhysBody*>(b->m_nextUpdate) )
    {
        if ( b->getUserData() == _userData )
        {
            if ( _end )
            {
                b->postUpdate(LOGICDT);
            }
            else
            {
                b->update(LOGICDT);
            }
        }
    }
}

void PhysIsland::drawPhantoms()
{
#ifdef ITF_SUPPORT_DEBUGFEATURE
    for ( PhysPhantom* phantom = m_firstPhantom; phantom != NULL; phantom = static_cast<PhysPhantom*>(phantom->m_nextCollideable) )
    {
        Pickable * pick = phantom->m_owner.getObject()->DynamicCast<Pickable>(ITF_GET_STRINGID_CRC(Pickable,594660361));
        DebugDraw::shape(phantom->getPos(),getZ(),phantom->getAngle(),phantom->getShape(),Color::pink(),0.0f,pick?pick->getUserFriendly().cStr():"");
    }
#endif //ITF_SUPPORT_DEBUGFEATURE
}

void PhysIsland::dumpCollidingBodies()
{
#ifdef ITF_SUPPORT_DEBUGFEATURE
    LOG("COLLIDABLES");
    for ( PhysBody* body = m_firstBody; body != NULL; body = static_cast<PhysBody*>(body->m_nextCollideable) )
    {
        Pickable * pick = body->m_owner.getObject()->DynamicCast<Pickable>(ITF_GET_STRINGID_CRC(Pickable,594660361));
        LOG("%s",pick?pick->getUserFriendly().cStr():"");
    }
#endif //ITF_SUPPORT_DEBUGFEATURE
}

void PhysIsland::dumpPhantoms()
{
#ifdef ITF_SUPPORT_DEBUGFEATURE
    LOG("PHANTOMS");
    for ( PhysPhantom* phantom = m_firstPhantom; phantom != NULL; phantom = static_cast<PhysPhantom*>(phantom->m_nextCollideable) )
    {
        Pickable * pick = phantom->m_owner.getObject()->DynamicCast<Pickable>(ITF_GET_STRINGID_CRC(Pickable,594660361));
        LOG("%s",pick?pick->getUserFriendly().cStr():"");
    }
#endif //ITF_SUPPORT_DEBUGFEATURE
}

}