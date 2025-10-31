#include "precompiled_gameplay.h"

#ifndef _ITF_RENDERBEZIERPATCHCURVECOMPONENT_H_
#include "gameplay/Components/Display/RenderBezierPatchCurveComponent.h"
#endif //_ITF_RENDERBEZIERPATCHCURVECOMPONENT_H_

#ifndef _ITF_BEZIERCURVECOMPONENT_H_
#include "gameplay/Components/Misc/BezierCurveComponent.h"
#endif //_ITF_BEZIERCURVECOMPONENT_H_


namespace ITF
{

IMPLEMENT_OBJECT_RTTI(RenderBezierPatchCurveComponent)
BEGIN_SERIALIZATION_CHILD(RenderBezierPatchCurveComponent)
END_SERIALIZATION()


RenderBezierPatchCurveComponent::RenderBezierPatchCurveComponent()
: m_curveComponent(NULL)
{
}

RenderBezierPatchCurveComponent::~RenderBezierPatchCurveComponent()
{
}

void RenderBezierPatchCurveComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    m_curveComponent = m_actor->GetComponent<BezierCurveComponent>();

    m_bezierRenderer.onLoaded(&getTemplate()->getBezierRenderer(), m_actor->getResourceGroup());
    m_bezierRenderer.setColor(getColorFactor());
    m_bezierRenderer.setFogColor(getColorFog());
}


void RenderBezierPatchCurveComponent::Draw()
{
    Super::Draw();

    const BezierCurve& bezier = m_curveComponent->getBezierCurve();
    const Transform3d transform = getWorldInitTransform(m_actor);
    m_bezierRenderer.draw(bezier, &transform);
}

//-------------------------------------------------------------------------------------

IMPLEMENT_OBJECT_RTTI(RenderBezierPatchCurveComponent_Template)
BEGIN_SERIALIZATION(RenderBezierPatchCurveComponent_Template)
    SERIALIZE_OBJECT("bezierRenderer",m_bezierRenderer);
END_SERIALIZATION()

}
