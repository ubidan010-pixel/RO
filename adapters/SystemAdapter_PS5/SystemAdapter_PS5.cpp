#include "precompiled_SystemAdapter_PS5.h"

#include <libsysmodule.h>
#include <common_dialog.h>
#include <message_dialog.h>
#include <video_out.h>
#include <app_content.h>
#include <np/np_entitlement_access.h>
#include <system_service.h>
#include <rtc.h>
#include <ult.h>

#include "adapters/SystemAdapter_PS5/SystemAdapter_PS5.h"
#include "core/system/PS5/Synchronize_ps5.h"
#include "core/utility.h"
#include "core/UnicodeTools.h"
#include "core/error/ErrorHandler.h"
#include "Engine/singleton/Singletons.h"

#include "adapters/InputAdapter_PS5/InputAdapter_PS5.h"
#include "adapters/GfxAdapter_PS5/GfxAdapter_PS5.h"
#include "core/Config.h"

namespace ITF
{
    SystemAdapter_PS5::SystemAdapter_PS5()
    {
        LARGE_INTEGER ticksPerSec = {};
        QueryPerformanceFrequency( &ticksPerSec );
        m_QPFTicksPerSec = ticksPerSec.QuadPart;

        timerStart();

        // from Ubiservices 1st Party list
        ITF_VERIFY_SCE_CALL(sceSysmoduleLoadModule(SCE_SYSMODULE_JSON2));
        ITF_VERIFY_SCE_CALL(sceSysmoduleLoadModule(SCE_SYSMODULE_NP_AUTH));
        ITF_VERIFY_SCE_CALL(sceSysmoduleLoadModule(SCE_SYSMODULE_ULT));
        ITF_VERIFY_SCE_CALL(sceUltInitialize());

        // load user message dialog dynamic libraries
        ITF_VERIFY_SCE_CALL(sceSysmoduleLoadModule(SCE_SYSMODULE_MESSAGE_DIALOG));
        ITF_VERIFY_SCE_CALL(sceSysmoduleLoadModule(SCE_SYSMODULE_WEB_BROWSER_DIALOG));
        ITF_VERIFY_SCE_CALL(sceCommonDialogInitialize());
        m_messageBoxLibInit = btrue;

        ITF_VERIFY_SCE_CALL(sceSysmoduleLoadModule(SCE_SYSMODULE_APP_CONTENT));
        SceAppContentInitParam initParam{};
        m_bootParam = {};
        ITF_VERIFY_SCE_CALL(sceAppContentInitialize(&initParam, &m_bootParam));

        ITF_VERIFY_SCE_CALL(sceAppContentAppParamGetInt(SCE_APP_CONTENT_APPPARAM_ID_USER_DEFINED_PARAM_1, &m_userDefinedAppParam1));
        ITF_VERIFY_SCE_CALL(sceAppContentAppParamGetInt(SCE_APP_CONTENT_APPPARAM_ID_USER_DEFINED_PARAM_2, &m_userDefinedAppParam2));
        ITF_VERIFY_SCE_CALL(sceAppContentAppParamGetInt(SCE_APP_CONTENT_APPPARAM_ID_USER_DEFINED_PARAM_3, &m_userDefinedAppParam3));
        ITF_VERIFY_SCE_CALL(sceAppContentAppParamGetInt(SCE_APP_CONTENT_APPPARAM_ID_USER_DEFINED_PARAM_4, &m_userDefinedAppParam4));

        ITF_VERIFY_SCE_CALL(sceSysmoduleLoadModule(SCE_SYSMODULE_NP_CPP_WEB_API));
        ITF_VERIFY_SCE_CALL(sceSysmoduleLoadModule(SCE_SYSMODULE_NP_ENTITLEMENT_ACCESS));

        SceNpEntitlementAccessInitParam initParam2;
        SceNpEntitlementAccessBootParam	bootParam2;
        memset(&initParam2, 0, sizeof(SceNpEntitlementAccessInitParam));
        memset(&bootParam2, 0, sizeof(SceNpEntitlementAccessBootParam));
        ITF_VERIFY_SCE_CALL(sceNpEntitlementAccessInitialize(&initParam2, &bootParam2));

        fill(m_usersIDs.userId, SCE_USER_SERVICE_USER_ID_INVALID);
    }

