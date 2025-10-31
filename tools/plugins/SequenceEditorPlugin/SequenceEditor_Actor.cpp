// GLOBAL ENGINE CONFIG FILE. Must be the first included file
#ifndef _ENGINECOMMON_H_
    #include "core/enginecommon.h"
#endif // _ENGINECOMMON_H_

#if !defined(ITF_FINAL) && defined(ITF_DEBUG_STRINGID)

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_SEQUENCEEDITORACTOR_H_
#include "SequenceEditor_Actor.h"
#endif //_ITF_SEQUENCEEDITORACTOR_H_

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_PICKABLE_H_
#include "engine/picking/Pickable.h"
#endif //_ITF_PICKABLE_H_

#ifndef _ITF_BLOB_H_
#include "core/Blob.h"
#endif //_ITF_BLOB_H_

#ifndef _ITF_ID_SERVER_H_
#include "core/IDServer.h"
#endif //_ITF_ID_SERVER_H_

#ifndef _ITF_SEQUENCEEDITORPLUGIN_H_
#include "SequenceEditorPlugin.h"
#endif //_ITF_SEQUENCEEDITORPLUGIN_H_

#ifndef _ITF_PLUGINGATEWAY_H_
#include "PluginGateway/PluginGateway.h"
#endif //_ITF_PLUGINGATEWAY_H_

#ifndef _ITF_SEQUENCEPLAYERCOMPONENT_H_
#include "engine/sequenceplayer/SequencePlayerComponent.h"
#endif //_ITF_SEQUENCEPLAYERCOMPONENT_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_ANIMATION_H_
#include "engine/animation/Animation.h"
#endif //_ITF_ANIMATION_H_

#ifndef _ITF_ANIMATEDCOMPONENT_H_
#include "engine/actors/components/animatedcomponent.h"
#endif //_ITF_ANIMATEDCOMPONENT_H_

#ifndef _ITF_SOUNDCOMPONENT_H_
#include "engine/actors/components/SoundComponent.h"
#endif //_ITF_SOUNDCOMPONENT_H_

#ifndef _ITF_ANIMATIONTRACK_H_
#include "engine/animation/AnimationTrack.h"
#endif //_ITF_ANIMATIONTRACK_H_

#ifndef _ITF_SUBANIMLIGHTSET_H_
#include    "engine/animation/SubAnimSet.h"
#endif //_ITF_SUBANIMLIGHTSET_H_

#ifndef _ITF_EDITOR_H_
#include "editor/editor.h"
#endif //_ITF_EDITOR_H_

#ifndef _ITF_XMLALL_H_
#include "engine/XML/XML_All.h"
#endif //_ITF_XMLALL_H_

#ifndef _ITF_ANIMATIONTRACK_H_
#include "engine/animation/AnimationTrack.h"
#endif //_ITF_ANIMATIONTRACK_H_

#ifndef _ITF_MATHTOOLS_H_
#include "core/math/MathTools.h"
#endif //_ITF_MATHTOOLS_H_


#ifndef _ITF_EDITORDRAW_H_
#include "editor/EditorDraw.h"
#endif //_ITF_EDITORDRAW_H_

#ifndef _ITF_ACTOREDITOR_H_
#include "tools/plugins/ActorEditor/ActorEditor.h"
#endif //_ITF_ACTOREDITOR_H_

#ifndef _ITF_INPUTADAPTER_H_
#include "engine/AdaptersInterfaces/InputAdapter.h"
#endif //_ITF_INPUTADAPTER_H_

#ifndef _ITF_PLAYANIM_EVT_H_
#include "engine/sequenceplayer/seq_events/PlayAnim_evt.h"
#endif //_ITF_PLAYANIM_EVT_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_SOUND_H_
#include "engine/sound/Sound.h"
#endif //_ITF_SOUND_H_

