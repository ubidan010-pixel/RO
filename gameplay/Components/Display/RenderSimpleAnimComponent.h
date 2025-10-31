
#ifndef _ITF_RENDERSIMPLEANIMCOMPONENT_H_
#define _ITF_RENDERSIMPLEANIMCOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

namespace ITF
{
    class RenderSimpleAnimComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(RenderSimpleAnimComponent,ActorComponent,448027741);

    public:

        DECLARE_SERIALIZE()

        RenderSimpleAnimComponent();
        virtual ~RenderSimpleAnimComponent();

        virtual bbool needsUpdate() const { return btrue; }
        virtual bbool needsDraw() const;
        virtual bbool needsDraw2D() const;
        virtual void                onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
#ifdef ITF_SUPPORT_EDITOR
        virtual void                onEditorMove(bbool _modifyInitialPos = btrue);
#endif // ITF_SUPPORT_EDITOR
        virtual void                Update( f32 _deltaTime );
        virtual void                Draw();
        virtual void                Draw2D();

        void                        setComputeLocalToGlobal( bbool _val ) { m_computeLocalToGlobal = _val; }

    protected:

        class AnimLightComponent*   m_animComponent;
        bbool                       m_computeLocalToGlobal;
    };

    //---------------------------------------------------------------------------------------------------

    class RenderSimpleAnimComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(RenderSimpleAnimComponent_Template,TemplateActorComponent,2172854595);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(RenderSimpleAnimComponent);

    public:

        RenderSimpleAnimComponent_Template() {}
        ~RenderSimpleAnimComponent_Template() {}

    private:

    };
}

#endif // _ITF_RENDERSIMPLEANIMCOMPONENT_H_
