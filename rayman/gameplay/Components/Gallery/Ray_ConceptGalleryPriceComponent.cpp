#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_CONCEPTGALLERYPRICECOMPONENT_H_
#include "rayman/gameplay/Components/Gallery/Ray_ConceptGalleryPriceComponent.h"
#endif //_ITF_RAY_CONCEPTGALLERYPRICECOMPONENT_H_

#ifndef _ITF_RAY_CONCEPTGALLERYCOMPONENT_H_
#include "rayman/gameplay/Components/Gallery/Ray_ConceptGalleryComponent.h"
#endif //_ITF_RAY_CONCEPTGALLERYCOMPONENT_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_UITEXTBOXESCOMPONENT_H_
#include "gameplay/components/UI/UITextBoxesComponent.h"
#endif //_ITF_UITEXTBOXESCOMPONENT_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/Scene.h"
#endif //_ITF_SCENE_H_

namespace ITF
{
//---------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_ConceptGalleryPriceComponent)

BEGIN_SERIALIZATION_CHILD(Ray_ConceptGalleryPriceComponent)
END_SERIALIZATION()

BEGIN_VALIDATE_COMPONENT(Ray_ConceptGalleryPriceComponent)
    VALIDATE_COMPONENT_PARAM("AnimComponent", m_animComponent != NULL, "Needs an anim component");
    VALIDATE_COMPONENT_PARAM("TextComponent", m_textComponent != NULL, "Needs a text component");
END_VALIDATE_COMPONENT()

Ray_ConceptGalleryPriceComponent::Ray_ConceptGalleryPriceComponent()
: m_animComponent(NULL)
, m_textComponent(NULL)
, m_parentConceptComponent(NULL)
{
}

Ray_ConceptGalleryPriceComponent::~Ray_ConceptGalleryPriceComponent()
{
}

void Ray_ConceptGalleryPriceComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    m_animComponent = m_actor->GetComponent<AnimLightComponent>();
    m_textComponent = m_actor->GetComponent<UITextBoxesComponent>();

    getTemplate()->getMedalPath().getString(m_medalPath);

    // Find and cache parent concept actor through LinkComponent
    if (m_actor)
    {
        Scene* scene = m_actor->getScene();
        if (scene)
        {
            // Use ObjectPath to find parent - search through scene's actor list
            // We'll find it by checking LinkComponents in the scene
            // For now, we'll find it in Update() since we need to iterate scene differently
            m_parentConceptComponent = NULL;
        }
    }
}

void Ray_ConceptGalleryPriceComponent::Update( f32 _dt )
{
    Super::Update(_dt);

    if (!m_actor)
    {
        return;
    }

    // Find parent concept component if not cached yet
    if (!m_parentConceptComponent)
    {
        // Find the concept actor that has this price actor as a linked child
        Scene* scene = m_actor->getScene();
        if (!scene)
        {
            return;
        }

        ObjectId thisActorId = m_actor->getObjectId();

        // Get all actors from scene
        const PickableList& actors = scene->getActors();
        for (PickableList::const_iterator it = actors.begin(); it != actors.end(); ++it)
        {
            Pickable* pickable = *it;
            if (!pickable)
            {
                continue;
            }

            // Check if this is an Actor (using ObjectType, no RTTI needed)
            // Pickable inherits from BaseObject, so we can check ObjectType directly
            BaseObject* baseObj = static_cast<BaseObject*>(pickable);
            if (!baseObj || baseObj->getObjectType() != BaseObject::eActor)
            {
                continue;
            }

            Actor* candidateActor = static_cast<Actor*>(baseObj);
            if (!candidateActor || candidateActor == m_actor)
            {
                continue;
            }

            LinkComponent* linkComp = candidateActor->GetComponent<LinkComponent>();
            if (!linkComp)
            {
                continue;
            }

            // Check if this actor is linked as a child
            const LinkComponent::ChildrenList& children = linkComp->getChildren();
            for (LinkComponent::ChildrenList::const_iterator childIt = children.begin(); childIt != children.end(); ++childIt)
            {
                BaseObject* childObj = linkComp->getChildObject(childIt->getPath());
                if (childObj && childObj->getObjectId() == thisActorId)
                {
                    // Found parent! Check if it has Ray_ConceptGalleryComponent
                    Ray_ConceptGalleryComponent* conceptComp = candidateActor->GetComponent<Ray_ConceptGalleryComponent>();
                    if (conceptComp)
                    {
                        m_parentConceptComponent = conceptComp;
                        break;
                    }
                }
            }

            if (m_parentConceptComponent)
            {
                break;
            }
        }

        // If still not found, return early - will try again next frame
        if (!m_parentConceptComponent)
        {
            return;
        }
    }

    u32 requiredMedals = m_parentConceptComponent->getMedalRequirement();
    u32 currentMedals = RAY_GAMEMANAGER->computeCompleteMedalCount();
    if (requiredMedals > 0)
    {
        // Display format: "current/total [medal icon]"
        m_prizeStr.setTextFormat("%u/%u [actor:%s]", currentMedals, requiredMedals, m_medalPath.cStr());
        if (m_textComponent)
        {
            m_textComponent->setText(getTemplate()->getBoneName(), m_prizeStr);
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_ConceptGalleryPriceComponent_Template)
BEGIN_SERIALIZATION(Ray_ConceptGalleryPriceComponent_Template)

    SERIALIZE_MEMBER("offset", m_offset);
    SERIALIZE_MEMBER("bone", m_boneName);
    SERIALIZE_MEMBER("medalPath", m_medalPath);

END_SERIALIZATION()

Ray_ConceptGalleryPriceComponent_Template::Ray_ConceptGalleryPriceComponent_Template()
: m_offset(0.f, 2.f)
{
}

Ray_ConceptGalleryPriceComponent_Template::~Ray_ConceptGalleryPriceComponent_Template()
{
}

}

