#include "precompiled_SystemAdapter_Nintendo.h"

#include "adapters/SystemAdapter_Nintendo/SystemAdapter_Nintendo.h"

#include "adapters/GFXAdapter_NVN/GFXAdapter_NVN.h"
#include "adapters/InputAdapter_Nintendo/InputAdapter_Nintendo.h"
#include "adapters/Adapter_Savegame_Nintendo/Adapter_Savegame_Nintendo.h"
#include "core/error/ErrorHandler.h"
#include "core/system/Synchronize.h"
#include <nn/time/time_Api.h>
#include <nn/time/time_StandardUserSystemClock.h>
#include <nn/time/time_CalendarTime.h>
#include <nn/time/time_TimeZoneApi.h>
#include <nn/account.h>
#include <nn/vi.h>

#ifdef ITF_SUPPORT_NINTENDO_PROFILER
#include <nn/profiler.h>
#endif

#define SYS_LOG(...) LOG(__VA_ARGS__)

namespace ITF
{

    SystemAdapter_Nintendo::SystemAdapter_Nintendo()
    {
    #if defined(ITF_SUPPORT_NINTENDO_PROFILER) && defined(ITF_NX)
        ITF_VERIFY_NN_CALL(nn::profiler::InstallGraphicsHooks());
        // Never delete, definitely given to the profiler.
        u8* profilerBuffer = new (MemoryId::mId_System) u8[nn::profiler::MinimumBufferSize];
        ITF_VERIFY_NN_CALL(nn::profiler::Initialize(profilerBuffer, nn::profiler::MinimumBufferSize));
    #endif
    }

    SystemAdapter_Nintendo::~SystemAdapter_Nintendo()
    {
    }

    void SystemAdapter_Nintendo::preInitialize()
    {
        nn::time::Initialize();
        timerStart();

        nn::oe::Initialize();
        nn::oe::SetFocusHandlingMode(nn::oe::FocusHandlingMode_InFocusOnly); // let the system handle game pause and resume
        nn::oe::SetOperationModeChangedNotificationEnabled(true); // to receive MessageOperationModeChanged
        nn::oe::SetPerformanceModeChangedNotificationEnabled(true); // to receive MessagePerformanceModeChanged

        registerNotificationListener(this);

        updateDisplayResolution(); // Set the GFX_ADAPTER resolution based on the current performance mode

        static_cast<GFXAdapter_NVN*>(GFX_ADAPTER)->preInit();
    }

    bbool SystemAdapter_Nintendo::initialize()
    {
        auto* inputAdapter = new (MemoryId::mId_System) InputAdapter_Nintendo();
        Singletons::get().setInputAdapter(inputAdapter);
        inputAdapter->initialize();

        initUsers();

        return btrue;
    }

    void SystemAdapter_Nintendo::initUsers()
    {
        nn::account::Initialize();

        // more efforts has to be done later to handle multiple users

        ITF_VERIFY_MSG(nn::account::TryOpenPreselectedUser(&m_userHandle), "nn::account::TryOpenPreselectedUser failed, is StartupUserAccount set to Required in nmeta?");
        ITF_VERIFY_NN_CALL(nn::account::GetLastOpenedUser(&m_initialUserID));
    }

    void SystemAdapter_Nintendo::registerNotificationListener(ISystemNotificationListener_Nintendo* _listener)
    {
        ITF_ASSERT(_listener != nullptr);
        m_notificationListeners.push_back(_listener);
    }

    void SystemAdapter_Nintendo::unregisterNotificationListener(ISystemNotificationListener_Nintendo* _listener)
    {
        ITF_ASSERT(_listener != nullptr);
        auto it = std::find(m_notificationListeners.begin(), m_notificationListeners.end(), _listener);
        ITF_ASSERT(it != m_notificationListeners.end()); // caller error, listener not registered
        if (it != m_notificationListeners.end())
            m_notificationListeners.erase(it);
    }

    void SystemAdapter_Nintendo::updateNintendoSystemMessages()
    {
        nn::oe::Message message;
        if (nn::oe::TryPopNotificationMessage(&message))
        {
            for (ISystemNotificationListener_Nintendo* notificationListener : m_notificationListeners)
            {
                notificationListener->onSystemNotification(message);
            }
        }
    }