    SystemAdapter_PS5::~SystemAdapter_PS5()
    {
        ITF_VERIFY_SCE_CALL(sceMsgDialogTerminate());
        ITF_VERIFY_SCE_CALL(sceUltFinalize());

        ITF_VERIFY_SCE_CALL(sceSysmoduleUnloadModule(SCE_SYSMODULE_NP_ENTITLEMENT_ACCESS));
        ITF_VERIFY_SCE_CALL(sceSysmoduleUnloadModule(SCE_SYSMODULE_NP_CPP_WEB_API));
        ITF_VERIFY_SCE_CALL(sceSysmoduleUnloadModule(SCE_SYSMODULE_APP_CONTENT));
        ITF_VERIFY_SCE_CALL(sceSysmoduleUnloadModule(SCE_SYSMODULE_WEB_BROWSER_DIALOG));
        ITF_VERIFY_SCE_CALL(sceSysmoduleUnloadModule(SCE_SYSMODULE_MESSAGE_DIALOG));
        ITF_VERIFY_SCE_CALL(sceSysmoduleUnloadModule(SCE_SYSMODULE_ULT));
        ITF_VERIFY_SCE_CALL(sceSysmoduleUnloadModule(SCE_SYSMODULE_NP_AUTH));
        ITF_VERIFY_SCE_CALL(sceSysmoduleUnloadModule(SCE_SYSMODULE_JSON2));
    }

    bbool SystemAdapter_PS5::initialize()
    {
        m_inputAdapter = new InputAdapter_PS5();
        Singletons::get().setInputAdapter(m_inputAdapter);

        initTitleId();
        initUsers();

        updateTrialSkuFlag();
        return btrue;
    }

    bbool SystemAdapter_PS5::isSkipNoticeScreens()
    {
        bool skipNoticeScreen = false;
        sceSystemServiceGetNoticeScreenSkipFlag(&skipNoticeScreen);
        return skipNoticeScreen;
    }

    void SystemAdapter_PS5::initUsers()
    {
        ITF_VERIFY_SCE_CALL(sceUserServiceGetInitialUser(&m_initialUserId));

        updateUsers();
    }

    static SceNpTitleId gs_TitleId_SCEE = { "PPSA34569_00", {0} };
    //static SceNpTitleId gs_TitleId_SCEA = { "xyz", {0} };
    //static SceNpTitleId gs_TitleId_SCEJ = { "xyz", {0} };

    static const SceNpTitleSecret gs_NpTitleSecret_SCEE = { 0xac,0x28,0xc5,0x55,0x2f,0x09,0x86,0x9f,0xf8,0x1b,0x47,0x98,0x03,0x68,0xc7,0x0d,0xa7,0xa5,0xec,0x4c,0x52,0x90,0x8b,0xa8,0xc4,0x45,0x3c,0x4a,0x3c,0xfb,0x1d,0xe1,0xa0,0x92,0xa4,0x5b,0xcb,0x9d,0x1f,0x89,0x00,0xad,0xc6,0x8b,0xa1,0xda,0xc1,0xda,0xa1,0xa1,0xd5,0x0d,0xc8,0xe6,0xb5,0xc5,0xd0,0xe9,0x70,0xd2,0x64,0xb1,0x64,0xc7,0x03,0xfa,0xd7,0xe7,0xb2,0x59,0x69,0xe0,0x8b,0x24,0x49,0xea,0xaf,0x2d,0x61,0xb4,0xb3,0xc1,0xd5,0x93,0x39,0x33,0xc6,0x98,0xc8,0x37,0xc3,0xaf,0xc6,0x87,0x0b,0x8a,0x09,0x78,0x4e,0x03,0xc4,0xde,0x73,0x85,0x49,0x50,0x4d,0xb5,0x57,0x63,0x33,0xe3,0xd9,0xfb,0x19,0x81,0xdc,0x31,0xb4,0xb2,0xe7,0xb7,0x66,0x48,0x50,0xcc,0x44,0x23 };
    //static const SceNpTitleSecret gs_NpTitleSecret_SCEA = {};
    //static const SceNpTitleSecret gs_NpTitleSecret_SCEJ = {};

    void SystemAdapter_PS5::initTitleId()
    {
        SceNpTitleId npTitleId;
        SceNpTitleSecret npTitleSecret;

        ITF_MEMCOPY(&npTitleId, &gs_TitleId_SCEE, sizeof(npTitleId));
        ITF_MEMCOPY(&npTitleSecret, &gs_NpTitleSecret_SCEE, sizeof(npTitleSecret));

        int result = sceNpSetNpTitleId(&npTitleId, &npTitleSecret);
        if (result != SCE_OK)
        {
            LOG("[SystemAdapter_PS5] sceNpSetNpTitleId failed with error code %x", result);
        }

        LOG("[SystemAdapter_PS5] initTitleId %s OK", npTitleId.id);
    }

