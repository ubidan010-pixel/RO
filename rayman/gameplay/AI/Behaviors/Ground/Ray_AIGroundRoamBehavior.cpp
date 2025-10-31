#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AIGROUNDROAMBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ground/Ray_AIGroundRoamBehavior.h"
#endif //_ITF_RAY_AIGROUNDROAMBEHAVIOR_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_AIFALLACTION_H_
#include "gameplay/ai/Actions/AIFallAction.h"
#endif //_ITF_AIFALLACTION_H_

#ifndef _ITF_AIPLAYANIMACTION_H_
#include "gameplay/ai/Actions/AIPlayAnimAction.h"
#endif //_ITF_AIPLAYANIMACTION_H_

#ifndef _ITF_AIJUMPACTION_H_
#include "gameplay/ai/Actions/AIJumpAction.h"
#endif //_ITF_AIJUMPACTION_H_


#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_WAYPOINTSMANAGER_H_
#include "gameplay/managers/WaypointsManager.h"
#endif //_ITF_WAYPOINTSMANAGER_H_

#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

//#ifndef _ITF_PUSHEDCOMPONENT_H_
//#include "gameplay/components/misc/PushedComponent.h"
//#endif //_ITF_PUSHEDCOMPONENT_H_


namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_AIGroundRoamBehavior_Template)

    BEGIN_SERIALIZATION_CHILD(Ray_AIGroundRoamBehavior_Template)
        // ACTIONS
        SERIALIZE_OBJECT_WITH_FACTORY("fall", m_fall, ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("jumpUp", m_jumpUp, ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("jumpDown", m_jumpDown, ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("roamPause", m_roamPause, ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("slopePause", m_slopePause, ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("obstaclePause", m_obstaclePause, ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("wallPause", m_wallPause, ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("defaultPause", m_defaultPause, ACTORSMANAGER->getAIActionsFactory());

        SERIALIZE_MEMBER("slopeDetectionRange", m_slopeDetectionRange);
        SERIALIZE_MEMBER("maxSlopeAngleUp", m_maxSlopeAngleUp);
        SERIALIZE_MEMBER("maxSlopeAngleDown", m_maxSlopeAngleDown);
        SERIALIZE_MEMBER("waypointID", m_waypointID);
        SERIALIZE_MEMBER("startDelay", m_startDelay);
        SERIALIZE_MEMBER("canPush", m_canPush);
        SERIALIZE_MEMBER("pushForce", m_pushForce);
        SERIALIZE_MEMBER("lowWallHeight", m_lowWallHeight);
        BEGIN_CONDITION_BLOCK(ESerialize_DataRaw)
#ifdef ITF_SUPPORT_DEBUGFEATURE
            SERIALIZE_MEMBER("drawDebug", m_drawDebug);
#endif // ITF_SUPPORT_DEBUGFEATURE
        END_CONDITION_BLOCK()
    END_SERIALIZATION()

        Ray_AIGroundRoamBehavior_Template::Ray_AIGroundRoamBehavior_Template()
        : m_fall(NULL)
        , m_jumpUp(NULL)
        , m_jumpDown(NULL)
        , m_roamPause(NULL)
        , m_slopePause(NULL)
        , m_obstaclePause(NULL)
        , m_wallPause(NULL)
        , m_defaultPause(NULL)
        , m_slopeDetectionRange(1.f)
        , m_maxSlopeAngleUp(btrue,30.f)
        , m_maxSlopeAngleDown(btrue,30.f)
        , m_startDelay(1.f)
        , m_canPush(bfalse)
        , m_pushForce(100.f)
        , m_lowWallHeight(0.f)
#ifdef ITF_SUPPORT_DEBUGFEATURE
        , m_drawDebug(bfalse)    
#endif // ITF_SUPPORT_DEBUGFEATURE
    {

    }

    Ray_AIGroundRoamBehavior_Template::~Ray_AIGroundRoamBehavior_Template()
    {
        SF_DEL(m_fall);
        SF_DEL(m_jumpUp);
        SF_DEL(m_jumpDown);
        SF_DEL(m_roamPause);
        SF_DEL(m_slopePause);
        SF_DEL(m_obstaclePause);
        SF_DEL(m_wallPause);
        SF_DEL(m_defaultPause);
    }
    IMPLEMENT_OBJECT_RTTI(Ray_AIGroundRoamBehavior)
    BEGIN_VALIDATE_BEHAVIOR(Ray_AIGroundRoamBehavior)
        //VALIDATE_BEHAVIOR_PARAM("fall", m_fall, "this action is mandatory");
    END_VALIDATE_BEHAVIOR()

    Ray_AIGroundRoamBehavior::Ray_AIGroundRoamBehavior()
        : Super()
        , m_fall(NULL)
        , m_jumpUp(NULL)
        , m_jumpDown(NULL)
        , m_roamPause(NULL)
        , m_slopePause(NULL)
        , m_obstaclePause(NULL)
        , m_wallPause(NULL)
        , m_defaultPause(NULL)

        , m_startTimer(0.f)
        , m_uturnAfterPause(bfalse)

        , m_blockingPoly(NULL)
        , m_blockingEdge(NULL)


        , m_pushing(bfalse)
        , m_wallHeight(0.f)

        , m_angleBeforeHitWall(0.f)
        , m_targetDirection(Vec2d::Zero)
        , m_tryReachPosition(bfalse)


    {
#ifndef ITF_FINAL
//#ifdef DEVELOPER_BEN
//        m_drawDebug = btrue;
//#endif // DEVELOPER_BEN
#endif // ITF_FINAL
    }

    Ray_AIGroundRoamBehavior::~Ray_AIGroundRoamBehavior()
    {
    }

    void Ray_AIGroundRoamBehavior::onActorLoaded(Pickable::HotReloadType _hotReload)
    {

        m_fall = static_cast<AIIdleAction*>(createAiAction(getTemplate()->getFall()));
        m_jumpUp = static_cast<AIJumpAction*>(createAiAction(getTemplate()->getJumpUp()));
        m_jumpDown = static_cast<AIJumpAction*>(createAiAction(getTemplate()->getJumpDown()));
        m_roamPause = static_cast<AIIdleAction*>(createAiAction(getTemplate()->getRoamPause()));
        m_slopePause = static_cast<AIIdleAction*>(createAiAction(getTemplate()->getSlopePause()));
        m_obstaclePause = static_cast<AIIdleAction*>(createAiAction(getTemplate()->getObstaclePause()));
        m_wallPause = static_cast<AIIdleAction*>(createAiAction(getTemplate()->getWallPause()));
        m_defaultPause = static_cast<AIIdleAction*>(createAiAction(getTemplate()->getDefaultPause()));

        Super::onActorLoaded(_hotReload);

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301),m_aiComponent);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventBlockedByPolyline,2950236768),m_aiComponent);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventSetDirection,187560938),m_aiComponent);
    }


    void Ray_AIGroundRoamBehavior::onActivate()
    {
        Super::onActivate();

        m_blockingPoly = NULL;
        m_blockingEdge = NULL;

        m_startTimer = getTemplate()->getStartDelay();

        startIdle();
    }

    void Ray_AIGroundRoamBehavior::update( f32 _delta  )
    {
#ifdef ITF_SUPPORT_DEBUGFEATURE
        if (getTemplate()->getDrawDebug())
        {
            debugContacts();
        }
#endif // ITF_SUPPORT_DEBUGFEATURE

        if ( m_currentAction == m_idle )
        {
            updateIdle(_delta);
        }
        else if ( m_currentAction == m_move )
        {
            updateMove(_delta);
        }

        m_blockingPoly = NULL;
        m_blockingEdge = NULL;

        Super::update(_delta);
    }

    void Ray_AIGroundRoamBehavior::onActionFinished()
    {
        if ( m_currentAction == m_fall )
        {
            onActionFinishedFall();
        }
        else if ( m_currentAction == m_wallPause )
        {
            onActionFinishedHitWall();
        }
        else if ( m_currentAction == m_roamPause ||
                  m_currentAction == m_slopePause ||
                  m_currentAction == m_obstaclePause ||
                  m_currentAction == m_defaultPause )
        {
            onActionFinishedPause();
        }
        else if ( m_currentAction == m_uturn )
        {
            onActionFinishedUturn();
        }
        else if ( m_currentAction == m_move )
        {
            onActionFinishedMove();
        }
        else if ( m_currentAction == m_jumpUp )
        {
            onActionFinishedJumpUp();
        }
        else if ( m_currentAction == m_jumpDown )
        {
            onActionFinishedJumpDown();
        }
        else
        {
            // we may end up here if m_idle and m_move were incorrectly setup with time limits
            // see AIRoamingBehavior for time-limited roaming
            ITF_ASSERT_MSG(0, "Unhandled finished action");
        }
    }

    void Ray_AIGroundRoamBehavior::onEvent( Event* _event )
    {
        Super::onEvent(_event);

        if ( AnimGameplayEvent* animEvent = _event->DynamicCast<AnimGameplayEvent>(ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301)) )
        {
            if (animEvent->getName() == ITF_GET_STRINGID_CRC(MRK_Restore_Angle,3001074192))
            {
                restoreAfterHitWall();
            }
        }
        else if ( EventBlockedByPolyline* blockedEvent = _event->DynamicCast<EventBlockedByPolyline>(ITF_GET_STRINGID_CRC(EventBlockedByPolyline,2950236768)) )
        {
            processEventBlockedByPolyline(blockedEvent);
        }
        else if (EventSetDirection * onSetDirection = _event->DynamicCast<EventSetDirection>(ITF_GET_STRINGID_CRC(EventSetDirection,187560938)))
        {
            m_targetDirection = onSetDirection->getDirection();
        }
    }

    void Ray_AIGroundRoamBehavior::updateIdle( f32 _dt )
    {
        PolyLine* poly = m_physComponent->getStickedPolyline();

        // If there is no floor we fall
        if ( !poly )
        {
            setAction(m_fall);
            return;
        }

        // Wait some time before doing something
        if ( m_startTimer )
        {
            m_startTimer -= _dt;

            if ( m_startTimer > 0.f )
            {
                return;
            }
            else
            {
                m_startTimer = 0.f;
            }
        }

        if ( getTemplate()->getWaypointID().isValid() )
        {
            // If there is a waypoint we will walk in the direction indicated by it
            Actor* waypoint = WAYPOINTS_MANAGER->getWaypoint(m_actor->get2DPos(), getTemplate()->getWaypointID(), m_actor->getDepth());
            if ( waypoint )
            {
                Vec2d waypointDir = Vec2d::Right.Rotate(waypoint->getAngle());
                m_targetDirection = waypointDir;

            }
        }

        if (m_targetDirection != Vec2d::Zero)
        {
            Vec2d myDir = AIUtils::getLookDir(m_actor, m_actor->isFlipped());
            if ( myDir.dot(m_targetDirection) < 0.f )
            {
                performUTurnImmediate();
            }
            else
            {
                startMove(m_actor->isFlipped());
            }

            return;
        }
        
        // just move forward
        startMove(m_actor->isFlipped());
    }

    bbool Ray_AIGroundRoamBehavior::updateMove( f32 _dt )
    {
        PolyLine* poly = m_physComponent->getStickedPolyline();

        if ( !poly )
        {
            // No floor, we fall
            startFall();
            return bfalse;
        }

        if ( m_targetDirection != Vec2d::Zero)
        {
            Vec2d myDir = AIUtils::getLookDir(m_actor, m_actor->isFlipped());
            if ( myDir.dot(m_targetDirection) < 0.f )
            {
                performUTurnImmediate();
                return bfalse;
            }
        }

        if (checkBlockingObstacle())
        {
            return bfalse;
        }

        //if (checkBlockingPolyline(m_blockingPoly, m_blockingEdge))
        //{
        //    return bfalse;
        //}

        if (checkStuck(_dt))
        {
            return bfalse;
        }

        if (checkWall(poly))
        {
            return bfalse;
        }

        if (checkHole(poly))
        {
            return bfalse;
        }

        if (checkSlope(poly))
        {
            return bfalse;
        }

        if (checkWaypoint())
        {
            return bfalse;
        }

        return btrue;
    }

    void Ray_AIGroundRoamBehavior::startAnimPause( AIAction* _pause, bbool _uturnAfterwards )
    {
        if (!_pause)
        {
            _pause = m_defaultPause;
        }

        if (_pause)
        {
            m_uturnAfterPause = _uturnAfterwards;
            setAction(_pause);
        }
        else if (_uturnAfterwards)
        {
            performUTurnImmediate();
        }
    }

    void Ray_AIGroundRoamBehavior::onActionFinishedFall()
    {
        startIdle();
    }

    void Ray_AIGroundRoamBehavior::onActionFinishedPause()
    {
        if (m_uturnAfterPause)
        {
            m_uturnAfterPause = bfalse;
            performUTurnImmediate();
        }
        else
        {
            // keep same dir
            startMove(m_actor->isFlipped());
        }
    }

    void Ray_AIGroundRoamBehavior::onActionFinishedHitWall()
    {
        restoreAfterHitWall();
        onActionFinishedPause();
    }

    void Ray_AIGroundRoamBehavior::onActionFinishedUturn()
    {
        Super::onActionFinishedUturn();
        startMove(m_actor->isFlipped());
    }

    void Ray_AIGroundRoamBehavior::onActionFinishedMove()
    {
        startAnimPause(m_roamPause, bfalse);
    }

    void Ray_AIGroundRoamBehavior::onActionFinishedJumpUp()
    {
        setAction(m_move);
    }

    void Ray_AIGroundRoamBehavior::onActionFinishedJumpDown()
    {
        setAction(m_move);
    }

    void Ray_AIGroundRoamBehavior::startIdle()
    {
        setAction(m_idle);
    }

    void Ray_AIGroundRoamBehavior::startMove( bbool _walkLeft)
    {
        m_move->setWalkLeft(_walkLeft);
        setAction(m_move, btrue);
    }

    void Ray_AIGroundRoamBehavior::startJumpUp( const PolyLineEdge* _wallEdge, f32 _wallHeight )
    {
        ITF_ASSERT_MSG(m_jumpUp, "no jumpUp action defined, you need to check and do something else!");
        if (m_jumpUp)
        {
            setAction(m_jumpUp);
        }
    }

    void Ray_AIGroundRoamBehavior::startJumpDown( const PolyLineEdge* _holeEdge, f32 _holeDepth )
    {
        ITF_ASSERT_MSG(m_jumpDown, "no jumpDown action defined, you need to check and do something else!");
        if (m_jumpDown)
        {
            setAction(m_jumpDown);
        }
    }

    void Ray_AIGroundRoamBehavior::startFall()
    {
        setAction(m_fall);
    }

    void Ray_AIGroundRoamBehavior::startHitWall( const PolyLineEdge* _wallEdge, f32 _wallHeight )
    {
        if (!m_wallPause)
        {
            startAnimPause(m_defaultPause, btrue);
            return;
        }

#ifndef ITF_FINAL
        //DebugDraw::edge(_wallEdge, Color::red(), 1.f);
#endif // ITF_FINAL

        m_physComponent->setDisabled(btrue, bfalse, btrue);    // disable physics so we can snap, but don't unstick
        m_posBeforeHitWall = m_actor->get2DPos();
        m_angleBeforeHitWall = m_actor->getAngle();

        m_wallHeight = _wallHeight;

        // orient along the wall
        f32 wallAngle = _wallEdge->m_normalizedVector.getPerpendicular().getAngle() + (m_actor->isFlipped() ? 0 : MTH_PI);
        m_actor->setAngle(wallAngle);

        // snap to wall
        Vec2d projPos;
        if (_wallHeight > getTemplate()->getLowWallHeight())
        {
            // project our position on the wall
            Vec2d wallToActor = m_actor->get2DPos() - _wallEdge->getPos();
            f32 projDist = _wallEdge->m_normalizedVector.dot(wallToActor);
            projPos = _wallEdge->getPos() + _wallEdge->m_normalizedVector * projDist;
        }
        else
        {
            // snap to top of wall
            projPos = _wallEdge->getPos();
            if (_wallEdge->m_vector.dot(Vec2d::Up) > 0)
                projPos += _wallEdge->m_vector;
        }
        m_actor->set2DPos( projPos + _wallEdge->m_normalizedVector.getPerpendicular() * m_physComponent->getRadius() );

        startAnimPause(m_wallPause, btrue);
    }

    void Ray_AIGroundRoamBehavior::restoreAfterHitWall()
    {
        if (m_physComponent->getDisabled())
        {
            m_actor->set2DPos(m_posBeforeHitWall);
            m_actor->setAngle(m_angleBeforeHitWall);    // just to be safe, but the physics should set the angle anyway
            m_physComponent->setDisabled(bfalse);
        }
    }


    void Ray_AIGroundRoamBehavior::updateAnimInputs()
    {
        Super::updateAnimInputs();

        if (m_animatedComponent)
        {
            m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(Pushing,2679047304), m_pushing);
            m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(WallHeight,494976609), m_wallHeight);
        }
    }


    /*bbool Ray_AIGroundRoamBehavior::checkBlockingObstacle()
    {
        m_pushing = bfalse;

        Vec2d lookDir = AIUtils::getLookDir(m_actor, m_actor->isFlipped());

        const PhysContactsContainer& contacts = m_physComponent->getContacts();
        const u32 contactCount = contacts.size();
        for (u32 i = 0; i < contactCount; i++)
        {
            const SCollidableContact& contact = contacts[i];

            f32 angle = lookDir.getShortestAngle(contact.m_normal);
            bbool blocking = f32_Abs(angle) < (30.f * MTH_DEGTORAD);
            if (blocking)
            {
                PolyLine* contactPoly(NULL);
                const PolyLineEdge* contactEdge(NULL);
                AIUtils::getPolyLine(contact.m_collidableUserData, contact.m_edgeIndex, contactPoly, contactEdge);

                return checkBlockingPolyline(contactPoly, contactEdge, contact.m_edgeIndex, contact.m_contactPoint);
            }
        }

        return bfalse;
    }*/
    bbool Ray_AIGroundRoamBehavior::checkBlockingObstacle()
    {
        m_pushing = bfalse;

        Vec2d lookDir = AIUtils::getLookDir(m_actor, m_actor->isFlipped());

        const PhysContactsContainer& contacts = m_physComponent->getContacts();
        const u32 contactCount = contacts.size();
        for (u32 i = 0; i < contactCount; i++)
        {
            const SCollidableContact& contact = contacts[i];


            //ignore self
            if ( contact.m_collidableUserData == m_actor->getRef() )
                continue;
            BaseObject* obj = GETOBJECT(contact.m_collidableUserData);

            PolyLine* polyLine = obj?obj->DynamicCast<PolyLine>(ITF_GET_STRINGID_CRC(PolyLine,1932163747)):NULL;
            if (polyLine)
            {
                if (polyLine->getOwnerActor() == m_actor)
                    continue;

                if (polyLine->getOwnerActor() && m_actor->getBinding()->isDescendant(polyLine->getOwnerActor()->getRef()))
                    continue;
            }

            PolyLine* contactPoly(NULL);
            const PolyLineEdge* contactEdge(NULL);
            AIUtils::getPolyLine(contact.m_collidableUserData, contact.m_edgeIndex, contactPoly, contactEdge);

            if (contactPoly && contactEdge)
            {
                if ( AIUtils::isContactBlockingObstacle(contactEdge->getPos(),
                                                        contactEdge->m_normalizedVector,
                                                        contactEdge->m_length,
                                                        World::getGameMaterial(contactEdge->getGameMaterial()),
                                                        contactPoly,contactEdge,1.f,contact,
                                                        m_targetDirection,m_physComponent->getRadius(),
                                                        m_physComponent->getStickedPolylineRef(),m_physComponent->getStickedEdgeIndex(),
                                                        m_physComponent->getRelativeEdgeDistance(),m_aiComponent->getCurrentGravityDir(),bfalse) )
                {
                    return checkBlockingPolyline(contactPoly, contactEdge, contact.m_edgeIndex, contact.m_contactPoint);
                }

                /*
                Vec2d normal = -contactEdge->m_normalizedVector.getPerpendicular();
                f32 angle = lookDir.getShortestAngle(normal);
                bbool blocking = f32_Abs(angle) < (30.f * MTH_DEGTORAD);
                if (blocking)
                {
                    return checkBlockingPolyline(contactPoly, contactEdge, contact.m_edgeIndex, contact.m_contactPoint);
                }
                */
            }
        }

        return bfalse;
    }

    void Ray_AIGroundRoamBehavior::processEventBlockedByPolyline( EventBlockedByPolyline* _blockedEvent )
    {
        BaseObject* obj = GETOBJECT(_blockedEvent->getPolyline());
        if ( obj )
        {
            // remember the blocking and process it on update
            m_blockingPoly = static_cast<PolyLine*>(obj);
            m_blockingEdge = &m_blockingPoly->getEdgeAt(_blockedEvent->getEdge());
#ifndef ITF_FINAL
            //DebugDraw::edge(m_blockingEdge, m_blockingPoly->getDepth(), Color::red(), 1.f);
#endif // ITF_FINAL
        }
    }

    bbool Ray_AIGroundRoamBehavior::checkBlockingPolyline( PolyLine* _poly, const PolyLineEdge* _edge, u32 _edgeIndex, const Vec2d& _contactPoint )
    {
        if (!_poly || !_edge)
        {
            return bfalse;
        }

        bbool left = _edge->m_vector.getPerpendicular().dot(Vec2d::Right) > 0;  // if the edge normal goes right, that means we're hitting it from the left
        f32 edgeHeight;
        AIUtils::hasWallInFront
        (
            _contactPoint,
            left,
            _poly,
            _edgeIndex,
            0.5f,
            5.f,
            edgeHeight,
#ifdef ITF_SUPPORT_DEBUGFEATURE
            getTemplate()->getDrawDebug()
#else // ITF_SUPPORT_DEBUGFEATURE
            bfalse
#endif // ITF_SUPPORT_DEBUGFEATURE
        );
#ifdef ITF_SUPPORT_DEBUGFEATURE
        if (getTemplate()->getDrawDebug())
        {
            DebugDraw::edge(_edge, _poly->getDepth(), Color::blue(), 1.f);
        }
#endif // ITF_SUPPORT_DEBUGFEATURE

        if (0 < edgeHeight && edgeHeight < getTemplate()->getObstacleJumpHeight())
        {
            if (m_jumpUp)
            {
                startJumpUp(_edge, edgeHeight);
            }
            else
            {
                startHitWall(_edge, edgeHeight);
            }
            return btrue;
        }

        if (edgeHeight < 0)
        {
            // TODO: this is a bit dumb: how can we have a contact with an edge going down...
            ITF_ASSERT_MSG(0, "how the hell did I find a blocking edge that goes down??");
            if (m_jumpDown && -edgeHeight < getTemplate()->getHoleJumpDepth())
            {
                startJumpDown(_edge, edgeHeight);
            }
            else
            {
                startAnimPause(m_obstaclePause, btrue);
            }
            return btrue;
        }

        // cannot jump or push...
        startHitWall(_edge, edgeHeight);
        return btrue;
    }


    // If we don't have speed for too long it means we are stuck: pause & u-turn
    // TODO: this is a failsafe, getting stuck most likely means that we failed to detect an obstacle...
    bbool Ray_AIGroundRoamBehavior::checkStuck( f32 _dt )
    {
        if ( !m_pushing &&
             f32_Abs(m_physComponent->getWorldSpeedAverageX()) < 0.1f )
        {
            m_stuckTimer += _dt;

            if ( m_stuckTimer >= getTemplate()->getStuckDelay() )
            {
                m_stuckTimer = 0.f;
                startAnimPause(m_obstaclePause, btrue);
                return btrue;
            }
        }
        else
        {
            m_stuckTimer = 0.f;
        }

        return bfalse;
    }

    // If there is too much slope: pause and u-turn
    bbool Ray_AIGroundRoamBehavior::checkSlope( PolyLine* poly )
    {
        f32 slope = AIUtils::getSlopeAngle(
            m_actor->get2DPos(),
            poly, m_physComponent->getStickedEdgeIndex(),
            m_move->isWalkingLeft(),
            getTemplate()->getSlopeDetectionRange(),
            m_aiComponent->getCurrentGravityDir(),
            NULL);

        if ( !isValidSlope(slope) )
        {
            startAnimPause(m_slopePause, btrue);
            return btrue;
        }

        return bfalse;
    }

    // If there is an obstacle: jump it if it's low enough, otherwise pause & u-turn.
    bbool Ray_AIGroundRoamBehavior::checkWall( PolyLine* poly )
    {
        /*f32 wallHeight;
        AIUtils::hasWallInFront(
            m_physComponent->getStickedPosition(), m_actor->isFlipped(),
            poly, m_physComponent->getStickedEdgeIndex(),
            m_obstacleDetectionRange, 5.f,
            wallHeight );

        if ( m_jumpUp && 0 < wallHeight && wallHeight < m_obstacleJumpHeight )
        {
            startJumpUp(NULL, wallHeight);
            return btrue;
        }*/

        return bfalse;
    }

    // If there is an obstacle: jump it if it's low enough, otherwise pause & u-turn.
    /*bbool Ray_AIGroundRoamBehavior::checkWall( const PolyLine* poly )
    {
        f32 wallHeight;
        const PolyLineEdge* wallEdge;
        bbool hasWall = AIUtils::hasWallInFront(
            m_actor, m_actor->isFlipped(),
            poly, m_physComponent->getStickedEdgeIndex(),
            m_obstacleDetectionRange,
            m_physComponent->getRadius(),
            m_aiComponent->getCurrentGravityDir(),
            wallHeight, &wallEdge );

        if ( hasWall )
        {
            if ( m_jumpUp && wallHeight < m_obstacleJumpHeight )
            {
                startJumpUp(wallEdge, wallHeight);
            }
            else
            {
                startHitWall(wallEdge, wallHeight);
            }

            return btrue;
        }

        return bfalse;
    }*/

    // If there is a hole: jump down if it's not too deep, otherwise pause & u-turn.
    bbool Ray_AIGroundRoamBehavior::checkHole( PolyLine* poly )
    {
        f32 holeDepth;
        bbool hasHole = AIUtils::hasHoleInFront(
            m_actor, m_actor->isFlipped(),
            poly, m_physComponent->getStickedEdgeIndex(),
            getTemplate()->getHoleDetectionRange(),
            m_physComponent->getRadius(),
            m_aiComponent->getCurrentGravityDir(),
            holeDepth);

        if ( hasHole )
        {
            if ( m_jumpDown && holeDepth < getTemplate()->getHoleJumpDepth() )
            {
                startJumpDown(m_physComponent->getStickedEdge(), holeDepth);
            }
            else if ( m_tryReachPosition )
            {
                startJumpUp(NULL,0.f);
            }
            else
            {
                startAnimPause(m_obstaclePause, btrue);
            }

            return btrue;
        }

        return bfalse;
    }

    // If we find a waypoint going in the opposite dir: pause and u-turn
    bbool Ray_AIGroundRoamBehavior::checkWaypoint()
    {
        if ( getTemplate()->getWaypointID().isValid() )
        {
            Actor* waypoint = WAYPOINTS_MANAGER->getWaypoint(m_actor->get2DPos(), getTemplate()->getWaypointID(), m_actor->getDepth());
            if ( waypoint )
            {
                Vec2d waypointDir = Vec2d::Right.Rotate(waypoint->getAngle());
                Vec2d myDir = AIUtils::getLookDir(m_actor, m_actor->isFlipped());

                if ( myDir.dot(waypointDir) < 0.f )
                {
                    startAnimPause(m_roamPause, btrue);
                    return btrue;
                }
            }
        }

        return bfalse;
    }

    bbool Ray_AIGroundRoamBehavior::isValidSlope( f32 _slopeAngle ) const
    {
        if ( _slopeAngle >= 0.f )
        {
            return _slopeAngle <= getTemplate()->getMaxSlopeAngleUp().ToRadians();
        }
        else
        {
            return -_slopeAngle <= getTemplate()->getMaxSlopeAngleDown().ToRadians();
        }
    }

