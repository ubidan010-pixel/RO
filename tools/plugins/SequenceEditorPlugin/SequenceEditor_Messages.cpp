// GLOBAL ENGINE CONFIG FILE. Must be the first included file
#ifndef _ENGINECOMMON_H_
    #include "core/enginecommon.h"
#endif // _ENGINECOMMON_H_

#if !defined(ITF_FINAL) && defined(ITF_DEBUG_STRINGID)


#ifndef _ITF_SEQUENCEEDITORMESSAGES_H_
#include "SequenceEditor_Messages.h"
#endif //_ITF_SEQUENCEEDITORMESSAGES_H_

#ifndef _ITF_PLUGINGATEWAY_H_
#include "PluginGateway/PluginGateway.h"
#endif //_ITF_PLUGINGATEWAY_H_

#ifndef _ITF_EDITOR_H_
#include "editor/editor.h"
#endif //_ITF_EDITOR_H_

#ifndef _ITF_LUAHANDLER_H_
#include "core/script/LUAHandler.h"
#endif //_ITF_LUAHANDLER_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_
#include "engine/sound/SoundDescriptor.h"

#ifndef _ITF_SEQUENCEPLAYERCOMPONENT_H_
#include "engine/sequenceplayer/SequencePlayerComponent.h"
#endif //_ITF_SEQUENCEPLAYERCOMPONENT_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_SEQUENCEEDITORPLUGIN_H_
#include "SequenceEditorPlugin.h"
#endif //_ITF_SEQUENCEEDITORPLUGIN_H_

#ifndef _ITF_ID_SERVER_H_
#include "core/IDserver.h"
#endif //_ITF_ID_SERVER_H_

#ifndef _ITF_ANIMATEDCOMPONENT_H_
#include "engine/actors/components/animatedcomponent.h"
#endif //_ITF_ANIMATEDCOMPONENT_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_ACTOREDITOR_H_
#include "tools/plugins/ActorEditor/ActorEditor.h"
#endif //_ITF_ACTOREDITOR_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_UNDOREDOMANAGER_H_
#include "editor/UndoRedoManager.h"
#endif //_ITF_UNDOREDOMANAGER_H_

namespace ITF
{

    SequenceEditorMessages::SequenceEditorMessages()
        : m_owner(NULL), m_previousUpdateCmdState(btrue), m_listPlayersSendOk(bfalse)
    {
    }

    void SequenceEditorMessages::receive(Blob& _blob, const NETPeer* _peer, u32 _senderID)
    {
        String message;
        _blob.extractString(message);
        #ifdef DEBUG_SEQUENCES
            StringConverter mmsg(message);
            if (m_owner->m_log)
                fprintf(m_owner->m_log, "received message: %s\n", mmsg.getChar());
        #endif
        
        if(message == playSequenceCmdName())
            onPlaySequenceCmd(_blob);
        else if(message == stopSequenceCmdName())
            onStopSequenceCmd(_blob);
        else if (message == addNewSequenceCmdName())
            onAddNewSequenceCmd(_blob);
        else if(message == setCurrentFrameCmdName())
            onSetCurrentFrameCmd(_blob);
        else if(message == getSessionInfoCmdName())
            onGetSessionInfoCmd(_blob);
        else if(message == getListSequenceCmdName())
            onGetListSequenceCmd(_blob);
        else if(message == getListActorCmdName())
            onGetListActorCmd(_blob);
        else if(message == getListPlayersCmdName())
            onGetListPlayersCmd(_blob);
        else if(message == getSequenceCmdName())
            onGetSequenceCmd(_blob);
        else if (message == getInfoListAnimationsByFriendlyCmdName())
            onGetInfoListAnimationsByFriendlyCmd(_blob, bfalse);
        else if (message == getInfoListSubAnimsByFriendlyCmdName())
            onGetInfoListAnimationsByFriendlyCmd(_blob, btrue);
        else if (message == getInfoListSoundByFriendlyCmdName())
            onGetInfoListSoundByFriendlyCmd(_blob);
        else if(message == sendUnselectCmdName())
            onUnselectCmd(_blob);
        else if(message == sendSelectedEventsCmdName())
            onSelectedEventsCmd(_blob);
        else if(message == sendSaveAllSequencesCmdName())
            onSaveAllSequencesCmd(_blob);
        else if(message == sendSaveOneSequenceCmdName())
            onSaveOneSequenceCmd(_blob);
        else if(message == sendSaveWorldCmdName())
            onSaveWorldCmd(_blob);
        else if(message == addEventCmdName())
            onAddEventCmd(_blob);
        else if(message == removeEventCmdName())
            onRemoveEventCmd(_blob);
        else if(message == updateEventCmdName())
            onUpdateEventCmd(_blob);
        else if(message == addTrackCmdName())
            onAddTrackCmd(_blob);
        else if(message == removeTrackCmdName())
            onRemoveTrackCmd(_blob);
        else if(message == updateTrackCmdName())
            onUpdateTrackCmd(_blob);
        else if(message == groupTracksCmdName())
            onGroupTracksCmd(_blob);
        else if(message == ungroupTracksCmdName())
            onUngroupTracksCmd(_blob);
        else if(message == splitEventCmdName())
            onSplitEventCmd(_blob);
        else if(message == mergeEventCmdName())
            onMergeEventCmd(_blob);
        else if (message == getGameplayEventsListCmdName())
            onGetGameplayEventsListCmdName(_blob);
        else if (message == setRecordModeStateCmdName())
            onSetRecordModeStateCmd(_blob);
        else if (message == getRecordModeStateCmdName())
            sendRecordModeState();
        else if (message == setDefaultSplineModeCmdName())
            onSetDefaultSplineModeCmd(_blob);
        else if (message == getDefaultSplineModeCmdName())
            sendDefaultSplineMode();
        else if (message == setBezierLinkedCmdName())
            onSetBezierLinkedCmd(_blob);
        else if (message == getBezierLinkedCmdName())
            sendBezierLinked();
        else if (message == setActicePlayerListCmdName())
            onSetActivePlayerListCmd(_blob);
        else if (message == setDisablePlayersCmdName())
            onSetDisablePlayersCmd(_blob);
        else if (message == setReactivatePlayersAliveCmdName())
            onSetReactivatePlayersAliveCmd(_blob);
        else if (message == setReactivatePlayersCmdName())
            onSetReactivatePlayersCmd(_blob);
        else if (message == setSaveInitialViewCmdName())
            onSetSaveInitialViewCmd(_blob);
        else if (message == getActicePlayerListCmdName())
            sendActivePlayerList();
        else if (message == setInactivateChannels())
            onSetInactivateChannels(_blob);
        else if (message == setCameraModeStateCmdName())
            onSetCameraModeStateCmd(_blob);
        else if (message == getCameraModeStateCmdName())
            sendCameraModeState();
        else if (message == setCameraOnActorCmdName())
            setCameraOnActor(_blob);
        else if(message == setPreviewPlayAnimCmdName())
            m_owner->getActorEditor().setPreviewPlayAnim(_blob);
        else if(message == stopPreviewPlayAnimCmdName())
            m_owner->getActorEditor().stopPreviewPlayAnim(_blob);
        else if(message == undoCmdName())
            UNDOREDO_MANAGER->requestUndo(); // Do not need to send data -> done by serialization
        else if(message == redoCmdName())
            UNDOREDO_MANAGER->requestRedo(); // Do not need to send data -> done by serialization
        else if (message == exitCmdName())
            m_owner->onToolDisconnect(_peer);

        else 
            ITF_ASSERT(0); // unknown command
    }

