#include "precompiled_SystemAdapter_WII.h"

#ifndef _ITF_SYSTEMADAPTER_WII_H_
#include "adapters/SystemAdapter_WII/SystemAdapter_WII.h"
#endif //_ITF_SYSTEMADAPTER_WII_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_WII_DRAWPRIM_H_
#include "adapters/GFXAdapter_WII/GFXAdapter_WII.h"
#endif //_ITF_WII_DRAWPRIM_H_

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

#ifndef _ITF_CONFIG_H_
#include "core/Config.h"
#endif //_ITF_CONFIG_H_

#ifndef _ITF_INPUTADAPTER_WII_H_
#include "adapters/InputAdapter_WII/InputAdapter_WII.h"
#endif //_ITF_INPUTADAPTER_WII_H_

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_ADAPTER_SAVEGAME_WII_H__
#include "adapters/Adapter_Savegame_WII/Adapter_Savegame_WII.h"
#endif

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_

#ifndef _ITF_TRCManagerAdapter_H_
#include "engine/AdaptersInterfaces/TRCManager_Adapter.h"
#endif //_ITF_TRCManagerAdapter_H_

#ifndef _ITF_TRCADAPTER_WII_H_
#include "adapters/TRCAdapter_WII/TRCAdapter_WII.h"
#endif //_ITF_TRCADAPTER_WII_H_

#ifndef _ITF_INPUTADAPTER_TPL_WII_H_
#include "adapters/SystemAdapter_WII/SystemAdapter_TPL_WII.h"
#endif //_ITF_INPUTADAPTER_TPL_WII_H_

namespace ITF
{
    /// constructor.
    SystemAdapter_WII::SystemAdapter_WII()
        :   m_stopTime(0),
            m_lastElapsedTime(0),
            m_baseTime(0),
            m_LastFPSTime(0.0),
            m_timerStopped(btrue),
            m_NumFrames(0),
            m_fPs(0.0),
#ifndef ITF_FINAL
            m_VBLSyncTime(0.0),
            m_fPs_NoVsync(0.0),
#endif // ITF_FINAL
            m_isFullTitle(bfalse)
    {
        checkLicense();

        timerStart();
    }
    //
    bbool SystemAdapter_WII::isTrialMode()
    {
        checkLicense();
        return !m_isFullTitle;
    }
    //
    void SystemAdapter_WII::checkLicense()
    {
        m_isFullTitle = btrue;
    }

    /// destructor
    SystemAdapter_WII::~SystemAdapter_WII()
    {
    }

    bbool SystemAdapter_WII::initialize(   )
    {    	
        return btrue;
    }

    bbool SystemAdapter_WII::openGraphics (int _width, int _height, int _alphaBits, int _depthBits, int _stencilBits, bbool _fullscreen, bbool _mouseCursor, const String& _name, bbool /*_waitVBL*/ )
    {
		MEM_M_PushExt(MEM_C_MEM1_ALLOC);

        const u32 width   = 812;
        const u32 height  = 456;
        GFX_ADAPTER->setResolution(width,height);
        
        // Initialize Graphic Renderer.
        GFX_ADAPTER->init();

        Singletons::get().setInputAdapter(newAlloc(mId_System,ITF::InputAdapter_WII()));
		MEM_M_PopExt();
        return btrue;
    }

    void SystemAdapter_WII::terminate(   )
    {               
        
    }

    void  SystemAdapter_WII::present()
    {
        // Be sure icons will be render in last position
        if(!TRC_ADAPTER->isAFatalErrorRunning())
        {
            renderSplashScreen();
            renderHomeButtonIcons();
        }

        // Present frame buffer.
        GFX_ADAPTER->endRendering();
        GFX_ADAPTER->present();

        /// inform adapter.
        GFX_ADAPTER->setfPs(getfPs());
        #ifndef ITF_FINAL
        GFX_ADAPTER_WII->setfPsNoVsync(getfPsNoVsync());
        #endif // ITF_FINAL

        m_NumFrames++;
    }

    void SystemAdapter_WII::update(bbool _minimal)
    {      
        handleEpilepsyScreen();

        if(!_minimal)
        {
            /// Update All controllers states.
            INPUT_ADAPTER_WII->updateAllInputState();

            // INIT Both first
            initHomeButton();
            initSplashScreen();

            // THEN Update Both
            updateSplashScreen();
            updateHomeButton();
        }

		//SAVEGAME_ADAPTER_WII->update();
    }
    
