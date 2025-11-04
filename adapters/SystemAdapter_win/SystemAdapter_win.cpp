#include "precompiled_SystemAdapter_win.h"

#include <shellapi.h>

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#include "adapters/SystemAdapter_win/SystemAdapter_win.h"

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_DIRECTX9_DRAWPRIM_H_
#include "adapters/GFXAdapter_Directx9/GFXAdapter_Directx9.h"
#endif //_ITF_DIRECTX9_DRAWPRIM_H_

#ifndef __glfw_h_
    #include "glfw/include/gl/glfw.h"
#endif //__glfw_h_

#ifndef _ITF_INPUTADAPTER_DINPUT_H_
#include "adapters/InputAdapter_DirectInput/InputAdapter_DINPUT.h"
#endif //_ITF_INPUTADAPTER_DINPUT_H_

#ifndef _ITF_INPUTADAPTER_SDL3_H_
#include "adapters/InputAdapter_SDL3/InputAdapter_SDL3.h"
#endif
#ifndef _ITF_CONFIG_H_
#include "core/Config.h"
#endif //_ITF_CONFIG_H_

#ifndef _ITF_DEBUGINFO_H_
#include "engine/debug/debugInfo.h"
#endif //_ITF_DEBUGINFO_H_

#include <time.h>

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_PLUGINGATEWAY_H_
#include "PluginGateway/PluginGateway.h"
#endif //_ITF_PLUGINGATEWAY_H_

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

#ifndef _ITF_EDITOR_H_
#include "editor/editor.h"
#endif //_ITF_EDITOR_H_

#ifndef _SHLOBJ_H_
#include <shlobj.h>   /* For IShellLink */
#endif //_SHLOBJ_H_

#include <tlhelp32.h>

    /*
     * GetShortcutTarget
     * Retrieves the path that a shortcut file points to.
     *
     * Paramaters:
     *    szShortcutFile    The path to the shortcut file.
     *    szTarget          Pointer to a buffer that will receive the target path.
     *                      The buffer length should be at least MAX_PATH characters.
     *    cchTarget         The size of the szTarget buffer, in characters.
     *
     * Return:
     *    A non-zero value is returned on success. If the function fails zero
     *    will be returned. GetLastError can NOT be called to get an additional error code.
     *
     * Runtime requirements:
     *    CoInitialize or CoInitializeEx must have been called before using this function.
     *
     * Compile requirements:
     *    C++. Include <windows.h>, <objidl.h> & <shlobj.h>. Link "uuid.lib".
     */
    BOOL GetShortcutTarget(LPCTSTR szShortcutFile, LPTSTR szTarget, SIZE_T cchTarget)
    {
        IShellLink*    psl     = NULL;
        IPersistFile*  ppf     = NULL;
        BOOL           bResult = FALSE;

        if (FAILED( CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void **) &psl) ))
            goto cleanup;

        if (FAILED( psl->QueryInterface(IID_IPersistFile, (void **) &ppf) ))
            goto cleanup;

    #if !defined(UNICODE)
            WCHAR wsz[MAX_PATH];
            if (0 == MultiByteToWideChar(CP_ACP, 0, szShortcutFile, -1, wsz, MAX_PATH) )
                goto cleanup;
    #else
            LPCWSTR wsz = szShortcutFile;
    #endif

        if (FAILED( ppf->Load(wsz, STGM_READ) ))
            goto cleanup;

        if (NOERROR != psl->GetPath(szTarget, int(cchTarget), NULL, 0) )
            goto cleanup;

        bResult = TRUE;

    cleanup:
        if (ppf) ppf->Release();
        if (psl) psl->Release();
        if (!bResult && cchTarget != 0) szTarget[0] = TEXT('\0');

        return bResult;
    }

namespace ITF
{

    void WinApp::onFileDrop(const String& _absoluteFile)
    {
        const String _absoluteFileNormalized = FilePath::normalizePath(_absoluteFile);

#ifdef ITF_SUPPORT_EDITOR
        if (PLUGINGATEWAY)
            PLUGINGATEWAY->onFileDrop(_absoluteFileNormalized);

        if (EDITOR)
            EDITOR->onFileDrop(_absoluteFileNormalized);
#endif //ITF_SUPPORT_EDITOR
    }

