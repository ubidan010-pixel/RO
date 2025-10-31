#if defined (ITF_PS3)

#ifndef ITF_GFXADAPTER_GCM_H
#define ITF_GFXADAPTER_GCM_H

#include <sysutil/sysutil_sysparam.h>

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_SYNCHRONIZE_H_
#include "engine/system/Synchronize.h"
#endif //_ITF_SYNCHRONIZE_H_

#ifndef _ITF_GFXADAPTER_SHADERMANAGER_H_
#include "engine/AdaptersInterfaces/GFXAdapter_ShaderManager.h"
#endif //_ITF_GFXADAPTER_SHADERMANAGER_H_

#include "FxParameterDB.h"
#include "VertexDecl_GCM.h"
#include "RenderContext_GCM.h"
#include "WorldViewProj_GCM.h"

#include "Tools_GCM.h"
#include "VRAMAllocator_GCM.h"
#include "Surface_GCM.h"
#include "Label_GCM.h"
#include "Report_GCM.h"
#include "FlipMgr_GCM.h"

#define _USE_SHADER

#ifndef ITF_DISABLE_DEBUGINFO
    #define _USE_FONTDEBUG
#endif //!ITF_DISABLE_DEBUGINFO

namespace ITF
{

#define IMPLEMENTED_NOT_TESTED ITF_ASSERT(0)

#define IMPROVE_THIS 
    //do { printf(__FUNCTION__);printf(" to improved\n"); } while(0);
#define IMPLEMENT_THIS 
    //do { printf(__FUNCTION__);printf(" not implemented\n"); } while(0);
#undef NOT_IMPLEMENTED_ON_GCM 
#define NOT_IMPLEMENTED_ON_GCM IMPLEMENT_THIS
    
// GFXA_OVERRIDE  GFXAdapter methods are tagged
#define GFXA_OVERRIDE 

class GFXAdapter_GCM : public GFXAdapter
{
public:

    static const u32 MaxCPUFrameInAdvance = 1; // the maximum frame the CPU can take in advance
    static const bool CPUSyncOnFlip = false; // set to true to get the sync at flip
    // activate after fx buffers depending on what to do
    static const bool UseAfterFXDown2x2 = true;
    static const bool UseAfterFXDown4x4 = false;
    static const bool UseAfterFXFullRT = true;
    static const bool UseRefract2x2 = false;

    ITF_COMPILE_TIME_ASSERT(Refract2x2, !UseRefract2x2 || UseAfterFXDown2x2);
    
    GFXAdapter_GCM();
    virtual ~GFXAdapter_GCM();

    //////////////////////////////////////////////////////////////////////////
    // Override methods
    //////////////////////////////////////////////////////////////////////////
    GFXA_OVERRIDE  void         init();

    bool         isSucessfullyInit() { return m_successfullInit; }

    GFXA_OVERRIDE  void         destroy();
    GFXA_OVERRIDE  void         destroyResources();

