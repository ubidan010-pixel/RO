#pragma once

#include "engine/AdaptersInterfaces/GFXAdapter.h"
#include "engine/AdaptersInterfaces/GFXAdapter_ShaderManager.h"
#include "engine/AdaptersInterfaces/WorldViewProj.h"

#include "VRAMAllocator/VRAMRelocHdl.h"
#include "core/memory/MemMng/MemMngPage_PS5.h"
#include "GFXAdapter_device_context_PS5.h"
#include "RenderTarget_PS5.h"
#include "Texture_PS5.h"
#include "FxParameterDB_PS5.h"
#include "GFXAdapter_struct_PS5.h"
#include "Technique_PS5.h"

namespace ITF
{

    struct PS5IndexBuffer;

    class GFXAdapter_PS5 final : public GFXAdapter
    {
    public:

        GFXAdapter_PS5();

        virtual void                    project(const Vec3d& _in3d, Vec2d& _out2d) override;

#ifndef ITF_CONSOLE_FINAL
        virtual void                    drawSphere(float /*_x*/, float /*_y*/, float /*_z*/, float /*_radius*/, int /*lats*/, int /*longs*/, int /*_wireFrame*/, float /*_r*/ = 1, float /*_g*/ = 1, float /*_b*/ = 1) override;
        virtual void                    drawSphere(float /*_x*/, float /*_y*/, float /*_radius*/, float /*_r*/ = 1, float /*_g*/ = 1, float /*_b*/ = 1, float /*_z*/ = 0.f, u32 /*_segsCount*/ = 16, bbool _volume = btrue, bbool _xaxis = btrue, bbool _yaxis = btrue, bbool _zaxis = btrue) override;
        virtual void                    drawCircle(float /*_x*/, float /*_y*/, float /*_radius*/, float /*_r*/ = 1, float /*_g*/ = 1, float /*_b*/ = 1, float /*_z*/ = 0.f, u32 /*_segsCount*/ = 16) override;
        virtual void                    drawBox(const Vec2d& /*_pos*/, f32 /*_angle*/, const Vec2d& /*_extent*/, f32 /*_r*/ = 1, f32 /*_g*/ = 1, f32 /*_b*/ = 1, float /*_z*/ = 0.f, bbool /* _fill */ = bfalse) override;
        virtual void                    drawLine(const Vec2d& /*_p1*/, const Vec2d& /*_p2*/, float /*_r*/ = 1, float /*_g*/ = 1, float /*_b*/ = 1, float /*_size*/ = 1.f, float /*_z*/ = 0.f, float _alpha = 1.f) override;
#endif // ITF_CONSOLE_FINAL

        virtual void                    draw2dLine(const Vec2d& /*_p1*/, const Vec2d& /*_p2*/, float /*_size*/ = 1.f, u32 /*_color*/ = COLOR_WHITE) override;
        virtual void                    draw2dTriangle(const Vec2d& /*_p1*/, const Vec2d& /*_p2*/, const Vec2d& /*_p3*/, u32 /*_color*/ = COLOR_WHITE, i32 /*_rank*/ = 0) override;
        virtual void                    draw2dBox(const Vec2d& /*_p*/, f32 /*_width*/, f32 /*_height*/, u32 /*_color0*/ = COLOR_WHITE, u32 /*_color1*/ = COLOR_WHITE, u32 /*_color2*/ = COLOR_WHITE, u32 /*_color3*/ = COLOR_WHITE, i32 /*_rank*/ = 0) override;
        virtual void                    drawTriangle(const Vec2d& /*_p1*/, const Vec2d& /*_p2*/, const Vec2d& /*_p3*/, float /*_r*/ = 1, float /*_g*/ = 1, float /*_b*/ = 1, float /*_a*/ = 1, float /*_z*/ = 0.f) override;
        virtual void                    drawTriangle(const Vertex& /*_v1*/, const Vertex& /*_v2*/, const Vertex& /*_v3*/, const Texture* /*_tex*/, bbool /*_showWireFrame*/ = 0) override;