    //--------------------------------------------------------
    // SplashScreen management 
    void SystemAdapter_WII::initSplashScreen()
    {
        if(!m_splashScreen.isInitialized())
        {
            m_splashScreen.InitSplashScreen();
            m_splashScreen.BeginDisplaying();
        }
    }

    void SystemAdapter_WII::updateSplashScreen()
    {
        ITF_ASSERT(m_splashScreen.isInitialized());
        m_splashScreen.update();
    }

    void SystemAdapter_WII::renderSplashScreen()
    {
        m_splashScreen.render();
    }

    void SystemAdapter_WII::pauseSplashScreen()
    {
        m_splashScreen.pause();
    }

    void SystemAdapter_WII::unpauseSplashScreen()
    {
        m_splashScreen.unpause();
    }

    //--------------------------------------------------------
    void SystemAdapter_WII::updateHomeButton()
    {
        ITF_ASSERT(_HomeButton.bIsInitialized());

    	if(_HomeButton.bIsAnyHomeJustPressed())
		{
			if(_HomeButton.bIsProhibited())
			{
                if(!m_splashScreen.isDisplayingSplashScreen())
    				_HomeButton.ActiveProhibitedIcon();
			} 
			else
			{
				//ProcessHomeMenu is a loop (main update will not be called during this time)
				_HomeButton.SetLeaveMode(SystemAdapter_HomeButton::LeaveWithSaveWarning);
                TRC_ADAPTER->pauseGame();
				_HomeButton.ProcessHomeMenu();
                TRC_ADAPTER->resumeGame();
			}
		}
        else
        {
            _HomeButton.HandleProhibited();
        }
    }

    void SystemAdapter_WII::renderHomeButtonIcons()
    {
        _HomeButton.ShowProhibited();
    }
    
    bbool SystemAdapter_WII::isInHomeMenu() const    
    {
    	return _HomeButton.bIsInHome();
    }
    
    void SystemAdapter_WII::initHomeButton()
    {
        if(!_HomeButton.bIsInitialized())
        {
    	    _HomeButton.InitHomeMenu();
        }
    }

    bbool SystemAdapter_WII::disableHomeButton()
    {
        if(_HomeButton.bIsInitialized())
        {
        	_HomeButton.SetProhibited();
            return btrue;
        }
        else
            return bfalse;
    }

    bbool SystemAdapter_WII::enableHomeButton()
    {
        if(_HomeButton.bIsInitialized())
        {
        	_HomeButton.SetAllowed();
            return btrue;
        }
        else
            return bfalse;
    }

    bbool SystemAdapter_WII::gameUpdateAllowed()
    {
        return (!TRC_ADAPTER->isAFatalErrorRunning());
    }

    bbool SystemAdapter_WII::getWindowSize(int& _width, int& _height)
    {
		_width  = GFX_ADAPTER->getScreenWidth();
		_height = GFX_ADAPTER->getScreenHeight();
		return btrue;
    }

    void QueryPerformanceCounter(LARGE_INTEGER *counter)
    {
        counter->QuadPart = OSGetTime();
    }

    void SystemAdapter_WII::queryPerformanceFrequency(LARGE_INTEGER *TicksPerSecond)
    {
        TicksPerSecond->HighPart = 0;
        TicksPerSecond->LowPart  = OS_TIMER_CLOCK;
    }

    void SystemAdapter_WII::queryPerformanceCounter(LARGE_INTEGER *counter)
    {
        QueryPerformanceCounter( counter );
    }

    void SystemAdapter_WII::timerStart()
    {
        // Get the current time
        LARGE_INTEGER time = { 0 };
        QueryPerformanceCounter(&time);

        if(m_timerStopped)
            m_baseTime += time.QuadPart - m_stopTime;
        m_stopTime = 0;
        m_lastElapsedTime = time.QuadPart;
        m_timerStopped = FALSE;
    }

    LARGE_INTEGER SystemAdapter_WII::GetAdjustedCurrentTime()
    {
        LARGE_INTEGER time;
        if(m_stopTime != 0)
            time.QuadPart = m_stopTime;
        else
            QueryPerformanceCounter(&time);
        return time;
    }

    f64 SystemAdapter_WII::getTime()
    {
        LARGE_INTEGER time = GetAdjustedCurrentTime();
        return (f64)(time.QuadPart - m_baseTime) / (f64)OS_TIMER_CLOCK;
    }

