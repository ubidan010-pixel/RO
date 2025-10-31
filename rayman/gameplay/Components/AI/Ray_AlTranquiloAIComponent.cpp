#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_ALTRANQUILOAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_alTranquiloAiComponent.h"
#endif //_ITF_RAY_ALTRANQUILOAICOMPONENT_H_

#ifndef _ITF_RAY_BUBBLEAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_BubbleAiComponent.h"
#endif //_ITF_RAY_BUBBLEAICOMPONENT_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_RAY_GEYSERPLATFORMAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_GeyserPlatformAIComponent.h"
#endif //_ITF_RAY_GEYSERPLATFORMAICOMPONENT_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_alTranquiloAiComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_alTranquiloAiComponent)
        BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
            SERIALIZE_MEMBER("bubbleLifetime", m_bubbleLifetime);
        END_CONDITION_BLOCK()
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(Ray_alTranquiloAiComponent)
        VALIDATE_COMPONENT_PARAM_CATEGORY(GPP, "", m_mode != Mode_None, "AlTranquilo must emit either bubbles or a geyser");
    END_VALIDATE_COMPONENT()


    Ray_alTranquiloAiComponent::Ray_alTranquiloAiComponent()
        : Super()
        , m_state(State_Idle)
        , m_staySquashedTimer(0.0f)
        , m_fastScale(bfalse)
        , m_mode(Mode_None)
        , m_respawnTimer(0.f)
        , m_bubbleLifetime(10.f)
        , m_geyser(NULL)
        , m_spawnBoneIndex(U32_INVALID)
    {
    }

    Ray_alTranquiloAiComponent::~Ray_alTranquiloAiComponent()
    {
    }

    void Ray_alTranquiloAiComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventStickOnPolyline,471231076),this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(PunchStim,200533519),this);

        m_geyser = m_actor->GetComponent<Ray_GeyserPlatformAIComponent>();

        m_stickedActors.clear();

        if (!getTemplate()->getBubblePath().isEmpty())
        {
            m_mode = Mode_Bubble;
            SPAWNER->declareNeedsSpawnee(m_actor, &m_bubbleSpawner , getTemplate()->getBubblePath());
        }
        else if (m_geyser)
        {
            m_mode = Mode_Geyser;
        }

        if (m_mode != Mode_None)
        {
            startIdle();
        }
    }

    void Ray_alTranquiloAiComponent::onResourceReady()
    {
        Super::onResourceReady();

        m_spawnBoneIndex = m_animatedComponent->getBoneIndex(getTemplate()->getSpawnBone());
    }

    void Ray_alTranquiloAiComponent::Update( f32 _dt )
    {
        Super::Update(_dt);

        if (m_state == State_Idle)
        {
            updateIdle();
        }
        else if (m_state == State_Squashed)
        {
            updateSquashed(_dt);
        }

        switch (m_mode)
        {
        case Mode_Bubble:
            update_bubble(_dt);
            break;
        }
    }

    void Ray_alTranquiloAiComponent::onEvent( Event* _event )
    {
        Super::onEvent(_event);

        if ( EventStickOnPolyline* stickEvent = _event->DynamicCast<EventStickOnPolyline>(ITF_GET_STRINGID_CRC(EventStickOnPolyline,471231076)) )
        {
            processStick(stickEvent);
        }
        else if (PunchStim* punch = _event->DynamicCast<PunchStim>(ITF_GET_STRINGID_CRC(PunchStim,200533519)))
        {
            processPunch(punch);
        }
    }

    void Ray_alTranquiloAiComponent::processStick( EventStickOnPolyline* stickEvent )
    {
        if ( stickEvent->isSticked() )
        {
            ActorRef stickedActorRef = stickEvent->getActor();
            if (GAMEMANAGER->isPlayerActor(stickedActorRef))
            {
                if ( Actor* stickedActor = stickedActorRef.getActor() )
                {
                    Ray_EventQueryAIData query;
                    stickedActor->onEvent(&query);

                    if (getTemplate()->getMini())
                    {
                        if (m_stickedActors.find(stickedActorRef) == -1)
                        {
                            m_stickedActors.push_back(stickedActorRef);
                        }

                        if (!query.getReducedSize())
                        {
                            m_fastScale = btrue;
                        }
                    }
                    else
                    {
                        if (!query.getReducedSize())
                        {
                            if (m_stickedActors.find(stickedActorRef) == -1)
                            {
                                m_stickedActors.push_back(stickedActorRef);
                            }
                        }

                        m_fastScale = bfalse;
                    }
                }
            }
        }
        else
        {
            i32 index = m_stickedActors.find(stickEvent->getActor());
            if ( index != -1 )
            {
                m_stickedActors.eraseNoOrder(index);
            }
        }
    }

    void Ray_alTranquiloAiComponent::processPunch( PunchStim* punch )
    {
        if ( punch->getReceivedHitType() != RECEIVEDHITTYPE_EARTHQUAKE )
        {
            return;
        }

        if ( !AIUtils::isEnemyHit(punch, getFaction()) )
        {
            return;
        }



        m_fastScale = btrue;

        if ( m_state == State_Idle )
        {
            startSquashed();
        }
        else
        {
            startResquash();
        }

        sendBounce(punch->getSender());
    }

    void Ray_alTranquiloAiComponent::sendBounce( ActorRef _otherActor )
    {
        if ( Actor* otherActor = _otherActor.getActor() )
        {
            switch (m_mode)
            {
            case Mode_Bubble:
                sendBounce_bubble(otherActor);
                break;

            case Mode_Geyser:
                sendBounce_geyser(otherActor);
                break;
            }
        }
    }

    void Ray_alTranquiloAiComponent::sendBounce_bubble( Actor* _otherActor ) const
    {
        Vec2d bounceOffset = getBouncePos_bubble() - m_actor->get2DPos(); // Ray_EventBounceToLayer requires a target actor, and just adds the offset
        Ray_EventBounceToLayer bounceStim(0.f, 0.f, 0.f,
            m_actor->getRef(), bounceOffset, 0);

        bounceStim.setSender(m_actor->getRef());
        _otherActor->onEvent(&bounceStim);
    }

    void Ray_alTranquiloAiComponent::sendBounce_geyser( Actor* _otherActor ) const
    {
        BounceStim bounceStim;
        bounceStim.setSender(m_actor->getRef());
        bounceStim.setBounceType(getTemplate()->getBounceType());
        bounceStim.setBounceDir(Vec2d::Right.Rotate(getTemplate()->getBounceAngle().Radians()));
        bounceStim.setIsRadial(bfalse);
        bounceStim.setMultiplier(1.f);

        bounceStim.setSender(m_actor->getRef());
        _otherActor->onEvent(&bounceStim);
    }



    Vec2d Ray_alTranquiloAiComponent::getBouncePos() const
    {
        switch (m_mode)
        {
        case Mode_Bubble:
            return getBouncePos_bubble();

        case Mode_Geyser:
            return getBouncePos_geyser();
        }

        ITF_ASSERT(0);
        return Vec2d::Zero;
    }

    void Ray_alTranquiloAiComponent::startIdle()
    {
        m_state = State_Idle;
        onRelease();
        m_animatedComponent->setAnim(getTemplate()->getIdleAnim());
    }

    void Ray_alTranquiloAiComponent::startSquashed()
    {
        m_state = State_Squashed;
        onSquash();
        m_staySquashedTimer = getTemplate()->getStaySquashedTime();
        m_animatedComponent->setAnim(getTemplate()->getSquashedAnim());
    }

    void Ray_alTranquiloAiComponent::startResquash()
    {
        onResquash();
        m_staySquashedTimer = getTemplate()->getStaySquashedTime();
        // TODO: reset squash anim, or play special re-squash anim
        m_animatedComponent->setAnim(getTemplate()->getSquashedAnim());
    }

    void Ray_alTranquiloAiComponent::updateIdle()
    {
        if (m_stickedActors.size() > 0)
        {
            startSquashed();
        }
    }

    void Ray_alTranquiloAiComponent::updateSquashed(f32 _dt)
    {
        if (m_stickedActors.size() == 0)
        {
            m_staySquashedTimer = Max(0.0f,m_staySquashedTimer-_dt);
            if (m_staySquashedTimer == 0.0f)
            {
                startIdle();
            }
            else
            {
                m_animatedComponent->setAnim(getTemplate()->getStaySquashedAnim());
            }
        }
        else
        {
            if (getTemplate()->getAutoBounce())
            {
                // NOTE: it's a bit dirty to do it like that, but if we bounce onStick we miss some crush attacks
                for (u32 i = 0; i < m_stickedActors.size(); i++)
                {
                    sendBounce(m_stickedActors[i]);
                }
            }

            m_staySquashedTimer = getTemplate()->getStaySquashedTime();

            m_animatedComponent->setAnim(getTemplate()->getSquashedAnim());
        }
    }



    void Ray_alTranquiloAiComponent::onRelease()
    {
        ITF_ASSERT(m_mode != Mode_None);
        switch (m_mode)
        {
        case Mode_Bubble:
            onRelease_bubble();
            break;

        case Mode_Geyser:
            onRelease_geyser();
            break;
        }

        m_fastScale = bfalse;
    }

    void Ray_alTranquiloAiComponent::onSquash()
    {
        ITF_ASSERT(m_mode != Mode_None);
        switch (m_mode)
        {
        case Mode_Bubble:
            onSquash_bubble();
            break;

        case Mode_Geyser:
            onSquash_geyser();
            break;
        }
    }

    void Ray_alTranquiloAiComponent::onResquash()
    {
        ITF_ASSERT(m_mode != Mode_None);
        switch (m_mode)
        {
        case Mode_Bubble:
            onResquash_bubble();
            break;

        //case Mode_Geyser:
        //    onResquash_geyser();
        //    break;
        }
    }


    // Mode_Bubble

    void Ray_alTranquiloAiComponent::onSquash_bubble()
    {
        spawnBubble();
    }

    void Ray_alTranquiloAiComponent::onResquash_bubble()
    {
        if ( Actor* bubbleActor = m_currentBubble.getActor() )
        {
            Ray_BubbleAiComponent* bubble = bubbleActor->GetComponent<Ray_BubbleAiComponent>();
            bubble->setScale(m_fastScale);
        }
        else
        {
            spawnBubble();
        }
    }

    void Ray_alTranquiloAiComponent::onRelease_bubble()
    {
        // nothing to do, the bubble will tell us when it's done
    }

    // sent by the spawned bubble
    void Ray_alTranquiloAiComponent::onBubbleReleased( ActorRef _bubble )
    {
        ITF_ASSERT(_bubble == m_currentBubble);
        m_currentBubble.invalidate();
    }

    void Ray_alTranquiloAiComponent::spawnBubble()
    {
        ITF_ASSERT(m_bubbleSpawner.isInitialized());
        if (!m_bubbleSpawner.isInitialized())
        {
            return;
        }


        Vec3d pos;
        m_animatedComponent->getBonePos(m_spawnBoneIndex, pos, bfalse);

        if ( Actor* bubble = m_bubbleSpawner.getSpawnee(m_actor->getScene(), pos) )
        {
            m_currentBubble = bubble->getRef();

            Ray_BubbleAiComponent* bubbleComponent = bubble->GetComponent<Ray_BubbleAiComponent>();
            bubbleComponent->setOwner(m_actor->getRef());
            bubbleComponent->setScale(m_fastScale);
            bubbleComponent->setLifetime(m_bubbleLifetime);
        }

        m_respawnTimer = getTemplate()->getRespawnTime();
    }

    void Ray_alTranquiloAiComponent::update_bubble( const f32 _dt )
    {
        if (!getTemplate()->getRespawnTime())
        {
            // auto-spawn disabled
            return;
        }

        if (m_state == State_Squashed)
        {
            if (m_stickedActors.size())
            {
                m_respawnTimer -= _dt;
                if (m_respawnTimer < 0)
                {
                    spawnBubble();
                }
            }
        }
    }

    Vec2d Ray_alTranquiloAiComponent::getBouncePos_bubble() const
    {
        const Transform2d& transform = getWorldTransform2d(m_actor);
        return transform.transformPos( getTemplate()->getBouncePos() );
    }


    // Mode_Geyser

    void Ray_alTranquiloAiComponent::onSquash_geyser()
    {
        m_geyser->open();
    }

    void Ray_alTranquiloAiComponent::onRelease_geyser()
    {
        m_geyser->close();
    }

    Vec2d Ray_alTranquiloAiComponent::getBouncePos_geyser() const
    {
        const Transform2d& geyserTransform = m_geyser->getWorldTransform2d();
        return geyserTransform.transformPos( getTemplate()->getBouncePos() );
    }

