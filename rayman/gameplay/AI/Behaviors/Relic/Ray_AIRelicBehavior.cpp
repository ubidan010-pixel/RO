#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AIRELICBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/Relic/Ray_AIRelicBehavior.h"
#endif //_ITF_RAY_AIRELICBEHAVIOR_H_

#ifndef _ITF_PHANTOMCOMPONENT_H_
#include "gameplay/Components/Misc/PhantomComponent.h"
#endif //_ITF_PHANTOMCOMPONENT_H_

#ifndef _ITF_RAY_AICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_AIComponent.h"
#endif //_ITF_RAY_AICOMPONENT_H_

#ifndef _ITF_PHYSPHANTOM
#include "engine/physics/PhysPhantom.h"
#endif //_ITF_PHYSPHANTOM

#ifndef _ITF_POLYLINECOMPONENT_H_
#include "gameplay/Components/Environment/PolylineComponent.h"
#endif //_ITF_POLYLINECOMPONENT_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_


#ifndef _ITF_TRIGGERCOMPONENT_H_
#include "gameplay/Components/Trigger/TriggerComponent.h"
#endif //_ITF_TRIGGERCOMPONENT_H_


#define RELIC_STAND_ANIM            "Stand"
#define RELIC_APPEARS_ANIM            "Appears"



namespace ITF
{

    IMPLEMENT_OBJECT_RTTI(Ray_AIRelicBehavior_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_AIRelicBehavior_Template)
        SERIALIZE_MEMBER("relicIndex", m_relicIndex);
    END_SERIALIZATION()

    Ray_AIRelicBehavior_Template::Ray_AIRelicBehavior_Template()
    : m_relicIndex(-1)
    {
    }

    IMPLEMENT_OBJECT_RTTI(Ray_AIRelicBehavior)
    BEGIN_VALIDATE_BEHAVIOR(Ray_AIRelicBehavior)
        VALIDATE_BEHAVIOR_PARAM("relicIndex", m_relicIndex>=0, "invalid relic index");
        VALIDATE_BEHAVIOR_PARAM("", m_triggerComponent!=NULL, "There must be a TriggerComponent for detecting picking");
    END_VALIDATE_BEHAVIOR()


    #define relicOutStopMarker ITF_GET_STRINGID_CRC(MRK_Out_GameplayStop,3127983330)

    ///////////////////////////////////////////////////////////////////////////////////////////
    Ray_AIRelicBehavior::Ray_AIRelicBehavior()
        : Super()
        , m_state(State_NA)
        , m_triggerComponent(NULL)
        , m_timeInCurrentState(0.0f)
        , m_isFromChest(bfalse)
        , m_relicIndex(-1)
    {        
    }

    Ray_AIRelicBehavior::~Ray_AIRelicBehavior()
    {
        if ( EVENTMANAGER )
        {
            EVENTMANAGER->unregisterEvent(ITF_GET_STRINGID_CRC(EventRevertToLastCheckpoint,3654122082), m_aiComponent);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AIRelicBehavior::setRelicIndex(i32 _relicIndex)
    {
        m_relicIndex=_relicIndex;
        m_animComponent->setBankState(m_relicIndex);
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AIRelicBehavior::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        m_relicIndex = getTemplate()->getRelicIndex();
        m_triggerComponent = m_actor->GetComponent<TriggerComponent>();
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301), m_aiComponent);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventRewardPickedUp,3660427225), m_aiComponent);
        EVENTMANAGER_REGISTER_EVENT_LISTENER(ITF_GET_STRINGID_CRC(EventRevertToLastCheckpoint,3654122082),m_aiComponent);
        //
        resetState();

    }

    void Ray_AIRelicBehavior::onResourceReady()
    {
        Super::onResourceReady();                        
    }

    void Ray_AIRelicBehavior::resetState()
    {
        if (RAY_GAMEMANAGER->getIsRelicTaken(m_relicIndex))
            setState(State_Taken);
        else
        {
            if (m_isFromChest)
            {
                setState(State_Appears);        
            }
            else
            {
                setState(State_Stands);
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AIRelicBehavior::update( f32 _deltaTime )
    {
        Super::update(_deltaTime);

        m_timeInCurrentState += _deltaTime;

        m_animComponent->setBankState(m_relicIndex);

        //Update the states. 
        switch (m_state)
        {
            ///////////////////////////////////////////////////////////////////////////////////////////
            case State_Preappears:
            {
                setState(State_Appears);
            }break;
            ///////////////////////////////////////////////////////////////////////////////////////////
            case State_Stands:
            {
            } break;
            ///////////////////////////////////////////////////////////////////////////////////////////
            case State_BeingTaken:
            {

                setState(State_Taken);
            } break;
            ///////////////////////////////////////////////////////////////////////////////////////////
            case State_Taken:
            {
            } break;
            ///////////////////////////////////////////////////////////////////////////////////////////
            case State_Appears:
            {
                //Fetch movement from root
                Vec3d delta;
                f32 zero=0;
                delta.setFromVec2d(m_animComponent->getRootDelta(), &zero);
                m_actor->forceMove(m_actor->getPos()+delta);

            } break;

        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_AIRelicBehavior::onEvent( Event* _event )
    {
        Super::onEvent(_event);

        if (AnimGameplayEvent *animEvent = _event->DynamicCast<AnimGameplayEvent>(ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301)))
        {
            if ((animEvent->getName()==relicOutStopMarker) && m_state==State_Appears)
            {
                setState(State_Stands);
            }
        }
        else if (Ray_EventRewardPickedUp *pickedupEvt = _event->DynamicCast<Ray_EventRewardPickedUp>(ITF_GET_STRINGID_CRC(Ray_EventRewardPickedUp,3660427225)))
        {
            setState(State_BeingTaken);
        }
        else if (EventRevertToLastCheckpoint *checkpointEvent = _event->DynamicCast<EventRevertToLastCheckpoint>(ITF_GET_STRINGID_CRC(EventRevertToLastCheckpoint,3654122082)))
        {
            resetState();
        }

    }



    ///////////////////////////////////////////////////////////////////////////////////////////
    ///Set state, and initialize the state we enter
    void Ray_AIRelicBehavior::setState(State_t _state)
    {
        if (_state!=m_state)
        {
            m_timeInCurrentState = 0;
            m_state = _state;
        }

        if (_state==State_Stands)
        {
            m_triggerComponent->unpause();
        }
        else
        {
            m_triggerComponent->pause();
        }

        switch(_state)
        {
            case State_Stands:
            {
                m_animComponent->setAnim(RELIC_STAND_ANIM);
                m_animComponent->setBankState(m_relicIndex);
            } break;
            case State_BeingTaken:
            {
                RAY_GAMEMANAGER->setCurrentLevelIsRelicTaken(m_relicIndex, btrue);
            } break;
            case State_Appears: 
            {
                m_actor->forceMove(m_actor->getWorldInitialPos());
                m_animComponent->setAnim(RELIC_APPEARS_ANIM);
                m_animComponent->setBankState(m_relicIndex);
            } break;
            case State_Taken:
            {
                m_actor->disable();
            } break;
        }
    }

}
