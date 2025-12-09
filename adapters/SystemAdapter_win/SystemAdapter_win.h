#ifndef ITF_SYSTEMADAPTER_WIN_H_
#define ITF_SYSTEMADAPTER_WIN_H_

#pragma once

#ifdef ITF_WINDOWS

#ifndef _ITF_SYSTEMADAPTER_
#include "core/AdaptersInterfaces/SystemAdapter.h"
#endif //_ITF_SYSTEMADAPTER_

#define TARGET_SCREEN_RATIO (16.0f / 9.0f)

namespace ITF
{

class WinApp
{
public:
    static HWND createWindow(int _width, int _height, bbool _fullscreen, const String& _name = "", const String& _class = "");
    static LRESULT CALLBACK wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    // error strings displayed during initialisation
    static const wchar_t  REGISTER_CLASS_ERROR[];
    static const wchar_t  CREATE_WINDOW_ERROR[];

    static void    onFileDrop(const String& _fileAbsolute);

    static bool getEvents(bbool& _idle);
};

//--------------------------------------------------------------------------------------
// Callback registration
//--------------------------------------------------------------------------------------

typedef void    (CALLBACK *W32SYSTEMCALLBACKKEYBOARD)( u32 nChar, bool bKeyDown, bool bLAlt, bool bRAlt, bool bLCtrl, bool bRCtrl, bool bLShift, bool bRShift, void* pUserContext );
typedef void    (CALLBACK *W32SYSTEMCALLBACKMOUSEPOS)( i32 xPos, i32 yPos, void* pUserContext );
typedef void    (CALLBACK *W32SYSTEMCALLBACKMOUSEBUTTON)( u32 _Button, u32 _action, void* pUserContext );
typedef void    (CALLBACK *W32SYSTEMCALLBACKMOUSEWHEEL)( i32 nMouseWheelDelta, void* pUserContext );

//--------------------------------------------------------------------------------------

class SystemAdapter_win : public SystemAdapter
{
public:
    bbool   initialize          (   );
    bbool   openGraphics        (   int _width, int _height, int _alphaBits, int _depthBits, int _stencilBits, bbool _fullscreen = bfalse, bbool _mouseCursor = bfalse, const String& _name = "", bbool _waitVBL=btrue );
    void    terminate           (   );
    void    present             (   );
    f64     getTime() const override;
    f64     getElapsedTime      (   );
    f32     getfPs              (   );
    void    update              (   bbool _minimal  );
    bbool   gameUpdateAllowed   (   );
    bbool   getWindowSize       (   int& _width, int& _height   );
    bbool   getWindowPos        (   int& _x, int& _y    );
    void    setWindowCloseCb    (   void* _func);
    void    timerStart          (   );
    void    timerStop           (   );
    void    getTime(Time & _time) const;
    void    setSize(int _w, int _h );
    void    setPositionAndSize(int _x, int _y,int _w,int _h);

    void    setCursorIcon(CursorIcon::Type _icon)const;
    i32     messageBox( const String& _title, const String& _msg, u32 _type,ITF_MESSAGEBOX_RESULT silentResult );
    f64     getEllaspedFromStart (  );
    void    toggleShowTitleBar();
    void    SetRegistryValue(const String& name, DWORD value) override;
    void    SetRegistryValue(const String& name, const String& value) override;
    void    toggleFullScreen();
    bbool   isAllowResize()         {return m_allowResize;}


    ITF_LANGUAGE getSystemLanguage(const ITF_LANGUAGE *_supportedLanguages, u32 _supportedCount, ITF_LANGUAGE _defaultLanguage) const;
    String  userName()const;
    virtual String  dateTime() const;

    void    saveWindowPos(const String& _szFilename);
    void*   registerWindow(u32 _screenWidth,u32 _screenHeight,const String& _szName,const String& _szFilename);

    void    registerClipBoardFormat();
    void    addTextToTitle(const String& _extraInfo);


    void    queryPerformanceCounter(LARGE_INTEGER *counter) const;
    void    queryPerformanceFrequency(LARGE_INTEGER *TicksPerSecond) const;

    const  char* getPlatformName() {return "win32";} // Why is it not changed to win or win64 ?

    void *allocatePhysicalMemory(u32 _size, u32 _alignment);
    void freePhysicalMemory(void *_pointer);

    virtual bbool isFullScreenMode() {return m_bFullScreenMode;}
    virtual void launchExe(const char* _exeName, const char* _params = NULL, bbool _avoidDoublons = bfalse, bbool _focus = btrue);

    SystemAdapter_win();
    ~SystemAdapter_win();

    /// Windows.
    HWND                        m_hwnd;
    WinApp*                     mp_winApp;

    /// Timer.
    LARGE_INTEGER   GetAdjustedCurrentTime() const;
    i64 m_QPFTicksPerSec;
    i64 m_stopTime;
    i64 m_lastElapsedTime;
    i64 m_baseTime;
    f64 m_LastFPSTime;
    bbool m_timerStopped;
    u64 m_NumFrames;
    f64 m_fPs;

    enum WINDOWSCREEN_STATE
    {
        Windowed,
        FakeFullscreen,
        Fullscreen,
        ENUM_FORCE_SIZE_32(0)
    };

    /// Calbacks.
    W32SYSTEMCALLBACKMOUSEPOS       mp_MousePosCallback;
    W32SYSTEMCALLBACKMOUSEBUTTON    mp_MouseButtonCallback;
    W32SYSTEMCALLBACKMOUSEWHEEL     mp_MouseWheelCallback;
    W32SYSTEMCALLBACKKEYBOARD       mp_KeyBoardCallback;

    void CALLBACK SetCallbackKeyboard( W32SYSTEMCALLBACKKEYBOARD pCallback, void* pUserContext = NULL );
    void CALLBACK SetCallbackMousePos( W32SYSTEMCALLBACKMOUSEPOS pCallback, void* pUserContext = NULL );
    void CALLBACK SetCallbackMouseButton( W32SYSTEMCALLBACKMOUSEBUTTON pCallback, void* pUserContext = NULL );
    void CALLBACK SetCallbackMouseWheel( W32SYSTEMCALLBACKMOUSEWHEEL pCallback, void* pUserContext = NULL );

    bool    m_Keys[256];                                // array of key state
    bool    m_LastKeys[256];                            // array of last key state
    i32     m_mouseWheelPosition;

protected:
    bbool       m_windowRectWhenGoingToFakeFullscreen_Valid;
    RECT        m_windowRectWhenGoingToFakeFullscreen;

    WINDOWSCREEN_STATE m_windowScreenState;

    String      m_OriginalCaption;
    bbool       m_bFullScreenMode;
    bbool       m_allowUpdate;
    volatile bbool m_allowResize;


    void    getWindowAdjusted(const RECT& source,int& Width,int& height);

    String      recordModuleName();

    void        toggleWindowMode(bbool bFullScreen);

    void*       findWindowbyName(const String& _szName);

    static BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData/*contains pointer to output RECT*/);
private:
    HCURSOR m_cursorIcons[CursorIcon::Count];
    u32         m_switchToFullScreenQuerryFrame;

};

} // namespace ITF

#endif // ITF_WINDOWS

#endif // ITF_SYSTEMADAPTER_WIN_H_
