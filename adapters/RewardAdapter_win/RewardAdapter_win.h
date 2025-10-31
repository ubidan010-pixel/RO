#ifndef ITF_REWARDADAPTER_WIN_H_
#define ITF_REWARDADAPTER_WIN_H_

#pragma once

#ifdef ITF_WINDOWS

#ifndef _ITF_REWARDADAPTER_H_
#include "engine/AdaptersInterfaces/RewardAdapter.h"
#endif //_ITF_REWARDADAPTER_H_

namespace ITF
{
    class RewardAdapter_win : public RewardAdapter
    {
    public:
        RewardAdapter_win(): m_displayRewardsTime(0.0) {}
        virtual ~RewardAdapter_win() {}

        bbool       unlock (RewardID _id);
        bbool       isLocked (RewardID _id) const;
        void        addReward(const RewardID& _id, u32 _plateformID);
        u32         getRewardsCount() const ;

        // Save Utils
        void        getRewardsData(u8* pData) const;
        u32         getRewardsDataSize() const;
        bbool       setRewardsData(u8* pData, u32 dataSize);

        void        displayRewards();
    private:
        // Locked?
        typedef ITF_MAP<RewardID, bbool> RewardState;
        RewardState m_rewardsState;
        f64         m_displayRewardsTime;
    };

#define REWARD_ADAPTER_WIN 	(static_cast<RewardAdapter_win*> (REWARD_ADAPTER))
}

#endif // ITF_WINDOWS

#endif // ITF_REWARDADAPTER_WIN_H_
