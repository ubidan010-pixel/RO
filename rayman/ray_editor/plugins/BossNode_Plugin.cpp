#include "precompiled_ray_editor.h"

// GLOBAL ENGINE CONFIG FILE. Must be the first included file
#ifndef _ENGINECOMMON_H_
    #include "core/enginecommon.h"
#endif // _ENGINECOMMON_H_

#ifndef _ITF_BOSSNODE_PLUGIN_H_
    #include "rayman/ray_editor/plugins/BossNode_Plugin.h"
#endif //_ITF_BOSSNODE_PLUGIN_H_

#ifndef _ITF_RAY_BOSSPLANTNODECOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_BossPlantNodeComponent.h"
#endif //_ITF_RAY_BOSSPLANTNODECOMPONENT_H_

#ifndef _ITF_RAY_BOSSMORAYNODECOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_BossMorayNodeComponent.h"
#endif //_ITF_RAY_BOSSMORAYNODECOMPONENT_H_


#ifndef _ITF_EDITORCONTEXTMENU_H_
    #include "editor/EditorContextMenu.h"
#endif //_ITF_EDITORCONTEXTMENU_H_

namespace ITF
{
    BossNode_Plugin* BossNode_Plugin::m_instance = NULL;

    Interface_ActorEditorListener* BossNode_Plugin::create()
    {
        ITF_ASSERT(!m_instance);
        m_instance = newAlloc(mId_Editor, BossNode_Plugin());
        ActorEditor::getInstance()->registerListener(m_instance, Ray_BossPlantNodeComponent::GetClassCRCStatic());
        ActorEditor::getInstance()->registerListener(m_instance, Ray_BossMorayNodeComponent::GetClassCRCStatic());
        return m_instance;
    }

    void BossNode_Plugin::destroy()
    {
        ITF_ASSERT(m_instance);
    }

    void BossNode_Plugin::onObjectDuplicated( Actor* _pActor, Actor* _pActorSrc, ActorComponent* _pComp )
    {
        _pActor->onEditorCreated(_pActorSrc);
    }

    void BossNode_Plugin::onObjectChanged(Actor* _pActor, ActorComponent* _pComp)
    {
    }

    void BossNode_Plugin::onSelected(Actor* _pActor)
    {
    }

    void BossNode_Plugin::fillContextMenu(EditorContextMenu& _menu, Actor* _pActor, ActorComponent* _pComp)
    {
    }

    void BossNode_Plugin::onContextMenuItemSelected(ContextMenuItem* _item, Actor* _pActor, ActorComponent* _pComp)
    {
    }
    
    void BossNode_Plugin::onUpdateSelectedActor(Actor* _pActor, ActorComponent* _pComp)
    {
    }

    void BossNode_Plugin::addComponentSpecificIcons(Actor* _pActor, ActorComponent* _pComp, SafeArray<Editor_IconStock::EditorIconID>& _icons, f32& _IconSize)
    {
        _icons.clear();
        if (_pActor->isSelected())
            _icons.push_back(Editor_IconStock::Icon_SelectedTrajectoryNodeActor);
        else
            _icons.push_back(Editor_IconStock::Icon_TrajectoryNodeActor);
    }

    void    BossNode_Plugin::fillMainMenu (EditorContextMenu& _menu)
    {
    }
    
    void    BossNode_Plugin::onMainMenuItemSelected(ContextMenuItem* _item)
    {
    }

    void BossNode_Plugin::fillAltContextMenu( EditorContextMenu& _menu )
    {
        String s;
        ContextMenuItem* LDMenuItem = _menu.getItem(EditorContextMenu::ItemId_DBG_MenuLD);
        if (LDMenuItem)
        {
            EditorContextMenu& LDmenu = LDMenuItem->m_subMenu;

            s.setTextFormat("Boss plant nodes %s", Ray_BossPlantNodeComponent::getDrawDebug() ? "ON" : "OFF");
            LDmenu.addSubItemFromExternalPlugin(BossNode_Plant, s, RayItemId_SubMenu, "Boss");

            s.setTextFormat("Boss moray nodes %s", Ray_BossMorayNodeComponent::getDrawDebug() ? "ON" : "OFF");
            LDmenu.addSubItemFromExternalPlugin(BossNode_Moray, s, RayItemId_SubMenu, "Boss");
        }
    }

    void BossNode_Plugin::onAltContextMenuItemSelected( ContextMenuItem* _item )
    {
        switch (_item->getIdFromExternal())
        {
        case BossNode_Plant:
            {
                Ray_BossPlantNodeComponent::setDrawDebug( !Ray_BossPlantNodeComponent::getDrawDebug() );
            }
            break;

        case BossNode_Moray:
            {
                Ray_BossMorayNodeComponent::setDrawDebug( !Ray_BossMorayNodeComponent::getDrawDebug() );
            }
            break;
        }
    }

} // namespace ITF
