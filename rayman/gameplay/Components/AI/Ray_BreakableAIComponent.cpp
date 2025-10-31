#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_BREAKABLECOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_BreakableAIComponent.h"
#endif //_ITF_RAY_BREAKABLECOMPONENT_H_

#ifndef _ITF_PHANTOMCOMPONENT_H_
#include "gameplay/Components/Misc/PhantomComponent.h"
#endif //_ITF_PHANTOMCOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/Managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef SERIALIZEROBJECTBINARY_H
#include "core/serializer/ZSerializerObjectBinary.h"
#endif // SERIALIZEROBJECTBINARY_H


namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_BreakableAIComponent)
    BEGIN_SERIALIZATION_CHILD(Ray_BreakableAIComponent)
        BEGIN_CONDITION_BLOCK(ESerializeGroup_Checkpoint)
            SERIALIZE_MEMBER("currentDestructionStage",m_currentDestructionStage);
            SERIALIZE_MEMBER("targetDestructionStage",m_targetDestructionStage);
        END_CONDITION_BLOCK()
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(Ray_BreakableAIComponent)
        //VALIDATE_COMPONENT_PARAM("", m_phantomComponent, "Ray_BreakableAIComponent requires a PhantomComponent");
        //VALIDATE_COMPONENT_PARAM("", m_receiveHitBehavior, "This behavior is mandatory");
    END_VALIDATE_COMPONENT()


    Ray_BreakableAIComponent::Ray_BreakableAIComponent()
        : Super()
        , m_currentDestructionStage(0)
        , m_targetDestructionStage(0)
        , m_justReceivedHit(bfalse)
        , m_isPlayingDestruction(bfalse)
        , m_mustBreak(bfalse)
        , m_mustUnbreak(bfalse)
        , m_restoring(bfalse)
        , m_reward(NULL)
    {
    }

    Ray_BreakableAIComponent::~Ray_BreakableAIComponent()
    {
        SF_DEL(m_reward);
    }

    void Ray_BreakableAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(PunchStim,200533519), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventQueryCanStickOrCollide,4105599404), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventBreakableBreak,4257664379), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventBreakableQuery,4136568804), this);

        if ( getTemplate()->getRewardEvent() )
        {
            m_reward = getTemplate()->getRewardEvent()->CreateNewObject()->DynamicCast<Ray_EventSpawnReward >(ITF_GET_STRINGID_CRC(Ray_EventSpawnReward,145553727));
            BinaryClone(const_cast<Ray_EventSpawnReward*>(getTemplate()->getRewardEvent()),m_reward);
        }

        reactivateActor();
    }

    void Ray_BreakableAIComponent::onCheckpointLoaded()
    {
        Super::onCheckpointLoaded();

        if (!m_animComponent)
        {
            return;
        }
        
        if ( getTemplate()->getResetOnCheckpoint() )
        {
            m_currentDestructionStage = 0;
            m_targetDestructionStage = 0;
        }
        else
        {
            if ( m_targetDestructionStage == getTemplate()->getDestructionStages().size() )
            {
                if (m_phantomComponent)
                {
                    m_phantomComponent->pause();
                }

                if (getTemplate()->getOpenAnim().isValid())
                    m_animComponent->setAnim(getTemplate()->getOpenAnim());

            }
            else
            {
                if (m_animComponent->getDefaultAnimation().isValid())
                {
                    m_animComponent->setAnim(m_animComponent->getDefaultAnimation());
                }
            }
        }
    }


    void Ray_BreakableAIComponent::deactivateActor()
    {
        if (m_phantomComponent)
        {
            m_phantomComponent->pause();
        }

        EventDead eventDead;
        m_actor->onEvent(&eventDead);


        // send reward
        if ( m_reward != NULL && getLastAttacker().isValid() )
        {
            m_reward->clear();
            m_reward->setup(m_actor);
            m_reward->setRewardReceiver(getLastAttacker());
            GAMEMANAGER->onEvent(m_reward);
        }
    }

    void Ray_BreakableAIComponent::reactivateActor()
    {
        if (m_phantomComponent)
        {
            m_phantomComponent->unpause();
        }
    }


    void Ray_BreakableAIComponent::onEvent( Event* _event )
    {
        Super::onEvent(_event);

        if ( PunchStim* stim = _event->DynamicCast<PunchStim>(ITF_GET_STRINGID_CRC(PunchStim,200533519)) )
        {
            processStim(stim);
        }
        else if (EventQueryCanStickOrCollide * query = _event->DynamicCast<EventQueryCanStickOrCollide>(ITF_GET_STRINGID_CRC(EventQueryCanStickOrCollide,4105599404)))
        {
            query->setCanCollide(m_targetDestructionStage != getTemplate()->getDestructionStages().size());
        }
        else if (Ray_EventBreakableBreak* breakEvent = _event->DynamicCast<Ray_EventBreakableBreak>(ITF_GET_STRINGID_CRC(Ray_EventBreakableBreak,4257664379)))
        {
            processBreakEvent(breakEvent);
        }
        else if (Ray_EventBreakableQuery* breakQuery = _event->DynamicCast<Ray_EventBreakableQuery>(ITF_GET_STRINGID_CRC(Ray_EventBreakableQuery,4136568804)))
        {
            processBreakQuery(breakQuery);
        }
    }

    void Ray_BreakableAIComponent::processStim( PunchStim* _stim )
    {
        if ( _stim->getSender() == m_actor->getRef() )
        {
            return;
        }

        if ( !AIUtils::isEnemyHit(_stim, getFaction()) )
        {
            return;
        }


        //if (m_justReceivedHit)
        //{
        //    return;
        //}

        if (m_isPlayingDestruction)
        {
            return;
        }

        if (m_currentDestructionStage == getTemplate()->getDestructionStages().size())
        {
            return;
        }

        const Vec2d myDir = AIUtils::getLookDir(m_actor->getAngle() + getTemplate()->getHitAngleOffset(), m_actor->isFlipped());
        const f32 dot =  myDir.dot(_stim->getDirection());

        if (getTemplate()->getHitAngleMinIncidence() != 0.0f)
        {
            if ( f32_Abs(dot)  < f32_Cos(getTemplate()->getHitAngleMinIncidence()))
                return;
        }

        Actor * sender = ActorRef(_stim->getSender()).getActor();
        if (getTemplate()->getCheckHitSenderDirection())
        {
            if (sender)
            {
                const Vec2d toMe = m_actor->get2DPos() - sender->get2DPos();
                const f32 dotP = myDir.dot(toMe);
                if ( dotP > 0.0f)
                {
                    //sender is on other side of me
                    // hit direction must go towards me
                    if (dot  < 0.0f)
                        return;
                }
                else
                {
                    //sender is on same side.
                    // hit must go in opposite direction
                    if (dot > 0.0f)
                        return;
                }
            }
        }
        // what side did the hit come from?
        bbool isFrontHit = (_stim->getDirection().dot(myDir) > 0);
        m_justReceivedHit = (isFrontHit) ? 1 : -1;

        if (_stim->IsClassCRC(ITF_GET_STRINGID_CRC(RehitStim,1008690800)))
        {
            receiveHit(_stim);
        }
        else if ( _stim->getReceivedHitType() == RECEIVEDHITTYPE_EARTHQUAKE )
        {
            receiveHit(_stim, bfalse, getTemplate()->getCrushAttackDamage());
        }
        else
        {
            u32 overrideDamage = isFrontHit ? getTemplate()->getFrontDamage() : getTemplate()->getBackDamage();
            receiveHit(_stim, overrideDamage == U32_INVALID, overrideDamage);
        }

        f32 hpPerStage = (f32)getTemplate()->getHealth_default() / (f32)getTemplate()->getDestructionStages().size();
        m_targetDestructionStage = (u32)((getTemplate()->getHealth_default() - m_health_current) / hpPerStage);


        bbool isDead = bfalse;
        if ( m_targetDestructionStage == getTemplate()->getDestructionStages().size() )
        {
            deactivateActor();
            isDead = btrue;
        }

        EventHitSuccessful success;
        success.setSender(m_actor->getRef());
        success.setIsDead(isDead);
        if (sender)
        {
            sender->onEvent(&success);
        }
    }

    void Ray_BreakableAIComponent::processBreakEvent( Ray_EventBreakableBreak* _breakEvent )
    {
        if ( _breakEvent->getIsBreak() )
        {
            m_mustBreak = btrue;
            m_mustUnbreak = bfalse;
        }
        else
        {
            m_mustUnbreak = btrue;
            m_mustBreak = bfalse;
        }
    }

    void Ray_BreakableAIComponent::processBreakQuery( Ray_EventBreakableQuery* _breakQuery )
    {
        _breakQuery->setIsBroken(m_mustUnbreak == bfalse && (m_targetDestructionStage == getTemplate()->getDestructionStages().size()));
        _breakQuery->setIsBreakable(btrue);
    }

    void Ray_BreakableAIComponent::onBecomeActive()
    {
        Super::onBecomeActive();

        if ( !m_isPlayingDestruction )
        {
            if ( m_mustBreak )
            {
                if ( m_currentDestructionStage < getTemplate()->getDestructionStages().size() )
                {
                    const Ray_BreakableAIComponent_Template::DestructionStage& stage = getTemplate()->getDestructionStages()[m_currentDestructionStage];

                    if ( stage.getStand().isValid() )
                    {
                        m_animComponent->setAnim(stage.getStand());
                        m_mustBreak = bfalse;
                        m_currentDestructionStage++;
                        m_targetDestructionStage=m_currentDestructionStage;
                        m_restoring = bfalse;
                    }
                }
            }
            else if ( m_mustUnbreak )
            {
                m_restoring = bfalse;
                m_mustUnbreak = bfalse;
                m_targetDestructionStage = m_currentDestructionStage = 0;
                m_animComponent->setAnim(m_animComponent->getDefaultAnimation());
            }
        }
    }

    void Ray_BreakableAIComponent::Update( f32 _dt )
    {
        Super::Update(_dt);
        if ( m_mustBreak )
        {
            m_targetDestructionStage = getTemplate()->getDestructionStages().size();
            playNextAvailableDestruction();
            m_mustBreak = bfalse;
        }
        else if ( m_mustUnbreak )
        {
            m_currentDestructionStage = 0;
            m_targetDestructionStage = 0;
            m_mustUnbreak = bfalse;
            m_animComponent->setAnim(getTemplate()->getRestoreAnim());
            m_restoring = btrue;
            m_isPlayingDestruction = bfalse;
        }
        else if (m_justReceivedHit)
        {
            playLastHitFx();

            if ( !playNextAvailableDestruction() )
            {
                const Ray_BreakableAIComponent_Template::DestructionStage& stage = getTemplate()->getDestructionStages()[m_currentDestructionStage];
                m_animComponent->setAnim(stage.getRumble());
            }

            m_justReceivedHit = bfalse;
        }
        else if ( m_restoring )
        {
            if ( m_animComponent->isSubAnimFinished() )
            {
                m_restoring = bfalse;
                m_animComponent->setAnim(m_animComponent->getDefaultAnimation());
            }
        }
        else if (m_isPlayingDestruction && m_animComponent->isSubAnimFinished())
        {
            m_isPlayingDestruction = bfalse;
            m_currentDestructionStage++;

            //if (m_currentDestructionStage == getTemplate()->getDestructionStages().size())
            //{
            //    deactivateActor();
            //}
            //else
            //{
            //    playNextAvailableDestruction();
            //}

            if (m_currentDestructionStage < getTemplate()->getDestructionStages().size())
            {
                playNextAvailableDestruction();
            }
        }
    }

    bbool Ray_BreakableAIComponent::playNextAvailableDestruction()
    {
        while ( m_currentDestructionStage != m_targetDestructionStage &&
                m_currentDestructionStage != getTemplate()->getDestructionStages().size() )
        {
            const Ray_BreakableAIComponent_Template::DestructionStage& stage = getTemplate()->getDestructionStages()[m_currentDestructionStage];

            if ( stage.getDestroy().isValid() )
            {
                m_isPlayingDestruction = btrue;

                if (m_justReceivedHit == -1 &&
                    stage.getDestroyReverse().isValid())
                {
                    m_animComponent->setAnim(stage.getDestroyReverse());
                }
                else
                {
                    m_animComponent->setAnim(stage.getDestroy());
                }

                m_restoring = bfalse;
                return btrue;
            }
            else
            {
                m_currentDestructionStage++;

                if (m_currentDestructionStage == getTemplate()->getDestructionStages().size())
                {
                    //deactivateActor();
                    if (m_justReceivedHit == -1 && getTemplate()->getOpenAnimReverse().isValid())
                        m_animComponent->setAnim(getTemplate()->getOpenAnimReverse());
                    else if (getTemplate()->getOpenAnim().isValid())
                        m_animComponent->setAnim(getTemplate()->getOpenAnim());

                    m_restoring = bfalse;
                    return btrue;
                }
            }
        }

        return bfalse;
    }

    //-------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(Ray_BreakableAIComponent_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_BreakableAIComponent_Template)
        SERIALIZE_OBJECT_WITH_FACTORY("reward", m_reward, ACTORSMANAGER->getEventFactory());
        SERIALIZE_MEMBER("crushAttackDamage", m_crushAttackDamage);
        SERIALIZE_MEMBER("frontDamage", m_frontDamage);
        SERIALIZE_MEMBER("backDamage", m_backDamage);
        SERIALIZE_MEMBER("hitAngleOffset", m_hitAngleOffset);
        SERIALIZE_MEMBER("hitAngleMinIncidence", m_hitAngleMinIncidence);
        SERIALIZE_MEMBER("restoreAnim", m_restoreAnim);
        SERIALIZE_MEMBER("checkHitSenderDirection", m_checkHitSenderDirection);
        SERIALIZE_CONTAINER_OBJECT("destructionStages", m_destructionStages);
        SERIALIZE_MEMBER("openAnim", m_openAnim);
        SERIALIZE_MEMBER("openAnimReverse", m_openAnimReverse);
        SERIALIZE_MEMBER("resetOnCheckpoint", m_resetOnCheckpoint);
    END_SERIALIZATION()

    BEGIN_SERIALIZATION_SUBCLASS(Ray_BreakableAIComponent_Template, DestructionStage)
        SERIALIZE_MEMBER("rumble", m_rumble);
        SERIALIZE_MEMBER("destroy", m_destroy);
        SERIALIZE_MEMBER("destroyReverse", m_destroyReverse);
        SERIALIZE_MEMBER("stand", m_stand);
    END_SERIALIZATION()

    Ray_BreakableAIComponent_Template::Ray_BreakableAIComponent_Template()
        : m_reward(NULL)
        , m_crushAttackDamage(100)
        , m_hitAngleMinIncidence(btrue,0.0f)
        , m_frontDamage(U32_INVALID)    // invalid = use hit level, otherwise override
        , m_backDamage(U32_INVALID)     // invalid = use hit level, otherwise override
        , m_checkHitSenderDirection(btrue)
        , m_resetOnCheckpoint(bfalse)
    {
    }

    Ray_BreakableAIComponent_Template::~Ray_BreakableAIComponent_Template()
    {
        SF_DEL(m_reward);
    }

};