    void SystemAdapter_Nintendo::updateInputAdapter()
    {
        if (INPUT_ADAPTER_NINTENDO)
        {
            INPUT_ADAPTER_NINTENDO->updateAllInputState();
        }
    }

    void SystemAdapter_Nintendo::update(bbool _minimal)
    {
    #ifndef ITF_FINAL
        m_nbUpdateCalled++;
    #endif // ITF_FINAL

        updateNintendoSystemMessages();

        if (gameUpdateAllowed() && !_minimal)
        {
            updateInputAdapter();
        }

        updateGamePlayMode();
    }

    void SystemAdapter_Nintendo::updateGamePlayMode()
    {
        if (isDocked())
            m_gamePlayMode = GamePlayMode::Docked;
        else
        {
            if (INPUT_ADAPTER_NINTENDO->isHandheld())
                m_gamePlayMode = GamePlayMode::Handheld;
            else
                m_gamePlayMode = GamePlayMode::TableTop;
        }
    }

    void SystemAdapter_Nintendo::updateDisplayResolution()
    {
        nn::oe::PerformanceMode mode = nn::oe::GetPerformanceMode();

        if (mode == nn::oe::PerformanceMode_Normal)
        {
#ifdef ITF_NX
            m_resolutionWidth = 1280;
            m_resolutionHeight = 720;
#else
            m_resolutionWidth = 1920;
            m_resolutionHeight = 1080;
#endif
        }
        else if (mode == nn::oe::PerformanceMode_Boost)
        {
#ifdef ITF_NX
            m_resolutionWidth = 1920;
            m_resolutionHeight = 1080;
#else
            m_resolutionWidth = 3840;
            m_resolutionHeight = 2160;
#endif
        }
        else
        {
            SYS_LOG("[SYSTEM] Unknown Performance Mode: %d", static_cast<int>(mode));
        }
        if (GFX_ADAPTER->getScreenWidth() != m_resolutionWidth || GFX_ADAPTER->getScreenHeight() != m_resolutionHeight)
        {
            static_cast<GFXAdapter_NVN*>(GFX_ADAPTER)->requestRenderTargetsRebuild(m_resolutionWidth, m_resolutionHeight);
        }
    }

    void SystemAdapter_Nintendo::onSystemNotification(const nn::oe::Message& _message)
    {
        switch (_message)
        {
            case nn::oe::MessageFocusStateChanged:
            {
                ITF_ASSERT(0); // This should never happen, as we are using FocusHandlingMode_InFocusOnly
                SYS_LOG("[SYSTEM] RECEIVING MessageFocusStateChanged");
                break;
            }
            case nn::oe::MessageOperationModeChanged:
            {
                updateGamePlayMode();
                nn::oe::OperationMode mode = nn::oe::GetOperationMode();
                ITF_UNUSED(mode);
                SYS_LOG("[SYSTEM] RECEIVING MessageOperationModeChanged -> %s",
                    (mode == nn::oe::OperationMode_Console ? "console" : (mode == nn::oe::OperationMode_Handheld ? "handheld" : "?")));
                break;
            }
            case nn::oe::MessagePerformanceModeChanged:
            {
                updateDisplayResolution();
                nn::oe::PerformanceMode mode = nn::oe::GetPerformanceMode();
                ITF_UNUSED(mode);
                SYS_LOG("[SYSTEM] RECEIVING MessagePerformanceModeChanged -> %s",
                    (mode == nn::oe::PerformanceMode_Boost ? "boost" : (mode == nn::oe::PerformanceMode_Normal ? "normal" : "?")));
                break;
            }
            case nn::oe::MessageExitRequest:
            {
                SYS_LOG("[SYSTEM] RECEIVING MessageExitRequest");
                requestExit();
                break;
            }
        }
    }

    ITF_TERRITORY SystemAdapter_Nintendo::getSystemTerritory() const
    {
        // Not available on Nintendo Switch, so we return ITF_TERRITORY_UNKNOWN
        return ITF_TERRITORY_UNKNOWN;
    }

