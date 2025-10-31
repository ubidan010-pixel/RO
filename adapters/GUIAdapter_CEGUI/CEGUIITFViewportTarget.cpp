#include "precompiled_GUIAdapter_CEGUI.h"

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif // _ITF_GFX_ADAPTER_H_

#include "CEGUIITFViewportTarget.h"
#include "CEGUIRenderQueue.h"
#include "CEGUIGeometryBuffer.h"
#include "CEGUIExceptions.h"

// Start of CEGUI namespace section
namespace CEGUI
{
//----------------------------------------------------------------------------//
ITFViewportTarget::ITFViewportTarget(ITFRenderer& owner) :
    ITFRenderTarget(owner)
{
    // viewport area defaults to whatever the current ITF viewport is set to
    ITF::GFX_RECT area;
    d_owner.getAdapter()->getViewport(&area);

    Rect init_area(Vector2(static_cast<float>(area.left), static_cast<float>(area.top)),
        Size(static_cast<float>(area.right), static_cast<float>(area.bottom)));

    setArea(init_area);
}

//----------------------------------------------------------------------------//
ITFViewportTarget::ITFViewportTarget(ITFRenderer& owner,
    const Rect& area) :
        ITFRenderTarget(owner)
{
    setArea(area);
}

//----------------------------------------------------------------------------//
bool ITFViewportTarget::isImageryCache() const
{
    return false;
}

//----------------------------------------------------------------------------//

} // End of  CEGUI namespace section
