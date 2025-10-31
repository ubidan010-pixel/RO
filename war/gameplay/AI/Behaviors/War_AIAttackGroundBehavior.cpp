#include "precompiled_gameplay_war.h"

#ifndef _ITF_WAR_AIATTACKGROUNDBEHAVIOR_H_
#include "war/gameplay/ai/Behaviors/War_AIAttackGroundBehavior.h"
#endif //_ITF_WAR_AIATTACKGROUNDBEHAVIOR_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_WAR_AITARGETCOMPONENT_H_
#include "war/gameplay/Components/Misc/War_AITargetComponent.h"
#endif //_ITF_WAR_AITARGETCOMPONENT_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(War_AIAttackGroundBehavior)

    BEGIN_SERIALIZATION_CHILD(War_AIAttackGroundBehavior)
        // ACTIONS
        SERIALIZE_ACTION("surprise", m_surpriseAction, ESerialize_Template);
        SERIALIZE_ACTION("walk", m_walkAction, ESerialize_Template);
        SERIALIZE_ACTION("hitWait", m_hitWaitAction, ESerialize_Template);
        SERIALIZE_ACTION("hit", m_hitAction, ESerialize_Template);
        SERIALIZE_ACTION("reload", m_reloadAction, ESerialize_Template);

        // PARAMS
        SERIALIZE_MEMBER("attackRadius", m_attackRadius, ESerialize_Template);
        SERIALIZE_MEMBER("maxConsecutiveHits", m_maxConsecutiveHits, ESerialize_Template);

        SERIALIZE_FUNCTION(onSerialize, ESerialize_Template);
    END_SERIALIZATION()

    BEGIN_VALIDATE_BEHAVIOR(War_AIAttackGroundBehavior)
        VALIDATE_BEHAVIOR_PARAM("", m_physComponent, "War_AIAttackGroundBehavior requires a StickToPolylinePhysComponent");
    END_VALIDATE_BEHAVIOR()

    void War_AIAttackGroundBehavior::onSerialize()
    {
        f32 radius = Seeder::getSharedSeeder().GetFloat(m_attackRadius * 0.9f, m_attackRadius * 1.1f);
        m_sqrAttackRadius = radius * radius;
    }

    War_AIAttackGroundBehavior::War_AIAttackGroundBehavior()
        : Super()
        , m_physComponent(NULL)
        , m_walkLeft(bfalse)
        , m_walkLeft_dest(bfalse)
        , m_attackRadius(1.f)
        , m_sqrAttackRadius(-1)
        , m_maxConsecutiveHits(U32_INVALID) // default: infinite
        , m_consecutiveHitCount(0)
    {
        m_destroyRegisteredActions = bfalse;
    }

    War_AIAttackGroundBehavior::~War_AIAttackGroundBehavior()
    {
    }

    void War_AIAttackGroundBehavior::onActorLoaded()
    {
        Super::onActorLoaded();

        m_actor->registerEvent(War_HitStim::GetClassNameStatic(), m_aiComponent);

        m_physComponent = m_actor->GetComponent<StickToPolylinePhysComponent>();    // mandatory, validated
    }


    void War_AIAttackGroundBehavior::onActivate()
    {
        setAction(&m_surpriseAction);
        m_walkAction.setWalkLeft(m_walkLeft);
    }

    void War_AIAttackGroundBehavior::update( f32 _delta  )
    {
        Super::update(_delta);

        if ( m_currentAction == &m_walkAction )
        {
            checkEnemyInRange();        
        }

        if ( m_currentAction == &m_walkAction )
        {
            m_walkLeft = m_walkLeft_dest;
            updateWalk(_delta);
        }
    }

    void War_AIAttackGroundBehavior::onActionFinished()
    {
        if (m_currentAction == &m_surpriseAction)
        {
            setAction(&m_walkAction);
        }
        else if (m_currentAction == &m_hitWaitAction)
        {
            setAction(&m_hitAction);
        }
        else if (m_currentAction == &m_hitAction)
        {
            if (++m_consecutiveHitCount == m_maxConsecutiveHits)
            {
                m_consecutiveHitCount = 0;
                setAction(&m_reloadAction);
            }
            else
            {
                setAction(&m_walkAction);
            }
        }
        else
        {
            setAction(&m_hitWaitAction);               
        }
    }

    void War_AIAttackGroundBehavior::updateWalk( f32 _delta )
    {
        if ( !m_physComponent->getStickedEdge() )
            return;

        m_walkAction.setWalkLeft(m_walkLeft);

        f32 speed = fabs(m_physComponent->getSpeed().m_x);

        f32 remainingDistance = AIUtils::getRemainingDistanceOnEdge(
            m_actor->get2DPos(),
            m_aiComponent->getCurrentGravityDir(),
            m_physComponent->getStickedPolyline(),
            m_physComponent->getStickedEdgeIndex(),
            m_walkLeft, 0.f, speed);

        // this is bypassed by checkEnemyInRange(), but it could happen if there's a hole between AI and player...?
        if ( remainingDistance < speed )
        {
            setAction(&m_hitWaitAction);
        }
    }

    void War_AIAttackGroundBehavior::checkEnemyInRange()
    {
        Vec2d myPos = m_actor->get2DPos();

        War_AITargetComponent* closestTarget = NULL;
        f32 minDistSq = m_sqrAttackRadius;

        for ( u32 i = 0; i < GAMEMANAGER->getMaxPlayerCount(); i++ )
        {
            Actor* player = GAMEMANAGER->getActivePlayer(i);
            if ( !player )
                continue;

            if ( DepthRange(player->getDepth()) != DepthRange(m_actor->getDepth()) )
                continue;


            War_AITargetComponent* target = player->GetComponent<War_AITargetComponent>();
            if (!target)
                continue;

            //if (!target->canGetAttacked())
            //    continue;


            Vec2d pos = player->get2DPos();
            f32 distSq = ( pos - myPos ).sqrnorm();

            if ( distSq < minDistSq )
            {
                if ( m_walkLeft ? ( pos.m_x < myPos.m_x ) : ( pos.m_x > myPos.m_x ) )
                {
                    minDistSq = distSq;
                    closestTarget = target;
                }
            }
        }

        if ( closestTarget )
        {
            m_hitWaitAction.setTarget(closestTarget);
            m_hitAction.setTarget(closestTarget);
            setAction(&m_hitWaitAction);
        }
    }

}