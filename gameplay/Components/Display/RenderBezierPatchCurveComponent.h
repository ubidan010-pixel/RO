
#ifndef _ITF_RENDERBEZIERPATCHCURVECOMPONENT_H_
#define _ITF_RENDERBEZIERPATCHCURVECOMPONENT_H_

#ifndef _ITF_GRAPHICCOMPONENT_H_
#include "engine/actors/components/graphiccomponent.h"
#endif //_ITF_GRAPHICCOMPONENT_H_

#ifndef _ITF_BEZIERCURVE_H_
#include "engine/gameplay/BezierCurve.h"
#endif // _ITF_BEZIERCURVE_H_

namespace ITF
{
    class RenderBezierPatchCurveComponent : public GraphicComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(RenderBezierPatchCurveComponent,GraphicComponent,2750093899);

    public:

        DECLARE_SERIALIZE()

        RenderBezierPatchCurveComponent();
        virtual ~RenderBezierPatchCurveComponent();

        virtual bbool needsUpdate() const { return bfalse; }
        virtual bbool needsDraw() const { return btrue; }
        virtual bbool needsDraw2D() const {return bfalse; }
        virtual void  onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void  Draw();

    protected:
        ITF_INLINE const class RenderBezierPatchCurveComponent_Template*  getTemplate() const;

        class BezierCurveComponent*   m_curveComponent;
        BezierCurveRenderer m_bezierRenderer;
    };

    //---------------------------------------------------------------------------------------------------

    class RenderBezierPatchCurveComponent_Template : public GraphicComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(RenderBezierPatchCurveComponent_Template,GraphicComponent_Template,2089577523);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(RenderBezierPatchCurveComponent);

    public:

        RenderBezierPatchCurveComponent_Template() {}
        ~RenderBezierPatchCurveComponent_Template() {}
        ITF_INLINE const BezierCurveRenderer_Template & getBezierRenderer() const { return m_bezierRenderer; }

    private:
        BezierCurveRenderer_Template m_bezierRenderer;
    };

    const RenderBezierPatchCurveComponent_Template*  RenderBezierPatchCurveComponent::getTemplate() const {return static_cast<const RenderBezierPatchCurveComponent_Template*>(m_template);}
}

#endif // _ITF_RENDERBEZIERPATCHCURVECOMPONENT_H_
