#include "precompiled_ray_editor.h"

// GLOBAL ENGINE CONFIG FILE. Must be the first included file
#ifndef _ENGINECOMMON_H_
#include "core/enginecommon.h"
#endif //_ENGINECOMMON_H_

#ifndef _ITF_RAY_BEZIERTREE_PLUGIN_H_
#include "rayman/ray_editor/plugins/Ray_BezierTree_Plugin.h"
#endif //_ITF_RAY_BEZIERTREE_PLUGIN_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_PLUGINGATEWAY_H_
#include "PluginGateway/PluginGateway.h"
#endif //_ITF_PLUGINGATEWAY_H_

namespace ITF
{
    Ray_BezierTree_Plugin* Ray_BezierTree_Plugin::s_instance = NULL;

    Interface_ActorEditorListener* Ray_BezierTree_Plugin::create()
    {
        ITF_ASSERT(!s_instance);
        s_instance = newAlloc(mId_Editor, Ray_BezierTree_Plugin());

        ActorEditor::getInstance()->registerListener(s_instance, Ray_BezierTreeAIComponent::GetClassCRCStatic());

        if (INPUT_ADAPTER)
        {
            INPUT_ADAPTER->addListener(s_instance, 0);
        }

        return s_instance;
    }

    void Ray_BezierTree_Plugin::destroy()
    {
        ITF_ASSERT(s_instance);
    }

    Ray_BezierTree_Plugin::Ray_BezierTree_Plugin()
        : m_state(State_None)
    {
    }

    Ray_BezierTree_Plugin::~Ray_BezierTree_Plugin()
    {
    }


    void Ray_BezierTree_Plugin::onCreateShapes( Actor* _actor, Ray_BezierTreeAIComponent* _component )
    {
        for (u32 iB = 0; iB < _component->m_branches.size(); iB++)
        {
            Branch& branch = _component->m_branches[iB];

            for (u32 iN = 0; iN < branch.m_nodes.size(); iN++)
            {
                ShapeData data;
                data.m_branchIndex = iB;
                data.m_nodeIndex = iN;

                data.m_type = ShapeType_NodePos;
                createShape(_actor, data, btrue);

                data.m_type = ShapeType_NodeTangent;
                createShape(_actor, data, bfalse);

                if (!_component->getTemplate()->getLockLastNodeScale() ||
                    iN < branch.m_nodes.size()-1)
                {
                    data.m_type = ShapeType_NodeScale;
                    createShape(_actor, data, bfalse);
                }
            }
        }
    }

    Editor_IconStock::EditorIconID Ray_BezierTree_Plugin::getShapeIcon( Actor* _actor, Ray_BezierTreeAIComponent* _component, PickingShape_Disc* _shape, const ShapeData _shapeData, f32& _iconSize )
    {
        switch (_shapeData.m_type)
        {
        case ShapeType_NodePos:
            {
                return isSelected(_shape) ? Editor_IconStock::Icon_SelectedTrajectoryNode : Editor_IconStock::Icon_TrajectoryNode;
            }
        case ShapeType_NodeTangent:
            {
                _iconSize = 16;
                return isSelected(_shape) ? Editor_IconStock::Icon_SelectedTrajectoryTangNode : Editor_IconStock::Icon_TrajectoryTangNode;
            }
        case ShapeType_NodeScale:
            {
                _iconSize = 16;
                return Editor_IconStock::Icon_Plus;
            }
        }

        ITF_ASSERT(0);
        return Editor_IconStock::Icon_PolyLine_Cut;
    }

