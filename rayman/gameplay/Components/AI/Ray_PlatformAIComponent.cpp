#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_PLATFORMAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_PlatformAIComponent.h"
#endif //_ITF_RAY_PLATFORMAICOMPONENT_H_

#ifndef _ITF_POLYLINECOMPONENT_H_
#include "gameplay/Components/Environment/PolylineComponent.h"
#endif //_ITF_POLYLINECOMPONENT_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_RAY_PLATFORMTREECOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_PlatformTreeComponent.h"
#endif //_ITF_RAY_PLATFORMTREECOMPONENT_H_

#ifndef _ITF_RAY_TRIGGERBOUNCECOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_TriggerBounceComponent.h"
#endif //_ITF_RAY_TRIGGERBOUNCECOMPONENT_H_

//#ifdef DEVELOPER_BEN
//    #define LOG_STATE(_anim)\
//        LOG_BEN("%d - %s - play state %s", CURRENTFRAME, m_actor->getUserFriendly().cStr(), _anim.getDebugString())
//    #define LOG_TRANSITION(_anim)\
//        LOG_BEN("%d - %s - play transition %s", CURRENTFRAME, m_actor->getUserFriendly().cStr(), _anim.getDebugString())
//#else
//    #define LOG_STATE(_anim)
//    #define LOG_TRANSITION(_anim)
//#endif
#define LOG_STATE(_anim)
#define LOG_TRANSITION(_anim)

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_PlatformAIComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_PlatformAIComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(Ray_PlatformAIComponent)
        VALIDATE_COMPONENT_PARAM("", m_animComponent, "Ray_PlatformAIComponent requires an AnimLightComponent/AnimatedComponent");
        //VALIDATE_COMPONENT_PARAM("", m_polylineComponent, "Ray_PlatformAIComponent requires a PolylineComponent");
        VALIDATE_COMPONENT_PARAM("", !getTemplate()->getSynchronizeOpenCloseTransitions() || (getTemplate()->getOpenTransition().isValid() && getTemplate()->getCloseTransition().isValid()), "you need an open and close transition if you want to synchronize them!");
        VALIDATE_COMPONENT_PARAM("", !getTemplate()->getCloseOnHit() || getTemplate()->getCloseOnHitTransition().isValid(), "you can't closeOnHit if you don't have a closeOnHitTransition");
    END_VALIDATE_COMPONENT()

    Ray_PlatformAIComponent::Ray_PlatformAIComponent()
        : Super()
        , m_animComponent(NULL)
        , m_animatedComponent(NULL)
        , m_polylineComponent(NULL)
        , m_triggerBounceComponent(NULL)
        , m_transitionQueryTimer(0.f)
        , m_hitTimer(0.f)
        , m_currentWeight(0.f)
        , m_activateCount(0)
        , m_openCount(0)
        , m_openFullCount(0)
        , m_targetState(State_Hidden)
        , m_state(State_Hidden)
        , m_dirtyCoordinates(bfalse)
        , m_targetPos(Vec2d::Zero)
        , m_targetAngle(0.f)
        , m_hasBeenVacuumed( bfalse )
    {
    }

    Ray_PlatformAIComponent::~Ray_PlatformAIComponent()
    {
    }

    void Ray_PlatformAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        m_animComponent = m_actor->GetComponent<AnimLightComponent>();      // mandatory
        m_animatedComponent = m_actor->GetComponent<AnimatedComponent>();
        m_polylineComponent = m_actor->GetComponent<PolylineComponent>();   // not mandatory (only used for weightTransitions)
        m_triggerBounceComponent = m_actor->GetComponent<Ray_TriggerBounceComponent>();   // not mandatory (only used to check isBouncing)

        if (getTemplate()->getActivateEvent())
        {
            m_actor->registerEvent(getTemplate()->getActivateEvent()->GetObjectClassCRC(), this);
        }
        if (getTemplate()->getDeactivateEvent())
        {
            m_actor->registerEvent(getTemplate()->getDeactivateEvent()->GetObjectClassCRC(), this);
        }
        if (getTemplate()->getOpenEvent())
        {
            m_actor->registerEvent(getTemplate()->getOpenEvent()->GetObjectClassCRC(), this);
        }
        if (getTemplate()->getCloseEvent())
        {
            m_actor->registerEvent(getTemplate()->getCloseEvent()->GetObjectClassCRC(), this);
        }
        if (getTemplate()->getOpenFullEvent())
        {
            m_actor->registerEvent(getTemplate()->getOpenFullEvent()->GetObjectClassCRC(), this);
        }
        if (getTemplate()->getCloseFullEvent())
        {
            m_actor->registerEvent(getTemplate()->getCloseFullEvent()->GetObjectClassCRC(), this);
        }

        if (getTemplate()->getCloseOnHit())
        {
            ACTOR_REGISTER_EVENT_COMPONENT(m_actor, ITF_GET_STRINGID_CRC(HitStim,2121727796), this );
        }
        if (getTemplate()->getManageVacuum())
        {
            ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventHasBeenVacuumed,1042014523), this);
        }

        if (getTemplate()->getStartOpened())
        {
            m_activateCount = 1;
            m_openCount = 1;
        }
        else if (getTemplate()->getStartActivated())
        {
            m_activateCount = 1;
        }

        m_targetState = evaluateState();
        playNewStateIdleAnim();
    }

    void Ray_PlatformAIComponent::onEvent( Event* _event )
    {
        Super::onEvent(_event);

        if ( AIUtils::matchEvent(getTemplate()->getActivateEvent(), _event) )
        {
            onActivate();
        }
        else if ( AIUtils::matchEvent(getTemplate()->getDeactivateEvent(), _event) )
        {
            onDeactivate();
        }
        else if ( AIUtils::matchEvent(getTemplate()->getOpenEvent(), _event) )
        {
            onOpen();
        }
        else if ( AIUtils::matchEvent(getTemplate()->getCloseEvent(), _event) )
        {
            onClose();
        }
        else if ( AIUtils::matchEvent(getTemplate()->getOpenFullEvent(), _event) )
        {
            onOpenFull();
        }
        else if ( AIUtils::matchEvent(getTemplate()->getCloseFullEvent(), _event) )
        {
            onCloseFull();
        }
        else if ( HitStim* hit = _event->DynamicCast<HitStim>(ITF_GET_STRINGID_CRC(HitStim,2121727796)) )
        {
            onReceiveHit(hit);
        }
        else if ( Ray_EventHasBeenVacuumed* hasBeenVacuumed = _event->DynamicCast<Ray_EventHasBeenVacuumed>(ITF_GET_STRINGID_CRC(Ray_EventHasBeenVacuumed,1042014523)) )
        {           
            m_hasBeenVacuumed = btrue;
        }
    }

    // when we're driven by a PlatformTree, we get our state from it (instead of from events)
    void Ray_PlatformAIComponent::setState( const bbool _activated, const f32 _cursor, const bbool _forceSet, const Ray_PlatformTreeComponent_Template* _treeParams )
    {
        State _state;

        if (_activated)
        {
            if (_cursor > _treeParams->getChildOpenDist())
            {
                _state = State_Open;
            }
            else if (_cursor > _treeParams->getChildAppearDist())
            {
                _state = State_Closed;
            }
            else
            {
                _state = State_Hidden;
            }
        }
        else
        {
            if (_cursor < _treeParams->getChildDisappearDist())
            {
                _state = State_Hidden;
            }
            else if (_cursor < _treeParams->getChildCloseDist())
            {
                _state = State_Closed;
            }
            else
            {
                _state = State_Open;
            }
        }

        switch (_state)
        {
        case State_Hidden:
            m_activateCount = 0;
            m_openCount = 0;
            break;
        case State_Closed:
            m_activateCount = 1;
            m_openCount = 0;
            break;
        case State_Open:
            m_activateCount = 1;
            m_openCount = 1;
            break;
        case State_OpenFull:
            ITF_ASSERT_MSG(0, "shouldn't happen in this context");
            m_activateCount = 1;
            m_openCount = 1;
            m_openFullCount = 1;
            break;
        }

        if (_forceSet)
        {
            m_targetState = _state;
            m_state = _state;
            m_currentTransition.invalidate();
            playNewStateIdleAnim();
        }
    }

    void Ray_PlatformAIComponent::Update( f32 _dt )
    {
        Super::Update(_dt);

        // bouncing has precedence over our own AI
        if ( m_triggerBounceComponent && m_triggerBounceComponent->isBouncing() )
        {
            m_currentTransition.invalidate();
            m_state = m_targetState;
            return;
        }


        updateCoordinates(_dt);


        m_hitTimer -= _dt;
        bbool justChanged = playTransitionToNewState(_dt);

        if ( !justChanged && m_currentTransition.isValid() )
        {
            if ( AIUtils::isAnimFinished(m_animComponent, m_animatedComponent) )
            {
                onTransitionFinished();
            }
        }


        if (m_polylineComponent && m_state >= State_Open)
        {
            f32 weight = m_polylineComponent->getWeight();

            if ( m_currentWeight != weight &&
                 m_currentWeight >= getTemplate()->getWeightThreshold() )
            {
                // weight change transitions are interruptible
                const StringID& transition = (weight > m_currentWeight) ? getTemplate()->getWeightIncreaseTransition() : getTemplate()->getWeightDecreaseTransition();
                if (m_currentTransition == transition)
                {
                    m_currentTransition.invalidate();
                    m_animComponent->resetCurTime();
                }
                playTransition(transition);
            }

            m_currentWeight = weight;
        }
    }

    Ray_PlatformAIComponent::State Ray_PlatformAIComponent::evaluateState()
    {
        if (m_hitTimer > 0.f)
        {
            return State_Closed;
        }

        if (m_activateCount > 0)
        {
            if (m_openCount > 0)
            {
                if (m_openFullCount > 0)
                {
                    return State_OpenFull;
                }

                return State_Open;
            }

            return State_Closed;
        }

        return State_Hidden;
    }

    bbool Ray_PlatformAIComponent::playTransitionToNewState( f32 _dt )
    {
        // Only play the transition here, the actual m_state is set later,
        // once the transition is finished (playNewStateIdleAnim()).

        State newTargetState = evaluateState();

        if (newTargetState == m_targetState)
        {
            return bfalse;
        }


        State prevTargetState = m_targetState;

        switch (m_targetState)
        {
        case State_Hidden:
            {
                if (newTargetState > m_targetState)
                {
                    if ( playTransition(getTemplate()->getActivateTransition(), getTemplate()->getDeactivateTransition(), getTemplate()->getSynchronizeActivationTransitions()) )
                    {
                        m_targetState = State_Closed;
                    }
                }
            }
            break;

        case State_Closed:
            {
                if (newTargetState < m_targetState)
                {
                    if (_dt)
                    {
                        m_transitionQueryTimer += _dt;
                        if (m_transitionQueryTimer < getTemplate()->getDeactivateTimeHysteresis())
                        {
                            return bfalse;
                        }
                    }

                    if ( playTransition(getTemplate()->getDeactivateTransition(), getTemplate()->getActivateTransition(), getTemplate()->getSynchronizeActivationTransitions()) )
                    {
                        m_targetState = State_Hidden;
                    }
                }
                else if (newTargetState > m_targetState)
                {
                    if ( playTransition(getTemplate()->getOpenTransition(), getTemplate()->getCloseTransition(), getTemplate()->getSynchronizeOpenCloseTransitions()) )
                    {
                        m_targetState = State_Open;
                    }
                }
            }
            break;

        case State_Open:
            {
                if (newTargetState < m_targetState)
                {
                    if (m_hitTimer > 0.f)
                    {
                        m_currentTransition.invalidate();
                        playTransition(getTemplate()->getCloseOnHitTransition());
                        m_targetState = State_Closed;
                    }
                    else
                    {
                        if (_dt)
                        {
                            m_transitionQueryTimer += _dt;
                            if (m_transitionQueryTimer < getTemplate()->getCloseTimeHysteresis())
                            {
                                return bfalse;
                            }
                        }

                        if ( playTransition(getTemplate()->getCloseTransition(), getTemplate()->getOpenTransition(), getTemplate()->getSynchronizeOpenCloseTransitions()) )
                        {
                            m_targetState = State_Closed;
                        }
                    }
                }
                else if (newTargetState > m_targetState)
                {
                    if ( playTransition(getTemplate()->getOpenFullTransition(), getTemplate()->getCloseFullTransition(), getTemplate()->getSynchronizeOpenCloseTransitions()) )
                    {
                        m_targetState = State_OpenFull;
                    }
                }
            }
            break;

        case State_OpenFull:
            {
                if (newTargetState < m_targetState)
                {
                    if ( playTransition(getTemplate()->getCloseFullTransition(), getTemplate()->getOpenFullTransition(), getTemplate()->getSynchronizeOpenCloseFullTransition()) )
                    {
                        m_targetState = State_Open;
                    }
                }
            }
            break;
        }

        m_transitionQueryTimer = 0.f;

        return m_targetState != prevTargetState;
    }

    void Ray_PlatformAIComponent::playNewStateIdleAnim()
    {
        m_state = m_targetState;

        StringID idleAnim;

        switch (m_state)
        {
        case State_Hidden:
            {
                idleAnim = getTemplate()->getDeactivateIdle();
            }
            break;

        case State_Closed:
            {
                idleAnim = getTemplate()->getActivateIdle();
            }
            break;

        case State_Open:
            {
                if (m_currentWeight > getTemplate()->getWeightThreshold())
                {
                    idleAnim = getTemplate()->getOpenCarryingIdle();
                }
                else
                {
                    idleAnim = getTemplate()->getOpenIdle();
                }
            }
            break;

        case State_OpenFull:
            {
                if (m_currentWeight > getTemplate()->getWeightThreshold())
                {
                    idleAnim = getTemplate()->getOpenCarryingIdle();
                }
                else
                {
                    idleAnim = getTemplate()->getOpenFullIdle();
                }
            }
            break;
        }

        m_animComponent->setAnim(idleAnim);
        LOG_STATE(idleAnim);
    }


    void Ray_PlatformAIComponent::onActivate()
    {
        m_activateCount++;
    }

    void Ray_PlatformAIComponent::onDeactivate()
    {
        if (m_activateCount == 0)
        {
            ITF_WARNING(m_actor, 0, "getting deactivated but we were never activated!");
            return;
        }

        m_activateCount--;
    }

    void Ray_PlatformAIComponent::onOpen()
    {
        m_openCount++;
    }

    void Ray_PlatformAIComponent::onClose()
    {
        ITF_ASSERT(m_openCount > 0);
        if (m_openCount == 0)
        {
            ITF_WARNING(m_actor, 0, "getting closed but we were never opened!");
            return;
        }

        m_openCount--;
    }

    void Ray_PlatformAIComponent::onReceiveHit( HitStim* _hit )
    {
        if (getTemplate()->getCloseOnHit())
        {
            m_hitTimer = getTemplate()->getCloseOnHitDuration();
        }
    }

    void Ray_PlatformAIComponent::onOpenFull()
    {
        m_openFullCount++;
    }

    void Ray_PlatformAIComponent::onCloseFull()
    {
        ITF_ASSERT(m_openFullCount > 0);
        if (m_openFullCount == 0)
        {
            ITF_WARNING(m_actor, 0, "getting closedFull but we were never opened!");
            return;
        }

        m_openFullCount--;
    }

    void Ray_PlatformAIComponent::onTransitionFinished()
    {
        if ( m_currentTransition == getTemplate()->getOpenTransition() )
        {
            m_currentTransition.invalidate();

            // special case for PlatformMan: after opening,
            // either play another transition to go to attract stance,
            // OR go directly to OpenFull (if we have someone in range)

            switch ( evaluateState() )
            {
            case State_OpenFull:
                m_state = m_targetState = State_OpenFull;
                playNewStateIdleAnim();
                return;

            case State_Open:
                m_state = m_targetState = State_Open;
                playTransition(getTemplate()->getCloseFullTransition());
                return;
            }
        }

        // We don't interrupt transitions, so if the target state changed
        // while we were transitioning (ex re-open or disappear while we were closing),
        // re-transition to the new state.
        m_currentTransition.invalidate();
        playTransitionToNewState(0.f);

        // ... otherwise play the new state's idle.
        if (!m_currentTransition.isValid())
        {
            playNewStateIdleAnim();
        }
    }



    void Ray_PlatformAIComponent::playTransition( const StringID& _transition )
    {
        if (m_currentTransition.isValid())
        {
            return;
        }

        if (_transition.isValid())
        {
            m_currentTransition = _transition;
            m_animComponent->setAnim(_transition);
            LOG_TRANSITION(_transition);
        }
        else
        {
            playNewStateIdleAnim();
        }
    }

    bbool Ray_PlatformAIComponent::playTransition( const StringID& _transition, const StringID& _reverseTransition, bbool _synchronize )
    {
        if (m_currentTransition.isValid() &&
            (m_currentTransition == getTemplate()->getWeightIncreaseTransition() ||
             m_currentTransition == getTemplate()->getWeightDecreaseTransition()))
        {
            // weight transitions are negligible and can be interrupted
            m_currentTransition.invalidate();
        }

        if (m_currentTransition.isValid())
        {
            if (_synchronize && m_currentTransition == _reverseTransition)
            {
                // the reverse transition is currently playing, interrupt it
                // and play this one at the corresponding time
                m_currentTransition = _transition;
                f32 reverseTime = m_animComponent->getCurTime();
                if (SubAnim* subAnim = m_animComponent->getSubAnim(_transition)) // TODO: validate all anims?
                {
                    f32 duration = subAnim->getDuration();
                    m_animComponent->setAnim(_transition);
                    LOG_TRANSITION(_transition);
                    m_animComponent->setCurTime(duration - reverseTime);
                }
                else
                {
                    ITF_WARNING(m_actor, 0, "unknown anim %s", _transition.getDebugString());
                }

                return btrue;
            }

            return bfalse;
        }
        else
        {
            ITF_ASSERT(_transition.isValid());
            m_currentTransition = _transition;
            m_animComponent->setAnim(_transition);
            LOG_TRANSITION(_transition);

            return btrue;
        }
    }


    bbool Ray_PlatformAIComponent::canMove( const bbool _activated ) const
    {
        // has been vacuumed, can't move 
        if ( m_hasBeenVacuumed )
        {
            return bfalse;
        }
        else if (_activated)
        {
            // parent is opening
            //  Spiky: cannot move when bouncing
            return ( !m_triggerBounceComponent || !m_triggerBounceComponent->isBouncing() );
        }
        else
        {
            // parent is closing
            //  cannot move when open / openFull / opening / closing
            //  Spiky: cannot move when bouncing

            return  ( m_state < State_Open )
                 && ( !m_currentTransition.isValid() ||
                      (m_currentTransition != getTemplate()->getCloseTransition() &&
                       m_currentTransition != getTemplate()->getOpenTransition()) )
                 && ( !m_triggerBounceComponent || !m_triggerBounceComponent->isBouncing() );
        }
    }

    // is free means that the actor must have no more links with his parent
    bbool Ray_PlatformAIComponent::isFree() const
    {
        return m_hasBeenVacuumed;
    }

    void Ray_PlatformAIComponent::setTargetCoordinates( const Vec2d& _pos, const f32 _angle )
    {
        m_dirtyCoordinates = btrue;
        m_targetPos = _pos;
        m_targetAngle = _angle;
    }

    void Ray_PlatformAIComponent::updateCoordinates( f32 _dt )
    {
        if (m_dirtyCoordinates)
        {
            m_dirtyCoordinates = bfalse;
            m_actor->set2DPos(m_targetPos);
            m_actor->setAngle(m_targetAngle);
        }
    }

    //-------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(Ray_PlatformAIComponent_Template)
    BEGIN_SERIALIZATION(Ray_PlatformAIComponent_Template)
        SERIALIZE_MEMBER("startActivated", m_startActivated);
        SERIALIZE_MEMBER("startOpened", m_startOpened);

        SERIALIZE_OBJECT_WITH_FACTORY("activateEvent", m_activateEvent, ACTORSMANAGER->getEventFactory());
        SERIALIZE_MEMBER("activateTransition", m_activateTransition);
        SERIALIZE_MEMBER("activateIdle", m_activateIdle);
        SERIALIZE_OBJECT_WITH_FACTORY("deactivateEvent", m_deactivateEvent, ACTORSMANAGER->getEventFactory());
        SERIALIZE_MEMBER("deactivateTransition", m_deactivateTransition);
        SERIALIZE_MEMBER("deactivateIdle", m_deactivateIdle);
        SERIALIZE_MEMBER("deactivateTimeHysteresis", m_deactivateTimeHysteresis);
        SERIALIZE_MEMBER("synchronizeActivationTransitions", m_synchronizeActivationTransitions);

        SERIALIZE_OBJECT_WITH_FACTORY("openEvent", m_openEvent, ACTORSMANAGER->getEventFactory());
        SERIALIZE_MEMBER("openTransition", m_openTransition);
        SERIALIZE_MEMBER("openIdle", m_openIdle);
        SERIALIZE_MEMBER("openCarryingIdle", m_openCarryingIdle);
        SERIALIZE_OBJECT_WITH_FACTORY("closeEvent", m_closeEvent, ACTORSMANAGER->getEventFactory());
        SERIALIZE_MEMBER("closeTransition", m_closeTransition);
        SERIALIZE_MEMBER("synchronizeOpenCloseTransitions", m_synchronizeOpenCloseTransitions);
        SERIALIZE_MEMBER("closeTimeHysteresis", m_closeTimeHysteresis);
        SERIALIZE_MEMBER("closeOnHit", m_closeOnHit);
        SERIALIZE_MEMBER("closeOnHitTransition", m_closeOnHitTransition);
        SERIALIZE_MEMBER("closeOnHitDuration", m_closeOnHitDuration);

        SERIALIZE_OBJECT_WITH_FACTORY("openFullEvent", m_openFullEvent, ACTORSMANAGER->getEventFactory());
        SERIALIZE_MEMBER("openFullTransition", m_openFullTransition);
        SERIALIZE_MEMBER("openFullIdle", m_openFullIdle);
        SERIALIZE_OBJECT_WITH_FACTORY("closeFullEvent", m_closeFullEvent, ACTORSMANAGER->getEventFactory());
        SERIALIZE_MEMBER("closeFullTransition", m_closeFullTransition);
        SERIALIZE_MEMBER("synchronizeOpenCloseFullTransitions", m_synchronizeOpenCloseFullTransitions);

        SERIALIZE_MEMBER("weightIncreaseTransition", m_weightIncreaseTransition);
        SERIALIZE_MEMBER("weightDecreaseTransition", m_weightDecreaseTransition);
        SERIALIZE_MEMBER("weightThreshold", m_weightThreshold);
        SERIALIZE_MEMBER("manageVacuum",    m_manageVacuum);
    END_SERIALIZATION()

    Ray_PlatformAIComponent_Template::Ray_PlatformAIComponent_Template()
        : m_startActivated(bfalse)
        , m_startOpened(bfalse)
        , m_activateEvent(NULL)
        , m_deactivateEvent(NULL)
        , m_deactivateTimeHysteresis(0.f)
        , m_synchronizeActivationTransitions(bfalse)
        , m_openEvent(NULL)
        , m_closeEvent(NULL)
        , m_closeTimeHysteresis(0.f)
        , m_synchronizeOpenCloseTransitions(bfalse)
        , m_closeOnHit(bfalse)
        , m_closeOnHitDuration(5.f)
        , m_openFullEvent(NULL)
        , m_closeFullEvent(NULL)
        , m_synchronizeOpenCloseFullTransitions(bfalse)
        , m_weightThreshold(0.f)
        , m_manageVacuum( bfalse )
    {
    }

    Ray_PlatformAIComponent_Template::~Ray_PlatformAIComponent_Template()
    {
        SF_DEL(m_openEvent);
        SF_DEL(m_closeEvent);
        SF_DEL(m_activateEvent);
        SF_DEL(m_deactivateEvent);
        SF_DEL(m_openFullEvent);
        SF_DEL(m_closeFullEvent);
    }
}