    Actor* SequenceEditorMessages::getSequence(Blob& _blob)
    {
        ObjectRef sequenceRef = _blob.extractUint32();
        Actor* sequence = (Actor*) GETOBJECT(sequenceRef);
            
        if (!sequence)
            return NULL;

        if (!m_owner->setCurrentSequenceActor(sequence, /*bfalse*/btrue))
            return NULL;

        return sequence;
    }

    void SequenceEditorMessages::onUnselectCmd(Blob& _blob)
    {
        m_owner->UnselectAll(btrue);
    }

    void SequenceEditorMessages::onSelectedEventsCmd( Blob& _blob )
    {
        m_owner->UnselectAll(btrue);
        Actor* actSequence = getSequence(_blob);
        
        if (actSequence)
            m_owner->setCurrentSequenceActor(actSequence, bfalse);

        u32 count = _blob.extractUint32();
        for (u32 i = 0; i < count; i++)
        {
            u32 uid = _blob.extractUint32();
            m_owner->selectEvent( uid, _blob.extractInt32());
        }

        m_owner->processSelectedEvent();
    }

    void SequenceEditorMessages::updateLUA(Blob& _blob)
    {
        String message;
        _blob.extractString(message);
        StringConverter script(message);
        LUA_HANDLER->executeString(script.getChar());
    }

    void SequenceEditorMessages::onReceiveSequence(Blob& _blob)
    {
        String message;
        _blob.extractString(message);
    }

    
    void SequenceEditorMessages::onStopSequenceCmd(Blob& _blob)
    {
        Actor* actSequence = getSequence(_blob);
        if (actSequence)
        {
            SequencePlayerComponent *seqPlayComp = actSequence->GetComponent<SequencePlayerComponent>();
            if (seqPlayComp)
            {
                seqPlayComp->stop();
                seqPlayComp->allowCameraControl(bfalse);
#ifdef DEBUG_SEQUENCES
                if (m_owner->m_log)
                    fputs("stop sequence & disable camera control\n", m_owner->m_log);
#endif
            }
            m_owner->sendCurrentFrame(actSequence->getRef(), 0);
        }     

    }


    void SequenceEditorMessages::onAddEventCmd(Blob& _blob)
    {
        Actor* actSequence = getSequence(_blob);

        if (!actSequence)
            return;

        SequencePlayerComponent          *seqPlayComp         = m_owner->getCurrentSequenceComponent();
        SequencePlayerComponent_Template *seqPlayCompTemplate = m_owner->getCurrentSequenceComponentTemplate();
        if (!seqPlayComp || !seqPlayCompTemplate)
            return;

        seqPlayComp->unpause();

        bbool somethingDone = bfalse;
        u32 countEvts = (u32)_blob.extractInt32();

        for (u32 i = 0; i < countEvts; i++)
        {
            SequenceEvent_Template* evt = seqPlayCompTemplate->createEventFromBlob(&_blob);
            bbool selectedAfterAdd = _blob.extractBool();
        
            if(evt)
            {
                somethingDone = btrue;
                evt->setSelected(selectedAfterAdd ? 0 : -1);
            }
        }

        if(somethingDone)
        {
            m_owner->fillEmptyTrajectories(actSequence->getPos());
            m_owner->propagateActorChanges(actSequence, btrue);
        }
    }

    void SequenceEditorMessages::onUpdateEventCmd(Blob& _blob)
    {
        Actor* actSequence = getSequence(_blob);
        bbool returnValues = _blob.extractBool();
        bbool isMajor = _blob.extractBool();

        if (!actSequence)
            return;

        SequencePlayerComponent *seqPlayComp = m_owner->getCurrentSequenceComponent();
        SequencePlayerComponent_Template *seqPlayCompTemplate = m_owner->getCurrentSequenceComponentTemplate();
        if (!seqPlayComp || !seqPlayCompTemplate)
            return;

        bbool valide = btrue;

        // send the response / validation
        ITF::Blob blob(ITF::BLOB_EXTRACT_AND_WRITE_ENABLED);
        blob.pushString(SequenceEditorPlugin::getPluginName());                  // plugin ID
        blob.pushString(SequenceEditorMessages::updateEventIsDoneCmdName());     // cmd ID
        blob.pushUInt32(seqPlayComp->GetActor()->getRef().getValue());              // sequence ObjectRef
        
        i32 countEvts = _blob.extractInt32();                
        blob.pushUInt32(countEvts);


        for (i32 i = 0; i < countEvts; i++)
        {
            u32 uid = _blob.extractUint32();
            SequenceEvent_Template* evt = seqPlayCompTemplate->getEventTemplateWithUID(uid);
            if (evt)
            {
                evt->fillFromBlob(&_blob);

                blob.pushBool(returnValues);                                             // return new event values to tool
                blob.pushUInt32(uid);                                    // event ObjectRef
            
                if(returnValues)
                    evt->serializeToBlob(&blob);
            }
            else
            {
                valide = bfalse;
                break;
            }
        }

        if(valide)
        {
            if (isMajor)
            {
                m_owner->fillEmptyTrajectories(actSequence->getPos());
				m_owner->propagateActorChanges(actSequence, m_previousUpdateCmdState, bfalse);
            } else
            {
				if (returnValues)
                    PLUGINGATEWAY->sendPacket(blob, SequenceEditorPlugin::getNetPeer(), SequenceEditorPlugin::getSenderID());

				seqPlayComp->refreshEnableEventsByChannel();
				seqPlayComp->refreshLimitFrameSequence();

				m_owner->propagateActorChanges(actSequence, m_previousUpdateCmdState, btrue, returnValues);
                m_owner->frameStep(0);

				if (returnValues)
				{
					m_owner->processSelectedEvent();
				}
            }
        }
        else
        {
            m_owner->revertUnpropagateActorChanges(actSequence);
        }

        m_previousUpdateCmdState = returnValues;
    }