    bbool Ray_BezierTree_Plugin::onUpdateShape( Actor* _actor, Ray_BezierTreeAIComponent* _component, PickingShape_Disc* _shape, const ShapeData _shapeData )
    {
        if ( _shapeData.m_branchIndex >= _component->m_branches.size() ||
             _shapeData.m_nodeIndex >= _component->m_branches[_shapeData.m_branchIndex].m_nodes.size() )
        {
            return bfalse;
        }

        const Transform3d worldTransform = getWorldInitTransform(_actor);
        Branch& branch = _component->m_branches[_shapeData.m_branchIndex];
        Node& node = branch.m_nodes[_shapeData.m_nodeIndex];

        const Vec3d nodePos = worldTransform.transformPos(node.m_initialPos);

        switch (_shapeData.m_type)
        {
        case ShapeType_NodePos:
            {
                _shape->setPos(nodePos);
            }
            break;
        case ShapeType_NodeTangent:
            {
                const Vec3d shapePos = worldTransform.transformPos(node.m_initialPos + node.getInitialTangent());
                _shape->setPos(shapePos);
                _shape->setAngle( worldTransform.transformAngle(node.m_initialAngle) );

                DebugDraw::line3D(nodePos, shapePos, Color::yellow(), 2.0f);
            }
            break;
        case ShapeType_NodeScale:
            {
                const Vec3d perp = node.getInitialTangent().getPerpendicular().normalize();
                const Vec3d shapePos = worldTransform.transformPos(node.m_initialPos + node.m_initialScale * perp);
                _shape->setPos(shapePos);
                DebugDraw::line3D(nodePos, shapePos, Color::green());
            }
            break;
        }

        return btrue;
    }

    void Ray_BezierTree_Plugin::onSetShapePos( Actor* _actor, Ray_BezierTreeAIComponent* _component, PickingShape_Disc* _shape, const ShapeData _shapeData, const Vec3d& _worldPos )
    {
        if ( _shapeData.m_branchIndex >= _component->m_branches.size() ||
             _shapeData.m_nodeIndex >= _component->m_branches[_shapeData.m_branchIndex].m_nodes.size() )
        {
            return;
        }

        const Transform3d worldTransform = getWorldInitTransform(_actor);
        Branch& branch = _component->m_branches[_shapeData.m_branchIndex];
        Node& node = branch.m_nodes[_shapeData.m_nodeIndex];

        const Vec3d localPos = worldTransform.inverseTransformPos(_worldPos);

        switch (_shapeData.m_type)
        {
        case ShapeType_NodePos:
            {
                node.m_initialPos = localPos;
            }
            break;
        case ShapeType_NodeTangent:
            {
                node.setInitialTangent(localPos - node.m_initialPos);
            }
            break;
        case ShapeType_NodeScale:
            {
                node.m_initialScale = (localPos - node.m_initialPos).norm();
            }
            break;
        }

        reinit(_component);
    }



    void Ray_BezierTree_Plugin::onUpdate()
    {
        m_state = State_None;

        if ( Ray_BezierTreeAIComponent* component = getSelectedComponent() )
        {
            if ( INPUT_ADAPTER->isKeyPressed(KEY_LSHIFT) ||
                 INPUT_ADAPTER->isKeyPressed(KEY_RSHIFT) )
            {
                m_state = State_Add;
            }
        }

        if ( Actor* actor = m_justAddedActor.getActor() )
        {
            m_justAddedActor.invalidate();

            recreateShapes(actor->getRef());

            const SafeArray<PickingShape*>* shapes = EDITOR->getPickingShapesForObject(actor->getRef());
            ITF_ASSERT(shapes);
            if (shapes)
            {
                for (u32 i = 0; i < shapes->size(); i++)
                {
                    PickingShape* shape = (*shapes)[i];
                    if (shape->getCreatorPickingShapePlugin() == this)
                    {
                        ShapeData data = getData(shape);
                        if (data.m_branchIndex == m_justAddedData.m_branchIndex &&
                            data.m_nodeIndex == m_justAddedData.m_nodeIndex)
                        {
                            Editor::get().selectShape(shape, btrue);
                            break;
                        }
                    }
                }
            }
        }
    }

