#include "precompiled_RewardAdapter_win.h"

#include "adapters/RewardAdapter_win/RewardAdapter_win.h"
#include "engine/singleton/Singletons.h"
#include "engine/networkservices/UPlayService.h"

namespace ITF
{
    static const ITF_MAP<RewardID, u32> MapRewardToUCAchievementID = {
    {"REWARD_RAY_BETILLA_BACK", 1},
    {"REWARD_RAY_BEAUTIFUL_BEATS", 2},
    {"REWARD_RAY_FOODIE_FAIRY", 3},
    {"REWARD_RAY_MERMAID", 4},
    {"REWARD_RAY_NYMPHS_ROCK", 5},
    {"REWARD_RAY_SCOUT", 6},
    {"REWARD_RAY_Explorer", 7},
    {"REWARD_RAY_DR_LIVIDSTONE", 8},
    {"REWARD_RAY_SPEEDY", 9},
    {"REWARD_RAY_TURBO", 10},
    {"REWARD_RAY_NITRO", 11},
    {"REWARD_RAY_ELEC_FRIEND", 12},
    {"REWARD_RAY_ELEC_HERO", 13},
    {"REWARD_RAY_ELEC_LEGEND", 14},
    {"REWARD_RAY_MILK_MOUTH", 15},
    {"REWARD_RAY_FULL_MOUTH", 16},
    {"REWARD_RAY_THE_JAW", 17},
    {"REWARD_RAY_POP_POP_BOOM", 18},
    {"REWARD_RAY_BUBBLIZER", 19},
    {"REWARD_RAY_IAM_BACK", 20},
    {"REWARD_RAY_PAINLESS", 21},
    {"REWARD_RAY_VACUUM_KNACK", 22},
    {"REWARD_RAY_NOPANIC", 23},
    {"REWARD_RAY_FINISH_WITH_COSTUME", 24}, // B Side!
    {"REWARD_RAY_HOVER_HAPPY", 25},
    {"REWARD_RAY_CRUSHER", 26},
    {"REWARD_RAY_CRUSH_COMBO", 27},
    {"REWARD_RAY_SPRINTER", 28},
    {"REWARD_RAY_FISHER_KING", 29},
    {"REWARD_RAY_KUNGFUCOMBO", 30},
    {"REWARD_RAY_HYPERSPEED", 31},
    {"REWARD_RAY_BOING", 32},
    {"REWARD_RAY_BLUE_BARON", 33},
    {"REWARD_RAY_SURVIVEDPIRANHAPOND", 34},
    {"REWARD_RAY_BACK_AT_YOU", 35},
    {"REWARD_RAY_BIG_MAMA", 36},    // Ding Dong
    {"REWARD_RAY_CLEAR_SIGHTED", 38},
    {"REWARD_RAY_EAGLE_EYED", 39},
    };

    u32 RewardAdapter_win::getRewardsCount() const 
    {
        return m_rewardsState.size();
    }
    
    void RewardAdapter_win::addReward(const RewardID& _id, u32 _plateformID)
    {
        RewardAdapter::addReward(_id, _plateformID);

        // insert new pair
        STD_PAIR<RewardID, bbool> newRewardState;
        newRewardState.first    = _id;
        newRewardState.second   = btrue;

        m_rewardsState.insert(newRewardState);
    } 

    bbool RewardAdapter_win::unlock(RewardID _id)
    {       
        RewardState::iterator ite = m_rewardsState.find(_id);
        if (ite == m_rewardsState.end())
            return bfalse;

#ifdef ITF_UPLAYPC
        if (UPLAYSERVICE)
        {
            bool found = false;
            auto it = ITF::MapRewardToUCAchievementID.find(_id);
            if (it != MapRewardToUCAchievementID.end())
            {
                found = UPLAYSERVICE->unlockAchievement(it->second);
            }

            if (!found)
                LOG("[RewardAdapter] unlocking failed for %s", _id.getDebugString());
        }
#endif

        ite->second = bfalse;
        return btrue;
    }

    bbool RewardAdapter_win::isLocked(RewardID _id) const
    {
        RewardState::const_iterator ite = m_rewardsState.find(_id);
        if (ite == m_rewardsState.end())
        {
            return btrue;
        }

        bbool upc_locked = btrue;

#ifdef ITF_UPLAYPC
        if (UPLAYSERVICE)
        {
            auto it = ITF::MapRewardToUCAchievementID.find(_id);
            if (it != MapRewardToUCAchievementID.end())
            {
                upc_locked = UPLAYSERVICE->isAchievementLocked(it->second);
            }
        }
#endif

        return (ite->second & upc_locked);
    }

    void RewardAdapter_win::getRewardsData(u8* pData) const
    {
        if(!pData) return;

        RewardState::const_iterator ite = m_rewardsState.begin();
        for (; ite != m_rewardsState.end(); ++ite)
        {
            StringID::StringIdValueType  value = (ite->first).GetValue();
            ITF_Memcpy(pData, &value,sizeof(StringID::StringIdValueType));
            pData += sizeof(StringID::StringIdValueType);

            ITF_Memcpy(pData, &(ite->second), sizeof(bbool));
            pData += sizeof(bbool);
        }        
    }

    u32 RewardAdapter_win::getRewardsDataSize() const
    {
        static const u32 dataSize = (getRewardsCount() *  ( sizeof(StringID::StringIdValueType) + sizeof(bbool) ) );
        return dataSize;
    }

    bbool RewardAdapter_win::setRewardsData(u8* pData, u32 dataSize)
    {
        if(dataSize != getRewardsDataSize()) 
        {
            return bfalse;
        }

        u32 count = getRewardsCount();        
        for (u32 i = 0; i<count; ++i)
        {
            // insert new pair
            StringID::StringIdValueType IdValue; 
            bbool                       IsLocked;

            ITF_Memcpy(&IdValue, pData, sizeof(StringID::StringIdValueType));
            pData += sizeof(StringID::StringIdValueType);

            ITF_Memcpy(&IsLocked, pData, sizeof(bbool));
            pData += sizeof(bbool);

            RewardID ID  ( IdValue );
            if (!IsLocked)  // not locked ?
            {
                unlock(ID);
            }
        }   

        return btrue;
    }

    void RewardAdapter_win::displayRewards()
    {
    }
}