    bbool SystemAdapter_PS5::openGraphics(int _width, int _height, int _alphaBits, int _depthBits, int _stencilBits, bbool _fullscreen, bbool _mouseCursor, const String& _name, bbool _waitVBL)
    {
        GFXAdapter_PS5 *gfxadapter = static_cast<GFXAdapter_PS5*>(GFX_ADAPTER);
        gfxadapter->setWaitVBL(_waitVBL);

        int32_t videoOutHdl = sceVideoOutOpen(SCE_USER_SERVICE_USER_ID_SYSTEM, SCE_VIDEO_OUT_BUS_TYPE_MAIN, 0, nullptr);
        if (videoOutHdl < 0)
        {
            ITF_ASSERT_SCE_RESULT(videoOutHdl);
        }
        m_hwnd = reinterpret_cast<void*>(videoOutHdl);

        if (!gfxadapter->createDevice(videoOutHdl))
                return bfalse;

        gfxadapter->createDebugFonts(GFX_ADAPTER->getScreenWidth(), gfxadapter->getScreenHeight());
        gfxadapter->init();

        return btrue;
    }


    void SystemAdapter_PS5::terminate()
    {
        ITF_ASSERT_CRASH(false, "Never terminate a PS5 executable. It's OS responsibility to do it.");
    }

    void  SystemAdapter_PS5::present()
    {
        /// inform adapter.
        GFX_ADAPTER->setfPs(getfPs());

        GFX_ADAPTER->endRendering();
        GFX_ADAPTER->present();

        m_NumFrames++;
    }

    void SystemAdapter_PS5::update(bbool _minimal)
    {
        if (!_minimal)
        {
            if (m_inputAdapter != nullptr)
                m_inputAdapter->updateAllInputState();
        }

        updateSystemServiceEvents();
        updateUsers();
        updatePauseState();
    }

    void SystemAdapter_PS5::registerSystemEventCallback(SystemEventCallback cb, void* userParam)
    {
        m_systemEvtCB = cb;
        m_systemEvtUserParam = userParam;
    }

    void SystemAdapter_PS5::updateSystemServiceEvents()
    {
        // Get background/overlay status and detect resume event
        sceSystemServiceGetStatus(&m_systemStatus);
        for (ix i = 0; i != m_systemStatus.eventNum; ++i)
        {
            SceSystemServiceEvent serviceEvent;
            if (SCE_OK == sceSystemServiceReceiveEvent(&serviceEvent))
            {
                SystemEventType evtType = serviceEvent.eventType == SCE_SYSTEM_SERVICE_EVENT_ON_RESUME ? SystemEventType::ON_RESUME : SystemEventType::ON_UNKNOWN;

                if (evtType == SystemEventType::ON_RESUME)
                {
                    LOG("RESUME");
                }

                if (m_systemEvtCB!=nullptr)
                {
                    m_systemEvtCB(m_systemEvtUserParam, evtType, &serviceEvent);
                }
                else
                {
                    LOG("Service Event not handled 0x%08X", serviceEvent.eventType);
                }
            }
        }
    }

    SystemAdapter_PS5::SystemStatus SystemAdapter_PS5::getSystemStatus() const
    {
        SystemStatus status;
        status.isSystemUiOverlaid = m_systemStatus.isSystemUiOverlaid;
        status.isInBackgroundExecution = m_systemStatus.isInBackgroundExecution;
        return status;
    }

    void SystemAdapter_PS5::updateUsers()
    {
        SceUserServiceEvent event;
        ix ret;
        while ((ret = sceUserServiceGetEvent(&event)) == SCE_OK)
        {
            if (event.eventType == SCE_USER_SERVICE_EVENT_TYPE_LOGIN)
            {
                i32 userIdx = getUserIdx(event.userId);
                if (userIdx == -1)
                    userIdx = getUnusedUserIdx();
                if (userIdx != -1)
                {
                    m_usersIDs.userId[userIdx] = event.userId;
                    m_inputAdapter->loginUser(userIdx, event.userId);
                }
            }
            else if (event.eventType == SCE_USER_SERVICE_EVENT_TYPE_LOGOUT)
            {
                i32 userIdx = getUserIdx(event.userId);
                if (userIdx != -1)
                {
                    m_inputAdapter->logoutUser(userIdx, event.userId);
                    m_usersIDs.userId[userIdx] = SCE_USER_SERVICE_USER_ID_INVALID;
                }
            }

#ifdef ITF_SUPPORT_NETWORKSERVICES
            //NETWORKSERVICES->getLoginModule()->onUserLogEvent(event);
#endif // ITF_SUPPORT_NETWORKSERVICES
        }

        ITF_ASSERT(ret == SCE_USER_SERVICE_ERROR_NO_EVENT);
    }

