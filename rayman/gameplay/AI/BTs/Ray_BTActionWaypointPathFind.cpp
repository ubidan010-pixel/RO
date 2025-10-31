
#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_BTACTIONWAYPOINTPATHFIND_H_
#include "rayman/gameplay/ai/BTs/Ray_BTActionWaypointPathFind.h"
#endif // _ITF_RAY_BTACTIONWAYPOINTPATHFIND_H_

#ifndef _ITF_BEHAVIORTREE_H_
#include "gameplay/ai/BTs/BehaviorTree.h"
#endif // _ITF_BEHAVIORTREE_H_

#ifndef _ITF_RAY_AIBTFACTS_H_
#include "rayman/gameplay/ai/BTs/Ray_AIBTFacts.h"
#endif //_ITF_RAY_AIBTFACTS_H_

#ifndef _ITF_AIBTFACTS_H_
#include "gameplay/ai/BTs/AIBTFacts.h"
#endif //_ITF_AIBTFACTS_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(Ray_BTActionWaypointPathFind);

Ray_BTActionWaypointPathFind::Ray_BTActionWaypointPathFind()
{
}

void Ray_BTActionWaypointPathFind::onActivate()
{
    // Do we have a current waypoint?
    // If we don't, we walk blindly in the direction of the target
    // If we do:
    //      find a path between the current waypoint and the target waypoint
    //      if a path doesn't exist we walk blindly in the direction of the target
    //      if a path exist, check the connection to the next waypoint
    //              if it's a jump, we need to move back to the current waypoint and jump from there
    //              if it's a walk, then we walk directly to the next waypoint
    ObjectRef currentWaypoint = ITF_INVALID_OBJREF;
    ObjectRef targetWaypoint = ITF_INVALID_OBJREF;

    m_tree->getBlackBoard().getFact(Ray_AIFact_currentWaypoint,currentWaypoint);
    m_tree->getBlackBoard().getFact(Ray_AIFact_targetWaypoint,targetWaypoint);

    Actor* currentWaypointActor = AIUtils::getActor(currentWaypoint);
    Actor* targetWaypointActor = AIUtils::getActor(targetWaypoint);

    ITF_ASSERT_MSG(targetWaypointActor,"Starting pathfind with no target waypoint");

    if ( !targetWaypointActor )
    {
        return;
    }

    if ( !currentWaypointActor )
    {
        setObjective(targetWaypointActor,Ray_AINetworkUtils::WAYPOINTLINKTYPE_WALK);
    }
    else
    {
        Ray_AINetworkUtils::WaypointPath path;

        if ( Ray_AINetworkUtils::pathFind(currentWaypointActor,targetWaypointActor,path) )
        {
            Ray_AINetworkUtils::PathWaypointInfo& fromInfo = path[0];
            Ray_AINetworkUtils::PathWaypointInfo& toInfo = path[1];

            setObjective(toInfo.m_obj,fromInfo.m_linkType);
        }
        else
        {
            setObjective(targetWaypointActor,Ray_AINetworkUtils::WAYPOINTLINKTYPE_WALK);
        }
    }
}

bbool Ray_BTActionWaypointPathFind::isAtWaypoint( Actor* _waypoint ) const
{
    StickToPolylinePhysComponent* physComponent = m_tree->getActor()->GetComponent<StickToPolylinePhysComponent>();
    static const f32 distCheckSq = 0.1f;
    Vec2d currentP = m_tree->getActor()->get2DPos();
    Vec2d targetP = _waypoint->get2DPos();

    if ( physComponent )
    {
        const PolyLineEdge* edge = physComponent->getStickedEdge();

        if ( edge )
        {
            Vec2d vecToCurrent = currentP - edge->getPos();
            Vec2d vecToTarget = targetP - edge->getPos();
            f32 distToCurrent = edge->m_normalizedVector.dot(vecToCurrent);
            f32 distToTarget = edge->m_normalizedVector.dot(vecToTarget);
            
            return f32_Abs( distToTarget - distToCurrent ) < distCheckSq;
        }
    }

    return ( currentP - targetP ).sqrnorm() <= (distCheckSq*distCheckSq);
}

void Ray_BTActionWaypointPathFind::setObjective( Pickable* _objective, Ray_AINetworkUtils::WaypointLinkType _linkType )
{
    m_tree->getBlackBoard().setFact(AIFact_navTargetPos,_objective->get2DPos());
    m_tree->getBlackBoard().setFact(AIFact_navTargetActor,_objective->getRef());
    m_tree->getBlackBoard().setFact(Ray_AIFact_waypointLinkType,static_cast<i32>(_linkType));
}

bbool Ray_BTActionWaypointPathFind::decide( f32 _dt )
{
    if ( m_tree->getBlackBoard().factExists(Ray_AIFact_waypointChanged) )
    {
        ObjectRef targetWaypoint = ITF_INVALID_OBJREF;

        m_tree->getBlackBoard().getFact(Ray_AIFact_targetWaypoint,targetWaypoint);

        return AIUtils::getActor(targetWaypoint) != NULL;
    }
    else
    {
        return bfalse;
    }
}

bbool Ray_BTActionWaypointPathFind::update( f32 _dt )
{
    Super::update(_dt);

    return bfalse;
}

IMPLEMENT_OBJECT_RTTI(Ray_BTActionWaypointPathFind_Template);

BEGIN_SERIALIZATION_CHILD(Ray_BTActionWaypointPathFind_Template)

END_SERIALIZATION()

Ray_BTActionWaypointPathFind_Template::Ray_BTActionWaypointPathFind_Template()
{
}

}