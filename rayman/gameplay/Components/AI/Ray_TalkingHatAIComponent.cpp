#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_TALKINGHATAICOMPONENT_H_
#include "rayman/gameplay/Components/ai/Ray_TalkingHatAIComponent.h"
#endif //_ITF_RAY_TALKINGHATAICOMPONENT_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_RAY_ANIMATIONMARKERS_H_
#include "rayman/gameplay/Ray_AnimationMarkers.h"
#endif //_ITF_RAY_ANIMATIONMARKERS_H_

#ifndef _ITF_RAY_TALKINGBUBBLECOMPONENT_H_
#include "rayman/gameplay/Components/ai/Ray_TalkingBubbleComponent.h"
#endif //_ITF_RAY_TALKINGBUBBLECOMPONENT_H_

#ifndef _ITF_LOCALISATIONMANAGER_H_
#include "engine/localisation/LocalisationManager.h"
#endif //_ITF_LOCALISATIONMANAGER_H_

#ifndef _ITF_MUSICMANAGER_H_
#include "engine/gameplay/musicmanager.h"
#endif //_ITF_MUSICMANAGER_H_

#include "engine/AdaptersInterfaces/AudioMiddlewareAdapter.h"

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(Ray_TalkingHatAIComponent)
BEGIN_SERIALIZATION_CHILD(Ray_TalkingHatAIComponent)

    BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
        SERIALIZE_CONTAINER("sentences",m_sentences);
    END_CONDITION_BLOCK()

END_SERIALIZATION()

Ray_TalkingHatAIComponent::Ray_TalkingHatAIComponent()
: m_animatedComponent(NULL)
, m_bounceTriggered(bfalse)
, m_visible(bfalse)
, m_bounce(bfalse)
, m_showText(bfalse)
, m_pointingArrow(ARROWSTATE_HIDDEN)
, m_bubbleRef(ITF_INVALID_OBJREF)
, m_bubbleBoneIndex(U32_INVALID)
{
}

Ray_TalkingHatAIComponent::~Ray_TalkingHatAIComponent()
{
}

void Ray_TalkingHatAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    ResourceGroup *rs = m_actor->getResourceGroup();

    m_animatedComponent = m_actor->GetComponent<AnimatedComponent>();

    if (m_animatedComponent)
    {
        m_animatedComponent->setUpdateAnimInput(this);
    }

    if ( !getTemplate()->getBubblePath().isEmpty() )
    {
        SPAWNER->declareNeedsSpawnee(m_actor, &m_bubbleSpawner, getTemplate()->getBubblePath());

        Actor* bubbleActor = m_bubbleSpawner.getSpawnee(m_actor->getScene(),m_actor->getPos(),0.f);

        if ( bubbleActor )
        {
            m_bubbleRef = bubbleActor->getRef();

            Ray_TalkingBubbleComponent* bubble = bubbleActor->GetComponent<Ray_TalkingBubbleComponent>();

            if ( bubble )
            {
                u32 numSentences = m_sentences.size();

                for ( u32 i = 0; i < numSentences; i++ )
                {
                    Path audioPath = LOCALISATIONMANAGER->getAudioPath(m_sentences[i]);
                    if (!audioPath.isEmpty())
                    {
#ifdef  ITF_SUPPORT_WWISE
                        ResourceID mpR = rs->addResource(Resource::ResourceType_MusicPart, audioPath);
                        const StringID wwiseEventGuid = AUDIO_ADAPTER->getLocalizationWWiseGUIdFromLocId(m_sentences[i].value);
                        MusicPart* musicPart = (MusicPart*)mpR.getResource();
                        if (musicPart)
                            musicPart->setWwiseEventGuid(wwiseEventGuid);
#else
                        rs->addResource(Resource::ResourceType_MusicPart, audioPath);
#endif
                    }
                }

                bubble->setSentences(m_sentences);
            }

            bubbleActor->disable();
        }
    }

    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventBlockedByPolyline,2950236768),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventStickOnPolyline,471231076),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventPolylineBlockingContact,1992742979),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(PunchStim,200533519),this);
}

void Ray_TalkingHatAIComponent::onResourceReady()
{
    Super::onResourceReady();

    if ( getTemplate()->getBubbleBone().isValid() )
    {
        m_bubbleBoneIndex = m_animatedComponent->getBoneIndex(getTemplate()->getBubbleBone());
    }
}

