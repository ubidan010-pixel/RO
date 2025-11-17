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
        }
    }
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
