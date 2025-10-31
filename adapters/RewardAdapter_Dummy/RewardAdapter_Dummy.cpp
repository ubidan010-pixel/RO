#include "precompiled_RewardAdapter_Dummy.h"

#include "adapters/RewardAdapter_Dummy/RewardAdapter_Dummy.h"

namespace ITF
{
    RewardAdapter_Dummy::RewardAdapter_Dummy()
    {
        ITF_REWARD_NOT_IMPLEMENTED();
    }

    RewardAdapter_Dummy::~RewardAdapter_Dummy()
    {
        ITF_REWARD_NOT_IMPLEMENTED();
    }

    bbool RewardAdapter_Dummy::unlock(RewardID _id)
    {
        ITF_REWARD_NOT_IMPLEMENTED();
        return btrue;
    }

    bbool RewardAdapter_Dummy::isLocked(RewardID _id) const
    {
        ITF_REWARD_NOT_IMPLEMENTED();
        return btrue;
    }

    u32 RewardAdapter_Dummy::getRewardsCount() const
    {
        ITF_REWARD_NOT_IMPLEMENTED();
        return 0;
    }

    void RewardAdapter_Dummy::addReward(const RewardID& _id, u32 _plateformID)
    {
        ITF_REWARD_NOT_IMPLEMENTED();
    }

} // namespace ITF