    void SequenceEditorMessages::onRemoveEventCmd           (   Blob& _blob )
    {
        Actor* actSequence = getSequence(_blob);    // extract actorID and getActor
        
        if (!actSequence)
            return;

        SequencePlayerComponent          *seqPlayComp         = m_owner->getCurrentSequenceComponent();
        SequencePlayerComponent_Template *seqPlayCompTemplate = m_owner->getCurrentSequenceComponentTemplate();
        if (!seqPlayComp || !seqPlayCompTemplate)
            return;

        bbool valide = btrue;
        i32 countEvts = _blob.extractInt32();                
        for (i32 i = 0; i < countEvts; i++)
        {
            u32 removeUID =_blob.extractUint32();   // extract ObjectRef of event removing

            SequenceEvent_Template* evt = seqPlayCompTemplate->getEventTemplateWithUID(removeUID);
            if (evt)
            {
                seqPlayCompTemplate->removeEvent(removeUID);
            }
            else
            {
                valide = bfalse;
                break;
            }
        }
        
        if (valide)
        {
            m_owner->propagateActorChanges(actSequence, btrue);
        } else
        {
            m_owner->revertUnpropagateActorChanges(actSequence);
        }
    }

    void SequenceEditorMessages::onAddTrackCmd( Blob& _blob )
    {
        Actor* actSequence = getSequence(_blob);

        if (!actSequence)
            return;

        SequencePlayerComponent          *seqPlayComp         = m_owner->getCurrentSequenceComponent();
        SequencePlayerComponent_Template *seqPlayCompTemplate = m_owner->getCurrentSequenceComponentTemplate();
        if (!seqPlayComp || !seqPlayCompTemplate)
            return;
        
        seqPlayComp->unpause();

        u32 trackIndex = _blob.extractUint32();                

        seqPlayCompTemplate->checkIndex( trackIndex );
        ITF_VECTOR<SequenceTrackInfo_Template> & trackList = seqPlayCompTemplate->getTrackList();
        u32 parentGroup = trackList[trackIndex].getParentGroup();

        if(seqPlayCompTemplate->insertTrack(trackIndex))
        {
            trackList[trackIndex].setParentGroup(parentGroup);
            m_owner->propagateActorChanges(actSequence, btrue);
        }
    }

    void SequenceEditorMessages::onRemoveTrackCmd( Blob& _blob )
    {
        Actor* actSequence = getSequence(_blob);    // extract actorID and getActor

        if (!actSequence)
            return;

        SequencePlayerComponent          *seqPlayComp         = m_owner->getCurrentSequenceComponent();
        SequencePlayerComponent_Template *seqPlayCompTemplate = m_owner->getCurrentSequenceComponentTemplate();
        if (!seqPlayComp || !seqPlayCompTemplate)
            return;

        SafeArray<u32> tracksToDelete;

        u32 countTracks = _blob.extractUint32();  
        tracksToDelete.reserve(countTracks);
        for (u32 i = 0; i < countTracks; i++)
        {
            tracksToDelete.push_back(_blob.extractUint32());
        }

        // if some tracks are groups, search for children track to also remove them
        ITF_VECTOR<SequenceTrackInfo_Template> & trackList = seqPlayCompTemplate->getTrackList();
        u32 trackListSize = trackList.size();
        u32 beginCount = 0;
        u32 endCount = countTracks;
        do {
            for (u32 i = beginCount; i < endCount; i++)
            {
                u32 trk = tracksToDelete[i];
                if (trackList[trk].isGroup())
                {
                    for (u32 nxt=trk+1; nxt<trackListSize; nxt++)
                    {
                        if (trackList[nxt].getParentGroup() != trk)
                            break;

                        if (tracksToDelete.find(nxt) < 0)
                        {
                            tracksToDelete.push_back(nxt);
                            countTracks++;
                        }
                    }
                }
            }
            beginCount  = endCount;
            endCount    = countTracks;
        } while (beginCount != endCount);
        

        for (u32 i = 0; i < countTracks; i++)
        {
            for (u32 j = i+1; j < countTracks; j++)
            {
                if (tracksToDelete[j] > tracksToDelete[i])
                {
                    u32 swap = tracksToDelete[i];
                    tracksToDelete[i] = tracksToDelete[j];
                    tracksToDelete[j] = swap;
                }
            }
        }
        
        for (u32 i = 0; i < countTracks; i++)
        {
            seqPlayCompTemplate->removeTrack(tracksToDelete[i]);
        }
        if (trackListSize > 0)
            seqPlayCompTemplate->checkIndex( trackListSize-1 );

        m_owner->propagateActorChanges(actSequence, btrue);
    }


    struct TrackChange
    {
        u32 m_index;
        u32 m_newIndex;
        u32 m_mode;


        TrackChange(u32 _index, u32 _newIndex, u32 _mode)
            : m_index(_index)
            , m_newIndex(_newIndex)
            , m_mode(_mode)
        {}

        TrackChange(const TrackChange & _trkChange)
            : m_index(_trkChange.m_index)
            , m_newIndex(_trkChange.m_newIndex)
            , m_mode(_trkChange.m_mode)
        {}

    };

    void SequenceEditorMessages::onUpdateTrackCmd( Blob& _blob )
    {
        Actor* actSequence = getSequence(_blob);    // extract actorID and getActor

        if (!actSequence)
            return;

        SequencePlayerComponent          *seqPlayComp         = m_owner->getCurrentSequenceComponent();
        SequencePlayerComponent_Template *seqPlayCompTemplate = m_owner->getCurrentSequenceComponentTemplate();
        if (!seqPlayComp || !seqPlayCompTemplate)
            return;

        ITF_VECTOR<SequenceTrackInfo_Template> & trackList = seqPlayCompTemplate->getTrackList();
        u32 countTracks = _blob.extractUint32();
        if (countTracks == 0)
            return;

        ITF_VECTOR<TrackChange> trackChangeList;
        bbool somethingDone = bfalse;
        for (u32 i = 0; i < countTracks; i++)
        {
            u32     trackIdx    = _blob.extractUint32();
            u32     newTrackIdx = _blob.extractUint32();
            u32     mode        = _blob.extractUint32();

            seqPlayCompTemplate->checkIndex( trackIdx );
            seqPlayCompTemplate->checkIndex( newTrackIdx );

            if (trackIdx != newTrackIdx)
            {
                TrackChange trkChange(trackIdx, newTrackIdx, mode);
                trackChangeList.push_back(trkChange);
            }
            ITF_ASSERT(trackIdx < trackList.size());
            if (trackIdx < trackList.size())
            {
                i32 prevStart = trackList[trackIdx].getStartFrame();
                trackList[trackIdx].fillFromBlob(&_blob);
                if (prevStart != trackList[trackIdx].getStartFrame())
                {
                    // start has changed -> we need to offset all events
                    somethingDone = btrue;
                    i32 delta = trackList[trackIdx].getStartFrame() - prevStart;
                    u32 evtCount = seqPlayCompTemplate->getEventsCount();
                    for (u32 evtIdx=0; evtIdx<evtCount; evtIdx++)
                    {
                        SequenceEvent_Template *evt = seqPlayCompTemplate->getEventAtIndex(evtIdx);
                        u32 trackLine = evt->getTrackLine();
                        while (trackLine != U32_INVALID)
                        {
                            if (trackLine == trackIdx)
                            {
                                evt->setStartFrame(evt->getStartFrame() + delta);
                                break;
                            }
                            trackLine = trackList[trackLine].getParentGroup();
                        }
                    }
                }
            }
        }

        if (trackChangeList.size())
        {
            ITF_ASSERT(trackChangeList.size() == 1);
            if (trackChangeList.size() != 1)
                return;

            TrackChange trkChange   = trackChangeList[0];
            if (!trackList[trkChange.m_index].isGroup())
            {
                seqPlayCompTemplate->moveTrack(trkChange.m_index,
                                               trkChange.m_newIndex,
                                               trkChange.m_mode,
                                               trackList[trkChange.m_newIndex].getParentGroup());
            } else
            {
                seqPlayCompTemplate->moveGroup(trkChange.m_index,
                    trkChange.m_newIndex,
                    trkChange.m_mode,
                    trackList[trkChange.m_newIndex].getParentGroup());
            }
        }

        m_owner->propagateActorChanges(actSequence, btrue);
    }

