#ifndef _ITF_RAY_GAMESCREEN_GAMEPLAY_H_
#define _ITF_RAY_GAMESCREEN_GAMEPLAY_H_

#ifndef _ITF_GAMESCREENBASE_H_
#include "gameplay/Managers/GameScreens/GameScreenBase.h"
#endif //_ITF_GAMESCREENBASE_H_

#ifndef _ITF_RAY_PAUSE_MENUITEMACTIONLISTENER_H_
#include "rayman/gameplay/GameScreens/Ray_Pause_MenuItemActionListener.h"
#endif //_ITF_RAY_PAUSE_MENUITEMACTIONLISTENER_H_

#ifndef _ITF_OBJECTPATH_H_
#include "core/ObjectPath.h"
#endif //_ITF_OBJECTPATH_H_

#ifndef _ITF_RAY_JOINLEAVEGAMEHANDLER_H_
#include "rayman/gameplay/GameScreens/Ray_JoinLeaveGameHandler.h"
#endif //_ITF_RAY_JOINLEAVEGAMEHANDLER_H_

#ifndef _ITF_RAY_OPTIONMENUHELPER_H_
#include "rayman/gameplay/Managers/GameOptions/Ray_OptionMenuHelper.h"
#endif //_ITF_RAY_OPTIONMENUHELPER_H_

namespace ITF
{
    //////////////////////////////////////////////////////////////////////////
    ///The gameplay phase
    class Ray_GameScreen_Gameplay : public GameScreenBase, public Ray_Pause_MenuItemActionListener
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_GameScreen_Gameplay,GameScreenBase,4191223421);

    public:
        struct InitializationParameters : public GameScreenBase::InitializationParameters
        {
            InitializationParameters() : m_loadCheckpointData(bfalse),m_forceUnloadBaseMap(bfalse), m_worldRef(ObjectRef::InvalidRef) {}
            virtual ~InitializationParameters() {}

            virtual GameScreenBase::InitializationParameters *clone() const {return newAlloc(mId_Gameplay, InitializationParameters(*this));}

            Path        m_mapName;
            ObjectPath  m_checkPointPath;
            bbool       m_loadCheckpointData;
            bbool       m_forceUnloadBaseMap;
            ObjectRef   m_worldRef;
        };

        //Implementation of base class members

        Ray_GameScreen_Gameplay();
        ~Ray_GameScreen_Gameplay();

        virtual void  init();

        virtual void  close();

        virtual void  update();

        virtual bbool hasGameplay() {return btrue;}

        virtual bbool canPause();

        virtual void  onWorldLoaded();
        virtual void    onEndOfLevelReached() ;

        virtual void  showPauseMenu (bbool _isVisible);

        virtual bbool inTutorial() const;
        virtual void  showTutorial();
        virtual void  hideTutorial();

        virtual void onMapLoaded(ObjectRef _world);
        virtual void onMenuItemAction (UIComponent* _UIComponent);
        virtual bbool checkSignInOutChanges();

    private:
        void spawnSkipLevel();
        void destroySkipLevel();
        void createLumMusicManager();


        virtual void pause_showWorldMap();
        virtual void pause_addPlayer();

        Ray_JoinLeaveGameHandler m_joinLeaveGameHandler;
        bbool m_isLoaded;
        Path m_lumMusicManagerACT;

        bbool m_confirmRestart, m_confirmExit, m_confirmMainMenu;
        bbool m_backToWMOnConfirm;

        ActorRef m_skipLevelActor;

        bbool   m_wasPreloaded;
        Ray_OptionMenuHelper m_optionMenuHelper;
    };
}

#endif
