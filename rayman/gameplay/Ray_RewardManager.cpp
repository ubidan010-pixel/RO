#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_REWARDMANAGER_H_
#include "rayman/gameplay/Ray_RewardManager.h"
#endif //_ITF_RAY_REWARDMANAGER_H_

#ifdef ITF_WINDOWS
#include "adapters/RewardAdapter_win/RewardAdapter_win.h"
#endif //ITF_WINDOWS

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_ALIASMANAGER_H_
#include "engine/aliasmanager/aliasmanager.h"
#endif //_ITF_ALIASMANAGER_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef ITF_RAYONLINETRACKING_MANAGER_H
#include "Ray_OnlineTrackingManager.h"
#endif


namespace ITF
{
    // List of triggers
    #define s_RewardTrigger_None            ITF_GET_STRINGID_CRC(None,2748391529)
    #define s_RewardTrigger_SkullCoin       ITF_GET_STRINGID_CRC(SkullCoin,820627580)
    #define s_SkullTooth                    ITF_GET_STRINGID_CRC(SkullTooth,3094571089)
    #define s_RewardTrigger_EnnemyKill      ITF_GET_STRINGID_CRC(EnnemyKill,2363318970)
    #define s_RewardTrigger_CrushedEnemy    ITF_GET_STRINGID_CRC(CrushedEnemies,3936105523)
    #define s_RewardTrigger_CrushedEnemySameAttack ITF_GET_STRINGID_CRC(CrushedEnemiesSameAttack,2651288741)
    #define s_SprintDistance                ITF_GET_STRINGID_CRC(SprintDistance,62419951)
    #define s_SwimDistance                  ITF_GET_STRINGID_CRC(SwimDistance,3248660811)
    #define s_FinishLevelNoHit              ITF_GET_STRINGID_CRC(FinishLevelNoHit,4063480714)
    #define s_FinishLevelSprintNotReleased  ITF_GET_STRINGID_CRC(FinishLevelSprintNotReleased,3158634012)
    #define s_SurvivedPiranhaPond           ITF_GET_STRINGID_CRC(SurvivedPiranhaPond,2436711209)
    #define s_BouncedEnemiesSameJump        ITF_GET_STRINGID_CRC(BouncedEnemiesSameJump,1803645200)
    #define s_RescuedRedWizardsOceanA1      ITF_GET_STRINGID_CRC(RescuedRedWizardsOceanA1,3268153304)
    #define s_KungFuCombo                   ITF_GET_STRINGID_CRC(KungFuCombo,426059213)
    #define s_DoingHelico                   ITF_GET_STRINGID_CRC(DoingHelico,3424112305)
    #define s_ReplayACompletedMap           ITF_GET_STRINGID_CRC(ReplayACompletedMap,1188847882)
    #define s_PoppedEnemyBubble             ITF_GET_STRINGID_CRC(PoppedEnemyBubble,1083857607)
    #define s_PoppedEnemyBubbleByTapping    ITF_GET_STRINGID_CRC(PoppedEnemyBubbleByTapping,4172254992)
    #define s_PoppedItemBubbleByTapping     ITF_GET_STRINGID_CRC(PoppedItemBubbleByTapping,571441270)
    #define s_PickedRelic                   ITF_GET_STRINGID_CRC(PickedRelic,3362468156)
    #define s_GoThroughCageMapDoor          ITF_GET_STRINGID_CRC(GoThroughCageMapDoor,1901608063)
    #define s_KillHunterWithOwnBullet       ITF_GET_STRINGID_CRC(KillHunterWithOwnBullet,325747422)
    #define s_ChainBubble                   ITF_GET_STRINGID_CRC(ChainBubble,3186012396)
    #define s_VacuumKnack                   ITF_GET_STRINGID_CRC(VacuumKnack,968902111)
    #define s_ShooterKillBoss               ITF_GET_STRINGID_CRC(ShooterKillBoss,660892036)
    #define s_KillBigMama                   ITF_GET_STRINGID_CRC(KillBigMama,3929792807)
    #define s_BlueBaron                     ITF_GET_STRINGID_CRC(BlueBaron,3459223780)
    #define s_TimeAttackWon                 ITF_GET_STRINGID_CRC(TimeAttackWon,2060670818)
    #define s_MedalCompleted                ITF_GET_STRINGID_CRC(MedalCompleted,738589224)
    #define s_FinishedLevelWithCostume      ITF_GET_STRINGID_CRC(FinishedLevelWithCostume,3874240283)

