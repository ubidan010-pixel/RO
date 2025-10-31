#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_ACTIVATIONSEQUENCESTONECOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Stargate/Ray_ActivationSequenceStoneComponent.h"
#endif //_ITF_RAY_ACTIVATIONSEQUENCESTONECOMPONENT_H_

#ifndef _ITF_RAY_ACTIVATIONSEQUENCEVISUALCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Stargate/Ray_ActivationSequenceVisualComponent.h"
#endif //_ITF_RAY_ACTIVATIONSEQUENCEVISUALCOMPONENT_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/Managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_DETECTORCOMPONENT_H_
#include "gameplay/Components/Trigger/DetectorComponent.h"
#endif //_ITF_DETECTORCOMPONENT_H_

namespace ITF
{
IMPLEMENT_OBJECT_RTTI(Ray_ActivationSequenceStoneComponent)

BEGIN_VALIDATE_COMPONENT(Ray_ActivationSequenceStoneComponent)
    VALIDATE_COMPONENT_PARAM("", m_linkComponent, "Ray_ActivationSequenceStoneComponent requires a LinkComponent");
    VALIDATE_COMPONENT_PARAM("", m_animComponent, "Ray_ActivationSequenceVisualComponent requires an AnimatedComponent.");
END_VALIDATE_COMPONENT()

BEGIN_SERIALIZATION_CHILD(Ray_ActivationSequenceStoneComponent)
END_SERIALIZATION()


Ray_ActivationSequenceStoneComponent::Ray_ActivationSequenceStoneComponent()
: m_linkComponent(NULL)
, m_animComponent(NULL)
, m_fxRef(ITF_INVALID_OBJREF)
, m_fxBoneIndex(U32_INVALID)
, m_user(ITF_INVALID_OBJREF)
, m_visual(ITF_INVALID_OBJREF)
, m_activationCounter(0.f)
, m_activationFinished(bfalse)
{
}

Ray_ActivationSequenceStoneComponent::~Ray_ActivationSequenceStoneComponent()
{
}

void Ray_ActivationSequenceStoneComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    m_linkComponent = m_actor->GetComponent<LinkComponent>();

    if (m_linkComponent)
    {
        const LinkComponent::ChildrenList& childList  = m_linkComponent->getChildren();
        ITF_WARNING(m_actor, childList.size() != 0, "This activation stone is not link with a visual actor!");
        ITF_WARNING(m_actor, childList.size() <= 1, "This activation stone is linked with too many actors!");
        
        for (LinkComponent::ChildrenList::const_iterator it = childList.begin(); it != childList.end(); ++it)
        {
            const ObjectPath& objPath = it->getPath();
            Pickable* pickable = SceneObjectPathUtils::getObjectFromRelativePath(m_actor, objPath);

            if (pickable)
            {
                m_visual = pickable->getRef();
                break;
            }
        }
    }

    m_animComponent = m_actor->GetComponent<AnimatedComponent>();

    if ( m_animComponent )
    {
        m_animComponent->setUpdateAnimInput(this);
    }

    if (!getTemplate()->getFX().isEmpty())
    {
        SPAWNER->declareNeedsSpawnee(m_actor, &m_fxSpawn, getTemplate()->getFX());
    }

    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventQueryCanActivateStone,1176450728), this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventQueryEmptyStone,343141899), this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventActivateStone,2310010701), this);
}

void Ray_ActivationSequenceStoneComponent::Update( f32 _dt )
{
    Super::Update(_dt);

    f32 activationTime = getTemplate()->getActivationTime();

    if ( activationTime == m_activationCounter )
    {
        updateFx();
    }

    if ( m_activationFinished )
    {
        return;
    }

    if ( m_user.isValid() )
    {
        if ( m_activationCounter < activationTime )
        {
            m_activationCounter += _dt;

            if ( m_activationCounter >= activationTime )
            {
                m_activationCounter = activationTime;
                spawnFx();
            }
        }
    }

    // Update Visual
    Actor* actor = m_visual.getActor();
    Ray_ActivationSequenceVisualComponent* visualActorComponent = actor ? actor->GetComponent<Ray_ActivationSequenceVisualComponent>() : NULL;

    if (visualActorComponent)
    {
        visualActorComponent->setOccupiedState(m_actor->getRef(),m_user,getTemplate()->getActivationTime());
    }
}

