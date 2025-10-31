#include "precompiled_SystemAdapter_WII.h"

#ifndef _ITF_SYSTEMADAPTER_SPLASHSCREEN_WII_H_
#include "adapters/SystemAdapter_WII/SystemAdapter_Splashscreen_WII.h"
#endif //_ITF_SYSTEMADAPTER_SPLASHSCREEN_WII_H_

#ifndef _ITF_SYSTEMADAPTER_WII_H_
#include "adapters/SystemAdapter_WII/SystemAdapter_WII.h"
#endif //_ITF_SYSTEMADAPTER_WII_H_

#ifndef _ITF_SYSTEMADAPTER_TPL_WII_H_
#include "adapters/SystemAdapter_WII/SystemAdapter_TPL_WII.h"
#endif //_ITF_SYSTEMADAPTER_TPL_WII_H_

#ifndef _ITF_TEXTURE_H_
#include "engine/display/Texture.h"
#endif //_ITF_TEXTURE_H_

#ifndef _ITF_WII_DRAWPRIM_H_
#include "adapters/GFXAdapter_WII/GFXAdapter_WII.h"
#endif //_ITF_WII_DRAWPRIM_H_

#ifndef _ITF_TRCManagerAdapter_H_
#include "engine/AdaptersInterfaces/TRCManager_Adapter.h"
#endif //_ITF_TRCManagerAdapter_H_

#ifndef _ITF_INPUTADAPTER_WII_H_
#include "adapters/InputAdapter_WII/InputAdapter_WII.h"
#endif //_ITF_INPUTADAPTER_WII_H_


