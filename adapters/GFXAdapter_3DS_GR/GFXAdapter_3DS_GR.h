#ifndef _ITF_3DS_GR_DRAWPRIM_H_
#define _ITF_3DS_GR_DRAWPRIM_H_

#ifdef ITF_CTR
#include <nn/gx.h>
#include <nn/fnd.h>
#include <nn/math.h>
#include <nn/gr.h>
#include <nn/font.h>
#include <nn/ulcd.h>

#ifndef _ITF_3DS_MACROS_H_
#include "adapters/GFXAdapter_3DS_GR/Macros.h"
#endif // _ITF_3DS_MACROS_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif

#ifndef _ITF_RASTERSMANAGER_H_
#include "engine/rasters/rastersmanager.h"
#endif //_ITF_RASTERSMANAGER_H_

#ifndef _ITF_GFXADAPTER_SHADERMANAGER_H_
#include "engine/AdaptersInterfaces/GFXAdapter_ShaderManager.h"
#endif //_ITF_GFXADAPTER_SHADERMANAGER_H_


#ifndef _ITF_3DS_BUFFEREDCOMMANDLIST_H_
#include "adapters/GFXAdapter_3DS_GR/BufferedCommandList.h"
#endif //_ITF_3DS_BUFFEREDCOMMANDLIST_H_

#ifndef _ITF_3DS_GRAPHICSMEMORYALLOCATOR_H_
#include "adapters/GFXAdapter_3DS_GR/GraphicsMemoryAllocator.h"
#endif //_ITF_3DS_GRAPHICSMEMORYALLOCATOR_H_


namespace ITF
{
	enum ScreenKind
	{
		UPPER_SCREEN        = 0x1 << 0,
		LOWER_SCREEN        = 0x1 << 1,
		EXTENSION_SCREEN    = 0x1 << 2,
		BOTH_SCREENS        = UPPER_SCREEN | LOWER_SCREEN | EXTENSION_SCREEN
	};

	enum BufferTransferMode
	{
		TRANSFER_MODE_ANTIALIASE_NOT_USED,
		TRANSFER_MODE_ANTIALIASE_2x1,
		TRANSFER_MODE_ANTIALIASE_2x2
	};

	enum DisplayBufferFormat
	{
		DISPLAY_BUFFER_RGB8_OES,
		DISPLAY_BUFFER_RGBA4,
		DISPLAY_BUFFER_RGB5_A1,
		DISPLAY_BUFFER_RGB565
	};


	class DisplayBuffer
	{
	public:
		struct Description
		{
			ScreenKind                  screenKind; 
			DisplayBufferFormat         format;
			NN_PADDING2;

			s32                         width;
			s32                         height;

			BufferTransferMode          transferMode; 
			bool                        isTransferFlipX;
			NN_PADDING2;

			s32                         transferOffsetX;
			s32                         transferOffsetY;
			s32                         displayOffsetX;
			s32                         displayOffsetY;

			Description()
				: screenKind( UPPER_SCREEN )
				, format( DISPLAY_BUFFER_RGB8_OES )
				, width( NN_GX_DISPLAY0_WIDTH )
				, height( NN_GX_DISPLAY0_HEIGHT )
				, transferMode( TRANSFER_MODE_ANTIALIASE_NOT_USED )
				, isTransferFlipX( false )
				, transferOffsetX( 0 )
				, transferOffsetY( 0 )
				, displayOffsetX( 0 )
				, displayOffsetY( 0 )
			{}
		};

		class Builder
		{
		public:
			Builder &bufferDescription( const Description &desc_ )
			{
				m_Desc = desc_;
				return *this;
			}

			Builder &screenKind( ScreenKind kind )
			{
				m_Desc.screenKind = kind;

				return *this;
			}

			Builder &format( DisplayBufferFormat format )
			{
				m_Desc.format = format;

				return *this;
			}

			Builder &bufferSize( s32 width, s32 height )
			{
				m_Desc.width = width;
				m_Desc.height = height;

				return *this;
			}

			Builder &transferMode( BufferTransferMode mode )
			{
				m_Desc.transferMode = mode;

				return *this;
			}

