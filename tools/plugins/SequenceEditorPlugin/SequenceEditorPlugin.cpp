// GLOBAL ENGINE CONFIG FILE. Must be the first included file
#ifndef _ENGINECOMMON_H_
    #include "core/enginecommon.h"
#endif // _ENGINECOMMON_H_

#if !defined(ITF_FINAL) && defined(ITF_DEBUG_STRINGID)

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

#ifndef _ITF_SEQUENCEEDITORPLUGIN_H_
#include "tools/plugins/SequenceEditorPlugin/SequenceEditorPlugin.h"
#endif //_ITF_SEQUENCEEDITORPLUGIN_H_

#ifndef _ITF_PLUGINGATEWAY_H_
#include "PluginGateway/PluginGateway.h"
#endif //_ITF_PLUGINGATEWAY_H_

#ifndef _ITF_EDITOR_H_
#include "editor/editor.h"
#endif //_ITF_EDITOR_H_

#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_

#ifndef _ITF_SEQUENCEPLAYERCOMPONENT_H_
#include "engine/sequenceplayer/SequencePlayerComponent.h"
#endif //_ITF_SEQUENCEPLAYERCOMPONENT_H_

#ifndef _ITF_ID_SERVER_H_
#include "core/IDserver.h"
#endif //_ITF_ID_SERVER_H_

#ifndef _ITF_TRAJECTORY_H_
#include "engine/animation/trajectory.h"
#endif //_ITF_TRAJECTORY_H_

#ifndef _ITF_PLAYTRAJECTORY_EVT_H_
#include "engine/sequenceplayer/seq_events/PlayTrajectory_evt.h"
#endif //_ITF_PLAYTRAJECTORY_EVT_H_

#ifndef _ITF_PICKINGSHAPE_H_
#include "editor/PickingShape.h"
#endif //_ITF_PICKINGSHAPE_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_CHEATMANAGER_H_
#include "engine/gameplay/CheatManager.h"
#endif //_ITF_CHEATMANAGER_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef SERIALIZEROBJECTBINARY_H
#include "core/serializer/ZSerializerObjectBinary.h"
#endif // SERIALIZEROBJECTBINARY_H

#ifndef _ITF_UNDOREDOMANAGER_H_
#include "editor/UndoRedoManager.h"
#endif //_ITF_UNDOREDOMANAGER_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_CAMERACONTROLLERMANAGER_H_
#include "engine/actors/components/Camera/CameraControllerManager.h"
#endif // _ITF_CAMERACONTROLLERMANAGER_H_

#ifndef _ITF_ACTOREDITOR_H_
#include "tools/plugins/ActorEditor/ActorEditor.h"
#endif //_ITF_ACTOREDITOR_H_

namespace ITF
{
    SequenceEditorPlugin* SequenceEditorPlugin::m_singleton = NULL;

    void SequenceEditorProcessUndoRedoChange(ObjectRef _actorRef)
    {
        SequenceEditorPlugin* sequence = SequenceEditorPlugin::m_singleton;
        if (!sequence)
            return;

        ITF_VECTOR<SequenceEditorPlugin::SeqState> & seqState = sequence->getSequencesState();
        ITF_VECTOR<SequenceEditorPlugin::SeqState>::iterator it = seqState.begin();
        for (; it != seqState.end(); ++it)
        {
            if ((*it).m_actorSequenceRef == _actorRef && SequenceEditorPlugin::m_singleton)
            {
                (*it).m_processUndo = btrue;
            }
        }
    }

    SequenceEditorPlugin::SequenceEditorPlugin()
    {
#ifdef DEBUG_SEQUENCES
        m_log = fopen("sequence_debug.txt", "wt");
#endif
        ITF_ASSERT(!m_singleton);
        m_singleton = this;
        SET_SEQPLUG_STATE(state_Idle);

        m_messagesEditor.setOwner(this);
        m_actorEditor.setOwner(this);
        m_trajectoryEditor.setOwner(this);
        m_currentSequenceActorIndex = -1;
        m_autoKeyMode = false;
        m_cameraModeSequence = true;
        m_defaultSplineMode = Spline::interp_spline;
        m_bezierLinked = btrue;
        m_useExternInactiveChannels = bfalse;
    }


    SequenceEditorPlugin::~SequenceEditorPlugin()
    { 
        m_templateClientHandler.freeUsedTemplates();
        clearSequenceStates();
#ifdef DEBUG_SEQUENCES
        if(m_log)
            fclose(m_log);
#endif
    }

    void SequenceEditorPlugin::stopAll()
    {
        m_templateClientHandler.freeUsedTemplates();
        Actor* actSequence = getCurrentSequenceActor();
        if (!actSequence)
            return;

        SequencePlayerComponent *seqPlayComp = actSequence->GetComponent<SequencePlayerComponent>();
        if (seqPlayComp)
        {
            seqPlayComp->stop();
        }
        EDITOR->backToGame();
        hideTrajectories();
    }

    void  SequenceEditorPlugin::hideTrajectories()
    {
        m_trajectoryEditor.setShowAllTraj(bfalse);
        m_trajectoryEditor.SET_CUR_TRAJ(-1);
    }


    void SequenceEditorPlugin::getSelectionStatus (ObjectRef& _selection, bbool& _selectionIsLocked)
    {
        m_trajectoryEditor.getSelectionStatus (_selection, _selectionIsLocked);
    }

    void    SequenceEditorPlugin::sendCurrentFrame(ObjectRef _sequenceRef, i32 _frame)
    {
        ITF::Blob blob(ITF::BLOB_EXTRACT_AND_WRITE_ENABLED);
        blob.pushString(SequenceEditorPlugin::getPluginName());  // plugin ID
        blob.pushString(SequenceEditorMessages::getTimerSynchroCmdName());               // cmd ID
        blob.pushUInt32(_sequenceRef.getValue());
        blob.pushInt32(_frame);
        PLUGINGATEWAY->sendPacket(blob, SequenceEditorPlugin::getNetPeer(), SequenceEditorPlugin::getSenderID());
    }



