#include "precompiled_ray_editor.h"

// GLOBAL ENGINE CONFIG FILE. Must be the first included file
#ifndef _ENGINECOMMON_H_
#include "core/enginecommon.h"
#endif //_ENGINECOMMON_H_

#ifndef _ITF_BEZIERCURVE_PLUGIN_H_
#include "rayman/ray_editor/plugins/BezierCurve_Plugin.h"
#endif //_ITF_BEZIERCURVE_PLUGIN_H_

#ifndef _ITF_EDITORCONTEXTMENU_H_
#include "editor/EditorContextMenu.h"
#endif //_ITF_EDITORCONTEXTMENU_H_

#ifndef _ITF_EDITOR_H_
#include "editor/editor.h"
#endif //_ITF_EDITOR_H_

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_UNDOREDOMANAGER_H_
#include "editor/UndoRedoManager.h"
#endif //_ITF_UNDOREDOMANAGER_H_

#ifndef _ITF_PLUGINGATEWAY_H_
#include "PluginGateway/PluginGateway.h"
#endif //_ITF_PLUGINGATEWAY_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#define TYPE_SHAPE_POINT        0
#define TYPE_SHAPE_TANGA        1
#define TYPE_SHAPE_TANGB        2


namespace ITF
{
    BezierCurve_Plugin* BezierCurve_Plugin::s_instance = NULL;

    Interface_ActorEditorListener* BezierCurve_Plugin::create()
    {
        ITF_ASSERT(!s_instance);
        s_instance = newAlloc(mId_Editor, BezierCurve_Plugin());
        ActorEditor::getInstance()->registerListener(s_instance, BezierCurveComponent::GetClassCRCStatic());
        return s_instance;
    }

    // ********************************************************************

    void BezierCurve_Plugin::destroy()
    {
        ITF_ASSERT(s_instance);
    }

    // ********************************************************************

    BezierCurve_Plugin::BezierCurve_Plugin()
    {
        //none
    }

    // ********************************************************************

    BezierCurve_Plugin::~BezierCurve_Plugin()
    {
        // none
    }

    // ********************************************************************

    void BezierCurve_Plugin::fillContextMenu( EditorContextMenu& _menu, Actor* _actor, ActorComponent* _component )
    {
        if (EDITOR->getSelectionSize() == 1)
        {
             EditorContextMenu& subMenu = _menu.addItemFromExternalPlugin(BezierCurve_SubMenu, "Trajectory Plugin")->m_subMenu;
             subMenu.addItemFromExternalPlugin(BezierCurve_AddBefore, "Add node before");
             subMenu.addItemFromExternalPlugin(BezierCurve_AddAfter, "Add node after");
        }
    }

    // ********************************************************************

    void BezierCurve_Plugin::onContextMenuItemSelected( ContextMenuItem* _item, Actor* _actor, ActorComponent* _component )
    {
        if (EDITOR->getSelectionSize() == 1)
        {
            BezierCurveComponent* trajCompo = static_cast<BezierCurveComponent*>(_component);

            PickingShape* shape = EDITOR->getSelectionAt(0);

            if (shape)
            {
                u32 indexShape = shape->getData();

                switch (_item->getIdFromExternal())
                {
                case BezierCurve_AddBefore:
                    addNode(_actor, trajCompo, indexShape, bfalse);
                    break;

                case BezierCurve_AddAfter:
                    addNode(_actor, trajCompo, indexShape, btrue);
                    break;
                }
            }
              }
        else
        {
            ITF_ASSERT_MSG(0, "Selection size is invalid for Trajectory Plugin !");
        }

    }

    // ********************************************************************

    void BezierCurve_Plugin::fillMainMenu (EditorContextMenu& _menu)
    {
        // none
    }
    
    // ********************************************************************

    void BezierCurve_Plugin::onMainMenuItemSelected(ContextMenuItem* _item)
    {
        // none
    }

    // ********************************************************************