    LRESULT CALLBACK WinApp::wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
    {
        if (uMsg == WM_CREATE)
            DragAcceptFiles(hWnd, TRUE);

        SystemAdapter_win *adapter;
        adapter = (SystemAdapter_win*)SYSTEM_ADAPTER;

#ifdef ITF_USE_SDL
        InputAdapter_SDL3* input = static_cast<InputAdapter_SDL3*>(INPUT_ADAPTER);
#else
        InputAdapter_DINPUT* input = static_cast<InputAdapter_DINPUT*>(INPUT_ADAPTER);
#endif
        if(!adapter || !input)
            return DefWindowProc(hWnd, uMsg, wParam, lParam);


        bbool forward = btrue;

        //skip the screen saving and monitor power ...
        if (uMsg == WM_SYSCOMMAND)
        {
            if (wParam == SC_SCREENSAVE || wParam == SC_MONITORPOWER)
            {
                return 0;
            }

        }

        // keyboard messages send to the keyboard callback
        if( uMsg == WM_KEYDOWN ||
            uMsg == WM_SYSKEYDOWN ||
            uMsg == WM_KEYUP ||
            uMsg == WM_SYSKEYUP )
        {

            bool bKeyDown = ( uMsg == WM_KEYDOWN || uMsg == WM_SYSKEYDOWN );

            /// Virtual Key.
            bool bLAlt = false;
            bool bRAlt = false;
            bool bLCtrl = false;
            bool bRCtrl = false;
            bool bLShift = false;
            bool bRShift = false;

            const bool bExtendedKey = (lParam & 0x01000000) != 0;

            if (wParam == VK_CONTROL)
            {
                if (bExtendedKey)
                    bRCtrl = true;
                else
                    bLCtrl = true;
            }
            else if (wParam == VK_SHIFT)
            {
                bLShift = (GetAsyncKeyState(VK_LSHIFT) & 0x8000) ? true : false;
                bRShift = (GetAsyncKeyState(VK_RSHIFT) & 0x8000) ? true : false;
            }
            else if (wParam == VK_MENU)
            {
                if (bExtendedKey)
                    bRAlt = true;
                else
                    bLAlt = true;

                forward = bfalse;
            }

            bool* bKeys = adapter->m_Keys;

            bKeys[ ( BYTE )( wParam & 0xFF ) ] = bKeyDown;

            if( adapter->mp_KeyBoardCallback )
                adapter->mp_KeyBoardCallback( ( unsigned int )wParam, bKeyDown, bLAlt, bRAlt, bLCtrl, bRCtrl, bLShift, bRShift, /*GetDXUTState().GetKeyboardFuncUserContext()*/0 );
        }

        if(uMsg == WM_MOUSEWHEEL)
        {
#if 0
            // WM_MOUSEWHEEL passes screen mouse coords
            // so convert them to client coords
            int xPos = ( short )LOWORD( lParam );
            int yPos = ( short )HIWORD( lParam );
            POINT pt;
            pt.x = xPos; pt.y = yPos;
            ScreenToClient( hWnd, &pt );
            xPos = pt.x; yPos = pt.y;
#endif

            const int nMouseWheelDelta = ( short ) HIWORD( wParam );

            if (nMouseWheelDelta > 0)
                adapter->m_mouseWheelPosition++;
            else if (nMouseWheelDelta < 0)
                adapter->m_mouseWheelPosition--;

            if( adapter->mp_MouseWheelCallback )
                adapter->mp_MouseWheelCallback( adapter->m_mouseWheelPosition, 0/*GetDXUTState().GetMouseFuncUserContext()*/ );
         }

        if(/*GetNotifyOnMouseMove() && */ uMsg == WM_MOUSEMOVE)
        {
            if( adapter->mp_MousePosCallback )
            {
                const int xPos = ( short ) LOWORD( lParam );
                const int yPos = ( short ) HIWORD( lParam );

                adapter->mp_MousePosCallback( xPos, yPos, 0/*GetDXUTState().GetMouseFuncUserContext()*/ );

                adapter->updateMousePos(xPos, yPos);
            }
        }


        u32 button = U32_INVALID;
        i32 action = -1;
        switch(uMsg)
        {
        case WM_LBUTTONDOWN:
            button = (u32)ITF::InputAdapter::MB_Left;
            action = ITF::InputAdapter::Pressed;
        break;

        case WM_LBUTTONUP:
            button = (u32)ITF::InputAdapter::MB_Left;
            action = ITF::InputAdapter::Released;
        break;

        case WM_LBUTTONDBLCLK:
        break;

        case WM_RBUTTONDOWN:
            button = (u32)ITF::InputAdapter::MB_Right;
            action = ITF::InputAdapter::Pressed;
        break;

        case WM_RBUTTONUP:
            button = (u32)ITF::InputAdapter::MB_Right;
            action = ITF::InputAdapter::Released;
        break;

        case WM_RBUTTONDBLCLK:
        break;

        case WM_MBUTTONDOWN:
            button = (u32)ITF::InputAdapter::MB_Middle;
            action = ITF::InputAdapter::Pressed;
        break;

        case WM_MBUTTONUP:
            button = (u32)ITF::InputAdapter::MB_Middle;
            action = ITF::InputAdapter::Released;
        break;

        case WM_MBUTTONDBLCLK:
        break;

        case WM_COPYDATA:
        {
            COPYDATASTRUCT* cpd = (COPYDATASTRUCT*)(lParam);

            if (cpd->dwData == WM_DROPFILES)
            {
                String fileDropped_absolute = (u16*)cpd->lpData;
                WinApp::onFileDrop(fileDropped_absolute);
            }
        }
        break;

        case WM_DROPFILES:
        {
            HDROP hDrop = (HDROP)wParam;
            u32 index = 0;
            for(;;index++)
            {
                TCHAR buffer[512];

                if (!DragQueryFile(hDrop, index, (LPWSTR)buffer, 512))
                    break;

                String fileDropped_absolute((u16*)buffer);
                FilePath::normPath(fileDropped_absolute);
                String ext = FilePath::getExtension(fileDropped_absolute);

                if(ext == "lnk")
                {
                    TCHAR szTarget[MAX_PATH];
                    if(GetShortcutTarget((LPCTSTR)fileDropped_absolute.cStr(), szTarget, MAX_PATH))
                        fileDropped_absolute = (u16*)szTarget;
                }
                WinApp::onFileDrop(fileDropped_absolute);
            }
            DragFinish(hDrop);
        }
        break;

        case WM_QUIT:
        case WM_CLOSE:
            adapter->requestExit();
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        case WM_SIZE:
            if( SIZE_MINIMIZED == wParam )
            {
                break;
            }
            else
            {
                i32 width, height;

                ITF_MemoryBarrier();
                if (ITF::SYSTEM_ADAPTER->isAllowResize() && ITF::SYSTEM_ADAPTER->getWindowSize(width, height))
                {
                    if (!width || !height)
                    {
                        break;
                    }

                    ITF::GFX_ADAPTER->setResolution(width, height);
                }
            }
            break;

        case WM_SETFOCUS:
            input->setFocus();

#ifdef ITF_SUPPORT_EDITOR
            if (EDITOR)
                EDITOR->onGainFocus();
#endif //ITF_SUPPORT_EDITOR
            break;

        case WM_KILLFOCUS:
            input->unsetFocus();

#ifdef ITF_SUPPORT_EDITOR
            if (EDITOR)
                EDITOR->onLoseFocus();
            break;
#endif //ITF_SUPPORT_EDITOR

        default:
            if(forward)
                return DefWindowProc(hWnd, uMsg, wParam, lParam);
            else
                return 0;
            break;
        }

        if( adapter->mp_MouseButtonCallback && action != -1)
            adapter->mp_MouseButtonCallback( button, action, 0 );

        return 0;
    }