    void    SequenceEditorPlugin::sendSenquenceState(ObjectRef _sequenceRef, SequencePlayerComponent::State _seqState)
    {
        ITF::Blob blob(ITF::BLOB_EXTRACT_AND_WRITE_ENABLED);
        blob.pushString(SequenceEditorPlugin::getPluginName());  // plugin ID
        blob.pushString(SequenceEditorMessages::sequenceStateCmdName());               // cmd ID
        blob.pushUInt32(_sequenceRef.getValue());
        blob.pushInt32(_seqState);
        PLUGINGATEWAY->sendPacket(blob, SequenceEditorPlugin::getNetPeer(), SequenceEditorPlugin::getSenderID());
    }

    void  SequenceEditorPlugin::onCameraModeChange (int _prevMode, int _newMode)
    {
        SequencePlayerComponent *seqPlayComp = getCurrentSequenceComponent();

        if (!seqPlayComp)
            return;

        switch (_newMode)
        {
            case Camera::Camera_Editor:
                seqPlayComp->allowCameraControl(bfalse);
                #ifdef DEBUG_SEQUENCES
                if(m_log)
                    fputs("disable camera control from onCameraModeChange event", m_log);
                #endif
            break;
            case Camera::Camera_InGame:
                seqPlayComp->allowCameraControl(btrue);
                #ifdef DEBUG_SEQUENCES
                if(m_log)
                    fputs("enable camera control from onCameraModeChange event", m_log);
                #endif
            break;
            case Camera::Camera_Slave:
                seqPlayComp->allowCameraControl(btrue);
                #ifdef DEBUG_SEQUENCES
                if(m_log)    
                    fputs("enable camera control from onCameraModeChange event", m_log);
                #endif
            break;
            default:
                ITF_ASSERT(0); // unhandled mode
            break;
        }
    }

    void    SequenceEditorPlugin::update()
    {
  
        // check state sequence changed
        if(SequenceEditorPlugin::getNetPeer() != NULL)
        {
            bbool updateList = false;
            ITF_VECTOR<SeqState>::iterator seqIter = m_sequencesState.begin();

            for (; seqIter != m_sequencesState.end();)
            {
                SeqState & seqState = (*seqIter);
                Actor* pAct = (Actor*)GETOBJECT(seqState.m_actorSequenceRef);
                if(pAct == NULL)
                {
                    updateList = true;
                    ActorEditor::getInstance()->changeTemplate((*seqIter).m_path, NULL, bfalse);
                    (*seqIter).clear();
                    seqIter = m_sequencesState.erase(seqIter);
                    continue;
                }

                if (seqState.m_processUndo)
                {
                    fullClone(seqState.m_actorTemplateUndoRedo, seqState.m_actorTemplate);
                    propagateActorChanges(pAct, bfalse);
                    seqState.m_processUndo = bfalse;
                }

                SequencePlayerComponent *seqComp = pAct->GetComponent<SequencePlayerComponent>();
                if (seqComp && seqState.m_needReload)
                {
                    m_trajectoryEditor.clearSelection();

                    seqComp->allActorsResourceLoad();
                    RESOURCE_MANAGER->flushPendingOps();
                    seqComp->allActorsUpdatePhysicalReady();

                    seqComp->setExternInativeChannels(m_useExternInactiveChannels ? &m_externInactiveChannels : NULL);
                    if (seqState.m_oldState != SequencePlayerComponent::State_Stopped)
                        forceCurrentFrame(pAct, seqState.m_sequenceFrame);

                    seqComp->setState(seqState.m_oldState);
                    processSelectedEvent();

                    m_messagesEditor.sendSequence (pAct);
                    CAMERACONTROLLERMANAGER->pauseMode(bfalse);
                    seqState.m_needReload = bfalse;
                }

				if (seqComp)
				{
                    // save state of sequences
                    SequencePlayerComponent::State state = seqComp->getState();
                    if(seqState.m_oldState != state)
                    {
                        seqState.m_oldState = state;
                        sendSenquenceState(pAct->getRef(), state);
                    }

                    if(seqState.m_sequenceFrame != seqComp->getCurrentFrame())
                        sendCurrentFrame(pAct->getRef(), seqComp->getCurrentFrame());
                    seqState.m_sequenceFrame = seqComp->getCurrentFrame();
                    seqComp->setExternInativeChannels(m_useExternInactiveChannels ? &m_externInactiveChannels : NULL);
                }
                seqIter++;
            }

            // if a sequence delete/remove from scene, send list sequence at SequenceEditor
            if(updateList)
            {
                m_messagesEditor.sendSequenceList();
            }

            if (!m_messagesEditor.playerListSendOk())
                m_messagesEditor.sendListPlayers();

        }

        


        m_actorEditor.update();

        bbool selectionIsLocked, hasSelection;
        ObjectRef selectionRef;

        // get focus if selection is locked
        getSelectionStatus(selectionRef, selectionIsLocked);
        hasSelection = (selectionRef != ITF_INVALID_OBJREF);
        const Plugin* focus = PLUGINGATEWAY->getExclusiveFocus();
        if (hasSelection && selectionIsLocked && (!focus))
        {
            focus = (const Plugin*)this;
        }

        if (focus)
        {
            if (focus == this)
            {
                if (getState() != state_MovePivot)
                {
                    if (hasSelection)
                    {
                        if(selectionIsLocked)
                        {
                            SET_SEQPLUG_STATE(state_EditSequence);
                        }
                        else
                        {
                            EDITOR->setSubEditedItem(ITF_INVALID_OBJREF);
                        }
                    }
                    else
                    {
                        SET_SEQPLUG_STATE(state_Idle);
                    }
                }
            }
            else
                UnselectAll(bfalse);
        }

        if (INPUT_ADAPTER->isKeyPressed(EDITOR_CAMERA_KEY)) // camera control
        {
            if (CAMERA->getMode() == Camera::Camera_Slave)
                CAMERA->setMode(Camera::Camera_Editor);
            
            SequencePlayerComponent *seqPlayComp = getCurrentSequenceComponent();
            if (seqPlayComp)
            {
                seqPlayComp->allowCameraControl(bfalse);
                #ifdef DEBUG_SEQUENCES
                if(m_log)
                    fputs("disable camera control", m_log);
                #endif
            }
        }

        m_trajectoryEditor.update();
    }


