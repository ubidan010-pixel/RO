#include "precompiled_editor.h"

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef ITF_CONSOLE_FINAL

#ifndef _ITF_UNDOREDOACTION_H_
#include "editor/UndoRedoAction.h"
#endif //_ITF_UNDOREDOACTION_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_SUBSCENEACTOR_H_
#include "engine/actors/SubSceneActor.h"
#endif //_ITF_SUBSCENEACTOR_H_

#ifndef SERIALIZEROBJECTBINARY_H
#include "core/serializer/ZSerializerObjectBinary.h"
#endif // SERIALIZEROBJECTBINARY_H

#ifndef _ITF_BASEOBJECTFACTORY_H_
#include "engine/factory/baseObjectFactory.h"
#endif //_ITF_BASEOBJECTFACTORY_H_

#ifndef _ITF_EDITOR_H_
#include "editor/editor.h"
#endif //_ITF_EDITOR_H_

#ifndef _ITF_PLUGINGATEWAY_H_
#include "PluginGateway/PluginGateway.h"
#endif //_ITF_PLUGINGATEWAY_H_

namespace ITF
{

    UndoRedoActionSingle::UndoRedoActionSingle( ActionType _type, Pickable* _pPickable )
        : UndoRedoAction(_type)
        , m_pickableIsSelected(_pPickable->isSelected())
        , m_pickableWasHiddenBeforeUndo(_pPickable->getHiddenForEditorFlag())
        , m_pickableWasHiddenBeforeRedo(btrue) // the appropriate value is set later
        , m_isSubSceneActor(_pPickable->DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817)) != NULL)
    {
#if !defined(ITF_FINAL)
        m_debugInfo.setTextFormat("[UndoRedoActionSingle] Friendly: %s | Ref: %u | Ptr: %p", _pPickable->getUserFriendly().cStr(), _pPickable->getRef().getValue(), _pPickable);
#endif // !ITF_FINAL

        SceneObjectPathUtils::getAbsolutePathFromObject(_pPickable, m_pickableAbsolutePath);
    }

    Scene* UndoRedoActionSingle::getContextScene()const
    {
        return SceneObjectPathUtils::getSceneFromAbsolutePath(m_pickableAbsolutePath);
    }

    Pickable* UndoRedoActionSingle::getPickable() const
    {
        return SceneObjectPathUtils::getObjectFromAbsolutePath(m_pickableAbsolutePath);
    }

    //------------------------------------------------------------------------------------------------------------------------
    //------------------------------------------------------------------------------------------------------------------------
    //------------------------------------------------------------------------------------------------------------------------
    //------------------------------------------------------------------------------------------------------------------------

    UndoRedoAction_preChange::UndoRedoAction_preChange( Pickable* _pPickable )
        : UndoRedoActionSingle(preChange, _pPickable)
        , m_pickableContentBeforeRedo(NULL)
    {
        // Save state for undo
        m_pickableContentBeforeUndo = newAlloc(mId_Editor, ArchiveMemory);
        CSerializerObjectBinary serializeBinForUndo(m_pickableContentBeforeUndo);
        _pPickable->Serialize(&serializeBinForUndo, m_isSubSceneActor ? ESerialize_Editor_Save : ESerialize_Data_Save);
    }
    
    UndoRedoAction_preChange::~UndoRedoAction_preChange()
    {
        SF_DEL(m_pickableContentBeforeUndo);
        SF_DEL(m_pickableContentBeforeRedo);
    }
    
    void UndoRedoAction_preChange::applyAsUndo(SequenceStep _step)
    {
        switch(_step)
        {
            case First:
            {
                if(Pickable* pPickable = getPickable())
                {
                    // Save state for redo if undo was never done
                    if(m_pickableContentBeforeRedo == NULL)
                    {
                        m_pickableContentBeforeRedo = newAlloc(mId_Editor, ArchiveMemory);
                        CSerializerObjectBinary serializeBinForRedo(m_pickableContentBeforeRedo);
                        pPickable->Serialize(&serializeBinForRedo, m_isSubSceneActor ? ESerialize_Editor_Save : ESerialize_Data_Save);
                        m_pickableWasHiddenBeforeRedo = pPickable->getHiddenForEditorFlag();
                    }

                    // Apply undo
                    m_pickableContentBeforeUndo->rewindForReading();
                    CSerializerObjectBinary serializeBinForUndo(m_pickableContentBeforeUndo);

                    const String8 prevFriendly = pPickable->getUserFriendly();

                    if(!m_isSubSceneActor)
                    {
                        pPickable->hotReloadFromMem(m_pickableContentBeforeUndo);
                    }
                    else
                    {
                        SubSceneActor* pSSA = static_cast<SubSceneActor*>(pPickable);
                        const bbool wasFlipped = pSSA->startFlipped();

                        pSSA->clearBinds(bfalse);

                        pPickable->Serialize(&serializeBinForUndo, ESerialize_Editor_Load);

                        if(ActorBind* pbind = pSSA->getParentBind())
                        {
                            pbind->setPosOffset(pbind->getInitialPosOffset());
                            pbind->setAngleOffset(pbind->getInitialAngleOffset());

                            Pickable* pParent = NULL;
                            const ObjectPath& parentPath = pbind->m_parent;
                            if(parentPath.getIsAbsolute())
                                pParent = SceneObjectPathUtils::getObjectFromAbsolutePath(parentPath);
                            else
                                pParent = SceneObjectPathUtils::getObjectFromRelativePath(pSSA, parentPath);

                            if(pParent)
                            {
                                ITF_ASSERT_CRASH(pParent->getObjectType() == BaseObject::eActor, "bad type");
                                Actor* pActorParent = (Actor*)pParent;
                                    pActorParent->getChildrenBindHandler().addChild(pSSA); // here the children make the parent to compute curr and init pos info
                            }
                        }

                        if(wasFlipped != pSSA->startFlipped())
                            pSSA->setFlipped(pSSA->startFlipped());

                        pPickable->storeCurrentToInitial();

                        pSSA->updateCurrentWorldDataContentFromLocal();
                    }

                    if(prevFriendly != pPickable->getUserFriendly())
                        PLUGINGATEWAY->onPickableRenamed(pPickable, prevFriendly);

                    PLUGINGATEWAY->onObjectChanged(pPickable);

                    if(m_pickableWasHiddenBeforeUndo)
                        EDITOR->addToHiddenList(pPickable);
                    else
                    {
                        EDITOR->removeFromHiddenList(pPickable);
                        if(m_pickableIsSelected)
                            EDITOR->selectObject(pPickable->getRef(), bfalse);
                    }
                }
                else
                {
                    ITF_ASSERT_MSG(pPickable, "The pickable ID is not referenced in the context scene");

                    // This may happen, for exemple a sequence actor spawn some actors and the user move them
                    // then the sequence actor is moved...  then undo is called => the sequence actor delete and respawn his actors...
                    // at this moment theses actors don't have the same ObjectId then before
                    // so the next undo will try to apply to a non referenced ObjectID...
                }
            }
            break;

            default:
            break;
        }
    }

    void UndoRedoAction_preChange::applyAsRedo(SequenceStep _step)
    {
        switch(_step)
        {
            case First:
            {
                if(Pickable* pPickable = getPickable())
                {
                    // Apply redo
                    m_pickableContentBeforeRedo->rewindForReading();
                    CSerializerObjectBinary serializeBinForRedo(m_pickableContentBeforeRedo);
                    
                    const String8 prevFriendly = pPickable->getUserFriendly();

                    if(!m_isSubSceneActor)
                    {
                        pPickable->hotReloadFromMem(m_pickableContentBeforeRedo);
                    }
                    else
                    {
                        SubSceneActor* pSSA = static_cast<SubSceneActor*>(pPickable);
                        const bbool wasFlipped = pSSA->startFlipped();

                        pSSA->clearBinds(bfalse);

                        pPickable->Serialize(&serializeBinForRedo, ESerialize_Editor_Load);

                        if(ActorBind* pbind = pSSA->getParentBind())
                        {
                            pbind->setPosOffset(pbind->getInitialPosOffset());
                            pbind->setAngleOffset(pbind->getInitialAngleOffset());

                            Pickable* pParent = NULL;
                            const ObjectPath& parentPath = pbind->m_parent;
                            if(parentPath.getIsAbsolute())
                                pParent = SceneObjectPathUtils::getObjectFromAbsolutePath(parentPath);
                            else
                                pParent = SceneObjectPathUtils::getObjectFromRelativePath(pSSA, parentPath);

                            if(pParent)
                            {
                                ITF_ASSERT_CRASH(pParent->getObjectType() == BaseObject::eActor, "bad type");
                                Actor* pActorParent = (Actor*)pParent;
                                    pActorParent->getChildrenBindHandler().addChild(pSSA); // here the children make the parent to compute curr and init pos info
                            }
                        }

                        if(wasFlipped != pSSA->startFlipped())
                            pSSA->setFlipped(pSSA->startFlipped());

                        pPickable->storeCurrentToInitial();

                        pSSA->updateCurrentWorldDataContentFromLocal();
                    }
                    
                    if(prevFriendly != pPickable->getUserFriendly())
                        PLUGINGATEWAY->onPickableRenamed(pPickable, prevFriendly);
                    PLUGINGATEWAY->onObjectChanged(pPickable);

                    if(m_pickableWasHiddenBeforeRedo)
                        EDITOR->addToHiddenList(pPickable);
                    else
                    {
                        EDITOR->removeFromHiddenList(pPickable);
                        if(m_pickableIsSelected)
                            EDITOR->selectObject(pPickable->getRef(), bfalse);
                    }

                    // Swap undo/redo data
                    ArchiveMemory* pTmp = m_pickableContentBeforeUndo;
                    m_pickableContentBeforeUndo = m_pickableContentBeforeRedo;
                    m_pickableContentBeforeUndo = pTmp;
                }
                else
                {
                    ITF_ASSERT_MSG(pPickable, "The pickable ID is not referenced in the context scene");

                    // This may happen, for exemple a sequence actor spawn some actors and the user move them
                    // then the sequence actor is moved...  then undo is called => the sequence actor delete and respawn his actors...
                    // at this moment theses actors don't have the same ObjectId then before
                    // so the next undo will try to apply to a non referenced ObjectID...
                }
            }
            break;

            default:
            break;
        }
    }

    u32 UndoRedoAction_preChange::getMemoryUsage() const
    {
        u32 memUsed = sizeof(UndoRedoAction_preChange);

        if(m_pickableContentBeforeUndo)
            memUsed += sizeof(ArchiveMemory) + m_pickableContentBeforeUndo->getSize();

        if(m_pickableContentBeforeRedo)
            memUsed += sizeof(ArchiveMemory) + m_pickableContentBeforeRedo->getSize();

        return memUsed;
    }
    //------------------------------------------------------------------------------------------------------------------------
    //------------------------------------------------------------------------------------------------------------------------
    //------------------------------------------------------------------------------------------------------------------------
    //------------------------------------------------------------------------------------------------------------------------