    HWND WinApp::createWindow(int _width, int _height, bbool _fullscreen, const String& _name, const String& _class)
    {
        WNDCLASS    wndClass;

        wndClass.style          = 0;
        wndClass.lpfnWndProc    = wndProc;
        wndClass.cbClsExtra     = 0;
        wndClass.cbWndExtra     = 0;
        wndClass.hInstance      = GetModuleHandle(0);
        wndClass.hIcon          = LoadIcon(0, IDI_WINLOGO);
        wndClass.hCursor        = LoadCursor(0, IDC_ARROW);
        wndClass.hbrBackground  = static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
        wndClass.lpszMenuName   = 0;

        wndClass.lpszClassName  = _class.isEmpty() ? (const wchar_t *)_name.cStr() : (const wchar_t *)_class.cStr();

        HWND window = 0;

        if (RegisterClass(&wndClass))
        {
            RECT wnd_size = {0,0,_width, _height};
            DWORD style = _fullscreen?WS_POPUP:WS_OVERLAPPEDWINDOW;
            AdjustWindowRect(&wnd_size, WS_OVERLAPPEDWINDOW, FALSE);

            window = CreateWindow(wndClass.lpszClassName, (const wchar_t *)_name.cStr(),
                                  style,
                                  0, 0,
                                  wnd_size.right - wnd_size.left,
                                  wnd_size.bottom - wnd_size.top,
                                  0, 0, GetModuleHandle(0), 0);
        }
        else
        {
            ITF_ERROR("Failed to register window class : %S", _name.cStr());
            return 0;
        }

        ShowWindow(window, SW_NORMAL);
        UpdateWindow(window);

        return window;
    }

    bool WinApp::getEvents(bbool& _idle)
    {
        MSG msg;
        ZeroMemory( &msg, sizeof(msg) );

        if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                return false;
            }

            TranslateMessage(&msg);
            DispatchMessage(&msg);

