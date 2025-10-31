#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_DISPENSERCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_DispenserComponent.h"
#endif //_ITF_RAY_DISPENSERCOMPONENT_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_ANIMATEDCOMPONENT_H_
#include "engine/actors/components/animatedcomponent.h"
#endif //_ITF_ANIMATEDCOMPONENT_H_

#ifndef _ITF_ANIMATIONTRACK_H_
#include "engine/animation/AnimationTrack.h"
#endif //_ITF_ANIMATIONTRACK_H_

#ifndef _ITF_ANIMLIGHTCOMPONENT_H_
#include "engine/actors/components/AnimLightComponent.h"
#endif //_ITF_ANIMLIGHTCOMPONENT_H_

#ifndef _ITF_EVENTS_H_
#include "engine/events/Events.h"
#endif //_ITF_EVENTS_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_PADRUMBLEMANAGER_H_
#include "gameplay/managers/PadRumbleManager.h"
#endif //_ITF_PADRUMBLEMANAGER_H_

#ifndef _ITF_PHANTOMCOMPONENT_H_
#include "gameplay/Components/Misc/PhantomComponent.h"
#endif //_ITF_PHANTOMCOMPONENT_H_

#ifndef _ITF_PLAYERDETECTORCOMPONENT_H_
#include "gameplay/Components/Trigger/PlayerDetectorComponent.h"
#endif //_ITF_PLAYERDETECTORCOMPONENT_H_

#ifndef _ITF_RAY_CHILDLAUNCHCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_ChildLaunchComponent.h"
#endif //_ITF_RAY_CHILDLAUNCHCOMPONENT_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_RAY_GAMEPLAYEVENTS_H_
#include "rayman/gameplay/Ray_GameplayEvents.h"
#endif //_ITF_RAY_GAMEPLAYEVENTS_H_

#ifndef SERIALIZEROBJECTBINARY_H
#include "core/serializer/ZSerializerObjectBinary.h"
#endif // SERIALIZEROBJECTBINARY_H

namespace ITF
{

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_DispenserComponent)
BEGIN_SERIALIZATION_CHILD(Ray_DispenserComponent)
    BEGIN_CONDITION_BLOCK(ESerializeGroup_Checkpoint)
        SERIALIZE_MEMBER("goodsRemaining",m_goodsRemaining);
        SERIALIZE_ENUM_BEGIN("state",m_state);
        SERIALIZE_ENUM_END();
    END_CONDITION_BLOCK()
END_SERIALIZATION()
BEGIN_VALIDATE_COMPONENT(Ray_DispenserComponent)
    VALIDATE_COMPONENT_PARAM("isChildLauncher,lowLifeEvent", !getTemplate()->getChildLauncher() || !getTemplate()->getLowLifeEvent(), "Incompatible");
    VALIDATE_COMPONENT_PARAM("animIdle", getTemplate()->getAnimIdle().isValid(), "Anim name mandatory");
    VALIDATE_COMPONENT_PARAM("animDispense", getTemplate()->getAnimDispense().isValid(), "Anim name mandatory");
    VALIDATE_COMPONENT_PARAM("animShrink", (getTemplate()->getGoodsCount()==0) || getTemplate()->getChildLauncher() || getTemplate()->getAnimShrink().isValid(), "Anim name mandatory");
    VALIDATE_COMPONENT_PARAM("animDone", (getTemplate()->getGoodsCount()==0) || getTemplate()->getChildLauncher() || getTemplate()->getAnimDone().isValid(), "Anim name mandatory");
    VALIDATE_COMPONENT_PARAM("dispenseEvent", getTemplate()->getChildLauncher() || getTemplate()->getDispenseEvent(), "Event mandatory");
    VALIDATE_COMPONENT_PARAM("", m_animatedComponent, "AnimatedComponent mandatory");
    VALIDATE_COMPONENT_PARAM("", !getTemplate()->getChildLauncher() || m_childLaunchComponent, "Ray_ChildLaunchComponent mandatory");
    VALIDATE_COMPONENT_PARAM("", !getTemplate()->getCanWiggle() || m_playerDetector, "PlayerDetectorComponent mandatory");