#define PRE_DELETE_LABEL "preDelete"

    UndoRedoAction_preDelete::UndoRedoAction_preDelete( Pickable* _pPickable, bbool _delayOnLoaded )
        : UndoRedoActionSingle(preDelete, _pPickable)
        , m_delayOnLoaded(_delayOnLoaded)
        , m_pickableContent(bfalse)
    {
#if !defined(ITF_FINAL)
        m_debugInfo.setTextFormat("[UndoRedoAction_preDelete] Friendly: %s | Ref: %u | Ptr: %p | DelayOnLoaded %u", _pPickable->getUserFriendly().cStr(), _pPickable->getRef().getValue(), _pPickable, u32(_delayOnLoaded));
#endif // !ITF_FINAL

        CSerializerObjectBinary serializeBin(&m_pickableContent);
        serializeBin.SerializeObject(PRE_DELETE_LABEL, _pPickable, &BASEOBJECT_FACTORY->m_objectFactory, ESerialize_Data_Save);
    }

    void UndoRedoAction_preDelete::applyAsUndo(SequenceStep _step)
    {
        switch(_step)
        {
            case First:
            {
                if(Scene* pScene = getContextScene())
                {
                    Pickable* pPickable = getPickable();
                    ITF_ASSERT_MSG(pPickable == NULL, "The pickable ID is still referenced in the context scene");

                    if(!pPickable)
                    {
                        m_pickableContent.rewindForReading();
                        CSerializerObjectBinary serializeBin(&m_pickableContent);
                        serializeBin.SerializeObject(PRE_DELETE_LABEL, pPickable, &BASEOBJECT_FACTORY->m_objectFactory, ESerialize_Data_Load);

                        UndoRedoAction_preDelete::addPickable(pScene, pPickable);
                        m_pickableAdded = pPickable->getRef();

                        if(!m_delayOnLoaded)
                        {
                            pPickable->onLoaded();
                            m_pickableAdded.invalidate();
                            PLUGINGATEWAY->onObjectCreated(pPickable);
                        }
                    }
                }
                else
                {
                    ITF_ASSERT_MSG(pScene, "The context scene no longer exists");
                }
            }
            break;

            case Second:
            {
                if(m_delayOnLoaded)
                {
                    if(Pickable* pPickable = static_cast<Pickable*>(m_pickableAdded.getObject()))
                    {
                        pPickable->onLoaded();
                        m_pickableAdded.invalidate();
                        PLUGINGATEWAY->onObjectCreated(pPickable);
                    }
                    else
                    {
                        ITF_ASSERT(0);
                    }
                }
            }
            break;

            default:
            break;
        }
        
    }

    void UndoRedoAction_preDelete::applyAsRedo(SequenceStep _step)
    {
        switch(_step)
        {
            case First:
            {
                if(Scene* pScene = getContextScene())
                {
                    if(Pickable* pPickable = getPickable())
                    {
                        UndoRedoAction_postAdd::deletePickable(pScene, pPickable);
                    }
                    else
                    {
                        ITF_ASSERT_MSG(pPickable, "The pickable ID is not referenced in the context scene");
                    }
                }
                else
                {
                    ITF_ASSERT_MSG(pScene, "The context scene no longer exists");
                }
            }
            break;

            default:
            break;
        }
    }

    void UndoRedoAction_preDelete::addPickable( Scene* _pScene, Pickable* _pPickable )
    {
        ITF_ASSERT(_pScene && _pPickable);
        if(!_pScene || !_pPickable)
            return;

        _pScene->registerPickable(_pPickable);
        _pScene->flushPending();

        if(SubSceneActor* pSSA = _pPickable->DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817)))
        {
            World* pWorld = _pScene->getWorld();
            if(!pSSA->getEmbedScene())
                pSSA->loadExternalScene();
            pWorld->addScene(pSSA->getSubScene());
        }

        EDITOR->setSceneAsModified(_pScene);
    }

    u32 UndoRedoAction_preDelete::getMemoryUsage() const
    {
        u32 memUsed = sizeof(UndoRedoAction_preDelete);
        
        memUsed += m_pickableContent.getSize();

        return memUsed;
    }

    //------------------------------------------------------------------------------------------------------------------------
    //------------------------------------------------------------------------------------------------------------------------
    //------------------------------------------------------------------------------------------------------------------------
    //------------------------------------------------------------------------------------------------------------------------

