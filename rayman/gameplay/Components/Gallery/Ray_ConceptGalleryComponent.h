#ifndef _ITF_RAY_CONCEPTGALLERYCOMPONENT_H_
#define _ITF_RAY_CONCEPTGALLERYCOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

namespace ITF
{
    class Ray_ConceptGalleryComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_ConceptGalleryComponent, ActorComponent, 4291508693)
        DECLARE_SERIALIZE();
        DECLARE_VALIDATE_COMPONENT()

    public:

        Ray_ConceptGalleryComponent();
        virtual ~Ray_ConceptGalleryComponent();

        virtual bbool               needsUpdate()   const { return btrue; }
        virtual bbool               needsDraw()     const { return bfalse; }
        virtual bbool               needsDraw2D()   const { return bfalse; }

        virtual void                onActorLoaded( Pickable::HotReloadType _hotReload );
        virtual void                Update( f32 _dt );

    private:
        const class Ray_ConceptGalleryComponent_Template* getTemplate() const;
        void                        updateTexture();

        class TextureGraphicComponent* m_textureComponent;
        bbool                       m_isUnlocked;
    };

    class Ray_ConceptGalleryComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_ConceptGalleryComponent_Template, TemplateActorComponent, 1792344745);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_ConceptGalleryComponent);

    public:

        Ray_ConceptGalleryComponent_Template();
        ~Ray_ConceptGalleryComponent_Template();

        u32                         getMedalRequirement() const { return m_medalRequirement; }
        const Path&                 getTexturePath() const { return m_texturePath; }
        const Path&                 getLockedTexturePath() const { return m_lockedTexturePath; }

    private:

        u32                         m_medalRequirement;
        Path                        m_texturePath;          // Unlocked texture
        Path                        m_lockedTexturePath;     // Locked texture (_lock suffix)
    };

    ITF_INLINE const Ray_ConceptGalleryComponent_Template* Ray_ConceptGalleryComponent::getTemplate() const
    {
        return static_cast<const Ray_ConceptGalleryComponent_Template*>(m_template);
    }
}

#endif //_ITF_RAY_CONCEPTGALLERYCOMPONENT_H_
