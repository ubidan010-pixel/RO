#ifndef _CEGUIITFRENDERER_H_
#define _CEGUIITFRENDERER_H_

#include "CEGUIBase.h"
#include "CEGUIRenderer.h"
#include "CEGUISize.h"
#include "CEGUIVector.h"

#include <vector>

#ifndef _ITF_TEXTURE_H_
#include "engine/display/Texture.h"
#endif // _ITF_TEXTURE_H_

#include "adapters/GFXAdapter_OpenGL/GFXAdapter_OpenGL.h"

#define ITF_GUIRENDERER_API

#if defined(_MSC_VER)
#   pragma warning(push)
#   pragma warning(disable : 4251)
#endif


// Start of CEGUI namespace section
namespace CEGUI
{
class ITFTexture;
class ITFTextureTarget;
class ITFGeometryBuffer;
// ?? class OGLTextureTargetFactory;

/*!
\brief
    Renderer class to interface with ITF
*/
class ITF_GUIRENDERER_API ITFRenderer : public Renderer
{
public:
    /*!
    \brief
        Create an OpenGLRenderer object.
    */
    static ITFRenderer& create(ITF::GFXAdapter*    ITFAdapter);

    /*!
    \brief
        Destroy an ITFRenderer object.

    \param renderer
        The ITFRenderer object to be destroyed.
    */
    static void destroy(ITFRenderer& renderer);

    //! return the Adapter used by this renderer object.
    ITF::GFXAdapter* getAdapter() const;

    // implement Renderer interface
    RenderingRoot& getDefaultRenderingRoot();
    GeometryBuffer& createGeometryBuffer();
    void destroyGeometryBuffer(const GeometryBuffer& buffer);
    void destroyAllGeometryBuffers();
    TextureTarget* createTextureTarget();
    void destroyTextureTarget(TextureTarget* target);
    void destroyAllTextureTargets();
    Texture& createTexture();
    Texture& createTexture(const String& filename, const String& resourceGroup);
    Texture& createTexture(const Size& size);
    void destroyTexture(Texture& texture);
    void destroyAllTextures();
    void beginRendering();
    void endRendering();
    void setDisplaySize(const Size& sz);
    const Size& getDisplaySize() const;
    const Vector2& getDisplayDPI() const;
    uint getMaxTextureSize() const;
    const String& getIdentifierString() const;

    /*!
    \brief
        Create a texture that uses an existing ITF texture with the specified
        size.  Note that it is your responsibility to ensure that the OpenGL
        texture is valid and that the specified size is accurate.

    \param sz
        Size object that describes the pixel size of the OpenGL texture
        identified by \a tex.

    \return
        Texture object that wraps the ITF texture \a tex, and whose size is
        specified to be \a sz.
    */
    Texture& createTexture(ITF::Texture *pTex, const Size& sz);

    /*!
    \brief
        Tells the renderer to initialise some extra states beyond what it
        directly needs itself for CEGUI.

        This option is useful in cases where you've made changes to the default
        OpenGL state and do not want to save/restore those between CEGUI
        rendering calls.  Note that this option will not deal with every
        possible state or extension - if you want a state added here, make a
        request and we'll consider it ;)
    */
    void enableExtraStateSettings(bool setting);

    /*!
    \brief
        Grabs all the loaded textures from Texture RAM and stores them in a
        local data buffer.  This function invalidates all textures, and
        restoreTextures must be called before any CEGUI rendering is done for
        predictable results.
    */
    void grabTextures();

    /*!
    \brief
        Restores all the loaded textures from the local data buffers previously
        created by 'grabTextures'
    */
    void restoreTextures();

private:
    /*!
    \brief
        Constructor for ITF Renderer objects

    /*!
    \brief
        Constructor for OpenGL Renderer objects.

    \param display_size
        Size object describing the initial display resolution.

    \param tt_type
        Specifies one of the TextureTargetType enumerated values indicating the
        desired TextureTarget type to be used.
    */
    ITFRenderer(ITF::GFXAdapter* ITFAdapter/*const Size& display_size, const TextureTargetType tt_type*/);//adapter

    /*!
    \brief
        Destructor for ITFRenderer objects
    */
    virtual ~ITFRenderer();

    //! init the extra GL states enabled via enableExtraStateSettings
    void setupExtraStates();

    //! cleanup the extra GL states enabled via enableExtraStateSettings
    void cleanupExtraStates();

    //! String holding the renderer identification text.
    static String d_rendererID;
    //! ITF Adapter
    ITF::GFXAdapter*    d_ITFAdapter;
    //! What the renderer considers to be the current display size.
    Size d_displaySize;
    //! What the renderer considers to be the current display DPI resolution.
    Vector2 d_displayDPI;
    //! The default rendering root object
    RenderingRoot* d_defaultRoot;
    //! The default RenderTarget (used by d_defaultRoot)
    RenderTarget* d_defaultTarget;
    //! container type used to hold TextureTargets we create.
    typedef std::vector<TextureTarget*> TextureTargetList;
    //! Container used to track texture targets.
    TextureTargetList d_textureTargets;
    //! container type used to hold GeometryBuffers we create.
    typedef std::vector<ITFGeometryBuffer*> GeometryBufferList;
    //! Container used to track geometry buffers.
    GeometryBufferList d_geometryBuffers;
    //! container type used to hold Textures we create.
    typedef std::vector<ITFTexture*> TextureList;
    //! Container used to track textures.
    TextureList d_textures;
    //! What the renderer thinks the max texture size is.
    uint d_maxTextureSize;
    //! option of whether to initialise extra states that may not be at default
    bool d_initExtraStates;
  };

} // End of  CEGUI namespace section

#if defined(_MSC_VER)
#   pragma warning(pop)
#endif

#endif // end of guard _CEGUIITFRENDERER_H_
