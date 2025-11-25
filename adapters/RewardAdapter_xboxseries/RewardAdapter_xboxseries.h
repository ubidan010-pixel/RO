#ifdef ITF_XBOX_SERIES

#ifndef _ITF_REWARDADAPTER_H_
#include "engine/AdaptersInterfaces/RewardAdapter.h"
#endif //_ITF_REWARDADAPTER_H_

#include <unordered_map>

namespace ITF
{
#define ACHIEVEMENT_COUNT       36
#define ACHIEVEMENT_QUEUE_MAX   5   // max achievement to unlock in a row

    class RewardAdapter_xboxseries : public RewardAdapter
    {
    public:
        RewardAdapter_xboxseries();
        virtual ~RewardAdapter_xboxseries() {}

        void init(bbool enable);
        void initializeAchievements();
        void resetAchievements();
        void term();

        bbool       unlock (RewardID _id);
        void        processUnlock();
        bbool       isLocked (RewardID _id) const;

        u32         getRewardsCount() const { return ACHIEVEMENT_COUNT; }
        void        update();

        bbool       needsSaveSystemToWork() const { return bfalse; }

    private:
        u64 m_xuid = 0;

        bbool m_waitingForUserSignin;
        bbool m_operationPending;
        bbool m_initializedWithAchievementsManager;

        ITF_MAP<RewardID, bool> m_unlockedByReward;

        typedef ITF_VECTOR<RewardID> UnlockingRewardID;
        UnlockingRewardID m_unlockingRewardID;
    };

}

#endif
