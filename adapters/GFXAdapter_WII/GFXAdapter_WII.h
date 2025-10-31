#ifndef _ITF_WII_DRAWPRIM_H_
#define _ITF_WII_DRAWPRIM_H_

#ifdef ITF_WII
#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif

#ifndef _ITF_RASTERSMANAGER_H_
#include "engine/rasters/rastersmanager.h"
#endif //_ITF_RASTERSMANAGER_H_

#ifndef _ITF_GFXADAPTER_SHADERMANAGER_H_
#include "engine/AdaptersInterfaces/GFXAdapter_ShaderManager.h"
#endif //_ITF_GFXADAPTER_SHADERMANAGER_H_

namespace ITF
{	
struct GFXDisplayList
{
    void *m_ptr;
    u32   m_size;
    bool  m_bUseInd16;
    u32   m_usage;
};

class GFXAdapter_WII : public GFXAdapter
{
    public:
        enum Tev_Type
        {
            TEV_TC,
            TEV_TA,
            TEV_T,
            TEV_C,
            
            TEV_AFX_2X,
            TEV_AFX_BIGBLUR,
            TEV_AFX_GLOW_COPY,
            TEV_AFX_GLOW_2X,
            TEV_REFRACTION,
            
            TEV_MAX
        };

        enum RT_Pass
        {
            RT_PASS_COLOR,
            RT_PASS_ALPHA
        };

    public:
		GFXAdapter_WII();
		~GFXAdapter_WII();

		void			init();
#ifndef ITF_FINAL
		void			reshape();
        void			project(const Vec3d& _in3d, Vec2d & _out2d);
#endif // ITF_FINAL
#ifndef ITF_CONSOLE_FINAL
		void			drawSphere(float _x, float _y, float _z, float r, int lats, int longs, int _wireFrame, float _r=1, float _g=1, float _b=1);
		void			drawCircle(float _x, float _y, float r, float _r=1, float _g=1, float _b=1, float _z = 0.f, u32 _segsCount = 32);
        void			drawBox(const Vec2d& _pos, f32 _angle, const Vec2d& _extent, f32 _r=1, f32 _g=1, f32 _b=1, float _z = 0.f, bbool _fill = bfalse);
		void			drawLine(const Vec2d& _p1, const Vec2d& _p2, float _r=1, float _g=1, float _b=1, float _size=1.f, float _z = 0.f);
		void			draw2dLine(const Vec2d& _p1, const Vec2d& _p2, float _size=1.f, u32 _color = COLOR_WHITE);
        void            draw2dTriangle(const Vec2d& _p1, const Vec2d& _p2, const Vec2d& _p3, u32 _color = COLOR_WHITE, i32 _rank = 0);
		void			draw2dBox(const Vec2d& _p, f32 _width, f32 _height, u32 _color0 = COLOR_WHITE, u32 _color1 = COLOR_WHITE, u32 _color2 = COLOR_WHITE, u32 _color3 = COLOR_WHITE, i32 _rank = 0);
		void			drawTriangle(const Vec2d& _p1, const Vec2d& _p2, const Vec2d& _p3, float _r=1, float _g=1, float _b=1, float _a=1, float _z = 0.f);
		void			drawTriangle(const Vertex& _v1, const Vertex& _v2, const Vertex& _v3, const Texture* _tex, bbool _showWireFrame = 0);
#endif // ITF_CONSOLE_FINAL
		void			drawQuad2D(f32 _px, f32 _py, f32 _width, f32 _height, f32 _z, u32 _color = COLOR_WHITE, const Texture* _tex = NULL, const Vec2d* _uvStart = NULL,  const Vec2d* _uvEnd = NULL);
		void			draw3DQuad(GFX_3DQUAD* _t_Quad); 

		bbool			loadTexture(Texture* _tex, const char* _path, u32 _alhpa = 0);
		bbool       	loadTextureCooked(Texture* _t, const char* _path);

        void            GDK_images_TileForWii(void *_dst, const void *_src, const u16 W, const u16 H, const u32 _mipMapLevels);
        void            GDK_buffer_TileForWii(void *_pv_DestImg, const void *_pv_SrcImg, u32 _u32_Width, u32 _u32_Height, u32 _u32_BPP);        