    void Ray_BezierTree_Plugin::onUpdate( Actor* _actor, Ray_BezierTreeAIComponent* _component )
    {
        f32 expectedCount = 0;
        for (u32 iB = 0; iB < _component->m_branches.size(); iB++)
        {
            Branch& branch = _component->m_branches[iB];
            expectedCount += branch.m_nodes.size() * 3; // 3 shapes: pos, tangent, scale
            if (branch.m_nodes.size() && _component->getTemplate()->getLockLastNodeScale())
            {
                expectedCount--;
            }
        }

        if (getShapeCount(_actor->getRef()) != expectedCount)
        {
            recreateShapes(_actor->getRef());
        }
    }

    bbool Ray_BezierTree_Plugin::onMouseButton( InputAdapter::MouseButton _button, InputAdapter::PressStatus _status )
    {
        if ( _button == InputAdapter::MB_Left &&
             _status == InputAdapter::Pressed )
        {
            if ( Ray_BezierTreeAIComponent* component = getSelectedComponent() )
            {
                if ( m_state == State_Add )
                {
                    ShapeData data;
                    if ( getSelectedSubShape(&data) )
                    {
                        addNode(component->GetActor(), component, data);
                    }
                    else
                    {
                        addBranch(component->GetActor(), component);
                    }
                    return bfalse;
                }
            }
        }

        return btrue;
    }

    void Ray_BezierTree_Plugin::addBranch( Actor* _actor, Ray_BezierTreeAIComponent* _component )
    {
        UNDOREDO_MANAGER->preChange(_actor);
        {
            const Transform3d worldTransform = getWorldInitTransform(_actor);
            const Vec3d mousePos = EDITOR->getMouseOnCurrentWorkingDepth();
            const Vec3d localPos = worldTransform.inverseTransformPos(mousePos);

            _component->m_branches.push_back(Branch());
            Branch& branch = _component->m_branches.back();

            if (!_component->getTemplate()->getLockFirstNode())
            {
                // create an editable first node at 0
                branch.m_nodes.push_back(Node());
                Node& node = branch.m_nodes.back();
                node.m_initialPos = Vec3d::Zero;
                node.m_initialAngle = Vec3d_getAngle(localPos);
            }

            branch.m_nodes.push_back(Node());
            Node& node = branch.m_nodes.back();
            node.m_initialPos = localPos;
            node.m_initialAngle = Vec3d_getAngle(localPos);


            m_justAddedActor = _actor->getRef();
            m_justAddedData.m_branchIndex = _component->m_branches.size()-1;
            m_justAddedData.m_nodeIndex = branch.m_nodes.size()-1;

            reinitFull(_component);
        }
        PLUGINGATEWAY->onObjectChanged(_actor);
    }

    void Ray_BezierTree_Plugin::addNode( Actor* _actor, Ray_BezierTreeAIComponent* _component, const ShapeData _shapeData )
    {
        UNDOREDO_MANAGER->preChange(_actor);
        {
            const Transform3d worldTransform = getWorldInitTransform(_actor);
            const Vec3d mousePos = EDITOR->getMouseOnCurrentWorkingDepth();
            const Vec3d localPos = worldTransform.inverseTransformPos(mousePos);

            const u32 newNodeIndex = _shapeData.m_nodeIndex + 1;

            Branch& branch = _component->m_branches[_shapeData.m_branchIndex];
            branch.m_nodes.insert(Node(), newNodeIndex);
            Node& node = branch.m_nodes[newNodeIndex];

            node.m_initialPos = localPos;

            const Vec3d p0 = branch.m_nodes[_shapeData.m_nodeIndex].m_initialPos;
            const Vec3d p1 = (newNodeIndex == branch.m_nodes.size()-1) ? localPos : branch.m_nodes[newNodeIndex + 1].m_initialPos;
            const Vec3d dir0 = (localPos - p0).normalize();
            const Vec3d dir1 = (p1 - localPos).normalize();
            node.m_initialAngle = Vec3d_getAngle( (dir0 + dir1).normalize() );


            m_justAddedActor = _actor->getRef();
            m_justAddedData.m_branchIndex = _shapeData.m_branchIndex;
            m_justAddedData.m_nodeIndex = newNodeIndex;

            reinitFull(_component);
        }
        PLUGINGATEWAY->onObjectChanged(_actor);
    }

