#include "precompiled_ray_editor.h"

// GLOBAL ENGINE CONFIG FILE. Must be the first included file
#ifndef _ENGINECOMMON_H_
#include "core/enginecommon.h"
#endif //_ENGINECOMMON_H_

#ifndef _ITF_TWEENING_PLUGIN_H_
#include "rayman/ray_editor/plugins/Tweening_Plugin.h"
#endif //_ITF_TWEENING_PLUGIN_H_

#ifndef _ITF_EDITORCONTEXTMENU_H_
#include "editor/EditorContextMenu.h"
#endif //_ITF_EDITORCONTEXTMENU_H_

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_WORLD_MANAGER_H_
#include "engine/scene/worldManager.h"
#endif //_ITF_WORLD_MANAGER_H_

#ifndef _ITF_WORLD_H_
#include "engine/scene/world.h"
#endif //_ITF_WORLD_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_EDITORDRAW_H_
#include "editor/EditorDraw.h"
#endif //_ITF_EDITORDRAW_H_

#ifndef _ITF_EDITOR_H_
#include "editor/editor.h"
#endif //_ITF_EDITOR_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_PLUGINGATEWAY_H_
#include "PluginGateway/PluginGateway.h"
#endif //_ITF_PLUGINGATEWAY_H_

namespace ITF
{
    Tweening_Plugin* Tweening_Plugin::s_instance = NULL;

    Interface_ActorEditorListener* Tweening_Plugin::create()
    {
        ITF_ASSERT(!s_instance);
        s_instance = newAlloc(mId_Editor,  Tweening_Plugin());
        ActorEditor::getInstance()->registerListener(s_instance, TweenComponent::GetClassCRCStatic());
        return s_instance;
    }

    void Tweening_Plugin::destroy()
    {
        ITF_ASSERT(s_instance);
    }

    Tweening_Plugin::Tweening_Plugin()
        : m_drawDebugAll(bfalse)
    {
    }

    Tweening_Plugin::~Tweening_Plugin()
    {
    }


    void Tweening_Plugin::fillAltContextMenu( EditorContextMenu& _menu )
    {
        ContextMenuItem* LDMenuItem = _menu.getItem(EditorContextMenu::ItemId_DBG_MenuLD);
        if (LDMenuItem)
        {
            EditorContextMenu& LDmenu = LDMenuItem->m_subMenu;
            String s;

            s.setTextFormat("Tweening: debug all %s", m_drawDebugAll ? "ON" : "OFF");
            LDmenu.addItemFromExternalPlugin(Tweening_ToggleDebugAll, s);

            s.setTextFormat("Tweening: lock %s", TweenComponent::s_debugLock ? "ON" : "OFF");
            LDmenu.addItemFromExternalPlugin(Tweening_ToggleLock, s);
        }
    }

    void Tweening_Plugin::onAltContextMenuItemSelected( ContextMenuItem* _item )
    {
        switch (_item->getIdFromExternal())
        {
        case Tweening_ToggleDebugAll:
            {
                m_drawDebugAll = !m_drawDebugAll;
            }
            break;
        case Tweening_ToggleLock:
            {
                TweenComponent::s_debugLock = !TweenComponent::s_debugLock;
            }
            break;
        }
    }

    void Tweening_Plugin::fillContextMenu( EditorContextMenu& _menu, Actor* _actor, ActorComponent* _component )
    {
        TweenComponent* component = static_cast<TweenComponent*>(_component);
        EditorContextMenu& subMenu = _menu.addItemFromExternalPlugin(Tweening_Menu, "Tweening")->m_subMenu;


        bbool hasDebug = m_drawDebugActors.find(_actor->getRef()) >= 0;
        String s;s.setTextFormat("Debug %s", hasDebug ? "ON" : "OFF");
        subMenu.addItemFromExternalPlugin(Tweening_ToggleDebugActor, s);

        fillContextMenu_StartSet(subMenu, component);

        fillContextMenu_FxTarget(subMenu, component);
    }

