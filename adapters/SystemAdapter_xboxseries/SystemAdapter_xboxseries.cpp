#include "precompiled_SystemAdapter_xboxseries.h"

#include "adapters/SystemAdapter_xboxseries/SystemAdapter_xboxseries.h"

#include "adapters/InputAdapter_xboxseries/InputAdapter_xboxseries.h"
#include "adapters/GFXAdapter_DX12/GFXAdapter_DX12.h"
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#include "engine/singleton/Singletons.h"
#include "core/error/ErrorHandler.h"
#include "core/UnicodeTools.h"
#include "core/memory/UniquePtr.h"

#include <memory>

#include <XUser.h>
#include <appnotify.h>
#include <XPackage.h>
#include <XGameUI.h>
#include <XSystem.h>
#include <XGame.h>

#include <xsapi-c/services_c.h>

namespace ITF
{
    SystemAdapter_XBoxSeries::SystemAdapter_XBoxSeries()
    {
        LARGE_INTEGER ticksPerSec = { 0 };
        QueryPerformanceFrequency( &ticksPerSec );
        m_QPFTicksPerSec = ticksPerSec.QuadPart;

        timerStart();

        // Determine the device type
        const XSystemDeviceType deviceType = ::XSystemGetDeviceType();

        if (deviceType == XSystemDeviceType::XboxScarlettLockhart)
            m_devicePlatformType = DevicePlatformType::XboxSeriesS;
        else if (deviceType == XSystemDeviceType::XboxScarlettAnaconda || deviceType == XSystemDeviceType::XboxScarlettDevkit)
            m_devicePlatformType = DevicePlatformType::XboxSeriesX; // Assume Series X for devkit
        else
            m_devicePlatformType = DevicePlatformType::Unknown;

        m_isDeviceDevkit = (deviceType == XSystemDeviceType::XboxScarlettDevkit);


    }
    
    SystemAdapter_XBoxSeries::~SystemAdapter_XBoxSeries()
    {
    }

    bbool SystemAdapter_XBoxSeries::initialize()
    {
        initTitleId();
        addInitialUser();
        initSuspendEvents();
        initConstrainedEvents();
        initXBoxLive();
        return btrue;
    }

    void SystemAdapter_XBoxSeries::initSuspendEvents()
    {
        // m_plmSuspendProcessComplete will be waited by the callback and signaled by the app to acknowledge the suspend.
        m_plmSuspendProcessComplete = ::CreateEventExA(nullptr, nullptr, 0, EVENT_MODIFY_STATE | SYNCHRONIZE);
        // m_plmResumeProcessFromSuspend will be waited by the app and signaled by the callback to indicate that the app can resume from suspend.
        m_plmResumeProcessFromSuspend = ::CreateEventExA(nullptr, nullptr, 0, EVENT_MODIFY_STATE | SYNCHRONIZE);

        ITF_VERIFY_HR_CALL(RegisterAppStateChangeNotification(
            [](BOOLEAN quiesced, PVOID context)
            {
                auto systemAdapter = static_cast<SystemAdapter_XBoxSeries*>(context);
                if (quiesced)
                {
                    {
                        ScopeLockMutex locker(systemAdapter->m_mutexPLM);
                        ITF_ASSERT(!systemAdapter->m_suspendRequested);
                        systemAdapter->m_suspendRequested = btrue;
                    }
                    //ITF_VERIFY(::SetEvent(systemAdapter->m_plmResumeProcessFromSuspend));
                    // Wait until the app acknowledges that it has completed its suspend processing.
                    ITF_VERIFY(WAIT_OBJECT_0 == ::WaitForSingleObject(systemAdapter->m_plmSuspendProcessComplete, INFINITE));
                }
                else
                {
                    {
                        ScopeLockMutex locker(systemAdapter->m_mutexPLM);
                        ITF_ASSERT(systemAdapter->m_suspendRequested);
                        systemAdapter->m_suspendRequested = bfalse;
                    }
                    ITF_VERIFY(::SetEvent(systemAdapter->m_plmResumeProcessFromSuspend));
                }
            },
            this, &m_hProcessLifetimeManagement));
    }

