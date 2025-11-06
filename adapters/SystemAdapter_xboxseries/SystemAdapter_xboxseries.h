#pragma once

#include "core/AdaptersInterfaces/SystemAdapter.h"
#include "core/container/extendedVector.h"
#include <XUser.h>
#include <appnotify.h>

namespace ITF
{

    class GFXAdapter_DX12;

    class SystemAdapter_XBoxSeries : public SystemAdapter
    {
    public:
        SystemAdapter_XBoxSeries();
        virtual ~SystemAdapter_XBoxSeries() override;

        virtual bbool initialize() override;
        virtual bbool openGraphics(int _width, int _height, int _alphaBits, int _depthBits, int _stencilBits, bbool _fullscreen = bfalse, bbool _mouseCursor = bfalse, const String& _name = "", bbool _waitVBL = btrue) override;
        virtual void terminate() override;
        virtual void present() override;
        virtual f64 getTime() const override;
        virtual f64 getElapsedTime() override;
        virtual f32 getfPs() override;
        virtual void update(bbool _minimal) override;
        virtual bbool getWindowSize(int& _width, int& _height) override;

        virtual void timerStart() override;
        virtual void timerStop() override;

        virtual String userName() const override;
        virtual ITF_LANGUAGE getSystemLanguage(const ITF_LANGUAGE *_supportedLanguages, u32 _supportedCount, ITF_LANGUAGE _defaultLanguage) const override;
        virtual String dateTime() const override;
        virtual void getTime(Time & _time) const override;

        virtual bbool isTrialMode() const override;

        virtual const char* getPlatformName() override;

        LARGE_INTEGER   GetAdjustedCurrentTime() const;

        virtual void queryPerformanceCounter(LARGE_INTEGER *counter) const override;
        virtual void queryPerformanceFrequency(LARGE_INTEGER *counter) const override;

        virtual i32 messageBox(const String& _title, const String& _msg, u32 _type, ITF_MESSAGEBOX_RESULT silentResult) override;

        virtual void *allocatePhysicalMemory(u32 _size, u32 _alignment) override;
        virtual void freePhysicalMemory(void *_pointer) override;

        virtual void hideStartupSplashScreen() override {} // nothing like that in the GDK.

        enum class DevicePlatformType
        {
            XboxSeriesX,
            XboxSeriesS,
            Unknown
        };

        DevicePlatformType getDevicePlatformType() const { return m_devicePlatformType; }

        HWND getHwnd() const { return static_cast<HWND>(m_hwnd); }

        struct UserInfo
        {
            XUserHandle handle = nullptr;
            uint64_t userId = 0;
            XUserLocalId localId = XUserNullUserLocalId;
        };
        UserInfo getInitialUserInfo() const;
        const char* getServiceConfigurationId() const { return m_serviceConfigurationIdStr.cStr(); }

    protected:
        void setResolution();
        void createWindow(String _name);
        static LRESULT CALLBACK SystemAdapter_XBoxSeries::winProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
        LRESULT handleWindowMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
        void addInitialUser();
        void initSuspendEvents();
        void initConstrainedEvents();
        void initTitleId();
        void initXBoxLive();
        void fetchSuspendRequest();
        void updatePauseState();

        void addUserComplete(XAsyncBlock& asyncBlock, bool isInitialUser);

        i32 messageBoxCustom(const String& _title, const String& _msg, u32 _type, ITF_MESSAGEBOX_RESULT silentResult, bool _forceDisplay = false, const char* option1 = nullptr, const char* option2 = nullptr, const char* option3 = nullptr, const char* option4 = nullptr);

        DevicePlatformType m_devicePlatformType = DevicePlatformType::Unknown;
        bool m_isDeviceDevkit = false;

        GFXAdapter_DX12* m_gfxAdapterDX12 = nullptr;

        i64 m_QPFTicksPerSec = 0;
        i64 m_stopTime = 0;
        i64 m_lastElapsedTime = 0;
        i64 m_baseTime = 0;
        f64 m_LastFPSTime = 0.0;
        bbool m_timerStopped = bfalse;
        u64 m_NumFrames = 0;
        f64 m_fPs = 0.0;

        mutable Mutex m_mutexDialog; // Ensure only one message box at a time

        mutable Mutex m_mutexUser;
        UserInfo m_initialUserInfo;
        Vector<UserInfo> m_usersInfo;

        u32 m_titleId = 0x7E750470; // This default value come from the sample GameSaveCombo. It should be changed after init by requesting it.
        String8 m_titleIdStr;
        String8 m_serviceConfigurationIdStr;

        PAPPSTATE_REGISTRATION m_hProcessLifetimeManagement = nullptr;
        PAPPCONSTRAIN_REGISTRATION m_hProcessLifetimeManagementConstrained = nullptr;
        HANDLE m_plmSuspendProcessComplete = nullptr;
        HANDLE m_plmResumeProcessFromSuspend = nullptr;

        mutable Mutex m_mutexPLM{};
        bool m_suspendRequested = false;
        bool m_isConstrained = false;

        bool m_gamePausedByConstrained = false;
    };

    #define SYSTEM_ADAPTER_XBOX_SERIES (static_cast<SystemAdapter_XBoxSeries*>(SYSTEM_ADAPTER))

} // namespace ITF