    void SequenceEditorMessages::onGroupTracksCmd(   Blob& _blob )
    {
        Actor* actSequence = getSequence(_blob);

        if (!actSequence)
            return;

        SequencePlayerComponent          *seqPlayComp         = m_owner->getCurrentSequenceComponent();
        SequencePlayerComponent_Template *seqPlayCompTemplate = m_owner->getCurrentSequenceComponentTemplate();
        if (!seqPlayComp || !seqPlayCompTemplate)
            return;

        seqPlayComp->unpause();

        u32 trackBegin  = _blob.extractUint32();                
        u32 trackEnd    = _blob.extractUint32();                
        
        seqPlayCompTemplate->checkIndex(trackBegin);
        seqPlayCompTemplate->checkIndex(trackEnd);

        ITF_VECTOR<SequenceTrackInfo_Template> & trackList = seqPlayCompTemplate->getTrackList();
        u32 parentGroup = trackList[trackBegin].getParentGroup();
        
        for (u32 i=trackBegin+1; i<=trackEnd; i++)
        {
            if (trackList[i].getParentGroup() != parentGroup)
                return;
        }

        if(seqPlayCompTemplate->insertTrack(trackBegin))
        {
            trackList[trackBegin].setParentGroup(parentGroup);
            for (u32 i=trackBegin+1; i<=trackEnd+1; i++)
            {
                trackList[i].setParentGroup(trackBegin);
            }
            trackList[trackBegin].setGroup(btrue);
            m_owner->propagateActorChanges(actSequence, btrue);
        }
    }

    void SequenceEditorMessages::onUngroupTracksCmd(   Blob& _blob )
    {
        Actor* actSequence = getSequence(_blob);

        if (!actSequence)
            return;

        SequencePlayerComponent          *seqPlayComp         = m_owner->getCurrentSequenceComponent();
        SequencePlayerComponent_Template *seqPlayCompTemplate = m_owner->getCurrentSequenceComponentTemplate();
        if (!seqPlayComp || !seqPlayCompTemplate)
            return;

        ITF_VECTOR<SequenceTrackInfo_Template> & trackList = seqPlayCompTemplate->getTrackList();

        seqPlayComp->unpause();

        u32 trackIndex = _blob.extractUint32();                
        if (trackIndex >= trackList.size()  || !trackList[trackIndex].isGroup() )
        {
            // ungrouping invalid track
            return;
        }

        u32 parentGroup = trackList[trackIndex].getParentGroup();
        if(seqPlayCompTemplate->removeTrack(trackIndex))
        {
            // add empty line on track list
            for (u32 i=trackIndex; i<trackList.size() && trackList[i].getParentGroup() == trackIndex; i++)
            {
                trackList[i].setParentGroup(parentGroup);
            }
            m_owner->propagateActorChanges(actSequence, btrue);
        }
    }

    void SequenceEditorMessages::onSplitEventCmd(   Blob& _blob )
    {
        Actor* actSequence = getSequence(_blob);    // extract actorID and getActor
        i32 frame = _blob.extractInt32();

        if (!actSequence)
            return;

        SequencePlayerComponent_Template *seqPlayCompTemplate = m_owner->getCurrentSequenceComponentTemplate();
        bbool valide = btrue;
        if (!seqPlayCompTemplate)
            return;

        u32 countEvts = _blob.extractUint32();      
        for (u32 i = 0; i < countEvts; i++)
        {
            u32 uid           =_blob.extractUint32();   // extract ObjectRef of event removing
            SequenceEvent_Template* evt      = seqPlayCompTemplate->getEventTemplateWithUID(uid);
            SequenceEvent_Template* newEvt   = NULL;
            if (evt)
            {
                newEvt = evt->split(frame);
                if (newEvt)
                {
                    seqPlayCompTemplate->addEvent(newEvt);
                }
            }
            valide = newEvt != NULL;
            if (!valide)
                break;
        }

        if(valide)
        {
            m_owner->propagateActorChanges(actSequence, btrue);
        } else
        {
            m_owner->revertUnpropagateActorChanges(actSequence);
        }
    }

    void SequenceEditorMessages::onMergeEventCmd(   Blob& _blob )
    {
        Actor* actSequence = getSequence(_blob);    // extract actorID and getActor

        if (!actSequence)
            return;

        SequencePlayerComponent_Template *seqPlayCompTemplate = m_owner->getCurrentSequenceComponentTemplate();
        if (!seqPlayCompTemplate)
            return;

        u32 countEvts = _blob.extractUint32();
        SafeArray<SequenceEvent_Template*> evtList;
        evtList.reserve(countEvts);

        for (u32 i = 0; i < countEvts; i++)
        {
            u32 uid           =_blob.extractUint32();   // extract ObjectRef of event removing
            SequenceEvent_Template* evt      = seqPlayCompTemplate->getEventTemplateWithUID(uid);
            if (evt)
            {
                evtList.push_back(evt);
            }
        }
        
        countEvts = evtList.size();
        if (countEvts < 2)
            return;

        SequenceEvent_Template::EvtType type = evtList[0]->getType();
        for (u32 i=1; i<countEvts; i++)
        {
            if (evtList[i]->getType() != type)
            {
                // trying to merge heterogeneous types !
                return;
            }
        }

        // doing naive sort because no much elements
        for (u32 i=0; i<countEvts; i++)
        {
            for (u32 j=i+1; j<countEvts; j++)
            {
                if (evtList[j]->getStartFrame() < evtList[i]->getStartFrame())
                {
                    SequenceEvent_Template* evt = evtList[i];
                    evtList[i] = evtList[j];
                    evtList[j] = evt;
                }
            }
        }

        // last verification component are contiguous
        for (u32 i=0; i<countEvts-1; i++)
        {
            if (evtList[i]->getStopFrame() > evtList[i+1]->getStartFrame())
            {
                // trajectory collision -> quit
                return;
            }
        }

        SequenceEvent_Template* mergedEvent = evtList[0];
        u32 i;
        for (i=1; i<countEvts; i++)
        {
            if (!mergedEvent->concat(evtList[i]))
                break;

            seqPlayCompTemplate->removeEvent(evtList[i]->getUID());
        }
        
        if (i == countEvts)
        {
            m_owner->propagateActorChanges(actSequence, btrue);
        } else
        {
            m_owner->revertUnpropagateActorChanges(actSequence);
        }
    }