    void BezierCurve_Plugin::addComponentSpecificPickingShape(Actor* _actor, ActorComponent* _component)
    {

        BezierCurveComponent* trajCompo = static_cast<BezierCurveComponent*>(_component);
        BezierCurve & bezier = trajCompo->getBezierCurve();
        Vec3d initPosActor = _actor->getBoundWorldInitialPos();

        if (bezier.getNumPoints() == 0)
        {
            for (u32 i = 0; i < trajCompo->getDefaultNodeCount(); i++)
            {
                const Vec3d pos =Vec3d(2.f * i,0.0f,0.0f);
                if (i==0 && trajCompo->getLockFirstTangent())
                {
                    bezier.addPoint(pos,pos - Vec3d(0.0f, 0.5f, 0.0f),pos + Vec3d(0.0f, 0.5f, 0.0f));
                }
                else
                {
                    bezier.addPoint(pos,pos - Vec3d(0.5f, 0.0f, 0.0f),pos + Vec3d(0.5f, 0.0f, 0.0f));
                }
            }
            _actor->replaceReinitDataByCurrent();
        }

        for (u32 i = 0; i < bezier.getNumPoints(); i++)
        {
            BezierCurve::Point & point = bezier.getPointAt(i);
            // Create position point
            PickingShape_Disc* discPos = newAlloc(mId_Editor, PickingShape_Disc(this));

            BezierTrajectoryExtraInfo extraInfo;
            extraInfo.m_type = TYPE_SHAPE_POINT;
            extraInfo.m_index = i;

            discPos->setData(extraInfo.m_raw);
            discPos->setPos(trajCompo->actorTransform(point.m_pos));
            discPos->setSubAnchor(btrue);
            EDITOR->addPickingShapeForObject(_actor->getRef(), discPos);

            // Create tangent A
            PickingShape_Disc* discTangA = newAlloc(mId_Editor, PickingShape_Disc(this));

            BezierTrajectoryExtraInfo extraInfoTangA;
            extraInfoTangA.m_type = TYPE_SHAPE_TANGA;
            extraInfoTangA.m_index = i;

            discTangA->setData(extraInfoTangA.m_raw);
            discTangA->setPos(trajCompo->actorTransform(point.m_tanA));
            discTangA->setSubAnchor(btrue);
            EDITOR->addPickingShapeForObject(_actor->getRef(), discTangA);

            // Create tangent B
            PickingShape_Disc* discTangB = newAlloc(mId_Editor, PickingShape_Disc(this));

            BezierTrajectoryExtraInfo extraInfoTangB;
            extraInfoTangB.m_type = TYPE_SHAPE_TANGB;
            extraInfoTangB.m_index = i;

            discTangB->setData(extraInfoTangB.m_raw);
            discTangB->setPos(trajCompo->actorTransform(point.m_tanB));
            discTangB->setSubAnchor(btrue);
            EDITOR->addPickingShapeForObject(_actor->getRef(), discTangB);

        }       
        trajCompo->init();
    }

    // ********************************************************************

    void BezierCurve_Plugin::updateComponentSpecificPickingShape(Actor* _actor, PickingShape* _shape, PickingShapeVector& _activeShapes)
    {
        BezierCurveComponent* trajCompo = _actor->GetComponent<BezierCurveComponent>();
        BezierCurve & bezier = trajCompo->getBezierCurve();

        // TODO : for Undo/Redo
        //// Check if number of pickingShape is ok
        //const SafeArray<PickingShape*>* shapesForObj = EDITOR->getPickingShapesForObject(_shape->getOwner());
        //if (shapesForObj)
        //{
        //    if (nodeArray.size() != (shapesForObj->size()-1) / 3)
        //    {
        //        // Delete all shape, and re-create shapes from data
        //        EDITOR->deletePickingShapesForObject(_actor->getRef());
        //        ActorEditor::createPickingShapeForActor(_actor->getRef());
        //        return;
        //    }
        //}

        PickingShape_Disc* disc = static_cast<PickingShape_Disc*>(_shape);

        BezierTrajectoryExtraInfo extraInfo;
        extraInfo.m_raw = disc->getData();
        u32 type = extraInfo.m_type;
        u32 indexNode = extraInfo.m_index;

        Vec3d initPosActor = _actor->getBoundWorldInitialPos();

        Vec3d newPos;
        if (indexNode >= bezier.getNumPoints())
            return;
        BezierCurve::Point & point = bezier.getPointAt(indexNode);

        if (type == TYPE_SHAPE_POINT)
        {
           newPos = trajCompo->actorTransform(point.m_pos);
        }
        else if (type == TYPE_SHAPE_TANGA)
        {
            newPos = trajCompo->actorTransform(point.m_tanA);
        }
        else if (type == TYPE_SHAPE_TANGB)
        {
            newPos = trajCompo->actorTransform(point.m_tanB);
        }

        disc->setPos(newPos);
        disc->setlastUpdateFrame(CURRENTFRAME);

        if ((type == TYPE_SHAPE_POINT)
            && (indexNode != 0))
        {
            _activeShapes.push_back(disc);
        }
        else if (((type == TYPE_SHAPE_TANGA) || (type == TYPE_SHAPE_TANGB))
                && _actor->isSelected())
        {
            _activeShapes.push_back(disc);
        }
    }