    void SystemAdapter_PS5::updatePauseState()
    {
        bool needPause = m_systemStatus.isInBackgroundExecution || m_systemStatus.isSystemUiOverlaid;

        if (!m_gamePaused && needPause)
        {
            setSystemPauseFlag(btrue);
            // pause sound should be  called by GameManager::enterPause
            //SOUND_MANAGER->pauseAll(SoundManager::PAUSE_LEVEL_MAX);

        }

        if (m_gamePaused && !needPause)
        {
            setSystemPauseFlag(bfalse);
            // need to resume sound ?
            //SOUND_MANAGER->resumeAll(SoundManager::PAUSE_LEVEL_MAX);
        }

        m_gamePaused = needPause;
    }

    i32 SystemAdapter_PS5::getUserIdx(i32 _userID)
    {
        for (i32 i = 0; i < SCE_USER_SERVICE_MAX_LOGIN_USERS; i++)
        {
            if (m_usersIDs.userId[i] == _userID)
                return i;
        }
        return -1;
    }

    i32 SystemAdapter_PS5::getUnusedUserIdx()
    {
        for (i32 i = 0; i < SCE_USER_SERVICE_MAX_LOGIN_USERS; i++)
        {
            if ((m_usersIDs.userId[i] == SCE_USER_SERVICE_USER_ID_INVALID)
                || (m_inputAdapter->getUserServiceIdFromPadIndex(i) == SCE_USER_SERVICE_USER_ID_INVALID))
                return i;
        }
        return -1;
    }

    String8 SystemAdapter_PS5::getUserSystemName(u32 _userIndexControlerId) const
    {
        SceUserServiceUserId userId;
        ix res = sceUserServiceGetInitialUser(&userId);
        if (res != SCE_OK)
            return String8::emptyString;

        char username[SCE_USER_SERVICE_MAX_USER_NAME_LENGTH + 1]{};
        res = sceUserServiceGetUserName(userId, username, sizeof(username));
        if (res != SCE_OK)
            return String8::emptyString;

        return username;
    }


    bbool SystemAdapter_PS5::getWindowSize(int& _width, int& _height)
    {
        _width = GFX_ADAPTER->getScreenWidth();
        _height = GFX_ADAPTER->getScreenHeight();
        return btrue;
    }

    void SystemAdapter_PS5::timerStart()
    {
        // Get the current time
        LARGE_INTEGER time = {};
        QueryPerformanceCounter(&time);
        if (m_timerStopped)
            m_baseTime += time.QuadPart - m_stopTime;
        m_stopTime = 0;
        m_lastElapsedTime = time.QuadPart;
        m_timerStopped = bfalse;
    }

    void SystemAdapter_PS5::timerStop()
    {
        if (!m_timerStopped)
        {
            LARGE_INTEGER time = {};
            QueryPerformanceCounter(&time);
            m_stopTime = time.QuadPart;
            m_lastElapsedTime = time.QuadPart;
            m_timerStopped = btrue;
        }
    }

    f64 SystemAdapter_PS5::getTime() const
    {
        LARGE_INTEGER curtime = GetAdjustedCurrentTime();
        f64 time = (f64)(curtime.QuadPart - m_baseTime) / (f64)m_QPFTicksPerSec;
        return time;
    }

    LARGE_INTEGER SystemAdapter_PS5::GetAdjustedCurrentTime() const
    {
        LARGE_INTEGER time;
        if (m_stopTime != 0)
            time.QuadPart = m_stopTime;
        else
            QueryPerformanceCounter(&time);
        return time;
    }

