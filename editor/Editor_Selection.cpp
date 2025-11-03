#include "precompiled_editor.h"

#ifndef _ITF_WARNINGMANAGER_H_
#include "engine/common/WarningManager.h"
#endif // _ITF_WARNINGMANAGER_H_

#ifndef ITF_CONSOLE_FINAL

#ifndef _ITF_EDITOR_H_
#include "editor/Editor.h"
#endif //_ITF_EDITOR_H_

#ifndef _ITF_PICKING_H_
#include "editor/Picking.h"
#endif //_ITF_PICKING_H_

#ifndef _ITF_ID_SERVER_H_
#include "core/IDserver.h"
#endif //_ITF_ID_SERVER_H_

#ifndef _ITF_PLUGINGATEWAY_H_
#include "PluginGateway/PluginGateway.h"
#endif //_ITF_PLUGINGATEWAY_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_XMLALL_H_
#include "engine/XML/XML_All.h"
#endif //_ITF_XMLALL_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_FILEMANAGER_ITF_H_
#include "adapters/FileManager_ITF/FileManager_ITF.h"
#endif //_ITF_FILEMANAGER_ITF_H_

#ifndef _ITF_SUBSCENECOMPONENT_H_
#include "engine/actors/components/SubSceneComponent.h"
#endif //_ITF_SUBSCENECOMPONENT_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_ACTOREDITOR_H_
#include "tools/plugins/ActorEditor/ActorEditor.h"
#endif //_ITF_ACTOREDITOR_H_

#ifndef _ITF_WORLD_H_
#include "engine/scene/world.h"
#endif //_ITF_WORLD_H_

#ifndef _ITF_WORLD_MANAGER_H_
#include "engine/scene/worldManager.h"
#endif //_ITF_WORLD_MANAGER_H_

#ifndef _ITF_OBJECTID_SERVER_H_
#include "core/ObjectIDserver.h"
#endif //_ITF_OBJECTID_SERVER_H_

#ifndef _ITF_SUBSCENEACTOR_H_
#include "engine/actors/SubSceneActor.h"
#endif //_ITF_SUBSCENEACTOR_H_


#ifndef _ITF_PLUGIN_FRIEZEEDITOR_H_
#include "tools/plugins/Plugin_FriezeEditor/Plugin_FriezeEditor.h"
#endif //_ITF_PLUGIN_FRIEZEEDITOR_H_


#ifndef _ITF_UNDOREDOMANAGER_H_
#include "editor/UndoRedoManager.h"
#endif //_ITF_UNDOREDOMANAGER_H_

#ifndef _ITF_EDITORDRAW_H_
#include "editor/EditorDraw.h"
#endif //_ITF_EDITORDRAW_H_

#ifndef _ITF_GRAPHICCOMPONENT_H_
#include "engine/actors/components/graphiccomponent.h"
#endif //_ITF_GRAPHICCOMPONENT_H_

#ifndef _ITF_EDITOR_GUI_TEXTINPUT_H_
#include "editor\GUI.h"
#endif // _ITF_EDITOR_GUI_TEXTINPUT_H_

#ifndef _ITF_DEBUGINFO_H_
#include "engine/debug/debugInfo.h"
#endif //_ITF_DEBUGINFO_H_

#ifndef _ITF_VERSIONING_H_
#include "core/versioning.h"
#endif //_ITF_VERSIONING_H_

#ifndef _ITF_CHEATMANAGER_H_
#include "engine/gameplay/CheatManager.h"
#endif //_ITF_CHEATMANAGER_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef SERIALIZEROBJECTBINARY_H
#include "core/serializer/ZSerializerObjectBinary.h"
#endif // SERIALIZEROBJECTBINARY_H

#ifndef _ITF_BASEOBJECTFACTORY_H_
#include "engine/factory/baseObjectFactory.h"
#endif //_ITF_BASEOBJECTFACTORY_H_

#ifndef _ITF_SPAWNMANAGERCOMPONENT_H_
#include "gameplay/Managers/SpawnManagerComponent.h"
#endif //_ITF_SPAWNMANAGERCOMPONENT_H_

#ifndef _ITF_NAMEGENERATOR_H_
#include "engine/common/NameGenerator.h"
#endif //_ITF_NAMEGENERATOR_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_SOURCECONTROLADAPTER_H_
#include "engine/AdaptersInterfaces/SourceControlAdapter.h"
#endif // _ITF_SOURCECONTROLADAPTER_H_

#ifndef _ITF_ALIASMANAGER_H_
#include "engine/aliasmanager/aliasmanager.h"
#endif //_ITF_ALIASMANAGER_H_

namespace ITF
{
    void postCloneGenericProcess(const SafeArray<BaseObject*>& _sources, const SafeArray<BaseObject*>& _duplicates)
    {
        ITF_ASSERT_CRASH(_sources.size() == _duplicates.size(), "Provide valid parameters");

        const u32 iSize = _duplicates.size();

        for(u32 iObj = 0; iObj < iSize; ++iObj)
        {
            Pickable* pSource       = static_cast<Pickable*>(_sources[iObj]); // CAN BE NULL
            Pickable* pDuplicate    = static_cast<Pickable*>(_duplicates[iObj]);

            if(pSource && pSource->getSceneRef() == pDuplicate->getSceneRef())
                pDuplicate->generateNewUserFriendly();
            
            // Handle subscene duplication
            if(SubSceneActor* pSSA = pDuplicate->DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817)))
            {
                if(!pSSA->getEmbedScene())
                {
                    pSSA->loadExternalScene();
                }
                else
                {
                    if ( Scene* pSubScene = pSSA->getSubScene() )
                        pSubScene->loadSubScenes();
                }

                World* pWorld = pDuplicate->getWorld();
                pWorld->addScene(pSSA->getSubScene());
            }

            pDuplicate->onLoaded();

            if(pSource)
            {
                Vec3d offset = pSource->getPos() - pSource->getWorldInitialPos();
                pDuplicate->setPos(pDuplicate->getWorldInitialPos() + offset);

                if(SubSceneActor* pSSA = pDuplicate->DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817)))
                    pSSA->updateCurrentWorldDataContentFromLocal();
            }
        }

        for(u32 iObj = 0; iObj < iSize; ++iObj)
        {
            Pickable* pDuplicate    = static_cast<Pickable*>(_duplicates[iObj]);

            ITF_ASSERT_CRASH(pDuplicate, "Working with NULL pointers doesn't brings so much fun for user.");

            UNDOREDO_MANAGER->postAdd(pDuplicate);
        }

        PLUGINGATEWAY->onObjectsDuplicated(_sources, _duplicates);
    }

    void Editor::selectObject(ObjectRef _selection, bbool _bClearPreviousSelection)
    {
        ObjectRefList _newSel;
        _newSel.push_back(_selection);
        selectObjects(_newSel, _bClearPreviousSelection);
    }

    void Editor::selectObjects(const ObjectRefList &_selection, bbool _bClearPreviousSelection)
    {
        SafeArray<PickingShape*> shapes(_selection.size());
        for(u32 i = 0; i < _selection.size(); ++i)
        {
            ObjectRef localShape = _selection[i];
            PickingShape* localMain = getMainPickingShapeForObject(localShape);
            if(localMain)
                shapes.push_back(localMain);
        }

        selectShapes(shapes, _bClearPreviousSelection);
    }
    
    void Editor::selectShape(PickingShape* _shape, bbool _bClearPreviousSelection)
    {
        ITF_ASSERT(_shape);
        if (!_shape)
            return;
        SafeArray<PickingShape*> shapes;
        shapes.push_back(_shape);
        selectShapes(shapes, _bClearPreviousSelection);
    }

    void Editor::selectShapes(const SafeArray<PickingShape*>& _shapes, bbool _bClearPreviousSelection)
    {
        if(!PLUGINGATEWAY->isPickingAllowed())
            return;
#ifdef ITF_WINDOWS
        ObjectRefList _filteredSelection(_shapes.size());
        for(u32 i = 0; i < _shapes.size(); ++i)
        {
            ITF_ASSERT(_shapes[i]);
            ObjectRef ref = _shapes[i]->getOwner();
            if (const Pickable* pObj = static_cast<const Pickable*>(ref.getObject())) // yes, this CAN happen
            {
                if(pObj == m_pSelectionObject)
                {
                    updateSelectionPickingInfos();
                    return;
                }

                if(_filteredSelection.find(ref) == -1)
                    _filteredSelection.push_back(ref);
            }
        }

        if(_bClearPreviousSelection)
        {
            // If we are not showing all sub content
            if(!m_showSubsceneContentShapes)
            {
                // If a soon selected object is contained in a displayed ssa or a ready to be unselect ssa then keep the current display list
                // and add the ssa in the previous selection to the display list
                SafeArray<BaseObject*> resolvedRefs;
                bbool addPreviousSubSceneToDisplayList = bfalse;
                ID_SERVER->getObjectListNoNull(_filteredSelection, resolvedRefs);
                ObjectRefList currentSelection = getEditorSelectionAsObjRefList();
                for(u32 i = 0; i < resolvedRefs.size(); ++i)
                {
                    const Pickable* pObj = static_cast<const Pickable*>(resolvedRefs[i]);
                    if(Scene* pScene = pObj->getScene())
                    {
                        if(SubSceneActor* pSSA = pScene->getSubSceneActorHolder())
                        {
                            if(currentSelection.find(pSSA->getRef()) != -1 || m_displaySubSceneContent.find(pSSA->getRef()) != -1)
                            {
                                addPreviousSubSceneToDisplayList = btrue;
                                break;
                            }
                        }
                    }
                }

                if(addPreviousSubSceneToDisplayList)
                {
                    resolvedRefs.clear();
                    ID_SERVER->getObjectListNoNull(currentSelection, resolvedRefs);
                    for(u32 i = 0; i < resolvedRefs.size(); ++i)
                    {
                        const Pickable* pObj = static_cast<const Pickable*>(resolvedRefs[i]);
                        
                        if(const SubSceneActor* pSSA = pObj->DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817)))
                        {
                            ObjectRef oRef = pSSA->getRef();
                            if(m_displaySubSceneContent.find(oRef) == -1)
                                m_displaySubSceneContent.push_back(oRef);
                        }
                    }
                }
                else
                    m_displaySubSceneContent.clear();
            }
            else
                m_displaySubSceneContent.clear();

            unselectAll();
        }

        setState(EditorState_EditMode);

        const u32 uShapesCount = _shapes.size();
        bbool subSceneFroJoinGroupFound = bfalse;
        for (u32 index = 0; index < uShapesCount; index++)
        {
            PickingShape* pPickingShape = _shapes[index];
            if(_filteredSelection.find(pPickingShape->getOwner()) == -1)
                continue;

            const Pickable* pObj = static_cast<const Pickable*>(pPickingShape->getOwnerPtr());

            PickingInfo2D& info = *pPickingShape->getPickingInfoPtr();
            pPickingShape->fillPickingInfo(&info);
            if(PLUGINGATEWAY->onPickShape(pPickingShape))
            {
                ITF_ASSERT_MSG(static_cast<const Pickable*>(pObj)->isSelected(), "At this point, the pickable must be selected");

                m_selection.push_back(pPickingShape);
            }
            else
            {
                ITF_ASSERT_MSG(0, "onPickShape(...) failed");
            }
            if (m_joingroupList.size() && !subSceneFroJoinGroupFound && pObj && pObj->getObjectType() == BaseObject::eActor)
            {
                Actor* pActor = (Actor*)pObj;
                if(SubSceneActor* pssa = pActor->DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817)))
                {
                    if(allowEditSubScene(pssa))
                    {
                        SubSceneComponent * subScene = pActor->GetComponent<SubSceneComponent>();

                        subSceneFroJoinGroupFound = btrue;
                        setJoingroupListTarget(subScene);
                    }
                }
            }
        }

        updateWarningFilter();
