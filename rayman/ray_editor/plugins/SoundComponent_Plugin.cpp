#include "precompiled_ray_editor.h"

// GLOBAL ENGINE CONFIG FILE. Must be the first included file
#ifndef _ENGINECOMMON_H_
    #include "core/enginecommon.h"
#endif // _ENGINECOMMON_H_

#ifndef _ITF_SOUNDCOMPONENT_PLUGIN_H_
    #include "rayman/ray_editor/plugins/SoundComponent_Plugin.h"
#endif //_ITF_SOUNDCOMPONENT_PLUGIN_H_

#ifndef _ITF_EDITORCONTEXTMENU_H_
    #include "editor/EditorContextMenu.h"
#endif //_ITF_EDITORCONTEXTMENU_H_

#ifndef _ITF_RAY_CONTEXTMENU_H_
#include "rayman/ray_editor/ray_ContextMenuItemsIds.h"
#endif //_ITF_RAY_CONTEXTMENU_H_

#ifndef _ITF_SOUNDCOMPONENT_H_
#include "engine/actors/components/SoundComponent.h"
#endif //_ITF_SOUNDCOMPONENT_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_METRONOMEMANAGER_H_
#include "engine/sound/MetronomeManager.h"
#endif //_ITF_METRONOMEMANAGER_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

namespace ITF
{
    SoundComponent_Plugin* SoundComponent_Plugin::m_instance = NULL;

    Interface_ActorEditorListener* SoundComponent_Plugin::create()
    {
        ITF_ASSERT(!m_instance);
        m_instance = newAlloc(mId_Editor, SoundComponent_Plugin());
        ActorEditor::getInstance()->registerListener(m_instance, SoundComponent::GetClassCRCStatic());
        return m_instance;
    }

    void SoundComponent_Plugin::destroy()
    {
        ITF_ASSERT(m_instance);
    }

    SoundComponent_Plugin::SoundComponent_Plugin()
        : m_debugMetronome(bfalse)
    {
    }

    SoundComponent_Plugin::~SoundComponent_Plugin()
    {
    }


    void SoundComponent_Plugin::onObjectChanged(Actor* _pActor, ActorComponent* _pComp)
    {
    }

    void SoundComponent_Plugin::onSelected(Actor* _pActor)
    {
    }

    EditorContextMenu& SoundComponent_Plugin::createSubMenu( EditorContextMenu& _menu )
    {
        return _menu.addItemFromExternalPlugin(SoundComponent_Item, "Sound")->m_subMenu;
    }

    void SoundComponent_Plugin::fillContextMenu(EditorContextMenu& _menu, Actor* _pActor, ActorComponent* _pComp)
    {
        EditorContextMenu& subMenu = createSubMenu(_menu);

        if (m_showDebugInfoActors.find(_pActor->getRef()) < 0)
            subMenu.addItemFromExternalPlugin(Sound_ShowDebugInfo, "show debug info");
        else
            subMenu.addItemFromExternalPlugin(Sound_ShowDebugInfo, "hide debug info");
    }

    void SoundComponent_Plugin::onContextMenuItemSelected(ContextMenuItem* _item, Actor* _pActor, ActorComponent* _pComp)
    {
        switch (_item->getIdFromExternal())
        {
            case Sound_ShowDebugInfo:
            {
                i32 index = m_showDebugInfoActors.find(_pActor->getRef());
                if (index < 0)
                    m_showDebugInfoActors.push_back(_pActor->getRef());
                else
                    m_showDebugInfoActors.eraseNoOrder(index);
                break;
            }
            break;

            default:
                break;
        }
    }

