#include "precompiled_SystemAdapter_x360.h"

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_SYSTEMADAPTER_X360_H_
#include "adapters/SystemAdapter_x360/SystemAdapter_x360.h"
#endif //_ITF_SYSTEMADAPTER_X360_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

#ifndef _ITF_DIRECTX9_DRAWPRIM_H_
#include "adapters/GFXAdapter_Directx9/GFXAdapter_Directx9.h"
#endif //_ITF_DIRECTX9_DRAWPRIM_H_

#ifndef _ITF_INPUTADAPTER_X360_H_
#include "adapters/InputAdapter_x360/InputAdapter_x360.h"
#endif //_ITF_INPUTADAPTER_X360_H_

#include <xonline.h>

#ifndef _ITF_CONFIG_H_
#include "core/Config.h"
#endif //_ITF_CONFIG_H_

#define LICENSE_MASK_FULL       1 //bit mask for full game license 
#define LICENSE_MASK_FALLBACK   0 //Error=>no license

// Usefull for XBLA game, not a FULL TITLE
static bool g_bForceFullTitle = true;

DWORD g_acceptedNotifications[]=
{
    XN_SYS_SIGNINCHANGED,
    XN_LIVE_CONTENT_INSTALLED
};

namespace ITF
{
    /// constructor.
    SystemAdapter_X360::SystemAdapter_X360() :
        m_messageListener(g_acceptedNotifications, ARRAYSIZE(g_acceptedNotifications))            
    {
        m_QPFTicksPerSec = 0;
        m_stopTime = 0;
        m_lastElapsedTime = 0;
        m_baseTime = 0;
        m_LastFPSTime = 0.0;
        m_timerStopped = bfalse;
        m_NumFrames = 0;
        m_fPs = 0.0f;
        m_licenseChecked = bfalse;

        LARGE_INTEGER ticksPerSec = { 0 };
        QueryPerformanceFrequency( &ticksPerSec );
        m_QPFTicksPerSec = ticksPerSec.QuadPart;

        Synchronize::createCriticalSection(&m_csDialog);

        timerStart();

        // Register our notification listener
        m_hNotification = XNotifyCreateListener( XNOTIFY_SYSTEM | XNOTIFY_LIVE );
        if( m_hNotification == NULL || m_hNotification == INVALID_HANDLE_VALUE )
        {
            ITF_FATAL_ERROR("Failed to create state notification listener.\n");
        }

        registerSystemMessageListener(&m_messageListener);
        checkLicense();
    }
    
    bbool SystemAdapter_X360::isTrialMode()
    {
        return !m_isFullTitle;
    }

    void    SystemAdapter_X360::queryPerformanceFrequency(LARGE_INTEGER *TicksPerSecond)
    {
        QueryPerformanceFrequency( TicksPerSecond );
    }

    void    SystemAdapter_X360::queryPerformanceCounter(LARGE_INTEGER *counter)
    {
        QueryPerformanceCounter( counter );
    }

    void SystemAdapter_X360::checkLicense()
    {
        // We can't call "XContentGetLicenseMask" during the game
        if ( m_licenseChecked )
            return ;

        DWORD currentLicense = 0;
        if(g_bForceFullTitle)
        {
            currentLicense = LICENSE_MASK_FULL;
        }
        else
        {
            if (XContentGetLicenseMask(&currentLicense, NULL)!=ERROR_SUCCESS)
                currentLicense = LICENSE_MASK_FALLBACK;   
        }

        if (currentLicense & LICENSE_MASK_FULL)
            m_isFullTitle = btrue;

        m_licenseChecked = btrue;
    }
    /// destructor
    SystemAdapter_X360::~SystemAdapter_X360()
    {
        unregisterSystemMessageListener(&m_messageListener);
        ITF_ASSERT(m_systemMessageListeners.size()==0);
        XCloseHandle(m_hNotification);
        Synchronize::destroyCriticalSection(&m_csDialog);
    }

