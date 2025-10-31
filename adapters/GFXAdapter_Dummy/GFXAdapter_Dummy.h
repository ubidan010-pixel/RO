#pragma once

#ifdef ITF_DUMMY_GFXADAPTER

#include "engine/AdaptersInterfaces/GFXAdapter.h"

#ifdef ITF_DUMMY_ADAPTERS_ASSERT_ON_NOT_IMPLEMENTED
    #define ITF_GFX_NOT_IMPLEMENTED() ITF_ASSERT_MSG(0, "GFX not implemented")
#else
    #define ITF_GFX_NOT_IMPLEMENTED() do {} while(0)
#endif

namespace ITF
{

    class GFXAdapter_Dummy : public GFXAdapter
    {
    public:

        GFXAdapter_Dummy();

        virtual void                    project(const Vec3d& _in3d, Vec2d& _out2d) override
        {
            ITF_GFX_NOT_IMPLEMENTED(); _out2d = _in3d.truncateTo2D();
        }

#ifndef ITF_CONSOLE_FINAL
        virtual void                    drawSphere(float /*_x*/, float /*_y*/, float /*_z*/, float /*_radius*/, int /*lats*/, int /*longs*/, int /*_wireFrame*/, float /*_r*/ = 1, float /*_g*/ = 1, float /*_b*/ = 1) override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual void                    drawSphere(float /*_x*/, float /*_y*/, float /*_radius*/, float /*_r*/ = 1, float /*_g*/ = 1, float /*_b*/ = 1, float /*_z*/ = 0.f, u32 /*_segsCount*/ = 16, bbool _volume = btrue, bbool _xaxis = btrue, bbool _yaxis = btrue, bbool _zaxis = btrue) override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual void                    drawCircle(float /*_x*/, float /*_y*/, float /*_radius*/, float /*_r*/ = 1, float /*_g*/ = 1, float /*_b*/ = 1, float /*_z*/ = 0.f, u32 /*_segsCount*/ = 16) override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual void                    drawBox(const Vec2d& /*_pos*/, f32 /*_angle*/, const Vec2d& /*_extent*/, f32 /*_r*/ = 1, f32 /*_g*/ = 1, f32 /*_b*/ = 1, float /*_z*/ = 0.f, bbool /* _fill */ = bfalse) override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual void                    drawLine(const Vec2d& /*_p1*/, const Vec2d& /*_p2*/, float /*_r*/ = 1, float /*_g*/ = 1, float /*_b*/ = 1, float /*_size*/ = 1.f, float /*_z*/ = 0.f, float _alpha = 1.f) override { ITF_GFX_NOT_IMPLEMENTED(); }
#endif // ITF_CONSOLE_FINAL

        virtual void                    draw2dLine(const Vec2d& /*_p1*/, const Vec2d& /*_p2*/, float /*_size*/ = 1.f, u32 /*_color*/ = COLOR_WHITE) override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual void                    draw2dTriangle(const Vec2d& /*_p1*/, const Vec2d& /*_p2*/, const Vec2d& /*_p3*/, u32 /*_color*/ = COLOR_WHITE, i32 /*_rank*/ = 0) override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual void                    draw2dBox(const Vec2d& /*_p*/, f32 /*_width*/, f32 /*_height*/, u32 /*_color0*/ = COLOR_WHITE, u32 /*_color1*/ = COLOR_WHITE, u32 /*_color2*/ = COLOR_WHITE, u32 /*_color3*/ = COLOR_WHITE, i32 /*_rank*/ = 0) override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual void                    drawTriangle(const Vec2d& /*_p1*/, const Vec2d& /*_p2*/, const Vec2d& /*_p3*/, float /*_r*/ = 1, float /*_g*/ = 1, float /*_b*/ = 1, float /*_a*/ = 1, float /*_z*/ = 0.f) override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual void                    drawTriangle(const Vertex& /*_v1*/, const Vertex& /*_v2*/, const Vertex& /*_v3*/, const Texture* /*_tex*/, bbool /*_showWireFrame*/ = 0) override { ITF_GFX_NOT_IMPLEMENTED(); }