        virtual void                    drawQuad2D(f32 /*_px*/, f32 /*_py*/, f32 /*_width*/, f32 /*_height*/, f32 /*_z*/, u32 /*_color*/ = COLOR_WHITE, const Texture* /*_tex*/ = NULL, const Vec2d* /*_uvStart */= NULL, const Vec2d* /*_uvEnd */= NULL) override;
        virtual void                    draw3DQuad(GFX_3DQUAD* /*_t_Quad*/) override;
        virtual bbool                   loadTexture(Texture* /*_tex*/, const char* /*_path*/, u32 /*_alpha*/ = 0) override;

        virtual void                    coord2DTo3D(const Vec2d& _in2d, float _zPlane, Vec2d& _out3d, bool /*noTranslation*/ = btrue) override;
        virtual void                    compute3DTo2D(const Vec3d& _in, Vec3d& _out) override;
        virtual void                    compute2DTo3D(const Vec3d& _in, Vec3d& _out) override;


        virtual void                    drawScreenQuad(f32 /*_px*/, f32 /*_py*/, f32 /*_width*/, f32 /*_height*/, f32 /*_z*/, u32 /*_color*/, bbool /*_noTex*/ = 0) override;
        virtual void                    setCamera(float _x, float _y, float _z, Camera* _cam) override;
        virtual                         ~GFXAdapter_PS5() override;
        virtual void                    init() override;

        virtual void                    destroy() override;
        virtual void                    destroyResources() override;

        virtual void                    setBiasEyeLookAt(const Vec3d& lookAtDir) override;
        virtual void                    init2DRender() override;
        virtual void                    end2DRender() override;
        virtual void                    init2DGuiRender() override;
        virtual void                    setOrthoView(f32 /*_l*/, f32 /*_r*/, f32 /*_b*/, f32 /*_t*/) override;
        virtual void                    setOrthoView2DYNeg(f32 /*_l*/, f32 /*_r*/, f32 /*_b*/, f32 /*_t*/) override;

        virtual void                    setAlphaBlend(GFX_BLENDMODE /*_blend*/) override;
        virtual void                    setAlphaTest(bbool _enabled) override;
        virtual void                    setCullMode(GFX_CULLMODE /*_cull*/) override;
        virtual void                    setFillMode(GFX_FILLMODE /*_fillmode*/) override;

        virtual void                    depthMask(u32 /*_depthMask*/) override;
        virtual void                    depthFunc(u32 /*_func*/) override;
        virtual void                    depthTest(u32 /*_test*/) override;

        virtual void                    setScissorRect(GFX_RECT* /*_clipRect*/) override;
        virtual void                    SetTextureBind(int _sampler, void* _bind, bbool _linearFiltering = btrue) override;
        virtual void                    lockTexture(Texture* /*_tex*/, LOCKED_TEXTURE* /*_lockTex*/, u32 /*_flag*/ = 0) override;
        virtual void                    unlockTexture(Texture* /*_tex*/) override;

        virtual void                    cleanupTexture(Texture* /*_texture*/) override;
        virtual void                    DrawPrimitive(PRIMITIVETYPE /*_type*/, const void* /*_p_Vertex*/, u32 /*_NumberVertex*/) override;

        // Matrix.
        virtual void                    setMatrixTransform(MATRIXMODE /*_type*/, const GMatrix44* /*_Matrix*/) override;
        virtual void                    setupViewport(GFX_RECT* /*_Viewport*/) override;
#if !defined(ITF_FINAL)
        virtual void                    unprojectPoint(const Vec2d& /*_in*/, float /*_viewDistance*/, const GMatrix44* /*_MatrixModel*/, const GMatrix44* /*_MatrixProj*/, const int* /*_View*/, Vec2d& /*_out*/) override;
        virtual void                    setLookAtMatrix(GMatrix44* /*_MatrixOut*/, double /*_midx*/, double /*_midy*/, float /*_viewDistance*/, float /*_aspect*/) override;
#endif // !defined(ITF_FINAL)
        virtual void                    getViewport(GFX_RECT* /*_RectOut*/) override;

        /// GfxMaterials.
        virtual void                    setGfxMaterial(const GFX_MATERIAL& /*_gfxMat*/) override;
        virtual void                    setGfxMatDefault() override;
        virtual void                    setGfxMatRefraction(const GFX_MATERIAL& /*_gfxMat*/) override;