		void            loadTextureTGA(const void *_buffer, u32 _contentSize, unsigned _target, int _level, bool& _useAlpha, bool _is2D = true, u32 *_width=0, u32 *_height=0, unsigned _forceformat=0);
		bbool           loadFromFileInMemory(Texture *_t,const void* _buffer, u32 _contentSize);
		void			drawScreenQuad(f32 _px, f32 _py, f32 _width, f32 _height, f32 _z, u32 _color, bbool _noTex = 0);
		void			drawScreenQuadInternal(f32 _px, f32 _py, f32 _width, f32 _height, f32 _z, u32 _color, bbool _noTex = 0, Tev_Type _type = TEV_TC);

		void			setCamera(float _x, float _y, float _z, Camera* _cam);
		void			init2DRender();
		void			setOrthoView(f32 _l, f32 _r, f32 _b, f32 _t);
        void			setResolution_WII(u32 _screenWidth, u32 _screenHeight, u8 _aspectRatio);

		void			CalcCoord(const Vec2d& _in2d, float _zPlane, Vec2d& _out3d);

		void			clear(u32 _buffer, u32 _Col = 0);
		void			clear(u32 _buffer, float _r=0.f, float _g=0.f, float _b=0.f, float _a=0.f);
		void			setAlphaBlend(GFX_BLENDMODE _Blend);
        bbool			useSrcAlphaForBlend(GFX_BLENDMODE _Blend);
		void			setCullMode(GFX_CULLMODE _cull);
		void			depthMask(u32 _depthMask);
		void			depthFunc(u32 _func);
		void			depthTest(u32 _test);
		void			setScissorRect(GFX_RECT* _clipRect);
		void			SetTextureBind(int _Sampler, unsigned int _Bind, bbool _linearFiltering = btrue);
		void			cleanupTexture(Texture* _texture);

        void            setVertexFormat(u32 _vformat);
		void			DrawPrimitive(PRIMITIVETYPE _type, const void *_p_Vertex, u32 _NumberVertex);

		void			setMatrixTransform(MATRIXMODE _type, const GMatrix44 *_Matrix);
		void			setupViewport(GFX_RECT* _Viewport);
#ifndef ITF_FINAL
		void			unprojectPoint(const Vec2d& _in, float _viewDistance, const GMatrix44 *_MatrixModel, const GMatrix44 *_MatrixProj, const int *_View, Vec2d& _out);
		void			setLookAtMatrix(GMatrix44* _MatrixOut, double _midx, double _midy, float _viewDistance, float _aspect);
#endif // ITF_FINAL
		void			getViewport(GFX_RECT *_RectOut);

        ITF_INLINE f32  getRealScreenWidth()  const  { return mv_RealScreenSize.m_x; }
        ITF_INLINE f32  getRealScreenHeight() const  { return mv_RealScreenSize.m_y; }

        ITF_INLINE f32  getEFBScaleWidth()    const  { return mf32_EFBScale[0]; }
        ITF_INLINE f32  getEFBScaleHeight()   const  { return mf32_EFBScale[1]; }

		/// GfxMaterials.
		void		    setGfxMaterial(const GFX_MATERIAL& _gfxMat);
		void            unsetGfxMaterial(const GFX_MATERIAL& _gfxMat, Tev_Type _curTevType);
		void			setGfxMatDefault();
		void			setGfxMatRefraction(const GFX_MATERIAL& _gfxMat);

		/// TEV
		void			setTev(Tev_Type _type);
		void            finalizeTevSetting();

		/// Texture.
		void			generateTexture(Texture* _texture);
		void			cleanupBaseTexture(Texture* _texture);
		void			setTextureSize(Texture* _texture, const Size& _size);
		unsigned char*	grabTexture(Texture* _texture);
		u32				getMaxTextureSize();
		void			loadFromMemory(const void* _buffer, const Size& _texture_size, Texture* _texture, Texture::PixFormat _pixel_format,const u32 _buffer_size,bbool bCooked, u32 _mipmapCount);
		void			SetTexture(int _Sampler, Texture* _Texture, bbool _linearFiltering = btrue);
		void			lockTexture(Texture* _tex, LOCKED_TEXTURE* _lockTex, u32 _flag);
		void			unlockTexture(Texture* _tex);
		void			saveToMemory(void* _buffer, Texture* _Tex);
		float			getTexelOffset() {return 0;}

		void            setSamplerState( u32 _samp, GFX_SAMPLERSTATETYPE _type, i32 _value );

