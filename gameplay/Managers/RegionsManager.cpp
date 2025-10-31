
#include "precompiled_gameplay.h"

#ifndef _ITF_REGIONSMANAGER_H_
#include "gameplay/managers/RegionsManager.h"
#endif //_ITF_REGIONSMANAGER_H_

#ifndef _ITF_ID_SERVER_H_
#include "core/IDserver.h"
#endif //_ITF_ID_SERVER_H_

namespace ITF
{

RegionsManager::RegionsManager()
{
}

RegionsManager::~RegionsManager()
{
}

void RegionsManager::addRegion( ObjectRef _ref, const StringID& _id, const DepthRange& _depthRange )
{
    DepthRangeData* depthRange = NULL;

    for ( DepthRangeDataList::iterator it = m_depthRanges.begin(); it != m_depthRanges.end(); ++it )
    {
        if ( it->m_depthRange == _depthRange )
        {
            depthRange = &(*it);
            break;
        }
    }

    if ( !depthRange )
    {
        m_depthRanges.push_back(DepthRangeData());
        depthRange = &m_depthRanges.back();
        depthRange->m_depthRange = _depthRange;
    }

#ifdef _DEBUG
    for ( u32 i = 0; i < depthRange->m_regions.size(); i++ )
    {
        ITF_ASSERT(depthRange->m_regions[i].m_ref != _ref);
    }
#endif

    depthRange->m_regions.push_back(RegionData());

    RegionData* region = &depthRange->m_regions[depthRange->m_regions.size()-1];

    region->m_id = _id;
    region->m_ref = _ref;
}

void RegionsManager::removeRegion( ObjectRef _ref )
{
    for ( DepthRangeDataList::iterator it = m_depthRanges.begin(); it != m_depthRanges.end(); ++it )
    {
        DepthRangeData& depthRange = *it;

        for ( u32 i = 0; i < depthRange.m_regions.size(); i++ )
        {
            RegionData& region = depthRange.m_regions[i];

            if ( region.m_ref == _ref )
            {
                depthRange.m_regions.eraseNoOrder(i);
                return;
            }
        }
    }
    ITF_ASSERT_MSG(0,"RegionsManager::removeRegion: region not found");
}

const RegionsManager::RegionDataList* RegionsManager::getRegions( const DepthRange& _depthRange ) const
{
    const RegionsManager::RegionDataList* ret = NULL;

    for ( DepthRangeDataList::const_iterator it = m_depthRanges.begin(); it != m_depthRanges.end(); ++it )
    {
        const DepthRangeData& depthRange = *it;

        if ( depthRange.m_depthRange != _depthRange )
        {
            continue;
        }

        ret = &depthRange.m_regions;
        break;
    }

    return ret;
}

}