Ray_TalkingBubbleComponent* Ray_TalkingHatAIComponent::getBubbleComponent() const
{
    Actor* bubbleActor = m_bubbleRef.getActor();

    if ( bubbleActor )
    {
        return bubbleActor->GetComponent<Ray_TalkingBubbleComponent>();
    }

    return NULL;
}

void Ray_TalkingHatAIComponent::Update( f32 _dt )
{
    Super::Update(_dt);

    if ( !m_animatedComponent->isInTransition() && !m_bounce )
    {
        if ( m_pointingArrow == ARROWSTATE_HIDDEN ||
             m_pointingArrow == ARROWSTATE_IDLE )
        {
            updateDetectPlayer();
        }

        if ( m_bounceTriggered )
        {
            if ( m_visible )
            {
                processBounce();
            }
            else
            {
                m_bounceTriggered = bfalse;
            }
        }
        else if ( m_visible && m_pointingArrow == ARROWSTATE_HIDDEN )
        {
            Ray_TalkingBubbleComponent* bubble = getBubbleComponent();

            if ( bubble && bubble->isHidden() )
            {
                m_pointingArrow = ARROWSTATE_APPEAR;
            }
        }
    }

    updateBubble();
}

void Ray_TalkingHatAIComponent::updateDetectPlayer()
{
    bbool playerFound = bfalse;
    const u32 playerCount = GAMEMANAGER->getMaxPlayerCount();
    f32 distSq = m_visible ? getTemplate()->getPlayerDetectRadiusDisappear() : getTemplate()->getPlayerDetectRadiusAppear();
    const DepthRange depthRange(m_actor->getDepth());

    distSq *= distSq;

    for ( u32 i = 0; i < playerCount; ++i )
    {
        Player* player = GAMEMANAGER->getPlayer(i);

        if (player->getActive() && !player->isDead())
        {
            Actor* playerActor = player->getCurrentActor().getActor();

            if ( playerActor )
            {
                if ( depthRange == DepthRange(playerActor->getDepth()) )
                {
                    f32 distToPlayerSq = ( m_actor->get2DPos() - playerActor->get2DPos() ).sqrnorm();

                    if ( distToPlayerSq < distSq )
                    {
                        playerFound = btrue;
                        break;
                    }
                }
            }
        }
    }

    if ( playerFound != m_visible )
    {
        if ( playerFound )
        {
            setAppear();
        }
        else
        {
            setDisappear();
        }
    }
    else
    {
        Ray_TalkingBubbleComponent* bubble = getBubbleComponent();

        if ( bubble )
        {
            if ( !bubble->isHidden() )
            {
                bubble->GetActor()->enable();
            }
            else
            {
                bubble->GetActor()->disable();
            }
        }
    }
}

void Ray_TalkingHatAIComponent::updateBubble()
{
    Actor* bubble = m_bubbleRef.getActor();

    if ( bubble )
    {
        if ( m_bubbleBoneIndex != U32_INVALID )
        {
            Vec3d bonePos;
            Vec2d boneOrientation;

            if ( m_animatedComponent->getBonePos(m_bubbleBoneIndex,bonePos) )
            {
                if ( bubble->getTemplate()->isZForced() )
                {
                    f32 sceneZ = AIUtils::getSceneZFromPickable(bubble);

                    bonePos.m_z = sceneZ + bubble->getTemplate()->getForcedZ() + getTemplate()->getBubbleZOffset();
                }
                else
                {
                    bonePos.m_z = m_actor->getPos().m_z + getTemplate()->getBubbleZOffset();
                }

                bubble->setPos(bonePos);
            }

            if ( m_animatedComponent->getBoneOrientation(m_bubbleBoneIndex,boneOrientation) )
            {
                Ray_TalkingBubbleComponent* bubbleC = getBubbleComponent();

                if ( bubbleC )
                {
                    f32 angle = boneOrientation.getAngleOpt();

                    NormalizeAnglePI(angle);

                    f32 t = fabsf(angle)/MTH_PI;

                    bubbleC->setBubbleHeight(t);
                }
            }
        }
    }
}