    bbool SequenceEditorPlugin::isSubEditAllowed()
    {
        return m_trajectoryEditor.isSubEditAllowed();
    }

    void SequenceEditorPlugin::UnselectAll(bbool _force)
    {
        EDITOR->allowStoreAllPickableInitData();

        m_trajectoryEditor.UnselectAll(_force);
        m_trajectoryEditor.setShowAllTraj(bfalse);

        if (_force)
        {
            SequencePlayerComponent_Template * seqPlayComp = getCurrentSequenceComponentTemplate();
            if (seqPlayComp)
            {
                u32 evtCount = seqPlayComp->getEventsCount();
                for (u32 iEvt = 0; iEvt < evtCount; iEvt++)
                {
                    SequenceEvent_Template* evt = seqPlayComp->getEventAtIndex(iEvt);
                    if(!evt)
                        continue;
                    evt->setSelected(-1);
                }
            }
        }

        if (!_force)
            m_actorEditor.onActorPicked(ITF_INVALID_OBJREF);
    }

    bbool SequenceEditorPlugin::selectEvent( u32 _uid, i32 selectFlag)
    {
        SequencePlayerComponent_Template * seqPlayComp = getCurrentSequenceComponentTemplate();
        if (seqPlayComp)
        {
            u32 evtCount = seqPlayComp->getEventsCount();
            for (u32 iEvt = 0; iEvt < evtCount; iEvt++)
            {
                SequenceEvent_Template* evt = seqPlayComp->getEventAtIndex(iEvt);
                if(!evt || evt->getUID() != _uid) continue;

                evt->setSelected(selectFlag);
                return btrue;
            }
        }
        return bfalse;
    }


    void SequenceEditorPlugin::processSelectedEvent( )
    {
        SequencePlayerComponent_Template * seqPlayComp = getCurrentSequenceComponentTemplate();
        if (!seqPlayComp)
            return;

        u32                 evtCount = seqPlayComp->getEventsCount();

        u32                 trajEvtCount = 0;
        u32                 trajEvtIndex = U32_INVALID;

        for (u32 iEvt = 0; iEvt < evtCount; iEvt++)
        {
            SequenceEvent_Template* evt = seqPlayComp->getEventAtIndex(iEvt);
            if(!evt || !evt->isSelected()) continue;

            switch (evt->getType())
            {
            case SequenceEvent_Template::event_PlayTrajectory:
                trajEvtIndex = iEvt;
                trajEvtCount++;
                break;
            }
        }
        if (trajEvtIndex != U32_INVALID && trajEvtCount == 1)
        {
            m_trajectoryEditor.selectTrajectory(trajEvtIndex);
        }
    }

    void SequenceEditorPlugin::onKey(   i32 _key, InputAdapter::PressStatus _status )
    {
        SequencePlayerComponent *seqPlayComp =  NULL;
        if (_status != InputAdapter::Released)
        {
            switch (_key)
            {
            case 'S':
                if (INPUT_ADAPTER->isKeyPressed(KEY_LCTRL) || INPUT_ADAPTER->isKeyPressed(KEY_RCTRL))
                {
                    m_messagesEditor.saveSequences();
                    break;
                }
            }
        }
        
        if (!INPUT_ADAPTER->isKeyPressed(KEY_LCTRL) && !INPUT_ADAPTER->isKeyPressed(KEY_LSHIFT) && _status != InputAdapter::Released )
        {
            switch (_key)
            {
            //case KEY_LEFT:
            //    goToPrevFrame(1);
            //    break;

            //case KEY_RIGHT:
            //    goToNextFrame(1);
            //    break;

            case 'Q':
                seqPlayComp = getCurrentSequenceComponent();
                if (seqPlayComp)
                {
                    if (seqPlayComp->getState() != SequencePlayerComponent::State_Playing)
                    {
                        seqPlayComp->setFrameStartSaved(seqPlayComp->getCurrentFrame());
                        seqPlayComp->start();
                        seqPlayComp->unpause();
                        seqPlayComp->allowCameraControl(btrue);
                    } else
                    {
                        seqPlayComp->setCurrentFrame(seqPlayComp->getFrameStartSaved(), btrue);
                        seqPlayComp->pause();
                        seqPlayComp->allowCameraControl(false);
                        Actor * seqActor = getCurrentSequenceActor();
                        if (seqActor)
                            sendCurrentFrame(seqActor->getRef(), seqPlayComp->getFrameStartSaved());
                    }
                }
                break;

            //case 'Q':
            //    seqPlayComp = getCurrentSequenceComponent();
            //    if (seqPlayComp && seqPlayComp->getState() == SequencePlayerComponent::State_Playing)
            //    {
            //        seqPlayComp->pause();
            //        seqPlayComp->allowCameraControl(false);
            //        sendCurrentFrame(getCurrentSequenceActor()->getRef(), seqPlayComp->getFrameStartSaved());
            //    }
            //    break;

            //case 'W':
            //    seqPlayComp = getCurrentSequenceComponent();
            //    if (seqPlayComp)
            //    {
            //        seqPlayComp->unpause();
            //        seqPlayComp->allowCameraControl(btrue);
            //        seqPlayComp->setCurrentFrame(seqPlayComp->getCurrentFrame(), btrue);
            //    }
            //    break;


            case 'J':
                setNewInitialPosition();
                break;

            case KEY_ESC:
                seqPlayComp = getCurrentSequenceComponent();
                if (seqPlayComp)
                    seqPlayComp->stop();
                break;

            default:
                break;
            }
        }

        m_trajectoryEditor.onKey(_key, _status);
       
    }

    void SequenceEditorPlugin::onStartRotation()
    {
        m_trajectoryEditor.onStartRotation();
    }

    void SequenceEditorPlugin::onEndRotation()
    {
        m_trajectoryEditor.onEndRotation();
    }


    Actor* SequenceEditorPlugin::getCurrentSequenceActor() 
    {
        if (m_currentSequenceActorIndex >= 0 && m_currentSequenceActorIndex < (i32)m_sequencesState.size())
            return (Actor*)m_sequencesState[m_currentSequenceActorIndex].m_actorSequenceRef.getObject();
        return NULL;
    }


