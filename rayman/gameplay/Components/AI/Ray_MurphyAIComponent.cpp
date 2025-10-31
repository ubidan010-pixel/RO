#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_MURPHYAICOMPONENT_H_
#include "rayman/gameplay/Components/ai/Ray_MurphyAIComponent.h"
#endif //_ITF_RAY_MURPHYAICOMPONENT_H_

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

IMPLEMENT_OBJECT_RTTI(Ray_MurphyAIComponent)
BEGIN_SERIALIZATION_CHILD(Ray_MurphyAIComponent)

    BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
        SERIALIZE_CONTAINER("sentences",m_sentences);
    END_CONDITION_BLOCK()

END_SERIALIZATION()

Ray_MurphyAIComponent::Ray_MurphyAIComponent()
: m_animatedComponent(NULL)
, m_bubbleRef(ITF_INVALID_OBJREF)
, m_bubbleBoneIndex(U32_INVALID)
, m_appearRequested(bfalse)
, m_disappearRequested(bfalse)
, m_state(State_Idle)
{
}

Ray_MurphyAIComponent::~Ray_MurphyAIComponent()
{
}

void Ray_MurphyAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    m_animatedComponent = m_actor->GetComponent<AnimatedComponent>();

    if (m_animatedComponent)
    {
        m_animatedComponent->setUpdateAnimInput(this);
    }
    ResourceGroup *rs = m_actor->getResourceGroup();

    if ( !getTemplate()->getBubblePath().isEmpty() )
    {
        SPAWNER->declareNeedsSpawnee(m_actor, &m_bubbleSpawner, getTemplate()->getBubblePath());

        Actor* bubbleActor = m_bubbleSpawner.getSpawnee(m_actor->getScene(),m_actor->getPos(),m_actor->getAngle());

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
        }
    }

    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTrigger,1343042510),this);

    setState(State_Idle);

}

void Ray_MurphyAIComponent::onResourceReady()
{
    Super::onResourceReady();

    if ( getTemplate()->getBubbleBone().isValid() )
    {
        m_bubbleBoneIndex = m_animatedComponent->getBoneIndex(getTemplate()->getBubbleBone());
    }
}

Ray_TalkingBubbleComponent* Ray_MurphyAIComponent::getBubbleComponent() const
{
    Actor* bubbleActor = m_bubbleRef.getActor();

    if ( bubbleActor )
    {
        return bubbleActor->GetComponent<Ray_TalkingBubbleComponent>();
    }

    return NULL;
}

void Ray_MurphyAIComponent::Update( f32 _dt )
{
    Super::Update(_dt);

    if (m_state == State_Idle)
    {
        if (m_appearRequested)
        {
            m_appearRequested = bfalse;
            setState(State_Appear);
        }
        if (m_disappearRequested)
            m_disappearRequested = bfalse;
    }
    else if (m_state == State_Appear)
    {
        if (getTemplate()->getAppearAnim().isValid() && AIUtils::isAnimFinished(NULL,m_animatedComponent))
        {
            setState(State_Talk);
        }
    }
    else if (m_state == State_Talk)
    {
        if (m_appearRequested)
            m_appearRequested = bfalse;
        if (getBubbleComponent())
        {
            if (getBubbleComponent()->isDoneTalking())
            {
                if (m_disappearRequested)
                {
                    m_disappearRequested = bfalse;
                    setState(State_Disappear);
                }
            }
        }
    }
    else if (m_state == State_Disappear)
    {
        if (getTemplate()->getDisappearAnim().isValid() && AIUtils::isAnimFinished(NULL,m_animatedComponent))
        {
            m_actor->disable();
        }
    }
    else
    {
        ITF_ASSERT(0);
    }


    updateBubble();
}

void Ray_MurphyAIComponent::updateBubble()
{
    Actor* bubble = m_bubbleRef.getActor();

    if ( bubble )
    {
        if ( m_bubbleBoneIndex != U32_INVALID )
        {
            Vec3d bonePos;

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
        }
    }
}

void Ray_MurphyAIComponent::onEvent( Event * _event)
{
    Super::onEvent(_event);
    if (EventTrigger * onTrigger = _event->DynamicCast<EventTrigger>(ITF_GET_STRINGID_CRC(EventTrigger,1343042510)))
    {
        if (onTrigger->getActivated())
            m_appearRequested = btrue;
        else
            m_disappearRequested = btrue;
    }
}



void Ray_MurphyAIComponent::updateAnimInput()
{
 }

void Ray_MurphyAIComponent::setState( State _state )
{
    m_state = _state;
    if (_state == State_Idle)
    {
        m_animatedComponent->setAnim(getTemplate()->getIdleAnim());
    }
    else if (_state == State_Appear)
    {
        Ray_TalkingBubbleComponent* bubble = getBubbleComponent();

        if ( bubble && bubble->isHidden())
        {
            bubble->setAppear();
        }
        if (getTemplate()->getAppearAnim().isValid())
            m_animatedComponent->setAnim(getTemplate()->getAppearAnim());
        else
            setState(State_Talk);

    }
    else if (_state == State_Talk)
    {
        m_animatedComponent->setAnim(getTemplate()->getTalkAnim());
    }
    else if (_state == State_Disappear)
    {
        Ray_TalkingBubbleComponent* bubble = getBubbleComponent();

        if ( bubble && !bubble->isHidden())
        {
            bubble->setDisappear();
        }
        if (getTemplate()->getDisappearAnim().isValid())
            m_animatedComponent->setAnim(getTemplate()->getDisappearAnim());
        else
            setState(State_Idle);
    }
    else
    {
        ITF_ASSERT(0);
    }

}

void Ray_MurphyAIComponent::onActorClearComponents()
{
    Super::onActorClearComponents();
    Actor* bubbleActor = m_bubbleRef.getActor();
    if (bubbleActor)
    {
        bubbleActor->requestDestruction();
        m_bubbleRef.invalidate();
    }

}

///////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_OBJECT_RTTI(Ray_MurphyAIComponent_Template)
BEGIN_SERIALIZATION(Ray_MurphyAIComponent_Template)

    SERIALIZE_MEMBER("playerDetectRadiusAppear",m_playerDetectRadiusAppear);
    SERIALIZE_MEMBER("playerDetectRadiusDisappear",m_playerDetectRadiusDisappear);
    SERIALIZE_MEMBER("bubbleBone",m_bubbleBone);
    SERIALIZE_MEMBER("bubblePath",m_bubbleAct);
    SERIALIZE_MEMBER("bubbleZOffset",m_bubbleZOffset);
    SERIALIZE_MEMBER("appearAnim",m_appearAnim);
    SERIALIZE_MEMBER("talkAnim",m_talkAnim);
    SERIALIZE_MEMBER("disappearAnim",m_disappearAnim);
    SERIALIZE_MEMBER("idleAnim",m_idleAnim);

END_SERIALIZATION()

Ray_MurphyAIComponent_Template::Ray_MurphyAIComponent_Template()
: m_playerDetectRadiusAppear(5.f)
, m_playerDetectRadiusDisappear(8.f)
, m_bubbleZOffset(0.f)
{
}

Ray_MurphyAIComponent_Template::~Ray_MurphyAIComponent_Template()
{
}

}
