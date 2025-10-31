#ifndef _ITF_RAY_GAMESCREEN_WORLDMAPLOADING_H_
#define _ITF_RAY_GAMESCREEN_WORLDMAPLOADING_H_

#ifndef _ITF_GAMESCREENBASE_H_
#include "gameplay/Managers/GameScreens/GameScreenBase.h"
#endif //_ITF_GAMESCREENBASE_H_

#ifndef _ITF_RAY_PAUSE_MENUITEMACTIONLISTENER_H_
#include "rayman/gameplay/GameScreens/Ray_Pause_MenuItemActionListener.h"
#endif //_ITF_RAY_PAUSE_MENUITEMACTIONLISTENER_H_

#ifndef _ITF_RAY_JOINLEAVEGAMEHANDLER_H_
#include "rayman/gameplay/GameScreens/Ray_JoinLeaveGameHandler.h"
#endif //_ITF_RAY_JOINLEAVEGAMEHANDLER_H_

namespace ITF {

//------------------------------------------------------------------------------
/// The world map
class Ray_GameScreen_WorldMapLoading : public GameScreenBase, public Ray_Pause_MenuItemActionListener, public IInputListener
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_GameScreen_WorldMapLoading,GameScreenBase,1826584106)

public:
    Ray_GameScreen_WorldMapLoading();
    ~Ray_GameScreen_WorldMapLoading();

    struct InitializationParameters : public GameScreenBase::InitializationParameters
    {
        InitializationParameters()
            : m_loadCheckpointData(bfalse)
            , m_forceUnloadBaseMap(bfalse)
            , m_worldRef(ObjectRef::InvalidRef)
            , m_startFade(btrue)
        {}
        virtual ~InitializationParameters() {}

        virtual GameScreenBase::InitializationParameters *clone() const {return newAlloc(mId_Gameplay, InitializationParameters(*this));}

        Path        m_mapName;
        ObjectPath  m_checkPointPath;
        bbool       m_loadCheckpointData;
        bbool       m_forceUnloadBaseMap;
        bbool       m_startFade;
        ObjectRef   m_worldRef;
        ObjectPath  m_prefetchPath;
    };

    // base class overrides
    virtual void init();
    virtual void close();
    virtual void update();

    void updatePlayers();

    virtual bbool hasGameplay() {return btrue;}
    virtual bbool getSpawnPlayersAlways() {return btrue;}
    virtual bbool canPause() {return bfalse;}
    virtual bbool canDisplayTRCMessages() {return bfalse;}
    virtual void onWorldLoaded();
    virtual bbool checkSignInOutChanges();
    virtual void Receive( u32 deviceID, f32 axis, const StringID& action );

    static void preload(const Path & _path);

    static ObjectRef m_worldRef;
private:
    ObjectRef m_newWorld;
    i32 m_timer;
    bbool m_validLoadingScreen;
    ActorRef m_levelTitle;
    bbool m_driveInputs;
    Ray_JoinLeaveGameHandler m_joinLeaveGameHandler;

    enum State
    {
        State_PrefetchedLoading, 
        State_FadeToLoading,
        State_WaitForLevelLoad,
        State_WaitForMusicLoad,
        State_TitleDisappearing,
    };

    State m_state;
};

} // namespace ITF

#endif