namespace ITF
{
    SequenceEditorActor::SequenceEditorActor ()
        : m_owner(NULL)
    { 
        m_seqPlayCompSelected.invalidate();
        m_framePreviewAnim = 0;
        m_eventPreviewAnim = NULL;
    }

    void SequenceEditorActor::getSelectionStatus (ObjectRef& _selection, bbool& _selectionIsLocked)
    {
        _selection = ITF_INVALID_OBJREF;
        _selectionIsLocked = bfalse;
    }

    // add in blob a list of actor action and animation actor
    void SequenceEditorActor::getInfoListAnimationActor(String _cmd, Actor* _pActor, Blob& _blob)
    {
        _blob.pushString(_cmd);

        GameMode playerMode = GAMEMODE_UNKNOWN;
        Player* player = GAMEMANAGER->isPlayerActor(_pActor->getRef()) ? GAMEMANAGER->getPlayerAndModeFromActor( _pActor->getRef(), playerMode ) : NULL;
        int indexPlayer = player ? player->getIndex()  + 100 * (int)playerMode : -1;

        // if the actor selected is a player then send "Player0" for the player1
        if(indexPlayer != -1)
        {
            _blob.pushString(_pActor->getUserFriendly());     // FriendlyName:  "player"
        }
        else
        {
            Actor*                      actSequence = m_owner->getCurrentSequenceActor();
            SequencePlayerComponent *   seqPlayComp = m_owner->getCurrentSequenceComponent();
            bbool                       nameOk = bfalse;
            ObjectPath                  objPath;

            if (actSequence && seqPlayComp)
            {
                ObjectRefList  * actorList = seqPlayComp->getSpawnActors();
                u32 spawnActorCount = actorList->size();
                for (u32 i = 0; i < spawnActorCount; i++)
                {
                    if ((*actorList)[i] == _pActor->getRef())
                    {
                        objPath.fromString(_pActor->getUserFriendly().cStr());
                        nameOk = true;
                        break;
                    }
                }
            }
            if (nameOk || ( actSequence && SceneObjectPathUtils::getRelativePathFromObject(actSequence, _pActor, objPath)))
            {
                ITF_STDSTRING objStr; 
                objPath.toString( objStr );
                _blob.pushString(objStr.c_str());       // Friendly name
            }
            else
                _blob.pushString("");
        }
    }
    
    void SequenceEditorActor::onSequencePlayerActorPicked(Actor* _pActor, Blob& _blob)
    {
        m_owner->setCurrentSequenceActor(_pActor, bfalse);

        Actor * actSequence =  m_owner->getCurrentSequenceActor();
        if (!actSequence)
            return;

        _blob.pushString(SequenceEditorMessages::sequenceSelectedCmdName());
        _blob.pushUInt32(actSequence->getRef().getValue());
        
        SequencePlayerComponent_Template *seq = m_owner->getCurrentSequenceComponentTemplate();
        if (seq)
            seq->serializeToBlob(&_blob, actSequence->getLua().getString());

        m_seqPlayCompSelected = actSequence->getRef();
        m_owner->getTrajectoryEditor().setShowAllTraj(btrue);
        m_owner->getTrajectoryEditor().clearSelection();
    }

