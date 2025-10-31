
#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_BTACTIONRUNDARKTOONIFIED_H_
#include "rayman/gameplay/ai/BTs/Ray_BTActionRunDarktoonified.h"
#endif // _ITF_RAY_BTACTIONRUNDARKTOONIFIED_H_

#ifndef _ITF_BEHAVIORTREE_H_
#include "gameplay/ai/BTs/BehaviorTree.h"
#endif // _ITF_BEHAVIORTREE_H_

#ifndef _ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_
#include "gameplay/Components/common/StickToPolylinePhysComponent.h"
#endif //_ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_

#ifndef _ITF_RAY_GROUNDAICONTROLLERCOMPONENT_H_
#include "rayman/gameplay/Components/ai/Ray_GroundAIControllerComponent.h"
#endif //_ITF_RAY_GROUNDAICONTROLLERCOMPONENT_H_

#ifndef _ITF_RAY_AIBTFACTS_H_
#include "rayman/gameplay/ai/BTs/Ray_AIBTFacts.h"
#endif //_ITF_RAY_AIBTFACTS_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(Ray_BTActionRunDarktoonified);

Ray_BTActionRunDarktoonified::Ray_BTActionRunDarktoonified()
: m_physComponent(NULL)
, m_animComponent(NULL)
, m_aiController(NULL)
, m_walkState(WALKSTATE_GROUND)
, m_swimPolyline()
, m_swimingSurfaceEdge(U32_INVALID)
, m_swimingSurfaceT(0.f)
, m_swimingClosestEdge(U32_INVALID)
, m_swimingClosestEdgeT(0.f)
, m_moveDir(Vec2d::Zero)
, m_blockedPolyline(ITF_INVALID_OBJREF)
, m_blockedEdgeIndex(U32_INVALID)
, m_crashedDist(0.f)
, m_runningTime(0.f)
, m_launchIdleTimer(0.f)
, m_stayIdleTimer(0.f)
, m_flyFallStayInAirTimer(0.f)
, m_swimingIsOnSurface(bfalse)
, m_swimingIsInside(bfalse)
, m_wasBlocked(bfalse)
{
}

void Ray_BTActionRunDarktoonified::init( BehaviorTree* _tree, const BTNode_Template* _template )
{
    Super::init(_tree,_template);

    m_physComponent = _tree->getActor()->GetComponent<StickToPolylinePhysComponent>();
    m_animComponent = _tree->getActor()->GetComponent<AnimatedComponent>();
    m_aiController = _tree->getActor()->GetComponent<Ray_GroundAIControllerComponent>();

    ACTOR_REGISTER_EVENT_COMPONENT(m_tree->getActor(),ITF_GET_STRINGID_CRC(EventBlockedByPolyline,2950236768), m_tree);
}

bbool Ray_BTActionRunDarktoonified::validate( Actor* _actor ) const
{
    bbool retVal = Super::validate(_actor);

    if ( !retVal )
    {
        return bfalse;
    }

    if ( !m_animComponent )
    {
        ITF_WARNING_PARAM(_actor,"AnimatedComponent",m_animComponent!=NULL,"%s: animated component is mandatory",StringID(GetObjectClassCRC()).getDebugString());
        return bfalse;
    }

    if ( !m_physComponent )
    {
        ITF_WARNING_PARAM(_actor,"stickToPolylinePhysComponent",m_physComponent!=NULL,"%s: stickToPolylinePhysComponent is mandatory",StringID(GetObjectClassCRC()).getDebugString());
        return bfalse;
    }

    if ( !m_aiController )
    {
        ITF_WARNING_PARAM(_actor,"Ray_groundAIControllerComponent",m_aiController!=NULL,"%s: Ray_groundAIControllerComponent is mandatory",StringID(GetObjectClassCRC()).getDebugString());
        return bfalse;
    }

    return btrue;
}