        /// Texture.
        virtual void                    generateTexture(Texture* /*_texture*/) override;
        virtual void                    createTexture(Texture* /*_texture*/, u32 /*_sizeX*/, u32 /*_sizeY*/, u32 /*_mipLevel*/, Texture::PixFormat /*_pixformat*/, u32 /*_pool*/ = 1, bbool /*_dynamic*/ = bfalse) override;
        virtual void                    cleanupBaseTexture(Texture* /*_texture*/) override;
        virtual void                    setTextureSize(Texture* /*_texture*/, const Size& /*_size*/) override;
        virtual unsigned char*          grabTexture(Texture* /*_texture*/) override;
        virtual u32                     getMaxTextureSize() override;
                u32                     getMinTextureSize() const { return 8; }
        virtual void                    loadFromMemory(const void* /*_buffer*/, const Size& /*_texture_size*/, Texture* /*_texture*/, Texture::PixFormat /*_pixel_format*/, const u32 /*_buffer_size*/, bbool /*bCooked*/, u32 _mipmapCount) override;
        // loadFromFileInMemory also not implemented on other platforms
        virtual bbool                   loadFromFileInMemory(Texture* _texture, const void* _buffer, u32 _contentSize) override { ITF_NOT_IMPLEMENTED(); return btrue; }
        virtual void                    SetTexture(int /*_Sampler*/, Texture* /*_Texture*/, bbool /*_linearFiltering*/ = btrue) override;
        virtual void                    saveToMemory(void* /*_buffer*/, Texture* /*_Tex*/) override;
        virtual float                   getTexelOffset() override { return 0.5f; }

        virtual void                    setSamplerState(u32 /*_samp*/, GFX_SAMPLERSTATETYPE /*_type*/, i32 /*_value*/) override;
        virtual void                    setTextureAdressingMode(u32 _samp, GFX_TEXADRESSMODE _texAdressModeU, GFX_TEXADRESSMODE _texAdressModeV) override;

        // Texture Target.   
        virtual void                    clearRenderTarget(renderTarget* /*_target*/, u32 /*_color*/) override;
        virtual void                    initialiseRenderTarget(renderTarget* /*_target*/) override;
        virtual void                    resizeRenderTarget(renderTarget* /*_target*/, const Size& /*_size*/) override;
        virtual void                    enableRenderTarget(renderTarget* /*_target*/) override;
        virtual void                    disableRenderTarget(renderTarget* /*_target*/) override;
        virtual void                    cleanupRenderTarget(renderTarget* /*_target*/) override;
        virtual bbool                   isRenderTargetInverted() override { return bfalse; }

        virtual void                    clear(u32 /*_buffer*/, float /*_r*/ = 0.f, float /*_g*/ = 0.f, float /*_b*/ = 0.f, float /*_a*/ = 0.f) override;

        virtual void                    present() override;
        virtual void                    startRendering() override;
        virtual void                    endRendering() override;
        // waitPreviousGPUframeEnd and syncGPU also not implemented on other platforms
        virtual bbool                   waitPreviousGPUframeEnd() override { ITF_NOT_IMPLEMENTED(); return btrue; }
        virtual void                    syncGPU() override { ITF_NOT_IMPLEMENTED(); }

        virtual bbool                   setScreenProtection(bbool _enable) override { return btrue; }

        virtual void                    reshape() override;

        virtual bbool                   isDeviceLost() override;
        virtual bbool                   resetDevice(bbool /*_forceReset*/) override;

        /// Vertex declaration.
        virtual void                    setVertexFormat(u32 /*_vformat*/) override;

        virtual const String            getRenderName() override { return "Gnmp"; }

        /// Shaders.
        virtual void                    createShader(ITF_shader* /*_shader*/) override;
        virtual void                    releaseShader(ITF_shader* /*_shader*/) override;
        virtual bbool                   loadShader(ITF_shader* /*_shader*/) override;
        virtual void                    beginShader(ITF_shader* /*_shader*/) override;
        virtual void                    endShader(ITF_shader* /*_shader*/) override;

        virtual void                    fx_setVectorArray(uPtr /*_fx*/, void* /*_h*/, const GFX_Vector4* /*_va*/, u32 /*_count*/) override;
        virtual void                    fx_setVector(uPtr /*_fx*/, void* /*_h*/, const GFX_Vector4* /*_v*/) override;
        virtual void                    fx_setFloat(uPtr /*_fx*/, void* /*_h*/, float/*_f*/) override;
        virtual void                    fx_setInt(uPtr /*_fx*/, void* /*_h*/, i32/*_i*/) override;
        virtual void                    fx_commitChange(uPtr /*_fx*/) override;