    void SystemAdapter_XBoxSeries::initConstrainedEvents()
    {
        ITF_VERIFY_HR_CALL(RegisterAppConstrainedChangeNotification([](BOOLEAN constrained, PVOID context)
        {
            auto systemAdapter = static_cast<SystemAdapter_XBoxSeries*>(context);
            ScopeLockMutex locker(systemAdapter->m_mutexPLM);
            systemAdapter->m_isConstrained = constrained;
        }, this, &m_hProcessLifetimeManagementConstrained));
    }

    void SystemAdapter_XBoxSeries::initTitleId()
    {
        if (FAILED(XGameGetXboxTitleId(&m_titleId)))
        {
            LOG("Failed to get Title ID. Assuming a default value for development.");
        }
        m_titleIdStr.setTextFormat("%08X", m_titleId);
        m_serviceConfigurationIdStr.setTextFormat("00000000-0000-0000-0000-0000%08X", m_titleId);
        LOG("Title ID: %s", m_titleIdStr.cStr());
    }

    void SystemAdapter_XBoxSeries::initXBoxLive()
    {
        XblInitArgs xblIniArgs = {};
        xblIniArgs.scid = m_serviceConfigurationIdStr.cStr();
        HRESULT hr = XblInitialize(&xblIniArgs);
        ITF_ASSERT_HR_RESULT_NAMED(hr, "XblInitialize");
        if (FAILED(hr))
        {
            LOG("XboxLive Initialisation failed!");
        }
    }

    void SystemAdapter_XBoxSeries::addUserComplete(XAsyncBlock & _asyncBlock, bool _isInitialUser)
    {
        UserInfo userInfo{};
        ITF_VERIFY_HR_CALL(XUserAddResult(&_asyncBlock, &userInfo.handle));
        ITF_VERIFY_HR_CALL(XUserGetLocalId(userInfo.handle, &userInfo.localId));
        ITF_VERIFY_HR_CALL(XUserGetId(userInfo.handle, &userInfo.userId));

        ScopeLock lock(m_mutexUser);
        if (_isInitialUser)
        {
            m_initialUserInfo = userInfo;
        }

        auto iter = std::find_if(
            m_usersInfo.begin(),
            m_usersInfo.end(),
            [&userInfo](const UserInfo& elt)
            {
                return userInfo.localId.value == elt.localId.value;
            });

        if (iter != m_usersInfo.end())
        {
            LOG("User already known\n");
        }
        else
        {
            m_usersInfo.push_back(userInfo);
        }
    }

    void SystemAdapter_XBoxSeries::addInitialUser()
    {
        auto asyncBlock = makeUnique<MemoryId::mId_System, XAsyncBlock>();
        *asyncBlock = {};
        asyncBlock->queue = nullptr; // default queue
        asyncBlock->context = this;
        asyncBlock->callback = [](XAsyncBlock* ab)
        {
            auto asyncBlock = UniquePtr<XAsyncBlock>{ ab };
            static_cast<SystemAdapter_XBoxSeries*>(ab->context)->addUserComplete(*ab, true);
        };

        XUserAddOptions options = XUserAddOptions::AddDefaultUserSilently;

        HRESULT hrUserAdd = XUserAddAsync(options, asyncBlock.get());
        ITF_ASSERT_HR_RESULT(hrUserAdd);
        if (SUCCEEDED(hrUserAdd))
        {
            // The call succeeded, so the destroy of the AsyncBlock will be done after the callback.
            // If the call fails, the unique_ptr will keep ownership and delete the XAsyncBlock*
            asyncBlock.release();
        }
    }

    void SystemAdapter_XBoxSeries::setResolution()
    {
        u32 resolutionWidth = 3840;
        u32 resolutionHeight = 2160;

        // Assume 4K resolution except on SeriesS.
        if (m_devicePlatformType == DevicePlatformType::XboxSeriesS)
        {
            resolutionWidth = 2560;
            resolutionHeight = 1440;
        }
        GFX_ADAPTER->setResolution(resolutionWidth, resolutionHeight);
    }

