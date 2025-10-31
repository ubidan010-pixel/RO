#include "precompiled_RewardAdapter_x360.h"

#ifndef _ITF_REWARDADAPTER_X360_H__
#include "adapters/RewardAdapter_x360/RewardAdapter_x360.h"
#endif

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#include "../../../make/Techno/x360/rayman.spa.h"

#ifndef _ITF_NETWORKSERVICES_X360_H_
#include "engine/NetworkServices/X360/NetworkServices_x360.h"
#endif //_ITF_NETWORKSERVICES_X360_H_

namespace ITF
{
    RewardAdapter_x360::RewardAdapter_x360() : 
m_Achievements(NULL)
, m_waitingForUserSignin(btrue)
, m_operationPending(bfalse)
, m_CurrentAchievement(NULL)
, RewardAdapter()
{        
}

void RewardAdapter_x360::term()
{
    if(m_operationPending)
        XCancelOverlapped(&m_Overlapped);

    if(m_Achievements)          Memory::free(m_Achievements);
    if(m_CurrentAchievement)    Memory::free(m_CurrentAchievement);

    RewardAdapter::term();
}

XACHIEVEMENT_DETAILS* RewardAdapter_x360::getReward(RewardID _id) const
{
    if(!m_Achievements) return NULL;

    // Retrieve achievement pictures
    XACHIEVEMENT_DETAILS* rgAchievements = ( XACHIEVEMENT_DETAILS* )m_Achievements;
    u32 rwdID = getRewardPlateform(_id);

    for( DWORD i = 0; i < getRewardsCount(); ++i )
    {
        if( rgAchievements[i].dwId == rwdID ) return &rgAchievements[i];            
    }
    return NULL;
}

void RewardAdapter_x360::resetAchievements()
{
    term();
    m_rewardMap.clear();
    m_unlockingRewardID.clear();
}

void RewardAdapter_x360::initializeAchievements()
{

    HANDLE hEnum;
    DWORD cbBuffer;        

    // Create enumerator for the default device
    DWORD dwStatus;
    dwStatus = XUserCreateAchievementEnumerator(
        TITLEID_RAYMAN_ORIGINS,         // Enumerate for the current title
        m_mainPlayerIndex,              // User Index
        INVALID_XUID,                   // If INVALID_XUID, the current user's achievements are enumerated
        XACHIEVEMENT_DETAILS_ALL,
        0,                              // starting achievement index
        getRewardsCount(),              // number of achievements to return
        &cbBuffer,                      // bytes needed
        &hEnum );
    if ( dwStatus != ERROR_SUCCESS )
    {        
        ITF_ASSERT( dwStatus == ERROR_SUCCESS );
        m_enable = bfalse;
        return ; 
    }


    // Enumerate achievements
    DWORD dwItems;            
    u32 totalSize = XACHIEVEMENT_SIZE_FULL * getRewardsCount();

    m_Achievements = (BYTE*)Memory::mallocCategory(totalSize,MemoryId::mId_System);
    if( XEnumerate( hEnum, m_Achievements, totalSize, &dwItems, NULL ) == ERROR_SUCCESS )
    {
        ITF_ASSERT( getRewardsCount() == dwItems );
    }
    CloseHandle( hEnum );


    addReward(s_REWARD_RAY_BETILLA_BACK, ACHIEVEMENT_TRP01);
    addReward(s_REWARD_RAY_BEAUTIFUL_BEATS, ACHIEVEMENT_TRP02);
    addReward(s_REWARD_RAY_FOODIE_FAIRY, ACHIEVEMENT_TRP03);
    addReward(s_REWARD_RAY_MERMAID, ACHIEVEMENT_TRP04);
    addReward(s_REWARD_RAY_NYMPHS_ROCK, ACHIEVEMENT_TRP05);
    addReward(s_REWARD_RAY_BIG_MAMA, ACHIEVEMENT_TRP06);        
    addReward(s_REWARD_RAY_SCOUT, ACHIEVEMENT_TRP07);
    addReward(s_REWARD_RAY_Explorer, ACHIEVEMENT_TRP08);
    addReward(s_REWARD_RAY_DR_LIVIDSTONE, ACHIEVEMENT_TRP09);
    addReward(s_REWARD_RAY_SPEEDY, ACHIEVEMENT_TRP10);
    addReward(s_REWARD_RAY_TURBO, ACHIEVEMENT_TRP11);
    addReward(s_REWARD_RAY_NITRO, ACHIEVEMENT_TRP12);
    addReward(s_REWARD_RAY_ELEC_FRIEND, ACHIEVEMENT_TRP13);
    addReward(s_REWARD_RAY_ELEC_HERO, ACHIEVEMENT_TRP14);
    addReward(s_REWARD_RAY_ELEC_LEGEND, ACHIEVEMENT_TRP15);
    addReward(s_REWARD_RAY_MILK_MOUTH, ACHIEVEMENT_TRP16);
    addReward(s_REWARD_RAY_FULL_MOUTH, ACHIEVEMENT_TRP17);
    addReward(s_REWARD_RAY_THE_JAW, ACHIEVEMENT_TRP18);
    addReward(s_REWARD_RAY_POP_POP_BOOM, ACHIEVEMENT_TRP19);
    addReward(s_REWARD_RAY_BUBBLIZER, ACHIEVEMENT_TRP20);
    addReward(s_REWARD_RAY_IAM_BACK, ACHIEVEMENT_TRP21);
    addReward(s_REWARD_RAY_PAINLESS, ACHIEVEMENT_TRP22);
    addReward(s_REWARD_RAY_VACUUM_KNACK, ACHIEVEMENT_TRP23);
    addReward(s_REWARD_RAY_NOPANIC, ACHIEVEMENT_TRP24);
    addReward(s_REWARD_RAY_FINISH_WITH_COSTUME, ACHIEVEMENT_TRP25);
    addReward(s_REWARD_RAY_HOVER_HAPPY, ACHIEVEMENT_TRP26);
    addReward(s_REWARD_RAY_CRUSHER, ACHIEVEMENT_TRP27);
    //addReward(s_REWARD_RAY_PEACEFUL_DREAMS, ACHIEVEMENT_TRP28);
    addReward(s_REWARD_RAY_CRUSH_COMBO, ACHIEVEMENT_TRP28);
    addReward(s_REWARD_RAY_SPRINTER, ACHIEVEMENT_TRP29);
    addReward(s_REWARD_RAY_FISHER_KING, ACHIEVEMENT_TRP30);   
    addReward(s_REWARD_RAY_KUNGFUCOMBO, ACHIEVEMENT_TRP31);
    addReward(s_REWARD_RAY_HYPERSPEED, ACHIEVEMENT_TRP32);        
    addReward(s_REWARD_RAY_BOING, ACHIEVEMENT_TRP33);
    addReward(s_REWARD_RAY_BLUE_BARON, ACHIEVEMENT_TRP34);
    addReward(s_REWARD_RAY_SURVIVEDPIRANHAPOND, ACHIEVEMENT_TRP35);
    addReward(s_REWARD_RAY_BACK_AT_YOU, ACHIEVEMENT_TRP36);

    m_CurrentAchievement = (XUSER_ACHIEVEMENT*)Memory::mallocCategory(ACHIEVEMENT_QUEUE_MAX*sizeof(XUSER_ACHIEVEMENT),MemoryId::mId_System);

    XNotifyPositionUI(XNOTIFYUI_POS_TOPRIGHT);
}

void RewardAdapter_x360::update()
{
    RewardAdapter::update();
    if(!m_enable) return;

    if(m_waitingForUserSignin)
    {
        if ( m_mainPlayerIndex != U32_INVALID && NETWORKSERVICES->isUserSignedIn(m_mainPlayerIndex) )
        {
            m_waitingForUserSignin  = bfalse;
            initializeAchievements();
        }
    } else
    {
        if( m_mainPlayerIndex == U32_INVALID )
        {
            m_waitingForUserSignin = btrue;
            resetAchievements();
        }
        else if (m_operationPending)
        {
            checkForOperationPending();
        } else
        {
            processUnlock();
        }
    }
}

void RewardAdapter_x360::init(bbool enable)
{
    RewardAdapter::init(enable);
}

bbool RewardAdapter_x360::unlock (RewardID _id)
{
    if(m_waitingForUserSignin)
        return bfalse;

    if ( std::find(m_unlockingRewardID.begin(), m_unlockingRewardID.end(), _id) ==
        m_unlockingRewardID.end() )
    {
        XUSER_ACHIEVEMENT   achievement;
        achievement.dwAchievementId = getRewardPlateform(_id);
        achievement.dwUserIndex = m_mainPlayerIndex;

        m_unlockingRewardID.push_back(_id);
        m_AchievementsToUnlock.push_back(achievement);
        return btrue; 
    }
    return bfalse;
}

void RewardAdapter_x360::processUnlock()
{
    u32 achievementCount = m_AchievementsToUnlock.size();
    if(achievementCount == 0) return;

    if(achievementCount > ACHIEVEMENT_QUEUE_MAX)
        achievementCount = ACHIEVEMENT_QUEUE_MAX;

    u32 achievementCount_backup = achievementCount;

    for(u32 i = 0; i < achievementCount; i++ )
    {
        ITF_Memcpy(
            &m_CurrentAchievement[i], 
            &m_AchievementsToUnlock.back(), sizeof(XUSER_ACHIEVEMENT));

        m_AchievementsToUnlock.pop_back();
    }

    ZeroMemory( &m_Overlapped, sizeof( XOVERLAPPED ) );
    DWORD dwStatus = XUserWriteAchievements(achievementCount_backup, 
        m_CurrentAchievement, &m_Overlapped);

    ITF_ASSERT ( dwStatus ==  ERROR_IO_PENDING );
    if (dwStatus == ERROR_IO_PENDING)
    {
        m_operationPending = btrue;
    } else
    {
        // Error handling
    }
}

bbool RewardAdapter_x360::isLocked (RewardID _id) const
{
    if(m_waitingForUserSignin)
        return bfalse;

    if ( std::find(m_unlockingRewardID.begin(), m_unlockingRewardID.end(), _id) !=
        m_unlockingRewardID.end() )
    {
        return bfalse;
    }

    XACHIEVEMENT_DETAILS* pAchievement = getReward(_id);
    if( pAchievement )
    {
        return !( AchievementEarned(pAchievement->dwFlags) );
    }
    return bfalse;
}

void RewardAdapter_x360::checkForOperationPending()
{
    if(XHasOverlappedIoCompleted(&m_Overlapped))
    {
        m_operationPending = bfalse;
    }
}
}
