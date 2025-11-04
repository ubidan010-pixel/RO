#ifndef _ITF_RAY_GAMEOPTIONNAMES_H_
#define _ITF_RAY_GAMEOPTIONNAMES_H_

#ifndef _ITF_STRINGID_H_
#include "core/StringID.h"
#endif //_ITF_STRINGID_H_

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

    enum EVibrationMode
    {
        VibrationMode_Off = 0,
        VibrationMode_On = 1,
        VIBRATION_CHOICES = 2
    };

    static const u32 RESOLUTION_CHOICES = 4;
    static const u32 LANGUAGE_CHOICES = 15;
    static const u32 DIAMOND_HEART_MAX_CONSECUTIVE_HITS = 10;
    static const f32 DIAMOND_HEART_CONSECUTIVE_HIT_WINDOW = 2.0f;
}

#define OPTION_MENU_NAME "optionmenu"
#define RESOLUTION_TEXT                       ITF_GET_STRINGID_CRC(resolution_text,793270934)
#define START_WITH_HEART                      ITF_GET_STRINGID_CRC(start_with_heart_text,1828577030)
#define RUN_BUTTON_NAME                       ITF_GET_STRINGID_CRC(run_button_text,2188319908)
#define VIBRATION_TEXT                        ITF_GET_STRINGID_CRC(vibration_text,238956682)
#define LANGUAGE_TEXT                         ITF_GET_STRINGID_CRC(language_text,3720481777)
// Action buttons
#define OPTIONMENU_RESET_TO_DEFAULT_BUTTON    ITF_GET_STRINGID_CRC(reset_to_default_button,2697802978)
#define OPTIONMENU_ACCEPT_BUTTON              ITF_GET_STRINGID_CRC(accept_button,25226343)
#define OPTIONMENU_CANCEL_BUTTON              ITF_GET_STRINGID_CRC(cancel_button,4260770984)
#define OPTION_RESOLUTION               ITF_GET_STRINGID_CRC(Resolution,1477600601)
#define OPTION_WINDOWED                 ITF_GET_STRINGID_CRC(Windowed,1545748265)
#define OPTION_LANGUAGE                 ITF_GET_STRINGID_CRC(Language,3611137521)

#define OPTION_START_WITH_HEART         ITF_GET_STRINGID_CRC(StartWithHeart,1992734674)
#define OPTION_RUN_BUTTON               ITF_GET_STRINGID_CRC(RunButton,2713075566)
#define OPTION_MURFY_ASSIST             ITF_GET_STRINGID_CRC(MurfyAssist,1587373204)
#define OPTION_VIBRATIONS               ITF_GET_STRINGID_CRC(Vibrations,4113726075)

#define OPTION_MUSIC_VOLUME             ITF_GET_STRINGID_CRC(MusicVolume,2854727058)
#define OPTION_SFX_VOLUME               ITF_GET_STRINGID_CRC(SFXVolume,410342323)
#define LAST_PLAY_TIME                  ITF_GET_STRINGID_CRC(LastPlayTime,1448759483)

// LINE ID
static const ITF::u32 LINEID_HEART_NONE = 4294967295;
static const ITF::u32 LINEID_HEART_RED = 4294967295;
static const ITF::u32 LINEID_HEART_GOLDEN = 4294967295;
static const ITF::u32 LINEID_HEART_DIAMOND = 4294967295;

static const ITF::u32 LINEID_RUN_HOLD = 4294967295;
static const ITF::u32 LINEID_RUN_TOGGLE = 4294967295;

static const ITF::u32 LINEID_VIBRATION_OFF = 4294967295;
static const ITF::u32 LINEID_VIBRATION_ON = 4294967295;

// Option Menu UI Component IDs - Normal Arrows
#define OPTIONMENU_LEFT_ARROW_LANGUAGE              ITF_GET_STRINGID_CRC(left_arrow_language,2347641254)
#define OPTIONMENU_LEFT_ARROW_RESOLUTION            ITF_GET_STRINGID_CRC(left_arrow_resolution,3204633007)
#define OPTIONMENU_LEFT_ARROW_START_WITH_HEART      ITF_GET_STRINGID_CRC(left_arrow_start_with_heart,1909382093)
#define OPTIONMENU_LEFT_ARROW_VIBRATION             ITF_GET_STRINGID_CRC(left_arrow_vibration,2699569848)
#define OPTIONMENU_LEFT_ARROW_RUN_BUTTON            ITF_GET_STRINGID_CRC(left_arrow_run_button,2381311211)

