#include "precompiled_editor.h"

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef ITF_CONSOLE_FINAL

#ifndef _ITF_EDITOR_H_
#include "editor/Editor.h"
#endif //_ITF_EDITOR_H_

#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_ID_SERVER_H_
#include "core/IDserver.h"
#endif //_ITF_ID_SERVER_H_

#ifndef _ITF_PICKING_H_
#include "editor/Picking.h"
#endif //_ITF_PICKING_H_

#ifndef _ITF_PLUGINGATEWAY_H_
#include "PluginGateway/PluginGateway.h"
#endif //_ITF_PLUGINGATEWAY_H_

#ifndef _ITF_SCENE_MANAGER_H_
#include "engine/scene/sceneManager.h"
#endif //_ITF_SCENE_MANAGER_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_MATHTOOLS_H_
#include "core/math/MathTools.h"
#endif //_ITF_MATHTOOLS_H_

#ifndef _ITF_PICKINGSHAPE_H_
#include "editor/PickingShape.h"
#endif //_ITF_PICKINGSHAPE_H_


#ifndef _ITF_PLUGIN_FRIEZEEDITOR_H_
#include "tools/plugins/Plugin_FriezeEditor/Plugin_FriezeEditor.h"
#endif //_ITF_PLUGIN_FRIEZEEDITOR_H_


namespace ITF
{

    void Editor::fillPickingInfos()
    {
        // Update mouse over, and mousemove
        {
            refillPickableList();

            // In theses modes, don't get picking info
            if(!isDragActive() && !m_bRotateSelection && !m_bUnselectOrDuplicate && !isContextMenuVisible())
            {
                PickingShape* pickingShapeOfObjectUnderMouse = NULL;
                PickingShape *reallyUnderMousePickingShape = Picking::findUnderMouseRealPickingShape(m_underMouseShapePickingInfo, m_activeShapesListOrdered);
                pickingShapeOfObjectUnderMouse = reallyUnderMousePickingShape;

                if (reallyUnderMousePickingShape)
                {
                    setUnderMouseRealPickingShape(reallyUnderMousePickingShape);
                }
                else
                {
                    if(m_pSelectionObject->isMouseOver())
                    {
                        pickingShapeOfObjectUnderMouse = m_pSelectionObject->getShape();
                        pickingShapeOfObjectUnderMouse->fillPickingInfo(&m_underMouseShapePickingInfo);
                        setUnderMouseRealPickingShape(pickingShapeOfObjectUnderMouse);
                    }
                    else
                    {
                        setUnderMouseRealPickingShape(NULL);
                        PickingShape* shapeFromFrameBuffer = NULL;
                        // test hovered object from frame buffer picking
                        if(EDITOR->getSubEditedItem() == ITF_INVALID_OBJREF)
                        {
                            Pickable* hoveredPickable = (Pickable*)GETOBJECT(EDITOR->getLastHoveredActorOrFriseObjRef());
                            if (hoveredPickable && EDITOR->isPickableObject(hoveredPickable->getRef()))
                            {
                                shapeFromFrameBuffer = EDITOR->getMainPickingShapeForObject(hoveredPickable->getRef());

                                ITF_ASSERT_MSG(shapeFromFrameBuffer, "The hovered object has no shapes! Friendly : '%ls'", hoveredPickable->getUserFriendly().cStr());
                                if (shapeFromFrameBuffer && shapeFromFrameBuffer->getPriority() < PickingShape::m_UnselectablePriority)
                                    shapeFromFrameBuffer->fillPickingInfo(&m_underMouseShapePickingInfo);
                            }
                        }                    

                        if(shapeFromFrameBuffer)
                            pickingShapeOfObjectUnderMouse = shapeFromFrameBuffer;
                    }
                }
                
                setPickingShapeOfObjectUnderMouse(pickingShapeOfObjectUnderMouse);
                if (pickingShapeOfObjectUnderMouse)
                {
                    ITF_MEMCOPY(pickingShapeOfObjectUnderMouse->getPickingInfoPtr(), &m_underMouseShapePickingInfo, sizeof(PickingInfo2D));
                }
            }
        }
    }

