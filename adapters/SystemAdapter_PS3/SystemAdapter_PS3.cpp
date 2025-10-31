#include "precompiled_SystemAdapter_PS3.h"

#include <sysutil/sysutil_gamecontent.h>

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#include "core/system/PS3/Purs/Purs_PS3.h"

#ifndef _ITF_SYSTEMADAPTER_PS3_H_
#include "adapters/SystemAdapter_PS3/SystemAdapter_PS3.h"
#endif //_ITF_SYSTEMADAPTER_PS3_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_


#ifndef ITF_GFXADAPTER_GCM_H
#include "adapters/GFXAdapter_GCM/GFXAdapter_GCM.h"
#endif // ITF_GFXADAPTER_GCM_H


#ifndef _ITF_INPUTADAPTER_PS3_H_
#include "adapters/InputAdapter_PS3/InputAdapter_PS3.h"
#endif //_ITF_INPUTADAPTER_PS3_H_

#ifndef _ITF_CONFIG_H_
#include "core/Config.h"
#endif //_ITF_CONFIG_H_

#include "engine/compression/EdgeUncompress_PS3.h"

#include <time.h>
#include <sys/sys_time.h>
#include <sys/time_util.h>
#include <sys/timer.h>
#include <sys/spu_initialize.h>
#include <cell/sysmodule.h>
#include <cell/rtc.h>
#include <cell/spurs/control.h>
#include <spu_printf.h>

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

#ifndef _ITF_MODULEMANAGER_PS3_H_
#include "core/system/PS3/ModuleManager_ps3.h"
#endif //_ITF_MODULEMANAGER_PS3_H_

#include "core/system/PS3/Purs/PursUtil_PS3.h"

namespace ITF
{
    /// constructor.

    SystemAdapter_PS3::SystemAdapter_PS3()
		: m_stopTime(0)
		, m_lastElapsedTime(0)
		, m_baseTime(0)
		, m_LastFPSTime(0.0)
		, m_timerStopped(bfalse)
		, m_NumFrames(0)
		, m_fPs(0.0)
        , m_invertedAssignButtons(bfalse)
        , m_mainSpurs(NULL)
        , m_secondarySpurs(NULL)
        , m_gameTitle(NULL)
        , m_titleID(NULL)
        , m_bootType(CELL_GAME_GAMETYPE_DISC)
        , m_bootAttributes(0)
        , m_launchedFromAppHome(bfalse)
        , m_bootPath(0)
    {
        MODULE_MANAGER->loadModule(CELL_SYSMODULE_RTC);
        LARGE_INTEGER ticksPerSec = { 0 };
        QueryPerformanceFrequency( &ticksPerSec );
        m_QPFTicksPerSec = ticksPerSec.QuadPart;

        m_NetworkServiceProduct = Product_Standard;
        timerStart();
    }

    /// destructor;   
    SystemAdapter_PS3::~SystemAdapter_PS3()
    {
        if(m_titleID)   Memory::free(m_titleID);
        if(m_gameTitle) Memory::free(m_gameTitle);

        delete m_bootPath;

        EdgeUnCompress::Destroy();
        m_mainSpurs->Shutdown();
        m_secondarySpurs->Shutdown(); 
        // effective destruction of spurs instance objects done at atexit
    }

    CellSpurs * SystemAdapter_PS3::getMainSpurs() const
    {
        return m_mainSpurs->GetSpurs();
    }

    // the secondary one only has 1 spu assigned to it.
    // Right now it's used for file decompression (note that decompression tasks take several milliseconds)
    CellSpurs * SystemAdapter_PS3::getSecondarySpurs() const
    {
        return m_secondarySpurs->GetSpurs();
    }

    static const u32 SpursNBSpu = 5;
    