			Builder &isTransferFlipX( bool isTransferFlipX )
			{
				m_Desc.isTransferFlipX = isTransferFlipX;

				return *this;
			}

			Builder &transferOffset( s32 x, s32 y )
			{
				m_Desc.transferOffsetX = x;
				m_Desc.transferOffsetY = y;

				return *this;
			}

			Builder &displayOffset( s32 x, s32 y )
			{
				m_Desc.displayOffsetX = x;
				m_Desc.displayOffsetY = y;

				return *this;
			}

			DisplayBuffer *Create();

		private:
			Description m_Desc;
		};

		void TransferBuffer( const nn::gr::CTR::FrameBuffer::ColorBuffer* pColorBuffer,
			const bool isAddSplitDrawCmdlist = true );
		void ActivateBuffer( void );

		const Description &GetDescription( void ) const
		{
			return m_Description;
		}

	private:
		DisplayBuffer( const Description &desc );

	protected:
		~DisplayBuffer();

	private:
		Description     m_Description;
		u32             m_CurrentDisplay;
		GLuint	        m_DisplayBuffers[2];
		GLenum          m_Display;
		GLenum          m_TransferMode;
		GLenum          m_Format;
	};


	class GFXAdapter_3DS_GR : public GFXAdapter
	{
	public:
		GFXAdapter_3DS_GR();
		~GFXAdapter_3DS_GR();

		float			m_x, m_y;
		void			init();
		void			reshape();
		void			project(const Vec3d& _in3d, Vec2d & _out2d);
		void			coord2DTo3D(const Vec2d& _in2d, float _zPlane, Vec2d& _out3d, bool noTranslation = btrue);
		void			compute3DTo2D(const Vec3d& _in, Vec3d& _out);
		void			compute2DTo3D(const Vec3d& _in, Vec3d& _out);
		void			drawCircle(float _x, float _y, float r, float _r=1, float _g=1, float _b=1, float _z = 0.f, u32 _segsCount = 32);
		void			drawBox(const Vec2d& _pos, f32 _angle, const Vec2d& _extent, f32 _r=1, f32 _g=1, f32 _b=1, float _z = 0.f, bbool _fill = bfalse);
		void			drawLine(const Vec2d& _p1, const Vec2d& _p2, float _r=1, float _g=1, float _b=1, float _size=1.f, float _z = 0.f, float _alpha = 1.f);
		void			draw2dLine(const Vec2d& _p1, const Vec2d& _p2, float _size=1.f, u32 _color = COLOR_WHITE);
		void			draw2dBox(const Vec2d& _p, f32 _width, f32 _height, u32 _color0 = COLOR_WHITE, u32 _color1 = COLOR_WHITE, u32 _color2 = COLOR_WHITE, u32 _color3 = COLOR_WHITE, i32 _rank = 0);
		void			drawTriangle(const Vec2d& _p1, const Vec2d& _p2, const Vec2d& _p3, float _r=1, float _g=1, float _b=1, float _a=1, float _z = 0.f);
		void			drawTriangle(const Vertex& _v1, const Vertex& _v2, const Vertex& _v3, const Texture* _tex, bbool _showWireFrame = 0);
		void			drawQuad2D(f32 _px, f32 _py, f32 _width, f32 _height, f32 _z, u32 _color = COLOR_WHITE, const Texture* _tex = NULL, const Vec2d* _uvStart = NULL,  const Vec2d* _uvEnd = NULL);
		void			draw3DQuad(GFX_3DQUAD* _t_Quad);
//CASA
        void            draw3DQuadWithoutTexture(GFX_3DQUAD* _t_Quad);
//CASA>
		bbool			loadTexture(Texture* _tex, const char* _path, u32 _alhpa = 0);
		void            loadTextureTGA(const void *_buffer, u32 _contentSize, unsigned _target, int _level, bool& _useAlpha, bool _is2D = true, u32 *_width=0, u32 *_height=0, unsigned _forceformat=0);
		bbool			loadTextureCooked(Texture* _t, const char* _path) {return bfalse;}
		bbool           loadFromFileInMemory    (Texture *_t,const void* _buffer, u32 _contentSize);
		void			drawScreenQuad(f32 _px, f32 _py, f32 _width, f32 _height, f32 _z, u32 _color, bbool _noTex = 0);