    GFXA_OVERRIDE  void         project                 (const Vec3d& _in3d, Vec2d & _out2d);
    GFXA_OVERRIDE  void         coord2DTo3D             (const Vec2d& _in2d, float _zPlane, Vec2d& _out3d, bool noTranslation = btrue);
    GFXA_OVERRIDE  void         drawSphere              (float _x, float _y, float _z, float _radius, int lats, int longs, int _wireFrame, float _r=1, float _g=1, float _b=1  );
    GFXA_OVERRIDE  void         drawSphere              (float _x, float _y, float _radius, float _r=1, float _g=1, float _b=1, float _z = 0.f, u32 _segsCount = 16, bbool _volume = btrue, bbool _xaxis = btrue, bbool _yaxis = btrue, bbool _zaxis = btrue );
    GFXA_OVERRIDE  void         drawCircle              (float _x, float _y, float r, float _r=1, float _g=1, float _b=1, float _z = 0.f, u32 _segsCount = 32  );
    GFXA_OVERRIDE  void         drawBox                 (const Vec2d& _pos, f32 _angle, const Vec2d& _extent, f32 _r=1, f32 _g=1, f32 _b=1, float _z = 0.f, bbool _fill = bfalse);
    GFXA_OVERRIDE  void         drawLine                (const Vec2d& _p1, const Vec2d& _p2, float _r=1, float _g=1, float _b=1, float _size = 1.f, float _z = 0.f, float _alpha = 1.f );
    GFXA_OVERRIDE  void         draw2dLine              (const Vec2d& _p1, const Vec2d& _p2, float _size = 1.f, u32 _color = COLOR_WHITE);
    GFXA_OVERRIDE  void         draw2dTriangle          (const Vec2d& _p1, const Vec2d& _p2, const Vec2d& _p3, u32 _color = COLOR_WHITE, i32 _rank = 0);
    GFXA_OVERRIDE  void         draw2dBox               (const Vec2d& _p, f32 _width, f32 _height, u32 _color0 = COLOR_WHITE, u32 _color1 = COLOR_WHITE, u32 _color2 = COLOR_WHITE, u32 _color3 = COLOR_WHITE, i32 _rank = 0);
    GFXA_OVERRIDE  void         drawTriangle            (const Vec2d& _p1, const Vec2d& _p2, const Vec2d& _p3, float _r=1, float _g=1, float _b=1, float _a=1, float _z = 0.f    );
    GFXA_OVERRIDE  void         drawTriangle            (const Vertex& _v1, const Vertex& _v2, const Vertex& _v3, const Texture* _tex, bbool _showWireFrame = 0);
    
    GFXA_OVERRIDE  void         drawQuad2D              (f32 _px, f32 _py, f32 _width, f32 _height, f32 _z, u32 _color = COLOR_WHITE, const Texture* _tex = NULL, const Vec2d* _uvStart = NULL,  const Vec2d* _uvEnd = NULL );
    GFXA_OVERRIDE  void         draw3DQuad              (GFX_3DQUAD* _t_Quad ); 

    GFXA_OVERRIDE  bbool        loadTexture             (Texture* _tex, const char* _path, u32 _alpha = 0 );

    GFXA_OVERRIDE  void         compute3DTo2D           (const Vec3d& _in, Vec3d& _out);
    GFXA_OVERRIDE  void         compute2DTo3D           (const Vec3d& _in, Vec3d& _out);


    GFXA_OVERRIDE  void         drawScreenQuad          (f32 _px, f32 _py, f32 _width, f32 _height, f32 _z, u32 _color, bbool _noTex = 0 );
                   void         drawScreenQuadC         (f32 _px, f32 _py, f32 _width, f32 _height, f32 _z, u32 _color, f32 _centroid, bbool _noTex = 0);

    GFXA_OVERRIDE  void         setCamera               (float _x, float _y, float _z, Camera* _cam );

    GFXA_OVERRIDE  void         setBiasEyeLookAt        (const Vec3d & lookAtDir);


    GFXA_OVERRIDE  void         init2DRender            ();
	GFXA_OVERRIDE  void		    init2DGuiRender         ();

    GFXA_OVERRIDE  void         setOrthoView            (f32 _l, f32 _r, f32 _b, f32 _t);
	GFXA_OVERRIDE  void		    setOrthoView2DYNeg      (f32 _l, f32 _r, f32 _b, f32 _t);

    GFXA_OVERRIDE  bbool        isDrawingForScreenshot  ();

    GFXA_OVERRIDE  void         setAlphaBlend           (GFX_BLENDMODE _Blend);
    GFXA_OVERRIDE  void         setCullMode             (GFX_CULLMODE _cull);
    GFXA_OVERRIDE  void         setFillMode             (GFX_FILLMODE _fillmode);

    GFXA_OVERRIDE  void         depthMask               (u32 _depthMask);
    GFXA_OVERRIDE  void         depthFunc               (u32 _func);
    GFXA_OVERRIDE  void         depthTest               (u32 _test);

    GFXA_OVERRIDE  void         setScissorRect          (GFX_RECT* _clipRect);
    GFXA_OVERRIDE  void         SetTextureBind          (int _Sampler, unsigned int _Bind, bbool _linearFiltering = btrue);
                   // following lmethods are version without virtual
            inline void         internSetTextureBind    (u32 _Sampler, u32 _Bind, bbool _linearFiltering = btrue);
            inline void         internSetTextureAdressingMode(u32 _samp, GFX_TEXADRESSMODE _u, GFX_TEXADRESSMODE _v);

