#include "precompiled_GUIAdapter_CEGUI.h"

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif // _ITF_GFX_ADAPTER_H_

#include "CEGUIITFRenderTarget.h"
#include "CEGUIRenderQueue.h"
#include "CEGUIITFGeometryBuffer.h"
#include <cmath>

// Start of CEGUI namespace section
namespace CEGUI
{
//----------------------------------------------------------------------------//
    const double ITFRenderTarget::d_yfov_tan = 0.267949192431123;

//----------------------------------------------------------------------------//
ITFRenderTarget::ITFRenderTarget(ITFRenderer& owner) :
    d_owner(owner),
    d_ITFAdapter(owner.getAdapter()),
    d_area(0, 0, 0, 0),
    d_matrixValid(false),
    d_viewDistance(0.0)
{
    
}

//----------------------------------------------------------------------------//
void ITFRenderTarget::draw(const GeometryBuffer& buffer)
{
    buffer.draw();
}

//----------------------------------------------------------------------------//
void ITFRenderTarget::draw(const RenderQueue& queue)
{
    queue.draw();
}

//----------------------------------------------------------------------------//
void ITFRenderTarget::setArea(const Rect& area)
{
    d_area = area;
    d_matrixValid = false;
}

//----------------------------------------------------------------------------//
const Rect& ITFRenderTarget::getArea() const
{
    return d_area;
}

//----------------------------------------------------------------------------//
void ITFRenderTarget::activate()
{
    ITF::GFX_RECT area;
    area.bottom = static_cast<unsigned int>(d_area.d_bottom);
    area.left = static_cast<unsigned int>(d_area.d_left);
    area.right = static_cast<unsigned int>(d_area.d_right);
    area.top = static_cast<unsigned int>(d_area.d_top);

    d_ITFAdapter->setupViewport(&area);

    if (!d_matrixValid)
        updateMatrix();
    
    d_ITFAdapter->setMatrixTransform(ITF::GFX_PROJECTION, &d_matrix);
}

//----------------------------------------------------------------------------//
void ITFRenderTarget::deactivate()
{
}

//----------------------------------------------------------------------------//
void ITFRenderTarget::unprojectPoint(const GeometryBuffer& buff,
    const Vector2& p_in, Vector2& p_out) const
{
    if (!d_matrixValid)
        updateMatrix();

    const ITFGeometryBuffer& gb =
        static_cast<const ITFGeometryBuffer&>(buff);

    const int vp[4] = {
        static_cast<int>(d_area.d_left),
        static_cast<int>(d_area.d_top),
        static_cast<int>(d_area.getWidth()),
        static_cast<int>(d_area.getHeight())
    };

    ITF::Vec2d v_in;
    ITF::Vec2d v_out;
    v_in.m_x = static_cast<float>(p_in.d_x);
    v_in.m_y = static_cast<float>(p_in.d_y);

    d_ITFAdapter->unprojectPoint(v_in, (float)d_viewDistance, gb.getMatrix(), &d_matrix, vp, v_out);

    p_out.d_x = static_cast<float>(v_out.m_x);
    p_out.d_y = static_cast<float>(v_out.m_y);
}

//----------------------------------------------------------------------------//
void ITFRenderTarget::updateMatrix() const
{

    const double w = d_area.getWidth();
    const double h = d_area.getHeight();
    const double aspect = w / h;
    const double midx = w * 0.5;
    const double midy = h * 0.5;
    d_viewDistance = midx / (aspect * 0.267949192431123f);

    d_ITFAdapter->setLookAtMatrix(&d_matrix, midx, midy, (float)d_viewDistance, (float)aspect);

    d_matrixValid = true;
}

//----------------------------------------------------------------------------//

} // End of  CEGUI namespace section