		void			clearRenderTarget(renderTarget* _target, u32 _color);
		void			initialiseRenderTarget(renderTarget* _target);
		void			resizeRenderTarget(renderTarget* _target, const Size& _size);
		void			enableRenderTarget(renderTarget* _target);
		void			disableRenderTarget(renderTarget* _target);
		void			cleanupRenderTarget(renderTarget* _target);
		bbool			isRenderTargetInverted() {return bfalse;}
		void			doScreenCapture() {}
		void			startScreenCapture() {}
		void			stopScreenCapture() {}
		void			setShaderForFont(bbool bOutline);
		void			restoreDefaultShader();

		void			present();
		void			startRendering();
		void			endRendering();
        bbool           waitPreviousGPUframeEnd();
        void            syncGPU();

        bbool           setScreenProtection(bbool _enable);

		void			LockRendering();
		void			UnlockRendering();

		void            resetGX();

        #ifndef ITF_FINAL
        /// fps from system information.
        ITF_INLINE f32                  getfPsNoVsync()         {return m_fPsNoVsync;};
        ITF_INLINE void                 setfPsNoVsync(f32 _fps) {m_fPsNoVsync = _fps;};
        #endif // ITF_FINAL

		void        	setObjectMatrix(const GMatrix44& _matrix);
		void        	setObjectMatrix(const GMatrix44& _matrix, GXPosNrmMtx _mtxID);

		// Fluid Shader.
        void            drawFluid(GMatrix44* _matrix, Texture* _texture, const FixedArray<u32, GFXADAPTER_FLUID_COLORTAB_ENTRY_COUNT> &_colorTab, const Vec2d _heightTab[], const u32 _heightTabSize, const FixedArray<Vec2d, 4> &_uvTab, u32 mode, f32 _delta1, f32 _delta2, f32 _stepCount );

		/// VertexBuffer.
		void			createVertexBuffer( ITF_VertexBuffer* _vertexBuffer);
		void			releaseVertexBuffer(ITF_VertexBuffer* _vertexBuffer);
		void			setVertexBuffer(ITF_VertexBuffer* _vertexBuffer);
		void			LockVertexBuffer(ITF_VertexBuffer* _vertexBuffer, void** _data, u32 _offset, u32 _size, u32 _flag);
		void			UnlockVertexBuffer(ITF_VertexBuffer* _vertexBuffer);

		/// IndexBuffer.
		void			createIndexBuffer(ITF_IndexBuffer* _indexBuffer);
		void			releaseIndexBuffer(ITF_IndexBuffer* _indexBuffer);
		void			setIndexBuffer(ITF_IndexBuffer* _indexBuffer);
		void			LockIndexBuffer(ITF_IndexBuffer* _indexBuffer, void** _data);
		void			UnlockIndexBuffer(ITF_IndexBuffer* _indexBuffer);

private:
        void        	updateWiiObjectMatrix();

        /// Vertex/Index Buffer Draw.
		void 			DrawIndexedPC(ITF_IndexBuffer* _indexBuffer, ITF_VertexBuffer* _vertexBuffer, u32 _indexNumber);
		void 			DrawIndexedPT(ITF_IndexBuffer* _indexBuffer, ITF_VertexBuffer* _vertexBuffer, u32 _indexNumber);
		void 			DrawIndexedPCT(ITF_IndexBuffer* _indexBuffer, ITF_VertexBuffer* _vertexBuffer, u32 _indexNumber);
		void 			DrawIndexedPNC3T(ITF_IndexBuffer* _indexBuffer, ITF_VertexBuffer* _vertexBuffer, u32 _indexNumber);
		void 			DrawIndexedPCBT(ITF_IndexBuffer* _indexBuffer, ITF_VertexBuffer* _vertexBuffer, u32 _indexNumber);

		void 			DrawPC(ITF_VertexBuffer* _vertexBuffer, u32 _st, u32 _nb);
		void 			DrawPT(ITF_VertexBuffer* _vertexBuffer, u32 _st, u32 _nb);
		void 			DrawPCT(ITF_VertexBuffer* _vertexBuffer, u32 _st, u32 _nb);
		void 			DrawPNC3T(ITF_VertexBuffer* _vertexBuffer, u32 _st, u32 _nb);
		void 			DrawPCBT(ITF_VertexBuffer* _vertexBuffer, u32 _st, u32 _nb);

        void            DrawBegin(u32 _type, u32 _nverts);
        void            DrawEnd();