    bbool SystemAdapter_PS3::initialize()
    {
        ITF_VERIFY(CELL_OK == sys_spu_initialize(6, 0)); // The first parameter 6 is always ignored (always 6)

        int buttonAssign = CELL_SYSUTIL_ENTER_BUTTON_ASSIGN_CROSS;
        int ret = cellSysutilGetSystemParamInt(CELL_SYSUTIL_SYSTEMPARAM_ID_ENTER_BUTTON_ASSIGN, &buttonAssign);
        if (ret == CELL_OK)
            m_invertedAssignButtons = (buttonAssign==CELL_SYSUTIL_ENTER_BUTTON_ASSIGN_CIRCLE);

#if !defined(ITF_FINAL)
        spu_printf_initialize(PursUtil::GetCurrentThreadPriority()-1, NULL) ;
#endif

        // Spurs instance put static has it cannot be in virtual memory (and the heap could use virtual memory)
        static Purs::SpursInstance mainSpurs("MainSpurs");
        static Purs::SpursInstance secondarySpurs("SecondarySpurs");
        
        m_mainSpurs = &mainSpurs;
        m_secondarySpurs = &secondarySpurs;

        m_mainSpurs->Init(false, SpursNBSpu); // 5 first spu without context
        m_secondarySpurs->Init(true, 1); // 1 additional spu with context, as it's shared with OS
        // the secondary spurs is currently used for big decompression tasks (>1ms)
        // so beware of other tasks/job latency

        // init Edge ZLib for uncompress
        EdgeUnCompress::Init(m_secondarySpurs->GetSpurs(), 1);

        return btrue;
    }

    bbool SystemAdapter_PS3::openGraphics (int _width, int _height, int _alphaBits, int _depthBits, int _stencilBits, bbool _fullscreen, bbool _mouseCursor, const String& _name, bbool /*_waitVBL*/ )
    {
        GFXAdapter_GCM* adapter = (GFXAdapter_GCM*)GFX_ADAPTER;
        // Initialize Graphic Renderer.
        adapter->init();

        Singletons::get().setInputAdapter(new ITF::InputAdapter_PS3());

        return adapter->isSucessfullyInit();
    }

    void SystemAdapter_PS3::terminate(   )
    {
       
    }

    void  SystemAdapter_PS3::present()
    {
        /// inform adapter.
        
        GFX_ADAPTER->setfPs(getfPs());

        //Present frame buffer.
        GFX_ADAPTER->endRendering();
        GFX_ADAPTER->present();


        m_NumFrames++;
    }

    
    void SystemAdapter_PS3::update(bbool _minimal)
    {
        if(!_minimal)
        {
            InputAdapter_PS3 *inputAdapter;
            inputAdapter = (InputAdapter_PS3*)INPUT_ADAPTER;
            if (inputAdapter)
                inputAdapter->updateAllInputState();
        }
    }

    bbool SystemAdapter_PS3::getWindowSize(int& _width, int& _height)
    {
        //[Todo] change to real values
           // Need Resize ?.
        
       _width = ITF::GFX_ADAPTER->getScreenWidth();
       _height = ITF::GFX_ADAPTER->getScreenHeight();
        return btrue;

    }

    void SystemAdapter_PS3::timerStart()
    {
        // Get the current time
        LARGE_INTEGER time = { 0 };
        QueryPerformanceCounter( &time );

        if( m_timerStopped )
            m_baseTime += time.QuadPart - m_stopTime;
        m_stopTime = 0;
        m_lastElapsedTime = time.QuadPart;
        m_timerStopped = bfalse;
    }



    f64 SystemAdapter_PS3::getTime()
    {
       LARGE_INTEGER curtime = GetAdjustedCurrentTime();
       f64 time = ( f64 )( curtime.QuadPart - m_baseTime ) / ( f64 )m_QPFTicksPerSec;
       return time;


    }

    LARGE_INTEGER SystemAdapter_PS3::GetAdjustedCurrentTime()
    {
        LARGE_INTEGER time;
        if( m_stopTime != 0 )
            time.QuadPart = m_stopTime;
        else
            QueryPerformanceCounter( &time );
        return time;

    }

    void SystemAdapter_PS3::timerStop()
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

    f64 SystemAdapter_PS3::getElapsedTime()
    {

        LARGE_INTEGER time = GetAdjustedCurrentTime();

        f64 elapsedTime = ( ( f64 )( time.QuadPart - m_lastElapsedTime ) / ( f64 )
            m_QPFTicksPerSec );
        m_lastElapsedTime = time.QuadPart;

        if( elapsedTime < 0.0f )
            elapsedTime = 0.0f;

        return elapsedTime;
    }

