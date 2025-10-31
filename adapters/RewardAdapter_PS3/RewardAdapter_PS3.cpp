#include "precompiled_RewardAdapter_PS3.h"

#ifndef _ITF_REWARDADAPTER_PS3_H__
#include "adapters/RewardAdapter_PS3/RewardAdapter_PS3.h"
#endif //_ITF_REWARDADAPTER_PS3_H__

#ifndef _ITF_NETWORKSERVICES_PS3_H_
#include "engine/networkservices/PS3/NetworkServices_PS3.h"
#endif //_ITF_NETWORKSERVICES_PS3_H_

#ifndef _ITF_TASKMANAGER__H_
#include "engine/taskmanager/TaskManager.h"
#endif //_ITF_TASKMANAGER__H_

#ifndef _ITF_TRCADAPTER_PS3_H_
#include "adapters/TRCAdapter_PS3/TRCAdapter_PS3.h"
#endif //_ITF_TRCADAPTER_PS3_H_

#ifndef _ITF_SYSTEMADAPTER_PS3_H_
#include "adapters/SystemAdapter_PS3/SystemAdapter_PS3.h"
#endif //_ITF_SYSTEMADAPTER_PS3_H_

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

namespace ITF
{
#ifndef ITF_FINAL
    //#define REWARD_PS3_READONLY
#endif

    RewardAdapter_PS3::RewardAdapter_PS3() :
        m_contextRegistered(bfalse), m_firstInstall(bfalse), m_context(SCE_NP_TROPHY_INVALID_CONTEXT),
            m_handle(SCE_NP_TROPHY_INVALID_HANDLE), m_trophyCount(0), m_taskManager(NULL), m_initState(Idle)
            , RewardAdapter()
    {
    }

    void RewardAdapter_PS3::init(bbool enable)
    {
        RewardAdapter::init(enable);
        if(m_enable)
        {
            if(!m_taskManager)
                m_taskManager = newAlloc(mId_System, TaskManager());
        }
    }
    void RewardAdapter_PS3::update()
    {
        RewardAdapter::update();
        if(m_enable)
        {
            if(m_taskManager)
                m_taskManager->update();
        }
    }

#define CHECK_INIT(func) \
    if ((ret = func()) < 0) { \
        m_enable = bfalse; change_InitState(Aborted); \
        LOG("Reward initialisation failed: 0x%x", ret); \
        if(m_initState <= CreatingHandle) \
            init_checkRewardsSize(btrue); \
        return 0; \
    }


    int RewardAdapter_PS3::initForBoot()
    {
        if(m_initState != Idle) return 0;
#ifndef ITF_FINAL
        f64 startTimer = SYSTEM_ADAPTER->getTime();
#endif //ITF_FINAL

        int ret ;
        SCE_NP_TROPHY_FLAG_ZERO(&m_trophyUnlockStates);
        
        change_InitState(InitializingLib);        
        CHECK_INIT(init_lib);

        change_InitState(CreatingContext);
        CHECK_INIT(init_context);

        change_InitState(CreatingHandle);
        CHECK_INIT(init_handle);

        if(!init_checkRewardsSize())
        {
            m_enable = bfalse;
            change_InitState(Aborted);
#ifndef ITF_FINAL        
            LOG("Rewards aborted in %04f seconds*************", SYSTEM_ADAPTER->getTime()-startTimer);
#endif //ITF_FINAL
            return 0;
        }
        
        change_InitState(RegisteringContext);
        CHECK_INIT(register_context);

        change_InitState(InitializingTrophyStates);
        CHECK_INIT(init_trophyStates);
                
        init_rewardList();       
        change_InitState(Initialized);

#ifndef ITF_FINAL        
        LOG("Rewards initialized in %04f seconds*************", SYSTEM_ADAPTER->getTime()-startTimer);
#endif //ITF_FINAL
        return 1;
    }

    bbool RewardAdapter_PS3::init_checkRewardsSize(bbool forceFakeCheck)
    {
        u32 requiredDiskSpaceKB = 0;
        bbool forceFalseReturns = bfalse;
        if ( !forceFakeCheck )
        {
            u64 requiredDiskSpaceBytes = 0;
            int ret = sceNpTrophyGetRequiredDiskSpace(m_context, m_handle, &requiredDiskSpaceBytes, 0);
            if(ret == 0)
            {
                requiredDiskSpaceKB = TRCTools::Bytes2KBytes((u32)requiredDiskSpaceBytes);
            } else
            {
                // Let it return the checkSpaceDiskRes with 0 as argument
                LOG("CheckRewards Size failed: 0x%x (file not found?)", ret);                
                forceFalseReturns = btrue;
            }
        } else 
            forceFalseReturns = btrue;

        bbool res = (TRC_ADAPTER->checkSpaceDisk(TRCManagerAdapter::Rwd_CheckSpaceBoot, requiredDiskSpaceKB));
        LOG("[SPACEDISK] Reward system needs %d KB", requiredDiskSpaceKB);

        if ( forceFalseReturns ) return bfalse; 
        return res ;
    }

