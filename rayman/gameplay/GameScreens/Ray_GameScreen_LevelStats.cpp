#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_LEVELSTATS_H_
#include "rayman/gameplay/GameScreens/Ray_GameScreen_LevelStats.h"
#endif //_ITF_RAY_LEVELSTATS_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_SINGLETONS_H_
#include "core/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#define LEVELSTATS_FRIENDLY "levelstats"

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_GameScreen_LevelStats)
    //
    //
    ///////////////////////////////////////////////////////////////////////////////////////////
    Ray_GameScreen_LevelStats::Ray_GameScreen_LevelStats()
    {}

    ///////////////////////////////////////////////////////////////////////////////////////////
    Ray_GameScreen_LevelStats::~Ray_GameScreen_LevelStats()
    {}

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameScreen_LevelStats::init()
    {
        RAY_GAMEMANAGER->enableBaseScene(bfalse);
        showLevelStats (btrue);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameScreen_LevelStats::onWorldLoaded()
    {
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameScreen_LevelStats::update()
    {
        //if (!m_isWorldLoaded)
        //{
        //    return;
        //}

        // Check for pads
        for (u32 i = 0; i < GAMEMANAGER->getMaxPlayerCount(); i++)
        {
            // Buttons
            InputAdapter::PressStatus buts[JOY_MAX_BUT];
            INPUT_ADAPTER->getGamePadButtons(InputAdapter::EnvironmentLua,i, buts, JOY_MAX_BUT);

            if( buts[m_joyButton_X] == InputAdapter::JustPressed )
            {
                RAY_GAMEMANAGER->goToTheMainMenu();
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameScreen_LevelStats::close()
    {
        RAY_GAMEMANAGER->enableBaseScene(btrue);
        showLevelStats (bfalse);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    bbool Ray_GameScreen_LevelStats::canPause()
    {
        return bfalse;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameScreen_LevelStats::showLevelStats (bbool _isVisible)
    {
        if (_isVisible)
        {
            UI_MENUMANAGER->showMenuPage(GAMEINTERFACE->getPauseMenuPriority(), LEVELSTATS_FRIENDLY, btrue, this);
            writeStats();
        }
        else
        {
            UI_MENUMANAGER->hideMenu(GAMEINTERFACE->getPauseMenuPriority());
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_GameScreen_LevelStats::writeStats()
    {
        RewardManager::GameSession * gameSession = REWARD_MANAGER->Session_GetPrevious();
        const StringID& baseName("levelStats");

        u32     totalCounter = 0;
        String8 fieldName, valueString;

        // Fills in lums counter
        for ( u32 i = 0 ; i < RewardManager::PlayersMax ; ++i )
        {
            if ( gameSession->m_actions[i].m_actif )
            {
                totalCounter += RAY_GAMEMANAGER->getCurrentScore().getLums(i);
            }
        }

        // Total Lums counter
        fieldName.setText("lumCounter");
        valueString.setTextFormat("%d",totalCounter);
        UI_MENUMANAGER->changeMenuItemFriendly(baseName, fieldName, valueString);

        // Total coins counter
        fieldName.setText("skullCoinCounter");
        valueString.setTextFormat("%d",RAY_GAMEMANAGER->getTotalTakenToothCount());
        UI_MENUMANAGER->changeMenuItemFriendly(baseName, fieldName, valueString);

        // Total time counter
        fieldName.setText("timeCounter");
        valueString.setTextFormat("%02d:%02d",gameSession->m_LengthInSec / 60, gameSession->m_LengthInSec % 60);
        UI_MENUMANAGER->changeMenuItemFriendly(baseName, fieldName, valueString);

        // Total cage counter
        fieldName.setText("cageCounter");
        valueString.setTextFormat("%d",RAY_GAMEMANAGER->getBrokenCageCount());
        UI_MENUMANAGER->changeMenuItemFriendly(baseName, fieldName, valueString);
    }

}
