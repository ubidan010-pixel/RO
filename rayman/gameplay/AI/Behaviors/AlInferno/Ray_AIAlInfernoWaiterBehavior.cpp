#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AIALINFERNOWAITERBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/AlInferno/Ray_AIAlInfernoWaiterBehavior.h"
#endif //_ITF_RAY_AIALINFERNOWAITERBEHAVIOR_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#ifndef _ITF_AIFOLLOWACTORACTION_H_
#include "gameplay/ai/Actions/AIFollowActorAction.h"
#endif //_ITF_AIFOLLOWACTORACTION_H_

#ifndef _ITF_PHANTOMCOMPONENT_H_
#include "gameplay/Components/Misc/PhantomComponent.h"
#endif //_ITF_PHANTOMCOMPONENT_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_AIAlInfernoWaiterBehavior)
    BEGIN_VALIDATE_BEHAVIOR(Ray_AIAlInfernoWaiterBehavior)
    END_VALIDATE_BEHAVIOR()

    const f32 Ray_AIAlInfernoWaiterBehavior::MIN_TIME_BETWEEN_ATTACK = 1.0f;

    //-------------------------------------------------------
    Ray_AIAlInfernoWaiterBehavior::Ray_AIAlInfernoWaiterBehavior()
    : m_state(WaiterState_UNDEFINED)
    , m_fallAction(NULL)
    , m_attackLongRangeAction(NULL)
    , m_patrolAction(NULL)
    , m_crushedAction(NULL)
    , m_uturnWalkAction(NULL)
    , m_phantomComponent(NULL)
    , m_target(ActorRef::InvalidRef)
    , m_targetUturn(bfalse)
    , m_aggroTime(0.0f)
    , m_flamesFXHanfle(U32_INVALID)
    , m_timeSinceLastAttack(0.0f)
    , m_linkedToActivator(bfalse)
    , m_activated(bfalse)
    {        
    }

    //-------------------------------------------------------
    Ray_AIAlInfernoWaiterBehavior::~Ray_AIAlInfernoWaiterBehavior()
    {
    }

    //-------------------------------------------------------
    void Ray_AIAlInfernoWaiterBehavior::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        //create actions from template
        m_attackLongRangeAction = createAiAction(getTemplate()->getAttackLongRangeAction());
        m_fallAction = createAiAction(getTemplate()->getFallAction());
        AIAction * patrolAction = createAiAction(getTemplate()->getPatrolAction());
        ITF_WARNING( m_actor, patrolAction != NULL, "Ray_AIAlInfernoWaiterBehavior has an invalid serve action" );
        if( patrolAction != NULL )
        {
            ITF_WARNING( m_actor, patrolAction->GetObjectClassCRC() == ITF_GET_STRINGID_CRC(AIWalkInDirAction,2047489259), "Ray_AIAlInfernoWaiterBehavior has an invalid serve action" );
            if(patrolAction->GetObjectClassCRC() == ITF_GET_STRINGID_CRC(AIWalkInDirAction,2047489259) )
            {
                m_patrolAction = static_cast<AIWalkInDirAction *>( patrolAction );
            }
        }
        m_crushedAction = createAiAction(getTemplate()->getCrushedAction());
        m_uturnWalkAction = createAiAction(getTemplate()->getUturnWalkAction());

        
        //register behavior to events
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301),m_aiComponent);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventActivate,2639596100),m_aiComponent);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventStickOnPolyline,471231076),m_aiComponent);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(PunchStim,200533519),m_aiComponent);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventDetach,889341216),m_aiComponent);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventQueryCanReceiveHit,1176328316),m_aiComponent);

        //set anim bank state
        if( m_animatedComponent != NULL )
        {
            m_animatedComponent->setBankState( 1 );
        }

        //get phantom component
        m_phantomComponent = m_actor->GetComponent<PhantomComponent>();
    }

    //-------------------------------------------------------
    void Ray_AIAlInfernoWaiterBehavior::onActivate()
    {
        Super::onActivate();

        //start idle behavior
        startState(WaiterState_IDLE);
    }

    //-------------------------------------------------------
    void Ray_AIAlInfernoWaiterBehavior::onDeactivate()
    {
        Super::onDeactivate();
        
        //stop fx
        stopFlamesFX();

        //reset default phantom shape
        if( m_phantomComponent != NULL )
        {
            m_phantomComponent->setDisabled(bfalse);
        }
    }

    //-------------------------------------------------------
    void Ray_AIAlInfernoWaiterBehavior::update(f32 _dt)
    {
        Super::update(_dt);

        //update target
        updateTarget(_dt);

        switch(m_state)
        {
        case WaiterState_IDLE:
            updateIdle();
            break;

        case WaiterState_PATROL:
            updatePatrol();
            break;

        case WaiterState_CATCH:
            updateCatch();
            break;

        case WaiterState_ATTACK:
            updateAttack();
            break;

        case WaiterState_CRUSHED:
            updateCrushed();
            break;

        default:
            ITF_ASSERT_MSG( m_state != WaiterState_UNDEFINED, "Try to update Al inferno waiter behavior with an invalid state" );
            break;
        }

        if( m_state != WaiterState_ATTACK )
        {
            m_timeSinceLastAttack +=_dt;
        }
    }

    //-------------------------------------------------------
    void Ray_AIAlInfernoWaiterBehavior::startState(const WaiterState _state)
    {
        switch( m_state )
        {
        case WaiterState_ATTACK:
            stopAttack();
            break;
        case WaiterState_CRUSHED:
            stopCrushed();
            break;
        case WaiterState_UTURN_WALK:
            stopUturn();
            break;
        case WaiterState_CATCH:
            stopCatch();
            break;
        default:
            break;
        }

        //update state
        m_state = _state;

        switch(m_state)
        {
        case WaiterState_IDLE:
            startIdle();
            break;
        case WaiterState_ATTACK:
            startAttack();
            break;
        case WaiterState_FALL:
            startFall();
            break;
        case WaiterState_UTURN_WALK:
            startUturnWalk();
            break;
        case WaiterState_PATROL:
            startPatrol();
            break;
        case WaiterState_CATCH:
            startCatchOnTray();
            break;
        case WaiterState_CRUSHED:
            startCrushed();
            break;
        default:
            break;
        }
    }

    //-------------------------------------------------------
    void Ray_AIAlInfernoWaiterBehavior::startIdle()
    {
        setAction( m_idle );
    }

    //-------------------------------------------------------
    void Ray_AIAlInfernoWaiterBehavior::updateIdle()
    {
        if( !m_linkedToActivator || m_activated )
        {
            startState(WaiterState_PATROL);
        }
    }

    //-------------------------------------------------------
    void Ray_AIAlInfernoWaiterBehavior::startAttack()
    {
        setAction( m_attackLongRangeAction );
    }

    //-------------------------------------------------------
    void Ray_AIAlInfernoWaiterBehavior::updateAttack()
    {
        //check fall
        if( checkImmediateHole() )
        {
            startState(WaiterState_FALL);
            return;
        }
        else if( checkObstacle(btrue) )
        {
            const Vec2d & speed = m_physComponent->getSpeed();
            m_physComponent->addForce( speed * (-1.0f * getTemplate()->getBrakingForce()) );
        }
    }

    //-------------------------------------------------------
    void Ray_AIAlInfernoWaiterBehavior::stopAttack()
    {
        //stop flame FX
        stopFlamesFX();
        m_timeSinceLastAttack = 0.0f;
    }

    //-------------------------------------------------------
    void Ray_AIAlInfernoWaiterBehavior::startFall()
    {
        setAction(m_fallAction);
    }

    //-------------------------------------------------------
    void Ray_AIAlInfernoWaiterBehavior::startUturnWalk()
    {
        setAction(m_uturnWalkAction);
    }

    //-------------------------------------------------------
    void Ray_AIAlInfernoWaiterBehavior::stopUturn()
    {
        m_actor->setFlipped(!m_actor->isFlipped());
    }

    //-------------------------------------------------------
    void Ray_AIAlInfernoWaiterBehavior::startPatrol()
    {
        m_patrolAction->setWalkLeft( m_actor->isFlipped() );
        setAction( m_patrolAction );
    }

    //-------------------------------------------------------
    void Ray_AIAlInfernoWaiterBehavior::updatePatrol()
    {
        //check fall
        if( checkImmediateHole() )
        {
            startState(WaiterState_FALL);
            return;
        }
        else
        {
            if( checkAttack() )
            {
                if( !m_targetUturn )
                {
                    startState(WaiterState_ATTACK);
                    return;
                }
                else
                {
                    m_actor->setFlipped(!m_actor->isFlipped());
                    return;
                }
            }
            else
            {
                //check blocked by obstacle only if speed and flip value are in the same direction
                bbool speedXPositive = isWaiterSpeedXPositive();
                if( speedXPositive == !m_actor->isFlipped() )
                {
                    if( checkObstacle(btrue) )
                    {
                        startState(WaiterState_UTURN_WALK);
                        return;
                    }
                }
            }
        }
    }

    //-------------------------------------------------------
    void Ray_AIAlInfernoWaiterBehavior::startCatchOnTray()
    {
        ITF_ASSERT( m_state == WaiterState_CATCH );
        if( m_currentAction != NULL )
        {
            const StringID & catchAnim = getTemplate()->getCatchWalkAnimation();
            if( catchAnim.isValid() )
            {
                m_currentAction->playAction( catchAnim );
            }
        }
    }

    //-------------------------------------------------------
    void Ray_AIAlInfernoWaiterBehavior::updateCatch()
    {
        //check fall
        if( checkImmediateHole() )
        {
            startState(WaiterState_FALL);
            return;
        }
        else
        {
            //check obstacles
            if( checkObstacle() )
            {
                startState(WaiterState_UTURN_WALK);
                return;
            }
            else
            {
                if( m_animatedComponent != NULL )
                {
                    if( !m_animatedComponent->isAnimLooped() )
                    {
                        if( m_animatedComponent->isSubAnimFinished() )
                        {
                            startState( WaiterState_PATROL );
                        }
                    }
                }
                else
                {
                    startState( WaiterState_PATROL );
                }
            }
        }
    }

    //-------------------------------------------------------
    void Ray_AIAlInfernoWaiterBehavior::stopCatch()
    {
        ITF_ASSERT( m_state == WaiterState_CATCH );
        if( m_currentAction != NULL )
        {
            m_currentAction->playAction( m_currentAction->getAnimAction() );
        }
    }

    //-------------------------------------------------------
    void Ray_AIAlInfernoWaiterBehavior::startCrushed()
    {
        setAction(m_crushedAction);
        //disable phantom shape
        if( m_phantomComponent != NULL )
        {
            m_phantomComponent->setDisabled(btrue);
        }
    }

    //-------------------------------------------------------
    void Ray_AIAlInfernoWaiterBehavior::updateCrushed()
    {
        if( checkObstacle() )
        {
            const Vec2d & speed = m_physComponent->getSpeed();
            m_physComponent->addForce( speed * (-1.0f * getTemplate()->getBrakingForce()) );
            return;
        }
    }

    //-------------------------------------------------------
    void Ray_AIAlInfernoWaiterBehavior::stopCrushed()
    {
        //enable phantom shape
        if( m_phantomComponent != NULL )
        {
           m_phantomComponent->setDisabled(bfalse);
        }
    }

    //-------------------------------------------------------
    void Ray_AIAlInfernoWaiterBehavior::onActionFinished()
    {
        if( !m_linkedToActivator || m_activated )
        {
            //start patrol
            startState(WaiterState_PATROL);
        }
        else
        {
            //start idle
            startState(WaiterState_IDLE);
        }
    }

    //-------------------------------------------------------
    void Ray_AIAlInfernoWaiterBehavior::updateTarget(const f32 _dt)
    {
        const Actor * const enemy = checkLongRangeAttackTarget( m_targetUturn );
        if( enemy )
        {
            m_target = enemy->getRef();
            m_aggroTime += _dt;
        }
        else
        {
            m_target.invalidate();
            m_aggroTime = 0.0f;
        }
    }

    //-------------------------------------------------------
    bbool Ray_AIAlInfernoWaiterBehavior::checkAttack() const
    {
        if( m_target.isValid() )
        {
            return (m_aggroTime >= getTemplate()->getAttackTimeThreshold() && m_timeSinceLastAttack >= MIN_TIME_BETWEEN_ATTACK);
        }
        return bfalse;
    }

    //-------------------------------------------------------
    const Actor * Ray_AIAlInfernoWaiterBehavior::checkLongRangeAttackTarget(bbool &_uTurn) const
    {
        const Actor * const enemy = AIUtils::getClosestLivePlayer(m_actor, getTemplate()->getLongRangeAttackDetectionRange());
        if( enemy != NULL )
        {
            const Vec2d & enemyPos = enemy->get2DPos();
            const Vec2d & myPos = m_actor->get2DPos();
            const Vec2d direction = enemyPos - myPos;
            const Vec2d sight = AIUtils::getLookDir( m_actor, m_actor->isFlipped() );
            const f32 dot = Vec2d::Dot( &sight, &direction );
            _uTurn = ( dot < 0.0f );
            return enemy;
        }

        return NULL;
    }

    //-------------------------------------------------------
    bbool Ray_AIAlInfernoWaiterBehavior::checkObstacle(const bbool _useSpeedDirAsFlip /*= bfalse*/) const
    {
        bbool flipped = m_actor->isFlipped();
        if( _useSpeedDirAsFlip )
        {
            flipped = !isWaiterSpeedXPositive();
        }
        if( checkStickedPolylineObstacle(flipped) )
        {
            return btrue;
        }
        else if( checkPhysObstacle(flipped) )
        {
            return btrue;
        }
        return bfalse;
    }

    //-------------------------------------------------------
    bbool Ray_AIAlInfernoWaiterBehavior::checkStickedPolylineObstacle(const bbool _flipped) const
    {
        PolyLine * polyline = m_physComponent->getStickedPolyline();
        if (polyline != NULL)
        {
            u32 edgeIndex = m_physComponent->getStickedEdgeIndex();
            u32 previousEdgeIndex = edgeIndex;

            const PolyLine * previousPolyline = polyline;
            
            //compute detection distance according to waiter speed
            const f32 speed = m_physComponent->getSpeed().norm();
            const f32 detectionDistance = speed * getTemplate()->getHoleDetectionRange();

            Vec2d currentPosition = m_actor->get2DPos();

            f32 distance = 0.0f;
            while (distance < detectionDistance)
            {
                //get next edge info
                AIUtils::EdgeType edgeType = AIUtils::EdgeType_Invalid;
                f32 edgeLength = 0.0f;
                f32 distanceToEdge = 0.0f;
                bbool up = bfalse;
                edgeIndex = AIUtils::getNextEdgeInfo(currentPosition, polyline, edgeIndex, _flipped, m_aiComponent->getCurrentGravityDir(),
                    edgeType, distanceToEdge, edgeLength, up, &polyline);

                //update distance
                distance += distanceToEdge;
                if (distance > detectionDistance)
                {
                    return bfalse;
                }

                //no edge
                if (edgeIndex == U32_INVALID)
                {
                    return btrue;
                }
                //edge is wall or ceiling => obstacle
                else if (edgeType != AIUtils::EdgeType_Floor)
                {
                    return btrue;
                }

                //update position
                if (_flipped)
                {
                    currentPosition = previousPolyline->getEdgeAt(previousEdgeIndex).getPos();
                }
                else
                {
                    currentPosition = polyline->getEdgeAt(edgeIndex).getPos();
                }
                //update previous edge index and previous polyline
                previousEdgeIndex = edgeIndex;
                previousPolyline = polyline;
            }
        }

        return bfalse;
    }

    //-------------------------------------------------------
    bbool Ray_AIAlInfernoWaiterBehavior::checkPhysObstacle(const bbool _flipped) const
    {
        const Vec2d lookDir = AIUtils::getLookDir(m_actor, m_actor->isFlipped());
        const PhysContactsContainer& contacts = m_physComponent->getContacts();
        const u32 contactCount = contacts.size();
        for(u32 index = 0; index < contactCount; ++index)
        {
            const SCollidableContact& contact = contacts[index];

            if( checkPolylineObstacle(_flipped, contact.m_collidableUserData, contact.m_edgeIndex, contact.m_contactPoint) )
            {
                return btrue;
            }
        }

        return bfalse;
    }

    //-------------------------------------------------------
    bbool Ray_AIAlInfernoWaiterBehavior::checkPolylineObstacle(const bbool _flipped, const ObjectRef &_poly, const u32 _edgeIndex, const Vec2d &_contact) const
    {
        Vec2d lookDir = AIUtils::getLookDir(m_actor, _flipped);

        //ignore self
        if (_poly == m_actor->getRef())
        {
            return bfalse;
        }
        //ignore currently sticked polyline
        if (_poly == m_physComponent->getStickedPolylineRef())
        {
            return bfalse;
        }
        //polyline early out
        BaseObject * obj = _poly.getObject();
        PolyLine * polyLine = obj?obj->DynamicCast<PolyLine>(ITF_GET_STRINGID_CRC(PolyLine,1932163747)):NULL;
        if( !polyLine )
        {
            return bfalse;
        }
        if (polyLine->getOwnerActor() == m_actor)
        {
            return bfalse;
        }
        if (polyLine->getOwnerActor() && m_actor->getBinding()->isDescendant(polyLine->getOwnerActor()->getRef()))
        {
            return bfalse;
        }
        if (polyLine->getVectorsCount() <= _edgeIndex)
        {
            return bfalse;
        }

        //it should not be possible to contact with a wall considered a hole...
        if ((_contact - m_actor->get2DPos()).dot(lookDir) > 0.0f)
        {
            const PolyLineEdge & contactEdge = polyLine->getEdgeAt(_edgeIndex);
            const AIUtils::EdgeType type = AIUtils::getEdgeType(contactEdge);
            return (type != AIUtils::EdgeType_Floor);
        }

        return bfalse;
    }

    //-------------------------------------------------------
    bbool Ray_AIAlInfernoWaiterBehavior::checkImmediateHole() const
    {
        PolyLine* poly = m_physComponent->getStickedPolyline();
        return (poly == NULL);
    }

    //-------------------------------------------------------
    void Ray_AIAlInfernoWaiterBehavior::onEvent(ITF::Event *_event)
    {
        Super::onEvent( _event );
        
        if( AnimGameplayEvent * animEvent = _event->DynamicCast<AnimGameplayEvent>(ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301)) )
        {
            processAnimEvent( animEvent );
        }
        else if( EventStickOnPolyline * stickEvent = _event->DynamicCast<EventStickOnPolyline>(ITF_GET_STRINGID_CRC(EventStickOnPolyline,471231076)) )
        {
            processStickEvent( stickEvent );
        }
        else if( PunchStim * stim = _event->DynamicCast<PunchStim>(ITF_GET_STRINGID_CRC(PunchStim,200533519)) )
        {
            processPunchEvent( stim );
        }
        else if( EventActivate * activateEvent = _event->DynamicCast<EventActivate>(ITF_GET_STRINGID_CRC(EventActivate,2639596100)) )
        {
            processActivateEvent( activateEvent );
        }
        else if ( const EventDetach * const detachEvent = _event->DynamicCast<EventDetach>(ITF_GET_STRINGID_CRC(EventDetach,889341216)) )
        {
            processDetachEvent(detachEvent);
        }
        else if ( EventQueryCanReceiveHit * const query = _event->DynamicCast<EventQueryCanReceiveHit>(ITF_GET_STRINGID_CRC(EventQueryCanReceiveHit,1176328316)) )
        {
            processQueryCanReceiveHit(query);
        }
    }

    //-------------------------------------------------------
    void Ray_AIAlInfernoWaiterBehavior::processAnimEvent(const AnimGameplayEvent * const _event)
    {
        ITF_ASSERT( _event != NULL );
        //check anim markers to launch stop fx
        const StringID startFlamesMarker = "MRK_Flame_Start";
        const StringID stopFlamesMarker = "MRK_Flame_Stop"; 
        if( _event->getName() == startFlamesMarker )
        {
            startFlamesFX();
        }
        else if( _event->getName() == stopFlamesMarker )
        {
            stopFlamesFX();
        }
    }

    //-------------------------------------------------------
    void Ray_AIAlInfernoWaiterBehavior::processStickEvent(const ITF::EventStickOnPolyline *const _event)
    {
        ITF_ASSERT( _event != NULL );

        //check sticked actor validity
        const BaseObject * const stickedObject = _event->getActor().getObject();
        if( stickedObject->getObjectType() == BaseObject::eActor )
        {
            const Actor * const stickedActor = static_cast<const Actor *>( stickedObject );
            if( stickedActor )
            {
                //check that sticked actor is an enemy
                Faction waiterFaction = AIUtils::getFaction(m_actor);
                Faction stickedActorFaction = AIUtils::getFaction(stickedActor);
                if( AIUtils::isEnemy(waiterFaction, stickedActorFaction) )
                {
                    if( _event->isSticked() )
                    {
                        if( _event->getSpeed().m_y < getTemplate()->getCrushedVerticalSpeedThreshold() )
                        {
                            startState(WaiterState_CRUSHED);
                        }
                        else if( m_state == WaiterState_PATROL )
                        {
                            startState(WaiterState_CATCH);
                        }
                    }
                }
            }
        }
    }

    //-------------------------------------------------------
    void Ray_AIAlInfernoWaiterBehavior::processDetachEvent(const EventDetach * const _detachEvent)
    {
        m_physComponent->unstick();
    }

    //-------------------------------------------------------
    void Ray_AIAlInfernoWaiterBehavior::processPunchEvent(const PunchStim * const _event)
    {
        ITF_ASSERT( _event != NULL );

        if( _event->getReceivedHitType() == RECEIVEDHITTYPE_EARTHQUAKE )
        {
            if( m_currentAction != m_fallAction )
            {
                startCrushed();
            }
        }
    }

    //-------------------------------------------------------
    void Ray_AIAlInfernoWaiterBehavior::processActivateEvent(const EventActivate * const _event)
    {
        m_linkedToActivator = btrue;
        m_activated = _event->getActivated();
    }

    //-------------------------------------------------------
    void Ray_AIAlInfernoWaiterBehavior::processQueryCanReceiveHit(EventQueryCanReceiveHit * const _query)
    {
        ITF_ASSERT(_query);
        const HitStim * const hit = _query->getStim();
        if( hit )
        {
            //filter hit to not accept "from above" hits
            const Vec2d & hitDirection = hit->getDirection();
            const bbool isDownHit = (hitDirection.m_y < MTH_EPSILON) && (f32_Abs(hitDirection.m_x) < MTH_EPSILON);
            _query->setCanReceiveHit(!isDownHit);
        }
        else
        {
            _query->setCanReceiveHit(bfalse);
        }
    }

    //-------------------------------------------------------
    void Ray_AIAlInfernoWaiterBehavior::startFlamesFX()
    {
        if( m_fxController != NULL )
        {
            //if fx isn't launched start it
            if( m_flamesFXHanfle == U32_INVALID )
            {
                m_flamesFXHanfle = m_fxController->playFX( getTemplate()->getFlamesFXName() );
            }
        }
    }

    //-------------------------------------------------------
    void Ray_AIAlInfernoWaiterBehavior::stopFlamesFX()
    {
        if( m_fxController != NULL )
        {
            //if fx isn't launched start it
            if( m_flamesFXHanfle != U32_INVALID )
            {
                m_fxController->stopFXFromHandle( m_flamesFXHanfle );
                m_flamesFXHanfle = U32_INVALID;
            }
        }
    }

    //-------------------------------------------------------
    bbool Ray_AIAlInfernoWaiterBehavior::isWaiterSpeedXPositive() const
    {
        const Vec2d speed = m_physComponent->getSpeed();
        return speed.dot(Vec2d::Right) > 0.0f;
    }