    GFXA_OVERRIDE  void         lockTexture             (Texture* _tex, LOCKED_TEXTURE* _lockTex, u32 _flag);
    GFXA_OVERRIDE  void         unlockTexture           (Texture* _tex);

    GFXA_OVERRIDE  void         cleanupTexture          (Texture* _texture);
    GFXA_OVERRIDE  void         DrawPrimitive           (PRIMITIVETYPE _type, const void* _p_Vertex, u32 _NumberVertex);

    // Matrix.
    GFXA_OVERRIDE  void         setMatrixTransform      (MATRIXMODE _type, const GMatrix44* _Matrix);
    GFXA_OVERRIDE  void         setupViewport           (GFX_RECT* _Viewport);
    GFXA_OVERRIDE  void         unprojectPoint          (const Vec2d& _in, float _viewDistance, const GMatrix44* _MatrixModel, const GMatrix44* _MatrixProj, const int* _View, Vec2d& _out);
    GFXA_OVERRIDE  void         setLookAtMatrix         (GMatrix44* _MatrixOut, double _midx, double _midy, float _viewDistance, float _aspect);
    GFXA_OVERRIDE  void         getViewport             (GFX_RECT* _RectOut);

    /// GfxMaterials.
    GFXA_OVERRIDE  void         setGfxMaterial          (const GFX_MATERIAL& _gfxMat);
    GFXA_OVERRIDE  void         setGfxMatDefault        ();
    GFXA_OVERRIDE  void         setGfxMatRefraction     (const GFX_MATERIAL& _gfxMat);

    /// Texture.
    GFXA_OVERRIDE  void         generateTexture         (Texture* _texture);
    GFXA_OVERRIDE  void         createTexture           (Texture * _texture, u32 _sizeX, u32 _sizeY, u32 _mipLevel, Texture::PixFormat _pixformat, u32 _pool, bbool _dynamic);
    GFXA_OVERRIDE  void         cleanupBaseTexture      (Texture* _texture);
    GFXA_OVERRIDE  void         setTextureSize          (Texture* _texture, const Size& _size   );
    
    GFXA_OVERRIDE  unsigned char*  grabTexture          (Texture* _texture);
    GFXA_OVERRIDE  u32          getMaxTextureSize       ();
    GFXA_OVERRIDE  void         loadFromMemory          (const void* _buffer, const Size& _texture_size, Texture* _texture, Texture::PixFormat _pixel_format, const u32 _buffer_size, bbool bCooked, u32 _mipmapCount);
    GFXA_OVERRIDE  bbool        loadFromFileInMemory    (Texture *_texture, const void* _buffer, u32 _contentSize) {ITF_ASSERT(0);return bfalse;}
    GFXA_OVERRIDE  void         SetTexture              (int _Sampler, Texture* _Texture, bbool _linearFiltering = btrue);
    GFXA_OVERRIDE  void         saveToMemory            (void* _buffer, Texture* _Tex);

    GFXA_OVERRIDE  float        getTexelOffset          () { return 0.5f; };
    GFXA_OVERRIDE  void         setSamplerState         (u32 _samp, GFX_SAMPLERSTATETYPE _type, i32 _value);
    // dispatch to one of the following
            inline void         setSamplerAddressState   (u32 _samp, u32 _uWrap, u32 _vWrap);
                   void         setSamplerFilterState    (u32 _samp, u32 _minFilter, u32 _magFilter);

    // Texture Target.
    GFXA_OVERRIDE  void         clearRenderTarget       (renderTarget* _target, u32 _color   );
    GFXA_OVERRIDE  void         initialiseRenderTarget  (renderTarget* _target);
    GFXA_OVERRIDE  void         resizeRenderTarget      (renderTarget* _target, const Size& _size    );
    GFXA_OVERRIDE  void         enableRenderTarget      (renderTarget* _target);
    GFXA_OVERRIDE  void         disableRenderTarget     (renderTarget* _target);
    GFXA_OVERRIDE  void         cleanupRenderTarget     (renderTarget* _target);
    GFXA_OVERRIDE  bbool        isRenderTargetInverted  () { return bfalse; };

    GFXA_OVERRIDE  void         clear                   (u32 _buffer, float _r=0.f, float _g=0.f, float _b=0.f, float _a=0.f);