void Ray_BTActionRunDarktoonified::onActivate()
{
    Super::onActivate();

    m_walkState = WALKSTATE_UNKNOWN;
    m_currentRegions.clear();

    AIUtils::getPolylineRegions(m_tree->getActor()->getDepth(),m_tree->getActor()->get2DPos(),m_currentRegions);

    if ( getSwimPolyline() )
    {
        setState(WALKSTATE_SWIM);
    }
    else if ( m_physComponent->getStickedEdge() )
    {
        setState(WALKSTATE_GROUND);
    }
    else
    {
        setState(WALKSTATE_FALL);
    }

    if ( m_moveDir == Vec2d::Zero )
    {
        m_moveDir = AIUtils::getLookDir(m_tree->getActor(),m_tree->getActor()->isFlipped());
    }
}

void Ray_BTActionRunDarktoonified::onDeactivate()
{
    Super::onDeactivate();

    m_physComponent->setDisabled(bfalse);
    m_animComponent->resetLockMagicBox();
    m_physComponent->setGravityMultiplier(1.f);
}

bbool Ray_BTActionRunDarktoonified::update( f32 _dt )
{
    Super::update(_dt);

    m_currentRegions.clear();
    m_swimPolyline = NULL;

    AIUtils::getPolylineRegions(m_tree->getActor()->getDepth(),m_tree->getActor()->get2DPos(),m_currentRegions);

    // Iterate until set the current state
    while ( checkStateChange() )
        ;

    switch(m_walkState)
    {
        case WALKSTATE_FALL:
            updateFall(_dt);
            break;
        case WALKSTATE_SWIM:
            updateSwim(_dt);
            break;
        case WALKSTATE_GROUND:
            updateGround(_dt);
            break;
        case WALKSTATE_CRASHED:
            updateCrashed(_dt);
            break;
        case WALKSTATE_FLYFALL:
            updateFlyFall(_dt);
            break;
    }

    m_wasBlocked = bfalse;
    
    return m_tree->getBlackBoard().factExists(Ray_AIFact_darktoonified);
}

void Ray_BTActionRunDarktoonified::updateGround( f32 _dt )
{
    const PolyLineEdge* edge = m_physComponent->getStickedEdge();

    if ( !edge )
    {
        return;
    }

    Vec2d moveDir;
    Actor* myActor = m_tree->getActor();

    if ( m_stayIdleTimer )
    {
        moveDir = Vec2d::Zero;

        m_stayIdleTimer -= _dt;

        if ( m_stayIdleTimer <= 0.f )
        {
            m_stayIdleTimer = 0.f;
            m_launchIdleTimer = Seeder::getSharedSeeder().GetFloat(getTemplate()->getLaunchIdleMinTime(),getTemplate()->getLaunchIdleMaxTime());
        }
    }
    else
    {
        moveDir = edge->m_normalizedVector;

        if ( moveDir.dot(m_moveDir) < 0.f )
        {
            moveDir *= -1.f;
        }

        if ( m_launchIdleTimer )
        {
            m_launchIdleTimer -= _dt;

            if ( m_launchIdleTimer <= 0.f )
            {
                m_launchIdleTimer = 0.f;
                m_stayIdleTimer = Seeder::getSharedSeeder().GetFloat(getTemplate()->getStayIdleMinTime(),getTemplate()->getStayIdleMaxTime());
            }
        }
    }

    f32 xPos = m_physComponent->getCurrentGravityDir().getPerpendicular().dot(m_moveDir);
    bbool flipped = xPos < 0.f;

    if ( flipped != m_tree->getActor()->isFlipped() )
    {
        m_tree->getActor()->setFlipped(flipped);
    }

    m_runningTime += _dt;

    m_aiController->setMoveDir(moveDir);
    m_aiController->setSprint(btrue);

    if ( m_wasBlocked )
    {
        const BaseObject* obj = GETOBJECT(m_blockedPolyline);

        if ( obj )
        {
            const PolyLine* poly = obj->DynamicCast<PolyLine>(ITF_GET_STRINGID_CRC(PolyLine,1932163747));

            if ( poly )
            {
                const PolyLineEdge& edge = poly->getEdgeAt(m_blockedEdgeIndex);
                Vec2d dirToActor = myActor->get2DPos() - edge.getPos();

                m_crashedDist = edge.m_normalizedVector.dot(dirToActor);

                Vec2d normal = edge.m_normalizedVector.getPerpendicular();
                f32 targetAngle;

                f32 angle = getVec2Angle(normal);
                f32 difA = getShortestAngleDelta(myActor->getAngle(),angle);
                f32 difB = getShortestAngleDelta(myActor->getAngle(),angle+MTH_PI);

                if ( f32_Abs(difA) < f32_Abs(difB) )
                {
                    targetAngle = angle;
                }
                else
                {
                    targetAngle = angle + MTH_PI;
                }

                Vec2d dir = Vec2d::Right.Rotate(targetAngle);

                bbool lookRight = dir.dot(edge.m_normalizedVector.getPerpendicular()) < 0.f;

                m_tree->getActor()->setFlipped(!lookRight);
                myActor->setAngle(targetAngle);
                setState(WALKSTATE_CRASHED);
            }
        }
    }
    else if ( !getTemplate()->getCanRunInTheAir() && m_stayIdleTimer == 0.f )
    {
        static const f32 s_distanceCheck = 2.f;
        f32 remDistance = AIUtils::getRemainingDistanceOnEdge(myActor->get2DPos(),
                                                              m_physComponent->getCurrentGravityDir(),
                                                              m_physComponent->getStickedPolyline(),
                                                              m_physComponent->getStickedEdgeIndex(),
                                                              flipped,
                                                              m_physComponent->getRadius(),
                                                              s_distanceCheck);

        if ( remDistance < s_distanceCheck )
        {
            m_launchIdleTimer = 0.f;
            m_stayIdleTimer = Seeder::getSharedSeeder().GetFloat(getTemplate()->getStayIdleMinTime(),getTemplate()->getStayIdleMaxTime());
            myActor->setFlipped(!myActor->isFlipped());
            m_moveDir *= -1.f;
        }
    }
}

