#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_BOSSMAPENDAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Boss/Ray_BossMapEndAIComponent.h"
#endif //_ITF_RAY_BOSSMAPENDAICOMPONENT_H_

#ifndef _ITF_RAY_AIUTILS_H_
#include "Rayman/gameplay/AI/Utils/Ray_AIUtils.h"
#endif //_ITF_RAY_AIUTILS_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_


namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_BossMapEndAIComponent)

        BEGIN_SERIALIZATION_CHILD(Ray_BossMapEndAIComponent)
        END_SERIALIZATION()

        BEGIN_VALIDATE_COMPONENT(Ray_BossMapEndAIComponent)
        END_VALIDATE_COMPONENT()

    Ray_BossMapEndAIComponent::Ray_BossMapEndAIComponent()
        : Super()
    {
    }

    Ray_BossMapEndAIComponent::~Ray_BossMapEndAIComponent()
    {
    }

    void Ray_BossMapEndAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTrigger,1343042510), this);
    }

    void Ray_BossMapEndAIComponent::onEvent(Event* _event)
    {
        Super::onEvent(_event);
        if(EventTrigger* _evt = _event->DynamicCast<EventTrigger>(ITF_GET_STRINGID_CRC(EventTrigger,1343042510)))
        {
            ActorRef actor = getCinematic();
            if(actor != ActorRef::InvalidRef)
            {
                Ray_AIUtils::showBossMedal(actor , m_actor->getRef());
            }
        }
    }

    ActorRef Ray_BossMapEndAIComponent::getCinematic()
    {
        ActorRef actor;
        LinkComponent *linkComp = m_actor->GetComponent<LinkComponent>();
        if (linkComp)
        {
            const LinkComponent::ChildrenList &children = linkComp->getChildren();
            if(children.size() > 0)
            {
                Pickable *p= SceneObjectPathUtils::getObjectFromRelativePath(m_actor, children[0].getPath());
                if (p) actor = p->getRef();
            }
        }
        return actor;
    }

    //-------------------------------------------------------------------------------------

    IMPLEMENT_OBJECT_RTTI(Ray_BossMapEndAIComponent_Template);
    BEGIN_SERIALIZATION_CHILD(Ray_BossMapEndAIComponent_Template);
    END_SERIALIZATION();

    Ray_BossMapEndAIComponent_Template::Ray_BossMapEndAIComponent_Template()
        : Super()
    {
    }
};