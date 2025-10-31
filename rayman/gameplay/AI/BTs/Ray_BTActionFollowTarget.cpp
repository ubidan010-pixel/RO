
#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_BTACTIONFOLLOWTARGET_H_
#include "rayman/gameplay/ai/BTs/Ray_BTActionFollowTarget.h"
#endif // _ITF_RAY_BTACTIONFOLLOWTARGET_H_

#ifndef _ITF_RAY_GROUNDAICONTROLLERCOMPONENT_H_
#include "rayman/gameplay/Components/ai/Ray_GroundAIControllerComponent.h"
#endif //_ITF_RAY_GROUNDAICONTROLLERCOMPONENT_H_

#ifndef _ITF_BEHAVIORTREE_H_
#include "gameplay/ai/BTs/BehaviorTree.h"
#endif // _ITF_BEHAVIORTREE_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_RAY_AINETWORKMANAGER_H_
#include "rayman/gameplay/ai/Network/Ray_AINetworkManager.h"
#endif //_ITF_RAY_AINETWORKMANAGER_H_

#ifndef _ITF_RAY_AIBTFACTS_H_
#include "rayman/gameplay/ai/BTs/Ray_AIBTFacts.h"
#endif //_ITF_RAY_AIBTFACTS_H_

#ifndef _ITF_RAY_AINETWORKUTILS_H_
#include "rayman/gameplay/ai/Network/Ray_AINetworkUtils.h"
#endif //_ITF_RAY_AINETWORKUTILS_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(Ray_BTActionFollowTarget);

const f32 Ray_BTActionFollowTarget::s_searchMinTime = 0.5f;
const f32 Ray_BTActionFollowTarget::s_searchMaxnTime = 1.f;

Ray_BTActionFollowTarget::Ray_BTActionFollowTarget()
: m_searchTimer(0.f)
, m_jumping(bfalse)
{
}

Ray_BTActionFollowTarget::~Ray_BTActionFollowTarget()
{
}

bbool Ray_BTActionFollowTarget::validate( Actor* _actor ) const
{
    bbool retVal = Super::validate(_actor);

    if ( !retVal )
    {
        return bfalse;
    }

    if ( !getTemplate()->getFollowTarget().isValid() )
    {
        ITF_WARNING_PARAM(_actor,"followTarget",0,"%s: the fact to retrieve the actor to follow is not set",StringID(GetObjectClassCRC()).getDebugString());
        return bfalse;
    }

    return btrue;
}

bbool Ray_BTActionFollowTarget::decide( f32 _dt )
{
    m_finalTarget = ITF_INVALID_OBJREF;

    m_tree->getBlackBoard().getFact(getTemplate()->getFollowTarget(),m_finalTarget);

    if ( m_finalTarget == ITF_INVALID_OBJREF )
    {
        return bfalse;
    }

    setCurrentTargetPos();

    return m_currentTarget != ITF_INVALID_OBJREF;
}

void Ray_BTActionFollowTarget::onActivate()
{
    Super::onActivate();

    m_jumping = bfalse;
    setSearchTime();
}

void Ray_BTActionFollowTarget::setSearchTime()
{
    m_searchTimer = Seeder::getSharedSeeder().GetFloat(s_searchMinTime,s_searchMaxnTime);
}

bbool Ray_BTActionFollowTarget::update( f32 _dt )
{
    if ( m_jumping )
    {
        bbool finished = m_tree->getActor()->getPos().IsEqual(m_prevJumpTarget,MTH_EPSILON);
        Actor* jumpTarget = AIUtils::getActor(m_currentTarget);
        Vec3d targetP;

        if ( jumpTarget )
        {
            targetP = jumpTarget->getPos();
        }
        else
        {
            targetP = m_prevJumpTarget;
        }

        m_aiController->jumpToTarget(targetP);
        m_prevJumpTarget = targetP;

        if ( finished )
        {
            m_jumping = bfalse;
            setCurrentTargetPos();
            setSearchTime();
        }

        return btrue;
    }
    else
    {
        m_finalTarget = ITF_INVALID_OBJREF;
        m_tree->getBlackBoard().getFact(getTemplate()->getFollowTarget(),m_finalTarget);

        if ( m_finalTarget == ITF_INVALID_OBJREF )
        {
            return bfalse;
        }

        m_searchTimer -= _dt;

        if ( m_searchTimer <= 0.f )
        {
            setCurrentTargetPos();
            setSearchTime();
        }

        return Super::update(_dt);
    }
}