        void            Send_frize_PNC3T_Pos(const VertexPNC3T &_v) const;
        void            computeMeshElement(const ITF_VertexBuffer* _vertexBufferIn, const ITF_Mesh& _mesh, const ITF_MeshElement& _elem);

        ITF_INLINE bool NeedDupUV() const { return m_CurrentTevType == TEV_REFRACTION; }

public:
		void			DrawVertexBuffer(u32 _type, ITF_VertexBuffer* _vertexBuffer, u32 _vertexStart, u32 _vertexNumber);
		void			DrawIndexedVertexBuffer(u32 _type, ITF_VertexBuffer* _vertexBuffer, ITF_IndexBuffer* _indexBuffer, u32 _indexNumber);

		void			drawMesh(ITF_Mesh& _mesh, bbool _showWireFrame);
		void            drawMeshPT(const ITF_Mesh& _mesh, bbool _showWireFrame);
#ifndef ITF_CONSOLE_FINAL
		void			drawFlatMesh(const ITF_Mesh& _mesh);
#endif // ITF_CONSOLE_FINAL

public:
		/// After FX.
		void			copyCurrentColorBuffer(u32 _rt);
		void			AFTERFX_BigBlur(f32 _factor, u32 _MaxPass, bbool _clearRT, GFX_QUALITY _quality);
		void			AFTERFX_Glow(f32 _factor, f32 _glowfactor, f32 _addalpha, GFX_BLENDMODE _finalBlendMode);
		void            AFTERFX_objectsGlow(f32 _factor);
		void			AFTERFX_Remanence(f32 _factor, f32 _glowfactor, f32 _addalpha);
		void			AFTERFX_RayCenter(f32 _v1, f32 _v2, f32 _v3, f32 _v4, f32 _v5, f32 _v6, f32 _v7);
		void			AFTERFX_ColorSetting(f32 _sat, f32 _contrast, f32 _contrastscale, f32 _bright);
		void			AFTERFX_ColorRemap(Texture* _tex);
		void            AFTERFX_ColorLevels(f32 _inBlack,  f32 _inGamma, f32 _inWhite, f32 _outBlack, f32 _outWhite);
		void			AFTERFX_addSceneAndMul(f32 _mul);
		void			AFTERFX_byTech(u32 _tech);
		void			AFTERFX_byTechParams(u32 _tech, f32 _p0, f32 _p1, f32 _p2, f32 _p3);
		void			AFTERFX_draw(AFX_Group* _group);
		void			AFTERFX_startRenderTarget(Color _color);
		void            AFTERFX_prepareObjectGlow();
		void            prepareAfterFx(bbool _nocopy);
		void            endAfterfx(bbool _nocopy, GFX_BLENDMODE _finalBlend);
        void            prepareInternalRT();
        void            endInternalRT();

		void			startImpostorRendering(renderTarget* _rt, f32 xmin, f32 ymin, f32 xmax, f32 ymax, bbool _mustClear);
        void            startImpostorAlphaRendering(renderTarget* _rt);
		void			stopImpostorRendering(renderTarget* _rt);

		u32				getAFXBufferSize();
        u8*             lockAFXBuffer(u32 _size);
        void            unlockAFXBuffer();

        // Textures
        GXTexFmt            PixFormat_TO_CopyTexFmt(Texture::PixFormat _format);
        GXTexFmt            PixFormat_TO_GXTexFmt(Texture::PixFormat _format);
        Texture::PixFormat  GXTexFmt_TO_PixFormat(GXTexFmt _format);
        void                createDynamicTexture(Texture *_p_Tex, u16 _W, u16 _H, Texture::PixFormat _format);
        void                initializeDynamicTexture(Texture *_p_Tex, u16 _W, u16 _H, Texture::PixFormat _format, unsigned long _rgbBufferSize=0, unsigned long _aBufferSize=0);
        void			    copyCurrentColorBuffer(Texture *_p_Tex, u16 l, u16 t, u16 w, u16 h);

		/// UV.
		void			setUVAnim(const GFX_UVANIM& _uvanim);
        void			resetUVAnim();

		/// Render Context.
		void			renderContext_Set();
		void			renderContext_SetDefault();

		void			setGlobalColor(const Color& _color);
    	void	        setFogColor(const Color& _color);
    	void            disableFog() { setFogColor(Color::zero()); }
   
		const GMatrix44*		getViewMatrix();

		const String	getRenderName() {return "WII";}

