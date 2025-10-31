#include "precompiled_GUIAdapter_CEGUI.h"

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif // _ITF_GFX_ADAPTER_H_

#include "CEGUIITFGeometryBuffer.h"
#include "CEGUIRenderEffect.h"
#include "CEGUIITFTexture.h"
#include "CEGUIVertex.h"

// Start of CEGUI namespace section
namespace CEGUI
{
//----------------------------------------------------------------------------//
ITFGeometryBuffer::ITFGeometryBuffer(ITF::GFXAdapter* ITFAdapter) :
    d_activeTexture(0),
    d_translation(0, 0, 0),
    d_rotation(0, 0, 0),
    d_pivot(0, 0, 0),
    d_effect(0),
    d_ITFAdapter(ITFAdapter),
    d_matrixValid(false)
{
}
//----------------------------------------------------------------------------//
void ITFGeometryBuffer::appendVertex(const Vertex& vertex)
{
    appendGeometry(&vertex, 1);
}

//----------------------------------------------------------------------------//
void ITFGeometryBuffer::draw() const
{
    // setup clip region
    ITF::GFX_RECT clip;
    clip.left   = static_cast<unsigned int>(d_clipRect.d_left);
    clip.top    = static_cast<unsigned int>(d_clipRect.d_top);
    clip.right  = static_cast<unsigned int>(d_clipRect.d_right);
    clip.bottom = static_cast<unsigned int>(d_clipRect.d_bottom);
    d_ITFAdapter->setScissorRect(&clip);

    // apply the transformations we need to use.
    if (!d_matrixValid)
        updateMatrix();

    d_ITFAdapter->setMatrixTransform(ITF::GFX_MWORLDTOWIEW, &d_matrix);

    const int pass_count = d_effect ? d_effect->getPassCount() : 1;
    for (int pass = 0; pass < pass_count; ++pass)
    {
        // set up RenderEffect
        if (d_effect)
            d_effect->performPreRenderFunctions(pass);

        // draw the batches
        size_t pos = 0;
        BatchList::const_iterator i = d_batches.begin();
        for ( ; i != d_batches.end(); ++i)
        {
            d_ITFAdapter->SetTexture(0, (*i).first);
            d_ITFAdapter->DrawPrimitive(ITF::GFX_TRIANGLES, &d_vertices[pos], (*i).second);
            pos += (*i).second;
        }
    }

    // clean up RenderEffect
    if (d_effect)
        d_effect->performPostRenderFunctions();
}

//----------------------------------------------------------------------------//
void ITFGeometryBuffer::setTranslation(const Vector3& v)
{
    d_translation = v;
    d_matrixValid = false;
}

//----------------------------------------------------------------------------//
void ITFGeometryBuffer::setRotation(const Vector3& r)
{
    d_rotation = r;
    d_matrixValid = false;
}

//----------------------------------------------------------------------------//
void ITFGeometryBuffer::setPivot(const Vector3& p)
{
    d_pivot = Vector3(p.d_x, p.d_y, p.d_z);
    d_matrixValid = false;
}

//----------------------------------------------------------------------------//
void ITFGeometryBuffer::setClippingRegion(const Rect& region)
{
    d_clipRect = region;
}

//----------------------------------------------------------------------------//
void ITFGeometryBuffer::appendGeometry(const Vertex* const vbuff,
    uint vertex_count)
{
    performBatchManagement();

    // update size of current batch
    d_batches.back().second += vertex_count;

    // buffer these vertices
    ITF::Vertex vd;
    const Vertex* vs = vbuff;
    float TexOffet = d_ITFAdapter->getTexelOffset();
    d_vertices.reserve(d_vertices.capacity() + vertex_count);
    for (uint i = 0; i < vertex_count; ++i, ++vs)
    {
        // copy vertex info the buffer, converting from CEGUI::Vertex to
        // something directly usable by ITF as needed.
        vd.m_uv.m_x         = vs->tex_coords.d_x;
        vd.m_uv.m_y         = vs->tex_coords.d_y;
        vd.m_color          = ITFCOLOR_FROM_ARGB(vs->colour_val.getARGB());
        vd.m_pos.m_x        = vs->position.d_x - TexOffet;
        vd.m_pos.m_y        = vs->position.d_y - TexOffet;
        vd.m_pos.m_z        = vs->position.d_z;
        d_vertices.push_back(vd);
    }
}

//----------------------------------------------------------------------------//
void ITFGeometryBuffer::setActiveTexture(Texture* texture)
{
    d_activeTexture = static_cast<ITFTexture*>(texture);
}

//----------------------------------------------------------------------------//
void ITFGeometryBuffer::reset()
{
    d_batches.clear();
    d_vertices.clear();
    d_activeTexture = 0;
}

//----------------------------------------------------------------------------//
Texture* ITFGeometryBuffer::getActiveTexture() const
{
    return d_activeTexture;
}

//----------------------------------------------------------------------------//
uint ITFGeometryBuffer::getVertexCount() const
{
    return d_vertices.size();
}

//----------------------------------------------------------------------------//
uint ITFGeometryBuffer::getBatchCount() const
{
    return d_batches.size();
}

//----------------------------------------------------------------------------//
void ITFGeometryBuffer::performBatchManagement()
{
    ITF::Texture* tex = d_activeTexture ?
                            d_activeTexture->getITFTexture() : 0;

    // create a new batch if there are no batches yet, or if the active texture
    // differs from that used by the current batch.
    if (d_batches.empty() || (tex != d_batches.back().first))
        d_batches.push_back(BatchInfo(tex, 0));
}

//----------------------------------------------------------------------------//
void ITFGeometryBuffer::setRenderEffect(RenderEffect* effect)
{
    d_effect = effect;
}

//----------------------------------------------------------------------------//
RenderEffect* ITFGeometryBuffer::getRenderEffect()
{
    return d_effect;
}

//----------------------------------------------------------------------------//
const ITF::GFX_Matrix* ITFGeometryBuffer::getMatrix() const
{
    if (!d_matrixValid)
        updateMatrix();

    return &d_matrix;
}

//----------------------------------------------------------------------------//
void ITFGeometryBuffer::updateMatrix() const
{
    const ITF::Vec3d p(d_pivot.d_x, d_pivot.d_y, d_pivot.d_z);
    const ITF::Vec3d t(d_translation.d_x, d_translation.d_y, d_translation.d_z);
    const ITF::Vec3d r(d_rotation.d_x, d_rotation.d_y, d_rotation.d_z);

    d_ITFAdapter->updateMatrix(&d_matrix, p, t, r);

    d_matrixValid = true;
}

//----------------------------------------------------------------------------//

} // End of  CEGUI namespace section