        virtual void                    drawQuad2D(f32 /*_px*/, f32 /*_py*/, f32 /*_width*/, f32 /*_height*/, f32 /*_z*/, u32 /*_color*/ = COLOR_WHITE, const Texture* /*_tex*/ = NULL, const Vec2d* /*_uvStart */= NULL, const Vec2d* /*_uvEnd */= NULL) override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual void                    draw3DQuad(GFX_3DQUAD* /*_t_Quad*/) override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual bbool                   loadTexture(Texture* /*_tex*/, const char* /*_path*/, u32 /*_alpha*/ = 0) override { ITF_GFX_NOT_IMPLEMENTED(); return btrue; }

        virtual void                    coord2DTo3D(const Vec2d& _in2d, float _zPlane, Vec2d& _out3d, bool /*noTranslation*/ = btrue) override { ITF_GFX_NOT_IMPLEMENTED(); _out3d = _in2d; }
        virtual void                    compute3DTo2D(const Vec3d& _in, Vec3d& _out) override { ITF_GFX_NOT_IMPLEMENTED(); _out = _in; }
        virtual void                    compute2DTo3D(const Vec3d& _in, Vec3d& _out) override { ITF_GFX_NOT_IMPLEMENTED(); _out = _in; }

        virtual void                    drawScreenQuad(f32 /*_px*/, f32 /*_py*/, f32 /*_width*/, f32 /*_height*/, f32 /*_z*/, u32 /*_color*/, bbool /*_noTex*/ = 0) override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual                         ~GFXAdapter_Dummy() override;
        virtual void                    init() override { ITF_GFX_NOT_IMPLEMENTED(); }

        virtual void                    init2DRender() override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual void                    init2DGuiRender() override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual void                    setOrthoView(f32 /*_l*/, f32 /*_r*/, f32 /*_b*/, f32 /*_t*/) override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual void                    setOrthoView2DYNeg(f32 /*_l*/, f32 /*_r*/, f32 /*_b*/, f32 /*_t*/) override { ITF_GFX_NOT_IMPLEMENTED(); }

        virtual void                    setAlphaBlend(GFX_BLENDMODE /*_blend*/) override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual void                    setCullMode(GFX_CULLMODE /*_cull*/) override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual void                    setFillMode(GFX_FILLMODE /*_fillmode*/) override { ITF_GFX_NOT_IMPLEMENTED(); }

        virtual void                    depthMask(u32 /*_depthMask*/) override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual void                    depthFunc(u32 /*_func*/) override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual void                    depthTest(u32 /*_test*/) override { ITF_GFX_NOT_IMPLEMENTED(); }

        virtual void                    setScissorRect(GFX_RECT* /*_clipRect*/) override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual void                    SetTextureBind(int /*_Sampler*/, void* /*_Bind*/, bbool /*_linearFiltering*/ = btrue) override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual void                    lockTexture(Texture* /*_tex*/, LOCKED_TEXTURE* _lockTex, u32 /*_flag*/ = 0) override { *_lockTex = LOCKED_TEXTURE{}; ITF_GFX_NOT_IMPLEMENTED(); }
        virtual void                    unlockTexture(Texture* /*_tex*/) override { ITF_GFX_NOT_IMPLEMENTED(); }

        virtual void                    cleanupTexture(Texture* /*_texture*/) override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual void                    DrawPrimitive(PRIMITIVETYPE /*_type*/, const void* /*_p_Vertex*/, u32 /*_NumberVertex*/) override { ITF_GFX_NOT_IMPLEMENTED(); }

