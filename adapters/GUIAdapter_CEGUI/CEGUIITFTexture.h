#ifndef _CEGUIITFTexture_h_
#define _CEGUIITFTexture_h_

#include "CEGUIBase.h"
#include "CEGUIRenderer.h"
#include "CEGUITexture.h"
#include "CEGUIITFRenderer.h"

// Start of CEGUI namespace section
namespace CEGUI
{
//! Texture implementation for the ITFRenderer.
class ITF_GUIRENDERER_API ITFTexture : public Texture
{
public:
    /*!
    \brief
        set the ITF texture that this Texture is based on to the specified
        texture, with the specified size.
    */
    void setITFTexture(ITF::Texture* tex, const Size& _size);

    /*!
    \brief
        Return the internal ITF texture id used by this Texture object.

    \return
        id of the ITF texture that this object is using.
    */
    ITF::Texture* getITFTexture() const;

    /*!
    \brief
        set the size of the internal texture.

    \param sz
        size for the internal texture, in pixels.

    \note
        Depending upon the hardware capabilities, the actual final size of the
        texture may be larger than what is specified when calling this function.
        The texture will never be smaller than what you request here.  To
        discover the actual size, call getSize.

    \exception RendererException
        thrown if the hardware is unable to support a texture large enough to
        fulfill the requested size.

    \return
        Nothing.
    */
    void setTextureSize(const Size& sz);

    /*!
    \brief
        Grab the texture to a local buffer.

        This will destroy the ITF texture, and restoreTexture must be called
        before using it again.
    */
    void grabTexture();

    /*!
    \brief
        Restore the texture from the locally buffered copy previously create by
        a call to grabTexture.
    */
    void restoreTexture();

    // implement abstract members from base class.
    const Size& getSize() const;
    const Size& getOriginalDataSize() const;
   
    /*!
    \brief
        Sets what the texture should consider as the original data size.

    \note
        This also causes the texel scaling values to be updated.
    */
    void setOriginalDataSize(const Size& sz);
    
    
    const Vector2& getTexelScaling() const;
    void loadFromFile(const String& filename, const String& resourceGroup);
    void loadFromMemory(const void* buffer, const Size& buffer_size,
                        PixelFormat pixel_format);
    void saveToMemory(void* buffer);
 
protected:
    // Friends (to allow construction and destruction)
    friend Texture& ITFRenderer::createTexture(void);
    friend Texture& ITFRenderer::createTexture(const String&, const String&);
    friend Texture& ITFRenderer::createTexture(const Size&);
    friend Texture& ITFRenderer::createTexture(ITF::Texture*, const Size&);
    friend void ITFRenderer::destroyTexture(Texture&);

    //! Basic constructor.
    ITFTexture(ITFRenderer& owner);
    //! Constructor that creates a Texture from an image file.
    ITFTexture(ITFRenderer& owner, const String& filename, const String& resourceGroup);
    //! Constructor that creates a Texture with a given size.
    ITFTexture(ITFRenderer& owner, const Size& size);
    //! Constructor that wraps an existing ITF texture.
    ITFTexture(ITFRenderer& owner, ITF::Texture *pTex, const Size& size);
    //! Destructor.
    virtual ~ITFTexture();

    //! generate the ITF texture and set some initial options.
    void generateITFTexture();

    //! updates cached scale value used to map pixels to texture co-ords.
    void updateCachedScaleValues();

    //! returns next power of 2 size if \a size is not power of 2
    float getSizeNextPOT(float sz) const;

    //! clean up the ITF texture, or the grab buffer if it had been grabbed
    void cleanupITFTexture();

    //!ITFRenderer object that created and owns this texture.
    ITFRenderer& d_owner;
    //! The ITF texture we're wrapping.
    ITF::Texture* d_texture;
    
    //! Size of the texture.
    Size d_size;
    //! cached image data for restoring the texture.
    uint8* d_grabBuffer;
    //! original pixel of size data loaded into texture
    Size d_dataSize;
    //! cached pixel to texel mapping scale values.
    Vector2 d_texelScaling;
};

} // End of  CEGUI namespace section


#endif // end of guard _CEGUIITFTexture_h_