    bbool SequenceEditorActor::onActorPicked(ObjectRef _actorRef)
    {
        m_owner->getTrajectoryEditor().setShowAllTraj(bfalse);

        Pickable* pp = (Pickable*)GETOBJECT(_actorRef);
        if (!pp)
        {
            if(INPUT_ADAPTER->isMousePressed(InputAdapter::MB_Left))
            {
                m_seqPlayCompSelected = ITF_INVALID_OBJREF;
                

                ITF::Blob blob(ITF::BLOB_EXTRACT_AND_WRITE_ENABLED);
                blob.pushString(SequenceEditorPlugin::getPluginName());       
                blob.pushString(SequenceEditorMessages::actorSelectedCmdName());
                blob.pushString("");       // Friendly name
                PLUGINGATEWAY->sendPacket(blob, SequenceEditorPlugin::getNetPeer(), SequenceEditorPlugin::getSenderID());

                return bfalse;
            }
            return true;
        }

        ITF_ASSERT(pp->getObjectType() == BaseObject::eActor);
        Actor* currentActor = static_cast<Actor*>(pp);


        SequencePlayerComponent *seqPlayComp = currentActor->GetComponent<SequencePlayerComponent>();
        if (seqPlayComp)
        {
            // send the actor to the external tool
            if (m_seqPlayCompSelected != currentActor->getRef())
            {
                ITF::Blob blob(ITF::BLOB_EXTRACT_AND_WRITE_ENABLED);
                blob.pushString(SequenceEditorPlugin::getPluginName());       

                onSequencePlayerActorPicked(currentActor, blob);
                // be sure to have Edition mode showing the actor
                // also cancel other creation (no multiple creation for new sequence
                EditorDraw::PickableDetailMode  pickMode = EditorDraw::getPickableDetailMode();
                if (pickMode < EditorDraw::PickableDetails_ShowAnchors)
                {
                    EditorDraw::setPickableDetailModes(EditorDraw::PickableDetails_ShowAnchors);
                }
                Plugin* plugin = PLUGINGATEWAY->getPluginByName(ActorEditor::getPluginName());
                if(plugin) 
                {
                    ((ActorEditor*)plugin)->resetState();
                }
                PLUGINGATEWAY->sendPacket(blob, SequenceEditorPlugin::getNetPeer(), SequenceEditorPlugin::getSenderID());

            } else
            {
                ITF::Blob blob(ITF::BLOB_EXTRACT_AND_WRITE_ENABLED);
                // also select the actor
                blob.pushString(SequenceEditorPlugin::getPluginName());       
                blob.pushString(SequenceEditorMessages::actorSelectedCmdName());
                blob.pushString(SequencePlayerComponent::getMyselfKey());       // Friendly name
                PLUGINGATEWAY->sendPacket(blob, SequenceEditorPlugin::getNetPeer(), SequenceEditorPlugin::getSenderID());
            }
        }
        else 
        {
            // actor's
            // send the actor to the external tool
            ITF::Blob blob(ITF::BLOB_EXTRACT_AND_WRITE_ENABLED);
            blob.pushString(SequenceEditorPlugin::getPluginName());       
            getInfoListAnimationActor(SequenceEditorMessages::actorSelectedCmdName(), currentActor, blob);
            PLUGINGATEWAY->sendPacket(blob, SequenceEditorPlugin::getNetPeer(), SequenceEditorPlugin::getSenderID());
        }


        return btrue;
    }

    bbool SequenceEditorActor::onActorSubEdit(ObjectRef _actorRef)
    {
        onActorPicked(_actorRef);
        return bfalse;
    }

    void SequenceEditorActor::onPickableRenamed( Pickable* _pickable, const String8& _name )
    {
        // renamed actor is selected ...
        if(_pickable->getObjectType() == BaseObject::eActor)
            onActorPicked(_pickable->getRef());
    }