        // Matrix.
        virtual void                    setMatrixTransform(MATRIXMODE /*_type*/, const GMatrix44* /*_Matrix*/) override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual void                    setupViewport(GFX_RECT* /*_Viewport*/) override { ITF_GFX_NOT_IMPLEMENTED(); }
#if !defined(ITF_FINAL)
        virtual void                    unprojectPoint(const Vec2d& /*_in*/, float /*_viewDistance*/, const GMatrix44* /*_MatrixModel*/, const GMatrix44* /*_MatrixProj*/, const int* /*_View*/, Vec2d& /*_out*/) override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual void                    setLookAtMatrix(GMatrix44* /*_MatrixOut*/, double /*_midx*/, double /*_midy*/, float /*_viewDistance*/, float /*_aspect*/) override { ITF_GFX_NOT_IMPLEMENTED(); }
#endif // !defined(ITF_FINAL)
        virtual void                    getViewport(GFX_RECT* /*_RectOut*/) override { ITF_GFX_NOT_IMPLEMENTED(); }

        /// GfxMaterials.
        virtual void                    setGfxMaterial(const GFX_MATERIAL& /*_gfxMat*/) override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual void                    setGfxMatDefault() override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual void                    setGfxMatRefraction(const GFX_MATERIAL& /*_gfxMat*/) override { ITF_GFX_NOT_IMPLEMENTED(); }

        /// Texture.
        virtual void                    generateTexture(Texture* /*_texture*/) override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual void                    createTexture(Texture* /*_texture*/, u32 /*_sizeX*/, u32 /*_sizeY*/, u32 /*_mipLevel*/, Texture::PixFormat /*_pixformat*/, u32 /*_pool*/ = 1, bbool /*_dynamic*/ = bfalse) override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual void                    cleanupBaseTexture(Texture* /*_texture*/) override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual void                    setTextureSize(Texture* /*_texture*/, const Size& /*_size*/) override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual unsigned char*          grabTexture(Texture* /*_texture*/) override { ITF_GFX_NOT_IMPLEMENTED(); return NULL; }
        virtual u32                     getMaxTextureSize() override { ITF_GFX_NOT_IMPLEMENTED(); return 4096; }
        virtual void                    loadFromMemory(const void* /*_buffer*/, const Size& /*_texture_size*/, Texture* /*_texture*/, Texture::PixFormat /*_pixel_format*/, const u32 /*_buffer_size*/, bbool /*bCooked*/, u32 _mipmapCount) override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual bbool                   loadFromFileInMemory(Texture* _texture, const void* _buffer, u32 _contentSize) override { ITF_GFX_NOT_IMPLEMENTED(); return btrue; }
        virtual void                    SetTexture(int /*_Sampler*/, Texture* /*_Texture*/, bbool /*_linearFiltering*/ = btrue) override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual void                    saveToMemory(void* /*_buffer*/, Texture* /*_Tex*/) override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual float                   getTexelOffset() override { ITF_GFX_NOT_IMPLEMENTED(); return 0.0f; }

        virtual void                    setSamplerState(u32 /*_samp*/, GFX_SAMPLERSTATETYPE /*_type*/, i32 /*_value*/) override { ITF_GFX_NOT_IMPLEMENTED(); }

        // Texture Target.   
        virtual void                    clearRenderTarget(renderTarget* /*_target*/, u32 /*_color*/) override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual void                    initialiseRenderTarget(renderTarget* /*_target*/) override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual void                    resizeRenderTarget(renderTarget* /*_target*/, const Size& /*_size*/) override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual void                    enableRenderTarget(renderTarget* /*_target*/) override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual void                    disableRenderTarget(renderTarget* /*_target*/) override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual void                    cleanupRenderTarget(renderTarget* /*_target*/) override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual bbool                   isRenderTargetInverted() override { ITF_GFX_NOT_IMPLEMENTED(); return bfalse; }

        virtual void                    clear(u32 /*_buffer*/, float /*_r*/ = 0.f, float /*_g*/ = 0.f, float /*_b*/ = 0.f, float /*_a*/ = 0.f) override { ITF_GFX_NOT_IMPLEMENTED(); }

        virtual void                    present() override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual void                    startRendering() override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual void                    endRendering() override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual bbool                   waitPreviousGPUframeEnd() override { ITF_GFX_NOT_IMPLEMENTED(); return btrue; }
        virtual void                    syncGPU() override { ITF_GFX_NOT_IMPLEMENTED(); }