            _idle = bfalse;
        }
        else
        {
            _idle = btrue;
        }

        return true;
    }

    //--------------------------------------------------------------------------------------

    void CALLBACK SystemAdapter_win::SetCallbackKeyboard( W32SYSTEMCALLBACKKEYBOARD pCallback, void* pUserContext )
    {
        //SetKeyboardFuncUserContext( pUserContext );
        mp_KeyBoardCallback = pCallback;
    }

    void CALLBACK SystemAdapter_win::SetCallbackMousePos( W32SYSTEMCALLBACKMOUSEPOS pCallback, void* pUserContext )
    {
        //SetMouseFuncUserContext( pUserContext );
        mp_MousePosCallback = pCallback;
    }

    void CALLBACK SystemAdapter_win::SetCallbackMouseWheel( W32SYSTEMCALLBACKMOUSEWHEEL pCallback, void* pUserContext )
    {
        //SetMouseFuncUserContext( pUserContext );
        mp_MouseWheelCallback = pCallback;
    }

    void CALLBACK SystemAdapter_win::SetCallbackMouseButton( W32SYSTEMCALLBACKMOUSEBUTTON pCallback, void* pUserContext )
    {
        //SetMouseFuncUserContext( pUserContext );
        mp_MouseButtonCallback = pCallback;
    }

	// bbool SystemAdapter_win::IsMouseCursorEnabled()
	// {
	// 	return m_cursorEnabled;
	// }
	//
	// void SystemAdapter_win::EnableMouseCursor(bbool enabled)
	// {
	// 	m_cursorEnabled = enabled;
	// 	ShowMouseCursor(enabled);
	// }

    class TWInputAdapter_w32DirectX :public  ITF::Interface_InputListener
    {
    public:
        bbool                onMouseButton   (   InputAdapter::MouseButton _but, InputAdapter::PressStatus _status );
        bbool                onMousePos        (   i32 _x, i32 _y );
        bbool                onMouseWheel    (   i32 _wheelValue, i32 _wheelDelta );
        bbool                onKey           (   i32 _key, InputAdapter::PressStatus _status );
    };


    /// constructor.
    SystemAdapter_win::SystemAdapter_win()
    {
        mp_MousePosCallback = 0;
        mp_MouseButtonCallback = 0;
        mp_MouseWheelCallback = 0;
        mp_KeyBoardCallback = 0;

        m_hwnd = 0;
        m_timerStopped = btrue;
        m_QPFTicksPerSec = 0;
        m_stopTime = 0;
        m_lastElapsedTime = 0;
        m_baseTime = 0;
        m_LastFPSTime = 0;
        m_NumFrames = 0;
        m_fPs = 0;
        m_mouseWheelPosition = 0;
        m_windowRectWhenGoingToFakeFullscreen_Valid = bfalse;
        m_bFullScreenMode = bfalse;
        m_allowResize = btrue;
        m_windowScreenState = Windowed;
        m_switchToFullScreenQuerryFrame = 0;
        m_allowUpdate = btrue;

#ifndef ITF_FINAL
        SetProcessDumpOnCrash(true);
#endif //ITF_FINAL

        LARGE_INTEGER ticksPerSec = { 0 };
        QueryPerformanceFrequency( &ticksPerSec );
        m_QPFTicksPerSec = ticksPerSec.QuadPart;

        timerStart();

        mp_winApp = NULL;
        memset(&m_windowRectWhenGoingToFakeFullscreen,0,sizeof(m_windowRectWhenGoingToFakeFullscreen));

        memset(&m_Keys,0,sizeof(m_Keys));
        memset(&m_LastKeys,0,sizeof(m_LastKeys));

        for(u32 i = 0; i < CursorIcon::Count; ++i)
        {
            switch((CursorIcon::Type)i)
            {
            case CursorIcon::Normal:
                m_cursorIcons[i] = LoadCursor(NULL, IDC_ARROW);
                break;

            case CursorIcon::Scale_X:
                m_cursorIcons[i] = LoadCursor(NULL, IDC_SIZEWE);
                break;

            case CursorIcon::Scale_Y:
                m_cursorIcons[i] = LoadCursor(NULL, IDC_SIZENS);
                break;

            case CursorIcon::Scale_XY:
                m_cursorIcons[i] = LoadCursor(NULL, IDC_SIZENESW);
                break;

            case CursorIcon::Move:
                m_cursorIcons[i] = LoadCursor(NULL, IDC_SIZEALL);
                break;

            case CursorIcon::Rotate:
                m_cursorIcons[i] = LoadCursor(NULL, IDC_ARROW);
                break;

            default:
                m_cursorIcons[i] = LoadCursor(NULL, IDC_ARROW);
            }
        }
    }

    /// destructor;
    SystemAdapter_win::~SystemAdapter_win()
    {
        DestroyWindow(m_hwnd);
        SF_DEL(mp_winApp);
    }

    bbool SystemAdapter_win::initialize(   )
    {
        m_szModuleName = recordModuleName();
        return btrue;
    }

    void    SystemAdapter_win::addTextToTitle(const String& _extraInfo)
    {
        String completeTitle;
        completeTitle.setStringFormat("%s %s",m_OriginalCaption.cStr(),_extraInfo.cStr());
        ::SetWindowText(m_hwnd,(LPCWSTR)completeTitle.cStr());
    }

    bbool SystemAdapter_win::openGraphics (int _width, int _height, int _alphaBits, int _depthBits, int _stencilBits, bbool _fullscreen, bbool _mouseCursor, const String& _name, bbool _waitVBL )
    {
        m_bFullScreenMode = _fullscreen;

        m_windowScreenState = _fullscreen ? Fullscreen : Windowed;
        m_hwnd = WinApp::createWindow(_width, _height, _fullscreen, _name/*, "UAF"*/);
        m_OriginalCaption = _name;

        GFX_ADAPTER->setResolution(_width, _height);
        GFX_ADAPTER->setWaitVBL(_waitVBL);

        // Create DirectX Device.
        GFXAdapter_Directx9* adapter = (GFXAdapter_Directx9*)GFX_ADAPTER;
        if (!adapter->createDXDevice(_alphaBits, _depthBits, _stencilBits, _fullscreen, m_hwnd))
        {
            SYSTEM_ADAPTER->messageBox("Failed to create DIRECTX", "May your drivers are outdated", ITF_MSG_ICONERROR | ITF_MSG_OK, ITF_IDYES);

            /*if ( _fullscreen )
            {
                SYSTEM_ADAPTER->messageBox( TEXT( "fullscreen resolution not supported" ),
                            TEXT( "Failed to create DIRECTX" ),
                            MB_ICONERROR | MB_OK );
                EndDialog( m_hwnd, 0 );
                }*/

            return bfalse;
        }

        adapter->createDebugFonts(_width, _height);

        /// clip cursor if fullscreen.
        if(_fullscreen)
        {
            //ShowWindow( m_hwnd, SW_HIDE );
            RECT rcWindow;
            GetWindowRect( m_hwnd, &rcWindow );
            ClipCursor( &rcWindow );
        }

        // Initialize Graphic Renderer.
        ITF::GFX_ADAPTER->init();

        if (_mouseCursor)
        {
            // TODO: glfwEnable(GLFW_MOUSE_CURSOR);
        }
        else
        {
            // TODO: glfwDisable(GLFW_MOUSE_CURSOR);
        }
#ifdef ITF_USE_SDL
        Singletons::get().setInputAdapter(newAlloc(mId_System, ITF::InputAdapter_SDL3()));
#else
        Singletons::get().setInputAdapter(newAlloc(mId_System,ITF::InputAdapter_DINPUT()));
#endif
        return btrue;
    }

    void SystemAdapter_win::terminate(   )
    {
    }

    void  SystemAdapter_win::present()
    {
        /// inform adapter.
        ITF::GFX_ADAPTER->setfPs(getfPs());

        // Present frame buffer.
        ITF::GFX_ADAPTER->endRendering();
        ITF::GFX_ADAPTER->present();

        m_NumFrames++;
    }


    void    SystemAdapter_win::queryPerformanceFrequency(LARGE_INTEGER *TicksPerSecond) const
    {
        QueryPerformanceFrequency( TicksPerSecond );
    }

    void    SystemAdapter_win::queryPerformanceCounter(LARGE_INTEGER *counter) const
    {
        QueryPerformanceCounter( counter );
    }

    void SystemAdapter_win::update(bbool _minimal)
    {
        WinApp::getEvents(m_allowUpdate);

        if(!_minimal && m_allowUpdate)
        {
            HWND hActiveWindow  = GetActiveWindow();
            const bbool hasFocus = (hActiveWindow == m_hwnd);

            /// Update All controllers states.
#ifdef ITF_USE_SDL
            InputAdapter_SDL3* inputAdapter = static_cast<InputAdapter_SDL3*>(INPUT_ADAPTER);
#else
            InputAdapter_DINPUT* inputAdapter = static_cast<InputAdapter_DINPUT*>(INPUT_ADAPTER);
#endif
            if (inputAdapter && (hasFocus || m_bAutoFocus))
                inputAdapter->updateAllInputState();
        }

        if (m_switchToFullScreenQuerryFrame)
        {
            toggleWindowMode(btrue);
        }
    }

    void * SystemAdapter_win::allocatePhysicalMemory(u32 _size, u32 _alignment)
    {
        return _aligned_malloc(_size, _alignment);
    }
    void SystemAdapter_win::freePhysicalMemory(void *_pointer)
    {
        _aligned_free(_pointer);
    }


    bbool SystemAdapter_win::gameUpdateAllowed()
    {
        if(m_allowUpdate)
        {
            HWND hActiveWindow  = GetActiveWindow();
            const bbool hasFocus = (hActiveWindow == m_hwnd);
            if (hasFocus)// && (mt_PresentParams.Windowed == FALSE) )
                if (GFX_ADAPTER->isDeviceLost())
                    m_allowUpdate = bfalse;
        }

        return m_allowUpdate;
    }

