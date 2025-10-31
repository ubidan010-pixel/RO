#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_CYCLOPEUSAICOMPONENT_H_
#include "Rayman/gameplay/Components/AI/Ray_CyclopeusAIComponent.h"
#endif //_ITF_RAY_CYCLOPEUSAICOMPONENT_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef _ITF_AIRECEIVEHITBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIReceiveHitBehavior.h"
#endif //_ITF_AIRECEIVEHITBEHAVIOR_H_

#ifndef _ITF_RAY_AIDARKTOONUSCYCLOPEUSBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/DarktoonusCyclopeus/Ray_AIDarktoonusCyclopeusBehavior.h"
#endif //_ITF_RAY_AIDARKTOONUSCYCLOPEUSBEHAVIOR_H_

#ifndef _ITF_AISIMPLEPLAYANIMBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AISimplePlayAnimBehavior.h"
#endif //_ITF_AISIMPLEPLAYANIMBEHAVIOR_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_


namespace ITF
{

    IMPLEMENT_OBJECT_RTTI(Ray_CyclopeusAIComponent)
    BEGIN_SERIALIZATION_CHILD(Ray_CyclopeusAIComponent)
    END_SERIALIZATION()

    //*****************************************************************************

    Ray_CyclopeusAIComponent::Ray_CyclopeusAIComponent()
    : m_hurtBehavior(NULL)
    , m_hitCount(0)
    {
        // none
    }

    //*****************************************************************************

    Ray_CyclopeusAIComponent::~Ray_CyclopeusAIComponent()
    {
        // none
    }

    //*****************************************************************************

    void Ray_CyclopeusAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {

        m_hurtBehavior = static_cast<AISimplePlayAnimBehavior*>(createAiBehavior(getTemplate()->getHurtBehavior()));

        Super::onActorLoaded(_hotReload);
    }

    //*****************************************************************************

    void Ray_CyclopeusAIComponent::Update( f32 _dt )
    {
        Super::Update(_dt);

        UpdatePupil(_dt);
      
    }

    //*****************************************************************************

    void Ray_CyclopeusAIComponent::UpdatePupil( f32 _dt)
    {
        static_cast<Ray_AIDarktoonusCyclopeusBehavior*>(m_behavior)->UpdatePupil(_dt);
    }

    //*****************************************************************************

    void Ray_CyclopeusAIComponent::onBehaviorFinished()
    {
        if (m_currentBehavior == m_hurtBehavior)
        {
            onHurtBehaviorFinished();
        }
        else
        {
            Super::onBehaviorFinished();
        }
    }

    //*****************************************************************************

    void Ray_CyclopeusAIComponent::onReceiveHitBehaviorFinished()
    {
        m_hitCount = 0;
        setBehavior(m_hurtBehavior);
    }

    //*****************************************************************************

    void Ray_CyclopeusAIComponent::onHurtBehaviorFinished()
    {
        setBehavior(m_behavior);
    }

    //*****************************************************************************

    void Ray_CyclopeusAIComponent::processHit( HitStim* _hit )
    {
        if (m_currentBehavior == m_deathBehavior ||
            m_currentBehavior == m_hurtBehavior ||
            (m_currentBehavior == m_behavior && !static_cast<Ray_AIDarktoonusCyclopeusBehavior*>(m_currentBehavior)->isHitable()) ||
            m_hitCount > getTemplate()->getHitMax())
        {
            return;
        }

        if ( _hit->getSender() == m_actor->getRef() )
        {
            // don't get hit by our own stims
            return;
        }

        // can't get hit by friendlies, except for domino hits
        if ( !m_receiveHitBehavior->canReceiveHit(_hit))
        {
            return;
        }

        // notify the sender that he touched us
        Actor* sender = static_cast<Actor*>(_hit->getSender().getObject());
        if (sender)
        {
            EventHitSuccessful hitSuccessful;
            hitSuccessful.setSender(m_actor->getRef());
            sender->onEvent(&hitSuccessful);
        }

        // Forward hit to parent
        ActorRef parentRef = m_actor->getParent();
        if (parentRef.isValid())
        {
            Actor* parentAct = parentRef.getActor();
            if (parentAct)
            {
                EventGeneric eventHit;
                eventHit.setId(ITF_GET_STRINGID_CRC(DarkDoor_HitFromChild,1773128499));
                eventHit.setSender(m_actor->getRef());
                parentAct->onEvent(&eventHit);
            }
        }

        setBehavior(m_receiveHitBehavior);
        m_receiveHitBehavior->setData(_hit);
        m_hitCount++;
    }



    //*****************************************************************************
    // Template
    //*****************************************************************************

    IMPLEMENT_OBJECT_RTTI(Ray_CyclopeusAIComponent_Template)
        BEGIN_SERIALIZATION_CHILD(Ray_CyclopeusAIComponent_Template)
        SERIALIZE_OBJECT_WITH_FACTORY("hurtBehavior", m_hurtBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
        SERIALIZE_MEMBER("hitMax", m_hitMax);
     END_SERIALIZATION()

    Ray_CyclopeusAIComponent_Template::Ray_CyclopeusAIComponent_Template()
    : m_hurtBehavior(NULL)
    , m_hitMax(3)
    {
        // none
    }

    //*****************************************************************************

    Ray_CyclopeusAIComponent_Template::~Ray_CyclopeusAIComponent_Template()
    {
        SF_DEL(m_hurtBehavior);
    }

    //*****************************************************************************


}