    f64 SystemAdapter_WII::getEllaspedFromStart()
    {
        LARGE_INTEGER time = GetAdjustedCurrentTime();
        return (f64)time.QuadPart / (f64)OS_TIMER_CLOCK;
    }

    void SystemAdapter_WII::timerStop()
    {
        if(!m_timerStopped)
        {
            LARGE_INTEGER time = { 0 };
            QueryPerformanceCounter(&time);
            m_stopTime = time.QuadPart;
            m_lastElapsedTime = time.QuadPart;
            m_timerStopped = btrue;
        }
    }

    f64 SystemAdapter_WII::getElapsedTime()
    {
        LARGE_INTEGER time = GetAdjustedCurrentTime();

        f64 elapsedTime = (f64)(time.QuadPart - m_lastElapsedTime) / (f64)OS_TIMER_CLOCK;
        m_lastElapsedTime = time.QuadPart;

        if(elapsedTime < 0.0)
            elapsedTime = 0.0;

        return elapsedTime;
    }

    f32 SystemAdapter_WII::getfPs()
    {
#ifndef ITF_FINAL
        m_VBLSyncTime += GFX_ADAPTER_WII->getVBLSyncTime();
        GFX_ADAPTER_WII->setVBLSyncTime(0.0);
#endif // ITF_FINAL

        f64 time = getTime();
	    if((time - m_LastFPSTime) > 0.5)
	    {
		    m_fPs = (f64)m_NumFrames / (time - m_LastFPSTime);

#ifndef ITF_FINAL
		    m_fPs_NoVsync = (f64)m_NumFrames / (time - m_LastFPSTime - m_VBLSyncTime);
		    m_VBLSyncTime = 0.0;
#endif // ITF_FINAL

		    m_LastFPSTime = time;
		    m_NumFrames = 0L;
	    }

        return (f32)m_fPs;
    }

    String  SystemAdapter_WII::dateTime() const
    {        
        return "";
    }

    void SystemAdapter_WII::getTime(Time & _time) const
    {
		OSCalendarTime td;
		OSTicksToCalendarTime(OSGetTime(), &td);

        _time.m_year = td.year;
        _time.m_month = 1 + td.mon;
        _time.m_day = td.mday;
        _time.m_hour = td.hour;
        _time.m_minute = td.min;
        _time.m_second = td.sec;
    }

	i32 SystemAdapter_WII::messageBox(const String& _title, const String& _msg, u32 _type, ITF_MESSAGEBOX_RESULT _silentResult)
    {
#ifndef ITF_FINAL
    	static bbool sb_DisplayAssertionInProgress = bfalse;
    	
	    if(TRCManagerAdapter_WII::IsShutdownRequested() 
            || sb_DisplayAssertionInProgress)
	    {
	        return ITF_IDNO;
	    }
	    
		i32 result = ITF_IDNO;

        // Don't let other thread render until the message is skip.
	    GFX_ADAPTER_WII->LockRendering();

		if(SYSTEM_ADAPTER->IsSilent())
		{
			if(_type & ITF_MSG_OK)
				result = ITF_IDYES;
			else
				result = _silentResult;
			goto endassert;                
		}

	    sb_DisplayAssertionInProgress = btrue;

		InputAdapter::PressStatus	buttons[JOY_MAX_BUT];
		
		while(1)
		{
			GFX_ADAPTER_WII->DisplayMessageBox(_title, _msg, _type);

			update(bfalse);
			INPUT_ADAPTER->getGamePadButtons(InputAdapter::EnvironmentAll, 0, buttons, JOY_MAX_BUT);
			if(buttons[m_joyButton_1] == InputAdapter::JustPressed
				|| buttons[m_joyButton_A] == InputAdapter::JustPressed)	// 1 or A
			{
				result = ITF_IDYES;
				goto endassert;                
			}
			if(buttons[m_joyButton_2] == InputAdapter::JustPressed
				|| buttons[m_joyButton_Start] == InputAdapter::JustPressed)	// 2 or +
			{
				result = ITF_IDNO;
				goto endassert;                
			}
			if(buttons[m_joyButton_Minus] == InputAdapter::JustPressed
				|| buttons[m_joyButton_B] == InputAdapter::JustPressed) // - or B
			{
				result = ITF_IDCANCEL;
				goto endassert;                
			}
			
			sleep(10);
		}

endassert:
	    sb_DisplayAssertionInProgress = bfalse;

	    GFX_ADAPTER_WII->UnlockRendering();

        // Reset Projection
        CAMERA->apply();

        return result;
#else // ITF_FINAL
        return ITF_IDCANCEL;
#endif // ITF_FINAL
    }