    #define s_PowerUp_Fight                 ITF_GET_STRINGID_CRC(PowerUp_Fight,1054894714)
    #define s_PowerUp_Helicopter            ITF_GET_STRINGID_CRC(PowerUp_Helicopter,1000445382)
    #define s_PowerUp_Reduction             ITF_GET_STRINGID_CRC(PowerUp_Reduction,726222457)
    #define s_Powerup_Dive                  ITF_GET_STRINGID_CRC(Powerup_Dive,4273977784)
    #define s_PowerUp_WalkOnWallsGlobal     ITF_GET_STRINGID_CRC(PowerUp_WalkOnWallsGlobal,2966471932)


    Ray_RewardManager::~Ray_RewardManager()
    {
        static const Path rewardFile(GETPATH_ALIAS("rewardlist"));
        TEMPLATEDATABASE->removeTemplateClient(rewardFile.getStringID(), this);
    }

    void Ray_RewardManager::init()
    {
        if(!m_isInit)
        {
            initTags();
            m_isInit = btrue;
        }
    }

    void Ray_RewardManager::addTriggerForTags(Tags _tag, const StringID& _id)
    {
        STD_PAIR<Tags, StringID> newTrigger;

        newTrigger.first    = _tag; 
        newTrigger.second   = _id;

        m_tagsTriggersList.insert(newTrigger);
    }


    void Ray_RewardManager::addTriggerForTimers( Timers _timer, const StringID& _id )
    {
        STD_PAIR<Timers, StringID> newTrigger;

        newTrigger.first    = _timer; 
        newTrigger.second   = _id;

        m_timersTriggersList.insert(newTrigger);
    }


