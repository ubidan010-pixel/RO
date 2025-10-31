#include "precompiled_Adapter_WWISE.h"
#ifdef ITF_SUPPORT_WWISE
#include "adapters/Adapter_WWISE/Adapter_WWISE.h"
#include "engine/events/Events.h"
#include "adapters/AudioSerializedData/Wwise/WwiseEvents.h"
//#include "core/IDServer.h"
#include "engine/actors/actor.h"
#include "engine/scene/SceneObjectPath.h"
#include "AudioSDK/error.h"
#include "gameplay/Managers/GameManager.h"
#include "adapters/Adapter_WWISE/Adapter_WWISE_MetronomeState.h"

namespace	ITF
{

    //
    // ===================================================================================================================
    // ===================================================================================================================
    //
    static Actor* getPlayerFromID(StringID _playerID)
    {

        for (u32 i = 0; i < GAMEMANAGER->getMaxPlayerCount(); ++i)
        {
            Actor* player = GAMEMANAGER->getActivePlayer(i);
            if (player && StringID(player->getUserFriendly()) == _playerID)
            {
                return player;
            }
        }

        return NULL;
    }

    //
    // ===================================================================================================================
    // ===================================================================================================================
    //
    static Actor* getActorFromObjectPath(Actor* _father, const ObjectPath& _friendlyName, const StringID& _friendlyID)
    {


        Pickable* pick = SceneObjectPathUtils::getObjectFromRelativePath(_father, _friendlyName);
        if (pick)
        {
            if (pick->getObjectType() == BaseObject::eActor)
                return (Actor*)pick;
            else
                return NULL;
        }

        return getPlayerFromID(_friendlyID);
    }