END_VALIDATE_COMPONENT()

//------------------------------------------------------------------------------
Ray_DispenserComponent::Ray_DispenserComponent()
    : Super()
    , m_state(State_NA)
    , m_goodsRemaining(0)
    , m_lowLifeGoodsRemaining(0)
    , m_dispenseCountdown(0.0f)
    , m_shakeCountdown(0.0f)
    , m_shakeFactor(0.0f)
    , m_ejectionAngleListShuffled()
    , m_ejectionAngleIndex(0)
    , m_dispenseEvent(NULL)
    , m_lowLifeEvent(NULL)
    , m_softCollision()
    , m_playersInsideCount(0)
    , m_animatedComponent(NULL)
    , m_phantomComponent(NULL)
    , m_childLaunchComponent(NULL)
    , m_playerDetector(NULL)
{
}

//------------------------------------------------------------------------------
Ray_DispenserComponent::~Ray_DispenserComponent()
{
    SF_DEL(m_dispenseEvent);
    SF_DEL(m_lowLifeEvent);
}

//------------------------------------------------------------------------------
void Ray_DispenserComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    // mandatory, validated
    m_animatedComponent = m_actor->GetComponent<AnimatedComponent>();
    if (m_animatedComponent)
    {
        m_animatedComponent->setUpdateAnimInput(this);
    }

    // optional
    m_phantomComponent = m_actor->GetComponent<PhantomComponent>();
    
    // mandatory if isChildLauncher specified
    m_childLaunchComponent = m_actor->GetComponent<Ray_ChildLaunchComponent>();

    // mandatory if canWiggle specified
    m_playerDetector = m_actor->GetComponent<PlayerDetectorComponent>();

    // goods count
    m_goodsRemaining = (getTemplate()->getGoodsCount() == 0)
        ? U32_INVALID // infinite
        : getTemplate()->getGoodsCount();
    m_lowLifeGoodsRemaining = (getTemplate()->getLowLifeGoodsMax() == 0)
        ? U32_INVALID // infinite
        : getTemplate()->getLowLifeGoodsMax();
    m_lowLifeGoodsRemaining = getTemplate()->getLowLifeGoodsMax();

    // mandatory if not a child launcher, validated
    if (getTemplate()->getDispenseEvent())
    {
        m_dispenseEvent = getTemplate()->getDispenseEvent()->CreateNewObject()->DynamicCast<Event>(ITF_GET_STRINGID_CRC(Event,772421353));
        BinaryClone(const_cast<Event*>(getTemplate()->getDispenseEvent()), m_dispenseEvent);
    }

    // not mandatory
    if (getTemplate()->getLowLifeEvent())
    {
        m_lowLifeEvent = getTemplate()->getLowLifeEvent()->CreateNewObject()->DynamicCast<Event>(ITF_GET_STRINGID_CRC(Event,772421353));
        BinaryClone(const_cast<Event*>(getTemplate()->getLowLifeEvent()), m_lowLifeEvent);
    }

    // init soft collision
    m_softCollision.init(&getTemplate()->getSoftCollision(), this);
    if (getTemplate()->getSoftCollisionDisabled())
    {
        m_softCollision.setDisabled(btrue);
    }

    // listen to punch stim & anim events
    if ( getTemplate()->getProcessStim() )
    {
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor, ITF_GET_STRINGID_CRC(PunchStim,200533519), this );
    }    
    else
    {
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor, ITF_GET_STRINGID_CRC(EventTrigger,1343042510), this );
    }

    // shuffle ejection angle (Knuth shuffle)
    u32 ejectionAngleCount = getTemplate()->getEjectionAngleList().size();
    if (ejectionAngleCount > 0)
    {
        m_ejectionAngleListShuffled = getTemplate()->getEjectionAngleList();
        for (int i=ejectionAngleCount; i>1; --i)
        {
            u32 index = Seeder::getSharedSeeder().GetU32(i-1);
            std::swap(m_ejectionAngleListShuffled[i-1], m_ejectionAngleListShuffled[index]);
        }
    }
}

