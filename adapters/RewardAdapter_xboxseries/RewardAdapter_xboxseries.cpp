#include "precompiled_RewardAdapter_xboxseries.h"

#ifndef _ITF_REWARDADAPTER_XBOXSERIES_H__
#include "adapters/RewardAdapter_xboxseries/RewardAdapter_xboxseries.h"
#endif

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#include "adapters/SystemAdapter_XBoxSeries/SystemAdapter_XBoxSeries.h"

#include <adapters/AccountAdapter_xboxseries/AccountAdapter_xboxseries.h>

namespace ITF
{
    RewardAdapter_xboxseries::RewardAdapter_xboxseries() : 
m_waitingForUserSignin(btrue)
, m_operationPending(bfalse)
, m_initializedWithAchievementsManager(bfalse)
, RewardAdapter()
{        
}

void RewardAdapter_xboxseries::term()
{
    if (m_initializedWithAchievementsManager)
    {
        auto userInfo = SYSTEM_ADAPTER_XBOX_SERIES->getInitialUserInfo();
        if (userInfo.handle)
        {
            XblAchievementsManagerRemoveLocalUser(userInfo.handle);
        }
        m_initializedWithAchievementsManager = bfalse;
    }

    m_unlockedByReward.clear();
    m_unlockingRewardID.clear();

    RewardAdapter::term();
}

void RewardAdapter_xboxseries::resetAchievements()
{
    term();
    m_rewardMap.clear();
    m_unlockingRewardID.clear();
}

void RewardAdapter_xboxseries::initializeAchievements()
{
    ITF::SystemAdapter_XBoxSeries::UserInfo userInfo = SYSTEM_ADAPTER_XBOX_SERIES->getInitialUserInfo();

    if (!userInfo.handle || userInfo.userId == 0)
    {
        m_enable = bfalse;
        return;
    }

    m_xuid = userInfo.userId;

    HRESULT hr = XblAchievementsManagerAddLocalUser(userInfo.handle, nullptr);
    if (FAILED(hr))
    {
        m_enable = bfalse;
        return;
    }

    m_initializedWithAchievementsManager = btrue;

    // addReward is done from Ray_RewardManager now
    // on all platforms, to be easy to edit from inside rewardlist.isg
}

void RewardAdapter_xboxseries::update()
{
    RewardAdapter::update();
    if (!m_enable)
        return;

    if (m_waitingForUserSignin)
    {
        auto userInfo = SYSTEM_ADAPTER_XBOX_SERIES->getInitialUserInfo();
        if (userInfo.handle && userInfo.userId != 0)
        {
            m_waitingForUserSignin = bfalse;
            initializeAchievements();
        }
        return;
    }

    auto userInfo = SYSTEM_ADAPTER_XBOX_SERIES->getInitialUserInfo();
    if (!userInfo.handle || userInfo.userId == 0)
    {
        m_waitingForUserSignin = btrue;
        resetAchievements();
        return;
    }

    if (m_initializedWithAchievementsManager)
    {
        const XblAchievementsManagerEvent* events = nullptr;
        size_t eventCount = 0;

        HRESULT hr =  XblAchievementsManagerDoWork(&events, &eventCount);

        for (size_t i = 0; i < eventCount; ++i)
        {
            const auto& ev = events[i];
            switch (ev.eventType)
            {
            case XblAchievementsManagerEventType::LocalUserInitialStateSynced:
                break;

            case XblAchievementsManagerEventType::AchievementUnlocked:
            case XblAchievementsManagerEventType::AchievementProgressUpdated:
            {
                const auto& progress = ev.progressInfo;

                u32 platformId = 0;
                if (progress.achievementId && progress.achievementId[0] != '\0')
                {
                    platformId = (u32)std::strtoul(progress.achievementId, nullptr, 10);
                }

                if (platformId == 0)
                    break;

                RewardID rid{};
                bbool found = bfalse;

                for (RewardAdapter::RewardsMap::const_iterator it = m_rewardMap.begin();
                    it != m_rewardMap.end();
                    ++it)
                {
                    if (it->second == platformId)
                    {
                        rid = it->first;
                        found = btrue;
                        break;
                    }
                }

                if (!found)
                    break;

                m_unlockedByReward[rid] = progress.progressState == XblAchievementProgressState::Achieved;

                break;
            }

            default:
                break;
            }
        }
    }

    processUnlock();
}

void RewardAdapter_xboxseries::init(bbool enable)
{
    RewardAdapter::init(enable);
}

bbool RewardAdapter_xboxseries::unlock (RewardID _id)
{
    if (m_waitingForUserSignin || !m_initializedWithAchievementsManager)
        return bfalse;

    if (std::find(m_unlockingRewardID.begin(),
        m_unlockingRewardID.end(),
        _id) != m_unlockingRewardID.end())
    {
        return bfalse;
    }

    m_unlockingRewardID.push_back(_id);
    return btrue;
}

void RewardAdapter_xboxseries::processUnlock()
{
    if (!m_initializedWithAchievementsManager || m_unlockingRewardID.empty())
        return;

    u32 achievementCount = (u32)m_unlockingRewardID.size();
    if (achievementCount > ACHIEVEMENT_QUEUE_MAX)
        achievementCount = ACHIEVEMENT_QUEUE_MAX;

    for (u32 i = 0; i < achievementCount; ++i)
    {
        RewardID rid = m_unlockingRewardID.back();
        m_unlockingRewardID.pop_back();

        u32 platformId = getRewardPlateform(rid);
        if (platformId == U32_INVALID)
            continue; // safety

        std::string achievementIdStr = std::to_string(platformId);

        HRESULT hr = XblAchievementsManagerUpdateAchievement(
            m_xuid,
            achievementIdStr.c_str(),
            100);

        if (SUCCEEDED(hr))
        {

        }
    }
}

bbool RewardAdapter_xboxseries::isLocked (RewardID _id) const
{
    if (m_waitingForUserSignin || !m_initializedWithAchievementsManager)
        return bfalse;

    if (std::find(m_unlockingRewardID.begin(),
        m_unlockingRewardID.end(),
        _id) != m_unlockingRewardID.end())
    {
        return bfalse;
    }

    auto it = m_unlockedByReward.find(_id);
    if (it == m_unlockedByReward.end())
    {
        return btrue;
    }

    return !it->second;
}

}