		void			DisplayMessageBox(const String& _title, const String& _msg, u32 _type);
		void			DisplayMessageBox(const String& _msgString, u32 _maxcharline = (u32)-1);

        #ifndef ITF_FINAL
        f64             getVBLSyncTime() const      { return mf64_VBLSyncTime;   }
        void            setVBLSyncTime(f64 _value)  { mf64_VBLSyncTime = _value; }
        #endif // ITF_FINAL

        void            ForceDeprecatedDL(const ITF_IndexBuffer* _IndexBuffer);

	private:
		void*			createFramebuffer(u32 _size);
		void			configureMem();
		void			initGX();
		void			setRenderMode(GXRenderModeObj* _mode);
		void			startVI();

		void			extractFrustumPlanes
							(
								const GMatrix44	&_m,
								Quaternion		&_leftPlane,
								Quaternion		&_rightPlane,
								Quaternion		&_topPlane,
								Quaternion		&_bottomPlane,
								Quaternion		&_nearPlane
							);
		void			updateCameraFrustumPlanes(Camera *_camera);
		void			normalizePlane(Quaternion &_plane);

	private:
		enum			VertexType {VERTEXTYPE_PT,VERTEXTYPE_PC,VERTEXTYPE_PCT,VERTEXTYPE_PCBT,VERTEXTYPE_PNC3T,VERTEXTYPE_MAX};

        static void     setVertexDesc(const ITF_VertexBuffer* _vertexBuffer, bool _bIndex16 = true, bool _dupUV = bfalse);
		static void     drawIndexedPC(ITF_IndexBuffer* _indexBuffer, ITF_VertexBuffer* _vertexBuffer, u32 _indexNumber, bool _bIndex16 = true);
		static void     drawIndexedPT(ITF_IndexBuffer* _indexBuffer, ITF_VertexBuffer* _vertexBuffer, u32 _indexNumber, bool _bIndex16 = true, bool _dupUV = bfalse);
		static void     drawIndexedPCT(ITF_IndexBuffer* _indexBuffer, ITF_VertexBuffer* _vertexBuffer, u32 _indexNumber, bool _bIndex16 = true, bool _dupUV = bfalse);
               void     drawIndexedPNC3T(ITF_IndexBuffer* _indexBuffer, ITF_VertexBuffer* _vertexBuffer, u32 _indexNumber, bool _bIndex16 = true, bool _dupUV = bfalse);
		static void     drawIndexedPCBT(ITF_IndexBuffer* _indexBuffer, ITF_VertexBuffer* _vertexBuffer, u32 _indexNumber, bool _bIndex16 = true, bool _dupUV = bfalse);

        static void     drawPC_direct(ITF_VertexBuffer* _vertexBuffer, u32 _st, u32 _nb);
        static void     drawPT_direct(ITF_VertexBuffer* _vertexBuffer, u32 _st, u32 _nb);
        static void     drawPCT_direct(ITF_VertexBuffer* _vertexBuffer, u32 _st, u32 _nb);
        static void     drawPNC3T_direct(ITF_VertexBuffer* _vertexBuffer, u32 _st, u32 _nb);
        static void     drawPCBT_direct(ITF_VertexBuffer* _vertexBuffer, u32 _st, u32 _nb);

		static void     drawPC(ITF_VertexBuffer* _vertexBuffer, u32 _st, u32 _nb, bool _bIndex16 = true);
		static void     drawPT(ITF_VertexBuffer* _vertexBuffer, u32 _st, u32 _nb, bool _bIndex16 = true, bool _dupUV = bfalse);
		static void     drawPCT(ITF_VertexBuffer* _vertexBuffer, u32 _st, u32 _nb, bool _bIndex16 = true, bool _dupUV = bfalse);
		       void     drawPNC3T(ITF_VertexBuffer* _vertexBuffer, u32 _st, u32 _nb, bool _bIndex16 = true, bool _dupUV = bfalse);
		static void     drawPCBT(ITF_VertexBuffer* _vertexBuffer, u32 _st, u32 _nb, bool _bIndex16 = true, bool _dupUV = bfalse);
		        
		void			drawMeshElement(const ITF_Mesh& _mesh, ITF_VertexBuffer* _vertexBuffer, const ITF_MeshElement& _elem);
		