void Ray_BTActionRunDarktoonified::updateFall( f32 _dt )
{
    m_aiController->setMoveDir(m_moveDir);
}

void Ray_BTActionRunDarktoonified::updateSwim( f32 _dt )
{
    PolyLine* poly = AIUtils::getPolyLine(m_swimPolyline);

    if ( !poly )
    {
        m_swimPolyline = ITF_INVALID_OBJREF;
        setState(WALKSTATE_FALL);
        return;
    }

    AIUtils::getSwimingPolylineData(poly,m_tree->getActor()->getPos(),m_physComponent->getCurrentGravityDir(),1.f,
                                    m_swimingSurfaceEdge,m_swimingSurfaceT,m_swimingIsOnSurface,
                                    m_swimingClosestEdge,m_swimingClosestEdgeT,m_swimingIsInside);

    m_aiController->setSwimingPolylineData(poly->getRef(),m_swimingSurfaceEdge,m_swimingSurfaceT,m_swimingIsOnSurface);

    const PolyLineEdge* closestEdge = &poly->getEdgeAt(m_swimingClosestEdge);

    if ( !swimHandleInsideOutside(_dt,closestEdge) )
    {
        return;
    }

    m_aiController->setMoveDir(m_moveDir);
}

bbool Ray_BTActionRunDarktoonified::swimHandleInsideOutside( f32 _dt, const PolyLineEdge* _closestEdge )
{
    if ( !m_swimingIsInside )
    {
        // stay inside the water region
        Vec2d closestPoint = _closestEdge->getPos() + (_closestEdge->m_vector * m_swimingClosestEdgeT);
        static const f32 s_maxDistanceSq = (0.5f*0.5f);
        Actor* actor = m_tree->getActor();
        const Vec3d& currentPos = actor->getPos();

        f32 dist = ( currentPos.truncateTo2D() - closestPoint ).sqrnorm();

        if ( dist > s_maxDistanceSq )
        {
            setState(WALKSTATE_FALL);
            m_swimPolyline = ITF_INVALID_OBJREF;
            return bfalse;
        }

        Vec2d adjPos = AIUtils::findFreeCollisionPosition(closestPoint,m_physComponent->getRadius(),currentPos.m_z,btrue);

        actor->updateComponentPosAngle(adjPos.to3d(currentPos.m_z),actor->getAngle(),NULL);
    }

    return btrue;
}

