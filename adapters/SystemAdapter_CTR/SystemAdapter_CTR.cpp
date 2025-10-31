#include "precompiled_SystemAdapter_CTR.h"

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_SYSTEMADAPTER_CTR_H_
#include "adapters/SystemAdapter_CTR/SystemAdapter_CTR.h"
#endif //_ITF_SYSTEMADAPTER_CTR_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

#ifndef _ITF_CONFIG_H_
#include "core/Config.h"
#endif //_ITF_CONFIG_H_

#ifndef _ITF_INPUTADAPTER_3DS_H_
#include "adapters/InputAdapter_3DS/InputAdapter_3DS.h"
#endif //_ITF_INPUTADAPTER_3DS_H_

#include <nn/gx.h>
#include <nn/fnd.h>

namespace ITF
{
    /// constructor.
    SystemAdapter_CTR::SystemAdapter_CTR() : m_isFullTitle(bfalse)
    {
        checkLicense();
    }
    //
    bbool SystemAdapter_CTR::isTrialMode()
    {
        checkLicense();
        return !m_isFullTitle;
    }
    //
    void SystemAdapter_CTR::checkLicense()
    {
        m_isFullTitle = btrue;
    }

    /// destructor
    SystemAdapter_CTR::~SystemAdapter_CTR()
    {
    }

    bbool SystemAdapter_CTR::initialize(   )
    {
        return btrue;
    }

    bbool SystemAdapter_CTR::openGraphics (int _width, int _height, int _alphaBits, int _depthBits, int _stencilBits, bbool _fullscreen, bbool _mouseCursor, const String& _name, bbool /*_waitVBL*/ )
    {
        u32 width   = nn::gx::CTR::DISPLAY0_HEIGHT;
        u32 height  = nn::gx::CTR::DISPLAY0_WIDTH;
        GFX_ADAPTER->setResolution(width,height);
        
        GFX_ADAPTER->createDebugFonts(width, height);

        // Initialize Graphic Renderer.
        ITF::GFX_ADAPTER->init();


        Singletons::get().setInputAdapter(newAlloc(mId_System,ITF::InputAdapter_3DS()));

        return btrue;
    }

    void SystemAdapter_CTR::terminate(   )
    {               
        
    }

    void  SystemAdapter_CTR::present()
    {
        /// inform adapter.
        ITF::GFX_ADAPTER->setfPs(getfPs());

        // Present frame buffer.
        ITF::GFX_ADAPTER->endRendering();
        ITF::GFX_ADAPTER->present();

        m_NumFrames++;
    }

    void SystemAdapter_CTR::update(bbool _minimal)
    {        
        if(!_minimal)
        {
            /// 
            /// Update All controllers states.
            InputAdapter_3DS *inputAdapter;
            inputAdapter = (InputAdapter_3DS*)INPUT_ADAPTER;
            if (inputAdapter)
                inputAdapter->updateAllInputState();
        }
    }

    bbool SystemAdapter_CTR::getWindowSize(int& _width, int& _height)
    {
        //[Todo] change to real values
           // Need Resize ?.
       _width = ITF::GFX_ADAPTER->getScreenWidth();
       _height = ITF::GFX_ADAPTER->getScreenHeight();
        return btrue;

    }

    f64 SystemAdapter_CTR::getTime()
    {
        return (s64)nn::os::Tick::GetSystemCurrent()/(f64)NN_HW_TICKS_PER_SECOND;

    }

    LARGE_INTEGER SystemAdapter_CTR::GetAdjustedCurrentTime()
    {        
        LARGE_INTEGER t;
        t.QuadPart = (s64)nn::os::Tick::GetSystemCurrent();
        return t;
   }

    f64 SystemAdapter_CTR::getEllaspedFromStart()
    {
        LARGE_INTEGER time = GetAdjustedCurrentTime();
        f64 elapsedTime = ( ( f64 )( time.QuadPart - m_lastElapsedTime ) / ( f64 )
            NN_HW_TICKS_PER_SECOND );

        return elapsedTime;     
    }


    f64 SystemAdapter_CTR::getElapsedTime()
    {
        LARGE_INTEGER time = GetAdjustedCurrentTime();

        f64 elapsedTime = ( ( f64 )( time.QuadPart - m_lastElapsedTime ) / ( f64 )
                                         NN_HW_TICKS_PER_SECOND );
        m_lastElapsedTime = time.QuadPart;
        ITF_ASSERT(elapsedTime>=0);
        return elapsedTime;
    }

    f32 SystemAdapter_CTR::getfPs()
    {
        double time = getTime();

	    if( time - m_LastFPSTime > 1.0f )
	    {
		    m_fPs = m_NumFrames / ( time - m_LastFPSTime );
		    m_LastFPSTime = time;
		    m_NumFrames  = 0L;
	    }

        return (f32)m_fPs;
    }

    String  SystemAdapter_CTR::dateTime() const
    {        
        return "";
    }

    void SystemAdapter_CTR::getTime(Time & _time) const
    {
        nn::fnd::DateTime now=nn::fnd::DateTime::GetNow();

        _time.m_year = now.GetYear();
        _time.m_month = now.GetMonth();
        _time.m_day = now.GetDay();
        _time.m_hour = now.GetHour();
        _time.m_minute = now.GetMinute();
        _time.m_second = now.GetSecond();        
    }


    i32 SystemAdapter_CTR::messageBox( const String& _title, const String& _msg, u32 _type, ITF_MESSAGEBOX_RESULT /*silentResult*/ )
    {
        NN_LOG("Message box : %ls\n%ls\n", (wchar_t*)_title.cStr(), (wchar_t*)_msg.cStr());

        return 0;
    }

    void *SystemAdapter_CTR::allocatePhysicalMemory(u32 /*_size*/, u32 /*_alignment*/)
    {
        ITF_ASSERT_MSG(bfalse, "allocatePhysicalMemory : not implemented");
        return NULL;
    }

    void SystemAdapter_CTR::freePhysicalMemory(void * /*_pointer*/)
    {
        ITF_ASSERT_MSG(bfalse, "freePhysicalMemory : not implemented");
    }


    String SystemAdapter_CTR::userName()const
    {
        return "user";
    }

    ITF_LANGUAGE SystemAdapter_CTR::getSystemLanguage(const ITF_LANGUAGE *_supportedLanguages, u32 _supportedCount, ITF_LANGUAGE _defaultLanguage) const
    {
        NN_LOG("No language handling yet\n");
        ITF_LANGUAGE language = ITF_LANGUAGE_ENGLISH;
        return filterSupportedLanguages(language, _supportedLanguages, _supportedCount, _defaultLanguage);
    }

} // namespace ITF



