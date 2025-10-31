#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AIGROUNDRUNAWAYBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ground/Ray_AIGroundRunAwayBehavior.h"
#endif //_ITF_RAY_AIGROUNDRUNAWAYBEHAVIOR_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_AIJUMPACTION_H_
#include "gameplay/ai/Actions/AIJumpAction.h"
#endif //_ITF_AIJUMPACTION_H_

#ifndef _ITF_AIFALLACTION_H_
#include "gameplay/ai/Actions/AIFallAction.h"
#endif //_ITF_AIFALLACTION_H_


#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_AIGroundRunAwayBehavior_Template)

    BEGIN_SERIALIZATION_CHILD(Ray_AIGroundRunAwayBehavior_Template)
        // ACTIONS
        SERIALIZE_OBJECT_WITH_FACTORY("jumpOverObstacle", m_jumpOverObstacle, ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("jumpOverVoid", m_jumpOverVoid, ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("fall", m_fall, ACTORSMANAGER->getAIActionsFactory());

        SERIALIZE_MEMBER("enemyJumpDetectDistance", m_jumpOverEnemyDetection);
        SERIALIZE_MEMBER("voidJumpDetectDistance", m_jumpOverVoidDetection);
    END_SERIALIZATION()

    Ray_AIGroundRunAwayBehavior_Template::Ray_AIGroundRunAwayBehavior_Template()
    : m_jumpOverObstacle(NULL)
    , m_jumpOverVoid(NULL)
    , m_fall(NULL)
    , m_jumpOverEnemyDetection(1.5f)
    , m_jumpOverVoidDetection(0.5f)
    {
    }

    Ray_AIGroundRunAwayBehavior_Template::~Ray_AIGroundRunAwayBehavior_Template()
    {
        SF_DEL(m_jumpOverObstacle);
        SF_DEL(m_jumpOverVoid);
        SF_DEL(m_fall);
    }
    IMPLEMENT_OBJECT_RTTI(Ray_AIGroundRunAwayBehavior)

    BEGIN_VALIDATE_BEHAVIOR(Ray_AIGroundRunAwayBehavior)
        VALIDATE_BEHAVIOR_PARAM("jumpOverObstacle", m_jumpOverObstacle, "this action is mandatory");
        VALIDATE_BEHAVIOR_PARAM("jumpOverVoid", m_jumpOverVoid, "this action is mandatory");
        VALIDATE_BEHAVIOR_PARAM("fall", m_fall, "this action is mandatory");
    END_VALIDATE_BEHAVIOR()

    Ray_AIGroundRunAwayBehavior::Ray_AIGroundRunAwayBehavior()
        : Super()
        , m_jumpOverObstacle(NULL)
        , m_jumpOverVoid(NULL)
        , m_fall(NULL)
        , m_attacker(ITF_INVALID_OBJREF)
    {
    }

    Ray_AIGroundRunAwayBehavior::~Ray_AIGroundRunAwayBehavior()
    {
    }

    void Ray_AIGroundRunAwayBehavior::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        m_jumpOverObstacle = static_cast<AIJumpAction*>(createAiAction(getTemplate()->getJumpOverObstacle()));
        m_jumpOverVoid = static_cast<AIJumpAction*>(createAiAction(getTemplate()->getJumpOverVoid()));
        m_fall = static_cast<AIFallAction*>(createAiAction(getTemplate()->getFall()));

        Super::onActorLoaded(_hotReload);
    }

    void Ray_AIGroundRunAwayBehavior::update( f32 _delta  )
    {
        if ( m_currentAction == m_move || m_currentAction == m_idle )
        {
            updateRunning(_delta);
        }

        Super::update(_delta);
    }

    void Ray_AIGroundRunAwayBehavior::onActivate()
    {
        Super::onActivate();

        ITF_ASSERT(m_attacker!=ITF_INVALID_OBJREF);

        Actor* attacker = AIUtils::getActor(m_attacker);
        Vec2d myDir = AIUtils::getLookDir(m_actor,m_actor->isFlipped());
        Vec2d dirToAttacker = ( attacker->get2DPos() - m_actor->get2DPos() );

        dirToAttacker.normalize();

        if ( dirToAttacker.dot(myDir) < 0.f )
        {
            m_move->setWalkLeft(m_actor->isFlipped());
            setAction(m_move);
        }
        else
        {
            performUTurnImmediate();
        }

        m_stuckTimer = 0.f;
    }

    void Ray_AIGroundRunAwayBehavior::onDeactivate()
    {
        Super::onDeactivate();

        m_attacker = ITF_INVALID_OBJREF;
    }

    void Ray_AIGroundRunAwayBehavior::onActionFinished()
    {
        if ( m_currentAction == m_uturn )
        {
            m_move->setWalkLeft(m_actor->isFlipped());
            setAction(m_move);
            m_stuckTimer = 0.f;
        }
        else
        {
            setAction(m_move);
            m_stuckTimer = 0.f;
        }
    }

    void Ray_AIGroundRunAwayBehavior::updateRunning( f32 _dt )
    {
        PolyLine* poly = m_physComponent->getStickedPolyline();

        if ( !poly )
        {
            setAction(m_fall);
            return;
        }

        f32 remainingDistance = AIUtils::getRemainingDistanceOnEdge(m_actor->get2DPos(),
                                                                    m_aiComponent->getCurrentGravityDir(),
                                                                    poly,
                                                                    m_physComponent->getStickedEdgeIndex(),
                                                                    m_move->isWalkingLeft(),
                                                                    2.f,
                                                                    getTemplate()->getJumpOverVoidDetection());
        if ( remainingDistance < getTemplate()->getJumpOverVoidDetection() )
        {
            performJump(btrue);
            return;
        }

        if ( hasEnemyInFront() )
        {
            performJump(bfalse);
            return;
        }

        f32 wallHeight;

        if ( AIUtils::hasWallInFront(m_actor, m_move->isWalkingLeft(),
                                     m_physComponent->getStickedPolyline(),
                                     m_physComponent->getStickedEdgeIndex(),
                                     getTemplate()->getObstacleDetectionRange(),
                                     m_physComponent->getRadius(),
                                     m_aiComponent->getCurrentGravityDir(),
                                     wallHeight) )
        {
            if ( wallHeight < getTemplate()->getObstacleJumpHeight() )
            {
                performJump(bfalse);
            }
            else
            {
                performUTurn(_dt);
            }

            return;
        }

        if ( f32_Abs(m_physComponent->getWorldSpeedAverageX()) < 0.1f )
        {
            m_stuckTimer += _dt;

            if ( m_stuckTimer >= getTemplate()->getStuckDelay() )
            {
                if ( m_currentAction == m_move )
                {
                    m_stuckTimer = 0.f;
                }

                performUTurn(_dt);
            }
            else if ( m_currentAction == m_move )
            {
                m_move->setWalkLeft(m_actor->isFlipped());
                setAction(m_move);
            }
        }
        else
        {
            m_stuckTimer = 0.f;
        }
    }

    void Ray_AIGroundRunAwayBehavior::performJump( bbool _jumpOverVoid )
    {
        if ( _jumpOverVoid )
        {
            setAction(m_jumpOverVoid);
        }
        else
        {
            setAction(m_jumpOverObstacle);
        }
    }

    bbool Ray_AIGroundRunAwayBehavior::hasEnemyInFront() const
    {
        ActorList enemies;
        AIUtils::getEnemies(m_aiComponent->getFaction(), m_actor->getDepth(), enemies);

        Vec2d myDir = AIUtils::getLookDir(m_actor,m_actor->isFlipped());

        for ( u32 i = 0; i < enemies.size(); i++ )
        {
            Actor* enemy = enemies[i];
            Vec2d dirToEnemy = enemy->get2DPos() - m_actor->get2DPos();
            f32 distSq = dirToEnemy.sqrnorm();

            if ( distSq > (getTemplate()->getJumpOverEnemyDetection()*getTemplate()->getJumpOverEnemyDetection()) )
            {
                continue;
            }

            dirToEnemy.normalize();

            if ( dirToEnemy.dot(myDir) <= 0.f )
            {
                continue;
            }

            return btrue;
        }

        return bfalse;
    }

}