		void			drawVertexBuffer_internal(u32 _type, ITF_VertexBuffer* _vertexBuffer, u32 _vertexStart, u32 _vertexNumber);
        void			drawVertexBuffer_direct(u32 _type, ITF_VertexBuffer* _vertexBuffer, u32 _vertexStart, u32 _vertexNumber);
		void			drawIndexedVertexBuffer_internal(u32 _type, ITF_VertexBuffer* _vertexBuffer, ITF_IndexBuffer* _indexBuffer, u32 _indexNumber);		
		
        GFXDisplayList *FindOrCreateMeshElementDisplayList(ITF_VertexBuffer* _vertexBuffer, const ITF_MeshElement& _elem);
        void            CheckDeprecatedDL();
#ifndef ITF_DISABLE_DEBUGINFO
        void            drawDebugText();
#endif // ITF_DISABLE_DEBUGINFO
        
		// CS : rendering can be done only by one thread at a time
		ITF_THREAD_CRITICAL_SECTION		m_csRendering;

		void*			m_FifoBuffer;
		GXFifoObj*		m_FifoObj;

		GXRenderModeObj	*Rmode;
		GXRenderModeObj	Rmodeobj;
		GXBool			m_VFilterEnable;

		u32				allocatedFrameBufferSize;

        Vec2d           mv_StaticUtilScreenSize; // Game size (always the same even if the real util size is different, like during splashscreen in 4/3 mode)
        Vec2d           mv_RealScreenSize;       // Real current size
		f32             mf32_EFBScale[2];
		
		GMatrix44       mg_ObjViewMatrix;
		GMatrix44       mg_ObjMatrix;

		// Textures
        Texture*        m_Texture;
		GXTexWrapMode	m_TexWrapS;
		GXTexWrapMode	m_TexWrapT;
        ITF_MAP<Texture*,Texture*>   m_NormalTex;

		// RenderMode
		Tev_Type        m_CurrentTevType;
        RT_Pass         m_CurrentRTPass;
        u32             mu32_CurClearColor;

		// Z setting
		GXCompare		m_zFuncComp;
		bbool			m_zTest;
		bbool			m_zWrite;

        // Overlay
        GFX_Vector4     m_VecOverlay;
        
        // Sync
        bbool           WaitRenderQueueLen(u32 _len = 1);
        #ifndef ITF_FINAL
        f64             mf64_VBLSyncTime;
        f32             m_fPsNoVsync;
        #endif // ITF_FINAL

		// Philippe fake AA
		u32     FakeAAFlag;
       
        // AFX
        enum AFX_RT_ID
        {
            AFX_FB          = 0,
            AFX_RT,
            AFX_MAX_FULL_RT,

            AFX_PART_RT_1   = AFX_MAX_FULL_RT,
            AFX_PART_RT_2,

            AFX_MAX_RT,
            AFX_MAX_PART_RT = AFX_MAX_RT - AFX_MAX_FULL_RT
        };
        renderTarget    *mp_AFX_RT[AFX_MAX_RT];
        bbool           mb_AFXBfrLocked;
        bbool           mb_ClearOnCopy;
        AFX_RT_ID       m_AFXCurRT;

    private:
        // AFX
        void            AFTERFX_PrepareDrawOne(AFX_RT_ID _RT, f32 _W, f32 _H, GFX_RECT *_saveViewport);
        void            AFTERFX_EndDrawOne(AFX_RT_ID _RT, GFX_RECT *_saveViewport);

        // FakeAA
        #define FAKEAA_FLAG_ACTIVE      0x1
        #define FAKEAA_FLAG_PERSP       0x2
        #define FAKEAA_FLAG_DISABLE     0x4
        ITF_INLINE bbool    isFakeAAEnabled() const  { return (FakeAAFlag & ~FAKEAA_FLAG_PERSP) == FAKEAA_FLAG_ACTIVE; }
        ITF_INLINE void     addFakeAAFlag(u32 _flag) { FakeAAFlag |=  _flag; }
        ITF_INLINE void     subFakeAAFlag(u32 _flag) { FakeAAFlag &= ~_flag; }
        ITF_INLINE void     xorFakeAAFlag(u32 _flag) { FakeAAFlag ^=  _flag; }
        void                forceDisableFakeAA(bbool _disable);

    public:
        static OSFontHeader* FontData;
        void OnGPUFrameEnd();
};

#define GFX_ADAPTER_WII		(static_cast< GFXAdapter_WII* > (GFX_ADAPTER))

} // namespace ITF

#endif // ITF_WII

#endif // _ITF_WII_DRAWPRIM_H_
