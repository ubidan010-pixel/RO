#include "precompiled_editor.h"

#ifndef _ITF_EDITOR_PLUGIN_H_
#include "editor/EditorPlugin.h"
#endif //_ITF_EDITOR_PLUGIN_H_

#ifndef _ITF_PICKINGSHAPE_H_
#include "editor/PickingShape.h"
#endif //_ITF_PICKINGSHAPE_H_

#ifndef _ITF_SELECTION_H_
#include "editor/selection.h"
#endif //_ITF_SELECTION_H_

#ifndef _ITF_EDITOR_H_
#include "editor/editor.h"
#endif //_ITF_EDITOR_H_

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_UNDOREDOMANAGER_H_
#include "editor/UndoRedoManager.h"
#endif //_ITF_UNDOREDOMANAGER_H_

#ifndef _ITF_EDITORPOLYLINE_H_
#include "tools/plugins/Plugin_FriezeEditor/Plugin_FriezeEditor.h"
#endif //_ITF_EDITORPOLYLINE_H_

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_SUBSCENEACTOR_H_
#include "engine/actors/SubSceneActor.h"
#endif //_ITF_SUBSCENEACTOR_H_

#ifndef SERIALIZEROBJECTBINARYMEM_H
#include "core/serializer/ZSerializerObjectBinaryMem.h"
#endif // SERIALIZEROBJECTBINARYMEM_H

#ifndef _ITF_PLUGINGATEWAY_H_
#include "PluginGateway/PluginGateway.h"
#endif //_ITF_PLUGINGATEWAY_H_

#ifndef _ITF_BASEOBJECTFACTORY_H_
#include "engine/factory/baseObjectFactory.h"
#endif //_ITF_BASEOBJECTFACTORY_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

namespace ITF
{
    EditorPlugin::EditorPlugin()
    {
    }

    EditorPlugin::~EditorPlugin()
    {
    }

    PickingShapePluginData* EditorPlugin::createPickingShapeData( PickingShape * _shape )
    {
        if(_shape && _shape->getOwnerPtr() && _shape->getOwnerPtr()->getObjectType()==BaseObject::eSelection)
            return newAlloc(mId_Editor, PickingShapeData(_shape, this));
        return NULL;
    }

