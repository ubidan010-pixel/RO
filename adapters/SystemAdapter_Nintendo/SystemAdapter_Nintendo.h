#ifndef ITF_SYSTEMADAPTER_NINTENDO_H_
#define ITF_SYSTEMADAPTER_NINTENDO_H_

#pragma once
#include "core/AdaptersInterfaces/SystemAdapter.h"
#include <nn/oe.h>
#include <nn/account/account_Types.h>

#include <nn/vi.h>

namespace ITF
{

    class ISystemNotificationListener_Nintendo
    {
       public:
        virtual void onSystemNotification(const nn::oe::Message& _message) = 0;
    };

    class SystemAdapter_Nintendo : public SystemAdapter, public ISystemNotificationListener_Nintendo
    {
    public:

        SystemAdapter_Nintendo();
        ~SystemAdapter_Nintendo();
        SystemAdapter_Nintendo(const SystemAdapter_Nintendo &) = delete;
        SystemAdapter_Nintendo & operator = (const SystemAdapter_Nintendo&) = delete;

        void preInitialize();

        virtual bbool initialize() override;
        virtual bbool openGraphics(int _width, int _height, int _alphaBits, int _depthBits, int _stencilBits, bbool _fullscreen = bfalse, bbool _mouseCursor = bfalse, const String& _name = "", bbool _waitVBL = btrue) override;
        virtual void terminate() override;
        virtual void present() override;
        virtual void update(bbool _minimal) override;

        virtual bbool getWindowSize(int& _width, int& _height) override;

        virtual void timerStart() override;
        virtual void timerStop() override;

        virtual f64 getTime() const override;
        virtual f64 getElapsedTime() override;
        virtual f32 getfPs() override;;

        virtual bbool isHDDMode() const override { return btrue; }

        virtual ITF_TERRITORY getSystemTerritory() const override;

        virtual ITF_LANGUAGE getSystemLanguage(const ITF_LANGUAGE* _supportedLanguages, u32 _supportedCount, ITF_LANGUAGE _defaultLanguage) const override;

        virtual String userName() const override;
        virtual String  dateTime() const override;
        virtual void getTime(Time& _time) const override;

        virtual void queryPerformanceCounter(LARGE_INTEGER* counter) const override;
        virtual void queryPerformanceFrequency(LARGE_INTEGER* TicksPerSecond) const override;

        virtual const char* getPlatformName()  override;

        virtual i32 messageBox(const String& _title, const String& _msg, u32 _type, ITF_MESSAGEBOX_RESULT silentResult) override;

        virtual void hideStartupSplashScreen() override;

        virtual bbool isBackAndSelectButtonsInverted() const override { return btrue; }

        // ISystemNotificationListener_Nintendo implementation
        virtual void onSystemNotification(const nn::oe::Message& _message) override;

        // non virtual methods
        void registerNotificationListener(ISystemNotificationListener_Nintendo* _listener);
        void unregisterNotificationListener(ISystemNotificationListener_Nintendo* _listener);

        nn::account::Uid getInitialUserId() const { return m_initialUserID; }
        nn::account::UserHandle getInitialUserHandle() const { return m_userHandle; }
        bool havePermissions(bool isSilent);

        enum class GamePlayMode
        {
            None,
            Docked,
            Handheld,
            TableTop,
        };
        GamePlayMode getGameplayMode() const { return m_gamePlayMode; }

    private:

        void initUsers();

        void updateNintendoSystemMessages();
        void updateInputAdapter();
        void updateGamePlayMode();
        void updateDisplayResolution();

        bool isDocked() const;

        LARGE_INTEGER getAdjustedCurrentTime() const;

        Vector<ISystemNotificationListener_Nintendo*> m_notificationListeners;
        bool m_splashScreenHidden = false;

        GamePlayMode m_gamePlayMode = GamePlayMode::None;
        nn::account::UserHandle m_userHandle{};
        nn::account::Uid m_initialUserID{};
        nn::account::NetworkServiceAccountId m_nsaId{};
        String8 m_token;
        
        nn::vi::Display* m_display = nullptr;
        nn::vi::Layer* m_layer = nullptr;

        i32 m_resolutionWidth = 0;
        i32 m_resolutionHeight = 0;

        i64 m_stopTime;
        i64 m_lastElapsedTime;
        i64 m_baseTime;
        f64 m_LastFPSTime;
        bbool m_timerStopped;
        u64 m_NumFrames;
        f64 m_fPs;
    #ifndef ITF_FINAL
        f64 m_VBLSyncTime;
        f64 m_fPs_NoVsync;
        u64 m_nbUpdateCalled;
    #endif // ITF_FINAL

    };

    #define SYSTEM_ADAPTER_NINTENDO (static_cast<SystemAdapter_Nintendo*>(SYSTEM_ADAPTER))

}  // namespace ITF

#endif  // ITF_SYSTEMADAPTER_NINTENDO_H_