    void SequenceEditorMessages::onPlaySequenceCmd(Blob& _blob)
    {
        Actor* actSequence = getSequence(_blob);
        i32 currentFrame =_blob.extractInt32();
        if (!actSequence)
            return;

        SequencePlayerComponent *seqPlayComp = m_owner->getCurrentSequenceComponent();
        if (!seqPlayComp)
            return;

        if (seqPlayComp->getState() != SequencePlayerComponent::State_Playing)
        {
            i32 frame = currentFrame;
            if (seqPlayComp->getState() == SequencePlayerComponent::State_Stopped)
                frame = 0;

            seqPlayComp->start();
            seqPlayComp->resetCamera();
            seqPlayComp->setCurrentFrame(frame, bfalse);
            seqPlayComp->setFrameStartSaved(seqPlayComp->getCurrentFrame());
            seqPlayComp->unpause();
            seqPlayComp->allowCameraControl(btrue);
        }
        else
        {
            i32 frame = seqPlayComp->getFrameStartSaved();

            seqPlayComp->setCurrentFrame(frame, true);
            seqPlayComp->pause();
            seqPlayComp->allowCameraControl(false);
            m_owner->sendCurrentFrame(actSequence->getRef(), frame);
        }
        // on set current frame, force camera !
        m_owner->updateGlobalSequenceData(btrue);
    }

    void SequenceEditorMessages::onAddNewSequenceCmd(Blob& _blob)
    {
        Plugin* plugin = PLUGINGATEWAY->getPluginByName(ActorEditor::getPluginName());
        if(plugin) 
        {
            _blob.extractString(m_sequencePlayerTemplateLUA);                
            ((ActorEditor*)plugin)->enterCreateActorState(m_sequencePlayerTemplateLUA);
        }
    }
    
    void SequenceEditorMessages::onSetCurrentFrameCmd(Blob& _blob)
    {
        Actor* actSequence = getSequence(_blob);
        i32 currentFrame =_blob.extractInt32();
        m_owner->forceCurrentFrame( actSequence, currentFrame );
    }

   void SequenceEditorMessages::onGetSessionInfoCmd(Blob& _blob )
    {
        Blob b(BLOB_EXTRACT_AND_WRITE_ENABLED);
        String path = DATA_FOLDER;
        path.replace('/', '\\');
        path.truncate(path.getLen()-1);
        b.pushString(SequenceEditorPlugin::getPluginName());  // plugin ID
        b.pushString(getSessionInfoCmdName()); 
        b.pushString(path);        
        PLUGINGATEWAY->sendPacket(b, SequenceEditorPlugin::getNetPeer(), SequenceEditorPlugin::getSenderID());
    }

    void SequenceEditorMessages::onGetInfoListAnimationsByFriendlyCmd(   Blob& _blob, bbool _subAnims )
    {
        String friendlyName;
        _blob.extractString(friendlyName);

        Blob b(BLOB_EXTRACT_AND_WRITE_ENABLED);
        b.pushString(SequenceEditorPlugin::getPluginName());  // plugin ID

        SequencePlayerComponent * spc = m_owner->getCurrentSequenceComponent();
        if (!spc)
            return;

        String8 friendlyName8(friendlyName.cStr());
        friendlyName8.toLower();
        ObjectPath objPath(friendlyName8.cStr());
        
        Actor *actor = spc->getActorFromObjectPath(objPath, friendlyName8.cStr());
        if (_subAnims)
        {
            m_owner->getActorEditor().sendInfoListSubAnimsActor(b, actor, friendlyName8);
        } else
        {
            m_owner->getActorEditor().sendInfoListAnimationsActor(b, actor, friendlyName8);
        }
    }    

    void SequenceEditorMessages::onGetInfoListSoundByFriendlyCmd(   Blob& _blob )
    {
        String friendlyName;
        _blob.extractString(friendlyName);

        Blob b(BLOB_EXTRACT_AND_WRITE_ENABLED);
        b.pushString(SequenceEditorPlugin::getPluginName());  // plugin ID

        SequencePlayerComponent * spc = m_owner->getCurrentSequenceComponent();
        if (!spc)
            return;

        String8 friendlyName8(friendlyName.cStr());
        friendlyName8.toLower();
        ObjectPath objPath(friendlyName8.cStr());

        Actor *actor = spc->getActorFromObjectPath(objPath, friendlyName8.cStr());
        m_owner->getActorEditor().sendInfoListSoundActor(b, actor, friendlyName8);
    }

    void SequenceEditorMessages::onGetListSequenceCmd       (   Blob& _blob )
    {
        sendSequenceList();
    }

    void SequenceEditorMessages::sendSequenceList ( bbool _forUpdateOnly )
    {
        Blob b(BLOB_EXTRACT_AND_WRITE_ENABLED);
        b.pushString(SequenceEditorPlugin::getPluginName());  // plugin ID
        b.pushString(getListSequenceCmdName()); 


        const World* pWorld = static_cast<const World*>(GETOBJECT(WORLD_MANAGER->getCurrentWorld()));
        ITF_VECTOR<SequenceEditorPlugin::SeqState> & seqStateList = m_owner->getSequencesState();
        if(pWorld)
        {
            if (_forUpdateOnly)
            {
                bbool needSend = bfalse;
                for (u32 indexScene = 0; !needSend && indexScene < pWorld->getSceneCount(); indexScene++)
                {
                    const Scene* pScene = pWorld->getSceneAt(indexScene);
                    if(!pScene) 
                        continue;
                    const PickableList& listActor =  pScene->getActors(); 
                    u32 uListSize = listActor.size();
                    for (u32 index = 0; index < uListSize; ++index)
                    {
                        Actor* pAct = static_cast<Actor*>(listActor[index]);
                        if (pAct)
                        {
                            Path actPath = pAct->getLua();
                            SequencePlayerComponent *seqPlayComp = pAct->GetComponent<SequencePlayerComponent>();
                            if (seqPlayComp && seqPlayComp->getTemplate())
                            {
                                bbool actorAlreadyPresent = bfalse;
                                for (u32 seqIdx=0; seqIdx < seqStateList.size(); seqIdx++)
                                {
                                    if (seqStateList[seqIdx].m_path == actPath.getString())
                                    {
                                        actorAlreadyPresent = btrue;
                                        break;
                                    }
                                }
                                if (!actorAlreadyPresent)
                                {
                                    needSend = btrue;
                                    break;
                                }
                            }
                        }
                    }
                }
                if (!needSend)
                    return;
            }


            Actor *act = m_owner->getCurrentSequenceActor();
            SafeArray<Actor*> actors;        
            for (u32 indexScene = 0; indexScene < pWorld->getSceneCount(); indexScene++)
            {
            
                const Scene* pScene = pWorld->getSceneAt(indexScene);
                if(!pScene) 
                    continue;
                const PickableList& listActor =  pScene->getActors(); 

                u32 uListSize = listActor.size();
                for (u32 index = 0; index < uListSize; ++index)
                {
                    Actor* pAct = static_cast<Actor*>(listActor[index]);
                    if (pAct)
                    {
                        SequencePlayerComponent *seqPlayComp = pAct->GetComponent<SequencePlayerComponent>();
                        if (seqPlayComp && seqPlayComp->getTemplate())
                        {
                            bbool actorAlreadyPresent = bfalse;
                            Path actPath = pAct->getLua();
                            for (u32 actIdx=0; actIdx < actors.size(); actIdx++)
                            {
                                if (actors[actIdx]->getLua() == actPath)
                                {
                                    actorAlreadyPresent = btrue;
                                    break;
                                }
                            }

                            if (!actorAlreadyPresent)
                            {
                                actors.push_back(pAct);
                                m_owner->setCurrentSequenceActor(pAct, btrue);
                            }
                        }
                    }
                }
            }
        
            u32 countActor = actors.size();
            b.pushInt32(countActor);

            for (u32 index = 0; index < countActor; ++index)
            {
                Actor* pAct = actors[index];
                if (pAct)
                {
                    SequencePlayerComponent          *seqPlayComp = pAct->GetComponent<SequencePlayerComponent>();
                    // No need for testing done before
                    ITF_ASSERT(seqPlayComp);

                    b.pushUInt32(pAct->getRef().getValue());           // ObjectRef

                    String luaPath = FilePath::getFilename(pAct->getLua().getString());
                    b.pushString(luaPath);                          // send sequence name

                    b.pushString(pAct->getUserFriendly());          // friendly name
                    b.pushInt32(seqPlayComp->getLastEventFrame());  // lastFrame
                    b.pushInt32(seqPlayComp->getState());           // state
                    b.pushInt32(seqPlayComp->getCurrentFrame());    // current frame
                    b.pushInt32(seqPlayComp->getTemplate()->getEventsCount());     // event count
                }
            }
            b.pushUInt32(act ? act->getRef().getValue() : U32_INVALID);

            PLUGINGATEWAY->sendPacket(b, SequenceEditorPlugin::getNetPeer(), SequenceEditorPlugin::getSenderID());
        }
    }

