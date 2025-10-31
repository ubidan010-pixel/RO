#include "precompiled_GFXAdapter_3DS_GR.h"
///////////////////////////////////////////////////////////////////////////////
///////////////////////       implementation of various shapes drawing
///////////////////////////////////////////////////////////////////////////////

#include <math.h>
#include <nn/math.h>
#define NN_PROF_DO_NOT_USE
#include <nn/prof.h>

#include "engine/scene/cell.h"
#include "engine/scene/world.h"
#include "engine/scene/worldManager.h"

#ifndef _ITF_3DS_DMPGL_DRAWPRIM_H_
#include "adapters/GFXAdapter_3DS_GR/GFXAdapter_3DS_GR.h"
#endif //_ITF_3DS_DMPGL_DRAWPRIM_H_

#ifndef _ITF_VEC2D_H_
#include "core/math/Vec2d.h"
#endif //_ITF_VEC2D_H_
#ifndef _ITF_TEXTURE_H_
#include "engine/display/Texture.h"
#endif //_ITF_TEXTURE_H_
#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_

#ifndef _ITF_FILE_H_
#include "core/file/File.h"
#endif //_ITF_FILE_H_

#ifndef _ITF_FILEMANAGER_ITF_H_
#include "adapters/FileManager_ITF/FileManager_ITF.h"
#endif //_ITF_FILEMANAGER_ITF_H_

#ifndef _ITF_GFXADAPTER_RENDERTARGET_H_
#include "engine/AdaptersInterfaces/GFXAdapter_RenderTarget.h"
#endif //_ITF_GFXADAPTER_RENDERTARGET_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_INPUTADAPTER_3DS_H_
#include "adapters/InputAdapter_3DS/InputAdapter_3DS.h"
#endif //_ITF_INPUTADAPTER_3DS_H_

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_RASTERSMANAGER_H_
#include "engine/rasters/RastersManager.h"
#endif //_ITF_RASTERSMANAGER_H_

#ifndef _ITF_ANIMATIONBONES_H_
#include    "engine/animation/AnimationBones.h"
#endif // _ITF_ANIMATIONBONES_H_


#define BUFFER_OFFSET(type,field) &(((type*)NULL)->field)

const char* FONT_SHADER_FILE_NAME     = "shaders/ctr/bin/fontrectdrawer.shbin";
const char* FONT_FILE_NAME            = "shaders/ctr/bin/tahoma.bcfnt";

