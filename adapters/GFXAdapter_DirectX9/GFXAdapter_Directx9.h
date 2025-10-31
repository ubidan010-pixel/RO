#ifndef _ITF_DIRECTX9_DRAWPRIM_H_
#define _ITF_DIRECTX9_DRAWPRIM_H_

#pragma once

#if defined (ITF_WINDOWS) || defined (ITF_X360)


#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#include "core/system/Synchronize.h"

#ifndef _ITF_GFXADAPTER_SHADERMANAGER_H_
#include "engine/AdaptersInterfaces/GFXAdapter_ShaderManager.h"
#endif //_ITF_GFXADAPTER_SHADERMANAGER_H_


#ifdef ITF_X360//yo
    #define ITF_EFFECTP FXLEffect* 
    #define ITF_HANDLE FXLHANDLE 
#else
    #define ITF_EFFECTP LPD3DXEFFECT
    #define ITF_HANDLE D3DXHANDLE
#endif

#include <d3dx9.h>

#ifdef ITF_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif

#ifndef ITF_DISABLE_DEBUGINFO
#define _USE_FONTDEBUG
#endif //ITF_DISABLE_DEBUGINFO

#define _USE_SHADER

#ifdef ITF_X360
//#define _USE_ZBUFFER_TILING
#ifdef _USE_FONTDEBUG
#include "x360/FontDebug_x360.h"
#endif
#endif