    void SequenceEditorMessages::onGetListActorCmd          (   Blob& _blob )
    {
        Blob b(BLOB_EXTRACT_AND_WRITE_ENABLED);
        b.pushString(SequenceEditorPlugin::getPluginName());  // plugin ID
        b.pushString(getListActorCmdName());

        SafeArray<Actor*> actors;
        const World* pWorld = static_cast<const World*>(GETOBJECT(WORLD_MANAGER->getCurrentWorld()));
        if(pWorld)
        {       
            for (u32 indexScene = 0; indexScene < pWorld->getSceneCount(); indexScene++)
            {
                const Scene* pScene = pWorld->getSceneAt(indexScene);
                if(!pScene)
                    continue;
                const PickableList& listActor =  pScene->getActors(); 

                u32 uListSize = listActor.size();

                for ( u32 i = 0; i < uListSize; ++i )
                {
                    Actor* pAct = static_cast<Actor*>(listActor[i]);
                    if (pAct && pAct->isOnScreen())
                    {
                        actors.push_back(pAct);
                    }
                }
            }
        }

        u32 count = actors.size();
        b.pushInt32(count);

        for ( u32 i = 0; i < count; ++i )
        {
            b.pushUInt32(actors[i]->getRef().getValue());
            b.pushString(actors[i]->getUserFriendly());
        }


        PLUGINGATEWAY->sendPacket(b, SequenceEditorPlugin::getNetPeer(), SequenceEditorPlugin::getSenderID());
    }

    void SequenceEditorMessages::onGetListPlayersCmd(   Blob& _blob )
    {
        sendListPlayers();
    }

    void SequenceEditorMessages::sendListPlayers(  )
    {
        Blob b(BLOB_EXTRACT_AND_WRITE_ENABLED);
        b.pushString(SequenceEditorPlugin::getPluginName());  // plugin ID
        b.pushString(getListPlayersCmdName());

        int nbGameMode = (int)GAMEMANAGER->getNbGameModeControllers();
        b.pushInt32(nbGameMode);
        
        m_listPlayersSendOk = bfalse; 

        for (int mode=0; mode<nbGameMode; mode++)
        {
            int nbPayers = GAMEMANAGER->getMaxPlayerCount();
            b.pushInt32(nbPayers);
            for (int pl=0; pl<nbPayers; pl++)
            {
                Player* player = GAMEMANAGER->getPlayer(pl);
                if (player)
                {
                    Actor * actor  = (Actor *)player->getActorForMode((GameMode)mode).getObject();
                    if (actor )
                    {
                        b.pushString(actor->getUserFriendly().cStr());
                        m_listPlayersSendOk = btrue;
                        continue;
                    }
                }
                b.pushString("unset (actor not loaded)");
            }
        }
        PLUGINGATEWAY->sendPacket(b, SequenceEditorPlugin::getNetPeer(), SequenceEditorPlugin::getSenderID());
    }

    void SequenceEditorMessages::onGetSequenceCmd           (   Blob& _blob )
    {
        Actor* actSequence = getSequence(_blob);    // extract actorID and getActor
        if(actSequence)
        {
            sendSequence(actSequence);
        }
        else
        {
            sendSequenceList();
        }
    }

    
    void SequenceEditorMessages::sendSequence ( Actor* _actSequence )
    {
        m_owner->setCurrentSequenceActor(_actSequence, bfalse);
        Actor * actSequence = m_owner->getCurrentSequenceActor();
        
        if (!actSequence)
            return;

        Blob b(BLOB_EXTRACT_AND_WRITE_ENABLED);
        b.pushString(SequenceEditorPlugin::getPluginName());  // plugin ID
        b.pushString(sequenceSelectedCmdName());
        b.pushUInt32(actSequence->getRef().getValue());

        SequencePlayerComponent          *seqPlayComp         = m_owner->getCurrentSequenceComponent();
        SequencePlayerComponent_Template *seqPlayCompTemplate = m_owner->getCurrentSequenceComponentTemplate();
        if (seqPlayComp && seqPlayCompTemplate)
        {
            seqPlayComp->setEditionMode(btrue);
            seqPlayComp->allActorsResourceLoad();
            
            m_owner->checkTracks(RECOMPUTE_ALL);
            seqPlayCompTemplate->serializeToBlob(&b, actSequence->getLua().getString());
        }
        PLUGINGATEWAY->sendPacket(b, SequenceEditorPlugin::getNetPeer(), SequenceEditorPlugin::getSenderID());

        // send also camera mode
        sendCameraModeState();
        // and player list
        sendActivePlayerList();
    }