void Ray_BTActionFollowTarget::setCurrentTargetPos()
{
    // By default, use the target actor
    m_currentTarget = m_finalTarget;

    ObjectRef targetWaypoint = RAY_AINETWORKMANAGER->getWaypointFromActor(m_finalTarget);

    if ( targetWaypoint != ITF_INVALID_OBJREF )
    {
        ObjectRef myWaypoint = ITF_INVALID_OBJREF;

        m_tree->getBlackBoard().getFact(Ray_AIFact_currentWaypoint,myWaypoint);

        if ( myWaypoint != ITF_INVALID_OBJREF && targetWaypoint != myWaypoint )
        {
            Actor* currentWaypointActor = AIUtils::getActor(myWaypoint);
            Actor* targetWaypointActor = AIUtils::getActor(targetWaypoint);

            if ( currentWaypointActor != NULL && targetWaypoint != NULL )
            {
                Ray_AINetworkUtils::WaypointPath path;

                if ( Ray_AINetworkUtils::pathFind(currentWaypointActor,targetWaypointActor,path) )
                {
                    Ray_AINetworkUtils::PathWaypointInfo& fromInfo = path[0];
                    Ray_AINetworkUtils::PathWaypointInfo& toInfo = path[1];

                    if ( fromInfo.m_linkType == Ray_AINetworkUtils::WAYPOINTLINKTYPE_JUMP ||
                         fromInfo.m_linkType == Ray_AINetworkUtils::WAYPOINTLINKTYPE_WALLJUMP )
                    {
                        f32 distToMyWaypoint;
                        
                        const PolyLineEdge* currentEdge = m_physComponent->getStickedEdge();
                        
                        if ( currentEdge )
                        {
                            Vec2d vecToTarget = currentWaypointActor->get2DPos() - currentEdge->getPos();
                            f32 dist = currentEdge->m_normalizedVector.dot(vecToTarget);
                            Vec2d posOnGround = currentEdge->getPos() + (currentEdge->m_normalizedVector*dist);

                            distToMyWaypoint = ( posOnGround - m_tree->getActor()->get2DPos() ).sqrnorm();
                        }
                        else
                        {
                            distToMyWaypoint = ( currentWaypointActor->get2DPos() - m_tree->getActor()->get2DPos() ).sqrnorm();
                        }

                        if ( distToMyWaypoint < 1.f )
                        {
                            // jump to target
                            m_jumping = btrue;
                            m_prevJumpTarget = currentWaypointActor->getPos();
                            m_animComponent->setAnim(getTemplate()->getJumpAnim());
                            m_aiController->jumpToTarget(targetWaypointActor->getPos());
                            m_currentTarget = toInfo.m_obj->getRef();
                        }
                        else
                        {
                            m_currentTarget = fromInfo.m_obj->getRef();
                        }
                    }
                    else
                    {
                        m_currentTarget = toInfo.m_obj->getRef();
                    }
                }
            }
        }
    }

    m_tree->getBlackBoard().setFact(getTemplate()->getFactTargetActor(),m_currentTarget);
}

IMPLEMENT_OBJECT_RTTI(Ray_BTActionFollowTarget_Template);

BEGIN_SERIALIZATION_CHILD(Ray_BTActionFollowTarget_Template)

    SERIALIZE_MEMBER("range",m_range);
    SERIALIZE_MEMBER("followTarget",m_followTarget);

END_SERIALIZATION()

Ray_BTActionFollowTarget_Template::Ray_BTActionFollowTarget_Template()
: m_range(0.5f)
{
}

}