    // ********************************************************************

    bbool BezierCurve_Plugin::onDeletePickingShape( PickingShape* _shape )
    {
        if (_shape->getCreatorPickingShapePlugin() != this)
        {
            // not our shape
            return bfalse;
        }

        // it's the "main" shape, do nothing
        if (!_shape->isSubAnchor())
        {
            return bfalse;
        }


        bbool shapeDeleted = bfalse;

        Actor* owner = static_cast<Actor*>(_shape->getOwnerPtr());
        if (owner)
        {
            BezierCurveComponent* trajCompo = owner->GetComponent<BezierCurveComponent>();
            
            if (trajCompo)
            {
                BezierCurve & bezier = trajCompo->getBezierCurve();

                BezierTrajectoryExtraInfo extraInfo;
                extraInfo.m_raw = _shape->getData();
                u32 type = extraInfo.m_type;
                u32 indexNode = extraInfo.m_index;


                const SafeArray<PickingShape*>* shapes = EDITOR->getPickingShapesForObject(_shape->getOwner());

                if (type == TYPE_SHAPE_POINT)
                {
                    if (indexNode < bezier.getNumPoints())
                    {
                        UNDOREDO_MANAGER->preChange(owner);

                        if (shapes)
                        {
                            SafeArray<PickingShape*> shapesCopy = *shapes;

                            for (u32 i = 0; i < shapesCopy.size(); i++)
                            {
                                PickingShape* curShape = shapesCopy[i];

                                BezierTrajectoryExtraInfo extraInfoCheck;
                                extraInfoCheck.m_raw = curShape->getData();
                                
                                if (extraInfoCheck.m_index == indexNode && extraInfoCheck.m_type != TYPE_SHAPE_POINT)
                                {
                                    if (EDITOR->getIndexOfShapeInSelection(curShape) == -1)
                                    {
                                        EDITOR->deletePickingShape(curShape);
                                    }
                                }
                            }
                        }

                        //nodeArray.eraseKeepOrder(indexNode);
                        bezier.removePointAt(indexNode);
                        shapeDeleted = EDITOR->deletePickingShape(_shape);
                        PLUGINGATEWAY->onObjectChanged(owner);

                        // Shift index in extraInfo
                        u32 count = shapes ? shapes->size() : 0;
                        for (u32 i = 0; i < count; i++)
                        {
                            PickingShape* shiftShape = (*shapes)[i];

                            BezierTrajectoryExtraInfo extraInfoShift;
                            extraInfoShift.m_raw = shiftShape->getData();

                            if (extraInfoShift.m_index > indexNode)
                            {                                
                                extraInfoShift.m_index--;
                                shiftShape->setData(extraInfoShift.m_raw);
                            }                       
                        }

                    }
                    else
                    {
                        ITF_ASSERT_MSG(0, "Shape index is invalid !");
                    }   

                }
                else if (type == TYPE_SHAPE_TANGA || type == TYPE_SHAPE_TANGB)
                {                        
                    if (shapes)
                    {
                        SafeArray<PickingShape*> shapesCopy = *shapes;

                        for (u32 i = 0; i < shapesCopy.size(); i++)
                        {
                            PickingShape* curShape = shapesCopy[i];

                            BezierTrajectoryExtraInfo extraInfoCheck;
                            extraInfoCheck.m_raw = curShape->getData();

                            if (extraInfoCheck.m_index == indexNode && extraInfoCheck.m_type == TYPE_SHAPE_POINT)
                            {
                                if (EDITOR->getIndexOfShapeInSelection(curShape) != -1)
                                {
                                    shapeDeleted = EDITOR->deletePickingShape(_shape);
                                }
                            }
                        }
                    }
                }        

            }
        }

        return shapeDeleted;
    }