//------------------------------------------------------------------------------
void Ray_DispenserComponent::onCheckpointLoaded()
{
    Super::onCheckpointLoaded();

    // force saved checkpoint state
    if (m_state != State_NA)
    {
        if (m_state == State_Shrink || m_state == State_Done)
        {
            changeState(State_Done, btrue);
        }
        else
        {
            changeState(State_Idle, btrue);
        }
    }
}

//------------------------------------------------------------------------------
void Ray_DispenserComponent::onEvent( Event* _event )
{
    Super::onEvent(_event);

    if (EventTrigger* triggerEvt = _event->DynamicCast<EventTrigger>(ITF_GET_STRINGID_CRC(EventTrigger,1343042510)))
    {
        processEventTrigger( triggerEvt );
    }
    if (PunchStim* stimEvt = _event->DynamicCast<PunchStim>(ITF_GET_STRINGID_CRC(PunchStim,200533519)))
    {
        processPunchStim( stimEvt );
    }

    // pass to soft collision    
    m_softCollision.onEvent(_event);
}

//------------------------------------------------------------------------------
void Ray_DispenserComponent::Update(f32 _dt)
{
    Super::Update(_dt);
    
    // start up
    if (m_state == State_NA)
    {
        changeState(State_Idle);
    }

    // update current state
    updateState(_dt);

    // shake less and less
    m_shakeCountdown = Max(0.0f, m_shakeCountdown-_dt);
    if (m_shakeFactor > 0.0f)
    {
        ITF_ASSERT(getTemplate()->getShakeDelay());
        m_shakeFactor = m_shakeCountdown / getTemplate()->getShakeDelay();
    }
    
    // update soft collision
    m_softCollision.update();
    
    // remember actors inside count
    if (m_playerDetector)
    {
        m_playersInsideCount = m_playerDetector->getActorsInside().size();
    }
}

//------------------------------------------------------------------------------
void Ray_DispenserComponent::updateAnimInput()
{
    ITF_ASSERT(m_animatedComponent);
    m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(ShakeFactor,3343377389), m_shakeFactor);
}

//------------------------------------------------------------------------------
void Ray_DispenserComponent::processEventTrigger( EventTrigger* _trigger )
{
    dispense( _trigger->getActivator(), bfalse );
}

//------------------------------------------------------------------------------
void Ray_DispenserComponent::processPunchStim( PunchStim* _stimEvent )
{
    ActorRef sender = _stimEvent->getSender();
    bbool all = ( ( _stimEvent->getLevel() > 1 ) || ( _stimEvent->getReceivedHitType() == RECEIVEDHITTYPE_EARTHQUAKE ) ) ? btrue : bfalse;
    if ( _stimEvent->getOriginalSender().isValid() )
    {
        sender = _stimEvent->getOriginalSender();
    }

    if ( dispense( sender, all ) )
    {
        // send hit successful event
        Actor * senderActor = AIUtils::getActor( _stimEvent->getSender() );
        ITF_ASSERT( senderActor != NULL );
        if ( senderActor != NULL )
        {
            EventHitSuccessful hitSuccess;
            hitSuccess.setSender( m_actor->getRef() );
            senderActor->onEvent( &hitSuccess );
        }
    }
}