namespace ITF
{

#define MAX_SAMPLER             8

const D3DVERTEXELEMENT9 VElem_PCT[] =
{
    { 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
    { 0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,   0 },
    { 0, 16, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
    D3DDECL_END()
};

const D3DVERTEXELEMENT9 VElem_PC[] =
{
    { 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
    { 0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR,   0 },
    D3DDECL_END()
};

const D3DVERTEXELEMENT9 VElem_PT[] =
{
    { 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
    { 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
    D3DDECL_END()
};

const D3DVERTEXELEMENT9 VElem_PCBT[] =
{
    {0,  0, D3DDECLTYPE_FLOAT3  , 0, D3DDECLUSAGE_POSITION    , 0},
    {0, 12, D3DDECLTYPE_D3DCOLOR, 0, D3DDECLUSAGE_COLOR       , 0},
    {0, 16, D3DDECLTYPE_UBYTE4  , 0, D3DDECLUSAGE_BLENDINDICES, 0},
    {0, 20, D3DDECLTYPE_FLOAT2  , 0, D3DDECLUSAGE_TEXCOORD    , 0},
    D3DDECL_END()
};

const D3DVERTEXELEMENT9 VElem_PNCT[] =
{
    {0,  0, D3DDECLTYPE_FLOAT3  , 0, D3DDECLUSAGE_POSITION    , 0},
    {0, 12, D3DDECLTYPE_FLOAT3  , 0, D3DDECLUSAGE_NORMAL      , 0},
    {0, 24, D3DDECLTYPE_D3DCOLOR, 0, D3DDECLUSAGE_COLOR       , 0},
    {0, 28, D3DDECLTYPE_FLOAT2  , 0, D3DDECLUSAGE_TEXCOORD    , 0},
    D3DDECL_END()
};

const D3DVERTEXELEMENT9 VElem_PNC3T[] =
{
    { 0,  0, D3DDECLTYPE_FLOAT3  , 0, D3DDECLUSAGE_POSITION    , 0},
    { 0, 12, D3DDECLTYPE_D3DCOLOR, 0, D3DDECLUSAGE_COLOR       , 0},
    { 0, 16, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,  0},
    { 0, 24, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,  1},
    { 0, 40, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,  2},
    { 0, 56, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,  3},
    D3DDECL_END()
};

enum eShaderPassLastState
{
    S_SHADERPASS_NONE = 0,
    S_SHADERPASS_BEGIN,
    S_SHADERPASS_END,
    ENUM_FORCE_SIZE_32(eShaderPassLastState)
};

class GFXAdapter_Directx9 final : public GFXAdapter
{
public:

    GFXAdapter_Directx9();


    // Implementations of GFXAdapter methods

    void        project                 (   const Vec3d& _in3d, Vec2d & _out2d) override;
    void        drawSphere              (   float _x, float _y, float _z, float r, int lats, int longs, int _wireFrame, float _r=1, float _g=1, float _b=1  ) override;
    void        drawSphere(float _x, float _y, float r, float _r = 1, float _g = 1, float _b = 1, float _z = 0.f, u32 _segsCount = 32, bbool _volume = btrue, bbool _xaxis = btrue, bbool _yaxis = btrue, bbool _zaxis = btrue) override;
    void        drawCircle              (   float _x, float _y, float r, float _r=1, float _g=1, float _b=1, float _z = 0.f, u32 _segsCount = 32 ) override;
    void        drawBox                 (   const Vec2d& _pos, f32 _angle, const Vec2d& _extent, f32 _r=1, f32 _g=1, f32 _b=1, float _z = 0.f, bbool _fill = bfalse) override;
    void        drawLine                (   const Vec2d& _p1, const Vec2d& _p2, float _r=1, float _g=1, float _b=1, float _size=1.f, float _z = 0.f, float _alpha = 1.f) override;
    void        draw2dLine              (   const Vec2d& _p1, const Vec2d& _p2, float _size = 1.f, u32 _color = COLOR_WHITE) override;
    void        draw2dTriangle(const Vec2d& _p1, const Vec2d& _p2, const Vec2d& _p3, u32 _color = COLOR_WHITE, i32 _rank = 0) override;
    void        draw2dBox               (   const Vec2d& _p, f32 _width, f32 _height, u32 _color0 = COLOR_WHITE, u32 _color1 = COLOR_WHITE, u32 _color2 = COLOR_WHITE, u32 _color3 = COLOR_WHITE, i32 _rank = 0) override;
    void        drawTriangle            (   const Vec2d& _p1, const Vec2d& _p2, const Vec2d& _p3, float _r=1, float _g=1, float _b=1, float _a=1, float _z = 0.f    ) override;
    void        drawTriangle            (   const Vertex& _v1, const Vertex& _v2, const Vertex& _v3, const Texture* _tex, bbool _showWireFrame = 0) override;
    void        drawQuad2D              (   f32 _px, f32 _py, f32 _width, f32 _height, f32 _z, u32 _color = COLOR_WHITE, const Texture* _tex = NULL, const Vec2d* _uvStart = NULL,  const Vec2d* _uvEnd = NULL) override;
    void        draw3DQuad              (    GFX_3DQUAD* _t_Quad ) override;
    bbool       loadTexture             (   Texture* _tex, const char* _path, u32 _alhpa = 0 ) override;
    void        coord2DTo3D             (   const Vec2d& _in2d, float _zPlane, Vec2d& _out3d, bool noTranslation = btrue) override;
    void        compute3DTo2D           (   const Vec3d& _in, Vec3d& _out) override;
    void        compute2DTo3D           (   const Vec3d& _in, Vec3d& _out) override;

    void        drawScreenQuad          (    f32 _px, f32 _py, f32 _width, f32 _height, f32 _z, u32 _color, bbool _noTex = 0 ) override;

                ~GFXAdapter_Directx9    (   ) override {};
    void        init                    (   ) override;

    void        setCamera               (   float _x, float _y, float _z, Camera* _cam ) override;
    void        destroy                 (   ) override;
    void        destroyResources        (   ) override;
    void        setBiasEyeLookAt        (   const Vec3d &lookAtDir ) override;
    void        init2DRender            (   ) override;
    void        init2DGuiRender         (   ) override;

    void        setOrthoView            (   f32 _l, f32 _r, f32 _b, f32 _t  ) override;
	void        setOrthoView2DYNeg      (   f32 _l, f32 _r, f32 _b, f32 _t  ) override;

    bbool       isDrawingForScreenshot() override { return m_pUltraHDBackBufferSurf != NULL; }

    void        setAlphaBlend           (   GFX_BLENDMODE _Blend    ) override;
    void        setCullMode             (   GFX_CULLMODE _cull    ) override;
    void        setFillMode             (   GFX_FILLMODE _fillmode  ) override;
    void        depthMask               (   u32 _depthMask    ) override;
    void        depthFunc               (   u32 _func    ) override;
    void        depthTest               (   u32 _test    ) override;

    void        setScissorRect          (   GFX_RECT* _clipRect) override;

    void        SetTextureBind          (   int _Sampler, void * _Bind, bbool _linearFiltering = btrue) override;
    void        lockTexture             (   Texture* _tex, LOCKED_TEXTURE* _lockTex, u32 _flag ) override;
    void        unlockTexture           (   Texture* _tex   ) override;
    void        cleanupTexture          (   Texture* _texture) override;

    void        DrawPrimitive           (    PRIMITIVETYPE _type, const void* _p_Vertex, u32 _NumberVertex) override;

    void        setMatrixTransform      (    MATRIXMODE _type, const GMatrix44* _Matrix) override;
    void        setupViewport           (    GFX_RECT* _Viewport) override;

    void        unprojectPoint(const Vec2d& _in, float _viewDistance, const GMatrix44* _MatrixModel, const GMatrix44* _MatrixProj, const int* _View, Vec2d& _out) override;
    void        setLookAtMatrix(GMatrix44* _MatrixOut, double _midx, double _midy, float _viewDistance, float _aspect) override;
    void        getViewport(GFX_RECT* _RectOut) override;

    /// GfxMaterials.
    void        setGfxMaterial(const GFX_MATERIAL& _gfxMat) override;
    void        setGfxMatDefault() override;
    void        setGfxMatRefraction(const GFX_MATERIAL& _gfxMat) override;

    /// Texture.
    void        generateTexture         (   Texture* _texture) override;
    void        createTexture           (   Texture* _texture, u32 _sizeX, u32 _sizeY, u32 _mipLevel, Texture::PixFormat _pixformat, u32 _pool, bbool _dynamic   ) override;
    void        cleanupBaseTexture      (   Texture* _texture) override;
    void        setTextureSize          (   Texture* _texture, const Size& _size   ) override;
    unsigned char* grabTexture          (   Texture* _texture   ) override;
    u32         getMaxTextureSize       (   ) override;
    void        loadFromMemory          (   const void* _buffer, const Size& _texture_size, Texture* _texture, Texture::PixFormat _pixel_format ,const u32 _buffer_size,bbool bCooked,u32 _dwMipMapCount) override;
    // Not implemented : bbool loadFromFileInMemory(Texture* _texture, const void* _buffer, u32 _contentSize) { ITF_ASSERT(0); return bfalse; }
    void        SetTexture              (   int _Sampler, Texture* _Texture, bbool _linearFiltering = btrue) override;
    void        saveToMemory            (   void* _buffer, Texture* _Tex    ) override;
    float       getTexelOffset          (   ) override { return 0.5f; };
    Size        getAdjustedSize         (   const Size& sz  );
    void        setSamplerState         ( u32 _samp, GFX_SAMPLERSTATETYPE _type, i32 _value ) override;

    // Texture Target.
    void        clearRenderTarget       (   renderTarget* _target, u32 _color   ) override;
    void        initialiseRenderTarget  (   renderTarget* _target   ) override;
    void        resizeRenderTarget      (   renderTarget* _target, const Size& _size    ) override;
    void        enableRenderTarget      (   renderTarget* _target   ) override;
    void        disableRenderTarget     (   renderTarget* _target   ) override;
    void        cleanupRenderTarget     (    renderTarget* _target   ) override;
    bbool       isRenderTargetInverted  (   )  override { return bfalse; }

    void        clear                   (   u32 _buffer, f32 _r=0.f, f32 _g=0.f, f32 _b=0.f, f32 _a=0.f) override;

    /// Renderer.
    void        present                 (   ) override;
    void        startRendering          (   ) override;
    void        endRendering            (   ) override;
    u32         isRender;
    // Not implemented: virtual bbool                   waitPreviousGPUframeEnd() { ITF_ASSERT(0); return bfalse; }
    // Not implemented: virtual void                    syncGPU() { ITF_ASSERT(0); }
    // Not implemented: virtual bbool                   setScreenProtection(bbool _enable) { return btrue; }

    void        reshape                 (   ) override;
    void        createDebugFonts        (   u32 _screenwidth, u32 _screenHeight) override;

    bbool       isDeviceLost            (   ) override;
    bbool       resetDevice             (   bbool _forceReset   ) override;

    void        setVertexFormat         (   u32 _vformat    ) override;

    const String getRenderName          (   ) override { return "directx"; }

    void        createShader            (   ITF_shader* _shader ) override;
    void        releaseShader           (   ITF_shader* _shader ) override;
    bbool       loadShader              (   ITF_shader* _shader ) override;
    void        beginShader             (   ITF_shader* _shader ) override;
    void        endShader               (   ITF_shader* _shader ) override;

    void        fx_setVectorArray       (   uPtr _fx, void* _h, const GFX_Vector4* _va, u32 _count) override;
    void        fx_setVector            (   uPtr _fx, void* _h, const GFX_Vector4* _v) override;
    void        fx_setFloat             (   uPtr _fx, void* _h, float _f) override;
    void        fx_setInt               (   uPtr _fx, void* _h, i32 _i) override;
    void        fx_commitChange         (   uPtr _fx ) override;

    /// Patch shader.
    void        prepareShaderPatch      (   GMatrix44* _matrix, f32 _z, u32 & _hdiv, u32 & _vdiv, GFX_BLENDMODE _blendMode = GFX_BLEND_ALPHA ) override;
    void        shaderDrawPatch32       (   const Texture *texture, const Vec2d *PointsNS,const ColorInteger* _lighttab ) override;
    void        endShaderPatch          (   ) override;
    void        shaderPatchFlush        (   u32 _hdiv, u32 _vdiv, u32 _pass) override;

    // Patch bezier shader
    void        prepareShaderBezierPatch      (   GMatrix44* _matrix, f32 _z, u32 & _hdiv, u32 & _vdiv, GFX_BLENDMODE _blendMode = GFX_BLEND_ALPHA ) override;
    bbool       shaderDrawBezierPatch32       (   Texture *texture, Vec2d *PointsNS, ColorInteger* _lighttab, f32* _widths, f32* _alphas, f32* _widths2 ) override;
    void        shaderBezierPatchFlush        (   u32 _hdiv, u32 _vdiv, u32 _pass, u32 _vdivToDraw ) override;

    /// Spline Shader
    void        drawSpline(GMatrix44* _matrix, Texture* _texture, const Spline* _spline, const Vec3d& _pos, f32 _height) override;

    // Fluid Shader
    void        drawFluid(GMatrix44* _matrix, Texture* _texture, const FixedArray<u32, GFXADAPTER_FLUID_COLORTAB_ENTRY_COUNT>& _colorTab, const Vec2d _heightTab[], const u32 _heightTabSize, const FixedArray<Vec2d, 4>& _uvTab, u32 mode, f32 _delta1, f32 _delta2, f32 _stepCount) override;

    /// Trail 3d Shader.
    void        drawTrail3D(Texture* _texture, Color _color, const SafeArray<Vec3d>& points, f32 _alphaBegin, f32 _alphaEnd, f32 _fadeLength) override;

    // Movie
    void        drawMovie(GMatrix44* _matrix, f32 _alpha, ITF_VertexBuffer* _pVertexBuffer, Texture** _pTextureArray, u32 _countTexture) override;

    // VertexBuffer.
    void        createVertexBuffer              (   ITF_VertexBuffer* _vertexBuffer) override;
    void        releaseVertexBuffer             (   ITF_VertexBuffer* _vertexBuffer) override;
    void        LockVertexBuffer                (   ITF_VertexBuffer* _vertexBuffer, void** _data, u32 _offset, u32 _size, u32 _flag ) override;
    void        UnlockVertexBuffer              (   ITF_VertexBuffer* _vertexBuffer ) override;

    // IndexBuffer.
    void        createIndexBuffer               (   ITF_IndexBuffer* _indexBuffer ) override;
    void        releaseIndexBuffer              (   ITF_IndexBuffer* _indexBuffer ) override;
    void        LockIndexBuffer                 (   ITF_IndexBuffer* _indexBuffer, void** _data ) override;
    void        UnlockIndexBuffer               (   ITF_IndexBuffer* _indexBuffer ) override;

    // Vertex/Index Buffer Draw
    void        DrawVertexBuffer                (   u32 _type, ITF_VertexBuffer* _vertexBuffer, u32 _vertexStart, u32 _vertexNumber ) override;
    void        DrawIndexedVertexBuffer         (   u32 _type, ITF_VertexBuffer* _vertexBuffer, ITF_IndexBuffer* _indexBuffer, u32 _indexNumber ) override;
    void        drawMesh                        (   ITF_Mesh& _mesh, bbool _showWireFrame) override;
    void        drawMeshPT                      (   const ITF_Mesh& _mesh, bbool _showWireFrame) override;
#ifndef ITF_CONSOLE_FINAL
    void        drawFlatMesh                    (   const ITF_Mesh& _mesh) override;
#endif //ITF_CONSOLE_FINAL

    // Font
    void        setShaderForFont                (   bbool bOutline) override;
    void        restoreDefaultShader            (   ) override;

    const GMatrix44* getViewMatrix              (   ) override;

    // AfterFX
    void        AFTERFX_BigBlur(f32 _factor, u32 _MaxPass, bbool _clearRT, GFX_QUALITY _quality) override;
    void        AFTERFX_Glow(f32 _factor, f32 _glowfactor, f32 _addalpha, GFX_BLENDMODE _finalBlendMode = GFX_BLEND_COPY) override;
    void        AFTERFX_objectsGlow(f32 _factor) override;
    void        AFTERFX_RayCenter(f32 _v1, f32 _v2, f32 _v3, f32 _v4, f32 _v5, f32 _v6, f32 _v7) override;
    void        AFTERFX_Remanence(f32 _factor, f32 _glowfactor, f32 _addalpha) override;
    void        AFTERFX_ColorSetting(f32 _sat, f32 _contrast, f32 _contrastscale, f32 _bright) override;
    void        AFTERFX_ColorRemap(Texture* _tex) override;
    void        AFTERFX_ColorLevels(f32 _inBlack, f32 _inGamma, f32 _inWhite, f32 _outBlack, f32 _outWhite) override;
    void        AFTERFX_addSceneAndMul(f32 _mul) override;
    void        AFTERFX_byTech(u32 _tech) override;
    void        AFTERFX_byTechParams(u32 _tech, f32 _p0, f32 _p1, f32 _p2, f32 _p3) override;
    void        AFTERFX_startRenderTarget(Color _color) override;
    void        AFTERFX_zoomCenter(f32 _factor, Vec3d _dir) override;
    void        prepareAfterFx(bbool _nocopy) override;
    void        endAfterfx(bbool _nocopy, GFX_BLENDMODE _finalBlend) override;
    void        prepareInternalRT() override;
    void        endInternalRT() override;
#ifdef ITF_X360
    void        setGammaRamp(Vec3d gamma, f32 brigthnessValue = 1.0f, f32 contrastValue = 1.0f) override;
#endif
    void        AFTERFX_draw(AFX_Group* _group) override;
    void        AFTERFX_prepareObjectGlow() override;

    void        startImpostorRendering(renderTarget* _rt, f32 xmin, f32 ymin, f32 xmax, f32 ymax, bbool _mustClear = btrue) override;
    void        stopImpostorRendering(renderTarget* _rt) override;

    // Surfaces.
    f32         ReadSurfaceFloat(void* _surf, u32 _posX, u32 _posY) override;

#ifdef ITF_WINDOWS
    void        setRenderTarget_Picking() override;
    void        setRenderTarget_BackBuffer() override;
    void*       resolvePicking() override;
    Color       getBufferValue(void* _pBackBuffer, u32 _posX, u32 _posY) override;
#endif

#ifdef ITF_SUPPORT_VIDEOCAPTURE
    void        doScreenCapture() override;
#endif// ITF_SUPPORT_VIDEOCAPTURE

    void        setAlphaRef(int _ref = 1) override;
    void        setBlendSeparateAlpha() override;

    void        startOverDrawRecord() override;
    void        showOverDrawRecord() override;

    // Not implemented: virtual bbool isAlphaTestEnabled(void) const { return m_isAlphaRefEnabled; }
    void        setAlphaTest(bbool _enable) override;

#ifdef ITF_SUPPORT_VIDEOCAPTURE
    void        startScreenCapture() override;
#endif// ITF_SUPPORT_VIDEOCAPTURE

    // Not implemented: virtual void setThreadUsageScreenCapture(u32 _uThreadUsage,bbool _bStepByStep);

    // UV.
    void        setUVAnim(GFX_UVANIM& _uvanim) override;

    void*       compilePixelShader(const char* _bufferText, u32 len, const char* _functionName) override;
    void*       compileVertexShader(const char* _bufferText, u32 len, const char* _functionName) override;

    // Render Context.
    void        renderContext_Set() override;
    void        renderContext_SetDefault() override;

    /// Light.
    void        setGlobalColor(const Color& _color) override;
    /// fog
    void        setFogColor(const Color& _color) override;

#ifdef ITF_X360
    void        acquireDeviceOwnerShip() override;
    void        releaseDeviceOwnerShip() override;
    void        minimal_present() override;
#endif //ITF_X360

 private:
     void        updateTextureSize(Texture* _Tex);

     // material refract 
     bbool       m_refractUse2x2;

    void        drawScreenQuadC(f32 _px, f32 _py, f32 _width, f32 _height, f32 _z, u32 _color, f32 _centroid, bbool _noTex = 0);


    void        OverDraw_StencilBuffer  (   );
    void        OverDraw_ColorRemap     (   );
    void        createOverDrawRamp      (   u32 _greenPos, u32 _yellowPos, u32 _redPos);

    /// Device.
    void        lostDevice              (   );
    void        postD3DReset            (   );
    void        preD3DReset             (   );
    void        BuildPresentParams      (   );

    void        useHDWLightning         (   u32 _activate    );

    /// Shaders.
    void		loadCoreShaders			(	);
	
	void        setShaderConstant       (   ITF_shader* _shader    );
    void        setShaderMatrix         (   ITF_shader* _shader    );
    void        setShaderTechnique      (   ITF_shader* _shader    );
    void        setShader               (   ITF_shader* _shader    );

    ITF_shader* mp_defaultShader; /// default shader for basic renderer.
    ITF_shader* mp_currentShader;    
    u32         m_currentVertexSize;

    void*       loadBinVShader          (   const String& _name );
    void*       loadBinPShader          (   const String& _name );
    eShaderPassLastState                m_shaderPassLastState;
    eShaderPassLastState const          getShaderPassLastState  (   )   const {   return m_shaderPassLastState;    }

    // Patch shader
    void        prepareShaderPatchTech  (   GMatrix44* _matrix, f32 _z, u32 & _hdiv, u32 & _vdiv , u32 _idTech, GFX_BLENDMODE _blendMode = GFX_BLEND_ALPHA);    
    void        shaderPatchFlushSize    (   u32 _hdiv, u32 _vdiv, u32 countPoint, u32 _pass, u32 _vdivToDraw = U32_INVALID );

    /// VertexBuffer.
    void        setVertexBuffer                 (   ITF_VertexBuffer* _vertexBuffer );
    void        VertexBufferManagerOnLostDevice (   );
    void        VertexBufferManagerOnResetDevice(   );
    void        VertexBufferOnResetDevice       (   ITF_VertexBuffer* _vertexBuffer );
    void        VertexBufferOnLostDevice        (   ITF_VertexBuffer* _vertexBuffer );

    /// IndexBuffer.
    void        setIndexBuffer                  (   ITF_IndexBuffer* _indexBuffer );
    void        IndexBufferManagerOnLostDevice  (   );
    void        IndexBufferManagerOnResetDevice (   );
    void        IndexBufferOnResetDevice        (   ITF_IndexBuffer* _indexBuffer );
    void        IndexBufferOnLostDevice         (   ITF_IndexBuffer* _indexBuffer );

    void                        createFxTargetDown2x2();
    void                        createFxTargetDown4x4();
    void                        createRenderTarget(LPDIRECT3DTEXTURE9* _ptex, LPDIRECT3DSURFACE9* _psurf, u32 _width, u32 height, D3DFORMAT _format, bbool _shift = 0);
    void                        copyCurrentColorBuffer(u32 _rt);
    void                        copyTexture(int *TexHan);
    void                        validateTexture(int *TexHan,int Mode,int ScreenSize,int ScreenSize_V);
    void                        AFTERFX_onResetDevice();
    void                        AFTERFX_onLostDevice();
    void                        AFTERFX_destroyTarget();
    void                        AFTERFX_Blur(f32 _factor, u32 _MaxPass, bbool _clearRT, GFX_QUALITY _quality);
    void                        AFTERFX_PrepareSwapRTDown2x2();
    void                        AFTERFX_PrepareSwapRTDown4x4();
    void                        AFTERFX_SwapTargetDown2x2();
    void                        AFTERFX_SwapTargetDown4x4();
    void                        AFTERFX_SwapFullScreenTarget();

    void                        getBackBuffer();
    void                        setBackBuffer();

    void                        resolve(LPDIRECT3DTEXTURE9 _ptex);

    void                        startScreenshotMode();
    void                        endScreenshotMode();

    /// shader:
    void                        setShaderFormat(i32 _format);
    void                        setShaderPreparePatch(u32 _idTech, f32 _z);
    void                        setShaderPatchFlush();
    void                        setShaderFluid();
    void                        setShaderGlobalConstant(ITF_shader* _shadergroup);
    void                        startProfilerFPScaptureMode();
    void                        endProfilerFPScaptureMode();

public:

    /// sets for rendering with shaders.
    void        setObjectMatrix                 (   D3DXMATRIX& _matrix     ) {mg_World = _matrix;};

    IDirect3DDevice9* getRenderingDevice        (   )   {    return m_pd3dDevice;  };
    IDirect3D9*       getD3DDevice              (   )   {    return m_pD3D;  };
    
    bbool       createDXDevice            (int _alphaBits, int _depthBits, int _stencilBits, bbool _fullscreen  , void* _hwnd);

    void  setFullScreenMode(bbool _bMode)       {m_fullscreen = _bMode;}

#ifdef ITF_X360
    void        buildGammaRamp( D3DGAMMARAMP& scaledRamp, Vec3d gamma, f32 brigthnessValue, f32 contrastValue );
#endif //ITF_X360

    u32                                     m_useAFTERFX;
    bbool                                   m_lostdevice;

 protected:
    bbool       loadTextureCooked(Texture* _t, const char* _path);
    bbool       loadTextureFromFile(Texture* _t, const String& _path);

    DWORD fontAlignmentToDirectXFormat(FONT_ALIGNMENT _alignment);
    void setDefaultAlpha();
    void extractFrustumPlanes(const D3DXMATRIX &_m,
                               D3DXVECTOR4 &_leftPlane,
                               D3DXVECTOR4 &_rightPlane,
                               D3DXVECTOR4 &_topPlane,
                               D3DXVECTOR4 &_bottomPlane,
                               D3DXVECTOR4 &_nearPlane);
    void updateCameraFrustumPlanes(Camera* _cam);
    void normalizePlane(D3DXVECTOR4 &_plane);


    LPDIRECT3D9             m_pD3D; // Used to create the D3DDevice
    LPDIRECT3DDEVICE9       m_pd3dDevice; // Our rendering device
    
    /// Parameters.
    D3DPRESENT_PARAMETERS   m_ppars;
    D3DFORMAT               m_depthStencilFormat;
    HWND                    m_hwnd;
    bbool                   m_fullscreen;
    //
    LPDIRECT3DSURFACE9      m_pUltraHDBackBufferSurf; //the framebuffer surface for Ultra HD screenshots. If not null, this surface will be used for rendering.
    u32                     m_preUHDScreenWidth, m_preUHDScreenHeight;


    /// Matrix.
    D3DXMATRIX   mg_Proj;
    D3DXMATRIX   mg_View;
    D3DXMATRIX   mg_ViewRender;
    D3DXMATRIX   mg_World;
    D3DXMATRIX   mg_WorldView;
    D3DXMATRIX   mg_WorldViewProj;
    D3DXMATRIX   mg_UVmat;

    D3DXMATRIX   mg_lastSetProj;
    D3DXMATRIX   mg_lastSetView;
    D3DXVECTOR3  m_lastTransformedRotationCenter;

    D3DXMATRIX  GFX_mWorldViewProj;

    Texture*    m_pWhiteTexture;

    LPDIRECT3DSURFACE9          m_pBackBufferSurf;

    LPDIRECT3DSURFACE9          m_pDepthStencil;

#ifdef ITF_WINDOWS
    D3DDISPLAYMODE*             m_displayMode;
    u32                         m_displayModeCount;
#endif // ITF_WINDOWS

#ifdef ITF_X360
    D3DTexture* m_pFrontBufferTexture;
    bbool       m_askToReleaseOwnerShip;
    ITF_THREAD_EVENT                m_AcquireOwnerShipEvent;
    ITF_THREAD_EVENT                m_ReleaseOwnerShipEvent;
#endif

    /// AfterFX.
    LPDIRECT3DTEXTURE9          m_overDrawRamptexture;
    LPDIRECT3DTEXTURE9          m_pSwapTextureDown2x2[2];
    LPDIRECT3DSURFACE9          m_pSwapTextureSurfDown2x2[2];
    LPDIRECT3DTEXTURE9          m_pSwapTextureDown4x4[2];
    LPDIRECT3DSURFACE9          m_pSwapTextureSurfDown4x4[2];

    LPDIRECT3DTEXTURE9          m_pFullRTtex[2];
    LPDIRECT3DSURFACE9          m_pFullRTsurf[2];

    LPDIRECT3DSURFACE9          m_pRenderTargetSurfacePicking;
    LPDIRECT3DSURFACE9          m_pPickingOffScreenBufferSurf;


    LPDIRECT3DTEXTURE9          m_pCurrentSwapSourceTexture;
    LPDIRECT3DSURFACE9          m_pCurrentSwapRenderSurf;
    u32                         m_curswap;

    LPDIRECT3DSURFACE9          m_currentRenderingSurface;

    /// Shaders.
    GFXAdapter_shaderManager    mp_shaderManager;

    SafeArray<void *,MAX_SAMPLER>            m_textureBinded;
    void*                                  mp_oldtexBindOnShader;

    ITF_THREAD_CRITICAL_SECTION   m_csPresent;//CS present

#ifdef _USE_FONTDEBUG
#ifdef ITF_X360
    Fontdebug               m_FontDebug;
#else
    /// Fonts Debug.
    ID3DXFont*              mp_FontDebug;
	ID3DXFont*              mp_FontDebug_Monospace;
    ID3DXFont*              mp_FontDebug_Big;
#endif
#endif

    // Vertex Declarations.
	bbool					createVertexDeclaration();
	void					releaseVertexDeclaration();

    IDirect3DVertexDeclaration9* mp_VDcl_PCT;
    IDirect3DVertexDeclaration9* mp_VDcl_PC;
    IDirect3DVertexDeclaration9* mp_VDcl_PT;
    IDirect3DVertexDeclaration9* mp_VDcl_PCBT;
    IDirect3DVertexDeclaration9* mp_VDcl_PNCT;
    IDirect3DVertexDeclaration9* mp_VDcl_PNC3T;
    
    ///! What the renderer thinks the max texture size is.
    u32         m_maxTextureSize;
    ///! whether the hardware supports non-power of two textures
    bbool       m_supportNPOTTex;
    ///! whether the hardware supports non-square textures.
    bbool       m_supportNonSquareTex;

    void messageD3DError(HRESULT hr);
};

} // namespace ITF

#endif //!defined (ITF_WINDOWS) || defined (ITF_X360)

#endif // _ITF_DIRECTX9_DRAWPRIM_H_
