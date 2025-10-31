#include "precompiled_Adapter_WWISE.h"
#ifdef todoisap
#if !defined ITF_FINAL && defined ITF_SUPPORT_EDITOR
#include "adapters/Adapter_WWISE/EditorPlugin/WwiseSoundComponent_Plugin.h"
#include "adapters/AudioSerializedData/Wwise/WwiseSoundComponent.h"
#include "engine/scene/scene.h"
#include "editor/editor.h"
#include "editor/EditorDraw.h"
#include "adapters/Adapter_WWISE/EditorPlugin/WWISE_EditorMenuID.h"

namespace	ITF
{
	enum ShowMask
	{
		SHOW_NONE					= 0,
		SHOW_ALL_SOUND_COMPONENTS	= 4,
	};

	WwiseSoundComponent_Plugin *WwiseSoundComponent_Plugin::	m_singleton = NULL;

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//

	Interface_ActorEditorListener *WwiseSoundComponent_Plugin::create()
	{
		AUDIO_ASSERT(!m_singleton, "WwiseSoundComponent_Plugin exist !");
		m_singleton = newAlloc(mId_Editor, WwiseSoundComponent_Plugin());

		ActorEditor::getInstance()->registerListener(m_singleton, SoundComponent::GetClassCRCStatic());
		return m_singleton;
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void WwiseSoundComponent_Plugin::destroy()
	{
		AUDIO_ASSERT(m_singleton, "WwiseSoundComponent_Plugin singleton is !=NULL");
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	WwiseSoundComponent_Plugin::WwiseSoundComponent_Plugin() :
		m_showMask(SHOW_NONE)
	{
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void WwiseSoundComponent_Plugin::fillContextMenu(EditorContextMenu &_menu, Actor *_pActor, ActorComponent *_pComp)
	{
		ContextMenuItem *soundMenuItem = _menu.getItem(EditorContextMenu::ActorCtxtMenu_SoundRangeStart);
		if(!soundMenuItem)
		{
			soundMenuItem = _menu.addItemFromExternalPlugin(EditorContextMenu::ActorCtxtMenu_SoundRangeStart, "Sound");
		}

		if(_pActor->GetComponent<SoundComponent> ())
		{
			EditorContextMenu	&subMenu = soundMenuItem->m_subMenu.addItemFromExternalPlugin((EditorContextMenu::ItemId) WwiseActorCtxtMenu_MenuID_SoundComponent, "Sound Component")->m_subMenu;
			if(m_showMask & SHOW_ALL_SOUND_COMPONENTS)
				subMenu.addItemFromExternalPlugin(WwiseActorCtxtMenu_Cmd_ShowAllSoundComponent, "Hide All");
			else
			{
				if(m_drawSoundComponentActorList.find(_pActor->getRef()) < 0)
					subMenu.addItemFromExternalPlugin(WwiseActorCtxtMenu_Cmd_ShowSoundComponent, "Show This");
				else
					subMenu.addItemFromExternalPlugin(WwiseActorCtxtMenu_Cmd_ShowSoundComponent, "Hide This");

				subMenu.addItemFromExternalPlugin(WwiseActorCtxtMenu_Cmd_ShowAllSoundComponent, "Show All");
			}
		}
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void WwiseSoundComponent_Plugin::onContextMenuItemSelected(ContextMenuItem *_item, Actor *_pActor, ActorComponent *_pComp)
	{
		switch(_item->getIdFromExternal())
		{
		case WwiseActorCtxtMenu_Cmd_ShowSoundComponent:
			{
				const i32	index = m_drawSoundComponentActorList.find(_pActor->getRef());
				if(index < 0)
				{
					m_drawSoundComponentActorList.push_back(_pActor->getRef());
					SoundComponent::s_setDrawMask(SoundComponent::s_getDrawMask() &~SoundComponent::HIDE);
					m_showMask &= ~SHOW_ALL_SOUND_COMPONENTS;
				}
				else
				{
					m_drawSoundComponentActorList.eraseNoOrder(index);
				}
			}
			break;

		case WwiseActorCtxtMenu_Cmd_ShowAllSoundComponent:
			m_showMask ^= SHOW_ALL_SOUND_COMPONENTS;
			if(m_showMask & SHOW_ALL_SOUND_COMPONENTS) SoundComponent::s_setDrawMask(SoundComponent::s_getDrawMask() &~SoundComponent::HIDE);
			break;

		default:
			break;
		}
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void WwiseSoundComponent_Plugin::fillAltContextMenu(EditorContextMenu &_menu)
	{
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void WwiseSoundComponent_Plugin::onAltContextMenuItemSelected(ContextMenuItem *_item)
	{
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void WwiseSoundComponent_Plugin::fillMainMenu(EditorContextMenu &_menu)
	{
		ContextMenuItem *SoundMenuItem = _menu.getItem(EditorContextMenu::AltRClickMenu_SoundRangeStart);

		if(!SoundMenuItem)
		{
			SoundMenuItem = _menu.addItemFromExternalPlugin(EditorContextMenu::AltRClickMenu_SoundRangeStart, "Sound");
		}

		if(SoundMenuItem)
		{
			EditorContextMenu	&soundMenu = SoundMenuItem->m_subMenu;

            
			ContextMenuItem		*soundComponentMenu = soundMenu.addItemFromExternalPlugin(WwiseTopBarMenuID_MenuID_SoundComponent, "Sound Component");
			soundComponentMenu->m_subMenu.addCheckableItemFromExternalPlugin(WwiseTopBarMenuID_Cmd_SoundComponentShowHide, "Show", SoundComponent::s_getDrawMask() & SoundComponent::HIDE?bfalse:btrue);
			soundComponentMenu->m_subMenu.addCheckableItemFromExternalPlugin(WwiseTopBarMenuID_Cmd_SoundComponentShowState, "Show State", SoundComponent::s_getDrawMask() & SoundComponent::SHOW_STATE?btrue:bfalse);
			soundComponentMenu->m_subMenu.addCheckableItemFromExternalPlugin(WwiseTopBarMenuID_Cmd_SoundComponentShowPlaying, "Show Playing", SoundComponent::s_getDrawMask() & SoundComponent::SHOW_PLAYING?btrue:bfalse);
			soundComponentMenu->m_subMenu.addCheckableItemFromExternalPlugin(WwiseTopBarMenuID_Cmd_SoundComponentShowInputs, "Show Inputs", SoundComponent::s_getDrawMask() & SoundComponent::SHOW_INPUTS?btrue:bfalse);
			soundComponentMenu->m_subMenu.addCheckableItemFromExternalPlugin(WwiseTopBarMenuID_Cmd_SoundComponentShowSounds, "Show Sounds", SoundComponent::s_getDrawMask() & SoundComponent::SHOW_SOUNDS?btrue:bfalse);
			soundComponentMenu->m_subMenu.addCheckableItemFromExternalPlugin(WwiseTopBarMenuID_Cmd_SoundComponentShowBanks, "Show Banks", SoundComponent::s_getDrawMask() & SoundComponent::SHOW_BANKS?btrue:bfalse);
			soundComponentMenu->m_subMenu.addCheckableItemFromExternalPlugin(WwiseTopBarMenuID_Cmd_SoundComponentShowAll, "Show All", m_showMask & SHOW_ALL_SOUND_COMPONENTS?btrue:bfalse);
			soundComponentMenu->m_subMenu.addCheckableItemFromExternalPlugin(WwiseTopBarMenuID_Cmd_SoundComponentShowWwiseName, "Show Wwise Name", SoundComponent::s_getDrawMask() & SoundComponent::SHOW_WWISE_NAME?btrue:bfalse);
		}
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	bbool WwiseSoundComponent_Plugin::onMainMenuItemSelected(ContextMenuItem *_item)
	{
		switch(_item->getIdFromExternal())
		{
		case WwiseTopBarMenuID_Cmd_SoundComponentShowHide:
			SoundComponent::s_setDrawMask(SoundComponent::s_getDrawMask() ^ SoundComponent::HIDE);
			break;

		case WwiseTopBarMenuID_Cmd_SoundComponentShowState:
			SoundComponent::s_setDrawMask(SoundComponent::s_getDrawMask() ^ SoundComponent::SHOW_STATE);
			break;

		case WwiseTopBarMenuID_Cmd_SoundComponentShowPlaying:
			SoundComponent::s_setDrawMask(SoundComponent::s_getDrawMask() ^ SoundComponent::SHOW_PLAYING);
			break;

		case WwiseTopBarMenuID_Cmd_SoundComponentShowInputs:
			SoundComponent::s_setDrawMask(SoundComponent::s_getDrawMask() ^ SoundComponent::SHOW_INPUTS);
			break;

		case WwiseTopBarMenuID_Cmd_SoundComponentShowSounds:
			SoundComponent::s_setDrawMask(SoundComponent::s_getDrawMask() ^ SoundComponent::SHOW_SOUNDS);
			break;

		case WwiseTopBarMenuID_Cmd_SoundComponentShowBanks:
			SoundComponent::s_setDrawMask(SoundComponent::s_getDrawMask() ^ SoundComponent::SHOW_BANKS);
			break;

		case WwiseTopBarMenuID_Cmd_SoundComponentShowAll:
			m_showMask ^= SHOW_ALL_SOUND_COMPONENTS;
			if(m_showMask & SHOW_ALL_SOUND_COMPONENTS) SoundComponent::s_setDrawMask(SoundComponent::s_getDrawMask() &~SoundComponent::HIDE);
			break;

		case WwiseTopBarMenuID_Cmd_SoundComponentShowWwiseName:
			SoundComponent::s_setDrawMask(SoundComponent::s_getDrawMask() ^ SoundComponent::SHOW_WWISE_NAME);
			break;

		default:
			return bfalse;
		}
        return btrue;
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void WwiseSoundComponent_Plugin::onDraw()
	{
		drawSoundComponentActors();
	}

	//
	// ===================================================================================================================
	// ===================================================================================================================
	//
	void WwiseSoundComponent_Plugin::drawSoundComponentActors()
	{
		EditorDraw	drawInterface;

		if(m_showMask & SHOW_ALL_SOUND_COMPONENTS)
		{
			const World *world = EDITOR->getCurrentWorld();
			if(!world)
			{
				return;
			}

			const u32	sceneCount = world->getSceneCount();
			for(u32 i = 0; i < sceneCount; i++)
			{
				const Scene					*scene = world->getSceneAt(i);
				const ITF_VECTOR<Actor *>	&actors = scene->getActors();
				for(u32 j = 0; j < actors.size(); j++)
				{
					const Actor *actor = actors[j];

					if(!actor->getTemplate())
					{
						// probably a data error
						continue;
					}

					const SoundComponent	*soundComponent = actor->GetComponent<SoundComponent> ();
					if(soundComponent)
					{
						soundComponent->drawEdit(&drawInterface, (u32) ActorComponent::DrawEditFlag_All);
					}
				}
			}
		}
		else
		{
			for(u32 i = 0; i < m_drawSoundComponentActorList.size();)
			{
				const Actor				*actor = m_drawSoundComponentActorList[i].getActor();
				const SoundComponent	*soundComponent = actor ? actor->GetComponent<SoundComponent> () : NULL;
				if(!soundComponent)
				{
					m_drawSoundComponentActorList.eraseNoOrder(i);
					continue;
				}

				soundComponent->drawEdit(&drawInterface, (u32) ActorComponent::DrawEditFlag_All);
				i++;
			}
		}
	}
}	// namespace ITF
#endif // ITF_FINAL
#endif
