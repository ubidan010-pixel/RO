#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_JALAPENOKINGAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_JalapenoKingAiComponent.h"
#endif //_ITF_RAY_JALAPENOKINGAICOMPONENT_H_
#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_JalapenoKingAiComponent)

        BEGIN_SERIALIZATION_CHILD(Ray_JalapenoKingAiComponent)
            BEGIN_CONDITION_BLOCK(ESerializeGroup_Checkpoint)
                SERIALIZE_MEMBER("dead",m_dead);
            END_CONDITION_BLOCK()
        END_SERIALIZATION()

        BEGIN_VALIDATE_COMPONENT(Ray_JalapenoKingAiComponent)
        END_VALIDATE_COMPONENT()


        Ray_JalapenoKingAiComponent::Ray_JalapenoKingAiComponent()
        : Super()
        , m_spewing(bfalse)
        , m_animComponent(NULL)
        , m_linkComponent(NULL)
        , m_fxController(NULL)
        , m_dead(bfalse)
        , m_hit(bfalse)
        , m_rehitTimer( 0.0f )
        , m_deathReward( NULL )
    {
    }

    Ray_JalapenoKingAiComponent::~Ray_JalapenoKingAiComponent()
    {
        SF_DEL( m_deathReward );
    }

    void Ray_JalapenoKingAiComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventInteractionQuery,209600608), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventCrushed,3831882623), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(HitStim,2121727796), this);

        m_animComponent = m_actor->GetComponent<AnimLightComponent>();
        m_linkComponent = m_actor->GetComponent<LinkComponent>();
        m_fxController = m_actor->GetComponent<FXControllerComponent>();

        m_animComponent->setAnim("idle");
        m_spewing = bfalse;

        if( getTemplate()->getDeathReward() != NULL )
        {
            m_deathReward = getTemplate()->getDeathReward()->CreateNewObject()->DynamicCast<Ray_EventSpawnReward>(ITF_GET_STRINGID_CRC(Ray_EventSpawnReward,145553727));
            if ( m_deathReward != NULL )
            {
                BinaryClone(const_cast<Ray_EventSpawnReward*>(getTemplate()->getDeathReward()),m_deathReward);
            }
        }

    }
    void Ray_JalapenoKingAiComponent::Update( f32 _dt )
    {
        Super::Update(_dt);
        if ( m_hit && m_animComponent->isSubAnimFinished() )
        {
            m_hit = bfalse;
            m_animComponent->setAnim("idle", 5 );
        }

        m_rehitTimer = f32_Max( 0.0f, m_rehitTimer - _dt );
    }

    void Ray_JalapenoKingAiComponent::onEvent( Event* _event )
    {
        Super::onEvent(_event);

        if ( AnimGameplayEvent* ge = _event->DynamicCast<AnimGameplayEvent>(ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301)) )
        {
            if (ge->getName() == "MRK_Start")
            {
                m_spewing = btrue;
                startFire();
            }
            if (ge->getName() == "MRK_Stop")
            {
                m_spewing = bfalse;
                stopFire();
            }
        }
        if ( EventInteractionQuery* query = _event->DynamicCast<EventInteractionQuery>(ITF_GET_STRINGID_CRC(EventInteractionQuery,209600608)) )
        {
            // quick & dirty fix for http://mdc-tomcat-jira01.ubisoft.org/jira/browse/RO-4095
            // update, using 45� now http://mdc-tomcat-jira01.ubisoft.org/jira/browse/RO-4656
            f32 angle = m_actor->getAngle();
            NormalizeAngle2PI(angle);                
            static const f32 epsilon = 0.01f;
            if (angle <= (MTH_PIBY4 - epsilon) || angle >= ((7.0f * MTH_PIBY4) + epsilon))
            {
                query->setInteraction(CharacterInteractionType_Crush);
            }
        }
        else if ( EventCrushed* crushed = _event->DynamicCast<EventCrushed>(ITF_GET_STRINGID_CRC(EventCrushed,3831882623)) )
        {
            m_animComponent->setAnim("hit");
            m_dead = btrue;
            if (m_spewing)
            {
                stopFire();
            }
            EventDisableCollision disableCollision;
            disableCollision.setSender(m_actor->getRef());
            disableCollision.setDisable(btrue);
            m_actor->onEvent(&disableCollision);
        }
        else if ( HitStim* stim = _event->DynamicCast<HitStim>(ITF_GET_STRINGID_CRC(HitStim,2121727796)) )
        {
            if (AIUtils::isEnemyHit(stim,getFaction()))
            {
                const StringID receivehitAnim = ITF_GET_STRINGID_CRC(receivehit, 848873176);
                ActorRef prevAttacker = m_lastAttacker;
                receiveHit( stim );
                
                SubAnim *subAnim = m_animComponent->getSubAnim( receivehitAnim );
                bbool hitSuccess = bfalse;
                                
                if ( ( getHealth() == 0 ) || ( subAnim == NULL ) )
                {
                    m_hit = bfalse;
                    hitSuccess = btrue;
                    m_animComponent->setAnim("hit");
                    m_dead = btrue;
                    if (m_spewing)
                    {
                        stopFire();
                        m_spewing = bfalse;
                    }
                    EventDisableCollision disableCollision;
                    disableCollision.setSender(m_actor->getRef());
                    disableCollision.setDisable(btrue);
                    m_actor->onEvent(&disableCollision);

                    if ( m_deathReward != NULL )
                    {
                        m_deathReward->setup( m_actor );
                        m_deathReward->setRewardReceiver( stim->getOriginalSender().isValid() ? stim->getOriginalSender() : stim->getSender() );
                        GAMEMANAGER->onEvent( m_deathReward );
                    }
                }
                else if ( ( m_rehitTimer == 0.0f ) || ( stim->getSender() != prevAttacker ) )
                {
                    m_rehitTimer = 0.2f;
                    m_animComponent->setAnim( receivehitAnim );
                    m_hit = btrue;
                    hitSuccess = btrue;
                    if (m_spewing)
                    {
                        stopFire();
                        m_spewing = bfalse;
                    }
                }

                if ( Actor * sender = AIUtils::getActor( stim->getSender() ) )
                {
                    EventHitSuccessful successEvent;
                    sender->onEvent( &successEvent );

                    if ( m_fxController != NULL )
                    {
                        const StringID &feedbackAction = stim->getActionFromHit();
                        u32 fxHandle = m_fxController->playFeedback(stim->getSender(), feedbackAction, m_actor->getRef());
                        m_fxController->setFXPos(fxHandle, stim->getFxPos());
                    }
                }
            }
        }
    }


    void Ray_JalapenoKingAiComponent::startFire()
    {
        const StringID startId = "Open";

        EventGeneric eventGen;
        eventGen.setSender(m_actor->getRef());
        eventGen.setId(startId);
        m_linkComponent->sendEventToChildren(&eventGen);
        m_actor->onEvent(&eventGen);
    }


    void Ray_JalapenoKingAiComponent::stopFire()
    {
        const StringID stopId = "Close";

        EventGeneric eventGen;
        eventGen.setSender(m_actor->getRef());
        eventGen.setId(stopId);
        m_linkComponent->sendEventToChildren(&eventGen);
        m_actor->onEvent(&eventGen);
    }

    void Ray_JalapenoKingAiComponent::onCheckpointLoaded()
    {
        Super::onCheckpointLoaded();
        if (m_dead)
        {
            m_actor->disable();
        }
    }



    //-------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(Ray_JalapenoKingAiComponent_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_JalapenoKingAiComponent_Template)
        SERIALIZE_OBJECT_WITH_FACTORY("deathReward", m_deathReward, ACTORSMANAGER->getEventFactory() );
    END_SERIALIZATION()

    Ray_JalapenoKingAiComponent_Template::Ray_JalapenoKingAiComponent_Template()
    :Super()
    ,m_deathReward(NULL)
    {
    }

    Ray_JalapenoKingAiComponent_Template::~Ray_JalapenoKingAiComponent_Template()
    {
        SF_DEL( m_deathReward );

    }
};
