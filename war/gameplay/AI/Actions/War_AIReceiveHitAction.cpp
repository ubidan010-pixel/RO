#include "precompiled_gameplay_war.h"

#ifndef _ITF_WAR_AIRECEIVEHITACTION_H_
#include "war/gameplay/ai/Actions/War_AIReceiveHitAction.h"
#endif //_ITF_WAR_AIRECEIVEHITACTION_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/Managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_ANIMATIONTRACK_H_
#include "engine/animation/AnimationTrack.h"
#endif //_ITF_ANIMATIONTRACK_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(War_AIReceiveHitAction)

    BEGIN_SERIALIZATION_CHILD(War_AIReceiveHitAction)
        SERIALIZE_MEMBER("aiReceiveHitForce", m_hitForce, ESerialize_PropertyEdit|ESerialize_Template);
        SERIALIZE_MEMBER("aiReceiveHitFrictionMultiplier", m_frictionMultiplier, ESerialize_PropertyEdit|ESerialize_Template);
        SERIALIZE_MEMBER("aiReceiveHitGravityMultiplier", m_gravityMultiplier, ESerialize_PropertyEdit|ESerialize_Template);
        SERIALIZE_MEMBER("playRateVariation", m_playRateVariation, ESerialize_PropertyEdit|ESerialize_Template);
        SERIALIZE_MEMBER("minStunTime", m_minStunTime, ESerialize_PropertyEdit|ESerialize_Template);
        SERIALIZE_MEMBER("maxStunTime", m_maxStunTime, ESerialize_PropertyEdit|ESerialize_Template);
    END_SERIALIZATION()

    War_AIReceiveHitAction::War_AIReceiveHitAction()
        : Super()
        , m_physComponent(NULL)
        , m_hitForce(200.f)
        , m_frictionMultiplier(0.5f)
        , m_gravityMultiplier(0.5f)
        , m_minStunTime(0.f)
        , m_maxStunTime(0.f)
        , m_playRateVariation(1.f)
        , m_attacker(ITF_INVALID_OBJREF)
        , m_hitDir(Vec2d::Right)
        , m_receivedHitType(WAR_RECEIVEDHITTYPE_NORMALHIT)
        , m_hitLevel(0)
        , m_prevFrictionMultiplier(1.f)
        , m_prevGravityMultiplier(1.f)
        , m_animCursor(0.f)
        , m_timeCounter(0.f)
        , m_stunned(bfalse)
        , m_canBeFinishedOff(bfalse)
        , m_hitFxPos(Vec3d::Zero)
    {
        static const StringID s_ReceiveHit = "RECEIVEHIT";
        m_animDefaultAction = s_ReceiveHit;
    }

    War_AIReceiveHitAction::~War_AIReceiveHitAction()
    {
    }

    void War_AIReceiveHitAction::onActorLoaded()
    {   
        Super::onActorLoaded();

        m_physComponent = m_actor->GetComponent<StickToPolylinePhysComponent>();

        m_actor->registerEvent(War_HitStim::GetClassNameStatic(), m_aiComponent);
    }

    void War_AIReceiveHitAction::onActivate()
    {
        Super::onActivate();

        m_physComponent->setSpeed(Vec2d::Zero);

        m_prevFrictionMultiplier = m_physComponent->getFrictionMultiplier();
        m_prevGravityMultiplier = m_physComponent->getGravityMultiplier();

        doDamage();
        setupHit();
    }

    void War_AIReceiveHitAction::onDeactivate()
    {
        m_physComponent->setFrictionMultiplier(m_prevFrictionMultiplier);
        m_physComponent->setGravityMultiplier(m_prevGravityMultiplier);
        m_attacker = ITF_INVALID_OBJREF;
        m_animComponent->setPlayRate(1.f);
    }

    void War_AIReceiveHitAction::setupHit()
    {
        m_animComponent->setPlayRate(1.f);
        m_stunned = bfalse;
        m_timeCounter = 0.f;
        m_canBeFinishedOff = bfalse;

        const Vec2d& gravityDir = m_aiComponent->getCurrentGravityDir();

        f32 cross = gravityDir.cross(m_hitDir);
        m_animComponent->setFlipped(cross>0.f);

        //Neil deactivated this needs to be rewritten with a WAR_AiComponent
        //if (m_fxController)
        //{
        //    m_fxController->playHit(m_attacker, m_hitLevel, m_hitFxPos);
        //}


        switch (m_receivedHitType)
        {
            case WAR_RECEIVEDHITTYPE_NORMALHIT:
            default:
                setNormalHit();
                setupNormalHitReward();
                break;
        }
    }

    void War_AIReceiveHitAction::setupNormalHitReward()
    {
        //Actor* attackerActor = AIUtils::getActor(m_attacker);
        //if ( attackerActor && AIUtils::isEnemy(m_actor, attackerActor) )
        //{
        //}
    }

    void War_AIReceiveHitAction::setNormalHit()
    {
        switch(m_hitLevel)
        {
        case 1: // level 2
        case 2: // level 3
            {
                Vec2d force = m_hitDir * ( m_hitForce * ( m_hitLevel + 1.f ) );

                m_physComponent->addForce(force);

                m_physComponent->setFrictionMultiplier(m_frictionMultiplier / static_cast<f32>(m_hitLevel) );

                if ( !m_physComponent->getStickedEdge() )
                {
                    m_physComponent->setGravityMultiplier(m_gravityMultiplier / static_cast<f32>(m_hitLevel) );
                }
            }
            break;
        default: // level 1
            // Just the animation
            break;
        }

        if ( m_playRateVariation )
        {
            f32 playRate;

            if ( m_playRateVariation < 0.f )
            {
                playRate = 1.f - ( fabs(m_playRateVariation) * Seeder::getSharedSeeder().GetFloat() );
            }
            else 
            {
                playRate = 1.f + ( m_playRateVariation * Seeder::getSharedSeeder().GetFloat() );
            }

            playRate = Max(playRate,0.1f);

            m_animComponent->setPlayRate(playRate);
        }
    }

    void War_AIReceiveHitAction::update( f32 _delta )
    {
        switch(m_receivedHitType)
        {
        case WAR_RECEIVEDHITTYPE_NORMALHIT:
        default:
            updateNormalHit(_delta);
            break;
        }
    }

    void War_AIReceiveHitAction::updateNormalHit( f32 _dt )
    {
        if ( !m_stunned )
        {
            if ( isAnimFinished() )
            {
                m_timeCounter = Seeder::getSharedSeeder().GetFloat(m_minStunTime, m_maxStunTime);

                if ( m_timeCounter )
                {
                    m_stunned = btrue;
                }
                else
                {
                    m_aiBehavior->onActionFinished();
                }
            }
        }
        else
        {
            if ( m_timeCounter )
            {
                m_timeCounter = Max(m_timeCounter - _dt, 0.f);

                if ( !m_timeCounter )
                {
                    m_aiBehavior->onActionFinished();
                }
            }
        }
    }

    void War_AIReceiveHitAction::onEvent( class Event * _event)
    {
        Super::onEvent(_event);

        if (AnimGameplayEvent* animEvt = _event->DynamicCast<AnimGameplayEvent>(ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301)))
        {
            if (animEvt->getName() == War_MRK_FinishOff_Start)
            {
                m_canBeFinishedOff = btrue;
            }
            else if (animEvt->getName() == War_MRK_FinishOff_Stop)
            {
                m_canBeFinishedOff = bfalse;
            }
        }
        else if (War_HitStim* hit = _event->DynamicCast<War_HitStim>(ITF_GET_STRINGID_CRC(War_HitStim,3270006373)))
        {
            // we're receiving a hit while we were already in the 'receive hit' action
            newHit(hit);
        }
    }

    void War_AIReceiveHitAction::newHit( War_HitStim* _hit )
    {
        if (m_canBeFinishedOff)
        {
            // die
            m_aiComponent->receiveHitDamage(WAR_HITLEVEL_DEATH);
            m_aiBehavior->onActionFinished();
            return;
        }

        // re-trigger the hit
        setData(_hit);

        doDamage();
        setupHit();

        m_animComponent->resetCurTime();
        m_aiComponent->computeRandomAnimNumber();
    }

    void War_AIReceiveHitAction::setData( HitStim* _hit )
    {
        War_HitStim* warHit = _hit->DynamicCast<War_HitStim>(ITF_GET_STRINGID_CRC(War_HitStim,3270006373));
        ITF_ASSERT_MSG(warHit, "received hit is not a War_HitStim!");
        if (!warHit)
            return;

        m_attacker = warHit->getSender();
        m_hitLevel = warHit->getLevel(m_actor->get2DPos());
        m_hitDir = warHit->getDirection(m_actor->get2DPos());
        m_receivedHitType = warHit->getReceivedHitType();
        m_hitFxPos = warHit->getFxPos();
    }

    void War_AIReceiveHitAction::doDamage()
    {
        m_aiComponent->receiveHitDamage(m_hitLevel);
    }

    void War_AIReceiveHitAction::onActorBecomeInactive()
    {
        //if ( m_ejected )
        //{
        //    m_actor->getScene()->unregisterActor(m_actor);
        //}
    }

    void War_AIReceiveHitAction::updateAnimInputs()
    {
        Super::updateAnimInputs();

        static const StringID s_ReceivedHitLevel("ReceivedHitLevel");
        static const StringID s_ReceivedHitType("ReceivedHitType");
        static const StringID s_Stunned("Stunned");

        if (m_animatedComponent)
        {
            m_animatedComponent->setInput(s_ReceivedHitLevel, m_hitLevel);
            m_animatedComponent->setInput(s_ReceivedHitType, static_cast<u32>(m_receivedHitType));
            m_animatedComponent->setInput(s_Stunned, static_cast<u32>(m_stunned));
            m_animatedComponent->setProceduralCursor(m_animCursor);
        }
    }

}