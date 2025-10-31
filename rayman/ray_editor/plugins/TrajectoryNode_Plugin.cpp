#include "precompiled_ray_editor.h"

// GLOBAL ENGINE CONFIG FILE. Must be the first included file
#ifndef _ENGINECOMMON_H_
    #include "core/enginecommon.h"
#endif // _ENGINECOMMON_H_

#ifndef _ITF_TRAJECTORYNODE_PLUGIN_H_
    #include "rayman/ray_editor/plugins/TrajectoryNode_Plugin.h"
#endif //_ITF_TRAJECTORYNODE_PLUGIN_H_


#ifndef _ITF_TRAJECTORYNODECOMPONENT_H_
#include "gameplay/Components/Trajectory/TrajectoryNodeComponent.h"
#endif //_ITF_TRAJECTORYNODECOMPONENT_H_


#ifndef _ITF_EDITORCONTEXTMENU_H_
    #include "editor/EditorContextMenu.h"
#endif //_ITF_EDITORCONTEXTMENU_H_

namespace ITF
{
    TrajectoryNode_Plugin* TrajectoryNode_Plugin::m_instance = NULL;

    Interface_ActorEditorListener* TrajectoryNode_Plugin::create()
    {
        ITF_ASSERT(!m_instance);
        m_instance = newAlloc(mId_Editor, TrajectoryNode_Plugin());
        ActorEditor::getInstance()->registerListener(m_instance, TrajectoryNodeComponent::GetClassCRCStatic());
        return m_instance;
    }

    void TrajectoryNode_Plugin::destroy()
    {
        ITF_ASSERT(m_instance);
    }

    void TrajectoryNode_Plugin::onObjectDuplicated( Actor* _pActor, Actor* _pActorSrc, ActorComponent* _pComp )
    {
        _pActor->onEditorCreated(_pActorSrc);
    }

    void TrajectoryNode_Plugin::onObjectChanged(Actor* _pActor, ActorComponent* _pComp)
    {
    }

    void TrajectoryNode_Plugin::onSelected(Actor* _pActor)
    {
    }

    void TrajectoryNode_Plugin::fillContextMenu(EditorContextMenu& _menu, Actor* _pActor, ActorComponent* _pComp)
    {
    }

    void TrajectoryNode_Plugin::onContextMenuItemSelected(ContextMenuItem* _item, Actor* _pActor, ActorComponent* _pComp)
    {
    }

    void TrajectoryNode_Plugin::onUpdate()
    {
    }
    
    void TrajectoryNode_Plugin::onUpdateSelectedActor(Actor* _pActor, ActorComponent* _pComp)
    {
    }

    void TrajectoryNode_Plugin::addComponentSpecificIcons(Actor* _pActor, ActorComponent* _pComp, SafeArray<Editor_IconStock::EditorIconID>& _icons, f32& _IconSize)
    {
        _icons.clear();
        if (_pActor->isSelected())
            _icons.push_back(Editor_IconStock::Icon_SelectedTrajectoryNodeActor);
        else
            _icons.push_back(Editor_IconStock::Icon_TrajectoryNodeActor);
    }

    void    TrajectoryNode_Plugin::fillMainMenu (EditorContextMenu& _menu)
    {
    }
    
    void    TrajectoryNode_Plugin::onMainMenuItemSelected(ContextMenuItem* _item)
    {
    }

    void TrajectoryNode_Plugin::fillAltContextMenu( EditorContextMenu& _menu )
    {
        ContextMenuItem* LDMenuItem = _menu.getItem(EditorContextMenu::ItemId_DBG_MenuLD);
        if (LDMenuItem)
        {
            EditorContextMenu& LDmenu = LDMenuItem->m_subMenu;

            String s;

            s.setTextFormat("Trajectory nodes %s", TrajectoryNodeComponent::getDrawDebug() ? "ON" : "OFF");
            LDmenu.addItemFromExternalPlugin(TrajectoryNode_Debug, s);
        }
    }

    void TrajectoryNode_Plugin::onAltContextMenuItemSelected( ContextMenuItem* _item )
    {
        switch (_item->getIdFromExternal())
        {
        case TrajectoryNode_Debug:
            {
                TrajectoryNodeComponent::setDrawDebug( !TrajectoryNodeComponent::getDrawDebug() );
            }
            break;
        }
    }
} // namespace ITF