    void EditorPlugin::updateNewSubSceneInstanceFromExistingInstance(SubSceneActor* pSSA)const
    {
        // Only manage instances
        if(pSSA->getEmbedScene())
            return;

        Scene* sceneToOverwrite = pSSA->getSubScene();

        SafeArray<Scene*> sceneInstanceList;
        findInstanceScenes(sceneToOverwrite, sceneInstanceList);

        if(sceneInstanceList.size())
        {
            Scene* pInstanceSource = sceneInstanceList[0];

            // Data copy
            {
                ArchiveMemory sceneContent;

                CSerializerObjectBinary serializeIN(&sceneContent);
                serializeIN.SerializeObject("CONTENT", pInstanceSource, ESerialize_Data_Save);

                sceneToOverwrite->clear();
                sceneContent.rewindForReading();

                CSerializerObjectBinary serializeOUT(&sceneContent);
                serializeOUT.SerializeObject("CONTENT", sceneToOverwrite, ESerialize_Data_Load);
            }

            const PickableList& pickableList = sceneToOverwrite->getPickableList();
            const u32 uSize = pickableList.size();
            for(u32 iPickable = 0; iPickable < uSize; ++iPickable)
                SCENE_MANAGER->broadcastObjectAdded(sceneToOverwrite, pickableList[iPickable]);
            
            World* pWorld = sceneToOverwrite->getWorld();
            SafeArray<BaseObject*> resolvedRefs;
            ID_SERVER->getObjectListNoNull(sceneToOverwrite->getSubSceneActors(), resolvedRefs);
            for(u32 i = 0; i < resolvedRefs.size(); ++i)
            {
                SubSceneActor* pSSA = static_cast<SubSceneActor*>(resolvedRefs[i]);
                pWorld->addScene(pSSA->getSubScene());
            }

            sceneToOverwrite->onLoadReposition();
            sceneToOverwrite->onSceneLoaded();

            // Scan actors to detect template changes
            SafeArray<Scene*> sceneToScan;
            sceneToScan.push_back(sceneToOverwrite);
            Pickable* objPathRef_DST = NULL;
            Pickable* objPathRef_SRC = NULL;
            while(sceneToScan.size())
            {
                Scene* pScene = sceneToScan[0];
                sceneToScan.eraseKeepOrder(0);

                // Recursive feed the while monster
                ObjectRefList ssas = pScene->getSubSceneActors();
                for(u32 iSSA = 0; iSSA < ssas.size(); ++iSSA)
                    if(BaseObject* pBO = ssas[iSSA].getObject())
                        if(SubSceneActor* pSSA = pBO->DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817)))
                            if(Scene* pSubScene = pSSA->getSubScene())
                                sceneToScan.push_back(pSubScene);

                const PickableList& content = pScene->getPickableList();
                for(u32 iC = 0; iC < content.size(); ++iC)
                {
                    Pickable* pPickableToOverwrite = content[iC];

                    if(!objPathRef_DST)
                    {
                        objPathRef_DST = pPickableToOverwrite;
                        objPathRef_SRC = pInstanceSource->getPickableList()[0];
                    }

                    ObjectPath relativePath;
                    SceneObjectPathUtils::getRelativePathFromObject(objPathRef_DST, pPickableToOverwrite, relativePath);

                    Pickable* pPickableSource = SceneObjectPathUtils::getObjectFromRelativePath(objPathRef_SRC, relativePath);

                    ITF_ASSERT_CRASH(pPickableSource && pPickableSource != pPickableToOverwrite, "Incoherent");

                    const Vec3d worldCurPosOffset   = pPickableSource->getPos() - pPickableSource->getWorldInitialPos();
                    const Vec3d newWorldPos         = pPickableToOverwrite->getWorldInitialPos() + worldCurPosOffset;
                    pPickableToOverwrite->setPos(newWorldPos);

                    const f32 worldCurAngleDelta    = pPickableSource->getAngle() - pPickableSource->getWorldInitialRot();
                    const f32 newWorldAngle         = pPickableToOverwrite->getWorldInitialRot() + worldCurAngleDelta;
                    pPickableToOverwrite->setAngle(newWorldAngle);

                    const Vec2d worldCurScaleDelta  = pPickableSource->getScale() - pPickableSource->getWorldInitialScale();
                    const Vec2d newWorldScale       = pPickableToOverwrite->getWorldInitialScale() + worldCurScaleDelta;
                    pPickableToOverwrite->setScale(newWorldScale);
                }
            }
            