    Actor_Template* SequenceEditorPlugin::getCurrentSequenceActorTemplate() 
    {
        if (m_currentSequenceActorIndex >= 0 && m_currentSequenceActorIndex < (i32)m_sequencesState.size())
            return (Actor_Template*)m_sequencesState[m_currentSequenceActorIndex].m_actorTemplate;
        return NULL;
    }

    SequencePlayerComponent*  SequenceEditorPlugin::getCurrentSequenceComponent()
    {
        Actor* actSequence = getCurrentSequenceActor();
        if (!actSequence)
            return NULL;

        return actSequence->GetComponent<SequencePlayerComponent>();
    }


    SequencePlayerComponent_Template*  SequenceEditorPlugin::getCurrentSequenceComponentTemplate()
    {
        if (m_currentSequenceActorIndex >= 0 && m_currentSequenceActorIndex < (i32)m_sequencesState.size())
        {
            return m_sequencesState[m_currentSequenceActorIndex].m_actorTemplate->GetComponent<SequencePlayerComponent_Template>();
        }

        return NULL;
    }


    void SequenceEditorPlugin::receive(Blob& _blob,const NETPeer* _peer, u32 _senderID)
    {
        m_messagesEditor.receive(_blob, _peer, _senderID);
   }
 
    bbool SequenceEditorPlugin::setCurrentSequenceActor (Actor* _act, bbool _allowAdd)
    {
        if (!_act)
        {
            m_currentSequenceActorIndex = -1;
            return bfalse;
        }

        m_templateClientHandler.addUsedTemplate(_act->getLua().getStringID());
        Actor_Template * actTemplate = (Actor_Template *)TEMPLATEDATABASE->getTemplate<Actor_Template>(&m_templateClientHandler, _act->getLua());
        SequencePlayerComponent *seqComponent = _act->GetComponent<SequencePlayerComponent>();

        if (!actTemplate || !seqComponent)
        {
            m_currentSequenceActorIndex = -1;
            return bfalse;
        }
        
        ObjectRef actRef = _act->getRef();
        ITF_VECTOR<SeqState>::iterator stateIter = m_sequencesState.begin();
        for (u32 i=0; stateIter != m_sequencesState.end(); ++stateIter, ++i)
        {
            if (actRef == (*stateIter).m_actorSequenceRef)
            {
                m_currentSequenceActorIndex = i;
                return btrue;
            }
        }

        if (!_allowAdd)
        {
            m_currentSequenceActorIndex = -1;
            return bfalse;
        }

        // before adding the new sequence actor verify if another object uses same path and remove it
        String path = _act->getLua().getString();

        
        stateIter = m_sequencesState.begin();
        for (u32 idx = 0; stateIter != m_sequencesState.end(); idx++)
        {
            Actor * act = (Actor * )((*stateIter).m_actorSequenceRef.getObject());
            if (act && act->getLua().getString() == path)
            {
                m_currentSequenceActorIndex = idx;
                return btrue;
            } else
            {
                ++stateIter;
            }
        }
        
        m_currentSequenceActorIndex = m_sequencesState.size();
        m_sequencesState.push_back(SeqState());
        SeqState *newSeqPtr = &m_sequencesState[m_currentSequenceActorIndex];
        newSeqPtr->m_actorSequenceRef   = actRef;
        
        
        newSeqPtr->m_actorTemplate         = newAlloc(mId_Editor, Actor_Template());
        newSeqPtr->m_actorTemplateOverride = newAlloc(mId_Editor, Actor_Template());
        newSeqPtr->m_actorTemplateUndoRedo = newAlloc(mId_Editor, Actor_Template());

        newSeqPtr->m_needReload            = btrue;
        newSeqPtr->m_path                  = path;

        // One copy for work
        fullClone(actTemplate, newSeqPtr->m_actorTemplate);
        // One copy for actor
        fullClone(actTemplate, newSeqPtr->m_actorTemplateOverride);
        // One copy for undo
        fullClone(actTemplate, newSeqPtr->m_actorTemplateUndoRedo);

        ActorEditor::getInstance()->changeTemplate(path, newSeqPtr->m_actorTemplateOverride, bfalse);
        return btrue;
    }


    void SequenceEditorPlugin::onActorPicked(ObjectRef _actor)
    {
        m_actorEditor.onActorPicked(_actor);
    }

    void SequenceEditorPlugin::onActorSubEdit(ObjectRef _actor)
    {
        m_actorEditor.onActorSubEdit(_actor);
    }

    void SequenceEditorPlugin::onPickableRenamed( Pickable* _pickable, const String8& _name )
    {
        m_actorEditor.onPickableRenamed( _pickable, _name );
    }

    bbool SequenceEditorPlugin::onPickShape( PickingShape* _shape ) 
    {
         BaseObject* pObj = _shape->getOwnerPtr();
         switch(pObj->getObjectType())
        {
        case BaseObject::eActor:
        {
            Pickable* p = (Pickable*)pObj;
            p->setSelected(_shape->getPickingInfoPtr());
            return m_actorEditor.onActorPicked(p->getRef());
            break;
        }

        case BaseObject::eTrajectory:
            return m_trajectoryEditor.onPickShape(_shape);
            break;

        default:
            break;
        }
        return bfalse;
    }

    void SequenceEditorPlugin::onNewSequence(ObjectRef _actorRef)
    {
        m_actorEditor.onActorPicked(   _actorRef  );
    }

    void SequenceEditorPlugin::fillEditorActiveList( ObjectRefList &_objects, PickingShapeVector& _orderedShapes, Scene* _pFilterScene ) const
    {
        m_trajectoryEditor.fillEditorActiveList(_objects, _orderedShapes, _pFilterScene);       
    }

    bbool SequenceEditorPlugin::isPickingAllowed()
    {
        if (!m_trajectoryEditor.isPickingAllowed( ))
            return bfalse;
        return btrue;
    }