        virtual bbool                   setScreenProtection(bbool _enable) override { return btrue; }

        virtual void                    reshape() override { ITF_GFX_NOT_IMPLEMENTED(); }

        virtual bbool                   isDeviceLost() override { ITF_GFX_NOT_IMPLEMENTED(); return bfalse; }
        virtual bbool                   resetDevice(bbool /*_forceReset*/) override { ITF_GFX_NOT_IMPLEMENTED(); return bfalse; }

        /// Vertex declaration.
        virtual void                    setVertexFormat(u32 /*_vformat*/) override { ITF_GFX_NOT_IMPLEMENTED(); }

        virtual const String            getRenderName() override { ITF_GFX_NOT_IMPLEMENTED(); return "Dummy"; }

        /// Shaders.
        virtual void                    createShader(ITF_shader* /*_shader*/) override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual void                    releaseShader(ITF_shader* /*_shader*/) override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual bbool                   loadShader(ITF_shader* /*_shader*/) override { ITF_GFX_NOT_IMPLEMENTED(); return btrue; }
        virtual void                    beginShader(ITF_shader* /*_shader*/) override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual void                    endShader(ITF_shader* /*_shader*/) override { ITF_GFX_NOT_IMPLEMENTED(); }

        virtual void                    fx_setVectorArray(uPtr /*_fx*/, void* /*_h*/, const GFX_Vector4* /*_va*/, u32 /*_count*/) override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual void                    fx_setVector(uPtr /*_fx*/, void* /*_h*/, const GFX_Vector4* /*_v*/) override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual void                    fx_setFloat(uPtr /*_fx*/, void* /*_h*/, float/*_f*/) override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual void                    fx_setInt(uPtr /*_fx*/, void* /*_h*/, i32/*_i*/) override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual void                    fx_commitChange(uPtr /*_fx*/) override { ITF_GFX_NOT_IMPLEMENTED(); }

        /// Patch shader.
        virtual void                    prepareShaderPatch(GMatrix44* /*_matrix*/, f32 /*_z*/, u32&/*_hdiv*/, u32&/*_vdiv*/, GFX_BLENDMODE _blendMode = GFX_BLEND_ALPHA) override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual void                    shaderDrawPatch32(const Texture* /*texture*/, const Vec2d* /*PointsNS*/, const ColorInteger* /*_lighttab*/) override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual void                    endShaderPatch() override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual void                    shaderPatchFlush(u32 /*_hdiv*/, u32 /*_vdiv*/, u32 /*_pass*/) override { ITF_GFX_NOT_IMPLEMENTED(); }

        /// Bezier Patch shader.
        virtual void                    prepareShaderBezierPatch(GMatrix44* /*_matrix*/, f32 /*_z*/, u32&/*_hdiv*/, u32&/*_vdiv*/, GFX_BLENDMODE _blendMode = GFX_BLEND_ALPHA) override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual bbool                   shaderDrawBezierPatch32(Texture* /*texture*/, Vec2d* /*PointsNS*/, ColorInteger* /*_lighttab*/, f32* /*_widths*/, f32* /*_alphas*/, f32* /*_widths2*/) override { ITF_GFX_NOT_IMPLEMENTED(); return btrue; }
        virtual void                    shaderBezierPatchFlush(u32 /*_hdiv*/, u32 /*_vdiv*/, u32 /*_pass*/, u32 /*_vdivToDraw*/) override { ITF_GFX_NOT_IMPLEMENTED(); }

        /// Spline Shader
        virtual void                    drawSpline(GMatrix44* _matrix, Texture* _texture, const Spline* _spline, const Vec3d& _pos, f32 _height) override { ITF_GFX_NOT_IMPLEMENTED(); }