#define POST_ADD_LABEL "postAdd"

    UndoRedoAction_postAdd::UndoRedoAction_postAdd( Pickable* _pPickable, bbool _delayOnLoaded )
        : UndoRedoActionSingle(postAdd, _pPickable)
        , m_contentSaved(bfalse)
        , m_delayOnLoaded(_delayOnLoaded)
        , m_pickableContent(bfalse)
    {
#if !defined(ITF_FINAL)
        m_debugInfo.setTextFormat("[UndoRedoAction_postAdd] Friendly: %s | Ref: %u | Ptr: %p | DelayOnLoaded %u", _pPickable->getUserFriendly().cStr(), _pPickable->getRef().getValue(), _pPickable, u32(_delayOnLoaded));
#endif // !ITF_FINAL
    }

    void UndoRedoAction_postAdd::applyAsUndo(SequenceStep _step)
    {
        switch(_step)
        {
            case First:
            {
                if(Scene* pScene = getContextScene())
                {
                    if(Pickable* pPickable = getPickable())
                    {
                        // Save data for redo
                        if(!m_contentSaved)
                        {
                            CSerializerObjectBinary serializeBin(&m_pickableContent);
                            serializeBin.SerializeObject(POST_ADD_LABEL, pPickable, &BASEOBJECT_FACTORY->m_objectFactory, ESerialize_Data_Save);
                            m_contentSaved = btrue;
                        }

                        UndoRedoAction_postAdd::deletePickable(pScene, pPickable);
                    }
                    else
                    {
                        ITF_ASSERT_MSG(pPickable, "The pickable ID is not referenced in the context scene");
                    }
                }
                else
                {
                    ITF_ASSERT_MSG(pScene, "The context scene no longer exists");
                }
            }
            break;

            default:
            break;
        }
    }

    void UndoRedoAction_postAdd::applyAsRedo(SequenceStep _step)
    {
        switch(_step)
        {
            case First:
            {
                if(Scene* pScene = getContextScene())
                {
                    Pickable* pPickable = getPickable();
                    ITF_ASSERT_MSG(pPickable == NULL, "The pickable ID is still referenced in the context scene");
                    
                    if(!pPickable)
                    {
                        m_pickableContent.rewindForReading();
                        CSerializerObjectBinary serializeBin(&m_pickableContent);
                        serializeBin.SerializeObject(POST_ADD_LABEL, pPickable, &BASEOBJECT_FACTORY->m_objectFactory, ESerialize_Data_Load);

                        UndoRedoAction_preDelete::addPickable(pScene, pPickable);
                        m_pickableAdded = pPickable->getRef();

                        if(!m_delayOnLoaded)
                        {
                            pPickable->onLoaded();
                            m_pickableAdded.invalidate();
                            PLUGINGATEWAY->onObjectCreated(pPickable);
                        }
                    }
                }
                else
                {
                    ITF_ASSERT_MSG(pScene, "The context scene no longer exists");
                }
            }
            break;

            case Second:
            {
                if(m_delayOnLoaded)
                {
                    if(Pickable* pPickable = static_cast<Pickable*>(m_pickableAdded.getObject()))
                    {
                        pPickable->onLoaded();
                        m_pickableAdded.invalidate();
                        PLUGINGATEWAY->onObjectCreated(pPickable);
                    }
                    else
                    {
                        ITF_ASSERT(0);
                    }
                }
            }
            break;

            default:
            break;
        }
    }

    void UndoRedoAction_postAdd::deletePickable(Scene* _pScene, Pickable* _pPickable )
    {
        ITF_ASSERT(_pScene && _pPickable);
        if(!_pScene || !_pPickable)
            return;

        if(const SafeArray<PickingShape*>* pShapes = EDITOR->getPickingShapesForObject(_pPickable->getRef()))
        {
            ITF_VERIFY( EDITOR->deleteGeneric(*pShapes) );
        }
        else
        {
            ITF_ASSERT(pShapes);
        }

        _pScene->flushPending();
    }

    u32 UndoRedoAction_postAdd::getMemoryUsage() const
    {
        u32 memUsed = sizeof(UndoRedoAction_postAdd);
        
        memUsed += m_pickableContent.getSize();

        return memUsed;
    }

    //------------------------------------------------------------------------------------------------------------------------
    //------------------------------------------------------------------------------------------------------------------------
    //------------------------------------------------------------------------------------------------------------------------
    //------------------------------------------------------------------------------------------------------------------------

    UndoRedoAction_sceneMigration::UndoRedoAction_sceneMigration(Pickable* _pPickable, const ObjectPath& _previousAbsolutePath)
        : UndoRedoActionSingle(sceneMigration, _pPickable)
        , m_SrcScenePath(_previousAbsolutePath)
    {
        SceneObjectPathUtils::getAbsolutePathFromObject(_pPickable, m_DstScenePath);

#if !defined(ITF_FINAL)
        ITF_STDSTRING src, dst;
        m_SrcScenePath.toString(src);
        m_DstScenePath.toString(dst);
        m_debugInfo.setTextFormat("[UndoRedoAction_sceneMigration] Friendly: %s | Ref: %u | Ptr: %p", _pPickable->getUserFriendly().cStr(), _pPickable->getRef().getValue(), _pPickable);
        m_debugInfo += " | Src: ";
        m_debugInfo += src.c_str();
        m_debugInfo += " | Dst: ";
        m_debugInfo += dst.c_str();
#endif // !ITF_FINAL
    }

    void UndoRedoAction_sceneMigration::applyAsUndo(SequenceStep _step)
    {
        switch(_step)
        {
            case First:
            {
                revertMigration();
            }
            break;

            default:
            break;
        }
    }

    void UndoRedoAction_sceneMigration::applyAsRedo(SequenceStep _step)
    {
        switch(_step)
        {
            case First:
            {
                revertMigration();
            }
            break;

            default:
            break;
        }
    }

    void UndoRedoAction_sceneMigration::revertMigration()
    {
        Scene* pSrcScene = SceneObjectPathUtils::getSceneFromAbsolutePath(m_SrcScenePath);
        Scene* pDstScene = SceneObjectPathUtils::getSceneFromAbsolutePath(m_DstScenePath);

        if(pSrcScene && pDstScene)
        {
            if(Pickable* pPickable = getPickable())
            {
                Vec3d prevInitialWorldPos = pPickable->getWorldInitialPos();

                pDstScene->unregisterPickable(pPickable, bfalse);
                pDstScene->flushPending();
                pSrcScene->registerPickable(pPickable);
                pSrcScene->flushPending();

                pPickable->setWorldInitialPos(prevInitialWorldPos);
                pPickable->setPos(pPickable->getWorldInitialPos());
                pPickable->setWorldInitialRot(pPickable->getAngle());
                pPickable->setWorldInitialScale(pPickable->getScale());

                EDITOR->addPickableToRefresh(pPickable->getRef());
                
                PLUGINGATEWAY->onPickableSceneChanged(pPickable, pDstScene, pSrcScene);

                // Swap data so next call to revert will do the inverse
                ObjectPath swap = m_SrcScenePath;
                m_SrcScenePath  = m_DstScenePath;
                m_DstScenePath  = swap;
                
                SceneObjectPathUtils::getAbsolutePathFromObject(pPickable, m_pickableAbsolutePath);
            }
            else
            {
                ITF_ASSERT_MSG(0, "Can't find pickable to migrate");
            }
        }
        else
        {
            ITF_ASSERT_MSG(0, "Can't find scene for migrate");
        }
    }

    u32 UndoRedoAction_sceneMigration::getMemoryUsage()const
    {
        return sizeof(UndoRedoAction_sceneMigration);
    }

    //------------------------------------------------------------------------------------------------------------------------
    //------------------------------------------------------------------------------------------------------------------------
    //------------------------------------------------------------------------------------------------------------------------
    //------------------------------------------------------------------------------------------------------------------------

    UndoRedoActionTemplate::UndoRedoActionTemplate(TemplateObj* _template, ObjectRef _actorRef, processObjectRef _callback)
        : UndoRedoAction(preTemplate)
        , m_templateContentBeforeRedo(NULL)
        , m_pTemplate(_template)
        , m_actorRef(_actorRef)
        , m_callback(_callback)
    {
#if !defined(ITF_FINAL)
        m_debugInfo.setTextFormat("[UndoRedoActionTemplate] TemplateObj ptr: 0x%x | ActorRef: %u", _template, _actorRef.getValue());
#endif // !ITF_FINAL

        // Save state for undo

        m_templateContentBeforeUndo = newAlloc(mId_Editor, ArchiveMemory);
        CSerializerObjectBinary serializeBinForUndo(m_templateContentBeforeUndo);
        
        _template->Serialize(&serializeBinForUndo, (ESerializeFlags)(ESerialize_Data_Save|ESerialize_Editor_Save));

    }

    UndoRedoActionTemplate::~UndoRedoActionTemplate()
    {
        SF_DEL(m_templateContentBeforeUndo);
        SF_DEL(m_templateContentBeforeRedo);
    }


    void UndoRedoActionTemplate::applyAsUndo(SequenceStep _step)
    {
        switch(_step)
        {
        case First:
            {
                // Save state for redo if undo was never done
                if(m_templateContentBeforeRedo == NULL)
                {
                    m_templateContentBeforeRedo = newAlloc(mId_Editor, ArchiveMemory);
                    CSerializerObjectBinary serializeBinForRedo(m_templateContentBeforeRedo);
                    m_pTemplate->Serialize(&serializeBinForRedo, (ESerializeFlags)(ESerialize_Data_Save|ESerialize_Editor_Save));
                }

                // Apply undo
                m_templateContentBeforeUndo->rewindForReading();
                CSerializerObjectBinary serializeBinForUndo(m_templateContentBeforeUndo);

                m_pTemplate->Serialize(&serializeBinForUndo, (ESerializeFlags)(ESerialize_Data_Load|ESerialize_Editor_Load));
                if (m_callback)
                {
                    m_callback(m_actorRef);
                }
            }
            break;

        default:
            break;
        }
    }

    void UndoRedoActionTemplate::applyAsRedo(SequenceStep _step)
    {
        switch(_step)
        {
        case First:
            {
                // Apply redo
                m_templateContentBeforeRedo->rewindForReading();
                CSerializerObjectBinary serializeBinForRedo(m_templateContentBeforeRedo); 

                m_pTemplate->Serialize(&serializeBinForRedo, (ESerializeFlags)(ESerialize_Data_Load|ESerialize_Editor_Load));
                if (m_callback)
                {
                    m_callback(m_actorRef);
                }

                // Swap undo/redo data
                ArchiveMemory* pTmp = m_templateContentBeforeUndo;
                m_templateContentBeforeUndo = m_templateContentBeforeRedo;
                m_templateContentBeforeUndo = pTmp;
            }
            break;

        default:
            break;
        }
    }

    u32 UndoRedoActionTemplate::getMemoryUsage() const
    {
        u32 memUsed = sizeof(UndoRedoActionTemplate);

        if(m_templateContentBeforeUndo)
            memUsed += sizeof(ArchiveMemory) + m_templateContentBeforeUndo->getSize();

        if(m_templateContentBeforeRedo)
            memUsed += sizeof(ArchiveMemory) + m_templateContentBeforeRedo->getSize();

        return memUsed;
    }

} //namespace ITF

#endif // !ITF_CONSOLE_FINAL