#ifdef ITF_PS3

#ifndef _ITF_REWARDADAPTER_PS3_H__
#define _ITF_REWARDADAPTER_PS3_H__

#ifndef _ITF_REWARDADAPTER_H_
#include "engine/AdaptersInterfaces/RewardAdapter.h"
#endif //_ITF_REWARDADAPTER_H_

#ifndef _ITF_SAFEARRAY_H_
#include "core/container/SafeArray.h"
#endif //_ITF_SAFEARRAY_H_


#include <np.h>


namespace ITF
{
    class TaskManager;
    class RewardAdapter_PS3 : public RewardAdapter
    {
    public:
        RewardAdapter_PS3();
        virtual ~RewardAdapter_PS3() {}

        void init(bbool enable);

        int         initForBoot();
        void        update();
        void        term();

        u32         getRewardsCount() const { return 0; }
        bbool       unlock (RewardID _id);
        
        bbool       isLocked (RewardID _id) const;

        bbool       isCallbackFirstCall() const { return m_contextCallbackFirstCall; }

        void        setContextCallbackCalledOnce() { m_contextCallbackFirstCall = bfalse; }
        void        setContextIsRegistered() { m_contextRegistered = btrue; }
        void        setIsFirstInstall() { m_firstInstall = btrue; }
        static int s_StatusCallback (SceNpTrophyContext context, SceNpTrophyStatus status, int completed, int total, void *arg);


    private:
        bbool                   m_contextRegistered;
        bbool                   m_firstInstall;
        bbool                   m_contextCallbackFirstCall;

        SceNpTrophyContext      m_context;        
        SceNpTrophyHandle       m_handle;
        SceNpTrophyFlagArray    m_trophyUnlockStates;

        SceNpTrophyId           m_platiniumID;
        u32                     m_trophyCount;

        // all init task
        int         init_lib();
        int         init_context();
        int         init_handle();
        bbool       init_checkRewardsSize(bbool forceFakeCheck = bfalse);
        int         register_context();
        int         init_trophyStates();
        void        init_rewardList();
        
        enum initState
        {
            Idle=0,
            InitializingLib, 
            CreatingContext, 
            CreatingHandle,
            RegisteringContext,
            InitializingTrophyStates,
            Initialized,
            Aborted
        };
        initState       m_initState;
        void            change_InitState (initState newState);
        ITF_INLINE bbool  isInitialized() const { return m_initState==Initialized; }

        struct UnlockingData
        {
            RewardID            rewardIDToUnlock;
            RewardAdapter_PS3*  pAdapter;
        };

        static int      unlock_task(void* pParam);
        static void     unlock_taskCallback(void* pParam, i32 _res);
        TaskManager *   m_taskManager;

        typedef ITF_VECTOR<RewardID> UnlockingRewardID;
        UnlockingRewardID m_unlockingRewardID;
    };
#define REWARD_ADAPTER_PS3             (static_cast< RewardAdapter_PS3* > (REWARD_ADAPTER))

}

#endif // _ITF_REWARDADAPTER_PS3_H__

#endif