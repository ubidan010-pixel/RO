#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_JOINLEAVEGAMEHANDLER_H_
#include "rayman/gameplay/GameScreens/Ray_JoinLeaveGameHandler.h"
#endif //_ITF_RAY_JOINLEAVEGAMEHANDLER_H_

#ifndef _ITF_CINEMATICMANAGER_H_
#include "gameplay/managers/CinematicManager.h"
#endif //_ITF_CINEMATICMANAGER_H_

#ifndef _ITF_CONFIG_H_
#include "core/Config.h"
#endif //_ITF_CONFIG_H_

#ifndef _ITF_LOADINGSCREEN_H_
#include "engine/loading/loadingscreen.h"
#endif //_ITF_LOADINGSCREEN_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_RAY_PLAYERHUDSCORECOMPONENT_H_
#include "rayman/gameplay/Components/Player/Ray_PlayerHudScoreComponent.h"
#endif //_ITF_RAY_PLAYERHUDSCORECOMPONENT_H_

#ifndef _ITF_TRCManagerAdapter_H_
#include "engine/AdaptersInterfaces/TRCManager_Adapter.h"
#endif //_ITF_TRCManagerAdapter_H_

#ifndef _ITF_RAY_SCORELUMAICOMPONENT_H_
#include "rayman/gameplay/Components/Lums/Ray_ScoreLumAIComponent.h"
#endif //_ITF_RAY_SCORELUMAICOMPONENT_H_

#ifndef _ITF_RAY_AIUTILS_H_
#include "Rayman/gameplay/AI/Utils/Ray_AIUtils.h"
#endif //_ITF_RAY_AIUTILS_H_

namespace ITF
{

//////////////////////////////////////////////////////////////////////////
// Players can join game with any button.
// (They can die/leave with "back", handled in the player controller itself.)
void Ray_JoinLeaveGameHandler::update()
{
    if (!CONFIG->m_pressStartToJoin)
    {
        return;
    }

    // cannot join while loading
    if ( LOADINGSCREEN && LOADINGSCREEN->isVisible())
    {
        return;
    }

    // cannot join while playing a cinematic
    if ( CINEMATICMANAGER && CINEMATICMANAGER->getLockPlayers())
    {
        return;
    }

    if ( TRC_ADAPTER->isDisplayingError() )
    {
        return;
    }

    for (u32 i = 0; i < GAMEMANAGER->getMaxPlayerCount(); i++)
    {
        if ( !GAMEMANAGER->getPlayer(i)->getActive() )
        {
            InputAdapter::PressStatus buttons[JOY_MAX_BUT];

            INPUT_ADAPTER->getGamePadButtons(InputAdapter::EnvironmentLua,i, buttons, JOY_MAX_BUT);

#ifdef ITF_SUPPORT_CHEAT
            // do not interfere with cheats
            if (buttons[m_joyButton_LB]==InputAdapter::Pressed && buttons[m_joyButton_RB]==InputAdapter::Pressed)
            {
                // do not interfere with the cheats
                continue;
            }
#endif

            // join with Start/A/B/X/Y
            bbool wasButtonPressed = bfalse;
            for (u32 button=0; button<JOY_MAX_BUT; button++)
            {
                if (buttons[button] == InputAdapter::JustPressed && button != m_joyButton_Home)
                {
                    wasButtonPressed = btrue;
                    break;
                }
            }

            if ( wasButtonPressed )
            {
                // http://mdc-tomcat-jira01.ubisoft.org/jira/browse/RO-6313
                if ( RAY_GAMEMANAGER->canOptInOptOut() )
                {
                    RAY_GAMEMANAGER->changePlayerActivation(i,bfalse); //do it at end

                    //scorelum
                    Ray_ScoreLumAIComponent *scoreLum = Ray_AIUtils::fetchScoreLumComponent();
                    if (scoreLum)
                    {
                        scoreLum->playJoinSound(btrue);
                    }
                    //
                    break;
                }
            }
        }
    }
}
    
} // namespace ITF