    // ********************************************************************

    void BezierCurve_Plugin::onUpdate()
    {
        // none
    }

    // ********************************************************************

    void BezierCurve_Plugin::onUpdateSelectedActor(Actor* _pActor, ActorComponent* _component)
    {

        BezierCurveComponent* trajCompo = static_cast<BezierCurveComponent*>(_component);
        BezierCurve & bezier = trajCompo->getBezierCurve();
        Vec3d initPosActor = _pActor->getBoundWorldInitialPos();

        if (bezier.getNumPoints() == 0)
            return;

        Vec3d prevPos, curPos;  
        Vec3d p0, p1, p2, p3;

        for (u32 i = 0; i < bezier.getNumPoints() - 1; i++)
        {
            BezierCurve::Point & pointA = bezier.getPointAt(i);
            BezierCurve::Point & pointB = bezier.getPointAt(i+1);
            p0 = trajCompo->actorTransform(pointA.m_pos);
            p1 = trajCompo->actorTransform(pointA.m_tanB);
            p2 = trajCompo->actorTransform(pointB.m_tanA);
            p3 = trajCompo->actorTransform(pointB.m_pos);

            // Display tangents lines
            DebugDraw::line3D(trajCompo->actorTransform(pointA.m_tanA), trajCompo->actorTransform(pointA.m_pos), Color::yellow(), 2.0f);
            DebugDraw::line3D(trajCompo->actorTransform(pointA.m_pos), trajCompo->actorTransform(pointA.m_tanB), Color::yellow(), 2.0f);

            prevPos   = p0;
            curPos    = p0;

            for (float t = 0.0f; t < 1.0f; t += 0.01f)
            {
                   
                f32 tInv = 1.0f - t;

                // control 4
                curPos =    ( p0 * tInv * tInv * tInv ) 
                                + ( p1 * 3.f * t * tInv * tInv ) 
                                + ( p2 * 3.f * t * t * tInv ) 
                                + ( p3 * t * t * t );

                DebugDraw::line3D(prevPos, curPos, Color::red(), 4.0f);

                prevPos = curPos;

            }
        }

        // Display last tangent 
        if (bezier.getNumPoints() > 0)
        {
            BezierCurve::Point & lastPoint = bezier.getPointAt(bezier.getNumPoints()-1);
            DebugDraw::line3D(trajCompo->actorTransform(lastPoint.m_tanA), trajCompo->actorTransform(lastPoint.m_pos), Color::yellow(), 2.0f);
            DebugDraw::line3D(trajCompo->actorTransform(lastPoint.m_pos), trajCompo->actorTransform(lastPoint.m_tanB), Color::yellow(), 2.0f);
        }
    }

    // ********************************************************************