    GFXA_OVERRIDE  void         present                 ();
    GFXA_OVERRIDE  void         startRendering          ();
    GFXA_OVERRIDE  void         endRendering            ();

    GFXA_OVERRIDE  void         reshape();
    GFXA_OVERRIDE  void         createDebugFonts        (u32 _screenwidth, u32 _screenHeight) {};

    GFXA_OVERRIDE  bbool        isDeviceLost            ();
    GFXA_OVERRIDE  bbool        resetDevice             (bbool _forceReset);

    /// Vertex declaration.
    GFXA_OVERRIDE  void         setVertexFormat         (u32 _vformat);

    /// identify GFXAdapter
    GFXA_OVERRIDE  const String getRenderName           () { return "GCM"; }

    /// Shaders.
    GFXA_OVERRIDE  void         createShader            (ITF_shader* _shader);
    GFXA_OVERRIDE  void         releaseShader           (ITF_shader* _shader);
    GFXA_OVERRIDE  bbool        loadShader              (ITF_shader* _shader);
    GFXA_OVERRIDE  void         beginShader             (ITF_shader* _shader);
    GFXA_OVERRIDE  void         endShader               (ITF_shader* _shader);

    GFXA_OVERRIDE  void         fx_setVectorArray       (u32 _fx, void* _h, const GFX_Vector4* _va, u32 _count);
    GFXA_OVERRIDE  void         fx_setVector            (u32 _fx, void* _h, const GFX_Vector4* _v);   
    GFXA_OVERRIDE  void         fx_setFloat             (u32 _fx, void* _h, float _f)  ;  
    GFXA_OVERRIDE  void         fx_setInt               (u32 _fx, void* _h, i32 _i)  ;  
    GFXA_OVERRIDE  void         fx_commitChange         (u32 _fx );

    /// Patch shader.
                   void         prepareShaderPatchTech  (GMatrix44* _matrix, f32 _z, u32 & _hdiv, u32 & _vdiv , u32 _idTech, GFX_BLENDMODE _blendMode = GFX_BLEND_ALPHA);
                   void         shaderPatchFlushSize    (u32 _hdiv, u32 _vdiv, u32 countPoint, u32 _pass, u32 _vdivToDraw = U32_INVALID);

    GFXA_OVERRIDE  void         prepareShaderPatch      (GMatrix44* _matrix, f32 _z, u32 & _hdiv, u32 & _vdiv, GFX_BLENDMODE _blendMode = GFX_BLEND_ALPHA);
    GFXA_OVERRIDE  void         shaderDrawPatch32       (const Texture *texture, const Vec2d *PointsNS, const ColorInteger* _lighttab);
    GFXA_OVERRIDE  void         endShaderPatch          ();
    GFXA_OVERRIDE  void         shaderPatchFlush        (u32 _hdiv, u32 _vdiv, u32 _pass);

    // Patch bezier shader
    GFXA_OVERRIDE  void        prepareShaderBezierPatch (GMatrix44* _matrix, f32 _z, u32 & _hdiv, u32 & _vdiv, GFX_BLENDMODE _blendMode = GFX_BLEND_ALPHA);    
    GFXA_OVERRIDE  bbool       shaderDrawBezierPatch32  (Texture *texture, Vec2d *PointsNS, ColorInteger* _lighttab, f32* _widths, f32* _alphas, f32* _widths2 );
    GFXA_OVERRIDE  void        shaderBezierPatchFlush   (u32 _hdiv, u32 _vdiv, u32 _pass, u32 _vDivToDraw);

    /// Spline Shader
    GFXA_OVERRIDE  void         drawSpline              (GMatrix44* _matrix, Texture* _texture, const Spline * _spline, const Vec3d &_pos, f32 _height);

    /// Fluid Shader.
    GFXA_OVERRIDE  void         drawFluid               (GMatrix44* _matrix, Texture* _texture, const FixedArray<u32, GFXADAPTER_FLUID_COLORTAB_ENTRY_COUNT> &_colorTab, const Vec2d _heightTab[], const u32 _heightTabSize, const FixedArray<Vec2d, 4> &_uvTab, u32 mode, f32 _delta1, f32 _delta2, f32 _stepCount );

