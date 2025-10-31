#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AIDARKTOONATTACKBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Darktoon/Ray_AIDarktoonAttackBehavior.h"
#endif //_ITF_RAY_AIDARKTOONATTACKBEHAVIOR_H_

#ifndef _ITF_RAY_AIGROUNDCOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_GroundAIComponent.h"
#endif //_ITF_RAY_AIGROUNDCOMPONENT_H_

#ifndef _ITF_RAY_AIPERFORMHITACTION_H_
#include "rayman/gameplay/AI/Actions/Ray_AIPerformHitAction.h"
#endif //_ITF_RAY_AIPERFORMHITACTION_H_

#ifndef _ITF_AIJUMPACTION_H_
#include "gameplay/ai/Actions/AIJumpAction.h"
#endif //_ITF_AIJUMPACTION_H_

#ifndef _ITF_AIFALLACTION_H_
#include "gameplay/ai/Actions/AIFallAction.h"
#endif //_ITF_AIFALLACTION_H_

#ifndef _ITF_AIPLAYANIMACTION_H_
#include "gameplay/ai/Actions/AIPlayAnimAction.h"
#endif //_ITF_AIPLAYANIMACTION_H_


#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

#ifndef _ITF_RAY_AVOIDANCEMANAGER_H_
#include "rayman/gameplay/Managers/Ray_AvoidanceManager.h"
#endif //_ITF_RAY_AVOIDANCEMANAGER_H_

#ifndef _ITF_RAY_FIGHTMANAGER_H_
#include "rayman/gameplay/Managers/Ray_FightManager.h"
#endif //_ITF_RAY_FIGHTMANAGER_H_

#ifndef _ITF_RAY_AVOIDANCECOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_AvoidanceComponent.h"
#endif //_ITF_RAY_AVOIDANCECOMPONENT_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_


namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_AIDarktoonAttackBehavior_Template)

    BEGIN_SERIALIZATION_CHILD(Ray_AIDarktoonAttackBehavior_Template)
        // ACTIONS
        SERIALIZE_OBJECT_WITH_FACTORY("detect", m_detect, ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("backDetect", m_backDetect, ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("fall", m_fall, ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("giveUp", m_giveUp, ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("outOfRange", m_outOfRange, ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("attack", m_attack, ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("jumpAttack", m_jumpAttack, ACTORSMANAGER->getAIActionsFactory());

        SERIALIZE_MEMBER("minIdleTime", m_minIdleTime);
        SERIALIZE_MEMBER("minWalkTime", m_minWalkTime);
        SERIALIZE_MEMBER("slopeDetectionRange", m_slopeDetectionRange);
        SERIALIZE_MEMBER("maxSlopeAngleUp", m_maxSlopeAngleUp);
        SERIALIZE_MEMBER("maxSlopeAngleDown", m_maxSlopeAngleDown);
        SERIALIZE_MEMBER("giveUpDistance", m_giveUpDistance);
        SERIALIZE_CONTAINER_OBJECT("moveRanges", m_moveRanges);
        SERIALIZE_MEMBER("outOfRangeAngle", m_outOfRangeAngle);
        SERIALIZE_MEMBER("outOfRangeDistance", m_outOfRangeDistance);
        SERIALIZE_MEMBER("moveCycleTime", m_moveCycleTime);
        SERIALIZE_MEMBER("attackDetectionRange", m_attackDetectionRange);
        SERIALIZE_MEMBER("setVictim", m_setVictim);

    END_SERIALIZATION()

    Ray_AIDarktoonAttackBehavior_Template::Ray_AIDarktoonAttackBehavior_Template()
    : m_detect(NULL)
    , m_backDetect(NULL)
    , m_fall(NULL)
    , m_giveUp(NULL)
    , m_outOfRange(NULL)
    , m_minIdleTime(1.f)
    , m_minWalkTime(1.f)
    , m_jumpAttack(NULL)
    , m_attack(NULL)
    , m_slopeDetectionRange(1.f)
    , m_maxSlopeAngleUp(btrue,30.f)
    , m_maxSlopeAngleDown(btrue,30.f)
    , m_giveUpDistance(10.f)
    , m_outOfRangeAngle(btrue,60.0f)
    , m_outOfRangeDistance(2.0f)
    , m_moveCycleTime(1.5f)
    , m_attackDetectionRange(3.0f)
    , m_setVictim(btrue)
    {
    }

    Ray_AIDarktoonAttackBehavior_Template::~Ray_AIDarktoonAttackBehavior_Template()
    {
        SF_DEL(m_detect);
        SF_DEL(m_backDetect);
        SF_DEL(m_fall);
        SF_DEL(m_giveUp);
        SF_DEL(m_outOfRange);
        SF_DEL(m_attack);
        SF_DEL(m_jumpAttack);
    }
    BEGIN_SERIALIZATION_SUBCLASS(Ray_AIDarktoonAttackBehavior_Template, MoveRange)
        SERIALIZE_MEMBER("maxDistance", m_maxDistance);
        SERIALIZE_MEMBER("walkForce", m_walkForce);
    END_SERIALIZATION()

    IMPLEMENT_OBJECT_RTTI(Ray_AIDarktoonAttackBehavior)

    BEGIN_VALIDATE_BEHAVIOR(Ray_AIDarktoonAttackBehavior)
        VALIDATE_BEHAVIOR_PARAM("fall", m_fall, "this action is mandatory");
        VALIDATE_BEHAVIOR_PARAM("", m_groundAIComponent, "At the moment we require this behavior to be contained in a Ray_GroundAIComponent; maybe the dependency could be removed...");
        VALIDATE_BEHAVIOR_PARAM("", m_avoidanceComponent, "Ray_AIDarktoonAttackBehavior requires a Ray_AvoidanceComponent");
    END_VALIDATE_BEHAVIOR()


    Ray_AIDarktoonAttackBehavior::Ray_AIDarktoonAttackBehavior()
        : Super()
        , m_groundAIComponent(NULL)
        , m_avoidanceComponent(NULL)

        , m_detect(NULL)
        , m_backDetect(NULL)
        , m_fall(NULL)
        , m_giveUp(NULL)
        , m_outOfRange(NULL)
        , m_attack(NULL)
        , m_jumpAttack(NULL)

        , m_target(NULL)
        , m_targetIsLeft(bfalse)
        , m_targetDistance(0.f)

        , m_blocked(BLOCKED_NONE)

        , m_avoidanceTimer(0.f)
        , m_neededToMove(bfalse)
        , m_currentMoveRangeIndex(U32_INVALID)

        , m_lastFlipped(bfalse)
        , m_wasLookingLeft(bfalse)

        , m_timer(0.0f)
        , m_nextAttackIsJumpAttack(btrue)
    {
    }

    Ray_AIDarktoonAttackBehavior::~Ray_AIDarktoonAttackBehavior()
    {
    }



    void Ray_AIDarktoonAttackBehavior::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        m_detect = static_cast<AIAction*>(createAiAction(getTemplate()->getDetect()));
        m_backDetect = static_cast<AIAction*>(createAiAction(getTemplate()->getBackDetect()));
        m_fall = static_cast<AIFallAction*>(createAiAction(getTemplate()->getFall()));
        m_giveUp = static_cast<AIPlayAnimAction*>(createAiAction(getTemplate()->getGiveUp()));
        m_outOfRange = static_cast<AIAction*>(createAiAction(getTemplate()->getOutOfRange()));
        m_attack = static_cast<AIPerformHitAction*>(createAiAction(getTemplate()->getAttack()));
        m_jumpAttack = static_cast<AIPerformHitAction*>(createAiAction(getTemplate()->getJumpAttack()));
        

        Super::onActorLoaded(_hotReload);

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventAttackGranted,3058211789),m_aiComponent);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventTriggerChangePage,3503198481),m_aiComponent);

        m_groundAIComponent = m_aiComponent->DynamicCast<Ray_GroundAIComponent>(ITF_GET_STRINGID_CRC(Ray_GroundAIComponent,2230428483));  // mandatory, validated
        m_avoidanceComponent = m_actor->GetComponent<Ray_AvoidanceComponent>();     // mandatory, validated
    }

    void Ray_AIDarktoonAttackBehavior::onActivate()
    {
        Super::onActivate();

        ITF_ASSERT(m_targetRef.isValid());

        m_target = NULL;
        m_blocked = BLOCKED_NONE;

        m_neededToMove = bfalse;
        m_avoidanceTimer = 0;

        m_avoidanceComponent->setEnabled(btrue);

        m_lastFlipped = m_actor->isFlipped();


        m_target = m_targetRef.getActor();
        if (m_target)
        {
            Vec2d vecToVictim = m_target->get2DPos() - m_actor->get2DPos();
            m_targetIsLeft = vecToVictim.dot(Vec2d::Right) < 0;
            m_wasLookingLeft = m_actor->isFlipped();
        }

        if ( !m_physComponent->getStickedPolyline() )
        {
            // no floor: fall
            startFall();
            return;
        }

        m_timer = getTemplate()->getMoveCycleTime();

        startDetect();
    }

    void Ray_AIDarktoonAttackBehavior::onDeactivate()
    {
        Super::onDeactivate();

        m_targetRef.invalidate();
        m_target = NULL;

        m_avoidanceComponent->setEnabled(bfalse);

        setBackPedalingInput(bfalse);
    }


    void Ray_AIDarktoonAttackBehavior::update( f32 _dt  )
    {
        m_target = m_targetRef.getActor();

        if ( AIUtils::isTargetStillValid(m_actor, m_target) )
        {


            Vec2d vecToVictim = m_target->get2DPos() - m_actor->get2DPos();
            m_targetIsLeft = vecToVictim.dot(Vec2d::Right) < 0;
            m_targetDistance = vecToVictim.norm();

            if ( m_currentAction == m_idle )
            {
                if ( m_targetIsLeft != m_wasLookingLeft )
                {

                    m_wasLookingLeft = m_targetIsLeft;
                    performUTurnImmediate();
                }
                else
                {
                    m_timer = Max(0.0f,m_timer -_dt);
                    updateIdle(_dt);
                }
            }
            else if ( m_currentAction == m_move )
            {
                if ( m_targetIsLeft != m_wasLookingLeft )
                {
                    m_wasLookingLeft = m_targetIsLeft;
                    performUTurnImmediate();
                }
                else
                {
                    m_timer = Max(0.0f,m_timer -_dt);
                    updateMove(_dt);
                }

            }
            else if ( isAttacking() )
            {
                m_timer = Max(0.0f,m_timer -_dt);
                updateAttack(_dt);
            }

            // update the actions last
            Super::update(_dt);

#ifdef ITF_SUPPORT_DEBUGFEATURE
            if ( m_target && RAY_AVOIDANCEMANAGER->getDrawDebug() )
            {
                GFX_ADAPTER->drawArrow(m_actor->getPos(), m_target->getPos(), Color::red(), 0.1f);
            }
#endif // ITF_SUPPORT_DEBUGFEATURE
        }
        else
        {
            // Don't interrupt attack
            if ( !isAttacking() )
            {
                m_aiComponent->onBehaviorFinished();
            }
            else
            {
                Super::update(_dt);
            }
        }
        m_obstacle.invalidate();
    }

    void Ray_AIDarktoonAttackBehavior::onActionFinished()
    {
        Super::onActionFinished();

        if ( m_currentAction == m_attack )
        {
            onFinishedAttack();
        }
        else if ( m_currentAction == m_jumpAttack )
        {
            onFinishedJumpAttack();
        }
        else if ( m_currentAction == m_detect )
        {
            onFinishedDetect();
        }
        else if ( m_currentAction == m_fall )
        {
            onFinishedFall();
        }
        else if ( m_currentAction == m_giveUp )
        {
            onFinishedGiveUp();
        }
        else if (m_outOfRange && m_currentAction == m_outOfRange)
        {
            onFinishedOutOfRange();
        }
        else if (m_backDetect && m_currentAction == m_backDetect)
        {
            onFinishedBackDetect();
        }
    }


    void Ray_AIDarktoonAttackBehavior::updateIdle( f32 _dt )
    {
        PolyLine* poly = m_physComponent->getStickedPolyline();

        if ( !poly )
        {
            // no floor: fall
            startFall();
            return;
        }

        if (checkGiveUpRange())
        {
            return;
        }

        checkBlocked(poly);

        // walk towards target, avoiding other actors
        m_avoidanceTimer += _dt;
        avoidOtherActors();

        if (checkOutOfRange())
        {
            return;
        }

        if (m_timer == 0.0f)
        {
            if (m_blocked == BLOCKED_LEFT || m_blocked == BLOCKED_RIGHT)
            {
                m_nextAttackIsJumpAttack = bfalse;
            }
            else
            {
                m_nextAttackIsJumpAttack = btrue;
            }

            f32 holeDepth;
            if ( AIUtils::hasHoleInFront(
                m_actor,
                m_targetIsLeft,
                poly,
                m_physComponent->getStickedEdgeIndex(),
                getTemplate()->getAttackDetectionRange(),
                m_physComponent->getRadius(),
                m_aiComponent->getCurrentGravityDir(),
                holeDepth) )
            {
                m_nextAttackIsJumpAttack = bfalse;
            }
            const PolyLineEdge * wallEdge = NULL;
            Vec2d wallTop;
            f32 wallHeight = 0.0f;
            if (AIUtils::hasWallInFront(
                m_actor,
                m_targetIsLeft,
                poly,
                m_physComponent->getStickedEdgeIndex(),
                getTemplate()->getAttackDetectionRange(),
                m_physComponent->getRadius(),
                m_aiComponent->getCurrentGravityDir(),
                wallHeight,
                &wallEdge, 
                &wallTop
                ))
            {
                if (wallHeight > getTemplate()->getObstacleJumpHeight())
                m_nextAttackIsJumpAttack = bfalse;
            }

            startAttack();
        }

    }

    void Ray_AIDarktoonAttackBehavior::updateMove( f32 _dt )
    {
        PolyLine* poly = m_physComponent->getStickedPolyline();

        if ( !poly )
        {
            // no floor: fall
            startFall();
            return;
        }
        Actor * obstacle = m_obstacle.getActor();
        if (obstacle)
        {
            const Vec2d dirContact = obstacle->get2DPos()-m_actor->get2DPos();
            const Vec2d lookDir = AIUtils::getLookDir(m_actor,m_actor->isFlipped());
            if (dirContact.dot(lookDir) > 0.0f)
            {
                if (m_timer == 0.0f)
                {
                    m_timer = getTemplate()->getUturnDelay();
                    performUTurnImmediate();
                    return ;
                }
                else
                {
                    startIdle();
                    return ;
                }
            }
        }

        if (checkGiveUpRange())
        {
            return;
        }

        if (checkBlockingObstacle())
        {
            startIdle();
            return;
        }

        if (checkHole(poly))
        {
            return;
        }

        if (checkSlope(poly))
        {
            return;
        }

        if (checkStuck(_dt))
        {
            return;
        }

        // walk towards target, avoiding other actors
        m_avoidanceTimer += _dt;
        avoidOtherActors();

        if (checkOutOfRange())
        {
            return;
        }

        if (m_timer == 0.0f)
        {
            if (m_blocked == BLOCKED_LEFT || m_blocked == BLOCKED_RIGHT)
            {
                m_nextAttackIsJumpAttack = bfalse;
            }
            else
            {
                m_nextAttackIsJumpAttack = btrue;
            }
            f32 holeDepth;
            if ( AIUtils::hasHoleInFront(
                m_actor,
                m_targetIsLeft,
                poly,
                m_physComponent->getStickedEdgeIndex(),
                getTemplate()->getAttackDetectionRange(),
                m_physComponent->getRadius(),
                m_aiComponent->getCurrentGravityDir(),
                holeDepth) )
            {
                m_nextAttackIsJumpAttack = bfalse;
            }

            const PolyLineEdge * wallEdge = NULL;
            Vec2d wallTop;
            f32 wallHeight = 0.0f;
            if (AIUtils::hasWallInFront(
                m_actor,
                m_targetIsLeft,
                poly,
                m_physComponent->getStickedEdgeIndex(),
                getTemplate()->getAttackDetectionRange(),
                m_physComponent->getRadius(),
                m_aiComponent->getCurrentGravityDir(),
                wallHeight,
                &wallEdge, 
                &wallTop
                ))
            {
                if (wallHeight > getTemplate()->getObstacleJumpHeight())
                    m_nextAttackIsJumpAttack = bfalse;
            }
            startAttack();
        }

    }




    void Ray_AIDarktoonAttackBehavior::updateAttack( f32 _dt )
    {
    }


    void Ray_AIDarktoonAttackBehavior::onEvent( Event* _event )
    {
        Super::onEvent(_event);
        if ( Ray_EventTriggerChangePage * onChangePage = _event->DynamicCast<Ray_EventTriggerChangePage>(ITF_GET_STRINGID_CRC(Ray_EventTriggerChangePage,3503198481)))
        {
            m_obstacle = onChangePage->getSender();
        }

    }

    void Ray_AIDarktoonAttackBehavior::startAttack()
    {
        m_stuckTimer = 0.f;
        m_blocked = BLOCKED_NONE;
        if(m_nextAttackIsJumpAttack)
        {
            m_jumpAttack->setData(m_targetRef);
            setAction(m_jumpAttack);
        }
        else
        {
            if (getTemplate()->getSetVictim())
                m_attack->setData(m_targetRef);
            setAction(m_attack);
        }
            
    }

    void Ray_AIDarktoonAttackBehavior::startDetect()
    {
        if (m_detect && !m_groundAIComponent->hadEnemyInSight() )
        {
            if (m_backDetect && ( m_targetIsLeft != m_wasLookingLeft ))
                setAction(m_backDetect);
            else
                setAction(m_detect);
        }
        else
        {
            onFinishedDetect();
        }
    }

    void Ray_AIDarktoonAttackBehavior::startIdle()
    {
        if ( m_targetIsLeft != m_wasLookingLeft )
        {
            m_wasLookingLeft = m_targetIsLeft;
            performUTurnImmediate();
        }
        else
        {
            setAction(m_idle);
        }
    }
 
    void Ray_AIDarktoonAttackBehavior::startWalk()
    {
        m_stuckTimer = 0.f;
        m_blocked = BLOCKED_NONE;
        setAction(m_move);
    }

    void Ray_AIDarktoonAttackBehavior::startFall()
    {
        m_blocked = BLOCKED_NONE;
        setAction(m_fall);
    }

    void Ray_AIDarktoonAttackBehavior::startGiveUp()
    {
        if (m_giveUp)
        {
            setAction(m_giveUp);
        }
        else
        {
            onFinishedGiveUp();
        }
    }


    void Ray_AIDarktoonAttackBehavior::setBlocked()
    {
        if ( m_move->isWalkingLeft() )
        {
            m_blocked = BLOCKED_LEFT;
        }
        else
        {
            m_blocked = BLOCKED_RIGHT;
        }
    }



    void Ray_AIDarktoonAttackBehavior::avoidOtherActors()
    {
        if (m_targetIsLeft && m_blocked == BLOCKED_LEFT)
            return;
        if (!m_targetIsLeft && m_blocked == BLOCKED_RIGHT)
            return;

        Ray_SquadMemberData* squadData = m_avoidanceComponent->getSquadData();
        ITF_ASSERT(squadData);
        if (!squadData) // Happened on 201/11/5th - TRAC #913
            return;

        Ray_SquadMemberData::AvoidanceData data;
        squadData->avoidNeighbors(m_targetIsLeft, data);

        squadData->setIsBlocked(data.blocked);


        f32 _penetration;

        f32 result = f32_Abs(data.penetrationLeft) - f32_Abs(data.penetrationRight);
        if (result > MTH_EPSILON)
        {
            _penetration = data.penetrationLeft;
        }
        else if (result < -MTH_EPSILON)
        {
            _penetration = -data.penetrationRight;
        }
        else
        {
            // TODO: should we force-stop here?
            _penetration = 0;
        }

        if (_penetration != 0)
        {
            ITF_ASSERT(f32_Abs(_penetration) > squadData->getMinDelta());
        }


        bbool needToMove = f32_Abs(_penetration) > squadData->getMinDelta();
        bbool needToKeepMoving = m_move->isWalkingLeft() ? data.destinationLeft < 0 : data.destinationRight > 0;

        f32 destination = 0;
        if (needToMove)
        {
            destination = _penetration;
        }
        else if (needToKeepMoving)
        {
            destination = m_move->isWalkingLeft() ? data.destinationLeft : data.destinationRight;
        }

        Vec2d finalDest = squadData->getPosition();
        finalDest.m_x += destination * m_avoidanceComponent->getAnticipationCoeff();
        squadData->setDestination(finalDest);
        //Vec2d dbgPos = squadData->getPosition();
        //Vec2d dbgDest = dbgPos;
        //d
        //GFX_ADAPTER->drawArrow(squadData->getPosition(), squadData->getPosition() + destination);


        bbool walking = (m_currentAction == m_move);

        bbool walkLeft = m_move->isWalkingLeft();
        if (needToMove)
            walkLeft = _penetration < 0.f;

        bbool uTurn = (walkLeft != m_move->isWalkingLeft());

        m_move->setWalkLeft(walkLeft);


        if (needToMove && !m_neededToMove && !walking)
        {
            // stay idle for a while before starting
            m_avoidanceTimer = 0;
        }
        m_neededToMove = needToMove;

        if (needToMove)
        {
            // need to move

#ifdef ITF_SUPPORT_DEBUGFEATURE
            if (RAY_AVOIDANCEMANAGER->getDrawDebug())
            {
                GFX_ADAPTER->drawArrow(m_actor->getPos(), m_actor->getPos() + Vec3d(_penetration, 0, 0), 0, 1, 0, 0.1f);
                DebugDraw::circle(m_actor->getPos(),m_avoidanceComponent->getSquadData()->getRadius());
            }
#endif // ITF_FINAL

            if (uTurn)
            {
                // need to go in opposite dir
                if (walking)
                {
                    // stop (immediately) before uturn
                    setAction(m_idle);
                    m_avoidanceTimer = 0;
                }
                else if (m_avoidanceTimer > getTemplate()->getMinIdleTime() && !isBlocked())
                {
                    // start walking
                    setAction(m_move);
                    m_avoidanceTimer = 0;
                }
                // else: don't start yet (to avoid micro-movements)
            }
            else
            {
                // need to go in same dir
                if (!walking)
                {
                    if (m_avoidanceTimer > getTemplate()->getMinIdleTime() && !isBlocked())
                    {
                        // start walking
                        setAction(m_move);
                        m_avoidanceTimer = 0;
                    }
                }
                // else: keep walking in the same dir
            }
        }
        else if (needToKeepMoving && walking && !uTurn)
        {
            // keep walking in the same dir
        }
        else
        {
            // no need to move
            if (walking)
            {
                if (m_avoidanceTimer > getTemplate()->getMinWalkTime())
                {
                    // stop walking
                    setAction(m_idle);
                    m_avoidanceTimer = 0;
                }
                // else: don't stop yet (to avoid micro-movements)
            }
        }



        f32 distance = f32_Abs(destination);

        const SafeArray<Ray_AIDarktoonAttackBehavior_Template::MoveRange> & moveRange = getTemplate()->getMoveRanges();
        for (u32 i = moveRange.size(); i > 0; i--)
        {
            const Ray_AIDarktoonAttackBehavior_Template::MoveRange& range = moveRange[i - 1];
            if (distance < range.m_maxDistance)
            {
                m_move->setWalkForce(range.m_walkForce);
                m_currentMoveRangeIndex = i;
            }
        }
    }

    void Ray_AIDarktoonAttackBehavior::updateAnimInputs()
    {
        Super::updateAnimInputs();

        bbool backPedaling = ( m_targetIsLeft != m_move->isWalkingLeft() );
        setBackPedalingInput(backPedaling);

        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(MoveRange,2518978727), m_currentMoveRangeIndex);
    }

    void Ray_AIDarktoonAttackBehavior::setBackPedalingInput( bbool backPedaling )
    {
        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(BackPedaling,1568802144), backPedaling);
    }

    bbool Ray_AIDarktoonAttackBehavior::checkStuck( f32 _dt )
    {
        if ( f32_Abs(m_physComponent->getWorldSpeedAverageX()) < 0.1f )
        {
            m_stuckTimer += _dt;

            if ( m_stuckTimer >= getTemplate()->getStuckDelay() )
            {
                setBlocked();
                startIdle();
                return btrue;
            }
        }
        else
        {
            m_stuckTimer = 0.f;
        }

        return bfalse;
    }

    bbool Ray_AIDarktoonAttackBehavior::checkGiveUpRange()
    {
        if ( m_targetDistance > getTemplate()->getGiveUpDistance() )
        {
            startGiveUp();
            return btrue;
        }

        return bfalse;
    }

    bbool Ray_AIDarktoonAttackBehavior::checkBlocked(PolyLine* poly)
    {
        f32 holeDepth;
        if ( AIUtils::hasHoleInFront(
            m_actor,
            m_targetIsLeft,
            poly,
            m_physComponent->getStickedEdgeIndex(),
            getTemplate()->getHoleDetectionRange(),
            m_physComponent->getRadius(),
            m_aiComponent->getCurrentGravityDir(),
            holeDepth) )
        {
            setBlocked();
            return btrue;
        }
        f32 slope = AIUtils::getSlopeAngle(
            m_actor->get2DPos(),
            poly, m_physComponent->getStickedEdgeIndex(),
            m_move->isWalkingLeft(),
            getTemplate()->getSlopeDetectionRange(),
            m_aiComponent->getCurrentGravityDir(),
            NULL);

        if ( !isValidSlope(slope) )
        {
            setBlocked();
            return btrue;
        }
       return checkBlockingObstacle();
    }


    bbool Ray_AIDarktoonAttackBehavior::checkHole( PolyLine* poly )
    {
        f32 holeDepth;
        if ( AIUtils::hasHoleInFront(
            m_actor,
            m_targetIsLeft,
            poly,
            m_physComponent->getStickedEdgeIndex(),
            getTemplate()->getHoleDetectionRange(),
            m_physComponent->getRadius(),
            m_aiComponent->getCurrentGravityDir(),
            holeDepth) )
        {
            setBlocked();
            startIdle();
            return btrue;
        }

        return bfalse;
    }
    bbool Ray_AIDarktoonAttackBehavior::checkSlope(PolyLine * poly)
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
            setBlocked();
            startIdle();
            return btrue;
        }
        return bfalse;
    }
    bbool Ray_AIDarktoonAttackBehavior::isValidSlope( f32 _slopeAngle ) const
    {
        if ( _slopeAngle >= 0.f )
        {
            return _slopeAngle <= getTemplate()->getMaxSlopeAngleUp().ToRadians();
        }
        else
        {
            return -_slopeAngle <= getTemplate()->getMaxSlopeAngleUp().ToRadians();
        }
    }

    bbool Ray_AIDarktoonAttackBehavior::canBeInterrupted() const
    {
        return !m_currentAction->IsClassCRC(ITF_GET_STRINGID_CRC(Ray_AIPerformHitAction,1258927796));
    }


    bbool Ray_AIDarktoonAttackBehavior::checkBlockingObstacle()
    {
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
            if (polyLine )
            {
                if (polyLine->getOwnerActor() == m_actor)
                    continue;
            }

            PolyLine* contactPoly(NULL);
            const PolyLineEdge* contactEdge(NULL);
            AIUtils::getPolyLine(contact.m_collidableUserData, contact.m_edgeIndex, contactPoly, contactEdge);

            if (contactPoly && contactEdge)
            {
                Vec2d normal = -contactEdge->m_normalizedVector.getPerpendicular();
                f32 angle = lookDir.getShortestAngle(normal);
                bbool blocking = f32_Abs(angle) < (30.f * MTH_DEGTORAD);
                if (blocking)
                {
                    return checkBlockingPolyline(contactPoly, contactEdge, contact.m_edgeIndex, contact.m_contactPoint);
                }
            }
        }
        return bfalse;
    }

    bbool Ray_AIDarktoonAttackBehavior::checkBlockingPolyline( PolyLine* _poly, const PolyLineEdge* _edge, u32 _edgeIndex, const Vec2d& _contactPoint )
    {
        if (!_poly || !_edge)
        {
            return bfalse;
        }

        bbool left = _edge->m_vector.getPerpendicular().dot(Vec2d::Right) > 0;  // if the edge normal goes right, that means we're hitting it from the left
        f32 edgeHeight;
        AIUtils::hasWallInFront(_contactPoint, left, _poly, _edgeIndex, 0.5f, 5.f, edgeHeight, bfalse);
  
        setBlocked();
        return btrue;

    }

    void Ray_AIDarktoonAttackBehavior::onFinishedGiveUp()
    {
        performUTurnImmediate();
        m_aiComponent->onBehaviorFinished();
    }

    void Ray_AIDarktoonAttackBehavior::onFinishedAttack()
    {
        m_timer = getTemplate()->getMoveCycleTime();
        startIdle();
    }

    void Ray_AIDarktoonAttackBehavior::onFinishedDetect()
    {
        startIdle();
    }

    void Ray_AIDarktoonAttackBehavior::onFinishedFall()
    {
        startIdle();
    }

    void Ray_AIDarktoonAttackBehavior::onFinishedJump()
    {
        startWalk();
    }

    void Ray_AIDarktoonAttackBehavior::onActionFinishedUturn()
    {
        Super::onActionFinishedUturn();
        startIdle();
    }
    void Ray_AIDarktoonAttackBehavior::onFinishedOutOfRange()
    {
        if (!checkOutOfRange())
        {
            startIdle();
        }
    }

    bbool Ray_AIDarktoonAttackBehavior::checkOutOfRange()
    {
        if (m_outOfRange )
        {

            Vec2d vecToVictim = m_target->get2DPos() - m_actor->get2DPos();
            f32 targetDistance = vecToVictim.norm();
            vecToVictim = vecToVictim/targetDistance;
            f32 angle = vecToVictim.getAngle();
            if ((targetDistance > getTemplate()->getOutOfRangeDistance()) && (angle > getTemplate()->getOutOfRangeAngle().Radians()) && (angle < (MTH_PI - getTemplate()->getOutOfRangeAngle().Radians())))
            {
                m_animComponent->resetCurTime();
                setAction(m_outOfRange);
                return btrue;
            }
        }
        return bfalse;
    }

    void Ray_AIDarktoonAttackBehavior::onFinishedBackDetect()
    {
        m_wasLookingLeft = m_targetIsLeft;
        m_actor->setFlipped(!m_actor->isFlipped());
        startIdle();
   }

    void Ray_AIDarktoonAttackBehavior::onFinishedJumpAttack()
    {
        m_timer = getTemplate()->getMoveCycleTime();
        startIdle();
   }

    bool Ray_AIDarktoonAttackBehavior::isBlocked()
    {
        return ( m_targetIsLeft && m_blocked == BLOCKED_LEFT ) ||
            ( !m_targetIsLeft && m_blocked == BLOCKED_RIGHT );
    }

    ITF::bbool Ray_AIDarktoonAttackBehavior::isAttacking() const
    {
        return m_currentAction == m_attack || m_currentAction == m_jumpAttack;
    }

    void Ray_AIDarktoonAttackBehavior::onActionFinishedBounceToLayer()
    {
        Super::onActionFinishedBounceToLayer(  );
        startIdle();
    }

    void Ray_AIDarktoonAttackBehavior::onActionFinishedBounce()
    {
        Super::onActionFinishedBounce(  );
        startIdle();
    }

#ifdef ITF_SUPPORT_EDITOR
    void Ray_AIDarktoonAttackBehavior::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        Super::drawEdit(_drawInterface, _flags);

        if (_flags != ActorComponent::DrawEditFlag_All)
            return;


        if (m_outOfRange)
        {
            DebugDraw::circle(m_actor->getPos(),getTemplate()->getOutOfRangeDistance(),Color::white(),0.0,"OutOfRangeDistance");
        }

        DebugDraw::circle(m_actor->getPos(),getTemplate()->getGiveUpDistance(),Color::red(),0.0,"GiveUpDistance");

    }
#endif // ITF_SUPPORT_EDITOR

}