    f64 SystemAdapter_PS5::getElapsedTime()
    {
        LARGE_INTEGER time = GetAdjustedCurrentTime();
        f64 elapsedTime = ((f64)(time.QuadPart - m_lastElapsedTime) / (f64)m_QPFTicksPerSec);
        m_lastElapsedTime = time.QuadPart;
        if (elapsedTime < 0.0f)
            elapsedTime = 0.0f;
        return elapsedTime;
    }

    f32 SystemAdapter_PS5::getfPs()
    {
        double time = getTime();
        if (time - m_LastFPSTime > 1.0)
        {
            m_fPs = m_NumFrames / (time - m_LastFPSTime);
            m_LastFPSTime = time;
            m_NumFrames = 0L;
        }
        return (f32)m_fPs;
    }

    bbool SystemAdapter_PS5::isHDDMode() const
    {
        return btrue;
    }

    ITF_TERRITORY SystemAdapter_PS5::getSystemTerritory() const
    {
        // ITF_TERRITORY_AMERICA = 0
        // ITF_TERRITORY_EUROPE  = 1
        // ITF_TERRITORY_OTHER   = 2
        return static_cast<ITF_TERRITORY>(m_userDefinedAppParam1);
    }

    bbool SystemAdapter_PS5::isBackAndSelectButtonsInverted() const
    {
        int32_t languageSys = SCE_SYSTEM_PARAM_LANG_ENGLISH_US;
        sceSystemServiceParamGetInt(SCE_SYSTEM_SERVICE_PARAM_ID_LANG, &languageSys);
        return (languageSys == SCE_SYSTEM_PARAM_LANG_JAPANESE) ? btrue : bfalse;
    }

    static ITF_LANGUAGE convertSceLanguage(int32_t languageSys)
    {
        switch (languageSys)
        {
        case SCE_SYSTEM_PARAM_LANG_JAPANESE: return ITF_LANGUAGE_JAPANESE;
        case SCE_SYSTEM_PARAM_LANG_ENGLISH_US: return ITF_LANGUAGE_ENGLISH;
        case SCE_SYSTEM_PARAM_LANG_FRENCH: return ITF_LANGUAGE_FRENCH;
        case SCE_SYSTEM_PARAM_LANG_SPANISH: return ITF_LANGUAGE_SPANISH;
        case SCE_SYSTEM_PARAM_LANG_GERMAN: return ITF_LANGUAGE_GERMAN;
        case SCE_SYSTEM_PARAM_LANG_ITALIAN: return ITF_LANGUAGE_ITALIAN;
        case SCE_SYSTEM_PARAM_LANG_DUTCH: return ITF_LANGUAGE_DUTCH;
        case SCE_SYSTEM_PARAM_LANG_PORTUGUESE_PT: return ITF_LANGUAGE_PORTUGUESE;
        case SCE_SYSTEM_PARAM_LANG_RUSSIAN: return ITF_LANGUAGE_RUSSIAN;
        case SCE_SYSTEM_PARAM_LANG_KOREAN: return ITF_LANGUAGE_KOREAN;
        case SCE_SYSTEM_PARAM_LANG_CHINESE_T: return ITF_LANGUAGE_TRADITIONALCHINESE;
        case SCE_SYSTEM_PARAM_LANG_CHINESE_S: return ITF_LANGUAGE_SIMPLIFIEDCHINESE;
        case SCE_SYSTEM_PARAM_LANG_FINNISH: return ITF_LANGUAGE_UNKNOWN;
        case SCE_SYSTEM_PARAM_LANG_SWEDISH: return ITF_LANGUAGE_UNKNOWN;
        case SCE_SYSTEM_PARAM_LANG_DANISH: return ITF_LANGUAGE_UNKNOWN;
        case SCE_SYSTEM_PARAM_LANG_NORWEGIAN: return ITF_LANGUAGE_UNKNOWN;
        case SCE_SYSTEM_PARAM_LANG_POLISH: return ITF_LANGUAGE_POLISH;
        case SCE_SYSTEM_PARAM_LANG_PORTUGUESE_BR: return ITF_LANGUAGE_PORTUGUESE;
        case SCE_SYSTEM_PARAM_LANG_ENGLISH_GB: return ITF_LANGUAGE_ENGLISH;
        case SCE_SYSTEM_PARAM_LANG_TURKISH: return ITF_LANGUAGE_UNKNOWN;
        case SCE_SYSTEM_PARAM_LANG_SPANISH_LA: return ITF_LANGUAGE_UNKNOWN;
        case SCE_SYSTEM_PARAM_LANG_ARABIC: return ITF_LANGUAGE_UNKNOWN;
        case SCE_SYSTEM_PARAM_LANG_FRENCH_CA: return ITF_LANGUAGE_UNKNOWN;
        case SCE_SYSTEM_PARAM_LANG_CZECH: return ITF_LANGUAGE_UNKNOWN;
        case SCE_SYSTEM_PARAM_LANG_HUNGARIAN: return ITF_LANGUAGE_UNKNOWN;
        case SCE_SYSTEM_PARAM_LANG_GREEK: return ITF_LANGUAGE_UNKNOWN;
        case SCE_SYSTEM_PARAM_LANG_ROMANIAN: return ITF_LANGUAGE_UNKNOWN;
        case SCE_SYSTEM_PARAM_LANG_THAI: return ITF_LANGUAGE_UNKNOWN;
        case SCE_SYSTEM_PARAM_LANG_VIETNAMESE: return ITF_LANGUAGE_UNKNOWN;
        case SCE_SYSTEM_PARAM_LANG_INDONESIAN: return ITF_LANGUAGE_UNKNOWN;
        case SCE_SYSTEM_PARAM_LANG_UKRAINIAN: return ITF_LANGUAGE_UNKNOWN;
        default: return ITF_LANGUAGE_UNKNOWN;
        }
    }

