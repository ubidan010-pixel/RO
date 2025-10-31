
#include "precompiled_gameplay.h"

#ifndef _ITF_WAYPOINTSMANAGER_H_
#include "gameplay/managers/WaypointsManager.h"
#endif //_ITF_WAYPOINTSMANAGER_H_

#ifndef _ITF_ID_SERVER_H_
#include "core/IDserver.h"
#endif //_ITF_ID_SERVER_H_

#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_

namespace ITF
{

const f32 WaypointsManager::s_distCheckSq = 2.f;

WaypointsManager::WaypointsManager()
{
}

WaypointsManager::~WaypointsManager()
{
}

void WaypointsManager::addWaypoint( ObjectRef _ref, StringID _id, const DepthRange& _depthRange )
{
    DepthRangeWaypoints* depthRangeData = NULL;

    for ( u32 i = 0; i < m_depthRanges.size(); i++ )
    {
        if ( m_depthRanges[i].m_depthRange == _depthRange )
        {
            depthRangeData = &m_depthRanges[i];
            break;
        }
    }

    if ( !depthRangeData )
    {
        m_depthRanges.push_back(DepthRangeWaypoints());
        depthRangeData = &m_depthRanges.back();
        depthRangeData->m_depthRange = _depthRange;
    }

    depthRangeData->m_waypoints.push_back(WaypointData());

    WaypointData& waypointData = depthRangeData->m_waypoints.back();

    waypointData.m_actorRef = _ref;
    waypointData.m_id = _id;
}

void WaypointsManager::removeWaypoint( ObjectRef _ref )
{
    for ( u32 i = 0; i < m_depthRanges.size(); i++ )
    {
        DepthRangeWaypoints& depthRangeData = m_depthRanges[i];

        for ( WaypointList::iterator it = depthRangeData.m_waypoints.begin(); it != depthRangeData.m_waypoints.end(); ++it )
        {
            WaypointData& waypointData = *it;

            if ( waypointData.m_actorRef == _ref )
            {
                depthRangeData.m_waypoints.erase(it);
                break;
            }
        }
    }
}

Actor* WaypointsManager::getWaypoint( const Vec2d& _pos, StringID _id, const DepthRange& _depthRange ) const
{
    for ( u32 i = 0; i < m_depthRanges.size(); i++ )
    {
        const DepthRangeWaypoints& depthRangeData = m_depthRanges[i];

        if ( depthRangeData.m_depthRange == _depthRange )
        {
            for ( WaypointList::const_iterator it = depthRangeData.m_waypoints.begin(); it != depthRangeData.m_waypoints.end(); ++it )
            {
                const WaypointData& waypointData = *it;

                if ( _id.isValid() &&
                     _id != waypointData.m_id )
                {
                    continue;
                }
                
                BaseObject* obj = GETOBJECT(waypointData.m_actorRef);

                if ( obj )
                {
                    Actor* actor = obj->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));

                    if ( actor )
                    {
                        const Vec2d& actorPos = actor->get2DPos();
                        f32 distSq = ( _pos - actorPos ).sqrnorm();

                        if ( distSq < (s_distCheckSq*s_distCheckSq) )
                        {
                            return actor;
                        }
                    }
                }
            }

            break;
        }
    }

    return NULL;
}

//----------------------------------------------------------------------------------------------------------
Actor* WaypointsManager::getWaypoint( StringID _id ) const
{
	for ( u32 i = 0; i < m_depthRanges.size(); i++ )
	{
		const DepthRangeWaypoints& depthRangeData = m_depthRanges[i];

		for ( WaypointList::const_iterator it = depthRangeData.m_waypoints.begin(); it != depthRangeData.m_waypoints.end(); ++it )
		{
			const WaypointData& waypointData = *it;

			if ( _id.isValid() &&
				_id != waypointData.m_id )
			{
				continue;
			}

			BaseObject* obj = GETOBJECT(waypointData.m_actorRef);

			if ( obj )
			{
				return obj->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));
			}
		}
	}
	return NULL;
}

}