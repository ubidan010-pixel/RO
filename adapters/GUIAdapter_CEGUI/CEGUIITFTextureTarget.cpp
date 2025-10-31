#include "precompiled_GUIAdapter_CEGUI.h"

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif // _ITF_GFX_ADAPTER_H_

#include "CEGUIITFTextureTarget.h"
#include "CEGUIExceptions.h"
#include "CEGUIRenderQueue.h"
#include "CEGUIGeometryBuffer.h"
 
#include "CEGUIITFRenderer.h"
#include "CEGUIITFTexture.h"

// Start of CEGUI namespace section
namespace CEGUI
{
//----------------------------------------------------------------------------//
const float ITFTextureTarget::DEFAULT_SIZE = 128.0f;

//----------------------------------------------------------------------------//
ITFTextureTarget::ITFTextureTarget(ITFRenderer& owner) :
    ITFRenderTarget(owner),
    d_textureTarget(0)
{
    // this essentially creates a 'null' CEGUI::Texture
    d_CEGUITexture = &static_cast<ITFTexture&>(
        d_owner.createTexture(0, d_area.getSize()));
    
    initialiseRenderTexture();

    // setup area and cause the initial texture to be generated.
    declareRenderSize(Size(DEFAULT_SIZE, DEFAULT_SIZE));
}

//----------------------------------------------------------------------------//
ITFTextureTarget::~ITFTextureTarget()
{
    d_owner.getAdapter()->deleteRenderTarget(d_textureTarget);

    d_owner.destroyTexture(*d_CEGUITexture);
}

//----------------------------------------------------------------------------//
void ITFTextureTarget::declareRenderSize(const Size& sz)
{
    // exit if current size is enough
    if ((d_area.getWidth() >= sz.d_width) && (d_area.getHeight() >=sz.d_height))
        return;

    setArea(Rect(d_area.getPosition(), sz));

    resizeRenderTexture();
    // if Render Texture clear it
    clearRenderTexture();
}

//----------------------------------------------------------------------------//
void ITFTextureTarget::clearRenderTexture()
{
    // switch to targetting our texture
    d_owner.getAdapter()->enableRenderTarget(d_textureTarget);
    // Clear it.
    d_owner.getAdapter()->clear((unsigned int)ITF::GFX_CLEAR_COLOR, 0.0f,0.0f,0);
    // switch back to rendering to previous target
    d_owner.getAdapter()->disableRenderTarget(d_textureTarget);
}

//----------------------------------------------------------------------------//
bool ITFTextureTarget::isImageryCache() const
{
    return true;
}

//----------------------------------------------------------------------------//
void ITFTextureTarget::activate()
{
    d_owner.getAdapter()->enableRenderTarget(d_textureTarget);
    ITFRenderTarget::activate();
}

//----------------------------------------------------------------------------//
void ITFTextureTarget::deactivate()
{
    ITFRenderTarget::deactivate();
    
    d_owner.getAdapter()->disableRenderTarget(d_textureTarget);
}

//----------------------------------------------------------------------------//
void ITFTextureTarget::clear()
{
    d_owner.getAdapter()->clearRenderTarget(d_textureTarget, 0x0);
}

//----------------------------------------------------------------------------//
Texture& ITFTextureTarget::getTexture() const
{
    return *d_CEGUITexture;
}

//----------------------------------------------------------------------------//
void ITFTextureTarget::initialiseRenderTexture()
{
    d_textureTarget = d_owner.getAdapter()->getNewRenderTarget();
    
    if (!d_owner.getAdapter()->isUseFrameBuffer()) return;

    d_owner.getAdapter()->initialiseRenderTarget(d_textureTarget);

    // ensure the CEGUI::Texture is wrapping the ITF texture and has correct size
    d_CEGUITexture->setITFTexture(d_textureTarget->getTexture(), d_area.getSize());
}

//----------------------------------------------------------------------------//
void ITFTextureTarget::resizeRenderTexture()
{
    const Size sz(d_area.getSize());
    ITF::Size Size(sz.d_width, sz.d_height);

    d_owner.getAdapter()->resizeRenderTarget(d_textureTarget, Size);

    // ensure the CEGUI::Texture is wrapping the texture and has correct size
    d_CEGUITexture->setITFTexture(d_textureTarget->getTexture(), sz);
}

//----------------------------------------------------------------------------//
bool ITFTextureTarget::isRenderingInverted() const
{
    return (d_owner.getAdapter()->isRenderTargetInverted() == ITF::btrue);
}

//----------------------------------------------------------------------------//

} // End of  CEGUI namespace section