    f32 SystemAdapter_PS3::getfPs()
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

    String  SystemAdapter_PS3::dateTime() const
    {
        time_t rawtime;
        struct tm * timeinfo;
        char buffer [80];

        ::time(&rawtime);
        timeinfo = ::localtime ( &rawtime );

        strftime(buffer,80,"%m-%d_%H-%M", timeinfo);
        return buffer;
    }

    void SystemAdapter_PS3::getTime(Time & _time) const
    {
        CellRtcDateTime dateTime;

        cellRtcGetCurrentClockLocalTime(&dateTime);
        _time.m_year = cellRtcGetYear(&dateTime);
        _time.m_month = cellRtcGetMonth(&dateTime);
        _time.m_day = cellRtcGetDay(&dateTime);
        _time.m_hour = cellRtcGetHour(&dateTime);
        _time.m_minute = cellRtcGetMinute(&dateTime);
        _time.m_second = cellRtcGetSecond(&dateTime);
    }

    i32 SystemAdapter_PS3::messageBox( const String& _title, const String& _msg, u32 _type, ITF_MESSAGEBOX_RESULT silentResult)
    {
        LOG("Message box : %ls\n%ls", (wchar_t*)_title.cStr(), (wchar_t*)_msg.cStr());

        InputAdapter_PS3 * inputAdapter = (InputAdapter_PS3*)INPUT_ADAPTER;
        #if defined(ROBOT_COMPILATION)
            LOG("-> ROBOT_COMPILATION mode : \"Yes\" selected\n");
            return ITF_IDYES; // say yes to every message box if robot compilation
        #endif
        if (!inputAdapter)
        {
            LOG("-> No input interface found : \"Yes\" selected\n");
            return ITF_IDYES; // say yes to every message box if no input available
        }
        else
        {
            LOG("-> X for \"Yes\", O for \"No\", /\\ for \"Cancel\"\n");
            InputAdapter::PressStatus buttons[JOY_MAX_BUT];
            do 
            {
                inputAdapter->updateAllInputState();
                inputAdapter->getGamePadButtons(InputAdapter::EnvironmentAll, 0, buttons, JOY_MAX_BUT);
                if (buttons[m_joyButton_A]==InputAdapter::JustPressed)
                {
                    return ITF_IDYES;
                }
                if (buttons[m_joyButton_B]==InputAdapter::JustPressed)
                {
                    return ITF_IDNO;
                }
                if (buttons[m_joyButton_Y]==InputAdapter::JustPressed)
                {
                    return ITF_IDCANCEL;
                }
                ITF::sleep(16);
            } while (1);
        }
    }

    String SystemAdapter_PS3::userName()const
    {
        return "PS3";
    }


    void    SystemAdapter_PS3::queryPerformanceFrequency(LARGE_INTEGER *TicksPerSecond)
    {
        QueryPerformanceFrequency( TicksPerSecond );
    }

    void    SystemAdapter_PS3::queryPerformanceCounter(LARGE_INTEGER *count)
    {
        QueryPerformanceCounter( count );
    }

