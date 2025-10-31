#include "precompiled_gameplay_war.h"

#ifndef _ITF_WAR_GAMEINTERFACE_H_
#include "war/gameplay/War_GameInterface.h"
#endif //_ITF_WAR_GAMEINTERFACE_H_

#ifndef _ITF_GAMEPLAYFACTORYFILLER_H_
#include "gameplay/GameplayFactoryFiller.h"
#endif //_ITF_GAMEPLAYFACTORYFILLER_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_



/********* GAME-SPECIFIC MANAGERS ***********/

#ifndef _ITF_WAR_GAMEMANAGER_H_
#include "war/gameplay/War_GameManager.h"
#endif //_ITF_WAR_GAMEMANAGER_H_



/********* GAMEPLAY COMPONENTS ***********/

#ifndef _ITF_WAR_PLAYERCONTROLLERCOMPONENT_H_
#include "war/gameplay/Components/Player/War_PlayerControllerComponent.h"
#endif //_ITF_WAR_PLAYERCONTROLLERCOMPONENT_H_

#ifndef _ITF_WAR_AIATTACKGROUNDNPCCOMPONENT_H_
#include "war/gameplay/Components/AI/War_AIAttackGroundNpcComponent.h"
#endif //_ITF_WAR_AIATTACKGROUNDNPCCOMPONENT_H_

#ifndef _ITF_WAR_PICKUPCOMPONENT_H_
#include "war/gameplay/Components/Misc/War_PickupComponent.h"
#endif //_ITF_WAR_PICKUPCOMPONENT_H_

#ifndef _ITF_WAR_AITARGETCOMPONENT_H_
#include "war/gameplay/Components/Misc/War_AITargetComponent.h"
#endif //_ITF_WAR_AITARGETCOMPONENT_H_



/********* GAMEPLAY AI Behaviours ***********/

#ifndef _ITF_WAR_AIROAMINGBEHAVIOR_H_
#include "war/gameplay/ai/Behaviors/War_AIRoamingBehavior.h"
#endif //_ITF_WAR_AIROAMINGBEHAVIOR_H_

#ifndef _ITF_WAR_AIATTACKGROUNDBEHAVIOR_H_
#include "war/gameplay/ai/Behaviors/War_AIAttackGroundBehavior.h"
#endif //_ITF_WAR_AIATTACKGROUNDBEHAVIOR_H_

#ifndef _ITF_WAR_AIRECEIVEHITBEHAVIOR_H_
#include "war/gameplay/AI/Behaviors/War_AIReceiveHitBehavior.h"
#endif //_ITF_WAR_AIRECEIVEHITBEHAVIOR_H_



/********* GAMEPLAY AI Actions ***********/

#ifndef _ITF_WAR_AIRECEIVEHITACTION_H_
#include "war/gameplay/ai/Actions/War_AIReceiveHitAction.h"
#endif //_ITF_WAR_AIRECEIVEHITACTION_H_

#ifndef _ITF_WAR_AIPERFORMHITRAYCASTACTION_H_
#include "war/gameplay/ai/Actions/War_AIPerformHitRaycastAction.h"
#endif //_ITF_WAR_AIPERFORMHITRAYCASTACTION_H_

#ifndef _ITF_WAR_AIPERFORMHITSTIMACTION_H_
#include "war/gameplay/ai/Actions/War_AIPerformHitStimAction.h"
#endif //_ITF_WAR_AIPERFORMHITSTIMACTION_H_


#ifndef _ITF_WAR_WORLDFILE_H_
#include "war/gameplay/War_WorldFile.h"
#endif //_ITF_WAR_WORLDFILE_H_


namespace ITF
{
    War_GameInterface::War_GameInterface()
    {
        War_GameManager::create();
    }

    War_GameInterface::~War_GameInterface()
    {
        War_GameManager::destroy();
    }

    void War_GameInterface::init()
    {
        GameInterface::init();

        GAMEMANAGER->init();
    }

    void War_GameInterface::preWorldUpdate( f32 _dt )
    {
        GAMEMANAGER->update(_dt);
    }
    void War_GameInterface::postWorldUpdate( f32 _dt )
    {
        Super::postWorldUpdate(_dt);
    }

    void War_GameInterface::registerObjects() const
    {
        // include generic components
        GameplayFactoryFiller::fillFactories();


        ObjectFactory* actorComponentsFactory = ACTORSMANAGER->getActorComponentsFactory();
        ObjectFactory* aiBehaviorsFactory = ACTORSMANAGER->getAIBehaviorsFactory();
        ObjectFactory* aiActionsFactory = ACTORSMANAGER->getAIActionsFactory();
        ObjectFactory* eventsFactory = ACTORSMANAGER->getEventFactory();

        // AI Components
        actorComponentsFactory->RegisterObject<War_PlayerControllerComponent>(ITF_GET_STRINGID_CRC(War_PlayerControllerComponent,2852347349));
        actorComponentsFactory->RegisterObject<War_AIAttackGroundNpcComponent>(ITF_GET_STRINGID_CRC(War_AIAttackGroundNpcComponent,889251451));
        actorComponentsFactory->RegisterObject<War_PickupComponent>(ITF_GET_STRINGID_CRC(War_PickupComponent,1727468072));
        actorComponentsFactory->RegisterObject<War_AITargetComponent>(ITF_GET_STRINGID_CRC(War_AITargetComponent,3124125847));

        // AI Behaviourse
        aiBehaviorsFactory->RegisterObject<War_AIRoamingBehavior>(ITF_GET_STRINGID_CRC(War_AIRoamingBehavior,3749437115));
        aiBehaviorsFactory->RegisterObject<War_AIAttackGroundBehavior>(ITF_GET_STRINGID_CRC(War_AIAttackGroundBehavior,718234413));
        aiBehaviorsFactory->RegisterObject<War_AIReceiveHitBehavior>(ITF_GET_STRINGID_CRC(War_AIReceiveHitBehavior,3140134935));

        // AI Actions
        aiActionsFactory->RegisterObject<War_AIReceiveHitAction>(ITF_GET_STRINGID_CRC(War_AIReceiveHitAction,3967443887));
        aiActionsFactory->RegisterObject<War_AIPerformHitRaycastAction>(ITF_GET_STRINGID_CRC(War_AIPerformHitRaycastAction,3352508404));
        aiActionsFactory->RegisterObject<War_AIPerformHitStimAction>(ITF_GET_STRINGID_CRC(War_AIPerformHitStimAction,582620550));


        // Events
        eventsFactory->RegisterObject<War_EventAttackRequestGranted>(ITF_GET_STRINGID_CRC(War_EventAttackRequestGranted,855934952));
        eventsFactory->RegisterObject<War_EventPickup>(ITF_GET_STRINGID_CRC(War_EventPickup,3375386123));

        // Stims
        eventsFactory->RegisterObject<War_HitStim>(ITF_GET_STRINGID_CRC(War_HitStim,3270006373));

        // World File
        WorldFile::registerProjectData<War_WorldFile>();
    }
}
