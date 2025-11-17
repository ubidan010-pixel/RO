#ifndef _ITF_RAY_CONCEPTGALLERYPRICECOMPONENT_H_
#define _ITF_RAY_CONCEPTGALLERYPRICECOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

namespace ITF
{
    class Ray_ConceptGalleryPriceComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_ConceptGalleryPriceComponent, ActorComponent, 934131534)
        DECLARE_SERIALIZE();
        DECLARE_VALIDATE_COMPONENT();

    public:

        Ray_ConceptGalleryPriceComponent();
        virtual ~Ray_ConceptGalleryPriceComponent();

        virtual bbool               needsUpdate()   const { return btrue; }
        virtual bbool               needsDraw()     const { return bfalse; }
        virtual bbool               needsDraw2D()   const { return bfalse; }

        virtual void                onActorLoaded( Pickable::HotReloadType _hotReload );
        virtual void                Update( f32 _deltaTime );

    private :

        const class Ray_ConceptGalleryPriceComponent_Template* getTemplate() const;

        class AnimLightComponent*       m_animComponent;
        class UITextBoxesComponent*     m_textComponent;
        class Ray_ConceptGalleryComponent* m_parentConceptComponent;  // Cached parent component
        String8                         m_prizeStr;
        String8                         m_medalPath;
    };

    class Ray_ConceptGalleryPriceComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_ConceptGalleryPriceComponent_Template, TemplateActorComponent, 2512516538);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_ConceptGalleryPriceComponent);

    public:

        Ray_ConceptGalleryPriceComponent_Template();
        ~Ray_ConceptGalleryPriceComponent_Template();

        const Vec2d&                getOffset() const { return m_offset; }
        const StringID&             getBoneName() const { return m_boneName; }
        const Path&                 getMedalPath() const { return m_medalPath; }

    private :

        Vec2d                       m_offset;
        StringID                    m_boneName;
        Path                        m_medalPath;
    };

    ITF_INLINE const Ray_ConceptGalleryPriceComponent_Template* Ray_ConceptGalleryPriceComponent::getTemplate() const
    {
        return static_cast<const Ray_ConceptGalleryPriceComponent_Template*>(m_template);
    }
}

#endif //_ITF_RAY_CONCEPTGALLERYPRICECOMPONENT_H_