#ifdef ITF_SUPPORT_EDITOR
    void Ray_alTranquiloAiComponent::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        Super::drawEdit(_drawInterface, _flags);

        if (_flags != ActorComponent::DrawEditFlag_All)
            return;

        DebugDraw::circle(getBouncePos(), m_actor->getDepth(), 0.1f, Color::white(), 0, "bounce target");
    }
#endif // ITF_SUPPORT_EDITOR

    //-------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(Ray_alTranquiloAiComponent_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_alTranquiloAiComponent_Template)
        SERIALIZE_MEMBER("idleAnim",m_idleAnim);
        SERIALIZE_MEMBER("squashedAnim",m_squashedAnim);
        SERIALIZE_MEMBER("staySquashedAnim",m_staySquashedAnim);
        SERIALIZE_MEMBER("staySquashedTime",m_staySquashedTime);
        SERIALIZE_MEMBER("autoBounce",m_autoBounce);
        SERIALIZE_MEMBER("bouncePos",m_bouncePos);
        SERIALIZE_MEMBER("isMini",m_isMini);
        SERIALIZE_MEMBER("bubblePath",m_bubblePath);
        SERIALIZE_MEMBER("spawnBone",m_spawnBone);
        SERIALIZE_MEMBER("respawnTime",m_respawnTime);
        SERIALIZE_MEMBER("bounceAngle",m_bounceAngle);
        SERIALIZE_ENUM_BEGIN("bounceType",m_bounceType);
        SERIALIZE_ENUM_END();
    END_SERIALIZATION()

    Ray_alTranquiloAiComponent_Template::Ray_alTranquiloAiComponent_Template()
        : m_autoBounce(bfalse)
        , m_bouncePos(5, 5)
        , m_staySquashedTime(1.0f)
        , m_isMini(bfalse)
        , m_respawnTime(1.0f)
        , m_bounceAngle(btrue,80.0f)
        , m_bounceType(BOUNCETYPE_BUMPER)
    {
    }

};