namespace ITF
{

#define         ETCCOMPRESS_FORMAT_ETC1_RGB8_NATIVE					(0x1011)
#define         ETCCOMPRESS_FORMAT_ETC1_ALPHA_RGBA8_NATIVE			(0x1021)
#define         ETCCOMPRESS_FORMAT_RGB8                             (0x2010)
#define         ETCCOMPRESS_FORMAT_RGBA8                            (0x2020)


struct ETCHeader
{
	unsigned int m_width;
	unsigned int m_height;
	unsigned int m_format;
	unsigned int m_mipMapCount;
};

enum
{
	DEFAULT_TECH0 = 0,
	DEFAULT_TECH1,
	DEFAULT_TECH2,
	DEFAULT_TECH3,
	DEFAULT_TECH4,
	DEFAULT_TECH5,
	DEFAULT_TECH6,
	DEFAULT_TECH7,
	DEFAULT_TECH8,
	PATCH_TECH,
	DEFAULT_TECH_COUNT
};

//------------------------------------------------------------------------------
DisplayBuffer *DisplayBuffer::Builder::Create( void )
{
	return new DisplayBuffer( m_Desc );
}

//------------------------------------------------------------------------------
DisplayBuffer::DisplayBuffer( const Description &desc )
: m_Description( desc )
, m_CurrentDisplay( 0 )
, m_Display( NN_GX_DISPLAY0 )
, m_TransferMode( NN_GX_ANTIALIASE_NOT_USED )
, m_Format( GL_RGB8_OES )
{
	std::memset( m_DisplayBuffers, 0, sizeof( m_DisplayBuffers ) );
    GLenum bufferPlacement = NN_GX_MEM_FCRAM;
	if( m_Description.screenKind == UPPER_SCREEN )
	{
        bufferPlacement = NN_GX_MEM_VRAMB;
		m_Display = NN_GX_DISPLAY0;
	}
	else if( m_Description.screenKind == LOWER_SCREEN )
	{
        bufferPlacement = NN_GX_MEM_VRAMA;
		m_Display = NN_GX_DISPLAY1;
	}
	else if( m_Description.screenKind == EXTENSION_SCREEN )
	{
        bufferPlacement = NN_GX_MEM_VRAMB;
		m_Display = NN_GX_DISPLAY0_EXT;
	}
	else
	{
		NN_ASSERT( 0 );
	}

	if( m_Description.transferMode == TRANSFER_MODE_ANTIALIASE_NOT_USED )
	{
		m_TransferMode = NN_GX_ANTIALIASE_NOT_USED;
	}
	else if( m_Description.transferMode == TRANSFER_MODE_ANTIALIASE_2x1 )
	{
		m_TransferMode = NN_GX_ANTIALIASE_2x1;
	}
	else if( m_Description.transferMode == TRANSFER_MODE_ANTIALIASE_2x2 )
	{
		m_TransferMode = NN_GX_ANTIALIASE_2x2;
	}
	else
	{
		NN_ASSERT( 0 );
	}

	if ( m_Description.format == DISPLAY_BUFFER_RGB8_OES )
	{
		m_Format = GL_RGB8_OES;
	}
	else if ( m_Description.format == DISPLAY_BUFFER_RGBA4 )
	{
		m_Format = GL_RGBA4;
	}
	else if ( m_Description.format == DISPLAY_BUFFER_RGB5_A1 )
	{
		m_Format = GL_RGB5_A1;
	}
	else if ( m_Description.format == DISPLAY_BUFFER_RGB565 )
	{
		m_Format = GL_RGB565;
	}
	else
	{
		NN_ASSERT( 0 );
	}

	nngxActiveDisplay( m_Display );
	nngxGenDisplaybuffers( 2, m_DisplayBuffers );

	for( s32 i = 0; i < 2; i++ )
	{
		nngxBindDisplaybuffer( m_DisplayBuffers[i] );
		nngxDisplaybufferStorage( m_Format,
			m_Description.height, m_Description.width, bufferPlacement );
	}

	nngxDisplayEnv( m_Description.displayOffsetY, m_Description.displayOffsetX );
	nngxBindDisplaybuffer( 0 );
	m_CurrentDisplay = 0;
}

//------------------------------------------------------------------------------
void DisplayBuffer::TransferBuffer( const nn::gr::CTR::FrameBuffer::ColorBuffer* pColorBuffer,
								   const bool isAddSplitDrawCmdlist )
{
	if ( pColorBuffer == NULL )
	{
		return;
	}

	GLint displayBufferVirtualAddr = 0;
	nngxBindDisplaybuffer( m_DisplayBuffers[ m_CurrentDisplay ] );
	nngxGetDisplaybufferParameteri( NN_GX_DISPLAYBUFFER_ADDRESS, 
		&displayBufferVirtualAddr );
	GLenum colorBufferFormat = GL_RGBA8_OES;
	if ( pColorBuffer->format == PICA_DATA_COLOR_RGB5_A1 )
	{
		colorBufferFormat = GL_RGB5_A1;
	}
	else if ( pColorBuffer->format == PICA_DATA_COLOR_RGB565 )
	{
		colorBufferFormat = GL_RGB565;
	}
	else if ( pColorBuffer->format == PICA_DATA_COLOR_RGBA4 )
	{
		colorBufferFormat = GL_RGBA4;
	}

	GLsizei blockSize = 8;
	if ( pColorBuffer->blockSize == nn::gr::CTR::FrameBuffer::BLOCK_SIZE32 )
	{
		blockSize = 32;
	}

	if ( isAddSplitDrawCmdlist )
	{
		nngxSplitDrawCmdlist();
	}

	nngxAddB2LTransferCommand( reinterpret_cast<GLvoid*>( pColorBuffer->virtualAddr ),
		pColorBuffer->width, pColorBuffer->height, colorBufferFormat,
		reinterpret_cast<GLvoid*>( displayBufferVirtualAddr ),
		m_Description.height, m_Description.width,
		m_Format,
		m_TransferMode,
		m_Description.isTransferFlipX,
		blockSize );
}

//------------------------------------------------------------------------------
void DisplayBuffer::ActivateBuffer()
{
	nngxActiveDisplay( m_Display );
	nngxBindDisplaybuffer( m_DisplayBuffers[ m_CurrentDisplay ] );

	m_CurrentDisplay = 1 - m_CurrentDisplay;
}

//----------------------------------------------------------------------------//
GFXAdapter_3DS_GR::GFXAdapter_3DS_GR()
{
       setFix30FPS(true);
}

//----------------------------------------------------------------------------//

GFXAdapter_3DS_GR::~GFXAdapter_3DS_GR()
{
	m_commandList.Finalize();

	destoryQuad();
	nngxFinalize();
	GraphicsMemoryAllocator::Finalize();
}

volatile s32 GFXAdapter_3DS_GR::ms_VBlankCount = 0;

void GFXAdapter_3DS_GR::VBlankCallback(GLenum screen)
{
    if (screen == NN_GX_DISPLAY0)
    {
       ms_VBlankCount++;
    }
}

inline void GFXAdapter_3DS_GR::switchToDefautlShader()
{
    if (m_geomShaderMode)
    {
        m_commandCursor = m_defaultShader.MakeFullCommand(m_commandCursor);
        UpdateGPUProj();
        m_commandCursor = m_defaultUniformUVMtx.MakeUniformCommand(m_commandCursor, m_UVmat);
        m_geomShaderMode = false;
    }
}

inline void GFXAdapter_3DS_GR::switchToPatchShader()
{
    if (!m_geomShaderMode)
    {
        m_commandCursor = m_patchShader.MakeFullCommand(m_commandCursor);
        UpdateGPUProjGeo();
        m_geomShaderMode = true;
    }
}

void GFXAdapter_3DS_GR::setCamera(float _x, float _y, float _z, Camera* _cam)
{
	SetStereoActive(nn::gx::CTR::IsStereoVisionAllowed());

	GFXAdapter::setCamera(_x, _y, _z, _cam);

	f32 nearPlane = _cam->isBiased() ? 1.f : _cam->m_near;
	f32 farPlane = _cam->m_far;

	//_cam->m_near = nearPlane;
	_cam->forcePosition(Vec3d(_x,_y,_z));

	nn::math::Vector3 vEyePt( _cam->getX(), _cam->getY(), _z );
	nn::math::Vector3 vLookatPt(_cam->getX(), _cam->getY(), _z - 1.f);
	nn::math::Vector3 vUpVec( 0.0f, 1.0f, 0.0f );
    nn::math::Matrix44 projStereo;
	nn::math::MTX44PerspectiveRad(&m_proj, _cam->getFocale(), _cam->m_screen_ratio, nearPlane, farPlane);
	m_picaProj = m_proj;
    projStereo = m_proj;
	m_proj.Transpose();
    nn::math::MTX44Pivot(&m_picaProj, nn::math::PIVOT_UPSIDE_TO_TOP);
    
	nn::math::Matrix34 eye;
	nn::math::MTX34LookAt(&eye, &vEyePt, &vUpVec, &vLookatPt);
	m_view = nn::math::Matrix44(eye);
	m_view.Transpose();

	m_world.SetupIdentity();
	nn::math::MTX44Mult(&m_worldView, &m_world, &m_view);
	nn::math::MTX44Mult(&m_worldViewProj, &m_worldView, &m_proj);

    nn::math::Matrix34 eyeL, eyeR;
    m_stereoCamera.CalculateMatrices(&m_projL, &eyeL, &m_projR, &eyeR, &projStereo, &eye, 5.0f, 0.5f, false);
    m_viewL = nn::math::Matrix44(eyeL);
    m_viewL.Transpose();
    m_viewR = nn::math::Matrix44(eyeR);
    m_viewR.Transpose();

    nn::math::MTX44Mult(&m_worldViewL, &m_world, &m_viewL);
    nn::math::MTX44Mult(&m_worldViewR, &m_world, &m_viewR);


	_cam->m_lookAt.m_x = vLookatPt.x;
	_cam->m_lookAt.m_y = vLookatPt.y;
	_cam->m_lookAt.m_z = vLookatPt.z;

	_cam->m_resultingBiasedPosition.m_x = vEyePt.x;
	_cam->m_resultingBiasedPosition.m_y = vEyePt.y;
	_cam->m_resultingBiasedPosition.m_z = vEyePt.z;

	//update parameters
	updateCameraFrustumPlanes(_cam);
}

void GFXAdapter_3DS_GR::updateCameraFrustumPlanes(Camera *_cam)
{
	nn::math::Vector4 leftPlane, rightPlane, topPlane, bottomPlane, nearPlane;
	extractFrustumPlanes(m_worldViewProj, leftPlane, rightPlane, topPlane, bottomPlane, nearPlane);

	Plane &left = _cam->m_frustumPlanes[Camera::CameraPlane_Left];
	Plane &right = _cam->m_frustumPlanes[Camera::CameraPlane_Right];
	Plane &bottom = _cam->m_frustumPlanes[Camera::CameraPlane_Bottom];
	Plane &top = _cam->m_frustumPlanes[Camera::CameraPlane_Top];
	Plane &nearP = _cam->m_frustumPlanes[Camera::CameraPlane_Near];

	left.m_normal = Vec3d(leftPlane.x, leftPlane.y, leftPlane.z);
	left.m_constant = -leftPlane.w;

	right.m_normal = Vec3d(rightPlane.x, rightPlane.y, rightPlane.z);
	right.m_constant = -rightPlane.w;

	top.m_normal = Vec3d(topPlane.x, topPlane.y, topPlane.z);
	top.m_constant = -topPlane.w;
	
	bottom.m_normal = Vec3d(bottomPlane.x, bottomPlane.y, bottomPlane.z);
	bottom.m_constant = -bottomPlane.w;

	nearP.m_normal = Vec3d(nearPlane.x, nearPlane.y, nearPlane.z);
	const Vec3d& pos = _cam->getCorrectedPos();
	nearP.m_constant = nearPlane.x * pos.m_x + nearPlane.y * pos.m_y + nearPlane.z * pos.m_z;
}

void GFXAdapter_3DS_GR::extractFrustumPlanes(const nn::math::Matrix44 &_m,
nn::math::Vector4 &_leftPlane,
nn::math::Vector4 &_rightPlane,
nn::math::Vector4 &_topPlane,
nn::math::Vector4 &_bottomPlane,
nn::math::Vector4 &_nearPlane)
{
	nn::math::Vector4 xColumn(_m.f._00, _m.f._10, _m.f._20, _m.f._30);
	nn::math::Vector4 yColumn(_m.f._01, _m.f._11, _m.f._21, _m.f._31);
	nn::math::Vector4 zColumn(_m.f._02, _m.f._12, _m.f._22, _m.f._32);
	nn::math::Vector4 wColumn(_m.f._03, _m.f._13, _m.f._23, _m.f._33);

	_leftPlane = -wColumn-xColumn;
	_rightPlane = xColumn-wColumn;
	_bottomPlane = -wColumn-yColumn;
	_topPlane = yColumn-wColumn;
	_nearPlane = -zColumn;

	normalizePlane(_leftPlane);
	normalizePlane(_rightPlane);
	normalizePlane(_bottomPlane);
	normalizePlane(_topPlane);
	normalizePlane(_nearPlane);
}

void GFXAdapter_3DS_GR::normalizePlane(nn::math::Vector4 &_plane)
{
    //_plane.Normalize();
	nn::math::PLANE &plane = (nn::math::PLANE&)_plane;
	plane.Normalize();
}
//----------------------------------------------------------------------------//

void GFXAdapter_3DS_GR::init2DRender(   )
{
	/// Set 2D Matrix.
	setOrthoView(0.0f, (f32) getScreenWidth(), 0.0f, (f32) getScreenHeight());
}


void GFXAdapter_3DS_GR::init2DRenderSubScreen(   )
{
    //drawQuad2D(63.0f, 195.0f, 6.0, 6.0, 1.0f, 0xFF00FFFF, NULL, NULL, NULL);

    //drawQuad2D(10.0f, 10.0f, 100.0, 100.0, 1.0f, 0xFFFF00FF, NULL, NULL, NULL);


    if (m_stereoActive)
    {
        DrawRightEye();
    }
    InitStereoState();
    setResolution(NN_GX_DISPLAY1_HEIGHT, NN_GX_DISPLAY1_WIDTH);
	/// Set 2D Matrix.
	setOrthoView(0.0f, (f32) getScreenWidth(), 0.0f, (f32) getScreenHeight());

    m_viewport.Set( 0, 0, NN_GX_DISPLAY1_WIDTH, NN_GX_DISPLAY1_HEIGHT );
    m_scissor.Set( 0, 0, NN_GX_DISPLAY1_WIDTH, NN_GX_DISPLAY1_HEIGHT );

    m_currentFrameBuffer = &m_frameBuffer1;
    clear(GFX_CLEAR_ZBUFFER | GFX_CLEAR_COLOR, 0.0f, 0.0f, 0.0f, 1.0f);

    //drawQuad2D(63.0f, 195.0f, 6.0, 6.0, 1.0f, 0xFF00FFFF, NULL, NULL, NULL);

    //drawQuad2D(10.0f, 10.0f, 100.0, 100.0, 1.0f, 0xFFFF00FF, NULL, NULL, NULL);
}

void GFXAdapter_3DS_GR::end2DRender()
{
    setResolution(NN_GX_DISPLAY0_HEIGHT, NN_GX_DISPLAY0_WIDTH);
}

void GFXAdapter_3DS_GR::init2DGuiRender()
{
	GFX_NOT_IMPLEMENTED(init2DGuiRender);
}

void GFXAdapter_3DS_GR::setOrthoView(f32 _l, f32 _r, f32 _b, f32 _t)
{
	m_world.SetupIdentity();
	m_view.SetupIdentity();
	
	nn::math::MTX44Ortho(&m_proj, _l, _r, _b, _t, -1.0f, 1.0f);
	m_picaProj = m_proj;
    m_proj.Transpose();
	nn::math::MTX44Pivot(&m_picaProj, nn::math::PIVOT_UPSIDE_TO_TOP);
	
    if (m_geomShaderMode)
    {
        UpdateGPUProjGeo();
    }
    else
    {
        UpdateGPUProj();
    }

    if (m_stereoMode)
    {
        m_viewL = m_viewR = m_view;
        m_projL = m_projR = m_picaProj;
    }

	nn::math::MTX44Mult(&m_worldView, &m_world, &m_view);
	nn::math::MTX44Mult(&m_worldViewProj, &m_worldView, &m_proj);

	updateCameraFrustumPlanes(CAMERA);
}

//----------------------------------------------------------------------------//
void GFXAdapter_3DS_GR::createQuad()
{
	m_quadVB.createVertexBuffer(4, VertexFormat_PT, sizeof(VertexPT), bfalse);
	VertexPT* vertexData;
	m_quadVB.Lock((void**)&vertexData);
	(vertexData++)->setData(Vec3d(0.0, 0.0, 0.0), Vec2d(0.0, 0.0));
	(vertexData++)->setData(Vec3d(1.0, 0.0, 0.0), Vec2d(0.0, 0.0));
	(vertexData++)->setData(Vec3d(2.0, 0.0, 0.0), Vec2d(0.0, 0.0));
	(vertexData++)->setData(Vec3d(3.0, 0.0, 0.0), Vec2d(0.0, 0.0));
	m_quadVB.Unlock();

	m_quadIB.createIndexBuffer(6, bfalse);
	u16* indexData;
	m_quadIB.Lock((void**)&indexData);
	*(indexData++) = 0;
	*(indexData++) = 1;
	*(indexData++) = 2;
	*(indexData++) = 0;
	*(indexData++) = 2;
	*(indexData++) = 3;
	m_quadIB.Unlock();
}

//----------------------------------------------------------------------------//
void GFXAdapter_3DS_GR::destoryQuad()
{
	m_quadIB.release();
	m_quadVB.release();
}

//----------------------------------------------------------------------------//
void GFXAdapter_3DS_GR::InitFrameBuffers(nn::gr::CTR::FrameBuffer* frameBuffer, u32 width, u32 height, ScreenKind screenKind)
{
    PicaDataColor colorBufferFormat = PICA_DATA_COLOR_RGBA8_OES;

    u32 colorBufferByteSize = width * height * 4;

    if ( colorBufferFormat != PICA_DATA_COLOR_RGBA8_OES )
    {
        colorBufferByteSize = width * height * 2;
    }

    PicaDataDepth depthBufferFormat = PICA_DATA_DEPTH24_STENCIL8_EXT;
    u32 depthBufferByteSize = width * height * 4;
    if ( depthBufferFormat == PICA_DATA_DEPTH_COMPONENT16 )
    {
        depthBufferByteSize = width * height * 2;
    }
    else if ( depthBufferFormat == PICA_DATA_DEPTH_COMPONENT24_OES )
    {
        depthBufferByteSize = width * height * 3;
    }
   
    frameBuffer->width                           = width;
    frameBuffer->height                          = height;
    if (screenKind == UPPER_SCREEN || screenKind == EXTENSION_SCREEN)
    {
        frameBuffer->colorBuffer.virtualAddr         = (uptr)GFXMEM::VramAAlloc(colorBufferByteSize, 64);
    }
    else
    {
        frameBuffer->colorBuffer.virtualAddr         = (uptr)GFXMEM::VramBAlloc(colorBufferByteSize, 64);
    }
    frameBuffer->colorBuffer.format              = colorBufferFormat;
    frameBuffer->colorBuffer.blockSize           = nn::gr::FrameBuffer::BLOCK_SIZE8;
    frameBuffer->colorBuffer.width               = width;
    frameBuffer->colorBuffer.height              = height;  
    frameBuffer->colorBuffer.clearColor[ 0 ]     = 0.0f;
    frameBuffer->colorBuffer.clearColor[ 1 ]     = 0.0f;
    frameBuffer->colorBuffer.clearColor[ 2 ]     = 0.0f;
    frameBuffer->colorBuffer.clearColor[ 3 ]     = 1.0f;

    if (screenKind == UPPER_SCREEN || screenKind == EXTENSION_SCREEN)
    {
         frameBuffer->depthStencilBuffer.virtualAddr  = (uptr)GFXMEM::VramBAlloc(depthBufferByteSize, 64);
    }
    else
    {
        frameBuffer->depthStencilBuffer.virtualAddr  = (uptr)GFXMEM::VramAAlloc(depthBufferByteSize, 64);
    }
    frameBuffer->depthStencilBuffer.format       = depthBufferFormat;
    frameBuffer->depthStencilBuffer.width        = width;
    frameBuffer->depthStencilBuffer.height       = height;
    frameBuffer->depthStencilBuffer.clearDepth   = 1.0f;
    frameBuffer->depthStencilBuffer.clearStencil = 0;
}

void GFXAdapter_3DS_GR::createDefaultIndexBuffer()
{
    m_defaultIndexBuffer.createIndexBuffer(MAX_INDICES, bfalse);
    u16* defaultIndices;
    m_defaultIndexBuffer.Lock((void**)&defaultIndices);

    for (u32 i = 0; i < MAX_INDICES; i++)
    {
        defaultIndices[i] = i;
    }

    m_defaultIndexBuffer.Unlock();
}

void GFXAdapter_3DS_GR::destroyDefaultIndexBuffer()
{
    m_defaultIndexBuffer.release();
}

//----------------------------------------------------------------------------//
void GFXAdapter_3DS_GR::init()
{
    gcCounter = 0;
	InputAdapter_3DS::mayInitAPI(); //We need it for the 3D level wheel

	GraphicsMemoryAllocator::Initialise();
	if ( nngxInitialize(GraphicsMemoryAllocator::Allocate, GraphicsMemoryAllocator::Deallocate) == GL_FALSE )
	{
		NN_TPANIC_("nngxInitialize() failed.\n");
	}

	nngxSetDisplayMode(NN_GX_DISPLAYMODE_NORMAL);

	const String shadersFolder("ctr/bin/");

	//WARNING : Shader loading must be in same order as in VertexType enum
	InitDefaultShader();
    InitPatchShader();
    InitVertexFormats();
	m_commandList.Initialize();
	m_techBitSet = 0;

    m_displayBuffers0 = DisplayBuffer::Builder().screenKind(UPPER_SCREEN).bufferSize(400, 240).format(DISPLAY_BUFFER_RGB8_OES).Create();
    m_displayBuffers0Ext = DisplayBuffer::Builder().screenKind(EXTENSION_SCREEN).bufferSize(400, 240).format(DISPLAY_BUFFER_RGB8_OES).Create();
    m_displayBuffers1 = DisplayBuffer::Builder().screenKind(LOWER_SCREEN).bufferSize(320, 240).format(DISPLAY_BUFFER_RGB8_OES).Create();

    InitFrameBuffers(&m_frameBuffer0, NN_GX_DISPLAY0_WIDTH, NN_GX_DISPLAY0_HEIGHT, UPPER_SCREEN);
    InitFrameBuffers(&m_frameBuffer0Ext, NN_GX_DISPLAY0_WIDTH, NN_GX_DISPLAY0_HEIGHT, EXTENSION_SCREEN);
    InitFrameBuffers(&m_frameBuffer1, NN_GX_DISPLAY1_WIDTH, NN_GX_DISPLAY1_HEIGHT, LOWER_SCREEN);

    m_stereoCamera.Initialize();
    nngxSetDisplayMode(NN_GX_DISPLAYMODE_STEREO);
    InitStereoState();
    m_stereoActive = true;
	
    /*
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	SetRenderTarget(NN_GX_DISPLAY0);
	Clear();

	SetRenderTarget(NN_GX_DISPLAY1);
	Clear();
	SwapBuffers();
	SetRenderTarget(NN_GX_DISPLAY0);
	Clear();

	SetRenderTarget(NN_GX_DISPLAY1);
	Clear();
	SwapBuffers();

	WaitVsync(NN_GX_DISPLAY_BOTH);
    */

	if (!m_lcdDisplayEnabled)
	{
		nngxStartLcdDisplay();
		m_lcdDisplayEnabled = true;
	}

	if (getScreenWidth() == 0 || getScreenHeight() == 0)
		LOG("GFXAdapter init error : resolution = 0, call setResolution(width,height) before init");

	for (u32 i = 0; i < 10 ; i++)
		FBTexture[i] = 0;

	setCamera(0.f, 0.f, 1.f, CAMERA);
	createInternalBuffers();
    createDefaultIndexBuffer();
	createQuad();
    
    m_scissor.SetBufferSize( nn::gx::DISPLAY0_WIDTH, nn::gx::DISPLAY0_HEIGHT );

    {
        u32* command = NULL;
        nngxGetCmdlistParameteri( NN_GX_CMDLIST_CURRENT_BUFADDR, 
            reinterpret_cast<GLint*>( &command ) );
        const u32* start_addr = command;

        command = nn::gr::CTR::MakeDisableAllCommand( command );

        nngxMoveCommandbufferPointer( ( command - start_addr ) * sizeof(u32) );
    }
	
    InvalidateStateCache();
    renderContext_SetDefault();
    m_MaxTextCount = DEFAULT_MAX_TEXT_COUNT;
    bool isSuccess = InitializeFont( FONT_SHADER_FILE_NAME, FONT_FILE_NAME );
    NN_ASSERTMSG( isSuccess , "Fail to initialize font .");

    nngxSetVSyncCallback(NN_GX_DISPLAY0, &GFXAdapter_3DS_GR::VBlankCallback);
}

//----------------------------------------------------------------------------//

void GFXAdapter_3DS_GR::reshape()
{
    GFX_NOT_IMPLEMENTED(reshape);
}

//----------------------------------------------------------------------------//

void GFXAdapter_3DS_GR::CalcCoord(const Vec2d& _in2d, float _zPlane, Vec2d& _out3d)
{
	GFX_NOT_IMPLEMENTED(CalcCoord);
}

//----------------------------------------------------------------------------//


void GFXAdapter_3DS_GR::coord2DTo3D( const Vec2d& _in2d, float _zPlane, Vec2d& _out3d, bool noTranslation )
{
    nn::math::Matrix44 view = m_view;
    if (noTranslation)
    {
        view.f._30 = 0.0f;
        view.f._31 = 0.0f;
        view.f._32 = 0.0f;
    }


    nn::math::Matrix44 m;
    nn::math::MTX44Mult(&m, m_world, view);
    nn::math::MTX44Mult(&m, m, m_proj);

    nn::math::Matrix44 invm;
    nn::math::MTX44Inverse(&invm, m);

    nn::math::Matrix44 transposed;
    nn::math::MTX44Transpose(&transposed, invm);

    GFX_Viewport _viewp;
    _viewp.m_x = 0;
    _viewp.m_y = 0;
    _viewp.m_width = getScreenWidth();
    _viewp.m_height = getScreenHeight();
    _viewp.m_minZ = 0.f;
    _viewp.m_maxZ = 1.f;

    nn::math::Vector4 vin(  -1.0f + (_in2d.m_x - _viewp.m_x) * 2.f / _viewp.m_width,
                            1.0f - (_in2d.m_y - _viewp.m_y) * 2.f / _viewp.m_height,
                            (_zPlane - _viewp.m_minZ) / (_viewp.m_maxZ - _viewp.m_minZ),
                            1.0f    );

    nn::math::Vector4 vout;
    nn::math::VEC4Transform(&vout, &transposed, &vin);

    const f32 invbw = (vout.w == 0.f) ? 0.f : 1.f/vout.w;
    _out3d.m_x = vout.x * invbw;
    _out3d.m_y = vout.y * invbw;
}

//----------------------------------------------------------------------------//

void GFXAdapter_3DS_GR::compute3DTo2D(const Vec3d& _in, Vec3d& _out)
{
    nn::math::Matrix44 viewProj;
    nn::math::MTX44Mult(&viewProj, &m_view, &m_proj);

    GMatrix44 viewProjG;
    ConvertMatrix(viewProjG, viewProj);

    GFX_Viewport viewp;
    viewp.m_x = 0;
    viewp.m_y = 0;
    viewp.m_width = getScreenWidth();
    viewp.m_height = getScreenHeight();
    viewp.m_minZ = 0.f;
    viewp.m_maxZ = 1.f;

    vec3dProject(&_out, const_cast<Vec3d*>(&_in), &viewp, &viewProjG);
}

//----------------------------------------------------------------------------//

void GFXAdapter_3DS_GR::compute2DTo3D(const Vec3d& _in, Vec3d& _out)
{
    nn::math::Matrix44 viewProj;
    nn::math::MTX44Mult(&viewProj, &m_view, &m_proj);

    nn::math::Matrix44 viewProjInv;
    nn::math::MTX44Inverse(&viewProjInv, viewProj);

    GMatrix44 viewProjInvG;
    ConvertMatrix(viewProjInvG, viewProjInv);

    GFX_Viewport viewp;
    viewp.m_x = 0;
    viewp.m_y = 0;
    viewp.m_width = getScreenWidth();
    viewp.m_height = getScreenHeight();
    viewp.m_minZ = 0.f;
    viewp.m_maxZ = 1.f;

    vec3dUnProject(&_out, const_cast<Vec3d*>(&_in), &viewp, &viewProjInvG);
}

//----------------------------------------------------------------------------//

void GFXAdapter_3DS_GR::drawScreenQuad( f32 _px, f32 _py, f32 _width, f32 _height, f32 _z, u32 _color, bbool _noTex )
{
	GFX_NOT_IMPLEMENTED(drawScreenQuad);
}

//----------------------------------------------------------------------------//

void GFXAdapter_3DS_GR::drawQuad2D( f32 _px, f32 _py, f32 _width, f32 _height, f32 _z, u32 _color, const Texture* _tex, const Vec2d* _uvStart,  const Vec2d* _uvEnd )
{
    selectTechnique(DEFAULT_TECH8);

    Vec2d uvStart, uvEnd;
    if (_uvStart)
        uvStart = *_uvStart;
    else
        uvStart = Vec2d(0.f, 0.f);
    if (_uvEnd)
        uvEnd = *_uvEnd;
    else
        uvEnd = Vec2d(1.f, 1.f);

    Color prevColor = getGFXGlobalColor();
    setGlobalColor(_color);

    if (_tex)
    {
        SetCombinerConfig(CombinerConfig_PCT);

        SetTexture(0, _tex);
        setTextureAdressingMode(0, GFXTADDRESS_WRAP, GFXTADDRESS_WRAP);
    }
    else
    {
        SetCombinerConfig(CombinerConfig_PC);
    }

    nn::math::Vector4 data[4];

    if (!m_stereoMode)
    {
        m_worldView = m_view;
        m_worldView.Transpose();
    }
    else
    {
        m_worldViewR = m_viewR;
        m_worldViewR.Transpose();

        m_worldViewL = m_viewL;
        m_worldViewL.Transpose();
    }

    //pos and uvs
    const f32 py = (f32)getScreenHeight() - _py - _height;
    data[0].Set(_px,            py,             uvStart.m_x,    uvEnd.m_y);
    data[1].Set(_px,            py + _height,   uvStart.m_x,    uvStart.m_y);
    data[2].Set(_px + _width,   py + _height,   uvEnd.m_x,      uvStart.m_y);
    data[3].Set(_px + _width,   py,             uvEnd.m_x,      uvEnd.m_y);

    m_commandCursor = m_defaultUniformArray0.MakeUniformCommand(m_commandCursor, data, 4);
    m_commandCursor = m_defaultUniformConst0.MakeUniformCommand(m_commandCursor, nn::math::Vector4(_z, 0, 0, 0));

    DrawIndexedVertexBuffer(GFX_TRIANGLES, &m_quadVB, &m_quadIB, 0, 6);
    setGlobalColor(prevColor);
}

//----------------------------------------------------------------------------//

void GFXAdapter_3DS_GR::draw3DQuad( GFX_3DQUAD* _t_Quad )
{
    /*selectTechnique(DEFAULT_TECH8);

    VertexPCT quad[4];

    float sx = _t_Quad->m_size.m_x * 0.5f;
    float sy = _t_Quad->m_size.m_y * 0.5f;

    quad[0].setData( Vec3d( -sx, -sy, 0.f ), Vec2d( 0.0f, 1.0f ), _t_Quad->m_color);
    quad[1].setData( Vec3d( sx, -sy, 0.f ), Vec2d( 1.0f, 1.0f ), _t_Quad->m_color);
    quad[3].setData( Vec3d( sx, sy, 0.f ), Vec2d( 1.0f, 0.0f ), _t_Quad->m_color);
    quad[2].setData( Vec3d( -sx, sy, 0.f ), Vec2d( 0.0f, 0.0f ), _t_Quad->m_color);

    D3DXMATRIX World;//, rot;
    D3DXMatrixTranslation(&World, _t_Quad->m_pos.m_x,_t_Quad->m_pos.m_y, _t_Quad->m_pos.m_z);

    Matrix44 oRot;

    if (!_t_Quad->m_rotation.IsEqual(Vec3d::Zero,MTH_EPSILON))
    {
        //reorient dir
        Matrix44 rotZ;
        Matrix44 rotX;
        Matrix44 rotY;

        M44_setMatrixRotationX(&rotX, _t_Quad->m_rotation.m_x);
        M44_setMatrixRotationY(&rotY, _t_Quad->m_rotation.m_y);
        M44_setMatrixRotationZ(&rotZ, _t_Quad->m_rotation.m_z);
        oRot = rotX * rotY * rotZ;
        D3DXMatrixMultiply(&World, (D3DXMATRIX*)&oRot, &World);
    }

    setObjectMatrix(World);

    setVertexFormat(VertexFormat_PCT);

    if (getOverDrawMode() > 1)
        setShader( mp_shaderManager.getShaderByIndex(4));
    else
    {
        if (isUseFog())
            setShader( mp_shaderManager.getShaderByIndex(5));
        else
            setShader(mp_defaultShader);
    }

    setAlphaBlend(_t_Quad->m_blendMode);
    DrawPrimitive(GFX_TRIANGLE_STRIP, (void*)quad, 4);*/
}
//CASA
//----------------------------------------------------------------------------//
void GFXAdapter_3DS_GR::draw3DQuadWithoutTexture( GFX_3DQUAD* _t_Quad )
{
    /*selectTechnique(DEFAULT_TECH8);

    VertexPC quad[4];

    float sx = _t_Quad->m_size.m_x * 0.5f;
    float sy = _t_Quad->m_size.m_y * 0.5f;

    quad[0].setData( Vec3d( -sx, -sy, 0.f ), _t_Quad->m_color);
    quad[1].setData( Vec3d( sx, -sy, 0.f ), _t_Quad->m_color);
    quad[3].setData( Vec3d( sx, sy, 0.f ), _t_Quad->m_color);
    quad[2].setData( Vec3d( -sx, sy, 0.f ), _t_Quad->m_color);

    D3DXMATRIX World;//, rot;
    D3DXMatrixTranslation(&World, _t_Quad->m_pos.m_x,_t_Quad->m_pos.m_y, _t_Quad->m_pos.m_z);

    Matrix44 oRot;

    if (!_t_Quad->m_rotation.IsEqual(Vec3d::Zero,MTH_EPSILON))
    {
        //reorient dir
        Matrix44 rotZ;
        Matrix44 rotX;
        Matrix44 rotY;

        M44_setMatrixRotationX(&rotX, _t_Quad->m_rotation.m_x);
        M44_setMatrixRotationY(&rotY, _t_Quad->m_rotation.m_y);
        M44_setMatrixRotationZ(&rotZ, _t_Quad->m_rotation.m_z);
        oRot = rotX * rotY * rotZ;
        D3DXMatrixMultiply(&World, (D3DXMATRIX*)&oRot, &World);
    }

    setObjectMatrix(World);

    setVertexFormat(VertexFormat_PC);

    if (getOverDrawMode() > 1)
        setShader( mp_shaderManager.getShaderByIndex(4));
    else
    {
        if (isUseFog())
            setShader( mp_shaderManager.getShaderByIndex(5));
        else
            setShader(mp_defaultShader);
    }

    setAlphaBlend(_t_Quad->m_blendMode);
    DrawPrimitive(GFX_TRIANGLE_STRIP, (void*)quad, 4);*/
}
//CASA>

//----------------------------------------------------------------------------//
bbool  GFXAdapter_3DS_GR::loadFromFileInMemory(Texture *_t, const void* _buffer, u32 _contentSize)
{
	// No need to check if texture is cooked assume it's true!
	//GFX_ASSERT(_t->isCooked(), "Texture not cooked\n");
	
	loadCookedFromFileInMemory(_t, _buffer, _contentSize);
	RESOURCE_MANAGER->addResourceAsLoaded(_t);

	return btrue;
}


bbool  GFXAdapter_3DS_GR::loadCookedFromFileInMemory(Texture *_t, const void* _buffer, u32 _contentSize)
{
	ETCHeader *header = (ETCHeader*)_buffer;
	Texture::PixFormat pixFormat;
	switch(header->m_format)
	{
	case ETCCOMPRESS_FORMAT_ETC1_RGB8_NATIVE :
		pixFormat = Texture::PF_ETC1_CTR;
		break;
	case ETCCOMPRESS_FORMAT_ETC1_ALPHA_RGBA8_NATIVE :
		pixFormat = Texture::PF_ETC1_A4_CTR;
		break;
	case ETCCOMPRESS_FORMAT_RGB8 :
		pixFormat = Texture::PF_RGB_NATIVE_CTR;
		break;
	case ETCCOMPRESS_FORMAT_RGBA8 :
		pixFormat = Texture::PF_RGBA_NATIVE_CTR;
		break;
	default:
		pixFormat = Texture::PF_RGBA;
	}

	Size size((float)header->m_width, (float)header->m_height);
	GFX_ADAPTER->setTextureSize(_t, size);

    _t->m_mipMapLevels = header->m_mipMapCount;
    _t->m_pixFormat = pixFormat;

	u8* rawData = (u8*)_buffer;
	rawData += sizeof(ETCHeader);
	u32  loadedBufferSize = _contentSize - sizeof(ETCHeader);

	loadFromMemory(rawData, size, _t, pixFormat, loadedBufferSize, true, header->m_mipMapCount);
    return true;
}

bbool GFXAdapter_3DS_GR::loadTexture(Texture* _t, const char* _path, u32 _alhpa)
{
	GFX_ASSERT(bfalse, "unsupported on CTR, must be multithread safe");
	return false;
}

//----------------------------------------------------------------------------//

void GFXAdapter_3DS_GR::drawTriangle(const Vec2d& _p1, const Vec2d& _p2, const Vec2d& _p3, float _r, float _g, float _b, float _a, float _z)
{
	GFX_NOT_IMPLEMENTED(drawTriangle);
}

//----------------------------------------------------------------------------//
void GFXAdapter_3DS_GR::drawTriangle(const Vertex& _v1, const Vertex& _v2, const Vertex& _v3, const Texture* _tex, bbool _showWireFrame)
{
	GFX_NOT_IMPLEMENTED(drawTriangle);
}

void GFXAdapter_3DS_GR::drawFlatMesh(const ITF_Mesh& _mesh)
{
	selectTechnique(DEFAULT_TECH2);

    setCullMode(GFX_CULL_NONE);

	ITF_VertexBuffer* pVertexBuffer =  _mesh.m_vtxBuffer ?   _mesh.m_vtxBuffer : _mesh.m_vtxBufferBuffered[_mesh.m_userBuffer];

	for (u32 i = 0; i < _mesh.m_ElementList.size(); i++)
	{
		if ( _mesh.m_ElementList[i].m_material.m_textureDiffuse == ResourceID::Invalid || _mesh.m_ElementList[i].m_count == 0)
		continue;

		setGfxMaterial(_mesh.m_ElementList[i].m_material);
 
		if (!_mesh.m_ElementList[i].m_indexBuffer)
		{
			DrawVertexBuffer(GFX_TRIANGLES, pVertexBuffer, _mesh.m_ElementList[i].m_startIndex, _mesh.m_ElementList[i].m_count);
		}
		else
		{
			DrawIndexedVertexBuffer(GFX_TRIANGLES, pVertexBuffer, _mesh.m_ElementList[i].m_indexBuffer, _mesh.m_ElementList[i].m_count);
		}
	}
}

void GFXAdapter_3DS_GR::drawMesh(ITF_Mesh& _mesh, bbool _showWireFrame)
{
	setCullMode(GFX_CULL_NONE);

	ITF_VertexBuffer* pVertexBuffer =  _mesh.m_vtxBuffer ?   _mesh.m_vtxBuffer : _mesh.m_vtxBufferBuffered[_mesh.m_userBuffer];
	

	selectTechniqueFromVertexFormat(pVertexBuffer->m_vertexFormat);
	
	SetWorldViewMatrix(&_mesh.getMatrix());
	
	for (u32 i = 0; i < _mesh.m_ElementList.size(); i++)
	{
		if ( _mesh.m_ElementList[i].m_material.m_textureDiffuse == ResourceID::Invalid || _mesh.m_ElementList[i].m_count == 0)
			continue;

		setGfxMaterial(_mesh.m_ElementList[i].m_material);

		u32 useUVanim = _mesh.m_ElementList[i].m_material.m_uvAnim.m_uvFlagAnim;
		if (useUVanim)
		{
			computeUVAnim(&_mesh.m_ElementList[i].m_material.m_uvAnim);
			setUVAnim(_mesh.m_ElementList[i].m_material.m_uvAnim);
            m_commandCursor = m_defaultUniformUVMtx.MakeUniformCommand(m_commandCursor, m_UVmat);
		}
        else
        {
            m_UVmat.SetupIdentity();
            m_commandCursor = m_defaultUniformUVMtx.MakeUniformCommand(m_commandCursor, m_UVmat);
        }

		if (!_mesh.m_ElementList[i].m_indexBuffer)
		{
			DrawVertexBuffer(GFX_TRIANGLES, pVertexBuffer, _mesh.m_ElementList[i].m_startIndex, _mesh.m_ElementList[i].m_count);
		}
		else
		{
			DrawIndexedVertexBuffer(GFX_TRIANGLES, pVertexBuffer, _mesh.m_ElementList[i].m_indexBuffer, _mesh.m_ElementList[i].m_count);
		}
	}
}

void GFXAdapter_3DS_GR::drawMeshPT(const ITF_Mesh& _mesh, bbool _showWireFrame)
{
	selectTechnique(DEFAULT_TECH2);
	
	SetWorldViewMatrix(&_mesh.getMatrix());

	setCullMode(GFX_CULL_NONE);

	ITF_VertexBuffer* pVertexBuffer =  _mesh.m_vtxBuffer ?   _mesh.m_vtxBuffer : _mesh.m_vtxBufferBuffered[_mesh.m_userBuffer];

	for (u32 i = 0; i < _mesh.m_ElementList.size(); i++)
	{
		if ( _mesh.m_ElementList[i].m_material.m_textureDiffuse == ResourceID::Invalid || _mesh.m_ElementList[i].m_count == 0)
			continue;

		setGfxMaterial(_mesh.m_ElementList[i].m_material);

		if (!_mesh.m_ElementList[i].m_indexBuffer)
		{
			DrawVertexBuffer(GFX_TRIANGLES, pVertexBuffer, _mesh.m_ElementList[i].m_startIndex, _mesh.m_ElementList[i].m_count);
		}
		else
		{
			DrawIndexedVertexBuffer(GFX_TRIANGLES, pVertexBuffer, _mesh.m_ElementList[i].m_indexBuffer, _mesh.m_ElementList[i].m_count);
		}
	}
}

//----------------------------------------------------------------------------//

void GFXAdapter_3DS_GR::drawLine(const Vec2d& _p1, const Vec2d& _p2, float _r, float _g, float _b, float _size, float _z, float _alpha)
{
	GFX_NOT_IMPLEMENTED(drawLine);
}

void GFXAdapter_3DS_GR::draw2dLine (   const Vec2d& _p1, const Vec2d& _p2, float _size, u32 _color)
{
	GFX_NOT_IMPLEMENTED(draw2dLine);
}

void GFXAdapter_3DS_GR::draw2dBox(const Vec2d& _p, f32 _width, f32 _height, u32 _color0, u32 _color1, u32 _color2, u32 _color3, i32 _rank)
{
	GFX_NOT_IMPLEMENTED(draw2dBox);
}

//----------------------------------------------------------------------------//
void GFXAdapter_3DS_GR::drawCircle(   float _x, float _y, float r, float _r, float _g, float _b, float _z, u32 _segsCount  )
{
	GFX_NOT_IMPLEMENTED(drawCircle);
}

//----------------------------------------------------------------------------//
void GFXAdapter_3DS_GR::drawBox( const Vec2d& _pos, f32 _angle, const Vec2d& _extent, f32 _r, f32 _g, f32 _b, float _z, bbool _fill )
{
	GFX_NOT_IMPLEMENTED(drawBox);
}


//----------------------------------------------------------------------------//

void GFXAdapter_3DS_GR::clear(u32 _buffer, u32 _Col)
{
    u32 buffBits = 0;
    m_currentFrameBuffer->colorBuffer.clearColor[0] = ITFCOLOR_U8_TO_F32(ITFCOLOR_U32_R(_Col));
    m_currentFrameBuffer->colorBuffer.clearColor[1] = ITFCOLOR_U8_TO_F32(ITFCOLOR_U32_G(_Col));
    m_currentFrameBuffer->colorBuffer.clearColor[2] = ITFCOLOR_U8_TO_F32(ITFCOLOR_U32_B(_Col));
    m_currentFrameBuffer->colorBuffer.clearColor[3] = ITFCOLOR_U8_TO_F32(ITFCOLOR_U32_A(_Col));
    m_currentFrameBuffer->depthStencilBuffer.clearDepth   = 1.0f;
    m_currentFrameBuffer->depthStencilBuffer.clearStencil = 0;

    if(_buffer & GFX_CLEAR_COLOR)
    {
        buffBits |= nn::gr::CTR::FrameBuffer::COLOR_BUFFER_BIT;
    }

    if(_buffer & GFX_CLEAR_ZBUFFER)
    {
        buffBits |= nn::gr::CTR::FrameBuffer::DEPTH_STENCIL_BUFFER_BIT;
    }

    m_commandCursor = m_viewport.MakeCommand(m_commandCursor);
    m_commandCursor = m_scissor.MakeCommand(m_commandCursor);
  
    m_commandCursor = m_currentFrameBuffer->MakeCommand(m_commandCursor, buffBits);

    nngxMoveCommandbufferPointer((m_commandCursor - m_startAddr) * sizeof(u32));

    bool isAddSplitDrawCmdlist = true;

    m_currentFrameBuffer->MakeClearRequest(buffBits, isAddSplitDrawCmdlist);
    
    nngxGetCmdlistParameteri(NN_GX_CMDLIST_CURRENT_BUFADDR, reinterpret_cast<GLint*>(&m_commandCursor));
    m_startAddr = m_commandCursor;

    if (m_stereoMode)
    {
        InitStereoState();
        m_stereoMode = true;
        m_lastLeftClearBuffBits = _buffer;
        lastLeftClearBuffR = m_currentFrameBuffer->colorBuffer.clearColor[0];
        lastLeftClearBuffG = m_currentFrameBuffer->colorBuffer.clearColor[1];
        lastLeftClearBuffB = m_currentFrameBuffer->colorBuffer.clearColor[2];
        lastLeftClearBuffA = m_currentFrameBuffer->colorBuffer.clearColor[3];
        m_startLeftRenderingAddr = m_commandCursor;
    }
}

void GFXAdapter_3DS_GR::clear(u32 _buffer, f32 _r, f32 _g, f32 _b, f32 _a)
{
    u32 buffBits = 0;

    m_currentFrameBuffer->colorBuffer.clearColor[0] = _r;
    m_currentFrameBuffer->colorBuffer.clearColor[1] = _g;
    m_currentFrameBuffer->colorBuffer.clearColor[2] = _b;
    m_currentFrameBuffer->colorBuffer.clearColor[3] = _a;

    m_currentFrameBuffer->depthStencilBuffer.clearDepth   = 1.0f;
    m_currentFrameBuffer->depthStencilBuffer.clearStencil = 0;

    if(_buffer & GFX_CLEAR_COLOR)
    {
        buffBits |= nn::gr::CTR::FrameBuffer::COLOR_BUFFER_BIT;
    }

    if(_buffer & GFX_CLEAR_ZBUFFER)
    {
        buffBits |= nn::gr::CTR::FrameBuffer::DEPTH_STENCIL_BUFFER_BIT;
    }

    m_commandCursor = m_viewport.MakeCommand(m_commandCursor);
    m_commandCursor = m_scissor.MakeCommand(m_commandCursor);
    m_commandCursor = m_currentFrameBuffer->MakeCommand(m_commandCursor, buffBits);

    nngxMoveCommandbufferPointer((m_commandCursor - m_startAddr) * sizeof(u32));

    bool isAddSplitDrawCmdlist = true;

    m_currentFrameBuffer->MakeClearRequest(buffBits, isAddSplitDrawCmdlist);

    nngxGetCmdlistParameteri(NN_GX_CMDLIST_CURRENT_BUFADDR, reinterpret_cast<GLint*>(&m_commandCursor));
    m_startAddr = m_commandCursor;

    if (m_stereoMode)
    {
        InitStereoState();
        m_stereoMode = true;
        m_lastLeftClearBuffBits = _buffer;
        lastLeftClearBuffR = m_currentFrameBuffer->colorBuffer.clearColor[0];
        lastLeftClearBuffG = m_currentFrameBuffer->colorBuffer.clearColor[1];
        lastLeftClearBuffB = m_currentFrameBuffer->colorBuffer.clearColor[2];
        lastLeftClearBuffA = m_currentFrameBuffer->colorBuffer.clearColor[3];
        m_startLeftRenderingAddr = m_commandCursor;
    }
}

//----------------------------------------------------------------------------//

void GFXAdapter_3DS_GR::setAlphaBlend(GFX_BLENDMODE _Blend)
{
    if (u_CurrentBlendingMode == _Blend)
    {
        return;
    }
    u_CurrentBlendingMode = _Blend;

    m_isRenderStateDirty = true;
    switch(_Blend)
    {
    case GFX_BLEND_UNKNOWN:
    case GFX_BLEND_COPY:
        m_renderState.blend.isEnable = false;
        m_renderState.blend.eqRgb = PICA_DATA_BLEND_EQUATION_ADD;
        m_renderState.blend.eqAlpha = PICA_DATA_BLEND_EQUATION_ADD;
        m_renderState.blend.srcRgb = PICA_DATA_BLEND_FUNC_ONE;
        m_renderState.blend.dstRgb = PICA_DATA_BLEND_FUNC_ZERO;
        m_renderState.blend.srcAlpha = PICA_DATA_BLEND_FUNC_ONE;
        m_renderState.blend.srcAlpha = PICA_DATA_BLEND_FUNC_ZERO;
        break;

    case GFX_BLEND_ALPHA:
        m_renderState.blend.isEnable = true;
        m_renderState.blend.eqRgb = PICA_DATA_BLEND_EQUATION_ADD;
        m_renderState.blend.eqAlpha = PICA_DATA_BLEND_EQUATION_ADD;
        m_renderState.blend.srcRgb = PICA_DATA_BLEND_FUNC_SRC_ALPHA;
        m_renderState.blend.dstRgb = PICA_DATA_BLEND_FUNC_ONE_MINUS_SRC_ALPHA;
        m_renderState.blend.srcAlpha = PICA_DATA_BLEND_FUNC_SRC_ALPHA;
        m_renderState.blend.srcAlpha = PICA_DATA_BLEND_FUNC_ONE_MINUS_SRC_ALPHA;
        break;

    case GFX_BLEND_ALPHAPREMULT:
        m_renderState.blend.isEnable = true;
        m_renderState.blend.eqRgb = PICA_DATA_BLEND_EQUATION_ADD;
        m_renderState.blend.eqAlpha = PICA_DATA_BLEND_EQUATION_ADD;
        m_renderState.blend.srcRgb = PICA_DATA_BLEND_FUNC_ONE;
        m_renderState.blend.dstRgb = PICA_DATA_BLEND_FUNC_ONE_MINUS_SRC_ALPHA;
        m_renderState.blend.srcAlpha = PICA_DATA_BLEND_FUNC_ONE;
        m_renderState.blend.srcAlpha = PICA_DATA_BLEND_FUNC_ONE_MINUS_SRC_ALPHA;
        break;

    case GFX_BLEND_ALPHADEST:
        m_renderState.blend.isEnable = true;
        m_renderState.blend.eqRgb = PICA_DATA_BLEND_EQUATION_ADD;
        m_renderState.blend.eqAlpha = PICA_DATA_BLEND_EQUATION_ADD;
        m_renderState.blend.srcRgb = PICA_DATA_BLEND_FUNC_DST_ALPHA;
        m_renderState.blend.dstRgb = PICA_DATA_BLEND_FUNC_ONE_MINUS_DST_ALPHA;
        m_renderState.blend.srcAlpha = PICA_DATA_BLEND_FUNC_DST_ALPHA;
        m_renderState.blend.srcAlpha = PICA_DATA_BLEND_FUNC_ONE_MINUS_DST_ALPHA;
        break;

    case GFX_BLEND_ALPHADESTPREMULT:
        m_renderState.blend.isEnable = true;
        m_renderState.blend.eqRgb = PICA_DATA_BLEND_EQUATION_ADD;
        m_renderState.blend.eqAlpha = PICA_DATA_BLEND_EQUATION_ADD;
        m_renderState.blend.srcRgb = PICA_DATA_BLEND_FUNC_DST_ALPHA;
        m_renderState.blend.dstRgb = PICA_DATA_BLEND_FUNC_ONE;
        m_renderState.blend.srcAlpha = PICA_DATA_BLEND_FUNC_DST_ALPHA;
        m_renderState.blend.srcAlpha = PICA_DATA_BLEND_FUNC_ONE;
        break;

    case GFX_BLEND_ADD:
        m_renderState.blend.isEnable = true;
        m_renderState.blend.eqRgb = PICA_DATA_BLEND_EQUATION_ADD;
        m_renderState.blend.eqAlpha = PICA_DATA_BLEND_EQUATION_ADD;
        m_renderState.blend.srcRgb = PICA_DATA_BLEND_FUNC_ONE;
        m_renderState.blend.dstRgb = PICA_DATA_BLEND_FUNC_ONE;
        m_renderState.blend.srcAlpha = PICA_DATA_BLEND_FUNC_ONE;
        m_renderState.blend.srcAlpha = PICA_DATA_BLEND_FUNC_ONE;
        break;

    case GFX_BLEND_ADDALPHA:
        m_renderState.blend.isEnable = true;
        m_renderState.blend.eqRgb = PICA_DATA_BLEND_EQUATION_ADD;
        m_renderState.blend.eqAlpha = PICA_DATA_BLEND_EQUATION_ADD;
        m_renderState.blend.srcRgb = PICA_DATA_BLEND_FUNC_SRC_ALPHA;
        m_renderState.blend.dstRgb = PICA_DATA_BLEND_FUNC_ONE;
        m_renderState.blend.srcAlpha = PICA_DATA_BLEND_FUNC_SRC_ALPHA;
        m_renderState.blend.srcAlpha = PICA_DATA_BLEND_FUNC_ONE;
        break;

    case GFX_BLEND_SUBALPHA:
        m_renderState.blend.isEnable = true;
        m_renderState.blend.eqRgb = PICA_DATA_BLEND_EQUATION_REVERSE_SUBTRACT;
        m_renderState.blend.eqAlpha = PICA_DATA_BLEND_EQUATION_REVERSE_SUBTRACT;
        m_renderState.blend.srcRgb = PICA_DATA_BLEND_FUNC_SRC_ALPHA;
        m_renderState.blend.dstRgb = PICA_DATA_BLEND_FUNC_ONE;
        m_renderState.blend.srcAlpha = PICA_DATA_BLEND_FUNC_SRC_ALPHA;
        m_renderState.blend.srcAlpha = PICA_DATA_BLEND_FUNC_ONE;
        break;

    case GFX_BLEND_SUB:
        m_renderState.blend.isEnable = true;
        m_renderState.blend.eqRgb = PICA_DATA_BLEND_EQUATION_ADD;
        m_renderState.blend.eqAlpha = PICA_DATA_BLEND_EQUATION_ADD;
        m_renderState.blend.srcRgb = PICA_DATA_BLEND_FUNC_ZERO;
        m_renderState.blend.dstRgb = PICA_DATA_BLEND_FUNC_ONE_MINUS_SRC_COLOR;
        m_renderState.blend.srcAlpha = PICA_DATA_BLEND_FUNC_ZERO;
        m_renderState.blend.srcAlpha = PICA_DATA_BLEND_FUNC_ONE_MINUS_SRC_COLOR;
        break;

    case GFX_BLEND_MUL:
        m_renderState.blend.isEnable = true;
        m_renderState.blend.eqRgb = PICA_DATA_BLEND_EQUATION_ADD;
        m_renderState.blend.eqAlpha = PICA_DATA_BLEND_EQUATION_ADD;
        m_renderState.blend.srcRgb = PICA_DATA_BLEND_FUNC_ZERO;
        m_renderState.blend.dstRgb = PICA_DATA_BLEND_FUNC_SRC_COLOR;
        m_renderState.blend.srcAlpha = PICA_DATA_BLEND_FUNC_ZERO;
        m_renderState.blend.srcAlpha = PICA_DATA_BLEND_FUNC_SRC_COLOR;
        break;

    case GFX_BLEND_ALPHAMUL:
        m_renderState.blend.isEnable = true;
        m_renderState.blend.eqRgb = PICA_DATA_BLEND_EQUATION_ADD;
        m_renderState.blend.eqAlpha = PICA_DATA_BLEND_EQUATION_ADD;
        m_renderState.blend.srcRgb = PICA_DATA_BLEND_FUNC_ZERO;
        m_renderState.blend.dstRgb = PICA_DATA_BLEND_FUNC_SRC_ALPHA;
        m_renderState.blend.srcAlpha = PICA_DATA_BLEND_FUNC_ZERO;
        m_renderState.blend.srcAlpha = PICA_DATA_BLEND_FUNC_SRC_ALPHA;
        break;

    case GFX_BLEND_IALPHAMUL:
        m_renderState.blend.isEnable = true;
        m_renderState.blend.eqRgb = PICA_DATA_BLEND_EQUATION_ADD;
        m_renderState.blend.eqAlpha = PICA_DATA_BLEND_EQUATION_ADD;
        m_renderState.blend.srcRgb = PICA_DATA_BLEND_FUNC_ZERO;
        m_renderState.blend.dstRgb = PICA_DATA_BLEND_FUNC_ONE_MINUS_SRC_ALPHA;
        m_renderState.blend.srcAlpha = PICA_DATA_BLEND_FUNC_ZERO;
        m_renderState.blend.srcAlpha = PICA_DATA_BLEND_FUNC_ONE_MINUS_SRC_ALPHA;
        break;

    case GFX_BLEND_IALPHA:
        m_renderState.blend.isEnable = true;
        m_renderState.blend.eqRgb = PICA_DATA_BLEND_EQUATION_ADD;
        m_renderState.blend.eqAlpha = PICA_DATA_BLEND_EQUATION_ADD;
        m_renderState.blend.srcRgb = PICA_DATA_BLEND_FUNC_ONE_MINUS_SRC_ALPHA;
        m_renderState.blend.dstRgb = PICA_DATA_BLEND_FUNC_SRC_ALPHA;
        m_renderState.blend.srcAlpha = PICA_DATA_BLEND_FUNC_ONE_MINUS_SRC_ALPHA;
        m_renderState.blend.srcAlpha = PICA_DATA_BLEND_FUNC_SRC_ALPHA;
        break;

    case GFX_BLEND_IALPHAPREMULT:
        m_renderState.blend.isEnable = true;
        m_renderState.blend.eqRgb = PICA_DATA_BLEND_EQUATION_ADD;
        m_renderState.blend.eqAlpha = PICA_DATA_BLEND_EQUATION_ADD;
        m_renderState.blend.srcRgb = PICA_DATA_BLEND_FUNC_ONE;
        m_renderState.blend.dstRgb = PICA_DATA_BLEND_FUNC_SRC_ALPHA;
        m_renderState.blend.srcAlpha = PICA_DATA_BLEND_FUNC_ONE;
        m_renderState.blend.srcAlpha = PICA_DATA_BLEND_FUNC_SRC_ALPHA;
        break;

    case GFX_BLEND_IALPHADEST:
        m_renderState.blend.isEnable = true;
        m_renderState.blend.eqRgb = PICA_DATA_BLEND_EQUATION_ADD;
        m_renderState.blend.eqAlpha = PICA_DATA_BLEND_EQUATION_ADD;
        m_renderState.blend.srcRgb = PICA_DATA_BLEND_FUNC_ONE_MINUS_DST_ALPHA;
        m_renderState.blend.dstRgb = PICA_DATA_BLEND_FUNC_DST_ALPHA;
        m_renderState.blend.srcAlpha = PICA_DATA_BLEND_FUNC_ONE_MINUS_DST_ALPHA;
        m_renderState.blend.srcAlpha = PICA_DATA_BLEND_FUNC_DST_ALPHA;
        break;

    case GFX_BLEND_IALPHADESTPREMULT:
        m_renderState.blend.isEnable = true;
        m_renderState.blend.eqRgb = PICA_DATA_BLEND_EQUATION_ADD;
        m_renderState.blend.eqAlpha = PICA_DATA_BLEND_EQUATION_ADD;
        m_renderState.blend.srcRgb = PICA_DATA_BLEND_FUNC_ONE_MINUS_DST_ALPHA;
        m_renderState.blend.dstRgb = PICA_DATA_BLEND_FUNC_ONE;
        m_renderState.blend.srcAlpha = PICA_DATA_BLEND_FUNC_ONE_MINUS_DST_ALPHA;
        m_renderState.blend.srcAlpha = PICA_DATA_BLEND_FUNC_ONE;
        break;

    case GFX_BLEND_MUL2X:
        m_renderState.blend.isEnable = true;
        m_renderState.blend.eqRgb = PICA_DATA_BLEND_EQUATION_ADD;
        m_renderState.blend.eqAlpha = PICA_DATA_BLEND_EQUATION_ADD;
        m_renderState.blend.srcRgb = PICA_DATA_BLEND_FUNC_DST_COLOR;
        m_renderState.blend.dstRgb = PICA_DATA_BLEND_FUNC_SRC_COLOR;
        m_renderState.blend.srcAlpha = PICA_DATA_BLEND_FUNC_DST_COLOR;
        m_renderState.blend.srcAlpha = PICA_DATA_BLEND_FUNC_SRC_COLOR;
        break;
    }
}

//----------------------------------------------------------------------------//

void GFXAdapter_3DS_GR::setCullMode(GFX_CULLMODE _Cull)
{
    if (m_previousCullMode == _Cull)
    {
        return;
    }
    m_previousCullMode = _Cull;

    m_isRenderStateDirty = true;
	switch(_Cull)
	{
	case GFX_CULL_NONE:
        m_renderState.cullingTest.isEnable = false;
        break;
    case GFX_CALL_CCW:
        m_renderState.cullingTest.isEnable = true;
        m_renderState.cullingTest.frontFace = nn::gr::CTR::RenderState::Culling::FRONT_FACE_CCW;
        m_renderState.cullingTest.cullFace = nn::gr::CTR::RenderState::Culling::CULL_FACE_BACK;
        break;
	case GFX_CULL_CW:
        m_renderState.cullingTest.isEnable = true;
        m_renderState.cullingTest.frontFace = nn::gr::CTR::RenderState::Culling::FRONT_FACE_CW;
        m_renderState.cullingTest.cullFace = nn::gr::CTR::RenderState::Culling::CULL_FACE_BACK;
        break;
	}
}

//----------------------------------------------------------------------------//

void GFXAdapter_3DS_GR::depthMask(u32 _depthMask)
{
    if(m_previousDepthMask == _depthMask)
    {
        return;
    }
    m_previousDepthMask = _depthMask;

    m_isRenderStateDirty = true;
    m_renderState.depthTest.isEnableWrite = (_depthMask) ? true : false;
}

//----------------------------------------------------------------------------//

void GFXAdapter_3DS_GR::depthFunc(u32 _func)
{
    if (m_previousDepthFunc == _func)
    {
        return;
    }
    m_previousDepthFunc = _func;

    m_isRenderStateDirty = true;
    switch(_func)
    {
    case GFX_CMP_NEVER:
        m_renderState.depthTest.func = PICA_DATA_DEPTH_TEST_NEVER;
        break;
    case GFX_CMP_LESS:
        m_renderState.depthTest.func = PICA_DATA_DEPTH_TEST_LESS;
        break;
    case GFX_CMP_EQUAL:
        m_renderState.depthTest.func = PICA_DATA_DEPTH_TEST_EQUAL;
        break;
    case GFX_CMP_LESSEQUAL:
        m_renderState.depthTest.func = PICA_DATA_DEPTH_TEST_LEQUAL;
        break;
    case GFX_CMP_GREATER:
        m_renderState.depthTest.func = PICA_DATA_DEPTH_TEST_GREATER;
        break;
    case GFX_CMP_NOTEQUAL:
        m_renderState.depthTest.func = PICA_DATA_DEPTH_TEST_NOTEQUAL;
        break;
    case GFX_CMP_GREATEREQUAL:
        m_renderState.depthTest.func = PICA_DATA_DEPTH_TEST_GEQUAL;
        break;
    case GFX_CMP_ALWAYS:
        m_renderState.depthTest.func = PICA_DATA_DEPTH_TEST_ALWAYS;
        break;
    }
}

//----------------------------------------------------------------------------//

void GFXAdapter_3DS_GR::depthTest(u32 _test)
{
    if (m_previousDepthTest == _test)
    {
        return;
    }
    m_previousDepthTest = _test;

    m_isRenderStateDirty = true;
    m_renderState.depthTest.isEnable = (_test) ? true : false;
}

//----------------------------------------------------------------------------//

void GFXAdapter_3DS_GR::setScissorRect(GFX_RECT* _clipRect)
{
	m_scissor.Set(_clipRect->top, _clipRect->left, _clipRect->bottom - _clipRect->top, _clipRect->right - _clipRect->left);
}

//----------------------------------------------------------------------------//

void GFXAdapter_3DS_GR::SetTexture(int _Sampler, Texture* _Texture, bbool _linearFiltering)
{
    SetTexture(_Sampler, (const Texture*)_Texture, _linearFiltering);
}

//----------------------------------------------------------------------------//

void GFXAdapter_3DS_GR::SetTexture(int _Sampler, const Texture* _Texture, bbool _linearFiltering)
{
    if (_Texture == m_currentTexture)
        return;

    m_currentTexture = _Texture;
    m_isTextureDirty = true;

    m_texture.unit0.texType = PICA_DATA_TEXTURE0_SAMPLER_TYPE_TEXTURE_2D;
    m_texture.unit0.physicalAddr = nngxGetPhysicalAddr( (uptr)( _Texture->m_adapterimplementationData ) );
    if (_Texture->m_mipMapLevels > 1)
    {
        m_texture.unit0.magFilter = PICA_DATA_TEXTURE_MAG_FILTER_LINEAR;
        m_texture.unit0.minFilter = PICA_DATA_TEXTURE_MIN_FILTER_LINEAR_MIPMAP_NEAREST;
        m_texture.unit0.minLodLevel = 0;
        m_texture.unit0.maxLodLevel = _Texture->m_mipMapLevels - 1;
    }
    else
    {
        m_texture.unit0.magFilter = PICA_DATA_TEXTURE_MAG_FILTER_LINEAR;
        m_texture.unit0.minFilter = PICA_DATA_TEXTURE_MIN_FILTER_LINEAR;
        m_texture.unit0.minLodLevel = 0;
        m_texture.unit0.maxLodLevel = 0;
    }

    m_texture.unit0.wrapS = PICA_DATA_TEXTURE_WRAP_REPEAT;
    m_texture.unit0.wrapT = PICA_DATA_TEXTURE_WRAP_REPEAT;

    m_texture.unit0.width  = _Texture->getSizeX();
    m_texture.unit0.height = _Texture->getSizeY();

    PicaDataTextureFormat picaTexFormat;
    switch(_Texture->m_pixFormat)
    {
    case Texture::PF_RGB_NATIVE_CTR:
        picaTexFormat = PICA_DATA_TEXTURE_FORMAT_RGB_UNSIGNED_BYTE;
        break;
    case Texture::PF_RGBA_NATIVE_CTR:
        picaTexFormat = PICA_DATA_TEXTURE_FORMAT_RGBA_UNSIGNED_BYTE;
        break;
    case Texture::PF_ETC1_CTR:
        picaTexFormat = PICA_DATA_TEXTURE_FORMAT_ETC1_RGB8_NATIVE_DMP;
        break;
    case Texture::PF_ETC1_A4_CTR:
        picaTexFormat = PICA_DATA_TEXTURE_FORMAT_ETC1_ALPHA_RGB8_A4_NATIVE_DMP;
        break;
    default:
        GFX_ASSERT(false, "Cannot bind a texture that are not cooked");
    }
    m_texture.unit0.format = picaTexFormat;
}


//----------------------------------------------------------------------------//

void GFXAdapter_3DS_GR::SetTextureBind(int _Sampler, unsigned int _Bind, bbool _linearFiltering)
{
	if (_Bind == 0 && m_currentTexture != NULL)
    {
        m_currentTexture = NULL;
        m_isTextureDirty = true;
    }
}

//----------------------------------------------------------------------------//

void GFXAdapter_3DS_GR::lockTexture( Texture* _tex, LOCKED_TEXTURE* _lockTex, u32 _flag )
{
	GFX_NOT_IMPLEMENTED(lockTexture);

}

//----------------------------------------------------------------------------//

void GFXAdapter_3DS_GR::unlockTexture( Texture* _tex)
{
	GFX_NOT_IMPLEMENTED(unlockTexture);
}

//----------------------------------------------------------------------------//

void GFXAdapter_3DS_GR::DrawPrimitive(PRIMITIVETYPE _type, const void* _p_Vertex, u32 _NumberVertex)
{
	GFX_NOT_IMPLEMENTED(DrawPrimitive);
}

//----------------------------------------------------------------------------//

void GFXAdapter_3DS_GR::setMatrixTransform(MATRIXMODE _type, const GMatrix44* _Matrix)
{
	//GFX_NOT_IMPLEMENTED(setMatrixTransform);
}


//----------------------------------------------------------------------------//

void GFXAdapter_3DS_GR::setupViewport(GFX_RECT* _Viewport)
{
    m_viewport.Set(_Viewport->top, _Viewport->left, _Viewport->bottom - _Viewport->top, _Viewport->right - _Viewport->left);
    m_scissor.Set(_Viewport->top, _Viewport->left, _Viewport->bottom - _Viewport->top, _Viewport->right - _Viewport->left);
}

//----------------------------------------------------------------------------//

void GFXAdapter_3DS_GR::unprojectPoint(const Vec2d& _in, float _viewDistance, const GMatrix44* _MatrixModel, const GMatrix44* _MatrixProj, const int* _View, Vec2d& _out)
{
	GFX_NOT_IMPLEMENTED(unprojectPoint);
}

//----------------------------------------------------------------------------//

void GFXAdapter_3DS_GR::setLookAtMatrix(GMatrix44* _MatrixOut, double _midx, double _midy, float _viewDistance, float _aspect)
{
	GFX_NOT_IMPLEMENTED(setLookAtMatrix);
}

//----------------------------------------------------------------------------//

void GFXAdapter_3DS_GR::getViewport(GFX_RECT* _RectOut)
{
    _RectOut->left = m_viewport.x;
    _RectOut->top =  m_viewport.y;
    _RectOut->right =  m_viewport.width;
    _RectOut->bottom = m_viewport.height;
}

//----------------------------------------------------------------------------//

void GFXAdapter_3DS_GR::generateTexture(Texture* _texture)
{
    GFX_NOT_IMPLEMENTED(generateTexture);
}

//----------------------------------------------------------------------------//

void GFXAdapter_3DS_GR::cleanupTexture(Texture* _texture)
{
	if (!_texture)
    {
        return;
    }

    GFXMEM::DevMemFree((void*)_texture->m_adapterimplementationData);
	_texture->m_adapterimplementationData = 0;
}

//----------------------------------------------------------------------------//

void GFXAdapter_3DS_GR::cleanupRenderTarget(renderTarget* _target)
{
	GFX_NOT_IMPLEMENTED(cleanupRenderTarget);
}


//----------------------------------------------------------------------------//

void GFXAdapter_3DS_GR::cleanupBaseTexture(Texture* _texture)
{
    if (!_texture)
    {
        return;
    }

    GFXMEM::DevMemFree((void*)_texture->m_adapterimplementationData);
    _texture->m_adapterimplementationData = 0;
}

//----------------------------------------------------------------------------//

void GFXAdapter_3DS_GR::setTextureSize(Texture* _texture, const Size& _size )
{
    GFX_ASSERT((_size.d_width <= 1024) || (_size.d_height <= 1024),  "setTextureSize: size too big");	
	_texture->setSize((u32)_size.d_width, (u32)_size.d_height); //CASA::3DS::RAC:: merge with CL 15306
	//_texture->m_sizeX = (u32)_size.d_width;
	//_texture->m_sizeY = (u32)_size.d_height;
}

//----------------------------------------------------------------------------//

unsigned char* GFXAdapter_3DS_GR::grabTexture(Texture* _texture)
{
	GFX_NOT_IMPLEMENTED(grabTexture);

	return NULL;
}


//----------------------------------------------------------------------------//

u32 GFXAdapter_3DS_GR::getMaxTextureSize()
{
	GFX_NOT_IMPLEMENTED(getMaxTextureSize);
    return 0;
}

//----------------------------------------------------------------------------//

void GFXAdapter_3DS_GR::loadFromMemory(   const void* _buffer, const Size& _texture_size, Texture* _texture, Texture::PixFormat _pixel_format,const u32 _buffer_sizeload,bbool bCooked, u32 _mipmapCount)
{
    void* data = GFXMEM::DevMemAlloc(_buffer_sizeload, 128);
    GFX_ASSERT(data, "Not enough memory to allocate a texture\n");
    ITF_Memcpy(data, _buffer, _buffer_sizeload);
    
     _texture->m_adapterimplementationData = (u32)data;
}

//----------------------------------------------------------------------------//

void GFXAdapter_3DS_GR::saveToMemory(void* _buffer, Texture* _Tex)
{
	GFX_NOT_IMPLEMENTED(saveToMemory);
}

//----------------------------------------------------------------------------//

void GFXAdapter_3DS_GR::setSamplerState( u32 _samp, GFX_SAMPLERSTATETYPE _type, i32 _value )
{
    PicaDataTextureWrap addrMode = PICA_DATA_TEXTURE_WRAP_REPEAT;
	switch (_value)
	{
	case GFXTADDRESS_WRAP:
		addrMode = PICA_DATA_TEXTURE_WRAP_REPEAT;
		break;
	case GFXTADDRESS_MIRROR:
		addrMode = PICA_DATA_TEXTURE_WRAP_MIRRORED_REPEAT;
		break;
	case GFXTADDRESS_CLAMP:
		addrMode = PICA_DATA_TEXTURE_WRAP_CLAMP_TO_EDGE;
		break;
	case GFXTADDRESS_BORDER:
		addrMode = PICA_DATA_TEXTURE_WRAP_CLAMP_TO_BORDER;
		break;
	}

    switch (_type)
    {
    case GFXSAMP_ADDRESSU:
        if (m_previousUaddressingMode != _value)
        {
            m_previousUaddressingMode = (GFX_TEXADRESSMODE)_value;
            m_texture.unit0.wrapS = addrMode;
            m_isTextureDirty = true;
        }
        break;
    case GFXSAMP_ADDRESSV:
        if (m_previousVaddressingMode != _value)
        {
            m_previousVaddressingMode = (GFX_TEXADRESSMODE)_value;
            m_texture.unit0.wrapT = addrMode;
            m_isTextureDirty = true;
        }
        break;
    }
}

//----------------------------------------------------------------------------//

void GFXAdapter_3DS_GR::clearRenderTarget(renderTarget* _target, u32 _color )
{
	GFX_NOT_IMPLEMENTED(clearRenderTarget);
}

//----------------------------------------------------------------------------//

void  GFXAdapter_3DS_GR::initialiseRenderTarget(renderTarget* _target)
{
	GFX_NOT_IMPLEMENTED(initialiseRenderTarget);
}

//----------------------------------------------------------------------------//

void GFXAdapter_3DS_GR::enableRenderTarget(renderTarget* _target)
{
	GFX_NOT_IMPLEMENTED(enableRenderTarget);
}

//----------------------------------------------------------------------------//

void GFXAdapter_3DS_GR::disableRenderTarget(renderTarget* _target)
{
	GFX_NOT_IMPLEMENTED(disableRenderTarget);
}

//----------------------------------------------------------------------------//

void GFXAdapter_3DS_GR::resizeRenderTarget(   renderTarget* _target, const Size& _size)
{
	GFX_NOT_IMPLEMENTED(resizeRenderTarget);
}

//----------------------------------------------------------------------------//

void GFXAdapter_3DS_GR::present()
{
    nngxGetCmdlistParameteri( NN_GX_CMDLIST_CURRENT_BUFADDR, reinterpret_cast<GLint*>( &m_commandCursor ) );
    m_startAddr = m_commandCursor;

    if ( isShowFPS() || isShowRasters() || m_DBGTexts.size() > 0)
    {
        m_commandCursor = m_viewport.MakeCommand( m_commandCursor );
        m_commandCursor = m_scissor.MakeCommand( m_commandCursor );
        // Cull, Depth, Stencil, AlphaTest, Blend, etc.
        m_commandCursor = m_fontRenderState.MakeCommand( m_commandCursor );
        m_commandCursor = m_fontCombiner.MakeCommand(m_commandCursor);
        m_commandCursor = nn::gr::CTR::Vertex::MakeDisableCommand(m_commandCursor);
        
        nngxMoveCommandbufferPointer( ( m_commandCursor - m_startAddr ) * sizeof(u32) );

        BeginDrawingString();

       // if (isShowRasters())
       // {
           /* World* pWorld = CURRENTWORLD;

            if (pWorld)
            {
                const CellMap& cells = pWorld->getCells();
       
                u32 enabledCellCount = 0;
                u32 objCount = 0;
                CellMap::const_iterator iter = cells.begin();
                for (;iter!=cells.end();++iter)
                {
                    const Cell* pCell = ((*iter).second);
                    
                    if (pCell->isEnable())
                    {
                        const ObjectRefList& vPickable = pCell->getAllObjects();
                        objCount += vPickable.size();
                        enabledCellCount++;
                    }
                }
                DrawString(5, 205, "cells : %d", enabledCellCount);
                DrawString(5, 220, "obj : %d", objCount);
            }*/

            if (getfPs() < 40.0f)
            {
                m_TextWriter.SetTextColor(nn::util::Color8(255, 0, 0, 255));
            }
            else if (getfPs() < 55.0f)
            {
                m_TextWriter.SetTextColor(nn::util::Color8(255, 255, 0, 255));
            }
            else
            {
                m_TextWriter.SetTextColor(nn::util::Color8(0, 255, 0, 255));
            }
            DrawString(0, 0, "%0.02f fps\n", getfPs() );

            m_TextWriter.SetTextColor(nn::util::Color8(255, 255, 255, 255));
            RastersManager::get().rastersMinimalDraw();
       // } 
       // else
       // {
            for (int i = 0; i < m_DBGTexts.size(); i++)
            {
                m_TextWriter.SetTextColor(nn::util::Color8(ITFCOLOR_F32_TO_U8(m_DBGTexts[i].m_r), ITFCOLOR_F32_TO_U8(m_DBGTexts[i].m_g), ITFCOLOR_F32_TO_U8(m_DBGTexts[i].m_b), 255));
                StringConverter content(m_DBGTexts[i].m_text);
                DrawString(m_DBGTexts[i].m_x/* - 200.f*/,m_DBGTexts[i].m_y/* - 100.f*/,content.getChar());
            }
            m_DBGTexts.clear();
        //}

        EndDrawingString();
    }

    cancelAllTasks();
    
    TransferBuffer(UPPER_SCREEN | LOWER_SCREEN | EXTENSION_SCREEN);
    nnprofRecordWaitForVBlank();
    WaitVsync(UPPER_SCREEN | LOWER_SCREEN | EXTENSION_SCREEN);

    InputAdapter_3DS *input_ctr = (InputAdapter_3DS*)INPUT_ADAPTER;
    if (input_ctr && (input_ctr->m_padSolo.m_virtualPadStatus[4]==InputAdapter::Pressed))
    {
        if ( input_ctr->m_padSolo.m_virtualPadStatus[1]==InputAdapter::JustPressed )
        {
            toggleRaster();
        }
        else if ( input_ctr->m_padSolo.m_virtualPadStatus[2]==InputAdapter::JustPressed )
        {
            RastersManager::get().cycleRasterGroup();
        }
    }
    gcCollect();
}

//----------------------------------------------------------------------------//

void GFXAdapter_3DS_GR::startRendering( )
{
    nngxGetCmdlistParameteri( NN_GX_CMDLIST_CURRENT_BUFADDR, 
        reinterpret_cast<GLint*>( &m_commandCursor ) );
    
    m_startAddr = m_commandCursor;
    if (m_stereoActive)
    {
        m_stereoMode = true;
    }
    
    InvalidateStateCache();
    renderContext_SetDefault();

    m_commandCursor = m_viewport.MakeCommand( m_commandCursor );
    m_commandCursor = m_scissor.MakeCommand( m_commandCursor );

    m_commandCursor = m_defaultShader.MakeFullCommand( m_commandCursor );

    nn::gr::CTR::Combiner defaultCombiner;
    m_commandCursor = defaultCombiner.MakeCommand(m_commandCursor);
    m_commandCursor = nn::gr::CTR::Texture::MakeDisableCommand(m_commandCursor);
    UpdateGPUProj();
    m_firstProjMtxR = m_projR;
    m_commandCursor = nn::gr::CTR::Vertex::MakeDisableCommand(m_commandCursor);

    m_currentFrameBuffer = &m_frameBuffer0;
}


//----------------------------------------------------------------------------//

void GFXAdapter_3DS_GR::endRendering()
{
    nngxMoveCommandbufferPointer( ( m_commandCursor - m_startAddr ) * sizeof(u32) );
    InvalidateStateCache();
    InitStereoState();
}

//----------------------------------------------------------------------------//

bbool GFXAdapter_3DS_GR::isDeviceLost()
{
	return bfalse;
}

//----------------------------------------------------------------------------//

bbool GFXAdapter_3DS_GR::resetDevice(bbool _forceReset)
{
	return bfalse;
}

///----------------------------------------------------------------------------//
/// Shaders.
///----------------------------------------------------------------------------//

void GFXAdapter_3DS_GR::createShader( ITF_shader* _shader )
{
	GFX_NOT_IMPLEMENTED(createShader);

}

void GFXAdapter_3DS_GR::releaseShader( ITF_shader* _shader )
{
	GFX_NOT_IMPLEMENTED(releaseShader);
}

///////////////////////////////////////////////////////////////////////////////////////////
bbool GFXAdapter_3DS_GR::loadShader( ITF_shader* _shader )
{
	GFX_NOT_IMPLEMENTED(loadShader);
	return true;
}


void GFXAdapter_3DS_GR::InitDefaultShader()
{
    String shaderpath = "shaders/ctr/bin/default.bin";
    File *file = FILEMANAGER->openFile(shaderpath, ITF_FILE_ATTR_READ);
    GFX_ASSERT(file, "Cannot open shader file %s\n", shaderpath.cStr());
    u32 fileSize = file->getLength();
    m_defaultShaderBinary = (u8*)GFXMEM::DevMemAlloc(fileSize, 128);
    file->read(m_defaultShaderBinary, fileSize);

    /////////////////////////////////////////////////////////////////////////////////////
    m_defaultShader.SetupBinary( m_defaultShaderBinary, VertexShader_Default, GeomShader_Default);
    bool success = false;
    success = m_defaultShader.SearchBindSymbol( &m_attribPosition, "aPosition" );
    GFX_ASSERT( success, "aPosition could not be found" );
    success = m_defaultShader.SearchBindSymbol( &m_attribColor, "aColor" );
    GFX_ASSERT( success, "aColor could not be found" );
    success = m_defaultShader.SearchBindSymbol( &m_attribTexCoord0, "aTexCoord0" );
    GFX_ASSERT( success, "aTexCoord0 could not be found" );
    success = m_defaultShader.SearchBindSymbol( &m_attribTexCoord1, "aTexCoord1" );
    GFX_ASSERT( success, "aTexCoord1 could not be found" );
    success = m_defaultShader.SearchBindSymbol( &m_attribTexCoord2, "aTexCoord2" );
    GFX_ASSERT( success, "aTexCoord2 could not be found" );
    success = m_defaultShader.SearchBindSymbol( &m_attribTexCoord3, "aTexCoord3" );
    GFX_ASSERT( success, "aTexCoord2 could not be found" );
    success = m_defaultShader.SearchBindSymbol( &m_attribNormal, "aNormal" );
    GFX_ASSERT( success, "aNormal could not be found" );
    success = m_defaultShader.SearchBindSymbol( &m_attribBlendIndices, "aBlendIndices" );
    GFX_ASSERT( success, "aBlendIndices could not be found" );


    success = m_defaultShader.SearchBindSymbol( &m_defaultUniformProjMtx, "uProjection" );
    GFX_ASSERT( success, "uProjection could not be found" );
    success = m_defaultShader.SearchBindSymbol( &m_defaultUniformModelViewMtx, "uModelView" );
    GFX_ASSERT( success, "uModelView could not be found" );
    success = m_defaultShader.SearchBindSymbol( &m_defaultUniformUVMtx, "uMatUV" );
    GFX_ASSERT( success, "uMatUV could not be found" );
    success = m_defaultShader.SearchBindSymbol( &m_defaultUniformArray0, "uArray0" );
    GFX_ASSERT( success, "uArray0 could not be found" );
    success = m_defaultShader.SearchBindSymbol( &m_defaultUniformArray1, "uArray1" );
    GFX_ASSERT( success, "uArray1 could not be found" );
    success = m_defaultShader.SearchBindSymbol( &m_defaultUniformConst0, "uConst0" );
    GFX_ASSERT( success, "uConst0 could not be found" );
    success = m_defaultShader.SearchBindSymbol( &m_defaultUniformConst1, "uConst1" );
    GFX_ASSERT( success, "uConst1 could not be found" );
    success = m_defaultShader.SearchBindSymbol( &m_defaultUniformConst2, "uConst2" );
    GFX_ASSERT( success, "uConst2 could not be found" );
    success = m_defaultShader.SearchBindSymbol( &m_defaultUniformConst3, "uConst3" );
    GFX_ASSERT( success, "uConst3 could not be found" ); 
}

void GFXAdapter_3DS_GR::InitPatchShader()
{
    String shaderpath = "shaders/ctr/bin/patch.bin";
    File *file = FILEMANAGER->openFile(shaderpath, ITF_FILE_ATTR_READ);
    GFX_ASSERT(file, "Cannot open shader file %s\n", shaderpath.cStr());
    u32 fileSize = file->getLength();
    m_patchShaderBinary = (u8*)GFXMEM::DevMemAlloc(fileSize, 128);
    file->read(m_patchShaderBinary, fileSize);

    /////////////////////////////////////////////////////////////////////////////////////
    m_patchShader.SetupBinary( m_patchShaderBinary, VertexShader_Patch, GeomShader_Patch);
    bool success = false;

    //m_patchShader.SetShaderIndex(VertexShader_Patch, GeomShader_Patch);
    success = m_patchShader.SearchBindSymbol( &m_attribPatchBoneIdx, "aTexCoord0" );
    GFX_ASSERT( success, "aTexCoord0 could not be found" );
    success = m_patchShader.SearchBindSymbol( &m_attribPatchPos_PosNrm, "aTexCoord1" );
    GFX_ASSERT( success, "aTexCoord1 could not be found" );
    success = m_patchShader.SearchBindSymbol( &m_attribPatchUV_UVNrm, "aTexCoord2" );
    GFX_ASSERT( success, "aTexCoord2 could not be found" );

    success = m_patchShader.SearchBindSymbol( &m_patchUniformProjMtx, "uProjectionGS" );
    GFX_ASSERT( success, "uProjection could not be found" );
    success = m_patchShader.SearchBindSymbol( &m_patchUniformModelViewMtx, "uModelViewGS" );
    GFX_ASSERT( success, "uModelView could not be found" );
    success = m_patchShader.SearchBindSymbol( &m_patchUniformZ, "uZ" );
    GFX_ASSERT( success, "uZ could not be found" );
    success = m_patchShader.SearchBindSymbol( &m_patchUniformBonesP1, "uBonesP1" );
    GFX_ASSERT( success, "uBones could not be found" );
    success = m_patchShader.SearchBindSymbol( &m_patchUniformBonesP2, "uBonesP2" );
    GFX_ASSERT( success, "uBones could not be found" );
    success = m_patchShader.SearchBindSymbol( &m_patchUniformMirrorRatio, "uMirrorRatio" );
    GFX_ASSERT( success, "uMirrorRatio could not be found" );
}


//----------------------------------------------------------------------------//
void GFXAdapter_3DS_GR::InitVertexFormats()
{
    m_vertexFormats[VertexFormatIdx_P].dataNum     = 1;
    m_vertexFormats[VertexFormatIdx_P].symbol[0]   = &m_attribPosition;
    m_vertexFormats[VertexFormatIdx_P].dataType[0] = PICA_DATA_SIZE_3_FLOAT;

    m_vertexFormats[VertexFormatIdx_PT].dataNum     = 2;
    m_vertexFormats[VertexFormatIdx_PT].symbol[0]   = &m_attribPosition;
    m_vertexFormats[VertexFormatIdx_PT].dataType[0] = PICA_DATA_SIZE_3_FLOAT;
    m_vertexFormats[VertexFormatIdx_PT].symbol[1]   = &m_attribTexCoord0;
    m_vertexFormats[VertexFormatIdx_PT].dataType[1] = PICA_DATA_SIZE_2_FLOAT;
    
    m_vertexFormats[VertexFormatIdx_PC].dataNum     = 2;
    m_vertexFormats[VertexFormatIdx_PC].symbol[0]   = &m_attribPosition;
    m_vertexFormats[VertexFormatIdx_PC].dataType[0] = PICA_DATA_SIZE_3_FLOAT;
    m_vertexFormats[VertexFormatIdx_PC].symbol[1]   = &m_attribColor;
    m_vertexFormats[VertexFormatIdx_PC].dataType[1] = PICA_DATA_SIZE_4_UNSIGNED_BYTE;

    m_vertexFormats[VertexFormatIdx_PCT].dataNum     = 3;
    m_vertexFormats[VertexFormatIdx_PCT].symbol[0]   = &m_attribPosition;
    m_vertexFormats[VertexFormatIdx_PCT].dataType[0] = PICA_DATA_SIZE_3_FLOAT;
    m_vertexFormats[VertexFormatIdx_PCT].symbol[1]   = &m_attribColor;
    m_vertexFormats[VertexFormatIdx_PCT].dataType[1] = PICA_DATA_SIZE_4_UNSIGNED_BYTE;
    m_vertexFormats[VertexFormatIdx_PCT].symbol[2]   = &m_attribTexCoord0;
    m_vertexFormats[VertexFormatIdx_PCT].dataType[2] = PICA_DATA_SIZE_2_FLOAT;

    m_vertexFormats[VertexFormatIdx_PNCT].dataNum     = 4;
    m_vertexFormats[VertexFormatIdx_PNCT].symbol[0]   = &m_attribPosition;
    m_vertexFormats[VertexFormatIdx_PNCT].dataType[0] = PICA_DATA_SIZE_3_FLOAT;
    m_vertexFormats[VertexFormatIdx_PNCT].symbol[1]   = &m_attribNormal;
    m_vertexFormats[VertexFormatIdx_PNCT].dataType[1] = PICA_DATA_SIZE_3_FLOAT;
    m_vertexFormats[VertexFormatIdx_PNCT].symbol[2]   = &m_attribColor;
    m_vertexFormats[VertexFormatIdx_PNCT].dataType[2] = PICA_DATA_SIZE_4_UNSIGNED_BYTE;
    m_vertexFormats[VertexFormatIdx_PNCT].symbol[3]   = &m_attribTexCoord0;
    m_vertexFormats[VertexFormatIdx_PNCT].dataType[3] = PICA_DATA_SIZE_2_FLOAT;

    m_vertexFormats[VertexFormatIdx_PNC3T].dataNum     = 6;
    m_vertexFormats[VertexFormatIdx_PNC3T].symbol[0]   = &m_attribPosition;
    m_vertexFormats[VertexFormatIdx_PNC3T].dataType[0] = PICA_DATA_SIZE_3_FLOAT;
    m_vertexFormats[VertexFormatIdx_PNC3T].symbol[1]   = &m_attribColor;
    m_vertexFormats[VertexFormatIdx_PNC3T].dataType[1] = PICA_DATA_SIZE_4_UNSIGNED_BYTE;
    m_vertexFormats[VertexFormatIdx_PNC3T].symbol[2]   = &m_attribTexCoord0;
    m_vertexFormats[VertexFormatIdx_PNC3T].dataType[2] = PICA_DATA_SIZE_2_FLOAT;
    m_vertexFormats[VertexFormatIdx_PNC3T].symbol[3]   = &m_attribTexCoord1;
    m_vertexFormats[VertexFormatIdx_PNC3T].dataType[3] = PICA_DATA_SIZE_4_FLOAT;
    m_vertexFormats[VertexFormatIdx_PNC3T].symbol[4]   = &m_attribTexCoord2;
    m_vertexFormats[VertexFormatIdx_PNC3T].dataType[4] = PICA_DATA_SIZE_4_FLOAT;
    m_vertexFormats[VertexFormatIdx_PNC3T].symbol[5]   = &m_attribTexCoord3;
    m_vertexFormats[VertexFormatIdx_PNC3T].dataType[5] = PICA_DATA_SIZE_2_FLOAT;

    m_vertexFormats[VertexFormatIdx_Patch].dataNum     = 3;
    m_vertexFormats[VertexFormatIdx_Patch].symbol[0]   = &m_attribPatchBoneIdx;
    m_vertexFormats[VertexFormatIdx_Patch].dataType[0] = PICA_DATA_SIZE_2_FLOAT;
    m_vertexFormats[VertexFormatIdx_Patch].symbol[1]   = &m_attribPatchPos_PosNrm;
    m_vertexFormats[VertexFormatIdx_Patch].dataType[1] = PICA_DATA_SIZE_4_FLOAT;
    m_vertexFormats[VertexFormatIdx_Patch].symbol[2]   = &m_attribPatchUV_UVNrm;
    m_vertexFormats[VertexFormatIdx_Patch].dataType[2] = PICA_DATA_SIZE_4_FLOAT;
}

void GFXAdapter_3DS_GR::selectTechnique(u32 tech)
{
    m_prevousTech = m_selectedTech;
    m_techBitSet = 1 << tech;
    m_selectedTech = tech;
}

void GFXAdapter_3DS_GR::selectTechniqueFromVertexFormat(u32 vertexFormat)
{
	switch(vertexFormat)
	{
	case VertexFormat_P:
		GFX_ASSERT(false, "VertexFormat_P is not a valid vertex Format");
		break;

	case VertexFormat_PC:
		selectTechnique(DEFAULT_TECH1);
		break;

	case VertexFormat_PT:
    case VertexFormat_PTa:
		selectTechnique(DEFAULT_TECH2);
		break;

	case VertexFormat_PCT:
		selectTechnique(DEFAULT_TECH0);
		break;

	case VertexFormat_PNCT:
		selectTechnique(DEFAULT_TECH4);
		break;

	case VertexFormat_PNC3T:
		selectTechnique(DEFAULT_TECH5);
		break;

	case VertexFormat_PCBT:
		selectTechnique(DEFAULT_TECH1);
		break;

	default:
		LOG("Primitive not recognized");
		break;
	}
}

void GFXAdapter_3DS_GR::beginShader( ITF_shader* _shader )
{
	GFX_NOT_IMPLEMENTED(beginShader);
}

void GFXAdapter_3DS_GR::endShader( ITF_shader* _shader )
{
	GFX_NOT_IMPLEMENTED(endShader);
}


///----------------------------------------------------------------------------//
/// VertexBuffer.
///----------------------------------------------------------------------------//
void GFXAdapter_3DS_GR::createVertexBuffer( ITF_VertexBuffer* _vertexBuffer )
{
	if (_vertexBuffer->bIsDynamic())
	{
		for (u32 i = 0; i < ITF_VertexBuffer::DYNAMIC_BUFFERS_COUNT; i++)
		{
            void* data = GFXMEM::DevMemAlloc(_vertexBuffer->m_nVertex * _vertexBuffer->m_structVertexSize, 4);
            GFX_ASSERT(data != NULL, "RAM vertex buffer allocation failed\n");
            nn::gr::CTR::Vertex *vertex = newAlloc(mId_Vertex ,nn::gr::CTR::Vertex());
            vertex->EnableInterleavedArray(m_vertexFormats[getInternalVertexFormat(_vertexBuffer->m_vertexFormat)], nngxGetPhysicalAddr((uptr)(data)));
            _vertexBuffer->m_dynamicBuffers[i] = (u32)vertex;
            _vertexBuffer->mp_VertexBuffers[i] = (u32)data;
		}
		_vertexBuffer->m_Id = _vertexBuffer->m_dynamicBuffers[0];
        _vertexBuffer->mp_VertexBuffer = _vertexBuffer->mp_VertexBuffers[0];
        _vertexBuffer->m_currentBuffer = 0;
	}
	else
	{
        void* data = GFXMEM::DevMemAlloc(_vertexBuffer->m_nVertex * _vertexBuffer->m_structVertexSize, 4);
        GFX_ASSERT(data != NULL, "RAM vertex buffer allocation failed\n");
        nn::gr::CTR::Vertex *vertex = newAlloc(mId_Vertex ,nn::gr::CTR::Vertex());
        vertex->EnableInterleavedArray(m_vertexFormats[getInternalVertexFormat(_vertexBuffer->m_vertexFormat)], nngxGetPhysicalAddr((uptr)(data)));
        _vertexBuffer->m_Id = (u32)vertex;
        _vertexBuffer->mp_VertexBuffer = (u32)data;
	}
}

void GFXAdapter_3DS_GR::releaseVertexBuffer( ITF_VertexBuffer* _vertexBuffer )
{	
	if (_vertexBuffer->bIsDynamic())
	{
		for (u32 i = 0; i < ITF_VertexBuffer::DYNAMIC_BUFFERS_COUNT; i++)
		{
            delete (nn::gr::CTR::Vertex*)_vertexBuffer->m_dynamicBuffers[i];
            gcFree((void*)_vertexBuffer->mp_VertexBuffers[i]);
			_vertexBuffer->m_dynamicBuffers[i] = 0;
		}
		_vertexBuffer->m_currentBuffer = 0;
	}
	else
	{
        delete (nn::gr::CTR::Vertex*)_vertexBuffer->m_Id;
        gcFree((void*)_vertexBuffer->mp_VertexBuffer);
        _vertexBuffer->m_Id = 0;
	}
}

void GFXAdapter_3DS_GR::LockVertexBuffer( ITF_VertexBuffer* _vertexBuffer, void** _data, u32 _offset, u32 _size, u32 _flag )
{
    if (_vertexBuffer->bIsDynamic())
    {
        _vertexBuffer->m_currentBuffer = (_vertexBuffer->m_currentBuffer+1) % ITF_VertexBuffer::DYNAMIC_BUFFERS_COUNT;
        _vertexBuffer->m_Id = _vertexBuffer->m_dynamicBuffers[_vertexBuffer->m_currentBuffer];
        _vertexBuffer->mp_VertexBuffer = _vertexBuffer->mp_VertexBuffers[_vertexBuffer->m_currentBuffer];
    }
    
    *_data = (void*)_vertexBuffer->mp_VertexBuffer;
}

void GFXAdapter_3DS_GR::UnlockVertexBuffer( ITF_VertexBuffer* _vertexBuffer )
{
}


void GFXAdapter_3DS_GR::setVertexBuffer( ITF_VertexBuffer* _vertexBuffer )
{
    if (m_currentBuffer == (nn::gr::CTR::Vertex*)_vertexBuffer->m_Id)
    {
        return;
    }

    m_previousBuffer = m_currentBuffer;
    m_currentBuffer = (nn::gr::CTR::Vertex*)_vertexBuffer->m_Id;

    if (m_previousBuffer != NULL)
    {
        m_commandCursor = m_previousBuffer->MakeDisableAttrCommand(m_commandCursor);
    }
    m_commandCursor = m_currentBuffer->MakeEnableAttrCommand(m_commandCursor);
}

void GFXAdapter_3DS_GR::appendVertexBuffer(ITF_VertexBuffer* _vertexBuffer, void* _additionalVertData, u32 _additionalVertCount)
{
    if (!_vertexBuffer->bIsDynamic())
    {
        u32 newVerticesCount = _vertexBuffer->m_nVertex + _additionalVertCount;
        
        void* oldData = (void*)_vertexBuffer->mp_VertexBuffer;
        void* newData = GFXMEM::DevMemAlloc(newVerticesCount * _vertexBuffer->m_structVertexSize, 4);
        GFX_ASSERT(newData != NULL, "RAM vertex buffer allocation failed\n");
        
        ITF_Memcpy(newData, oldData, _vertexBuffer->m_nVertex * _vertexBuffer->m_structVertexSize);
        ITF_Memcpy((char*)newData + _vertexBuffer->m_nVertex * _vertexBuffer->m_structVertexSize, _additionalVertData, _additionalVertCount);
        
        gcFree((void*)oldData);

        nn::gr::CTR::Vertex* vertex = (nn::gr::CTR::Vertex*)_vertexBuffer->m_Id;
        vertex->DisableAll();
        vertex->EnableInterleavedArray(m_vertexFormats[getInternalVertexFormat(_vertexBuffer->m_vertexFormat)], nngxGetPhysicalAddr((uptr)(newData)));
        _vertexBuffer->m_Id = (u32)vertex;
        _vertexBuffer->mp_VertexBuffer = (u32)newData;
        _vertexBuffer->m_nVertex = newVerticesCount;
        _vertexBuffer->m_size = newVerticesCount * _vertexBuffer->m_structVertexSize;
    }
}

///----------------------------------------------------------------------------//
/// VertexFormat.
///----------------------------------------------------------------------------//

void GFXAdapter_3DS_GR::setVertexFormat( u32 _vformat )
{
    m_previousVertexFormat = m_currentVertexFormat;
    m_currentVertexFormat = _vformat;

    switch(_vformat)
    {
    case VertexFormat_P:
        m_currentVertexFormat = VertexFormatIdx_P;
        break;

    case VertexFormat_PC:
        m_currentVertexFormat = VertexFormatIdx_PC;
        break;

    case VertexFormat_PT:
        m_currentVertexFormat = VertexFormatIdx_PT;
        break;

    case VertexFormat_PCT:
        m_currentVertexFormat = VertexFormatIdx_PCT;
        break;

    case VertexFormat_PNCT:
        m_currentVertexFormat = VertexFormatIdx_PNCT;
        break;

    case VertexFormat_PNC3T:
        m_currentVertexFormat = VertexFormatIdx_PNC3T;
        break;

    case VertexFormat_PatchAnim:
        m_currentVertexFormat = VertexFormatIdx_Patch;
        break;

    default:
        GFX_ASSERT(false ,"Vertex format not recognized");
        break;
    }
}

GFXAdapter_3DS_GR::VertexFormatIdx	GFXAdapter_3DS_GR::getInternalVertexFormat(u32 _vformat)
{
    VertexFormatIdx returnVal;
    switch(_vformat)
    {
    case VertexFormat_P:
        returnVal = VertexFormatIdx_P;
        break;

    case VertexFormat_PC:
        returnVal = VertexFormatIdx_PC;
        break;

    case VertexFormat_PT:
    case VertexFormat_PTa:
        returnVal = VertexFormatIdx_PT;
        break;

    case VertexFormat_PCT:
        returnVal = VertexFormatIdx_PCT;
        break;

    case VertexFormat_PNCT:
        returnVal = VertexFormatIdx_PNCT;
        break;

    case VertexFormat_PNC3T:
        returnVal = VertexFormatIdx_PNC3T;
        break;

    case VertexFormat_PatchAnim:
        returnVal = VertexFormatIdx_Patch;
        break;

    default:
        GFX_ASSERT(false ,"Vertex format not recognized");
        break;
    }

    return returnVal;
}

///----------------------------------------------------------------------------//
/// IndexBuffer.
///----------------------------------------------------------------------------//

void GFXAdapter_3DS_GR::createIndexBuffer( ITF_IndexBuffer* _indexBuffer )
{
    if (_indexBuffer->bIsDynamic())
    {
        for (u32 i = 0; i < ITF_IndexBuffer::DYNAMIC_BUFFERS_COUNT; i++)
        {
            void* _data = GFXMEM::DevMemAlloc(_indexBuffer->m_nIndices * sizeof(u16), 4);
            GFX_ASSERT(_data != NULL, "RAM vertex buffer allocation failed\n");
            _indexBuffer->m_dynamicBuffers[i] = (u32)_data;
        }
        _indexBuffer->m_Id = _indexBuffer->m_dynamicBuffers[0];
        _indexBuffer->m_currentBuffer = 0;
    }
    else
    {
        void* _data = GFXMEM::DevMemAlloc(_indexBuffer->m_nIndices * sizeof(u16), 4);
        GFX_ASSERT(_data != NULL, "RAM vertex buffer allocation failed\n");
        _indexBuffer->m_Id = (u32)_data;
    }
}

void GFXAdapter_3DS_GR::releaseIndexBuffer( ITF_IndexBuffer* _indexBuffer )
{
    if (_indexBuffer->bIsDynamic())
    {
        for (u32 i = 0; i < ITF_IndexBuffer::DYNAMIC_BUFFERS_COUNT; i++)
        {
            gcFree((void*)_indexBuffer->m_dynamicBuffers[i]);
            _indexBuffer->m_dynamicBuffers[i] = 0;
        }
        _indexBuffer->m_currentBuffer = 0;
    }
    else
    {
        gcFree((void*)_indexBuffer->m_Id);
        _indexBuffer->m_Id = 0;
    }
}


void GFXAdapter_3DS_GR::LockIndexBuffer( ITF_IndexBuffer* _indexBuffer, void** _data )
{
    if (_indexBuffer->bIsDynamic())
    {
        _indexBuffer->m_currentBuffer = (_indexBuffer->m_currentBuffer+1) % ITF_VertexBuffer::DYNAMIC_BUFFERS_COUNT;
        _indexBuffer->m_Id = _indexBuffer->m_dynamicBuffers[_indexBuffer->m_currentBuffer];
    }

    *_data = (void*)_indexBuffer->m_Id;
}

void GFXAdapter_3DS_GR::UnlockIndexBuffer( ITF_IndexBuffer* _indexBuffer )
{
}

void GFXAdapter_3DS_GR::UnlockIndexBuffer(ITF_IndexBuffer* _indexBuffer, u32 size)
{
}

void GFXAdapter_3DS_GR::setIndexBuffer( ITF_IndexBuffer* _indexBuffer )
{    
    m_indexBuffer.physicalAddr = nngxGetPhysicalAddr( (uptr)( _indexBuffer->m_Id ) );
    m_indexBuffer.drawVtxNum = _indexBuffer->m_nIndices;
    m_indexBuffer.isUnsignedByte = false;
}

void GFXAdapter_3DS_GR::setIndexBuffer(ITF_IndexBuffer* _indexBuffer, u32 _startIdx, u32 _nIdx)
{
    u16* startAddr = (u16*)_indexBuffer->m_Id;
    startAddr = startAddr + _startIdx;
    m_indexBuffer.physicalAddr = nngxGetPhysicalAddr( (uptr)( startAddr ) );
    m_indexBuffer.drawVtxNum = _nIdx;
    m_indexBuffer.isUnsignedByte = false;
}

///----------------------------------------------------------------------------//
/// Draw Vertex/IndexBuffer.
///----------------------------------------------------------------------------//

void  GFXAdapter_3DS_GR::DrawVertexBuffer( u32 _type, ITF_VertexBuffer* _vertexBuffer, u32 _vertexStart, u32 _vertexNumber )
{
	GFX_ASSERT(_vertexStart + _vertexNumber < MAX_INDICES, "You can't draw a non indexed vertex buffer with more than %d vertices", MAX_INDICES);
    DrawIndexedVertexBuffer(_type, _vertexBuffer, &m_defaultIndexBuffer, _vertexStart, _vertexNumber);
}

void GFXAdapter_3DS_GR::DrawIndexedVertexBuffer( u32 _type, ITF_VertexBuffer* _vertexBuffer, ITF_IndexBuffer* _indexBuffer, u32 _indexNumber )
{
    DrawIndexedVertexBuffer(_type, _vertexBuffer, _indexBuffer, 0, _indexNumber);
}

void GFXAdapter_3DS_GR::DrawIndexedVertexBuffer(u32 _type, ITF_VertexBuffer* _vertexBuffer, ITF_IndexBuffer* _indexBuffer, u32 _vertexStart, u32 _indexNumber)
{
    switch(_type)
    {
    default:
    case GFX_TRIANGLES: 
        break;
    case GFX_POINTS:
        GFX_NOT_IMPLEMENTED(DrawIndexedVertexBuffer(GFX_POINTS));
        break;
    case GFX_LINES:
        GFX_NOT_IMPLEMENTED(DrawIndexedVertexBuffer(GFX_LINES));
        break;
    case GFX_LINE_STRIP:
        GFX_NOT_IMPLEMENTED(DrawIndexedVertexBuffer(GFX_LINE_STRIP));
        break;
    case GFX_TRIANGLE_STRIP:
        GFX_NOT_IMPLEMENTED(DrawIndexedVertexBuffer(GFX_TRIANGLE_STRIP));
        break;
    case GFX_TRIANGLE_FAN:
        GFX_NOT_IMPLEMENTED(DrawIndexedVertexBuffer(GFX_TRIANGLE_FAN));
        break;
    }    
    switchToDefautlShader();
    
    UpdateGPUState();
    
    UpdateGPUWorldView();

    setVertexBuffer(_vertexBuffer);
    setIndexBuffer(_indexBuffer, _vertexStart, _indexNumber);
    
    m_commandCursor = m_currentBuffer->MakeDrawCommand(m_commandCursor, m_indexBuffer);
}

void GFXAdapter_3DS_GR::copyCurrentColorBuffer(u32 _rt)
{
	GFX_NOT_IMPLEMENTED(copyCurrentColorBuffer);
}

///----------------------------------------------------------------------------//
/// Shader Patch.
///----------------------------------------------------------------------------//

void GFXAdapter_3DS_GR::prepareShaderPatch(GMatrix44* _matrix, f32 _z, u32 & _hdiv, u32 & _vdiv, GFX_BLENDMODE _blendMode)
{
	selectTechnique(DEFAULT_TECH3);
    
    switchToDefautlShader();

    SetWorldViewMatrix(_matrix);

    ITF_IndexBuffer *  indexBuffer  = getIndexBuffer(_hdiv, _vdiv);
    ITF_VertexBuffer * vertexBuffer = getVertexBuffer(_hdiv, _vdiv);
    GFX_ASSERT(indexBuffer && vertexBuffer, "...");

    setVertexBuffer(vertexBuffer);
    setIndexBuffer(indexBuffer);

	m_shaderManager.setCurBuffFloatPos(0, 0);
	m_shaderManager.setVector4ConstToFlush(0, 0);
	m_shaderManager.setCurBuffFloatPos(1, 0);
	m_shaderManager.setVector4ConstToFlush(1, 0);

	/// set pos z to const.
	//mp_currentShader->setFloat(1, _z);
    m_commandCursor = m_defaultUniformConst2.MakeUniformCommand(m_commandCursor, nn::math::Vector4(_z, 0.0f, 0.0f, 0.0f));
    UpdateGPUWorldView();
}

void GFXAdapter_3DS_GR::shaderDrawPatch32(const Texture *texture, const Vec2d* PointsNS, const Color* _lighttab)
{
	ITF_ASSERT(_lighttab);

	f32 ratio = 1.f;

	if (texture)
	{
		SetTexture(0, texture);
		ratio = (f32)texture->getSizeX() / (f32)texture->getSizeY();
		setTextureAdressingMode(0, GFXTADDRESS_WRAP, GFXTADDRESS_WRAP);
        UpdateGPUState();
	}

	/// The stride of float Buffer is:
	/// Vector2 PointsNS for patch Vertex, Vector2 PointsNS+16 that is for UV.
	/// this is for Vector4 shader: xy = vertex zw = uv.
	for (u32 i = 0; i < 8; i++)
	{
		m_shaderManager.setBufferFloatAt(0, m_shaderManager.getCurBuffFloatPos(0), PointsNS[i].m_x);
		m_shaderManager.addCurBuffFloatPos(0, 1);
		m_shaderManager.setBufferFloatAt(0, m_shaderManager.getCurBuffFloatPos(0), PointsNS[i].m_y);
		m_shaderManager.addCurBuffFloatPos(0, 1);
		m_shaderManager.setBufferFloatAt(0, m_shaderManager.getCurBuffFloatPos(0), PointsNS[i + 8].m_x);
		m_shaderManager.addCurBuffFloatPos(0, 1);
		m_shaderManager.setBufferFloatAt(0, m_shaderManager.getCurBuffFloatPos(0), PointsNS[i + 8].m_y * ratio);
		m_shaderManager.addCurBuffFloatPos(0, 1);
	}

	/// number constant for control points is: f32 to vector4 -> 32 float / 4
	m_shaderManager.addVector4ConstToFlush(0, 8);

	/// color.
	for (u32 i = 0; i < 4; i++)
	{
		m_shaderManager.setBufferFloatAt(1, m_shaderManager.getCurBuffFloatPos(1), _lighttab[i].getRed());
		m_shaderManager.addCurBuffFloatPos(1, 1);
		m_shaderManager.setBufferFloatAt(1, m_shaderManager.getCurBuffFloatPos(1), _lighttab[i].getGreen());
		m_shaderManager.addCurBuffFloatPos(1, 1);
		m_shaderManager.setBufferFloatAt(1, m_shaderManager.getCurBuffFloatPos(1), _lighttab[i].getBlue());
		m_shaderManager.addCurBuffFloatPos(1, 1);
		m_shaderManager.setBufferFloatAt(1, m_shaderManager.getCurBuffFloatPos(1), _lighttab[i].getAlpha());
		m_shaderManager.addCurBuffFloatPos(1, 1);
	}

	m_shaderManager.addVector4ConstToFlush(1, 4);
}

void GFXAdapter_3DS_GR::shaderPatchFlush(u32 _hdiv, u32 _vdiv, u32 _pass)
{
	GLsizei pointsNum = m_shaderManager.getNumberVector4ConstToFlush(0);
	GLsizei colorsNum = m_shaderManager.getNumberVector4ConstToFlush(1);

	if (!pointsNum) return;

    m_commandCursor = m_defaultUniformArray0.MakeUniformCommand(m_commandCursor, (nn::math::Vector4*)m_shaderManager.getBufferFloat(0), pointsNum);
    m_commandCursor = m_defaultUniformArray1.MakeUniformCommand(m_commandCursor, (nn::math::Vector4*)m_shaderManager.getBufferFloat(1), colorsNum);

	/// draw patch.
	u32 nb = (pointsNum/8);
    
    m_indexBuffer.drawVtxNum = (_hdiv*_vdiv*2)*nb*3;

    m_commandCursor = m_currentBuffer->MakeDrawCommand(m_commandCursor, m_indexBuffer);

	m_shaderManager.setCurBuffFloatPos(0, 0);
	m_shaderManager.setVector4ConstToFlush(0, 0);
	m_shaderManager.setCurBuffFloatPos(1, 0);
	m_shaderManager.setVector4ConstToFlush(1, 0);
}

void GFXAdapter_3DS_GR::endShaderPatch()
{
	m_shaderManager.setCurBuffFloatPos(0, 0);
	m_shaderManager.setVector4ConstToFlush(0, 0);
	m_shaderManager.setCurBuffFloatPos(1, 0);
	m_shaderManager.setVector4ConstToFlush(1, 0);
}

///----------------------------------------------------------------------------//
/// Shader Bezier.
///----------------------------------------------------------------------------//

void GFXAdapter_3DS_GR::prepareShaderBezierPatch(GMatrix44* _matrix, f32 _z, u32 & _hdiv, u32 & _vdiv, GFX_BLENDMODE _blendMode)
{

}

bbool GFXAdapter_3DS_GR::shaderDrawBezierPatch32(Texture *texture, Vec2d *PointsNS, Color* _lighttab, f32* _widths, f32* _alphas)
{
    return btrue;
}

void GFXAdapter_3DS_GR::shaderBezierPatchFlush (u32 _hdiv, u32 _vdiv, u32 _pass )
{

}

///----------------------------------------------------------------------------//
/// Geometry Patch stuff.
///----------------------------------------------------------------------------//

bool GFXAdapter_3DS_GR::prepareGeomShaderPatch(GMatrix44* _matrix, AnimBoneRenderTable* _bones, f32 _z, u32 _hdiv, u32 _vdiv, bbool _mirror)
{
    u32 _boneCount = _bones->m_maxRenderableBoneIdx + 1;
    if (_boneCount < 1 || _boneCount > MAX_BONES_PER_ANIM_BATCH)
    {
        return false;
    }

    m_selectedTech = PATCH_TECH;
    
    switchToPatchShader();

    SetWorldViewMatrix(_matrix);
    UpdateGPUWorldViewGeo();

    m_commandCursor = m_patchUniformBonesP1.MakeUniformCommand(m_commandCursor, (nn::math::Vector4*)_bones->m_tableP1, _boneCount);
    m_commandCursor = m_patchUniformBonesP2.MakeUniformCommand(m_commandCursor, (nn::math::Vector4*)_bones->m_tableP2, _boneCount);
    
    m_saveCurrentPatchMirror = _mirror ? -1.0f : 1.0f;
    nn::math::Vector4 currentZ(_z, 0.0f, 0.0f, 0.0f);
    m_commandCursor = m_patchUniformZ.MakeUniformCommand(m_commandCursor, currentZ);
    
    return true;
}


bbool GFXAdapter_3DS_GR::drawAnimatedPatches(ITF_Mesh& _mesh)
{
    if (_mesh.m_vtxBuffer->m_nVertex < 4)
        return btrue;
    
    setCullMode(GFX_CULL_NONE);

    for (u32 i = 0; i < _mesh.m_ElementList.size(); i++)
    {
        if ( _mesh.m_ElementList[i].m_material.m_textureDiffuse  == ResourceID::Invalid || _mesh.m_ElementList[i].m_count == 0)
            continue;

        Texture* texture = (Texture*)_mesh.m_ElementList[i].m_material.m_textureDiffuse.getResource();
        f32 ratio = (f32)texture->getSizeX() / (f32)texture->getSizeY();

        nn::math::Vector4 currentMirrorRatio(m_saveCurrentPatchMirror, ratio, 0.0f, 0.0f);
        m_commandCursor = m_patchUniformMirrorRatio.MakeUniformCommand(m_commandCursor, currentMirrorRatio);

        setGfxMaterial(_mesh.m_ElementList[i].m_material);
        UpdateGPUState();

        setVertexBuffer(_mesh.m_vtxBuffer);
        setIndexBuffer(_mesh.m_ElementList[i].m_indexBuffer, _mesh.m_ElementList[i].m_startIndex, _mesh.m_ElementList[i].m_count);
        
        m_commandCursor = m_currentBuffer->MakeDrawCommand(m_commandCursor, m_indexBuffer);
    }

    return btrue;
}

void GFXAdapter_3DS_GR::endGeomShaderPatch()
{

}

///----------------------------------------------------------------------------//
/// After FX.
///----------------------------------------------------------------------------//

void GFXAdapter_3DS_GR::copyTexture(int *TexHan)
{
	GFX_NOT_IMPLEMENTED(copyTexture);
}

///----------------------------------------------------------------------------//

void GFXAdapter_3DS_GR::validateTexture(i32 *TexHan, i32 Mode,i32 ScreenSize,i32 ScreenSize_V)
{
	GFX_NOT_IMPLEMENTED(validateTexture);
}


///----------------------------------------------------------------------------//
/// Shader Fluid.
///----------------------------------------------------------------------------//
void GFXAdapter_3DS_GR::drawFluid(   GMatrix44* _matrix, Texture* _texture, const FixedArray<u32, GFXADAPTER_FLUID_COLORTAB_ENTRY_COUNT> &_colorTab, const Vec2d _heightTab[], const u32 _heightTabSize, const FixedArray<Vec2d, 4> &_uvTab, u32 mode, f32 _delta1, f32 _delta2, f32 _stepCount  )
{
	selectTechnique(DEFAULT_TECH6);
	
	if (_texture)
	{
        SetCombinerConfigFromTech();
		SetTexture(0, _texture);
		setTextureAdressingMode(0, GFXTADDRESS_WRAP, GFXTADDRESS_WRAP);
	}
	
	
	u32 hdiv = MESH_DEFAULT_HDIV;
	u32 vdiv = MESH_DEFAULT_VDIV;

	ITF_IndexBuffer *  indexBuffer  = getIndexBuffer(hdiv, vdiv);
	ITF_VertexBuffer * vertexBuffer = getVertexBuffer(hdiv, vdiv);
	ITF_ASSERT(indexBuffer && vertexBuffer);
	
	/// set pos z to const.
	GFX_Vector4 v1;
	GFX_Vector4 v2;

	v1.m_x = _uvTab[0].m_x;
	v1.m_y = _uvTab[0].m_y;
	v1.m_z = _uvTab[1].m_x;
	v1.m_w = _uvTab[1].m_y;
	v2.m_x = _uvTab[2].m_x;
	v2.m_y = _uvTab[2].m_y;
	v2.m_z = _uvTab[3].m_x;
	v2.m_w = _uvTab[3].m_y;

	static GFX_Vector4 colorVect[4];
	for (int i=0; i<4; i++)
	{
		Color           color(_colorTab[i]);
		GFX_Vector4     &colorVectElmt = colorVect[i];
		colorVectElmt.m_x = color.getRed(); 
		colorVectElmt.m_y = color.getGreen();
		colorVectElmt.m_z = color.getBlue();
		colorVectElmt.m_w = color.getAlpha();
	}


	static GFX_Vector4 bufferVect[24 + 2];

	int hdivCount = 0;
	int heightTabSize   = _heightTabSize;
	int rest            = (heightTabSize % 2);
	int vLen            = heightTabSize - 1;
	if (rest)
		heightTabSize--;
	for (int i=0; i<(int)heightTabSize;hdivCount++)
	{
		bufferVect[hdivCount].m_x = _heightTab[i].m_x;
		bufferVect[hdivCount].m_y = _heightTab[i++].m_y;
		bufferVect[hdivCount].m_z = _heightTab[i].m_x;
		bufferVect[hdivCount].m_w = _heightTab[i++].m_y;
	}
	if (rest)
	{
		bufferVect[hdivCount].m_x = _heightTab[heightTabSize].m_x;
		bufferVect[hdivCount].m_y = _heightTab[heightTabSize].m_y;
		bufferVect[hdivCount].m_z = 0.f;
		bufferVect[hdivCount].m_w = 0.f;
		heightTabSize++;
		hdivCount++;
	}

    SetWorldViewMatrix(_matrix);

	/// Flush control Points.
    m_commandCursor = m_defaultUniformArray0.MakeUniformCommand(m_commandCursor, (nn::math::VEC4*)bufferVect, hdivCount);
    m_commandCursor = m_defaultUniformArray1.MakeUniformCommand(m_commandCursor, (nn::math::VEC4*)colorVect, 4);
    m_commandCursor = m_defaultUniformConst0.MakeUniformCommand(m_commandCursor, (nn::math::VEC4*)&v1, 1);
    m_commandCursor = m_defaultUniformConst1.MakeUniformCommand(m_commandCursor, (nn::math::VEC4*)&v2, 1);

    nn::math::Vector4 constant((f32)vdiv, (f32)vdiv/(f32)vLen, 0.0f, 0.0f);
	m_commandCursor = m_defaultUniformConst2.MakeUniformCommand(m_commandCursor, constant);
    
    m_useFluid2 = (!mode) ? true : false;

    DrawIndexedVertexBuffer(GFX_TRIANGLES, vertexBuffer, indexBuffer, (hdiv*vLen*2)*3);
}

///----------------------------------------------------------------------------//
/// Shader Trail.
///----------------------------------------------------------------------------//
void GFXAdapter_3DS_GR::drawTrail3D(Texture* _texture, Color _color, const SafeArray<Vec3d> & _points, f32 _alphaBegin, f32 _alphaEnd, f32 _fadeLength )
{
    if (_points.size() < 3 || _points.size() > 48)
        return;
    
    switchToDefautlShader();

	selectTechnique(DEFAULT_TECH7);

	/// set Texture;
	if (_texture)
	{
        SetCombinerConfigFromTech();
		SetTexture(0, _texture);
		setTextureAdressingMode(0, GFXTADDRESS_WRAP, GFXTADDRESS_WRAP);
	}

	u32 hdiv = MESH_DEFAULT_HDIV;
	u32 vdiv = MESH_DEFAULT_VDIV;
	ITF_IndexBuffer* indexBuffer  = getIndexBuffer(hdiv, vdiv);
	ITF_VertexBuffer* vertexBuffer = getVertexBuffer(hdiv, vdiv);
	GFX_ASSERT(indexBuffer && vertexBuffer, "\n");

	/// set pos z to const.
	GFX_Vector4 v1(1.f, 1.f, 1.f, _alphaBegin);
	GFX_Vector4 v2(1.f, 1.f, 1.f, _alphaEnd);

	static GFX_Vector4 bufferVectXY[48];
	u32         bufferVectXYCount = 0;
	int         idx;
	u32         nbPoints = _points.size();

	for (idx = 0; idx<(int)nbPoints; bufferVectXYCount++)
	{
		bufferVectXY[bufferVectXYCount].m_x = _points[idx].m_x;
		bufferVectXY[bufferVectXYCount].m_y = _points[idx++].m_y;
		bufferVectXY[bufferVectXYCount].m_z = _points[idx].m_x;
		bufferVectXY[bufferVectXYCount].m_w = _points[idx++].m_y;
	}

	u32 count = nbPoints;
	u32 rest  = nbPoints % 8;
	if (rest > 0)
		count -= rest;

	GFX_Vector4 bufferVectZ[24];
	u32         bufferVectZCount = 0;
	for (idx = 0; idx<(int)count; bufferVectZCount++)
	{
		bufferVectZ[bufferVectZCount].m_x = _points[idx].m_z; idx += 2;
		bufferVectZ[bufferVectZCount].m_y = _points[idx].m_z; idx += 2;
		bufferVectZ[bufferVectZCount].m_z = _points[idx].m_z; idx += 2;
		bufferVectZ[bufferVectZCount].m_w = _points[idx].m_z; idx += 2;
	}
	if (rest > 0)
	{
		bufferVectZ[bufferVectZCount].m_x = _points[idx].m_z; idx += 2;
		rest -= 2;
	}
	if (rest > 0)
	{
		bufferVectZ[bufferVectZCount].m_y = _points[idx].m_z; idx += 2;
		rest -= 2;
	}
	if (rest > 0)
	{
		bufferVectZ[bufferVectZCount].m_z = _points[idx].m_z; idx += 2;
		rest -= 2;
	}
	if (count != nbPoints)
		bufferVectZCount++;

    SetWorldViewMatrix(&GMatrixIdentity);
    UpdateGPUWorldView();

    m_commandCursor = m_defaultUniformArray0.MakeUniformCommand(m_commandCursor, (nn::math::VEC4*)bufferVectXY, bufferVectXYCount);
    m_commandCursor = m_defaultUniformArray1.MakeUniformCommand(m_commandCursor, (nn::math::VEC4*)bufferVectZ, bufferVectZCount);
    m_commandCursor = m_defaultUniformConst0.MakeUniformCommand(m_commandCursor, (nn::math::VEC4*)&v1, 1);
    m_commandCursor = m_defaultUniformConst1.MakeUniformCommand(m_commandCursor, (nn::math::VEC4*)&v2, 1);

    nn::math::Vector4 constant((f32)vdiv, (f32)vdiv/(f32)bufferVectXYCount, _fadeLength, (_fadeLength > 0.f) ? 0.5f / _fadeLength : 0.0f);
    m_commandCursor = m_defaultUniformConst2.MakeUniformCommand(m_commandCursor, constant);

    DrawIndexedVertexBuffer(GFX_TRIANGLES, vertexBuffer, indexBuffer, (hdiv*(nbPoints/2-1)*2)*3);
}

///----------------------------------------------------------------------------//


void GFXAdapter_3DS_GR::AFTERFX_startRenderTarget(Color _color)
{
	GFX_NOT_IMPLEMENTED(AFTERFX_startRenderTarget);
}

void GFXAdapter_3DS_GR::prepareAfterFx(bbool _nocopy)
{
	GFX_NOT_IMPLEMENTED(prepareAfterFx);
}

void GFXAdapter_3DS_GR::endAfterfx(bbool _nocopy, GFX_BLENDMODE _finalBlend)
{
	GFX_NOT_IMPLEMENTED(endAfterfx);
}

void GFXAdapter_3DS_GR::AFTERFX_draw(AFX_Group* _group)
{
	GFX_NOT_IMPLEMENTED(AFTERFX_draw);
}

void GFXAdapter_3DS_GR::AFTERFX_BigBlur(f32 _factor, u32 _MaxPass, bbool _clearRT, GFX_QUALITY _quality)
{
	GFX_NOT_IMPLEMENTED(AFTERFX_BigBlur);
}

///----------------------------------------------------------------------------//

void GFXAdapter_3DS_GR::AFTERFX_Remanence(f32 _factor, f32 _glowfactor, f32 _addalpha)
{
	GFX_NOT_IMPLEMENTED(AFTERFX_Remanence);
}

///----------------------------------------------------------------------------//

void GFXAdapter_3DS_GR::AFTERFX_RayCenter( f32 _v1, f32 _v2, f32 _v3, f32 _v4, f32 _v5, f32 _v6, f32 _v7)
{
	GFX_NOT_IMPLEMENTED(AFTERFX_RayCenter);
}

///----------------------------------------------------------------------------//

void GFXAdapter_3DS_GR::AFTERFX_Glow(f32 _factor, f32 _glowfactor, f32 _addalpha )
{
	GFX_NOT_IMPLEMENTED(AFTERFX_Glow);
}

void GFXAdapter_3DS_GR::AFTERFX_objectsGlow(f32 _factor )
{
	GFX_NOT_IMPLEMENTED(AFTERFX_objectsGlow);
}

void GFXAdapter_3DS_GR::AFTERFX_prepareObjectGlow()
{
	GFX_NOT_IMPLEMENTED(AFTERFX_prepareObjectGlow);
}

void GFXAdapter_3DS_GR::AFTERFX_ColorSetting(f32 _sat, f32 _contrast, f32 _contrastscale, f32 _bright)
{
	GFX_NOT_IMPLEMENTED(AFTERFX_ColorSetting);
}

void GFXAdapter_3DS_GR::AFTERFX_ColorRemap(Texture* _tex)
{
	GFX_NOT_IMPLEMENTED(AFTERFX_ColorRemap);
}

void GFXAdapter_3DS_GR::AFTERFX_ColorLevels(f32 _inBlack,  f32 _inGamma, f32 _inWhite, f32 _outBlack, f32 _outWhite)
{
	GFX_NOT_IMPLEMENTED(AFTERFX_ColorLevels);

}

void GFXAdapter_3DS_GR::AFTERFX_addSceneAndMul(f32 _mul)
{
	GFX_NOT_IMPLEMENTED(AFTERFX_addSceneAndMul);
}

void GFXAdapter_3DS_GR::AFTERFX_byTech(u32 _tech)
{
	GFX_NOT_IMPLEMENTED(AFTERFX_byTech);
}

void GFXAdapter_3DS_GR::AFTERFX_byTechParams(u32 _tech, f32 _p0, f32 _p1, f32 _p2, f32 _p3)
{
	GFX_NOT_IMPLEMENTED(AFTERFX_byTechParams);
}

///----------------------------------------------------------------------------//
/// 
///----------------------------------------------------------------------------//

void GFXAdapter_3DS_GR::setFogColor(const Color& )
{

}

///----------------------------------------------------------------------------//
/// UV.
///----------------------------------------------------------------------------//

void GFXAdapter_3DS_GR::setUVAnim(GFX_UVANIM& _uvanim)
{
    m_UVmat = nn::math::Matrix44(_uvanim.m_UVMat.fa16);
    m_UVmat.Transpose();
}

///----------------------------------------------------------------------------//

void GFXAdapter_3DS_GR::project(const Vec3d& _in3d, Vec2d & _out2d)
{
	GFX_NOT_IMPLEMENTED(project);
}

f32 GFXAdapter_3DS_GR::ReadSurfaceFloat(void* _surf, u32 _posX, u32 _posY)
{
	GFX_NOT_IMPLEMENTED(ReadSurfaceFloat);

	/// TODO:
	return 9999.f;
}

void GFXAdapter_3DS_GR::startImpostorRendering(renderTarget* _rt, f32 xmin, f32 ymin, f32 xmax, f32 ymax, bbool _mustClear)
{
	GFX_NOT_IMPLEMENTED(startImpostorRendering);
}

void GFXAdapter_3DS_GR::stopImpostorRendering(renderTarget* _rt)
{
	GFX_NOT_IMPLEMENTED(stopImpostorRendering);
}

///----------------------------------------------------------------------------//
/// Render Context.
///----------------------------------------------------------------------------//
void GFXAdapter_3DS_GR::renderContext_Set()
{
	if (rC_getZWRITE())
		depthMask(1);
	if (rC_getZWRITE())
		depthTest(1);
}

void GFXAdapter_3DS_GR::renderContext_SetDefault()
{
	depthMask(0);
	depthTest(0);
	depthFunc(GFX_CMP_LESSEQUAL);
}

///----------------------------------------------------------------------------//
/// Gfx Material.
///----------------------------------------------------------------------------//
void GFXAdapter_3DS_GR::setGfxMaterial( const GFX_MATERIAL& _gfxMat )
{
    switch (_gfxMat.m_matType)
    {
    default:
    case GFX_MAT_DEFAULT:
        break;
    case GFX_MAT_FRIEZEANIM:
        {
            nn::math::Vector4 vec4(_gfxMat.m_matParams.getvParamsAt(0).m_x, _gfxMat.m_matParams.getvParamsAt(0).m_y, _gfxMat.m_matParams.getvParamsAt(0).m_z, _gfxMat.m_matParams.getvParamsAt(0).m_w);
            m_commandCursor = m_defaultUniformConst0.MakeUniformCommand(m_commandCursor, vec4);
        }
        break;
    case GFX_MAT_REFRACTION:
        break;

    case GFX_MAT_GLOW:
        break;

    case GFX_MAT_ALPHAFADE:
        break;
    }

    SetTextureResource(0, _gfxMat.m_textureDiffuse);
    setTextureAdressingMode(0, _gfxMat.m_TexAdressingModeU, _gfxMat.m_TexAdressingModeV);

    /// set alpha blend.
    setAlphaBlend(_gfxMat.m_blendMode);

    SetCombinerConfigFromTech();
}

void GFXAdapter_3DS_GR::SetCombinerConfigFromTech()
{
    switch (m_selectedTech)
    {
    case DEFAULT_TECH0:
    case DEFAULT_TECH3:
    case DEFAULT_TECH4:
    case DEFAULT_TECH5:
    case DEFAULT_TECH6:
    case DEFAULT_TECH7:
    case PATCH_TECH:
        SetCombinerConfig(CombinerConfig_PCT);
        break;
    case DEFAULT_TECH1:
        SetCombinerConfig(CombinerConfig_PC);
        break;
    case DEFAULT_TECH2:
        SetCombinerConfig(CombinerConfig_PT);
        break;
    default:
        break;
    }
}

void GFXAdapter_3DS_GR::SetCombinerConfig(GFXAdapter_3DS_GR::CombinerConfig config)
{
    switch (config)
    {
    case CombinerConfig_PCT:
        if (m_previousCombinerConfig == CombinerConfig_PCT)
        {
            return;
        }
        m_previousCombinerConfig = CombinerConfig_PCT;
        m_combiner.stage[ 0 ].rgb.combine = PICA_DATA_TEX_ENV_COMBINE_MODULATE;
        m_combiner.stage[ 0 ].rgb.source[0] = PICA_DATA_TEX_ENV_SRC_RGBA_TEXTURE0;
        m_combiner.stage[ 0 ].rgb.source[1] = PICA_DATA_TEX_ENV_SRC_RGBA_CONSTANT;
        m_combiner.stage[ 0 ].alpha.combine = PICA_DATA_TEX_ENV_COMBINE_MODULATE;
        m_combiner.stage[ 0 ].alpha.source[0] = PICA_DATA_TEX_ENV_SRC_RGBA_TEXTURE0;
        m_combiner.stage[ 0 ].alpha.source[1] = PICA_DATA_TEX_ENV_SRC_RGBA_CONSTANT;

        m_combiner.stage[ 1 ].rgb.combine = PICA_DATA_TEX_ENV_COMBINE_MODULATE;
        m_combiner.stage[ 1 ].rgb.source[0] = PICA_DATA_TEX_ENV_SRC_RGBA_PREVIOUS;
        m_combiner.stage[ 1 ].rgb.source[1] = PICA_DATA_TEX_ENV_SRC_RGBA_PRIMARY_COLOR;
        m_combiner.stage[ 1 ].alpha.combine = PICA_DATA_TEX_ENV_COMBINE_MODULATE;
        m_combiner.stage[ 1 ].alpha.source[0] = PICA_DATA_TEX_ENV_SRC_RGBA_PREVIOUS;
        m_combiner.stage[ 1 ].alpha.source[1] = PICA_DATA_TEX_ENV_SRC_RGBA_PRIMARY_COLOR;
        break;
    case CombinerConfig_PC:
        if (m_previousCombinerConfig == CombinerConfig_PC)
        {
            return;
        }
        m_previousCombinerConfig = CombinerConfig_PC;
        m_combiner.stage[ 0 ].rgb.combine = PICA_DATA_TEX_ENV_COMBINE_MODULATE;
        m_combiner.stage[ 0 ].rgb.source[0] = PICA_DATA_TEX_ENV_SRC_RGBA_CONSTANT;
        m_combiner.stage[ 0 ].rgb.source[1] = PICA_DATA_TEX_ENV_SRC_RGBA_PRIMARY_COLOR;
        m_combiner.stage[ 0 ].alpha.combine = PICA_DATA_TEX_ENV_COMBINE_MODULATE;
        m_combiner.stage[ 0 ].alpha.source[0] = PICA_DATA_TEX_ENV_SRC_RGBA_CONSTANT;
        m_combiner.stage[ 0 ].alpha.source[1] = PICA_DATA_TEX_ENV_SRC_RGBA_PRIMARY_COLOR;
        m_combiner.stage[ 1 ].SetupPrevious();

        break;
    case CombinerConfig_PT:
        if (m_previousCombinerConfig == CombinerConfig_PT)
        {
            return;
        }
        m_previousCombinerConfig = CombinerConfig_PT;
        m_combiner.stage[ 0 ].rgb.combine = PICA_DATA_TEX_ENV_COMBINE_MODULATE;
        m_combiner.stage[ 0 ].rgb.source[0] = PICA_DATA_TEX_ENV_SRC_RGBA_TEXTURE0;
        m_combiner.stage[ 0 ].rgb.source[1] = PICA_DATA_TEX_ENV_SRC_RGBA_CONSTANT;
        m_combiner.stage[ 0 ].alpha.combine = PICA_DATA_TEX_ENV_COMBINE_MODULATE;
        m_combiner.stage[ 0 ].alpha.source[0] = PICA_DATA_TEX_ENV_SRC_RGBA_TEXTURE0;
        m_combiner.stage[ 0 ].alpha.source[1] = PICA_DATA_TEX_ENV_SRC_RGBA_CONSTANT;
        m_combiner.stage[ 1 ].SetupPrevious();
        break;
    case CombinerConfig_Font:
        if (m_previousCombinerConfig == CombinerConfig_Font)
        {
            return;
        }
        m_combiner.stage[ 0 ].rgb.combine = PICA_DATA_TEX_ENV_COMBINE_MODULATE;
        m_combiner.stage[ 0 ].rgb.source[0] = PICA_DATA_TEX_ENV_SRC_RGBA_TEXTURE0;
        m_combiner.stage[ 0 ].rgb.source[1] = PICA_DATA_TEX_ENV_SRC_RGBA_PRIMARY_COLOR;
        m_combiner.stage[ 0 ].alpha.combine = PICA_DATA_TEX_ENV_COMBINE_MODULATE;
        m_combiner.stage[ 0 ].alpha.source[0] = PICA_DATA_TEX_ENV_SRC_RGBA_TEXTURE0;
        m_combiner.stage[ 0 ].alpha.source[1] = PICA_DATA_TEX_ENV_SRC_RGBA_PRIMARY_COLOR;
        m_combiner.stage[ 1 ].SetupPrevious();

        m_previousCombinerConfig = CombinerConfig_Font;
        break;

    }
    m_isCombinerStateDirty = true;
}

void GFXAdapter_3DS_GR::setGfxMatDefault( )
{
	GFX_NOT_IMPLEMENTED(setGfxMatDefault);
}

void GFXAdapter_3DS_GR::setGfxMatRefraction( const GFX_MATERIAL& _gfxMat )
{
	GFX_NOT_IMPLEMENTED(setGfxMatRefraction);
}

///----------------------------------------------------------------------------//
/// Light.
///----------------------------------------------------------------------------//

void GFXAdapter_3DS_GR::setGlobalColor(const Color& _color)
{
	setGFXGlobalColor(_color);
    m_combiner.stage[0].constColorR = getGFXGlobalColor().getRed() * 255;
    m_combiner.stage[0].constColorG = getGFXGlobalColor().getGreen() * 255;
    m_combiner.stage[0].constColorB = getGFXGlobalColor().getBlue() * 255;
    m_combiner.stage[0].constColorA = getGFXGlobalColor().getAlpha() * 255;  

    m_isConstColor0Dirty = true;
}

const GMatrix44*  GFXAdapter_3DS_GR::getViewMatrix()
{
	return reinterpret_cast<const GMatrix44*>(&m_view);
}

void GFXAdapter_3DS_GR::setShaderForFont(bbool bOutline)
{
    selectTechnique(DEFAULT_TECH0);

    setAlphaBlend(GFX_BLEND_ALPHA);

    SetCombinerConfig(CombinerConfig_Font);

    if (!m_stereoMode)
    {
        m_worldView = m_view;
        m_worldView.Transpose();
    }
    else
    {
        m_worldViewR = m_viewR;
        m_worldViewR.Transpose();

        m_worldViewL = m_viewL;
        m_worldViewL.Transpose();
    }
}

void GFXAdapter_3DS_GR::restoreDefaultShader()
{

}

void GFXAdapter_3DS_GR::TransferBuffer(const u32 targetDisplay, const bool isAddSplitDrawCmdlist)
{
    m_commandList.Wait();
    SwapBuffers(targetDisplay);
    
    GFXMEM::FlushDevMemContent();

    m_commandList.Run();
    switch( targetDisplay )
    {
    case EXTENSION_SCREEN | UPPER_SCREEN | LOWER_SCREEN:
        m_displayBuffers0->TransferBuffer(&m_frameBuffer0.colorBuffer, isAddSplitDrawCmdlist);
        m_displayBuffers1->TransferBuffer(&m_frameBuffer1.colorBuffer, isAddSplitDrawCmdlist);
        if (m_stereoActive)
        {
            m_displayBuffers0Ext->TransferBuffer(&m_frameBuffer0Ext.colorBuffer, isAddSplitDrawCmdlist);
        }
        else
        {
            m_displayBuffers0Ext->TransferBuffer(&m_frameBuffer0.colorBuffer, isAddSplitDrawCmdlist);
        }
        break;

    case EXTENSION_SCREEN | UPPER_SCREEN:
        m_displayBuffers0->TransferBuffer(&m_frameBuffer0.colorBuffer, isAddSplitDrawCmdlist);
        if (m_stereoActive)
        {
            m_displayBuffers0Ext->TransferBuffer(&m_frameBuffer0Ext.colorBuffer, isAddSplitDrawCmdlist);
        }
        else
        {
            m_displayBuffers0Ext->TransferBuffer(&m_frameBuffer0.colorBuffer, isAddSplitDrawCmdlist);
        }
        break;

    case EXTENSION_SCREEN | LOWER_SCREEN:
        m_displayBuffers1->TransferBuffer(&m_frameBuffer1.colorBuffer, isAddSplitDrawCmdlist);
        if (m_stereoActive)
        {
            m_displayBuffers0Ext->TransferBuffer(&m_frameBuffer0Ext.colorBuffer, isAddSplitDrawCmdlist);
        }
        else
        {
            m_displayBuffers0Ext->TransferBuffer(&m_frameBuffer0.colorBuffer, isAddSplitDrawCmdlist);
        }
        break;

    case EXTENSION_SCREEN:
        if (m_stereoActive)
        {
            m_displayBuffers0Ext->TransferBuffer(&m_frameBuffer0Ext.colorBuffer, isAddSplitDrawCmdlist);
        }
        else
        {
            m_displayBuffers0Ext->TransferBuffer(&m_frameBuffer0.colorBuffer, isAddSplitDrawCmdlist);
        }
        break;

    case UPPER_SCREEN | LOWER_SCREEN:
        m_displayBuffers0->TransferBuffer(&m_frameBuffer0.colorBuffer, isAddSplitDrawCmdlist);
        m_displayBuffers1->TransferBuffer(&m_frameBuffer1.colorBuffer, isAddSplitDrawCmdlist);
        break;

    case UPPER_SCREEN:
        m_displayBuffers0->TransferBuffer(&m_frameBuffer0.colorBuffer, isAddSplitDrawCmdlist);
        break;

    case LOWER_SCREEN:
        m_displayBuffers1->TransferBuffer(&m_frameBuffer1.colorBuffer, isAddSplitDrawCmdlist);
        break;
    }
}

void GFXAdapter_3DS_GR::SwapBuffers(const u32 targetDisplay)
{
    switch( targetDisplay )
    {
    case EXTENSION_SCREEN | UPPER_SCREEN | LOWER_SCREEN:
        m_displayBuffers0->ActivateBuffer();
        m_displayBuffers1->ActivateBuffer();
        m_displayBuffers0Ext->ActivateBuffer();
        nngxSwapBuffers( NN_GX_DISPLAY_BOTH );
        break;

    case EXTENSION_SCREEN | UPPER_SCREEN:
        m_displayBuffers0->ActivateBuffer();
        m_displayBuffers0Ext->ActivateBuffer();
        nngxSwapBuffers( NN_GX_DISPLAY0 );
        break;

    case EXTENSION_SCREEN | LOWER_SCREEN:
        m_displayBuffers1->ActivateBuffer();
        m_displayBuffers0Ext->ActivateBuffer();
        nngxSwapBuffers( NN_GX_DISPLAY_BOTH );
        break;

    case EXTENSION_SCREEN:
        m_displayBuffers0Ext->ActivateBuffer();
        nngxSwapBuffers( NN_GX_DISPLAY0 );
        break;

    case UPPER_SCREEN | LOWER_SCREEN:
        m_displayBuffers0->ActivateBuffer();
        m_displayBuffers1->ActivateBuffer();
        nngxSwapBuffers( NN_GX_DISPLAY_BOTH );
        break;

    case UPPER_SCREEN:
        m_displayBuffers0->ActivateBuffer();
        nngxSwapBuffers( NN_GX_DISPLAY0 );
        break;

    case LOWER_SCREEN:
        m_displayBuffers1->ActivateBuffer();
        nngxSwapBuffers( NN_GX_DISPLAY1 );
        break;
    }

    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}

void GFXAdapter_3DS_GR::WaitVsync(const u32 targetDisplay)
{
    if(!isWaitVSyncEnabled())
    {
        return;
    }
    switch( targetDisplay )
    {
    case EXTENSION_SCREEN | UPPER_SCREEN | LOWER_SCREEN:
    case EXTENSION_SCREEN | LOWER_SCREEN:
    case UPPER_SCREEN | LOWER_SCREEN:
        if(isFix30FPS())
        {
            if ( ms_VBlankCount < 2 )
            {
                switch ( ms_VBlankCount )
                {
                case 0 :
                    nngxWaitVSync(NN_GX_DISPLAY_BOTH);
                case 1 :
                    nngxWaitVSync(NN_GX_DISPLAY_BOTH);
                    break;
                }
            }
            ms_VBlankCount=0;
        }
        else
        {
            nngxWaitVSync(NN_GX_DISPLAY_BOTH);
        }
        break;

    case EXTENSION_SCREEN | UPPER_SCREEN:
    case UPPER_SCREEN:
    case EXTENSION_SCREEN:
        nngxWaitVSync(NN_GX_DISPLAY0);
        break;

    case LOWER_SCREEN:
        nngxWaitVSync(NN_GX_DISPLAY1);
        break;
    }
}

void GFXAdapter_3DS_GR::SetBooleanRegister()
{
    u32 registerData = (m_useFluid2) ? m_techBitSet | 1 << 15 : m_techBitSet;
    *m_commandCursor++ = PICA_CMD_DATA_VS_BOOL( registerData );
    *m_commandCursor++ = PICA_CMD_HEADER_SINGLE( PICA_REG_VS_BOOL );
}

void GFXAdapter_3DS_GR::SetConst0ColorRegister()
{
    *m_commandCursor++ = PICA_CMD_DATA_TEX_ENV_CONST(m_combiner.stage[0].constColorR, m_combiner.stage[0].constColorG, m_combiner.stage[0].constColorB, m_combiner.stage[0].constColorA);
    *m_commandCursor++ = PICA_CMD_HEADER_SINGLE( PICA_REG_TEX_ENV0_COLOR );
}

void GFXAdapter_3DS_GR::SetWorldViewMatrix(const GMatrix44 *world)
{
    nn::math::Matrix44 model(world->fa16);
    if (!m_stereoMode)
    {   
        nn::math::MTX44Mult(&m_worldView, model, m_view);
        m_worldView.Transpose();
    }
    else
    {
        nn::math::MTX44Mult(&m_worldViewL, model, m_viewL);
        m_worldViewL.Transpose();

        nn::math::MTX44Mult(&m_worldViewR, model, m_viewR);
        m_worldViewR.Transpose();
    }
}

void GFXAdapter_3DS_GR::UpdateGPUState()
{
    if (m_isTextureDirty)
    {
        if (m_currentTexture)
        {
            m_commandCursor = m_texture.MakeCommand(m_commandCursor);
            m_commandCursor = m_texture.unit0.MakeCommand(m_commandCursor, false);
        }
        else
        {
            m_commandCursor = m_texture.MakeDisableCommand(m_commandCursor);
        }
        m_isTextureDirty = false;
    }

    if (m_isRenderStateDirty)
    {
        m_commandCursor = m_renderState.MakeCommand(m_commandCursor);
        m_isRenderStateDirty = false;
    }

    if (m_isCombinerStateDirty)
    {
        m_commandCursor = m_combiner.stage[0].MakeCommand(m_commandCursor);
        m_commandCursor = m_combiner.stage[1].MakeCommand(m_commandCursor);
        m_commandCursor = m_combiner.MakeCombinerBufferCommand(m_commandCursor);
        m_isCombinerStateDirty = false;
        m_isConstColor0Dirty = false;
    }
    else if ( m_isConstColor0Dirty )
    {
        SetConst0ColorRegister();
        m_isConstColor0Dirty = false;
    }

    if (!m_geomShaderMode)
    {
        SetBooleanRegister();
    }
}

void GFXAdapter_3DS_GR::InvalidateStateCache()
{
    m_prevousTech = DEFAULT_TECH_COUNT;

    m_currentBuffer = NULL;
    m_prevousTech = NULL;
    
    m_currentVertexFormat = VertexFormatIdx_Count;
    m_previousVertexFormat = VertexFormatIdx_Count;

    u_CurrentBlendingMode = GFX_BLEND_UNKNOWN;
    m_previousCullMode = GFX_CULLMODE(GFX_CULL_CW + 1);
    m_previousDepthFunc = GFX_CMP_ALWAYS + 1;
    m_previousDepthMask = 0xFFFFFFFF;
    m_previousDepthTest = 0xFFFFFFFF;
    m_isTextureDirty = true;
    m_currentTexture = NULL;
    m_previousUaddressingMode = GFXTADDRESS_WRAP;
    m_previousVaddressingMode = GFXTADDRESS_WRAP;

    m_isCombinerStateDirty = true;
    m_isConstColor0Dirty = true;
    m_previousCombinerConfig = CombinerConfig_Invalid;

    m_isRenderStateDirty = true;
    m_geomShaderMode = false;
}


bool GFXAdapter_3DS_GR::InitializeFont(const char* fontShaderPath, const char* fontPath)
{
    File *shaderFile = FILEMANAGER->openFile(fontShaderPath, ITF_FILE_ATTR_READ);
    u32 shaderFileSize = shaderFile->getLength();
    void* shaderBuffer = static_cast<void*>( GFXMEM::DevMemAlloc( shaderFileSize, 128 ) );
    shaderFile->read(shaderBuffer, shaderFileSize);

    File *fontFile = FILEMANAGER->openFile(fontPath, ITF_FILE_ATTR_READ);
    u32 fontFileSize = fontFile->getLength();
    void* fontBuffer = static_cast<void*>( GFXMEM::DevMemAlloc( fontFileSize, 128 ) );
    fontFile->read(fontBuffer, fontFileSize);

    const u32 vtxBufCmdBufSize =
        nn::font::CTR::RectDrawer::GetVertexBufferCommandBufferSize( shaderBuffer, shaderFileSize );

    m_FontCommandBuffer = GFXMEM::DevMemAlloc( vtxBufCmdBufSize );
    NN_NULL_ASSERT( m_FontCommandBuffer );

    const u32 drawBufferSize = nn::font::ResFont::GetDrawBufferSize( fontBuffer );
    void* drawBuffer = GFXMEM::DevMemAlloc( drawBufferSize, 4 );
    NN_NULL_ASSERT( drawBuffer );

    const u32 stringBufferSize = nn::font::CTR::CharWriter::GetDispStringBufferSize( m_MaxTextCount );
    void *const stringBuffer = GFXMEM::DevMemAlloc( stringBufferSize ); 
    NN_NULL_ASSERT( stringBuffer );

    bool result = false;
    if ( ( fontBuffer != NULL ) && ( shaderBuffer != NULL ) )
    {
        result = this->InitializeFont( shaderBuffer, shaderFileSize, 
            fontBuffer, fontFileSize, drawBuffer, stringBuffer );
    }

    if ( ! result )
    {
        GFXMEM::DevMemFree( fontBuffer );
    }

    GFXMEM::DevMemFree( shaderBuffer );

    //InitializeGraphicsState();

    return result;
}

//--------------------------------------------------------------------------
void GFXAdapter_3DS_GR::DestroyFont()
{
    m_Drawer.Finalize();

    GFXMEM::DevMemFree( m_FontCommandBuffer );

    nn::font::CTR::DispStringBuffer* stringBuffer = m_TextWriter.GetDispStringBuffer();
    GFXMEM::DevMemFree( stringBuffer );

    if (! m_Font.IsManaging( NULL ) )
    {
        void* drawBuffer = m_Font.SetDrawBuffer(NULL);
        void* resource = m_Font.RemoveResource();

        GFXMEM::DevMemFree( resource );
        GFXMEM::DevMemFree( drawBuffer );
    }
}

//--------------------------------------------------------------------------
void GFXAdapter_3DS_GR::BeginDrawingString()
{
    m_Drawer.DrawBegin();

    SendFontMatrix();

    m_TextWriter.SetCursor( 0, 0 );
}

//--------------------------------------------------------------------------
f32 GFXAdapter_3DS_GR::DrawString( const char* format, ... )
{    
    NN_ASSERT( format );

    std::va_list vargs;
    va_start(vargs, format);

    m_TextWriter.StartPrint();
    f32 width = m_TextWriter.VPrintf(format, vargs);
    m_TextWriter.EndPrint();

    m_Drawer.BuildTextCommand(&m_TextWriter);
    m_TextWriter.UseCommandBuffer();

    va_end(vargs);

    return width;
}

//--------------------------------------------------------------------------
f32 GFXAdapter_3DS_GR::DrawString( s32 posh, s32 posv, const char* format, ... )
{    
    NN_ASSERT( format );

    m_TextWriter.SetCursor( static_cast<f32>(posh), static_cast<f32>(posv) );

    std::va_list vargs;
    va_start(vargs, format);

    f32 width = this->DrawStringArgs( posh, posv, format, vargs);

    va_end(vargs);

    return width;
}

//--------------------------------------------------------------------------
f32 GFXAdapter_3DS_GR::DrawStringArgs( s32 posh, s32 posv, const char* format, std::va_list args )
{    
    NN_ASSERT( format );

    m_TextWriter.SetCursor( static_cast<f32>(posh), static_cast<f32>( posv ) );

    m_TextWriter.StartPrint();
    f32 width = m_TextWriter.VPrintf(format, args);
    m_TextWriter.EndPrint();

    m_Drawer.BuildTextCommand( &m_TextWriter );
    m_TextWriter.UseCommandBuffer();

    return width;
}

//--------------------------------------------------------------------------
void GFXAdapter_3DS_GR::EndDrawingString()
{
    m_Drawer.DrawEnd();
}

//--------------------------------------------------------------------------
bool GFXAdapter_3DS_GR::InitializeFont( 
                                void* shaderBinary,
                                size_t shaderSize,
                                void* fontBinary,
                                size_t fontSize,
                                void* drawBuffer,
                                void* stringBuffer
                                )
{
    if ( ! this->InitializeFontResource( fontBinary, fontSize ) )
    {
        return false;
    }

    if ( ! this->InitializeFontShader( shaderBinary, shaderSize ) )
    {
        return false;
    }

    m_Font.SetDrawBuffer(drawBuffer);

    m_TextWriter.SetFont( &m_Font );
    m_TextWriter.EnableFixedWidth( true );
    m_TextWriter.SetFixedWidth( 6.f );
    m_TextWriter.SetFontSize(17.f,13.f);
    nn::font::CTR::DispStringBuffer *const pDrawStringBuf = 
        nn::font::CTR::CharWriter::InitDispStringBuffer( stringBuffer, m_MaxTextCount );
    m_TextWriter.SetDispStringBuffer( pDrawStringBuf );

    InitializeGraphicsState();

    return true;
}

//--------------------------------------------------------------------------
bool GFXAdapter_3DS_GR::InitializeFontResource( void* fontData, size_t fontSize )
{
    NN_UNUSED_VAR( fontSize );

    return m_Font.SetResource( fontData );
}


//--------------------------------------------------------------------------
bool GFXAdapter_3DS_GR::InitializeFontShader( void* shaderBinary, size_t shaderSize )
{
    m_Drawer.Initialize(m_FontCommandBuffer, shaderBinary, shaderSize);

    return true;
}

//--------------------------------------------------------------------------
void GFXAdapter_3DS_GR::SendFontMatrix( void )
{
    {
        nn::math::MTX44 proj;
        const f32 znear =   0.0f;
        const f32 zfar  = - 1.0f;
        const f32 t     =      0;
        f32 b           =  static_cast<f32>( NN_GX_DISPLAY1_WIDTH );
        const f32 l     =      0;
        f32 r           =  static_cast<f32>( NN_GX_DISPLAY1_HEIGHT );

        nn::math::MTX44OrthoPivot( &proj, l, r, b, t, znear, zfar, nn::math::PIVOT_UPSIDE_TO_TOP );

        m_Drawer.SetProjectionMtx( proj );
    }

    {
        nn::math::MTX34 mv;
        nn::math::MTX34Identity(&mv);

        m_Drawer.SetViewMtxForText(mv);
    }
}

void GFXAdapter_3DS_GR::InitializeGraphicsState()
{
    m_fontRenderState.blend.isEnable          = true;
    m_fontRenderState.blend.eqRgb             = PICA_DATA_BLEND_EQUATION_ADD;
    m_fontRenderState.blend.eqAlpha           = PICA_DATA_BLEND_EQUATION_ADD;
    m_fontRenderState.blend.srcRgb            = PICA_DATA_BLEND_FUNC_SRC_ALPHA;
    m_fontRenderState.blend.srcAlpha          = PICA_DATA_BLEND_FUNC_ONE_MINUS_SRC_ALPHA;
    m_fontRenderState.blend.dstRgb            = PICA_DATA_BLEND_FUNC_ONE_MINUS_SRC_ALPHA;
    m_fontRenderState.blend.dstAlpha          = PICA_DATA_BLEND_FUNC_ONE_MINUS_SRC_ALPHA;

    m_fontRenderState.stencilTest.isEnable    = false;

    m_fontRenderState.depthTest.isEnable      = false;
    m_fontRenderState.depthTest.isEnableWrite = false;

    m_fontRenderState.cullingTest.isEnable    = false;

    for ( u32 stageIndex = 0; stageIndex < nn::gr::CTR::Combiner::COMBINER_STAGE_MAX; stageIndex++ )
    {
        m_fontCombiner.stage[ stageIndex ].rgb.combine   = PICA_DATA_TEX_ENV_COMBINE_REPLACE;
        m_fontCombiner.stage[ stageIndex ].alpha.combine = PICA_DATA_TEX_ENV_COMBINE_REPLACE;

        for ( u32 sourceIndex = 0; sourceIndex < 3; sourceIndex++)
        {
            if ( ( stageIndex == 0 ) && ( sourceIndex == 0 ) )
            {        
                m_fontCombiner.stage[ stageIndex ].rgb.source[ sourceIndex ]   = PICA_DATA_TEX_ENV_SRC_RGBA_PRIMARY_COLOR;
                m_fontCombiner.stage[ stageIndex ].alpha.source[ sourceIndex ] = PICA_DATA_TEX_ENV_SRC_RGBA_PRIMARY_COLOR;
            }
            else
            {
                m_fontCombiner.stage[ stageIndex ].rgb.source[ sourceIndex ]   = PICA_DATA_TEX_ENV_SRC_RGBA_PREVIOUS;
                m_fontCombiner.stage[ stageIndex ].alpha.source[ sourceIndex ] = PICA_DATA_TEX_ENV_SRC_RGBA_PREVIOUS;
            }

            m_fontCombiner.stage[ stageIndex ].rgb.operand[ sourceIndex ]      = PICA_DATA_OPE_RGB_SRC_COLOR;                        
            m_fontCombiner.stage[ stageIndex ].alpha.operand[ sourceIndex ]    = PICA_DATA_OPE_ALPHA_SRC_ALPHA;
        }

        m_fontCombiner.stage[ stageIndex ].rgb.scale   = PICA_DATA_TEX_ENV_SCALE_1;
        m_fontCombiner.stage[ stageIndex ].alpha.scale = PICA_DATA_TEX_ENV_SCALE_1;
    }
}


void GFXAdapter_3DS_GR::gcFree(void* addr)
{
    m_gcTable[gcCounter].push_back(addr);
}

void GFXAdapter_3DS_GR::gcCollect()
{
    gcCounter = (gcCounter + 1) % GARBAGLE_COLLECTOR_CYCLE;
    for (u32 i = 0; i < m_gcTable[gcCounter].size(); i++)
    {
        GFXMEM::DevMemFree(m_gcTable[gcCounter][i]);
    }
    m_gcTable[gcCounter].clear();
}

void GFXAdapter_3DS_GR::InitStereoState()
{
    m_projectionPatchsCount = 0;
    m_modelViewPatchsCount = 0;
    m_projectionPatchsGeoCount = 0;
    m_modelViewPatchsGeoCount = 0;

    m_startLeftRenderingAddr = NULL;
    m_endLeftRenderingAddr = NULL;
    m_stereoMode = false;
}

void GFXAdapter_3DS_GR::UpdateGPUWorldView()
{
    if (!m_stereoMode)
    {
        m_commandCursor = m_defaultUniformModelViewMtx.MakeUniformCommand(m_commandCursor, m_worldView);
    }
    else
    {
        MatrixToPatch& mtx = m_modelViewPatchs[m_modelViewPatchsCount++];
        mtx.matrix = m_worldViewR;
        mtx.offset = m_commandCursor - m_startLeftRenderingAddr;
        m_commandCursor = m_defaultUniformModelViewMtx.MakeUniformCommand(m_commandCursor, m_worldViewL);
    }
}

void GFXAdapter_3DS_GR::UpdateGPUProj()
{
    if (!m_stereoMode)
    {
        m_commandCursor = m_defaultUniformProjMtx.MakeUniformCommand(m_commandCursor, m_picaProj);
    }
    else
    {
        MatrixToPatch& mtx = m_projectionPatchs[m_projectionPatchsCount++];
        mtx.matrix = m_projR;
        mtx.offset = m_commandCursor - m_startLeftRenderingAddr;
        m_commandCursor = m_defaultUniformProjMtx.MakeUniformCommand(m_commandCursor, m_projL);
    }
}

void GFXAdapter_3DS_GR::UpdateGPUWorldViewGeo()
{
    if (!m_stereoMode)
    {
        m_commandCursor = m_patchUniformModelViewMtx.MakeUniformCommand(m_commandCursor, m_worldView);
    }
    else
    {
        MatrixToPatch& mtx = m_modelViewPatchsGeo[m_modelViewPatchsGeoCount++];
        mtx.matrix = m_worldViewR;
        mtx.offset = m_commandCursor - m_startLeftRenderingAddr;
        m_commandCursor = m_patchUniformModelViewMtx.MakeUniformCommand(m_commandCursor, m_worldViewL);
    }
}

void GFXAdapter_3DS_GR::UpdateGPUProjGeo()
{
    if (!m_stereoMode)
    {
        m_commandCursor = m_patchUniformProjMtx.MakeUniformCommand(m_commandCursor, m_picaProj);
    }
    else
    {
        MatrixToPatch& mtx = m_projectionPatchsGeo[m_projectionPatchsGeoCount++];
        mtx.matrix = m_projR;
        mtx.offset = m_commandCursor - m_startLeftRenderingAddr;
        m_commandCursor = m_patchUniformProjMtx.MakeUniformCommand(m_commandCursor, m_projL);
    }
}

void GFXAdapter_3DS_GR::DrawRightEye()
{
    nngxMoveCommandbufferPointer((m_commandCursor - m_startAddr) * sizeof(u32));
    nngxGetCmdlistParameteri(NN_GX_CMDLIST_CURRENT_BUFADDR, reinterpret_cast<GLint*>(&m_commandCursor));
    m_startAddr = m_commandCursor;
    u32 commandCount = (m_commandCursor - m_startLeftRenderingAddr);
    
    m_currentFrameBuffer = &m_frameBuffer0Ext;
    m_stereoMode = false;
    m_commandCursor = m_viewport.MakeCommand( m_commandCursor );
    m_commandCursor = m_scissor.MakeCommand( m_commandCursor );

    m_commandCursor = m_defaultShader.MakeFullCommand( m_commandCursor );

    nn::gr::CTR::Combiner defaultCombiner;
    m_commandCursor = defaultCombiner.MakeCommand(m_commandCursor);
    m_commandCursor = nn::gr::CTR::Texture::MakeDisableCommand(m_commandCursor);
    m_commandCursor = m_defaultUniformProjMtx.MakeUniformCommand(m_commandCursor, m_firstProjMtxR);

    m_commandCursor = nn::gr::CTR::Vertex::MakeDisableCommand(m_commandCursor);

    clear(m_lastLeftClearBuffBits, lastLeftClearBuffR, lastLeftClearBuffG, lastLeftClearBuffB, lastLeftClearBuffA);
    if (commandCount)
    {
        memcpy(m_commandCursor, m_startLeftRenderingAddr, commandCount * sizeof(u32));

        for (u32 i = 0; i < m_modelViewPatchsCount; i++)
        {
            m_defaultUniformModelViewMtx.MakeUniformCommand(m_commandCursor + m_modelViewPatchs[i].offset, m_modelViewPatchs[i].matrix);
        }

        for (u32 i = 0; i < m_modelViewPatchsGeoCount; i++)
        {
            m_patchUniformModelViewMtx.MakeUniformCommand(m_commandCursor + m_modelViewPatchsGeo[i].offset, m_modelViewPatchsGeo[i].matrix);
        }

        for (u32 i = 0; i < m_projectionPatchsCount; i++)
        {
            m_defaultUniformProjMtx.MakeUniformCommand(m_commandCursor + m_projectionPatchs[i].offset, m_projectionPatchs[i].matrix);
        }

        for (u32 i = 0; i < m_projectionPatchsGeoCount; i++)
        {
            m_patchUniformProjMtx.MakeUniformCommand(m_commandCursor + m_projectionPatchsGeo[i].offset, m_projectionPatchsGeo[i].matrix);
        }

        nngxMoveCommandbufferPointer( commandCount * sizeof(u32) );
    }

    nngxGetCmdlistParameteri(NN_GX_CMDLIST_CURRENT_BUFADDR, reinterpret_cast<GLint*>(&m_commandCursor));
    m_startAddr = m_commandCursor;
}

void FrameworkDrawText(int x, int y, const char *fmt, ...)
{
    GFXAdapter_3DS_GR* adapter = static_cast<GFXAdapter_3DS_GR*>(GFX_ADAPTER);
    if (adapter)
    {
        const size_t BUFSIZE = 1024;
        static char textBuf[BUFSIZE+1];
        va_list va;
        va_start(va, fmt);
        vsnprintf(textBuf, BUFSIZE, fmt, va);
        va_end(va);
        textBuf[BUFSIZE]='\0';
        //NN_LOG("text(%d,%d) %s\n", x, y, textBuf);
        adapter->drawDBGText(textBuf, 200.f+static_cast<f32>(x), 100.f+static_cast<f32>(y));
    }
}


} // namespace ITF
