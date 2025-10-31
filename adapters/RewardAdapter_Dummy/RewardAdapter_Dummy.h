#pragma once

#ifdef ITF_DUMMY_REWARDADAPTER

#include "engine/AdaptersInterfaces/RewardAdapter.h"

#ifdef ITF_DUMMY_ADAPTERS_ASSERT_ON_NOT_IMPLEMENTED
    #define ITF_REWARD_NOT_IMPLEMENTED() ITF_ASSERT_MSG(0, "Reward not implemented")
#else
    #define ITF_REWARD_NOT_IMPLEMENTED() do {} while (0)
#endif

namespace ITF
{
    class RewardAdapter_Dummy : public RewardAdapter
    {
    public:
        RewardAdapter_Dummy();
        virtual ~RewardAdapter_Dummy() override;

        virtual bbool unlock(RewardID _id) override;
        virtual bbool isLocked(RewardID _id) const override;
        virtual u32 getRewardsCount() const override;
        virtual void addReward(const RewardID& _id, u32 _plateformID) override;
    };

    #define REWARD_ADAPTER_DUMMY    (static_cast<RewardAdapter_Dummy*>(REWARD_ADAPTER))
}

#endif // ITF_DUMMY