    /// Trail 3d Shader.
    GFXA_OVERRIDE  void         drawTrail3D             (Texture* _texture, Color _color, const SafeArray<Vec3d> & points, f32 _alphaBegin, f32 _alphaEnd, f32 _fadeLength);

    /// Movie full screen quad
    GFXA_OVERRIDE  void         drawMovie               (GMatrix44* _matrix, f32 _alpha, ITF_VertexBuffer* _pVertexBuffer, Texture** _pTextureArray, u32 _countTexture);

    /// VertexBuffer.
    GFXA_OVERRIDE  void         createVertexBuffer      (ITF_VertexBuffer* _vertexBuffer);
    GFXA_OVERRIDE  void         releaseVertexBuffer     (ITF_VertexBuffer* _vertexBuffer);
                   void         setVertexBuffer         (ITF_VertexBuffer* _vertexBuffer); // no more virtual, set the current VB
    GFXA_OVERRIDE  void         LockVertexBuffer        (ITF_VertexBuffer* _vertexBuffer, void** _data, u32 _offset, u32 _size, u32 _flag);
    GFXA_OVERRIDE  void         UnlockVertexBuffer      (ITF_VertexBuffer* _vertexBuffer );

    /// IndexBuffer.
    GFXA_OVERRIDE  void         createIndexBuffer       (ITF_IndexBuffer* _indexBuffer);
    GFXA_OVERRIDE  void         releaseIndexBuffer      (ITF_IndexBuffer* _indexBuffer);
                   void         setIndexBuffer          (ITF_IndexBuffer* _indexBuffer); // no more virtual, set the current IB
    GFXA_OVERRIDE  void         LockIndexBuffer         (ITF_IndexBuffer* _indexBuffer, void** _data);
    GFXA_OVERRIDE  void         UnlockIndexBuffer       (ITF_IndexBuffer* _indexBuffer);

    /// Vertex/Index Buffer Draw.
    GFXA_OVERRIDE  void         DrawVertexBuffer        (u32 _type, ITF_VertexBuffer* _vertexBuffer, u32 _vertexStart, u32 _vertexNumber);
    GFXA_OVERRIDE  void         DrawIndexedVertexBuffer (u32 _type, ITF_VertexBuffer* _vertexBuffer, ITF_IndexBuffer* _indexBuffer, u32 _indexNumber);
    GFXA_OVERRIDE  void         drawMesh                (ITF_Mesh& _mesh, bbool _showWireFrame);
    GFXA_OVERRIDE  void         drawMeshPT              (const ITF_Mesh& _mesh, bbool _showWireFrame);
#ifndef ITF_CONSOLE_FINAL
    GFXA_OVERRIDE  void         drawFlatMesh            (const ITF_Mesh& _mesh);
#endif // ITF_CONSOLE_FINAL

    GFXA_OVERRIDE  void         setShaderForFont        (bbool bOutline);
    GFXA_OVERRIDE  void         restoreDefaultShader    ();

    GFXA_OVERRIDE  const GMatrix44*   getViewMatrix           ();