    static ITF_LANGUAGE convertNintendoLanguage(nn::settings::LanguageCode languageCode)
    {
        if (languageCode == nn::settings::LanguageCode::Japanese())
            return ITF_LANGUAGE_JAPANESE;
        if (languageCode == nn::settings::LanguageCode::EnglishUs())
            return ITF_LANGUAGE_ENGLISH;
        if (languageCode == nn::settings::LanguageCode::EnglishUk())
            return ITF_LANGUAGE_ENGLISH;
        if (languageCode == nn::settings::LanguageCode::French())
            return ITF_LANGUAGE_FRENCH;
        if (languageCode == nn::settings::LanguageCode::FrenchCa())
            return ITF_LANGUAGE_FRENCH;
        if (languageCode == nn::settings::LanguageCode::Spanish())
            return ITF_LANGUAGE_SPANISH;
        if (languageCode == nn::settings::LanguageCode::SpanishLatin())
            return ITF_LANGUAGE_SPANISH;
        if (languageCode == nn::settings::LanguageCode::German())
            return ITF_LANGUAGE_GERMAN;
        if (languageCode == nn::settings::LanguageCode::Italian())
            return ITF_LANGUAGE_ITALIAN;
        if (languageCode == nn::settings::LanguageCode::Dutch())
            return ITF_LANGUAGE_DUTCH;
        if (languageCode == nn::settings::LanguageCode::Portuguese())
            return ITF_LANGUAGE_PORTUGUESE;
        if (languageCode == nn::settings::LanguageCode::PortugueseBr())
            return ITF_LANGUAGE_PORTUGUESE;
        if (languageCode == nn::settings::LanguageCode::Russian())
            return ITF_LANGUAGE_RUSSIAN;
        if (languageCode == nn::settings::LanguageCode::Korean())
            return ITF_LANGUAGE_KOREAN;
        if (languageCode == nn::settings::LanguageCode::ChineseSimplified())
            return ITF_LANGUAGE_SIMPLIFIEDCHINESE;
        if (languageCode == nn::settings::LanguageCode::ChineseTraditional())
            return ITF_LANGUAGE_TRADITIONALCHINESE;
        return ITF_LANGUAGE_UNKNOWN;
    }


    ITF_LANGUAGE SystemAdapter_Nintendo::getSystemLanguage(const ITF_LANGUAGE* _supportedLanguages, u32 _supportedCount, ITF_LANGUAGE _defaultLanguage) const
    {
        ITF_LANGUAGE language;

        nn::settings::LanguageCode bestNNLangCode = nn::oe::GetDesiredLanguage();

        language = convertNintendoLanguage(bestNNLangCode);

        return filterSupportedLanguages(language, _supportedLanguages, _supportedCount, _defaultLanguage);
    }


    void SystemAdapter_Nintendo::terminate()
    {
        // Not supposed to be called, the OS is closing the application.
        ITF_NOT_IMPLEMENTED();
    }

    bbool SystemAdapter_Nintendo::openGraphics(
        int _width,
        int _height,
        int _alphaBits,
        int _depthBits,
        int _stencilBits,
        bbool _fullscreen,
        bbool _mouseCursor,
        const String& _name,
        bbool _waitVBL)
    {
        nn::vi::Initialize();

        ITF_VERIFY_NN_CALL(nn::vi::OpenDefaultDisplay(&m_display));
        ITF_VERIFY_NN_CALL(nn::vi::CreateLayer(&m_layer, m_display));
        ITF_VERIFY_NN_CALL(nn::vi::GetNativeWindow(&m_hwnd, m_layer));

        GFX_ADAPTER->setWaitVBL(_waitVBL);

        if (!static_cast<GFXAdapter_NVN*>(GFX_ADAPTER)->createDevice(m_hwnd))
        {
            return bfalse;
        }

        GFX_ADAPTER->init();
        return btrue;
    }

    void SystemAdapter_Nintendo::present()
    {
        GFX_ADAPTER->endRendering();
        GFX_ADAPTER->present();

        GFX_ADAPTER->setfPs(getfPs());

    #ifdef ITF_SUPPORT_NINTENDO_PROFILER
        nn::profiler::RecordHeartbeat(nn::profiler::Heartbeats_Main);
    #endif
    }

    void SystemAdapter_Nintendo::hideStartupSplashScreen()
    {
        if (!m_splashScreenHidden)
        {
            nn::oe::FinishStartupLogo();  // Required if the "LogoHandling" parameter in NMETA file is set to "Manual"
            m_splashScreenHidden = true;
        }
    }


