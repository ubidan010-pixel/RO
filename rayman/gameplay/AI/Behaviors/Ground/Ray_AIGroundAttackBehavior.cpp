#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AIGROUNDATTACKBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ground/Ray_AIGroundAttackBehavior.h"
#endif //_ITF_RAY_AIGROUNDATTACKBEHAVIOR_H_

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
    IMPLEMENT_OBJECT_RTTI(Ray_AIGroundAttackBehavior_Template)

    BEGIN_SERIALIZATION_CHILD(Ray_AIGroundAttackBehavior_Template)
        // ACTIONS
        SERIALIZE_OBJECT_WITH_FACTORY("detect", m_detect, ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("backDetect", m_backDetect, ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("jump", m_jump, ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("fall", m_fall, ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("giveUp", m_giveUp, ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("outOfRange", m_outOfRange, ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_CONTAINER_OBJECT("attacks", m_attacks);

        SERIALIZE_MEMBER("slopeDetectionRange", m_slopeDetectionRange);
        SERIALIZE_MEMBER("maxSlopeAngleUp", m_maxSlopeAngleUp);
        SERIALIZE_MEMBER("maxSlopeAngleDown", m_maxSlopeAngleDown);
        SERIALIZE_MEMBER("giveUpDistance", m_giveUpDistance);
        SERIALIZE_MEMBER("minIdleTime", m_minIdleTime);
        SERIALIZE_MEMBER("minWalkTime", m_minWalkTime);
        SERIALIZE_CONTAINER_OBJECT("moveRanges", m_moveRanges);
        SERIALIZE_MEMBER("hackFlip", m_hackFlip);
        SERIALIZE_MEMBER("outOfRangeAngle", m_outOfRangeAngle);
        SERIALIZE_MEMBER("outOfRangeDistance", m_outOfRangeDistance);

    END_SERIALIZATION()

    Ray_AIGroundAttackBehavior_Template::Ray_AIGroundAttackBehavior_Template()
    : m_detect(NULL)
    , m_backDetect(NULL)
    , m_jump(NULL)
    , m_fall(NULL)
    , m_giveUp(NULL)
    , m_outOfRange(NULL)
    , m_slopeDetectionRange(1.f)
    , m_maxSlopeAngleUp(btrue,30.f)
    , m_maxSlopeAngleDown(btrue,30.f)
    , m_giveUpDistance(10.f)
    , m_minIdleTime(1.f)
    , m_minWalkTime(1.f)
    , m_hackFlip(bfalse)
    , m_outOfRangeAngle(btrue,60.0f)
    , m_outOfRangeDistance(2.0f)
    {
    }

    Ray_AIGroundAttackBehavior_Template::~Ray_AIGroundAttackBehavior_Template()
    {
        SF_DEL(m_detect);
        SF_DEL(m_backDetect);
        SF_DEL(m_jump);
        SF_DEL(m_fall);
        SF_DEL(m_giveUp);
        SF_DEL(m_outOfRange);
    }
    BEGIN_SERIALIZATION_SUBCLASS(Ray_AIGroundAttackBehavior_Template, AttackData)
        SERIALIZE_OBJECT("Range", m_range);
        SERIALIZE_MEMBER("cooldown", m_cooldownDuration);
        SERIALIZE_OBJECT_WITH_FACTORY("action", m_action, ACTORSMANAGER->getAIActionsFactory());
    END_SERIALIZATION()

    Ray_AIGroundAttackBehavior_Template::AttackData::~AttackData()
    {
        SF_DEL(m_action);
    }

    BEGIN_SERIALIZATION_SUBCLASS(Ray_AIGroundAttackBehavior_Template, MoveRange)
        SERIALIZE_MEMBER("maxDistance", m_maxDistance);
        SERIALIZE_MEMBER("walkForce", m_walkForce);
    END_SERIALIZATION()

    IMPLEMENT_OBJECT_RTTI(Ray_AIGroundAttackBehavior)

    BEGIN_VALIDATE_BEHAVIOR(Ray_AIGroundAttackBehavior)
        VALIDATE_BEHAVIOR_PARAM("jump", m_jump, "this action is mandatory");
        VALIDATE_BEHAVIOR_PARAM("fall", m_fall, "this action is mandatory");
        VALIDATE_BEHAVIOR_PARAM("", m_groundAIComponent, "At the moment we require this behavior to be contained in a Ray_GroundAIComponent; maybe the dependency could be removed...");
        VALIDATE_BEHAVIOR_PARAM("", m_avoidanceComponent, "Ray_AIGroundAttackBehavior requires a Ray_AvoidanceComponent");
    END_VALIDATE_BEHAVIOR()


    Ray_AIGroundAttackBehavior::Ray_AIGroundAttackBehavior()
        : Super()
        , m_groundAIComponent(NULL)
        , m_avoidanceComponent(NULL)

        , m_detect(NULL)
        , m_backDetect(NULL)
        , m_jump(NULL)
        , m_fall(NULL)
        , m_giveUp(NULL)
        , m_outOfRange(NULL)

        , m_target(NULL)
        , m_targetIsLeft(bfalse)
        , m_targetDistance(0.f)

        , m_cooldownTimer(0.f)
        , m_pendingAttack(U32_INVALID)
        , m_currentAttack(U32_INVALID)

        , m_blocked(BLOCKED_NONE)

        , m_avoidanceTimer(0.f)
        , m_neededToMove(bfalse)
        , m_currentMoveRangeIndex(U32_INVALID)

        , m_lastFlipped(bfalse)
        , m_wasLookingLeft(bfalse)
    {
    }

    Ray_AIGroundAttackBehavior::~Ray_AIGroundAttackBehavior()
    {
    }



    void Ray_AIGroundAttackBehavior::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        m_detect = static_cast<AIAction*>(createAiAction(getTemplate()->getDetect()));
        m_backDetect = static_cast<AIAction*>(createAiAction(getTemplate()->getBackDetect()));
        m_jump = static_cast<AIJumpAction*>(createAiAction(getTemplate()->getJump()));
        m_fall = static_cast<AIFallAction*>(createAiAction(getTemplate()->getFall()));
        m_giveUp = static_cast<AIPlayAnimAction*>(createAiAction(getTemplate()->getGiveUp()));
        m_outOfRange = static_cast<AIAction*>(createAiAction(getTemplate()->getOutOfRange()));

        const Ray_AIGroundAttackBehavior_Template::AttackDataList & attacks = getTemplate()->getAttacks();
        for (u32 i = 0; i < attacks.size(); ++i)
        {
            if (attacks[i].m_action)
            {
                AIPerformHitAction * action = static_cast<AIPerformHitAction*>(createAiAction(attacks[i].m_action));
                m_attacks.push_back(action);
            }
        }


        Super::onActorLoaded(_hotReload);

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventAttackGranted,3058211789),m_aiComponent);

        m_groundAIComponent = m_aiComponent->DynamicCast<Ray_GroundAIComponent>(ITF_GET_STRINGID_CRC(Ray_GroundAIComponent,2230428483));  // mandatory, validated
        m_avoidanceComponent = m_actor->GetComponent<Ray_AvoidanceComponent>();     // mandatory, validated
    }

    void Ray_AIGroundAttackBehavior::onActivate()
    {
        Super::onActivate();

        ITF_ASSERT(m_targetRef.isValid());

        m_target = NULL;
        m_pendingAttack = U32_INVALID;
        m_currentAttack = U32_INVALID;
        m_cooldownTimer = 0;    // never attack straight away, otherwise if we get hit, we'll go back and forth between behaviors and bypass the cooldown
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

        startDetect();
    }

    void Ray_AIGroundAttackBehavior::onDeactivate()
    {
        Super::onDeactivate();

        m_targetRef.invalidate();
        m_target = NULL;

        m_avoidanceComponent->setEnabled(bfalse);

        setBackPedalingInput(bfalse);
        m_cooldownTimer = 0.0f;
        m_currentAttack = U32_INVALID;
        m_pendingAttack = U32_INVALID;
    }


    void Ray_AIGroundAttackBehavior::update( f32 _dt  )
    {
        m_pendingAttack = U32_INVALID;

        bbool isAttacking = m_currentAction->IsClassCRC(ITF_GET_STRINGID_CRC(AIPerformHitAction,2299060822));

        m_target = m_targetRef.getActor();

        if ( AIUtils::isTargetStillValid(m_actor, m_target) )
        {
            if (getTemplate()->getHackFlip())
            {
                m_actor->setFlipped(m_lastFlipped);
            }

            m_cooldownTimer += _dt;

            Vec2d vecToVictim = m_target->get2DPos() - m_actor->get2DPos();
            m_targetIsLeft = vecToVictim.dot(Vec2d::Right) < 0;
            m_targetDistance = vecToVictim.norm();

            if ( m_currentAction == m_idle )
            {
                if ( m_targetIsLeft != m_wasLookingLeft )
                {
                    performUTurnImmediate();
                }
                else
                {
                    updateIdle(_dt);
                }

                m_wasLookingLeft = m_targetIsLeft;
            }
            else if ( m_currentAction == m_move )
            {
                if ( m_targetIsLeft != m_wasLookingLeft )
                {
                    performUTurnImmediate();
                }
                else
                {
                    updateMove(_dt);
                }

                m_wasLookingLeft = m_targetIsLeft;
            }
            else if ( isAttacking )
            {
                updateAttack(_dt);
            }

            // update the actions last
            Super::update(_dt);


            // HACK
            if (m_target && getTemplate()->getHackFlip())
            {
                m_lastFlipped = m_actor->isFlipped();
                m_actor->setFlipped(m_targetIsLeft);
            }

#ifdef ITF_SUPPORT_DEBUGFEATURE
            if ( m_target && RAY_AVOIDANCEMANAGER->getDrawDebug() )
            {
                GFX_ADAPTER->drawArrow(m_actor->getPos(), m_target->getPos(), Color::red(), 0.1f);
            }

            /* DEBUG 
            Vec3d pos = m_actor->getPos();
            pos.m_y += 1;
            Vec2d lookDir = AIUtils::getLookDir(m_actor, m_actor->isFlipped());
            Color color = m_backPedaling ? Color::red() : Color::green();
            //GFX_ADAPTER->drawDBGLine(pos, pos + lookDir.to3d(), color);
            GFX_ADAPTER->drawArrow(pos, pos + lookDir.to3d(), color.m_r, color.m_g, color.m_b, 0.1f);

            pos.m_y += 0.2f;
            Vec2d walkDir = Vec2d::Right * (m_move->isWalkingLeft() ? -1.f : 1.f);
            color = m_backPedaling ? Color::black() : Color::white();
            GFX_ADAPTER->drawArrow(pos, pos + walkDir.to3d(), color.m_r, color.m_g, color.m_b, 0.1f);
            */
#endif // ITF_SUPPORT_DEBUGFEATURE
        }
        else
        {
            // Don't interrupt attack
            if ( !isAttacking )
            {
                m_aiComponent->onBehaviorFinished();
            }
            else
            {
                Super::update(_dt);
            }
        }
    }

    void Ray_AIGroundAttackBehavior::onActionFinished()
    {
        Super::onActionFinished();

        if ( m_currentAction->IsClassCRC(ITF_GET_STRINGID_CRC(AIPerformHitAction,2299060822)) )
        {
            onFinishedAttack();
        }
        else if ( m_currentAction == m_detect )
        {
            onFinishedDetect();
        }
        else if ( m_currentAction == m_fall )
        {
            onFinishedFall();
        }
        else if ( m_currentAction == m_jump )
        {
            onFinishedJump();
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


    void Ray_AIGroundAttackBehavior::updateIdle( f32 _dt )
    {
        const PolyLine* poly = m_physComponent->getStickedPolyline();

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

        if (checkBlocked())
        {
            return;
        }


        // walk towards target, avoiding other actors
        m_avoidanceTimer += _dt;
        avoidOtherActors();

        // request an attack if we're in range
        if (!getBestAttack())
        {
            checkOutOfRange();
        }

    }

    void Ray_AIGroundAttackBehavior::updateMove( f32 _dt )
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

        if (checkBlockingObstacle())
        {
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

        // request an attack if we're in range
        if (!getBestAttack())
        {
            checkOutOfRange();
        }
    }


    // see if any of our attacks is in range, and request an attack
    bbool Ray_AIGroundAttackBehavior::getBestAttack()
    {
        //ASSUMING IN THIS BEHAVIOR WE ARE ALWAYS LOOKING IN THE DIRECTION OF THE TARGET

        bbool lookRight = (m_target->get2DPos()- m_actor->get2DPos()).dot(Vec2d::Right) > 0.0f;

        for (u32 i = 0; i < getTemplate()->getAttacks().size(); i++)
        {
            const Ray_AIGroundAttackBehavior_Template::AttackData* attack = &getTemplate()->getAttacks()[i];

            if (attack->isValid(m_cooldownTimer,lookRight,m_actor,m_target) && m_attacks[i]->isValid(m_actor,m_target))
            {
                m_pendingAttack = i;
                RAY_FIGHTMANAGER->requestAttack(m_actor->getRef(), m_targetRef);
                return btrue;
            }
        }
        return bfalse;
    }

    void Ray_AIGroundAttackBehavior::updateAttack( f32 _dt )
    {
    }


    void Ray_AIGroundAttackBehavior::onEvent( Event* _event )
    {
        Super::onEvent(_event);

        if ( m_currentAction != m_uturn)
        {
            if ( m_targetIsLeft != m_wasLookingLeft )
            {
            }
            else
            {
                if (Ray_EventAttackGranted* attackGranted = _event->DynamicCast<Ray_EventAttackGranted>(ITF_GET_STRINGID_CRC(Ray_EventAttackGranted,3058211789)) )
                {
                    attackGranted->setAttackUsed(btrue);
                    startAttack();
                }
            }
        }
    }

    void Ray_AIGroundAttackBehavior::startAttack()
    {
        m_currentAttack = m_pendingAttack;
        ITF_ASSERT(m_currentAttack < m_attacks.size());
        if (m_currentAttack>= m_attacks.size())
            return;

        m_cooldownTimer = 0;
        m_avoidanceTimer = 0;

        // make sure we face the enemy
        m_actor->setFlipped(m_targetIsLeft);
        m_attacks[m_currentAttack]->setData(m_targetRef);
        setAction(m_attacks[m_currentAttack]);
    }

    void Ray_AIGroundAttackBehavior::startDetect()
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

    void Ray_AIGroundAttackBehavior::startIdle()
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
 
    void Ray_AIGroundAttackBehavior::startWalk()
    {
        m_stuckTimer = 0.f;
        m_blocked = BLOCKED_NONE;
        setAction(m_move);
    }

    void Ray_AIGroundAttackBehavior::startJump()
    {
        m_blocked = BLOCKED_NONE;
        setAction(m_jump);
    }

    void Ray_AIGroundAttackBehavior::startFall()
    {
        m_blocked = BLOCKED_NONE;
        setAction(m_fall);
    }

    void Ray_AIGroundAttackBehavior::startGiveUp()
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


    void Ray_AIGroundAttackBehavior::setBlocked()
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



    void Ray_AIGroundAttackBehavior::avoidOtherActors()
    {
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
            }
#endif // ITF_SUPPORT_DEBUGFEATURE

            if (uTurn)
            {
                // need to go in opposite dir
                if (walking)
                {
                    // stop (immediately) before uturn
                    setAction(m_idle);
                    m_avoidanceTimer = 0;
                }
                else if (m_avoidanceTimer > getTemplate()->getMinIdleTime())
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
                    if (m_avoidanceTimer > getTemplate()->getMinIdleTime())
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

        const SafeArray<Ray_AIGroundAttackBehavior_Template::MoveRange> & moveRange = getTemplate()->getMoveRanges();
        for (u32 i = moveRange.size(); i > 0; i--)
        {
            const Ray_AIGroundAttackBehavior_Template::MoveRange& range = moveRange[i - 1];
            if (distance < range.m_maxDistance)
            {
                m_move->setWalkForce(range.m_walkForce);
                m_currentMoveRangeIndex = i;
            }
        }
    }

    void Ray_AIGroundAttackBehavior::updateAnimInputs()
    {
        Super::updateAnimInputs();

        bbool backPedaling = ( m_targetIsLeft != m_move->isWalkingLeft() );
        setBackPedalingInput(backPedaling);

        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(MoveRange,2518978727), m_currentMoveRangeIndex);
    }

    void Ray_AIGroundAttackBehavior::setBackPedalingInput( bbool backPedaling )
    {
        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(BackPedaling,1568802144), backPedaling);
    }

    bbool Ray_AIGroundAttackBehavior::checkStuck( f32 _dt )
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

    bbool Ray_AIGroundAttackBehavior::checkGiveUpRange()
    {
        if ( m_targetDistance > getTemplate()->getGiveUpDistance() )
        {
            startGiveUp();
            return btrue;
        }

        return bfalse;
    }

    bbool Ray_AIGroundAttackBehavior::checkBlocked()
    {
        if ( ( m_targetIsLeft && m_blocked == BLOCKED_LEFT ) ||
            ( !m_targetIsLeft && m_blocked == BLOCKED_RIGHT ) )
        {
            // I'm blocked and cannot go where I want to go
            startGiveUp();  // TODO: timer here?
            return btrue;
        }

        return bfalse;
    }


    bbool Ray_AIGroundAttackBehavior::checkHole( PolyLine* poly )
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
            if ( holeDepth < getTemplate()->getHoleJumpDepth() )
            {
                startJump();
            }
            else
            {
                setBlocked();
                startIdle();
            }

            return btrue;
        }

        return bfalse;
    }
    bbool Ray_AIGroundAttackBehavior::checkSlope(PolyLine * poly)
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
    bbool Ray_AIGroundAttackBehavior::isValidSlope( f32 _slopeAngle ) const
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

    bbool Ray_AIGroundAttackBehavior::canBeInterrupted() const
    {
        return !m_currentAction->IsClassCRC(ITF_GET_STRINGID_CRC(Ray_AIPerformHitAction,1258927796));
    }

    void Ray_AIGroundAttackBehavior::HACK_setDarktoonifyAction()
    {
        if (m_attacks.size() > 0)
        {
            setAction(m_attacks[m_attacks.size() - 1]);
        }
    }

    bbool Ray_AIGroundAttackBehavior::checkBlockingObstacle()
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

    bbool Ray_AIGroundAttackBehavior::checkBlockingPolyline( PolyLine* _poly, const PolyLineEdge* _edge, u32 _edgeIndex, const Vec2d& _contactPoint )
    {
        if (!_poly || !_edge)
        {
            return bfalse;
        }

        bbool left = _edge->m_vector.getPerpendicular().dot(Vec2d::Right) > 0;  // if the edge normal goes right, that means we're hitting it from the left
        f32 edgeHeight;
        AIUtils::hasWallInFront(_contactPoint, left, _poly, _edgeIndex, 0.5f, 5.f, edgeHeight, bfalse);
//            AIUtils::drawEdge(_edge, _poly->getDepth(), Color::blue(), 1.f);
  
        if (0 < edgeHeight && edgeHeight < getTemplate()->getObstacleJumpHeight() && m_jump)
        {
            startJump();
            return btrue;
        }
        else
        {
            setBlocked();
            startIdle();
            return btrue;
        }

    }

    void Ray_AIGroundAttackBehavior::onFinishedGiveUp()
    {
        performUTurnImmediate();
        m_aiComponent->onBehaviorFinished();
    }

    void Ray_AIGroundAttackBehavior::onFinishedAttack()
    {
        m_currentAttack = 0;
        startIdle();
    }

    void Ray_AIGroundAttackBehavior::onFinishedDetect()
    {
        startIdle();
    }

    void Ray_AIGroundAttackBehavior::onFinishedFall()
    {
        startIdle();
    }

    void Ray_AIGroundAttackBehavior::onFinishedJump()
    {
        startWalk();
    }

    void Ray_AIGroundAttackBehavior::onActionFinishedUturn()
    {
        Super::onActionFinishedUturn();
        startIdle();
        //m_move->setWalkLeft(m_actor->isFlipped());
        //startWalk();
    }
    void Ray_AIGroundAttackBehavior::onFinishedOutOfRange()
    {
        if (!checkOutOfRange())
        {
            startIdle();
        }
    }

    bbool Ray_AIGroundAttackBehavior::checkOutOfRange()
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

    void Ray_AIGroundAttackBehavior::onFinishedBackDetect()
    {
        m_wasLookingLeft = m_targetIsLeft;
        m_actor->setFlipped(!m_actor->isFlipped());
        startIdle();
    }

#ifdef ITF_SUPPORT_EDITOR
    void Ray_AIGroundAttackBehavior::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        Super::drawEdit(_drawInterface, _flags);

        if (_flags != ActorComponent::DrawEditFlag_All)
            return;

        Vec3d pos = m_actor->getPos();

        DebugDraw::circle(pos,getTemplate()->getGiveUpDistance(),Color::red(),0.f,"GiveUpDistance");
        DebugDraw::circle(pos,getTemplate()->getSlopeDetectionRange(),Color::green(),0.f,"SlopeDetectionRange");
        DebugDraw::circle(pos,getTemplate()->getOutOfRangeDistance(),Color::blue(),0.f,"OutOfRangeDistance");
        
        Vec2d pos2d = m_actor->get2DPos();
        bbool flip = m_actor->isFlipped();

        if (m_pendingAttack != U32_INVALID)
        {
            AABB aabb(getTemplate()->getAttacks()[m_pendingAttack].m_range);
            if (flip)
                aabb.FlipHorizontaly(bfalse);
            String8 text;
            m_attacks[m_pendingAttack]->getDebugText(text);
            DebugDraw::AABB(pos2d,m_actor->getDepth(),0.0f,aabb,Color::yellow(),0,1.0f,text);
        }
        else if (m_currentAttack != U32_INVALID)
        {
            AABB aabb(getTemplate()->getAttacks()[m_currentAttack].m_range);
            if (flip)
                aabb.FlipHorizontaly(bfalse);
            String8 text;
           m_attacks[m_currentAttack]->getDebugText(text);
            DebugDraw::AABB(pos2d,m_actor->getDepth(),0.0f,aabb,Color::yellow(),0,1.0f,text);
        }
        else
        {
            u32 i = 0;
            for ( Ray_AIGroundAttackBehavior_Template::AttackDataList::const_iterator it = getTemplate()->getAttacks().begin(); it != getTemplate()->getAttacks().end(); ++it)
            {
                AABB aabb(it->m_range);
                if (flip)
                    aabb.FlipHorizontaly(bfalse);
                String8 text;
                m_attacks[i]->getDebugText(text);
                DebugDraw::AABB(pos2d,m_actor->getDepth(),0.0f,aabb,Color::yellow(),0,1.0f,text);
                ++i;
            }
        }
    }
#endif // ITF_SUPPORT_EDITOR
}
