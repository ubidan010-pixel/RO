#ifdef ITF_X360

#ifndef _ITF_LEADERBOARDSERVICE_X360_H_
#define _ITF_LEADERBOARDSERVICE_X360_H_

#ifndef _ITF_LEADERBOARDSERVICE
#include "engine/networkservices/LeaderboardService.h"
#endif //_ITF_LEADERBOARDSERVICE


namespace ITF
{

class LeaderboardContext_x360 : public LeaderboardContext
{
public:
    LeaderboardContext_x360()
    {
      ZeroMemory( &m_Overlapped, sizeof( m_Overlapped ) );
      ZeroMemory( &m_Spec, sizeof( m_Spec ) );
    }

    virtual ~LeaderboardContext_x360() {};

    virtual void update();

    XUSER_STATS_SPEC*   getSpecification()  {return &m_Spec;}
    XOVERLAPPED*        getOverlapped()     {return &m_Overlapped;}

    void enumerate( HANDLE _hEnumerator,u32 _bufferSize);

protected:
     
   XUSER_STATS_SPEC   m_Spec;             // Stats specification
   XOVERLAPPED        m_Overlapped;       // Overlapped IO data structure

   PXUSER_STATS_READ_RESULTS m_pStats;    // Leaderboard or local stats
};


class LeaderboardService_x360: public LeaderboardService
{
protected:
    void initInternal();
    
    virtual LeaderboardContext* createContext(const Leaderboard* _pLeaderBoard);

    virtual void requestRangeDetails(const LeaderboardRequestID& _leaderboardRequestID,u32 _startrange,u32 _endrange);


private:

};

  

} // namespace ITF

#endif //_ITF_LEADERBOARDSERVICE_X360_H_
#endif //ITF_X360