    void Tweening_Plugin::onContextMenuItemSelected( ContextMenuItem* _item, Actor* _actor, ActorComponent* _component )
    {
        TweenComponent* component = static_cast<TweenComponent*>(_component);

        switch (_item->getIdFromExternal())
        {
        case Tweening_ToggleDebugActor:
            {
                i32 index = m_drawDebugActors.find(_actor->getRef());
                if (index < 0)
                {
                    m_drawDebugActors.push_back(_actor->getRef());
                }
                else
                {
                    m_drawDebugActors.eraseNoOrder(index);
                }
            }
            break;
        case Tweening_StartSet:
            {
                if (_item->m_userData < U32_INVALID)
                {
                    onContextMenuItemSelected_StartSet(_actor, component, u32(_item->m_userData));
                }
            }
            break;
        case Tweening_FxTarget:
            {
                if (_item->m_userData < U32_INVALID)
                {
                    onContextMenuItemSelected_FxTarget(_actor, component, InstructionData(u32(_item->m_userData)));
                }
            }
            break;
        }
    }


    void Tweening_Plugin::fillContextMenu_StartSet( EditorContextMenu& _menu, TweenComponent* _component )
    {
        ContextMenuItem* subMenuItem = _menu.addItemFromExternalPlugin(Tweening_StartSet, "Start set...");
        EditorContextMenu& subMenu = subMenuItem->m_subMenu;

        ContextMenuItem* item = subMenu.addItemFromExternalPlugin(Tweening_StartSet, "- None -");
        item->m_userData = U32_INVALID-1;

        const InstructionSetList_Template& tplSets = _component->getTemplate()->getInstructionSets();
        for (u32 i = 0; i < tplSets.size(); i++)
        {
            const InstructionSet_Template& tplSet = tplSets[i];

            if (tplSet.getName().isValid())
            {
                ContextMenuItem* item = subMenu.addItemFromExternalPlugin(Tweening_StartSet, tplSet.getName().getDebugString());
                item->m_userData = i;

                if (tplSet.getName() == _component->m_startSet)
                {
                    item->setInactiveColor();
                }
            }
        }

        if (subMenu.getItemsCount() == 0)
        {
            subMenuItem->setInactiveColor();
        }
    }

    void Tweening_Plugin::onContextMenuItemSelected_StartSet( Actor* _actor, TweenComponent* _component, u32 _setIndex )
    {
        UNDOREDO_MANAGER->preChange(_actor);
        {
            const InstructionSetList_Template& tplSets = _component->getTemplate()->getInstructionSets();
            if (_setIndex == U32_INVALID-1)
            {
                _component->m_startSet.invalidate();
            }
            else
            {
                _component->m_startSet = tplSets[_setIndex].getName();
            }
            _component->reinit();
        }
        PLUGINGATEWAY->onObjectChanged(_actor);
    }


    void Tweening_Plugin::fillContextMenu_FxTarget( EditorContextMenu& _menu, TweenComponent* _component )
    {
        Pickable* objectInClipboard = getObjectInClipboard();
        if (!objectInClipboard)
        {
            return;
        }

        String s;
        s.setTextFormat("Set FX target (%s)...", objectInClipboard->getUserFriendly().cStr());
        ContextMenuItem* subMenuItem = _menu.addItemFromExternalPlugin(Tweening_FxTarget, s);
        EditorContextMenu& subMenu = subMenuItem->m_subMenu;

        for (u32 i = 0; i < _component->m_instructionSets.size(); i++)
        {
            TweenComponent::InstructionSet& set = _component->m_instructionSets[i];

            for (u32 j = 0; j < set.m_instructions.size(); j++)
            {
                TweenInstructionFX* fxInstruction = set.m_instructions[j]->DynamicCast<TweenInstructionFX>(ITF_GET_STRINGID_CRC(TweenInstructionFX,3479894667));
                if (fxInstruction && fxInstruction->isSerializable())
                {
                    Pickable* currentTarget = fxInstruction->getTarget();
                    if (currentTarget)
                    {
                        s.setTextFormat("%s-%s (%s)", set.getName().getDebugString(), fxInstruction->getName().getDebugString(), currentTarget->getUserFriendly().cStr());
                    }
                    else
                    {
                        s.setTextFormat("%s-%s", set.getName().getDebugString(), fxInstruction->getName().getDebugString());
                    }

                    InstructionData data;
                    data.m_setIndex = i;
                    data.m_insIndex = j;

                    ContextMenuItem* item = subMenu.addItemFromExternalPlugin(Tweening_FxTarget, s);
                    item->m_userData = data.m_raw;

                    if (currentTarget == objectInClipboard)
                    {
                        item->setInactiveColor();
                    }
                }
            }
        }

        if (subMenu.getItemsCount() == 0)
        {
            subMenuItem->setInactiveColor();
        }
    }

