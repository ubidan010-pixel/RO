
#include "precompiled_gameplay.h"

#ifndef _ITF_BTACTIONWALKTOTARGET_H_
#include "gameplay/ai/BTs/BTActionWalkToTarget.h"
#endif // _ITF_BTACTIONWALKTOTARGET_H_

#ifndef _ITF_BEHAVIORTREE_H_
#include "gameplay/ai/BTs/BehaviorTree.h"
#endif // _ITF_BEHAVIORTREE_H_

#ifndef _ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_
#include "gameplay/Components/common/StickToPolylinePhysComponent.h"
#endif //_ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_

#ifndef _ITF_GROUNDAICONTROLLERCOMPONENT_H_
#include "gameplay/Components/AI/GroundAIControllerComponent.h"
#endif //_ITF_GROUNDAICONTROLLERCOMPONENT_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(BTActionWalkToTarget);

BTActionWalkToTarget::BTActionWalkToTarget()
: m_physComponent(NULL)
, m_animComponent(NULL)
, m_aiController(NULL)
, m_walkState(WALKSTATE_GROUND)
, m_jumpOutOfWaterCounter(0.f)
, m_swimPolyline(ITF_INVALID_OBJREF)
, m_swimingSurfaceEdge(U32_INVALID)
, m_swimingSurfaceT(0.f)
, m_swimingClosestEdge(U32_INVALID)
, m_swimingClosestEdgeT(0.f)
, m_swimingIsOnSurface(bfalse)
, m_swimingIsInside(bfalse)
, m_doingUturn(bfalse)
{
}

void BTActionWalkToTarget::init( BehaviorTree* _tree, const BTNode_Template* _template )
{
    Super::init(_tree,_template);

    m_physComponent = _tree->getActor()->GetComponent<StickToPolylinePhysComponent>();
    m_animComponent = _tree->getActor()->GetComponent<AnimLightComponent>();
    m_aiController = _tree->getActor()->GetComponent<GroundAIControllerComponent>();
}

bbool BTActionWalkToTarget::validate( Actor* _actor ) const
{
    bbool retVal = Super::validate(_actor);

    if ( !retVal )
    {
        return bfalse;
    }

    if ( !m_animComponent )
    {
        ITF_WARNING_PARAM(_actor,"AnimLightComponent",m_animComponent!=NULL,"%s: anim component is mandatory",StringID(GetObjectClassCRC()).getDebugString());
        return bfalse;
    }

    if ( !m_physComponent )
    {
        ITF_WARNING_PARAM(_actor,"stickToPolylinePhysComponent",m_physComponent!=NULL,"%s: stickToPolylinePhysComponent is mandatory",StringID(GetObjectClassCRC()).getDebugString());
        return bfalse;
    }

    if ( !m_aiController )
    {
        ITF_WARNING_PARAM(_actor,"groundAIControllerComponent",m_aiController!=NULL,"%s: groundAIControllerComponent is mandatory",StringID(GetObjectClassCRC()).getDebugString());
        return bfalse;
    }

    if ( !getTemplate()->getFactTargetActor().isValid() && !getTemplate()->getFactTargetPos().isValid() )
    {
        ITF_WARNING_PARAM(_actor,"factTargetActor/factTargetPos",0,"%s: the fact to retrieve the position is not set",StringID(GetObjectClassCRC()).getDebugString());
        return bfalse;
    }

    return btrue;
}

bbool BTActionWalkToTarget::decide( f32 _dt )
{
    bbool actorExists = getTemplate()->getFactTargetActor().isValid() && m_tree->getBlackBoard().factExists(getTemplate()->getFactTargetActor());
    bbool posExists = getTemplate()->getFactTargetPos().isValid() && m_tree->getBlackBoard().factExists(getTemplate()->getFactTargetPos());
    
    if ( !actorExists && !posExists )
    {
        return bfalse;
    }

    const PolyLineEdge* currentEdge = m_physComponent->getStickedEdge();

    Vec2d targetP = getTargetPoint(currentEdge);

    if ( targetP == Vec2d::Zero )
    {
        return bfalse;
    }

    Vec2d currentP = getCurrentPoint(currentEdge);

    f32 distToTargetSq = ( currentP - targetP ).sqrnorm();

    if ( distToTargetSq > 0.1f )
    {
        if ( currentEdge )
        {
            return btrue;
        }
        else
        {
            Vec2d dirToPoint = targetP - currentP;

            dirToPoint.normalize();

            f32 dotProd = dirToPoint.dot(m_physComponent->getCurrentGravityDir());

            if ( fabs(dotProd) > 0.9f )
            {
                return bfalse;
            }
            else
            {
                return btrue;
            }
        }
    }
    else
    {
        return bfalse;
    }
}

