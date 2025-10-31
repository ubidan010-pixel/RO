#ifndef ITF_SYSTEMADAPTER_PS5_H_
#define ITF_SYSTEMADAPTER_PS5_H_

#pragma once
#include <app_content.h>
#include <system_service.h>
#include "user_service/user_service_api.h"
#include "core/AdaptersInterfaces/SystemAdapter.h"

namespace ITF
{

class SystemAdapter_PS5 : public SystemAdapter
{
public:

    SystemAdapter_PS5();
    ~SystemAdapter_PS5();

    virtual bbool initialize() override;
    virtual bbool openGraphics(int _width, int _height, int _alphaBits, int _depthBits, int _stencilBits, bbool _fullscreen = bfalse, bbool _mouseCursor = bfalse, const String & _name = "", bbool _waitVBL = btrue) override;
    virtual void terminate() override;
    virtual void present() override;
    virtual void update(bbool _minimal) override;

    virtual bbool getWindowSize(int& _width, int& _height) override;

    virtual void timerStart() override;
    virtual void timerStop() override;

    virtual f64 getTime() const override;
    virtual f64 getElapsedTime() override;
    virtual f32 getfPs() override;;

    virtual bbool isHDDMode() const override;
    // On PS5, it is more the SKU territory setup in param.json userDefinedParam1
    virtual ITF_TERRITORY getSystemTerritory() const override;

    virtual ITF_LANGUAGE getSystemLanguage(const ITF_LANGUAGE* _supportedLanguages, u32 _supportedCount, ITF_LANGUAGE _defaultLanguage) const override;

    // On PS5 it returns a generic user name, not the system one
    virtual String userName() const override;
    virtual String  dateTime() const override;
    virtual void getTime(Time& _time) const override;

    virtual void queryPerformanceCounter(LARGE_INTEGER* counter) const override;
    virtual void queryPerformanceFrequency(LARGE_INTEGER* TicksPerSecond) const override;

    virtual const char* getPlatformName()  override { return "PS5"; }

    virtual i32 messageBox(const String& _title, const String& _msg, u32 _type, ITF_MESSAGEBOX_RESULT silentResult) override;

    virtual void hideStartupSplashScreen() override;

    String8 getUserSystemName(u32 _userIndexControlerId) const;

    enum class SystemEventType : u32
    {
        ON_RESUME,
        ON_UNKNOWN,
    };
    using SystemEventCallback = void (*)(void * userData, SystemEventType evtType, void * platformEventData);

    void registerSystemEventCallback(SystemEventCallback cb, void * userParam);

    struct SystemStatus
    {
        bool isSystemUiOverlaid = false;
        bool isInBackgroundExecution = false;
    };
    SystemStatus getSystemStatus() const;

    SceUserServiceUserId getInitialUserId() const { return m_initialUserId; }

private:
    void initUsers();

    LARGE_INTEGER GetAdjustedCurrentTime() const;
    void updateSystemServiceEvents();
    void updateUsers();
    void updatePauseState();

    i32 getUserIdx(i32 _userID);
    i32 getUnusedUserIdx();

    // message box init and reentrancy check
    bool m_messageBoxLibInit = false;

    // Timer relative members
    i64 m_QPFTicksPerSec = 0;
    i64 m_stopTime = 0;
    i64 m_lastElapsedTime = 0;
    i64 m_baseTime = 0;
    f64 m_LastFPSTime = 0;
    bool m_timerStopped = bfalse;
    u64 m_NumFrames = 0;
    f64 m_fPs = 60;

    class InputAdapter_PS5* m_inputAdapter = nullptr;

    SceSystemServiceStatus m_systemStatus{};
    SystemEventCallback m_systemEvtCB = nullptr;
    void* m_systemEvtUserParam = nullptr;

    bbool m_gamePaused = false;

    SceUserServiceLoginUserIdList m_usersIDs{};

    SceAppContentBootParam m_bootParam{};
    // The 4 values set in param.json as userDefinedParamN.
    // Use them to configure behavior of the game through param.json.
    i32 m_userDefinedAppParam1 = 0;
    i32 m_userDefinedAppParam2 = 0;
    i32 m_userDefinedAppParam3 = 0;
    i32 m_userDefinedAppParam4 = 0;

    bool m_splashScreenHidden = false;

    SceUserServiceUserId m_initialUserId = -1;
};

#define SYSTEM_ADAPTER_PS5 (static_cast< SystemAdapter_PS5* > (SYSTEM_ADAPTER))

} // namespace ITF


#endif //ITF_SYSTEMADAPTER_PS5_H_