    bbool Ray_BezierTree_Plugin::onDeleteShape( Actor* _actor, Ray_BezierTreeAIComponent* _component, PickingShape_Disc* _shape, const ShapeData _shapeData )
    {
        if (_shapeData.m_type != ShapeType_NodePos)
        {
            return btrue;
        }

        if (!_component->m_branches.size())
        {
            // if we have a picking shape on the first node, deleting the penultimate node will delete the whole branch;
            // so if we delete the last 2 nodes at once, this is normal.
            ITF_ASSERT(!_component->getTemplate()->getLockFirstNode());
            return btrue;
        }

        UNDOREDO_MANAGER->preChange(_actor);
        {
            const u32 minNodeCount = _component->getTemplate()->getLockFirstNode() ? 1 : 2;
            Branch& branch = _component->m_branches[_shapeData.m_branchIndex];
            if (branch.m_nodes.size() > minNodeCount)
            {
                // delete node
                branch.m_nodes[_shapeData.m_nodeIndex].clear();
                branch.m_nodes.eraseKeepOrder(_shapeData.m_nodeIndex);

                // when deleting multiple shapes at once, the indices stored in the shapes are wrong: we need to offset them
                const SafeArray<PickingShape*>* shapes = EDITOR->getPickingShapesForObject(_actor->getRef());
                if (shapes)
                {
                    for (u32 i = 0; i < shapes->size(); i++)
                    {
                        PickingShape* shape = (*shapes)[i];
                        if (shape == _shape)
                        {
                            continue;
                        }
                        if (shape->getCreatorPickingShapePlugin() == this)
                        {
                            ShapeData data = getData(shape);
                            if (data.m_branchIndex == _shapeData.m_branchIndex &&
                                data.m_nodeIndex > _shapeData.m_nodeIndex)
                            {
                                data.m_nodeIndex--;
                                shape->setData(data.m_raw);
                            }
                        }
                    }
                }
            }
            else
            {
                // delete branch
                branch.clear();
                _component->m_branches.removeAt(_shapeData.m_branchIndex);

                // when deleting multiple shapes at once, the indices stored in the shapes are wrong: we need to offset them
                const SafeArray<PickingShape*>* shapes = EDITOR->getPickingShapesForObject(_actor->getRef());
                if (shapes)
                {
                    for (u32 i = 0; i < shapes->size(); i++)
                    {
                        PickingShape* shape = (*shapes)[i];
                        if (shape == _shape)
                        {
                            continue;
                        }
                        if (shape->getCreatorPickingShapePlugin() == this)
                        {
                            ShapeData data = getData(shape);
                            if (data.m_branchIndex > _shapeData.m_branchIndex)
                            {
                                data.m_branchIndex--;
                                shape->setData(data.m_raw);
                            }
                        }
                    }
                }
            }

            reinitFull(_component);
        }
        PLUGINGATEWAY->onObjectChanged(_actor);
        return btrue;
    }

    void Ray_BezierTree_Plugin::reinit( Ray_BezierTreeAIComponent* _component )
    {
        //_component->clear();
        _component->init();
    }

    void Ray_BezierTree_Plugin::reinitFull( Ray_BezierTreeAIComponent* _component )
    {
        _component->clear();
        _component->init();
    }



    static const f32 s_tweenOffsetValues[] = { 0.f, 1.f/4, 1.f/3, 1.f/2, 2.f/3, 3.f/4 };
    static const String s_tweenOffsetNames[]  = { "0", "1/4", "1/3", "1/2", "2/3", "3/4" };
    static const u32 s_tweenOffsetCount = sizeof(s_tweenOffsetValues) / sizeof(f32);