void BTActionWalkToTarget::onActivate()
{
    Super::onActivate();

    m_doingUturn = bfalse;
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
}

bbool BTActionWalkToTarget::update( f32 _dt )
{
    Super::update(_dt);

    bbool retValue = btrue;

    if ( m_jumpOutOfWaterCounter )
    {
        m_jumpOutOfWaterCounter = Max(m_jumpOutOfWaterCounter-_dt,0.f);
    }

    m_currentRegions.clear();
    m_swimPolyline = ITF_INVALID_OBJREF;

    AIUtils::getPolylineRegions(m_tree->getActor()->getDepth(),m_tree->getActor()->get2DPos(),m_currentRegions);

    // Iterate until set the current state
    while ( checkStateChange() )
        ;

    switch(m_walkState)
    {
        case WALKSTATE_FALL:
            retValue = updateFall(_dt);
            break;
        case WALKSTATE_JUMP:
            retValue = updateJump(_dt);
            break;
        case WALKSTATE_SWIM:
            retValue = updateSwim(_dt);
            break;
        case WALKSTATE_GROUND:
            retValue = updateGround(_dt);
            break;
    }
    
    return retValue;
}

bbool BTActionWalkToTarget::updateGround( f32 _dt )
{
    if ( m_doingUturn )
    {
        if ( m_animComponent->isSubAnimFinished() )
        {
            resetUTurn();
        }
        else
        {
            return btrue;
        }
    }

    const PolyLineEdge* edge = m_physComponent->getStickedEdge();

    if ( !edge )
    {
        return bfalse;
    }

    Vec2d targetP = getTargetPoint(edge);

    if ( targetP == Vec2d::Zero )
    {
        return bfalse;
    }

    Vec2d dirToTarget = targetP - m_tree->getActor()->get2DPos();
    Vec2d moveDir = edge->m_normalizedVector;
    bbool retVal;

    if ( moveDir.dot(dirToTarget) < 0.f )
    {
        moveDir *= -1.f;
    }

    if ( updateCheckContactsBlocking(moveDir) )
    {
        m_aiController->setJump();
        setState(WALKSTATE_JUMP);
        retVal = btrue;
    }
    else
    {
        Vec2d currentP = getCurrentPoint(edge);
        f32 distToTarget = ( targetP - currentP ).norm();
        f32 totalSpeed = edge->m_normalizedVector.dot(m_physComponent->getSpeed());
        f32 walkDist = f32_Abs(totalSpeed*_dt);

        if ( distToTarget < 0.05f || distToTarget < walkDist )
        {
            m_physComponent->setSpeed(Vec2d::Zero);
            moveDir = Vec2d::Zero;
            retVal = bfalse;

            Vec2d targetPos = targetP + ( edge->m_normalizedVector.getPerpendicular() * m_physComponent->getRadius() );

            Actor* myActor = m_tree->getActor();
            myActor->updateComponentPosAngle(targetPos.to3d(myActor->getPos().m_z),myActor->getAngle(),NULL);
        }
        else
        {
            f32 xPos = m_physComponent->getCurrentGravityDir().getPerpendicular().dot(dirToTarget);
            bbool flipped = xPos < 0.f;

            if ( flipped != m_tree->getActor()->isFlipped() )
            {
                if ( getTemplate()->getCanPerformTurn() )
                {
                    m_doingUturn = btrue;
                    moveDir = Vec2d::Zero;
                }
                else
                {
                    m_tree->getActor()->setFlipped(flipped);
                }
            }

            retVal = btrue;
        }
    }

    m_aiController->setMoveDir(moveDir);

    return retVal;
}

bbool BTActionWalkToTarget::updateCheckContactsBlocking( const Vec2d& _moveDir )
{
    // We check if we are hitting a wall, and if we do we will try to jump over it
    bbool foundContact = bfalse;
    const PhysContactsContainer& contacts = m_physComponent->getContacts();
    u32 numContacts = contacts.size();

    for ( u32 i = 0; i < numContacts; i++ )
    {
        const SCollidableContact& contact = contacts[i];

        if ( contact.m_edgeIndex == U32_INVALID )
        {
            continue;
        }

        f32 dotProd = contact.m_normal.dot(_moveDir);

        if ( dotProd < 0.5f )
        {
            continue;
        }

        // A contact is blocking us, we should try to jump over it
        foundContact = btrue;
        break;
    }

    return foundContact;
}

bbool BTActionWalkToTarget::updateFall( f32 _dt )
{
    Vec2d targetP = getTargetPoint(NULL);
    Vec2d dirToTarget = targetP - m_tree->getActor()->get2DPos();
    f32 distToTarget = dirToTarget.norm();

    if ( distToTarget > m_physComponent->getRadius() )
    {
        dirToTarget.normalize();
        m_aiController->setMoveDir(dirToTarget);
    }

    return btrue;
}

