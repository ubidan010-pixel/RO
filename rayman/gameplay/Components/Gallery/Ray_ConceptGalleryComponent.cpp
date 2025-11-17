#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_CONCEPTGALLERYCOMPONENT_H_
#include "rayman/gameplay/Components/Gallery/Ray_ConceptGalleryComponent.h"
#endif //_ITF_RAY_CONCEPTGALLERYCOMPONENT_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_TEXTUREGRAPHICCOMPONENT_H_
#include "engine/actors/components/texturegraphiccomponent.h"
#endif //_ITF_TEXTUREGRAPHICCOMPONENT_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_RESOURCE_H_
#include "engine/resources/Resource.h"
#endif //_ITF_RESOURCE_H_

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_

#ifndef _ITF_RAY_CONCEPTGALLERYPRICECOMPONENT_H_
#include "rayman/gameplay/Components/Gallery/Ray_ConceptGalleryPriceComponent.h"
#endif //_ITF_RAY_CONCEPTGALLERYPRICECOMPONENT_H_

namespace ITF
{
//---------------------------------------------------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_ConceptGalleryComponent)

BEGIN_SERIALIZATION_CHILD(Ray_ConceptGalleryComponent)
END_SERIALIZATION()

BEGIN_VALIDATE_COMPONENT(Ray_ConceptGalleryComponent)
    VALIDATE_COMPONENT_PARAM("TextureComponent", m_textureComponent != NULL, "Needs a TextureGraphicComponent");
END_VALIDATE_COMPONENT()

Ray_ConceptGalleryComponent::Ray_ConceptGalleryComponent()
: m_textureComponent(NULL)
, m_linkComponent(NULL)
, m_isUnlocked(bfalse)
{
}

Ray_ConceptGalleryComponent::~Ray_ConceptGalleryComponent()
{
}

void Ray_ConceptGalleryComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    if (m_actor)
    {
        m_textureComponent = m_actor->GetComponent<TextureGraphicComponent>();
        m_linkComponent = m_actor->GetComponent<LinkComponent>();

        // Prefetch both textures to ensure they're loaded
        if (getTemplate())
        {
            Path lockedPath = getTemplate()->getLockedTexturePath();
            Path unlockedPath = getTemplate()->getTexturePath();

            if (lockedPath != Path::EmptyPath && lockedPath.getStringID().isValid())
            {
                m_actor->addResource(Resource::ResourceType_Texture, lockedPath);
            }

            if (unlockedPath != Path::EmptyPath && unlockedPath.getStringID().isValid())
            {
                m_actor->addResource(Resource::ResourceType_Texture, unlockedPath);
            }
        }

        // Set initial texture state
        updateTexture();
        updatePriceActor();
    }
}

void Ray_ConceptGalleryComponent::Update( f32 _dt )
{
    Super::Update(_dt);

    if (m_actor && getTemplate() && m_textureComponent)
    {
        u32 currentMedals = RAY_GAMEMANAGER->computeCompleteMedalCount();
        u32 requiredMedals = getTemplate()->getMedalRequirement();

        bbool shouldBeUnlocked = (currentMedals >= requiredMedals);

        if (shouldBeUnlocked != m_isUnlocked)
        {
            m_isUnlocked = shouldBeUnlocked;
            updateTexture();
            updatePriceActor();
        }
    }
}

u32 Ray_ConceptGalleryComponent::getMedalRequirement() const
{
    if (getTemplate())
    {
        return getTemplate()->getMedalRequirement();
    }
    return 0;
}

void Ray_ConceptGalleryComponent::updateTexture()
{
    if (!m_textureComponent || !getTemplate() || !m_actor)
    {
        return;
    }

    Path texturePath = m_isUnlocked ?
        getTemplate()->getTexturePath() :
        getTemplate()->getLockedTexturePath();

    if (texturePath != Path::EmptyPath && texturePath.getStringID().isValid())
    {
        // Set new texture file and reload
        String textureFileStr = texturePath.getString();
        m_textureComponent->setTextureFile(textureFileStr);

        // Reload the component to load the new texture
        m_textureComponent->onActorLoaded(Pickable::HotReloadType_None);
    }
}

void Ray_ConceptGalleryComponent::updatePriceActor()
{
    if (!m_linkComponent || !m_actor)
    {
        return;
    }

    // Find price actor in linked children
    const LinkComponent::ChildrenList& children = m_linkComponent->getChildren();
    for (LinkComponent::ChildrenList::const_iterator it = children.begin(); it != children.end(); ++it)
    {
        BaseObject* childObj = m_linkComponent->getChildObject(it->getPath());
        if (childObj && childObj->getObjectType() == BaseObject::eActor)
        {
            // Safe cast using ObjectType check (no RTTI needed)
            Actor* childActor = static_cast<Actor*>(childObj);
            if (childActor)
            {
                // Check if this is a price actor (has Ray_ConceptGalleryPriceComponent)
                Ray_ConceptGalleryPriceComponent* priceComp = childActor->GetComponent<Ray_ConceptGalleryPriceComponent>();
                if (priceComp)
                {
                    // Enable when locked, disable when unlocked
                    if (m_isUnlocked)
                    {
                        childActor->disable();
                    }
                    else
                    {
                        childActor->enable();
                    }
                    break;
                }
            }
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_ConceptGalleryComponent_Template)
BEGIN_SERIALIZATION(Ray_ConceptGalleryComponent_Template)
    SERIALIZE_MEMBER("medalRequirement", m_medalRequirement);
    SERIALIZE_MEMBER("texturePath", m_texturePath);
    SERIALIZE_MEMBER("lockedTexturePath", m_lockedTexturePath);
END_SERIALIZATION()

Ray_ConceptGalleryComponent_Template::Ray_ConceptGalleryComponent_Template()
: m_medalRequirement(0)
{
}

Ray_ConceptGalleryComponent_Template::~Ray_ConceptGalleryComponent_Template()
{
}

} // namespace ITF