    bbool BezierCurve_Plugin::onSetShapePosition( PickingShape* _pickingShape, const Vec2d& _screenspacepos )
    {
        if (!_pickingShape->isSubAnchor())
        {
            return bfalse;
        }

        Actor* actor = static_cast<Actor*>(_pickingShape->getOwnerPtr());
        
        if (actor)
        {
            Vec3d initPosActor = actor->getBoundWorldInitialPos();            
            BezierCurveComponent* trajCompo = actor->GetComponent<BezierCurveComponent>();

            if (trajCompo)
            {
                BezierTrajectoryExtraInfo extraInfo;
                extraInfo.m_raw = _pickingShape->getData();
                u32 type = extraInfo.m_type;
                u32 indexNode = extraInfo.m_index;

                // don't move first node
                if (type == TYPE_SHAPE_POINT && indexNode == 0)
                {
                    return btrue;
                }

                BezierCurve & bezier = trajCompo->getBezierCurve();

                if (indexNode >= bezier.getNumPoints())
                    return bfalse;

                PickingShape_Disc* disc = static_cast<PickingShape_Disc*>(_pickingShape);

                Vec3d projectedPos;
                Vec3d screenPos;

                GFX_ADAPTER->compute3DTo2D(disc->getPos(), screenPos);
                screenPos.m_x = _screenspacepos.m_x + _pickingShape->getPickingInfoPtr()->m_anchor.m_x;
                screenPos.m_y = _screenspacepos.m_y + _pickingShape->getPickingInfoPtr()->m_anchor.m_y;

                GFX_ADAPTER->compute2DTo3D(screenPos, projectedPos);


                BezierCurve::Point & point = bezier.getPointAt(indexNode);

                if (type == TYPE_SHAPE_POINT)
                {
                    // Position point
                    Vec3d moveVect = trajCompo->actorUntransform(projectedPos) - point.m_pos;
                    point.m_pos += moveVect;
                    point.m_tanA += moveVect;
                    point.m_tanB += moveVect;

                    return btrue;
                }
                else
                {
                    bbool pointIsSelect = bfalse;
                    bbool tangAIsSelect = bfalse;
                    bbool tangBIsSelect = bfalse;

                    const SafeArray<PickingShape*>* shapes = EDITOR->getPickingShapesForObject(_pickingShape->getOwner());

                    // Check shapes selected
                    if (shapes)
                    {
                        for (u32 i = 0; i < shapes->size(); i++)
                        {
                            PickingShape* curShape = (*shapes)[i];

                            BezierTrajectoryExtraInfo extraInfoCheck;
                            extraInfoCheck.m_raw = curShape->getData();

                            if (extraInfoCheck.m_index == indexNode) //TODO : mettre define
                            {                           
                                if (EDITOR->getIndexOfShapeInSelection(curShape) != -1)
                                {
                                    if (extraInfoCheck.m_type == TYPE_SHAPE_POINT)
                                    {
                                        pointIsSelect = btrue;                                
                                    }
                                    else if (extraInfoCheck.m_type == TYPE_SHAPE_TANGA)
                                    {
                                        tangAIsSelect = btrue;
                                    }
                                    else if (extraInfoCheck.m_type == TYPE_SHAPE_TANGB)
                                    {
                                        tangBIsSelect = btrue;
                                    }
                                }
                            }
                        }
                    }

                    // Move
                    if (pointIsSelect)
                    {
                        return btrue;
                    }
                    else
                    {
                        if (type == TYPE_SHAPE_TANGA)
                        {
                            ITF_ASSERT(tangAIsSelect);
                            if (!tangBIsSelect)
                            {
                                point.m_tanA = trajCompo->actorUntransform(projectedPos);

                                Vec3d moveVec = point.m_pos - point.m_tanA;
                                point.m_tanB = point.m_pos + moveVec;

                                applyTangentLock(trajCompo, indexNode, point);
                            }
                            else
                            {
                                // both tangents selected? move point instead
                                Vec3d moveVect = trajCompo->actorUntransform(projectedPos) - point.m_tanA;
                                point.m_pos += moveVect;
                                point.m_tanA += moveVect;
                                point.m_tanB += moveVect;
                            }

                            return btrue;
                        }
                        else if (type == TYPE_SHAPE_TANGB)
                        {
                            ITF_ASSERT(tangBIsSelect);
                            if (!tangAIsSelect)
                            {
                                point.m_tanB = trajCompo->actorUntransform(projectedPos);

                                Vec3d moveVec = point.m_pos - point.m_tanB;
                                point.m_tanA = point.m_pos + moveVec;

                                applyTangentLock(trajCompo, indexNode, point);
                            }
                            else
                            {
                                // both tangents selected? point moved above (tangent A)                                
                            }

                            return btrue;
                        }
                    }
                }
            }
        }

        return bfalse;
    }

    // ********************************************************************