    LRESULT CALLBACK SystemAdapter_XBoxSeries::winProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        SystemAdapter_XBoxSeries* systemAdapter = nullptr;
        if (uMsg == WM_CREATE)
        {
            CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
            systemAdapter = reinterpret_cast<SystemAdapter_XBoxSeries*>(pCreate->lpCreateParams);
            ::SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(systemAdapter));
            systemAdapter->setMainWindow(hwnd);
        }
        else
        {
            systemAdapter = reinterpret_cast<SystemAdapter_XBoxSeries*>(::GetWindowLongPtr(hwnd, GWLP_USERDATA));
        }

        if (systemAdapter)
        {
            return systemAdapter->handleWindowMessage(uMsg, wParam, lParam);
        }
        else
        {
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
    }

    LRESULT SystemAdapter_XBoxSeries::handleWindowMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        LOG("handleWindowMessage (type %d)", uMsg);

        switch (uMsg)
        {
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            return  S_OK;
        }

        case WM_ACTIVATEAPP:
        {
            const bool activated = (wParam);
            LOG("handleWindowMessage WM_ACTIVATEAPP %s", activated ? "Activate" : "Deactivate");
            break;
        }

        case WM_SHOWWINDOW:
        {
            const bool visible = (wParam);
            LOG("handleWindowMessage WM_SHOWWINDOW %s", visible ? "Show" : "Hide");
            break;
        }
        default:
            return DefWindowProc(getHwnd(), uMsg, wParam, lParam);
        }
        return  S_OK;

    }

    void SystemAdapter_XBoxSeries::createWindow(String _name)
    {
        String8 name8 = StringToUTF8(_name).get();
        String8 className8 = name8 + "_class";

        // Register Class
        HINSTANCE hInstance = ::GetModuleHandle(NULL);
        WNDCLASSEXA wcex = {};
        wcex.cbSize = sizeof(WNDCLASSEXA);
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = winProc;
        wcex.hInstance = hInstance;
        wcex.lpszClassName = className8.cStr();
        wcex.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
        ITF_VERIFY(::RegisterClassExA(&wcex));

        // Create Window
        m_hwnd = ::CreateWindowExA(0, className8.cStr(), name8.cStr(), WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, GFX_ADAPTER->getScreenWidth(), GFX_ADAPTER->getScreenHeight(), nullptr, nullptr, hInstance, this);
        ITF_ASSERT(m_hwnd);

        // The window creation should have set the user data to 'this'
        ITF_ASSERT(::GetWindowLongPtr(getHwnd(), GWLP_USERDATA) == reinterpret_cast<LONG_PTR>(this));

        ITF_VERIFY(::ShowWindow(getHwnd(), SW_SHOWDEFAULT));
    }

    bbool SystemAdapter_XBoxSeries::openGraphics (int _width, int _height, int _alphaBits, int _depthBits, int _stencilBits, bbool _fullscreen, bbool _mouseCursor, const String& _name, bbool _waitVBL )
    {
        setResolution();
        createWindow(_name);

        m_gfxAdapterDX12 = static_cast<GFXAdapter_DX12*>(GFX_ADAPTER);

        m_gfxAdapterDX12->setWaitVBL(_waitVBL);
        if (!m_gfxAdapterDX12->createRenderingPipeline(_alphaBits, _depthBits, _stencilBits, _fullscreen, m_hwnd))
        {
            return bfalse;
        }

        GFX_ADAPTER->init();

        Singletons::get().setInputAdapter(new (MemoryId::mId_System) InputAdapter_XBoxSeries());

        return btrue;
    }

    void SystemAdapter_XBoxSeries::terminate(   )
    {
        // What to do ?
        ITF_ASSERT(0);
    }

    void SystemAdapter_XBoxSeries::present()
    {
        /// inform adapter.
        ITF::GFX_ADAPTER->setfPs(getfPs());

        // Present frame buffer.
        ITF::GFX_ADAPTER->endRendering();
        ITF::GFX_ADAPTER->present();

        m_NumFrames++;
    }

    f64 SystemAdapter_XBoxSeries::getTime() const
    {
        LARGE_INTEGER curtime = GetAdjustedCurrentTime();
        f64 time = (f64)(curtime.QuadPart - m_baseTime) / (f64)m_QPFTicksPerSec;
        return time;

    }

    LARGE_INTEGER SystemAdapter_XBoxSeries::GetAdjustedCurrentTime() const
    {
        LARGE_INTEGER time;
        if (m_stopTime != 0)
            time.QuadPart = m_stopTime;
        else
            QueryPerformanceCounter(&time);
        return time;
    }

    f64 SystemAdapter_XBoxSeries::getElapsedTime()
    {
        LARGE_INTEGER time = GetAdjustedCurrentTime();

        f64 elapsedTime = ((f64)(time.QuadPart - m_lastElapsedTime) / (f64)
            m_QPFTicksPerSec);
        m_lastElapsedTime = time.QuadPart;

        if (elapsedTime < 0.0f)
            elapsedTime = 0.0f;

        return elapsedTime;
    }

    f32 SystemAdapter_XBoxSeries::getfPs()
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

    void SystemAdapter_XBoxSeries::fetchSuspendRequest()
    {
        auto isSuspendRequested = [&]()
            {
                ScopeLockMutex locker{ m_mutexPLM };
                return m_suspendRequested;
            };

        if (isSuspendRequested())
        {
            if (m_gfxAdapterDX12)
            {
                m_gfxAdapterDX12->onSuspendApp();
            }
            ITF_VERIFY(::SetEvent(m_plmSuspendProcessComplete));
            ITF_VERIFY(WAIT_OBJECT_0 == ::WaitForSingleObject(m_plmResumeProcessFromSuspend, INFINITE));
            if (m_gfxAdapterDX12)
            {
                ITF_ASSERT(!isSuspendRequested());
                m_gfxAdapterDX12->onResumeApp();
            }
        }
    }

    void SystemAdapter_XBoxSeries::updatePauseState()
    {
        auto isConstrained = [&]()
        {
            ScopeLockMutex locker{ m_mutexPLM };
            return m_isConstrained;
        };

        bool needPause = isConstrained();

        if (!m_gamePausedByConstrained && needPause)
        {
            setSystemPauseFlag(btrue);
            // need to pause sound ?
            //SOUND_MANAGER->pauseAll(SoundManager::PAUSE_LEVEL_MAX);

        }

        if (m_gamePausedByConstrained && !needPause)
        {
            setSystemPauseFlag(bfalse);
            // need to resume sound ?
            //SOUND_MANAGER->resumeAll(SoundManager::PAUSE_LEVEL_MAX);
        }

        m_gamePausedByConstrained = needPause;
    }

    void SystemAdapter_XBoxSeries::update(bbool _minimal)
    {
        fetchSuspendRequest();
        updatePauseState();

        if (!_minimal)
        {
            InputAdapter_XBoxSeries* inputAdapter = INPUT_ADAPTER_XBOXSERIES;
            if (inputAdapter)
                inputAdapter->updateAllInputState();
        }
    }

    bbool SystemAdapter_XBoxSeries::getWindowSize(int& _width, int& _height)
    {
        _width = GFX_ADAPTER->getScreenWidth();
        _height = GFX_ADAPTER->getScreenHeight();
        return btrue;
    }

    void SystemAdapter_XBoxSeries::timerStart()
    {
        // Get the current time
        LARGE_INTEGER time = { 0 };
        QueryPerformanceCounter(&time);

        if (m_timerStopped)
            m_baseTime += time.QuadPart - m_stopTime;
        m_stopTime = 0;
        m_lastElapsedTime = time.QuadPart;
        m_timerStopped = FALSE;
    }

    void SystemAdapter_XBoxSeries::timerStop()
    {
        if (!m_timerStopped)
        {
            LARGE_INTEGER time = { 0 };
            QueryPerformanceCounter(&time);
            m_stopTime = time.QuadPart;
            m_lastElapsedTime = time.QuadPart;
            m_timerStopped = btrue;
        }
    }

    String SystemAdapter_XBoxSeries::userName() const
    {
        ScopeLock lock(m_mutexUser);
        if (m_initialUserInfo.handle)
        {
            char gamertag[16]{};
            size_t gamertagUsed = 0;
            HRESULT hrGetGamerTag = XUserGetGamertag(
                m_initialUserInfo.handle,
                XUserGamertagComponent::Classic,
                ITF_ARRAY_SIZE(gamertag),
                gamertag,
                &gamertagUsed);
            ITF_ASSERT_HR_RESULT(hrGetGamerTag);
            ITF_ASSERT(gamertagUsed > 0);
            ITF_ASSERT(gamertagUsed <= ITF_ARRAY_SIZE(gamertag));
            if (SUCCEEDED(hrGetGamerTag))
            {
                return String(gamertag);
            }
        }
        return "Player";
    }

    static ITF_LANGUAGE convertXboxLocalLangToITF(const char * _langCode)
    {
        struct LangList
        {
            const char * langStr;
            ITF_LANGUAGE language;
        };
        static const LangList platformlanglist[] =
        {
            { "en-US", ITF_LANGUAGE_ENGLISH },
            { "en-GB", ITF_LANGUAGE_ENGLISH },
            { "en-AU", ITF_LANGUAGE_ENGLISH },
            { "en-CA", ITF_LANGUAGE_ENGLISH },
            { "en-CZ", ITF_LANGUAGE_ENGLISH },
            { "en-GR", ITF_LANGUAGE_ENGLISH },
            { "en-HK", ITF_LANGUAGE_ENGLISH },
            { "en-HU", ITF_LANGUAGE_ENGLISH },
            { "en-IN", ITF_LANGUAGE_ENGLISH },
            { "en-IE", ITF_LANGUAGE_ENGLISH },
            { "en-IL", ITF_LANGUAGE_ENGLISH },
            { "en-NZ", ITF_LANGUAGE_ENGLISH },
            { "en-SA", ITF_LANGUAGE_ENGLISH },
            { "en-SG", ITF_LANGUAGE_ENGLISH },
            { "en-SK", ITF_LANGUAGE_ENGLISH },
            { "en-ZA", ITF_LANGUAGE_ENGLISH },
            { "en-AE", ITF_LANGUAGE_ENGLISH },

            { "fr-FR", ITF_LANGUAGE_FRENCH },
            { "fr-BE", ITF_LANGUAGE_FRENCH },
            { "fr-CA", ITF_LANGUAGE_FRENCH },
            { "fr-CH", ITF_LANGUAGE_FRENCH },

            { "ja-JP", ITF_LANGUAGE_JAPANESE },

            { "de-DE", ITF_LANGUAGE_GERMAN },
            { "de-AT", ITF_LANGUAGE_GERMAN },
            { "de-CH", ITF_LANGUAGE_GERMAN },

            { "es-ES", ITF_LANGUAGE_SPANISH },
            { "es-AR", ITF_LANGUAGE_SPANISH },
            { "es-CL", ITF_LANGUAGE_SPANISH },
            { "es-CO", ITF_LANGUAGE_SPANISH },
            { "es-MX", ITF_LANGUAGE_SPANISH },

            { "it-IT", ITF_LANGUAGE_ITALIAN },

            { "ko-KR", ITF_LANGUAGE_KOREAN },

            { "zh-TW", ITF_LANGUAGE_TRADITIONALCHINESE },
            { "zh-HK", ITF_LANGUAGE_TRADITIONALCHINESE },

            { "pt-PT", ITF_LANGUAGE_PORTUGUESE },
            { "pt-BR", ITF_LANGUAGE_PORTUGUESE },

            { "zh-CN", ITF_LANGUAGE_SIMPLIFIEDCHINESE  },
            { "zh-SG", ITF_LANGUAGE_SIMPLIFIEDCHINESE  },

            { "pl-PL", ITF_LANGUAGE_POLISH },

            { "ru-RU", ITF_LANGUAGE_RUSSIAN },

            { "nl-NL", ITF_LANGUAGE_DUTCH },
            { "nl-BE", ITF_LANGUAGE_DUTCH },

            { "cs-CZ", ITF_LANGUAGE_CZECH },

            {"hu-HU", ITF_LANGUAGE_HUNGARIAN },
        };

        for(LangList const & langItem : platformlanglist)
        {
            if (0 == strcmp(_langCode, langItem.langStr))
                return langItem.language;
        }

        return ITF_LANGUAGE_UNKNOWN;
    }

    ITF_LANGUAGE SystemAdapter_XBoxSeries::getSystemLanguage(const ITF_LANGUAGE* _supportedLanguages, u32 _supportedCount, ITF_LANGUAGE _defaultLanguage) const
    {
        char userLocale[LOCALE_NAME_MAX_LENGTH] = { 0 };

        // Get the best user locale that is installed
        HRESULT hrGetLang = ::XPackageGetUserLocale(_countof(userLocale), userLocale);
        ITF_ASSERT_HR_RESULT(hrGetLang);
        ITF_UNUSED(hrGetLang);

        ITF_LANGUAGE language = convertXboxLocalLangToITF(userLocale);
        return filterSupportedLanguages(language, _supportedLanguages, _supportedCount, _defaultLanguage);
    }

    String SystemAdapter_XBoxSeries::dateTime() const
    {
        SYSTEMTIME localSysTime;
        ::GetLocalTime(&localSysTime);

        String result;
        result.setStringFormat(
            "%04d-%02d-%02d %02d:%02d:%02d",
            localSysTime.wYear, localSysTime.wMonth, localSysTime.wDay,
            localSysTime.wHour, localSysTime.wMinute, localSysTime.wSecond
        );
        return result;
    }

    void SystemAdapter_XBoxSeries::getTime(Time& _time) const
    {
        SYSTEMTIME localSysTime;
        ::GetLocalTime(&localSysTime);
        _time.m_year = localSysTime.wYear;
        _time.m_month = localSysTime.wMonth;
        _time.m_day = localSysTime.wDay;
        _time.m_hour = localSysTime.wHour;
        _time.m_minute = localSysTime.wMinute;
        _time.m_second = localSysTime.wSecond;
    }

    bbool SystemAdapter_XBoxSeries::isTrialMode() const
    {
        return bfalse;
    }

    const char* SystemAdapter_XBoxSeries::getPlatformName()
    {
        if (m_devicePlatformType == DevicePlatformType::XboxSeriesX)
            return "XBoxSeriesX";
        else if (m_devicePlatformType == DevicePlatformType::XboxSeriesS)
            return "XBoxSeriesS";
        else
            return "XBoxSeriesUnknown";
    }

    void SystemAdapter_XBoxSeries::queryPerformanceFrequency(LARGE_INTEGER* ticksPerSecond) const
    {
        ::QueryPerformanceFrequency(ticksPerSecond);
    }

    void SystemAdapter_XBoxSeries::queryPerformanceCounter(LARGE_INTEGER* counter) const
    {
        ::QueryPerformanceCounter(counter);
    }

    i32 SystemAdapter_XBoxSeries::messageBox(const String& _title, const String& _msg, u32 _type, ITF_MESSAGEBOX_RESULT _silentResult)
    {
        return messageBoxCustom(_title, _msg, _type, _silentResult);
    }