                   void         copyCurrentColorBuffer  (u32 _rt);
                   void         copyTexture             (int *TexHan);
                   void         validateTexture         (int *TexHan,int Mode,int ScreenSize,int ScreenSize_V);
    GFXA_OVERRIDE  void         AFTERFX_BigBlur         (f32 _factor, u32 _MaxPass, bbool _clearRT, GFX_QUALITY _quality);
    GFXA_OVERRIDE  void         AFTERFX_Glow            (f32 _factor, f32 _glowfactor, f32 _addalpha, GFX_BLENDMODE _finalBlendMode = GFX_BLEND_COPY);
    GFXA_OVERRIDE  void         AFTERFX_objectsGlow     (f32 _factor);
    GFXA_OVERRIDE  void         AFTERFX_RayCenter       (f32 _v1, f32 _v2, f32 _v3, f32 v4, f32 _v5, f32 _v6, f32 _v7);
    GFXA_OVERRIDE  void         AFTERFX_Remanence       (f32 _factor, f32 _glowfactor, f32 _addalpha);
    GFXA_OVERRIDE  void         AFTERFX_ColorSetting    (f32 _sat, f32 _contrast, f32 _contrastscale, f32 _bright);
    GFXA_OVERRIDE  void         AFTERFX_ColorRemap      (Texture* _tex);
    GFXA_OVERRIDE  void         AFTERFX_ColorLevels     (f32 _inBlack,  f32 _inGamma, f32 _inWhite, f32 _outBlack, f32 _outWhite);
    GFXA_OVERRIDE  void         AFTERFX_addSceneAndMul  (f32 _mul);
    GFXA_OVERRIDE  void         AFTERFX_byTech          (u32 _tech);
    GFXA_OVERRIDE  void         AFTERFX_byTechParams    (u32 _tech, f32 _p0, f32 _p1, f32 _p2, f32 _p3);
    GFXA_OVERRIDE  void         AFTERFX_startRenderTarget(Color _color);
    GFXA_OVERRIDE  void         AFTERFX_zoomCenter      (f32 _factor, Vec3d _dir);
    GFXA_OVERRIDE  void         prepareAfterFx          (bbool _nocopy);
    GFXA_OVERRIDE  void         endAfterfx              (bbool _nocopy, GFX_BLENDMODE _finalBlend);
    GFXA_OVERRIDE  void         prepareInternalRT       ();
    GFXA_OVERRIDE  void         endInternalRT           ();

    GFXA_OVERRIDE  void         AFTERFX_draw            (AFX_Group* _group);
    GFXA_OVERRIDE  void         AFTERFX_prepareObjectGlow();

    GFXA_OVERRIDE  void         startImpostorRendering  (renderTarget* _rt, f32 xmin, f32 ymin, f32 xmax, f32 ymax, bbool _mustClear=btrue);
    GFXA_OVERRIDE  void         stopImpostorRendering   (renderTarget* _rt);

    /// Surfaces.
    GFXA_OVERRIDE  f32          ReadSurfaceFloat        (void* _surf, u32 _posX, u32 _posY);

    // GFXA_OVERRIDE  Color        getBufferValue          (void* pBackBuffer, u32 _posX, u32 _posY); // Win32 only
    // GFXA_OVERRIDE  void         doScreenCapture         (); // Win32 only
    GFXA_OVERRIDE  void         setAlphaRef             (int _ref = 1);
                   void         setAlphaTest            (bbool _on);
                   void         setDefaultAlphaTest     ();

    GFXA_OVERRIDE  void         setBlendSeparateAlpha   ();

    GFXA_OVERRIDE  void         startOverDrawRecord     ();
    GFXA_OVERRIDE  void         showOverDrawRecord      ();

    // Win32 only
    // GFXA_OVERRIDE  void        startScreenCapture       ();
    // GFXA_OVERRIDE  void        setThreadUsageScreenCapture(u32 _uThreadUsage);

    /// UV.
    GFXA_OVERRIDE  void        setUVAnim                (GFX_UVANIM & _uvanim);

    /// Render Context.
    GFXA_OVERRIDE  void        renderContext_Set        ();
    GFXA_OVERRIDE  void        renderContext_SetDefault ();

    // Global colors
    GFXA_OVERRIDE  void        setGlobalColor           (const Color & _color);
    GFXA_OVERRIDE  void        setFogColor              (const Color & _color);

    // secondary thread rendering
    GFXA_OVERRIDE  void        acquireDeviceOwnerShip   ();
    GFXA_OVERRIDE  void        releaseDeviceOwnerShip   ();
    GFXA_OVERRIDE  void        minimal_present          ();

                   void        addFlipEventCB(void (*_cb)(u64 usrData), u64 _userData);
                   void        removeFlipEventCB(void (*_cb)(u64 usrData), u64 _userData);

    GCM::RenderContext &       getGCMRenderCtxt() { return m_rdrCtxt; }
    u64                        getLastFlipTime() const { return m_flipMgr->getLastFlipTime(); }

private:

    void         preInit();

    friend class MinimalDisplay;
    class MinimalDisplay * m_minimalDisplay;
    void minimalDisplay(); // this method, called by the minimal display class do a clear + flip and wait for gpu idle