    void Ray_BezierTree_Plugin::fillContextMenu( EditorContextMenu& _menu, Actor* _actor, ActorComponent* _component )
    {
        Ray_BezierTreeAIComponent* component = static_cast<Ray_BezierTreeAIComponent*>(_component);

        EditorContextMenu& subMenu = _menu.addItemFromExternalPlugin(BezierTree_SubMenu, "Bezier tree...")->m_subMenu;

        subMenu.addItemFromExternalPlugin(BezierTree_Open, "Start open");
        subMenu.addItemFromExternalPlugin(BezierTree_Close, "Start closed");

        if ( PickingShape_Disc* shape = getSelectedSubShape() )
        {
            ShapeData shapeData = getData(shape);
            MenuData menuData;
            menuData.m_branchIndex = shapeData.m_branchIndex;
            menuData.m_nodeIndex = shapeData.m_nodeIndex;
            Node& node = component->m_branches[shapeData.m_branchIndex].m_nodes[shapeData.m_nodeIndex];

            // tween set sub-menu
            {
                ContextMenuItem* subMenuItem = subMenu.addItemFromExternalPlugin(BezierTree_TweenMenu_Set, "Tweening...");

                const TweenInstructionSetList_Template& tplSets = getTweenSetList(component);
                if (tplSets.size())
                {
                    EditorContextMenu& subMenu = subMenuItem->m_subMenu;

                    ContextMenuItem* item = subMenu.addItemFromExternalPlugin(BezierTree_TweenMenu_Set, "- None -");
                    menuData.m_itemIndex = -1;
                    item->m_userData = u64(menuData.m_raw);
                    if (!node.m_tweenSet.isValid())
                    {
                        item->setInactiveColor();
                    }

                    for (u32 i = 0; i < tplSets.size(); i++)
                    {
                        const TweenInstructionSet_Template& tplSet = tplSets[i];
                        ContextMenuItem* item = subMenu.addItemFromExternalPlugin(BezierTree_TweenMenu_Set, tplSet.getName().getDebugString());
                        menuData.m_itemIndex = i;
                        item->m_userData = u64(menuData.m_raw);
                        if (node.m_tweenSet == tplSet.getName())
                        {
                            item->setInactiveColor();
                        }
                    }
                }
                else
                {
                    subMenuItem->setInactiveColor();
                }
            }

            // tween offset sub-menu
            {
                ContextMenuItem* subMenuItem = subMenu.addItemFromExternalPlugin(BezierTree_TweenMenu_Offset, "Offset...");

                const TweenInstructionSetList_Template& tplSets = getTweenSetList(component);
                if (tplSets.size())
                {
                    EditorContextMenu& subMenu = subMenuItem->m_subMenu;

                    for (u32 i = 0; i < s_tweenOffsetCount; i++)
                    {
                        ContextMenuItem* item = subMenu.addItemFromExternalPlugin(BezierTree_TweenMenu_Offset, s_tweenOffsetNames[i]);
                        menuData.m_itemIndex = i;
                        item->m_userData = u64(menuData.m_raw);
                        if (node.m_tweenOffset == s_tweenOffsetValues[i])
                        {
                            item->setInactiveColor();
                        }
                    }
                }
                else
                {
                    subMenuItem->setInactiveColor();
                }
            }

            // spawnable sub-menu
            {
                ContextMenuItem* subMenuItem = subMenu.addItemFromExternalPlugin(BezierTree_SpawnMenu, "Spawn...");

                const SpawnableList& tplSpawnables = component->getTemplate()->getSpawnables();
                if (tplSpawnables.size())
                {
                    EditorContextMenu& subMenu = subMenuItem->m_subMenu;

                    ContextMenuItem* item = subMenu.addItemFromExternalPlugin(BezierTree_SpawnMenu, "- None -");
                    menuData.m_itemIndex = -1;
                    item->m_userData = u64(menuData.m_raw);
                    if (!node.m_spawnableName.isValid())
                    {
                        item->setInactiveColor();
                    }

                    for (u32 i = 0; i < tplSpawnables.size(); i++)
                    {
                        const Spawnable& tplSpawnable = tplSpawnables[i];
                        ContextMenuItem* item = subMenu.addItemFromExternalPlugin(BezierTree_SpawnMenu, tplSpawnable.m_name.getDebugString());
                        menuData.m_itemIndex = i;
                        item->m_userData = u64(menuData.m_raw);
                        if (node.m_spawnableName == tplSpawnable.m_name)
                        {
                            item->setInactiveColor();
                        }
                    }
                }
                else
                {
                    subMenuItem->setInactiveColor();
                }
            }
        }
    }

