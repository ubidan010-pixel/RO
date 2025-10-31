#include "precompiled_gameplay_war.h"

#ifndef _ITF_WAR_AIROAMINGBEHAVIOR_H_
#include "war/gameplay/ai/Behaviors/War_AIRoamingBehavior.h"
#endif //_ITF_WAR_AIROAMINGBEHAVIOR_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(War_AIRoamingBehavior)

    BEGIN_SERIALIZATION_CHILD(War_AIRoamingBehavior)
        SERIALIZE_ACTION("idle", m_idle, ESerialize_Template);
        SERIALIZE_ACTION("walkInDir", m_walkInDir, ESerialize_Template);
        SERIALIZE_MEMBER("minTimeToWalk",m_minTimeToWalk,ESerialize_PropertyEdit|ESerialize_Template);
        SERIALIZE_MEMBER("maxTimeToWalk",m_maxTimeToWalk,ESerialize_PropertyEdit|ESerialize_Template);
        SERIALIZE_MEMBER("minTimeToIdle",m_minTimeToIdle,ESerialize_PropertyEdit|ESerialize_Template);
        SERIALIZE_MEMBER("maxTimeToIdle",m_maxTimeToIdle,ESerialize_PropertyEdit|ESerialize_Template);
    END_SERIALIZATION()

    BEGIN_VALIDATE_BEHAVIOR(War_AIRoamingBehavior)
        VALIDATE_BEHAVIOR_PARAM("", m_physComponent, "War_AIRoamingBehavior requires a StickToPolylinePhysComponent");
    END_VALIDATE_BEHAVIOR()

    War_AIRoamingBehavior::War_AIRoamingBehavior()
        : Super()
        , m_minTimeToWalk(1.f)
        , m_maxTimeToWalk(2.f)
        , m_minTimeToIdle(3.f)
        , m_maxTimeToIdle(4.f)
        , m_walkForce(0.f)
        , m_timeCounter(0.f)
        , m_walkLeft(bfalse)
        , m_physComponent(NULL)
    {
        m_destroyRegisteredActions = bfalse;
    }

    War_AIRoamingBehavior::~War_AIRoamingBehavior()
    {
    }

    void War_AIRoamingBehavior::onActorLoaded()
    {
        Super::onActorLoaded();

        m_physComponent = m_actor->GetComponent<StickToPolylinePhysComponent>();    // mandatory, validated

        m_actor->registerEvent(War_HitStim::GetClassNameStatic(), m_aiComponent);
    }

    void War_AIRoamingBehavior::update( f32 _delta  )
    {
        Super::update(_delta);

        m_timeCounter -= _delta;

        if ( m_timeCounter <= 0.f )
        {
            toggleWalkIdle();
        }
        else
        {
            if ( m_currentAction == &m_walkInDir )
            {
                updateWalk(_delta);
            }
        }
    }

    void War_AIRoamingBehavior::updateWalk( f32 _delta )
    {
        if ( !m_physComponent->getStickedEdge() )
        {
            return;
        }

        f32 speed = fabs(m_physComponent->getSpeed().m_x);

        f32 remainingDistance = AIUtils::getRemainingDistanceOnEdge(m_actor->get2DPos(),
                                                                    m_aiComponent->getCurrentGravityDir(),
                                                                    m_physComponent->getStickedPolyline(),
                                                                    m_physComponent->getStickedEdgeIndex(),
                                                                    m_walkLeft,0.f,speed);


        if ( remainingDistance < speed )
        {
            toggleWalkIdle();
        }
    }

    void War_AIRoamingBehavior::toggleWalkIdle()
    {
        if ( m_currentAction == &m_walkInDir )
        {
            setAction(&m_idle);
            setIdleTime();
            m_walkLeft = !m_walkLeft;
        }
        else
        {
            m_walkInDir.setWalkLeft(m_walkLeft);
            setAction(&m_walkInDir);
            setWalkTime();        
        }
    }

    void War_AIRoamingBehavior::onActivate()
    {
        setAction(&m_idle);
        setIdleTime();
    }

    void War_AIRoamingBehavior::onActionFinished()
    {
        setAction(&m_idle);
    }

    void War_AIRoamingBehavior::setIdleTime()
    {
        m_timeCounter = m_minTimeToIdle + ( ( m_maxTimeToIdle - m_minTimeToIdle ) * Seeder::getSharedSeeder().GetFloat() );
    }

    void War_AIRoamingBehavior::setWalkTime()
    {
        m_timeCounter = m_minTimeToWalk + ( ( m_maxTimeToWalk - m_minTimeToWalk ) * Seeder::getSharedSeeder().GetFloat() );
    }

}