    ITF_LANGUAGE SystemAdapter_PS3::getSystemLanguage( const ITF_LANGUAGE * _supportedLanguages, u32 _supportedCount, ITF_LANGUAGE _defaultLanguage) const
    {
        int lang; 
        ITF_LANGUAGE language;
        if (cellSysutilGetSystemParamInt(CELL_SYSUTIL_SYSTEMPARAM_ID_LANG, &lang) != CELL_OK)
        {
            ITF_ASSERT(0);
            language = ITF_LANGUAGE_UNKNOWN;
        }
        else
        {
            switch(lang)
            {
            // Supported language
            case CELL_SYSUTIL_LANG_JAPANESE:    language = ITF_LANGUAGE_JAPANESE;           break;
            case CELL_SYSUTIL_LANG_ENGLISH:     language = ITF_LANGUAGE_ENGLISH;            break;
            case CELL_SYSUTIL_LANG_FRENCH:      language = ITF_LANGUAGE_FRENCH;             break;
            case CELL_SYSUTIL_LANG_SPANISH:     language = ITF_LANGUAGE_SPANISH;            break;
            case CELL_SYSUTIL_LANG_GERMAN:      language = ITF_LANGUAGE_GERMAN;             break;
            case CELL_SYSUTIL_LANG_ITALIAN:     language = ITF_LANGUAGE_ITALIAN;            break;
            case CELL_SYSUTIL_LANG_PORTUGUESE:  language = ITF_LANGUAGE_PORTUGUESE;         break;
            case CELL_SYSUTIL_LANG_RUSSIAN:     language = ITF_LANGUAGE_RUSSIAN;            break;
            case CELL_SYSUTIL_LANG_KOREAN:      language = ITF_LANGUAGE_KOREAN;             break;
            case CELL_SYSUTIL_LANG_CHINESE_T:   language = ITF_LANGUAGE_TRADITIONALCHINESE; break;
            case CELL_SYSUTIL_LANG_CHINESE_S:   language = ITF_LANGUAGE_SIMPLIFIEDCHINESE;  break;
            case CELL_SYSUTIL_LANG_POLISH:      language = ITF_LANGUAGE_POLISH;             break;
            case CELL_SYSUTIL_LANG_DUTCH:       language = ITF_LANGUAGE_DUTCH;              break;

            // Unsupported language        
            case CELL_SYSUTIL_LANG_SWEDISH: 
            case CELL_SYSUTIL_LANG_FINNISH: 
            case CELL_SYSUTIL_LANG_DANISH: 
            case CELL_SYSUTIL_LANG_NORWEGIAN: 
            default:
                                                language = ITF_LANGUAGE_UNKNOWN;            break;
            }
        }

        return filterSupportedLanguages(language, _supportedLanguages, _supportedCount, _defaultLanguage);
    }

    void SystemAdapter_PS3::setGameTitle( const char* _gameTitle, u32 _gameTitleSize )
    {
        if(!m_gameTitle)
            m_gameTitle = (char*)Memory::mallocCategory(_gameTitleSize ,MemoryId::mId_System);
        ITF_Memcpy(m_gameTitle, _gameTitle, _gameTitleSize);
    }

    void SystemAdapter_PS3::setTitleID( const char* _titleID, u32 _titleIDSize )
    {
        if(!m_titleID)
            m_titleID = (char*)Memory::mallocCategory(_titleIDSize ,MemoryId::mId_System);
        ITF_Memcpy(m_titleID, _titleID, _titleIDSize);

        //associate the network product
        if ( (strcmp(m_titleID,"BLJM60431") == 0)    //Product_Japanese - BR
            || (strcmp(m_titleID,"NPJB00172") == 0) )//Product_Japanese - PSN
        {
            m_NetworkServiceProduct = Product_Japanese;
        }
        else if (strcmp(m_titleID,"BLES01510") == 0) //Product_PolRussian
        {
            m_NetworkServiceProduct = Product_PolishRussian;
        }
        else 
        {
            m_NetworkServiceProduct = Product_Standard;
        }

        LOG("Product:%s networdproduct:%d",m_titleID,m_NetworkServiceProduct);
     }

    void SystemAdapter_PS3::setBootType(u32 _bootType, u32 _bootAttributes, bool _fromAppHome, const char * _bootPath)
    {
        m_bootType = _bootType;
        m_bootAttributes = _bootAttributes;
        m_launchedFromAppHome = _fromAppHome;
        delete m_bootPath;

        m_bootPath = newAlloc(mId_System, char[strlen(_bootPath)+1]);
        strcpy(m_bootPath, _bootPath);
    }


    bbool SystemAdapter_PS3::isHDDMode() const
    {
        return m_bootType == CELL_GAME_GAMETYPE_HDD;
    }

    ITF_TERRITORY SystemAdapter_PS3::getSystemTerritory() const
    {
        if(m_titleID && strlen (m_titleID) > 3)
        {
            if (m_titleID[2] == 'U')
                return ITF_TERRITORY_AMERICA;
            if (m_titleID[2] == 'E')
                return ITF_TERRITORY_EUROPE;
            return ITF_TERRITORY_OTHER;
        }

        return SystemAdapter::getSystemTerritory();
    }
} // namespace ITF