    void SequenceEditorPlugin::onMouseButton(InputAdapter::MouseButton _but, InputAdapter::PressStatus _status )
    {
        if (_but == InputAdapter::MB_Left && _status == InputAdapter::Pressed)
        {
            switch (m_plugState)
            {
            case state_MovePivot:
            {
                Actor * act = m_setSequencePivotActor.getActor();
                if (act && act->GetComponent<SequencePlayerComponent>())
                {
                    UndoRedoSequenceScope changeScope("set sequence pivot");

                    Vec3d newPos = EDITOR->getMouse3d();
                    EDITOR->snap(newPos, NULL, btrue);

                    if(act->isZForced())
                        newPos.m_z = act->getDepth();

                    const Vec3d deltaPosInv = act->getPos() - newPos;
                    moveAllTrajectories(act, deltaPosInv);

                    UNDOREDO_MANAGER->preChange(act);
                    act->setPos(newPos);
                    act->setWorldInitialPos(newPos);

                    PLUGINGATEWAY->onObjectChanged(act);
                }

                m_setSequencePivotActor.invalidate();
                m_plugState = state_Idle;
                return;
            }
            
            default:
                break;
            }
        }

        m_trajectoryEditor.onMouseButton(_but, _status);
    }

    void SequenceEditorPlugin::onMouseWheel   (   i32 _wheelValue, i32 _wheelDelta)
    {
        m_trajectoryEditor.onMouseWheel(_wheelValue, _wheelDelta);
    }

    bbool SequenceEditorPlugin::bCanEnterEditMode ()
    {
        return m_trajectoryEditor.bCanEnterEditMode();
    }

    bbool SequenceEditorPlugin::bCanLeaveEditMode ()
    {
        return m_trajectoryEditor.bCanLeaveEditMode();
    }

#ifdef DEBUG_SEQUENCES
    void SequenceEditorPlugin::setState(state _state, const char* _file, int _line)
    {
        if (_state != m_plugState && m_log)
        {
            switch (_state)
            {
            case state_Idle:
                fputs("\n new state: - Idle -", m_log);
                break;
            case state_EditSequence:
                fputs("\n new state: - EditSequence -", m_log);
                break;
            default:
                ITF_ASSERT(0); // unhandled case
                break;
            }
            fprintf(m_log, "file: %s\nline: %d\n", _file, _line);
        }
        m_plugState = _state;
    }
#else
    void    SequenceEditorPlugin::setState(state _state) 
    {
        m_plugState = _state;
    }
#endif

    SequenceEditorPlugin::state   SequenceEditorPlugin::getState() 
    {
        return m_plugState;
    }

    bbool   SequenceEditorPlugin::bIsEditingActorTrajectory(const ObjectRef _actor)
    {
        return m_trajectoryEditor.bIsEditingActorTrajectory(_actor);
    }

    void SequenceEditorPlugin::frameStep(i32 _amount)
    {
        SequencePlayerComponent *seqPlayComp = getCurrentSequenceComponent();
        Actor * seqActor = getCurrentSequenceActor();
        if (seqPlayComp && seqActor)
        {
            seqPlayComp->setCurrentFrame(seqPlayComp->getCurrentFrame() + _amount, btrue);
            sendCurrentFrame(seqActor->getRef(), seqPlayComp->getCurrentFrame());
        }
    }


    void SequenceEditorPlugin::goToNextFrame(i32 _amount)
    {
        frameStep(_amount);//TARGETDT * 
    }

    void SequenceEditorPlugin::goToPrevFrame(i32 _amount)
    {
        frameStep(-_amount);//TARGETDT * 
    }


    void SequenceEditorPlugin::onPostSceneActivation(const Scene * _scene, bbool _activated)
    {
        if(_activated && SequenceEditorPlugin::getNetPeer())
        {
            m_messagesEditor.sendSequenceList(btrue);
        }
    }

    void SequenceEditorPlugin::onObjectCreated(BaseObject* _pObj )
    {
        if(!_pObj)
            return;


        if(_pObj->getObjectType() == BaseObject::eActor)
        {
            SequencePlayerComponent *seqPlayComp = ((Actor*)_pObj)->GetComponent<SequencePlayerComponent>();
            if (seqPlayComp && SequenceEditorPlugin::getNetPeer())
            {
                m_messagesEditor.sendSequenceList();
            }
        }
    }

    void SequenceEditorPlugin::onObjectAdded       ( const Scene* _pScene, BaseObject* _pObj )
    {
        onObjectCreated(_pObj);
    }

    void SequenceEditorPlugin::setNewInitialPosition()
    {
        SequencePlayerComponent          *seqPlayComp           = getCurrentSequenceComponent();
        SequencePlayerComponent_Template *seqPlayCompTemplate   = getCurrentSequenceComponentTemplate();
        if (!seqPlayComp || !seqPlayCompTemplate)
            return;

        u32 selectionCount = EDITOR->getSelectionSize();
        for(u32 i = 0; i < selectionCount; ++i)
        {
            PickingShape* pPS = EDITOR->getSelectionAt(i);
            Pickable* pPickable = static_cast<Pickable*>(pPS->getOwnerPtr());
            if(!pPickable)
                continue;
            
            ObjectRef actorRef = pPickable->getRef();                    
            if(pPickable->getObjectType() != BaseObject::eActor)
                continue;

            Vec3d offsetPos;
            bbool found = false;

            u32 evtCount = seqPlayComp->getEventsListCount();
            for (u32 iEvt = 0; iEvt < evtCount; iEvt++)
            {
                SequenceEvent         * evt          = seqPlayComp->getEventAtIndex(iEvt);
                SequenceEvent_Template* evtTemplate  = seqPlayCompTemplate->getEventAtIndex(iEvt);

                if(!evt || !evtTemplate)
                    continue;

                if (evtTemplate->getType() == SequenceEvent_Template::event_PlayTrajectory)
                {
                    PlayTrajectory_evt          * tjcEvt            = (PlayTrajectory_evt*)evt;
                    PlayTrajectory_evtTemplate  * tjcEvtTemplate    = (PlayTrajectory_evtTemplate*)evtTemplate;

                    Actor* act = tjcEvt->getBindedActor();
                    if(tjcEvt && act && act->getRef() == actorRef)
                    {
                        Trajectory_Template * tjcTemplate   = tjcEvtTemplate->getTrajectoryTemplate();
                        u32 posCount = tjcTemplate->getPosCount();
                        for (u32 i = 0; i < posCount; i++)
                        {
                            if(i == 0 && !found)
                            {
                                found = true;
                                
                                // current position
                                Vec3d vPos = pPickable->getPos();
                                // old position
                                Vec3d firstPos = tjcTemplate->getGlobalPosAt(i, act->getPos());
                                // offset
                                offsetPos = vPos - firstPos;
                            }

                            Vec3d newPos = tjcTemplate->getGlobalPosAt(i, act->getPos()) + offsetPos;
                            tjcTemplate->setGlobalPosDataAt(newPos, act->getPos(), tjcTemplate->getPosInterpAt(i), tjcTemplate->getPosTimeAt(i), i);
                        }
                        getTrajectoryEditor().sendTrajectory(tjcEvt->getTrajectory(), tjcEvtTemplate);
                    }
                }
            }
            propagateActorChanges(getCurrentSequenceActor(), bfalse);
        }
    }

