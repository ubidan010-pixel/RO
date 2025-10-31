#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AIBONUSSKULLBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/SkullTeeth/Ray_AIBonusSkullBehavior.h"
#endif //_ITF_RAY_AIBONUSSKULLBEHAVIOR_H_

#ifndef _ITF_RAY_AICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_AIComponent.h"
#endif //_ITF_RAY_AICOMPONENT_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_TRIGGERCOMPONENT_H_
#include "gameplay/Components/Trigger/TriggerComponent.h"
#endif //_ITF_TRIGGERCOMPONENT_H_

#ifndef _ITF_RAY_GAMEPLAYEVENTS_H_
#include "rayman/gameplay/Ray_GameplayEvents.h"
#endif //_ITF_RAY_GAMEPLAYEVENTS_H_

#ifndef _ITF_RAY_FIXEDAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_FixedAIComponent.h"
#endif //_ITF_RAY_FIXEDAICOMPONENT_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#ifndef _ITF_AIMANAGER_H_
#include "gameplay/managers/AIManager.h"
#endif //_ITF_AIMANAGER_H_



#define SKULL_TEETH_COUNT 4

#define SKULL_HIDDEN		        "Idle"
#define SKULL_APPEARS		        "AppearAndStand"
//#define SKULL_STANDS                "Stand"
#define SKULL_DISAPPEARS            "AppearAndStand"

#define SKULL_TOOTH_PRESENT_FORMAT  "e%dPresence"



namespace ITF
{

    IMPLEMENT_OBJECT_RTTI(Ray_AIBonusSkullBehavior_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_AIBonusSkullBehavior_Template)
        SERIALIZE_MEMBER("defaultSizePercent", m_defaultSizePercent);
        SERIALIZE_MEMBER("maxSizePercent", m_maxSizePercent);
        SERIALIZE_MEMBER("pulseDuration", m_pulseDuration);
        SERIALIZE_MEMBER("maxSizeDuration", m_maxSizeDuration);
        SERIALIZE_MEMBER("standDuration", m_standDuration);
    END_SERIALIZATION()

    Ray_AIBonusSkullBehavior_Template::Ray_AIBonusSkullBehavior_Template()
    : m_defaultSizePercent(0.33f)
    , m_maxSizePercent(0.5f)
    , m_pulseDuration(1.5f)
    , m_maxSizeDuration(1.f)
    , m_standDuration(10.f)
    {
    }

    Ray_AIBonusSkullBehavior_Template::~Ray_AIBonusSkullBehavior_Template()
    {
    }

    IMPLEMENT_OBJECT_RTTI(Ray_AIBonusSkullBehavior)
    BEGIN_SERIALIZATION(Ray_AIBonusSkullBehavior)
    END_SERIALIZATION()

    BEGIN_VALIDATE_BEHAVIOR(Ray_AIBonusSkullBehavior)
    END_VALIDATE_BEHAVIOR()


    ///////////////////////////////////////////////////////////////////////////////////////////
    Ray_AIBonusSkullBehavior::Ray_AIBonusSkullBehavior()
        : Super()
        , m_state(State_NA)
        , m_timeInCurrentState(0)
        , m_skullScale(0.5f)
        , m_takenTeethCount(0)
    {
    }

    Ray_AIBonusSkullBehavior::~Ray_AIBonusSkullBehavior()
    {
        EVENTMANAGER->unregisterEvent(ITF_GET_STRINGID_CRC(EventRevertToLastCheckpoint,3654122082), m_aiComponent );
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AIBonusSkullBehavior::onActivate()
    {
        resetState();
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AIBonusSkullBehavior::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        m_takenTeethCount = 0;
        EVENTMANAGER_REGISTER_EVENT_LISTENER(ITF_GET_STRINGID_CRC(EventRevertToLastCheckpoint,3654122082),m_aiComponent);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventShowUI,3318916553), m_aiComponent);

        m_aiComponent->setFaction(RAY_FACTION_NEUTRAL);

        //RAY_GAMEMANAGER->setSkullActor(m_actor);