#ifdef ITF_SUPPORT_EDITOR
    //-------------------------------------------------------
    void Ray_AIAlInfernoWaiterBehavior::drawEdit(ActorDrawEditInterface * _drawInterface, u32 _flags) const
    {
        Super::drawEdit(_drawInterface, _flags);

        //draw detection AABB
        const f32 angle = m_actor->getAngle();
        AABB aabb = getTemplate()->getLongRangeAttackDetectionRange();
        if (m_actor->isFlipped())
        {
            aabb.FlipHorizontaly(bfalse);
        }
        DebugDraw::AABB( m_actor->getPos(), angle, aabb, Color::red() );
        
        DebugDraw::text(m_actor->getPos(), Vec2d(0.0f, 50.0f), Color::white(), Color::black(), "Current state : %d", m_state);
    }
#endif // ITF_SUPPORT_EDITOR

    IMPLEMENT_OBJECT_RTTI(Ray_AIAlInfernoWaiterBehavior_Template)

    BEGIN_SERIALIZATION_CHILD(Ray_AIAlInfernoWaiterBehavior_Template)
        SERIALIZE_OBJECT_WITH_FACTORY("fall", m_fallAction, ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("patrol", m_patrolAction, ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("longRangeAttack", m_attackLongRangeAction, ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("crushed", m_crushedAction, ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("uturn_walk", m_uturnWalkAction, ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_OBJECT("longRangeAttackDetectionRange", m_longRangeAttackDetectionRange);
        SERIALIZE_MEMBER("flamesFXName", m_flamesFXName);
        SERIALIZE_MEMBER("catchWalkAnimation", m_catchWalkAnimation);
        SERIALIZE_MEMBER("crushedVerticalSpeedThreshold", m_crushedVerticalSpeedThreshold);
        SERIALIZE_MEMBER("attackTimeThreshold", m_attackTimeThreshold);
        SERIALIZE_MEMBER("brakingForce", m_brakingForce);
    END_SERIALIZATION()

    //-------------------------------------------------------
    Ray_AIAlInfernoWaiterBehavior_Template::Ray_AIAlInfernoWaiterBehavior_Template()
    : m_fallAction(NULL)
    , m_patrolAction(NULL)
    , m_attackLongRangeAction(NULL)
    , m_crushedAction(NULL)
    , m_uturnWalkAction(NULL)
    , m_flamesFXName(StringID::Invalid)
    , m_catchWalkAnimation(StringID::Invalid)
    , m_crushedVerticalSpeedThreshold(-30.0f)
    , m_attackTimeThreshold(0.5f)
    , m_brakingForce(10.0f)
    {
    }

    //-------------------------------------------------------
    Ray_AIAlInfernoWaiterBehavior_Template::~Ray_AIAlInfernoWaiterBehavior_Template()
    {
        SF_DEL(m_fallAction);
        SF_DEL(m_patrolAction);
        SF_DEL(m_attackLongRangeAction);
        SF_DEL(m_crushedAction);
        SF_DEL(m_uturnWalkAction);
    }
}
