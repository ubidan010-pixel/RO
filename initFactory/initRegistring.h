#include "FactoryRegistring.h"



BEGIN_REGISTER


#ifdef ITF_SUPPORT_LUA
REGISTER_ITF(LUAHandler)
#endif //ITF_SUPPORT_LUA


REGISTER_ITF_PARENT(ITF_ThreadManager,ThreadManager)

#if defined(ITF_SUPPORT_NET)
REGISTER_ITF_PARENT(TCPAdapter_ITF,TCPAdapter)
#endif //ITF_SUPPORT_NET

#ifdef ITF_SUPPORT_EDITOR
REGISTER_ITF(Editor)
#endif //ITF_SUPPORT_EDITOR

#ifdef ITF_SUPPORT_FILEMANAGER_REMOTE
REGISTER_ITF_PARENT(FileManager_ITF_Remote,FileManager)
#endif //ITF_SUPPORT_FILEMANAGER_REMOTE

#ifdef ITF_X360
REGISTER_ITF_PARENT(FileManager_ITF_X360,FileManager)
REGISTER_ITF_PARENT(SystemAdapter_X360,SystemAdapter)
REGISTER_ITF_PARENT(NetworkServices_x360,NetworkServices)
REGISTER_ITF_PARENT(Adapter_Savegame_x360, Adapter_Savegame)
REGISTER_ITF_PARENT(RewardAdapter_x360, RewardAdapter)
REGISTER_ITF_PARENT(TRCManagerAdapter_x360, TRCManagerAdapter)
#ifdef ITF_SUPPORT_ONLINETRACKING
REGISTER_ITF_PARENT(OnlineTrackingAdapter_x360, OnlineTrackingAdapter)
#endif //ITF_SUPPORT_ONLINETRACKING
#endif //ITF_X360

#ifdef ITF_PS3
REGISTER_ITF_PARENT(GFXAdapter_GCM,GFXAdapter)
REGISTER_ITF_PARENT(FileManager_ITF_PS3,FileManager)
REGISTER_ITF_PARENT(SystemAdapter_PS3,SystemAdapter)
REGISTER_ITF_PARENT(TRCManagerAdapter_PS3,TRCManagerAdapter)
REGISTER_ITF_PARENT(SoundAdapter_PS3,SoundAdapter)
REGISTER_ITF_PARENT(Adapter_Savegame_PS3, Adapter_Savegame)
REGISTER_ITF_PARENT(RewardAdapter_PS3, RewardAdapter)
REGISTER_ITF_PARENT(NetworkServices_PS3,NetworkServices)
#ifdef ITF_SUPPORT_ONLINETRACKING
REGISTER_ITF_PARENT(OnlineTrackingAdapter_ps3, OnlineTrackingAdapter)
#endif //ITF_SUPPORT_ONLINETRACKING
#endif //ITF_PS3

#ifdef ITF_SUPPORT_WWISE
REGISTER_ITF_PARENT(Adapter_WWISE, Adapter_AudioMiddleware)
#endif
#if defined(ITF_WINDOWS) || defined (ITF_X360)
REGISTER_ITF_PARENT(GFXAdapter_Directx9,GFXAdapter)
#ifdef ITF_SUPPORT_RAKI
REGISTER_ITF_PARENT(SoundAdapter_XAudio,SoundAdapter)
#endif
#endif

#ifdef ITF_WINDOWS
    #ifdef ITF_SUPPORT_PLUGIN
        REGISTER_ITF_PARENT(FreeImageCodec,CodecImageAdapter)
    #endif
REGISTER_ITF_PARENT(FileManager_ITF_Win,FileManager)
REGISTER_ITF_PARENT(SystemAdapter_win, SystemAdapter)
REGISTER_ITF_PARENT(Adapter_Savegame_win, Adapter_Savegame)
REGISTER_ITF_PARENT(TRCManagerAdapter_win, TRCManagerAdapter)
#ifdef ITF_SUPPORT_NETWORKSERVICES
REGISTER_ITF_PARENT(NetworkServices_Win, NetworkServices)
#endif
#ifdef ITF_SUPPORT_UPLAY
REGISTER_ITF_PARENT(UPlayService_Win, UPlayService)
#endif
#ifdef ITF_SUPPORT_ONLINETRACKING
REGISTER_ITF_PARENT(OnlineTrackingAdapter_win32, OnlineTrackingAdapter)
#endif //ITF_SUPPORT_ONLINETRACKING
#ifdef ITF_SUPPORT_UBISERVICES
REGISTER_ITF_PARENT(OnlineAdapter_Ubiservices, OnlineAdapter)
#endif
REGISTER_ITF_PARENT(RewardAdapter_win, RewardAdapter)
#endif  // ITF_WINDOWS