void Ray_BTActionRunDarktoonified::updateCrashed( f32 _dt )
{
    const BaseObject* obj = GETOBJECT(m_blockedPolyline);

    if ( obj )
    {
        const PolyLine* poly = obj->DynamicCast<PolyLine>(ITF_GET_STRINGID_CRC(PolyLine,1932163747));

        if ( poly )
        {
            const PolyLineEdge& edge = poly->getEdgeAt(m_blockedEdgeIndex);
            Vec2d pos = edge.getPos() + ( edge.m_normalizedVector * m_crashedDist );

            m_animComponent->lockMagicBox(pos.to3d(m_tree->getActor()->getPos().m_z));
        }
    }
}

void Ray_BTActionRunDarktoonified::updateFlyFall( f32 _dt )
{
    bbool run = m_physComponent->getGravityMultiplier() == 0.f;
    
    if ( run )
    {
        if ( m_physComponent->getSpeed() == Vec2d::Zero )
        {
            if ( m_animComponent->isMainNodeFinished() )
            {
                m_physComponent->setGravityMultiplier(1.f);
            }
        }
        else
        {
            Vec2d currentSpeed = m_physComponent->getSpeed();

            currentSpeed *= getTemplate()->getFlyFallSpeedMultiplier();

            if ( currentSpeed.IsEqual(Vec2d::Zero,0.2f) )
            {
                m_animComponent->setAnim(getTemplate()->getFlyFallAnim());
                m_physComponent->setSpeed(Vec2d::Zero);
            }
            else
            {
                m_physComponent->setSpeed(currentSpeed);
            }
        }
    }
}

bbool Ray_BTActionRunDarktoonified::checkStateChange()
{
    bbool retValue = bfalse;
    
    PolyLine* swimPolyline = getSwimPolyline();

    if ( swimPolyline )
    {
        m_swimPolyline = swimPolyline->getRef();
    }

    switch(m_walkState)
    {
    case WALKSTATE_GROUND:
        {
            if ( swimPolyline )
            {
                setState(WALKSTATE_SWIM);
                retValue = btrue;
            }
            else if ( !m_physComponent->getStickedEdge() )
            {
                if ( m_runningTime > 1.f )
                {
                    setState(WALKSTATE_FLYFALL);
                    retValue = btrue;
                }
                else
                {
                    setState(WALKSTATE_FALL);
                    retValue = btrue;
                }
            }
        }
        break;
    case WALKSTATE_FALL:
        {
            if ( swimPolyline )
            {
                setState(WALKSTATE_SWIM);
                retValue = btrue;
            }
            else if ( m_physComponent->getStickedEdge() )
            {
                setState(WALKSTATE_GROUND);
                retValue = btrue;
            }
        }
        break;
    case WALKSTATE_SWIM:
        {
        }
        break;
    case WALKSTATE_CRASHED:
        {
            if ( m_animComponent->isMainNodeFinished() )
            {
                setState(WALKSTATE_GROUND);
                retValue = btrue;
            }
        }
        break;
    case WALKSTATE_FLYFALL:
        {
            if ( m_physComponent->getStickedEdge() )
            {
                setState(WALKSTATE_GROUND);
                retValue = btrue;
            }
        }
        break;
    }

    return retValue;
}

PolyLine* Ray_BTActionRunDarktoonified::getSwimPolyline() const
{
    u32 numRegions = m_currentRegions.size();

    for ( u32 i = 0; i < numRegions; i++ )
    {
        const AIUtils::PolylineRegionData& data = m_currentRegions[i];

        if ( data.m_regionId == s_RegionName_Swim )
        {
            return data.m_polyline;
        }
    }

    return NULL;
}