    void SequenceEditorActor::sendInfoListAnimationsActor(Blob& _blob, Actor* actor, const String8 _friendlyName)
    {
        _blob.pushString(SequenceEditorMessages::getInfoListAnimationsCmdName());
        _blob.pushString(_friendlyName);
        if(!actor || !actor->updatePhysicalReady() || actor->hasDataError()) 
        {
            _blob.pushInt32(-1); // actor no ready
        }
        else
        {
            AnimLightComponent  *animComp = actor->GetComponent<AnimLightComponent>();
            if(!actor->isPhysicalReady() || !animComp)
            {
                _blob.pushInt32(-1); // actor no ready
            } else
            {
                const TrackSet* trackSet = (TrackSet*)animComp->getSubAnimSet();
                // send animation
                const NamesObjectRefMap& map  = trackSet->m_nameToTrack;

                _blob.pushInt32(map.size());
                NamesObjectRefMap::Iterator it(map);
                for (;  !it.isEnd(); it.increment())
                {
                    ITF_ASSERT(!it.dbg().isEmpty());
                    StringID        animName = it.dbg();
                    ResourceID      res;
                    trackSet->getTrackResource(animName, res);
                    _blob.pushString(String(res.getDebugString()));

                    AnimTrack *track = trackSet->getTrack(it.first());
                    if(track)
                    {
                        i32 duration =  (i32)timeToFrame(track->getDuration(), ANIMATION_FRAMERATE);
                        _blob.pushUInt32(duration);
                    }
                    else
                    {
                        _blob.pushUInt32(0);
                    }

                    getListOfMarkerAnimation(_blob, track);
                }
            }
        }

        PLUGINGATEWAY->sendPacket(_blob, SequenceEditorPlugin::getNetPeer(), SequenceEditorPlugin::getSenderID());
    }

    void SequenceEditorActor::sendInfoListSubAnimsActor(Blob& _blob, Actor* actor, const String8 _friendlyName)
    {
        _blob.pushString(SequenceEditorMessages::getInfoListSubAnimsCmdName());
        _blob.pushString(_friendlyName);
        
        if(!actor || !actor->updatePhysicalReady() || actor->hasDataError()) 
        {
            _blob.pushInt32(-1); // actor no ready
        }
        else
        {
            AnimLightComponent  *animComp = actor->GetComponent<AnimLightComponent>();
            if(!animComp)
            {
                _blob.pushInt32(-1);  // actor no ready
            } 
            else
            {
                ResourceID               res;
                String                   resName;
                SubAnimSet*              subAnimSet = animComp->getSubAnimSet();
                const NamesObjectRefMap& map        = subAnimSet->m_nameToTrack;

                u32 count = subAnimSet->getSubAnimCount();
                _blob.pushInt32(count + map.size());
                for (u32 i=0;  i<count; i++)
                {
                    SubAnim * sunAnim = subAnimSet->getSubAnim(i);
                    _blob.pushString(sunAnim->getAnimPath().getString());
                    _blob.pushString(sunAnim->getFriendlyName().getDebugString());
                    _blob.pushInt32((i32)timeToFrame(sunAnim->getDuration(), ANIMATION_FRAMERATE));

                    getListOfMarkerAnimation(_blob, sunAnim->getTrack());
                }

                NamesObjectRefMap::Iterator it(map);
                for (;  !it.isEnd(); it.increment())
                {
                    StringID        animName = it.dbg();
                    subAnimSet->getTrackResource(animName, res);
                    resName                  = String(res.getDebugString());
                    _blob.pushString(resName);
                    _blob.pushString(resName);

                    AnimTrack *track = subAnimSet->getTrack(it.first());
                    if(track)
                        _blob.pushInt32((i32)timeToFrame(track->getDuration(), ANIMATION_FRAMERATE));
                    else
                        _blob.pushInt32(0);

                    getListOfMarkerAnimation(_blob, track);
                }
            }
        }

        PLUGINGATEWAY->sendPacket(_blob, SequenceEditorPlugin::getNetPeer(), SequenceEditorPlugin::getSenderID());
    }


