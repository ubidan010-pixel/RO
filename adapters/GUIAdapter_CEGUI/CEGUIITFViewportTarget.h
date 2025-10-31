#ifndef _CEGUIITFViewportTarget_h_
#define _CEGUIITFViewportTarget_h_

#include "CEGUIITFRenderTarget.h"
#include "CEGUIRect.h"

// Start of CEGUI namespace section
namespace CEGUI
{
/*!
\brief
    ITF implementation of a RenderTarget that represents am on-scren
    viewport.
*/
class ITF_GUIRENDERER_API ITFViewportTarget : public ITFRenderTarget
{
public:
    /*!
    \brief
        Construct a default ITFViewportTarget that uses the currently
        defined ITF viewport as it's initial area.
    */
    ITFViewportTarget(ITFRenderer& owner);

    /*!
    \brief
        Construct a ITFViewportTarget that uses the specified Rect as it's
        initial area.

    \param area
        Rect object describing the initial viewport area that should be used for
        the RenderTarget.
    */
    ITFViewportTarget(ITFRenderer& owner, const Rect& area);

    // implementations of RenderTarget interface
    bool isImageryCache() const;
};

} // End of  CEGUI namespace section

#endif  // end of guard _CEGUIITFViewportTarget_h_