    void Editor::update_handleJoystick(double _elapsed)
    {
    }

    void Editor::updateMouse3d(const Vec2d& _mouse2d)
    {
        CAMERA->apply();

        Vec3d projectedFar, farPoint3d;
        projectedFar.m_x = _mouse2d.m_x;
        projectedFar.m_y = _mouse2d.m_y;
        projectedFar.m_z = 0.9999f;
        GFX_ADAPTER->compute2DTo3D(projectedFar, farPoint3d);

        if(WORLD_MANAGER->getCurrentWorld() == ITF_INVALID_OBJREF)
        {
            m_currentMouse3d = _mouse2d.to3d();
            m_currentMouse3d_NoWorkingDepth = farPoint3d;
            return;
        }


        {
            // Mouse on current depth range
            f32 dummy;
            Vec3d planePoint;
            Vec3d linePoint[2];
            linePoint[1] = CAMERA->getCorrectedPos();
            Vec3d pointWithZ(_mouse2d.m_x, _mouse2d.m_y, 0.9f);
            GFX_ADAPTER->compute2DTo3D(pointWithZ, linePoint[0]);            
            Plane objectPlane(Vec3d(0,0,1), getWorkingDepth().getReferenceDepth());
            bbool planeFound = bfalse;
            if (objectPlane.testLineIntersection(linePoint[1], linePoint[0]-linePoint[1], planePoint, dummy))
            {
                if (dummy > 0.f )
                {
                    planeFound = btrue;
                    m_mouseOnCurrentWorkingDepth = planePoint;
                }
            }

            m_currentMouse3d = m_mouseOnCurrentWorkingDepth;



            PickingShape *realPickingShape=getUnderMouseRealPickingShape();

            if (realPickingShape && realPickingShape->getShapeType()==PickingShape::ShapeType_Disc)
            {
                PickingShape_Disc *disc = (PickingShape_Disc*) realPickingShape;
                Vec3d projectedPickingShape;
                GFX_ADAPTER->compute3DTo2D(disc->getPos(), projectedPickingShape);
                if (projectedPickingShape.m_z < 0.f)
                    projectedPickingShape.m_z = 0.f;
                else if (projectedPickingShape.m_z > 1.f)
                    projectedPickingShape.m_z = 1.f;
                projectedPickingShape.m_x = _mouse2d.m_x;
                projectedPickingShape.m_y = _mouse2d.m_y;

                GFX_ADAPTER->compute2DTo3D(projectedPickingShape, m_currentMouse3d);                    
            }
            else if (getLastHoveredActorOrFriseObjRef().isValid() && getLastHoveredActorOrFrise_Frame()==CURRENTFRAME)
            {
                 GFX_ADAPTER->compute2DTo3D(_mouse2d.to3d(m_hoveredFriseOrActorProjectedZ), m_currentMouse3d);
            }
            else if(!planeFound)
            {   // no way to know where the mouse is, use far z
                m_currentMouse3d = farPoint3d;
            }

            //we store the m_currentMouse3d_NoWorkingDepth here
            m_currentMouse3d_NoWorkingDepth = m_currentMouse3d;

            //we add the WorkingDepth handling
            if(isWorkingDepthForced() && planeFound)
            {
                m_currentMouse3d = m_mouseOnCurrentWorkingDepth;
            }
            //
        }
        


#if 0 // DBG
        String dbgString;       
        dbgString.setTextFormat("Mouse3D: %f, %f, %f", m_currentMouse3d.m_x, m_currentMouse3d.m_y, m_currentMouse3d.m_z);
        GFX_ADAPTER->drawDBGText(dbgString);
        /*
        for(u32 i = 0; i < 10; ++i)
                {
                    GFX_ADAPTER->drawDBGCircle(m_currentMouse3d, 0.2f + i * 0.2f, Color::blue(), 0);
                }*/
        
        
        dbgString.setTextFormat("Mouse depth range 3D: %f, %f, %f", m_mouseOnCurrentWorkingDepth.m_x, m_mouseOnCurrentWorkingDepth.m_y, m_mouseOnCurrentWorkingDepth.m_z);
        GFX_ADAPTER->drawDBGText(dbgString);
        /*
        for(u32 i = 0; i < 10; ++i)
                {
                    GFX_ADAPTER->drawDBGCircle(m_mouseOnCurrentWorkingDepth, 0.2f + i * 0.2f, Color::red(), 0);
                }*/
        
#endif // 1
        
        m_selectionDeltaToSnap = Vec2d::Zero;

        PickingShape* lastPickedShape = getLastClickedShape();
        if(lastPickedShape)
        {
            Vec3d snapped2d;
            Vec3d snapped3d = m_currentMouse3d; // not snapped yet

            // DBG
            //GFX_ADAPTER->drawDBGCircle(snapped3d, 0.4f, Color::black(), 1);

            const Vec3d anchorOffset = lastPickedShape->getPickingInfoPtr()->m_anchor.to3d();

            // If the shape is an edge then snap using the first point
            if(lastPickedShape->getShapeType() == PickingShape::ShapeType_Edge)
            {
                PickingShape_Edge* edge = static_cast<PickingShape_Edge*>(lastPickedShape);

                const FriezeExtraInfo edgeExtraInfo = FriezeExtraInfo(edge->getData());

                const SafeArray<PickingShape*>* shapes = getPickingShapesForObject(lastPickedShape->getOwner());
                const u32 uCount = shapes ? shapes->size() : 0;
                for(u32 i = 0; i < uCount; ++i)
                {
                    PickingShape* pShape = (*shapes)[i];
                    const FriezeExtraInfo extraInfo = FriezeExtraInfo(pShape->getData());
                    if(!extraInfo.m_flags.m_isEdge && !extraInfo.m_flags.m_isPivot && edgeExtraInfo.m_flags.m_subItemIndex == extraInfo.m_flags.m_subItemIndex)
                    {
                        lastPickedShape = pShape;

                        break;
                    }
                }
            }

            // Use the position of the last picked object to check for snap
            // To do so, add the anchor delta in screenspace
            GFX_ADAPTER->compute3DTo2D(snapped3d, snapped2d);
            snapped2d += anchorOffset;
            GFX_ADAPTER->compute2DTo3D(snapped2d, snapped3d);
            
            // DBG
            //GFX_ADAPTER->drawDBGCircle(snapped3d, 0.6f, Color::blue(), 1);

            if(snap(snapped3d, lastPickedShape, btrue))
            {
                // DBG
                //GFX_ADAPTER->drawDBGCircle(snapped3d, 0.5f, Color::red(), 1);

                Vec3d snapped2d;
                GFX_ADAPTER->compute3DTo2D(snapped3d, snapped2d);
                m_selectionDeltaToSnap = snapped2d.truncateTo2D() - getMouse2d();

                // Remove picked object anchor added earlier
                m_selectionDeltaToSnap -= anchorOffset.truncateTo2D();
            }
        }
    }