    bbool BezierCurve_Plugin::onOffsetShapePosition( PickingShape* _pickingShape, const Vec3d& _deltaPos )
    {
        if (!_pickingShape->isSubAnchor())
        {
            return bfalse;
        }

        Actor* actor = static_cast<Actor*>(_pickingShape->getOwnerPtr());
        
        if (actor)
        {
            Vec3d initPosActor = actor->getBoundWorldInitialPos();            
            BezierCurveComponent* trajCompo = actor->GetComponent<BezierCurveComponent>();

            if (trajCompo)
            {
                BezierTrajectoryExtraInfo extraInfo;
                extraInfo.m_raw = _pickingShape->getData();
                u32 type = extraInfo.m_type;
                u32 indexNode = extraInfo.m_index;

                // don't move first node
                if (type == TYPE_SHAPE_POINT && indexNode == 0)
                {
                    return btrue;
                }

                BezierCurve & bezier = trajCompo->getBezierCurve();

                if (indexNode >= bezier.getNumPoints())
                    return bfalse;

                BezierCurve::Point & point = bezier.getPointAt(indexNode);

                Vec3d deltaPosActorSpace = trajCompo->actorUntransform(_deltaPos, bfalse);

                //const bbool dontMove = EDITOR->getMainPickingShapeForObject(_pickingShape->getOwner()) != NULL;

                //if(dontMove)
                //{
                //    return btrue;
                //}
                //else 
                    if (type == TYPE_SHAPE_POINT)
                {
                    point.m_pos += deltaPosActorSpace;
                    point.m_tanA += deltaPosActorSpace;
                    point.m_tanB += deltaPosActorSpace;
                    return btrue;
                }
                else
                {
                    bbool pointIsSelect = bfalse;
                    bbool tangAIsSelect = bfalse;
                    bbool tangBIsSelect = bfalse;
                    const SafeArray<PickingShape*>* shapes = EDITOR->getPickingShapesForObject(_pickingShape->getOwner());

                    // Check shapes selected
                    if (shapes)
                    {
                        for (u32 i = 0; i < shapes->size(); i++)
                        {
                            PickingShape* curShape = (*shapes)[i];

                            BezierTrajectoryExtraInfo extraInfoCheck;
                            extraInfoCheck.m_raw = curShape->getData();

                            if (extraInfoCheck.m_index == indexNode) //TODO : mettre define
                            {                           
                                if (EDITOR->getIndexOfShapeInSelection(curShape) != -1)
                                {
                                    if (extraInfoCheck.m_type == TYPE_SHAPE_POINT)
                                    {
                                        pointIsSelect = btrue;                                
                                    }
                                    else if (extraInfoCheck.m_type == TYPE_SHAPE_TANGA)
                                    {
                                        tangAIsSelect = btrue;
                                    }
                                    else if (extraInfoCheck.m_type == TYPE_SHAPE_TANGB)
                                    {
                                        tangBIsSelect = btrue;
                                    }
                                }
                            }
                        }
                    }

                    // Move
                    if (pointIsSelect)
                    {
                        return btrue;
                    }
                    else
                    {
                        if (type == TYPE_SHAPE_TANGA)
                        {
                            ITF_ASSERT(tangAIsSelect);
                            if (!tangBIsSelect)
                            {
                                point.m_tanA += deltaPosActorSpace;

                                Vec3d moveVec = point.m_pos - point.m_tanA;
                                point.m_tanB = point.m_pos + moveVec;

                                applyTangentLock(trajCompo, indexNode, point);
                            }
                            else
                            {
                                // both tangents selected? move point instead
                                point.m_pos += deltaPosActorSpace;
                                point.m_tanA += deltaPosActorSpace;
                                point.m_tanB += deltaPosActorSpace;
                            }
                            return btrue;
                        }
                        else if (type == TYPE_SHAPE_TANGB)
                        {
                            ITF_ASSERT(tangBIsSelect);
                            if (!tangAIsSelect)
                            {
                                point.m_tanB += deltaPosActorSpace;

                                Vec3d moveVec = point.m_pos - point.m_tanB;
                                point.m_tanA = point.m_pos + moveVec;

                                applyTangentLock(trajCompo, indexNode, point);
                            }
                            else
                            {
                                // both tangents selected? point moved above (tangent A)                                
                            }
                            return btrue;
                        }
                    }
                }
            }
        }

        return bfalse;
    }

    // ********************************************************************