    void SoundComponent_Plugin::fillAltContextMenu( EditorContextMenu& _menu )
    {
        ContextMenuItem* SoundMenuItem = _menu.getItem(EditorContextMenu::ItemId_DBG_MenuSound);
        if (SoundMenuItem)
        {
            EditorContextMenu& Soundmenu = SoundMenuItem->m_subMenu;

            String s;

            s.setTextFormat("Metronome debug %s", m_debugMetronome ? "ON" : "OFF");
            Soundmenu.addItemFromExternalPlugin(Sound_ShowMetronome, s);

            Soundmenu.addItemFromExternalPlugin(Sound_SetBpm60, "Set BPM 60");
            Soundmenu.addItemFromExternalPlugin(Sound_SetBpm70, "Set BPM 70");
            Soundmenu.addItemFromExternalPlugin(Sound_SetBpm80, "Set BPM 80");
            Soundmenu.addItemFromExternalPlugin(Sound_SetBpm90, "Set BPM 90");
            Soundmenu.addItemFromExternalPlugin(Sound_SetBpm100, "Set BPM 100");
            Soundmenu.addItemFromExternalPlugin(Sound_SetBpm110, "Set BPM 110");
            Soundmenu.addItemFromExternalPlugin(Sound_SetBpm120, "Set BPM 120");
            Soundmenu.addItemFromExternalPlugin(Sound_SetBpm130, "Set BPM 130");
            Soundmenu.addItemFromExternalPlugin(Sound_SetBpm140, "Set BPM 140");
            Soundmenu.addItemFromExternalPlugin(Sound_SetBpm150, "Set BPM 150");
            Soundmenu.addItemFromExternalPlugin(Sound_SetBpm160, "Set BPM 160");
        }
    }

    void SoundComponent_Plugin::onAltContextMenuItemSelected( ContextMenuItem* _item )
    {
        switch (_item->getIdFromExternal())
        {
        case Sound_ShowMetronome:
            m_debugMetronome = !m_debugMetronome;
            break;

        case Sound_SetBpm60:
            setBpm(60);
            break;

        case Sound_SetBpm70:
            setBpm(70);
            break;

        case Sound_SetBpm80:
            setBpm(80);
            break;

        case Sound_SetBpm90:
            setBpm(90);
            break;

        case Sound_SetBpm100:
            setBpm(100);
            break;

        case Sound_SetBpm110:
            setBpm(110);
            break;

        case Sound_SetBpm120:
            setBpm(120);
            break;

        case Sound_SetBpm130:
            setBpm(130);
            break;

        case Sound_SetBpm140:
            setBpm(140);
            break;

        case Sound_SetBpm150:
            setBpm(150);
            break;

        case Sound_SetBpm160:
            setBpm(160);
            break;
        }
    }

    void SoundComponent_Plugin::onUpdate()
    {
    }

    void SoundComponent_Plugin::onDraw()
    {
        debugMarkedActors();

        if (m_debugMetronome)
        {
            debugMetronome();
        }
    }

    void SoundComponent_Plugin::onUpdateSelectedActor(Actor* _pActor, ActorComponent* _pComp)
    {
    }



    void SoundComponent_Plugin::debugMarkedActors()
    {
        for (u32 i = 0; i < m_showDebugInfoActors.size();)
        {
            Actor* pActor = m_showDebugInfoActors[i].getActor();
            if (!pActor)
            {
                m_showDebugInfoActors.eraseNoOrder(i);
                continue;
            }
            GFX_ADAPTER->drawDBGCircle(pActor->getPos(), 1.f);
            i++;
        }
    }


    void SoundComponent_Plugin::debugMetronome()
    {
#ifndef ITF_FINAL
        METRONOME_MANAGER->debugMetronomes();
#endif //ITF_FINAL
    }

    void SoundComponent_Plugin::setBpm( u32 _bpm )
    {
        //METRONOME_MANAGER->reset(METRONOME_TYPE_DEFAULT);
        METRONOME_MANAGER->setTimeSignature(_bpm, 4, 4, METRONOME_TYPE_DEFAULT);
        //METRONOME_MANAGER->start(METRONOME_TYPE_DEFAULT);
    }

    void    SoundComponent_Plugin::fillMainMenu (EditorContextMenu& _menu)
    {
    }
    
    void    SoundComponent_Plugin::onMainMenuItemSelected(ContextMenuItem* _item)
    {
    }

} // namespace ITF