        /// Patch shader.
        virtual void                    prepareShaderPatch(GMatrix44* /*_matrix*/, f32 /*_z*/, u32&/*_hdiv*/, u32&/*_vdiv*/, GFX_BLENDMODE _blendMode = GFX_BLEND_ALPHA) override;
        virtual void                    shaderDrawPatch32(const Texture* /*texture*/, const Vec2d* /*PointsNS*/, const ColorInteger* /*_lighttab*/) override;
        virtual void                    endShaderPatch() override;
        virtual void                    shaderPatchFlush(u32 /*_hdiv*/, u32 /*_vdiv*/, u32 /*_pass*/) override;

        /// Bezier Patch shader.
        virtual void                    prepareShaderBezierPatch(GMatrix44* /*_matrix*/, f32 /*_z*/, u32&/*_hdiv*/, u32&/*_vdiv*/, GFX_BLENDMODE _blendMode = GFX_BLEND_ALPHA) override;
        virtual bbool                   shaderDrawBezierPatch32(Texture* /*texture*/, Vec2d* /*PointsNS*/, ColorInteger* /*_lighttab*/, f32* /*_widths*/, f32* /*_alphas*/, f32* /*_widths2*/) override;
        virtual void                    shaderBezierPatchFlush(u32 /*_hdiv*/, u32 /*_vdiv*/, u32 /*_pass*/, u32 /*_vdivToDraw*/) override;

        /// Spline Shader
        virtual void                    drawSpline(GMatrix44* _matrix, Texture* _texture, const Spline* _spline, const Vec3d& _pos, f32 _height) override;

        /// Fluid Shader.
        virtual void                    drawFluid(GMatrix44* _matrix, Texture* _texture, const FixedArray<u32, GFXADAPTER_FLUID_COLORTAB_ENTRY_COUNT>& _colorTab, const Vec2d _heightTab[], const u32 _heightTabSize, const FixedArray<Vec2d, 4>& _uvTab, u32 mode, f32 _delta1, f32 _delta2, f32 _stepCount) override;

        /// Trail 3d Shader.
        virtual void                    drawTrail3D(Texture* _texture, Color _color, const SafeArray<Vec3d>& points, f32 _alphaBegin, f32 _alphaEnd, f32 _fadeLength) override;

        //Movie    
        virtual void                    drawMovie(GMatrix44* _matrix, f32 _alpha, ITF_VertexBuffer* _pVertexBuffer, Texture** _pTextureArray, u32 _countTexture) override;

        /// VertexBuffer.
        virtual void                    createVertexBuffer(ITF_VertexBuffer* /*_vertexBuffer*/) override;
        virtual void                    releaseVertexBuffer(ITF_VertexBuffer* /*_vertexBuffer*/) override;
        virtual void                    LockVertexBuffer(ITF_VertexBuffer* /*_vertexBuffer*/, void** /*_data*/, u32 /*_offset*/, u32 /*_size*/, u32 /*_flag*/) override;
        virtual void                    UnlockVertexBuffer(ITF_VertexBuffer* /*_vertexBuffer*/) override;

        /// IndexBuffer.
        virtual void                    createIndexBuffer(ITF_IndexBuffer* /*_indexBuffer*/) override;
        virtual void                    releaseIndexBuffer(ITF_IndexBuffer* /*_indexBuffer*/) override;
        virtual void                    LockIndexBuffer(ITF_IndexBuffer* /*_indexBuffer*/, void** /*_data*/) override;
        virtual void                    UnlockIndexBuffer(ITF_IndexBuffer* /*_indexBuffer*/) override;