        /// Fluid Shader.
        virtual void                    drawFluid(GMatrix44* _matrix, Texture* _texture, const FixedArray<u32, GFXADAPTER_FLUID_COLORTAB_ENTRY_COUNT>& _colorTab, const Vec2d _heightTab[], const u32 _heightTabSize, const FixedArray<Vec2d, 4>& _uvTab, u32 mode, f32 _delta1, f32 _delta2, f32 _stepCount) override { ITF_GFX_NOT_IMPLEMENTED(); }

        /// Trail 3d Shader.
        virtual void                    drawTrail3D(Texture* _texture, Color _color, const SafeArray<Vec3d>& points, f32 _alphaBegin, f32 _alphaEnd, f32 _fadeLength) override { ITF_GFX_NOT_IMPLEMENTED(); }

        //Movie    
        virtual void                    drawMovie(GMatrix44* _matrix, f32 _alpha, ITF_VertexBuffer* _pVertexBuffer, Texture** _pTextureArray, u32 _countTexture) override { ITF_GFX_NOT_IMPLEMENTED(); }

        /// VertexBuffer.
        virtual void                    createVertexBuffer(ITF_VertexBuffer* /*_vertexBuffer*/) override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual void                    releaseVertexBuffer(ITF_VertexBuffer* /*_vertexBuffer*/) override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual void                    LockVertexBuffer(ITF_VertexBuffer* /*_vertexBuffer*/, void** _data, u32 /*_offset*/, u32 /*_size*/, u32 /*_flag*/) override { *_data = nullptr; ITF_GFX_NOT_IMPLEMENTED(); }
        virtual void                    UnlockVertexBuffer(ITF_VertexBuffer* /*_vertexBuffer*/) override { ITF_GFX_NOT_IMPLEMENTED(); }

        /// IndexBuffer.
        virtual void                    createIndexBuffer(ITF_IndexBuffer* /*_indexBuffer*/) override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual void                    releaseIndexBuffer(ITF_IndexBuffer* /*_indexBuffer*/) override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual void                    LockIndexBuffer(ITF_IndexBuffer* /*_indexBuffer*/, void** _data) override { *_data = nullptr; ITF_GFX_NOT_IMPLEMENTED(); }
        virtual void                    UnlockIndexBuffer(ITF_IndexBuffer* /*_indexBuffer*/) override { ITF_GFX_NOT_IMPLEMENTED(); }

        /// Vertex/Index Buffer Draw
        virtual void                    DrawVertexBuffer(u32 /*_type*/, ITF_VertexBuffer* /*_vertexBuffer*/, u32 /*_vertexStart*/, u32 /*_vertexNumber*/) override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual void                    DrawIndexedVertexBuffer(u32 /*_type*/, ITF_VertexBuffer* /*_vertexBuffer*/, ITF_IndexBuffer* /*_indexBuffer*/, u32 /*_indexNumber*/) override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual void                    drawMesh(ITF_Mesh& /*_mesh*/, bbool /*_showWireFrame*/) override { ITF_GFX_NOT_IMPLEMENTED(); };
        virtual void                    drawMeshPT(const ITF_Mesh& /*_mesh*/, bbool /*_showWireFrame*/) override { ITF_GFX_NOT_IMPLEMENTED(); }

#ifndef ITF_CONSOLE_FINAL
        virtual void                    drawFlatMesh(const ITF_Mesh& /*_mesh*/) override { ITF_GFX_NOT_IMPLEMENTED(); }
#endif // ITF_CONSOLE_FINAL

        virtual void                    setShaderForFont(bbool /*bOutline*/) override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual void                    restoreDefaultShader() override { ITF_GFX_NOT_IMPLEMENTED(); }

        virtual const GMatrix44* getViewMatrix() override { ITF_GFX_NOT_IMPLEMENTED(); return nullptr; }