#ifdef ITF_IPAD
REGISTER_ITF_PARENT(GFXAdapter_OpenGLES,GFXAdapter)
REGISTER_ITF_PARENT(FileManager_ITF_iPad,FileManager)
REGISTER_ITF_PARENT(SystemAdapter_iPad,SystemAdapter)
REGISTER_ITF_PARENT(SoundAdapter_iPad,SoundAdapter)
REGISTER_ITF_PARENT(Adapter_Savegame_iPad,Adapter_Savegame)
#endif //ITF_IPAD

#ifdef ITF_CTR
REGISTER_ITF_PARENT(GFXAdapter_3DS_GR,GFXAdapter)
REGISTER_ITF_PARENT(FileManager_ITF_CTR,FileManager)
REGISTER_ITF_PARENT(SystemAdapter_CTR,SystemAdapter)
REGISTER_ITF_PARENT(Adapter_Savegame_CTR, Adapter_Savegame)
REGISTER_ITF_PARENT(SoundAdapter_CTR,SoundAdapter)
#endif

#ifdef ITF_WII
REGISTER_ITF_PARENT(GFXAdapter_WII,GFXAdapter)
REGISTER_ITF_PARENT(FileManager_ITF_WII,FileManager)
REGISTER_ITF_PARENT(SystemAdapter_WII,SystemAdapter)
REGISTER_ITF_PARENT(Adapter_Savegame_WII, Adapter_Savegame)
REGISTER_ITF_PARENT(SoundAdapter_WII,SoundAdapter)
REGISTER_ITF_PARENT(TRCManagerAdapter_WII,TRCManagerAdapter)
#endif

#ifdef ITF_PS5
REGISTER_ITF_PARENT(GFXAdapter_PS5, GFXAdapter)
REGISTER_ITF_PARENT(FileManager_ITF, FileManager)
REGISTER_ITF_PARENT(SystemAdapter_PS5, SystemAdapter)
REGISTER_ITF_PARENT(Adapter_Savegame_PS5, Adapter_Savegame)
REGISTER_ITF_PARENT(NetworkServices_PS5, NetworkServices)
#ifdef ITF_SUPPORT_RAKI
REGISTER_ITF_PARENT(SoundAdapter_Dummy, SoundAdapter)
#endif
REGISTER_ITF_PARENT(TRCManagerAdapter_PS5, TRCManagerAdapter)
REGISTER_ITF_PARENT(RewardAdapter_PS5, RewardAdapter)
REGISTER_ITF_PARENT(AccountAdapter_PS5, AccountAdapter)
#endif

#ifdef ITF_NX
REGISTER_ITF_PARENT(GFXAdapter_NVN, GFXAdapter)
REGISTER_ITF_PARENT(FileManager_ITF, FileManager)
REGISTER_ITF_PARENT(SystemAdapter_Nintendo, SystemAdapter)
REGISTER_ITF_PARENT(Adapter_Savegame_Nintendo, Adapter_Savegame)
#ifdef ITF_SUPPORT_RAKI
REGISTER_ITF_PARENT(SoundAdapter_Dummy, SoundAdapter)
#endif
REGISTER_ITF_PARENT(TRCManagerAdapter_Nintendo, TRCManagerAdapter)
REGISTER_ITF_PARENT(RewardAdapter_Dummy, RewardAdapter)
#endif

#ifdef ITF_OUNCE
REGISTER_ITF_PARENT(GFXAdapter_NVN, GFXAdapter)
REGISTER_ITF_PARENT(FileManager_ITF, FileManager)
REGISTER_ITF_PARENT(SystemAdapter_Nintendo, SystemAdapter)
REGISTER_ITF_PARENT(Adapter_Savegame_Nintendo, Adapter_Savegame)
#ifdef ITF_SUPPORT_RAKI
REGISTER_ITF_PARENT(SoundAdapter_Dummy, SoundAdapter)
#endif
REGISTER_ITF_PARENT(TRCManagerAdapter_Nintendo, TRCManagerAdapter)
REGISTER_ITF_PARENT(RewardAdapter_Dummy, RewardAdapter)
#endif

#ifdef ITF_XBOX_SERIES
REGISTER_ITF_PARENT(GFXAdapter_DX12, GFXAdapter)
REGISTER_ITF_PARENT(FileManager_ITF, FileManager)
REGISTER_ITF_PARENT(SystemAdapter_XBoxSeries, SystemAdapter)
REGISTER_ITF_PARENT(Adapter_Savegame_XBoxSeries, Adapter_Savegame)
#ifdef ITF_SUPPORT_RAKI
REGISTER_ITF_PARENT(SoundAdapter_Dummy, SoundAdapter)
#endif
REGISTER_ITF_PARENT(TRCManagerAdapter_XBoxSeries, TRCManagerAdapter)
REGISTER_ITF_PARENT(AccountAdapter_xboxseries, AccountAdapter)
REGISTER_ITF_PARENT(RewardAdapter_xboxseries, RewardAdapter)
#endif


END_REGISTER