    void RewardAdapter_PS3::init_rewardList()
    {      
        addReward(s_REWARD_RAY_BETILLA_BACK, 1);
        addReward(s_REWARD_RAY_BEAUTIFUL_BEATS, 2);
        addReward(s_REWARD_RAY_FOODIE_FAIRY, 3);
        addReward(s_REWARD_RAY_MERMAID, 4);
        addReward(s_REWARD_RAY_NYMPHS_ROCK, 5);
        addReward(s_REWARD_RAY_BIG_MAMA, 6);
        addReward(s_REWARD_RAY_SCOUT, 7);
        addReward(s_REWARD_RAY_Explorer, 8);
        addReward(s_REWARD_RAY_DR_LIVIDSTONE, 9);
        addReward(s_REWARD_RAY_SPEEDY, 10);
        addReward(s_REWARD_RAY_TURBO, 11);
        addReward(s_REWARD_RAY_NITRO, 12);
        addReward(s_REWARD_RAY_ELEC_FRIEND, 13);
        addReward(s_REWARD_RAY_ELEC_HERO, 14);
        addReward(s_REWARD_RAY_ELEC_LEGEND, 15);
        addReward(s_REWARD_RAY_MILK_MOUTH, 16);
        addReward(s_REWARD_RAY_FULL_MOUTH, 17);
        addReward(s_REWARD_RAY_THE_JAW, 18);
        addReward(s_REWARD_RAY_POP_POP_BOOM, 19);
        addReward(s_REWARD_RAY_BUBBLIZER, 20);
        addReward(s_REWARD_RAY_IAM_BACK, 21);
        addReward(s_REWARD_RAY_PAINLESS, 22);
        addReward(s_REWARD_RAY_VACUUM_KNACK, 23);
        addReward(s_REWARD_RAY_NOPANIC, 24);
        addReward(s_REWARD_RAY_FINISH_WITH_COSTUME, 25);
        addReward(s_REWARD_RAY_HOVER_HAPPY, 26);
        addReward(s_REWARD_RAY_CRUSHER, 27);
        //addReward(s_REWARD_RAY_PEACEFUL_DREAMS, 28); --> removed
        addReward(s_REWARD_RAY_CRUSH_COMBO, 28);
        addReward(s_REWARD_RAY_SPRINTER, 29);
        addReward(s_REWARD_RAY_FISHER_KING, 30);   
        addReward(s_REWARD_RAY_KUNGFUCOMBO, 31);
        addReward(s_REWARD_RAY_HYPERSPEED, 32);        
        addReward(s_REWARD_RAY_BOING, 33);
        addReward(s_REWARD_RAY_BLUE_BARON, 34);
        addReward(s_REWARD_RAY_SURVIVEDPIRANHAPOND, 35);
        addReward(s_REWARD_RAY_BACK_AT_YOU, 36);
    }

    int RewardAdapter_PS3::init_lib()
    {
        return sceNpTrophyInit(NULL, 0, SYS_MEMORY_CONTAINER_ID_INVALID, 0);
    }

    int RewardAdapter_PS3::init_handle()
    {        
        return sceNpTrophyCreateHandle(&m_handle);
    }

    int RewardAdapter_PS3::init_context()
    {        
        uint64_t contextFlag = 0;
#ifdef REWARD_PS3_READONLY
        contextFlag = SCE_NP_TROPHY_OPTIONS_CREATE_CONTEXT_READ_ONLY;
#endif        
        //we are on ps3 ,we can cast safety
        u32 networkproduct = ((SystemAdapter_PS3*)(SYSTEM_ADAPTER))->getNetworkServiceProduct();
        return sceNpTrophyCreateContext(&m_context
            , &NetworkServices_PS3::s_communication_id[networkproduct]
            , &NetworkServices_PS3::s_communication_sig[networkproduct]
            , contextFlag
            );
    }

    int RewardAdapter_PS3::register_context()
    {      
        return sceNpTrophyRegisterContext(m_context
            , m_handle, RewardAdapter_PS3::s_StatusCallback, this, 
            SCE_NP_TROPHY_OPTIONS_REGISTER_CONTEXT_SHOW_ERROR_EXIT);
    }

    int RewardAdapter_PS3::init_trophyStates()
    {
        return sceNpTrophyGetTrophyUnlockState(m_context, m_handle, &m_trophyUnlockStates, &m_trophyCount);
    }

