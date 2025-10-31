#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_CHEATMANAGER_H_
#include "rayman/gameplay/Managers/Ray_CheatManager.h"
#endif //_ITF_RAY_CHEATMANAGER_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_COMMANDARGS_H_
#include "engine/common/CommandArgs.h"
#endif // _ITF_COMMANDARGS_H_

namespace ITF
{
    Ray_CheatManager::Ray_CheatManager()
    : m_allPlayersTogether(bfalse)
    , m_unlockAllCostumes(bfalse)
    , m_teethSequence(bfalse)
    , m_timeAttackEnabled(bfalse)
    , m_allWorldMap(bfalse)
    , m_allSkullTeeth(bfalse)
    , m_activateDebugFresco(bfalse)//CASA::VITA::TARIK::FRESCO GAMEPLAY
    {
    }

    Ray_CheatManager::~Ray_CheatManager()
    {
    }

    void Ray_CheatManager::init( const CommandArgs& _args )
    {
        CheatManager::init(_args);

        String szArg;

        if (_args.find("cheatAllPlayersTogether", szArg))
        {
            m_allPlayersTogether = (szArg.atoi32() != 0) ? btrue : bfalse;
        }
    }

    void Ray_CheatManager::update(f32 _dt)
    {
        CheatManager::update(_dt);
#ifdef ITF_SUPPORT_CHEAT

        //CASA::VITA::TARIK::FRESCO GAMEPLAY ---BEG
        InputAdapter::PressStatus buttons[JOY_MAX_BUT];
        f32 axes[JOY_MAX_AXES];
        INPUT_ADAPTER->getGamePadButtons(InputAdapter::EnvironmentEngine, 0, buttons, JOY_MAX_BUT);
        INPUT_ADAPTER->getGamePadPos(InputAdapter::EnvironmentEngine, 0, axes, JOY_MAX_AXES);

        if (axes[m_joyTrigger_Right] > 0.f &&
            axes[m_joyTrigger_Left] > 0.f &&
            buttons[m_joyButton_Y] == InputAdapter::JustPressed)
        {
            m_activateDebugFresco = !m_activateDebugFresco;
            if (!m_activateDebugFresco)
            {
                RAY_GAMEMANAGER->commitLevelStateToPersistentUniverse();
            }
        }

        if (!m_activateDebugFresco &&
            axes[m_joyTrigger_Right] > 0.f &&
            axes[m_joyTrigger_Left] > 0.f &&
            buttons[m_joyButton_X] == InputAdapter::Pressed)
        {

            if (buttons[m_joyButton_DPadL] == InputAdapter::JustPressed)
            {
                LOG("\n\n Ray_CheatManager-------Generate 2 Duplicate From Near For All Relics to test sending relic gift menu----------R + L + X + PAD_Left");
                RAY_GAMEMANAGER->setAllRelicsDuplicateCount(2);//Gift : duplicate to send
                RAY_GAMEMANAGER->saveGameState(bfalse);
                if (RAY_GAMEMANAGER->getPersistentGameData())
                {
                    RAY_GAMEMANAGER->getPersistentGameData()->DebugFrescoData();
                    RAY_GAMEMANAGER->getPersistentGameData()->DebugRelicsDuplicateCount();
                }
            }
            //else if (buttons[m_joyButton_DPadU] == InputAdapter::JustPressed)
            //{
            //    LOG("\n\n Ray_CheatManager-------Unlock All Relics From Near----------R + L + X + PAD_Up");
            //    RAY_GAMEMANAGER->UnlockAllRelics(RELIC_TYPE_NEAR);
            //    RAY_GAMEMANAGER->saveGameState(bfalse);

            //    if (RAY_GAMEMANAGER->getPersistentGameData())
            //    {
            //        RAY_GAMEMANAGER->getPersistentGameData()->DebugFrescoData();
            //        RAY_GAMEMANAGER->getPersistentGameData()->DebugRelicsDuplicateCount();
            //    }
            //}
            else if (buttons[m_joyButton_DPadD] == InputAdapter::JustPressed)
            {
                LOG("\n\n Ray_CheatManager-------Log Fresco state----------R + L + X + PAD_Down");
                if (RAY_GAMEMANAGER->getPersistentGameData())
                {
                    RAY_GAMEMANAGER->getPersistentGameData()->DebugFrescoData();
                    RAY_GAMEMANAGER->getPersistentGameData()->DebugRelicsDuplicateCount();
                }
            }
        }
        //CASA::VITA::TARIK::FRESCO GAMEPLAY ---END

        // Special E3
#ifndef ITF_DEMO
        if (!getActive())
        {
            return;
        }
#endif

#endif //ITF_SUPPORT_CHEAT
    }

    void Ray_CheatManager::teleportToCheckpoint( GameManager::teleportTarget _target )
    {
#ifdef ITF_CONSOLE

//###E3HACK
#ifndef ITF_DEMO
        CheatManager::teleportToCheckpoint(_target);
#endif // !ITF_DEMO

#else
        switch(_target)
        {
        case GameManager::NextCheckPoint:
            RAY_GAMEMANAGER->triggerTeleportCheatSequence(_target);
            break;

        case GameManager::PrevCheckPoint:
            RAY_GAMEMANAGER->triggerTeleportCheatSequence(_target);
            break;

        case GameManager::FirstCheckPoint:
            RAY_GAMEMANAGER->triggerTeleportCheatSequence(_target);
            break;

        case GameManager::LastCheckPoint:
            RAY_GAMEMANAGER->triggerTeleportCheatSequence(_target);
            break;

        default:
            ITF_ASSERT(0);
            break;
        }
#endif 
    }

    void Ray_CheatManager::forceAllPlayersTogether( bbool _val )
    {
        if ( m_allPlayersTogether != _val )
        {
            m_allPlayersTogether = _val;
        }
    }


    void Ray_CheatManager::onReceive( u32 deviceID /* player */, f32 axis, const StringID& action )
    {
#ifdef ITF_SUPPORT_CHEAT
        if(action==ITF_GET_STRINGID_CRC(CHEAT_JUSTBEDEAD, 651860516))
        {        
            Player* player = GAMEMANAGER->getPlayer(deviceID);
            if(player)
                player->toggleDeadMode();
        }
        else if(action==ITF_GET_STRINGID_CRC(CHEAT_INVINCIBLE, 1288607354))
        {
            Ray_Player* player = static_cast<Ray_Player*>(RAY_GAMEMANAGER->getPlayer(deviceID));
            if(player) 
                player->setInvincible(!player->getInvincible());
        }
        else if(action==ITF_GET_STRINGID_CRC(CHEAT_ALLTOGETHER, 2538788027))
        {
            forceAllPlayersTogether(!m_allPlayersTogether);
        }
#endif //ITF_SUPPORT_CHEAT
    }

}