		void			setCamera(float _x, float _y, float _z, Camera* _cam);
		void			init2DRenderSubScreen();
		void			init2DRender();
		void			end2DRender();
		void			init2DGuiRender();
		void			setOrthoView(f32 _l, f32 _r, f32 _b, f32 _t);

		void			CalcCoord(const Vec2d& _in2d, float _zPlane, Vec2d& _out3d);

		void			clear(u32 _buffer, u32 _Col = 0);
		void			clear(u32 _buffer, float _r=0.f, float _g=0.f, float _b=0.f, float _a=0.f);
		void			setAlphaBlend(GFX_BLENDMODE _blend);
		void			setCullMode(GFX_CULLMODE _cull);
		void			depthMask(u32 _depthMask);
		void			depthFunc(u32 _func);
		void			depthTest(u32 _test);
		void			setScissorRect(GFX_RECT* _clipRect);
		void			SetTextureBind(int _Sampler, unsigned int _Bind, bbool _linearFiltering = btrue);
		void			cleanupTexture(Texture* _texture);

		void			DrawPrimitive(PRIMITIVETYPE _type, const void *_p_Vertex, u32 _NumberVertex);

        void			setMatrixTransform(MATRIXMODE _type, const GMatrix44 *_Matrix);
		void			setupViewport(GFX_RECT* _Viewport);
		void			unprojectPoint(const Vec2d& _in, float _viewDistance, const GMatrix44 *_MatrixModel, const GMatrix44 *_MatrixProj, const int *_View, Vec2d& _out);
		void			setLookAtMatrix(GMatrix44* _MatrixOut, double _midx, double _midy, float _viewDistance, float _aspect);
		void			getViewport(GFX_RECT *_RectOut);

		/// GfxMaterials.
		void		    setGfxMaterial(const GFX_MATERIAL& _gfxMat);
		void			setGfxMatDefault();
		void			setGfxMatRefraction(const GFX_MATERIAL& _gfxMat);

		/// Texture.
		void			generateTexture(Texture* _texture);
		void			cleanupBaseTexture(Texture* _texture);
		void			setTextureSize(Texture* _texture, const Size& _size);
		unsigned char*	grabTexture(Texture* _texture);
		u32				getMaxTextureSize();
		void			loadFromMemory(const void* _buffer, const Size& _texture_size, Texture* _texture, Texture::PixFormat _pixel_format,const u32 _buffer_size,bbool bCooked, u32 _mipmapCount);
		bbool			loadCookedFromFileInMemory(Texture *_t, const void* _buffer, u32 _contentSize);
		void			SetTexture(int _Sampler, Texture* _Texture, bbool _linearFiltering = btrue);
        void			SetTexture(int _Sampler, const Texture* _Texture, bbool _linearFiltering = btrue);
		void			lockTexture(Texture* _tex, LOCKED_TEXTURE* _lockTex, u32 _flag);
		void			unlockTexture(Texture* _tex);
		void			saveToMemory(void* _buffer, Texture* _Tex);
		float			getTexelOffset() {return 0;}

		void             setSamplerState( u32 _samp, GFX_SAMPLERSTATETYPE _type, i32 _value );

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

		bbool			isDeviceLost();
		bbool			resetDevice(bbool _forceReset);

		void			setVertexFormat(u32 _vformat);

		/// shaders.
		void			createShader(ITF_shader* _shader);
		void			releaseShader(ITF_shader* _shader);
		bbool			loadShader(ITF_shader* _shader);
		void			beginShader(ITF_shader* _shader);
		void			endShader(ITF_shader* _shader);
		
		void			selectTechnique(u32 tech);
		void			selectTechniqueFromVertexFormat(u32 vertexFormat);

		// Patch shader
		void			prepareShaderPatch(GMatrix44* _matrix, f32 _z, u32 & _hdiv, u32 & _vdiv, GFX_BLENDMODE _blendMode = GFX_BLEND_ALPHA);        
		void			shaderDrawPatch32(const Texture* texture, const Vec2d *PointsNS, const Color* _lighttab);
		void			endShaderPatch();
		void			shaderPatchFlush(u32 _hdiv, u32 _vdiv, u32 _pass);

