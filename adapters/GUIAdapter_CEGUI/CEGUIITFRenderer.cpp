#include "precompiled_GUIAdapter_CEGUI.h"

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif // _ITF_GFX_ADAPTER_H_

#ifdef HAVE_CONFIG_H
#   include "config.h"
#endif

#include "CEGUIITFRenderer.h"
#include "CEGUIITFTexture.h"
#include "CEGUIExceptions.h"
#include "CEGUIEventArgs.h"
#include "CEGUIImageCodec.h"
#include "CEGUIDynamicModule.h"
#include "CEGUIITFViewportTarget.h"
#include "CEGUIITFGeometryBuffer.h"
#include "CEGUIRenderingRoot.h"
#include "CEGUIITFTextureTarget.h"

#include <sstream>
#include <algorithm>

// Start of CEGUI namespace section
namespace CEGUI
{
//----------------------------------------------------------------------------//
String ITFRenderer::d_rendererID(
"CEGUI::ITFRenderer - ITF renderer module.");

//----------------------------------------------------------------------------//
ITFRenderer& ITFRenderer::create(ITF::GFXAdapter* ITFAdapter)
{
    return *new ITFRenderer(ITFAdapter);
}

//----------------------------------------------------------------------------//

void ITFRenderer::destroy(ITFRenderer& renderer)
{
    delete (&renderer);
}

//----------------------------------------------------------------------------//
ITFRenderer::ITFRenderer(ITF::GFXAdapter* ITFAdapter) :
    d_ITFAdapter(ITFAdapter),
    d_displayDPI(96, 96),
    d_initExtraStates(false)
{
    // get rough max texture size
    d_maxTextureSize = ITFAdapter->getMaxTextureSize();

    ITF::GFX_RECT area;
    ITFAdapter->getViewport(&area);
    d_displaySize = Size(static_cast<float>(area.right), static_cast<float>(area.bottom));

    ITFAdapter->CEGUIinitialiseExtensions();

    d_defaultTarget = new ITFViewportTarget(*this);
    d_defaultRoot = new RenderingRoot(*d_defaultTarget);
}

//----------------------------------------------------------------------------//

ITFRenderer::~ITFRenderer()
{
    destroyAllGeometryBuffers();
    destroyAllTextureTargets();
    destroyAllTextures();

    SF_DEL(d_defaultRoot);
    SF_DEL(d_defaultTarget);
}

//----------------------------------------------------------------------------//
RenderingRoot& ITFRenderer::getDefaultRenderingRoot()
{
    return *d_defaultRoot;
}

//----------------------------------------------------------------------------//
GeometryBuffer& ITFRenderer::createGeometryBuffer()
{
    ITFGeometryBuffer* b= new ITFGeometryBuffer(d_ITFAdapter);
    d_geometryBuffers.push_back(b);
    return *b;
}

//----------------------------------------------------------------------------//
void ITFRenderer::destroyGeometryBuffer(const GeometryBuffer& buffer)
{
    GeometryBufferList::iterator i = std::find(d_geometryBuffers.begin(),
                                               d_geometryBuffers.end(),
                                               &buffer);

    if (d_geometryBuffers.end() != i)
    {
        d_geometryBuffers.erase(i);
        delete &buffer;
    }
}

//----------------------------------------------------------------------------//
void ITFRenderer::destroyAllGeometryBuffers()
{
    while (!d_geometryBuffers.empty())
        destroyGeometryBuffer(**d_geometryBuffers.begin());
}

//----------------------------------------------------------------------------//
TextureTarget* ITFRenderer::createTextureTarget()
{
    TextureTarget* t = new ITFTextureTarget(*this);
    d_textureTargets.push_back(t);
    return t;
}

//----------------------------------------------------------------------------//
void ITFRenderer::destroyTextureTarget(TextureTarget* target)
{
    TextureTargetList::iterator i = std::find(d_textureTargets.begin(),
                                              d_textureTargets.end(),
                                              target);

    if (d_textureTargets.end() != i)
    {
        d_textureTargets.erase(i);
        delete target;
    }
}

//----------------------------------------------------------------------------//
void ITFRenderer::destroyAllTextureTargets()
{
    while (!d_textureTargets.empty())
        destroyTextureTarget(*d_textureTargets.begin());
}

//----------------------------------------------------------------------------//
ITF::GFXAdapter* ITFRenderer::getAdapter() const
{
    return d_ITFAdapter;
}

//----------------------------------------------------------------------------//
Texture& ITFRenderer::createTexture()
{
    ITFTexture* tex = new ITFTexture(*this);
    d_textures.push_back(tex);
    return *tex;
}

//----------------------------------------------------------------------------//
Texture& ITFRenderer::createTexture(const String& filename,
    const String& resourceGroup)
{
    ITFTexture* tex = new ITFTexture(*this, filename, resourceGroup);
    d_textures.push_back(tex);
    return *tex;
}

//----------------------------------------------------------------------------//
Texture& ITFRenderer::createTexture(const Size& size)
{
    ITFTexture* tex = new ITFTexture(*this, size);
    d_textures.push_back(tex);
    return *tex;
}

//----------------------------------------------------------------------------//
void ITFRenderer::destroyTexture(Texture& texture)
{
    TextureList::iterator i = std::find(d_textures.begin(),
                                        d_textures.end(),
                                        &texture);

    if (d_textures.end() != i)
    {
        d_textures.erase(i);
        delete &static_cast<ITFTexture&>(texture);
    }
}

//----------------------------------------------------------------------------//
void ITFRenderer::destroyAllTextures()
{
    while (!d_textures.empty())
        destroyTexture(**d_textures.begin());
}

//----------------------------------------------------------------------------//
void ITFRenderer::beginRendering()
{
    d_ITFAdapter->initDefaultRendering();

    // if enabled, restores a subset of the GL state back to default values.
    if (d_initExtraStates)
        setupExtraStates();
}

//----------------------------------------------------------------------------//
void ITFRenderer::endRendering()
{
    if (d_initExtraStates)
        cleanupExtraStates();

    d_ITFAdapter->restoreRendering();
}

//----------------------------------------------------------------------------//
const Size& ITFRenderer::getDisplaySize() const
{
    return d_displaySize;
}

//----------------------------------------------------------------------------//
const Vector2& ITFRenderer::getDisplayDPI() const
{
    return d_displayDPI;
}

//----------------------------------------------------------------------------//
uint ITFRenderer::getMaxTextureSize() const
{
    return d_maxTextureSize;
}

//----------------------------------------------------------------------------//
const String& ITFRenderer::getIdentifierString() const
{
    return d_rendererID;
}

//----------------------------------------------------------------------------//
Texture& ITFRenderer::createTexture(ITF::Texture *pTex, const Size& sz)
{
 
    ITFTexture* t = new ITFTexture(*this, pTex, sz);
    d_textures.push_back(t);
    return *t;
}

//----------------------------------------------------------------------------//
void ITFRenderer::setDisplaySize(const Size& sz)
{
    if (sz != d_displaySize)
    {
        d_displaySize = sz;

        // update the default target's area
        Rect area(d_defaultTarget->getArea());
        area.setSize(sz);
        d_defaultTarget->setArea(area);
    }
}

//----------------------------------------------------------------------------//
void ITFRenderer::enableExtraStateSettings(bool setting)
{
    d_initExtraStates = setting;
}

//----------------------------------------------------------------------------//
void ITFRenderer::setupExtraStates()
{
    getAdapter()->setupExtraStates();
}

//----------------------------------------------------------------------------//
void ITFRenderer::cleanupExtraStates()
{
    getAdapter()->cleanupExtraStates();
}
//----------------------------------------------------------------------------//
void ITFRenderer::grabTextures()
{
    TextureList::iterator i = d_textures.begin();
    while (i != d_textures.end())
    {
        (*i)->grabTexture();
        i++;
    }
}

//----------------------------------------------------------------------------//
void ITFRenderer::restoreTextures()
{
    TextureList::iterator i = d_textures.begin();
    while (i != d_textures.end())
    {
        (*i)->restoreTexture();
        i++;
    }
}
//----------------------------------------------------------------------------//

} // End of  CEGUI namespace section