#endif // WIN32
    }

    void Editor::selectAll()
    {   
        ObjectRefList newSelection;

        if(const World* pWorld = CURRENTWORLD)
        {
            for(u32 iScene = 0; iScene < pWorld->getSceneCount(); ++iScene)
            {
                if(const Scene* pScene = pWorld->getSceneAt(iScene))
                {
                    for(u32 iPickable = 0; iPickable < pScene->getFrises().size(); ++iPickable)
                    {
                        newSelection.push_back(pScene->getFrises()[iPickable]->getRef());
                    }
                    
                    const PickableList& actors = pScene->getActors();
                    for(u32 iActor = 0; iActor < actors.size(); ++iActor)
                    {
                        const Actor* pActor = static_cast<const Actor*>(actors[iActor]);
                        if(pActor->isSerializable() || pActor->isSpawned())
                        {
                            newSelection.push_back(pActor->getRef());
                        }
                    }
                }
                else
                {
                    ITF_ASSERT_MSG(0, "NULL scene in world::sceneList");
                }
            }
        }
        else
        {
            ITF_ASSERT_MSG(0, "NULL world");
        }

        selectObjects(newSelection, btrue);
    }
    
    void Editor::unselectShape(PickingShape* _shape)
    {
        ITF_ASSERT(_shape);

        if(!getSubEditedItem().isValid() && !_shape->isSubAnchor())
        {
            unselectObject(_shape->getOwner());
        }
        else
        {
            const i32 indexShape = getIndexOfShapeInSelection(getLastClickedShape());
            if(indexShape >= 0)
                m_selection.eraseNoOrder(indexShape);
        }
    }

    void Editor::unselectObject(ObjectRef _selection)
    {
        ObjectRefList _newUnsel;
        _newUnsel.push_back(_selection);
        unselectObjects(_newUnsel);
    }

    void Editor::unselectObjects(const ObjectRefList &_pickableSelection)
    {
        for(u32 i = 0; i < _pickableSelection.size(); ++i)
        {
            ObjectRef pickableRef = _pickableSelection[i];
            i32 idx = getRefIndexInSelection(pickableRef);
            while(idx != -1)
            {
                m_selection.eraseNoOrder(idx);
                idx = getRefIndexInSelection(pickableRef);
            }

            if(Pickable* pPickable = static_cast<Pickable*>(pickableRef.getObject()))
                pPickable->setSelected(NULL);
        }
    }

    void Editor::unselectAll()
    {
        if(getSubEditedItem().isValid())
        {
            m_selection.clear();
            updateWarningFilter();
            return;
        }

        ObjectRefList _newUnsel;
        for(i32 i = 0; i < i32(getSelectionSize()); ++i)
        {
            const PickingShape* pPickingShape = getSelectionAt(i);
            if(pPickingShape)
                _newUnsel.push_back(pPickingShape->getOwner());
            else
                m_selection.eraseNoOrder(i--);
        }
            
        unselectObjects(_newUnsel);

        if(PLUGINGATEWAY)
            PLUGINGATEWAY->UnselectAll(bfalse);

        setSubEditedItem(ITF_INVALID_OBJREF);
        updateWarningFilter();
    }
    
    void Editor::updateWarningFilter()
    {
#ifndef ITF_FINAL
        //Warning filtering
        SafeArray<BaseObject*> objects;
        u32 selectionSize = getSelectionList().size();
        objects.reserve(selectionSize);
        for (u32 i=0; i<selectionSize; i++)
        {
            PickingShape *shape = getSelectionList()[i];
            if (shape && !shape->isSubAnchor())
            {
                BaseObject *object = shape->getOwnerPtr();
                if (object && WarningManager::hasWarningForObject(object))
                {
                    objects.push_back(object);
                }
            }

        }
        if (objects.size())
        {
            WarningManager::filterDisplayForObjects(&objects[0], objects.size());
        }
        else
        {
            WarningManager::filterDisplayForObjects(NULL, 0);
        }
#endif
    }

    void Editor::updateSelectionPickingInfos()
    {
        const u32 selectionSize = getSelectionSize();
        for(u32 i = 0; i < selectionSize; ++i)
        {
            if (PickingShape* pPickingShape = getSelectionAt(i))  // happened on March 4th, 2011
                pPickingShape->fillPickingInfo(pPickingShape->getPickingInfoPtr());
        }
    }

    i32 Editor::getRefIndexInSelection(const ObjectRef& _ref) const
    {
        i32 iResult = -1;
        const u32 uSelectionSize = getSelectionSize();
        for(u32 i = 0; i < uSelectionSize; ++i)
        {
            const PickingShape* pPickingShape = getSelectionAt(i);
            if(pPickingShape && _ref == pPickingShape->getOwner())
            {
                iResult = i;
                break;
            }
        }

        return iResult;
    }

    i32 Editor::getIndexOfShapeInSelection(const PickingShape* _pShape)const
    {
        i32 iResult = -1;
        const u32 uSelectionSize = getSelectionSize();
        for(u32 i = 0; i < uSelectionSize; ++i)
        {
            const PickingShape* pPickingShape = getSelectionAt(i);
            if(pPickingShape == _pShape)
            {
                iResult = i;
                break;
            }
        }

        return iResult;
    }

    bbool local_lastPauseStatus = bfalse;

    void Editor::startDrag()
    {
        pushPause();
        ITF_ASSERT(!m_bDragSelection);

        LOG_OLIV("[Editor] Start drag");

        m_bDragSelection = btrue;
        m_fDragSqrDistance = 0.0f;
        m_bDragAfterDuplicate = bfalse;
    }

    void Editor::endDrag()
    {
        popPause();
        if(!m_bDragSelection)
            return;

        LOG_OLIV("[Editor] End drag");

        m_bDragSelection = bfalse;
        m_bDragAfterDuplicate = bfalse;

        if(isDragMinDistReached(MIN_MOUSE_MOVE_PIXEL_SQR_DRAG))
        {
            ObjectRefList refList = getEditorSelectionAsObjRefList();
            // DONT PRECACHE THE LIST POINTERS, SOME WILL BE DELETED DURING THE LOOP ID_SERVER->getObjectListNoNull(refList, resolvedRefs);
            for(u32 i = 0; i < refList.size(); ++i)
            {
                BaseObject* pB = refList[i].getObject();
                if (pB)
                    PLUGINGATEWAY->onObjectChanged(pB);
            }
        }
    }


    void Editor::startZoneSelection()
    {
        if(m_zoneSelectionStarted)
            return;

        m_zoneSelectionStarted  = btrue;
        m_zoneSelectionStart    = getMouse2d();

        LOG_OLIV("[Editor] Start zone selection at %i:%i", (i32)m_zoneSelectionStart.m_x, (i32)m_zoneSelectionStart.m_y);
    }

    void Editor::endZoneSelection()
    {
        if(!m_zoneSelectionStarted)
            return;

        LOG_OLIV("[Editor] End zone selection at %i:%i", (i32)getMouse2d().m_x, (i32)getMouse2d().m_y);

        m_zoneSelectionStarted = bfalse;
    }

    Vec2d prevRotationDir = Vec2d::Zero;

    void computeRotationDir(const Vec3d& _rotationCenter, Vec2d& _res)
    {
        Vec3d pivot;
        GFX_ADAPTER->compute3DTo2D(_rotationCenter, pivot);
        _res = (EDITOR->getMouse2d() - pivot.truncateTo2D()).normalize();
        _res.m_y = -_res.m_y;
    }

    void Editor::startRotation(Vec3d* _rotationCenter /*= NULL*/)
    {
        if(m_bRotateSelection)
            return;

        pushPause();

        LOG_OLIV("[Editor] Start selection rotation");

        m_bRotateSelection = btrue;
        m_fRotationQty = 0.0f;

        if(_rotationCenter)
        {
            m_RotationOrigin = *_rotationCenter;
        }
        else
        {
            m_RotationOrigin = getMouseOnCurrentWorkingDepth();
            if(PickingShape* underMouseShape = getUnderMouseRealPickingShape())
            {
                if(underMouseShape->getShapeType() == PickingShape::ShapeType_Disc)
                {
                    m_RotationOrigin = static_cast<PickingShape_Disc*>(underMouseShape)->getPos();
                    setLastClickedShape(underMouseShape);
                }
            }
        }

        computeRotationDir(m_RotationOrigin, prevRotationDir);

        PLUGINGATEWAY->onStartRotation();

        local_lastPauseStatus = CHEATMANAGER->getPause();
    }

    void Editor::endRotation()
    {
        if(!m_bRotateSelection)
            return;

        popPause();

        LOG_OLIV("[Editor] End selection rotation");

        m_bRotateSelection = bfalse;
        PLUGINGATEWAY->onEndRotation();
        
        if(m_fRotationQty > 0.0f)
        {
            ObjectRefList refList = getEditorSelectionAsObjRefList();
            // DONT PRECACHE THE LIST POINTERS, SOME WILL BE DELETED DURING THE LOOP ID_SERVER->getObjectListNoNull(refList, resolvedRefs);
            for(u32 i = 0; i < refList.size(); ++i)
            {
                BaseObject* pB = refList[i].getObject();
                if (pB)
                    PLUGINGATEWAY->onObjectChanged(pB);
            }
        }
    }

    void Editor::startDuplicateSelection()
    {
        LOG_OLIV("[Editor] Start duplicate");

        endDrag();
        endRotation();
    }

    void Editor::endDuplicateSelection()
    {
        LOG_OLIV("[Editor] End duplicate");

        m_bUnselectOrDuplicate = bfalse;
        m_bMayDuplicateSelectedAfterDrag = bfalse;

        startDrag();
        m_bDragAfterDuplicate = btrue;
    }

    void Editor::setSubEditedItem( ObjectRef _ref )
    {
        if(getSubEditedItem().isValid() && _ref.isValid())
        {
            LOG_OLIV("[Editor] Start subedit");

            endDrag();
            endRotation();
        }
        else if(getSubEditedItem().isValid() && !_ref.isValid())
        {
            LOG_OLIV("[Editor] End subedit");
        }

        m_subEdited = _ref;
    }

    bbool Editor::deleteGeneric( const SafeArray<PickingShape*>& _shapesWithSameOwner )
    {
        return PLUGINGATEWAY->onEditorRemove(_shapesWithSameOwner);
    }

    bbool isClonable(Pickable* pObj)
    {
        if(World* pWorld = pObj->getWorld())
            if(pWorld->getRef() == GAMEMANAGER->getBaseMapWorld())
                return bfalse;

        if(pObj->getObjectType() == BaseObject::eTrajectory)
            return bfalse;

        if(SubSceneActor* pSSA = pObj->DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817)))
        {
            if(!pSSA->getEmbedScene())
            {
                const PathChar_t* path = pSSA->getSceneRelativePath().getBasename();

                bbool preventCopy = bfalse;
                preventCopy |= strstr(path, "_ld.isc") != NULL;
                preventCopy |= strstr(path, "_graph.isc") != NULL;
                preventCopy |= strstr(path, "_sound.isc") != NULL;

                if(preventCopy)
                {
                    ITF_ERROR("Duplication of subscene: '%s' is not possible", path);
                    return bfalse;
                }
            }
        }
        
        return btrue;
    }

    void Editor::handleSelectionDuplication()
    {
        if(getSubEditedItem().isValid())
            return;

        // Handle selection duplication
        if(!m_bUnselectOrDuplicate && !m_bMayDuplicateSelectedAfterDrag)
            return;

        // Prevent duplication if we just had a double-click
        if(m_justDoubleClicked)
        {
            m_justDoubleClicked = bfalse;
            return;
        }

        if(!isDragMinDistReached(MIN_MOUSE_MOVE_PIXEL_SQR_COPY))
            return;

        if(Plugin* pPlugin = PLUGINGATEWAY->getExclusiveFocus())
            return;

        LOG_OLIV("[Editor] It's not a drag it's a duplication");

        startDuplicateSelection();

        SafeArray<PickingShape*> prevSelectionCopy = getSelectionList();
        const u32 prevSelectionCopySize = prevSelectionCopy.size();
        unselectAll();

        SafeArray<BaseObject*> sources, duplicates;

        const UndoRedoSequenceScope undoScope("Duplicate the selection");
        for(u32 i = 0; i < prevSelectionCopySize; ++i)
        {
            PickingShape* pPickingShape = prevSelectionCopy[i];

            if(pPickingShape->isSubAnchor())
                continue;

            Pickable* pPickable = static_cast<Pickable*>(pPickingShape->getOwnerPtr());
            if(!pPickable || !isClonable(pPickable))
                continue;

            Pickable* pNewPickable = pPickable->clone(pPickable->getScene(), bfalse);
            ITF_ASSERT(pNewPickable);
            if(pNewPickable)
            {
                sources.push_back(pPickable);
                duplicates.push_back(pNewPickable);
            }
        }

        postCloneGenericProcess(sources, duplicates);

        for ( u32 i = 0; i < duplicates.size(); i++ )
        {
            const Pickable* pNewPickable = static_cast<const Pickable*>(duplicates[i]);
            PickingShape* pNewPickableShape = getMainPickingShapeForObject(pNewPickable->getRef());

            if ( !pNewPickableShape )
            {
                continue;
            }

            selectObject(pNewPickable->getRef(), bfalse);

            // Set the position of the new pickable
            // The picking anchor must be used
            {
                pNewPickableShape->fillPickingInfo(pNewPickableShape->getPickingInfoPtr());
                pNewPickableShape->dragByMouse(getMouse2d());
            }
        }

        endDuplicateSelection();
    }

    void Editor::handleSelectionRotation()
    {
        if(!m_bRotateSelection)
            return;
        
        const bbool bMultiRotation = getSelectionSize() > 1;

        // Draw rotation center icon
        if(bMultiRotation)
        {
            getIconStock().drawIcon(Editor_IconStock::Icon_Light, m_RotationOrigin, Anchor_DefaultSize / 5.0f);
        }

        const bbool itsTimeForUndo = (m_fRotationQty == 0.0f);

        Vec2d dir;
        computeRotationDir(m_RotationOrigin, dir);

        f32 angle       = dir.getAngle();
        f32 prevAngle   = prevRotationDir.getAngle();

        prevRotationDir = dir;

        snapAngle(angle);
        snapAngle(prevAngle);

        f32 deltaAngle  = angle - prevAngle;

        const f32 mouseRotateQty    = f32_Abs(deltaAngle);
        if(mouseRotateQty <= MTH_EPSILON)
            return;
        
        const String8 actionName("Rotating the selection");
        if(itsTimeForUndo)
            UNDOREDO_MANAGER->beginUndoRedoSequence(actionName);
        const u32 uSelectionSize = getSelectionSize();
        for(u32 i = 0; i < uSelectionSize; ++i)
        {
            PickingShape* pPickingShape = getSelectionAt(i);
            if (!pPickingShape)
                continue;

            Pickable* pPickable = static_cast<Pickable*>(pPickingShape->getOwnerPtr());
            if(!pPickable)
                continue;

            if(itsTimeForUndo)
                UNDOREDO_MANAGER->preChange(pPickable);

            // Check if the rotation is local to the object or not
            const bbool currentShapeIsRotationOrigin = (getLastClickedShape() == pPickingShape);
            Vec3d* pRotationOrigin = NULL;
            if(bMultiRotation && !currentShapeIsRotationOrigin)
                pRotationOrigin = &m_RotationOrigin;

            // Rotate the item
            pPickingShape->rotateAroundAxisZ(deltaAngle,  pRotationOrigin);
            pPickable->onEditorMove(isAllowStorePickableInitData(pPickable));
        }
        if(itsTimeForUndo)
            UNDOREDO_MANAGER->validateUndoRedoSequence(actionName);

        // Store rotation quantity for simple click detection
        m_fRotationQty += mouseRotateQty;
    }

    void Editor::handleSelectionOffset()
    {
        const f32 mouseDragDeltaSqr = m_frameDeltaMouse2d.sqrnorm();

        static bbool prevMinDistReachedToPerformDrag = isDragMinDistReached(MIN_MOUSE_MOVE_PIXEL_SQR_DRAG);
        const bbool currMinDistReachedToPerformDrag = isDragMinDistReached(MIN_MOUSE_MOVE_PIXEL_SQR_DRAG);
        const bbool itsTimeForUndoPoint = !prevMinDistReachedToPerformDrag && currMinDistReachedToPerformDrag && !m_bDragAfterDuplicate;
        prevMinDistReachedToPerformDrag = currMinDistReachedToPerformDrag;

        // And we are not trying to duplicate the selection
        if(isDragActive() && currMinDistReachedToPerformDrag && !m_bMayDuplicateSelectedAfterDrag && !m_bUnselectOrDuplicate)
        {
            PickingShape* lastClicked = getLastClickedShape();
            // If the user wants to move the selection pivot
            if(m_pSelectionObject->getShape() == lastClicked)
            {
                Vec3d mousePos = getMouse3d();
                snap(mousePos, lastClicked, bfalse);
                Vec3d delta = mousePos - m_pSelectionObject->getPos();
                m_pSelectionObject->setDisplayOffset(delta);
            }
            else
            {
                const String8 actionName("Dragging the selection");
                if(itsTimeForUndoPoint)
                    UNDOREDO_MANAGER->beginUndoRedoSequence(actionName);

                const u32 uSelectionSize = getSelectionSize();
                for(u32 i = 0; i < uSelectionSize; ++i)
                {
                    PickingShape* pPickingShape = getSelectionAt(i);
                    if (!pPickingShape)
                        continue;
                    Pickable* pPickable = static_cast<Pickable*>(pPickingShape->getOwnerPtr());
                    if(!pPickable)
                        continue;

                    if(itsTimeForUndoPoint)
                        UNDOREDO_MANAGER->preChange(pPickable);

                    pPickingShape->dragByMouse(getMouse2d() + m_selectionDeltaToSnap);
                }

                if(itsTimeForUndoPoint)
                    UNDOREDO_MANAGER->validateUndoRedoSequence(actionName);
            }
        }

        // Store drag distance (in pixel�) for simple click detection
        m_fDragSqrDistance += mouseDragDeltaSqr;
    }

    void Editor::offsetSelection( const Vec3d& _offset, bbool _singleAction /*= btrue*/ )const
    {
        UndoRedoSequenceScope scope("Offset the selection");

        Vec3d newOffset = _offset;
        if(_singleAction && INPUT_ADAPTER->isKeyPressed(KEY_LSHIFT))
            newOffset /= 5.0f;

        // const u32 uSelectionSize = getSelectionSize(); DON'T TRY TO PRECACHE THE SIZE, IT CAN CHANGE WITHIN THE LOOP
        for(u32 i = 0; i < getSelectionSize(); ++i)
        {
            PickingShape* pPickingShape = getSelectionAt(i);
            if (!pPickingShape)
                continue;
            Pickable* pPickable = static_cast<Pickable*>(pPickingShape->getOwnerPtr());
            if(!pPickable)
                continue;

            // If an ancestor of the object is also moved the skip it
            Scene * pScene = pPickable->getScene();
            if (pScene)
            {
                SubSceneActor* pAncestor = pScene->getSubSceneActorHolder();
                while(pAncestor)
                {
                    if(pAncestor->isSelected())
                        break;
                    pAncestor = pAncestor->getScene()->getSubSceneActorHolder();
                }
                if(pAncestor)
                    continue;
            } else
            {
                switch (pPickable->getObjectType())
                {
                case BaseObject::eTrajectory:
                case BaseObject:: eEditorDummy:
                    break;
                default:
                    ITF_ASSERT_MSG(bfalse, "object type can't work without a scene");
                    break;
                }
                
            }

            // If a child actor is moved with his parent then skip it
            if(Actor* pActor = pPickable->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115)))
                if(Actor* pActorParent = pActor->getParent().getActor())
                    if(pActorParent->isSelected())
                        continue;

            if(_singleAction)
                UNDOREDO_MANAGER->preChange(pPickable);

            PLUGINGATEWAY->onOffsetShapePosition(pPickingShape, newOffset);
            
            PLUGINGATEWAY->onObjectChanged(pPickable);
        }

        if(!_singleAction)
            scope.cancel();
    }

    void Editor::alignSelection(bbool _x, bbool _y, bbool _z) const
    {
        UndoRedoSequenceScope scope("Align the selection");

        Pickable* pRef = NULL;
        for(u32 i = 0; i < getSelectionSize(); ++i)
        {
            PickingShape* pPickingShape = getSelectionAt(i);
            if (pPickingShape)
            {
                if(Pickable* pObj = static_cast<Pickable*>(pPickingShape->getOwnerPtr()))
                {
                    if(!pRef)
                        pRef = pObj;
                    else
                    {
                        UNDOREDO_MANAGER->preChange(pObj);

                        Vec3d worldPos = pObj->getPos();

                        if(_x)
                            worldPos.setX(pRef->getPos().getX());
                        
                        if(_y)
                            worldPos.setY(pRef->getPos().getY());
                        
                        if(_z)
                            worldPos.setZ(pRef->getDepth());

                        pObj->setPos(worldPos);
                        pObj->setWorldInitialPos(pObj->getPos());

                        PLUGINGATEWAY->onObjectChanged(pObj);
                    }
                }
            }
        }
    }

    Vec3d Editor::getCentroidSelection(const ObjectRefList& _refList, bbool _skipPlayers) const
    {
        Vec3d   vCentroid = Vec3d::Zero;
        u32     selectionSize = 0;

        // Choose the scene origin as the selection's centroid
        for(u32 i = 0; i < _refList.size(); ++i)
        {
            const PickingShape* pPickingShape = getMainPickingShapeForObject(_refList[i]);

            if(!pPickingShape)
            {
                Pickable* pObj = static_cast<Pickable*>(_refList[i].getObject());
                ITF_ASSERT(pObj);
                if(pObj)
                {
                    vCentroid += pObj->getPos();
                    selectionSize++;
                }
            }
            else
            {
                if(_skipPlayers && GAMEMANAGER->isPlayerActor(_refList[i]))
                    continue;

                if(pPickingShape->getShapeType() != PickingShape::ShapeType_Disc)
                    continue;

                const PickingShape_Disc* pPickDisc = static_cast<const PickingShape_Disc*>(pPickingShape);
                vCentroid += pPickDisc->getPos();
                selectionSize++;
            }
        }

        // Finalize centroid computation
        if(selectionSize > 0)
            vCentroid /= f32(selectionSize);

        return vCentroid;
    }