    void Ray_RewardManager::initTags()
    {
        REWARD_MANAGER->setTagsCount((u32)(Tags_Count));
        REWARD_MANAGER->setTimersCount((u32) (Timers_Count));
        REWARD_MANAGER->computeRewardManager();

        bbool m_enable = (REWARD_ADAPTER && REWARD_ADAPTER->isEnable()) && loadLUAFile();

        if(m_enable)
        {
            // Tags
            addTriggerForTags(Action_GetSkull, s_RewardTrigger_SkullCoin);
            addTriggerForTags(Action_KillEnnemy, s_RewardTrigger_EnnemyKill);
            addTriggerForTags(Action_GetCrushedEnemy, s_RewardTrigger_CrushedEnemy);
            addTriggerForTags(Action_GetCrushedEnemySameAttack, s_RewardTrigger_CrushedEnemySameAttack);            
            addTriggerForTags(Action_GetSprintDistance, s_SprintDistance);
            addTriggerForTags(Action_GetSwimDistance, s_SwimDistance);
            addTriggerForTags(Action_FinishLevelNoHit, s_FinishLevelNoHit);
            addTriggerForTags(Action_FinishLevelSprintNotReleased, s_FinishLevelSprintNotReleased);
            addTriggerForTags(Action_SurvivedPiranhaPond, s_SurvivedPiranhaPond);
            addTriggerForTags(Action_GetBouncedEnemiesSameJump, s_BouncedEnemiesSameJump);
            addTriggerForTags(Action_GetRescuedRedWizardsOceanA1, s_RescuedRedWizardsOceanA1);
            addTriggerForTags(Action_GetKungFuCombo, s_KungFuCombo);
            addTriggerForTags(Action_ReplayingAMap, s_ReplayACompletedMap);
            addTriggerForTags(Action_PoppedEnemyBubble, s_PoppedEnemyBubble);
            addTriggerForTags(Action_PoppedEnemyBubbleByTapping, s_PoppedEnemyBubbleByTapping);
            addTriggerForTags(Action_PoppedItemBubbleByTapping, s_PoppedItemBubbleByTapping);
            addTriggerForTags(Action_PickedRelic, s_PickedRelic);
            addTriggerForTags(Action_GoThroughCageMapDoor, s_GoThroughCageMapDoor);
            addTriggerForTags(Action_KillHunterWithOwnBullet, s_KillHunterWithOwnBullet);
            addTriggerForTags(Action_VacuumKnack, s_VacuumKnack);
            addTriggerForTags(Action_ShooterKillBoss, s_ShooterKillBoss);
            addTriggerForTags(Action_KillBigMama, s_KillBigMama);
            addTriggerForTags(Action_GetSkullTooth, s_SkullTooth);            
            addTriggerForTags(Action_ChainBubble, s_ChainBubble);
            addTriggerForTags(Action_TimeAttackWon, s_TimeAttackWon);
            addTriggerForTags(Action_MedalCompleted, s_MedalCompleted);
            addTriggerForTags(Action_FinishedLevelWithCostume, s_FinishedLevelWithCostume);
            addTriggerForTags(Action_PowerUp_Fight, s_PowerUp_Fight);
            addTriggerForTags(Action_PowerUp_Helicopter, s_PowerUp_Helicopter);
            addTriggerForTags(Action_PowerUp_Reduction, s_PowerUp_Reduction);
            addTriggerForTags(Action_Powerup_Dive, s_Powerup_Dive);
            addTriggerForTags(Action_PowerUp_WalkOnWallsGlobal, s_PowerUp_WalkOnWallsGlobal);

            // Timers
            addTriggerForTimers(Timer_DoingHelico, s_DoingHelico);
            addTriggerForTimers(Timer_BlueBaron, s_BlueBaron );



#ifdef ITF_WINDOWS
            Ray_RewardContainer_Template::RewardDetailList::const_iterator ite = m_container->getRewards().begin();
            u32 win32Counter = 0;
            for (; ite != m_container->getRewards().end(); ite++ )
            {
                const Ray_RewardDetail*   pDetail = &(*ite);
                if(!pDetail) continue;

                REWARD_ADAPTER_WIN->addReward(pDetail->ID, win32Counter);
                win32Counter++;
            }
#endif //ITF_WINDOWS
        } else
        {
            REWARD_MANAGER->disable();
        }
    }

    bbool Ray_RewardManager::loadLUAFile()
    {
        static const Path rewardFile(GETPATH_ALIAS("rewardlist"));

        TEMPLATEDATABASE->addTemplateClient(rewardFile.getStringID(), this);
        m_container = TEMPLATEDATABASE->getTemplate<Ray_RewardContainer_Template>(this, rewardFile);
        if (!m_container)
        {
            ITF_ASSERT_MSG(0, "Couldn't load reward list: %s", rewardFile.getString8().cStr());
            return bfalse;
        }  
        m_templateIsLoaded = btrue;
        return btrue;
    }

    void Ray_RewardManager::fillRewardManagerFromLoadGame(Ray_PersistentGameData_Universe* gameData)
    {
        if(!REWARD_MANAGER->isEnable()) return;

        REWARD_MANAGER->Reset();

        REWARD_MANAGER->Session_New();
        REWARD_MANAGER->Action_AddPlayer(0);        
        REWARD_MANAGER->Session_Reset();    
        REWARD_MANAGER->Session_Stop(); 

        REWARD_MANAGER->m_updated = false;
    }

