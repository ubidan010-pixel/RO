#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AIBTORDERCOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_AIBTOrderComponent.h"
#endif //_ITF_RAY_AIBTORDERCOMPONENT_H_

#ifndef _ITF_DETECTORCOMPONENT_H_
#include "gameplay/Components/Trigger/DetectorComponent.h"
#endif //_ITF_DETECTORCOMPONENT_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

namespace ITF
{

///////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_OBJECT_RTTI(Ray_AIBTOrderComponent)
BEGIN_SERIALIZATION_CHILD(Ray_AIBTOrderComponent)
END_SERIALIZATION()

BEGIN_VALIDATE_COMPONENT(Ray_AIBTOrderComponent)
    VALIDATE_COMPONENT_PARAM("DetectorComponent", m_detector != NULL, "DetectorComponent is mandatory");
END_VALIDATE_COMPONENT()

Ray_AIBTOrderComponent::Ray_AIBTOrderComponent()
: m_detector(NULL)
, m_disabled(bfalse)
{
}

Ray_AIBTOrderComponent::~Ray_AIBTOrderComponent()
{
}

void Ray_AIBTOrderComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventDisableAIOrderBT,2752846623), this);

    m_detector = m_actor->GetComponent<DetectorComponent>();

    if ( getTemplate()->getType() != BTAIORDER_DETECTIONAREA )
    {
        m_order.setSender(m_actor->getRef());
        m_order.setType(getTemplate()->getType());
    }
    else
    {
        m_order.setType(BTAIORDER_UNKNOWN);
    }

    if ( getTemplate()->getDetectionArea().isValid() )
    {
        ObjectRef detectionAreaRef = ITF_INVALID_OBJREF;
        LinkComponent* linkComponent = m_actor->GetComponent<LinkComponent>();

        if ( linkComponent )
        {
            const LinkComponent::ChildrenList& children = linkComponent->getChildren();
            u32 numChildren = children.size();

            for ( u32 i = 0; i < numChildren; i++ )
            {
                const ChildEntry& child = children[i];
                const ObjectPath& path = child.getPath();
                Pickable* target;

                if ( path.getIsAbsolute() )
                {
                    target = SceneObjectPathUtils::getObjectFromAbsolutePath(path);
                }
                else
                {
                    target = SceneObjectPathUtils::getObjectFromRelativePath(m_actor,path);
                }

                if ( target )
                {
                    Actor* targetActor = target->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));

                    if ( targetActor )
                    {
                        DetectorComponent* detectComponent = targetActor->GetComponent<DetectorComponent>();

                        if ( detectComponent )
                        {
                            detectionAreaRef = targetActor->getRef();
                            break;
                        }
                    }
                }
            }
        }

        if ( detectionAreaRef == ITF_INVALID_OBJREF )
        {
            detectionAreaRef = m_actor->getRef();
        }

        if ( detectionAreaRef != ITF_INVALID_OBJREF )
        {
            m_detectionArea.setArea(detectionAreaRef);
            m_detectionArea.setId(getTemplate()->getDetectionArea());
            m_detectionArea.setSender(m_actor->getRef());
        }
    }
}

void Ray_AIBTOrderComponent::Update( f32 _dt )
{
    Super::Update(_dt);

    if ( !m_disabled )
    {
        sendOrders();
    }
}

void Ray_AIBTOrderComponent::sendOrders()
{
    const ActorRefList& actorsInside = m_detector->getActorsInside();
    u32 numInside = actorsInside.size();
    bbool detectionArea = m_detectionArea.getId().isValid();
    bbool hasOrder = m_order.getType() != BTAIORDER_UNKNOWN;

    m_detectionArea.setIsSet( btrue );

    for ( u32 i = 0; i < numInside; i++ )
    {
        ActorRef refInside = actorsInside[i];

        if ( m_actorsInside.find(refInside) == -1 )
        {
            Actor* actorInside = refInside.getActor();

            if ( actorInside )
            {
                if ( hasOrder )
                {
                    actorInside->onEvent(&m_order);
                }

                if ( detectionArea )
                {
                    actorInside->onEvent(&m_detectionArea);
                }
            }
        }
    }

    u32 numBefore = m_actorsInside.size();
    bbool removeOnExit = getTemplate()->getRemoveOnExit();
    Ray_EventAIOrderBT orderBt;

    orderBt.setSender(m_actor->getRef());
    orderBt.setType(BTAIORDER_REMOVECURRENTORDER);

    m_detectionArea.setIsSet(bfalse);

    for ( u32 i = 0; i < numBefore; i++ )
    {
        ActorRef refInsideBefore = m_actorsInside[i];

        if ( actorsInside.find(refInsideBefore) == -1 )
        {
            Actor* actorOutside = refInsideBefore.getActor();

            if ( !actorOutside )
            {
                continue;
            }

            if ( detectionArea )
            {
                actorOutside->onEvent(&m_detectionArea);
            }

            if ( removeOnExit )
            {
                actorOutside->onEvent(&orderBt);
            }
        }
    }

    m_actorsInside = actorsInside;
}

void Ray_AIBTOrderComponent::setDisabled( bbool _val )
{
    if ( m_disabled != _val )
    {
        m_disabled = _val;

        if ( m_disabled )
        {
            u32 numActors = m_actorsInside.size();
            Ray_EventDisableAIOrderBT disableOrder(m_actor->getRef(),btrue);

            for ( u32 i = 0; i < numActors; i++ )
            {
                Actor* actor = m_actorsInside[i].getActor();

                if ( actor )
                {
                    actor->onEvent(&disableOrder);
                }
            }

            m_actorsInside.clear();
        }
    }
}

void Ray_AIBTOrderComponent::onEvent( Event* _event )
{
    if ( Ray_EventDisableAIOrderBT* disableOrder = _event->DynamicCast<Ray_EventDisableAIOrderBT>(ITF_GET_STRINGID_CRC(Ray_EventDisableAIOrderBT,2752846623)) )
    {
        setDisabled(disableOrder->getDisable());
    }
}

///////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_OBJECT_RTTI(Ray_AIBTOrderComponent_Template)
BEGIN_SERIALIZATION(Ray_AIBTOrderComponent_Template)

    SERIALIZE_RAY_BTAIORDERTYPE("type",m_type);
    SERIALIZE_MEMBER("detectionArea",m_detectionArea);
    SERIALIZE_MEMBER("removeOnExit",m_removeOnExit);

END_SERIALIZATION()

Ray_AIBTOrderComponent_Template::Ray_AIBTOrderComponent_Template()
: m_type(BTAIORDER_UNKNOWN)
, m_removeOnExit(bfalse)
{
}

Ray_AIBTOrderComponent_Template::~Ray_AIBTOrderComponent_Template()
{
}

} // namespace ITF