#if defined(ITF_WINDOWS) && !defined(ITF_FINAL)

    World* Editor::exportSelection(bbool _transformToRelative) const
    {
        LoadInfo info;
        info.m_bNodata = btrue;
        info.m_disconnectFromCells = bfalse;

        World* pWorld = static_cast<World*>(GETOBJECT(WORLD_MANAGER->createAndLoadWorld(info)));
        Scene* pScene = pWorld->getRootScene();
        pScene->setPath(NameGenerator::getUniqueName(getEditedScene()->getPath().getString())); // For ObjectPath resolve process

        const ObjectRefList selection = getEditorSelectionAsObjRefList();
        Vec3d vCentroid = getCentroidSelection(selection, btrue);
        vCentroid.m_z = 0.f;

        SafeArray<BaseObject*> sources, duplicates;

        const u32 uSize = selection.size();
        for(u32 i = 0; i < uSize; ++i)
        {
            Pickable* pPickable = static_cast<Pickable*>(selection[i].getObject());
            if(!pPickable || !isClonable(pPickable))
                continue;

            if(pPickable->getObjectType() != BaseObject::eActor && pPickable->getObjectType() != BaseObject::eFrise)
                continue;

            if(!pPickable->isSerializable())
                continue;
 
            if(Pickable* pNewPickable = pPickable->clone(pScene, bfalse))
            {
                sources.push_back(pPickable);
                duplicates.push_back(pNewPickable);
            }
        }

        if (_transformToRelative)
        {
            // Process scene's elements to place them relatively to the new origin
            pScene->offsetPosition(-vCentroid, btrue, btrue);
        }

        postCloneGenericProcess(sources, duplicates);

        return pWorld;        
    }

    bbool Editor::exportSelectionToScene(const String& _absolutePath) const
    {
        UndoRedoSequenceScope sponge("cancel");
        sponge.cancel();

        bbool result = bfalse;

        World* pWorld = exportSelection(btrue);

        if(pWorld)
        {
            Scene* pScene = pWorld->getRootScene();

            if(SOURCECONTROL_ADAPTER)
                SOURCECONTROL_ADAPTER->checkout(_absolutePath);
            // Export
            ITF_VERIFY(XMLAll::saveScene(_absolutePath, pScene));

            ITF_VERIFY(WORLD_MANAGER->deleteWorld(pWorld));

            result = btrue;

        }

        return result;
    }

    void Editor::importScene()
    {
        #ifndef ITF_FINAL
        const World* pWorld = CURRENTWORLD;
        Scene* pDestScene = pWorld ? pWorld->getRootScene() : NULL;
        ITF_ASSERT(pDestScene);
        if(!pDestScene)
            return;

        const Vec3d     subScenePosition  = EDITOR->getMouse3d();
        String          subScenePath("*.isc");
        if(!FILEMANAGER->fileChooser(bfalse, subScenePath, L"ITF Scene\0*.isc\0"))
            return;
        FilePath::transformToRelative(subScenePath);

        switch(SYSTEM_ADAPTER->messageBox("Question", "Import as reference (YES) or merge (NO) ?", ITF_MSG_ICONQUESTION | ITF_MSG_YESNOCANCEL,ITF_IDCANCEL))
        {
        case ITF_IDYES:
            importSceneAsInstance(pDestScene, subScenePosition, subScenePath);
            break;
        case ITF_IDNO:
            importSceneAsMerge(pDestScene, subScenePosition, subScenePath);
            break;
        default:
            break;
        }
        #endif
    }

    void Editor::convertSelectionToScene()
    {
        #ifndef ITF_FINAL

        if (getSelectionSize() < 2)
            return;
        
        World* pWorld = CURRENTWORLD;
        if(!pWorld)
            return;

        String savepath("*.isc");
        if(FILEMANAGER->fileChooser(btrue, savepath, L"ITF Scene\0*.isc\0"))
        {
            if (exportSelectionToScene(savepath))
            {
                onKey(KEY_DEL, InputAdapter::Pressed);

                if(FilePath::transformToRelative(savepath))
                {
                    importSceneAsInstance(pWorld->getRootScene(), getCentroidSelection(getEditorSelectionAsObjRefList(), bfalse), FilePath::normalizePath(savepath));
                }
            }
        }
        #endif
    }

    void Editor::mergeSelectionOnMainScene()
    {
        if (getSelectionSize() != 1)
            return;

        PickingShape* pPickingShape = getSelectionAt(0);
        if (!pPickingShape)
            return;

        Pickable* pPickable = static_cast<Pickable*>(pPickingShape->getOwnerPtr());
        if(pPickable && pPickable->getObjectType() == BaseObject::eActor)// Crash skip
        {
            Actor *             actor    = (Actor *)pPickable;
            SubSceneComponent * subScene = actor->GetComponent<SubSceneComponent>();

            Scene* pBaseScene = NULL;
            World* pWorld = CURRENTWORLD;
            if(pWorld)
                pBaseScene = pWorld->getRootScene();

            if (subScene && subScene->getSubScene() != pBaseScene)
            {
                importSceneAsMerge(pBaseScene, pPickable->getPos(), subScene->getSubScene()->getPath());
                onKey(KEY_DEL, InputAdapter::Pressed);
            }
        }
    }