    int RewardAdapter_PS3::s_StatusCallback(SceNpTrophyContext context, SceNpTrophyStatus status, int completed, int total, void *arg)
    {
        RewardAdapter_PS3*  pRewardPS3 = (RewardAdapter_PS3*) arg;
        int callbackReturnValue = 0;
        
        if(status == SCE_NP_TROPHY_STATUS_NOT_INSTALLED)
        {
            pRewardPS3->setIsFirstInstall();
        }
        
        if(pRewardPS3->isCallbackFirstCall())
        {
            pRewardPS3->setContextCallbackCalledOnce();
            if(status == SCE_NP_TROPHY_STATUS_UNKNOWN)
                callbackReturnValue = -1;  // will abort the context registering
        }
        
        return callbackReturnValue;
    }

    bbool RewardAdapter_PS3::unlock (RewardID _id)
    {        
        if(!isInitialized()) return bfalse;

        if ( std::find(m_unlockingRewardID.begin(), m_unlockingRewardID.end(), _id) ==
            m_unlockingRewardID.end() )
        {
            UnlockingData*   data = (UnlockingData*)Memory::malloc(sizeof(UnlockingData));
            data->rewardIDToUnlock = _id;
            data->pAdapter = this;

            m_unlockingRewardID.push_back(_id);

            m_taskManager->pushTask(unlock_task, unlock_taskCallback, (void*)data);
            return btrue; 
        }
        return bfalse;   
    }

    void RewardAdapter_PS3::unlock_taskCallback(void* pParam, i32 _res)
    {
        UnlockingData*  pDatas = (UnlockingData*) pParam;
        RewardAdapter_PS3* pReward = pDatas->pAdapter;

        UnlockingRewardID::iterator it =std::find(pReward->m_unlockingRewardID.begin(), pReward->m_unlockingRewardID.end(), 
            pDatas->rewardIDToUnlock);

        ITF_ASSERT( it != pReward->m_unlockingRewardID.end() );

        pReward->m_unlockingRewardID.erase(it);
        Memory::free(pDatas);
    }

    int RewardAdapter_PS3::unlock_task(void* pParam)
    {
        UnlockingData*  pDatas = (UnlockingData*) pParam;
        RewardAdapter_PS3* pReward = pDatas->pAdapter;

        SceNpTrophyId trophyID = pReward->getRewardPlateform(pDatas->rewardIDToUnlock);
        ITF_ASSERT(trophyID != U32_INVALID);

        if(trophyID != U32_INVALID)
        {
            SceNpTrophyId platiniumID = SCE_NP_TROPHY_INVALID_TROPHY_ID;

            int NpRes = sceNpTrophyUnlockTrophy (pReward->m_context, pReward->m_handle, trophyID, &platiniumID);
            if (NpRes != CELL_OK)
            {
                return NpRes;
            } else
            {
                // Handle the platinium case !
                if (platiniumID != SCE_NP_TROPHY_INVALID_TROPHY_ID)
                {
                    pReward->m_platiniumID = platiniumID;
                    SCE_NP_TROPHY_FLAG_SET(platiniumID, &pReward->m_trophyUnlockStates);
                }

                SCE_NP_TROPHY_FLAG_SET(trophyID, &pReward->m_trophyUnlockStates);
                return CELL_OK;
            }
        }
        return -1;
    }
   

    bbool RewardAdapter_PS3::isLocked (RewardID _id) const
    {
        if(!isInitialized())return btrue;

        SceNpTrophyId   trophyID = getRewardPlateform(_id);
        if(trophyID != U32_INVALID)
        {
            return (SCE_NP_TROPHY_FLAG_ISSET(trophyID,&m_trophyUnlockStates) == 0);
        }
        ITF_ASSERT(0);
        return bfalse;
    }

    void RewardAdapter_PS3::term()
    {
        if(m_taskManager)
        {
            if(m_handle!=SCE_NP_TROPHY_INVALID_HANDLE)
            {
                // Abort current Handle to cancel the unlock action
                sceNpTrophyAbortHandle(m_handle);
            }

            SF_DEL(m_taskManager);
            m_taskManager = NULL;
        }

        if(m_handle!=SCE_NP_TROPHY_INVALID_HANDLE)
        {
            sceNpTrophyDestroyHandle(m_handle);
        }
        if(m_context!=SCE_NP_TROPHY_INVALID_CONTEXT)
        {
            sceNpTrophyDestroyContext(m_context);
        }
                
        sceNpTrophyTerm();
    }

    void RewardAdapter_PS3::change_InitState( initState newState )
    {
        m_initState = newState;

        if(newState==Aborted)
        {
            term();
            m_enable = bfalse;
        }
    }

}
