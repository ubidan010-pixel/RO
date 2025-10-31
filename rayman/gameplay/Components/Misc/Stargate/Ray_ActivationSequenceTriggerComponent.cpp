#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_ACTIVATIONSEQUENCETRIGGERCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Stargate/Ray_ActivationSequenceTriggerComponent.h"
#endif //_ITF_RAY_ACTIVATIONSEQUENCETRIGGERCOMPONENT_H_

#ifndef _ITF_RAY_ACTIVATIONSEQUENCEVISUALCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Stargate/Ray_ActivationSequenceVisualComponent.h"
#endif //_ITF_RAY_ACTIVATIONSEQUENCEVISUALCOMPONENT_H_

#ifndef _ITF_RAY_ACTIVATIONSEQUENCESTONECOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Stargate/Ray_ActivationSequenceStoneComponent.h"
#endif //_ITF_RAY_ACTIVATIONSEQUENCESTONECOMPONENT_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_DETECTORCOMPONENT_H_
#include "gameplay/Components/Trigger/DetectorComponent.h"
#endif //_ITF_DETECTORCOMPONENT_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

namespace ITF
{
IMPLEMENT_OBJECT_RTTI(Ray_ActivationSequenceTriggerComponent)

BEGIN_VALIDATE_COMPONENT(Ray_ActivationSequenceTriggerComponent)
    VALIDATE_COMPONENT_PARAM("", !getTemplate()->getCanActivate() || m_detectorComponent, "Ray_ActivationSequenceTriggerComponent needs a detector component in order to activate itself");
    VALIDATE_COMPONENT_PARAM("AnimComponent",m_animComponent,"Ray_ActivationSequenceTriggerComponent needs an animated component");
END_VALIDATE_COMPONENT()

BEGIN_SERIALIZATION_CHILD(Ray_ActivationSequenceTriggerComponent)
END_SERIALIZATION()


Ray_ActivationSequenceTriggerComponent::Ray_ActivationSequenceTriggerComponent()
: m_detectorComponent(NULL)
, m_animComponent(NULL)
, m_fxRef(ITF_INVALID_OBJREF)
, m_fxBoneIndex(U32_INVALID)
, m_activated(bfalse)
, m_activationFinished(bfalse)
, m_done(bfalse)
{
}

Ray_ActivationSequenceTriggerComponent::~Ray_ActivationSequenceTriggerComponent()
{
}

void Ray_ActivationSequenceTriggerComponent::onActorLoaded(Pickable::HotReloadType _loadType)
{
    Super::onActorLoaded(_loadType);

    m_detectorComponent = m_actor->GetComponent<DetectorComponent>();
    m_animComponent = m_actor->GetComponent<AnimatedComponent>();

    if ( m_animComponent )
    {
        m_animComponent->setUpdateAnimInput(this);
    }

    if (!getTemplate()->getFX().isEmpty())
    {
        SPAWNER->declareNeedsSpawnee(m_actor, &m_fxSpawn, getTemplate()->getFX());
    }


    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventQueryEmptyStone,343141899), this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301),this);
}

void Ray_ActivationSequenceTriggerComponent::Update( f32 _dt )
{
    Super::Update(_dt);

    if ( m_activated )
    {
        if ( m_activationFinished && !m_done )
        {
            processActivation();
            m_done = btrue;
        }

        updateFx();
        return;
    }

    if ( getTemplate()->getCanActivate() )
    {
        checkCharacterActivate();
    }

    updateSlotsList();
    updateSlotsActivation(_dt);
}

void Ray_ActivationSequenceTriggerComponent::checkCharacterActivate()
{
    if ( !m_detectorComponent )
    {
        return;
    }

    const ActorRefList& actorsInside = m_detectorComponent->getActorsInside();

    u32 numInside = actorsInside.size();

    for ( u32 i = 0; i < numInside; i++ )
    {
        ActorRef insideRef = actorsInside[i];
        i32 reservedIndex = findSlotInfoFromReserved(insideRef);

        if ( reservedIndex == -1 )
        {
            Actor* actorInside = insideRef.getActor();

            if ( actorInside )
            {
                bbool isPlayer = GAMEMANAGER->isPlayerActor(insideRef);

                if ( ( getTemplate()->getIsNPC() && !isPlayer ) ||
                     ( !getTemplate()->getIsNPC() && isPlayer ) )
                {
                    u32 freeSlotInfo = findSlotInfoFree();

                    if ( freeSlotInfo != U32_INVALID )
                    {
                        SlotInfo& slot = m_slotsInfo[freeSlotInfo];

                        Ray_EventActivateStone activateStone(m_actor->getRef(),slot.m_stone);
                        actorInside->onEvent(&activateStone);

                        if ( activateStone.isReceived() )
                        {
                            slot.m_reserved = insideRef;
                        }
                    }
                }
            }
        }
    }
}

