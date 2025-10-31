#ifndef _CEGUIITFGeometryBuffer_h_
#define _CEGUIITFGeometryBuffer_h_

#include "CEGUIGeometryBuffer.h"
#include "CEGUIITFRenderer.h"
#include "CEGUIRect.h"
#include <utility>
#include <vector>

#include "engine/AdaptersInterfaces/GFXAdapter.h"

#if defined(_MSC_VER)
#   pragma warning(push)
#   pragma warning(disable : 4251)
#endif

// Start of CEGUI namespace section
namespace CEGUI
{
class ITFTexture;

/*!
\brief
    ITF based implementation of the GeometryBuffer interface.
*/
class ITF_GUIRENDERER_API ITFGeometryBuffer : public GeometryBuffer
{
public:
    //! Constructor
    ITFGeometryBuffer(ITF::GFXAdapter* ITFAdapter);

    // implementation of abstract members from GeometryBuffer
    void draw() const;
    void setTranslation(const Vector3& t);
    void setRotation(const Vector3& r);
    void setPivot(const Vector3& p);
    void setClippingRegion(const Rect& region);
    void appendVertex(const Vertex& vertex);
    void appendGeometry(const Vertex* const vbuff, uint vertex_count);
    void setActiveTexture(Texture* texture);
    void reset();
    Texture* getActiveTexture() const;
    uint getVertexCount() const;
    uint getBatchCount() const;
    void setRenderEffect(RenderEffect* effect);
    RenderEffect* getRenderEffect();

    //! return the modelview matrix used for this buffer.
    const ITF::GFX_Matrix* getMatrix() const;

protected:
    //! perform batch management operations prior to adding new geometry.
    void performBatchManagement();
    //! update cached matrix
    void updateMatrix() const;

    //! last texture that was set as active
    ITFTexture* d_activeTexture;
    //! type to track info for per-texture sub batches of geometry
    typedef std::pair<ITF::Texture*, uint> BatchInfo;
    //! type of container that tracks BatchInfos.
    typedef std::vector<BatchInfo> BatchList;
    //! list of texture batches added to the geometry buffer
    BatchList d_batches;
    //! type of container used to queue the geometry
    typedef std::vector<ITF::Vertex> VertexList;
    //! container where added geometry is stored.
    VertexList d_vertices;
    //! rectangular clip region
    Rect d_clipRect;
    //! translation vector
    Vector3 d_translation;
    //! rotation vector
    Vector3 d_rotation;
    //! pivot point for rotation
    Vector3 d_pivot;
    //! RenderEffect that will be used by the GeometryBuffer
    RenderEffect* d_effect;
    //! ITF Adapter
    ITF::GFXAdapter*    d_ITFAdapter;
    //! model matrix cache
    mutable ITF::GFX_Matrix d_matrix;
    //! true when d_matrix is valid and up to date
    mutable bool d_matrixValid;
};


} // End of  CEGUI namespace section

#if defined(_MSC_VER)
#   pragma warning(pop)
#endif

#endif  // end of guard _CEGUIITFGeometryBuffer_h_
