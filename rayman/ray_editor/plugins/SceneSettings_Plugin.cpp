#include "precompiled_ray_editor.h"

// GLOBAL ENGINE CONFIG FILE. Must be the first included file
#ifndef _ENGINECOMMON_H_
    #include "core/enginecommon.h"
#endif // _ENGINECOMMON_H_

#ifndef _ITF_SCENESETTINGS_PLUGIN_H_
    #include "rayman/ray_editor/plugins/SceneSettings_Plugin.h"
#endif //_ITF_SCENESETTINGS_PLUGIN_H_

#ifndef _ITF_RAY_SCENESETTINGSCOMPONENT_H_
    #include "rayman/gameplay/Components/Common/Ray_SceneSettingsComponent.h"
#endif //_ITF_RAY_SCENESETTINGSCOMPONENT_H_

#ifndef _ITF_EDITORCONTEXTMENU_H_
    #include "editor/EditorContextMenu.h"
#endif //_ITF_EDITORCONTEXTMENU_H_

namespace ITF
{
    SceneSettings_Plugin* SceneSettings_Plugin::m_instance = NULL;

    Interface_ActorEditorListener* SceneSettings_Plugin::create()
    {
        ITF_ASSERT(!m_instance);
        m_instance = newAlloc(mId_Editor, SceneSettings_Plugin());
        ActorEditor::getInstance()->registerListener(m_instance, Ray_SceneSettingsComponent::GetClassCRCStatic());
        return m_instance;
    }

    void SceneSettings_Plugin::destroy()
    {
        ITF_ASSERT(m_instance);
    }

    void SceneSettings_Plugin::onObjectChanged(Actor* _pActor, ActorComponent* _pComp)
    {
    }

    void SceneSettings_Plugin::onSelected(Actor* _pActor)
    {
    }

    void SceneSettings_Plugin::fillContextMenu(EditorContextMenu& _menu, Actor* _pActor, ActorComponent* _pComp)
    {
    }

    void SceneSettings_Plugin::onContextMenuItemSelected(ContextMenuItem* _item, Actor* _pActor, ActorComponent* _pComp)
    {
    }

    void SceneSettings_Plugin::onUpdate()
    {
    }
    
    void SceneSettings_Plugin::onUpdateSelectedActor(Actor* _pActor, ActorComponent* _pComp)
    {
    }

    void SceneSettings_Plugin::addComponentSpecificIcons(Actor* _pActor, ActorComponent* _pComp, SafeArray<Editor_IconStock::EditorIconID>& _icons, f32& _IconSize)
    {
        _icons.clear();
        _icons.push_back(Editor_IconStock::Icon_YellowSquare);
        _icons.push_back(Editor_IconStock::Icon_Interrogation);
    }

    void    SceneSettings_Plugin::fillMainMenu (EditorContextMenu& _menu)
    {
    }
    
    void    SceneSettings_Plugin::onMainMenuItemSelected(ContextMenuItem* _item)
    {
    }

} // namespace ITF
