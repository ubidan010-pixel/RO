#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_GAMESCREEN_SPLASHSCREENS_H_
#include "rayman/gameplay/GameScreens/Ray_GameScreen_SplashScreens.h"
#endif //_ITF_RAY_GAMESCREEN_SPLASHSCREENS_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_SINGLETONS_H_
#include "core/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_RAY_GAMESCREEN_MAINMENU_H_
#include "rayman/gameplay/GameScreens/Ray_GameScreen_MainMenu.h"
#endif //_ITF_RAY_GAMESCREEN_MAINMENU_H_

#ifndef _ITF_MOVIEPLAYERCOMPONENT_H_
#include "engine/actors/components/MoviePlayerComponent.h"
#endif //_ITF_MOVIEPLAYERCOMPONENT_H_

#ifndef _ITF_TRCManagerAdapter_H_
#include "engine/AdaptersInterfaces/TRCManager_Adapter.h"
#endif //_ITF_TRCManagerAdapter_H_

#ifndef _ITF_LOADINGSCREEN_H_
#include "engine/loading/loadingscreen.h"
#endif //_ITF_LOADINGSCREEN_H_

#ifndef _ITF_CONFIG_H_
#include "core/Config.h"
#endif //_ITF_CONFIG_H_

#ifndef _ITF_TEXTUREGRAPHICCOMPONENT2D_H_
#include "engine/actors/components/texturegraphiccomponent2D.h"
#endif //_ITF_TEXTUREGRAPHICCOMPONENT2D_H_

#define ENABLE_UIROBOT_ANIM 0

namespace ITF
{

    IMPLEMENT_OBJECT_RTTI_TEMPLATE(Ray_GameScreen_SplashScreenBase)

    IMPLEMENT_OBJECT_RTTI(Ray_GameScreen_Framework)

    bbool Ray_GameScreen_SplashScreenBase::m_wasInitialized = bfalse;

    Ray_GameScreen_SplashScreenBase::Ray_GameScreen_SplashScreenBase() :
    m_animComponent(NULL), m_moviePlayerComponent(NULL),
        m_screenState(Idle), m_resetAnimDone(bfalse), m_loaded(bfalse),
        m_japLogoStartedTime(0.0), m_japLogo_Duration(0.0), m_currentJapLogo(Kenko_480)
#ifdef ITF_PS5
        , m_ps5StartupStartedTime(0.0)
        , m_ps5StartupHidden(bfalse)
#endif
    {        

        for(u32 i = 0; i < JapLogo_MAX; i++)
            m_japLogoComponent[i] = NULL;
    }

    Ray_GameScreen_SplashScreenBase::~Ray_GameScreen_SplashScreenBase()
    {        
    }