    bbool SequenceEditorPlugin::getSequenceState( ObjectRef _actorSequenceRef, SeqState* &_seqState )
    {
        u32 count = m_sequencesState.size();
        for (u32 i = 0; i < count; i++)
        {
            if(m_sequencesState[i].m_actorSequenceRef == _actorSequenceRef)
            {
                _seqState = &m_sequencesState[i];
                return btrue;
            }
        }
        return bfalse;
    }

    void SequenceEditorPlugin::propagateActorChanges(Actor * _actorSequence, bbool _undoRedo, bbool _noHotReaload, bbool _sendTrackList)
    {
        if (!_actorSequence)
            return;

        SequencePlayerComponent *seqComp = _actorSequence->GetComponent<SequencePlayerComponent>();
        if (!seqComp)
            return;

        SeqState *seqState;
        if (!getSequenceState(_actorSequence->getRef(), seqState))
            return;

        if (!seqState->m_actorTemplate || !seqState->m_actorTemplateOverride || !seqState->m_actorTemplateUndoRedo)
            return;


        if (_undoRedo && UNDOREDO_MANAGER)
        {
            UNDOREDO_MANAGER->preChangeTemplate(seqState->m_actorTemplateUndoRedo, _actorSequence->getRef(), SequenceEditorProcessUndoRedoChange);
        }

        String pathStr = _actorSequence->getLua().getString();

        if (_noHotReaload)
        {
            SequenceEvent::isReload = btrue;
			fullClone(seqState->m_actorTemplate, seqState->m_actorTemplateOverride);
			SequenceEvent::isReload = bfalse;
            if (_sendTrackList)
                m_messagesEditor.sendSequenceTrackList(_actorSequence);
        } else
        {
            CAMERACONTROLLERMANAGER->pauseMode(btrue);
            m_trajectoryEditor.selectTrajectory(-1);

            seqState->m_sequenceFrame = seqComp->getCurrentFrame();
            ActorEditor::getInstance()->modifyOverridedTemplate(pathStr, seqState->m_actorTemplate,
                (ESerializeFlags)(ESerialize_Data_Save|ESerialize_Editor_Save),
                (ESerializeFlags)(ESerialize_Data_Load|ESerialize_Editor_Load));

            seqState->m_needReload = btrue;
        }
        // always maintain last version copied on undoRedo
        seqState->m_modified = btrue;
        fullClone(seqState->m_actorTemplateOverride, seqState->m_actorTemplateUndoRedo);
        fullClone(seqState->m_actorTemplateOverride, seqState->m_actorTemplate);
    }

    void SequenceEditorPlugin::revertUnpropagateActorChanges(Actor * _actorSequence)
    {
        if (!_actorSequence)
            return;

        SeqState *seqState;
        if (getSequenceState(_actorSequence->getRef(), seqState))
            return;

        if (!seqState->m_actorTemplate || !seqState->m_actorTemplateOverride)
            return;

        fullClone(seqState->m_actorTemplateOverride, seqState->m_actorTemplate);
        seqState->m_needReload = btrue;
    }


    PickingShapePluginData* SequenceEditorPlugin::createPickingShapeData( PickingShape * _shape )
    {
        return getTrajectoryEditor().createPickingShapeData(_shape);
    }

    bbool SequenceEditorPlugin::onEditorRemove( const SafeArray<PickingShape*>& _shapesWithSameOwner  )
    {
        return getTrajectoryEditor().onEditorRemove(_shapesWithSameOwner);
    }

    void SequenceEditorPlugin::updateGlobalSequenceData(bbool _unpause)
    {
        // Update camera mode
        if (getCameraModeSequence())
        {
            if (CAMERA->getMode() != Camera::Camera_InGame)
                CAMERA->setMode(Camera::Camera_InGame);
        } else
        {
            if (CAMERA->getMode() != Camera::Camera_Editor)
                CAMERA->setMode(Camera::Camera_Editor);
        }

        // remove pause if pause is set
        if (_unpause)
            CHEATMANAGER->setPause(bfalse);
    }

    bbool SequenceEditorPlugin::isAutoKeyAllowed()
    {
        if (!m_autoKeyMode)
            return bfalse;

        SequencePlayerComponent *seqPlayerComp = getCurrentSequenceComponent();
        if (!seqPlayerComp)
            return bfalse;

        return seqPlayerComp->getState() != SequencePlayerComponent::State_Playing;
    }

    void SequenceEditorPlugin::onToolConnect( const NETPeer* _peer, u32 _senderID )
    {
        Plugin::onToolConnect(_peer, _senderID);

        // When we have a sequence, we remove auto pause mode
        EDITOR->setAutoPauseMode(bfalse);
        EDITOR->setCancelClickInitTime(0.f);
    }

    void SequenceEditorPlugin::onToolDisconnect( const NETPeer* _peer )
    {
        Plugin::onToolDisconnect(_peer);

        // When we quit the sequence plugin, we restore auto pause and cancel click
        EDITOR->setAutoPauseMode(btrue);
        EDITOR->setCancelClickInitTime(1.f);
        
        UnselectAll(btrue);
        setAutoKeyMode(bfalse);
    }

