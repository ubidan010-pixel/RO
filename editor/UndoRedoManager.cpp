#include "precompiled_editor.h"

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef ITF_CONSOLE_FINAL

#ifndef _ITF_UNDOREDOMANAGER_H_
#include "editor/UndoRedoManager.h"
#endif //_ITF_UNDOREDOMANAGER_H_

#ifndef _ITF_UNDOREDOACTION_H_
#include "editor/UndoRedoAction.h"
#endif //_ITF_UNDOREDOACTION_H_

#ifndef _ITF_EDITOR_H_
#include "editor/editor.h"
#endif //_ITF_EDITOR_H_

#ifndef _ITF_DEBUGINFO_H_
#include "engine/debug/debugInfo.h"
#endif //_ITF_DEBUGINFO_H_

#ifndef _ITF_SUBSCENEACTOR_H_
#include "engine/actors/SubSceneActor.h"
#endif //_ITF_SUBSCENEACTOR_H_

// For special trajectory process !!
#ifndef _ITF_PLAYTRAJECTORY_EVT_H_
#include "engine/sequenceplayer/seq_events/PlayTrajectory_evt.h"
#endif //_ITF_PLAYTRAJECTORY_EVT_H_

#ifndef _ITF_SEQUENCEEDITORPLUGIN_H_
#include "tools/plugins/SequenceEditorPlugin/SequenceEditorPlugin.h"
#endif //_ITF_SEQUENCEEDITORPLUGIN_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

namespace ITF
{

    UndoRedoManager::UndoRedoManager()
            : m_bManagerEnabled(btrue)
            , m_bUndoRequested(bfalse)
            , m_bRedoRequested(bfalse)
            , m_undoRedoMarker(0)
            , m_currentUndoRedoSequence(NULL)
            , m_lastSaveMarker(0)
        {
            WorldManager::addListener(this);
        }


    UndoRedoManager::~UndoRedoManager()
    {
        clear(btrue);
        WorldManager::removeListener(this);
    }

    void UndoRedoManager::clear(bbool _deleteSequences)
    {
        if(_deleteSequences)
        {
            for(u32 iList = 0; iList < m_undoRedoSequenceList.size(); ++iList)
            {
                SF_DEL(m_undoRedoSequenceList[iList]);
            }
        }

        m_undoRedoSequenceList.clear();
        m_undoRedoMarker = 0;
        m_bUndoRequested = bfalse;
        m_bRedoRequested = bfalse;

        m_lastSaveMarker = 0;
    }

    void UndoRedoManager::update()
    {
        if(!m_bManagerEnabled)
            return;
        
        ITF_ASSERT_CRASH(m_undoRedoMarker <= m_undoRedoSequenceList.size(), "Wrong marker!");

        bbool callbacks = (m_bUndoRequested || m_bRedoRequested);
        if(EDITOR && callbacks)
        {
            EDITOR->preUndoRedoOperation();
        }

        if (m_bUndoRequested)
        {
            applyUndo();
            m_bUndoRequested = bfalse;
        }
        else if(m_bRedoRequested)
        {
            applyRedo();
            m_bRedoRequested = bfalse;
        }

        if(EDITOR && callbacks)
        {
            EDITOR->postUndoRedoOperation();
        }
    }

    void UndoRedoManager::applyUndo()
    {
        if(!m_bManagerEnabled)
            return;

        m_bManagerEnabled = bfalse;
        ITF_ASSERT_CRASH(m_undoRedoMarker <= m_undoRedoSequenceList.size(), "Wrong marker!");

        if(m_undoRedoMarker > 0)
        {
            UndoRedoSequence* pSeq = m_undoRedoSequenceList[m_undoRedoMarker - 1];
            pSeq->applyAsUndo();

            --m_undoRedoMarker;

            LOG_OLIV("[UndoRedoManager] Applied an undo (%u undo  and %u redo left) => %s", m_undoRedoMarker, m_undoRedoSequenceList.size() - m_undoRedoMarker, pSeq->getName().cStr());
        }
        m_bManagerEnabled = btrue;
    }
    
