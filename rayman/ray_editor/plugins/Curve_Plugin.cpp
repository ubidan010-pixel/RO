#include "precompiled_ray_editor.h"

// GLOBAL ENGINE CONFIG FILE. Must be the first included file
#ifndef _ENGINECOMMON_H_
#include "core/enginecommon.h"
#endif //_ENGINECOMMON_H_

#ifndef _ITF_CURVE_PLUGIN_H_
#include "rayman/ray_editor/plugins/Curve_Plugin.h"
#endif //_ITF_CURVE_PLUGIN_H_

#ifndef _ITF_EDITORCONTEXTMENU_H_
#include "editor/EditorContextMenu.h"
#endif //_ITF_EDITORCONTEXTMENU_H_

#ifndef _ITF_CURVECOMPONENT_H_
#include "gameplay/Components/Misc/CurveComponent.h"
#endif //_ITF_CURVECOMPONENT_H_

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
    Curve_Plugin* Curve_Plugin::s_instance = NULL;

    Interface_ActorEditorListener* Curve_Plugin::create()
    {
        ITF_ASSERT(!s_instance);
        s_instance = newAlloc(mId_Editor, Curve_Plugin());
        ActorEditor::getInstance()->registerListener(s_instance, CurveComponent::GetClassCRCStatic());
        return s_instance;
    }

    // ********************************************************************

    void Curve_Plugin::destroy()
    {
        ITF_ASSERT(s_instance);
    }

    // ********************************************************************

    Curve_Plugin::Curve_Plugin()
    {
        //none
    }

    // ********************************************************************

    Curve_Plugin::~Curve_Plugin()
    {
        // none
    }

    // ********************************************************************

    void Curve_Plugin::fillContextMenu( EditorContextMenu& _menu, Actor* _actor, ActorComponent* _component )
    {
        if (EDITOR->getSelectionSize() == 1)
        {
             EditorContextMenu& subMenu = _menu.addItemFromExternalPlugin(Curve_SubMenu, "Trajectory Plugin")->m_subMenu;
             subMenu.addItemFromExternalPlugin(Curve_AddBefore, "Add node before");
             subMenu.addItemFromExternalPlugin(Curve_AddAfter, "Add node after");
        }
    }

    // ********************************************************************

    void Curve_Plugin::onContextMenuItemSelected( ContextMenuItem* _item, Actor* _actor, ActorComponent* _component )
    {
        if (EDITOR->getSelectionSize() == 1)
        {
            CurveComponent* trajCompo = static_cast<CurveComponent*>(_component);

            PickingShape* shape = EDITOR->getSelectionAt(0);

            if (shape)
            {
                u32 indexShape = shape->getData();

                switch (_item->getIdFromExternal())
                {
                case Curve_AddBefore:
                    addNode(_actor, trajCompo, indexShape, bfalse);
                    break;

                case Curve_AddAfter:
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

    void Curve_Plugin::fillMainMenu (EditorContextMenu& _menu)
    {
        // none
    }
    
    // ********************************************************************

    void Curve_Plugin::onMainMenuItemSelected(ContextMenuItem* _item)
    {
        // none
    }

    // ********************************************************************

    void Curve_Plugin::addComponentSpecificPickingShape(Actor* _actor, ActorComponent* _component)
    {

        CurveComponent* trajCompo = static_cast<CurveComponent*>(_component);

        CurveComponent::TrajectoryArray& nodeArray = trajCompo->getNodeArrayRef();

        Vec3d initPosActor = _actor->getBoundWorldInitialPos();

        if (nodeArray.size() == 0)
        {
            for (u32 i = 0; i < 4; i++) // TODO : expose default number nodes ?
            {
                CurveComponent::TrajectoryNode newNode;
                newNode.m_pos   = Vec3d(2.f * i,0.0f,0.0f);
                newNode.m_tangA = newNode.m_pos - Vec3d(0.5f, 0.0f, 0.0f);
                newNode.m_tangB = newNode.m_pos + Vec3d(0.5f, 0.0f, 0.0f);
                nodeArray.push_back(newNode);
            }
            _actor->replaceReinitDataByCurrent();
        }

        for (u32 i = 0; i < nodeArray.size(); i++)
        {
            // Create position point
            PickingShape_Disc* discPos = newAlloc(mId_Editor, PickingShape_Disc(this));

            TrajectoryExtraInfo extraInfo;
            extraInfo.m_type = TYPE_SHAPE_POINT;
            extraInfo.m_index = i;

            discPos->setData(extraInfo.m_raw);
            discPos->setPos(trajCompo->actorTransform(nodeArray[i].m_pos));
            discPos->setSubAnchor(btrue);
            EDITOR->addPickingShapeForObject(_actor->getRef(), discPos);

            // Create tangent A
            PickingShape_Disc* discTangA = newAlloc(mId_Editor, PickingShape_Disc(this));

            TrajectoryExtraInfo extraInfoTangA;
            extraInfoTangA.m_type = TYPE_SHAPE_TANGA;
            extraInfoTangA.m_index = i;

            discTangA->setData(extraInfoTangA.m_raw);
            discTangA->setPos(trajCompo->actorTransform(nodeArray[i].m_tangA));
            discTangA->setSubAnchor(btrue);
            EDITOR->addPickingShapeForObject(_actor->getRef(), discTangA);

            // Create tangent B
            PickingShape_Disc* discTangB = newAlloc(mId_Editor, PickingShape_Disc(this));

            TrajectoryExtraInfo extraInfoTangB;
            extraInfoTangB.m_type = TYPE_SHAPE_TANGB;
            extraInfoTangB.m_index = i;

            discTangB->setData(extraInfoTangB.m_raw);
            discTangB->setPos(trajCompo->actorTransform(nodeArray[i].m_tangB));
            discTangB->setSubAnchor(btrue);
            EDITOR->addPickingShapeForObject(_actor->getRef(), discTangB);

        }       
    }

    // ********************************************************************

    void Curve_Plugin::updateComponentSpecificPickingShape(Actor* _actor, PickingShape* _shape, PickingShapeVector& _activeShapes)
    {
        CurveComponent* trajCompo = _actor->GetComponent<CurveComponent>();
        CurveComponent::TrajectoryArray &nodeArray = trajCompo->getNodeArrayRef();

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

        TrajectoryExtraInfo extraInfo;
        extraInfo.m_raw = disc->getData();
        u32 type = extraInfo.m_type;
        u32 indexNode = extraInfo.m_index;

        Vec3d initPosActor = _actor->getBoundWorldInitialPos();

        Vec3d newPos;

        if (type == TYPE_SHAPE_POINT)
        {
           newPos = trajCompo->actorTransform(nodeArray[indexNode].m_pos);
        }
        else if (type == TYPE_SHAPE_TANGA)
        {
            newPos = trajCompo->actorTransform(nodeArray[indexNode].m_tangA);
        }
        else if (type == TYPE_SHAPE_TANGB)
        {
            newPos = trajCompo->actorTransform(nodeArray[indexNode].m_tangB);
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

    bbool Curve_Plugin::onDeletePickingShape( PickingShape* _shape )
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
            CurveComponent* trajCompo = owner->GetComponent<CurveComponent>();
            
            if (trajCompo)
            {
                CurveComponent::TrajectoryArray& nodeArray = trajCompo->getNodeArrayRef();

                TrajectoryExtraInfo extraInfo;
                extraInfo.m_raw = _shape->getData();
                u32 type = extraInfo.m_type;
                u32 indexNode = extraInfo.m_index;


                const SafeArray<PickingShape*>* shapes = EDITOR->getPickingShapesForObject(_shape->getOwner());

                if (type == TYPE_SHAPE_POINT)
                {
                    if (indexNode < nodeArray.size())
                    {
                        UNDOREDO_MANAGER->preChange(owner);

                        if (shapes)
                        {
                            SafeArray<PickingShape*> shapesCopy = *shapes;

                            for (u32 i = 0; i < shapesCopy.size(); i++)
                            {
                                PickingShape* curShape = shapesCopy[i];

                                TrajectoryExtraInfo extraInfoCheck;
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
                        nodeArray.removeAt(indexNode);
                        shapeDeleted = EDITOR->deletePickingShape(_shape);
                        PLUGINGATEWAY->onObjectChanged(owner);

                        // Shift index in extraInfo
                        u32 count = shapes ? shapes->size() : 0;
                        for (u32 i = 0; i < count; i++)
                        {
                            PickingShape* shiftShape = (*shapes)[i];

                            TrajectoryExtraInfo extraInfoShift;
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

                            TrajectoryExtraInfo extraInfoCheck;
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

    void Curve_Plugin::onUpdate()
    {
        // none
    }

    // ********************************************************************

    void Curve_Plugin::onUpdateSelectedActor(Actor* _pActor, ActorComponent* _component)
    {

        CurveComponent* trajCompo = static_cast<CurveComponent*>(_component);
        CurveComponent::TrajectoryArray& nodeArray = trajCompo->getNodeArrayRef();
        Vec3d initPosActor = _pActor->getBoundWorldInitialPos();

        if (nodeArray.size() == 0)
            return;

        Vec3d prevPos, curPos;  
        Vec3d p0, p1, p2, p3;

        for (u32 i = 0; i < nodeArray.size() - 1; i++)
        {
            p0 = trajCompo->actorTransform(nodeArray[i].m_pos);
            p1 = trajCompo->actorTransform(nodeArray[i].m_tangB);
            p2 = trajCompo->actorTransform(nodeArray[i+1].m_tangA);
            p3 = trajCompo->actorTransform(nodeArray[i+1].m_pos);

            // Display tangents lines
            DebugDraw::line3D(trajCompo->actorTransform(nodeArray[i].m_tangA), trajCompo->actorTransform(nodeArray[i].m_pos), Color::yellow(), 2.0f);
            DebugDraw::line3D(trajCompo->actorTransform(nodeArray[i].m_pos), trajCompo->actorTransform(nodeArray[i].m_tangB), Color::yellow(), 2.0f);

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
        if (nodeArray.size() > 0)
        {
            DebugDraw::line3D(trajCompo->actorTransform(nodeArray[nodeArray.size() - 1].m_tangA), trajCompo->actorTransform(nodeArray[nodeArray.size() - 1].m_pos), Color::yellow(), 2.0f);
            DebugDraw::line3D(trajCompo->actorTransform(nodeArray[nodeArray.size() - 1].m_pos), trajCompo->actorTransform(nodeArray[nodeArray.size() - 1].m_tangB), Color::yellow(), 2.0f);
        }
    }

    // ********************************************************************

    bbool Curve_Plugin::onSetShapePosition( PickingShape* _pickingShape, const Vec2d& _screenspacepos )
    {
        if (!_pickingShape->isSubAnchor())
        {
            return bfalse;
        }

        Actor* actor = static_cast<Actor*>(_pickingShape->getOwnerPtr());
        
        if (actor)
        {
            Vec3d initPosActor = actor->getBoundWorldInitialPos();            
            CurveComponent* trajCompo = actor->GetComponent<CurveComponent>();

            if (trajCompo)
            {
                TrajectoryExtraInfo extraInfo;
                extraInfo.m_raw = _pickingShape->getData();
                u32 type = extraInfo.m_type;
                u32 indexNode = extraInfo.m_index;

                // don't move first node
                if (type == TYPE_SHAPE_POINT && indexNode == 0)
                {
                    return btrue;
                }

                CurveComponent::TrajectoryArray& nodeArray = trajCompo->getNodeArrayRef();   

                PickingShape_Disc* disc = static_cast<PickingShape_Disc*>(_pickingShape);

                Vec3d projectedPos;
                Vec3d screenPos;

                GFX_ADAPTER->compute3DTo2D(disc->getPos(), screenPos);
                screenPos.m_x = _screenspacepos.m_x + _pickingShape->getPickingInfoPtr()->m_anchor.m_x;
                screenPos.m_y = _screenspacepos.m_y + _pickingShape->getPickingInfoPtr()->m_anchor.m_y;

                GFX_ADAPTER->compute2DTo3D(screenPos, projectedPos);


                if (type == TYPE_SHAPE_POINT)
                {
                    // Position point
                    Vec3d moveVect = trajCompo->actorUntransform(projectedPos) - nodeArray[indexNode].m_pos;
                    nodeArray[indexNode].m_pos += moveVect;
                    nodeArray[indexNode].m_tangA += moveVect;
                    nodeArray[indexNode].m_tangB += moveVect;

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

                            TrajectoryExtraInfo extraInfoCheck;
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
                                nodeArray[indexNode].m_tangA = trajCompo->actorUntransform(projectedPos);

                                Vec3d moveVec = nodeArray[indexNode].m_pos - nodeArray[indexNode].m_tangA;
                                nodeArray[indexNode].m_tangB = nodeArray[indexNode].m_pos + moveVec;
                            }
                            else
                            {
                                // both tangents selected? move point instead
                                Vec3d moveVect = trajCompo->actorUntransform(projectedPos) - nodeArray[indexNode].m_tangA;
                                nodeArray[indexNode].m_pos += moveVect;
                                nodeArray[indexNode].m_tangA += moveVect;
                                nodeArray[indexNode].m_tangB += moveVect;
                            }

                            return btrue;
                        }
                        else if (type == TYPE_SHAPE_TANGB)
                        {
                            ITF_ASSERT(tangBIsSelect);
                            if (!tangAIsSelect)
                            {
                                nodeArray[indexNode].m_tangB = trajCompo->actorUntransform(projectedPos);

                                Vec3d moveVec = nodeArray[indexNode].m_pos - nodeArray[indexNode].m_tangB;
                                nodeArray[indexNode].m_tangA = nodeArray[indexNode].m_pos + moveVec;
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

    bbool Curve_Plugin::onOffsetShapePosition( PickingShape* _pickingShape, const Vec3d& _deltaPos )
    {
        if (!_pickingShape->isSubAnchor())
        {
            return bfalse;
        }

        Actor* actor = static_cast<Actor*>(_pickingShape->getOwnerPtr());
        
        if (actor)
        {
            Vec3d initPosActor = actor->getBoundWorldInitialPos();            
            CurveComponent* trajCompo = actor->GetComponent<CurveComponent>();

            if (trajCompo)
            {
                TrajectoryExtraInfo extraInfo;
                extraInfo.m_raw = _pickingShape->getData();
                u32 type = extraInfo.m_type;
                u32 indexNode = extraInfo.m_index;

                // don't move first node
                if (type == TYPE_SHAPE_POINT && indexNode == 0)
                {
                    return btrue;
                }

                CurveComponent::TrajectoryArray& nodeArray = trajCompo->getNodeArrayRef();
                Vec3d deltaPosActorSpace = trajCompo->actorUntransform(_deltaPos, bfalse);

                const bbool dontMove = EDITOR->getMainPickingShapeForObject(_pickingShape->getOwner()) != NULL;

                if(dontMove)
                {
                    return btrue;
                }
                else if (type == TYPE_SHAPE_POINT)
                {
                    nodeArray[indexNode].m_pos += deltaPosActorSpace;
                    nodeArray[indexNode].m_tangA += deltaPosActorSpace;
                    nodeArray[indexNode].m_tangB += deltaPosActorSpace;
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

                            TrajectoryExtraInfo extraInfoCheck;
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
                                nodeArray[indexNode].m_tangA += deltaPosActorSpace;

                                Vec3d moveVec = nodeArray[indexNode].m_pos - nodeArray[indexNode].m_tangA;
                                nodeArray[indexNode].m_tangB = nodeArray[indexNode].m_pos + moveVec;
                            }
                            else
                            {
                                // both tangents selected? move point instead
                                nodeArray[indexNode].m_pos += deltaPosActorSpace;
                                nodeArray[indexNode].m_tangA += deltaPosActorSpace;
                                nodeArray[indexNode].m_tangB += deltaPosActorSpace;
                            }
                            return btrue;
                        }
                        else if (type == TYPE_SHAPE_TANGB)
                        {
                            ITF_ASSERT(tangBIsSelect);
                            if (!tangAIsSelect)
                            {
                                nodeArray[indexNode].m_tangB += deltaPosActorSpace;

                                Vec3d moveVec = nodeArray[indexNode].m_pos - nodeArray[indexNode].m_tangB;
                                nodeArray[indexNode].m_tangA = nodeArray[indexNode].m_pos + moveVec;
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

    void Curve_Plugin::addNode(Actor* _actor, ActorComponent* _component, u32 _indexNode, bbool _insertAfter)
    {

        CurveComponent* trajCompo = static_cast<CurveComponent*>(_component);

        CurveComponent::TrajectoryArray& nodeArray = trajCompo->getNodeArrayRef();

        Vec3d offset(1.0f, 0.0f, 0.0f);
        Vec3d offsetTang(0.5f, 0.0f, 0.0f);

        if (_indexNode < nodeArray.size())
        {

            CurveComponent::TrajectoryNode newNode;

            UNDOREDO_MANAGER->preChange(_actor);
            
            if (_insertAfter)
            {
                newNode.m_pos = nodeArray[_indexNode].m_pos + offset;
                newNode.m_tangA = newNode.m_pos - offsetTang;
                newNode.m_tangB = newNode.m_pos + offsetTang;
                
                if (_indexNode == nodeArray.size() - 1)
                {
                    nodeArray.push_back(newNode);
                }
                else
                {
                    //nodeArray.insert(newNode, _indexNode + 1); 
                    nodeArray.insert(nodeArray.begin() + _indexNode + 1, newNode); 
                }
            }
            else
            {
                // Insert before
                newNode.m_pos = nodeArray[_indexNode].m_pos - offset;
                newNode.m_tangA = newNode.m_pos - offsetTang;
                newNode.m_tangB = newNode.m_pos + offsetTang;

                //nodeArray.insert(newNode, _indexNode); 
                nodeArray.insert(nodeArray.begin() + _indexNode, newNode); 

            }

            // Delete all shape, and re-create shapes from data
            EDITOR->deletePickingShapesForObject(_actor->getRef());
            ActorEditor::createPickingShapeForActor(_actor->getRef());

            PLUGINGATEWAY->onObjectChanged(_actor);
        }
    }

    // ********************************************************************

    void Curve_Plugin::addPickingShapeSpecificIcons( PickingShape* _shape, SafeArray<Editor_IconStock::EditorIconID>& _icons, f32& _IconSize )
    {
        PickingShape_Disc* disc = static_cast<PickingShape_Disc*>(_shape);

        TrajectoryExtraInfo extraInfo;
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

    void Curve_Plugin::onObjectChanged( Actor* _actor, ActorComponent* _component )
    {
         CurveComponent* curveCompo = static_cast<CurveComponent*>(_component);

         if (curveCompo)
         {
            curveCompo->init();
         }
    }

    //------------------------------------------------------------------------------
    void Curve_Plugin::onShapePosChanged(Actor* _pActor, ActorComponent* _pComp, PickingShape* _pShape, const Vec3d& _deltaPos)
    {
        CurveComponent* curveCompo = static_cast<CurveComponent*>(_pComp);
        if (!curveCompo)
        {
            return;
        }

        // offset everything except the first node
        if (!_pShape->isSubAnchor()
            && INPUT_ADAPTER->isKeyPressed(KEY_LALT))
        {
            Vec3d moveVect = curveCompo->actorUntransform(-_deltaPos, bfalse);
            CurveComponent::TrajectoryArray& nodeArray = curveCompo->getNodeArrayRef();
            for (u32 i=1; i<nodeArray.size(); ++i)
            {
                nodeArray[i].m_pos += moveVect;
                nodeArray[i].m_tangA += moveVect;
                nodeArray[i].m_tangB += moveVect;
            }
        }

        // reinit        
        curveCompo->init();
    }

    // ********************************************************************


} // namespace ITF
