#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_WORLDMAPGAMEMODECONTROLLER_H_
#include "rayman/gameplay/Managers/Ray_WorldMapGameModeController.h"
#endif //_ITF_RAY_WORLDMAPGAMEMODECONTROLLER_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

namespace ITF
{

//------------------------------------------------------------------------------
Ray_WorldMapGameModeController::Ray_WorldMapGameModeController( GameMode _gamemode ) 
    : GameModeController( _gamemode )
{
}

//------------------------------------------------------------------------------
Ray_WorldMapGameModeController::~Ray_WorldMapGameModeController()
{
}

void Ray_WorldMapGameModeController::update()
{
    Super::update();

    /*InputAdapter::PressStatus buttons[JOY_MAX_BUT];
    INPUT_ADAPTER->getGamePadButtons(InputAdapter::EnvironmentEngine, 0, buttons, JOY_MAX_BUT);

    if (buttons[m_joyButton_X] == InputAdapter::JustReleased)
    {
        Player* player = RAY_GAMEMANAGER->getPlayer(0);

        if ( player && player->getCurrentMode() == RAY_GAMEMODE_WORLDMAP )
        {
            RAY_GAMEMANAGER->triggerChangeRoomSequence(btrue);
        }
    }*/
}

} // namespace