//------------------------------------------------------------------------------
void Ray_DispenserComponent::changeState(State_t _newState, bbool _force /*= bfalse*/)
{
    if (!_force && m_state==_newState)
    {
        return;
    }

    switch (m_state = _newState)
    {
    case State_Idle:
    {
        // play idle anim
        m_animatedComponent->setAnim(getTemplate()->getAnimIdle());
    }
    break;
    case State_Wiggle:
    {
        // play wiggle anim
        m_animatedComponent->setAnim(getTemplate()->getAnimWiggle());

        // pad rumble
        ITF_ASSERT(m_playerDetector->getActorsInside().size() > 0);
        if (m_playerDetector->getActorsInside().size() > 0)
        {
            ActorRef playerRef = m_playerDetector->getActorsInside().back();
            if (Player* player = GAMEMANAGER->getPlayerFromActor(playerRef))
            {
                if (getTemplate()->getPadRumbleWiggle().isValid())
                {
                    PADRUMBLEMANAGER->startRumble(
                        getTemplate()->getPadRumbleWiggle(),
                        player->getIndex()); 
                }
            }
        }
    }
    break;
    case State_Dispense:
    {
        // play dispense anim
        m_animatedComponent->setAnim(getTemplate()->getAnimDispense());
        m_animatedComponent->resetCurTime(); // reset anim if already playing
        m_state = State_Dispense;

        // init dispense & shake countdown
        m_dispenseCountdown = getTemplate()->getDispenseDelay();
        m_shakeFactor = (m_shakeCountdown > 0.0f) ? 1.0f : 0.0f;
        m_shakeCountdown = getTemplate()->getShakeDelay();
    }
    break;
    case State_Shrink:
    {
        // shouldn't shrink with goods remaining
        ITF_ASSERT(!hasGoodsRemaining());

        // play shrink anim
        m_animatedComponent->setAnim(getTemplate()->getAnimShrink());
        m_state = State_Shrink;
        m_softCollision.setDisabled(btrue);

        // disable phantom (RO-2343)
        if (m_phantomComponent)
        {
            m_phantomComponent->setDisabled(btrue);
        }
    }
    break;
    case State_Done:
    {
        // can't be done with goods remaining
        ITF_ASSERT(!hasGoodsRemaining());

        if (getTemplate()->getDisableWhenDone())
        {
            // disable if nothing remains (optim for RO-16688)
            m_actor->disable();
        }
        else
        {
            // play done anim
            m_animatedComponent->setAnim(getTemplate()->getAnimDone());
        }
    }
    break;
    default: ITF_ASSERT_MSG(bfalse, "Unhandled state");
    }
}

//------------------------------------------------------------------------------
void Ray_DispenserComponent::updateState(f32 _dt)
{
    switch (m_state)
    {
    case State_Idle:
    {
        // wiggle when player detected
        if (getTemplate()->getCanWiggle() &&
            m_playerDetector &&
            m_playerDetector->getActorsInside().size() > m_playersInsideCount)
        {
            changeState(State_Wiggle);
        }    
    }
    break;
    case State_Wiggle:
    {
        // back to idle after wiggle
        if (m_animatedComponent->isMainNodeFinished())
        {
            changeState(State_Idle);
        }
    }
    break;
    case State_Dispense:
    {
        // update countdown
        m_dispenseCountdown = Max(0.0f, m_dispenseCountdown-_dt);

        // back to idle after dispense
        if (m_animatedComponent->isMainNodeFinished())
        {
            changeState(State_Idle);
        }
    }
    break;
    case State_Shrink:
    {
        // shouldn't shrink with goods remaining
        ITF_ASSERT(!hasGoodsRemaining());
    
        // done when shrunk
        if (m_animatedComponent->isMainNodeFinished())
        {
            changeState(State_Done);
        }
    }
    break;
    case State_Done:
    {
        // shouldn't be done with goods remaining
        ITF_ASSERT(!hasGoodsRemaining());
    }
    break;
    default: ITF_ASSERT_MSG(bfalse, "Unhandled state");
    }
}

