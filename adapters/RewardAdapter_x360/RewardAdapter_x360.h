#ifdef ITF_X360

#ifndef _ITF_REWARDADAPTER_X360_H__
#define _ITF_REWARDADAPTER_X360_H__

#ifndef _ITF_REWARDADAPTER_H_
#include "engine/AdaptersInterfaces/RewardAdapter.h"
#endif //*88*/_ITF_REWARDADAPTER_H_

namespace ITF
{
#define ACHIEVEMENT_COUNT       36
#define ACHIEVEMENT_QUEUE_MAX   5   // max achievement to unlock in a row

    class RewardAdapter_x360 : public RewardAdapter
    {
    public:
        RewardAdapter_x360();
        virtual ~RewardAdapter_x360() {}

        void init(bbool enable);
        void initializeAchievements();
        void resetAchievements();
        void term();

        bbool       unlock (RewardID _id);
        void        processUnlock();
        void        checkForOperationPending();
        bbool       isLocked (RewardID _id) const;

        XACHIEVEMENT_DETAILS*   getReward(RewardID _id) const;
        u32         getRewardsCount() const { return ACHIEVEMENT_COUNT; }
        void        update();

        bbool       needsSaveSystemToWork() const { return bfalse; }

    private:
        // Datas collected from profile
        BYTE* m_Achievements;       

        XOVERLAPPED m_Overlapped; 

        // Current Achievement 
        SafeArray<XUSER_ACHIEVEMENT>    m_AchievementsToUnlock;
        XUSER_ACHIEVEMENT*              m_CurrentAchievement;

        bbool m_waitingForUserSignin;
        bbool m_operationPending;

        typedef ITF_VECTOR<RewardID> UnlockingRewardID;
        UnlockingRewardID m_unlockingRewardID;
    };

}

#endif // _ITF_REWARDADAPTER_X360_H__

#endif