#endif // WIN32 && !ITF_FINAL

    void Editor::pasteSceneContentAtMouse(Scene* _sceneToPaste)
    {
        UndoRedoSequenceScope scope("Import group");

        const Scene* _srcScene  = _sceneToPaste;
        const Vec3d _pos3d      = getMouse3d();
        Scene* _dstScene        = getEditedScene();

        ITF_ASSERT(_srcScene);
        Vec3d pos3d = _pos3d;
        
        PickableList listToClone;
        listToClone.mergeArray(_srcScene->getActors());
        listToClone.mergeArray(_srcScene->getFrises());

        const u32 uSize = listToClone.size();

        Vec3d centroid;
        {
            ObjectRefList refsForCentroid;

            for(u32 i = 0; i < uSize; ++i)
            {
                Pickable* pSrcPickable = listToClone[i];
                ITF_ASSERT(pSrcPickable);
                if(!pSrcPickable || !isClonable(pSrcPickable))
                    continue;
                if (!pSrcPickable->isSerializable())
                    continue;
                
                refsForCentroid.push_back(pSrcPickable->getRef());
            }
            centroid = getCentroidSelection(refsForCentroid, bfalse);
        }
        centroid.m_z = 0.f;

        ObjectRefList refs;
        SafeArray<BaseObject*> sources, duplicates;

        const UndoRedoSequenceScope undoScope("Import external scene content");
        for(u32 i = 0; i < uSize; ++i)
        {
            Pickable* pSrcPickable = listToClone[i];
            ITF_ASSERT(pSrcPickable);
            if(!pSrcPickable || !isClonable(pSrcPickable))
                continue;

            if (!pSrcPickable->isSerializable())
                continue;

            Pickable* pNewPickable = pSrcPickable->clone(_dstScene, bfalse);
            ITF_ASSERT(pNewPickable);
            if(!pNewPickable)
                continue;

            const bbool objWithSameNameFound = (_dstScene->getPickableFromUserFriendly(pSrcPickable->getUserFriendly()) != NULL);
            if(objWithSameNameFound)
                pNewPickable->generateNewUserFriendly(pNewPickable->getUserFriendly());
            
            sources.push_back(pSrcPickable);
            duplicates.push_back(pNewPickable);
            refs.push_back(pNewPickable->getRef());

            // pos3d.m_z is the Z in world coord of the final object
            Vec3d pos2d, posDef;
            GFX_ADAPTER->compute3DTo2D(Vec3d(pNewPickable->getPos().m_x, pNewPickable->getPos().m_y, pos3d.m_z), pos2d);
            Vec3d mouse2d;
            GFX_ADAPTER->compute3DTo2D(_pos3d, mouse2d);
            GFX_ADAPTER->compute2DTo3D(Vec3d(mouse2d.m_x, mouse2d.m_y, pos2d.m_z), posDef);

            ITF_ASSERT(f32_Abs(posDef.m_z - pos3d.m_z) <= 0.001f);
            pos3d.m_x = posDef.m_x;
            pos3d.m_y = posDef.m_y;
            pos3d.m_z -= pNewPickable->getDepth();

            pos3d -= centroid;

            pNewPickable->setWorldInitialPos(pNewPickable->getWorldInitialPos() + pos3d);
        }

        postCloneGenericProcess(sources, duplicates);

        selectObjects(refs, btrue);
    }

    void Editor::copySelectionData()
    {
        const f32 fZ = getWorkingDepth().getReferenceDepth();

        Vec3d onePos;
        AABB selectionbox; // will be initialized in the loop
        PickableList objList;
        bbool boxInit = bfalse;
        const ObjectRefList selection = getEditorSelectionAsObjRefList();
        for(u32 i = 0; i < selection.size(); ++i)
        {
            Pickable* pObj = static_cast<Pickable*>(selection[i].getObject());
            if(!pObj->isSerializable() || !isClonable(pObj))
                continue;

            switch(pObj->getObjectType())
            {
                case BaseObject::eActor:
                case BaseObject::eFrise:
                    objList.push_back(pObj);

                    if(boxInit)
                        selectionbox.grow(pObj->getAABB());
                    else
                    {
                        onePos = pObj->getAnchorPos();
                        onePos.m_z = fZ;

                        selectionbox = pObj->getAABB();
                        boxInit = btrue;
                    }
                    break;
                default:
                    break;
            }
        }
    
        if(objList.size() > 0)
        {
            destroyCopyPasteData();
            m_pCopyDataContainer = newAlloc(mId_Editor, ArchiveMemory);

            if(objList.size() == 1)
                m_selectionCentroidWhenCopy = onePos;
            else
                m_selectionCentroidWhenCopy = selectionbox.getCenter().to3d(fZ);

            CSerializerObjectBinary serializeBinForCopy(m_pCopyDataContainer);
            CSerializerObject* serializer = &serializeBinForCopy; // Prevent WEIRD compile error
            serializer->Serialize("copyPaste", objList, &BASEOBJECT_FACTORY->m_objectFactory, (ESerializeFlags)(ESerialize_Data_Save|ESerialize_Editor_Save));

            m_copySourceObjects.clear();
            for(u32 iObj = 0; iObj < objList.size(); ++iObj)
                m_copySourceObjects.push_back(objList[iObj]->getRef());
        }
    }

    void Editor::pasteCopyData()
    {
        const UndoRedoSequenceScope undoRedoScope("Paste copy data");

        ITF_ASSERT(m_pCopyDataContainer);
        m_pCopyDataContainer->rewindForReading();

        CSerializerObjectBinary serializeBinForPaste(m_pCopyDataContainer);

        PickableList objList;
        CSerializerObject* serializer = &serializeBinForPaste; // Prevent WEIRD compile error
        serializer->Serialize("copyPaste", objList, &BASEOBJECT_FACTORY->m_objectFactory, (ESerializeFlags)(ESerialize_Data_Load|ESerialize_Editor_Load));

        ITF_ASSERT(objList.size() == m_copySourceObjects.size());

        {
            ObjectRefList refsToSelect;
            SafeArray<BaseObject*> duplicateList;
            SafeArray<BaseObject*> sourceList; // may contain null obj
            ID_SERVER->getObjectList(m_copySourceObjects, sourceList);

            const Vec3d delta = getMouseOnCurrentWorkingDepth() - m_selectionCentroidWhenCopy;

            Scene* pDestScene = getEditedScene();
            for(u32 i = 0; i < objList.size(); ++i)
            {
                Pickable* pObj = objList[i];

                // Check highly rare case: the ref is recycled
                if(Pickable* pSrc = static_cast<Pickable*>(sourceList[i]))
                    if(pSrc->getUserFriendly() != pObj->getUserFriendly() || pSrc->getObjectId() != pObj->getObjectId()) 
                        sourceList[i] = NULL;

                const Pickable* pSource = static_cast<const Pickable*>(sourceList[i]);

                pObj->regenerateObjectId(); // because we do not use Pickable::clone(...)
                duplicateList.push_back(pObj);

                if(pDestScene->getPickableFromUserFriendly(pObj->getUserFriendly()))
                    pObj->generateNewUserFriendly();

                pDestScene->registerPickable(pObj);
                pDestScene->flushPending();
                setSceneAsModified(pDestScene);
                
                if(pSource && INPUT_ADAPTER->isKeyPressed(KEY_LSHIFT))
                {
                    pObj->setWorldInitialPos(pSource->getPos());
                }
                else
                {
                    if(pSource)
                    {
                        const Vec3d worldInit   = pSource->getWorldInitialPos();
                        const Vec3d worldOffset = pSource->getPos() - worldInit;
                        pObj->setWorldInitialPos(worldInit + delta);
                    }
                    else
                    {
                        pObj->setWorldInitialPos(pObj->getPos() + delta);
                    }
                    pObj->offsetPosition(delta, bfalse);
                }

                refsToSelect.push_back(pObj->getRef());
            }

            postCloneGenericProcess(sourceList, duplicateList);
            selectObjects(refsToSelect, btrue);
        }
    }

    SubSceneActor* Editor::importSceneAsInstance(Scene *_scene, const Vec3d& _pos3d, const String & _srcScenePath)
    {
        #ifndef ITF_FINAL
        ITF_ASSERT(_scene);
        if(!_scene)
            return NULL;

        // Check if the new sub scene is already in the current scene hierarchy
        {
            Scene* curScene = _scene;
            while(curScene)
            {
                if(curScene->getPath() == _srcScenePath)
                {
                    ITF_ERROR("You are trying to add a scene instance but this instance is already present in the current hierarchy.\nThis operation is not possible.");
                return NULL;
                }

                if(const SubSceneActor* pSSA = curScene->getSubSceneActorHolder())
                    curScene = pSSA->getScene();
                else
                    curScene = NULL;
            }
        }

        const Path path(GETPATH_ALIAS("subsceneactor"));
        SubSceneActor* pSubSceneActor = ACTORSMANAGER->spawnSubsceneActor(Vec3d::Zero, _scene, path);
        ITF_ASSERT_MSG(pSubSceneActor, "Failed to spawn a subSceneActor");
        if(pSubSceneActor)
        {
            UNDOREDO_MANAGER->postAdd(pSubSceneActor);

            pSubSceneActor->setSerializable(btrue);

            Scene* pSubScene = pSubSceneActor->setSubScene(_srcScenePath);
            World* pWorld = _scene->getWorld();
            pWorld->addScene(pSubScene);
            pSubScene->onSceneLoaded();
            pSubScene->setActive(_scene->isActive());

            pSubSceneActor->generateNewUserFriendly(FilePath::getFilenameWithoutExtension(_srcScenePath).cStr());

            pSubSceneActor->setPos(_pos3d);
            pSubSceneActor->setWorldInitialPos(_pos3d);

            PLUGINGATEWAY->onObjectCreated(pSubSceneActor);

            // Select the new subscene
            selectObject(pSubSceneActor->getRef(), btrue);
        }

        return pSubSceneActor;
    #else
        return NULL;
    #endif
    }

    void Editor::importSceneAsMerge(Scene *_dstScene, const Vec3d& _pos3d, const Scene* _srcScene)
    {
        ITF_ASSERT(_srcScene);
        
        PickableList listToClone;
        listToClone.mergeArray(_srcScene->getActors());
        listToClone.mergeArray(_srcScene->getFrises());

        ObjectRefList refs;
        SafeArray<BaseObject*> sources, duplicates;

        const UndoRedoSequenceScope undoScope("Import external scene content");

        const u32 uSize = listToClone.size();
        for(u32 i = 0; i < uSize; ++i)
        {
            Pickable* pSrcPickable = listToClone[i];
            ITF_ASSERT(pSrcPickable);
            if(!pSrcPickable || !isClonable(pSrcPickable))
                continue;

            if(!pSrcPickable->isSerializable())
                continue;

            Pickable* pNewPickable = pSrcPickable->clone(_dstScene, bfalse);
            ITF_ASSERT(pNewPickable);
            if(!pNewPickable)
                continue;
            duplicates.push_back(pNewPickable);
            sources.push_back(pSrcPickable);

            refs.push_back(pNewPickable->getRef());

            pNewPickable->offsetPosition(_pos3d, btrue);
        }

        postCloneGenericProcess(sources, duplicates);

        selectObjects(refs, btrue);
    }

    void Editor::importSceneAsMerge(Scene *_dstScene, const Vec3d& _pos3d, const Path & _subScenePath)
    {
        LoadInfo info;
        info.m_mapPath = _subScenePath;
        info.m_disconnectFromCells = bfalse;

        World* pWorld = static_cast<World*>(GETOBJECT(WORLD_MANAGER->createAndLoadWorld(info)));
        Scene* pScene = pWorld->getRootScene();

        
        pScene->removeUnserializableFromLists();
        importSceneAsMerge(_dstScene, _pos3d, pScene);
        pScene->restoreUnserializableFromLists();

        ITF_VERIFY(WORLD_MANAGER->deleteWorld(pWorld));
    }
    
    SubSceneActor* Editor::createEmptyGroup(const Vec3d& _worldPos, Scene* _pDestScene)const
    {
        UndoRedoSequenceScope scope("Create empty group");

        const Path path(GETPATH_ALIAS("subsceneactor"));
        SubSceneActor* pSubSceneActor = ACTORSMANAGER->spawnSubsceneActor(_worldPos, _pDestScene, path);

        ITF_ASSERT_CRASH(pSubSceneActor, "Error while creating a new SubSceneActor");

        pSubSceneActor->setSerializable(btrue);
        pSubSceneActor->setEmbedScene(btrue);

        LoadInfo info;
        info.m_worldRef = _pDestScene->getWorldID();
        Scene* pSubScene = SCENE_MANAGER->loadSceneSynchronous(info, btrue, bfalse, bfalse);

        ITF_ASSERT_CRASH(pSubScene, "Error while creating a new empty scene");

        _pDestScene->getWorld()->addScene(pSubScene);
        pSubScene->onSceneLoaded();        
        pSubSceneActor->setLoadedSubScene(pSubScene);
        pSubSceneActor->generateNewUserFriendly("grp");
        PLUGINGATEWAY->onObjectCreated(pSubSceneActor);

        UNDOREDO_MANAGER->postAdd(pSubSceneActor);

        return pSubSceneActor;
    }

    bbool Editor::canTransfertSelection( String& _reason, ObjectRefList& _refList, Scene* _dstScene ) const
    {
        bbool result = btrue;

        if (!_dstScene)
        {
            _reason = "Destination scene in invalid";
            result = bfalse;
        }
        else // Existing scene destination
        {
            if(_refList.size() < 1)
            {
                 _reason = "You must at least select one object";
                result = bfalse;
            }
            else // Selection is big enough
            {
                bbool invalidRefFound = bfalse;
                bbool spawnedObjectFound = bfalse;
                bbool errorObjectFound = bfalse;

                bbool forSpecificReason = bfalse;
                String specificReason;

                if(SubSceneActor* pSSA = _dstScene->getSubSceneActorHolder())
                {
                    if(!allowEditSubScene(pSSA))
                    {
                        forSpecificReason = btrue;
                        result = bfalse;
                        specificReason += "Can't tranfert to an template scene";
                    }
                }

                for(u32 i = 0; i < _refList.size(); )
                {
                    Pickable* pObj = static_cast<Pickable*>(GETOBJECT(_refList[i]));
                    if(!pObj)
                    {
                        invalidRefFound = btrue;
                        result = bfalse;
                        _refList.eraseNoOrder(i);
                        continue;
                    }

                    if (pObj->getObjectType() == BaseObject::eActor)
                    {
                        Actor* pAct = (Actor*) pObj;
                        if (pAct->hasDataError())
                        {
                            errorObjectFound = btrue;
                            result = bfalse;
                            _refList.eraseNoOrder(i);
                            continue;
                        }
                        if (!(pAct->isSerializable()))
                        {
                            spawnedObjectFound = btrue;
                            result = bfalse;
                            _refList.eraseNoOrder(i);
                            continue;
                        }
                        if(pAct->GetComponent<SpawnManagerComponent>())
                        {
                            forSpecificReason = btrue;
                            result = bfalse;
                            specificReason += "\n - Actor with SpawnManagerComponent can't be grouped (this prevent using pools properly)";
                            _refList.eraseNoOrder(i);
                            continue;
                        }
                    }

                    ++i;
                }

                if (!result)
                {
                    _reason = "Grouping cancelled because: ";
                    if (invalidRefFound)
                        _reason += "\n - invalid ref(s) found in selection";
                    if (spawnedObjectFound)
                        _reason += "\n - spawned object(s) found in selection";
                    if (errorObjectFound)
                        _reason += "\n - object(s) with errors found in selection";
                    if(forSpecificReason)
                        _reason += specificReason;
                }
            }
        }
        return result;
    }

    bbool Editor::canGroupSelection(String& _reason, ObjectRefList& _refList, Scene* _dstScene)const
    {
        bbool result = btrue;

        if(canTransfertSelection(_reason, _refList, _dstScene))
        {
            bbool fromDifferentScene = bfalse;

            for(u32 i = 0; i < _refList.size(); )
            {
                Pickable* pObj = static_cast<Pickable*>(GETOBJECT(_refList[i]));

                if(pObj->getScene() != _dstScene)
                {
                    fromDifferentScene = btrue;
                    result = bfalse;
                    _refList.eraseNoOrder(i);
                    continue;
                }

                ++i;
            }

            if (!result)
            {
                _reason = "Grouping cancelled because: ";
                if(fromDifferentScene)
                    _reason += "\n - all selected objects should belong to the same subscene";
            }
        }

        return result;
    }

    ObjectRef Editor::groupSelection(const ObjectRefList& _refList, Scene* _dstScene, SubSceneActor* _pSubSceneActor)
    {
        ObjectRefList refList = _refList;
        String reason;
        bbool error = bfalse;
        if(_pSubSceneActor)
        {
            if(!canTransfertSelection(reason, refList, _dstScene))
                error = btrue;
        }
        else if(!canGroupSelection(reason, refList, _dstScene))
            error = btrue;

        if(error)
        {
            SYSTEM_ADAPTER->messageBox("Impossible", reason, ITF_MSG_ICONWARNING | ITF_MSG_OK,ITF_IDYES);
            return ITF_INVALID_OBJREF;
        }

        UndoRedoSequenceScope scope("Group selection");

        Vec3d centroid = getCentroidSelection(refList, btrue);
        centroid.m_z = getWorkingDepth().getReferenceDepth();
        const Path path(GETPATH_ALIAS("subsceneactor"));
        SubSceneActor* pSubSceneActor = _pSubSceneActor;
        if (!pSubSceneActor) // create a new SubScene for the Group if none was passed as parameter 
        {
            pSubSceneActor = ACTORSMANAGER->spawnSubsceneActor(centroid, _dstScene, path);
            UNDOREDO_MANAGER->postAddGroup(pSubSceneActor);
        }
        ITF_ASSERT_CRASH(pSubSceneActor, "Failed to spawn a subSceneActor");
        
        Scene* pNewScene = NULL;
        if (_pSubSceneActor)
        {
            pNewScene = _pSubSceneActor->getSubScene();
        }
        else
        {
            pSubSceneActor->setSerializable(btrue);
            pSubSceneActor->setEmbedScene(btrue);

            LoadInfo info;
            info.m_worldRef = _dstScene->getWorldID();
            pNewScene = SCENE_MANAGER->loadSceneSynchronous(info, btrue, bfalse, bfalse);
        }
        ITF_ASSERT_CRASH(pNewScene, "Cannot retreive|create the destination scene");

        if (!_pSubSceneActor)
        {
            _dstScene->getWorld()->addScene(pNewScene);
            pNewScene->onSceneLoaded(); 
            pSubSceneActor->setLoadedSubScene(pNewScene);
            pSubSceneActor->generateNewUserFriendly("grp");
        }

        SafeArray<BaseObject*> resolvedRefs;
        SafeArray<Scene*> previousScenes;
        ID_SERVER->getObjectListNoNull(refList, resolvedRefs);
        const u32 uSelSize = resolvedRefs.size();
        for(u32 i = 0; i < uSelSize; ++i)
        {
            Pickable* pPickable = static_cast<Pickable*>(resolvedRefs[i]);

            Vec3d prevInitialWorldPos = pPickable->getWorldInitialPos();


            ObjectPath previousPath;
            SceneObjectPathUtils::getAbsolutePathFromObject(pPickable, previousPath);

            Scene* pScene = pPickable->getScene();

            pScene->unregisterPickable(pPickable, bfalse);
            pScene->flushPending();
            pNewScene->registerPickable(pPickable);
            pNewScene->flushPending();

            pPickable->setWorldInitialPos(prevInitialWorldPos);
            pPickable->setPos(pPickable->getWorldInitialPos());
            pPickable->setWorldInitialRot(pPickable->getAngle());
            pPickable->setWorldInitialScale(pPickable->getScale());

            EDITOR->addPickableToRefresh(pPickable->getRef());

            UNDOREDO_MANAGER->postSceneMigration(pPickable, previousPath);

            previousScenes.push_back(pScene);
        }

        ITF_ASSERT_CRASH(uSelSize == previousScenes.size(), "Wrong size");

        for(u32 i = 0; i < uSelSize; ++i)
        {
            Pickable* pPickable = static_cast<Pickable*>(resolvedRefs[i]);
            Scene* previousScene = previousScenes[i];
            Scene* newScene = pPickable->getScene();

            PLUGINGATEWAY->onPickableSceneChanged(pPickable, previousScene, newScene);
        }

        if (!_pSubSceneActor)
        {
            PLUGINGATEWAY->onObjectCreated(pSubSceneActor);
        }

        // Select the new subscene
        selectObject(pSubSceneActor->getRef(), btrue);

        EDITOR->addPickableToRefresh(pSubSceneActor->getRef());
        return pSubSceneActor->getRef();
    }
    
    bbool Editor::canUngroupSelection(String& _reason, ObjectRef _ref)
    {
        bbool result = btrue;

        BaseObject* pObj = _ref.getObject();
        if(!pObj)
        {
            _reason = "Cannot ungroup a deleted object";
            result = bfalse;
        }
        else
        {
            SubSceneActor* pSSA = pObj->DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817));
            if(!pSSA || !pSSA->getEmbedScene())
            {
                _reason = "A selected object cannot be ungrouped (it's not an embedded scene)";
                result = bfalse;
            }
        }

        return result;
    }

    void Editor::ungroupSelection(ObjectRef _ref, bbool _popFromGroup)
    {
        if(!_popFromGroup)
        {
            String reason;
            if(!canUngroupSelection(reason, _ref))
            {
                SYSTEM_ADAPTER->messageBox("Impossible", reason, ITF_MSG_ICONWARNING | ITF_MSG_OK,ITF_IDYES);
                return;
            }

            UndoRedoSequenceScope scope("Ungroup selection");
            
            SubSceneActor* pSSA = static_cast<SubSceneActor*>(_ref.getObject()); // Pointers are checked in canUngroupSelection(...)

            Scene* pSubScene = pSSA->getSubScene();
            PickableList listToMove;
            listToMove.mergeArray(pSubScene->getActors());
            listToMove.mergeArray(pSubScene->getFrises());

            SafeArray<Scene*> previousScenes;
        
            ObjectRefList newSelection;
            const u32 uSize = listToMove.size();
            for(u32 i = 0; i < uSize; ++i)
            {
                Pickable* pPickable = listToMove[i];

                ObjectPath previousPath;
                SceneObjectPathUtils::getAbsolutePathFromObject(pPickable, previousPath);

                Vec3d prevInitialWorldPos = pPickable->getWorldInitialPos();

                pSubScene->unregisterPickable(pPickable, bfalse);
                pSubScene->flushPending();
                pSSA->getScene()->registerPickable(pPickable);
                pSSA->getScene()->flushPending();
                
                pPickable->setWorldInitialPos(prevInitialWorldPos);
                pPickable->setPos(pPickable->getWorldInitialPos());
                pPickable->setWorldInitialRot(pPickable->getAngle());
                pPickable->setWorldInitialScale(pPickable->getScale());
                
                pPickable->onEditorMove(isAllowStorePickableInitData(pPickable));
                EDITOR->addPickableToRefresh(pPickable->getRef());

                newSelection.push_back(pPickable->getRef());

                UNDOREDO_MANAGER->postSceneMigration(pPickable, previousPath);

                previousScenes.push_back(pSubScene);
            }

            for(u32 i = 0; i < uSize; ++i)
            {
                Pickable* pPickable = listToMove[i];

                PLUGINGATEWAY->onPickableSceneChanged(pPickable, previousScenes[i], pPickable->getScene());
            }

            EDITOR->addPickableToRefresh(pSSA->getRef());
                
            selectObjects(newSelection, btrue);

            UNDOREDO_MANAGER->preDeleteGroup(pSSA);

            {
                const bbool prevState = UNDOREDO_MANAGER->isEnabled();
                UNDOREDO_MANAGER->setEnabled(bfalse); // Because the undo/redo delete is not compatible with undo/redo group/ungroup
            
                ObjectRefList refList;
                refList.push_back(_ref);
                deleteSelection(&refList);
                pSSA = NULL;

                UNDOREDO_MANAGER->setEnabled(prevState);
            }
        }
        else
        {
            if(BaseObject* pObj = _ref.getObject())
            {
                if(Pickable* pPickable = pObj->DynamicCast<Pickable>(ITF_GET_STRINGID_CRC(Pickable,594660361)))
                {
                    if(SubSceneActor* pSSAHolder = pPickable->getScene()->getSubSceneActorHolder())
                    {
                        UNDOREDO_MANAGER->preChange(pPickable);

                        Scene* pSRC = pPickable->getScene();
                        Scene* pDST = pSSAHolder->getScene();

                        ObjectPath previousPath;
                        SceneObjectPathUtils::getAbsolutePathFromObject(pPickable, previousPath);

                        Vec3d prevInitialWorldPos = pPickable->getWorldInitialPos();

                        pSRC->unregisterPickable(pPickable, bfalse);
                        pSRC->flushPending();
                        pDST->registerPickable(pPickable);
                        pDST->flushPending();
                        
                        pPickable->setWorldInitialPos(prevInitialWorldPos);
                        pPickable->setPos(pPickable->getWorldInitialPos());
                        pPickable->setWorldInitialRot(pPickable->getAngle());
                        pPickable->setWorldInitialScale(pPickable->getScale());
                        
                        pPickable->onEditorMove(isAllowStorePickableInitData(pPickable));
                        EDITOR->addPickableToRefresh(pPickable->getRef());
                        
                        UNDOREDO_MANAGER->postSceneMigration(pPickable, previousPath);
                        PLUGINGATEWAY->onPickableSceneChanged(pPickable, pSRC, pDST);
                    }
                }
            }
        }
    }

    bbool Editor::saveCurrentWorld(bbool _saveAs, bbool _forceSaveUnmodified)
    {
        bbool bOk = bfalse;

#ifndef RETAIL
        f64 rs = SYSTEM_ADAPTER->getTime();
#endif //RETAIL

        if(World* pWorld = CURRENTWORLD)
        {
            bbool oneSaveFailed = bfalse;
            SafeArray<StringID> alreadySavedPathCRC; // Used to save a scene once

            const u32 currentEngineVersion = Versioning::getEngineVersion();

            const bbool rootMapIsOfficial = isOfficialScene(*(pWorld->getRootScene()));

            String subPath;
            String userPath;

            for(u32 i = 0; i < pWorld->getSceneCount(); ++i)
            {
                Scene* pScene = pWorld->getSceneAt(i);

                // Skip already saved scenes
                if(alreadySavedPathCRC.find(pScene->getPath().getStringID()) >= 0)
                    continue;

                pScene->getPath().getString(userPath);
                
                // Check for dependancies between official and test maps, and prevent save if pb found
                {
                    static const char* testRootPath = "test/";
                    static const u32 testRootSize = 5;

                    String truncatedPath = userPath;
                    truncatedPath.truncate(testRootSize);

                    const bbool currentMapIsTest = (truncatedPath == testRootPath);
                    if(rootMapIsOfficial)
                    {
                        if(currentMapIsTest)
                        {
                            SubSceneActor* pSSA = pScene->getSubSceneActorHolder();
                            if(!pSSA->getEmbedScene())
                            {
                                String8 error;
                                error.setTextFormat("Official map depends on non-official data: '%ls'", userPath.cStr());
                                pSSA->setDataError(error);

                                ITF_WARNING(pScene->getSubSceneActorHolder(), 0, error.cStr());
                            }
                        }
                        else
                        {
                            const PickableList& frises = pScene->getFrises();
                            for(u32 iF = 0; iF < frises.size(); ++iF)
                            {
                                Frise* pFrz = (Frise*)frises[iF];
                                if(FriseConfig* pFC = pFrz->getConfig())
                                {
                                    String8 subPath = pFC->getPath();
                                    subPath.truncate(testRootSize);

                                    if(subPath == testRootPath)
                                    {
                                        String8 error;
                                        error.setTextFormat("Official map depends on non-official data: '%s'", pFC->getPath().cStr());
                                        pFrz->setDataError(error);
                                        ITF_WARNING(pFrz, 0, error.cStr());
                                    }
                                }
                            }

                            const PickableList& actors = pScene->getActors();

                            for(u32 iA = 0; iA < actors.size(); ++iA)
                            {
                                Actor* pAct = (Actor*)actors[iA];
                                pAct->getLua().getString(subPath);
                                subPath.truncate(testRootSize);

                                if(subPath == testRootPath)
                                {
                                    String8 error;
                                    error.setTextFormat("Official map depends on non-official data: '%ls'", subPath.cStr());
                                    pAct->setDataError(error);
                                    ITF_WARNING(pAct, 0, error.cStr());
                                }
                            }
                        }
                    }
                }
                
                // Do not save scene loaded from engine with higher version (if current version is not a beta engine)
                if(currentEngineVersion > 0 && currentEngineVersion < pScene->getEngineVersionWhenSaved())
                {
                    SYSTEM_ADAPTER->messageBox("Info", String("Save not allowed for this scene : ") + userPath, ITF_MSG_ICONERROR | ITF_MSG_OK,ITF_IDYES);
                    continue;
                }

                // Save grid size ?
                {
                    bbool saveGridSize = bfalse;
                    if(m_editedSubSceneHolderRef.isValid())
                    {
                        if(SubSceneActor* pSSA = pScene->getSubSceneActorHolder())
                            if(pSSA->getRef() == m_editedSubSceneHolderRef)
                                saveGridSize = btrue;
                    }
                    else if(0 == i)
                        saveGridSize = btrue;

#ifdef ITF_WINDOWS
                    if(saveGridSize)
                        pScene->setGridUnit(m_gridSize);
#endif //ITF_WINDOWS

                }

                const bbool askForSaveAs = (_saveAs && i == 0);
                if(askForSaveAs)
                {
                    if(!FILEMANAGER->fileChooser(btrue, userPath, L"Scene (*.isc)\0*.isc\0"))
                        return btrue;

                    String sceneName = FilePath::normalizePath(userPath);
                    ITF_VERIFY(FilePath::transformToRelative(sceneName));
                    pScene->setPath(sceneName);
                }
                else
                {
                    PathString_t pathName;
                    pScene->getPath().getString(pathName);
                    FILEMANAGER->TranslatePath(userPath, pathName);
                }

                // Don't try to save groups, but try to save modified subscenes if modified
                SubSceneActor* pSSA = pScene->getSubSceneActorHolder();
                if(!pSSA || (pSSA && !pSSA->getEmbedScene()))
                {
                    // Skip tsc when it's a subscene
                    if(i != 0 && String(pScene->getPath().getExtension()) == "tsc")
                        continue;

                    if(pScene->reorderSerializedLists())
                        setSceneAsModified(pScene);

                    if(askForSaveAs || _forceSaveUnmodified || sceneHasBeenModified(pScene))
                    {
                        EDITOR->fillAssociatedTabList(pScene);

                        const bbool bFileExistBeforeSave = FILEMANAGER->fileExists(userPath);
                        if(SOURCECONTROL_ADAPTER && bFileExistBeforeSave)
                            SOURCECONTROL_ADAPTER->checkout(userPath);

                        if(XMLAll::saveScene(userPath, pScene, bfalse))
                        {
                            if(SOURCECONTROL_ADAPTER && !bFileExistBeforeSave)
                                SOURCECONTROL_ADAPTER->add(userPath);

                            if(i == 0)
                            {
                                updateDebugInfo();
                                setAppTitleScene(pScene);
                            }

                            pScene->replaceReinitDataByCurrent();

                            alreadySavedPathCRC.push_back(pScene->getPath().getStringID());
                        }
                        else
                        {
                            oneSaveFailed = btrue;
                            SYSTEM_ADAPTER->messageBox("Error", String("Couldn't save the following scene : ") + userPath, ITF_MSG_ICONERROR | ITF_MSG_OK, ITF_IDYES);
                        }
                    }
                }
            }

            if(!_forceSaveUnmodified && alreadySavedPathCRC.size() == 0)
                if (DEBUGINFO)
                    DEBUGINFO->addDebugInfoDisplay("No changes detected, use SHIFT + S if you want to force resave.", 5.0f);

            // Ok if each scenes are saved
            bOk = !oneSaveFailed;

            if(bOk && alreadySavedPathCRC.size() > 0)
            {
                setOrphanTabsOwner(pWorld);
                m_SaveIconTimer = 0.3f;
                UNDOREDO_MANAGER->currentWorldJustSaved();
                resetSceneModifiedList();
            }
        }

#ifndef RETAIL
        LOG("[Editor::saveCurrentWorld] Time to save: %0.02f", (f32)(SYSTEM_ADAPTER->getTime()-rs));
#endif //RETAIL
        return bOk;
    }

    void Editor::deleteSelection(const ObjectRefList* _refList/* = NULL*/)
    {
        resetPickingInfo();
        
        const UndoRedoSequenceScope undoScope("Delete the selection");

        if(!_refList)
        {
            SafeArray<PickingShape*> selectionCopy = m_selection;

            while(selectionCopy.size())
            {
                SafeArray<PickingShape*> shapesWithSameOwner, ignoreList;

                ObjectRef currentOwner;
                for(u32 i = 0; i < selectionCopy.size(); ++i)
                {
                    PickingShape* pShape = selectionCopy[i];
                    if(ignoreList.find(pShape) != -1)
                        continue;

                    Pickable* pOwner = static_cast<Pickable*>(pShape->getOwnerPtr());

                    bbool gotoIgnoreList = bfalse;
                    // Test if we can delete
                    if(World* pWorld = pOwner->getWorld())
                        if(pWorld->getRef() == GAMEMANAGER->getBaseMapWorld())
                            gotoIgnoreList = btrue;

                    if (!pShape->isSubAnchor() && pShape->getOwner() == getSubEditedItem())
                        gotoIgnoreList = btrue;

                    // Do not delete if a parent is also selected for delete
                    if(Scene* pScene = pOwner->getScene())
                    {
                        SubSceneActor* pAncestor = pScene->getSubSceneActorHolder();
                        while(pAncestor)
                        {
                            if(pAncestor->isSelected())
                            {
                                gotoIgnoreList = btrue;
                                break;
                            }
                            pAncestor = pAncestor->getScene()->getSubSceneActorHolder();
                        }
                    }
                    // end tests
                    if(gotoIgnoreList)
                    {
                        ignoreList.push_back(pShape);
                        continue;
                    }

                    if(!currentOwner.isValid())
                        currentOwner = pShape->getOwner();

                    if(pShape->getOwner() == currentOwner)
                        shapesWithSameOwner.push_back(pShape);
                }

                if(shapesWithSameOwner.size() > 0)
                {
                    deleteGeneric(shapesWithSameOwner);
                }

                for(u32 i = 0; i < shapesWithSameOwner.size(); ++i)
                {
                    i32 index = selectionCopy.find(shapesWithSameOwner[i]);
                    selectionCopy.eraseNoOrder(index);
                }
                for(u32 i = 0; i < ignoreList.size(); ++i)
                {
                    i32 index = selectionCopy.find(ignoreList[i]);
                    selectionCopy.eraseNoOrder(index);
                }
            }

            unselectAll();
        }
        else
        {
            for(u32 i = 0; i < _refList->size(); ++i)
            {
                deleteGeneric(*getPickingShapesForObject((*_refList)[i]));
            }
        }

        WORLD_MANAGER->flushPending();
    }

    void Editor::destroyCopyPasteData()
    {
        SF_DEL(m_pCopyDataContainer);
    }

    void Editor::renameSelectionFriendlyNames()
    {
#if defined(ITF_WINDOWS)
        const u32 uSelSize = getSelectionSize();
        if(uSelSize == 0)
            return;

        // Do not allow multi renaming if a group/subscene is in a multi selection
        for(u32 i = 0; i < uSelSize; ++i)
        {
            Pickable* pObj = static_cast<Pickable*>(getSelectionAt(i)->getOwnerPtr());
            ITF_ASSERT(pObj);
            if(!pObj)
                continue;

            SubSceneActor* pSSA = pObj->DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817));
            if(pSSA && uSelSize > 1)
                return;
        }

        PickingShape* pShape = getSelectionAt(0);
        if (!pShape)
            return;

        Pickable* pMainSelection = static_cast<Pickable*>(pShape->getOwnerPtr());
        
        // Retreive current name
        String newFriendly;
        if(uSelSize == 1)
        {
            newFriendly = pMainSelection->getUserFriendly();
        }

        // Ask for new name
        if (!GUI_TextInput::inputText("Rename", "Change friendly :", newFriendly))
            return;

        bbool duplicateFriendly = bfalse;
        String8 newFriendly8(newFriendly.cStr());

        // Check if the new name is unique at same level of hierarchy for each soon renamed obj
        for(u32 i = 0; i < uSelSize; ++i)
        {
            PickingShape* pShape = getSelectionAt(i);
            if (!pShape)
                continue;
            BaseObject* pObj = pShape->getOwnerPtr();
            Pickable* pSelectedPickable = static_cast<Pickable*>(pObj);
            ITF_ASSERT(pObj);
            if(pObj)
            {
                const Scene* pScene = pSelectedPickable->getScene();
                const PickableList& pickables = pScene->getPickableList();
                const u32 listSize = pickables.size();
                for(u32 i = 0; i < listSize; ++i)
                {
                    // For batch renaming
                    if(uSelSize > 1)
                        newFriendly8.setTextFormat("%S %u", newFriendly.cStr(), i + 1);

                    Pickable* pSceneObj = pickables[i];
                    ITF_ASSERT(pSceneObj);
                    if((pSceneObj != pSelectedPickable) && (pSceneObj->getUserFriendly().equals(newFriendly8, true)))
                    {
                        duplicateFriendly = btrue;
                        break;
                    }
                }
            }
        }

        if(duplicateFriendly)
        {
            SYSTEM_ADAPTER->messageBox("Warning", "Name already taken, please choose another...", ITF_MSG_ICONWARNING | ITF_MSG_OK,ITF_IDYES);
        }
        else // Perform rename
        {
            UndoRedoSequenceScope scope("Rename selection friendly names");
            
            for(u32 i = 0; i < uSelSize; ++i)
            {
                PickingShape* pShape = getSelectionAt(i);
                if (!pShape)
                    continue;
                BaseObject* pObj = pShape->getOwnerPtr();
                Pickable* pPickable = static_cast<Pickable*>(pObj);
                ITF_ASSERT(pObj);
                if(pObj)
                {
                    UNDOREDO_MANAGER->preChange(pPickable);

                    // For batch renaming
                    if(uSelSize > 1)
                        newFriendly8.setTextFormat("%S %u", newFriendly.cStr(), i + 1);

                    const String8 previousFriendly = pPickable->getUserFriendly();
                    pPickable->setUserFriendly(newFriendly8);

                    PLUGINGATEWAY->onPickableRenamed(pPickable, previousFriendly);
                }
            }
        }