namespace ITF
{


void SystemAdapter_SplashScreen::InitSplashScreen()
{	
	ITF_ASSERT(!m_initialized);
	if(m_initialized) return;
	
    // Name of splash texture from dvdroot
    String8 baseNameTPL = "/logos/strap/";	

    // Get territory information
    // TODO: Check if this is the right way to do this
    u8 settingsLanguage = SCGetLanguage();

    //if (VIGetTvFormat()==VI_NTSC)
    //{
    //    
    //    if ((settingsLanguage == SC_LANG_JAPANESE) || 
    //        (settingsLanguage == SC_LANG_SIMP_CHINESE) || 
    //        (settingsLanguage == SC_LANG_TRAD_CHINESE) ||
    //        (settingsLanguage == SC_LANG_KOREAN))
    //    {
    //        baseNameTPL = baseNameTPL + "other";
    //    }
    //    else
    //    {
    //        baseNameTPL = baseNameTPL + "noa";
    //    }
    //}
    //else
    //{
    //    baseNameTPL = baseNameTPL + "noe";
    //}

    // Add the strap file base name 
    baseNameTPL = baseNameTPL + "strap_";

    // Add the language    
    switch (settingsLanguage)
    {
    default:
    case SC_LANG_ENGLISH:
        baseNameTPL = baseNameTPL + "en"; break;    
    case SC_LANG_GERMAN:
        baseNameTPL = baseNameTPL + "de"; break;
    case SC_LANG_FRENCH:
        baseNameTPL = baseNameTPL + "fr"; break;
    case SC_LANG_SPANISH:
        baseNameTPL = baseNameTPL + "sp"; break;
    case SC_LANG_ITALIAN:
        baseNameTPL = baseNameTPL + "it"; break;
    case SC_LANG_DUTCH:
        baseNameTPL = baseNameTPL + "du"; break;

    case SC_LANG_JAPANESE:
    case SC_LANG_SIMP_CHINESE:
    case SC_LANG_TRAD_CHINESE:
    case SC_LANG_KOREAN: 
        ITF_ASSERT_CRASH(0,"strap screen not present");
        break;
    }

    // Add screen ratio
    //if (SCGetAspectRatio()==SC_ASPECT_RATIO_4x3)
    //    baseNameTPL = baseNameTPL + "_43";
    //else
    //    baseNameTPL = baseNameTPL + "_169";

	// Load screens 
	u32 i,uAnim = 0;
	u32 uLanguage = SCGetLanguage();
	for (i=0; i<SPLASHSCREEN_COUNT; i++)
	{
		// Add number of splash screen
        String nameBuf("");
        nameBuf.setTextFormat("%s_%d.tpl", baseNameTPL.cStr(), i);
	    
	    // Get TPL datas			
		if(!SystemAdapter_TPL::readAndAlloc(&m_TPLList[i], nameBuf))
		{
			    	ITF_ASSERT(0);
		    return ;// FATAL ERROR
		}
		SystemAdapter_TPL::unpack(m_TPLList[i]);
	}
	
	m_initialized = btrue;
}

void SystemAdapter_SplashScreen::BeginDisplaying()
{
	SYSTEM_ADAPTER_WII->disableHomeButton();
    TRC_ADAPTER->setCanDrawContent(bfalse);

    m_lastTimeDisplayed     = SYSTEM_ADAPTER->getTime();
    m_currentTPLDisplayed   = 0;
    m_changeTPLNb = 0;
    m_currentRenderingStep  = FadeIn;
    m_isDisplayingSplashScreen = btrue;	

    GFX_ADAPTER_WII->setResolution_WII((SCGetAspectRatio() == SC_ASPECT_RATIO_16x9) ? 812 : 608, 456, SCGetAspectRatio());
}

void SystemAdapter_SplashScreen::EndDisplaying()
{
	SYSTEM_ADAPTER_WII->enableHomeButton();	
    TRC_ADAPTER->setCanDrawContent(btrue);

    m_isDisplayingSplashScreen = bfalse;

    for (u32 i = 0; i < SPLASHSCREEN_COUNT; i++)
    {
        Memory::alignedFree(m_TPLList[i]);
    }

    // Set previous resolution
    GFX_ADAPTER_WII->setResolution_WII(GFX_ADAPTER->getScreenWidth(), GFX_ADAPTER->getScreenHeight(), SC_ASPECT_RATIO_16x9);
    GFX_ADAPTER_WII->resetGX();
}

void SystemAdapter_SplashScreen::update()
{
	ITF_ASSERT(m_initialized);
	if(!m_initialized) return;
    if(!m_isDisplayingSplashScreen) return;
    if(m_isPaused) return;

    f64 currentTime =  SYSTEM_ADAPTER->getTime() - m_pausedTime;
    m_alpha = 255;
    switch ( m_currentRenderingStep )
    {
    case FadeIn:

        if(m_lastTimeDisplayed + SPLASHSCREEN_FADTETIME < currentTime)
        {
            m_lastTimeDisplayed     = currentTime;
            m_currentRenderingStep  = Rendering;

            if(!m_firstTimeDisplayed)
                m_firstTimeDisplayed = currentTime;
        }
        else
        {
            m_alpha = ( currentTime - m_lastTimeDisplayed ) / SPLASHSCREEN_FADTETIME * 255;
        }        

        break;
    case FadeOut:

        if(m_lastTimeDisplayed + SPLASHSCREEN_FADTETIME < currentTime)
        {           
            EndDisplaying();
            return; 
        }
        else
        {
            m_alpha = (1.0f -( currentTime - m_lastTimeDisplayed ) / SPLASHSCREEN_FADTETIME) * 255;
        }

        break;
    case Rendering:

        if((m_lastTimeDisplayed + SPLASHSCREEN_DISPLAYTIME < currentTime) && (m_changeTPLNb < 3))
        {
            m_changeTPLNb++;
            m_lastTimeDisplayed     = currentTime;
            m_currentTPLDisplayed   = (m_currentTPLDisplayed + 1) % SPLASHSCREEN_COUNT;
        }
        break;
    }

    // Force Fade-Out if max time is elapsed or if any button is pressed after min time is elapsed
    if( ( (m_firstTimeDisplayed + SPLASHSCREEN_MAXTIME < currentTime)
        || (m_firstTimeDisplayed + SPLASHSCREEN_MINTIME < currentTime && isAnyButtonPressed()) )
        && m_currentRenderingStep == Rendering)
    {
        m_currentRenderingStep  = FadeOut;
        m_lastTimeDisplayed     = currentTime;
    }
}

void SystemAdapter_SplashScreen::render()
{
    if(m_isDisplayingSplashScreen)
    {
		GFX_ADAPTER_WII->setResolution_WII((SCGetAspectRatio() == SC_ASPECT_RATIO_16x9) ? 812 : 608, 456, SCGetAspectRatio());

		// Display it 
		Vec2d pos , size;
        if(SCGetAspectRatio() == SC_ASPECT_RATIO_16x9)
        {   
            static const f32 ratio_4_3 = 4.0f / 3.0f;
            pos.set ( (GFX_ADAPTER_WII->getRealScreenWidth() - (GFX_ADAPTER_WII->getRealScreenWidth() / ratio_4_3) ) / 2.0f , 0.0f);
            size.set ( GFX_ADAPTER_WII->getRealScreenWidth() / ratio_4_3, GFX_ADAPTER_WII->getRealScreenHeight() );

        } else
        {
            size.set( GFX_ADAPTER_WII->getRealScreenWidth(), GFX_ADAPTER_WII->getRealScreenHeight() );
            pos = Vec2d::Zero;
        }

        SystemAdapter_TPL::draw(m_TPLList[m_currentTPLDisplayed], pos, size, m_alpha, btrue);
    }    
}

bbool SystemAdapter_SplashScreen::isAnyButtonPressed()
{
    InputAdapter::PressStatus	buttons[JOY_MAX_BUT];	
    bbool _anyButtonPressed = false;
    for (u32 _pad = 0; _pad < JOY_MAX_COUNT && !_anyButtonPressed; _pad ++)
    {
        INPUT_ADAPTER->getGamePadButtons(InputAdapter::EnvironmentEngine, _pad, buttons, JOY_MAX_BUT);
        for (u32 _button = 0; _button < JOY_MAX_BUT && !_anyButtonPressed; _button++)
        {
            if(_button != m_joyButton_Home)
                _anyButtonPressed = (buttons[_button] == InputAdapter::JustPressed);		
        }        
    }
    return _anyButtonPressed;
}

void SystemAdapter_SplashScreen::pause()
{
    if(m_isPaused || !m_isDisplayingSplashScreen) return;

    m_isPaused = btrue;
    m_pausedStart = SYSTEM_ADAPTER->getTime();
}

void SystemAdapter_SplashScreen::unpause()
{
    if(!m_isPaused || !m_isDisplayingSplashScreen) return;

    m_isPaused = bfalse;
    m_pausedTime += SYSTEM_ADAPTER->getTime() - m_pausedStart;
}


} // namespace ITF