    bbool SystemAdapter_X360::initialize(   )
    {
        XOnlineStartup(); //SDK : Since all Xbox 360 titles are LIVE-enabled, all titles should call XOnlineStartup before calling any other Xbox functions. 
        initMemory();
        return btrue;
    }

    bbool SystemAdapter_X360::openGraphics (int _width, int _height, int _alphaBits, int _depthBits, int _stencilBits, bbool _fullscreen, bbool _mouseCursor, const String& _name, bbool _waitVBL )
    {
        GFXAdapter_Directx9* adapter = (GFXAdapter_Directx9*)GFX_ADAPTER;
       
        adapter->setWaitVBL(_waitVBL);
        if (!adapter->createDXDevice(_alphaBits, _depthBits, _stencilBits, _fullscreen, m_hwnd))
        {
            return bfalse;
        }

        
        adapter->createDebugFonts(GFX_ADAPTER->getScreenWidth(), adapter->getScreenHeight());

        // Initialize Graphic Renderer.
        GFX_ADAPTER->init();

        Singletons::get().setInputAdapter(newAlloc(mId_System,ITF::InputAdapter_x360()));

        return btrue;
    }

    void SystemAdapter_X360::terminate(   )
    {               
        XOnlineCleanup();
    }

    void  SystemAdapter_X360::present()
    {
        /// inform adapter.
        ITF::GFX_ADAPTER->setfPs(getfPs());

        // Present frame buffer.
        ITF::GFX_ADAPTER->endRendering();
        ITF::GFX_ADAPTER->present();

        m_NumFrames++;
    }

    void SystemAdapter_X360::fetchSystemMessages()
    {
        DWORD dwNotificationID;
        ULONG_PTR ulParam;

        ITF_ASSERT( m_hNotification != NULL );  // ensure Initialize() was called


        //Dispatch messages to each listener stack
        u32 listenerCount=m_systemMessageListeners.size();
        while( XNotifyGetNext( m_hNotification, 0, &dwNotificationID, &ulParam ) )
        {
            for (u32 i=0; i<listenerCount; i++)
                m_systemMessageListeners[i]->pushMessage(dwNotificationID, ulParam);
        }
    }

    void SystemAdapter_X360::update(bbool _minimal)
    {
        if(!_minimal)
        {
            fetchSystemMessages();

            //
            DWORD message;
            ULONG_PTR param;
            //
            while (m_messageListener.popMessage(message, param))
            {
                switch(message)
                {
                case XN_SYS_SIGNINCHANGED:
                case XN_LIVE_CONTENT_INSTALLED:
                    checkLicense();
                    break;
                default:
                    ITF_ASSERT_MSG(bfalse, "Unhandled message");
                    break;
                };
            }

            /// 
            /// Update All controllers states.
            InputAdapter_x360 *inputAdapter;
            inputAdapter = (InputAdapter_x360*)INPUT_ADAPTER;
            if (inputAdapter)
                inputAdapter->updateAllInputState();
        }
    }

    bbool SystemAdapter_X360::getWindowSize(int& _width, int& _height)
    {
        //[Todo] change to real values
           // Need Resize ?.
       _width = ITF::GFX_ADAPTER->getScreenWidth();
       _height = ITF::GFX_ADAPTER->getScreenHeight();
        return btrue;

    }

    void SystemAdapter_X360::timerStart()
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

    f64 SystemAdapter_X360::getTime()
    {
        LARGE_INTEGER curtime = GetAdjustedCurrentTime();
        f64 time = ( f64 )( curtime.QuadPart - m_baseTime ) / ( f64 )m_QPFTicksPerSec;
        return time;

    }

    LARGE_INTEGER SystemAdapter_X360::GetAdjustedCurrentTime()
    {
        
        LARGE_INTEGER time;
        if( m_stopTime != 0 )
            time.QuadPart = m_stopTime;
        else
            QueryPerformanceCounter( &time );
        return time;

    }