    void Ray_GameScreen_SplashScreenBase::init()
    {
        ObjectRef rWorld = RAY_GAMEMANAGER->createMap();
        
        if(!rWorld.isValid())
        {
            //do nothing
            m_world = NULL;
        }
        else
        {       
            m_world = (World*)GETOBJECT(rWorld);
            RAY_GAMEMANAGER->loadMap(rWorld, RAY_GAMEMANAGER->getSplashMap(),btrue,btrue, btrue, GameManager::LEVEL_NAME_INITIALISATION_TYPE_EMPTY);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameScreen_SplashScreenBase::onWorldLoaded()
    {
        Scene * scene = m_world->getRootScene();
        const PickableList &pickables=scene->getActors();

        u32 count = pickables.size();
        for (u32 index=0; index<count; index++)
        {
#if ENABLE_UIROBOT_ANIM
            if (AnimLightComponent *animComp = ((Actor*)pickables[index])->GetComponent<AnimLightComponent>())
            {
                m_animComponent = animComp;
                continue;
            }
#endif // 0
            if (UIComponent *UIComp = ((Actor*)pickables[index])->GetComponent<UIComponent>())
            {
                if(UIComp->getID() == ITF_GET_STRINGID_CRC(LOGO_KENKO_480,3615141419))
                {
                    m_japLogoComponent[Kenko_480] = UIComp;
                }                 
                else if(UIComp->getID() == ITF_GET_STRINGID_CRC(LOGO_KENKO_720,2379300525))
                {
                    m_japLogoComponent[Kenko_720] = UIComp;
                } 
                else if(UIComp->getID() == ITF_GET_STRINGID_CRC(LOGO_KENKO_1080,950615127))
                {
                    m_japLogoComponent[Kenko_1080] = UIComp;
                } 
                else if(UIComp->getID() == ITF_GET_STRINGID_CRC(LOGO_KIDOJI_480,4258925526))
                {
                    m_japLogoComponent[Kidoji_480] = UIComp;
                } 
                else if(UIComp->getID() == ITF_GET_STRINGID_CRC(LOGO_KIDOJI_720,1091851443))
                {
                    m_japLogoComponent[Kidoji_720] = UIComp;
                } 
                else if(UIComp->getID() == ITF_GET_STRINGID_CRC(LOGO_KIDOJI_1080,4087243262))
                {
                    m_japLogoComponent[Kidoji_1080] = UIComp;
                } 
                else if(UIComp->getID() == ITF_GET_STRINGID_CRC(LOGO_SHIYOJO_480,2539791049))
                {
                    m_japLogoComponent[Shiyojo_480] = UIComp;
                } 
                else if(UIComp->getID() == ITF_GET_STRINGID_CRC(LOGO_SHIYOJO_720,1124236401))
                {
                    m_japLogoComponent[Shiyojo_720] = UIComp;
                } 
                else if(UIComp->getID() == ITF_GET_STRINGID_CRC(LOGO_SHIYOJO_1080,2164412602))
                {
                    m_japLogoComponent[Shiyojo_1080] = UIComp;
                } 

                UIComp->GetActor()->disable();
                continue;
            }

            if (MoviePlayerComponent *movieComp = ((Actor*)pickables[index])->GetComponent<MoviePlayerComponent>())
            {
                m_moviePlayerComponent = movieComp;
                continue;
            }
        }

        if (RAY_GAMEMANAGER->isInPause())
            RAY_GAMEMANAGER->leavePause(bfalse);
    }

    void Ray_GameScreen_SplashScreenBase::update()
    {
        // If the world is not loaded (not found), just ask for exit 
        if (!m_world && !m_currentlyExiting)
        {
            changeGameScreen();
        }

        // If there is something wrong , leave here !
        if(m_currentlyExiting || !m_isWorldLoaded) 
            return;

        // If the boot is disable, just skip it
#ifndef ITF_FINAL
        if(!CONFIG->m_enableBootLogos)
        {   
            changeGameScreen();
            return;
        }
#endif 

        bbool requiredLoadingComplete = GAMEMANAGER->isMenusLoaded();

#define USE_PROLOGUE_PRELOAD

#ifdef USE_PROLOGUE_PRELOAD
        static bool preloadStarted = bfalse;
        if(requiredLoadingComplete)
        {
            if(!preloadStarted)
            {
                RAY_GAMEMANAGER->preloadPrologue();
                preloadStarted = btrue;
            }
            else
                requiredLoadingComplete = RAY_GAMEMANAGER->isPreloadedPrologueReady();
        }
#endif // USE_PROLOGUE_PRELOAD

        switch(m_screenState)
        {
        case Idle:            
            // Wait for TRC opening
            if(!TRC_ADAPTER || TRC_ADAPTER->canDrawContent())
            {
#ifdef ITF_PS5
                if (m_ps5StartupStartedTime == 0.0)
                    m_ps5StartupStartedTime = SYSTEM_ADAPTER->getTime();
#endif

                m_japLogo_Duration = 4.0;
#ifdef ITF_PS3
                if ( SYSTEM_ADAPTER->getSystemTerritory() != ITF_TERRITORY_OTHER )
#endif //ITF_PS3
                {
                    m_japLogo_Duration = 0.0;
                }

                if ( !SYSTEM_ADAPTER->isHDDMode() )
                {
                    m_japLogo_Duration = 0.0;
                }

                if ( m_japLogo_Duration > 0 )  
                {
                    changeScreenState(WaitingKIDOJI);

                    if ( GFX_ADAPTER->getScreenHeight() == 480 )
                        m_currentJapLogo = Kidoji_480;
                    else if ( GFX_ADAPTER->getScreenHeight() == 720 )
                        m_currentJapLogo = Kidoji_720;
                    else
                        m_currentJapLogo = Kidoji_1080;

                    m_japLogoStartedTime = SYSTEM_ADAPTER->getTime();
                }
#ifdef ITF_PS5
                else if (!m_ps5StartupHidden && m_ps5StartupStartedTime > 0.0)
                {
                    if (SYSTEM_ADAPTER->getTime() - m_ps5StartupStartedTime >= PS5_SPLASH_DURATION)
                    {
                        SYSTEM_ADAPTER->hideStartupSplashScreen();
                        m_ps5StartupHidden = btrue;
                    }
                }
#endif
                else
                {
                    startPlayingVideo();
                }
            }
            break;
        case PlayingMovie:
            if(!m_moviePlayerComponent || !m_moviePlayerComponent->isPlayingMovie())
            {   
                changeScreenState(PlayingAnim);
                if (m_animComponent) 
                {
                    m_animComponent->setPlayRate(1.0f);
                }
            }
            else
            {
                // Reset anim here to avoid some visual glish
                if (m_animComponent && !m_resetAnimDone && m_animComponent->isLoaded())
                {
                    m_animComponent->resetCurTime();
                    m_animComponent->setPlayRate(0.0f);
                    m_resetAnimDone = btrue;
                } 
            }
            break;
        case PlayingAnim:
            if ( !m_animComponent || m_animComponent->isSubAnimFinished() )
            {
                if ( requiredLoadingComplete )
                    changeGameScreen();
            }
            break;
        case WaitingSHIYOJO:
        case WaitingKIDOJI:
        case WaitingKENKO:
            if ( SYSTEM_ADAPTER->getTime() - m_japLogoStartedTime >= m_japLogo_Duration )
            {
                if( m_screenState == WaitingKIDOJI )
                {
                    changeScreenState(WaitingKENKO);

                    if ( GFX_ADAPTER->getScreenHeight() == 480 )
                        m_currentJapLogo = Kenko_480;
                    else if ( GFX_ADAPTER->getScreenHeight() == 720 )
                        m_currentJapLogo = Kenko_720;
                    else
                        m_currentJapLogo = Kenko_1080;

                    m_japLogoStartedTime = SYSTEM_ADAPTER->getTime();
                }
                else if ( m_screenState == WaitingKENKO )
                {
                    changeScreenState(WaitingSHIYOJO);

                    if ( GFX_ADAPTER->getScreenHeight() == 480 )
                        m_currentJapLogo = Shiyojo_480;
                    else if ( GFX_ADAPTER->getScreenHeight() == 720 )
                        m_currentJapLogo = Shiyojo_720;
                    else
                        m_currentJapLogo = Shiyojo_1080;

                    m_japLogoStartedTime = SYSTEM_ADAPTER->getTime();
                }
                else
                    startPlayingVideo();               
            } 
            if(m_japLogoComponent[m_currentJapLogo]!=NULL)
            {
                TextureGraphicComponent2D* component2D = m_japLogoComponent[m_currentJapLogo]->GetActor()->GetComponent<TextureGraphicComponent2D>();
                if ( component2D && component2D->getTexture() && component2D->getTexture()->isPhysicallyLoaded() )
                {
                    Quad2DInfo texInfo;
                    Texture* tex = component2D->getTexture();
                    texInfo.m_angle = 0.f;
                    texInfo.m_color     = COLOR_WHITE;
                    texInfo.m_width     = (f32)tex->getSizeX();
                    texInfo.m_height    = (f32)tex->getSizeY();
                    texInfo.m_texture   = component2D->getTexture();
                    texInfo.m_uvStart   = Vec2d::Zero;
                    texInfo.m_uvEnd     = Vec2d::One;
                    texInfo.m_pos2D.m_x = GFX_ADAPTER->getScreenWidth() * 0.5f - texInfo.m_width     * 0.5f;
                    texInfo.m_pos2D.m_y = GFX_ADAPTER->getScreenHeight() * 0.5f - texInfo.m_height     * 0.5f;

                    GFX_ADAPTER->addPrimitive2d(Vec2d::Zero, Vec2d::One, 0, GFX_QUADS, NULL, NULL, &texInfo);
                }               
            }
            break;
        }
    }

    void Ray_GameScreen_SplashScreenBase::close()
    {
    }

    void Ray_GameScreen_SplashScreenBase::changeGameScreen()
    {
        m_currentlyExiting = btrue;
        RAY_GAMEMANAGER->setStartLoadingMainMenuFirstTime(btrue);
        RAY_GAMEMANAGER->postGameScreenChange_String(getNextScreen(), bfalse);
    }

    ITF::bbool Ray_GameScreen_SplashScreenBase::checkSignInOutChanges()
    {
        if (m_playerSignedInOut)
        {
            if (!m_currentlyExiting)
            {
                m_currentlyExiting = btrue;
                RAY_GAMEMANAGER->goToTheMainMenu();
            }
        }
        return m_currentlyExiting;

    }

    void Ray_GameScreen_SplashScreenBase::startPlayingVideo()
    {
        changeScreenState(PlayingMovie);                
        if(m_moviePlayerComponent)
            m_moviePlayerComponent->playMovie();
        m_loaded = btrue;

        for(u32 i = 0; i < JapLogo_MAX; i++)
        {
            if ( m_japLogoComponent[i] )
                m_japLogoComponent[i]->GetActor()->disable();
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////


    StringID Ray_GameScreen_Framework::getNextScreen() const 
    {
        return Ray_GameScreen_MainMenu::GetClassCRCStatic();
    }
}