    void SequenceEditorActor::sendInfoListSoundActor(Blob& _blob, Actor* actor, const String8 _friendlyName)
    {
        _blob.pushString(SequenceEditorMessages::getInfoListSoundCmdName());
        _blob.pushString(_friendlyName);

        if(!actor || !actor->updatePhysicalReady() || actor->hasDataError()) 
        {
            _blob.pushInt32(-1); // actor no ready
        }
        else
        {
            SoundComponent  *soundComp = actor->GetComponent<SoundComponent>();
            if(!soundComp)
            {
                _blob.pushInt32(-1);  // actor no ready
            } 
            else
            {
                ITF_VECTOR<SoundDescriptor> & soundList = soundComp->getSoundList();
                // send animation
                u32 count = soundList.size();
                i32 nbSound = 0;

                for (u32 i=0;  i<count; i++)
                {
                    const ITF_VECTOR<ResourceID> & resList = soundList[i].getResourceList();
                    for (u32 j=0; j<resList.size(); j++)
                    {
                        if (resList[j].getResource())
                            nbSound++;
                    }
                }

                _blob.pushInt32(nbSound);
#ifdef ITF_SUPPORT_RAKI
                for (u32 i=0;  i<count; i++)
                {
                    const ITF_VECTOR<ResourceID> & resList = soundList[i].getResourceList();
                    for (u32 j=0; j<resList.size(); j++)
                    {
                        Sound *sound = static_cast<Sound *>(resList[j].getResource());
                        if (sound)
                        {
                            _blob.pushString(sound->getPath().getString());
                            i32 duration =  (i32)timeToFrame(SOUND_ADAPTER->getSoundDuration(sound), ANIMATION_FRAMERATE);
                            _blob.pushInt32(duration);
                        }
                    }
                }
#endif
            }
        }

        PLUGINGATEWAY->sendPacket(_blob, SequenceEditorPlugin::getNetPeer(), SequenceEditorPlugin::getSenderID());
    }



    void SequenceEditorActor::getListOfMarkerAnimation(Blob& _blob, AnimTrack *_pAnimTrack)
    {
        if (_pAnimTrack)
        {
            // find list of frame (position) with animation marker 
            ITF_VECTOR<u32> frameList;

            f32 end = _pAnimTrack->m_endFrame;
            for(u32 i = 0; i < end; i++)            
            {
                AnimEventContainer eventList;
                _pAnimTrack->GetEvents((f32)i, (f32)i+1, eventList,btrue);

                u32 numEvents = eventList.size();
                u32 j = 0;
                while (j < numEvents)
                {
                    AnimMarkerEvent* data = eventList[j];
                    if( data->GetObjectClassCRC() == ITF_GET_STRINGID_CRC(AnimAnimationEvent,1183342651) )
                        frameList.push_back(i);
                    j++;
                }
            }
            
            // send number of marker frame            
            _blob.pushInt32(frameList.size());

            // send the list
            ITF_VECTOR<u32>::iterator it = frameList.begin();
            while (it != frameList.end())
            {
                _blob.pushUInt32(*it + 1);
                ++it;
            }
        }
        else
            _blob.pushInt32(0); // no animations
    }


    void SequenceEditorActor::update()
    {
        // update AnchorPosition
        Actor* actorSeq = (Actor*)GETOBJECT(m_seqPlayCompSelected);
        if(actorSeq && actorSeq->isPhysicalReady())
        {            
            SequencePlayerComponent *seqPlayComp = actorSeq->GetComponent<SequencePlayerComponent>();
            if (seqPlayComp)
            {
                seqPlayComp->updateAnchorPosition();
            }
        }

        //////////////////////////////////////////////////////////////////////////
        Actor* actSequence = m_owner->getCurrentSequenceActor();
        if (m_eventPreviewAnim && actSequence)
        {
            SequencePlayerComponent *seqPlayComp = actSequence->GetComponent<SequencePlayerComponent>();
            if (seqPlayComp)
            {
                Actor* actor = m_eventPreviewAnim->getBindedActor();
                if(actor)
                {
                  
                    m_eventPreviewAnim->resetSubAnimsToPlay();
                    m_eventPreviewAnim->setAnimation(m_framePreviewAnim);

                    f32 duration = m_eventPreviewAnim->getStopFrameAnim();
                    u32 durationFrame = (u32)timeToFrame((f32)duration, ANIMATION_FRAMERATE);
                    if(m_framePreviewAnim > durationFrame)
                        m_framePreviewAnim = 0;
                    

                    m_eventPreviewAnim->resetSubAnimsToPlay();
                    m_eventPreviewAnim->forceCurrentFrame(m_framePreviewAnim);
                    m_eventPreviewAnim->commitSubAnimsToPlay(actor->getRef(), seqPlayComp->getBankState()); 

                    // send the response / validation
                    ITF::Blob blob(ITF::BLOB_EXTRACT_AND_WRITE_ENABLED);
                    blob.pushString(SequenceEditorPlugin::getPluginName());                  // plugin ID
                    blob.pushString(SequenceEditorMessages::setPreviewPlayAnimCmdName());    // cmd ID
                    blob.pushUInt32(m_framePreviewAnim);
                    blob.pushUInt32(durationFrame);
                    PLUGINGATEWAY->sendPacket(blob, SequenceEditorPlugin::getNetPeer(), SequenceEditorPlugin::getSenderID());                   

                    m_framePreviewAnim++;
                }
            }
        }
    }

