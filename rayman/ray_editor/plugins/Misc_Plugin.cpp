#include "precompiled_ray_editor.h"

// GLOBAL ENGINE CONFIG FILE. Must be the first included file
#ifndef _ENGINECOMMON_H_
#include "core/enginecommon.h"
#endif //_ENGINECOMMON_H_

#ifndef _ITF_MISC_PLUGIN_H_
#include "rayman/ray_editor/plugins/Misc_Plugin.h"
#endif //_ITF_MISC_PLUGIN_H_

#ifndef _ITF_EDITORCONTEXTMENU_H_
#include "editor/EditorContextMenu.h"
#endif //_ITF_EDITORCONTEXTMENU_H_


#ifndef _ITF_RAY_AVOIDANCEMANAGER_H_
#include "rayman/gameplay/Managers/Ray_AvoidanceManager.h"
#endif //_ITF_RAY_AVOIDANCEMANAGER_H_

#ifndef _ITF_EDITORDRAW_H_
#include "editor/EditorDraw.h"
#endif //_ITF_EDITORDRAW_H_

#ifndef _ITF_EDITOR_H_
#include "editor/editor.h"
#endif //_ITF_EDITOR_H_

#ifndef _ITF_ANIMLIGHTCOMPONENT_H_
#include "engine/actors/components/AnimLightComponent.h"
#endif //_ITF_ANIMLIGHTCOMPONENT_H_

#ifndef _ITF_DEBUGINFO_H_
#include "engine/debug/debugInfo.h"
#endif //_ITF_DEBUGINFO_H_

#ifndef _ITF_PLUGINGATEWAY_H_
#include "PluginGateway/PluginGateway.h"
#endif //_ITF_PLUGINGATEWAY_H_

namespace ITF
{
    Misc_Plugin* Misc_Plugin::s_instance = NULL;

    Interface_ActorEditorListener* Misc_Plugin::create()
    {
        ITF_ASSERT(!s_instance);
        s_instance = new Misc_Plugin();
        ActorEditor::getInstance()->registerListener(s_instance, AnimLightComponent::GetClassCRCStatic());
        return s_instance;
    }

    void Misc_Plugin::destroy()
    {
        ITF_ASSERT(s_instance);
    }

    Misc_Plugin::Misc_Plugin()
    {
    }

    Misc_Plugin::~Misc_Plugin()
    {
    }


    void Misc_Plugin::fillAltContextMenu( EditorContextMenu& _menu )
    {
        String s;

        ContextMenuItem* LDMenuItem = _menu.getItem(EditorContextMenu::ItemId_DBG_MenuLD);
        if (LDMenuItem)
        {
            EditorContextMenu& LDmenu = LDMenuItem->m_subMenu;

            s.setTextFormat("Avoidance manager %s", RAY_AVOIDANCEMANAGER->getDrawDebug() ? "ON" : "OFF");
            LDmenu.addItemFromExternalPlugin(Misc_AvoidanceManager, s);
        }

        ContextMenuItem* gppMenuItem = _menu.getItem(EditorContextMenu::ItemId_DBG_MenuGameplay);
        if (gppMenuItem)
        {
            s.setTextFormat("DBG polyline indices %s", EditorDraw::getShowPolylineEdgeIndices() ? "ON" : "OFF");
            gppMenuItem->m_subMenu.addItemFromExternalPlugin(Misc_PolylineIndices, s);
        }
    }

    void Misc_Plugin::onAltContextMenuItemSelected( ContextMenuItem* _item )
    {
        switch (_item->getIdFromExternal())
        {
        case Misc_AvoidanceManager:
            {
                RAY_AVOIDANCEMANAGER->setDrawDebug( !RAY_AVOIDANCEMANAGER->getDrawDebug() );
            }
            break;
        case Misc_PolylineIndices:
            {
                EditorDraw::showPolylineEdgeIndices(!EditorDraw::getShowPolylineEdgeIndices());
            }
            break;
        }
    }

    void Misc_Plugin::fillContextMenu( EditorContextMenu& _menu, Actor* _actor, ActorComponent* _component )
    {
        _menu.addItemFromExternalPlugin(Misc_RandomizeAnimSyncOffset, "Randomize anim syncOffset");
    }

    void Misc_Plugin::onContextMenuItemSelected( ContextMenuItem* _item, Actor* _actor, ActorComponent* _component )
    {
        switch (_item->getIdFromExternal())
        {
        case Misc_RandomizeAnimSyncOffset:
            {
                randomizeSyncOffsetOnSelection();
            }
            break;
        }
    }

    void Misc_Plugin::randomizeSyncOffsetOnSelection()
    {
        SafeArray<BaseObject*> selection;
        ID_SERVER->getObjectListNoNull(EDITOR->getEditorSelectionAsObjRefList(), selection);

        SafeArray<AnimLightComponent*> anims;

        for (u32 i = 0; i < selection.size(); i++)
        {
            BaseObject* obj = selection[i];
            if (obj->getObjectType() != BaseObject::eActor)
            {
                continue;
            }

            Actor* actor = static_cast<Actor*>(obj);
            if ( AnimLightComponent* anim = actor->GetComponent<AnimLightComponent>() )
            {
                anims.push_back(anim);
            }
        }

        if (anims.size())
        {
            String8 undoSeq;undoSeq.setTextFormat("randomize syncOffset on %d anim(s)", anims.size());
            UNDOREDO_MANAGER->beginUndoRedoSequence(undoSeq);

            for (u32 i = 0; i < anims.size(); i++)
            {
                AnimLightComponent* anim = anims[i];
                Actor* actor = anim->GetActor();

                const f32 syncOffset = Seeder::getSharedSeeder().GetFloat(0.f, 1.f);

                UNDOREDO_MANAGER->preChange(actor);
                anim->editor_setSyncOffset(syncOffset);
                PLUGINGATEWAY->onObjectChanged(actor);

                String s;s.setTextFormat("randomized '%s'     - %.2f", actor->getUserFriendly().cStr(), syncOffset);
                DebugInfo::get().addDebugInfoDisplay(s, 5.f);
            }

            UNDOREDO_MANAGER->validateUndoRedoSequence(undoSeq);
        }
    }

} // namespace ITF