        /// Vertex/Index Buffer Draw
        virtual void                    DrawVertexBuffer(u32 /*_type*/, ITF_VertexBuffer* /*_vertexBuffer*/, u32 /*_vertexStart*/, u32 /*_vertexNumber*/) override;
        virtual void                    DrawIndexedVertexBuffer(u32 /*_type*/, ITF_VertexBuffer* /*_vertexBuffer*/, ITF_IndexBuffer* /*_indexBuffer*/, u32 /*_indexNumber*/) override;
        virtual void                    drawMesh(ITF_Mesh& /*_mesh*/, bbool /*_showWireFrame*/) override;
        virtual void                    drawMeshPT(const ITF_Mesh& /*_mesh*/, bbool /*_showWireFrame*/) override;

#ifndef ITF_CONSOLE_FINAL
        virtual void                    drawFlatMesh(const ITF_Mesh& /*_mesh*/) override;
#endif // ITF_CONSOLE_FINAL

        virtual void                    setShaderForFont(bbool /*bOutline*/) override;
        virtual void                    restoreDefaultShader() override;

        virtual const GMatrix44* getViewMatrix() override;

        virtual void        AFTERFX_BigBlur(f32 /*_factor*/, u32 /*_MaxPass*/, bbool /*_clearRT*/, GFX_QUALITY /*_quality*/ = GFX_QUALITY_MEDIUM) override;
        virtual void        AFTERFX_Glow(f32 /*_factor*/, f32 /*_glowfactor*/, f32 /*_addalpha*/, GFX_BLENDMODE _finalBlendMode = GFX_BLEND_COPY) override;
        virtual void        AFTERFX_objectsGlow(f32 /*_factor*/) override;
        virtual void        AFTERFX_RayCenter(f32 /*_v1*/, f32 /*_v2*/, f32 /*_v3*/, f32 /*_v4*/, f32 /*_v5*/, f32 /*_v6*/, f32 /*_v7*/) override;
        virtual void        AFTERFX_Remanence(f32 /*_factor*/, f32 /*_glowfactor*/, f32 /*_addalpha*/) override;
        virtual void        AFTERFX_ColorSetting(f32 /*_sat*/, f32 /*_contrast*/, f32 /*_contrastscale*/, f32 /*_bright*/) override;
        virtual void        AFTERFX_ColorRemap(Texture*) override;
        virtual void        AFTERFX_ColorLevels(f32 /*_inBlack*/, f32 /*_inGamma*/, f32 /*_inWhite*/, f32 /*_outBlack*/, f32 /*_outWhite*/) override;
        virtual void        AFTERFX_addSceneAndMul(f32 /*_mul*/) override;
        virtual void        AFTERFX_byTech(u32 /*_tech*/) override;
        virtual void        AFTERFX_byTechParams(u32 /*_tech*/, f32 /*_p0*/, f32 /*_p1*/, f32 /*_p2*/, f32 /*_p3*/) override;
        virtual void        AFTERFX_startRenderTarget(Color /*_color*/) override;
        virtual void        AFTERFX_zoomCenter(f32 /*_factor*/, Vec3d /*_dir*/) override;

        virtual void        prepareAfterFx(bbool /*_nocopy*/) override;
        virtual void        endAfterfx(bbool /*_nocopy*/, GFX_BLENDMODE /*_finalBlend*/) override;
        virtual void        prepareInternalRT() override;
        virtual void        endInternalRT() override;

        virtual void        setGammaRamp(Vec3d gamma, f32 brigthnessValue, f32 contrastValue) override { ITF_NOT_IMPLEMENTED(); }

        virtual void        AFTERFX_draw(AFX_Group* /*_group*/) override;

        virtual void        AFTERFX_prepareObjectGlow() override;

        virtual void        startImpostorRendering(renderTarget* _rt, f32 xmin, f32 ymin, f32 xmax, f32 ymax, bbool _mustClear = btrue) override;
        virtual void        stopImpostorRendering(renderTarget* _rt) override;

        /// Surface.
        virtual f32         ReadSurfaceFloat(void* /*_surf*/, u32 /*_posX*/, u32 /*_posY*/) override;

        virtual void         setAlphaRef(int _ref = 1) override;
        virtual void         setBlendSeparateAlpha() override;

        virtual void        startOverDrawRecord() override;
        virtual void        showOverDrawRecord() override;

        virtual void        setUVAnim(GFX_UVANIM& /*_uvanim*/) override;

        /// Render Context.
        virtual void        renderContext_Set() override;
        virtual void        renderContext_SetDefault() override;

        /// Global Color.
        virtual void        setGlobalColor(const Color&) override;
        virtual void        setFogColor(const Color&) override;