        /// Patch drawing using geometry shader.
        bool            prepareGeomShaderPatch(GMatrix44* _matrix, AnimBoneRenderTable* _bones, f32 _z, u32 _hdiv, u32 _vdiv, bbool _mirror);
        bbool           drawAnimatedPatches(ITF_Mesh& _mesh);
        void            endGeomShaderPatch();

        // Patch bezier shader
        void        prepareShaderBezierPatch      (   GMatrix44* _matrix, f32 _z, u32 & _hdiv, u32 & _vdiv, GFX_BLENDMODE _blendMode = GFX_BLEND_ALPHA );    
        bbool       shaderDrawBezierPatch32       (   Texture *texture, Vec2d *PointsNS, Color* _lighttab, f32* _widths, f32* _alphas );
        void        shaderBezierPatchFlush        (   u32 _hdiv, u32 _vdiv, u32 _pass );

		// Fluid Shader.
		void			drawFluid(   GMatrix44* _matrix, Texture* _texture, const FixedArray<u32, GFXADAPTER_FLUID_COLORTAB_ENTRY_COUNT> &_colorTab, const Vec2d _heightTab[], const u32 _heightTabSize, const FixedArray<Vec2d, 4> &_uvTab, u32 mode, f32 _delta1, f32 _delta2, f32 _stepCount  );

		/// Trail 3d Shader.
		void        drawTrail3D             (   Texture* _texture, Color _color, const SafeArray<Vec3d> & points, f32 _alphaBegin, f32 _alphaEnd, f32 _fadeLength );

		/// VertexBuffer.
		void			createVertexBuffer( ITF_VertexBuffer* _vertexBuffer);
		void			releaseVertexBuffer(ITF_VertexBuffer* _vertexBuffer);
		void			setVertexBuffer(ITF_VertexBuffer* _vertexBuffer);
		void			LockVertexBuffer(ITF_VertexBuffer* _vertexBuffer, void** _data, u32 _offset, u32 _size, u32 _flag);
		void			UnlockVertexBuffer(ITF_VertexBuffer* _vertexBuffer);
        void            appendVertexBuffer(ITF_VertexBuffer* _vertexBuffer, void* _additionalVertData, u32 _additionalVertCount);

		/// IndexBuffer.
		void			createIndexBuffer(ITF_IndexBuffer* _indexBuffer);
		void			releaseIndexBuffer(ITF_IndexBuffer* _indexBuffer);
		void			setIndexBuffer(ITF_IndexBuffer* _indexBuffer);
        void			setIndexBuffer(ITF_IndexBuffer* _indexBuffer, u32 _startIdx, u32 _nIdx);
		void			LockIndexBuffer(ITF_IndexBuffer* _indexBuffer, void** _data);
		void			UnlockIndexBuffer(ITF_IndexBuffer* _indexBuffer);
        void			UnlockIndexBuffer(ITF_IndexBuffer* _indexBuffer, u32 size);

		/// Vertex/Index Buffer Draw.
		void			DrawVertexBuffer(u32 _type, ITF_VertexBuffer* _vertexBuffer, u32 _vertexStart, u32 _vertexNumber);
		void			DrawIndexedVertexBuffer(u32 _type, ITF_VertexBuffer* _vertexBuffer, ITF_IndexBuffer* _indexBuffer, u32 _indexNumber);
        void			DrawIndexedVertexBuffer(u32 _type, ITF_VertexBuffer* _vertexBuffer, ITF_IndexBuffer* _indexBuffer, u32 _vertexStart, u32 _indexNumber);
		void			drawMesh(ITF_Mesh& _mesh, bbool _showWireFrame);
		void            drawMeshPT(const ITF_Mesh& _mesh, bbool _showWireFrame);
		void			drawFlatMesh(const ITF_Mesh& _mesh);

		/// After FX.
		void			copyCurrentColorBuffer(u32 _rt);
		void			copyTexture(int *TexHan);
		void			validateTexture(int *TexHan,int Mode,int ScreenSize,int ScreenSize_V);
		void			AFTERFX_BigBlur(f32 _factor, u32 _MaxPass, bbool _clearRT, GFX_QUALITY _quality);
		void			AFTERFX_Glow(f32 _factor, f32 _glowfactor, f32 _addalpha);
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

