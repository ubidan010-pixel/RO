#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AIJANODTWEENEDBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/Janod/Ray_AIJanodTweenedBehavior.h"
#endif //_ITF_RAY_AIJANODTWEENEDBEHAVIOR_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_


#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_AIPLAYANIMACTION_H_
#include "gameplay/ai/Actions/AIPlayAnimAction.h"
#endif //_ITF_AIPLAYANIMACTION_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/Managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_STIMSMANAGER_H_
#include "gameplay/managers/stimsmanager.h"
#endif //_ITF_STIMSMANAGER_H_

#ifndef _ITF_PHANTOMCOMPONENT_H_
#include "gameplay/Components/Misc/PhantomComponent.h"
#endif //_ITF_PHANTOMCOMPONENT_H_

#ifndef _ITF_PHYSBODY_H_
#include "engine/physics/PhysBody.h"
#endif //_ITF_PHYSBODY_H_

#ifndef _ITF_RAY_JANODAICOMPONENT_H__
#include "rayman/gameplay/Components/AI/Ray_JanodAIComponent.h"
#endif //_ITF_RAY_JANODAICOMPONENT_H__

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_RAY_FIGHTMANAGER_H_
#include "rayman/gameplay/Managers/Ray_FightManager.h"
#endif //_ITF_RAY_FIGHTMANAGER_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_AIMANAGER_H_
#include "gameplay/managers/AIManager.h"
#endif //_ITF_AIMANAGER_H_

#ifndef _ITF_PHYSPHANTOM
#include "engine/physics/PhysPhantom.h"
#endif //_ITF_PHYSPHANTOM

#ifndef _ITF_TWEENCOMPONENT_H_
#include "gameplay/Components/Common/TweenComponent.h"
#endif //_ITF_TWEENCOMPONENT_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#define JANOD_STAND_ANIM        "Idle"
#define JANOD_PREJUMP_ANIM      "prebounce"
#define JANOD_JUMPING_ANIM      "bounce"
#define JANOD_BRISTLE_ANIM      "bristle"
#define JANOD_FALL_ANIM         "FALL"
#define JANOD_APEX_ANIM         "apex"
#define JANOD_SLEEPS_ANIM       "sleeps"
#define JANOD_TINYCRUSH_ANIM    "TINYCRUSH"

