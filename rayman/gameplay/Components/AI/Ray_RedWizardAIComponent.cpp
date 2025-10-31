#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_REDWIZARDAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_RedWizardAIComponent.h"
#endif //_ITF_RAY_REDWIZARDAICOMPONENT_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/AI/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef _ITF_RAY_AIGROUNDFOLLOWBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ground/Ray_AIGroundFollowBehavior.h"
#endif //_ITF_RAY_AIGROUNDFOLLOWBEHAVIOR_H_


#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_RedWizardAIComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_RedWizardAIComponent)
    END_SERIALIZATION()

    Ray_RedWizardAIComponent::Ray_RedWizardAIComponent()
        : Super()
        , m_followBehavior(NULL)
    {
    }

    Ray_RedWizardAIComponent::~Ray_RedWizardAIComponent()
    {
    }

    void Ray_RedWizardAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {

        m_followBehavior = static_cast<Ray_AIGroundFollowBehavior *>(createAiBehavior(getTemplate()->getFollowBehavior()));
        const SafeArray<class TemplateAIBehavior*> & behaviors = getTemplate()->getEventTriggeredBehaviors();
        m_eventTriggeredBehaviors.reserve(behaviors.size());
        for (u32 i = 0; i < behaviors.size(); i++)
        {
            m_eventTriggeredBehaviors.push_back(createAiBehavior(behaviors[i]));
        }

        Super::onActorLoaded(_hotReload);
    }

    void Ray_RedWizardAIComponent::updateBehaviors( f32 _dt )
    {
        if ( m_currentBehavior == m_followBehavior )
        {
            // nothing to do ATM...
            Super::updateBehaviors(_dt);
        }
        else
        {
            for (u32 i = 0; i < m_eventTriggeredBehaviors.size(); i++)
            {
                if ( m_currentBehavior == m_eventTriggeredBehaviors[i] )
                {
                    // nothing to do ATM...
                    return;
                }
            }

            Super::updateBehaviors(_dt);

            // priority is given to attacking over following,
            // so if we're still roaming now that means we haven't found an enemy
            if ( m_currentBehavior == m_roamBehavior )
            {
                updateRoam(_dt);
            }
        }
    }

    void Ray_RedWizardAIComponent::onBehaviorFinished()
    {
        if ( m_currentBehavior == m_followBehavior )
        {
            onFinishedFollow();
        }
        else
        {
            for (u32 i = 0; i < m_eventTriggeredBehaviors.size(); i++)
            {
                if ( m_currentBehavior == m_eventTriggeredBehaviors[i] )
                {
                    startRoam();
                    return;
                }
            }

            Super::onBehaviorFinished();
        }
    }


    void Ray_RedWizardAIComponent::updateRoam( f32 _dt )
    {
        if (m_followBehavior)
        {
            ActorList players;
            AIUtils::getPlayers(m_actor->getDepth(), players);

            Actor* closestPlayer = AIUtils::getClosestActor( players,
                m_actor, m_actor->isFlipped(),
                getTemplate()->getFollowDetectDistance() );
            if (closestPlayer)
            {
                startFollow(closestPlayer->getRef());
            }
        }
    }


    void Ray_RedWizardAIComponent::startFollow( ObjectRef _target )
    {
        setStance(Stance_Normal);
        m_followBehavior->setTarget(_target);
        setBehavior(m_followBehavior);
    }

    void Ray_RedWizardAIComponent::onFinishedFollow()
    {
        startRoam();
    }


    void Ray_RedWizardAIComponent::onEvent( Event* _event )
    {
        Super::onEvent(_event);

        if (m_currentBehavior == m_darktoonedBehavior)
            return;

        for (u32 i = 0; i < m_eventTriggeredBehaviors.size(); i++)
        {
            AIBehavior* behavior = m_eventTriggeredBehaviors[i];
            if ( behavior->isActivatedByEvent(_event) )
            {
                behavior->onEvent(_event);
                setBehavior(behavior);
                break;
            }
        }
    }

    //-------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(Ray_RedWizardAIComponent_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_RedWizardAIComponent_Template)
        SERIALIZE_MEMBER("followDetectDistance", m_followDetectDistance);
        SERIALIZE_OBJECT_WITH_FACTORY("followBehavior",m_followBehavior,ACTORSMANAGER->getAIBehaviorsFactory());
        SERIALIZE_CONTAINER_WITH_FACTORY("eventTriggeredBehaviors",m_eventTriggeredBehaviors,ACTORSMANAGER->getAIBehaviorsFactory());
    END_SERIALIZATION()

    Ray_RedWizardAIComponent_Template::Ray_RedWizardAIComponent_Template()
    : m_followDetectDistance(5.f)
    , m_followBehavior(NULL)
    {
    }

    Ray_RedWizardAIComponent_Template::~Ray_RedWizardAIComponent_Template()
    {
        SF_DEL(m_followBehavior);
        for (u32 i = 0;i<m_eventTriggeredBehaviors.size(); ++i)
        {
            SF_DEL(m_eventTriggeredBehaviors[i]);
        }
    }
};