		void			startImpostorRendering(renderTarget* _rt, f32 xmin, f32 ymin, f32 xmax, f32 ymax, bbool _mustClear);
		void			stopImpostorRendering(renderTarget* _rt);

		void			setFloatConst(f32 _fvalue,u32 index) {}

		/// Surfaces.
		f32				ReadSurfaceFloat(void* _surf, u32 _posX, u32 _posY);

		/// UV.
		void			setUVAnim(GFX_UVANIM& _uvanim);

		/// Render Context.
		void			renderContext_Set();
		void			renderContext_SetDefault();

        /// Global Color.
		void			setGlobalColor(const Color& _color);
        void            setFogColor(const Color& );

		const GMatrix44*		getViewMatrix();

		const String	getRenderName() {return "CTR";}
	public:
		ScreenKind m_targetDisplay;

		DisplayBuffer* m_currentDisplayBuffer;
		nn::gr::CTR::FrameBuffer* m_currentFrameBuffer;

		DisplayBuffer* m_displayBuffers0;
        DisplayBuffer* m_displayBuffers0Ext;
        nn::gr::CTR::FrameBuffer m_frameBuffer0Ext;

		DisplayBuffer* m_displayBuffers1;
		nn::gr::CTR::FrameBuffer m_frameBuffer1;

		BufferedCommandList m_commandList;
		bool   m_lcdDisplayEnabled;

        void TransferBuffer(const u32 targetDisplay, const bool isAddSplitDrawCmdlist = true);
        void SwapBuffers(const u32 targetDisplay);
        void WaitVsync(const u32 targetDisplay);

	public:
        enum VertexFormatIdx
        {
            VertexFormatIdx_P,
            VertexFormatIdx_PT,
            VertexFormatIdx_PC,
            VertexFormatIdx_PCT,
            VertexFormatIdx_PNCT,
            VertexFormatIdx_PNC3T,
            VertexFormatIdx_Patch,

            VertexFormatIdx_Count
        };
        
        VertexFormatIdx	getInternalVertexFormat(u32 _vformat);

		/// Matrix.
		nn::math::Matrix44   m_proj;
        nn::math::Matrix44   m_picaProj;
		nn::math::Matrix44   m_view;
		nn::math::Matrix44   m_world;
		nn::math::Matrix44   m_worldView;
		nn::math::Matrix44   m_worldViewProj;
		nn::math::Matrix44   m_UVmat;

		
		ITF_VertexBuffer		m_quadVB;
		ITF_IndexBuffer			m_quadIB;
        
        GFXAdapter_shaderManager            m_shaderManager;
        u8*                                 m_defaultShaderBinary;
        nn::gr::CTR::Shader                 m_defaultShader;

        u8*                                 m_patchShaderBinary;
        nn::gr::CTR::Shader                 m_patchShader;

        nn::gr::CTR::BindSymbolVSInput      m_attribPosition;
        nn::gr::CTR::BindSymbolVSInput      m_attribColor;
        nn::gr::CTR::BindSymbolVSInput      m_attribTexCoord0;
        nn::gr::CTR::BindSymbolVSInput      m_attribTexCoord1;
        nn::gr::CTR::BindSymbolVSInput      m_attribTexCoord2;
        nn::gr::CTR::BindSymbolVSInput      m_attribTexCoord3;
        nn::gr::CTR::BindSymbolVSInput      m_attribNormal;
        nn::gr::CTR::BindSymbolVSInput      m_attribBlendIndices;

        nn::gr::CTR::BindSymbolVSFloat      m_defaultUniformProjMtx;
        nn::gr::CTR::BindSymbolVSFloat      m_defaultUniformModelViewMtx;
        nn::gr::CTR::BindSymbolVSFloat      m_defaultUniformUVMtx;
        nn::gr::CTR::BindSymbolVSFloat      m_defaultUniformArray0;
        nn::gr::CTR::BindSymbolVSFloat      m_defaultUniformArray1;
        nn::gr::CTR::BindSymbolVSFloat      m_defaultUniformConst0;
        nn::gr::CTR::BindSymbolVSFloat      m_defaultUniformConst1;
        nn::gr::CTR::BindSymbolVSFloat      m_defaultUniformConst2;
        nn::gr::CTR::BindSymbolVSFloat      m_defaultUniformConst3;
       