        virtual void		AFTERFX_BigBlur(f32 /*_factor*/, u32 /*_MaxPass*/, bbool /*_clearRT*/, GFX_QUALITY /*_quality*/ = GFX_QUALITY_MEDIUM) override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual void        AFTERFX_Glow(f32 /*_factor*/, f32 /*_glowfactor*/, f32 /*_addalpha*/, GFX_BLENDMODE _finalBlendMode = GFX_BLEND_COPY) override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual void		AFTERFX_objectsGlow(f32 /*_factor*/) override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual void        AFTERFX_RayCenter(f32 /*_v1*/, f32 /*_v2*/, f32 /*_v3*/, f32 /*_v4*/, f32 /*_v5*/, f32 /*_v6*/, f32 /*_v7*/) override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual void        AFTERFX_Remanence(f32 /*_factor*/, f32 /*_glowfactor*/, f32 /*_addalpha*/) override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual void        AFTERFX_ColorSetting(f32 /*_sat*/, f32 /*_contrast*/, f32 /*_contrastscale*/, f32 /*_bright*/) override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual void        AFTERFX_ColorRemap(Texture*) override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual void        AFTERFX_ColorLevels(f32 /*_inBlack*/, f32 /*_inGamma*/, f32 /*_inWhite*/, f32 /*_outBlack*/, f32 /*_outWhite*/) override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual void        AFTERFX_addSceneAndMul(f32 /*_mul*/) override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual void        AFTERFX_byTech(u32 /*_tech*/) override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual void        AFTERFX_byTechParams(u32 /*_tech*/, f32 /*_p0*/, f32 /*_p1*/, f32 /*_p2*/, f32 /*_p3*/) override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual void        AFTERFX_startRenderTarget(Color /*_color*/) override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual void        AFTERFX_zoomCenter(f32 /*_factor*/, Vec3d /*_dir*/) override { ITF_GFX_NOT_IMPLEMENTED(); }

        virtual void        prepareAfterFx(bbool /*_nocopy*/) override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual void        endAfterfx(bbool /*_nocopy*/, GFX_BLENDMODE /*_finalBlend*/) override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual void        prepareInternalRT() override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual void        endInternalRT() override { ITF_GFX_NOT_IMPLEMENTED(); }

        virtual void        setGammaRamp(Vec3d gamma, f32 brigthnessValue, f32 contrastValue) override { ITF_GFX_NOT_IMPLEMENTED(); }

        virtual void        AFTERFX_draw(AFX_Group* /*_group*/) override { ITF_GFX_NOT_IMPLEMENTED(); }

        virtual void        AFTERFX_prepareObjectGlow() override { ITF_GFX_NOT_IMPLEMENTED(); }

        virtual void        startImpostorRendering(renderTarget* /*_rt*/, f32 /*xmin*/, f32 /*ymin*/, f32 /*xmax*/, f32 /*ymax*/, bbool _mustClear = btrue) override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual void        stopImpostorRendering(renderTarget* /*_rt*/) override { ITF_GFX_NOT_IMPLEMENTED(); }

        /// Surface.
        virtual f32         ReadSurfaceFloat(void* /*_surf*/, u32 /*_posX*/, u32 /*_posY*/) override { ITF_GFX_NOT_IMPLEMENTED(); return 0.0f; }

        virtual void        startOverDrawRecord() override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual void        showOverDrawRecord() override { ITF_GFX_NOT_IMPLEMENTED(); }

        virtual void        setUVAnim(GFX_UVANIM& /*_uvanim*/) override { ITF_GFX_NOT_IMPLEMENTED(); }

        /// Render Context.
        virtual void        renderContext_Set() override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual void        renderContext_SetDefault() override { ITF_GFX_NOT_IMPLEMENTED(); }

        /// Global Color.
        virtual void        setGlobalColor(const Color&) override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual void        setFogColor(const Color&) override { ITF_GFX_NOT_IMPLEMENTED(); }

        virtual void        acquireDeviceOwnerShip() override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual void        releaseDeviceOwnerShip() override { ITF_GFX_NOT_IMPLEMENTED(); }
        virtual void        minimal_present() override { ITF_GFX_NOT_IMPLEMENTED(); }
    };

} // namespace ITF

#endif // ITF_DUMMY_GFXADAPTER