    void Ray_RewardManager::hideRewards()
    {

#ifdef ITF_SUPPORT_EDITOR
        m_displayRewardsMenu = bfalse;
#endif
    }
    void Ray_RewardManager::displayRewards()
    {
        if(!REWARD_ADAPTER) return;
        if(!REWARD_MANAGER->isEnable()) return;

#ifdef ITF_SUPPORT_EDITOR
        Ray_RewardContainer_Template::RewardDetailList::const_iterator ite = m_container->getRewards().begin();
        u32 Counter = 0;
        f32 yOffset = 50.0f, xOffset = 50.0f;
        f32 x = 25.0f, y = 25.0f;
        f32 border = 5.0f;
        GFX_ADAPTER->draw2dBox(Vec2d(x-border, y-border), 
            GFX_ADAPTER->getScreenWidth()-x-border, 
            GFX_ADAPTER->getScreenHeight()-y-border-150.0f, 
            0xDD555555, 0xDD555555,0xDD555555, 0xDD555555);

        for (; ite != m_container->getRewards().end(); ite++ )
        {
            const Ray_RewardDetail*   pDetail = &(*ite);
            if(!pDetail) continue;

            bbool isLocked = REWARD_ADAPTER->isLocked(pDetail->ID);

            GFX_ADAPTER->drawDBGText(pDetail->Name.getDebugString(), 
                Color(1.f,1.f,1.f,1.f), xOffset + 15.0f, yOffset);
            GFX_ADAPTER->drawDBGText(isLocked ? "LOCKED" : "UNLOCKED", 
                Color(1.f,isLocked? 1.f:0.f,isLocked? 0.f:1.f,0.f), xOffset + 270.0f, yOffset);

            yOffset += 16.0f;
            Counter++;

            if(Counter%20==0){xOffset+= 350.0f;yOffset=50.0f;}
        }
        m_displayRewardsMenu = btrue;
#else //ITF_SUPPORT_EDITOR
        REWARD_ADAPTER->displayRewards();
#endif //!ITF_SUPPORT_EDITOR
    }

    void Ray_RewardManager::update()
    {
        if(!REWARD_ADAPTER) return;
        if(!REWARD_MANAGER->isEnable()) return;

        REWARD_MANAGER->update();

#ifdef ITF_SUPPORT_EDITOR
        if(m_displayNotification)
        {
            Ray_RewardContainer_Template::RewardDetailList::const_iterator ite_win32 =
                m_container->getRewards().begin();
            f32 yOffset = 16.0f;
            for (; ite_win32 != m_container->getRewards().end(); ite_win32++ )
            {
                const Ray_RewardDetail*   pDetail = &(*ite_win32);
                RewardToDisplay* pToDisplay = NULL;
                for(u32 i = 0; i < m_toDisplay.size(); i ++)
                {
                    if(m_toDisplay[i].ID == pDetail->ID)
                    {
                        if(m_toDisplay[i].StartTime + 8.0 > SYSTEM_ADAPTER->getTime())
                            pToDisplay = &m_toDisplay[i];
                        else
                        {
                            m_toDisplay.eraseKeepOrder(i);
                            break;
                        }
                    }
                }
                if(!pToDisplay) continue;
                String8 str ("") ;
                str.setTextFormat("[Reward] \"%s\" unlocked !", pDetail->Name.getDebugString());
                GFX_ADAPTER->drawDBGText(str.cStr(), Color(1.f,1.f,1.f,1.f), 5.0f, yOffset);
                yOffset += 16.0f;
            }
            if(yOffset<=16.0f) m_displayNotification = bfalse;
        }   

        if(m_displayRewardsMenu)
        {
            displayRewards();
        }
#endif //ITF_SUPPORT_EDITOR

        if(!REWARD_MANAGER->m_updated) 
            return;

        if(!REWARD_ADAPTER)
            return;

        if(GAMEMANAGER->getUsePressConfMenu())
            return;

        if ( (!SAVEGAME_ADAPTER->IsSaveSystemEnable() && REWARD_ADAPTER->needsSaveSystemToWork()) )
            return;

        if(GAMEMANAGER->getMainIndexPlayer_Internal()==U32_INVALID)
            return;

        Ray_RewardContainer_Template::RewardDetailList::const_iterator ite = m_container->getRewards().begin();
        bbool   wantToSaveGame = bfalse;
        for (; ite != m_container->getRewards().end(); ite++ )
        {
            const Ray_RewardDetail*   pDetail = &(*ite);
            if(!pDetail) continue;

            RewardID        currentID = pDetail->ID;
            if(!REWARD_ADAPTER->isLocked(currentID))
                continue;

            bbool hasToUnlockReward = btrue;
            u32 TriggerCount = pDetail->Triggers.size();
            for(u32 trigger = 0; trigger < TriggerCount && hasToUnlockReward; trigger++)
            {
                Ray_RewardTrigger_Base* pTrigger = pDetail->Triggers[trigger];
                hasToUnlockReward = (pTrigger->check_reward() && hasToUnlockReward);
            }

            if(hasToUnlockReward)
            {
                // Don't break here, because reward manager can list several reward to unlock
                if (REWARD_ADAPTER->unlock(currentID))
                {


#ifdef ITF_SUPPORT_ONLINETRACKING
                    RAY_GAMEMANAGER->getOnlineTrackingManager()->AwardUnlock(REWARD_ADAPTER->m_rewardMap[currentID]);
#endif

#ifdef ITF_SUPPORT_EDITOR
                    if ( !m_container->m_isSilent )
                    {
                        m_displayNotification = btrue;
                        RewardToDisplay toDisplay = { currentID, SYSTEM_ADAPTER->getTime() };
                        m_toDisplay.push_back(toDisplay);
                    }                
#endif //ITF_SUPPORT_EDITOR
                    wantToSaveGame = btrue;
                }
            }
        }

        if(wantToSaveGame && RAY_GAMEMANAGER->getDebugSaves()) // DebugSave is temporary here ..
        {
            // Try to save the game 
            // RAY_GAMEMANAGER->saveGameState(bfalse);
        }

        // Update is done
        REWARD_MANAGER->postUpdate();
    }