    void Tweening_Plugin::onContextMenuItemSelected_FxTarget( Actor* _actor, TweenComponent* _component, InstructionData _menuData )
    {
        Pickable* objectInClipboard = getObjectInClipboard();
        ITF_ASSERT(objectInClipboard);
        if (!objectInClipboard)
        {
            return;
        }

        UNDOREDO_MANAGER->preChange(_actor);
        {
            TweenComponent::InstructionSet& set = _component->m_instructionSets[_menuData.m_setIndex];
            TweenInstructionFX* fxInstruction = set.m_instructions[_menuData.m_insIndex]->DynamicCast<TweenInstructionFX>(ITF_GET_STRINGID_CRC(TweenInstructionFX,3479894667));
            ITF_ASSERT(fxInstruction && fxInstruction->isSerializable());
            SceneObjectPathUtils::getRelativePathFromObject(_actor, objectInClipboard, fxInstruction->m_target);
        }
        PLUGINGATEWAY->onObjectChanged(_actor);
    }



    void Tweening_Plugin::onDraw()
    {
        EditorDraw drawInterface;

        if (m_drawDebugAll)
        {
            World* world = CURRENTWORLD;
            if (!world)
            {
                return;
            }

            const u32 sceneCount = world->getSceneCount();
            for (u32 i = 0; i < sceneCount; i++)
            {
                Scene* scene = world->getSceneAt(i);
                const PickableList& actors = scene->getActors();
                for (u32 j = 0; j < actors.size(); j++)
                {
                    Actor* actor = (Actor*)actors[j];

                    if (!actor->getTemplate())
                    {
                        // probably a data error
                        continue;
                    }

                    TweenComponent* tween = actor->GetComponent<TweenComponent>();
                    if (tween)
                    {
                        tween->drawEdit(&drawInterface, (u32)ActorComponent::DrawEditFlag_All);
                    }
                }
            }
        }
        else
        {
            for (u32 i = 0; i < m_drawDebugActors.size();)
            {
                Actor* actor = m_drawDebugActors[i].getActor();
                TweenComponent* tween = actor ? actor->GetComponent<TweenComponent>() : NULL;
                if (!tween)
                {
                    m_drawDebugActors.eraseNoOrder(i);
                    continue;
                }

                tween->drawEdit(&drawInterface, (u32)ActorComponent::DrawEditFlag_All);
                i++;
            }
        }
    }



    void Tweening_Plugin::onCreateShapes( Actor* _actor, TweenComponent* _component )
    {
        const Transform3d worldTransform = getWorldInitTransform(_actor, bfalse); // tweening doesn't handle flip

        for (u32 i = 0; i < _component->m_instructionSets.size(); i++)
        {
            InstructionList& instructions = _component->m_instructionSets[i].m_instructions;

            for (u32 j = 0; j < instructions.size(); j++)
            {
                TweenInstruction* instruction = instructions[j];

                if ( TweenBallistic* ballistic = instruction->DynamicCast<TweenBallistic>(ITF_GET_STRINGID_CRC(TweenBallistic,1641159219)) )
                {
                    if (!ballistic->isSerializable())
                    {
                        return;
                    }

                    InstructionData data;
                    data.m_setIndex = i;
                    data.m_insIndex = j;

                    // TARGET
                    {
                        data.m_type =  InstructionDataType_BallisticTarget;
                        createShape(_actor, data, btrue);
                    }


                    // TANGENT
                    if (ballistic->getTemplate()->getTangentMode() == TweenBallistic_Template::TangentMode_Custom)
                    {
                        data.m_type =  InstructionDataType_BallisticTangent;
                        createShape(_actor, data, btrue);
                    }
                }
            }
        }
    }