void Ray_ActivationSequenceStoneComponent::spawnFx()
{
    if ( getTemplate()->getFX().isEmpty() )
    {
        return;
    }

    Vec3d fxPos;

    m_fxBoneIndex = m_animComponent ? m_animComponent->getBoneIndex(getTemplate()->getFXBone()) : U32_INVALID;

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

void Ray_ActivationSequenceStoneComponent::updateFx()
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

void Ray_ActivationSequenceStoneComponent::onEvent( Event * _event)
{
    Super::onEvent(_event);

    if ( Ray_EventQueryCanActivateStone* canActivateEvent = _event->DynamicCast<Ray_EventQueryCanActivateStone>(ITF_GET_STRINGID_CRC(Ray_EventQueryCanActivateStone,1176450728)) )
    {
        bbool canActivate;
        bbool isOccupied;

        if ( m_activationFinished )
        {
            canActivate = btrue;
            isOccupied = bfalse;
        }
        else
        {
            if ( m_user == ITF_INVALID_OBJREF || m_user == _event->getSender() )
            {
                canActivate = btrue;
                isOccupied = bfalse;
            }
            else
            {
                canActivate = bfalse;
                isOccupied = btrue;
            }
        }
        canActivateEvent->setCanActivate(canActivate);
        canActivateEvent->setIsOccupied(isOccupied);
    }
    else if ( Ray_EventQueryEmptyStone* queryEmpty = _event->DynamicCast<Ray_EventQueryEmptyStone>(ITF_GET_STRINGID_CRC(Ray_EventQueryEmptyStone,343141899)) )
    {
        processQueryEmpty(queryEmpty);
    }
    else if ( Ray_EventActivateStone* activateStoneEvent = _event->DynamicCast<Ray_EventActivateStone>(ITF_GET_STRINGID_CRC(Ray_EventActivateStone,2310010701)) )
    {
        activateStoneEvent->setReceived(btrue);
        m_user = activateStoneEvent->getSender();
    }
}

void Ray_ActivationSequenceStoneComponent::processQueryEmpty( Ray_EventQueryEmptyStone* _queryEmpty )
{
    Actor* actor = m_visual.getActor();
    Ray_ActivationSequenceVisualComponent* visualActorComponent = actor ? actor->GetComponent<Ray_ActivationSequenceVisualComponent>() : NULL;

    if (visualActorComponent)
    {
        actor->onEvent(_queryEmpty);
    }
}

void Ray_ActivationSequenceStoneComponent::updateAnimInput()
{
    m_animComponent->setInput(ITF_GET_STRINGID_CRC(Occupied,540639817),m_user.isValid());
    m_animComponent->setInput(ITF_GET_STRINGID_CRC(ActivationPercent,2736565110),m_activationCounter/getTemplate()->getActivationTime());
    m_animComponent->setInput(ITF_GET_STRINGID_CRC(Done,2685805802),m_activationFinished);
}

//-------------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_ActivationSequenceStoneComponent_Template)

BEGIN_SERIALIZATION(Ray_ActivationSequenceStoneComponent_Template)

    SERIALIZE_MEMBER("fx",m_fx);
    SERIALIZE_MEMBER("fxBone",m_fxBone);
    SERIALIZE_MEMBER("activationTime",m_activationTime);

END_SERIALIZATION()

Ray_ActivationSequenceStoneComponent_Template::Ray_ActivationSequenceStoneComponent_Template()
: m_activationTime(2.f)
{
}

};