    const StringID & Ray_RewardManager::getTriggerFromTag( Ray_RewardManager::Tags& _tag )
    {
        TagsTriggersList::const_iterator ite = m_tagsTriggersList.find(_tag);
        if (ite == m_tagsTriggersList.end())
            return StringID::Invalid;
        return (ite->second);
    }

    Ray_RewardManager::Tags Ray_RewardManager::getTagFromTrigger( const StringID & _trigger )
    {
        TagsTriggersList::const_iterator ite = m_tagsTriggersList.begin();
        for ( ; ite != m_tagsTriggersList.end(); ++ite)
        {
            if ( ite->second == _trigger ) 
                return ite->first; 
        }

        return Ray_RewardManager::Action_Unknown;
    }

    Ray_RewardManager::Timers Ray_RewardManager::getTimerFromTrigger( const StringID & _trigger )
    {
        TimersTriggersList::const_iterator ite = m_timersTriggersList.begin();
        for ( ; ite != m_timersTriggersList.end(); ++ite)
        {
            if ( ite->second == _trigger ) 
                return ite->first; 
        }

        return Ray_RewardManager::Timer_Unknown;
    }

    bbool Ray_RewardManager::isRewardsListDisplayed() const
    {
#ifdef ITF_WINDOWS
        return m_displayRewardsMenu;
#else //ITF_WINDOWS
        return bfalse;
#endif
    }


    // ---------------------------------------------------------------
    // RewardTrigger -------------------------------------------------

    f32 Ray_RewardTrigger_Base::GetTotalAmountOfVarType(StringID var, u32 player)
    {
        ITF_ASSERT(var != s_RewardTrigger_None);
        const Ray_RewardManager::Tags  tag = RAY_REWARD_MANAGER->getTagFromTrigger(var);
        if(tag == Ray_RewardManager::Action_Unknown)
            return -1.0f;
        if(!REWARD_MANAGER->IsTagValueUpdated(REWARD_MANAGER->Session_GetTotal(), tag, player))
            return -1.0f;

        return REWARD_MANAGER->GetTotalOf(tag, player);
    }