#ifndef ITF_FINAL

    static i32 showXBoxSeriesMessage(
        const char* _title,
        const char* _msg,
        const XGameUiMessageDialogButton _defaultButton,
        const XGameUiMessageDialogButton _cancelButton,
        const char* _option1,
        const char* _option2,
        const char* _option3)
    {
        i32 choice = ITF_IDYES;

        XTaskQueueHandle queue;
        XTaskQueueCreate(XTaskQueueDispatchMode::ThreadPool, XTaskQueueDispatchMode::Immediate, &queue);

        XAsyncBlock* ab = new XAsyncBlock;
        ZeroMemory(ab, sizeof(XAsyncBlock));
        ab->queue = queue;

        XGameUiMessageDialogButton button;

        if (SUCCEEDED(XGameUiShowMessageDialogAsync(ab, _title, _msg,
            _option1, _option2, _option3,
            _defaultButton,
            _cancelButton)) &&
            SUCCEEDED(XAsyncGetStatus(ab, true)) &&
            SUCCEEDED(XGameUiShowMessageDialogResult(ab, &button)))
        {
            choice = i32(button);
        }
        else
        {
            choice = -1;
        }

        return choice;
    }

    static i32 showMessageBox(const char* _title, const char* _msg, u32 _type, const char* _option1, const char* _option2, const char* _option3, const char* /*_option4*/)
    {
        i32 choice = ITF_IDYES;

        u32 mask = 0;
        if (_type & ITF_MSG_YESNOCANCEL)
        {
            mask = ITF_MSG_YESNOCANCEL;
        }
        else if (_type & ITF_MSG_YESNO)
        {
            mask = ITF_MSG_YESNO;
        }
        else if (_type & ITF_MSG_OK)
        {
            mask = ITF_MSG_OK;
        }
        else
        {
            return -1; // otherwise mixing ITF_MSG_* flags
        }

        const char* option1(_option1 ? _option1 : "Yes");
        const char* option2(_option2 ? _option2 : "No");
        const char* option3(_option3 ? _option3 : "Cancel");

        switch (mask)
        {
        case ITF_MSG_YESNO:
            choice = showXBoxSeriesMessage(_title, _msg, XGameUiMessageDialogButton::First, XGameUiMessageDialogButton::Second, option1, option2, NULL);
            break;
        case ITF_MSG_YESNOCANCEL:
            choice = showXBoxSeriesMessage(_title, _msg, XGameUiMessageDialogButton::First, XGameUiMessageDialogButton::Third, option1, option2, option3);
            break;
        case ITF_MSG_OK:
            choice = showXBoxSeriesMessage(_title, _msg, XGameUiMessageDialogButton::First, XGameUiMessageDialogButton::First, option1, NULL, NULL);
            break;
        default:
            choice = -1;
        }

        return choice;
    }
