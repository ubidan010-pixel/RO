#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_
#include "rayman/gameplay/Components/Player/Ray_PlayerControllerComponent.h"
#endif //_ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_

#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_RAY_POWERUPNAMES_H_
#include "rayman/gameplay/Ray_PowerUpNames.h"
#endif //_ITF_RAY_POWERUPNAMES_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI_SUBCLASS(Ray_PlayerControllerComponent,StateSwimIdle)

////////////////////
// Swim Idle State
////////////////////
Ray_PlayerControllerComponent::StateSwimIdle::StateSwimIdle()
: m_prevUnstickMinFrictionMultiplier(1.f)
, m_canDive(bfalse)
, m_justEntered(bfalse)
{
}


void Ray_PlayerControllerComponent::StateSwimIdle::onEnter()
{
    Super::onEnter();

    // Disable sticking on polylines
    if ( m_parent->m_stance != STANCE_SWIM )
    {
        m_parent->setStance(STANCE_SWIM);
    }
    
    m_justEntered = btrue;
}

void Ray_PlayerControllerComponent::StateSwimIdle::onExit()
{
    Super::onExit();

    m_parent->m_swimingTwist = bfalse;
}

bbool Ray_PlayerControllerComponent::StateSwimIdle::checkStateChange( f32 _dt )
{
    if ( checkBounce() )
    {
        return btrue;
    }

    BaseObject* obj = GETOBJECT(m_parent->m_swimingPolyline);

    if ( !obj )
    {
        m_parent->changeState(&m_parent->m_stateFalling);
        return btrue;
    }

    PolyLine* poly = obj->DynamicCast<PolyLine>(ITF_GET_STRINGID_CRC(PolyLine,1932163747));

    if ( !poly )
    {
        m_parent->changeState(&m_parent->m_stateFalling);
        return btrue;
    }

    m_parent->swimingUpdatePolylines(poly,_dt);

    if ( shouldJumpOutOfWater() )
    {
        m_parent->setupSwimDolphinJump();
        return btrue;
    }

    if  ( m_parent->isHitQueued())
    {
        if ( m_parent->setWaterPunch() )
        {
            return btrue;
        }
    }

    if ( m_parent->trySwimSprintJump() && m_parent->isPowerUpEnabled(Ray_PowerUp_Dive) )
    {
        m_parent->setupSwimDolphinJump();
        return btrue;
    }

    if ( tryExitFromCurrent() )
    {
        m_parent->changeState(&m_parent->m_stateFalling);
        return btrue;
    }

    if ( m_parent->shouldExitSwiming() )
    {
        m_characterPhys->forceFindStickEdge();
        m_parent->changeState(&m_parent->m_stateIdle);
        return btrue;
    }

    return bfalse;
}

bbool Ray_PlayerControllerComponent::StateSwimIdle::shouldJumpOutOfWater() const
{
    if ( m_parent->m_swimingTime < m_parent->getTemplate()->getSwimingRegainControl() )
    {
        return bfalse;
    }

    if ( !m_parent->isJumpQueued() )
    {
        return bfalse;
    }

    if ( !m_parent->m_swimingIsOnJumpRange || m_parent->m_swimingTime < m_parent->getTemplate()->getSwimingTimeAllowAction() )
    {
        return bfalse;
    }

    return btrue;
}

void Ray_PlayerControllerComponent::StateSwimIdle::update(f32 _dt)
{
    if ( m_parent->m_listenerJump )
    {
        m_parent->m_swimingDashCounter = m_parent->getTemplate()->getSwimingDashDuration();
    }

    m_parent->UpdatePhysicSwim(_dt);

    m_canDive = m_parent->isPowerUpEnabled(Ray_PowerUp_Dive);
    m_justEntered = bfalse;
}

bbool Ray_PlayerControllerComponent::StateSwimIdle::tryExitFromCurrent() const
{
    if ( !m_parent->m_swimingIsOnSurface )
    {
        return bfalse;
    }

    if ( m_parent->m_currentGravityDir.dot(m_characterPhys->getSpeed()) > 0.f )
    {
        return bfalse;
    }

    u32 numForces = m_parent->m_currentWindForces.size();
    Vec2d totalForces = Vec2d::Zero;

    for ( u32 i = 0; i < numForces; i++ )
    {
        const PhysActorForce& wind = m_parent->m_currentWindForces[i];

        if ( m_actor->getBinding()->isDescendant(wind.m_actor) )
        {
            continue;
        }

        totalForces += wind.m_force;
    }

    f32 dotProd = m_parent->m_currentGravity.dot(totalForces);

    if ( dotProd < 0.f )
    {
        return btrue;
    }
    else
    {
        return bfalse;
    }
}

void Ray_PlayerControllerComponent::StateSwimIdle::onEvent(Event * _event)
{
    Super::onEvent(_event);

    if ( AnimGameplayEvent* animEvent = _event->DynamicCast<AnimGameplayEvent>(ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301)) )
    {
        if (animEvent->getName() == ITF_GET_STRINGID_CRC(MRK_Twist_Finished,2214363433) )
        {
            m_parent->m_swimingTwist = bfalse;
        }
    }
}

void Ray_PlayerControllerComponent::StateSwimIdle::updateAnimInputs()
{
    Super::updateAnimInputs();

    m_animComponent->setInputFromIndex(m_parent->m_animInputIndex[ANIMINPUT_DIVEMODE],static_cast<u32>(m_canDive));
    m_animComponent->setInputFromIndex(m_parent->m_animInputIndex[ANIMINPUT_SWIMENTERHEAD],static_cast<u32>(m_parent->m_swimingEnterDown));
    m_animComponent->setInputFromIndex(m_parent->m_animInputIndex[ANIMINPUT_SWIMCURVECURSOR],m_parent->m_swimingCurveCursor);
    m_animComponent->setInputFromIndex(m_parent->m_animInputIndex[ANIMINPUT_SWIMTWIST],m_parent->m_swimingTwist);
}

}