    //
    // ===================================================================================================================
    // ===================================================================================================================
    //
    void Adapter_WWISE::onEvent(Event* _event)
    {

        if (EventSetWwiseState* onSetWwiseState = _event->DynamicCast<EventSetWwiseState>(ITF_GET_STRINGID_CRC(EventSetWwiseState, 3353127953)))
        {
            const StringID		stateGroupGUID = onSetWwiseState->getStateGroupGUID();
            const StringID		stateGUID = onSetWwiseState->getStateGUID();

            const SoundRtpcID	stateGroupID = getIDFromGUID(stateGroupGUID);
            const SoundRtpcID	stateID = getIDFromGUID(stateGUID);

            setRtpc(stateGroupID, stateID, ObjectRef::InvalidRef);
        }
//         else if (EventLoadWwiseBank* onEventLoadWwiseBank = _event->DynamicCast<EventLoadWwiseBank>(ITF_GET_STRINGID_CRC(EventLoadWwiseBank, 3664813256)))
//         {
//             PathChar_t				bankPath[PATH_C_BUFFERSIZE];
//             const ITF_VECTOR<Path>& list = onEventLoadWwiseBank->getBankList();
// 
//             for (ITF_VECTOR<Path>::const_iterator it = list.cbegin(), itEnd = list.cend(); it != itEnd; ++it)
//             {
//                 //	it->toBuffer(bankPath);
//                 //loadBank(bankPath, onEventLoadWwiseBank->getSender()); //todo isap
//             }
//         }
        // 		else if(EventUnloadWwiseBank * onEventUnloadWwiseBank = DYNAMIC_CAST(_event, EventUnloadWwiseBank))
        // 		{
        // 			PathChar_t				bankPath[PATH_C_BUFFERSIZE];
        // 			const ITF_VECTOR<Path>	&list = onEventUnloadWwiseBank->getBankList();
        // 
        // 			for(ITF_VECTOR<Path>::const_iterator it = list.cbegin(), itEnd = list.cend(); it != itEnd; ++it)
        // 			{
        // 				//it->toBuffer(bankPath);
        // 				unloadBank(bankPath, onEventUnloadWwiseBank->getSender());
        // 			}
        // 		}
        else if (EventPlayWwiseEvent* onSendEventWwiseEvent = _event->DynamicCast<EventPlayWwiseEvent>(ITF_GET_STRINGID_CRC(EventPlayWwiseEvent, 4259299401)))
        {
            const StringID		eventGUID = onSendEventWwiseEvent->getEventGUID();
            const SoundEventID	eventID = getIDFromGUID(eventGUID);
            AudioPlayRequest	playRequest(eventID,
                onSendEventWwiseEvent->getSender(),
                onSendEventWwiseEvent->stopOnDestroy() == btrue,
                onSendEventWwiseEvent->getMetronomeID(),
                onSendEventWwiseEvent->getPlayAt(),
                onSendEventWwiseEvent->getPlayAtCue());

            if (onSendEventWwiseEvent->getOnPlayEvent())
            {
                if (onSendEventWwiseEvent->getMetronomeID() != METRONOME_TYPE_INVALID)
                {
                    // get sender (+ by default sendTo==sender)
                    const ObjectRef& senderRef = onSendEventWwiseEvent->getSender();
                    Actor* sendTo = static_cast<Actor*>(GETOBJECT(senderRef));
                    const Actor* senderActor = sendTo;

                    if (senderActor)
                    {
                        ITF_STDSTRING	friendlyStr;
                        onSendEventWwiseEvent->getOnPlayEventObjectPath().toString(friendlyStr);

                        StringID	objectID = friendlyStr;

                        sendTo = getActorFromObjectPath(sendTo, onSendEventWwiseEvent->getOnPlayEventObjectPath(), objectID);
                    }
                    //                         else
                    //                         {
                    //                             Pickable* pickable = SceneObjectPathUtils::getObjectFromAbsolutePath(onSendEventWwiseEvent->getOnPlayEventObjectPath());
                    //                             if (pickable) sendTo = DYNAMIC_CAST(pickable, Actor);
                    //                             senderActor = sendTo;
                    //                         }

                    if (sendTo)
                    {
                        onSendEventWwiseEvent->getOnPlayEvent()->setSender(senderActor->getRef());

                        if (onSendEventWwiseEvent->getPlayAt() == AUDIO_SYNC_PLAY_IMMEDIATE)
                        {
                            // immediate call
                            sendTo->onEvent(onSendEventWwiseEvent->getOnPlayEvent());
                        }
                        else
                        {
                            playRequest.setOnPlayEvent(onSendEventWwiseEvent->getOnPlayEvent());
                            playRequest.m_sentToObjectRef = sendTo->getRef();
                        }
                    }
                    else
                    {
                        AUDIO_WARNING("EventPlayWwiseEvent : The \"sento\" ActorRef is unknown.");
                    }
                }
                else
                {
                    AUDIO_WARNING("EventPlayWwiseEvent : there is a callback event but no metronome.");
                }
            }

            play(playRequest);
        }
        // 		else if(SendMetronomedEvent * onSendEventWwiseEvent = DYNAMIC_CAST(_event, SendMetronomedEvent))
        // 		{
        // 			AudioSendEventRequest	sendEventRequest(
        // 				onSendEventWwiseEvent->getMetronomeID(),
        // 				onSendEventWwiseEvent->getPlayAt(),
        // 				onSendEventWwiseEvent->getPlayAtCue());
        // 
        // 			if(onSendEventWwiseEvent->getOnPlayEvent())
        // 			{
        // 				if(onSendEventWwiseEvent->getMetronomeID() != METRONOME_TYPE_INVALID)
        // 				{
        // 					// get sender (+ by default sendTo==sender)
        // 					const ObjectRef &senderRef = onSendEventWwiseEvent->getSender();
        // 					Actor			*sendTo = static_cast<Actor *>(GETOBJECT(senderRef));
        // 					Actor			*senderActor = sendTo;
        // 
        // 					if(senderActor)
        // 					{
        // 						ITF_STDSTRING	friendlyStr;
        // 						onSendEventWwiseEvent->getOnPlayEventObjectPath().toString(friendlyStr);
        // 
        // 						StringID	objectID = friendlyStr;
        // 
        // 						sendTo = getActorFromObjectPath(sendTo, onSendEventWwiseEvent->getOnPlayEventObjectPath(), objectID);
        // 					}
        // 					else
        // 					{
        // 						Pickable	*pickable = SceneObjectPathUtils::getObjectFromAbsolutePath(onSendEventWwiseEvent->getOnPlayEventObjectPath());
        // 						if(pickable) sendTo = DYNAMIC_CAST(pickable, Actor);
        // 						senderActor = sendTo;
        // 					}
        // 					if(!sendTo)
        // 						sendTo = senderActor;
        // 					if(sendTo)
        // 					{
        // 						if( !onSendEventWwiseEvent->getOnPlayEvent()->getSender().isValid() )
        // 							onSendEventWwiseEvent->getOnPlayEvent()->setSender(senderActor->getRef());
        // 
        // 						if(onSendEventWwiseEvent->getPlayAt() == AUDIO_SYNC_PLAY_IMMEDIATE)
        // 						{
        // 							// immediate call
        // 							sendTo->onEvent(onSendEventWwiseEvent->getOnPlayEvent());
        // 						}
        // 						else
        // 						{
        // 							sendEventRequest.setOnPlayEvent(onSendEventWwiseEvent->getOnPlayEvent());
        // 							sendEventRequest.m_sentToObjectRef = sendTo->getRef();
        // 						}
        // 					}
        // 					else
        // 					{
        // 						AUDIO_WARNING("SendMetronomedEvent : The \"sento\" ActorRef is unknown.");
        // 					}
        // 				}
        // 				else
        // 				{
        // 					AUDIO_WARNING("SendMetronomedEvent : there is a callback event but no metronome.");
        // 				}
        // 			}
        // 			m_metronomeState[sendEventRequest.m_metronomeID]->sendEventOnNext(sendEventRequest);
        // 		}
        // 		else if(EventSetWwiseAuxBusEffect * onEventSetWwiseAuxBusEffect = DYNAMIC_CAST(_event, EventSetWwiseAuxBusEffect))
        // 		{
        // 			const StringID			auxBusGUID = onEventSetWwiseAuxBusEffect->getEffectGUID();
        // 			const StringID			busGUID = onEventSetWwiseAuxBusEffect->getBusGUID();
        // 			const AudioBusSlotID	slotID = onEventSetWwiseAuxBusEffect->getBusSlotID();
        // 
        // 			const SoundEffectID		auxBusID = getIDFromGUID(auxBusGUID);
        // 			const SoundBusID		busID = getIDFromGUID(busGUID);
        // 
        // 			setBusEffect(busID, slotID, auxBusID);
        // 		}
        // 		else if(EventResetWwiseAuxBusEffect * onEventResetWwiseAuxBusEffect = DYNAMIC_CAST(_event, EventResetWwiseAuxBusEffect))
        // 		{
        // 			const StringID			busGUID = onEventResetWwiseAuxBusEffect->getBusGUID();
        // 			const AudioBusSlotID	slotID = onEventResetWwiseAuxBusEffect->getBusSlotID();
        // 			const SoundBusID		busID = getIDFromGUID(busGUID);
        // 
        // 			//resetBusEffect(busID, slotID);
        // 			reInitBusEffect(busID, slotID);
        // 		}
    }
}
#endif // ITF_SUPPORT_WWISE
