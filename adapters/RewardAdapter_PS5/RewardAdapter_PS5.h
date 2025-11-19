#ifdef ITF_PS5

#ifndef _ITF_REWARDADAPTER_PS5_H__
#define _ITF_REWARDADAPTER_PS5_H__

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

    class RewardAdapter_PS5 : public RewardAdapter
    {
    public:
        enum State
        {
            STATE_NOT_INITIALIZED = 0,
            STATE_INITIALIZE,
            STATE_REGISTER_TROPHY,
            STATE_REGISTER_UDS,
            STATE_READY
        };

        RewardAdapter_PS5();
        void init_rewardList();

        virtual ~RewardAdapter_PS5()
        {
        }

        //  u32 getRewardsCount() override { return 0; };
        void update() override;
        void stateUpdate();
        void InitiateTrophySystem();
        bbool unlock(RewardID _id) override;
        bbool isLocked(RewardID _id) const override;

        void init(bbool _enable) override;
        void term() override;
        int initForBoot();
        int registerUnlockCalback();
        int createContexts();
        uint32_t m_trophyCount;
        SceNpTrophy2Data m_trophy2DataArray[SCE_NP_TROPHY2_NUM_MAX];
        State m_currentState;
        State m_stateLast;
        State m_nextState;
        SceNpTrophy2Context m_contextTrophy;
        SceNpUniversalDataSystemContext m_contextUniversalDataSystem;
        static void unlockCallback(SceNpTrophy2Context _context, SceNpTrophy2Id _trophyId, void* _userdata);
        SceNpTrophy2Handle createTrophyHandler();
        SceNpUniversalDataSystemHandle createUniversalDataSystemHandler();
        void registerContexAndGetUnlockTrophies();
        void unlockTrophy(SceNpTrophy2Id _id);
        void switchState(State _newState);
        u32 getRewardsCount() const override;
        i32 initializeNpUniversalDataSystemLib();
        TaskManager* m_taskManager;

        struct RegisterTrophyData
        {
            SceNpTrophy2Context context;
            SceNpTrophy2Handle handler;
            RewardAdapter_PS5* adapter;
        };
        struct RegisterUDSData
        {
            SceNpUniversalDataSystemContext context;
            SceNpUniversalDataSystemHandle handler;
            RewardAdapter_PS5* adapter;
        };
        struct UnlockTrophyData
        {
            SceNpUniversalDataSystemContext context;
            SceNpUniversalDataSystemHandle handler;
            RewardAdapter_PS5* adapter;
            u32 trophyId;
        };
    private :
        void registerUDSContext();
        const size_t kUdsMemSize = 16 * 1024;

    private : // Task function
        static i32 task_unlockTrophyTask(void* _param);
        static void task_unlockTrophyTaskCallback(void* _param, i32 res);
        static i32 task_runRegisterAndUnlockedTrophy(void* _param);
        static void task_runRegisterAndUnlockedTrophyCallback(void* _param, i32 res);
        static i32 task_runRegisterUDSContext(void* _param);
        static void task_runRegisterUDSContextCallback(void* _param, i32 res);
    };

#define REWARD_ADAPTER_PS5             (static_cast< RewardAdapter_PS5* > (REWARD_ADAPTER))
}

#endif // _ITF_REWARDADAPTER_PS5_H__

#endif