    void UndoRedoManager::applyRedo()
    {
        if(!m_bManagerEnabled)
            return;

        m_bManagerEnabled = bfalse;
        ITF_ASSERT_CRASH(m_undoRedoMarker <= m_undoRedoSequenceList.size(), "Wrong marker!");

        if(m_undoRedoMarker < m_undoRedoSequenceList.size())
        {
            UndoRedoSequence* pSeq = m_undoRedoSequenceList[m_undoRedoMarker];
            pSeq->applyAsRedo();

            ++m_undoRedoMarker;

            LOG_OLIV("[UndoRedoManager] Applied an redo (%u undo  and %u redo left) => %s", m_undoRedoMarker, m_undoRedoSequenceList.size() - m_undoRedoMarker, pSeq->getName().cStr());
        }
        m_bManagerEnabled = btrue;
    }

    void UndoRedoManager::onSwitchToWorld( World* /*_pWorld*/, bbool /*_bSwitchDone*/ )
    {
        clear(btrue);
    }

    void UndoRedoManager::beginUndoRedoSequence(const String8& _sequenceDescription)
    {
        if(!m_bManagerEnabled)
            return;

        if (!m_currentUndoRedoSequence)
        {
            m_currentUndoRedoSequence = newAlloc(mId_Editor, UndoRedoSequence(_sequenceDescription));
            LOG_OLIV("[UndoRedoManager] Start : %s", _sequenceDescription.cStr());
        }
    }

    void UndoRedoManager::preChange( Pickable* _pPickable )
    {
        if(m_bManagerEnabled)
        {
            ///// Special process for editing trajectories !!!*
#if !defined(ITF_FINAL) && defined(ITF_DEBUG_STRINGID)
            if (_pPickable->getObjectType() == BaseObject::eTrajectory)
            {
                Trajectory * traj = (Trajectory *)_pPickable;
                PlayTrajectory_evt * evt = traj->getBindedEvent();
                if (!evt)
                    return;

                SequencePlayerComponent * seq = evt->getOwnerSequence();
                if (!seq)
                    return;

                Actor * act = seq->GetActor();
                if (!act->getOverrideTemplate())
                    return;

                registerNewUndoRedoAction(newAlloc(mId_Editor, UndoRedoActionTemplate(act->getOverrideTemplate(), act->getRef(), SequenceEditorProcessUndoRedoChange)));
            } else
#endif
            if(!GAMEMANAGER->isPlayerActor(_pPickable->getRef()))
            {
                registerNewUndoRedoAction(newAlloc(mId_Editor, UndoRedoAction_preChange(_pPickable)));
            }
        }
    }

    void UndoRedoManager::preDelete( Pickable* _pPickable )
    {
        if(m_bManagerEnabled && !GAMEMANAGER->isPlayerActor(_pPickable->getRef()))
            registerNewUndoRedoAction(newAlloc(mId_Editor, UndoRedoAction_preDelete(_pPickable, btrue)));
    }

    void UndoRedoManager::postAdd( Pickable* _pPickable )
    {
        if(m_bManagerEnabled && !GAMEMANAGER->isPlayerActor(_pPickable->getRef()))
            registerNewUndoRedoAction(newAlloc(mId_Editor, UndoRedoAction_postAdd(_pPickable, btrue)));
    }

    void UndoRedoManager::preDeleteGroup( SubSceneActor* _pGroup )
    {
        if(m_bManagerEnabled)
            registerNewUndoRedoAction(newAlloc(mId_Editor, UndoRedoAction_preDelete(_pGroup, bfalse)));
    }

    void UndoRedoManager::postAddGroup( SubSceneActor* _pGroup )
    {
        if(m_bManagerEnabled)
            registerNewUndoRedoAction(newAlloc(mId_Editor, UndoRedoAction_postAdd(_pGroup, bfalse)));
    }
    
    void UndoRedoManager::postSceneMigration(Pickable* _pPickable, const ObjectPath& _previousAbsolutePath)
    {
        if(m_bManagerEnabled)
            registerNewUndoRedoAction(newAlloc(mId_Editor, UndoRedoAction_sceneMigration(_pPickable, _previousAbsolutePath)));
    }

    void UndoRedoManager::preChangeTemplate(TemplateObj* _template, ObjectRef _actorRef, processObjectRef _callback)
    {
        if(m_bManagerEnabled)
            registerNewUndoRedoAction(newAlloc(mId_Editor, UndoRedoActionTemplate(_template, _actorRef, _callback)));
    }