    Editor_IconStock::EditorIconID Tweening_Plugin::getShapeIcon( Actor* _actor, TweenComponent* _component, PickingShape_Disc* _shape, const InstructionData _shapeData, f32& _iconSize )
    {
        if (_shapeData.m_type == InstructionDataType_BallisticTarget)
        {
            _iconSize = 32;
            if ( isSelected(_shape) )
            {
                return Editor_IconStock::Icon_SelectedTrajectoryNode;
            }
            else
            {
                return Editor_IconStock::Icon_TrajectoryNode;
            }
        }
        else
        {
            _iconSize = 16;
            return Editor_IconStock::Icon_TrajectoryTangNode;
        }
    }

    bbool Tweening_Plugin::onUpdateShape( Actor* _actor, TweenComponent* _component, PickingShape_Disc* _shape, const InstructionData _shapeData )
    {
        const Transform3d worldTransform = getWorldInitTransform(_actor, bfalse); // tweening doesn't handle flip
        TweenInstruction* instruction = getInstruction(_component, _shapeData);

        TweenBallistic* ballistic = instruction->DynamicCast<TweenBallistic>(ITF_GET_STRINGID_CRC(TweenBallistic,1641159219));

        if (_shapeData.m_type == InstructionDataType_BallisticTarget)
        {
            // movement is relative to the actor
            Vec3d pos = worldTransform.transformPos(ballistic->m_movement);
            _shape->setPos(pos);

            if (_actor->isSelected())
            {
                const InstructionSet_Template& tplSet = _component->getTemplate()->getInstructionSets()[_shapeData.m_setIndex];
                DebugDraw::text(pos, Vec2d(-20, 15), Color::white(), DebugDraw::defaultBackgroundColor(), tplSet.getName().getDebugString());
            }
        }
        else
        {
            // startTangent is relative to the instruction
            Transform3d setTransform;
            if ( !_component->getSetTransform(ballistic, setTransform) )
            {
                return bfalse;
            }

            Vec3d localPos = setTransform.transformPos(ballistic->m_startTangent);
            Vec3d worldPos = worldTransform.transformPos(localPos);
            _shape->setPos(worldPos);
            _shape->setAngle(worldTransform.transformAngle(setTransform.getAngle()));

            DebugDraw::line3D(worldTransform.transformPos(setTransform.getPos()), worldPos, Color::yellow(), 2.0f);
        }

        return btrue;
    }

    void Tweening_Plugin::onSetShapePos( Actor* _actor, TweenComponent* _component, PickingShape_Disc* _shape, const InstructionData _shapeData, const Vec3d& _worldPos )
    {
        const Transform3d worldTransform = getWorldInitTransform(_actor, bfalse); // tweening doesn't handle flip
        TweenInstruction* instruction = getInstruction(_component, _shapeData);

        TweenBallistic* ballistic = instruction->DynamicCast<TweenBallistic>(ITF_GET_STRINGID_CRC(TweenBallistic,1641159219));

        if (_shapeData.m_type == InstructionDataType_BallisticTarget)
        {
            // movement is relative to the actor
            ballistic->m_movement = worldTransform.inverseTransformPos(_worldPos);
        }
        else
        {
            // startTangent is relative to the instruction
            Transform3d setTransform;
            if ( _component->getSetTransform(ballistic, setTransform) )
            {
                Vec3d localPos = worldTransform.inverseTransformPos(_worldPos);
                ballistic->m_startTangent = setTransform.inverseTransformPos(localPos);
            }
        }
    }



    TweenInstruction* Tweening_Plugin::getInstruction( TweenComponent* _component, const InstructionData _shapeData )
    {
        InstructionList& instructions = _component->m_instructionSets[_shapeData.m_setIndex].m_instructions;
        TweenInstruction* instruction = instructions[_shapeData.m_insIndex];
        return instruction;
    }

    Pickable* Tweening_Plugin::getObjectInClipboard()
    {
        ObjectPath objectPathInClipboard = ActorEditor::getObjectPathFromClipboard();
        if ( objectPathInClipboard.isValid() &&
             objectPathInClipboard.getIsAbsolute() )
        {
            return SceneObjectPathUtils::getObjectFromAbsolutePath(objectPathInClipboard);
        }

        return NULL;
    }

} // namespace ITF
