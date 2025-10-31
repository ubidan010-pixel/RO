#ifdef ITF_WII

#ifndef _ITF_SYSTEMADAPTER_SPLASHSCREEN_WII_H_
#define _ITF_SYSTEMADAPTER_SPLASHSCREEN_WII_H_

#ifndef TPL_H
#include <revolution/tpl.h>
#endif //TPL_H

namespace ITF
{
    // Configuration parameters
#define SPLASHSCREEN_COUNT 		    2       // With extension, and without extension
#define SPLASHSCREEN_DISPLAYTIME    5       // In sec, time for one screen 
#define SPLASHSCREEN_FADTETIME      0.5     // Fade-in and Fade-out time
#define SPLASHSCREEN_MINTIME        1       // In sec, time to wait before user can pass the screen
#define SPLASHSCREEN_MAXTIME        20      // In sec, time to wait before remove the screen

	class SystemAdapter_SplashScreen
	{
	  public:
        enum RenderingStep
        {
            Idle=0,
            FadeIn,
            Rendering,
            FadeOut
        };

	  	SystemAdapter_SplashScreen() : m_initialized(bfalse), m_isDisplayingSplashScreen(bfalse), m_lastTimeDisplayed(0.0), m_currentTPLDisplayed(0),
            m_currentRenderingStep(Idle), m_firstTimeDisplayed(0.0),
            m_alpha(0), m_isPaused(bfalse), m_pausedTime(0.0), m_pausedStart(0.0)
	  	{ 	}

	  	// Need to be done by SystemAdapter quickly
	  	void InitSplashScreen();
	  	
	  	// Begin/End the displaying of splashScreen
	  	void BeginDisplaying();
	  	void EndDisplaying();
	  	
	  	// Update to check some stuff
	  	void update();
        void render();

        // Pause/Unpause
        void pause();
        void unpause();
	  	
	  	ITF_INLINE bbool isDisplayingSplashScreen() const { return m_isDisplayingSplashScreen; }
        ITF_INLINE bbool isInitialized() const { return m_initialized; }

        static bbool isAnyButtonPressed();
	  private:
	  	bbool m_initialized;
	  	bbool m_isDisplayingSplashScreen;
        bbool m_isPaused;
        f64   m_pausedStart, m_pausedTime;
	  	u8    m_alpha;

        TPLPalettePtr m_TPLList[SPLASHSCREEN_COUNT];
        u32 m_currentTPLDisplayed;
        u32 m_changeTPLNb;
        f64 m_lastTimeDisplayed, m_firstTimeDisplayed;
        RenderingStep m_currentRenderingStep;        
	};
} // namespace ITF


#endif //_ITF_SYSTEMADAPTER_SPLASHSCREEN_WII_H_

#endif //ITF_WII