    void SequenceEditorMessages::sendSequenceTrackList( Actor* _actSequence )
    {
        m_owner->setCurrentSequenceActor(_actSequence, bfalse);
        Actor * actSequence = m_owner->getCurrentSequenceActor();
        if (!actSequence)
            return;

        Blob b(BLOB_EXTRACT_AND_WRITE_ENABLED);
        b.pushString(SequenceEditorPlugin::getPluginName());  // plugin ID
        b.pushString(sequenceTrackListCmdName());
        b.pushUInt32(actSequence->getRef().getValue());

        SequencePlayerComponent          *seqPlayComp         = m_owner->getCurrentSequenceComponent();
        SequencePlayerComponent_Template *seqPlayCompTemplate = m_owner->getCurrentSequenceComponentTemplate();
        if (seqPlayComp && seqPlayCompTemplate)
        {
            m_owner->checkTracks(RECOMPUTE_ALL);
            seqPlayCompTemplate->serializeTrackListToBlob(&b);
        }
        PLUGINGATEWAY->sendPacket(b, SequenceEditorPlugin::getNetPeer(), SequenceEditorPlugin::getSenderID());
    }

    void SequenceEditorMessages::onSaveOneSequenceCmd( Blob& _blob )
    {
        EDITOR->unselectAll();
        Actor* pAct = getSequence(_blob);
        if (pAct)
        {
            SequenceEditorPlugin::SeqState* _seqState = NULL;
            m_owner->getSequenceState(pAct->getRef(), _seqState);
            Actor_Template *actTemplate = m_owner->getCurrentSequenceActorTemplate();

            if (_seqState && _seqState->m_modified && actTemplate)
            {
                actTemplate->saveInFile();
                _seqState->m_modified = bfalse;
            }
        }
		Blob b(BLOB_EXTRACT_AND_WRITE_ENABLED);
		b.pushString(SequenceEditorPlugin::getPluginName());  // plugin ID
		b.pushString(sendSaveOneSequenceCmdName());
        PLUGINGATEWAY->sendPacket(b, SequenceEditorPlugin::getNetPeer(), SequenceEditorPlugin::getSenderID());
    }

    void SequenceEditorMessages::onSaveAllSequencesCmd( Blob& _blob )
    {
        saveSequences();

        Blob b(BLOB_EXTRACT_AND_WRITE_ENABLED);
        b.pushString(SequenceEditorPlugin::getPluginName());  // plugin ID
        b.pushString(sendSaveAllSequencesCmdName());
        PLUGINGATEWAY->sendPacket(b, SequenceEditorPlugin::getNetPeer(), SequenceEditorPlugin::getSenderID());
    }

    void SequenceEditorMessages::onSaveWorldCmd( Blob& _blob )
    {
        PLUGINGATEWAY->onSceneSave();
        EDITOR->saveCurrentWorld(bfalse, bfalse);
        saveSequences();

        Blob b(BLOB_EXTRACT_AND_WRITE_ENABLED);
        b.pushString(SequenceEditorPlugin::getPluginName());  // plugin ID
        b.pushString(sendSaveWorldCmdName());
        PLUGINGATEWAY->sendPacket(b, SequenceEditorPlugin::getNetPeer(), SequenceEditorPlugin::getSenderID());
    }

    void SequenceEditorMessages::saveSequences()
    {
        ITF_VECTOR<SequenceEditorPlugin::SeqState> & seqList = m_owner->getSequencesState();
        ITF_VECTOR<SequenceEditorPlugin::SeqState>::iterator seqListIt = seqList.begin();

        for (; seqListIt != seqList.end(); seqListIt++)
        {
            if ((*seqListIt).m_actorTemplate && (*seqListIt).m_modified)
            {
                (*seqListIt).m_actorTemplate->saveInFile();
                (*seqListIt).m_modified = bfalse;
            }
        }
    }

    void SequenceEditorMessages::onGetGameplayEventsListCmdName( Blob& _blob )
    {
        const ObjectFactory::ObjectsNamesContainer& names = ACTORSMANAGER->getEventFactory()->GetObjectsNames();
        Blob b(BLOB_EXTRACT_AND_WRITE_ENABLED);
        b.pushString(SequenceEditorPlugin::getPluginName());  // plugin ID
        b.pushString(getGameplayEventsListCmdName());
        b.pushUInt32(names.size());
        for (ObjectFactory::ObjectsNamesContainer::const_iterator it = names.begin(); it != names.end(); ++it)
        {
            b.pushString(it->getDebugString());
        }
        PLUGINGATEWAY->sendPacket(b, SequenceEditorPlugin::getNetPeer(), SequenceEditorPlugin::getSenderID());

    }

    
    void SequenceEditorMessages::sendRecordModeState()
    {
        Blob b(BLOB_EXTRACT_AND_WRITE_ENABLED);
        b.pushString(SequenceEditorPlugin::getPluginName());  // plugin ID
        b.pushString(getRecordModeStateCmdName());
        b.pushBool(m_owner->getAutoKeyMode());
        PLUGINGATEWAY->sendPacket(b, SequenceEditorPlugin::getNetPeer(), SequenceEditorPlugin::getSenderID());
    }


    void SequenceEditorMessages::onSetRecordModeStateCmd ( Blob& _blob )
    {
        bbool state = _blob.extractBool();
        m_owner->setAutoKeyMode(state); 
        m_owner->processSelectedEvent();

        // if autokey is not allowed, pickable default edition is restored
        if (!state)
            EDITOR->allowStoreAllPickableInitData();

        sendRecordModeState();
    }

    void SequenceEditorMessages::sendDefaultSplineMode()
    {
        Blob b(BLOB_EXTRACT_AND_WRITE_ENABLED);
        b.pushString(SequenceEditorPlugin::getPluginName());  // plugin ID
        b.pushString(getDefaultSplineModeCmdName());
        b.pushInt32((i32)m_owner->getDefaultSplineMode());
        PLUGINGATEWAY->sendPacket(b, SequenceEditorPlugin::getNetPeer(), SequenceEditorPlugin::getSenderID());
    }


    void SequenceEditorMessages::onSetDefaultSplineModeCmd ( Blob& _blob )
    {
        i32 splineMode = _blob.extractInt32();
        m_owner->setDefaultSplineMode((Spline::Interpolation)splineMode); 
        
        sendDefaultSplineMode();
    }


    void SequenceEditorMessages::sendBezierLinked()
    {
        Blob b(BLOB_EXTRACT_AND_WRITE_ENABLED);
        b.pushString(SequenceEditorPlugin::getPluginName());  // plugin ID
        b.pushString(getBezierLinkedCmdName());
        b.pushBool(m_owner->isBezierLinked());
        PLUGINGATEWAY->sendPacket(b, SequenceEditorPlugin::getNetPeer(), SequenceEditorPlugin::getSenderID());
    }


    void SequenceEditorMessages::onSetBezierLinkedCmd ( Blob& _blob )
    {
        m_owner->setBezierLinked(_blob.extractBool()); 

        sendBezierLinked();
    }