    bool SystemAdapter_Nintendo::isDocked() const
    {
        return (nn::oe::GetOperationMode() == nn::oe::OperationMode_Console);
    }


    bbool SystemAdapter_Nintendo::getWindowSize(int& _width, int& _height)
    {
        _width = GFX_ADAPTER->getScreenWidth();
        _height = GFX_ADAPTER->getScreenHeight();
        return btrue;
    }

    void SystemAdapter_Nintendo::queryPerformanceFrequency(LARGE_INTEGER* ticksPerSecond) const
    {
        ticksPerSecond->QuadPart = nn::os::GetSystemTickFrequency();
    }

    void SystemAdapter_Nintendo::queryPerformanceCounter(LARGE_INTEGER* counter) const
    {
        counter->QuadPart = nn::os::GetSystemTick().GetInt64Value();
    }

    const char* SystemAdapter_Nintendo::getPlatformName()
    {
    #if defined(ITF_NX)
        return "NX";
    #elif defined(ITF_OUNCE)
        return "OUNCE";
    #endif
    }

    void SystemAdapter_Nintendo::timerStart()
    {
        // Get the current time
        LARGE_INTEGER time = {};
        time.QuadPart = 0;
        queryPerformanceCounter(&time);

        if (m_timerStopped)
            m_baseTime += time.QuadPart - m_stopTime;

        m_stopTime = 0;
        m_lastElapsedTime = time.QuadPart;
        m_timerStopped = false;
    }

    LARGE_INTEGER SystemAdapter_Nintendo::getAdjustedCurrentTime() const
    {
        LARGE_INTEGER time;
        if (m_stopTime != 0)
            time.QuadPart = m_stopTime;
        else
            queryPerformanceCounter(&time);
        return time;
    }

    f64 SystemAdapter_Nintendo::getTime() const
    {
        LARGE_INTEGER time = getAdjustedCurrentTime();

        f64 result = (f64)(time.QuadPart - m_baseTime) / (f64)(nn::os::GetSystemTickFrequency());
        return result;
    }

    void SystemAdapter_Nintendo::timerStop()
    {
        if (!m_timerStopped)
        {
            LARGE_INTEGER time = {};
            time.QuadPart = 0;
            queryPerformanceCounter(&time);
            m_stopTime = time.QuadPart;
            m_lastElapsedTime = time.QuadPart;
            m_timerStopped = btrue;
        }
    }

    f64 SystemAdapter_Nintendo::getElapsedTime()
    {
        LARGE_INTEGER time = getAdjustedCurrentTime();

        f64 elapsedTime = (f64)(time.QuadPart - m_lastElapsedTime) / (f64)(nn::os::GetSystemTickFrequency());
        m_lastElapsedTime = time.QuadPart;

        if (elapsedTime < 0.0)
            elapsedTime = 0.0;

        return elapsedTime;
    }

    f32 SystemAdapter_Nintendo::getfPs()
    {
        f64 time = getTime();
        if ((time - m_LastFPSTime) > 0.5)
        {
            m_fPs = (f64)m_NumFrames / (time - m_LastFPSTime);

        #ifndef ITF_FINAL
            m_fPs_NoVsync = (f64)m_NumFrames / (time - m_LastFPSTime - m_VBLSyncTime);
            m_VBLSyncTime = 0.0;
        #endif  // ITF_FINAL

            m_LastFPSTime = time;
            m_NumFrames = 0L;
        }

        return (f32)m_fPs;
    }

    String SystemAdapter_Nintendo::dateTime() const
    {
        return {};
    }