void Ray_TalkingHatAIComponent::processBounce()
{
    Ray_TalkingBubbleComponent* bubble = getBubbleComponent();

    if ( bubble )
    {
        bubble->GetActor()->enable();

        if ( bubble->isHidden() )
        {
            bubble->setAppear();
        }
        else
        {
            bubble->showNextSentence();
        }

        if ( m_pointingArrow == ARROWSTATE_IDLE ||
             m_pointingArrow == ARROWSTATE_APPEAR )
        {
            m_pointingArrow = ARROWSTATE_DISAPPEAR;
        }
    }

    m_bounce = btrue;
    m_bounceTriggered = bfalse;
    m_animatedComponent->resetCurTime();
}

void Ray_TalkingHatAIComponent::onEvent( Event * _event)
{
    Super::onEvent(_event);

    if ( EventBlockedByPolyline* blockedEvent = _event->DynamicCast<EventBlockedByPolyline>(ITF_GET_STRINGID_CRC(EventBlockedByPolyline,2950236768)) )
    {
        m_bounceTriggered = btrue;
    }
    else if ( EventPolylineBlockingContact* block = _event->DynamicCast<EventPolylineBlockingContact>(ITF_GET_STRINGID_CRC(EventPolylineBlockingContact,1992742979)) )
    {
        m_bounceTriggered = btrue;
    }
    else if ( EventStickOnPolyline* _stickedEvent = _event->DynamicCast<EventStickOnPolyline>(ITF_GET_STRINGID_CRC(EventStickOnPolyline,471231076)) )
    {
        if ( _stickedEvent->isSticked() )
        {
            m_bounceTriggered = btrue;
        }
    }
    else if ( PunchStim* punch = _event->DynamicCast<PunchStim>(ITF_GET_STRINGID_CRC(PunchStim,200533519)) )
    {
        m_bounceTriggered = btrue;
    }
    else if ( AnimGameplayEvent* animEvent = _event->DynamicCast<AnimGameplayEvent>(ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301)) )
    {
        if ( animEvent->getName() == ITF_GET_STRINGID_CRC(MRK_Bounce_Finish,1890149707) )
        {
            m_bounce = bfalse;
        }
        else if ( animEvent->getName() == ITF_GET_STRINGID_CRC(MRK_PointingArrow_Appear,2791060617) )
        {
            m_pointingArrow = ARROWSTATE_IDLE;
        }
        else if ( animEvent->getName() == ITF_GET_STRINGID_CRC(MRK_PointingArrow_Disappear,970665747) )
        {
            m_pointingArrow = ARROWSTATE_HIDDEN;
        }
    }
}

void Ray_TalkingHatAIComponent::setAppear()
{
    if ( !m_visible )
    {
        m_visible = btrue;
    }
}

void Ray_TalkingHatAIComponent::setDisappear()
{
    m_visible = bfalse;

    if ( m_pointingArrow != ARROWSTATE_HIDDEN &&
         m_pointingArrow != ARROWSTATE_DISAPPEAR )
    {
        m_pointingArrow = ARROWSTATE_DISAPPEAR;
    }

    Ray_TalkingBubbleComponent* bubble = getBubbleComponent();

    if ( bubble )
    {
        bubble->setDisappear();
    }
}

void Ray_TalkingHatAIComponent::updateAnimInput()
{
    m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(visible,3088086709),m_visible);
    m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(bounce,421809396),m_bounce);
    m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(pointingArrowState,1201942587),static_cast<u32>(m_pointingArrow));
}

///////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_OBJECT_RTTI(Ray_TalkingHatAIComponent_Template)
BEGIN_SERIALIZATION(Ray_TalkingHatAIComponent_Template)

    SERIALIZE_MEMBER("playerDetectRadiusAppear",m_playerDetectRadiusAppear);
    SERIALIZE_MEMBER("playerDetectRadiusDisappear",m_playerDetectRadiusDisappear);
    SERIALIZE_MEMBER("bubbleBone",m_bubbleBone);
    SERIALIZE_MEMBER("bubblePath",m_bubbleAct);
    SERIALIZE_MEMBER("bubbleZOffset",m_bubbleZOffset);

END_SERIALIZATION()

Ray_TalkingHatAIComponent_Template::Ray_TalkingHatAIComponent_Template()
: m_playerDetectRadiusAppear(5.f)
, m_playerDetectRadiusDisappear(8.f)
, m_bubbleZOffset(0.f)
{
}

Ray_TalkingHatAIComponent_Template::~Ray_TalkingHatAIComponent_Template()
{
}

}
