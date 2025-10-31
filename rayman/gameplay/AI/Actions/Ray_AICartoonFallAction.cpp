#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AICARTOONFALLACTION_H_
#include "rayman/gameplay/AI/Actions/Ray_AICartoonFallAction.h"
#endif //_ITF_RAY_AICARTOONFALLACTION_H_

#ifndef _ITF_ANIMATEDCOMPONENT_H_
#include "engine/actors/components/animatedcomponent.h"
#endif //_ITF_ANIMATEDCOMPONENT_H_

#ifndef _ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_
#include "gameplay/Components/common/StickToPolylinePhysComponent.h"
#endif //_ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_

#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_AICartoonFallAction_Template)

    BEGIN_SERIALIZATION_CHILD(Ray_AICartoonFallAction_Template)
        SERIALIZE_MEMBER("gravityMultiplierDuringRun", m_gravityMultiplierDuringRun);
        SERIALIZE_MEMBER("gravityMultiplierDuringFall", m_gravityMultiplierDuringFall);
        SERIALIZE_MEMBER("airFrictionMultiplierDuringRun", m_airFrictionMultiplierDuringRun);
        SERIALIZE_MEMBER("minSpeedForFall", m_minSpeedForFall);
        SERIALIZE_MEMBER("initialFallSpeed", m_initialFallSpeed);
    END_SERIALIZATION()

    Ray_AICartoonFallAction_Template::Ray_AICartoonFallAction_Template()
    : m_gravityMultiplierDuringRun(0)
    , m_gravityMultiplierDuringFall(2)
    , m_airFrictionMultiplierDuringRun(20)
    , m_minSpeedForFall(0.5f)
    , m_initialFallSpeed(10.f)
    {
        m_animDefaultAction = ITF_GET_STRINGID_CRC(FALL,100613701);
    }

    IMPLEMENT_OBJECT_RTTI(Ray_AICartoonFallAction)

    Ray_AICartoonFallAction::Ray_AICartoonFallAction()
        : m_physComponent(NULL)
        , m_prevGravityMultiplier(1)
        , m_prevAirFrictionMultiplier(0)
        , m_airRunning(bfalse)
        , m_preFalling(bfalse)
        , m_falling(bfalse)
        , m_grounded(bfalse)
   {
    }

    Ray_AICartoonFallAction::~Ray_AICartoonFallAction()
    {
    }

    void Ray_AICartoonFallAction::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        m_physComponent = m_actor->GetComponent<StickToPolylinePhysComponent>();
    }

    void Ray_AICartoonFallAction::onActivate()
    {
        Super::onActivate();

        m_prevGravityMultiplier = m_physComponent->getGravityMultiplier();
        m_prevAirFrictionMultiplier = m_physComponent->getAirFrictionMultiplier();

        m_physComponent->setGravityMultiplier(getTemplate()->getGravityMultiplierDuringRun());
        m_physComponent->setAirFrictionMultiplier(getTemplate()->getAirFrictionMultiplierDuringRun());

        // start AIR-RUN
        m_airRunning = btrue;
        m_preFalling = bfalse;
        m_falling = bfalse;
        m_grounded = bfalse;
    }

    void Ray_AICartoonFallAction::onDeactivate()
    {
        Super::onDeactivate();

        m_physComponent->setGravityMultiplier(m_prevGravityMultiplier);
        m_physComponent->setAirFrictionMultiplier(m_prevAirFrictionMultiplier);
    }

    void Ray_AICartoonFallAction::update( f32 _delta )
    {
        if ( m_grounded )
        {
            if ( isAnimFinished() )
            {
                // DONE
                m_aiBehavior->onActionFinished();
            }
        }
        else if ( m_physComponent->getStickedEdge() )
        {
            // start CRASH
            m_grounded = btrue;
            m_airRunning = bfalse;
            m_preFalling = bfalse;
            m_falling = bfalse;
        }
        else if ( m_preFalling )
        {
            if ( isAnimFinished() )
            {
                // start FALL
                m_falling = btrue;
                m_preFalling = bfalse;

                m_physComponent->setSpeed( getTemplate()->getInitialFallSpeed() * m_aiComponent->getCurrentGravityDir() );
                m_physComponent->setGravityMultiplier(getTemplate()->getGravityMultiplierDuringFall());
                m_physComponent->setAirFrictionMultiplier(m_prevAirFrictionMultiplier);
            }
        }
        else
        {
            f32 sqrSpeed = m_physComponent->getSpeed().sqrnorm();
            if ( sqrSpeed < (getTemplate()->getMinSpeedForFall() * getTemplate()->getMinSpeedForFall()) )
            {
                // start PREFALL
                m_preFalling = btrue;
                m_airRunning = bfalse;
            }
        }
    }

    void Ray_AICartoonFallAction::updateAnimInputs()
    {
        Super::updateAnimInputs();

        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(AirRunning,3074545752), static_cast<u32>(m_airRunning));
        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(PreFalling,1076171183), static_cast<u32>(m_preFalling));
        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(Falling,2990802452), static_cast<u32>(m_falling));
        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(Grounded,4080858678), static_cast<u32>(m_grounded));
    }

}