        virtual void        acquireDeviceOwnerShip() override;
        virtual void        releaseDeviceOwnerShip() override;
        virtual void        minimal_present() override;

        // Not virtual methods (mostly called by the SystemAdapter)
        bbool createDevice(i32 _videoOutHdl);
        Gnmx::GfxContext* getRenderingContext() { return m_mainContext->getContext(); };
        void resetRenderingStates(); // reset rendering internal states (at new frame)

    private:
        void initMemoryManagement();
        MemoryPageManager m_memoryBigPageMgrDirectMemory{};
        MemoryPageManager m_memorySmallPageMgrDirectMemory{};

        void initDebugResourceRegistration();

        // created by createDevice
        GfxDeviceContext_PS5* m_mainContext = nullptr;
        //Gnmp::PrimitiveSetup m_primitiveState;
        //Gnmp::BlendControl m_blendState;
        Gnmp::DepthStencilControl m_depthStencilState;

        void fetchScreenResolution();
        void createRenderTargets();
        void registerRenderTargetsAsVideoOut();
        i32 m_videoOutHandle = -1;
        static constexpr ux NB_SCANOUT_RENDER_TARGETS = 2;
        PS5RenderTarget m_renderTargets[NB_SCANOUT_RENDER_TARGETS]{};
        SceVideoOutBuffers m_videoOutBuffersAddress[NB_SCANOUT_RENDER_TARGETS]{}; // aliases on m_renderTargets
        u32 m_backBufferIndex = 0; // index of the back buffer to be used for rendering, 0 to NB_SCANOUT_RENDER_TARGETS, indexing m_renderTargets
        PS5RenderTarget& getBackBufferRT() { return m_renderTargets[m_backBufferIndex]; }
        PS5Texture& getBackBufferRTAsTex() { return *getBackBufferRT().asTexture; }
        PS5RenderTarget& getFrontBufferRT() { return m_renderTargets[(m_backBufferIndex-1) % NB_SCANOUT_RENDER_TARGETS]; }
        PS5Texture& getFrontBufferRTAsTex() { return *getFrontBufferRT().asTexture; }

        void setDefaultAlphaTest();

        void setRenderTarget(PS5RenderTarget* _rt);
        PS5RenderTarget* m_currentRenderTarget = nullptr;

        bool loadCoreShaders();
        bool loadShader(const char* _shaderName);

        void initDefaultRenderStates();
        void createShaderParametersDB();

        WorldViewProj m_worldViewProj{};
        // Frustum planes managements (used by setCamera)
        void updateCameraFrustumPlanes(Camera* _camera);

        void internSetTexture(int _sampler, const Texture* _texture, bool _linearFiltering = btrue); // const Texture* version of SetTexture
        void internSetTextureBind(u32 _Sampler, PS5Texture * _Bind, bool _linearFiltering = btrue);

        void drawScreenQuadCentroid(f32 _px, f32 _py, f32 _width, f32 _height, f32 _z, u32 _color, f32 _centroid, bbool _noTex = 0);

        // Shaders => tmp
        enum class CoreShaderGroup : u32
        {
            // Any change in this enum should also change the shader names GFXAdapter_Shaders_PS5.cpp
            renderPCT = 0,
            afterfx,
            refract,
            font,
            renderOVERDRAW,
            renderFog,
            movie,
            COUNT
        };
        static u32 getShaderGroupIndex(CoreShaderGroup _group) { return static_cast<u32>(_group); }
        void setShader(ITF_shader* _shader);
        ITF_shader* mp_defaultShader = nullptr; // default shader for basic renderer.
        ITF_shader* mp_currentShader = nullptr;
        GFXAdapter_shaderManager mp_shaderManager{};
        void setShader(CoreShaderGroup group);
        void setShaderMatrix(ITF_shader* _shader);

        bool m_reloadShader = false; // set this to true to reload the shaders
        void checkShaderReload();

        // low level cache to set shader program in the gnmpcontext
        Gnmp::ShaderProgramCache m_shaderProgramCache{};

        // low level cache for primitive type
        Gnmp::PrimitiveTypeCache m_primitiveTypeCache{};

        // low level cache for blend mode
        Gnmp::BlendStateCache m_blendStateCache{};

        f32 m_currentAlphaTestRef = 0.f;
        void setAlphaTestInParamDB();

