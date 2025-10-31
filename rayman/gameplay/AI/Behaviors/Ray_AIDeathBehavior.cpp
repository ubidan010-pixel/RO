#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AIDEATHBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ray_AIDeathBehavior.h"
#endif //_ITF_RAY_AIDEATHBEHAVIOR_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/Managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_RAY_GAMEPLAYEVENTS_H_
#include "rayman/gameplay/Ray_GameplayEvents.h"
#endif //_ITF_RAY_GAMEPLAYEVENTS_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef SERIALIZEROBJECTBINARY_H
#include "core/serializer/ZSerializerObjectBinary.h"
#endif // SERIALIZEROBJECTBINARY_H

#ifndef _ITF_RAY_AICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_AIComponent.h"
#endif //_ITF_RAY_AICOMPONENT_H_


namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_AIDeathBehavior_Template)

    BEGIN_SERIALIZATION_CHILD(Ray_AIDeathBehavior_Template)
        SERIALIZE_OBJECT_WITH_FACTORY("reward", m_reward, ACTORSMANAGER->getEventFactory());
        SERIALIZE_MEMBER("soul",m_soulPath);
        SERIALIZE_CONTAINER("numRewards",m_numRewards);
        SERIALIZE_MEMBER("spawnOnMarker",m_spawnOnMarker);
    END_SERIALIZATION()
        Ray_AIDeathBehavior_Template::Ray_AIDeathBehavior_Template()
        : m_reward(NULL)
        , m_spawnOnMarker(btrue)
    {
        m_numRewards.push_back(0);
        m_numRewards.push_back(1);
        m_numRewards.push_back(2);
        m_numRewards.push_back(4);
    }
    IMPLEMENT_OBJECT_RTTI(Ray_AIDeathBehavior)
    Ray_AIDeathBehavior::Ray_AIDeathBehavior()
        : Super()
        , m_reward(NULL)
        , m_physComponent(NULL)
    {
    }

    Ray_AIDeathBehavior::~Ray_AIDeathBehavior()
    {
        SF_DEL(m_reward);
    }


    void Ray_AIDeathBehavior::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        m_physComponent = m_actor->GetComponent<StickToPolylinePhysComponent>();

        initRewardAndFx();

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301),m_aiComponent);
    }

    void Ray_AIDeathBehavior::onActivate()
    {
        Super::onActivate();

        if (m_physComponent)
        {
            m_physComponent->setDisabled(btrue, btrue, btrue);
        }

        if ( !getTemplate()->getSpawnOnMarker())
        {
            spawnRewardAndFx();
        }
   }

    void Ray_AIDeathBehavior::onEvent( Event * _event )
    {
        Super::onEvent( _event );
        if (AnimGameplayEvent * event = _event->DynamicCast<AnimGameplayEvent>(ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301)))
        {
            if (getTemplate()->getSpawnOnMarker() && event->getName() == ITF_GET_STRINGID_CRC(MRK_DeathFX,1031732103))
            {
                spawnRewardAndFx();
            }
        }
    }

    void Ray_AIDeathBehavior::spawnRewardAndFx()
    {
        if (m_soulSpawner.isInitialized())
        {
            m_soulSpawner.getSpawnee(m_actor->getScene(), m_actor->getPos(), m_actor->getAngle());
        }
        if ( m_reward != NULL && m_aiComponent->getLastAttacker().isValid() && m_aiComponent->getLastHitLevel() < getTemplate()->getNumRewards().size())
        {
            m_reward->clear();
            m_reward->setup(m_actor);
            ITF_ASSERT(m_aiComponent->getLastHitLevel() < getTemplate()->getNumRewards().size());
            m_reward->setNumRewards(getTemplate()->getNumRewards()[m_aiComponent->getLastHitLevel()]);

            m_reward->setRewardReceiver(m_aiComponent->getLastAttacker());
            if ( Ray_AIComponent * rayai = m_aiComponent->DynamicCast<Ray_AIComponent>(ITF_GET_STRINGID_CRC(Ray_AIComponent,2143961386)) )
            {
                m_reward->setRewardReceiver( rayai->getRewardReceiver() );
            }            
            m_reward->setEjectionDir(m_aiComponent->getLastHitDir());
            GAMEMANAGER->onEvent(m_reward);
        }
    }

    void Ray_AIDeathBehavior::initRewardAndFx()
    {
        if (!getTemplate()->getSoulPath().isEmpty())
        {
            SPAWNER->declareNeedsSpawnee(m_actor, &m_soulSpawner, getTemplate()->getSoulPath());
        }

        if ( getTemplate()->getReward() )
        {
            m_reward = getTemplate()->getReward()->CreateNewObject()->DynamicCast<Ray_EventSpawnReward>(ITF_GET_STRINGID_CRC(Ray_EventSpawnReward,145553727));;
            BinaryClone(const_cast<Ray_EventSpawnReward*>(getTemplate()->getReward()),m_reward);
        }
    }
}