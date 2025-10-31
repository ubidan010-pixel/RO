#ifndef _ITF_REGIONSMANAGER_H_
#define _ITF_REGIONSMANAGER_H_

#ifndef _ITF_VEC2D_H_
#include "core/math/vec2d.h"
#endif //_ITF_VEC2D_H_

#ifndef _ITF_DEPTH_RANGE_H_
#include "engine/scene/DepthRange.h"
#endif //_ITF_DEPTH_RANGE_H_

namespace ITF
{
    class RegionsManager
    {
    public:

        RegionsManager();
        ~RegionsManager();

        struct RegionData
        {
            ObjectRef       m_ref;
            StringID        m_id;
        };

        typedef SafeArray <RegionData> RegionDataList;

        void                            addRegion( ObjectRef _ref, const StringID& _regionId, const DepthRange& _depthRange );
        void                            removeRegion( ObjectRef _ref );
        const RegionDataList*           getRegions( const DepthRange& _depthRange ) const;

    private:

        struct DepthRangeData
        {
            DepthRange      m_depthRange;
            RegionDataList  m_regions;
        };

        typedef ITF_VECTOR <DepthRangeData> DepthRangeDataList;

        DepthRangeDataList                   m_depthRanges;
    };
};

#endif //_ITF_REGIONSMANAGER_H__
