#include "precompiled_core.h"

#include "core/system/PS5/SystemModuleManager_PS5.h"

#include <libsysmodule.h>

namespace ITF
{
    void SystemModuleManager_PS5::logMessage(const char* msg)
    {
        if (msg != nullptr)
        {
            printf("%s", msg);
        }
    }

    void SystemModuleManager_PS5::createInstance(LogCallback logCB)
    {
        if (logCB == nullptr)
        {
            logCB = logMessage;
        }
        setInstance(new SystemModuleManager_PS5(logCB));
    }

    i64 SystemModuleManager_PS5::doLoadModule(ModuleId _module)
    {
        return sceSysmoduleLoadModule(_module);
    }

    i64 SystemModuleManager_PS5::doUnloadModule(ModuleId _module)
    {
        return sceSysmoduleUnloadModule(_module);
    }

    i64 SystemModuleManager_PS5::getSuccessCode() const
    {
        return SCE_SYSMODULE_LOADED;
    }

#ifndef ITF_FINAL
    String8 SystemModuleManager_PS5::getModuleName(ModuleId _module) const
    {
        struct ModuleNameInfo
        {
            ModuleId id;
            const char* name;
        };

        static constexpr ModuleNameInfo moduleNames[] =
        {
            ModuleNameInfo{ SCE_SYSMODULE_FIBER, "Fiber library" },
            ModuleNameInfo{ SCE_SYSMODULE_ULT, "Ult library" },
            ModuleNameInfo{ SCE_SYSMODULE_NGS2, "Ngs2 library" },
            ModuleNameInfo{ SCE_SYSMODULE_XML, "Xml library" },
            ModuleNameInfo{ SCE_SYSMODULE_NP_UTILITY, "NpBandwidthTest library" },
            ModuleNameInfo{ SCE_SYSMODULE_VOICE, "Voice library" },
            ModuleNameInfo{ SCE_SYSMODULE_VOICEQOS, "VoiceQos library" },
            ModuleNameInfo{ SCE_SYSMODULE_RUDP, "Rudp library" },
            ModuleNameInfo{ SCE_SYSMODULE_GAME_LIVE_STREAMING, "GameLiveStreaming library" },
            ModuleNameInfo{ SCE_SYSMODULE_PLAYGO, "PlayGo library" },
            ModuleNameInfo{ SCE_SYSMODULE_FONT, "Font library" },
            ModuleNameInfo{ SCE_SYSMODULE_VIDEO_RECORDING, "VideoRecording library" },
            ModuleNameInfo{ SCE_SYSMODULE_AUDIODEC, "Audiodec library" },
            ModuleNameInfo{ SCE_SYSMODULE_JPEG_DEC, "JpegDec library" },
            ModuleNameInfo{ SCE_SYSMODULE_JPEG_ENC, "JpegEnc library" },
            ModuleNameInfo{ SCE_SYSMODULE_PNG_DEC, "PngDec library" },
            ModuleNameInfo{ SCE_SYSMODULE_PNG_ENC, "PngEnc library" },
            ModuleNameInfo{ SCE_SYSMODULE_LIBIME, "Ime library" },
            ModuleNameInfo{ SCE_SYSMODULE_IME_DIALOG, "ImeDialog library" },
            ModuleNameInfo{ SCE_SYSMODULE_FONT_FT, "Font interface for the Font library" },
            ModuleNameInfo{ SCE_SYSMODULE_FREETYPE_OT, "Font driver for the Font library (OpenType only)" },
            ModuleNameInfo{ SCE_SYSMODULE_FREETYPE_OL, "Font driver for the Font library (overall outline fonts including OpenType)" },
            ModuleNameInfo{ SCE_SYSMODULE_FREETYPE_OPT_OL, "Font driver for the Font library (outline fonts other than OpenType)" },
            ModuleNameInfo{ SCE_SYSMODULE_NP_AUTH, "NpAuth library" },
            ModuleNameInfo{ SCE_SYSMODULE_SULPHA, "Sulpha library" },
            ModuleNameInfo{ SCE_SYSMODULE_SAVE_DATA_DIALOG, "SaveDataDialog library" },
            ModuleNameInfo{ SCE_SYSMODULE_MESSAGE_DIALOG, "MsgDialog library" },
            ModuleNameInfo{ SCE_SYSMODULE_AV_PLAYER, "AvPlayer library" },
            ModuleNameInfo{ SCE_SYSMODULE_CONTENT_EXPORT, "ContentExport library" },
            ModuleNameInfo{ SCE_SYSMODULE_AUDIO_3D, "Audio3d library" },
            ModuleNameInfo{ SCE_SYSMODULE_NP_COMMERCE, "NpCommerce and NpCommerceDialog libraries" },
            ModuleNameInfo{ SCE_SYSMODULE_MOUSE, "Mouse library" },
            ModuleNameInfo{ SCE_SYSMODULE_WEB_BROWSER_DIALOG, "WebBrowserDialog library" },
            ModuleNameInfo{ SCE_SYSMODULE_ERROR_DIALOG, "ErrorDialog library" },
            ModuleNameInfo{ SCE_SYSMODULE_APP_CONTENT, "AppContent library" },
            ModuleNameInfo{ SCE_SYSMODULE_REMOTE_PLAY, "Remoteplay library" },
            ModuleNameInfo{ SCE_SYSMODULE_RANDOM, "Random library" },
           // ModuleNameInfo{ SCE_SYSMODULE_M4AAC_ENC, "M4aacEnc library" },
            ModuleNameInfo{ SCE_SYSMODULE_AUDIODEC_CPU, "AudiodecCpu library (wrapper library)" },
            ModuleNameInfo{ SCE_SYSMODULE_CONTENT_SEARCH, "ContentSearch library" },
            ModuleNameInfo{ SCE_SYSMODULE_GAME_UPDATE, "GameUpdate library" },
            ModuleNameInfo{ SCE_SYSMODULE_SYSTEM_GESTURE, "SystemGesture library" },
            ModuleNameInfo{ SCE_SYSMODULE_VIDEODEC2, "Videodec2 library" },
           // ModuleNameInfo{ SCE_SYSMODULE_AT9_ENC, "At9Enc library" },
            ModuleNameInfo{ SCE_SYSMODULE_CONVERT_KEYCODE, "ConvertKeycode library" },
            ModuleNameInfo{ SCE_SYSMODULE_SHARE_PLAY, "SharePlay library" },
            ModuleNameInfo{ SCE_SYSMODULE_AUDIODEC_CPU_HEVAG, "AudiodecCpu library (HE-VAG decoder core)" },
            ModuleNameInfo{ SCE_SYSMODULE_LOGIN_DIALOG, "LoginDialog library" },
            ModuleNameInfo{ SCE_SYSMODULE_LOGIN_SERVICE, "LoginService library" },
            ModuleNameInfo{ SCE_SYSMODULE_SIGNIN_DIALOG, "SigninDialog library" },
            ModuleNameInfo{ SCE_SYSMODULE_VDECSW, "Vdecsw library" },
            ModuleNameInfo{ SCE_SYSMODULE_JSON2, "Json2 library" },
            ModuleNameInfo{ SCE_SYSMODULE_CONTENT_DELETE, "ContentDelete library" },
            ModuleNameInfo{ SCE_SYSMODULE_IME_BACKEND, "ImeBackend library" },
            ModuleNameInfo{ SCE_SYSMODULE_NET_CTL_AP_DIALOG, "NetCtlAp library" },
            ModuleNameInfo{ SCE_SYSMODULE_PLAYGO_DIALOG, "PlayGoDialog library" },
            ModuleNameInfo{ SCE_SYSMODULE_JOB_MANAGER, "JobManager library" },
            ModuleNameInfo{ SCE_SYSMODULE_NP_UNIVERSAL_DATA_SYSTEM, "NpUniversalDataSystem library" },
            ModuleNameInfo{ SCE_SYSMODULE_KEYBOARD, "Keyboard library" },
            ModuleNameInfo{ SCE_SYSMODULE_DECI5, "Deci5 library" },
            ModuleNameInfo{ SCE_SYSMODULE_ACM, "Acm library" },
            ModuleNameInfo{ SCE_SYSMODULE_CES_CS, "Ces library (features for processing CESs other than those that are Unicode CESs)" },
            ModuleNameInfo{ SCE_SYSMODULE_PLAYER_INVITATION_DIALOG, "PlayerInvitationDialog library" },
            ModuleNameInfo{ SCE_SYSMODULE_PLAYER_SELECTION_DIALOG, "PlayerSelectionDialog library" },
            ModuleNameInfo{ SCE_SYSMODULE_SHARE, "Share library" },
            ModuleNameInfo{ SCE_SYSMODULE_NP_TROPHY2, "NpTrophy2 library" },
            ModuleNameInfo{ SCE_SYSMODULE_NP_SESSION_SIGNALING, "NpSessionSignaling library" },
            ModuleNameInfo{ SCE_SYSMODULE_NP_ENTITLEMENT_ACCESS, "NpEntitlementAccess library" },
            ModuleNameInfo{ SCE_SYSMODULE_VOICE_CHAT, "VoiceChat library" },
            ModuleNameInfo{ SCE_SYSMODULE_NP_CPP_WEB_API, "NpCppWebApi library" },
            ModuleNameInfo{ SCE_SYSMODULE_FRONT_PANEL_DISPLAY, "FrontPanelDisplay library" },
            ModuleNameInfo{ SCE_SYSMODULE_AUDIO_PROPAGATION, "AudioPropagation library" },
            ModuleNameInfo{ SCE_SYSMODULE_TEXT_TO_SPEECH2, "TextToSpeech2 library" },
            ModuleNameInfo{ SCE_SYSMODULE_HMD2, "Hmd2 library" },
            ModuleNameInfo{ SCE_SYSMODULE_VR_TRACKER2, "VrTracker2 library" },
            ModuleNameInfo{ SCE_SYSMODULE_VR_SETUP_DIALOG, "VrSetupDialog library" },
            ModuleNameInfo{ SCE_SYSMODULE_PROPRIETARY_VOICE_CHAT_HELPER, "ProprietaryVoiceChatHelper library" },
            ModuleNameInfo{ SCE_SYSMODULE_FONT_GSM, "Font library (graphics output support feature)" },
            ModuleNameInfo{ SCE_SYSMODULE_PSML, "Psml library" },
            ModuleNameInfo{ SCE_SYSMODULE_NP_AUTH_AUTHORIZED_APP_DIALOG, "NpAuthAuthorizedAppDialog library" },
            ModuleNameInfo{ SCE_SYSMODULE_VR_HAND, "VrHand library" },
        };

        auto itFound = std::find_if(std::begin(moduleNames), std::end(moduleNames),
            [_module](const auto & moduleNameInfo)
            {
                return moduleNameInfo.id == _module;
            });

        if (itFound != std::end(moduleNames))
        {
            return String8(itFound->name);
        }

        // Not known, just return the hex value as text
        String8 moduleName;
        moduleName.setTextFormat("0x%04x", static_cast<u16>(_module));
        return moduleName;
    }
#else
    String8 SystemModuleManager_PS5::getModuleName(ModuleId _module) const
    {
        String8 moduleName;
        moduleName.setTextFormat("0x%04x", static_cast<u16>(_module));
        return moduleName;
    }
#endif




}//namespace ITF