void Ray_ActivationSequenceTriggerComponent::processActivation()
{
    u32 numSlots = m_slotsInfo.size();

    for ( u32 i = 0; i < numSlots; i++ )
    {
        SlotInfo& info = m_slotsInfo[i];

        Actor* stoneActor = info.m_stone.getActor();

        if ( stoneActor )
        {
            Ray_ActivationSequenceStoneComponent* stone = stoneActor->GetComponent<Ray_ActivationSequenceStoneComponent>();

            if ( stone )
            {
                stone->setActivationFinished();
            }
        }

        Actor* visualActor = info.m_visual.getActor();

        if ( visualActor )
        {
            Ray_ActivationSequenceVisualComponent* visual = visualActor->GetComponent<Ray_ActivationSequenceVisualComponent>();

            if ( visual )
            {
                visual->setActivationFinished();
            }
        }

        Actor* user = info.m_user.getActor();

        if ( user )
        {
            Ray_EventStoneActivationFinished finishedEvent(m_actor->getRef(),info.m_stone);

            user->onEvent(&finishedEvent);
        }
    }

    EventTrigger trigger;

    m_actor->onEvent(&trigger);

    LinkComponent* linkC = m_actor->GetComponent<LinkComponent>();

    const LinkComponent::ChildrenList& children = linkC->getChildren();
    u32 numChildren = children.size();

    for ( u32 i = 0; i < numChildren; i++ )
    {
        const ObjectPath& objPath = children[i].getPath();
        Pickable* pickable;

        if ( objPath.getIsAbsolute() )
        {
            pickable = SceneObjectPathUtils::getObjectFromAbsolutePath(objPath);
        }
        else
        {
            pickable = SceneObjectPathUtils::getObjectFromRelativePath(m_actor,objPath);
        }

        if ( pickable )
        {
            Actor* child = pickable->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));

            if ( child )
            {
                child->onEvent(&trigger);
            }
        }
    }

    spawnFx();
}

void Ray_ActivationSequenceTriggerComponent::updateSlotsList()
{
    u32 numSlots = m_slotsInfo.size();

    for ( u32 i = 0; i < numSlots; i++ )
    {
        m_slotsInfo[i].m_valid = bfalse;
    }

    ActorBindHandler & bindHandler = m_actor->getChildrenBindHandler();
    const ObjectRefList&  children = bindHandler.getChildren();
    u32 numChildren = children.size();

    for ( u32 i = 0; i < numChildren; i++ )
    {
        ObjectRef childRef = children[i];
        u32 index = findSlotInfoFromVisual(childRef);

        if ( index == U32_INVALID )
        {
            Actor* childActor = AIUtils::getActor(childRef);

            if ( childActor )
            {
                Ray_ActivationSequenceVisualComponent* activationVisual = childActor->GetComponent<Ray_ActivationSequenceVisualComponent>();

                if ( activationVisual )
                {
                    SlotInfo info;
                    info.m_visual = childRef;
                    info.m_stone = activationVisual->getStone();
                    info.m_user = activationVisual->getUser();
                    info.m_reserved = info.m_user;
                    info.m_valid = btrue;
                    info.m_isDisabled = btrue;
                    m_slotsInfo.push_back(info);
                }
            }
        }
        else
        {
            Actor* visualActor = m_slotsInfo[index].m_visual.getActor();

            if ( visualActor )
            {
                SlotInfo& slotInfo = m_slotsInfo[index];

                slotInfo.m_valid = btrue;

                Ray_ActivationSequenceVisualComponent* activationVisual = visualActor->GetComponent<Ray_ActivationSequenceVisualComponent>();

                if ( activationVisual )
                {
                    slotInfo.m_stone = activationVisual->getStone();
                    slotInfo.m_user = activationVisual->getUser();
                    slotInfo.m_isDisabled = activationVisual->isDisabled();
                }
            }
        }
    }

    u32 slotIndex = 0;
    numSlots = m_slotsInfo.size();

    while ( slotIndex < numSlots )
    {
        if ( !m_slotsInfo[slotIndex].m_valid )
        {
            m_slotsInfo.eraseNoOrder(slotIndex);
            numSlots--;
        }
        else
        {
            slotIndex++;
        }
    }
}

void Ray_ActivationSequenceTriggerComponent::updateSlotsActivation( f32 _dt )
{
    u32 numSlots = m_slotsInfo.size();

    if ( numSlots )
    {
        m_activated = btrue;

        for ( u32 i = 0; i < numSlots; i++ )
        {
            SlotInfo& slotInfo = m_slotsInfo[i];
            Actor* visualActor = slotInfo.m_visual.getActor();

            if ( visualActor )
            {
                Ray_ActivationSequenceVisualComponent* activationVisual = visualActor->GetComponent<Ray_ActivationSequenceVisualComponent>();

                if ( activationVisual && !activationVisual->isActivated() )
                {
                    m_activated = bfalse;
                }
            }
        }
    }
}

u32 Ray_ActivationSequenceTriggerComponent::findSlotInfoFromVisual( ActorRef _visual ) const
{
    u32 numSlots = m_slotsInfo.size();

    for ( u32 i = 0; i < numSlots; i++ )
    {
        if ( m_slotsInfo[i].m_visual == _visual )
        {
            return i;
        }
    }

    return U32_INVALID;
}