    void SystemAdapter_Nintendo::getTime(Time& _time) const
    {
        nn::time::PosixTime posixTime;
        nn::time::StandardUserSystemClock::GetCurrentTime(&posixTime);

        nn::time::CalendarTime calendarTime;
        nn::time::ToCalendarTime(&calendarTime, nullptr, posixTime);

        _time.m_year = (calendarTime.year >= 2016) ? calendarTime.year : 2016;
        _time.m_month = calendarTime.month;
        _time.m_day = calendarTime.day;
        _time.m_hour = calendarTime.hour;
        _time.m_minute = calendarTime.minute;
        _time.m_second = calendarTime.second;
    }

#ifndef ITF_FINAL
    static void logMessageBox(const char * _title, const char* _msg, u32 _type)
    {
        String8 buttonMappingText;
        buttonMappingText.setTextFormat("| Y -> %-19s|\n", "Yes");

        String8 otherButtonsText;
        if (_type & (ITF_MSG_YESNO | ITF_MSG_YESNOCANCEL))
        {
            otherButtonsText.setTextFormat("| A -> %-19s|\n", "No");
            buttonMappingText += otherButtonsText;
        }

        if (_type & ITF_MSG_YESNOCANCEL)
        {
            otherButtonsText.setTextFormat("| B -> %-19s|\n", "Cancel");
            buttonMappingText += otherButtonsText;
        }

        LOG("+=========================+\n"
            "|       MESSAGE BOX       |\n"
            "+=========================+\n"
            "\n"
            "Title: %s\n"
            "\n"
            "Message: %s\n"
            "\n"
            "+=========================+\n"
            "|    WAITING FOR INPUT    |\n"
            "%s"
            "+=========================+\n",
            _title, _msg, buttonMappingText.cStr());
    }
#endif


    i32 SystemAdapter_Nintendo::messageBox(const String& _title, const String& _msg, u32 _type, ITF_MESSAGEBOX_RESULT _silentResult)
    {
    #ifndef ITF_FINAL

        hideStartupSplashScreen();

        static bbool sb_DisplayMessageBoxInProgress = bfalse;

        StringToUTF8 title8(_title);
        StringToUTF8 msg8(_msg);

        if (sb_DisplayMessageBoxInProgress || exitRequested() || !INPUT_ADAPTER_NINTENDO)
        {
            LOG("*** %s ***\n", title8.get());
            LOG("*** %s ***\n", msg8.get());
            if (ISDEBUGGER_PRESENT) { ITF_DEBUG_BREAK(); }
            return ITF_IDNO;
        }

        {
            eThreadId thID = ThreadSettings::getCurrentThreadId();

            if (thID != eThreadId_mainThread)
            {
                LOG("*** %s ***\n", title8.get());
                LOG("*** %s ***\n", msg8.get());
                if (ISDEBUGGER_PRESENT) { ITF_DEBUG_BREAK(); }
                return ITF_IDCANCEL;
            }
        }

        if (SYSTEM_ADAPTER->IsSilent())
        {
            if (_type & ITF_MSG_OK)
                return ITF_IDYES;
            else
                return _silentResult;
        }

        sb_DisplayMessageBoxInProgress = btrue;

        logMessageBox(title8.get(), msg8.get(), _type);

        InputAdapter::PressStatus buttons[JOY_MAX_BUT];
        i32 result = I32_MAX;

        while (INPUT_ADAPTER_NINTENDO)
        {
            if (Synchronize::getCurrentThreadId() == ThreadSettings::m_settings[eThreadId_mainThread].m_threadID)
            {
                update(bfalse);
            }

            for (u32 pad = 0; pad < INPUT_ADAPTER_NINTENDO->getGamePadCount(); ++pad)
            {
                INPUT_ADAPTER_NINTENDO->getGamePadButtons(InputAdapter::EnvironmentAll, pad, buttons, JOY_MAX_BUT);

                if (buttons[m_joyButton_Y] == InputAdapter::JustPressed)
                {
                    result = ITF_IDYES;
                    break;
                }

                if (_type & (ITF_MSG_YESNO | ITF_MSG_YESNOCANCEL))
                {
                    if (buttons[m_joyButton_A] == InputAdapter::JustPressed)
                    {
                        result = ITF_IDNO;
                        break;
                    }
                }

                if (_type & ITF_MSG_YESNOCANCEL)
                {
                    if (buttons[m_joyButton_B] == InputAdapter::JustPressed)
                    {
                        result = ITF_IDCANCEL;
                        break;
                    }
                }
            }

            if (result != I32_MAX)
            {
                break;
            }

            sleep(10);
        }

        sb_DisplayMessageBoxInProgress = bfalse;
        return result == I32_MAX ? ITF_IDCANCEL : result;

    #else  // ITF_FINAL

        return ITF_IDCANCEL;

    #endif  // ITF_FINAL
    }

    String SystemAdapter_Nintendo::userName() const
    {
        return "user";
    }

}  // namespace ITF