//------------------------------------------------------------------------------
bbool Ray_DispenserComponent::dispense( const ActorRef & _sender, bbool _all )
{
    // ignore non-player hits
    if (!GAMEMANAGER->isPlayerActor( _sender ) && !getTemplate()->getAllowNonPlayerHits() )
    {
        return bfalse;
    }

    // meh, really, nothing more in stock
    // + warn if there was nothing to dispense at all
    if (!hasGoodsRemaining())
    {
        if (m_state == State_Idle)
        {
            if (getTemplate()->getChildLauncher())
            {
                ITF_WARNING(
                    m_actor, bfalse,
                    "Nothing to dispense, did you link to a bubble prize?");
            }
            else
            {
                ITF_ASSERT_MSG(
                    bfalse,
                    "Nothing to dispense, not a child launcher, something's wrong");
            }
            changeState(State_Shrink);
        }
        return bfalse;
    }

    // if already dispensing, check if minimum delay reached
    if (m_dispenseCountdown > 0.0f)
    {
        return bfalse;
    }

    if (getTemplate()->getChildLauncher())
    {
        // launch child(ren)
        ITF_ASSERT(m_childLaunchComponent);
        if ( _all )
        {
            m_childLaunchComponent->launchAll();
        }
        else
        {
            m_childLaunchComponent->launch();
        }
    }
    else
    {
        // "regular" dispense
        if (_all && m_goodsRemaining != U32_INVALID) // can't launch all if infinite...
        {
            while (hasGoodsRemaining())
            {
                dispenseOne(_sender);
            }
        }
        else
        {
            dispenseOne(_sender);
        }
    }
    
    // dispense or shrink
    if (hasGoodsRemaining())
    {
        changeState(State_Dispense);
    }
    else
    {
        changeState(State_Shrink);
    }

    return btrue;
}

//------------------------------------------------------------------------------
void Ray_DispenserComponent::dispenseOne(const ActorRef & _sender)
{
    // dispense low life event if possible & necessary
    Event* event = m_dispenseEvent;
    if (m_lowLifeEvent &&
        hasLowLifeGoodsRemaining() &&
        RAY_GAMEMANAGER->heartNeeded())
    {
        event = m_lowLifeEvent;
        if (m_lowLifeGoodsRemaining != U32_INVALID) // infinite
        {
            --m_lowLifeGoodsRemaining;
        }
    }

    // one less good
    if (m_goodsRemaining != U32_INVALID) // infinite
    {
        --m_goodsRemaining;
    }

    // rewards spawn handled by the game manager
    if (Ray_EventSpawnReward* reward = event->DynamicCast<Ray_EventSpawnReward>(ITF_GET_STRINGID_CRC(Ray_EventSpawnReward,145553727)))
    {
        // ejection angle
        Angle ejectionAngle(bfalse, 0.0f);
        u32 ejectionAngleCount = m_ejectionAngleListShuffled.size();
        if (ejectionAngleCount > 0)
        {
            ejectionAngle = m_ejectionAngleListShuffled[m_ejectionAngleIndex];
            m_ejectionAngleIndex = (m_ejectionAngleIndex + 1) % ejectionAngleCount;
        }

        // actually spawn reward
        reward->clear();
        reward->setup(m_actor, getTemplate()->getSpawnOffset());
        reward->setRewardReceiver( _sender );
        reward->setEjectionDir(Vec2d::Up.Rotate(m_actor->getAngle() + ejectionAngle.ToRadians()));
        reward->setEjectionGravityAngle(Angle(bfalse, m_actor->getAngle()));
        GAMEMANAGER->onEvent(reward);
    }
    // power ups handled by the player controller
    else if (Ray_EventPowerUp* powerUp = event->DynamicCast<Ray_EventPowerUp>(ITF_GET_STRINGID_CRC(Ray_EventPowerUp,875814764)))
    {
        ActorRef receiverRef = _sender;
        Actor* receiver = receiverRef.getActor();
        ITF_WARNING(m_actor, receiver, "Can't access receiver actor");
        if (receiver)
        {
            receiver->onEvent(powerUp);
        }
    }
    // not implemented: who should we send this event to?
    else
    {
        ITF_WARNING(m_actor, bfalse, "Unhandled event type");
    }
}

//------------------------------------------------------------------------------
bbool Ray_DispenserComponent::hasLowLifeGoodsRemaining() const
{
    if (getTemplate()->getChildLauncher())
    {
        return bfalse;
    }
    return ((m_lowLifeGoodsRemaining == U32_INVALID) || (m_lowLifeGoodsRemaining > 0));
}