//#define _STOP_WHEN_BRISTLE

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_AIJanodTweenedBehavior_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_AIJanodTweenedBehavior_Template)
        SERIALIZE_MEMBER("stimFeedback", m_stimFeedback);
        SERIALIZE_MEMBER("stimFeedbackDist", m_stimFeedbackDist);
    END_SERIALIZATION()

    Ray_AIJanodTweenedBehavior_Template::Ray_AIJanodTweenedBehavior_Template() :
        m_stimFeedback(500.f)
        , m_stimFeedbackDist(1)
    {
    }

    IMPLEMENT_OBJECT_RTTI(Ray_AIJanodTweenedBehavior)

    BEGIN_VALIDATE_BEHAVIOR(Ray_AIJanodTweenedBehavior)
    END_VALIDATE_BEHAVIOR()

    Ray_AIJanodTweenedBehavior::Ray_AIJanodTweenedBehavior()
    : m_physComponent(NULL)
    , m_state(State_NA)
    , m_timeInCurrentState(0)
    , m_signaledTinyCrush(bfalse)
    {
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    Ray_AIJanodTweenedBehavior::~Ray_AIJanodTweenedBehavior()
    {
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AIJanodTweenedBehavior::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventCrushed,3831882623),m_aiComponent);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventAttackGranted,3058211789),m_aiComponent);

        m_physComponent = m_actor->GetComponent<StickToPolylinePhysComponent>();
        m_physComponent->setUpdateRotation(bfalse);
        m_physComponent->setGravityMultiplier(1);

        resetState();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AIJanodTweenedBehavior::resetState()
    {
        m_state = State_NA;
        m_signaledTinyCrush = bfalse;

        m_animatedComponent->setAnim(JANOD_STAND_ANIM);
        setState(State_Tweened);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AIJanodTweenedBehavior::enableTweening(bbool _enable)
    {
        TweenComponent *tween = m_actor->GetComponent<TweenComponent>();
        if (tween)
        {
            bbool currentlyEnabled = !tween->isTweeningSuspended();

            if (currentlyEnabled!=_enable)
            {
                if (_enable)
                {
                    m_physComponent->setDisabled(btrue);
                    tween->resumeTweening();
                }
                else
                {
                    m_physComponent->setDisabled(bfalse);
                    tween->suspendTweening();
                }
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AIJanodTweenedBehavior::setState(State_t _state, const void *_params)
    {
        Ray_JanodAIComponent *janod = m_actor->GetComponent<Ray_JanodAIComponent>();
        ITF_ASSERT_CRASH(janod, "no janod component");

        m_timeInCurrentState = 0;
        m_state = _state;
        m_physComponent->setSpeedMultiplier(Vec2d::One);
        m_physComponent->setGravityMultiplier(1);
        m_physComponent->setAirFrictionMultiplier(0);        
        
        janod->setIsInBristleMode(bfalse);

        switch(_state)
        {
        case State_Other:
            enableTweening(bfalse);
            break;

        case State_ReactingToTinyCrush:
            {
                m_animatedComponent->setAnim(JANOD_TINYCRUSH_ANIM);
            } break;
        case State_Tweened:
            {
                m_physComponent->setGravityMultiplier(0);
                m_animatedComponent->setAnim(JANOD_STAND_ANIM);
                m_physComponent->setSpeedMultiplier(Vec2d::Zero);
                enableTweening(btrue);
            } break;
        case State_Bristling:
            m_animatedComponent->setAnim(JANOD_BRISTLE_ANIM);
            m_physComponent->setSpeed(Vec2d(0,0));
            m_physComponent->setGravityMultiplier(0.f);
#ifdef _STOP_WHEN_BRISTLE
            enableTweening(bfalse);
#endif
            break;
        default:
            ITF_ASSERT(0);
            break;
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AIJanodTweenedBehavior::onActivate()
    {
        resetState();
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AIJanodTweenedBehavior::onDeactivate()
    {
        setState(State_Other);
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    bbool Ray_AIJanodTweenedBehavior::checkIfSwitchToBristle()
    {
        StickToPolylinePhysComponent *phys = m_actor->GetComponent<StickToPolylinePhysComponent>();
        static PhysContactsContainer contacts;

        if (wasCrushedRecently())
            return bfalse;

        ITF_ASSERT_CRASH(phys, "no StickToPolylinePhysComponent");

        if (phys)
        {
            PhysShapeCircle circle(phys->getRadius());
            
            {
                PhysSweepInfo janodSweepInfo;
                PhysCollisionSolver::calculateSweepInfo(m_actor->get2DPos(), 
                    m_actor->get2DPos(), 
                    m_actor->getAngle(),
                    &circle,
                    janodSweepInfo);

                for (u32 i=0; i<RAY_GAMEMANAGER->getMaxPlayerCount(); i++)
                {
                    Ray_Player *player = (Ray_Player*)RAY_GAMEMANAGER->getPlayer(i);
                    if (player && player->getActive() && !player->isDead())
                    {
                        PhysSweepInfo playerSweepInfo;
                        EventQueryPhysShape queryShape;
                        Actor *playerActor = player->getActor();
                        if (!playerActor)
                            continue;

                        playerActor->onEvent(&queryShape);
                        const PhysShape *playerShape = queryShape.getPhysShape();
                        if (!playerShape)
                        {
                            continue;
                        }
                        PhysCollisionSolver::calculateSweepInfo(queryShape.getPosition(),
                            queryShape.getPosition(),
                            queryShape.getAngle(),
                            queryShape.getPhysShape(),
                            playerSweepInfo);

                        if (PhysCollisionSolver::collide(playerSweepInfo, janodSweepInfo, contacts))
                        {
                            contacts.clear();
                            return btrue;
                        }                        
                    }
                }

            }
        }
        return bfalse;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AIJanodTweenedBehavior::update( f32 _delta  )
    {
        Super::update(_delta);
        m_timeInCurrentState += _delta;

        switch (m_state)
        {
            ///////////////////////////////////////////////////////////////////////////////////////////
            case State_Other:
            {                
            } break;
            ///////////////////////////////////////////////////////////////////////////////////////////
            case State_Tweened:
            {
                if (!m_physComponent->getDisabled()) //safety check
                {
                    m_physComponent->setDisabled(btrue);
                }
                if (checkIfSwitchToBristle())
                {
                    setState(State_Bristling);
                }
                else if (m_signaledTinyCrush)
                {
                    setState(State_ReactingToTinyCrush);
                }

            } break;
            ///////////////////////////////////////////////////////////////////////////////////////////
            case State_ReactingToTinyCrush:
            {
                if (m_animatedComponent->isMainNodeFinished())
                {
                    setState(State_Tweened);
                }
            } break;
            ///////////////////////////////////////////////////////////////////////////////////////////
            case State_Bristling:
                {
                    bbool isReallyBristling = m_timeInCurrentState>=LOGICDT;
                    Ray_JanodAIComponent *janod = m_actor->GetComponent<Ray_JanodAIComponent>();
                    ITF_ASSERT_CRASH(janod, "no janod component");
                    janod->setIsInBristleMode(isReallyBristling);

                    if (m_signaledTinyCrush && !checkIfSwitchToBristle())
                    {
                        setState(State_ReactingToTinyCrush);
                    }
                    else
                    {
                        if (isReallyBristling && checkIfSwitchToBristle() )
                        {
                            attackEnnemies();
                        }

                        if (m_timeInCurrentState>=getTemplate()->getBristlingTime())
                        {
                            setState(State_Tweened);
                        }
                    }
                } break;
        }

        m_signaledTinyCrush = bfalse;

    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AIJanodTweenedBehavior::onActionFinished()
    {
        Super::onActionFinished();

    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AIJanodTweenedBehavior::onEvent(Event * _event)
    {
        Super::onEvent(_event);
        Ray_JanodAIComponent *janod = m_actor->GetComponent<Ray_JanodAIComponent>();
        ITF_ASSERT_CRASH(janod, "no janod component");

        if ( EventCrushed *crushed = _event->DynamicCast<EventCrushed>(ITF_GET_STRINGID_CRC(EventCrushed,3831882623)))
        {
            if (crushed->getCharacterSize()==CHARACTERSIZE_SMALL)
                m_signaledTinyCrush = btrue;
        }
        else if ( PunchStim* hit = _event->DynamicCast<PunchStim>(ITF_GET_STRINGID_CRC(PunchStim,200533519)) )
        {
            janod->processHit(hit);            
        }
        /*
        else if ( Ray_EventAttackGranted* attackGranted = _event->DynamicCast<Ray_EventAttackGranted>(ITF_GET_STRINGID_CRC(Ray_EventAttackGranted,3058211789)) )
        {
            bbool attacks = m_state == State_Bristling;
            attackGranted->setAttackUsed(attacks);

            if (attacks && !wasCrushedRecently())
            {
                sendStim(attackGranted->getSender());
            }
        }
        */
    }

}

