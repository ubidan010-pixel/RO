#include "precompiled_editor.h"

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef ITF_CONSOLE_FINAL

#ifndef _ITF_EDITOR_H_
#include "editor/Editor.h"
#endif //_ITF_EDITOR_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_PICKINGSHAPE_H_
#include "editor/PickingShape.h"
#endif //_ITF_PICKINGSHAPE_H_

#ifndef _ITF_PLUGINGATEWAY_H_
#include "PluginGateway/PluginGateway.h"
#endif //_ITF_PLUGINGATEWAY_H_

#ifndef _ITF_UNDOREDOMANAGER_H_
#include "editor/UndoRedoManager.h"
#endif //_ITF_UNDOREDOMANAGER_H_

#ifndef _ITF_CONFIG_H_
#include "core/Config.h"
#endif //_ITF_CONFIG_H_

#ifndef _ITF_EDITORDRAW_H_
#include "editor/EditorDraw.h"
#endif //_ITF_EDITORDRAW_H_

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_DEBUGINFO_H_
#include "engine/debug/debugInfo.h"
#endif //_ITF_DEBUGINFO_H_

#ifndef _ITF_CHEATMANAGER_H_
#include "engine/gameplay/CheatManager.h"
#endif //_ITF_CHEATMANAGER_H_

namespace ITF
{

    ///////////////////////////////////////////////////////////////////////////////
    void Editor::dispatchMouseMoveForPickingShapes()
    {
        const Vec2d& mouse = getMouse2d();

        PickingShapeVector::iterator it = m_activeShapesListOrdered.begin();
        PickingShapeVector::iterator itEnd = m_activeShapesListOrdered.end();
        while(it != itEnd)
        {
            (*it)->handleMouseMove(mouse);
            ++it;
        }

        m_pSelectionObject->getShape()->handleMouseMove(mouse);
    }

    ///////////////////////////////////////////////////////////////////////////////
    void Editor::dispatchMousePressedForPickingShapes()
    {
        const Vec2d& mouse = getMouse2d();

        PickingShapeVector::iterator it = m_activeShapesListOrdered.begin();
        PickingShapeVector::iterator itEnd = m_activeShapesListOrdered.end();
        while(it != itEnd)
        {
            (*it)->handleMousePressed(mouse);
            ++it;
        }

        m_pSelectionObject->getShape()->handleMousePressed(mouse);
    }

    ///////////////////////////////////////////////////////////////////////////////
    void Editor::dispatchMouseReleasedForPickingShapes()
    {
        const Vec2d& mouse = getMouse2d();

        PickingShapeVector::iterator it = m_activeShapesListOrdered.begin();
        PickingShapeVector::iterator itEnd = m_activeShapesListOrdered.end();
        while(it != itEnd)
        {
            (*it)->handleMouseReleased(mouse);
            ++it;
        }

        m_pSelectionObject->getShape()->handleMouseReleased(mouse);
    }

    ///////////////////////////////////////////////////////////////////////////////
    void Editor::dispatchMouseWheelForPickingShapes(i32 _wheelDelta)
    {
        PickingShapeVector::iterator it = m_activeShapesListOrdered.begin();
        PickingShapeVector::iterator itEnd = m_activeShapesListOrdered.end();
        while(it != itEnd)
        {
            (*it)->handleMouseWheel(_wheelDelta);
            ++it;
        }

        m_pSelectionObject->getShape()->handleMouseWheel(_wheelDelta);
    }