        resetState();
    }

    void Ray_AIBonusSkullBehavior::resetState()
    {
        setState(State_Hidden);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    bbool Ray_AIBonusSkullBehavior::updatePulse(f32 _t)
    {
        f32 pulseDuration = getTemplate()->getPulseDuration();
        f32 intensity = 0;
        bbool finished=bfalse;
        if (_t<pulseDuration)
        {
            intensity = sinf((_t*MTH_PIBY2)/pulseDuration);
        }
        else
        {
            f32 apexDuration = getTemplate()->getMaxSizeDuration();
            if (_t < (pulseDuration+apexDuration))
            {
                intensity = 1;
            }
            else if (_t<(pulseDuration+apexDuration+pulseDuration))
            {
                f32 percent = (_t-(pulseDuration+apexDuration))/pulseDuration;
                intensity = sinf((1-percent)*MTH_PIBY2);
            }
            else
            {
                intensity = 0;
                finished = btrue;
            }
        }


        m_skullScale = getTemplate()->getDefaultSizePercent()*(1-intensity)+
            getTemplate()->getMaxSizePercent()*intensity;
        f32 alpha = 1.f*(1-intensity)+0.5f*intensity;
        m_animComponent->setColor(Color(alpha,1,1,1));
        return finished;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AIBonusSkullBehavior::updateScaleAndPosition()
    {
        f32 scaleX = (f32)GFX_ADAPTER->getScreenWidth();
        f32 scaleY = (f32)GFX_ADAPTER->getScreenHeight();
        f32 scale = std::min(scaleX, scaleY)*m_skullScale;

        Vec2d p(GFX_ADAPTER->getScreenWidth()-scale*0.5f,
            scale*0.5f);

        m_actor->set2DPos(p);
        m_actor->setScale(Vec2d(scale,scale));
    }


    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AIBonusSkullBehavior::update( f32 _deltaTime )
    {
        Super::update(_deltaTime);

        m_timeInCurrentState += _deltaTime;

        updateScaleAndPosition();

        //Update the states. 
        switch (m_state)
        {
            ///////////////////////////////////////////////////////////////////////////////////////////
        case State_Hidden:
            {
            } break;
            ///////////////////////////////////////////////////////////////////////////////////////////
        case State_Appears:
            {
                if (updatePulse(m_timeInCurrentState))
                {
                    setState(State_Stands);
                }
            } break;
        case State_Stands:
            {
                if (m_timeInCurrentState>=getTemplate()->getStandDuration())
                {
                    setState(State_Disappears);
                }
            } break;
        case State_Disappears:
            {
                if (m_animatedComponent->isMainNodeFinished())
                {
                    setState(State_Hidden);
                }
            } break;
        }

    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AIBonusSkullBehavior::onEvent( Event* _event )
    {
        Super::onEvent(_event);

        if ( EventRevertToLastCheckpoint* eventRevert = _event->DynamicCast<EventRevertToLastCheckpoint>(ITF_GET_STRINGID_CRC(EventRevertToLastCheckpoint,3654122082)) )
        {
            resetState();
        }
        else if (Ray_EventShowUI *eventShow = _event->DynamicCast<Ray_EventShowUI>(ITF_GET_STRINGID_CRC(Ray_EventShowUI,3318916553)) )
        {
            setState(State_Appears);
        }

    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AIBonusSkullBehavior::setToothPresence(u32 _room, bbool _isPresent)
    {
        char buffer[256];
        f32 presenceWeight = _isPresent?1.f:0.f;
        sprintf(buffer, SKULL_TOOTH_PRESENT_FORMAT, _room+1);
        m_animatedComponent->setInput(buffer, presenceWeight);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    ///Set state, and initialize the state we enter
    void Ray_AIBonusSkullBehavior::setState(State_t _state)
    {
        if (_state!=m_state)
        {
            m_timeInCurrentState = 0;
            m_state = _state;
        }

        switch(_state)
        {
        case State_Hidden:
            m_animatedComponent->setAnim(SKULL_HIDDEN);
            m_animatedComponent->enableDraw(bfalse);
            m_animComponent->setColor(Color(1,1,1,1));
            m_skullScale = getTemplate()->getDefaultSizePercent();
            break;
        case State_Appears:
            {
                m_takenTeethCount = 1;//RAY_GAMEMANAGER->getTakenTeethCount();
                m_animatedComponent->setAnim(SKULL_APPEARS);
                m_animatedComponent->enableDraw(btrue);
                for (u32 i=0; i<SKULL_TEETH_COUNT; i++)
                {
                    setToothPresence(i,(i<=m_takenTeethCount));
                }

            } break;
        case State_Stands:
            {                    
            } break;
        case State_Disappears:
            {
                m_animatedComponent->setAnim(SKULL_DISAPPEARS);
            } break;
        }

    }


}