    void UndoRedoManager::validateUndoRedoSequence(const String8& _sequenceDescription)
    {
        if(!m_bManagerEnabled)
            return;

        if(m_currentUndoRedoSequence && m_currentUndoRedoSequence->getName() == _sequenceDescription)
        {
            if(m_currentUndoRedoSequence->getActionCount() > 0/*  TODO check overusage|| m_undoRedoSequenceList*/)
            {
                // Delete all undo/redo info after the marker if a new sequence is validated
                while(m_undoRedoMarker < m_undoRedoSequenceList.size())
                {
                    UndoRedoSequence* pSeq = m_undoRedoSequenceList.back();
                    SF_DEL(pSeq);
                    m_undoRedoSequenceList.pop_back();
                    
                    LOG_OLIV("[UndoRedoManager] Remove a redoable data");
                }

                Time timeInfo;
                SYSTEM_ADAPTER->getTime(timeInfo);
                String8 sequenceDescription;
                sequenceDescription.setTextFormat("[%02u:%02u:%02u] - %s", timeInfo.m_hour, timeInfo.m_minute, timeInfo.m_second, _sequenceDescription.cStr());
                m_currentUndoRedoSequence->setName(sequenceDescription);

                ++m_undoRedoMarker;
                m_undoRedoSequenceList.push_back(m_currentUndoRedoSequence);
                m_currentUndoRedoSequence = NULL;

                LOG_OLIV("[UndoRedoManager] End : add a undoable data");
            }
            else
            {
                cancelUndoRedoSequence(_sequenceDescription);
            }
        }
    }

    void UndoRedoManager::cancelUndoRedoSequence(const String8& _sequenceDescription)
    {
        if(!m_bManagerEnabled)
            return;

        if(m_currentUndoRedoSequence && m_currentUndoRedoSequence->getName() == _sequenceDescription)
        {
            SF_DEL(m_currentUndoRedoSequence);
        }
    }

    void UndoRedoManager::registerNewUndoRedoAction( UndoRedoAction* _action )
    {
        if(!m_bManagerEnabled)
        {
            SF_DEL(_action);
            return;
        }

        if(m_currentUndoRedoSequence)
        {
            m_currentUndoRedoSequence->addAction(_action);
        }
        else
        {
            const UndoRedoSequenceScope undoScope("Single action");
            registerNewUndoRedoAction(_action);
        }
    }
    
    u32 UndoRedoManager::getMemoryUsage()const
    {
        u32 memUsed = 0;

        const u32 uSize = m_undoRedoSequenceList.size();
        for(u32 iList = 0; iList < uSize; ++iList)
            memUsed += m_undoRedoSequenceList[iList]->getMemoryUsage();

        return memUsed;
    }

    bbool UndoRedoManager::dumpToFile(const String8& _absPath) const
    {
        bbool result = bfalse;
#ifdef ITF_WINDOWS
        FILE* plof = fopen(_absPath.cStr(), "wt");
        if(plof)
        {
            ITF_VERIFY(fprintf(plof, "Total memory usage: %u bytes\n\n", getMemoryUsage()) > 0);

            const u32 uSize = m_undoRedoSequenceList.size();
            for(u32 iList = 0; iList < uSize; ++iList)
            {
                if(m_undoRedoMarker == 0 && iList == 0)
                    fprintf(plof, "- Undo/Redo marker\n");

                if(m_lastSaveMarker == 0 && iList == 0)
                    fprintf(plof, "- Save marker\n");

                UndoRedoSequence* pSeq = m_undoRedoSequenceList[iList];
                ITF_VERIFY(fprintf(plof, "- %s (%u bytes)\n", pSeq->getName().cStr(), pSeq->getMemoryUsage()) > 0);

                if((m_undoRedoMarker-1 == iList))
                {
                    if(m_undoRedoMarker > 0)
                        fprintf(plof, "- Undo/Redo marker\n");

                    if(m_lastSaveMarker > 0)
                        fprintf(plof, "- Save marker\n");
                }
            }
            ITF_VERIFY(fclose(plof) == 0);
            result = btrue;
        }
#endif // ITF_WINDOWS
        return result;
    }

    void UndoRedoManager::fillWorldData( UndoRedoWorldData& _worldData ) const
    {
        ITF_ASSERT_CRASH(m_undoRedoMarker <= m_undoRedoSequenceList.size(), "Wrong marker!");
        ITF_ASSERT_CRASH(_worldData.m_sequenceList.size() == 0, "Wrong state!");

        _worldData.m_lastSaveMarker = m_lastSaveMarker;
        _worldData.m_sequenceList   = m_undoRedoSequenceList;
        _worldData.m_marker         = m_undoRedoMarker;

        ITF_ASSERT_CRASH(_worldData.m_marker <= _worldData.m_sequenceList.size(), "Wrong marker!");
    }