    void BezierCurve_Plugin::addNode(Actor* _actor, ActorComponent* _component, u32 _indexNode, bbool _insertAfter)
    {

        BezierCurveComponent* trajCompo = static_cast<BezierCurveComponent*>(_component);
        BezierCurve & bezier = trajCompo->getBezierCurve();

        Vec3d offset(1.0f, 0.0f, 0.0f);
        Vec3d offsetTang(0.5f, 0.0f, 0.0f);

        if (_indexNode < bezier.getNumPoints())
        {

            BezierCurve::Point & point = bezier.getPointAt(_indexNode);

            UNDOREDO_MANAGER->preChange(_actor);
            
            if (_insertAfter)
            {
                BezierCurve::Point newPoint;
                newPoint.m_pos = point.m_pos + offset;
                newPoint.m_tanA = newPoint.m_pos - offsetTang;
                newPoint.m_tanB = newPoint.m_pos + offsetTang;
                
                if (_indexNode == bezier.getNumPoints() - 1)
                {
                    bezier.addPoint(newPoint);
                }
                else
                {
                    //nodeArray.insert(newNode, _indexNode + 1); 
                    bezier.insertPointBefore(newPoint,_indexNode + 1);
                    //nodeArray.insert(nodeArray.begin() + _indexNode + 1, newNode); 
                }
            }
            else
            {
                // Insert before
                BezierCurve::Point newPoint;
                newPoint.m_pos = point.m_pos - offset;
                newPoint.m_tanA = newPoint.m_pos - offsetTang;
                newPoint.m_tanB = newPoint.m_pos + offsetTang;

                //nodeArray.insert(newNode, _indexNode); 
                bezier.insertPointBefore(newPoint,_indexNode );
                //nodeArray.insert(nodeArray.begin() + _indexNode, newNode); 

            }

            // Delete all shape, and re-create shapes from data
            EDITOR->deletePickingShapesForObject(_actor->getRef());
            ActorEditor::createPickingShapeForActor(_actor->getRef());

            PLUGINGATEWAY->onObjectChanged(_actor);
        }
    }

    // ********************************************************************

    void BezierCurve_Plugin::addPickingShapeSpecificIcons( PickingShape* _shape, SafeArray<Editor_IconStock::EditorIconID>& _icons, f32& _IconSize )
    {
        PickingShape_Disc* disc = static_cast<PickingShape_Disc*>(_shape);

        BezierTrajectoryExtraInfo extraInfo;
        extraInfo.m_raw = disc->getData();
        u32 type = extraInfo.m_type;
       
        _icons.clear();

        // Shape selected ?
        bbool shapeIsSelected = bfalse;
        for (u32 i = 0; i < EDITOR->getSelectionSize(); i++)
        {
            if (EDITOR->getSelectionAt(i) == _shape)
            {
                shapeIsSelected = btrue;
                break;
            }
        }   

        if (type == TYPE_SHAPE_POINT)
        {
            _IconSize = 32.0f;
            if (shapeIsSelected)
            {
                _icons.push_back(Editor_IconStock::Icon_SelectedTrajectoryNode);
            }
            else
            {
                _icons.push_back(Editor_IconStock::Icon_TrajectoryNode);
            }
        }
        else
        {
            _IconSize = 16.0f;
            if (shapeIsSelected)
            {
                _icons.push_back(Editor_IconStock::Icon_SelectedTrajectoryTangNode);
            }
            else
            {
                _icons.push_back(Editor_IconStock::Icon_TrajectoryTangNode);
            }
        }
    }

    // ********************************************************************

    void BezierCurve_Plugin::onObjectChanged( Actor* _actor, ActorComponent* _component )
    {
         BezierCurveComponent* curveCompo = static_cast<BezierCurveComponent*>(_component);

         if (curveCompo)
         {
            curveCompo->init();
         }
    }

    //------------------------------------------------------------------------------
    void BezierCurve_Plugin::onShapePosChanged(Actor* _pActor, ActorComponent* _pComp, PickingShape* _pShape, const Vec3d& _deltaPos)
    {
        BezierCurveComponent* curveCompo = static_cast<BezierCurveComponent*>(_pComp);
        if (!curveCompo)
        {
            return;
        }

        // offset everything except the first node
        if (!_pShape->isSubAnchor()
            && INPUT_ADAPTER->isKeyPressed(KEY_LALT))
        {
            Vec3d moveVect = curveCompo->actorUntransform(-_deltaPos, bfalse);
            BezierCurve & bezier = curveCompo->getBezierCurve();
            for (u32 i=1; i<bezier.getNumPoints(); ++i)
            {

                BezierCurve::Point & point = bezier.getPointAt(i);
                point.m_pos += moveVect;
                point.m_tanA += moveVect;
                point.m_tanB += moveVect;
            }
        }

        // reinit        
        curveCompo->init();
    }

    // ********************************************************************

    void BezierCurve_Plugin::applyTangentLock(BezierCurveComponent* _component, u32 _index, BezierCurve::Point& _point)
    {
        if (_index==0 && _component->getLockFirstTangent())
        {
            _point.m_tanA.m_x = _point.m_pos.m_x;
            _point.m_tanB.m_x = _point.m_pos.m_x;
        }
    }

} // namespace ITF
