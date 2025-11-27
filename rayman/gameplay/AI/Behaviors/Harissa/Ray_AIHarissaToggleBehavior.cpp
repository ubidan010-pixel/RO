#include "precompiled_gameplay_rayman.h"

#ifndef _RAY_AI_HARISSATOGGLEBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Harissa/Ray_AIHarissaToggleBehavior.h"
#endif //_RAY_AI_HARISSATOGGLEBEHAVIOR_H_

//#ifndef _ITF_ANIMATIONTRACK_H_
//#include "engine/animation/AnimationTrack.h"
//#endif //_ITF_ANIMATIONTRACK_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#ifndef _ITF_PHYSPHANTOM
#include "engine/physics/PhysPhantom.h"
#endif //_ITF_PHYSPHANTOM

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_AIHarissaToggleBehavior)

    BEGIN_VALIDATE_BEHAVIOR(Ray_AIHarissaToggleBehavior)
    END_VALIDATE_BEHAVIOR()

    //-------------------------------------------------------
    Ray_AIHarissaToggleBehavior::Ray_AIHarissaToggleBehavior()
    : m_offAction(NULL)
    , m_onAction(NULL)
    , m_hitAction(NULL)
    , m_flameHandle(U32_INVALID)
    , m_stickedActorsCount(0)
    , m_forcedTime(0.0f)
    , m_onActionStep(OnActionStep_UNDEFINED)
    {
    }

    //-------------------------------------------------------
    Ray_AIHarissaToggleBehavior::~Ray_AIHarissaToggleBehavior()
    {
    }

    //-------------------------------------------------------
    void Ray_AIHarissaToggleBehavior::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        //create actions
        m_offAction = createAiAction(getTemplate()->getOffAction());
        m_onAction = createAiAction(getTemplate()->getOnAction());
        m_hitAction = createAiAction(getTemplate()->getHitAction());

        //register events
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301),m_aiComponent);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(PunchStim,200533519),m_aiComponent);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventStickOnPolyline,471231076),m_aiComponent);
    }

    //-------------------------------------------------------
    void Ray_AIHarissaToggleBehavior::onActivate()
    {
        //start off action
        startOffAction();
    }

    //-------------------------------------------------------
    void Ray_AIHarissaToggleBehavior::onDeactivate()
    {
        Super::onDeactivate();

        //stop fx
        stopFlameFX();
    }

    //-------------------------------------------------------
    void Ray_AIHarissaToggleBehavior::onActorBecomeInactive()
    {
        Super::onActorBecomeActive();

        startOffAction();
    }

    //-------------------------------------------------------
    void Ray_AIHarissaToggleBehavior::update(f32 _dt)
    {
        Super::update(_dt);

        //update current action
        if( m_currentAction == m_onAction )
        {
            updateOnAction(_dt);
        }
        else if( m_currentAction == m_offAction )
        {
            updateOffAction();
        }
        else if ( ( m_hitAction != NULL ) && ( m_currentAction == m_hitAction ) )
        {
            updateOnAction(_dt);
        }
        else
        {
            ITF_ASSERT_MSG( bfalse, "Ray_AIHarissaToggleBehavior is currently not in off or on action." );
        }

        //update forced time
        if( m_forcedTime > 0.0f )
        {
            m_forcedTime = f32_Max(0.0f, m_forcedTime - _dt);
        }
    }

    //-------------------------------------------------------
    void Ray_AIHarissaToggleBehavior::onEvent(Event * _event)
    {
        Super::onEvent(_event);

        if( const EventStickOnPolyline * const stickEvent = _event->DynamicCast<EventStickOnPolyline>(ITF_GET_STRINGID_CRC(EventStickOnPolyline,471231076)) )
        {
            if( stickEvent->getWeight() >= getTemplate()->getWeightThreshold() )
            {
                if( stickEvent->isSticked() )
                {
                    ++m_stickedActorsCount;
                }
                else
                {
                    ITF_ASSERT( m_stickedActorsCount > 0 );
                    --m_stickedActorsCount;
                }
            }
        }
        else if( const AnimGameplayEvent * const animEvent = _event->DynamicCast<AnimGameplayEvent>(ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301)) )
        {
            if ( ( m_currentAction == m_onAction ) || ( m_currentAction == m_hitAction ) )
            {
                const StringID startFXMarker = "MRK_Flame_Start";
                const StringID stopFXMarker = "MRK_Flame_Stop";

                if( animEvent->getName() == startFXMarker )
                {
                    startFlameFX();
                }
                else if( animEvent->getName() == stopFXMarker )
                {               
                    stopFlameFX();
                }
            }
        }
        else if( const PunchStim * const punch = _event->DynamicCast<PunchStim>(ITF_GET_STRINGID_CRC(PunchStim,200533519)) )
        {
            if( punch->getReceivedHitType() == RECEIVEDHITTYPE_EARTHQUAKE )
            {
                m_forcedTime = getTemplate()->getFlamesDurationOnCrushAttack();
            }
            else if ( m_hitAction != NULL )
            {
                startHitAction();

                if ( Actor* sender = AIUtils::getActor( _event->getSender() ) )
                {
                    EventHitSuccessful success;
                    success.setSender( m_actor->getRef() );
                    sender->onEvent( &success );
                }
            }
        }
    }

    //-------------------------------------------------------
    void Ray_AIHarissaToggleBehavior::startOffAction()
    {
        //stop fx (failsafe for anim event)
        stopFlameFX();
        //set off action
        setAction(m_offAction);
        //invalidate on action step
        m_onActionStep = OnActionStep_UNDEFINED;
    }

    //-------------------------------------------------------
    void Ray_AIHarissaToggleBehavior::startOnAction()
    {
        //set on action
        setAction(m_onAction);
        //start animation
        if( m_animComponent != NULL )
        {
            m_animComponent->setAnim( getTemplate()->getSquashedInAnim() );
        }
        //init on action step
        m_onActionStep = OnActionStep_IN;
    }

    //-------------------------------------------------------
    void Ray_AIHarissaToggleBehavior::startHitAction()
    {
        if ( ( m_hitAction == NULL ) || ( m_currentAction == m_onAction ) )
        {
            return;
        }
        
        // start the action if not yet, 
        //
        if ( m_currentAction != m_hitAction )
        {
            //set on action
            setAction(m_hitAction);
            //start animation
            if( m_animComponent != NULL )
            {
                m_animComponent->setAnim( getTemplate()->getSquashedInAnim() );
            }
            //init on action step
            m_onActionStep = OnActionStep_IN;
        }

        // init hit duration
        m_forcedTime = getTemplate()->getHitActionDuration();
    }

    //-------------------------------------------------------
    void Ray_AIHarissaToggleBehavior::updateOffAction()
    {
        if( m_stickedActorsCount > 0 || m_forcedTime > 0.0f )
        {
            startOnAction();
        }
    }

    //-------------------------------------------------------
    void Ray_AIHarissaToggleBehavior::updateOnAction(const f32 _dt)
    {
        if( m_onActionStep == OnActionStep_IN )
        {
            if( m_animComponent != NULL )
            {
                if( m_animComponent->isSubAnimFinished() )
                {
                    m_animComponent->setAnim( getTemplate()->getSquashedLoopAnim() );
                    m_onActionStep = OnActionStep_LOOP;
                }
            }
            else
            {
                startOffAction();
            }
        }
        else if( m_onActionStep == OnActionStep_LOOP )
        {
            if( m_stickedActorsCount == 0 && m_forcedTime == 0.0f )
            {
                if( m_animComponent != NULL )
                {
                    m_animComponent->setAnim( getTemplate()->getSquashedOutAnim() );
                    m_onActionStep = OnActionStep_OUT;
                }
            }
        }
        else if( m_onActionStep == OnActionStep_OUT )
        {
            if( m_animComponent != NULL )
            {
                if( m_animComponent->isSubAnimFinished() )
                {
                    startOffAction();
                }
            }
        }
    }

    //-------------------------------------------------------
    void Ray_AIHarissaToggleBehavior::startFlameFX()
    {
        if( m_flameHandle == U32_INVALID )
        {            
            if( m_fxController != NULL )
            {
                const StringID & fxName = getTemplate()->getFlameFXName();
                if( fxName.isValid() )
                {
                    m_flameHandle = m_fxController->playFX( fxName );
                }
            }
        }
    }

    //-------------------------------------------------------
    void Ray_AIHarissaToggleBehavior::stopFlameFX()
    {
        if( m_flameHandle != U32_INVALID )
        {
            if( m_fxController != NULL )
            {
                m_fxController->stopFXFromHandle( m_flameHandle );
            }
            m_flameHandle = U32_INVALID;
        }
    }



    IMPLEMENT_OBJECT_RTTI(Ray_AIHarissaToggleBehavior_Template)

    BEGIN_SERIALIZATION_CHILD(Ray_AIHarissaToggleBehavior_Template)
        // ACTIONS
        SERIALIZE_OBJECT_WITH_FACTORY("offAction", m_offAction, ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("onAction", m_onAction, ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("hitAction", m_hitAction, ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_MEMBER("flameFXName", m_flameFXName);
        SERIALIZE_MEMBER("squashedInAnim", m_squashedInAnim);
        SERIALIZE_MEMBER("squashedLoopAnim", m_squashedLoopAnim);
        SERIALIZE_MEMBER("squashedOutAnim", m_squashedOutAnim);
        SERIALIZE_MEMBER("polylineName", m_polylineName);
        SERIALIZE_MEMBER("weightThreshold", m_weightThreshold);
        SERIALIZE_MEMBER("flamesDurationOnCrushAttack", m_flamesDurationOnCrushAttack);
		SERIALIZE_MEMBER("hitActionDuration", m_hitActionDuration);
    END_SERIALIZATION();

    Ray_AIHarissaToggleBehavior_Template::Ray_AIHarissaToggleBehavior_Template()
    : m_offAction(NULL)
    , m_onAction(NULL)
    , m_hitAction(NULL)
    , m_flameFXName(StringID::Invalid)
    , m_squashedInAnim(StringID::Invalid)
    , m_squashedLoopAnim(StringID::Invalid)
    , m_squashedOutAnim(StringID::Invalid)
    , m_polylineName(StringID::Invalid)
    , m_weightThreshold(0.0f)
    , m_flamesDurationOnCrushAttack(1.0f)
	, m_hitActionDuration( 0.25f )
    {
    }

    Ray_AIHarissaToggleBehavior_Template::~Ray_AIHarissaToggleBehavior_Template()
    {
        SF_DEL(m_offAction);
        SF_DEL(m_onAction);
        SF_DEL(m_hitAction);
    }
}
