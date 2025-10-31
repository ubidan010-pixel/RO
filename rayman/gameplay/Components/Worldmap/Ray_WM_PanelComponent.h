#ifndef _ITF_RAYWMPANELCOMPONENT_H_
#define _ITF_RAYWMPANELCOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

namespace ITF
{
    class Ray_WM_PanelComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_WM_PanelComponent, ActorComponent,1061832985)
        DECLARE_SERIALIZE();
        DECLARE_VALIDATE_COMPONENT();

    public:

        Ray_WM_PanelComponent();
        virtual ~Ray_WM_PanelComponent();

        virtual bbool               needsUpdate()   const { return btrue; }
        virtual bbool               needsDraw()     const { return bfalse; }
        virtual bbool               needsDraw2D()   const { return bfalse; }

        virtual void                onActorLoaded( Pickable::HotReloadType _hotReload );
        virtual void                Update( f32 _deltaTime );

        void                        setTag(const StringID& _tag) { m_tag = _tag; }

    private :

        const class Ray_WM_PanelComponent_Template* getTemplate() const;

        class UITextBoxesComponent*     m_textComponent;

        StringID m_tag;
        String8 m_electoonPath;
        String8 m_prizeStr;
    };

    class Ray_WM_PanelComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_WM_PanelComponent_Template, TemplateActorComponent,615166642)
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_WM_PanelComponent);

    public:

        Ray_WM_PanelComponent_Template();
        ~Ray_WM_PanelComponent_Template();

        const Vec2d&                getOffset() const { return m_offset; }
        const StringID&             getBoneName() const { return m_boneName; }
        const Path&                 getElectoonPath() const { return m_electoonPath; }

    private :

        Vec2d                       m_offset;
        StringID                    m_boneName;
        Path                        m_electoonPath;
    };

    ITF_INLINE const Ray_WM_PanelComponent_Template* Ray_WM_PanelComponent::getTemplate() const
    {
        return static_cast<const Ray_WM_PanelComponent_Template*>(m_template);
    }
}

#endif //_ITF_RAYWMPANELCOMPONENT_H_