#include "precompiled_gameplay.h"

#ifndef _ITF_FRIEZECONTACTDETECTORCOMPONENT_H_
#include "gameplay/Components/Trigger/FriezeContactDetectorComponent.h"
#endif //_ITF_FRIEZECONTACTDETECTORCOMPONENT_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(FriezeContactDetectorComponent)

    BEGIN_SERIALIZATION_CHILD(FriezeContactDetectorComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(FriezeContactDetectorComponent)
    VALIDATE_COMPONENT_PARAM("FriezeContactDetectorComponent::linkComponent",m_linkComponent,"FriezeContactDetectorComponent needs a link component");
    END_VALIDATE_COMPONENT()


    FriezeContactDetectorComponent::FriezeContactDetectorComponent()
        : m_linkComponent(NULL)
    {
    }

    FriezeContactDetectorComponent::~FriezeContactDetectorComponent()
    {
    }

    void FriezeContactDetectorComponent::onActorClearComponents()
    {
        clear();
    }

    void FriezeContactDetectorComponent::onActorLoaded( Pickable::HotReloadType _hotReload )
    {
        Super::onActorLoaded(_hotReload);

        clear();

        if (getTemplate()->getDetectHang())
        {
            ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventHanging,2826410601),this);
        }

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventStickOnPolyline,471231076),this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventOnLink,1558255516),this);

        m_linkComponent = m_actor->GetComponent<LinkComponent>();

        const LinkComponent::ChildrenList& children  = m_linkComponent->getChildren();
        for (u32 i = 0; i < children.size(); i++)
        {
            Pickable* pickable = SceneObjectPathUtils::getObjectFromRelativePath(m_actor,children[i].getPath());

            if ( pickable )
            {
                if (pickable->getObjectType() == BaseObject::eFrise)
                {
                    Frise * frise = (Frise*)pickable;
                    frise->addEventListener(m_actor->getRef());
                }
            }
        }

    }


    void FriezeContactDetectorComponent::onEvent( Event* _event )
    {
        Super::onEvent(_event);
        if (EventHanging * onHang = _event->DynamicCast<EventHanging>(ITF_GET_STRINGID_CRC(EventHanging,2826410601)))
        {
            ActorRef actorRef = onHang->getActor();
            if(onHang->isHanging())
            {
                checkAdd(actorRef);
            }
            else
            {
                checkRemove(actorRef);
            }
        }
        else if (EventStickOnPolyline * onStick = _event->DynamicCast<EventStickOnPolyline>(ITF_GET_STRINGID_CRC(EventStickOnPolyline,471231076)))
        {
            ActorRef actorRef = onStick->getActor();
            if (onStick->isSticked())
            {
                checkAdd(actorRef);
            }
            else
            {
                checkRemove(actorRef);
            }
        }
        else if (EventOnLink * onLink = _event->DynamicCast<EventOnLink>(ITF_GET_STRINGID_CRC(EventOnLink,1558255516)))
        {
            BaseObject * obj = onLink->getChild().getObject();
            if (obj->getObjectType() == BaseObject::eFrise)
            {
                Frise * frise = (Frise*)obj;
                if (onLink->getLinked())
                    frise->addEventListener(m_actor->getRef());
                else
                    frise->removeEventListener(m_actor->getRef());
            }

        }
    }

    void FriezeContactDetectorComponent::checkAdd( ActorRef &actorRef )
    {
        Actor * actor = actorRef.getActor();
        if (!actor)
            return;

        Faction factionToDetect = getTemplate()->getFactionToDetect();
        if ( factionToDetect != FACTION_UNKNOWN &&
            factionToDetect != AIUtils::getFaction(actor) )
        {
            return;
        }
        if (!getTemplate()->getAllowDeadActors())
        {
            EventQueryIsDead query;
            actor->onEvent(&query);
            if (query.getIsDead())
            {
                return;
            }
        }
        if (m_actorsInside.find(actorRef) == -1)
            m_actorsInside.push_back(actorRef);
    }

    void FriezeContactDetectorComponent::checkRemove( ActorRef actorRef )
    {
        i32 index = m_actorsInside.find(actorRef);
        if (index != -1)
            m_actorsInside.eraseNoOrder(index);
    }

    void FriezeContactDetectorComponent::clear()
    {
        if (m_linkComponent)
        {
            const LinkComponent::ChildrenList& children  = m_linkComponent->getChildren();
            for (u32 i = 0; i < children.size(); i++)
            {
                Pickable* pickable = SceneObjectPathUtils::getObjectFromRelativePath(m_actor,children[i].getPath());

                if ( pickable )
                {
                    if (pickable->getObjectType() == BaseObject::eFrise)
                    {
                        Frise * frise = (Frise*)pickable;
                        frise->removeEventListener(m_actor->getRef());
                    }
                }
            }
        }
    }

    //-------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(FriezeContactDetectorComponent_Template)
    BEGIN_SERIALIZATION(FriezeContactDetectorComponent_Template)
    SERIALIZE_MEMBER("factionToDetect", m_factionToDetect);
    SERIALIZE_MEMBER("allowDeadActors", m_allowDeadActors);
    SERIALIZE_MEMBER("detectHang", m_detectHang);

    END_SERIALIZATION()

    FriezeContactDetectorComponent_Template::FriezeContactDetectorComponent_Template()
    : m_factionToDetect(FACTION_UNKNOWN)    // unknown = don't filter
    , m_allowDeadActors(bfalse)
    , m_detectHang(btrue)
    {
    }
};