    //////////////////////////////////////////////////////////////////////////
    // tools
    //////////////////////////////////////////////////////////////////////////
    void         AFTERFX_PrepareSwapRTDown2x2(bool _clear = true);
    void         AFTERFX_PrepareSwapRTDown4x4(bool _clear = true);
    void         AFTERFX_SwapTargetDown2x2();
    void         AFTERFX_SwapTargetDown4x4();
    void         AFTERFX_SwapFullScreenTarget();
    void         AFTERFX_destroyTarget();

    GFX_QUALITY  AFTERFX_AdjustQualityFromBufferAvailable(GFX_QUALITY _quality);

    /// Shaders parameters handling.
    void        setShaderConstant       (ITF_shader* _shader    );
    void        setShaderMatrix         (ITF_shader* _shader    );
    void        setShader               (ITF_shader* _shader    );

    void                        initAfterFx();

    // Flip callback->event system
    void                        initFlipEvent();
    static void                 onFlip(uint32_t arg);
    void                        onFlip();

    // set render target at each flip (with the swap chain index)
    void                        setRenderTarget(const GCM::Surface * _rtSurface);
    // init gcm library + resc
    bool                        initCmdBuffer();
    bool                        initDisplay();
    bool                        initTools();
    bool                        initDebugTools();
    void                        initDefaultRenderStates();
    bool                        createRenderingSurfaces();

    void                        resetVertexDeclCache();
    void                        resetRenderStateAfterFlip();

    
    void waitForGPUIdle();

    // Frustum planes managements (used by setCamera)
    void updateCameraFrustumPlanes(Camera *_camera);

    bool m_successfullInit;

    ITF_shader* mp_defaultShader; /// default shader for basic renderer.
    ITF_shader* mp_currentShader;    

    struct VertexStream
    {
        VertexStream() : stride(-1), rsxOffset(-1), rsxLocation(CELL_GCM_LOCATION_LOCAL) {}
        VertexStream(const VertexStream & _vtxStream) : stride(_vtxStream.stride), rsxOffset(_vtxStream.rsxOffset), rsxLocation(_vtxStream.rsxLocation) {}

        u32 stride;
        u32 rsxOffset;
        u32 rsxLocation;
    };
    VertexStream m_currentVertexStream;
    u32 m_currentIBufferRSXOffset;
    u32 m_currentIBufferRSXLocation;

    // All the hard coded vertex declarations
    GCM::VertexDecl mp_VDcl_PCT;
    GCM::VertexDecl mp_VDcl_PC;
    GCM::VertexDecl mp_VDcl_PT;
    GCM::VertexDecl mp_VDcl_PCBT;
    GCM::VertexDecl mp_VDcl_PNCT;
    GCM::VertexDecl mp_VDcl_PNC3T;
    GCM::VertexDecl mp_VDcl_P;
    // the selected one
    GCM::VertexDecl * m_currentVertexDecl; // right now, only one of the vertex decl above

    Matrix44 m_UVmat;

    /// Shaders.
    GFXAdapter_shaderManager    mp_shaderManager;

    GCM::WorldViewProj          m_worldViewProj;

    GFX_RECT m_lastSetViewPort;

    // keep a trace of the sampler states
    enum 
    {
        NB_MAX_TEX_SAMPLER = 16
    };

    struct SamplerState
    {
        u32 alphaKill;
        u16 bias;
        u8 wrapU;
        u8 wrapV;
        u8 activated;
        u8 minFilter;
        u8 magFilter;
    };
    SamplerState m_currentTexSamplerState[NB_MAX_TEX_SAMPLER];
    SamplerState m_defaultTexSamplerState[NB_MAX_TEX_SAMPLER];
    u32 m_minLinearFilteringMode; // mode used for linear filtering (debug purpose)
    u32 m_linearFilteringOptimization; // mode used for trilinear filtering optim (debug purpose)
    f32 m_texLodBiasFloat; // lod bias applied to texture (debug purpose)

    // alpha test state management
    bbool m_alphaTestState; // On/Off

    // vertex declaration state
    void createCoreVertexDeclarations();
    // apply vertex decl
    void applyVertexDecl(const VertexStream & _vtxStream, const GCM::VertexDecl & _vertexDecl);
    // apply vertex decl for inline draw
    void applyVertexDecl(u32 _stride, const GCM::VertexDecl & _vertexDecl); 
    // cache of vertex decl
    bool m_lastUsageSet[GCM::VertexDecl::NbMaxVertexInputAttributes]; // used to invalidate vdecl left
    const GCM::VertexDecl * m_lastVertexDecl; // resend vertex data format if m_lastVertexDecl changed
    u32 m_lastVertexStride; // resend vertex data format if m_lastVertexStride changed