#ifdef ITF_SUPPORT_DEBUGFEATURE
    void Ray_AIGroundRoamBehavior::debugContacts()
    {
        PolyLine* poly = m_physComponent->getStickedPolyline();
        if (poly)
        {
            f32 slopeAngle = AIUtils::getSlopeAngle(
                m_actor->get2DPos(),
                poly, m_physComponent->getStickedEdgeIndex(),
                m_actor->isFlipped(),
                getTemplate()->getSlopeDetectionRange(),
                m_aiComponent->getCurrentGravityDir(),
                NULL);

            f32 wallHeight(-1);
            AIUtils::hasWallInFront(
                m_actor, m_actor->isFlipped(),
                poly, m_physComponent->getStickedEdgeIndex(),
                getTemplate()->getObstacleDetectionRange(),
                m_physComponent->getRadius(),
                m_aiComponent->getCurrentGravityDir(),
                wallHeight);

            f32 holeDepth(-1);
            AIUtils::hasHoleInFront(
                m_actor, m_actor->isFlipped(),
                poly, m_physComponent->getStickedEdgeIndex(),
                getTemplate()->getHoleDetectionRange(),
                m_physComponent->getRadius(),
                m_aiComponent->getCurrentGravityDir(),
                holeDepth);

            String msg;
            msg.setTextFormat("angle %.f", slopeAngle * MTH_RADTODEG);
            GFX_ADAPTER->drawDBGText(msg);
            msg.setTextFormat("wall %.2f", wallHeight);
            GFX_ADAPTER->drawDBGText(msg);
            msg.setTextFormat("hole %.2f", holeDepth);
            GFX_ADAPTER->drawDBGText(msg);
        }


        Vec2d lookDir = AIUtils::getLookDir(m_actor, m_actor->isFlipped());
        if (m_currentAction == m_wallPause)
        {
            lookDir = Vec2d::Right.Rotate(m_angleBeforeHitWall);
            if ( m_actor->isFlipped() )
                lookDir *= -1.f;
        }

        const PhysContactsContainer& contacts = m_physComponent->getContacts();
        const u32 contactCount = contacts.size();
        for (u32 i = 0; i < contactCount; i++)
        {
            const SCollidableContact& contact = contacts[i];

            f32 angle = lookDir.getShortestAngle(contact.m_normal);
            bbool blocking = f32_Abs(angle) < (30.f * MTH_DEGTORAD);
            GFX_ADAPTER->drawArrow(contact.m_contactPoint, contact.m_contactPoint + contact.m_normal, blocking ? Color::red() : Color::white());

            PolyLine* contactPoly(NULL);
            const PolyLineEdge* contactEdge(NULL);
            AIUtils::getPolyLine(contact.m_collidableUserData, contact.m_edgeIndex, contactPoly, contactEdge);

            //if (contactPoly && contactEdge)
            //{
            //    PushedComponent* pushable = contactPoly->getOwnerActor() ? contactPoly->getOwnerActor()->GetComponent<PushedComponent>() : NULL;

            //    Vec2d edgeEnd = contactEdge->getPos() + contactEdge->m_vector;
            //    f32 edgeHeight = edgeEnd.m_y - m_actor->getPos().m_y;
            //    bbool jumpable = pushable && (0 < edgeHeight && edgeHeight < getTemplate()->getObstacleJumpHeight());

            //    String msg;msg.setTextFormat("%d", i);
            //    DebugDraw::edge(
            //        contactEdge,
            //        contactPoly->getDepth(),
            //        jumpable ? Color::yellow() :
            //        pushable ? Color::orange() :
            //        blocking ? Color::red() : Color::green(),
            //        0,
            //        msg
            //    );
            //}
        }
    }
#endif // ITF_SUPPORT_DEBUGFEATURE
}