#ifndef _ITF_RAY_PLAYERCOSTUMEPRICECOMPONENT_H_
#define _ITF_RAY_PLAYERCOSTUMEPRICECOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

namespace ITF
{
    class Ray_PlayerCostumePriceComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_PlayerCostumePriceComponent, ActorComponent,760770928)
        DECLARE_SERIALIZE();
        DECLARE_VALIDATE_COMPONENT();

    public:

        Ray_PlayerCostumePriceComponent();
        virtual ~Ray_PlayerCostumePriceComponent();

        virtual bbool               needsUpdate()   const { return btrue; }
        virtual bbool               needsDraw()     const { return bfalse; }
        virtual bbool               needsDraw2D()   const { return bfalse; }

        virtual void                onActorLoaded( Pickable::HotReloadType _hotReload );
        virtual void                Update( f32 _deltaTime );

    private :

        const class Ray_PlayerCostumePriceComponent_Template* getTemplate() const;

        class AnimLightComponent*       m_animComponent;
        class UITextBoxesComponent*     m_textComponent;
        String8                         m_prizeStr;
        String8                         m_electoonPath;
    };

    class Ray_PlayerCostumePriceComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_PlayerCostumePriceComponent_Template, TemplateActorComponent,3369407398);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_PlayerCostumePriceComponent);

    public:

        Ray_PlayerCostumePriceComponent_Template();
        ~Ray_PlayerCostumePriceComponent_Template();

        const Vec2d&                getOffset() const { return m_offset; }
        const StringID&             getBoneName() const { return m_boneName; }
        const Path&                 getElectoonPath() const { return m_electoonPath; }

    private :

        Vec2d                       m_offset;
        StringID                    m_boneName;
        Path                        m_electoonPath;
    };

    ITF_INLINE const Ray_PlayerCostumePriceComponent_Template* Ray_PlayerCostumePriceComponent::getTemplate() const
    {
        return static_cast<const Ray_PlayerCostumePriceComponent_Template*>(m_template);
    }
}

#endif //_ITF_RAY_PLAYERCOSTUMEPRICECOMPONENT_H_