void Ray_BTActionRunDarktoonified::setState( WalkState _state )
{
    if ( m_walkState != _state )
    {
        // reset previous state
        switch(m_walkState)
        {
        case WALKSTATE_FALL:
            break;
        case WALKSTATE_SWIM:
            m_aiController->setSwimingPolylineData(ITF_INVALID_OBJREF);
            break;
        case WALKSTATE_CRASHED:
            m_physComponent->setDisabled(bfalse);
            m_animComponent->resetLockMagicBox();
            m_moveDir *= -1.f;
            break;
        case WALKSTATE_GROUND:
            break;
        case WALKSTATE_FLYFALL:
            m_physComponent->setGravityMultiplier(1.f);
            break;
        }

        m_walkState = _state;

        // set new state
        switch(m_walkState)
        {
        case WALKSTATE_FALL:
            m_animComponent->setAnim(getTemplate()->getFallAnim());
            break;
        case WALKSTATE_SWIM:
            m_animComponent->setAnim(getTemplate()->getSwimAnim());
            break;
        case WALKSTATE_CRASHED:
            m_physComponent->setDisabled(btrue);
            m_animComponent->setAnim(getTemplate()->getCrashAnim());
            break;
        case WALKSTATE_GROUND:
            {
                m_animComponent->setAnim(getTemplate()->getWalkAnim());
                m_runningTime = 0.f;
                m_launchIdleTimer = Seeder::getSharedSeeder().GetFloat(getTemplate()->getLaunchIdleMinTime(),getTemplate()->getLaunchIdleMaxTime());
            }
            break;
        case WALKSTATE_FLYFALL:
            {
                m_physComponent->setGravityMultiplier(0.f);
                m_flyFallStayInAirTimer = 0.f;
            }
            break;
        }
    }
}

void Ray_BTActionRunDarktoonified::updateAnimInput()
{
}

void Ray_BTActionRunDarktoonified::onEvent(Event * _event)
{
    Super::onEvent(_event);

    if ( EventBlockedByPolyline* blockedEvent = _event->DynamicCast<EventBlockedByPolyline>(ITF_GET_STRINGID_CRC(EventBlockedByPolyline,2950236768)) )
    {
        if ( m_walkState != WALKSTATE_CRASHED )
        {
            PolyLine* poly = NULL;
            const PolyLineEdge* edge = NULL;

            AIUtils::getPolyLine(blockedEvent->getPolyline(),blockedEvent->getEdge(),poly,edge);

            if ( edge )
            {
                f32 dotProd = edge->m_normalizedVector.getPerpendicular().dot(blockedEvent->getSpeed());

                // Make sure our speed was going quite opposite to the edge we are blocked by
                if ( dotProd < -1.f )
                {
                    m_wasBlocked = btrue;
                    m_blockedPolyline = blockedEvent->getPolyline();
                    m_blockedEdgeIndex = blockedEvent->getEdge();
                }
            }
        }
    }
}

IMPLEMENT_OBJECT_RTTI(Ray_BTActionRunDarktoonified_Template);

BEGIN_SERIALIZATION_CHILD(Ray_BTActionRunDarktoonified_Template)

    SERIALIZE_MEMBER("walkAnim",m_walkAnim);
    SERIALIZE_MEMBER("swimAnim",m_swimAnim);
    SERIALIZE_MEMBER("fallAnim",m_fallAnim);
    SERIALIZE_MEMBER("flyFallAnim",m_flyfallAnim);
    SERIALIZE_MEMBER("crashWallAnim",m_crashWallAnim);
    SERIALIZE_MEMBER("launchIdleMinTime",m_launchIdleMinTime);
    SERIALIZE_MEMBER("launchIdleMaxTime",m_launchIdleMaxTime);
    SERIALIZE_MEMBER("stayIdleMinTime",m_stayIdleMinTime);
    SERIALIZE_MEMBER("stayIdleMaxTime",m_stayIdleMaxTime);
    SERIALIZE_MEMBER("flyFallSpeedMultiplier",m_flyfallSpeedMultiplier);
    SERIALIZE_MEMBER("canRunInTheAir",m_runInTheAir);

END_SERIALIZATION()

Ray_BTActionRunDarktoonified_Template::Ray_BTActionRunDarktoonified_Template()
: m_launchIdleMinTime(1.f)
, m_launchIdleMaxTime(2.f)
, m_stayIdleMinTime(2.f)
, m_stayIdleMaxTime(3.f)
, m_flyfallSpeedMultiplier(0.9f)
, m_runInTheAir(bfalse)
{
}

}
