#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AIRECEIVEHITACTION_H_
#include "rayman/gameplay/AI/Actions/Ray_AIReceiveHitAction.h"
#endif //_ITF_RAY_AIRECEIVEHITACTION_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/Managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef SERIALIZEROBJECTBINARY_H
#include "core/serializer/ZSerializerObjectBinary.h"
#endif // SERIALIZEROBJECTBINARY_H

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_AIReceiveHitAction)

    Ray_AIReceiveHitAction::Ray_AIReceiveHitAction()
        : Super()
        , m_reward(NULL)
        , m_attacker(ITF_INVALID_OBJREF)
        , m_hitType(RECEIVEDHITTYPE_FRONTPUNCH)
        , m_hitLevel(0)
        , m_hitDir(Vec2d::Right)
        , m_hitPushBackDistance(0.0f)
        , m_hitDirection(FRONT)
        , m_prevWindMultiplier(1.0f)
    {
    }

    Ray_AIReceiveHitAction::~Ray_AIReceiveHitAction()
    {
        SF_DEL(m_reward);

#ifdef ITF_SUPPORT_EDITOR
        EVENTMANAGER->unregisterEvent(ITF_GET_STRINGID_CRC(Ray_EventQueryLumCount,3794901482), m_aiComponent);
#endif
    }

    void Ray_AIReceiveHitAction::onActorLoaded(Pickable::HotReloadType _hotReload )
    {
        Super::onActorLoaded( _hotReload );

#ifdef ITF_SUPPORT_EDITOR
        EVENTMANAGER_REGISTER_EVENT_LISTENER(ITF_GET_STRINGID_CRC(Ray_EventQueryLumCount,3794901482),m_aiComponent);
#endif
    }

    void Ray_AIReceiveHitAction::setData( HitStim* _hit )
    {
        PunchStim* punch = _hit->DynamicCast<PunchStim>(ITF_GET_STRINGID_CRC(PunchStim,200533519));
        ITF_ASSERT_MSG(punch, "received hit is not a punch!");
        if (!punch)
            return;

        m_attacker = punch->getOriginalSender().isValid() ? punch->getOriginalSender() : punch->getSender();
        m_hitLevel = punch->getLevel();

        if ( punch->getIsRadial() )
        {
            m_hitDir = m_actor->get2DPos() - punch->getPos();
            m_hitDir.normalize();
        }
        else
        {
            m_hitDir = punch->getDirection();
        }
        m_hitDir = punch->getDirection();
        m_hitType = punch->getReceivedHitType();

        m_hitPushBackDistance = punch->getPushBackDistance();

        //AIUtils::drawDBGPhysShape(_hit->getPos(), _hit->getAngle(), _hit->getShape(), Color::red(), 0.f);
    }

    void Ray_AIReceiveHitAction::onActivate()
    {
        Super::onActivate();

        const Actor * actor = m_attacker.getActor();
        if (actor)
        {
            const Vec2d vecToVictim = actor->get2DPos() - m_actor->get2DPos();
            bbool targetIsLeft = (vecToVictim.dot(Vec2d::Right) < 0);
            bbool lookingLeft = m_actor->isFlipped();

            if (lookingLeft != targetIsLeft)
                m_hitDirection = BACK;
            else
                m_hitDirection = FRONT;
        }
            
        faceHitDir(m_hitDir);

        setupPlayRate();

        setupReward();

        if (getTemplate()->getIgnoreWind() && m_physComponent)
        {

            m_prevWindMultiplier = m_physComponent->getWindMultiplier();
            m_physComponent->setWindMultiplier(0.0f);
        }
    }

    void Ray_AIReceiveHitAction::onDeactivate()
    {
        Super::onDeactivate();

        //m_attacker = ITF_INVALID_OBJREF;

        m_animComponent->setPlayRate(1.f);
        
        if (getTemplate()->getIgnoreWind() && m_physComponent)
        {
            m_physComponent->setWindMultiplier(m_prevWindMultiplier);
        }
    }
    
    void Ray_AIReceiveHitAction::onEvent( Event * _event )
    {
        Super::onEvent( _event );

        if ( Ray_EventQueryLumCount *queryLumCount = _event->DynamicCast<Ray_EventQueryLumCount>(ITF_GET_STRINGID_CRC(Ray_EventQueryLumCount,3794901482)))
        {
            if (m_actor->getWorld() != SPAWNER->getSpawnerWorld())
            {
                const Ray_EventSpawnRewardLum *reward = IRTTIObject::SafeDynamicCast<const Ray_EventSpawnRewardLum>(getTemplate()->getReward(),ITF_GET_STRINGID_CRC(Ray_EventSpawnRewardLum,812488722));
                if (reward)
                {
                    queryLumCount->addNormalLums(reward->getNumRewards());
                }
            }
        }
    }

    void Ray_AIReceiveHitAction::faceHitDir( const Vec2d& _hitDir )
    {
        if (getTemplate()->getFaceHitDir() && m_hitDirection == BACK)
        {
            m_actor->setFlipped(!m_actor->isFlipped());
        }

    }

    void Ray_AIReceiveHitAction::setupPlayRate()
    {
        f32 rateVar = getTemplate()->getPlayRateVariation();

        if ( rateVar )
        {
            f32 playRate;

            if ( rateVar < 0.f )
            {
                playRate = 1.f - ( f32_Abs(rateVar) * Seeder::getSharedSeeder().GetFloat() );
            }
            else 
            {
                playRate = 1.f + ( rateVar * Seeder::getSharedSeeder().GetFloat() );
            }

            playRate = Max(playRate,0.1f);

            m_animComponent->setPlayRate(playRate);
        }
    }

    void Ray_AIReceiveHitAction::setupReward()
    {
        const Ray_EventSpawnReward* template_reward = getTemplate()->getReward();

        if ( template_reward && GAMEMANAGER->isPlayerActor(m_attacker) )
        {
            if ( !m_reward )
            {
                m_reward = static_cast<Ray_EventSpawnReward*>(template_reward->CreateNewObject());
                BinaryClone(const_cast<Ray_EventSpawnReward*>(template_reward),m_reward);
            }

            m_reward->clear();
            m_reward->setup(m_actor);
            m_reward->setRewardReceiver(m_attacker);
            GAMEMANAGER->onEvent(m_reward);
        }
    }

    void Ray_AIReceiveHitAction::updateAnimInputs()
    {
        Super::updateAnimInputs();

        if (m_animatedComponent)
        {
            m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(ReceivedHitLevel,3435787350), m_hitLevel);
            m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(ReceivedHitType,490560957), static_cast<u32>(m_hitType));
            m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(ReceivedHitDirection,1947665929), static_cast<u32>(m_hitDirection));
        }
    }

    IMPLEMENT_OBJECT_RTTI(Ray_AIReceiveHitAction_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_AIReceiveHitAction_Template)
        
        SERIALIZE_OBJECT_WITH_FACTORY("reward", m_reward, ACTORSMANAGER->getEventFactory());
        SERIALIZE_MEMBER("playRateVariation", m_playRateVariation);
        SERIALIZE_MEMBER("faceHitDir",m_faceHitDir);
        SERIALIZE_MEMBER("ignoreWind",m_ignoreWind);
    
    END_SERIALIZATION()

    Ray_AIReceiveHitAction_Template::Ray_AIReceiveHitAction_Template()
    : m_reward(NULL)
    , m_playRateVariation(0.0f)
    , m_faceHitDir(btrue)
    , m_ignoreWind(btrue)
    {
        m_animDefaultAction = ITF_GET_STRINGID_CRC(RECEIVEHIT,848873176);

    }

    Ray_AIReceiveHitAction_Template::~Ray_AIReceiveHitAction_Template()
    {
        SF_DEL(m_reward);
    }
}