bbool BTActionWalkToTarget::updateJump( f32 _dt )
{
    Vec2d targetP = getTargetPoint(NULL);
    Vec2d dirToTarget = targetP - m_tree->getActor()->get2DPos();
    f32 distToTarget = dirToTarget.norm();

    if ( distToTarget > m_physComponent->getRadius() )
    {
        dirToTarget.normalize();
        m_aiController->setMoveDir(dirToTarget);

        if ( updateCheckContactsBlocking(dirToTarget) )
        {
            m_aiController->setJump();
        }
    }

    return btrue;
}

bbool BTActionWalkToTarget::updateSwim( f32 _dt )
{
    PolyLine* poly = AIUtils::getPolyLine(m_swimPolyline);

    if ( !poly )
    {
        m_swimPolyline = ITF_INVALID_OBJREF;
        setState(WALKSTATE_FALL);
        return btrue;
    }

    AIUtils::getSwimingPolylineData(poly,m_tree->getActor()->getPos(),m_physComponent->getCurrentGravityDir(),1.f,
                                    m_swimingSurfaceEdge,m_swimingSurfaceT,m_swimingIsOnSurface,
                                    m_swimingClosestEdge,m_swimingClosestEdgeT,m_swimingIsInside);

    m_aiController->setSwimingPolylineData(poly->getRef(),m_swimingSurfaceEdge,m_swimingSurfaceT,m_swimingIsOnSurface);

    const PolyLineEdge* closestEdge = &poly->getEdgeAt(m_swimingClosestEdge);

    if ( !swimHandleInsideOutside(_dt,closestEdge) )
    {
        return btrue;
    }

    Vec2d targetP = getTargetPoint(NULL);
    Vec2d moveDir = targetP - m_tree->getActor()->get2DPos();
    f32 distToTarget = moveDir.norm();

    moveDir.normalize();

    f32 swimDist = moveDir.dot(m_physComponent->getSpeed())*_dt;

    if ( swimDist < distToTarget )
    {
        if ( !swimAdjustMoveToCollisions(_dt,moveDir) )
        {
            m_aiController->setMoveDir(moveDir);
        }

        return btrue;
    }
    else
    {
        return bfalse;
    }
}

bbool BTActionWalkToTarget::swimHandleInsideOutside( f32 _dt, const PolyLineEdge* _closestEdge )
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

bbool BTActionWalkToTarget::swimAdjustMoveToCollisions( f32 _dt, Vec2d& _moveDir )
{
    // We check if we are hitting a wall,
    // we may want to continue swiming along the wall
    // or jump directly out of the water if we are on the surface
    bbool exitState = bfalse;
    const PhysContactsContainer& contacts = m_physComponent->getContacts();
    u32 numContacts = contacts.size();

    for ( u32 i = 0; i < numContacts; i++ )
    {
        const SCollidableContact& contact = contacts[i];

        if ( contact.m_edgeIndex == U32_INVALID )
        {
            continue;
        }

        f32 dotProd = contact.m_normal.dot(_moveDir);

        if ( dotProd < 0.5f )
        {
            continue;
        }

        if ( m_swimingIsOnSurface )
        {
            // A contact is blocking us and we are on the surface, jump out of the water
            m_aiController->setJump();
            setState(WALKSTATE_JUMP);
            exitState = btrue;
            break;
        }

        // Move along the polyline towards the surface
        _moveDir = -m_physComponent->getCurrentGravityDir();
        break;
    }

    return exitState;
}

bbool BTActionWalkToTarget::checkStateChange()
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
                setState(WALKSTATE_FALL);
                retValue = btrue;
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
    case WALKSTATE_JUMP:
        {
            if ( !m_jumpOutOfWaterCounter &&
                 swimPolyline )
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
    }

    return retValue;
}

PolyLine* BTActionWalkToTarget::getSwimPolyline() const
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