        nn::gr::CTR::BindSymbolVSInput      m_attribPatchPos_PosNrm;
        nn::gr::CTR::BindSymbolVSInput      m_attribPatchUV_UVNrm;
        nn::gr::CTR::BindSymbolVSInput      m_attribPatchBoneIdx;

        nn::gr::CTR::BindSymbolGSFloat      m_patchUniformProjMtx;
        nn::gr::CTR::BindSymbolGSFloat      m_patchUniformModelViewMtx;
        nn::gr::CTR::BindSymbolGSFloat      m_patchUniformZ;
        nn::gr::CTR::BindSymbolVSFloat      m_patchUniformBonesP1;
        nn::gr::CTR::BindSymbolVSFloat      m_patchUniformBonesP2;
        nn::gr::CTR::BindSymbolVSFloat      m_patchUniformMirrorRatio;


        enum Shaders
        {
            VertexShader_Default = 0,
            GeomShader_Default = -1,

            VertexShader_Patch = 0,
            GeomShader_Patch = 1
        };
        bool                                m_geomShaderMode;
        f32                                 m_saveCurrentPatchMirror;
        void switchToDefautlShader();
        void switchToPatchShader();

        nn::gr::CTR::Vertex::InterleaveInfo m_vertexFormats[VertexFormatIdx_Count];

        nn::gr::CTR::FrameBuffer            m_frameBuffer0;
        nn::gr::CTR::Viewport               m_viewport;
        nn::gr::CTR::Scissor                m_scissor;

        nn::gr::CTR::Vertex*                m_currentBuffer;
        nn::gr::CTR::Vertex*                m_previousBuffer;
        i32                                 m_currentVertexFormat;
        i32                                 m_previousVertexFormat;

        nn::gr::CTR::Vertex::IndexStream    m_indexBuffer;

        nn::gr::CTR::RenderState            m_renderState;
        GFX_CULLMODE                        m_previousCullMode;
        u32                                 m_previousDepthTest;
        u32                                 m_previousDepthFunc;
        u32                                 m_previousDepthMask;
        bool                                m_isRenderStateDirty;

        nn::gr::CTR::Combiner               m_combiner;
        bool                                m_isCombinerStateDirty;
        bool                                m_isConstColor0Dirty;
        enum CombinerConfig
        {
            CombinerConfig_PCT,
            CombinerConfig_PT,
            CombinerConfig_PC,
            CombinerConfig_Font,

            CombinerConfig_Invalid
        };
        CombinerConfig                  m_previousCombinerConfig;

        nn::gr::CTR::Texture            m_texture;
        const Texture*                  m_currentTexture;
        GFX_TEXADRESSMODE               m_previousUaddressingMode;
        GFX_TEXADRESSMODE               m_previousVaddressingMode;
        bool                            m_isTextureDirty;

        u32*                            m_commandCursor;
        u32*                            m_startAddr;
        u32                             m_selectedTech;
        u32                             m_prevousTech;
        u32                             m_techBitSet;
        bool                            m_useFluid2;
        
        static const int MAX_INDICES = 2048;
        ITF_IndexBuffer                 m_defaultIndexBuffer;
        
        nn::font::ResFont               m_Font;
        nn::font::CTR::TextWriter       m_TextWriter;
        nn::font::CTR::RectDrawer       m_Drawer;
        s32                             m_MaxTextCount;    
        void*                           m_FontCommandBuffer;
        
        nn::gr::CTR::RenderState        m_fontRenderState;
        nn::gr::CTR::Combiner           m_fontCombiner;
	public:
        GMatrix44& ConvertMatrix(GMatrix44& out, nn::math::Matrix44 in)
        {
            ITF_Memcpy(out.fa16, &in, sizeof(nn::math::Matrix44));
            return out;
        }
        void extractFrustumPlanes(const nn::math::Matrix44 &_m,
            nn::math::Vector4 &_leftPlane,
            nn::math::Vector4 &_rightPlane,
            nn::math::Vector4 &_topPlane,
            nn::math::Vector4 &_bottomPlane,
            nn::math::Vector4 &_nearPlane);
        void updateCameraFrustumPlanes(Camera *_camera);
        void normalizePlane(nn::math::Vector4 &_plane);

