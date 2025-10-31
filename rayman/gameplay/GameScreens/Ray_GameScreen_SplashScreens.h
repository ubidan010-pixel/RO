#ifndef _ITF_RAY_GAMESCREEN_SPLASHSCREENS_H_
#define _ITF_RAY_GAMESCREEN_SPLASHSCREENS_H_

#ifndef _ITF_GAMESCREENBASE_H_
#include "gameplay/Managers/GameScreens/GameScreenBase.h"
#endif //_ITF_GAMESCREENBASE_H_

namespace ITF
{
    class AnimLightComponent;
    class MoviePlayerComponent;
    class UIComponent;

    extern const char* Ray_GameScreen_SplashScreenBase_GetClassNameStatic();

    //////////////////////////////////////////////////////////////////////////
    ///Splash screen base class
    class Ray_GameScreen_SplashScreenBase : public GameScreenBase
    {
        DECLARE_OBJECT_CHILD_TEMPLATE_RTTI_ABSTRACT(Ray_GameScreen_SplashScreenBase,GameScreenBase,3625070760);
    public:
        //Implementation of base class members

        Ray_GameScreen_SplashScreenBase();
        ~Ray_GameScreen_SplashScreenBase();

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Next screen (you must override it)
        virtual StringID getNextScreen() const = 0;

        virtual void init();

        virtual void close();

        virtual void update();

        virtual bbool hasGameplay() {return bfalse;}

        virtual bbool canPause() {return bfalse;}
        
        virtual bbool canDisplayTRCMessages() {return bfalse;}

        virtual bbool isLoaded() const { return m_loaded; }

        void onWorldLoaded();

        virtual bbool checkSignInOutChanges();

        enum ScreenState
        {
            Idle=0,
            PlayingMovie,
            PlayingAnim,
            WaitingKENKO,
            WaitingKIDOJI,
            WaitingSHIYOJO,
            Exiting
        };
    private:
        static bbool m_wasInitialized;
        AnimLightComponent *m_animComponent;
        bbool m_resetAnimDone;
        MoviePlayerComponent *m_moviePlayerComponent;

        enum JapLogo
        {
            Kenko_480=0,
            Kenko_720,
            Kenko_1080,
            Kidoji_480,
            Kidoji_720,
            Kidoji_1080,
            Shiyojo_480,
            Shiyojo_720,
            Shiyojo_1080,
            JapLogo_MAX
        };
        JapLogo m_currentJapLogo;
        UIComponent* m_japLogoComponent[JapLogo_MAX]; 

        bbool m_loaded;
        f64 m_japLogoStartedTime, m_japLogo_Duration;

        ScreenState  m_screenState;
        void changeScreenState(ScreenState screenState) { m_screenState = screenState; }
        void changeGameScreen();
        void startPlayingVideo();
    };


    //////////////////////////////////////////////////////////////////////////
    ///Splash screen for ITF Framework
    class Ray_GameScreen_Framework : public Ray_GameScreen_SplashScreenBase
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_GameScreen_Framework,Ray_GameScreen_SplashScreenBase,3413566858);
    public:
        virtual StringID getNextScreen() const;


        ///1ST SPLASH LOADS BASE SCENE
        virtual void init() {Ray_GameScreen_SplashScreenBase::init();}
    };
}

#endif
