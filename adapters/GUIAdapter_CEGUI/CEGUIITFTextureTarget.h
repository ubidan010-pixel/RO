#ifndef _CEGUIITFTextureTarget_h_
#define _CEGUIITFTextureTarget_h_

#include "CEGUIITFRenderTarget.h"
#include "CEGUITextureTarget.h"
#include "CEGUIRect.h"

#if defined(_MSC_VER)
#   pragma warning(push)
#   pragma warning(disable : 4250)
#endif

// Start of CEGUI namespace section
namespace CEGUI
{
class ITFTexture;

/*!
\brief
    ITFTextureTarget - allows rendering to an ITF texture.
*/
class ITF_GUIRENDERER_API ITFTextureTarget : public ITFRenderTarget,
                                                        public TextureTarget
{
public:
    ITFTextureTarget(ITFRenderer& owner);
    virtual ~ITFTextureTarget();

    // overrides from ITFRenderTarget
    void activate();
    void deactivate();
    // implementation of RenderTarget interface
    bool isImageryCache() const;
    // implementation of TextureTarget interface
    void clear();
    Texture& getTexture() const;
    void declareRenderSize(const Size& sz);
    bool isRenderingInverted() const;

protected:
    //! default size of created texture objects
    static const float DEFAULT_SIZE;

    //! allocate and set up the texture used with the FBO.
    void initialiseRenderTexture();
    //! resize the texture
    void resizeRenderTexture();
    //! Clear Render Texture
    void clearRenderTexture();

    ITF::renderTarget* d_textureTarget;
 
    //! we use this to wrap d_texture so it can be used by the core CEGUI lib.
    ITFTexture* d_CEGUITexture;
};

} // End of  CEGUI namespace section

#if defined(_MSC_VER)
#   pragma warning(pop)
#endif

#endif  // end of guard _CEGUIITFTextureTarget_h_