#endif // ITF_WINDOWS
    }

    ObjectRefList Editor::getEditorSelectionAsObjRefList()const
    {
        ObjectRefList refList;
        for(u32 i = 0; i < getSelectionSize(); ++i)
        {
            PickingShape* pShape = getSelectionAt(i);
            if (!pShape)
                continue;
            if(refList.find(pShape->getOwner()) == -1)
                refList.push_back(pShape->getOwner());
        }

        return refList;
    }

    void Editor::teleportSelectionToMouse()const
    {
        if(getSelectionSize() > 0)
        {
            const ObjectRefList selection   = getEditorSelectionAsObjRefList();
            const Vec3d centroid            = getCentroidSelection(selection, bfalse);
            Vec3d dest                      = limitRayToWorldAABB(CAMERA->getCorrectedPos(), getMouse3d(), btrue);
            Vec3d delta                     = dest - centroid;

            UndoRedoSequenceScope scope("teleport selection");

            offsetSelection(delta);
        }
    }

    void Editor::toggleSelectionFlipX() const
    {
        UndoRedoSequenceScope scope("x flip selection");
        
        const ObjectRefList selection = getEditorSelectionAsObjRefList();        
        SafeArray<BaseObject*> resolvedRefs;
        ID_SERVER->getObjectListNoNull(selection, resolvedRefs);

        const u32 uSelSize = resolvedRefs.size();
        for(u32 iRef = 0; iRef < uSelSize; ++iRef)
        {
            if(Pickable* pObj = static_cast<Pickable*>(resolvedRefs[iRef]))
            {
                switch(pObj->getObjectType())
                {
                case BaseObject::eActor:
                    {
                        Actor* pActor = static_cast<Actor*>(pObj);

                        UNDOREDO_MANAGER->preChange(pActor);

                        pActor->setFlipped(!pActor->isFlipped());
                        pActor->setStartFlipped(pActor->isFlipped());

                        PLUGINGATEWAY->onObjectChanged(pActor);

                        if(SubSceneActor* ssa = pActor->DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817)))
                        {
                            SafeArray<SubSceneActor*> potentialList;
                            potentialList.push_back(ssa);

                            while(potentialList.size())
                            {
                                SubSceneActor* pSSA = potentialList[0];
                                potentialList.eraseNoOrder(0);

                                if(Scene* pSubScene = pSSA->getSubScene())
                                {
                                    for(u32 i = 0; i < pSubScene->getFrises().size(); ++i)
                                    {
                                        Frise* pFrz = static_cast<Frise*>(pSubScene->getFrises()[i]);

                                        UNDOREDO_MANAGER->preChange(pFrz);

                                        pFrz->setPos(pFrz->getWorldInitialPos());

                                        pFrz->mirror(btrue, bfalse, pFrz->getPos().truncateTo2D());
                                        pFrz->setAngle(-pFrz->getAngle());

                                        PLUGINGATEWAY->onObjectChanged(pFrz);
                                    }

                                    const ObjectRefList& ssaList = pSubScene->getSubSceneActors();
                                    for(u32 iSSA = 0; iSSA < ssaList.size(); ++iSSA)
                                        if(SubSceneActor* pSubSSA = static_cast<SubSceneActor*>(ssaList[iSSA].getObject()))
                                            potentialList.push_back(pSubSSA);
                                }
                            }
                        }
                    }
                    break;

                case BaseObject::eFrise:
                    {
                        FriezeEditor::mirror(static_cast<Frise*>(pObj), btrue, bfalse);
                    }
                    break;

                default:
                    break;
                }
            }
        }
    }

} // namespace ITF

#endif //ITF_CONSOLE_FINAL