    ITF_LANGUAGE SystemAdapter_PS5::getSystemLanguage(const ITF_LANGUAGE* _supportedLanguages, u32 _supportedCount, ITF_LANGUAGE _defaultLanguage) const
    {
        ITF_LANGUAGE language;
        int32_t languageSys;
        i32 ret = sceSystemServiceParamGetInt(SCE_SYSTEM_SERVICE_PARAM_ID_LANG, &languageSys);
        if (ret != SCE_OK)
        {
            ITF_ASSERT(0);
            language = ITF_LANGUAGE_UNKNOWN;
        }
        else
        {
            language = convertSceLanguage(languageSys);
        }

        return filterSupportedLanguages(language, _supportedLanguages, _supportedCount, _defaultLanguage);
    }

    String SystemAdapter_PS5::userName() const
    {
        return "PS5";
    }

    String SystemAdapter_PS5::dateTime() const
    {
        time_t rawtime;
        struct tm* timeinfo;
        char buffer[80]{};
        ::time(&rawtime);

        timeinfo = ::localtime(&rawtime);

        strftime(buffer, 80, "%m-%d_%H-%M", timeinfo);

        return buffer;
    }

    void SystemAdapter_PS5::getTime(Time& _time) const
    {
        SceRtcDateTime datetime;
        ITF_VERIFY_SCE_CALL(sceRtcGetCurrentClockLocalTime(&datetime));
        _time.m_year = datetime.year; // 0000-2047
        _time.m_month = datetime.month; // 1-12
        _time.m_day = datetime.day; // 1-31
        _time.m_hour = datetime.hour;
        _time.m_minute = datetime.minute;
        _time.m_second = datetime.second;
    }

    void SystemAdapter_PS5::queryPerformanceCounter(LARGE_INTEGER* _count) const
    {
        QueryPerformanceCounter(_count);
    }

    void SystemAdapter_PS5::queryPerformanceFrequency(LARGE_INTEGER* _ticksPerSecond) const
    {
        QueryPerformanceFrequency(_ticksPerSecond);
    }

