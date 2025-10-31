#ifndef _ITF_WAYPOINTSMANAGER_H_
#define _ITF_WAYPOINTSMANAGER_H_

#ifndef _ITF_VEC2D_H_
#include "core/math/vec2d.h"
#endif //_ITF_VEC2D_H_

namespace ITF
{
    class WaypointsManager
    {
    public:

        WaypointsManager();
        ~WaypointsManager();

        void                            addWaypoint( ObjectRef _ref, StringID _id, const DepthRange& _depthRange );
        void                            removeWaypoint( ObjectRef _ref );

        class Actor*                    getWaypoint( const Vec2d& _pos, StringID _id, const DepthRange& _depthRange ) const;
		class Actor*                    getWaypoint( StringID _id ) const;

    private:

        struct WaypointData
        {
            ObjectRef                   m_actorRef;
            StringID                    m_id;
        };
#ifndef ITF_IPAD
        typedef ITF_LIST <WaypointData> WaypointList;
#else
		typedef std::list <WaypointData> WaypointList;
#endif

        struct DepthRangeWaypoints
        {
            DepthRange                  m_depthRange;
            WaypointList                m_waypoints;
        };
        
        typedef ITF_VECTOR <DepthRangeWaypoints> DepthRangeArray;

        DepthRangeArray                 m_depthRanges;

        static const f32                s_distCheckSq;
    };
};

#endif //_ITF_WAYPOINTSMANAGER_H_