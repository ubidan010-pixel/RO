#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AIBOUNCEROAMINGBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/Ray_AIBounceRoamingBehavior.h"
#endif //_ITF_RAY_AIBOUNCEROAMINGBEHAVIOR_H_

#ifndef _ITF_AIJUMPACTION_H_
#include "gameplay/ai/Actions/AIJumpAction.h"
#endif //_ITF_AIJUMPACTION_H_

#ifndef _ITF_AIJUMPINDIRACTION_H_
#include "gameplay/ai/Actions/AIJumpInDirAction.h"
#endif //_ITF_AIJUMPINDIRACTION_H_

#ifndef _ITF_AIIDLEACTION_H_
#include "gameplay/ai/Actions/AIIdleAction.h"
#endif //_ITF_AIIDLEACTION_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_


#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_AIBounceRoamingBehavior_Template)

    BEGIN_SERIALIZATION_CHILD(Ray_AIBounceRoamingBehavior_Template)

        SERIALIZE_OBJECT_WITH_FACTORY("bounceMove", m_bounceMove, ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("bounceIdle", m_bounceIdle, ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("bounceJump", m_bounceJump, ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_MEMBER("minTimeToWalk",m_minTimeToMove);
        SERIALIZE_MEMBER("maxTimeToWalk",m_maxTimeToMove);
        SERIALIZE_MEMBER("minTimeToIdle",m_minTimeToIdle);
        SERIALIZE_MEMBER("maxTimeToIdle",m_maxTimeToIdle);
        SERIALIZE_MEMBER("obstacleDetectionDistance",m_obstacleDetectionDistance);
        SERIALIZE_MEMBER("wallJumpHeight",m_wallJumpHeight);
    END_SERIALIZATION()
        Ray_AIBounceRoamingBehavior_Template::Ray_AIBounceRoamingBehavior_Template()
        : m_bounceMove(NULL)
        , m_bounceIdle(NULL)
        , m_bounceJump(NULL)
        , m_minTimeToMove(1.f)
        , m_maxTimeToMove(2.f)
        , m_minTimeToIdle(3.f)
        , m_maxTimeToIdle(4.f)
        , m_obstacleDetectionDistance(2.0f)
        , m_wallJumpHeight(3.0f)
    {

    }

    Ray_AIBounceRoamingBehavior_Template::~Ray_AIBounceRoamingBehavior_Template()
    {
        SF_DEL(m_bounceMove);
        SF_DEL(m_bounceIdle);
        SF_DEL(m_bounceJump);

    }
    BEGIN_VALIDATE_BEHAVIOR(Ray_AIBounceRoamingBehavior)
        VALIDATE_BEHAVIOR_PARAM("bounceMove", m_bounceMove, "this action is mandatory");
        VALIDATE_BEHAVIOR_PARAM("bounceIdle", m_bounceIdle, "this action is mandatory");
        VALIDATE_BEHAVIOR_PARAM("bounceJump", m_bounceJump, "this action is mandatory");
    END_VALIDATE_BEHAVIOR()

    IMPLEMENT_OBJECT_RTTI(Ray_AIBounceRoamingBehavior)
    Ray_AIBounceRoamingBehavior::Ray_AIBounceRoamingBehavior()
        : m_bounceMove(NULL)
        , m_bounceIdle(NULL)
        , m_bounceJump(NULL)
        , m_timeCounter(0.f)
        , m_moveLeft(bfalse)
        , m_physComponent(NULL)
        , m_toggle(bfalse)
        , m_previousSpeedLossMultiplier(1.0f)
    {
    }

    Ray_AIBounceRoamingBehavior::~Ray_AIBounceRoamingBehavior()
    {
    }

    void Ray_AIBounceRoamingBehavior::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        m_bounceMove = static_cast<AIJumpInDirAction*>(createAiAction(getTemplate()->getBounceMove()));
        m_bounceIdle = static_cast<AIJumpInDirAction*>(createAiAction(getTemplate()->getBounceIdle()));
        m_bounceJump = static_cast<AIJumpAction*>(createAiAction(getTemplate()->getBounceJump()));

        m_physComponent = m_actor->GetComponent<StickToPolylinePhysComponent>();

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(PunchStim,200533519), m_aiComponent);

        ITF_ASSERT(m_physComponent);

        //ALWAYS CALL THIS LAST
        Super::onActorLoaded(_hotReload);
    }

    void Ray_AIBounceRoamingBehavior::update( f32 _delta  )
    {
        Super::update(_delta);

        m_timeCounter -= _delta;

        if ( m_timeCounter <= 0.f )
        {
            m_toggle = btrue;
        }
    }

    void Ray_AIBounceRoamingBehavior::onActionFinished()
    {
        if ( m_currentAction )
        {
            if (m_toggle)
                toggleMoveIdle();
            else
            {
                AIAction * previousAction = m_currentAction;
                if (m_currentAction == m_bounceIdle)
                {
                    updateIdle();
                }
                else if (m_currentAction == m_bounceMove || m_currentAction == m_bounceJump)
                {
                    updateMove();
                }

                if (m_currentAction == previousAction)
                {
                    m_currentAction->onDeactivate();
                    m_aiComponent->computeRandomAnimNumber();
                    m_currentAction->onActivate();
                }
            }
        }
    }

    void Ray_AIBounceRoamingBehavior::updateMove()
    {
        f32 speed = f32_Abs(m_physComponent->getSpeed().m_x);

        f32 remainingDistance = AIUtils::getRemainingDistanceOnEdge(m_actor->get2DPos(),
            m_aiComponent->getCurrentGravityDir(),
            m_physComponent->getStickedPolyline(),
            m_physComponent->getStickedEdgeIndex(),
            m_moveLeft,0.f,speed);


        if ( remainingDistance < speed )
        {
            toggleMoveIdle();
        }

        f32 wallHeight;

        if ( m_physComponent->getStickedPolyline() && 
            AIUtils::hasWallInFront(m_actor, m_actor->isFlipped(),
            m_physComponent->getStickedPolyline(),
            m_physComponent->getStickedEdgeIndex(),
            getTemplate()->getObstacleDetectionDistance(),
            m_physComponent->getRadius(),
            m_aiComponent->getCurrentGravityDir(),
            wallHeight) )
        {
            if ( getTemplate()->getWallJumpHeight() >= wallHeight )
            {
                setAction(m_bounceJump);
            }
            else
            {
                toggleMoveIdle();
            }

        }
        else
            setAction(m_bounceMove);
    }

    void Ray_AIBounceRoamingBehavior::updateIdle()
    {
    }

    void Ray_AIBounceRoamingBehavior::toggleMoveIdle()
    {
        m_toggle = bfalse;
        if ( m_currentAction == m_bounceMove )
        {
            startIdle();
        }
        else
        {
            startMove();
        }
    }

    void Ray_AIBounceRoamingBehavior::startIdle()
    {
        m_physComponent->setSpeed(Vec2d::Zero);
        setAction(m_bounceIdle);
        setIdleTime();
        m_moveLeft = !m_moveLeft;

    }

    void Ray_AIBounceRoamingBehavior::startMove()
    {

        m_physComponent->setSpeed(Vec2d::Zero);
        m_actor->setFlipped(m_moveLeft);
        setAction(m_bounceMove);
        setMoveTime();        
    }

    void Ray_AIBounceRoamingBehavior::onActivate()
    {
        startIdle();
        setIdleTime();
        m_previousSpeedLossMultiplier = m_physComponent->getSpeedLossMultiplier();
        m_physComponent->setSpeedLossMultiplier(0.0f);
        
    }

    void Ray_AIBounceRoamingBehavior::onDeactivate()
    {
        m_physComponent->setSpeedLossMultiplier(m_previousSpeedLossMultiplier);
    }

   void Ray_AIBounceRoamingBehavior::setIdleTime()
    {
        m_timeCounter = getTemplate()->getMinTimeToIdle() + ( ( getTemplate()->getMaxTimeToIdle()- getTemplate()->getMinTimeToIdle() ) * Seeder::getSharedSeeder().GetFloat() );
    }

    void Ray_AIBounceRoamingBehavior::setMoveTime()
    {
        m_timeCounter = getTemplate()->getMinTimeToMove() + ( ( getTemplate()->getMaxTimeToMove() - getTemplate()->getMinTimeToMove() ) * Seeder::getSharedSeeder().GetFloat() );
    }

}

