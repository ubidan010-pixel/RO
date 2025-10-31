#ifndef _ITF_RAY_GAMESCREEN_WORLDMAP_H_
#define _ITF_RAY_GAMESCREEN_WORLDMAP_H_

#ifndef _ITF_GAMESCREENBASE_H_
#include "gameplay/Managers/GameScreens/GameScreenBase.h"
#endif //_ITF_GAMESCREENBASE_H_

#ifndef _ITF_RAY_PAUSE_MENUITEMACTIONLISTENER_H_
#include "rayman/gameplay/GameScreens/Ray_Pause_MenuItemActionListener.h"
#endif //_ITF_RAY_PAUSE_MENUITEMACTIONLISTENER_H_

#ifndef _ITF_TRAJECTORYNODECOMPONENT_H_
#include "gameplay/Components/Trajectory/TrajectoryNodeComponent.h"
#endif //_ITF_TRAJECTORYNODECOMPONENT_H_

#ifndef _ITF_RAY_JOINLEAVEGAMEHANDLER_H_
#include "rayman/gameplay/GameScreens/Ray_JoinLeaveGameHandler.h"
#endif //_ITF_RAY_JOINLEAVEGAMEHANDLER_H_

namespace ITF {

//------------------------------------------------------------------------------
/// The world map
class Ray_GameScreen_WorldMap : public GameScreenBase, public Ray_Pause_MenuItemActionListener
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_GameScreen_WorldMap,GameScreenBase,3566109816);

public:
    Ray_GameScreen_WorldMap();
    ~Ray_GameScreen_WorldMap();

    struct InitializationParameters : public GameScreenBase::InitializationParameters
    {
        InitializationParameters() : m_worldRef(ObjectRef::InvalidRef) {}
        virtual ~InitializationParameters() {}

        virtual GameScreenBase::InitializationParameters *clone() const {return newAlloc(mId_Gameplay, InitializationParameters(*this));}
        ObjectRef   m_worldRef;
    };

    // base class overrides
    virtual void init();
    virtual void close();
    virtual void update();
    virtual bbool hasGameplay() {return btrue;}
    virtual bbool getSpawnPlayersAlways() {return btrue;}
    virtual void onWorldLoaded();
    virtual bbool canPause();
    virtual void showPauseMenu (bbool _isVisible);
    virtual bbool checkSignInOutChanges();
    // WORLDMAP
    void        setPreLoadedMap( ObjectRef _mapRef ) { m_preLoadedMap = _mapRef; }
    ObjectRef   getPreLoadedMap() const { return m_preLoadedMap; }
    
    static void saveSystemEnableStatutCallback(bbool _enable);
    void        showConfig(bbool _isVisible);
    
    static bbool findSpot(
        TrajectoryNodeComponent* _from,
        TrajectoryNodeComponent::NodeList& _path,
        TrajectoryNodeComponent::NodeList& _visited,
        u32 _recursionCount);

private:
    bbool m_firstFrame;
    Ray_JoinLeaveGameHandler m_joinLeaveGameHandler;

    ObjectRef       m_preLoadedMap;
    bbool           m_showingControls;
    bbool           m_musicThemeSet;
};

} // namespace ITF

#endif
