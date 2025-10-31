#ifndef __WWISE_EDITORMENUID_H__
#define __WWISE_EDITORMENUID_H__

#if defined todoisap
#include "editor/EditorContextMenu.h"
#include "adapters/Adapter_WWISE/AudioSDK/error.h"

namespace	ITF
{
	// R click + actor selected
	enum WwiseActorCtxtMenuID
	{
		WwiseActorCtxtMenu_MenuID					= EditorContextMenu::ActorCtxtMenu_SoundRangeStart,
		WwiseActorCtxtMenu_MenuID_SoundComponent,
		WwiseActorCtxtMenu_Cmd_ShowSoundComponent,
		WwiseActorCtxtMenu_Cmd_ShowAllSoundComponent,
		WWISEACTORCTXTMENUID_LAST_ID
	};

	AUDIO_STATIC_ASSERT((int)WWISEACTORCTXTMENUID_LAST_ID <= (int)EditorContextMenu::ActorCtxtMenu_SoundRangeStop, "Increase EditorContextMenu::ActorCtxtMenu_SoundRangeStop value");

	// top bar menu
	enum WwiseTopBarMenuID
	{
		WwiseTopBarMenuID_MenuID								= EditorContextMenu::AltRClickMenu_SoundRangeStart,
		WwiseTopBarMenuID_Cmd_DisplaySounds,
		WwiseTopBarMenuID_Cmd_ShowMetronome,
		WwiseTopBarMenuID_Cmd_ShowEffects,
		WwiseTopBarMenuID_MenuID_SoundInstances,
		WwiseTopBarMenuID_Cmd_SoundInstances_ShowAll,
		WwiseTopBarMenuID_Cmd_SoundInstances_ShowOnlySelected,
		WwiseTopBarMenuID_MenuID_SoundComponent,
		WwiseTopBarMenuID_Cmd_SoundComponentShowHide,
		WwiseTopBarMenuID_Cmd_SoundComponentShowState,
		WwiseTopBarMenuID_Cmd_SoundComponentShowSounds,
		WwiseTopBarMenuID_Cmd_SoundComponentShowPlaying,
		WwiseTopBarMenuID_Cmd_SoundComponentShowInputs,
		WwiseTopBarMenuID_Cmd_SoundComponentShowBanks,
		WwiseTopBarMenuID_Cmd_SoundComponentShowAll,
		WwiseTopBarMenuID_Cmd_SoundComponentShowWwiseName,
		WWISETOPBARMENUID_LAST_ID
	};

	AUDIO_STATIC_ASSERT((int)WWISETOPBARMENUID_LAST_ID <= (int)EditorContextMenu::AltRClickMenu_SoundRangeStop, "Increase EditorContextMenu::AltRClickMenu_SoundRangeStop value");
}	// namespace ITF
#endif
#endif // __WWISE_EDITORMENUID_H__