    i32 SystemAdapter_PS5::messageBox(const String& _title, const String& _msg, u32 _type, ITF_MESSAGEBOX_RESULT silentResult)
    {
        hideStartupSplashScreen();
        LOG("Message box: %ls\n%ls", _title.cStr(), _msg.cStr());

    #if defined(ROBOT_COMPILATION)
        LOG("-> ROBOT_COMPILATION mode: \"Yes\" selected\n");
        return ITF_IDYES; // say yes to every message box if robot compilation (not a dev build)
    #else

        if (!m_messageBoxLibInit)
        {
            LOG("-> Message box lib not init: \"Yes\" selected\n");
            return ITF_IDYES;
        }

        if (sceCommonDialogIsUsed())
        {
            LOG("-> Message box already displayed: \"Yes\" selected\n");
            return ITF_IDYES;
        }

        sceMsgDialogInitialize();

        const String8 title = UTF16ToUTF8(_title.cStr()).get();
        const String8 msg = title + "\n" + UTF16ToUTF8(_msg.cStr()).get();

        SceMsgDialogParam param;
        SceMsgDialogUserMessageParam userMsgParam;
        memset(&userMsgParam, 0, sizeof(userMsgParam));

        sceMsgDialogParamInitialize(&param);
        param.mode = SCE_MSG_DIALOG_MODE_USER_MSG;
        param.userMsgParam = &userMsgParam;
        param.userMsgParam->msg = msg.cStr();
        param.userMsgParam->buttonType = SCE_MSG_DIALOG_BUTTON_TYPE_2BUTTONS;
        SceMsgDialogButtonsParam buttonParams{};
        buttonParams.msg1 = "Yes";
        buttonParams.msg2 = "No";
        param.userMsgParam->buttonsParam = &buttonParams;

        int res = sceMsgDialogOpen(&param);
        if (res < 0)
        {
            LOG("sceMsgDialogOpen failed: 0x%08x\n", res);
        }
        if (res >= 0)
        {
            while (sceMsgDialogUpdateStatus() != SCE_COMMON_DIALOG_STATUS_FINISHED);
            SceMsgDialogResult choice;
            memset(&choice, 0, sizeof(choice));
            int32_t ret = sceMsgDialogGetResult(&choice);
            sceMsgDialogTerminate();
            if (ret >= 0)
            {
                switch (choice.result)
                {
                case SCE_COMMON_DIALOG_RESULT_OK:
                    switch (choice.buttonId)
                    {
                    case SCE_MSG_DIALOG_BUTTON_ID_YES:
                        return ITF_IDYES;
                    case SCE_MSG_DIALOG_BUTTON_ID_NO:
                        return ITF_IDNO;
                    }
                    break;

                case SCE_COMMON_DIALOG_RESULT_USER_CANCELED:
                    return ITF_IDCANCEL;
                }
            }
        }
        else
        {
            sceMsgDialogTerminate();

            InputAdapter_PS5* inputAdapter = INPUT_ADAPTER_PS5;
            if (!inputAdapter)
            {
                LOG("-> No input interface found : \"Yes\" selected\n");
                return ITF_IDYES; // say yes to every message box if no input available
            }

            LOG("-> X for \"Yes\", O for \"No\", /\\ for \"Cancel\"\n");
            InputAdapter::PressStatus buttons[JOY_MAX_BUT];
            do
            {
                inputAdapter->updateAllInputState();
                for (ux padIdx = 0; padIdx < SCE_USER_SERVICE_MAX_LOGIN_USERS; ++padIdx)
                {
                    if (inputAdapter->isConnected(padIdx))
                    {
                        inputAdapter->getGamePadButtons(InputAdapter::EnvironmentAll, padIdx, buttons, JOY_MAX_BUT);
                        if (buttons[m_joyButton_A] == InputAdapter::JustPressed)
                        {
                            return ITF_IDYES;
                        }
                        if (buttons[m_joyButton_B] == InputAdapter::JustPressed)
                        {
                            return ITF_IDNO;
                        }
                        if (buttons[m_joyButton_Y] == InputAdapter::JustPressed)
                        {
                            return ITF_IDCANCEL;
                        }
                    }
                }
                ITF::sleep(16);
            } while (1);
        }
        return ITF_IDYES;
    #endif // defined(ROBOT_COMPILATION)
    }

    void SystemAdapter_PS5::hideStartupSplashScreen()
    {
        if (!m_splashScreenHidden)
        {
            m_splashScreenHidden = btrue;
            sceSystemServiceHideSplashScreen();
        }
    }

    void SystemAdapter_PS5::updateTrialSkuFlag()
    {
        SceNpEntitlementAccessSkuFlag skuFlag = 0;
        SceNpEntitlementAccessGameTrialsFlag gameTrialsFlag = 0;

        ITF_VERIFY_SCE_CALL(sceNpEntitlementAccessGetSkuFlag(&skuFlag));
        ITF_VERIFY_SCE_CALL(sceNpEntitlementAccessGetGameTrialsFlag(&gameTrialsFlag));

        m_isTrialMode = (skuFlag == SCE_NP_ENTITLEMENT_ACCESS_SKU_FLAG_TRIAL) ||
            (gameTrialsFlag == SCE_NP_ENTITLEMENT_ACCESS_GAME_TRIALS_FLAG_ON);

        LOG("sku flag:%d game trials flag:%d, mode: %s", skuFlag, gameTrialsFlag, m_isTrialMode ? "Trial" : "Full");
    }

} // namespace ITF