u32 Ray_ActivationSequenceTriggerComponent::findSlotInfoFromReserved( ActorRef _reserved ) const
{
    u32 numSlots = m_slotsInfo.size();

    for ( u32 i = 0; i < numSlots; i++ )
    {
        if ( m_slotsInfo[i].m_reserved == _reserved )
        {
            return i;
        }
    }

    return U32_INVALID;
}

u32 Ray_ActivationSequenceTriggerComponent::findSlotInfoFree() const
{
    u32 numSlots = m_slotsInfo.size();

    for ( u32 i = 0; i < numSlots; i++ )
    {
        const SlotInfo& slotInfo = m_slotsInfo[i];

        if ( slotInfo.m_reserved == ITF_INVALID_OBJREF &&
             slotInfo.m_user == ITF_INVALID_OBJREF &&
             slotInfo.m_stone.isValid() &&
             slotInfo.m_visual.isValid() &&
             !slotInfo.m_isDisabled)
        {
            return i;
        }
    }

    return U32_INVALID;
}

void Ray_ActivationSequenceTriggerComponent::spawnFx()
{
    if ( getTemplate()->getFX().isEmpty() )
    {
        return;
    }

    Vec3d fxPos;

    m_fxBoneIndex = m_animComponent->getBoneIndex(getTemplate()->getFXBone());

    if ( m_fxBoneIndex != U32_INVALID )
    {
        Vec2d bonePos;
        m_animComponent->getBonePos(m_fxBoneIndex,bonePos);
        fxPos = bonePos.to3d(m_actor->getPos().m_z);
    }
    else
    {
        fxPos = m_actor->getPos();
    }

    Actor* fx = m_fxSpawn.getSpawnee(m_actor->getScene(), fxPos);

    if (fx)
    {
        if (fx->getScene()->getWorld()->getIgnoreCells())
        {
            fx->loadResources();
        }
        fx->enable();
        m_fxRef = fx->getRef();
        m_actor->getBinding()->bindChild(m_fxRef);
    }
}

void Ray_ActivationSequenceTriggerComponent::updateFx()
{
    Actor* fx = m_fxRef.getActor();

    if (fx)
    {
        Vec3d fxPos = m_actor->getPos();

        if ( m_fxBoneIndex != U32_INVALID )
        {
            Vec2d bonePos;
            m_animComponent->getBonePos(m_fxBoneIndex,bonePos);
            fxPos = bonePos.to3d(m_actor->getPos().m_z);
        }
        else
        {
            fxPos = m_actor->getPos();
        }

        fx->setPos(fxPos);
    }
}

void Ray_ActivationSequenceTriggerComponent::onEvent( Event * _event)
{
    Super::onEvent(_event);

    if ( Ray_EventQueryEmptyStone* queryEmpty = _event->DynamicCast<Ray_EventQueryEmptyStone>(ITF_GET_STRINGID_CRC(Ray_EventQueryEmptyStone,343141899)) )
    {
        processQueryEmpty(queryEmpty);
    }
    else if ( AnimGameplayEvent * animTrackEvent = _event->DynamicCast<AnimGameplayEvent>(ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301)) )
    {
        processAnimEvent(animTrackEvent);
    }
}

void Ray_ActivationSequenceTriggerComponent::processQueryEmpty( Ray_EventQueryEmptyStone* _queryEmpty ) const
{
    u32 numSlots = m_slotsInfo.size();

    for ( u32 i = 0; i < numSlots; i++ )
    {
        const SlotInfo& slotInfo = m_slotsInfo[i];

        if ( slotInfo.m_user == ITF_INVALID_OBJREF )
        {
            _queryEmpty->setEmptyStone(slotInfo.m_stone);
            break;
        }
    }
}

void Ray_ActivationSequenceTriggerComponent::processAnimEvent( AnimGameplayEvent* _animEvent )
{
    if ( _animEvent->getName() == ITF_GET_STRINGID_CRC(MRK_Activation_Finished,190730898) )
    {
        m_activationFinished = btrue;
    }
}

void Ray_ActivationSequenceTriggerComponent::updateAnimInput()
{
    m_animComponent->setInput(ITF_GET_STRINGID_CRC(activated,2250205201),m_activated);
}

//-------------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_ActivationSequenceTriggerComponent_Template)

BEGIN_SERIALIZATION(Ray_ActivationSequenceTriggerComponent_Template)

    SERIALIZE_MEMBER("fx",m_fx);
    SERIALIZE_MEMBER("fxBone",m_fxBone);
    SERIALIZE_MEMBER("canActivate",m_canActivate);
    SERIALIZE_MEMBER("npc",m_npc);

END_SERIALIZATION()

Ray_ActivationSequenceTriggerComponent_Template::Ray_ActivationSequenceTriggerComponent_Template()
: m_canActivate(btrue)
, m_npc(btrue)
{
}

};