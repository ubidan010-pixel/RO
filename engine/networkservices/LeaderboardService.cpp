#include "precompiled_engine.h"

#ifndef _ITF_LEADERBOARDSERVICE_
#include "engine/networkservices/LeaderboardService.h"
#endif //_ITF_LEADERBOARDSERVICE_
/*
#ifdef ITF_X360
#include "../make/Techno/x360/spa.h"
#endif //ITF_X360
*/
//------------------------------------------------------------------------------------------------------------------------------------------------------

namespace ITF
{
    Leaderboard::~Leaderboard()
    {
        for (u32 index = 0;index<m_PropertiesArray.size();++index)
        {
            SF_DEL(m_PropertiesArray[index]);
        }
    }

    void Leaderboard::addTemplateProperty(const String& _name,u32 _alias,LeaderboardPropertyEnum _enum)
    {
        switch (_enum)
        {
        case eLeaderBoardString:
            break;
        case eLeaderboardU32:
            m_PropertiesArray.push_back(new LeaderboardTemplateProperty(_name,_alias,eLeaderboardU32));
            break;
        case eLeaderboardDouble:
            break;
        default:
            ITF_ASSERT(0);
            break;
        }
    }
    //------------------------------------------------------------------------------------------------------------------------------------------------------

    LeaderboardService::~LeaderboardService()
    {
        destroy();
    }

    void LeaderboardService::init()
    {
        //target initialization
        initInternal();
        ITF_UNUSED(m_incrementLeaderboardId);
/*
#ifdef ITF_X360
        return;
        //for test
        Leaderboard* pLeadboard = new Leaderboard(++m_incrementLeaderboardId);
        pLeadboard->addTemplateProperty("MaxPoints",STATS_COLUMN_POINTS_LEADERBOARD_MAX_POINTS,eLeaderboardU32);
        pLeadboard->addTemplateProperty("LastPoints",STATS_COLUMN_POINTS_LEADERBOARD_LAST_POINTS,eLeaderboardU32);
        pLeadboard->addTemplateProperty("MinPoints",STATS_COLUMN_POINTS_LEADERBOARD_MIN_POINTS,eLeaderboardU32);
        m_Leaderboards.push_back(pLeadboard);
        //create a request test;
        getInfosByRange(*pLeadboard,1,10);
#endif //ITF_X360
*/

    }

    void LeaderboardService::destroy()
    {
        for (u32 indexLeaderboard = 0;indexLeaderboard< m_Leaderboards.size();++indexLeaderboard)
        {
            Leaderboard* pLeadboard = m_Leaderboards[indexLeaderboard];
            SF_DEL(pLeadboard);
        }
    }

    LeaderboardRequestID  LeaderboardService::writeDetails(const Leaderboard& _leaderboard,const LeaderboardArray& _properties)
    {
        const LeaderboardRequestID currentRequestId = getNewRequestID(_leaderboard);
        m_workingRequestId.push_back(currentRequestId);
        return currentRequestId;
    }

    LeaderboardRequestID LeaderboardService::getDetails(const Leaderboard& _leaderboard)
    {
        const LeaderboardRequestID currentRequestId = getNewRequestID(_leaderboard);
        m_workingRequestId.push_back(currentRequestId);
        return currentRequestId;
    }


    LeaderboardRequestID LeaderboardService::getMyRank(const Leaderboard& _leaderboard)
    {
        const LeaderboardRequestID currentRequestId = getNewRequestID(_leaderboard);
        m_workingRequestId.push_back(currentRequestId);
        return currentRequestId;
    }

    LeaderboardRequestID LeaderboardService::getInfosByRange(const Leaderboard& _leaderboard,u32 _startRange,u32 _endRange)
    {
        const LeaderboardRequestID currentRequestId = getNewRequestID(_leaderboard);

        //target interaction
        requestRangeDetails(currentRequestId,_startRange,_endRange);

        m_workingRequestId.push_back(currentRequestId);
        return currentRequestId;
    }

    void LeaderboardService::update()
    {
        for (u32 index = 0;index< m_workingRequestId.size();++index)
        {
            LeaderboardRequestID& requestID = m_workingRequestId[index];
            requestID.getContext()->update();

        }
    }

    bbool LeaderboardService::poll(const LeaderboardRequestID& _requestID)
    {
        return btrue;
    }

    LeaderboardRequestID LeaderboardService::getNewRequestID(const Leaderboard& _pLeaderboard)
    {
        return LeaderboardRequestID(createContext(&_pLeaderboard),++m_incrementRequestId);
    }

} // namespace ITF

