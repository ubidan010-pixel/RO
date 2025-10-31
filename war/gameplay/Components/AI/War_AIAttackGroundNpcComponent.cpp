#include "precompiled_gameplay_war.h"

#ifndef _ITF_WAR_AIATTACKGROUNDNPCCOMPONENT_H_
#include "war/gameplay/Components/AI/War_AIAttackGroundNpcComponent.h"
#endif //_ITF_WAR_AIATTACKGROUNDNPCCOMPONENT_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

namespace ITF
{
IMPLEMENT_OBJECT_RTTI(War_AIAttackGroundNpcComponent)

BEGIN_SERIALIZATION_CHILD(War_AIAttackGroundNpcComponent)
    SERIALIZE_MEMBER("aiDetectRadius",m_detectRadius,ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_OBJECT("roamingBehavior", m_roamingBehavior, ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_OBJECT("attackGroundBehavior", m_attackGroundBehavior, ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_OBJECT("receiveHitBehavior", m_receiveHitBehavior, ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_OBJECT("deathBehavior", m_deathBehavior, ESerialize_PropertyEdit|ESerialize_Template);
    SERIALIZE_FUNCTION(postLoad, ESerialize_Template_Load);
END_SERIALIZATION()

War_AIAttackGroundNpcComponent::War_AIAttackGroundNpcComponent() 
: m_walkLeft(bfalse)
, m_detectRadius(3.f)
{
}

void War_AIAttackGroundNpcComponent::postLoad()
{
    addBehavior(&m_roamingBehavior);
    addBehavior(&m_attackGroundBehavior);
    addBehavior(&m_receiveHitBehavior);
    addBehavior(&m_deathBehavior);
}

void War_AIAttackGroundNpcComponent::onActorLoaded()
{
    Super::onActorLoaded();

    m_actor->registerEvent(War_HitStim::GetClassNameStatic(),this);

    setBehavior(&m_roamingBehavior);
}


void War_AIAttackGroundNpcComponent::Update( f32 _deltaTime )
{
    Super::Update(_deltaTime);

    if (getHealth() == 0)
    {
        if (m_currentBehavior != &m_deathBehavior)
        {
            // die!
            setBehavior(&m_deathBehavior);
            unregisterObjects();    // de-register phantom and AI
        }
    }
    else if (m_currentBehavior == &m_receiveHitBehavior)
    {
        // nothing to do...
    }
    else if ( checkEnemy() )
    {
        m_attackGroundBehavior.setWalkDir(m_walkLeft);

        if ( m_currentBehavior != &m_attackGroundBehavior )
        {
            setBehavior(&m_attackGroundBehavior);            
        }
    }
    else
    {
        if ( m_currentBehavior != &m_roamingBehavior )
        {
            setBehavior(&m_roamingBehavior);
        }
    }

    //String debug;debug.setTextFormat("health : %d", m_health);
    //GFX_ADAPTER->drawDBGText(debug);
}

bbool War_AIAttackGroundNpcComponent::checkEnemy()
{
    Vec2d myPos = GetActor()->get2DPos();
    Vec2d closest;
    f32 minDistSq = 0.f;

    for ( u32 i = 0; i < GAMEMANAGER->getMaxPlayerCount(); i++ )
    {
        Actor* player = GAMEMANAGER->getActivePlayer(i);

        if ( !player )
        {
            continue;
        }

        if ( DepthRange(GetActor()->getDepth()) != DepthRange(player->getDepth()) )
        {
            continue;
        }

        Vec2d pos = player->get2DPos();
        f32 distSq = ( pos - myPos ).sqrnorm();

        if ( distSq > (m_detectRadius*m_detectRadius) )
        {
            continue;
        }

        if ( !minDistSq || distSq < minDistSq )
        {
            closest = pos;
            minDistSq = distSq;
        }
    }

    if ( minDistSq )
    {
            // An enemy is close, attack
        m_walkLeft = (closest.m_x - myPos.m_x) < 0.f;

        return btrue;
    }

    return bfalse;
}


void War_AIAttackGroundNpcComponent::onEvent( Event* _event )
{
    Super::onEvent(_event);

    if ( War_HitStim* hit = _event->DynamicCast<War_HitStim>(ITF_GET_STRINGID_CRC(War_HitStim,3270006373)) )
    {
        receiveHit(hit);
    }
}

void War_AIAttackGroundNpcComponent::receiveHit( War_HitStim* _hit )
{
    if ( m_currentBehavior != &m_receiveHitBehavior )
    {
        // we are already processing a hit, so we need to reset the behavior
        m_receiveHitBehavior.setData(_hit);
        setBehavior(&m_receiveHitBehavior);
    }
}

void War_AIAttackGroundNpcComponent::onBehaviorFinished()
{
    Super::onBehaviorFinished();

    if ( m_currentBehavior == &m_receiveHitBehavior )
    {
        // let Update decide what to do next
        setNullBehavior();
    }
}

} // namespace ITF

