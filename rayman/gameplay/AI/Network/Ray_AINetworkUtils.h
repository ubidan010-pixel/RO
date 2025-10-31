
#ifndef _ITF_RAY_AINETWORKUTILS_H_
#define _ITF_RAY_AINETWORKUTILS_H_

namespace ITF
{

namespace Ray_AINetworkUtils
{
    enum WaypointLinkType
    {
        WAYPOINTLINKTYPE_WALK = 0,
        WAYPOINTLINKTYPE_JUMP = 1,
        WAYPOINTLINKTYPE_WALLJUMP = 2,
        WAYPOINTLINKTYPE_WALLRUN = 3,
    };

    struct PathWaypointInfo
    {
        Pickable*           m_obj;
        WaypointLinkType    m_linkType;
    };

    typedef FixedArray <PathWaypointInfo,64> WaypointPath;

    bbool       pathFind( Actor* _start, Pickable* _end, WaypointPath& _path );
}

}

#endif // _ITF_RAY_AINETWORKUTILS_H_

