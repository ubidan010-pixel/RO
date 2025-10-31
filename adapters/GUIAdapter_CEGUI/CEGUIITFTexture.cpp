#include "precompiled_GUIAdapter_CEGUI.h"

#ifndef _ITF_SINGLETONS_H_
    #include "core/Singletons.h"
#endif // _ITF_SINGLETONS_H_

#ifndef _ITF_GFX_ADAPTER_H_
    #include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif // _ITF_GFX_ADAPTER_H_

#ifndef _ITF_RESOURCEMANAGER_H_
    #include "engine\resources\ResourceManager.h"
#endif // _ITF_RESOURCEMANAGER_H_


#include "CEGUIITFTexture.h"
#include "CEGUIExceptions.h"
#include "CEGUISystem.h"
#include "CEGUIImageCodec.h"

// Start of CEGUI namespace section
namespace CEGUI
{
//----------------------------------------------------------------------------//
ITFTexture::ITFTexture(ITFRenderer& owner) :
    d_owner(owner),
    d_texture(0),
    d_size(0, 0),
    d_grabBuffer(0),
    d_dataSize(0, 0),
    d_texelScaling(0, 0)
{
   generateITFTexture();
}

//----------------------------------------------------------------------------//
ITFTexture::ITFTexture(ITFRenderer& owner,
                       const String& filename,
                       const String& resourceGroup) :
    d_owner(owner),
    d_size(0, 0),
    d_grabBuffer(0),
    d_dataSize(0, 0)
{
    generateITFTexture();
    loadFromFile(filename, resourceGroup);
}

//----------------------------------------------------------------------------//
ITFTexture::ITFTexture(ITFRenderer& owner, const Size& size) :
    d_owner(owner),
    d_size(0, 0),
    d_grabBuffer(0),
    d_dataSize(0, 0)
{
    generateITFTexture();
    setTextureSize(size);
}

//----------------------------------------------------------------------------//
ITFTexture::ITFTexture(ITFRenderer& owner,
                       ITF::Texture *pTex,
                       const Size& size) :
    d_owner(owner),
    d_texture(pTex),
    d_size(size),
    d_grabBuffer(0),
    d_dataSize(size)
{
    setITFTexture(pTex, size);
    if (pTex)
    {
        owner.getAdapter()->updateCachedScaleValues(d_texture);
        d_size.d_width = (float)d_texture->m_sizeX;
        d_size.d_height = (float)d_texture->m_sizeY;
        d_dataSize.d_width = (float)d_texture->m_datasizeX;
        d_dataSize.d_height = (float)d_texture->m_datasizeY;
        d_texelScaling.d_x = (float)d_texture->m_texelScaling.m_x;
        d_texelScaling.d_y = (float)d_texture->m_texelScaling.m_y;
    }
}

//----------------------------------------------------------------------------//
ITFTexture::~ITFTexture()
{
    cleanupITFTexture();
}

//----------------------------------------------------------------------------//
const Size& ITFTexture::getSize() const
{
    return d_size;
}

//----------------------------------------------------------------------------//
const Size& ITFTexture::getOriginalDataSize() const
{
    return d_dataSize;
}

//----------------------------------------------------------------------------//

void ITFTexture::setOriginalDataSize(const Size& sz)
{
    d_dataSize = sz;
    if (d_texture)
    {
        d_texture->m_datasizeX = (unsigned int)d_dataSize.d_width;
        d_texture->m_datasizeY = (unsigned int)d_dataSize.d_height;
        d_owner.getAdapter()->updateCachedScaleValues(d_texture);
        d_texelScaling.d_x = (float)d_texture->m_texelScaling.m_x;
        d_texelScaling.d_y = (float)d_texture->m_texelScaling.m_y;
    }
    else
    {
        d_texelScaling.d_x = 0;
        d_texelScaling.d_y = 0;
    }
}

//----------------------------------------------------------------------------//
const Vector2& ITFTexture::getTexelScaling() const
{
    return d_texelScaling;
}

//----------------------------------------------------------------------------//
void ITFTexture::loadFromFile(const String& filename,
    const String& resourceGroup)
{
    // Note from PDT:
    // There is somewhat tight coupling here between OpenGLTexture and the
    // ImageCodec classes - we have intimate knowledge of how they are
    // implemented and that knowledge is relied upon in an unhealthy way; this
    // should be addressed at some stage.

    // load file to memory via resource provider
    RawDataContainer texFile;
    System::getSingleton().getResourceProvider()->
        loadRawDataContainer(filename, texFile, resourceGroup);

    // get and check existence of CEGUI::System (needed to access ImageCodec)
    System* sys = System::getSingletonPtr();
    if (!sys)
        throw RendererException("ITFTexture::loadFromFile - "
                                "CEGUI::System object has not been created: "
                                "unable to access ImageCodec.");

    Texture* res = sys->getImageCodec().load(texFile, this);

    // unload file data buffer
    System::getSingleton().getResourceProvider()->
        unloadRawDataContainer(texFile);

    if (!res)
        // It's an error
        throw RendererException("ITFTexture::loadFromFile - " +
                                sys->getImageCodec().getIdentifierString()+
                                " failed to load image '" + filename + "'.");

}

//----------------------------------------------------------------------------//
void ITFTexture::loadFromMemory(const void* buffer, const Size& buffer_size,
                    PixelFormat pixel_format)
{
    // Convert to ITF::Size.
    ITF::Size buf_size(buffer_size.d_width, buffer_size.d_height);
    d_owner.getAdapter()->loadFromMemory(buffer, buf_size, d_texture, (ITF::Texture::PixFormat)pixel_format,0);

    d_size.d_width = (float)d_texture->m_sizeX;
    d_size.d_height = (float)d_texture->m_sizeY;
    d_dataSize.d_width = (float)d_texture->m_datasizeX;
    d_dataSize.d_height = (float)d_texture->m_datasizeY;
    d_texelScaling.d_x = (float)d_texture->m_texelScaling.m_x;
    d_texelScaling.d_y = (float)d_texture->m_texelScaling.m_y; 
}

//----------------------------------------------------------------------------//
void ITFTexture::saveToMemory(void* buffer)
{
    d_owner.getAdapter()->saveToMemory(buffer, d_texture);
}

//----------------------------------------------------------------------------//
void ITFTexture::setTextureSize(const Size& sz)
{
    // Convert to ITF::Size.
    ITF::Size size(sz.d_width, sz.d_height);
    ITF::Size sizeout;

    d_owner.getAdapter()->setTextureSize(d_texture, size);

    d_dataSize.d_height = d_size.d_height = sizeout.d_height;
    d_dataSize.d_width = d_size.d_width = sizeout.d_width;
    d_owner.getAdapter()->updateCachedScaleValues(d_texture);
    d_size.d_width = (float)d_texture->m_sizeX;
    d_size.d_height = (float)d_texture->m_sizeY;
    d_dataSize.d_width = (float)d_texture->m_datasizeX;
    d_dataSize.d_height = (float)d_texture->m_datasizeY;
    d_texelScaling.d_x = (float)d_texture->m_texelScaling.m_x;
    d_texelScaling.d_y = (float)d_texture->m_texelScaling.m_y;
}

//----------------------------------------------------------------------------//
void ITFTexture::grabTexture()
{
    d_grabBuffer = d_owner.getAdapter()->grabTexture(d_texture);
}

//----------------------------------------------------------------------------//
void ITFTexture::restoreTexture()
{
    d_owner.getAdapter()->restoreTexture(d_grabBuffer, d_texture);
}

//----------------------------------------------------------------------------//

void ITFTexture::updateCachedScaleValues()
{
    //
    // calculate what to use for x scale
    //
    const float orgW = d_dataSize.d_width;
    const float texW = d_size.d_width;

    // if texture and original data width are the same, scale is based
    // on the original size.
    // if texture is wider (and source data was not stretched), scale
    // is based on the size of the resulting texture.
    d_texelScaling.d_x = 1.0f / ((orgW == texW) ? orgW : texW);

    //
    // calculate what to use for y scale
    //
    const float orgH = d_dataSize.d_height;
    const float texH = d_size.d_height;

    // if texture and original data height are the same, scale is based
    // on the original size.
    // if texture is taller (and source data was not stretched), scale
    // is based on the size of the resulting texture.
    d_texelScaling.d_y = 1.0f / ((orgH == texH) ? orgH : texH);
}

//----------------------------------------------------------------------------//
float ITFTexture::getSizeNextPOT(float sz) const
{
    return d_owner.getAdapter()->getSizeNextPOT(sz);
}

//----------------------------------------------------------------------------//
void ITFTexture::generateITFTexture()
{
    d_texture = (ITF::Texture*)ITF::RESOURCE_MANAGER->newEmptyResource(ITF::Resource::ResourceType_Texture);
    d_owner.getAdapter()->generateTexture(d_texture);
    //d_texture->addUser();
}

//----------------------------------------------------------------------------//
void ITFTexture::cleanupITFTexture()
{
    // if the grabbuffer is not empty then free it
    if (d_grabBuffer)
    {
        delete[] d_grabBuffer;
        d_grabBuffer = 0;
    }
    // otherwise delete any texture associated with this object.
    else
    {
        if (d_texture)
        {
            d_owner.getAdapter()->cleanupBaseTexture(d_texture);
            ITF::RESOURCE_MANAGER->removeUserToResourcePhysicalData(d_texture);
            d_texture = 0;
        }
    }
}

//----------------------------------------------------------------------------//
ITF::Texture* ITFTexture::getITFTexture() const
{
    return d_texture;
}

//----------------------------------------------------------------------------//
void ITFTexture::setITFTexture(ITF::Texture* tex, const Size& _size)
{
    if (!tex) return;

    ITF::Size size(_size.d_width, _size.d_height);
    d_owner.getAdapter()->bindTexturefromTexture(&d_texture, tex, size);

    if (!d_texture ) return;

    d_size.d_width = (float)d_texture->m_sizeX; 
    d_size.d_height = (float)d_texture->m_sizeY; 
    d_dataSize.d_width = (float)d_texture->m_datasizeX;
    d_dataSize.d_height = (float)d_texture->m_datasizeY;
    d_texelScaling.d_x = (float)d_texture->m_texelScaling.m_x;
    d_texelScaling.d_y = (float)d_texture->m_texelScaling.m_y;
}

} // End of  CEGUI namespace section