    f32 Ray_RewardTrigger_Base::GetTotalAmountOfVarTypeForCurrentSession(StringID var, u32 player /* = 0 */)
    {
        ITF_ASSERT(var != s_RewardTrigger_None);
        const Ray_RewardManager::Tags  tag = RAY_REWARD_MANAGER->getTagFromTrigger(var);
        if(tag == Ray_RewardManager::Action_Unknown)
            return -1.0f;
        if(!REWARD_MANAGER->IsTagValueUpdated(REWARD_MANAGER->Session_GetCurrent(), tag, player))
            return -1.0f;

        return REWARD_MANAGER->GetTotalOf(tag, player, bfalse);
    }

    bbool Ray_RewardTrigger_Sum::check_reward()
    {
        u32 player = GAMEMANAGER->getMainIndexPlayer();
        f32 currentAmount = m_useCurrentOnly ?
            GetTotalAmountOfVarTypeForCurrentSession(m_varTypeToSum, player) :
        GetTotalAmountOfVarType(m_varTypeToSum, player);

        if ( m_strictlyEqual )
            return (currentAmount == m_amountToGet);
        else
            return (currentAmount >= m_amountToGet);
    }

    bbool Ray_RewardTrigger_SumWithTimer::check_reward()
    {
        if(!m_validTime)
        {
            m_validTime = (f32*)Memory::malloc(m_amountToGet*sizeof(f32));
            ITF_MemSet(m_validTime, 0, m_amountToGet*sizeof(f32));
        }


        u32 currentTotal = static_cast<u32>(GetTotalAmountOfVarTypeForCurrentSession(m_varTypeToSum, GAMEMANAGER->getMainIndexPlayer()));
        if(currentTotal==U32_INVALID)return bfalse;
        if(currentTotal != m_lastAmount)
        {
            if(currentTotal<m_lastAmount)
            {
                m_lastAmount = currentTotal;
                return bfalse;
            }

            if(currentTotal-m_lastAmount>=m_amountToGet) return btrue;
            f32 currentTime = (f32)SYSTEM_ADAPTER->getTime();
            for (u32 i = m_lastAmount+1; i <= currentTotal; i++)
            {
                if(m_validTime[i%m_amountToGet] + m_time >= currentTime
                    && m_time < currentTime)
                    return btrue;

                m_validTime[i%m_amountToGet] = currentTime;
            }
            m_lastAmount = currentTotal;
        }

        return bfalse;
    }


    bbool Ray_RewardTrigger_FinishLevel::check_reward()
    {
        if ( m_WorldTag.isValid() )
        {
            bbool isTriggered = REWARD_MANAGER->IsGlobalEventTriggered(Ray_RewardManager::Event_CompleteWorld);
            if(isTriggered &&  (m_WorldTag == REWARD_MANAGER->GetGlobalEventID(Ray_RewardManager::Event_CompleteWorld) ))
            {
                return btrue;
            }
            return bfalse;
        }

        return REWARD_MANAGER->IsGlobalEventTriggered(Ray_RewardManager::Event_EndOfLevel);
    }

    bbool Ray_RewardTrigger_Event::check_reward()
    {
        RewardManager::Tag tagEvent = RAY_REWARD_MANAGER->getTagFromTrigger(m_Event);
        return REWARD_MANAGER->IsGlobalEventTriggered(tagEvent);
    }

