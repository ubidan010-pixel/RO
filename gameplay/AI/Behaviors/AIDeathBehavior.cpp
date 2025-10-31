#include "precompiled_gameplay.h"

#ifndef _ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_
#include "gameplay/Components/common/StickToPolylinePhysComponent.h"
#endif //_ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_

#ifndef _ITF_AIDEATHBEHAVIOR_H_
#include "gameplay/AI/Behaviors/AIDeathBehavior.h"
#endif //_ITF_AIDEATHBEHAVIOR_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/Managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(AIDeathBehavior_Template)

    BEGIN_SERIALIZATION_CHILD(AIDeathBehavior_Template)
        SERIALIZE_MEMBER("pauseComponentWhenDone", m_pauseComponentWhenDone);
        SERIALIZE_MEMBER("pauseActorWhenDone", m_pauseActorWhenDone);
        SERIALIZE_MEMBER("destroyActorWhenDone", m_destroyActorWhenDone);
        SERIALIZE_MEMBER("deactivatePhysics", m_deactivatePhysics);
        SERIALIZE_MEMBER("nullWeight", m_nullWeight);
    END_SERIALIZATION()

        AIDeathBehavior_Template::AIDeathBehavior_Template()
        : Super()
        , m_pauseComponentWhenDone(bfalse)
        , m_pauseActorWhenDone(btrue)
        , m_destroyActorWhenDone(bfalse)
        , m_deactivatePhysics(bfalse)
        , m_nullWeight(bfalse)
    {

    }

    IMPLEMENT_OBJECT_RTTI(AIDeathBehavior)

    AIDeathBehavior::AIDeathBehavior()
    {
    }

    AIDeathBehavior::~AIDeathBehavior()
    {
    }


    void AIDeathBehavior::onActivate()
    {
        Super::onActivate();

        StickToPolylinePhysComponent * physComponent = m_actor->GetComponent<StickToPolylinePhysComponent>();
        if (physComponent)
        {
            if (getTemplate()->getDeactivatePhysics())
            {
                physComponent->setProcessContacts(bfalse);
                physComponent->setDisabled(btrue, btrue, btrue);
            }
            if (getTemplate()->getNullWeight())
            {
                physComponent->setWeightMultiplier(0.0f);
            }
        }
    }

    void AIDeathBehavior::onActionFinished()
    {
        Super::onActionFinished();

        if (m_currentActionIndex >= m_actions.size())
        {
            // all actions are done

            if (getTemplate()->getPauseComponentWhenDone())
            {
                m_aiComponent->pause();
            }

            if (getTemplate()->getPauseActorWhenDone())
            {
                m_actor->disable();
            }

            if (getTemplate()->getDestroyActorWhenDone())
            {
                // same as AIDestroyAction; if actor is spawned, destroy it,
                // otherwise just disable it
                if (m_actor->isSerializable())
                {
                    m_actor->disable();
                }
                else
                {
                    m_actor->requestDestruction();
                }
            }
        }
    }

}