    ///////////////////////////////////////////////////////////////////////////////
    void Editor::handleLeftMB(InputAdapter::PressStatus _status)
    {
        if (!EditorDraw::pickableDetail_canShowAnyDetail())
            EditorDraw::cyclePickableDetailModes();

        if (handleContextMenu())
            return;

        if (!INPUT_ADAPTER->isKeyPressed(EDITOR_CAMERA_KEY) && InputAdapter::Released != _status)
            pushPause();

        if (!INPUT_ADAPTER->isKeyPressed(EDITOR_CAMERA_KEY) && InputAdapter::Released == _status)
            popPause();

        CAMERA->apply();

        if (!EditorDraw::pickableDetail_canShowAnyDetail() && InputAdapter::Released != _status)
            return;

        const bbool bMultiSelectionOrDuplicationKeyPressed = INPUT_ADAPTER->isKeyPressed(MULTI_SELECTION_KEY) || INPUT_ADAPTER->isKeyPressed(MULTI_DUPLICATION_KEY);

        if(InputAdapter::Released == _status)
        {
            handleLeftMB_Release(bMultiSelectionOrDuplicationKeyPressed);
        }
        
        if(InputAdapter::Pressed == _status)
        {
            handleLeftMB_Pressed(bMultiSelectionOrDuplicationKeyPressed);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    ///Left button release
    void Editor::handleLeftMB_Pressed(bbool _bMultiSelectionOrDuplicationKeyPressed)
    {
        m_bMayDuplicateSelectedAfterDrag = bfalse;

        switch(getState())
        {
        case EditorState_EditMode_Measure:
            if(!isPanActive())
            {
                Vec3d mousePos = getMouse3d();
                snap(mousePos, NULL, bfalse);

                m_MeasureLines.push_back(mousePos);
            }
            break;

        case EditorState_Idle:
            enterEditMode();
            // break; INTENTIONAL !!
        case EditorState_EditMode:
        case EditorState_PluginFocus:
            {
                if(isPanActive())
                    break;

                if (getState()!=EditorState_Idle)
                    dispatchMousePressedForPickingShapes();

                if(isInRotationScaleHelperMode())
                    break;

                PickingShape* pPickingShapeOfObjectUnderMouse = getPickingShapeOfObjectUnderMouse();
                setLastClickedShape(pPickingShapeOfObjectUnderMouse);

                if(!PLUGINGATEWAY->isPickingAllowed())
                    break;

                if (pPickingShapeOfObjectUnderMouse)
                {
                    CAMERA->setMode(Camera::Camera_Editor);

                    Pickable* pPickable = static_cast<Pickable*>(pPickingShapeOfObjectUnderMouse->getOwnerPtr());
                    ITF_ASSERT(pPickable);
                    if(!pPickable)
                        break;
                    const bbool underMouseIsInSelection = (getIndexOfShapeInSelection(pPickingShapeOfObjectUnderMouse) >= 0);

                    if(_bMultiSelectionOrDuplicationKeyPressed)
                    {
                        if(underMouseIsInSelection)
                        {
                            if(!m_bDragAfterDuplicate)
                            {
                                LOG_OLIV("[Editor] At this time we don't know if the user wants to unselect a pickable or duplicate the current selection");

                                // Update picking info in case of duplication
                                updateSelectionPickingInfos();

                                m_bUnselectOrDuplicate = btrue;
                            }
                        }
                        else
                        {
                            LOG_OLIV("[Editor] Add pickable to selection and set MayDuplicate for further analysis");

                            selectShape(pPickingShapeOfObjectUnderMouse, bfalse);

                            updateSelectionPickingInfos();
                            
                            m_bMayDuplicateSelectedAfterDrag = btrue;
                        }
                    }
                    else // !_bMultiSelectionOrDuplicationKeyPressed
                    {
                        if (!underMouseIsInSelection)
                        {
                            const ObjectRef subEditedRef = getSubEditedItem();

                            // If we are in sub edit then only select shapes of the subedited item
                            if((subEditedRef == ITF_INVALID_OBJREF) || ( pPickingShapeOfObjectUnderMouse && pPickingShapeOfObjectUnderMouse->getOwner() == subEditedRef))
                            {
                                LOG_OLIV("[Editor] Select a shape");

                                selectShape(pPickingShapeOfObjectUnderMouse, btrue);
                                ITF_MEMCOPY(pPickingShapeOfObjectUnderMouse->getPickingInfoPtr(), &m_underMouseShapePickingInfo, sizeof(PickingInfo2D));
                            }
                        }
                        else
                        {
                            LOG_OLIV("[Editor] Update selection picking info");

                            updateSelectionPickingInfos();
                        }
                    }
                    startDrag();
                }
                else if(!_bMultiSelectionOrDuplicationKeyPressed) // The user clicked on nothing
                {
                    unselectAll();
                }
            }
            break;

            // Get the depth of the object under mouse and select it
        case EditorState_EditMode_SelectDepthRange:
            {
                PickingShape* pPickingShapeOfObjectUnderMouse = getPickingShapeOfObjectUnderMouse();
                if(!pPickingShapeOfObjectUnderMouse)
                    break;

                Pickable* pPickable = static_cast<Pickable*>(pPickingShapeOfObjectUnderMouse->getOwnerPtr());
                ITF_ASSERT(pPickable);
                if(!pPickable)
                    break;

                setForcedWorkingDepth(DepthRange(pPickable->getDepth()));

                if (DEBUGINFO)
                {
                    String userInfo;
                    userInfo.setTextFormat("New working depth: %f", getWorkingDepth().getReferenceDepth());
                    DEBUGINFO->addDebugInfoDisplay(userInfo, 5.0f);
                }
            }
            break;

        default:
            break;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////
    ///Left button release
    void Editor::handleLeftMB_Release(bbool _bMultiSelectionOrDuplicationKeyPressed)
    {
        // Check for double-click detection
        const f32 currentTime = ELAPSEDTIME;
        const f32 doubleClickThreshold = 0.3f; // 300ms threshold for double-click
        
        // Detect double-click by checking time between clicks
        if (currentTime - m_lastClickTime < doubleClickThreshold)
        {
            m_justDoubleClicked = btrue;
        }
        else
        {
            m_justDoubleClicked = bfalse;
        }
        m_lastClickTime = currentTime;

        //dispatch to pickable shapes
        switch(getState())
        {
            case EditorState_EditMode:
            case EditorState_PluginFocus:
            {
                if(isPanActive())
                    break;

                dispatchMouseReleasedForPickingShapes();
            }
            break;
        }

        if(isRotationActive())
        {
            if((m_fRotationQty * m_fRotationQty) < MIN_MOUSE_MOVE_PIXEL_SQR_ROTATE)
            {
                LOG_OLIV("[Editor] It's not a rotation it's a simple right click");

                setLastClickedShape(getPickingShapeOfObjectUnderMouse());
            }
            endRotation();
        }
        else if(isDragActive())
        {
            if(!isDragMinDistReached(MIN_MOUSE_MOVE_PIXEL_SQR_DRAG))
            {
                LOG_OLIV("[Editor] It's not a drag it's a simple click");

                PickingShape* clickedShape = getLastClickedShape();

                ITF_ASSERT_MSG(clickedShape, "To enter drag mode, the user has to click on an element, m_lastClickedShape can't be NULL !");

                // The user picked something
                if(clickedShape && m_pSelectionObject->getShape() != clickedShape)
                {
                    if(!_bMultiSelectionOrDuplicationKeyPressed)
                    {
                        const u32 uSelectionSize            = getSelectionSize();
                        const bbool underMouseInSelection = (getIndexOfShapeInSelection(clickedShape) >= 0);

                        if(Pickable* pPickable = static_cast<Pickable*>(clickedShape->getOwnerPtr()))
                        {
                            if(underMouseInSelection && 1 == uSelectionSize)
                            {
                                LOG_OLIV("[Editor] Selection stays unchanged");
                            }
                            else
                            {
                                LOG_OLIV("[Editor] Unselect all and select undermouse");

                                selectShape(clickedShape, btrue);
                            }
                        }
                    }
                    else // _bMultiSelectionOrDuplicationKeyPressed
                    {
                        if(m_bUnselectOrDuplicate)
                        {
                            LOG_OLIV("[Editor] Unselect undermouse");

                            unselectShape(clickedShape);

                            m_bUnselectOrDuplicate = bfalse;
                        }
                    }
                }
            }
            endDrag();
        }

        setLastClickedShape(NULL);
        m_bMayDuplicateSelectedAfterDrag = bfalse;
    }
    
    void Editor::handleRightMB(bbool _isPressed)
    {
        if (INPUT_ADAPTER->isKeyPressed(EDITOR_CAMERA_KEY) || (!EditorDraw::pickableDetail_canShowAnyDetail()))
            return;

        if(!_isPressed)
        {
            if(getState() == EditorState_EditMode_SelectDepthRange)
            {
                resetForcedWorkingDepth();
                setState(EditorState_Idle);
                return;
            }

            if(m_bDragAfterDuplicate) // if we are draging a newly duplicated selection then cancel the duplication
            {
                LOG_OLIV("[Editor] Canceling duplication");
                endDrag();
                UNDOREDO_MANAGER->requestUndo();
            }
            else // normal right click release
            {
                if (!m_topBarMenu.isFolded())
                    m_topBarMenu.fold();
                else
                    switchContextMenu();
            }
        }
    }

    void Editor::handleMiddleMB(InputAdapter::PressStatus _status)
    {
        switch(_status)
        {
        case InputAdapter::Pressed:
            {
                if(getState() == EditorState_Idle)
                    enterEditMode();
                
                if(!isPanActive())
                    startZoneSelection();
            }
            break;

        case InputAdapter::Released:
            if (zoneSelectionInProgress())
            {
                // Handle multi selection confirmation
                endZoneSelection();
                
                // If it's not a simple click
                const Vec2d& v1_2D = m_zoneSelectionStart;
                const Vec2d& v3_2D = getMouse2d();

                AABB zoneSS(v1_2D);
                zoneSS.grow(Vec2d(v1_2D.m_x, v3_2D.m_y));
                zoneSS.grow(v3_2D);
                zoneSS.grow(Vec2d(v3_2D.m_x, v1_2D.m_y));

                SafeArray<PickingShape*> shapesInZone;
                
                // Apply cam to allow "compute3DTo2D(...)"
                CAMERA->apply();

                // Challenge objects in zone of selection for new selection status
                PickingShapeVector::iterator it = m_activeShapesListOrdered.begin();
                PickingShapeVector::iterator itEnd = m_activeShapesListOrdered.end();
                while(it != itEnd)
                {
                    PickingShape* pick = (*it);
                    ++it;
                    ITF_ASSERT(pick);
                    if (!pick || pick->getShapeType() != PickingShape::ShapeType_Disc)
                        continue;

                    if(getSubEditedItem() != ITF_INVALID_OBJREF && getSubEditedItem() != pick->getOwner())
                        continue;

                    const PickingShape_Disc* pShapeDisc = static_cast<const PickingShape_Disc*>(pick);

                    Vec3d pos2D;
                    GFX_ADAPTER->compute3DTo2D(pShapeDisc->getPos(), pos2D);
                    if(zoneSS.contains(pos2D.truncateTo2D()))
                    {
                        shapesInZone.push_back(pick);
                    }
                }

                const bbool bMultiSelectionOrDuplicationKeyPressed = INPUT_ADAPTER->isKeyPressed(MULTI_SELECTION_KEY) || INPUT_ADAPTER->isKeyPressed(MULTI_DUPLICATION_KEY);

                selectShapes(shapesInZone, !bMultiSelectionOrDuplicationKeyPressed);
            }
            break;

        default:
            break;
        }
    }

    bbool Editor::onMouseButton(InputAdapter::MouseButton _but, InputAdapter::PressStatus _status)
    {
        updatePanModeStatus();

        switch(_but)
        {
        case InputAdapter::MB_Left:
            handleLeftMB(_status);
        break;

        case InputAdapter::MB_Right:
            handleRightMB(_status != InputAdapter::Released);
        break;

        case InputAdapter::MB_Middle:
            handleMiddleMB(_status);
        break;
        }

        return !isPanActive();
    }

    bbool Editor::onMousePos(i32 _x, i32 _y)
    {
        m_currentMouse2d = Vec2d(f32(_x), f32(_y));

        return btrue;
    }

    bbool Editor::onMouseWheel(i32 /*_mouseWheel*/, i32 _wheelDelta )
    {
        if(CAMERA->getMode() != Camera::Camera_Editor)
            return bfalse;

        dispatchMouseWheelForPickingShapes(_wheelDelta);

        if (INPUT_ADAPTER->isKeyPressed(KEY_RALT))
        {
            i32 val = (i32)m_FrameBufferPickingAlphaRef;
            if (INPUT_ADAPTER->isKeyPressed(KEY_RSHIFT))
                val += _wheelDelta;
            else
                val += _wheelDelta * 5;
            if (val < 0)
                val = 0;
            else if (val > 255)
                val = 255;

            m_FrameBufferPickingAlphaRef = val;
            String msg;
            msg.setTextFormat("Picking Alpha ref: %d", m_FrameBufferPickingAlphaRef);
            DEBUGINFO->addDebugInfoDisplay(msg, 1.0f);
        }


        if(INPUT_ADAPTER->isKeyPressed(EDITOR_CAMERA_KEY))// change zoom
        {
            f32 fMoveFactor = CONFIG->m_editorZoomFactor;
            if(INPUT_ADAPTER->isKeyPressed(EDITOR_SMOOTH_KEY))
                fMoveFactor /= 5.0f;

            fMoveFactor *= _wheelDelta;

            Vec3d vFinalPos = CAMERA->getPos();

            //get target (pickedPos)
            Vec3d pickedPos;
            if (CAMERA->isBiased() && INPUT_ADAPTER->isMousePressed(InputAdapter::MB_Right))
                pickedPos = CAMERA->m_bias_LookAt_RotationCenter;
            else
                pickedPos = getMouse3d_NoWorkingDepth();


            //we get zoom axis from camera position and picked position
            Vec3d v3WSCamPos(CAMERA->getCorrectedPos());
            pickedPos = limitRayToWorldAABB(v3WSCamPos, pickedPos, btrue);

            Vec3d v3WSZoomAxis = pickedPos - v3WSCamPos;

            f32 fDistance2Mouse   = v3WSZoomAxis.norm();
            Vec3d v3Dir2Mouse     = v3WSZoomAxis.normalize();
            Vec3d lookAtDir = CAMERA->m_lookAt - CAMERA->getCorrectedPos();

            //if we went too far, revert to standard direction as dictated by lookat;
            static const f32 minDistance2Mouse = 0.1f;
            static const f32 maxDistance2Mouse = 200.f;
            if (lookAtDir.dot(v3Dir2Mouse)<=0 || fDistance2Mouse < minDistance2Mouse)
            {
                v3Dir2Mouse = lookAtDir;
                fDistance2Mouse = minDistance2Mouse;
            }
            if ((lookAtDir.dot(v3Dir2Mouse)>0) && (fDistance2Mouse > maxDistance2Mouse))
            {
                fDistance2Mouse = maxDistance2Mouse;
            }

            if (CAMERA->isBiased())
            {                            
                CAMERA->setBiasedDeltaPos(getDeltaMoveForBiasedCamera(0.f, 0.f, fDistance2Mouse * fMoveFactor * f32(_wheelDelta * _wheelDelta), v3Dir2Mouse));
            }
            else
            {
                // New cam pos
                const Vec3d v3Offset = v3Dir2Mouse * (fDistance2Mouse * fMoveFactor * f32(_wheelDelta * _wheelDelta));

                // we must invert the offset when camera got a z direction < 0.0f
                {
                    if(v3WSCamPos.m_z - pickedPos.m_z >= 0.0f) 
                        v3WSCamPos += v3Offset;
                    else
                        v3WSCamPos -= v3Offset;
                }
                vFinalPos = v3WSCamPos;
            }

            CAMERA->forcePosition(vFinalPos);
        }
        else // !INPUT_ADAPTER->isKeyPressed(EDITOR_CAMERA_KEY)
        {
            switch(m_state)
            {
            case EditorState_EditMode_SelectDepthRange:
                {
                    DepthRange range = getWorkingDepth();
                    DepthRange newRange(range.getReferenceDepth() + f32(_wheelDelta) * -1.0f);

                    setForcedWorkingDepth(newRange);
                }
                break;

            default:
                {
                    if(INPUT_ADAPTER->isKeyPressed(KEY_LSHIFT) && getSelectionSize() > 0) 
                    {
                        // If there is much time since the last mwheel than make an undo
                        static f32 fLastWheelTime = 0;
                        const f32 diffTime = ELAPSEDTIME - fLastWheelTime;
                        fLastWheelTime = ELAPSEDTIME;
                        const bbool makeUndoPoint = (diffTime > 0.5f);

                        // On mouse wheel, if there is a selection, change their Z
                        const ObjectRefList selection   = getEditorSelectionAsObjRefList();
                        const f32 camToSelectionDist    = (CAMERA->getCorrectedPos() - getCentroidSelection(selection, bfalse)).norm();
                        const f32 wheelFactor           = CONFIG->m_editorMoveDepthFactor*0.2f;
                        f32 fDeltaZ                     = camToSelectionDist * wheelFactor * -f32(_wheelDelta);

                        fDeltaZ = fDeltaZ*f32_Abs(fDeltaZ);

                        const f32 maxDeltaZ = 100.f;
                        if (f32_Abs(fDeltaZ)>maxDeltaZ)
                        {
                            if (fDeltaZ<0)
                                fDeltaZ = -maxDeltaZ;
                            else
                                fDeltaZ = maxDeltaZ;
                        }

                        const String8 actionName = "Changing selection depth with SHIFT+MWHEEL";
                        if(makeUndoPoint)
                        {
                            UNDOREDO_MANAGER->beginUndoRedoSequence(actionName);
                            
                            SafeArray<BaseObject*> resolvedRefs;
                            ID_SERVER->getObjectListNoNull(selection, resolvedRefs);
                            for(u32 iSel = 0; iSel < resolvedRefs.size(); ++iSel)
                                UNDOREDO_MANAGER->preChange(static_cast<Pickable*>(resolvedRefs[iSel]));
                        }
                        
                        offsetSelection(Vec3d(0.0f, 0.0f, fDeltaZ), bfalse);

                        if(makeUndoPoint)
                            UNDOREDO_MANAGER->validateUndoRedoSequence(actionName);
                    }
                }
                break;
            }
        }
        
        // This is for drag and drop, when the z changed (pickable or camera), we need to recompute picking info
        updateSelectionPickingInfos();
        
        return btrue;
    }

} // namespace ITF

#endif //ITF_CONSOLE_FINAL