    /// Shaders.
    void		createShaderParametersDB(); // create the hardcoded DB of shader parameters
    bool		loadCoreShaders			(); // load hard coded shaders
    bool		loadShader			    (const char * _shaderName); // load a shader from it's name, assuming default directory

#ifdef _USE_FONTDEBUG
    // Fonts Debug.
    GCM::SimpleVRAMRelocHandle m_fontMemory;
#endif

    void printDebugInfoOnScreen();

    enum { NbMaxFrameBuffer = 3 }; // max triple bufering
    GCM::Surface * m_frameBuffers[NbMaxFrameBuffer];
    GCM::Texture * m_frameBuffersAsTex[NbMaxFrameBuffer];
    u32 m_nbFrameBuffer; // current policy : set to 1 in SD or 3 in HD
    u32 m_backBufferIndex; // currently rendering to m_frameBuffer[m_backBufferIndex]
    const GCM::Surface * m_currentRenderTarget; // the current render target
    GCM::FlipMgr * m_flipMgr; // flip mgr using libresc or not
    bool m_usingLibResc; // set to true when libresc is used to rescale for the flip

    // AfterFX Surfaces
    static const u32 AfterFXNbSwapSurface = 2;

    GCM::Surface *              m_pSwapTextureSurfDown2x2[AfterFXNbSwapSurface]; 
    GCM::Texture *              m_pSwapTextureDown2x2[AfterFXNbSwapSurface]; // same as above but seen as a texture
    GCM::Surface *              m_pSwapTextureSurfDown4x4[AfterFXNbSwapSurface];
    GCM::Texture *              m_pSwapTextureDown4x4[AfterFXNbSwapSurface]; // same as above but seen as a texture

    GCM::Surface *              m_pFullRTsurf[AfterFXNbSwapSurface]; 
    GCM::Texture *              m_pFullRTtex[AfterFXNbSwapSurface]; // same as above but seen as a texture

    const GCM::Texture *        m_pCurrentSwapSourceTexture;
    const GCM::Surface *        m_pCurrentSwapRenderSurf;
    u32                         m_curswap;

    // states of display at init
    CellVideoOutResolution  m_videoOutResolution;
    CellVideoOutState       m_videoOutState;
    CellGcmConfig           m_gcmConfig;

    GCM::VRAMAddress m_cmdBufferPageAddress; 
    GCM::VRAMAddress m_cmdBufferAddress; // m_cmdBufferPageAddress + prefix

    GCM::RenderContext m_rdrCtxt;

    GCM::Label m_gpuRenderFrameCountLabel; // Count of the last fully rendered frame
    GCM::Label m_gpuFlipFrameCountLabel; // Count of the las flipped frame (ie. shown to the player)
    u32 m_cpuFrameCount;

    u32 m_finishCount;

    f32 m_cpuWaitForGPUFrameCount;
    friend i32 TimedGCMCallback(struct CellGcmContextData* _ctxt, u32 _size);
    static f32 ms_cpuWaitForCBAvailable;

    GCM::TimerProfiler m_cpuTimeProfiler;

    GCM::TimeStamp<MaxCPUFrameInAdvance+1> m_gpuRenderTimer;
    GCM::TimeStamp<MaxCPUFrameInAdvance+1> m_gpuPresentTimer;

    enum
    {
        // Note that choosing CELL_GCM_LOCATION_LOCAL can lead to problems with snTuner RSX Markers
        REPORT_LOCATION = CELL_GCM_LOCATION_MAIN,
    };

    // flip CB system
    typedef void (*FlipCB)(u64 _userData);
    struct FlipCBInfo
    {
        u64 usrData;
        FlipCB cb;
    };
    Vector<FlipCBInfo> m_flipCBs;
    Mutex m_flipCBMutex;
};

} // namespace ITF


#endif // ITF_GFXADAPTER_GCM_H

#endif //ITF_PS3