    Vec3d Editor::getDeltaMoveForBiasedCamera(f32 _x, f32 _y, f32 _z, const Vec3d &zoomDirection, f32 _z2D /*= -1.0f*/ )
    {
        ITF_ASSERT(CAMERA->isBiased());

        Vec3d ref2DProj(f32(GFX_ADAPTER->getScreenWidth() / 2), f32(GFX_ADAPTER->getScreenHeight() / 2), (_z2D > 0.0f ? _z2D : 0.1f));
        Vec3d old3DPos;
        GFX_ADAPTER->compute2DTo3D(ref2DProj, old3DPos);
        
        ref2DProj.m_x += _x; 
        ref2DProj.m_y -= _y;
        Vec3d moved3DPos;
        GFX_ADAPTER->compute2DTo3D(ref2DProj, moved3DPos);

        if (f32_Abs(_z) > MTH_EPSILON)
        {
            Vec3d delta = zoomDirection;
            delta.normalize();
            delta *= _z;
            moved3DPos += delta;
        }
        return moved3DPos-old3DPos;
    }

    void Editor::update_handleTranslation(double _elapsed)
    {
        if (CAMERA->getMode() != Camera::Camera_Editor || !INPUT_ADAPTER->isKeyPressed(EDITOR_CAMERA_KEY) || INPUT_ADAPTER->isKeyPressed(KEY_LALT))
            return;

        f32 amount = 0.5f;
        if (INPUT_ADAPTER->isKeyPressed(KEY_LCTRL))
            amount = 2.5f;

        f32 amount2D = GFX_ADAPTER->getScreenWidth()*0.1f;
        if (INPUT_ADAPTER->isKeyPressed(KEY_LCTRL))
            amount2D *= 2.f;

        Vec3d camDelta = Vec3d::Zero;
        Vec3d zoomDirection = CAMERA->m_lookAt-CAMERA->getCorrectedPos();

        if (INPUT_ADAPTER->isKeyPressed(KEY_UP))
        {
            if (CAMERA->isBiased())
                CAMERA->setBiasedDeltaPos(getDeltaMoveForBiasedCamera(0.f, amount2D, 0.f, zoomDirection));
            else
                camDelta.m_y += amount;
        }
        if (INPUT_ADAPTER->isKeyPressed(KEY_DOWN))
        {
            if (CAMERA->isBiased())
                CAMERA->setBiasedDeltaPos(getDeltaMoveForBiasedCamera(0.f, -amount2D, 0.f, zoomDirection));
            else
                camDelta.m_y -= amount;
        }
        if (INPUT_ADAPTER->isKeyPressed(KEY_LEFT))
        {
            if (CAMERA->isBiased())
                CAMERA->setBiasedDeltaPos(getDeltaMoveForBiasedCamera(-amount2D, 0.f, 0.f, zoomDirection));
            else
                camDelta.m_x -= amount;
        }
        if (INPUT_ADAPTER->isKeyPressed(KEY_RIGHT))
        {
            if (CAMERA->isBiased())
                CAMERA->setBiasedDeltaPos(getDeltaMoveForBiasedCamera(amount2D, 0.f, 0.f, zoomDirection));
            else
                camDelta.m_x += amount;
        }
        if (INPUT_ADAPTER->isKeyPressed(KEY_PAGEUP))
        {
            if (CAMERA->isBiased())
                CAMERA->setBiasedDeltaPos(getDeltaMoveForBiasedCamera(0.f, 0.f, amount, zoomDirection));
            else
                camDelta.m_z -= amount;
        }
        if (INPUT_ADAPTER->isKeyPressed(KEY_PAGEDOWN))
        {
            if (CAMERA->isBiased())
                CAMERA->setBiasedDeltaPos(getDeltaMoveForBiasedCamera(0.f, 0.f, -amount, zoomDirection));
            else
                camDelta.m_z += amount;
        }

        // Pan Camera
        updatePanModeStatus();
        if(isPanActive())
        {
            Vec3d origin, newMousePos;
            Vec3d v2D;

            //We moved cursor to a given place. We want to compute delta position 
            //we switch to 2D, then to 3D because we want every 3D transform to be applied with the matrix of the same frame
            Vec3d projectedOrigin = m_vPanProjectedOrigin;
            Vec3d projectedNewMousePos;
            projectedNewMousePos.setFromVec2d(EDITOR->getMouse2d(), &projectedOrigin.m_z);

            GFX_ADAPTER->compute2DTo3D(projectedOrigin, origin);
            GFX_ADAPTER->compute2DTo3D(projectedNewMousePos, newMousePos);

            Vec3d delta(origin-newMousePos);
            if (CAMERA->isBiased())
            {
                CAMERA->setBiasedDeltaPos(delta);
            }
            else
            {
                camDelta += delta;
            }

            ((Vec2d&)m_vPanProjectedOrigin) = EDITOR->getMouse2d();
        }



        // Apply cam
        if (m_debugCellsCamMode)
            m_debugCellsCamPos += camDelta;
        else
            CAMERA->forcePosition(CAMERA->getPos() + camDelta);
    }

    void Editor::update_handleMouseDragged()
    {
    }

} // namespace ITF

#endif //ITF_CONSOLE_FINAL