    void SequenceEditorActor::setPreviewPlayAnim( Blob& _blob )
    {
        // TODO

        //Actor* actSequence = m_owner->getCurrentSequenceActor();
        //bbool   nameOk = bfalse;
        //if (actSequence)
        //{
        //    SequencePlayerComponent *seqPlayComp = actSequence->GetComponent<SequencePlayerComponent>();
        //    if (seqPlayComp)
        //    {
        //        String friendlyName;
        //        _blob.extractString(friendlyName); // friendly name
        //        
        //        i32 type;
        //        type = _blob.extractInt32();        // type
        //        
        //        String anim_str;
        //        _blob.extractString(anim_str);      // anim

        //        if(m_eventPreviewAnim)
        //            m_eventPreviewAnim->onBecomeInactive();

        //        // preview event                                
        //        PlayAnim_evt* eventPreviewAnim = new PlayAnim_evt();
        //        m_framePreviewAnim = 0;

        //        eventPreviewAnim->setOwnerSequence(seqPlayComp);
        //        eventPreviewAnim->setTargetFriendlyName(String8(friendlyName.cStr()));

        //        eventPreviewAnim->setType((PlayAnim_evt::typeAnim)type);
        //        eventPreviewAnim->setAnim(String8(anim_str.cStr()));

        //        eventPreviewAnim->resetSubAnimsToPlay();
        //        eventPreviewAnim->setAnimation(m_framePreviewAnim);

        //        f32 duration = eventPreviewAnim->getStopFrameAnim();
        //        u32 durationFrame = (u32)timeToFrame((f32)duration, TARGETDT); 

        //        eventPreviewAnim->setStartFrame(0);
        //        eventPreviewAnim->setDuration(durationFrame);
        //        eventPreviewAnim->setLoop(btrue);
        //        eventPreviewAnim->setOffset(0);

        //        // switch local -> member class
        //        delete(m_eventPreviewAnim);
        //        m_eventPreviewAnim = eventPreviewAnim;
        //        m_eventPreviewAnim->onBecomeActive();
        //    }
        //}
    }

    void SequenceEditorActor::stopPreviewPlayAnim( Blob& _blob )
    {
        if(m_eventPreviewAnim)
        {
            m_eventPreviewAnim->onBecomeInactive();
            delete(m_eventPreviewAnim);
        }
        m_eventPreviewAnim = NULL;

        // send the response / validation
        ITF::Blob blob(ITF::BLOB_EXTRACT_AND_WRITE_ENABLED);
        blob.pushString(SequenceEditorPlugin::getPluginName());                 // plugin ID
        blob.pushString(SequenceEditorMessages::stopPreviewPlayAnimCmdName());  // cmd ID
        PLUGINGATEWAY->sendPacket(blob, SequenceEditorPlugin::getNetPeer(), SequenceEditorPlugin::getSenderID());                   

    }

} // namespace ITF

#endif //!ITF_FINAL
