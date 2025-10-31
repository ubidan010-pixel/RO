#include "precompiled_gameplay.h"

#ifndef _ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_
#include "gameplay/Components/common/StickToPolylinePhysComponent.h"
#endif //_ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_

#ifndef _ITF_AIROAMINGBEHAVIOR_H_
#include "gameplay/AI/Behaviors/AIRoamingBehavior.h"
#endif //_ITF_AIROAMINGBEHAVIOR_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_


#ifndef _ITF_AIIDLEACTION_H_
#include "gameplay/ai/Actions/AIIdleAction.h"
#endif //_ITF_AIIDLEACTION_H_

#ifndef _ITF_AIWALKINDIRACTION_H_
#include "gameplay/ai/Actions/AIWalkInDirAction.h"
#endif //_ITF_AIWALKINDIRACTION_H_


#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(AIRoamingBehavior_Template)

    BEGIN_SERIALIZATION_CHILD(AIRoamingBehavior_Template)
        SERIALIZE_OBJECT_WITH_FACTORY("idle", m_idle, ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("walk", m_walk, ACTORSMANAGER->getAIActionsFactory());
    END_SERIALIZATION()

    //BEGIN_VALIDATE_BEHAVIOR(AIRoamingBehavior_Template)
    //    VALIDATE_BEHAVIOR_PARAM("m_idle", m_idle, "this action is mandatory");
    //    VALIDATE_BEHAVIOR_PARAM("m_walk", m_walk, "this action is mandatory");
    //END_VALIDATE_BEHAVIOR()

        AIRoamingBehavior_Template::AIRoamingBehavior_Template()
        : Super()
        , m_idle(NULL)
        , m_walk(NULL)
    {

    }

    AIRoamingBehavior_Template::~AIRoamingBehavior_Template()
    {
        SF_DEL(m_idle);
        SF_DEL(m_walk);
    }
    IMPLEMENT_OBJECT_RTTI(AIRoamingBehavior)
    AIRoamingBehavior::AIRoamingBehavior()
        : Super()
        , m_idle(NULL)
        , m_walk(NULL)
        , m_physComponent(NULL)
    {
    }

    AIRoamingBehavior::~AIRoamingBehavior()
    {
    }

    void AIRoamingBehavior::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        m_idle = static_cast<AIIdleAction*>(createAiAction(getTemplate()->getIdle()));
        m_walk = static_cast<AIWalkInDirAction*>(createAiAction(getTemplate()->getWalk()));
        m_physComponent = m_actor->GetComponent<StickToPolylinePhysComponent>();    // mandatory

        //ALWAYS CALL THIS LAST
        Super::onActorLoaded(_hotReload);
    }

    void AIRoamingBehavior::onActivate()
    {
        setAction(m_idle);
    }

    void AIRoamingBehavior::onActionFinished()
    {
        if ( m_currentAction == m_idle )
        {
            m_walk->setWalkLeft(!m_walk->isWalkingLeft());
            setAction(m_walk);
        }
        else
        {
            setAction(m_idle);
        }
    }

    void AIRoamingBehavior::update( f32 _dt  )
    {
        Super::update(_dt);

        if ( m_currentAction == m_walk )
        {
            updateWalk(_dt);
        }
    }

    void AIRoamingBehavior::updateWalk( f32 _dt )
    {
        PolyLine* poly = m_physComponent->getStickedPolyline();

        if ( !poly )
        {
            return;
        }

        f32 speed = f32_Abs(m_physComponent->getSpeed().m_x);

        f32 remainingDistance = AIUtils::getRemainingDistanceOnEdge(m_actor->get2DPos(),
                                                                    m_aiComponent->getCurrentGravityDir(),
                                                                    poly,
                                                                    m_physComponent->getStickedEdgeIndex(),
                                                                    m_walk->isWalkingLeft(), 0.f, speed);

        if ( remainingDistance < speed )
        {
            setAction(m_idle);
        }
    }

}