    bbool Ray_RewardTrigger_Timer::check_reward()
    {
        const Ray_RewardManager::Timers  timer = RAY_REWARD_MANAGER->getTimerFromTrigger(m_timer);
        if(timer == Ray_RewardManager::Timer_Unknown)
            return bfalse;

        RewardManager::GameSession* pGameSession = NULL; 
        if(m_useCurrentOnly) pGameSession = REWARD_MANAGER->Session_GetCurrent();
        else 
            pGameSession = REWARD_MANAGER->Session_GetTotal();

        if(!REWARD_MANAGER->IsTimerUpdated(pGameSession, timer, GAMEMANAGER->getMainIndexPlayer()))
        {
            return bfalse;
        }

        if ( m_reachTimeToGet )
        {
            return (f32) REWARD_MANAGER->GetTimeElapsedTimer(pGameSession, timer, GAMEMANAGER->getMainIndexPlayer())  >= m_timeToGet;
        }

        return (f32) REWARD_MANAGER->GetTimeElapsedTimer(pGameSession, timer, GAMEMANAGER->getMainIndexPlayer())  < m_timeToGet;
    }

    Ray_RewardDetail::~Ray_RewardDetail()
    {
        ITF_VECTOR<Ray_RewardTrigger_Base*>::iterator it = Triggers.begin(); 
        for (; it != Triggers.end(); ++it)
            if(*it) delete (*it);
        Triggers.clear();
    }

    IMPLEMENT_OBJECT_RTTI(Ray_RewardTrigger_Base)

        IMPLEMENT_OBJECT_RTTI(Ray_RewardTrigger_Sum)
        BEGIN_SERIALIZATION(Ray_RewardTrigger_Sum)
        SERIALIZE_MEMBER("typeToGet", m_varTypeToSum);
    SERIALIZE_MEMBER("amountToGet", m_amountToGet);
    SERIALIZE_MEMBER("currentSessionOnly", m_useCurrentOnly);
    SERIALIZE_MEMBER("strictlyEqual", m_strictlyEqual);
    END_SERIALIZATION()

        IMPLEMENT_OBJECT_RTTI(Ray_RewardTrigger_SumWithTimer)
        BEGIN_SERIALIZATION_CHILD(Ray_RewardTrigger_SumWithTimer)
        SERIALIZE_MEMBER("timeToGet", m_time);
    END_SERIALIZATION()

        IMPLEMENT_OBJECT_RTTI(Ray_RewardTrigger_FinishLevel)
        BEGIN_SERIALIZATION(Ray_RewardTrigger_FinishLevel)
        SERIALIZE_MEMBER("worldTag", m_WorldTag);
    END_SERIALIZATION()

        IMPLEMENT_OBJECT_RTTI(Ray_RewardTrigger_Event)
        BEGIN_SERIALIZATION(Ray_RewardTrigger_Event)
        SERIALIZE_MEMBER("eventID", m_Event);
    END_SERIALIZATION()


        IMPLEMENT_OBJECT_RTTI(Ray_RewardTrigger_Timer)
        BEGIN_SERIALIZATION(Ray_RewardTrigger_Timer)
        SERIALIZE_MEMBER("timer", m_timer);
    SERIALIZE_MEMBER("timeToGet", m_timeToGet);
    SERIALIZE_MEMBER("currentSessionOnly", m_useCurrentOnly);
    SERIALIZE_MEMBER("reachTimeToGet", m_reachTimeToGet);
    END_SERIALIZATION()

        BEGIN_SERIALIZATION(Ray_RewardDetail)           
        SERIALIZE_MEMBER("id",ID);
    SERIALIZE_MEMBER("name",Name);
    SERIALIZE_CONTAINER_WITH_FACTORY("REWARD_TRIGGER", Triggers, RAY_REWARD_MANAGER->getTriggerFactory()/*REWARD_MANAGER->getTriggerFactory()*/);
    END_SERIALIZATION()

        IMPLEMENT_OBJECT_RTTI(Ray_RewardContainer_Template)
        BEGIN_SERIALIZATION(Ray_RewardContainer_Template)
        SERIALIZE_CONTAINER_OBJECT("rewards",m_rewards);
    SERIALIZE_MEMBER("isSilent", m_isSilent);        
    END_SERIALIZATION()

} // namespace ITF
