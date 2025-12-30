#ifndef _ITF_RAY_GAMEOPTIONNAMES_H_
#define _ITF_RAY_GAMEOPTIONNAMES_H_

namespace ITF
{
    enum EHealthModifier
    {
        HealthModifier_Default = 0,
        HealthModifier_RedHeart = 1,
        HealthModifier_GoldenHeart = 2,
        HealthModifier_DiamondHeart = 3,
        START_WITH_HEART_CHOICES = 4,
    };

    enum ERunButtonMode
    {
        RunButtonMode_Hold = 0,
        RunButtonMode_Toggle = 1,
        RUN_BUTTON_CHOICES = 2
    };

    static const u32 RESOLUTION_CHOICES = 4;
    static const u32 LANGUAGE_CHOICES = 15;
    static const u32 DIAMOND_HEART_MAX_CONSECUTIVE_HITS = 10;
    static const f32 DIAMOND_HEART_CONSECUTIVE_HIT_WINDOW = 20.0f;

    static const u32 LINEID_HEART_NONE = 4060;
    static const u32 LINEID_HEART_RED = 7020;
    static const u32 LINEID_HEART_GOLDEN = 7018;
    static const u32 LINEID_HEART_DIAMOND = 7019;

    static const u32 LINEID_RUN_HOLD = 7004;
    static const u32 LINEID_RUN_TOGGLE = 7005;
}

#define OPTION_MENU_NAME "optionmenu"

// Action buttons
#define OPTIONMENU_RESET_TO_DEFAULT_BUTTON    ITF_GET_STRINGID_CRC(reset_to_default_button,2697802978)
#define OPTIONMENU_UBISOFTCONNECT_BUTTON      ITF_GET_STRINGID_CRC(ubisoftconnect_button,3586303504)
#define OPTIONMENU_ACCEPT_BUTTON              ITF_GET_STRINGID_CRC(accept_button,25226343)
#define OPTIONMENU_CANCEL_BUTTON              ITF_GET_STRINGID_CRC(cancel_button,4260770984)

#define OPTION_RESOLUTION               ITF_GET_STRINGID_CRC(Resolution,1477600601)
#define OPTION_WINDOWED                 ITF_GET_STRINGID_CRC(Windowed,1545748265)
#define OPTION_LANGUAGE                 ITF_GET_STRINGID_CRC(Language,3611137521)

#define OPTION_START_WITH_HEART         ITF_GET_STRINGID_CRC(StartWithHeart,1992734674)
#define OPTION_RUN_BUTTON               ITF_GET_STRINGID_CRC(RunButton,2713075566)
#define OPTION_MURFY_ASSIST             ITF_GET_STRINGID_CRC(MurfyAssist,1587373204)

#define OPTION_MASTER_VOLUME            ITF_GET_STRINGID_CRC(MasterVolume,598641050)
#define OPTION_MUSIC_VOLUME             ITF_GET_STRINGID_CRC(MusicVolume,2854727058)
#define OPTION_SFX_VOLUME               ITF_GET_STRINGID_CRC(SFXVolume,410342323)
#define OPTION_INTENSITY                ITF_GET_STRINGID_CRC(Intensity,245404497)
#define OPTION_CONTROLLER_SPEAKER_VOLUME ITF_GET_STRINGID_CRC(ControllerSpeakerVolume,3145883637)
#define LAST_PLAY_TIME                  ITF_GET_STRINGID_CRC(LastPlayTime,1448759483)

// Keeps track of Offline mode with cross progression status
// https://confluence.ubisoft.com/display/onlineservices/UC+Authentication+-+In-game+Flows
#define AUTH_ALREADY_LINKED             ITF_GET_STRINGID_CRC(AuthAlreadyLinked,2336798753)
#define AUTH_BOOT_COUNT                 ITF_GET_STRINGID_CRC(AuthBootCount,3102935158)

#if defined(ITF_WINDOWS)
#define OPTION_PC_KEYBOARD_CONTROLLER_SHARING ITF_GET_STRINGID_CRC(PCKeyboardControllerSharing,3022196002)
#endif

#endif //_ITF_RAY_GAMEOPTIONNAMES_H_