    void UndoRedoManager::applyWorldData( UndoRedoWorldData& _worldData )
    {
        ITF_ASSERT_CRASH(m_undoRedoMarker <= m_undoRedoSequenceList.size(), "Wrong marker!");

        ITF_ASSERT_CRASH(m_undoRedoSequenceList.size() == 0, "Wrong state!");
        m_undoRedoMarker        = _worldData.m_marker;
        m_lastSaveMarker        = _worldData.m_lastSaveMarker;
        m_undoRedoSequenceList  = _worldData.m_sequenceList;
        _worldData.m_sequenceList.clear();

        ITF_ASSERT_CRASH(m_undoRedoMarker <= m_undoRedoSequenceList.size(), "Wrong marker!");
    }

    //------------------------------------------------------------------------------------------------------------------------
    //------------------------------------------------------------------------------------------------------------------------
    //------------------------------------------------------------------------------------------------------------------------
    //------------------------------------------------------------------------------------------------------------------------

    UndoRedoSequence::UndoRedoSequence(const String8& _name)
        : m_name(_name)
    {
    }
    
    UndoRedoSequence::~UndoRedoSequence()
    {
        destroy();
    }

    void UndoRedoSequence::applyAsUndo()
    {
        updateDebugInfo();

        EDITOR->unselectAll();

        for(u32 iStep = 0; iStep < UndoRedoAction::Count; ++iStep)
        {
            const u32 uSeqSize = getActionCount();
            for(i32 i = (uSeqSize - 1); i >= 0; --i)
            {
                UndoRedoAction* pURA = getActionAt(i);
                pURA->applyAsUndo(UndoRedoAction::SequenceStep(iStep));
            }
        }

        DEBUGINFO->clearUndoRedoInfo();
    }

    void UndoRedoSequence::applyAsRedo()
    {
        updateDebugInfo();

        EDITOR->unselectAll();

        for(u32 iStep = 0; iStep < UndoRedoAction::Count; ++iStep)
        {
            const u32 uSeqSize = getActionCount();
            for(u32 i = 0; i < uSeqSize; ++i)
            {
                UndoRedoAction* pURA = getActionAt(i);
                pURA->applyAsRedo(UndoRedoAction::SequenceStep(iStep));
            }
        }

        DEBUGINFO->clearUndoRedoInfo();
    }

    void UndoRedoSequence::destroy()
    {
        const u32 uSize = m_actionList.size();
        for(u32 iList = 0; iList < uSize; ++iList)
        {
            SF_DEL(m_actionList[iList]);
        }
        m_actionList.clear();
    }

    u32 UndoRedoSequence::getMemoryUsage() const
    {
        u32 memUsage = 0;

        const u32 uSeqSize = getActionCount();
        for(u32 i = 0; i < uSeqSize; ++i)
            memUsage += getActionAt(i)->getMemoryUsage();

        return memUsage;
    }

    void UndoRedoSequence::updateDebugInfo()
    {
#if !defined(ITF_FINAL)
        String infos;
        const u32 uSeqSize = getActionCount();
        for(u32 i = 0; i < uSeqSize; ++i)
        {
            infos += getActionAt(i)->getDebugInfo();
            infos += "\r\n";
        }

        DEBUGINFO->setUndoRedoInfo(infos);
#endif // !ITF_FINAL
    }

    UndoRedoSequenceScope::UndoRedoSequenceScope(String8 _name)
        : m_Name(_name)
        , m_Cancelled(bfalse)
    {
        if(UNDOREDO_MANAGER)
            UNDOREDO_MANAGER->beginUndoRedoSequence(m_Name);
    }

    UndoRedoSequenceScope::~UndoRedoSequenceScope()
    {
        if(UNDOREDO_MANAGER)
        {
            if(!m_Cancelled)
                UNDOREDO_MANAGER->validateUndoRedoSequence(m_Name);
            else
                UNDOREDO_MANAGER->cancelUndoRedoSequence(m_Name);
        }
    }

    void UndoRedoSequenceScope::cancel()
    {
        m_Cancelled = btrue;
    }


} //namespace ITF

#endif //!ITF_CONSOLE_FINAL