    void SequenceEditorPlugin::forceCurrentFrame( Actor *actSequence, int frame )
    {
        if (actSequence && actSequence->updatePhysicalReady())
        {
            SequencePlayerComponent *seqPlayComp = actSequence->GetComponent<SequencePlayerComponent>();
            if (seqPlayComp)
            {
                CAMERACONTROLLERMANAGER->pauseMode(btrue);
                seqPlayComp->unpause(); // enable component if it's paused by cells
                seqPlayComp->refreshEnableEventsByChannel();
                seqPlayComp->resetCamera();
                seqPlayComp->setCurrentFrame(frame, btrue);
                seqPlayComp->allowCameraControl(btrue);

                // on set current frame, force camera !
                CAMERACONTROLLERMANAGER->pauseMode(bfalse);
                updateGlobalSequenceData(bfalse);

                if (CHEATMANAGER->getPause() && actSequence->isActive())
                {
                    actSequence->update(0);
                    updateSequenceActors(seqPlayComp);
                }
            }
        }
    }

    void SequenceEditorPlugin::fillEmptyTrajectories(const Vec3d& _pos)
    {
        SequencePlayerComponent          *seqPlayComp         = getCurrentSequenceComponent();
        SequencePlayerComponent_Template *seqPlayCompTemplate = getCurrentSequenceComponentTemplate();

        if (!seqPlayComp || !seqPlayCompTemplate)
            return;

        // add trajectory default keys if necessary
        u32 count = seqPlayCompTemplate->getEventsCount();
        for (u32 i=0; i<count; i++)
        {
            SequenceEvent_Template* evtTemplate = seqPlayCompTemplate->getEventAtIndex(i);
            if (  evtTemplate->getType() == SequenceEvent_Template::event_PlayTrajectory)
			{
				PlayTrajectory_evtTemplate * evtTraj = ((PlayTrajectory_evtTemplate *)evtTemplate);

				if (evtTraj->getTrajectoryTemplate()->isEmpty())
				{
					if (evtTraj->isBindedToCamera())
					{
						((PlayTrajectory_evtTemplate *)evtTemplate)->fillFromActor(NULL, _pos);
					} else
					{
						Actor * actor = seqPlayComp->getActorFromObjectPath(((PlayTrajectory_evtTemplate *)evtTemplate)->getObjectPath(), ((PlayTrajectory_evtTemplate *)evtTemplate)->getObjectID());
						if(actor)
						{
							((PlayTrajectory_evtTemplate *)evtTemplate)->fillFromActor(actor, _pos);
						}
					}
				}
			}
        }
    }

    void SequenceEditorPlugin::clearSequenceStates()
    {
        if (!ActorEditor::getInstance())
            return;

        ITF_VECTOR<SeqState>::iterator stateIter = m_sequencesState.begin();
        for (u32 i=0; stateIter != m_sequencesState.end(); ++stateIter, ++i)
        {
            // clear actor before doing binary clone to have correct data pointers
            ActorEditor::getInstance()->changeTemplate((*stateIter).m_path, NULL, bfalse);
            (*stateIter).clear();
        }
        m_sequencesState.clear();
    }

    void SequenceEditorPlugin::onObjectChanged( BaseObject* pBO )
    {   
        m_trajectoryEditor.onObjectChanged(pBO);
    }

    bbool SequenceEditorPlugin::onSetShapePosition( PickingShape* _pPickingShape, const Vec2d& _screenspacepos )
    {
        return m_trajectoryEditor.onSetShapePosition(_pPickingShape, _screenspacepos);
    }

    bbool SequenceEditorPlugin::onOffsetShapePosition( PickingShape* _pPickingShape, const Vec3d& _deltaPos )
    {
        return m_trajectoryEditor.onOffsetShapePosition(_pPickingShape, _deltaPos);
    }

    void SequenceEditorPlugin::checkTracks(u32 _mask)
    {
        SequencePlayerComponent_Template *seqPlayCompTemplate = getCurrentSequenceComponentTemplate();
        if (!seqPlayCompTemplate)
            return;

        u32 evtCount = seqPlayCompTemplate->getEventsCount();
        // search max track to allocate maxTrack + 1 elements
        u32 maxTrack = 0;
        for (u32 i=0; i<evtCount; i++)
        {
            SequenceEvent_Template *evtTemplate = seqPlayCompTemplate->getEventAtIndex(i);
            if (maxTrack < evtTemplate->getTrackLine())
                maxTrack = evtTemplate->getTrackLine();
        }
        ITF_VECTOR<SequenceTrackInfo_Template> &trackList = seqPlayCompTemplate->getTrackList();
        if (trackList.size() <= maxTrack)
            trackList.resize(maxTrack + 1);

        if (_mask & FORCE_RECOMPUTE)
        {
            ITF_VECTOR<SequenceTrackInfo_Template>::iterator trackIter = trackList.begin();
            for (; trackIter != trackList.end(); ++trackIter)
            {
                trackIter->setStartFrame(0);
                trackIter->setDuration(-1);
            }
        }

        if (_mask & COMPUTE_TRACK_LENGHT)
        {
            for (u32 i=0; i<evtCount; i++)
            {
                SequenceEvent_Template *evtTemplate = seqPlayCompTemplate->getEventAtIndex(i);
                SequenceTrackInfo_Template &track =  trackList[evtTemplate->getTrackLine()];

                if (track.getDuration() < 0)
                {
                    track.setStartFrame(evtTemplate->getStartFrame());
                    track.setDuration(evtTemplate->getDuration());
                } else
                {
                    // need to do union
                    i32 startFrameEvt   = evtTemplate->getStartFrame();
                    i32 stopFrameEvt    = evtTemplate->getStopFrame();
                    i32 startFrameTrk   = track.getStartFrame();
                    i32 stopFrameTrk    = startFrameTrk + track.getDuration();

                    if (startFrameTrk > startFrameEvt)
                        startFrameTrk = startFrameEvt;

                    if (stopFrameTrk < stopFrameEvt)
                        stopFrameTrk = stopFrameEvt;

                    track.setStartFrame(startFrameTrk);
                    track.setDuration(stopFrameTrk - startFrameTrk);
                }
            }

            // recompute groups Lenght -> work as event
            ITF_VECTOR<SequenceTrackInfo_Template>::iterator trackIter = trackList.begin();
            for (; trackIter != trackList.end(); ++trackIter)
            {
                SequenceTrackInfo_Template & track = *trackIter;
                u32  parentGroup = track.getParentGroup();
                if (track.getDuration() < 0) //ignore empty tracks
                    continue;
                while (parentGroup != U32_INVALID && parentGroup < trackList.size())
                {
                    SequenceTrackInfo_Template & trackParent = trackList[parentGroup];
                    if (trackParent.getDuration() < 0)
                    {
                        trackParent.setStartFrame(track.getStartFrame());
                        trackParent.setDuration(track.getDuration());
                    } else
                    {
                        // need to do union
                        i32 startFrameChild   = track.getStartFrame();
                        i32 stopFrameChild    = startFrameChild + track.getDuration();
                        i32 startFrameParent  = trackParent.getStartFrame();
                        i32 stopFrameParent   = startFrameParent + trackParent.getDuration();

                        if (startFrameParent > startFrameChild)
                            startFrameParent = startFrameChild;

                        if (stopFrameParent < stopFrameChild)
                            stopFrameParent = stopFrameChild;

                        trackParent.setStartFrame(startFrameParent);
                        trackParent.setDuration(stopFrameParent - startFrameParent);
                    }
                    // we propagate change to be sure nothing is forgotten
                    // but for the moment, we assume that new parentGroup will be == U32_INVALID
                    parentGroup = trackParent.getParentGroup();
                }
            }
        }
        // udpate group track
        for (u32 i=0; i<trackList.size(); i++)
        {
            trackList[i].setGroup(bfalse);
        }

        for (u32 i=0; i<trackList.size(); i++)
        {
            u32 parent = trackList[i].getParentGroup();
            if (parent != U32_INVALID)
                trackList[parent].setGroup(btrue);
        }
    }

