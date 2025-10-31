#ifndef _ITF_RAY_WORLDMAPGAMEMODECONTROLLER_H_
#define _ITF_RAY_WORLDMAPGAMEMODECONTROLLER_H_

#ifndef _ITF_GAMEMODECONTROLLER_H_
#include "gameplay/managers/GameModeController.h"
#endif //_ITF_GAMEMODECONTROLLER_H_

namespace ITF
{

//------------------------------------------------------------------------------
class Ray_WorldMapGameModeController : public GameModeController 
{
    typedef GameModeController Super;

public :
    Ray_WorldMapGameModeController() { ITF_ASSERT(bfalse); m_gameMode = GAMEMODE_UNKNOWN; } // nope!
    Ray_WorldMapGameModeController(GameMode _gamemode);
	virtual ~Ray_WorldMapGameModeController();

    virtual void        update();

private :
	Ray_WorldMapGameModeController(const GameModeController &_other) { ITF_ASSERT(bfalse); } // nope!

};

}// namespace ITF

#endif //_ITF_RAY_WORLDMAPGAMEMODECONTROLLER_H_