void BTActionWalkToTarget::setState( WalkState _state )
{
    if ( m_walkState != _state )
    {
        resetUTurn();

        // reset previous state
        switch(m_walkState)
        {
        case WALKSTATE_JUMP:
            break;
        case WALKSTATE_FALL:
            break;
        case WALKSTATE_SWIM:
            m_aiController->setSwimingPolylineData(ITF_INVALID_OBJREF);

            if ( _state == WALKSTATE_JUMP )
            {
                m_jumpOutOfWaterCounter = 0.5f;
            }
            break;
        case WALKSTATE_GROUND:
            m_aiController->setRunOnWalls(bfalse);

            if ( getTemplate()->getForceSprint() )
            {
                m_aiController->setSprint(bfalse);
            }
            break;
        default:
            break;
        }

        m_walkState = _state;

        // set new state
        switch(m_walkState)
        {
        case WALKSTATE_JUMP:
            m_animComponent->setAnim(getTemplate()->getJumpAnim());
            break;
        case WALKSTATE_FALL:
            m_animComponent->setAnim(getTemplate()->getFallAnim());
            break;
        case WALKSTATE_SWIM:
            m_animComponent->setAnim(getTemplate()->getSwimAnim());
            break;
        case WALKSTATE_GROUND:
        default:
            m_aiController->setRunOnWalls(getTemplate()->getWallRun());
            m_animComponent->setAnim(getTemplate()->getWalkAnim());

            if ( getTemplate()->getForceSprint() )
            {
                m_aiController->setSprint(btrue);
            }
            break;
        }
    }
}

void BTActionWalkToTarget::resetUTurn()
{
    if ( m_doingUturn )
    {
        m_doingUturn = bfalse;
        m_tree->getActor()->setFlipped(!m_tree->getActor()->isFlipped());
    }
}

Vec2d BTActionWalkToTarget::getTargetPoint( const PolyLineEdge* _currentEdge ) const
{
    Vec2d targetPos = Vec2d::Zero;
    ObjectRef targetActor = ITF_INVALID_OBJREF;

    if ( getTemplate()->getFactTargetActor().isValid() )
    {
        m_tree->getBlackBoard().getFact(getTemplate()->getFactTargetActor(),targetActor);
    }

    if ( targetActor != ITF_INVALID_OBJREF )
    {
        Actor* actor = AIUtils::getActor(targetActor);

        if ( actor )
        {
            targetPos = actor->get2DPos();
        }
        else
        {
            if ( getTemplate()->getFactTargetPos().isValid() )
            {
                m_tree->getBlackBoard().getFact(getTemplate()->getFactTargetPos(),targetPos);
            }
        }
    }
    else
    {
        if ( getTemplate()->getFactTargetPos().isValid() )
        {
            m_tree->getBlackBoard().getFact(getTemplate()->getFactTargetPos(),targetPos);
        }
    }

    if ( targetPos != Vec2d::Zero )
    {
        if ( _currentEdge )
        {
            Vec2d vecToTarget = targetPos - _currentEdge->getPos();
            Vec2d dirToPos = vecToTarget;

            dirToPos.normalize();

            if ( _currentEdge->m_normalizedVector.getPerpendicular().dot(dirToPos) >= 0.f )
            {
                f32 dist = _currentEdge->m_normalizedVector.dot(vecToTarget);
                targetPos = _currentEdge->getPos() + (_currentEdge->m_normalizedVector*dist);
            }
        }
    }

    return targetPos;
}

Vec2d BTActionWalkToTarget::getCurrentPoint( const PolyLineEdge* _currentEdge ) const
{
    Vec2d currentP = m_tree->getActor()->get2DPos();

    if ( _currentEdge )
    {
        Vec2d vecToTarget = currentP - _currentEdge->getPos();
        f32 dist = _currentEdge->m_normalizedVector.dot(vecToTarget);
        currentP = _currentEdge->getPos() + (_currentEdge->m_normalizedVector*dist);
    }

    return currentP;
}

void BTActionWalkToTarget::updateAnimInput()
{
    if ( AnimatedComponent* animComponent = m_animComponent->DynamicCast<AnimatedComponent>(ITF_GET_STRINGID_CRC(AnimatedComponent,1654726928)) )
    {
        animComponent->setInput(ITF_GET_STRINGID_CRC(performingUTurn,1740082645),m_doingUturn);
    }
}

IMPLEMENT_OBJECT_RTTI(BTActionWalkToTarget_Template);

BEGIN_SERIALIZATION_CHILD(BTActionWalkToTarget_Template)

    SERIALIZE_MEMBER("walkAnim",m_walkAnim);
    SERIALIZE_MEMBER("swimAnim",m_swimAnim);
    SERIALIZE_MEMBER("fallAnim",m_fallAnim);
    SERIALIZE_MEMBER("jumpAnim",m_jumpAnim);
    SERIALIZE_MEMBER("factTargetActor",m_factTargetActor);
    SERIALIZE_MEMBER("factTargetPos",m_factTargetPos);
    SERIALIZE_MEMBER("canPerformTurn",m_performTurn);
    SERIALIZE_MEMBER("wallRun",m_wallRun);
    SERIALIZE_MEMBER("forceSprint",m_forceSprint);

END_SERIALIZATION()

BTActionWalkToTarget_Template::BTActionWalkToTarget_Template()
: m_performTurn(btrue)
, m_wallRun(bfalse)
, m_forceSprint(bfalse)
{
}

}