#define     GetWindowStyle(hwnd)    ((DWORD)GetWindowLong(hwnd, GWL_STYLE))
#define     GetWindowExStyle(hwnd)  ((DWORD)GetWindowLong(hwnd, GWL_EXSTYLE))


    void  SystemAdapter_win::getWindowAdjusted(const RECT& source,int& Width,int& height)
    {
        RECT rcWindow= source;
        AdjustWindowRectEx(&rcWindow,     // pointer to the RECT structure to use
            GetWindowStyle(m_hwnd),     // window styles
            GetMenu(m_hwnd) != NULL,    // TRUE if the window has a menu, FALSE if not
            GetWindowExStyle(m_hwnd));  // extended window styles

        Width   = rcWindow.right-rcWindow.left;
        height  = rcWindow.bottom-rcWindow.top;
    }


    void  SystemAdapter_win::setPositionAndSize(int _x, int _y,int _w,int _h)
    {
        RECT rcWindow = {_x, _y, _x + _w, _y + _h};
        int W,H;
        getWindowAdjusted(rcWindow,W,H);

        SetWindowPos((HWND)m_hwnd,HWND_TOP,_x,_y,W,H,SWP_FRAMECHANGED);
    }

    void  SystemAdapter_win::setSize(int width, int height )
    {
        RECT rcWindow = {0, 0, width, height};
        int W,H;
        getWindowAdjusted(rcWindow,W,H);


        SetWindowPos((HWND)m_hwnd, HWND_TOP, 0, 0, W, H, SWP_NOMOVE | SWP_NOZORDER);
    }

    bbool SystemAdapter_win::getWindowSize(int& _width, int& _height)
    {
        if (m_hwnd)
        {
            RECT rect;
            GetClientRect(m_hwnd,&rect);
            _width = rect.right - rect.left;
            _height = rect.bottom - rect.top;

            // Need Resize ?.
            if (u32(_width) != ITF::GFX_ADAPTER->getScreenWidth() || u32(_height) != ITF::GFX_ADAPTER->getScreenHeight())
                return btrue;
         }
         return bfalse;
    }

    bbool SystemAdapter_win::getWindowPos(int& _x, int& _y)
    {
        if (m_hwnd)
        {
            POINT pt;
            pt.x = 0;
            pt.y = 0;
            ClientToScreen(m_hwnd, &pt);
            _x = pt.x;
            _y = pt.y;
            return btrue;
        }
        return bfalse;
    }

    void SystemAdapter_win::setWindowCloseCb(void* _func)
    {
         //TODO: glfwSetWindowCloseCallback((GLFWwindowclosefun) _func);
    }

    void SystemAdapter_win::timerStart()
    {
        // Get the current time
        LARGE_INTEGER time = { 0 };
        QueryPerformanceCounter( &time );

        if( m_timerStopped )
            m_baseTime += time.QuadPart - m_stopTime;
        m_stopTime = 0;
        m_lastElapsedTime = time.QuadPart;
        m_timerStopped = FALSE;
    }

    f64 SystemAdapter_win::getTime() const
    {
        LARGE_INTEGER curtime = GetAdjustedCurrentTime();
        f64 time = ( f64 )( curtime.QuadPart - m_baseTime ) / ( f64 )m_QPFTicksPerSec;
        return time;
    }

    void SystemAdapter_win::getTime(Time & _time) const
    {
        time_t currentTime;
        ::time(&currentTime);
        ::tm* t = ::localtime(&currentTime);
        _time.m_year = 1900 + t->tm_year;       // tm years are since 1900
        _time.m_month = 1 + t->tm_mon;          // tm month: 0-11 we needs 1-12
        _time.m_day = t->tm_mday;
        _time.m_hour = t->tm_hour;
        _time.m_minute = t->tm_min;
        _time.m_second = t->tm_sec;
    }

    LARGE_INTEGER SystemAdapter_win::GetAdjustedCurrentTime() const
    {
        LARGE_INTEGER time;
        if( m_stopTime != 0 )
            time.QuadPart = m_stopTime;
        else
            QueryPerformanceCounter( &time );
        return time;
    }

    f64 SystemAdapter_win::getEllaspedFromStart()
    {
        LARGE_INTEGER time = GetAdjustedCurrentTime();
        return ( ( f64 ) time.QuadPart  / ( f64 ) m_QPFTicksPerSec );
    }


    void SystemAdapter_win::timerStop()
    {
        if( !m_timerStopped )
        {
            LARGE_INTEGER time = { 0 };
            QueryPerformanceCounter( &time );
            m_stopTime = time.QuadPart;
            m_lastElapsedTime = time.QuadPart;
            m_timerStopped = btrue;
        }
    }

    f64 SystemAdapter_win::getElapsedTime()
    {
        LARGE_INTEGER time = GetAdjustedCurrentTime();

        f64 elapsedTime = ( ( f64 )( time.QuadPart - m_lastElapsedTime ) / ( f64 )
                                         m_QPFTicksPerSec );
        m_lastElapsedTime = time.QuadPart;

        if( elapsedTime < 0.0f )
            elapsedTime = 0.0f;

        return elapsedTime;
    }

    f32 SystemAdapter_win::getfPs()
    {
        double time = getTime();

        if( time - m_LastFPSTime > 1.0 )
        {
            m_fPs = m_NumFrames / ( time - m_LastFPSTime );
            m_LastFPSTime = time;
            m_NumFrames  = 0L;
        }

        return (f32)m_fPs;
    }

    String  SystemAdapter_win::dateTime() const
    {
        time_t rawtime;
        struct tm * timeinfo;
        char buffer [80];

        ::time ( &rawtime );
        timeinfo = ::localtime ( &rawtime );

        strftime(buffer,80,"%m-%d_%H-%M", timeinfo);
        return buffer;
    }

    ITF_LANGUAGE SystemAdapter_win::getSystemLanguage(const ITF_LANGUAGE *_supportedLanguages, u32 _supportedCount, ITF_LANGUAGE _defaultLanguage) const
    {
        LANGID lang = GetUserDefaultUILanguage();
        u16 primaryLang = PRIMARYLANGID(lang);
        u16 subLang = SUBLANGID(lang);
        ITF_LANGUAGE language = ITF_LANGUAGE_UNKNOWN;

        switch(primaryLang)
        {
        case LANG_ENGLISH: language = ITF_LANGUAGE_ENGLISH; break;
        case LANG_FRENCH: language = ITF_LANGUAGE_FRENCH; break;
        case LANG_JAPANESE: language = ITF_LANGUAGE_JAPANESE; break;
        case LANG_GERMAN : language = ITF_LANGUAGE_GERMAN; break;
        case LANG_SPANISH : language = ITF_LANGUAGE_SPANISH; break;
        case LANG_ITALIAN : language = ITF_LANGUAGE_ITALIAN; break;
        case LANG_KOREAN : language = ITF_LANGUAGE_KOREAN; break;
        case LANG_PORTUGUESE : language = ITF_LANGUAGE_PORTUGUESE; break;
        case LANG_POLISH : language = ITF_LANGUAGE_POLISH; break;
        case LANG_RUSSIAN : language = ITF_LANGUAGE_RUSSIAN; break;
        case LANG_DUTCH : language = ITF_LANGUAGE_DUTCH; break;

        case LANG_CHINESE :
        {
            if (subLang==SUBLANG_CHINESE_TRADITIONAL)
                language = ITF_LANGUAGE_TRADITIONALCHINESE;
            else
                language = ITF_LANGUAGE_SIMPLIFIEDCHINESE;
            break;
        }

        default: language = ITF_LANGUAGE_UNKNOWN; break;
        }
        return filterSupportedLanguages(language, _supportedLanguages, _supportedCount, _defaultLanguage);
    }

    String SystemAdapter_win::userName()const
    {
        wchar_t aUserName[1024];
        DWORD   uSize = sizeof(aUserName);
        if (GetUserName(aUserName, &uSize))
        {
            return String((u16*)aUserName);
        }

        return "unknown user";
    }

    String SystemAdapter_win::recordModuleName()
    {
        wchar_t strExePath [MAX_PATH];
        const DWORD uSize = GetModuleFileName(NULL, strExePath, MAX_PATH);

        if(uSize == 0)
        {
            ITF_ASSERT(0); // Could not retrieve module name
            return "";
        }

        return String((u16*)strExePath);
    }

    void* SystemAdapter_win::findWindowbyName(const String& _szName)
    {
        return 0;
    }

    void* SystemAdapter_win::registerWindow(u32 _screenWidth,u32 _screenHeight,const String& _szName,const String& _szFilename)
    {
        return m_hwnd;
    }

    void SystemAdapter_win::saveWindowPos(const String& _szFilename)
    {
    }

    void SystemAdapter_win::registerClipBoardFormat()
    {
        m_ClipBoardFormat = ::RegisterClipboardFormat(L"ITF_CLIPBOARD");
    }
    //////////////////////////////////////////////////////////////////////////
    BOOL CALLBACK SystemAdapter_win::MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData/*contains pointer to output RECT*/)
    {
        MONITORINFO monitorInfo;
        ITF_MemSet(&monitorInfo, 0, sizeof(monitorInfo));
        monitorInfo.cbSize = sizeof(monitorInfo);
        GetMonitorInfo(hMonitor, &monitorInfo);
        *((RECT*)dwData) = monitorInfo.rcMonitor;
        return FALSE;
    }

    void SystemAdapter_win::toggleWindowMode(bbool _bFullScreen)
    {
        switch (m_windowScreenState)
        {
            case  Windowed:
                if (_bFullScreen)
                {
                    if (!m_switchToFullScreenQuerryFrame)
                    {
                        m_switchToFullScreenQuerryFrame = CURRENTFRAME;   // let some time to recover DX surface
                        return;
                    }
                    if (abs((int)((int)CURRENTFRAME - (int)m_switchToFullScreenQuerryFrame)) < 3)  // abs() in case CURRENTFRAME wraps through 0
                        return;
                    m_switchToFullScreenQuerryFrame = 0;
                    m_windowScreenState = Fullscreen;
                }
                else
                    m_windowScreenState = FakeFullscreen;
                break;
            case  Fullscreen:
            case  FakeFullscreen:
                    m_windowScreenState = Windowed;
                break;
        }
#ifndef ITF_FINAL
        wchar_t* text = L"";
        switch (m_windowScreenState)
        {
            case  Windowed:
                text = L"Switch to windowed mode";
                break;
            case  Fullscreen:
                text = L"Switch to fullscreen mode";
                break;
            case  FakeFullscreen:
                text = L"Switch to fake fullscreen mode";
                break;
        }

        DEBUGINFO->addDebugInfoDisplay((u16*)text,3.0f);
#endif //!ITF_FINAL

        //flush the keyboard
        INPUT_ADAPTER->flushKeys();

        GFXAdapter_Directx9* adapter = (GFXAdapter_Directx9*)GFX_ADAPTER;
        m_bFullScreenMode = (m_windowScreenState == Fullscreen);
        adapter->setFullScreenMode(m_bFullScreenMode);

        bbool mustChangeSize = bfalse;
        bbool mustChangeWindowMode = bfalse;
        bbool mustChangePos = bfalse;
        int windowMode = SW_RESTORE;

        DWORD style = ::GetWindowLong(m_hwnd, GWL_STYLE);
        RECT windowRect;
        ::GetWindowRect(m_hwnd, &windowRect);

        if (m_windowScreenState != Windowed)
        {
            style &= ~WS_OVERLAPPEDWINDOW;
            style |= WS_POPUP;
            m_windowRectWhenGoingToFakeFullscreen_Valid = btrue;
            m_windowRectWhenGoingToFakeFullscreen = windowRect;
            windowMode = SW_SHOWNORMAL | SW_MAXIMIZE;
            RECT smallRect = windowRect;
            smallRect.left = (smallRect.left+smallRect.right)/2;
            smallRect.top = (smallRect.top+smallRect.bottom)/2;
            smallRect.right = smallRect.left+1;
            smallRect.bottom = smallRect.top+1;
            GetWindowRect(GetDesktopWindow(), &windowRect);
            EnumDisplayMonitors( NULL, &smallRect, &MonitorEnumProc, (LPARAM)&windowRect);

            mustChangeSize = btrue;
            mustChangePos = btrue;
            mustChangeWindowMode = btrue;

        }
        else
        {
            style &= ~WS_POPUP;
            style |= WS_OVERLAPPEDWINDOW;
            mustChangeWindowMode = btrue;
            windowMode = SW_SHOWNORMAL;
            if (m_windowRectWhenGoingToFakeFullscreen_Valid)
            {
                mustChangeSize = btrue;
                mustChangePos = btrue;
                m_windowRectWhenGoingToFakeFullscreen_Valid = bfalse;
                windowRect = m_windowRectWhenGoingToFakeFullscreen;
            }
        }

        m_allowResize = bfalse;

        ::SetWindowLong(m_hwnd, GWL_STYLE, style);
        UINT flags = SWP_FRAMECHANGED|SWP_NOOWNERZORDER;
        if (!mustChangePos)
            flags |= SWP_NOMOVE;
        if (!mustChangeSize)
            flags |= SWP_NOSIZE;

        m_allowResize = btrue;
        ::SetWindowPos(m_hwnd, (m_windowScreenState == Windowed) ? HWND_NOTOPMOST :0 , windowRect.left,windowRect.top,windowRect.right-windowRect.left,windowRect.bottom-windowRect.top, flags);
        if (mustChangeWindowMode)
            ShowWindow(m_hwnd, windowMode);

    }

    //////////////////////////////////////////////////////////////////////////
    void SystemAdapter_win::toggleShowTitleBar()
    {
        toggleWindowMode(bfalse);
    }

    void    SystemAdapter_win::toggleFullScreen()
    {
        toggleWindowMode(btrue);
    }

	//  silentResult => give a default result when the tool must not display a messagebox
    i32 SystemAdapter_win::messageBox( const String& _title, const String& _msg, u32 _type, ITF_MESSAGEBOX_RESULT silentResult )
    {
        u32 windowType  = 0;

        if (_type & ITF_MSG_ICONERROR)
            windowType |= MB_ICONERROR;

        if (_type & ITF_MSG_ICONWARNING)
            windowType |= MB_ICONWARNING;

        if (_type & ITF_MSG_OK)
            windowType |= MB_OK;

        if (_type & ITF_MSG_YESNOCANCEL)
            windowType |= MB_YESNOCANCEL;

        if (_type & ITF_MSG_ICONQUESTION)
            windowType |= MB_ICONQUESTION;

        if (_type & ITF_MSG_YESNO)
            windowType |= MB_YESNO;

        wchar_t msgbuffer[1024];
        i32 lenght = std::min(_msg.getLen(), u32(sizeof(msgbuffer)/sizeof(wchar_t)-1));
        wcsncpy_s(msgbuffer,(wchar_t*)_msg.cStr(), lenght);

        FullScreenDialog_ProtectScope fullscreenGuard;
		if(SYSTEM_ADAPTER->IsSilent())
		{
			if(_type & ITF_MSG_OK)
			{
				return ITF_IDYES;
			}
			else
				return silentResult;
		}
        u32 result = ::MessageBox(NULL, (LPCWSTR)msgbuffer, (LPCWSTR)_title.cStr(), windowType | MB_TOPMOST);

        switch (result)
        {
            case IDYES:
                return ITF_IDYES;
            case IDNO:
                return ITF_IDNO;
            case IDCANCEL:
                return ITF_IDCANCEL;
           default:
                return ITF_IDYES;
        }
    }

    void SystemAdapter_win::setCursorIcon( CursorIcon::Type _icon ) const
    {
        SetCursor(m_cursorIcons[(u32)_icon]);
    }

    struct Result
    {
        const char* _moduleName;
        HWND        _hwnd;
        bbool       _found;
    };

    BOOL CALLBACK EWP_FindByModule(HWND hWnd, LPARAM lParam)
    {
        if (!IsWindowVisible(hWnd))
            return TRUE;

        Result *pResult = (Result *)lParam;

        // Get process ID
        DWORD dwProcessID;
        GetWindowThreadProcessId (hWnd, &dwProcessID);

        // Get module name
        HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, dwProcessID);
        PROCESSENTRY32 processInfo;
        processInfo.dwSize=sizeof(PROCESSENTRY32);
        while(Process32Next(hSnapShot, &processInfo) != FALSE)
        {
            if (processInfo.th32ProcessID == dwProcessID)
            {
                StringConverter processName((u16*)processInfo.szExeFile);
                if (!strcmp(processName.getChar(), pResult->_moduleName))
                {
                    pResult->_found = btrue;
                    pResult->_hwnd = hWnd;
                    break;
                }
            }
        }

        return TRUE;
    }

    BOOL AppIsAllreadyRunning(const char* _appName, HWND& _hwnd)
    {
        Result r;
        r._moduleName = _appName;
        r._found = bfalse;

        EnumWindows(EWP_FindByModule, (LPARAM)&r);

        if(r._found)
        {
            _hwnd = r._hwnd;
        }
        return r._found;
    }

    void SystemAdapter_win::launchExe(const char* _exeName, const char* _params, bbool _avoidDoublons /* = bfalse*/, bbool _focus /* = btrue*/)
    {
        HWND mainHWND = 0;
        if (_avoidDoublons && AppIsAllreadyRunning(_exeName, mainHWND))
        {
            if(mainHWND != 0 && _focus)
            {
                SetForegroundWindow(mainHWND);
                ShowWindow(mainHWND, SW_NORMAL);
            }

            return;
        }

        String path(m_szModuleName);
        path.replace('/', '\\');
        i32 index = 0;
        path.strstr('\\', btrue, &index);
        if (index > 0)
            path.truncate(index);
        path += "\\";
        path += _exeName;
        if (_params)
        {
            String params(_params);
            ShellExecute(0, NULL, (LPCWSTR)path.cStr(), (LPCWSTR)params.cStr(), NULL, SW_NORMAL);
        } else
        {
            ShellExecute(0, NULL, (LPCWSTR)path.cStr(), NULL, NULL, SW_NORMAL);
        }
    }

} // namespace ITF



