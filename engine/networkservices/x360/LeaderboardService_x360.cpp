#include "precompiled_engine.h"

#include "xbox.h"

#ifndef _ITF_LEADERBOARDSERVICE_X360_H_
#include "engine/networkservices/x360/LeaderboardService_x360.h"
#endif //_ITF_LEADERBOARDSERVICE_X360_H_

//#include "../make/Techno/x360/spa.h"

namespace ITF
{
//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void LeaderboardContext_x360::enumerate( HANDLE _hEnumerator,u32 _bufferSize)
{
    // Allocate the buffer
    m_pStats = ( PXUSER_STATS_READ_RESULTS )new u8[ _bufferSize ];

    u32 ret = XEnumerate(
        _hEnumerator,           // Enumeration handle
        m_pStats,              // Buffer
        _bufferSize,             // Size of buffer
        NULL,                  // Number of rows returned; not used for asynch
        &m_Overlapped );       // Overlapped structure

    ITF_ASSERT(ret == ERROR_IO_PENDING  );
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void LeaderboardContext_x360::update()
{/*
    if (m_bDone)
        return;

    if (XHasOverlappedIoCompleted(&m_Overlapped ) == btrue)
    {
        m_bDone = btrue;
        HRESULT hr = XGetOverlappedExtendedError( &m_Overlapped );

        if( SUCCEEDED( hr ) )
        {
            //fill the properties result

            
            for (u32 index = 0;index<m_pStats->dwNumViews;++index)
            {
                //row count
                //m_ResultProperties 
                LeaderboardPropertyArray* pPropertiesArray = new LeaderboardPropertyArray();
                for( UINT i = 0; i < m_pStats->pViews[ 0 ].dwNumRows; ++i )
                {
                     const XUSER_STATS_ROW& tt = m_pStats->pViews[ 0 ].pRows[ i ];
                    
                     //Missing XUID
                     pPropertiesArray->push_back(new LeaderboardProperty("GamerTag",tt.szGamertag));
                     pPropertiesArray->push_back(new LeaderboardProperty("Rank",(u32)tt.dwRank));

                     //column for test
                     for (u32 col = 0;col < tt.dwNumColumns;++col)
                     {
                         XUSER_STATS_COLUMN column = tt.pColumns[col];
                         LeaderboardTemplateProperty* pTemplate = getLeaderboard()->getTemplatePropertyByIndex(column.wColumnId);
                         //column.wColumnId 
                     }

                }

                m_ResultProperties.push_back(pPropertiesArray);
            }
        }
        else
        {
            ITF_ASSERT(0);
        }
    }
*/
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void LeaderboardService_x360::initInternal()
{

}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

LeaderboardContext* LeaderboardService_x360::createContext(const Leaderboard* _pLeaderBoard)
{
    LeaderboardContext* pContext = new LeaderboardContext_x360();
    pContext->setLeaderboard(_pLeaderBoard);
    return pContext;
}
//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void LeaderboardService_x360::requestRangeDetails(const LeaderboardRequestID& _leaderboardRequestID,u32 _startrange,u32 _endrange)
{/*
    HANDLE hEnumerator;
    DWORD cbResults;

    LeaderboardContext_x360* pContext = (LeaderboardContext_x360*)_leaderboardRequestID.getContext();

    XUSER_STATS_SPEC* pSpecification = pContext->getSpecification();

    //for testing
    pSpecification->dwViewId = 1;//TDB

    const Leaderboard* pLeaderboard = pContext->getLeaderboard();
    const LeaderboardTemplatePropertyArray& propertiesTemplateArray = pLeaderboard->getTemplatePropertyArray();

    pSpecification->dwNumColumnIds = propertiesTemplateArray.size();

    for (u32 index = 0;index<propertiesTemplateArray.size();++index)
    {
        const LeaderboardTemplateProperty* pTemplateProperty = propertiesTemplateArray[index];
        pSpecification->rgwColumnIds[ index ] = pTemplateProperty->getAlias();
    }

    ITF_ASSERT(_endrange>_startrange);
    u32 ret = XUserCreateStatsEnumeratorByRank(0, // Current title ID
        _startrange,                     // Index to start enumerating from
        //_endrange-_startrange,   // Number of rows to retrieve
        8,
        1,                       // One stats spec
        pContext->getSpecification(),                 // Stats spec,
        &cbResults,              // Size of buffer
        &hEnumerator );          // Enumeration handle

    ITF_ASSERT(ret == ERROR_SUCCESS );

    pContext->enumerate(hEnumerator,cbResults);*/


}
//----------------------------------------------------------------------------------------------------------------------------------------------------------------

}
// namespace ITF