    void Ray_BezierTree_Plugin::onContextMenuItemSelected( ContextMenuItem* _item, Actor* _actor, ActorComponent* _component )
    {
        Ray_BezierTreeAIComponent* component = static_cast<Ray_BezierTreeAIComponent*>(_component);

        MenuData menuData;
        menuData.m_raw = u32(_item->m_userData);

        switch (_item->getIdFromExternal())
        {
        case BezierTree_Open:
            {
                UNDOREDO_MANAGER->preChange(_actor);
                component->m_ai.editor_setActivated(btrue);
                PLUGINGATEWAY->onObjectChanged(_actor);
            }
            break;

        case BezierTree_Close:
            {
                UNDOREDO_MANAGER->preChange(_actor);
                component->m_ai.editor_setActivated(bfalse);
                PLUGINGATEWAY->onObjectChanged(_actor);
            }
            break;

        case BezierTree_TweenMenu_Set:
            if (_item->m_userData < U32_INVALID)
            {
                setTweenSet(_actor, component, menuData);
            }
            break;

        case BezierTree_TweenMenu_Offset:
            if (_item->m_userData < U32_INVALID)
            {
                setTweenOffset(_actor, component, menuData);
            }
            break;

        case BezierTree_SpawnMenu:
            if (_item->m_userData < U32_INVALID)
            {
                setSpawnable(_actor, component, menuData);
            }
            break;
        }
    }

    const TweenInstructionSetList_Template& Ray_BezierTree_Plugin::getTweenSetList( Ray_BezierTreeAIComponent* _component )
    {
        return _component->getTemplate()->getTweenInterpreter().getInstructionSets();
    }

    void Ray_BezierTree_Plugin::setTweenSet( Actor* _actor, Ray_BezierTreeAIComponent* _component, MenuData& _menuData )
    {
        UNDOREDO_MANAGER->preChange(_actor);
        {
            Branch& branch = _component->m_branches[_menuData.m_branchIndex];
            Node& node = branch.m_nodes[_menuData.m_nodeIndex];

            if (_menuData.m_itemIndex >= 0)
            {
                const TweenInstructionSetList_Template& tplSets = getTweenSetList(_component);
                node.m_tweenSet = tplSets[_menuData.m_itemIndex].getName();
            }
            else
            {
                node.m_tweenSet.invalidate();
                node.m_tween.clear();
            }

            reinitFull(_component);
        }
        PLUGINGATEWAY->onObjectChanged(_actor);
    }

    void Ray_BezierTree_Plugin::setTweenOffset( Actor* _actor, Ray_BezierTreeAIComponent* _component, MenuData& _menuData )
    {
        UNDOREDO_MANAGER->preChange(_actor);
        {
            Branch& branch = _component->m_branches[_menuData.m_branchIndex];
            Node& node = branch.m_nodes[_menuData.m_nodeIndex];

            node.m_tweenOffset = s_tweenOffsetValues[_menuData.m_itemIndex];

            reinitFull(_component);
        }
        PLUGINGATEWAY->onObjectChanged(_actor);
    }

    void Ray_BezierTree_Plugin::setSpawnable( Actor* _actor, Ray_BezierTreeAIComponent* _component, MenuData& _menuData )
    {
        UNDOREDO_MANAGER->preChange(_actor);
        {
            Branch& branch = _component->m_branches[_menuData.m_branchIndex];
            Node& node = branch.m_nodes[_menuData.m_nodeIndex];

            if (_menuData.m_itemIndex >= 0)
            {
                node.m_spawnableName = _component->getTemplate()->getSpawnables()[_menuData.m_itemIndex].m_name;
            }
            else
            {
                node.m_spawnableName.invalidate();
            }

            reinitFull(_component);
        }
        PLUGINGATEWAY->onObjectChanged(_actor);
    }

} // namespace ITF
