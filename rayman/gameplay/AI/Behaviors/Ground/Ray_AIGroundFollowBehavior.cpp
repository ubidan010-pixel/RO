#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AIGROUNDFOLLOWBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ground/Ray_AIGroundFollowBehavior.h"
#endif //_ITF_RAY_AIGROUNDFOLLOWBEHAVIOR_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_RAY_AVOIDANCEMANAGER_H_
#include "rayman/gameplay/Managers/Ray_AvoidanceManager.h"
#endif //_ITF_RAY_AVOIDANCEMANAGER_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/Managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_RAY_AVOIDANCECOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_AvoidanceComponent.h"
#endif //_ITF_RAY_AVOIDANCECOMPONENT_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_AIGroundFollowBehavior_Template)

    BEGIN_SERIALIZATION_CHILD(Ray_AIGroundFollowBehavior_Template)
        SERIALIZE_OBJECT_WITH_FACTORY("detect", m_detect, ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("giveUp", m_giveUp, ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_MEMBER("giveUpDistance", m_giveUpDistance);
        SERIALIZE_MEMBER("minIdleTime", m_minIdleTime);
        SERIALIZE_MEMBER("minWalkTime", m_minWalkTime);
    END_SERIALIZATION()

    Ray_AIGroundFollowBehavior_Template::Ray_AIGroundFollowBehavior_Template()
    : m_detect(NULL)
    , m_giveUp(NULL)
    , m_giveUpDistance(10.f)
    , m_minIdleTime(1.f)
    , m_minWalkTime(1.f)
    {
    }

    Ray_AIGroundFollowBehavior_Template::~Ray_AIGroundFollowBehavior_Template()
    {
        SF_DEL(m_detect);
        SF_DEL(m_giveUp);
    }
    IMPLEMENT_OBJECT_RTTI(Ray_AIGroundFollowBehavior)

    BEGIN_VALIDATE_BEHAVIOR(Ray_AIGroundFollowBehavior)
        VALIDATE_BEHAVIOR_PARAM("", m_avoidanceComponent, "Ray_AIGroundFollowBehavior requires a Ray_AvoidanceComponent");
    END_VALIDATE_BEHAVIOR()

    Ray_AIGroundFollowBehavior::Ray_AIGroundFollowBehavior()
        : Super()
        , m_avoidanceComponent(NULL)
        , m_detect(NULL)
        , m_giveUp(NULL)
        , m_avoidanceTimer(0.f)
        , m_neededToMove(bfalse)
        , m_target(ITF_INVALID_OBJREF)
        , m_targetIsLeft(bfalse)
    {
    }

    Ray_AIGroundFollowBehavior::~Ray_AIGroundFollowBehavior()
    {
    }


    void Ray_AIGroundFollowBehavior::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        m_detect = createAiAction(getTemplate()->getDetect());
        m_giveUp = createAiAction(getTemplate()->getGiveUp());


        Super::onActorLoaded(_hotReload);

        m_avoidanceComponent = m_actor->GetComponent<Ray_AvoidanceComponent>(); // mandatory, validated
        m_tryReachPosition = btrue;
    }

    void Ray_AIGroundFollowBehavior::onActivate()
    {
        Super::onActivate();

        ITF_ASSERT(m_target != ITF_INVALID_OBJREF);

        m_neededToMove = bfalse;
        m_avoidanceTimer = 0;
        m_avoidanceComponent->setEnabled(btrue);

        if (m_target.getActor())
        {
            Vec2d vecToVictim = m_target.getActor()->get2DPos() - m_actor->get2DPos();
            m_targetIsLeft = vecToVictim.dot(Vec2d::Right) < 0;
        }
        startDetect();
    }

    void Ray_AIGroundFollowBehavior::onDeactivate()
    {
        m_target = ITF_INVALID_OBJREF;

        m_avoidanceComponent->setEnabled(bfalse);
    }

    void Ray_AIGroundFollowBehavior::startIdle()
    {
        if ( m_targetIsLeft != m_actor->isFlipped() )
        {
            performUTurnImmediate();
        }
        else
        {
            setAction(m_idle);
        }
    }

    void Ray_AIGroundFollowBehavior::updateIdle( f32 _dt )
    {
        if (checkTarget(_dt))
        {
            return;
        }

        Actor* target = AIUtils::getActor(m_target);
        Vec2d vecToTarget = target->get2DPos() - m_actor->get2DPos();
        m_targetIsLeft = vecToTarget.dot(Vec2d::Right) < 0;

        if ( m_targetIsLeft != m_actor->isFlipped() )
        {
            performUTurnImmediate();
        }
        else
        {

        }
        m_avoidanceTimer += _dt;

        avoidOtherActors();

        //Super::updateIdle(_dt);
    }

    bbool Ray_AIGroundFollowBehavior::updateMove( f32 _dt )
    {
        m_stuckTimer = 0.0f;
        if (checkTarget(_dt))
        {
            return bfalse;
        }

        if ( !Super::updateMove(_dt) )
        {
            return bfalse;
        }


        // none of the checks have interrupted the move
        Actor* target = AIUtils::getActor(m_target);
        Vec2d vecToTarget = target->get2DPos() - m_actor->get2DPos();
        m_targetIsLeft = vecToTarget.dot(Vec2d::Right) < 0;

        if ( m_targetIsLeft != m_actor->isFlipped() )
        {
            performUTurnImmediate();
        }

        m_avoidanceTimer += _dt;
        avoidOtherActors();


        return btrue;
    }

    void Ray_AIGroundFollowBehavior::onActionFinished()
    {
        if ( m_detect && m_currentAction == m_detect )
        {
            onFinishedDetect();
        }
        else if ( m_giveUp && m_currentAction == m_giveUp )
        {
            onFinishedGiveUp();
        }
        else
        {
            Super::onActionFinished();
        }
    }

    void Ray_AIGroundFollowBehavior::startDetect()
    {
        if (m_detect)
        {
            setAction(m_detect);
        }
        else
        {
            onFinishedDetect();
        }
    }

    void Ray_AIGroundFollowBehavior::startGiveUp()
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


    void Ray_AIGroundFollowBehavior::onFinishedDetect()
    {
        setAction(m_idle);
    }

    void Ray_AIGroundFollowBehavior::onFinishedGiveUp()
    {
        m_aiComponent->onBehaviorFinished();
    }


    // check if the target is still valid; return true if we interrupt the update, false otherwise (ie. we can do something else after)
    bbool Ray_AIGroundFollowBehavior::checkTarget( f32 _dt )
    {
        Actor* target = AIUtils::getActor(m_target);
        if (target)
        {
            // give up if target is too far
            Vec2d vecToTarget = target->get2DPos() - m_actor->get2DPos();
            if ( vecToTarget.sqrnorm() > ( getTemplate()->getGiveUpDistance() * getTemplate()->getGiveUpDistance() ) )
            {
                startGiveUp();
                return btrue;
            }

            return bfalse;

            //m_targetIsLeft = vecToTarget.dot(Vec2d::Right) < 0;
            //m_avoidanceTimer += _dt;
            //avoidOtherActors();

            //// HACK: normally we would allow to do something else here,
            //// but for now we'll just *always* follow
            //return btrue;
        }
        else
        {
            // target has disappeared: give up
            ITF_ASSERT_MSG(0, "target has disappeared!");
            startGiveUp();
            return btrue;
        }
    }


    void Ray_AIGroundFollowBehavior::avoidOtherActors()
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



        //f32 distance = f32_Abs(destination);

        //for (u32 i = m_moveRanges.size(); i > 0; i--)
        //{
        //    MoveRange& range = m_moveRanges[i - 1];
        //    if (distance < range.m_maxDistance)
        //    {
        //        m_move->setWalkForce(range.m_walkForce);
        //        m_currentMoveRangeIndex = i;
        //    }
        //}
    }
    void Ray_AIGroundFollowBehavior::updateAnimInputs()
    {
        Super::updateAnimInputs();

        bbool backPedaling = ( m_targetIsLeft != m_move->isWalkingLeft() );
        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(BackPedaling,1568802144), backPedaling);

    }

}