#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_STARGATEMANAGERAICOMPONENT_H_
#include "Rayman/gameplay/Components/AI/Ray_StargateManagerAIComponent.h"
#endif //_ITF_RAY_STARGATEMANAGERAICOMPONENT_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_


#define STATE_SGM_STEP_0       0
#define STATE_SGM_STEP_1       1
#define STATE_SGM_STEP_2       2
#define STATE_SGM_STEP_3       3


    namespace ITF
    {
    
    IMPLEMENT_OBJECT_RTTI(Ray_StargateManagerAIComponent)
    
    BEGIN_SERIALIZATION_CHILD(Ray_StargateManagerAIComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(Ray_StargateManagerAIComponent)
        VALIDATE_COMPONENT_PARAM("", m_linkComponent, "This component requires an LinkComponent.");
    END_VALIDATE_COMPONENT()

    const StringID flagWorld_JU = ITF_GET_STRINGID_CRC(ju_bossmaster,2141460896); // Jungle boss
    const StringID flagWorld_MU = ITF_GET_STRINGID_CRC(BossBird,3778756995);      // Music boss
    const StringID flagWorld_OC = ITF_GET_STRINGID_CRC(OC_B3,1116743380);         // Ocean boss
    const StringID flagWorld_FO = ITF_GET_STRINGID_CRC(FO_B3,241303687);         // Food boss


    //*****************************************************************************

    Ray_StargateManagerAIComponent::Ray_StargateManagerAIComponent()
    : m_worldChecked(bfalse)
    {
        // none
    }

    //*****************************************************************************

    Ray_StargateManagerAIComponent::~Ray_StargateManagerAIComponent()
    {
        // none
    }

    //*****************************************************************************

    void Ray_StargateManagerAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        // Get components
        m_linkComponent = m_actor->GetComponent<LinkComponent>();

         //Event
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTrigger,1343042510), this);

    }


    //*****************************************************************************

    void Ray_StargateManagerAIComponent::onBecomeActive( )
    {
        Super::onBecomeActive();

    }

    //*****************************************************************************

    void Ray_StargateManagerAIComponent::changeState( u32 _newState )
    {

        switch(_newState)
        {
        case STATE_SGM_STEP_1:
            {
                // Unlock all map B
                const StringID cmdUnlock = ITF_GET_STRINGID_CRC(TRIGGER_MOUNTAIN,611478250);
                Ray_EventWorldMapUnlock eventUnlockWorld_B;
                eventUnlockWorld_B.addUnlock(cmdUnlock);
                GAMEMANAGER->onEvent(&eventUnlockWorld_B);

                startSequence(1);

                break;
            }
        case STATE_SGM_STEP_2:
            startSequence(2);


            break;
        case STATE_SGM_STEP_3:
			startSequence(3);
            break;
        }



    }

    //*****************************************************************************

    bbool Ray_StargateManagerAIComponent::startSequence( i32 _numSequence  )
    {

        if (m_linkComponent)
        {
            const LinkComponent::ChildrenList& children  = m_linkComponent->getChildren();
            for (u32 i = 0; i < children.size(); i++)
            {
                Pickable* pickable = SceneObjectPathUtils::getObjectFromRelativePath(m_actor, children[i].getPath());

                if ( pickable )
                {
                    Actor* childActor = pickable->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));

                    if (childActor)
                    {
                        const StringID nameTag = ITF_GET_STRINGID_CRC(SequenceNum,3757833701);
                        i32 valueTag; 
                        if (children[i].getTagValue(nameTag, valueTag))
                        {
                            if (valueTag == _numSequence)
                            {
                                EventTrigger eventTrig;
                                eventTrig.setSender( m_actor->getRef() );
                                eventTrig.setActivated(btrue);
                                eventTrig.setActivator(m_actor->getRef());
                                childActor->onEvent(&eventTrig);

                                return btrue;
                            }
                        }                      
                    }
                }
            }
        }

        return bfalse;
    }

    //*****************************************************************************

    void Ray_StargateManagerAIComponent::onEvent(Event * _event)
    {
        Super::onEvent(_event);

        if ( EventTrigger* eventGen = _event->DynamicCast<EventTrigger>(ITF_GET_STRINGID_CRC(EventTrigger,1343042510)) )
        {
            processTrigger();
        }

    }

    //*****************************************************************************

    void Ray_StargateManagerAIComponent::processTrigger( )
    {
        if(!m_worldChecked)
        {
            ESpot_State stateBoss_JU = RAY_GAMEMANAGER->getWMSpotState(flagWorld_JU);
            ESpot_State stateBoss_MU = RAY_GAMEMANAGER->getWMSpotState(flagWorld_MU);
            ESpot_State stateBoss_OC = RAY_GAMEMANAGER->getWMSpotState(flagWorld_OC);
            ESpot_State stateBoss_FO = RAY_GAMEMANAGER->getWMSpotState(flagWorld_FO);


            const StringID flagWorld_B = ITF_GET_STRINGID_CRC(JU_B,2419180662); // One B map

            ESpot_State stateWorld_B = RAY_GAMEMANAGER->getWMSpotState(flagWorld_B);

            if ( 
                stateBoss_JU == SPOT_STATE_COMPLETED && 
                stateBoss_MU == SPOT_STATE_COMPLETED && 
                stateBoss_OC == SPOT_STATE_COMPLETED && 
                stateBoss_FO == SPOT_STATE_COMPLETED 
                )
            {
                // All boss are dead
                changeState(STATE_SGM_STEP_3);
            } 
            else if (stateWorld_B != SPOT_STATE_CLOSED)
            {
                changeState(STATE_SGM_STEP_2);
            }
            else
            {
                changeState(STATE_SGM_STEP_1);
            }

            m_worldChecked = btrue;
        }
    }

    //*****************************************************************************


    void Ray_StargateManagerAIComponent::Update( f32 _dt )
    {
        Super::Update(_dt);
     
    }

    //*****************************************************************************

#ifdef ITF_SUPPORT_EDITOR

    void Ray_StargateManagerAIComponent::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        Super::drawEdit(_drawInterface, _flags);

        if (_flags != ActorComponent::DrawEditFlag_All)
            return;

    }

#endif // ITF_SUPPORT_EDITOR

    //*****************************************************************************
    // Template
    //*****************************************************************************

    IMPLEMENT_OBJECT_RTTI(Ray_StargateManagerAIComponent_Template)

    BEGIN_SERIALIZATION_CHILD(Ray_StargateManagerAIComponent_Template)
    END_SERIALIZATION()

    //*****************************************************************************

    Ray_StargateManagerAIComponent_Template::Ray_StargateManagerAIComponent_Template()   
    {
        // none
    }

    //*****************************************************************************

    Ray_StargateManagerAIComponent_Template::~Ray_StargateManagerAIComponent_Template()
    {
        // none
    }

    //*****************************************************************************



}