        void createQuad();
        void destoryQuad();
        void createDefaultIndexBuffer();
        void destroyDefaultIndexBuffer();

        static const s32 DEFAULT_MAX_TEXT_COUNT              = 512;

        void InitDefaultShader();
        void InitPatchShader();
        void InitVertexFormats();
        void InitFrameBuffers(nn::gr::CTR::FrameBuffer* frameBuffer, u32 width, u32 height, ScreenKind screenKind);

        void SetBooleanRegister();
        void SetConst0ColorRegister();
        void SetWorldViewMatrix(const GMatrix44 *world);
        void UpdateGPUState();
        void InvalidateStateCache();

        /*****************************************************************************/
        bool InitializeFont( const char* fontShaderPath, const char* fontPath );
        void DestroyFont();

        void BeginDrawingString();
        void EndDrawingString();
        bool InitializeFont( void* shaderBinary, size_t shaderSize, void* fontBinary, size_t fontSize, void* drawBuffer, void* stringBuffer );

        bool InitializeFontResource( void* fontData, size_t fontSize );
        bool InitializeFontShader( void* shaderData, size_t shaderSize );    
        void SendFontMatrix();

        void InitializeGraphicsState();
        void SetCombinerConfigFromTech();
        void SetCombinerConfig(CombinerConfig config);
        
    public:
        f32 DrawString(const char* format, ...);
        f32 DrawString(s32 posh, s32 posv, const char* format, ...);
        f32 DrawStringArgs(s32 posh, s32 posv, const char* format, std::va_list args);    

        nn::font::CTR::TextWriter& GetWriter()
        { 
            return m_TextWriter; 
        }
        const nn::font::CTR::TextWriter& GetWriter() const
        {
            return m_TextWriter;
        } 

        static volatile s32	ms_VBlankCount;
        static void VBlankCallback(GLenum screen);

    private:
        static const u32 GARBAGLE_COLLECTOR_CYCLE = 3;
        SafeArray<void*, 64> m_gcTable[GARBAGLE_COLLECTOR_CYCLE];
        u32 gcCounter;

        void gcFree(void* addr);
        void gcCollect();

    private:
        struct MatrixToPatch
        {
            nn::math::Matrix44 matrix;
            u32 offset;
        };

        MatrixToPatch m_projectionPatchs[32];
        u32 m_projectionPatchsCount;

        MatrixToPatch m_modelViewPatchs[256];
        u32 m_modelViewPatchsCount;

        MatrixToPatch m_projectionPatchsGeo[32];
        u32 m_projectionPatchsGeoCount;

        MatrixToPatch m_modelViewPatchsGeo[256];
        u32 m_modelViewPatchsGeoCount;

        u32 m_lastLeftClearBuffBits;
        f32 lastLeftClearBuffR, lastLeftClearBuffG, lastLeftClearBuffB, lastLeftClearBuffA;

        nn::ulcd::CTR::StereoCamera m_stereoCamera;
        nn::math::Matrix44 m_projL;
        nn::math::Matrix44 m_viewL;
        nn::math::Matrix44 m_projR;
        nn::math::Matrix44 m_viewR;
        nn::math::Matrix44 m_worldViewL;
        nn::math::Matrix44 m_worldViewR;
        nn::math::Matrix44 m_firstProjMtxR;

        u32* m_startLeftRenderingAddr;
        u32* m_endLeftRenderingAddr;
        bool m_stereoMode;

        void InitStereoState();

        void UpdateGPUWorldView();
        void UpdateGPUProj();

        void UpdateGPUWorldViewGeo();
        void UpdateGPUProjGeo();

        void DrawRightEye();
        
        void SetStereoActive(bool active)
        {
            m_stereoActive = active;
        }
        bool m_stereoActive;
	};

} // namespace ITF

#endif //ITF_CTR

#endif // _ITF_3DS_GR_DRAWPRIM_H_
