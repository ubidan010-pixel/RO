#ifndef _CEGUIITFRenderTarget_h_
#define _CEGUIITFRenderTarget_h_

#include "CEGUIITFRenderer.h"
#include "CEGUIRenderTarget.h"
#include "CEGUIRect.h"

// Start of CEGUI namespace section
namespace CEGUI
{
/*!
\brief
    Intermediate ITF implementation of a RenderTarget.
*/
class ITF_GUIRENDERER_API ITFRenderTarget : public virtual RenderTarget
{
public:
    //! Constructor
    ITFRenderTarget(ITFRenderer& owner);

    // implement parts of RenderTarget interface
    void draw(const GeometryBuffer& buffer);
    void draw(const RenderQueue& queue);
    void setArea(const Rect& area);
    const Rect& getArea() const;
    void activate();
    void deactivate();
    void unprojectPoint(const GeometryBuffer& buff,
                        const Vector2& p_in, Vector2& p_out) const;

protected:
    //! helper that initialises the cached matrix
    virtual void updateMatrix() const;

    //! ITFRenderer that created this object
    ITFRenderer& d_owner;
    //! holds defined area for the RenderTarget
    Rect d_area;
    //! ITF Adapter
    ITF::GFXAdapter* d_ITFAdapter;
    //! tangent of the y FOV half-angle; used to calculate viewing distance.
    static const double d_yfov_tan;
    //! saved copy of projection matrix
    mutable ITF::GFX_Matrix d_matrix;
    //! true if saved matrix is up to date
    mutable bool d_matrixValid;
    //! tracks viewing distance (this is set up at the same time as d_matrix)
    mutable double d_viewDistance;
};

} // End of  CEGUI namespace section

#endif  // end of guard _CEGUIITFRenderTarget_h_