    void SystemAdapter_X360::timerStop()
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

    f64 SystemAdapter_X360::getElapsedTime()
    {
        LARGE_INTEGER time = GetAdjustedCurrentTime();

        f64 elapsedTime = ( ( f64 )( time.QuadPart - m_lastElapsedTime ) / ( f64 )
                                         m_QPFTicksPerSec );
        m_lastElapsedTime = time.QuadPart;

        if( elapsedTime < 0.0f )
            elapsedTime = 0.0f;

        return elapsedTime;
    }

    f32 SystemAdapter_X360::getfPs()
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

    String  SystemAdapter_X360::dateTime() const
    {
        SYSTEMTIME LocalSysTime;
        GetLocalTime( &LocalSysTime );

        /*char buf[512];

        strcpy_s(buf, ctime(&LocalSysTime));
        char* scan = buf;
        while(*scan)
        {
            if (' ' == *scan) *scan = '_';
            if (':' == *scan) *scan = '-';
            scan++;
        }*/

        return "fix me";
    }

    void SystemAdapter_X360::getTime(Time & _time) const
    {
        SYSTEMTIME LocalSysTime;
        GetLocalTime( &LocalSysTime );
        _time.m_year = LocalSysTime.wYear;
        _time.m_month = LocalSysTime.wMonth;
        _time.m_day = LocalSysTime.wDay;
        _time.m_hour = LocalSysTime.wHour;
        _time.m_minute = LocalSysTime.wMinute;
        _time.m_second = LocalSysTime.wSecond;
    }


    i32 SystemAdapter_X360::messageBox( const String& _title, const String& _msg, u32 _type)
    {
        csAutoLock cs(m_csDialog);
        u32 dwResult;
        MESSAGEBOX_RESULT messageBoxResult;
        XOVERLAPPED Overlapped; 
        ZeroMemory( &Overlapped, sizeof( XOVERLAPPED ) );

        static LPCWSTR listText[] =   { L"Yes", L"No",L"Cancel" };

        LPCWSTR *pwstrButtons = NULL;
        u32 countButton = 0;

        u32 flags = XMB_NOICON;

        if (_type & ITF_MSG_ICONERROR)
            flags = XMB_ERRORICON;

        if (_type & ITF_MSG_ICONWARNING)
            flags = XMB_WARNINGICON;

        if (_type & ITF_MSG_YESNOCANCEL)
        {
            countButton = 3;
            pwstrButtons = new LPCWSTR[3];
            pwstrButtons[0] = listText[0];
            pwstrButtons[1] = listText[1];
            pwstrButtons[2] = listText[2];
        }
        else
        {
            countButton = 1;
            pwstrButtons = new LPCWSTR[1];
            pwstrButtons[0] = listText[0];
        }
        
        dwResult = XShowMessageBoxUI(XUSER_INDEX_ANY,(LPCWSTR) _title.cStr(),(LPCWSTR) _msg.cStr(), countButton,pwstrButtons,0,flags,&messageBoxResult,&Overlapped);

        if( dwResult != ERROR_IO_PENDING ) 
        {
            //just skipping
            return ITF_IDNO;
        }
        
        u32 buttonResult = 0;

        if (GFX_ADAPTER)
            GFX_ADAPTER->acquireDeviceOwnerShip();
        
        while (!XHasOverlappedIoCompleted( &Overlapped ) )
        {
            Sleep(15);

            if (GFX_ADAPTER)
                GFX_ADAPTER->minimal_present();

            if( XGetOverlappedResult( &Overlapped, NULL, TRUE ) == ERROR_SUCCESS )
            {
                //this pattern must change if you don't respect the button order
                switch( messageBoxResult.dwButtonPressed )
                {
                case 0:
                    buttonResult = ITF_IDYES;
                    break;
                case 1:
                    buttonResult = ITF_IDNO;
                    break;
                case 2:
                    buttonResult = ITF_IDCANCEL;
                    break;
                default :
                    buttonResult = ITF_IDYES;
                    break;
                } 
            }
        }

        if (GFX_ADAPTER)
            GFX_ADAPTER->releaseDeviceOwnerShip();
        
        //let the system freed the message screen thingie...
        Sleep(1000);

        SF_DEL_ARRAY(pwstrButtons);
        return buttonResult;
    }