        // Vertex declaration

        // use an enum to be sure to not forget one
        enum class VertexFormatAsEnum : int
        {
            P = VertexFormat_P,
            PC = VertexFormat_PC,
            PCT = VertexFormat_PCT,
            PT = VertexFormat_PT,
            PCBT = VertexFormat_PCBT,
            PNCT = VertexFormat_PNCT,           // not used. Old code?
            PNC3T = VertexFormat_PNC3T,
            PatchAnim = VertexFormat_PatchAnim, // not used, depending on define ANIMUSEGEOMETRYSHADER
            PTa = VertexFormat_PTa,
        };

        const VertexFormatDescriptor& getVertexFormatDescriptor(VertexFormatAsEnum _vertexFormat) const;
        const VertexFormatDescriptor& getVertexFormatDescriptor(u32 _vertexFormat) const;
        void setVertexBuffer(ITF_VertexBuffer* _vertexBuffer);
        VertexFormatAsEnum m_currentVertexFormat = VertexFormatAsEnum::PCT; // set by setVertexFormat. Note that setVertexFormat also set mp_currentShader->m_selectedTech
        ITF_VertexBuffer* m_currentVertexBuffer = nullptr;
        void applyVertexBuffer(ITF_VertexBuffer* _vb); // set the vertex buffer in the gnm context (to call once the VS is set, so after beginShader)
        void fillGnmpVertexBufferFormat(Gnmp::Buffer* _vertexBuffer, u8* _vertexData[], VertexFormatAsEnum _vertexFormat, u32 _nbVertex, u32& _numElem);

        void setIndexBuffer(ITF_IndexBuffer* _indexBuffer);
        PS5IndexBuffer* m_currentIndexBuffer = nullptr;
        void applyIndexBuffer(PS5IndexBuffer* _ib); // set the index buffer in the gnm context (to call once the VS is set, so after beginShader)

        FxParameterDB m_paramDB{};
        FxParameterDB::FloatHdl m_texelRatioBlendHdl{};

        float m_texelRatioBlend = 0.f;

        // UV Animation matrix
        Matrix44 m_UVmat = GMatrixIdentity;

        GFX_RECT m_lastSetViewPort{};

        // called at present
        void printDebugInfoOnScreen();

        // Patch
        void prepareShaderPatchTech(GMatrix44* _matrix, f32 _z, u32& _hdiv, u32& _vdiv, u32 _idTech, GFX_BLENDMODE _blendMode = GFX_BLEND_ALPHA);
        void shaderPatchFlushSize(u32 _hdiv, u32 _vdiv, u32 _countPoint, u32 _pass, u32 _vdivToDraw = U32_INVALID);

        // AfterFX
        void initAfterFx();
        void copyCurrentColorBuffer(u32 _rt);
        void AFTERFX_SwapFullScreenTarget();
        void AFTERFX_PrepareSwapRTDown2x2(bool _clear = true);
        void AFTERFX_SwapTargetDown2x2();

        // AfterFX Surfaces
        static const u32 AfterFXNbSwapSurface = 2;

        PS5RenderTarget m_afxFullRT[AfterFXNbSwapSurface];
        PS5RenderTarget m_afxHalfRT[AfterFXNbSwapSurface];

        PS5RenderTarget* m_pCurrentSwapRenderSurf = nullptr;
        PS5Texture* m_pCurrentSwapSourceTexture = nullptr;

        u32 m_curswap;

    #ifdef ITF_DEBUG
        i32 m_allowedShaderIdx = -1; // global variable to filter shader by index for debugging
        i32 m_allowedShaderTech = -1; // global variable to filter shader by tech
        GFX_BLENDMODE m_allowedBlendMode = GFX_BLEND_UNKNOWN;
        i32 m_allowedDrawCallIndexInFrame = -1;

        i32 m_currentShaderInBeginEndIndex = -1;
        i32 m_currentShaderInBeginEndTech = -1;
        // current blend mode if in u_CurrentBlendingMode
        i32 m_currentDrawCallIndexInFrame = -1;

        bool isDrawAllowed(bool _incrementDrawCallIndex = true);
    #else
        bool isDrawAllowed(bool _incrementDrawCallIndex = true) const { return true; }
    #endif
    };

} // namespace ITF