            EDITOR->setSceneAsModified(sceneToOverwrite);
        }
    }

    void EditorPlugin::fullInstancesReload( const Scene* _modifiedScene ) const
    {
        SafeArray<Scene*> sceneInstanceList;
        findInstanceScenes(_modifiedScene, sceneInstanceList);

        if(sceneInstanceList.size())
        {
            ArchiveMemory sceneContent(bfalse);
            {
                Scene* pNotModifiableScene = const_cast<Scene*>(_modifiedScene); // const casting used because the serialization shouldn' modify the scene while using flag SAVE
                CSerializerObjectBinary serializeIN(&sceneContent);
                serializeIN.SerializeObject("CONTENT", pNotModifiableScene, ESerialize_Data_Save);
            }

            for(u32 iInstance = 0; iInstance < sceneInstanceList.size(); ++iInstance)
            {
                Scene* pInstance = sceneInstanceList[iInstance];

#ifdef DEVELOPER_OLIV
                SubSceneActor* pAncestor = pInstance->getSubSceneActorHolder();
                while(pAncestor && pAncestor->getEmbedScene())
                    pAncestor = pAncestor->getScene()->getSubSceneActorHolder();

                ITF_ASSERT(pAncestor);
                ObjectPath path;
                SceneObjectPathUtils::getAbsolutePathFromObject(pAncestor, path);
                ITF_STDSTRING str;
                path.toString(str);
                LOG_OLIV("FULL INSTANCE RELOAD: '%s'", str.c_str());
#endif
                pInstance->clear();

                sceneContent.rewindForReading();
                CSerializerObjectBinary serializeOUT(&sceneContent);
                serializeOUT.SerializeObject("CONTENT", pInstance, ESerialize_Data_Load);

                const PickableList& pickableList = pInstance->getPickableList();
                const u32 uSize = pickableList.size();
                for(u32 iPickable = 0; iPickable < uSize; ++iPickable)
                    SCENE_MANAGER->broadcastObjectAdded(pInstance, pickableList[iPickable]);

                World* pWorld = pInstance->getWorld();
                SafeArray<BaseObject*> resolvedRefs;
                ID_SERVER->getObjectListNoNull(pInstance->getSubSceneActors(), resolvedRefs);
                for(u32 i = 0; i < resolvedRefs.size(); ++i)
                {
                    SubSceneActor* pSSA = static_cast<SubSceneActor*>(resolvedRefs[i]);
                    pWorld->addScene(pSSA->getSubScene());
                }

                pInstance->onLoadReposition();
                pInstance->onSceneLoaded();

                EDITOR->setSceneAsModified(pInstance);
            }
        }
    }

    void EditorPlugin::onObjectCreated( BaseObject* _pObject )
    {
        if (!_pObject)
            return;

        switch(_pObject->getObjectType())
        {
            case BaseObject::eFrise:
            case BaseObject::eActor:
                {
                    Pickable* pObjSource = static_cast<Pickable*>(_pObject);

                    fullInstancesReload(pObjSource->getScene());

                    if(SubSceneActor* pSSA = _pObject->DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817)))
                        updateNewSubSceneInstanceFromExistingInstance(pSSA);
                }
                break;

            default:
                break;
        }
    }

    void EditorPlugin::onObjectDeleted( BaseObject* _pObject )
    {
        if (!_pObject)
            return;

        #if defined(ITF_SUPPORT_EDITOR)
        if (EDITOR)
            EDITOR->onObjectDeleted(_pObject);
        #endif


        switch(_pObject->getObjectType())
        {
            case BaseObject::eFrise:
            case BaseObject::eActor:
                {
                    Pickable* pObjSource = static_cast<Pickable*>(_pObject);
                    PickableList instanceList;
                    EditorPlugin::findInstances(pObjSource, instanceList);

                    for(u32 iInstance = 0; iInstance < instanceList.size(); ++iInstance)
                    {
                        Pickable* pInstance = instanceList[iInstance];
                        Scene* pScene       = pInstance->getScene();

                        pScene->unregisterPickable(pInstance);
                        EDITOR->setSceneAsModified(pScene);
                    }
                }
                break;

            default:
                break;
        }
    }

    void EditorPlugin::onObjectChanged( BaseObject* _pObject )
    {
        if (!_pObject)
            return;

        switch(_pObject->getObjectType())
        {
            case BaseObject::eFrise:
            case BaseObject::eActor:
                {
                    Pickable* pObjSource = static_cast<Pickable*>(_pObject);
                    fullInstancesReload(pObjSource->getScene());
                }
                break;

            default:
                break;
        }
    }

    void EditorPlugin::onPickableRenamed( Pickable* _pObj, const String8& _previousUserFriendly )
    {
        if (!_pObj)
            return;

        if(Scene* pScene = _pObj->getScene())
        {
            ITF_VECTOR<String> platforms;
            const u32 uResultCount = pScene->getFilteredPlatformsForObject(_previousUserFriendly, platforms);

            for(u32 i = 0; i < uResultCount; ++i)
            {
                const String& platform = platforms[i];
                pScene->unfilterObject(_previousUserFriendly, platform);
                pScene->filterObject(_pObj->getUserFriendly(), platform);
            }
        }

        switch(_pObj->getObjectType())
        {
            case BaseObject::eFrise:
            case BaseObject::eActor:
                    fullInstancesReload(_pObj->getScene());
                break;

            default:
                break;
        }
    }

    void EditorPlugin::onPickableSceneChanged( Pickable* _pPickable, Scene* _src, Scene* _dst )
    {
        // TODO fix when transfering obj between shared instance
        fullInstancesReload(_src);
        fullInstancesReload(_dst);
    }

    void EditorPlugin::findInstances( Pickable* _object, PickableList& results )const
    {
        SafeArray<Scene*> scenes;
        findInstanceScenes(_object->getScene(), scenes);

        for(u32 iScene = 0; iScene < scenes.size(); ++iScene)
        {
            if(Pickable* pObj = static_cast<Pickable*>(WorldManager::getObject(scenes[iScene], _object->getObjectId())))
            {
                results.push_back(pObj);
            }
            else
            {
                ITF_ASSERT_CRASH(0, "Instance mismatch, can't find the matching subobject instance");
            }
        }
    }

    void EditorPlugin::findInstanceScenes( const Scene* _scene, SafeArray<Scene*>& results ) const
    {
#if 0   // DISABLE INSTANCE SYSTEM
        if(const SubSceneActor* pSSA = _scene->getSubSceneActorHolder())
        {
            bbool bInsideInstanceScene = bfalse;
            const SubSceneActor* pSSA_FirstInstanceAncestor = pSSA;
            Pickable* pObjReference = NULL;
            while(pSSA_FirstInstanceAncestor)
            {
                if(!pSSA_FirstInstanceAncestor->getEmbedScene())
                {
                    if(Scene* pSubScene = pSSA_FirstInstanceAncestor->getSubScene())
                        if(!pSubScene->isEmpty())
                            pObjReference = pSubScene->getPickableList()[0];
                    ITF_ASSERT_MSG(pObjReference, "Trying to modify an empty group");

                    bInsideInstanceScene = btrue;
                    break;
                }

                pSSA_FirstInstanceAncestor = pSSA_FirstInstanceAncestor->getScene()->getSubSceneActorHolder();
            }

            if(bInsideInstanceScene && pObjReference)
            {
                if(const World* pWorld = _scene->getWorld())
                {
                    // Get the relative path from the scene to it's first instance ancestor
                    ObjectPath relativePath;
                    if(pSSA->getEmbedScene())
                    {
                        ITF_VERIFY(SceneObjectPathUtils::getRelativePathFromObject(pObjReference, pSSA, relativePath));
                    }

                    // The root can't be an instance, so just manage subscenes
                    ObjectRefList potential = pWorld->getRootScene()->getSubSceneActors();

                    // Remove non-instance subscenes
                    for(i32 i = 0; i < i32(potential.size()); ++i)
                    {
                        if(const SubSceneActor* pSSA_Current = static_cast<const SubSceneActor*>(potential[i].getObject()))
                        {
                            if(pSSA_Current->getEmbedScene())
                            {
                                potential.eraseNoOrder(i);
                                --i;
                            }
                        }
                    }

                    // Recursive search amonst instances
                    while(potential.size())
                    {
                        ObjectRef ssaRef = potential[0];
                        potential.eraseNoOrder(0);

                        if(const SubSceneActor* pSSA_Current = static_cast<const SubSceneActor*>(ssaRef.getObject()))
                        {
                            if(Scene* pSubScene = pSSA_Current->getSubScene())
                            {
                                if(_scene == pSubScene) // skip the source scene, we are searching for others instances
                                    continue;

                                bbool instanceFound = bfalse;
                                bbool scanSubContent = btrue;

                                 // Group case
                                if(pSSA_Current->getEmbedScene() && pSSA->getEmbedScene())
                                {
                                    // Find the first instance ancestor of the current scene
                                    const SubSceneActor* pSSA_FirstInstanceAncestor_Current = pSSA_Current;
                                    while(pSSA_FirstInstanceAncestor_Current)
                                    {
                                        if(!pSSA_FirstInstanceAncestor_Current->getEmbedScene()) // when found
                                        {
                                            if(pSSA_FirstInstanceAncestor_Current != pSSA_FirstInstanceAncestor) // if it's not the same instance ancestor
                                            {
                                                Pickable* pRef = NULL;
                                                if(Scene* pSubScene = pSSA_FirstInstanceAncestor_Current->getSubScene())
                                                    if(!pSubScene->isEmpty())
                                                        pRef = pSubScene->getPickableList()[0];
												ITF_ASSERT_MSG(pRef, "Trying to modify an empty group");

												ObjectPath relativePath_Current;

												if(pRef)
												{
													ITF_VERIFY(SceneObjectPathUtils::getRelativePathFromObject(pRef, pSSA_Current, relativePath_Current));
												}

                                                instanceFound = pRef && (relativePath == relativePath_Current);
                                            }
                                            else
                                            {
                                                scanSubContent = bfalse;
                                            }

                                            break;
                                        }

                                        pSSA_FirstInstanceAncestor_Current = pSSA_FirstInstanceAncestor_Current->getScene()->getSubSceneActorHolder();
                                    }
                                }

                                // Instance case
                                if(!pSSA_Current->getEmbedScene() && !pSSA->getEmbedScene()) 
                                {
                                    instanceFound = (_scene->getPath() == pSubScene->getPath());
                                }

                                if(instanceFound) // an instance is found, stop recursive branch here: recursive sceen inclusion isn't allowed
                                    results.push_back(pSubScene);

                                if(scanSubContent) // feed the recursive monster
                                    potential.mergeArray(pSubScene->getSubSceneActors());
                            }
                            else
                            {
                                ITF_ASSERT(0);
                            }
                        }
                    }
                }
            }
        }
#endif // 0
    }

    EditorPlugin::PickingShapeData::PickingShapeData( PickingShape* _shape, Plugin * _plugin)
        : PickingShapeData_RotationScale(_shape, _plugin)
    {
    }

    void EditorPlugin::PickingShapeData::update()
    {
        PickingShapeData_RotationScale::update();

        const Selection* pObj = static_cast<const Selection*>(m_pickingShape->getOwnerPtr());
        static_cast<PickingShape_Disc*>(m_pickingShape)->setPos(pObj->getAnchorPos());
    }

    void EditorPlugin::PickingShapeData::draw()
    {
        if(EDITOR->getSelectionSize() > 1)
        {
            PickingShapeData_RotationScale::draw();

            EDITOR->getIconStock().drawIcon(Editor_IconStock::Icon_CameraPan, static_cast<PickingShape_Disc*>(m_pickingShape)->getPos(), PickingShape::m_disc_picking_tolerance / 2.0f);
        }
    }

    void EditorPlugin::PickingShapeData::scale( const Vec2d& _newScale )
    {
        Selection* pSelectionObj = static_cast<Selection*>(m_pickingShape->getOwnerPtr());
        Vec2d deltaScale = _newScale - pSelectionObj->getScale();
        const Vec3d& scaleCenter = pSelectionObj->getAnchorPos();

        if ( deltaScale != Vec2d::Zero )
        {
            const f32 xCoef = _newScale.m_x / pSelectionObj->getScale().m_x;
            const f32 yCoef = _newScale.m_y / pSelectionObj->getScale().m_y;
            
            pSelectionObj->scale(deltaScale, btrue);
            
            Vec2d currentHelperPos2d;
            ITF_VERIFY(getCurrentHelperUsedPos(currentHelperPos2d));

            for(u32 iShape = 0; iShape < EDITOR->getSelectionSize(); ++iShape)
            {
                PickingShape* pShape = EDITOR->getSelectionAt(iShape);

                if(pShape->isSubAnchor())
                    continue;

                Pickable* pObj       = static_cast<Pickable*>(pShape->getOwnerPtr());
                const bbool editInit = EDITOR->isAllowStorePickableInitData(pObj);

                switch(pObj->getObjectType())
                {
                case BaseObject::eActor:
                    {
                        Actor* pActor = static_cast<Actor*>(pObj);
                        Vec2d scaleToApply;
                        if ( pActor->getTemplate() && pActor->getTemplate()->getScaleForced() != Vec2d::Zero )
                            scaleToApply = pActor->getTemplate()->getScaleForced();
                        else
                        {
                            scaleToApply = pObj->getScale();
                            scaleToApply.m_x *= xCoef;
                            scaleToApply.m_y *= yCoef;
                        }

                        pObj->setScale(scaleToApply);
                        if(editInit)
                            pObj->setWorldInitialScale(scaleToApply);
                    }
                    break;

                case BaseObject::eFrise:
                    currentHelperPos2d = FriezeEditor::PickingShapeData::scale(static_cast<Frise*>(pObj), this, _newScale, m_prevHelperPos, scaleCenter);
                    break;

                default:
                    break;
                }

                const Vec3d dirToAnchor = pObj->getAnchorPos() - scaleCenter;
                Vec3d newDirToAnchor = dirToAnchor;
                newDirToAnchor.m_x *= xCoef;
                newDirToAnchor.m_y *= yCoef;

                const Vec3d deltaAnchor = newDirToAnchor - dirToAnchor;

                pObj->setPos(pObj->getPos() + deltaAnchor);
                if(editInit)
                    pObj->setLocalInitialPos(pObj->getLocalPos());
            }
            m_prevHelperPos = currentHelperPos2d;
        }
    }

    void EditorPlugin::PickingShapeData::preChange()
    {
        UndoRedoSequenceScope scope("selection prechange");

        ObjectRefList refs = EDITOR->getEditorSelectionAsObjRefList();
        SafeArray<BaseObject*> resolvedRefs;
        ID_SERVER->getObjectListNoNull(refs, resolvedRefs);

        for(u32 i = 0; i < resolvedRefs.size(); ++i)
            UNDOREDO_MANAGER->preChange(static_cast<Pickable*>(resolvedRefs[i]));
       
    }

    void EditorPlugin::PickingShapeData::postChange()
    {
        ObjectRefList refs = EDITOR->getEditorSelectionAsObjRefList();
        SafeArray<BaseObject*> resolvedRefs;
        ID_SERVER->getObjectListNoNull(refs, resolvedRefs);

        for(u32 i = 0; i < resolvedRefs.size(); ++i)
        {
            Pickable* pObj = static_cast<Pickable*>(resolvedRefs[i]);
            pObj->onEditorMove(EDITOR->isAllowStorePickableInitData(pObj));
        }
    }

    void EditorPlugin::PickingShapeData::finalChange()
    {
        ObjectRefList refs = EDITOR->getEditorSelectionAsObjRefList();
        SafeArray<BaseObject*> resolvedRefs;
        ID_SERVER->getObjectListNoNull(refs, resolvedRefs);

        for(u32 i = 0; i < resolvedRefs.size(); ++i)
        {
            Pickable* pObj = static_cast<Pickable*>(resolvedRefs[i]);
            PLUGINGATEWAY->onObjectChanged(pObj);
        }
    }

    void EditorPlugin::PickingShapeData::handleMousePressed( const Vec2d & _mousePos )
    {
        PickingShapeData_RotationScale::handleMousePressed(_mousePos);

        Selection* pObj = static_cast<Selection*>(m_pickingShape->getOwnerPtr());
        Vec3d pos = pObj->getAnchorPos();

        switch(m_state)
        {
        case PickingShapeData_RotationScale::State_ClickedGreenTriangle:
        case PickingShapeData_RotationScale::State_ClickedScaleRotationXBar:
        case PickingShapeData_RotationScale::State_ClickedScaleRotationYBar:
            EDITOR->startRotation(&pos);
            break;

        default:
            break;
        }
    }

} // namespace ITF