    void SequenceEditorPlugin::fullClone( Actor_Template *actSrc, Actor_Template *actDst )
    {
        //if (actSrc != actDst)
        {
            BinaryClone(actSrc, actDst,
                (ESerializeFlags)(ESerialize_Data_Save|ESerialize_Editor_Save),
                (ESerializeFlags)(ESerialize_Data_Load|ESerialize_Editor_Load));

            actDst->setFile(actSrc->getFile());
        }
        actDst->onTemplateLoaded();
    }

    bbool SequenceEditorPlugin::fillContextMenu( EditorContextMenu& _menu, ObjectRef _target, bbool _topItems )
    {
        Pickable *pick = (Pickable *)_target.getObject();
        if (!pick)
            return bfalse;

        switch (pick->getObjectType())
        {
        case BaseObject::eActor:
            {
                Actor                   * act = (Actor *)pick;
                SequencePlayerComponent * seq = act->GetComponent<SequencePlayerComponent>();
                if (!seq)
                    return bfalse;

                _menu.addItemFromExternalPlugin(EditorContextMenu::ItemId_SequenceSetPivot, "Move without moving trajectories");
            }
            break;
        case BaseObject::eTrajectory:
            return m_trajectoryEditor.fillContextMenu(_menu, _target, _topItems);
        default:
            return bfalse;
        }
        return bfalse;
    }

    void SequenceEditorPlugin::onContextMenuItemSelected( ContextMenuItem* _item, ObjectRef _menuTarget )
    {
        switch (_item->getIdFromExternal())
        {
        case EditorContextMenu::ItemId_SequenceSetPivot:
            if(PickingShape* pShape = EDITOR->getSelectionAt(0))
            {
                if (!pShape->getOwnerPtr() || pShape->getOwnerPtr()->getObjectType() != BaseObject::eActor)
                    return;
                Actor * act = (Actor *)pShape->getOwnerPtr();
                SequencePlayerComponent * seq = act->GetComponent<SequencePlayerComponent>();
                if (!seq)
                    return;

                PLUGINGATEWAY->setExclusiveFocus(this);
                m_plugState = state_MovePivot;
                m_setSequencePivotActor = act->getRef();
            }
            return;
        }
        m_trajectoryEditor.onContextMenuItemSelected(_item, _menuTarget);
    }

    void SequenceEditorPlugin::moveAllTrajectories( Actor *act, const Vec3d & _delta )
    {
        setCurrentSequenceActor(act, bfalse);
        
        SequencePlayerComponent_Template *seqPlayCompTemplate = getCurrentSequenceComponentTemplate();
        if (!seqPlayCompTemplate)
            return;

        bbool somethingDone = bfalse;
        u32 evtCount = seqPlayCompTemplate->getEventsCount();
        // search max track to allocate maxTrack + 1 elements
        for (u32 i=0; i<evtCount; i++)
        {
            SequenceEvent_Template *evtTemplate = seqPlayCompTemplate->getEventAtIndex(i);
            if (evtTemplate->getType() == SequenceEvent_Template::event_PlayTrajectory)
            {
                ((PlayTrajectory_evtTemplate *)evtTemplate)->globalPosMove(_delta);
                somethingDone = btrue;
            }
        }
        if (somethingDone)
            propagateActorChanges(act, btrue);
    }

    void SequenceEditorPlugin::draw()
    {
        if(state_MovePivot == m_plugState)
        {
            Vec3d newPos = EDITOR->getMouseOnCurrentWorkingDepth();
            EDITOR->snap(newPos, NULL, btrue);
            Editor_IconStock::EditorIconID icon = Editor_IconStock::Icon_SelectedCinema_Clap;
            EDITOR->getIconStock().drawIcon(icon, newPos, PickingShape::m_disc_picking_tolerance);
        }

    }

    void SequenceEditorPlugin::updateSequenceActors(SequencePlayerComponent *seqPlayComp )
    {
        const ITF_VECTOR<ActorEventDefinitionZones> & evtZone = seqPlayComp->getTemplate()->getActorEventZoneList();
        if (evtZone.size())
        {
            ITF_VECTOR<ActorEventDefinitionZones>::const_iterator evtZoneIter = evtZone.begin();
            for (; evtZoneIter != evtZone.end(); evtZoneIter++)
            {
                StringID name = (*evtZoneIter).getActorFriendly();
                Actor * act = seqPlayComp->getActorFromObjectPath(ITF_STDSTRING(name.getDebugString()), name);
                if (act && act->isPhysicalReady())
                {
                    //act->update(0);
                    act->onEditorMove(bfalse);
                }
            }
        }
    }



} // namespace ITF

#endif //!ITF_FINAL
