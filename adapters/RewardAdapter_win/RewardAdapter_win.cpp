#include "precompiled_RewardAdapter_win.h"

#include "adapters/RewardAdapter_win/RewardAdapter_win.h"

namespace ITF
{
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
        return (ite->second) ;
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
