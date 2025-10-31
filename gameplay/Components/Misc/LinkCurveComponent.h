#ifndef _ITF_LINKCURVECOMPONENT_H_
#define _ITF_LINKCURVECOMPONENT_H_

#ifndef _ITF_PATCHCURVECOMPONENT_H_
#include "gameplay/Components/Misc/PatchCurveComponent.h"
#endif //_ITF_PATCHCURVECOMPONENT_H_

namespace ITF
{
    class LinkComponent;

    /*
        Draws a curve between the actor and each of its linked children.
        Requires a LinkComponent.
    */
    class LinkCurveComponent : public PatchCurveComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(LinkCurveComponent, PatchCurveComponent,3124044823);
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        LinkCurveComponent();
        virtual ~LinkCurveComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return btrue; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        Draw();
        virtual void        Update( f32 _dt );

        f32                 getAlpha( Actor* _child );
        void                setAlpha( Actor* _child, f32 _value );

    private:
        ITF_INLINE const class LinkCurveComponent_Template*  getTemplate() const;

        LinkComponent*  m_linkComponent;

        class ChildData
        {
        public:
            ChildData()
                : m_alpha(1.f)
            {}

            f32         m_alpha;
            //PatchCurve  m_patchCurve;
            BezierPatch m_patchCurve;
        };
        typedef ITF_MAP<ActorRef, ChildData> ChildDataMap;
        ChildDataMap    m_childDataMap;

#ifdef ITF_WINDOWS
        bbool           m_warningSent;
#endif // ITF_WINDOWS
    };

    //-------------------------------------------------------------------------------------
    class LinkCurveComponent_Template : public PatchCurveComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(LinkCurveComponent_Template, PatchCurveComponent_Template,3414317268);
        DECLARE_ACTORCOMPONENT_TEMPLATE(LinkCurveComponent);
        DECLARE_SERIALIZE();

    public:

        LinkCurveComponent_Template();
        ~LinkCurveComponent_Template() {}

    private:

    };

    const LinkCurveComponent_Template*  LinkCurveComponent::getTemplate() const {return static_cast<const LinkCurveComponent_Template*>(m_template);}

};

#endif //_ITF_LINKCURVECOMPONENT_H_