//------------------------------------------------------------------------------
bbool Ray_DispenserComponent::hasGoodsRemaining() const
{
    if (getTemplate()->getChildLauncher())
    {
        ITF_ASSERT(m_childLaunchComponent);
        return m_childLaunchComponent->canLaunch();
    }
    return ((m_goodsRemaining == U32_INVALID) || (m_goodsRemaining > 0));
}

//------------------------------------------------------------------------------
#ifdef ITF_SUPPORT_EDITOR
void Ray_DispenserComponent::drawEdit(ActorDrawEditInterface* _drawInterface, u32 _flags) const
{
    Super::drawEdit(_drawInterface, _flags);

    // soft collision debug draw    
    m_softCollision.drawEdit(_drawInterface, _flags);
}


#endif // ITF_SUPPORT_EDITOR

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_DispenserComponent_Template)
BEGIN_SERIALIZATION(Ray_DispenserComponent_Template)
    SERIALIZE_MEMBER("goodsCount", m_goodsCount);
    SERIALIZE_MEMBER("lowLifeGoodsMax", m_lowLifeGoodsMax);
    SERIALIZE_MEMBER("lowLifeThreshold", m_lowLifeThreshold);
    SERIALIZE_MEMBER("dispenseDelay", m_dispenseDelay);
    SERIALIZE_MEMBER("shakeDelay", m_shakeDelay);
    SERIALIZE_MEMBER("spawnOffset", m_spawnOffset);
    SERIALIZE_CONTAINER("ejectionAngleList", m_ejectionAngleList);
    SERIALIZE_MEMBER("isChildLauncher", m_isChildLauncher);
    SERIALIZE_MEMBER("animIdle", m_animIdle);
    SERIALIZE_MEMBER("canWiggle", m_canWiggle);
    SERIALIZE_MEMBER("padRumbleWiggle", m_padRumbleWiggle);
    SERIALIZE_MEMBER("animWiggle", m_animWiggle);
    SERIALIZE_MEMBER("animDispense", m_animDispense);
    SERIALIZE_MEMBER("animShrink", m_animShrink);
    SERIALIZE_MEMBER("animDone", m_animDone);
    SERIALIZE_OBJECT_WITH_FACTORY("dispenseEvent", m_dispenseEvent, ACTORSMANAGER->getEventFactory());
    SERIALIZE_OBJECT_WITH_FACTORY("lowLifeEvent", m_lowLifeEvent, ACTORSMANAGER->getEventFactory());
    SERIALIZE_MEMBER("softCollisionDisabled", m_softCollisionDisabled);
    SERIALIZE_OBJECT("softCollision", m_softCollision);
    SERIALIZE_MEMBER("processStim", m_processStim );
    SERIALIZE_MEMBER("allowNonPlayerHits", m_allowNonPlayerHits );
    SERIALIZE_MEMBER("disableWhenDone", m_disableWhenDone);
END_SERIALIZATION()

//------------------------------------------------------------------------------
Ray_DispenserComponent_Template::Ray_DispenserComponent_Template()
    : Super()
    , m_goodsCount(0)
    , m_lowLifeGoodsMax(0)
    , m_lowLifeThreshold(1.0f)
    , m_dispenseDelay(0.0f)
    , m_shakeDelay(0.0f)
    , m_spawnOffset(Vec3d::Zero)
    , m_ejectionAngleList()
    , m_isChildLauncher(bfalse)
    , m_animIdle()
    , m_canWiggle(bfalse)
    , m_padRumbleWiggle()
    , m_animWiggle()
    , m_animDispense()
    , m_animShrink()
    , m_animDone()
    , m_dispenseEvent(NULL)
    , m_lowLifeEvent(NULL)
    , m_softCollisionDisabled(btrue)
    , m_softCollision()
    , m_processStim( btrue )
    , m_allowNonPlayerHits( bfalse )
    , m_disableWhenDone(bfalse)
{
}

//------------------------------------------------------------------------------
Ray_DispenserComponent_Template::~Ray_DispenserComponent_Template()
{
    SF_DEL(m_dispenseEvent);
    SF_DEL(m_lowLifeEvent);
}

} // namespace ITF