    void *SystemAdapter_WII::allocatePhysicalMemory(u32 /*_size*/, u32 /*_alignment*/)
    {
        ITF_ASSERT_MSG(bfalse, "allocatePhysicalMemory : not implemented");
        return NULL;
    }

    void SystemAdapter_WII::freePhysicalMemory(void * /*_pointer*/)
    {
    }

    String SystemAdapter_WII::userName()const
    {
        return "user";
    }

    ITF_LANGUAGE SystemAdapter_WII::getSystemLanguage(const ITF_LANGUAGE *_supportedLanguages, u32 _supportedCount, ITF_LANGUAGE _defaultLanguage) const
    {
        ITF_LANGUAGE language ;
        
	    switch (SCGetLanguage())
	    {	    
	    case SC_LANG_ENGLISH: 	language = ITF_LANGUAGE_ENGLISH; break;
	    case SC_LANG_GERMAN: 	language = ITF_LANGUAGE_GERMAN; break;
	    case SC_LANG_FRENCH: 	language = ITF_LANGUAGE_FRENCH; break;
	    case SC_LANG_SPANISH:	language = ITF_LANGUAGE_SPANISH; break;
	    case SC_LANG_ITALIAN:	language = ITF_LANGUAGE_ITALIAN; break;
	    case SC_LANG_DUTCH:		language = ITF_LANGUAGE_DUTCH; break;
	    case SC_LANG_KOREAN:	language = ITF_LANGUAGE_KOREAN; break;
	    case SC_LANG_SIMP_CHINESE:	language = ITF_LANGUAGE_SIMPLIFIEDCHINESE; break;
	    case SC_LANG_TRAD_CHINESE:	language = ITF_LANGUAGE_TRADITIONALCHINESE; break;
	    case SC_LANG_JAPANESE: 	language = ITF_LANGUAGE_JAPANESE; break;
	    default:
	    	language = ITF_LANGUAGE_ENGLISH; 
	    }	    

        return filterSupportedLanguages(language, _supportedLanguages, _supportedCount, _defaultLanguage);
    }

    SystemAdapter_WII::SoundMode SystemAdapter_WII::getSoundModeFromWiiSettings() const
    {
        static u8 soundMode = SCGetSoundMode();

        if(soundMode == SC_SOUND_MODE_MONO)
            return SoundMode_Mono;
        else if(soundMode == SC_SOUND_MODE_STEREO)
            return SoundMode_Stereo;
        else if(soundMode == SC_SOUND_MODE_SURROUND)
            return SoundMode_Surround;

        return SoundMode_Unknown;
    }

    void SystemAdapter_WII::handleEpilepsyScreen()
    {
        if(!CONFIG->m_enableEpilepsyScreen)
            return; 

        TPLPalettePtr _pal; 
        const String EpilepsyPath ("epilepsy.tpl");
        if (hwFile::exists(EpilepsyPath) 
            && SystemAdapter_TPL::readAndAlloc(&_pal, EpilepsyPath))
        {
            SystemAdapter_TPL::unpack(_pal);

            const Vec2d size ( GFX_ADAPTER_WII->getRealScreenWidth(), GFX_ADAPTER_WII->getRealScreenHeight() );
            const Vec2d pos (0.0f, 0.0f);

            TRC_ADAPTER->disableMessagesListener();
            do 
            {                
                GFX_ADAPTER->startRendering();
                SystemAdapter_TPL::draw(_pal, pos, size);
                INPUT_ADAPTER_WII->updateAllInputState();                
                present();
                GFX_ADAPTER->cleanBufferEndOfFrame();
            } while ( !SystemAdapter_SplashScreen::isAnyButtonPressed() );
            TRC_ADAPTER->enableMessagesListener();
        }
        CONFIG->m_enableEpilepsyScreen = bfalse;
    }

    bbool SystemAdapter_WII::isInSplashScreen()
    {
        return m_splashScreen.isInitialized() && m_splashScreen.isDisplayingSplashScreen();
    }



} // namespace ITF