    void *SystemAdapter_X360::allocatePhysicalMemory(u32 _size, u32 _alignment)
    {
        return XPhysicalAlloc(_size,MAXULONG_PTR,_alignment,PAGE_READWRITE);
    }

    void SystemAdapter_X360::freePhysicalMemory(void *_pointer)
    {
        XPhysicalFree(_pointer);
    }


    String SystemAdapter_X360::userName()const
    {
        return "MY360";
    }

    ITF_LANGUAGE SystemAdapter_X360::getSystemLanguage(const ITF_LANGUAGE *_supportedLanguages, u32 _supportedCount, ITF_LANGUAGE _defaultLanguage) const
    {
        DWORD lang = XGetLanguage();
        ITF_LANGUAGE language = ITF_LANGUAGE_UNKNOWN;

        switch(lang)
        {
        case XC_LANGUAGE_ENGLISH: language = ITF_LANGUAGE_ENGLISH; break;
        case XC_LANGUAGE_JAPANESE : language = ITF_LANGUAGE_JAPANESE; break;
        case XC_LANGUAGE_GERMAN : language = ITF_LANGUAGE_GERMAN; break;
        case XC_LANGUAGE_FRENCH : language = ITF_LANGUAGE_FRENCH; break;
        case XC_LANGUAGE_SPANISH : language = ITF_LANGUAGE_SPANISH; break;
        case XC_LANGUAGE_ITALIAN : language = ITF_LANGUAGE_ITALIAN; break;
        case XC_LANGUAGE_KOREAN : language = ITF_LANGUAGE_KOREAN; break;
        case XC_LANGUAGE_TCHINESE : language = ITF_LANGUAGE_TRADITIONALCHINESE; break;
        case XC_LANGUAGE_PORTUGUESE : language = ITF_LANGUAGE_PORTUGUESE; break;
        case XC_LANGUAGE_SCHINESE  : language = ITF_LANGUAGE_SIMPLIFIEDCHINESE; break;
        case XC_LANGUAGE_POLISH : language = ITF_LANGUAGE_POLISH; break;
        case XC_LANGUAGE_RUSSIAN : language = ITF_LANGUAGE_RUSSIAN; break;
        
        default: language = ITF_LANGUAGE_UNKNOWN; break;
        }

        if(language != ITF_LANGUAGE_UNKNOWN)
        {
            DWORD langLocal = XGetLocale();

            switch(langLocal)
            {
            case XC_LOCALE_NETHERLANDS:
            //case XC_LOCALE_BELGIUM: // this is suggested by ms (!?)
                {
                    const ITF_LANGUAGE* endLang = _supportedLanguages + _supportedCount;
                    const ITF_LANGUAGE* foundDutch = std::find(_supportedLanguages, endLang, ITF_LANGUAGE_DUTCH);
                    if (foundDutch != endLang)
                        language = ITF_LANGUAGE_DUTCH;
                }
                break;

            default: break;
            }
        }

        return filterSupportedLanguages(language, _supportedLanguages, _supportedCount, _defaultLanguage);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void SystemAdapter_X360::registerSystemMessageListener(SystemMessageListener_x360* _listener)
    {
        ITF_ASSERT(m_systemMessageListeners.find(_listener)<0);
        m_systemMessageListeners.push_back(_listener);
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void SystemAdapter_X360::unregisterSystemMessageListener(SystemMessageListener_x360* _listener)
    {
        int position = m_systemMessageListeners.find(_listener);
        ITF_ASSERT(position>=0);
        m_systemMessageListeners.eraseKeepOrder(position);
    }


} // namespace ITF



