#include "precompiled_ray_editor.h"

// GLOBAL ENGINE CONFIG FILE. Must be the first included file
#ifndef _ENGINECOMMON_H_
#include "core/enginecommon.h"
#endif //_ENGINECOMMON_H_

#ifndef _ITF_PLATFORMTREE_PLUGIN_H_
#include "rayman/ray_editor/plugins/PlatformTree_Plugin.h"
#endif //_ITF_PLATFORMTREE_PLUGIN_H_

#ifndef _ITF_EDITORCONTEXTMENU_H_
#include "editor/EditorContextMenu.h"
#endif //_ITF_EDITORCONTEXTMENU_H_

#ifndef _ITF_RAY_PLATFORMTREECOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_PlatformTreeComponent.h"
#endif //_ITF_RAY_PLATFORMTREECOMPONENT_H_

#ifndef _ITF_PLUGINGATEWAY_H_
#include "PluginGateway/PluginGateway.h"
#endif //_ITF_PLUGINGATEWAY_H_

namespace ITF
{
    PlatformTree_Plugin* PlatformTree_Plugin::s_instance = NULL;

    Interface_ActorEditorListener* PlatformTree_Plugin::create()
    {
        ITF_ASSERT(!s_instance);
        s_instance = newAlloc(mId_Editor, PlatformTree_Plugin());
        ActorEditor::getInstance()->registerListener(s_instance, Ray_PlatformTreeComponent::GetClassCRCStatic());
        return s_instance;
    }

    void PlatformTree_Plugin::destroy()
    {
        ITF_ASSERT(s_instance);
    }

    PlatformTree_Plugin::PlatformTree_Plugin()
    {
    }

    PlatformTree_Plugin::~PlatformTree_Plugin()
    {
    }


    void PlatformTree_Plugin::fillContextMenu( EditorContextMenu& _menu, Actor* _actor, ActorComponent* _component )
    {
        const String& subMenuText = "Platform tree";
        _menu.addSubItemFromExternalPlugin(PlatformTree_Open, "Start open", PlatformTree_SubMenu, subMenuText);
        _menu.addSubItemFromExternalPlugin(PlatformTree_Close, "Start closed", PlatformTree_SubMenu, subMenuText);
    }

    void PlatformTree_Plugin::onContextMenuItemSelected( ContextMenuItem* _item, Actor* _actor, ActorComponent* _component )
    {
        Ray_PlatformTreeComponent* tree = _component->DynamicCast<Ray_PlatformTreeComponent>(ITF_GET_STRINGID_CRC(Ray_PlatformTreeComponent,4107374076));

        switch (_item->getIdFromExternal())
        {
        case PlatformTree_Open:
            {
                UNDOREDO_MANAGER->preChange(_actor);
                tree->editor_setActivated(btrue);
                PLUGINGATEWAY->onObjectChanged(_actor);
            }
            break;

        case PlatformTree_Close:
            {
                UNDOREDO_MANAGER->preChange(_actor);
                tree->editor_setActivated(bfalse);
                PLUGINGATEWAY->onObjectChanged(_actor);
            }
            break;

        default:
            break;
        }
    }

    void PlatformTree_Plugin::fillMainMenu (EditorContextMenu& _menu)
    {
    }
    
    void PlatformTree_Plugin::onMainMenuItemSelected(ContextMenuItem* _item)
    {
    }
} // namespace ITF