#endif ITF_FINAL

    i32 SystemAdapter_XBoxSeries::messageBoxCustom(const String& _title, const String& _msg, u32 _type, ITF_MESSAGEBOX_RESULT _silentResult, bool _forceDisplay, const char* _option1, const char* _option2, const char* _option3, const char* _option4)
    {
    #ifndef ITF_FINAL
        hideStartupSplashScreen();

        if (Synchronize::getCurrentThreadId() == ThreadSettings::m_settings[eThreadId_mainThread].m_threadID)
        {
            LOG("* MESSAGE BOX *\n"
                "Title: %ls\n"
                "Message: %ls\n",
                _title.wcharCStr(),
                _msg.wcharCStr());
            return showMessageBox(StringToUTF8(_title).get(), StringToUTF8(_msg).get(), _type, _option1, _option2, _option3, _option4);
        }
        else
        {
            LOG("+=========================+\n"
                "|       MESSAGE BOX       |\n"
                "+=========================+\n"
                "\n"
                "Title: %s\n"
                "\n"
                "Message: %s\n"
                "\n"
                "Message box on a thread, return silent result %s\n",
                _title, _msg,
                _silentResult == ITF_IDYES
                    ? "YES"
                : (_silentResult == ITF_IDNO ? "NO" : "CANCEL"));
            return _silentResult;
        }
#else
        return -1;
#endif
    }

    void* SystemAdapter_XBoxSeries::allocatePhysicalMemory(u32 _size, u32 _alignment)
    {
        ITF_NOT_IMPLEMENTED();
        return nullptr;
    }

    void SystemAdapter_XBoxSeries::freePhysicalMemory(void* _pointer)
    {
        ITF_NOT_IMPLEMENTED();
    }

    SystemAdapter_XBoxSeries::UserInfo SystemAdapter_XBoxSeries::getInitialUserInfo() const
    {
        ScopeLockMutex lock(m_mutexUser);
        return m_initialUserInfo;
    }
} // namespace ITF