#define OPTIONMENU_RIGHT_ARROW_RESOLUTION           ITF_GET_STRINGID_CRC(right_arrow_resolution,1576475882)
#define OPTIONMENU_RIGHT_ARROW_LANGUAGE             ITF_GET_STRINGID_CRC(right_arrow_language,1579790645)
#define OPTIONMENU_RIGHT_ARROW_START_WITH_HEART     ITF_GET_STRINGID_CRC(right_arrow_start_with_heart,3235932730)
#define OPTIONMENU_RIGHT_ARROW_VIBRATION            ITF_GET_STRINGID_CRC(right_arrow_vibration,4278439894)
#define OPTIONMENU_RIGHT_ARROW_RUN_BUTTON           ITF_GET_STRINGID_CRC(right_arrow_run_button,579901351)

// Option Menu UI Component IDs - Hover Overlays
#define OPTIONMENU_LEFT_ARROW_LANGUAGE_HOVER        ITF_GET_STRINGID_CRC(left_arrow_language_hover,874662542)
#define OPTIONMENU_LEFT_ARROW_RESOLUTION_HOVER      ITF_GET_STRINGID_CRC(left_arrow_resolution_hover,1026877035)
#define OPTIONMENU_LEFT_ARROW_START_WITH_HEART_HOVER ITF_GET_STRINGID_CRC(left_arrow_start_with_heart_hover,2364315210)
#define OPTIONMENU_LEFT_ARROW_VIBRATION_HOVER       ITF_GET_STRINGID_CRC(left_arrow_vibration_hover,1503912500)
#define OPTIONMENU_LEFT_ARROW_RUN_BUTTON_HOVER      ITF_GET_STRINGID_CRC(left_arrow_run_button_hover,584211202)

#define OPTIONMENU_RIGHT_ARROW_RESOLUTION_HOVER     ITF_GET_STRINGID_CRC(right_arrow_resolution_hover,1457834039)
#define OPTIONMENU_RIGHT_ARROW_LANGUAGE_HOVER       ITF_GET_STRINGID_CRC(right_arrow_language_hover,1227610361)
#define OPTIONMENU_RIGHT_ARROW_START_WITH_HEART_HOVER ITF_GET_STRINGID_CRC(right_arrow_start_with_heart_hover,177560806)
#define OPTIONMENU_RIGHT_ARROW_VIBRATION_HOVER      ITF_GET_STRINGID_CRC(right_arrow_vibration_hover,2552987418)
#define OPTIONMENU_RIGHT_ARROW_RUN_BUTTON_HOVER     ITF_GET_STRINGID_CRC(right_arrow_run_button_hover,1193638931)

// Option Menu UI Component IDs - Backgrounds and Titles
#define OPTIONMENU_BACKGROUND                        ITF_GET_STRINGID_CRC(background,2090311284)
#define OPTION_BACKGROUND                            ITF_GET_STRINGID_CRC(option_background,3199356166)
#define ACCESSIBILITY_BACKGROUND                     ITF_GET_STRINGID_CRC(accessibility_background,2105765664)
#define SOUND_BACKGROUND                             ITF_GET_STRINGID_CRC(sound_background,3031097850)

#define DISPLAY_OPTION_TEXT                          ITF_GET_STRINGID_CRC(display_option_text,1293830620)
#define ACCESSIBILITY_TEXT                           ITF_GET_STRINGID_CRC(accessibility_text,3712526628)
#define SOUND_OPTION_TEXT                            ITF_GET_STRINGID_CRC(sound_option_text,574868985)

// Option Menu UI Component IDs - Option Labels
#define RESOLUTION_LABEL                             ITF_GET_STRINGID_CRC(resolution_label,3234668118)
#define LANGUAGE_LABEL                               ITF_GET_STRINGID_CRC(language_label,1407013127)
#define HEARTS_LABEL                                 ITF_GET_STRINGID_CRC(hearts_label,2023820601)
#define RUN_LABEL                                    ITF_GET_STRINGID_CRC(run_label,1777549370)
#define VIBRATION_LABEL                              ITF_GET_STRINGID_CRC(vibration_label,2117878705)
#define INTENSITY_LABEL                              ITF_GET_STRINGID_CRC(intensity_label,1510300201)
#define MASTER_LABEL                                 ITF_GET_STRINGID_CRC(master_label,3437403484)
#define MUSIC_LABEL                                  ITF_GET_STRINGID_CRC(music_label,244164885)
#define SFX_LABEL                                    ITF_GET_STRINGID_CRC(sfx_label,3437403483)
#define WINDOW_LABEL                                 ITF_GET_STRINGID_CRC(window_label,4139731100)

// Option Menu UI Component IDs - Window checkbox
#define OPTIONMENU_CHECKBOX_WINDOW_OFF                ITF_GET_STRINGID_CRC(checkbox_window_off,3734255018)
#define OPTIONMENU_CHECKBOX_WINDOW_ON                 ITF_GET_STRINGID_CRC(checkbox_window_on,675055890)

#endif //_ITF_RAY_GAMEOPTIONNAMES_H_