    void SequenceEditorMessages::onSetInactivateChannels( Blob& _blob )
    {
        ITF_VECTOR<StringID> & inativateChannels =  m_owner->getExternalInactiveChannels();
        inativateChannels.clear();
        
        i32 count = _blob.extractInt32();
        inativateChannels.resize(count);
        String8 channelName;
        for (i32 i=0; i<count; ++i)
        {
            _blob.extractString8(channelName);
            inativateChannels[i] = channelName.cStr();
        }

        m_owner->setExternInactiveChannels(btrue);
        SequencePlayerComponent *seqPlayComp = m_owner->getCurrentSequenceComponent();
        if (seqPlayComp)
        {
            m_owner->forceCurrentFrame(seqPlayComp->GetActor(), seqPlayComp->getCurrentFrame());
        }
    }


    void SequenceEditorMessages::sendCameraModeState()
    {
        Blob b(BLOB_EXTRACT_AND_WRITE_ENABLED);
        b.pushString(SequenceEditorPlugin::getPluginName());  // plugin ID
        b.pushString(getCameraModeStateCmdName());
        b.pushBool(m_owner->getCameraModeSequence());
        PLUGINGATEWAY->sendPacket(b, SequenceEditorPlugin::getNetPeer(), SequenceEditorPlugin::getSenderID());
    }

    void SequenceEditorMessages::sendActivePlayerList()
    {
        Blob b(BLOB_EXTRACT_AND_WRITE_ENABLED);
        b.pushString(SequenceEditorPlugin::getPluginName());  // plugin ID
        b.pushString(getActicePlayerListCmdName());
        
        u32 nbPlayers = (i32)GAMEMANAGER->getMaxPlayerCount();
        b.pushInt32((i32)nbPlayers);

        for (u32 i=0; i<nbPlayers; i++)
        {
            b.pushBool(GAMEMANAGER->getPlayer(i)->getActive());
        }
        
        PLUGINGATEWAY->sendPacket(b, SequenceEditorPlugin::getNetPeer(), SequenceEditorPlugin::getSenderID());
    }

    void SequenceEditorMessages::onSetActivePlayerListCmd( Blob& _blob )
    {
        u32 nbPlayer = (u32)_blob.extractInt32();
        u32 i;

        Vec3d   actorPos;
        f32     actorAngle = 0.f;
        u32     maxPlayer = GAMEMANAGER->getMaxPlayerCount();
        for (i=0; i<maxPlayer; i++)
        {
            Player * p = GAMEMANAGER->getPlayer(i);
            if (p && p->getActive())
            {
                Actor * act = p->getActor();
                if (!act)
                    continue;

                actorPos    = act->getPos();
                actorAngle  = act->getAngle();
                break;
            }
        }

        if (i == maxPlayer)
            return;

        EventTeleport teleport(actorPos,actorAngle,btrue);

        for (i=0; i<nbPlayer; i++)
        {
            bbool setActive = _blob.extractBool();
            bbool isActive = GAMEMANAGER->getPlayer(i)->getActive();
            if (setActive != isActive)
            {
                if (setActive)
                {
                    GAMEMANAGER->activatePlayer(i);
                    Player * p = GAMEMANAGER->getPlayer(i);
                    Actor * act = p ? p->getActor() : NULL;
                    if (act)
                    {
                        act->onEvent(&teleport);
                    }
                } else
                {
                    GAMEMANAGER->deactivatePlayer(i);
                }
            }
        }
            
        sendActivePlayerList();
    }


    void SequenceEditorMessages::onSetDisablePlayersCmd(Blob& _blob)
    {
        Actor* actSequence = getSequence(_blob);
        bbool disable = _blob.extractBool();
        SequencePlayerComponent_Template *seqPlayCompTemplate = m_owner->getCurrentSequenceComponentTemplate();

        if (!actSequence || !seqPlayCompTemplate)
            return;

        seqPlayCompTemplate->setDeactivatedPlayers(disable);
        m_owner->propagateActorChanges(actSequence, btrue, bfalse);
    }

    void SequenceEditorMessages::onSetReactivatePlayersAliveCmd(Blob& _blob)
    {
        Actor* actSequence = getSequence(_blob);
        bbool reactivate = _blob.extractBool();
        SequencePlayerComponent_Template *seqPlayCompTemplate = m_owner->getCurrentSequenceComponentTemplate();

        if (!actSequence || !seqPlayCompTemplate)
            return;

        seqPlayCompTemplate->setReactivateAlive(reactivate);
        m_owner->propagateActorChanges(actSequence, btrue, bfalse);
    }

    void SequenceEditorMessages::onSetReactivatePlayersCmd(Blob& _blob)
    {
        Actor* actSequence = getSequence(_blob);
        bbool reactivate = _blob.extractBool();
        SequencePlayerComponent_Template *seqPlayCompTemplate = m_owner->getCurrentSequenceComponentTemplate();

        if (!actSequence || !seqPlayCompTemplate)
            return;

        seqPlayCompTemplate->setReactivatePlayers(reactivate);
        m_owner->propagateActorChanges(actSequence, btrue, bfalse);
    }


    void SequenceEditorMessages::onSetSaveInitialViewCmd(Blob& _blob)
    {
        Actor* actSequence = getSequence(_blob);
        bbool save = _blob.extractBool();
        SequencePlayerComponent_Template *seqPlayCompTemplate = m_owner->getCurrentSequenceComponentTemplate();

        if (!actSequence || !seqPlayCompTemplate)
            return;

        seqPlayCompTemplate->setSaveInitialView(save);
        m_owner->propagateActorChanges(actSequence, btrue, bfalse);
    }

    void SequenceEditorMessages::onSetCameraModeStateCmd ( Blob& _blob )
    {
        bbool state = _blob.extractBool();
        m_owner->setCameraModeSequence(state); 

        sendCameraModeState();
    }

    void SequenceEditorMessages::setCameraOnActor( Blob& _blob )
    {

        String friendlyName;
        _blob.extractString(friendlyName);

        Blob b(BLOB_EXTRACT_AND_WRITE_ENABLED);
        b.pushString(SequenceEditorPlugin::getPluginName());  // plugin ID

        Actor * actSequence = m_owner->getCurrentSequenceActor();
        if (!actSequence)
            return;

        SequencePlayerComponent* spc = actSequence->GetComponent<SequencePlayerComponent>();
        if (!spc)
            return;

        Scene* sc = actSequence->getScene();
        if(!sc)
            return;

        String8 friendlyName8(friendlyName.cStr());
        friendlyName8.toLower();
        ObjectPath objPath(friendlyName8.cStr());

        const Actor *actor = spc->getActorFromObjectPath(objPath, friendlyName8.cStr());
        if (actor)                
        {
            spc->pause();

            CAMERA->setMode(Camera::Camera_Editor);
            EDITOR->unselectAll();

            if (EDITOR)
            {
                ObjectRefList refInZone(1);
                refInZone.push_back(actor->getRef());
                EDITOR->selectObjects(refInZone, btrue);

                Vec3d newCameraPos = actor->getPos();
                newCameraPos.m_z += 8;
                CAMERA->cancelBiasMode();
                CAMERA->forcePosition(newCameraPos);
            }
        }
        m_owner->processSelectedEvent();
    }



} // namespace ITF


#endif //!ITF_FINAL
