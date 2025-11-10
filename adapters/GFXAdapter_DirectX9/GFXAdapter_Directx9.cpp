#include "precompiled_GFXAdapter_Directx9.h"

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_DIRECTX9_DRAWPRIM_H_
#include "adapters/GFXAdapter_Directx9/GFXAdapter_Directx9.h"
#endif //_ITF_DIRECTX9_DRAWPRIM_H_

#ifndef _ITF_FILEPACKFORMAT_H_
#include "engine/file/FilePackFormat.h"
#endif //_ITF_FILEPACKFORMAT_H_

#ifndef _ITF_VIDEOCAPTURE_H_
#include "engine/display/videoCapture.h"
#endif //_ITF_VIDEOCAPTURE_H_

#ifndef _ITF_SLOTALLOCATORMANAGER_H_
#include "core/memory/slotallocatorManager.h"
#endif //_ITF_SLOTALLOCATORMANAGER_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifdef ITF_WINDOWS
#include "../../../extern/CaptureJpeg/Source/INTERFCE.h"
#include <shlobj.h>
#endif

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

///////////////////////////////////////////////////////////////////////////////
///////////////////////       implementation of various shapes drawing
///////////////////////////////////////////////////////////////////////////////

#include <math.h>
#ifdef ITF_WINDOWS
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #include <windows.h>        // otherwise GL.h and GLU.h won't compile on PC
#endif // ITF_WINDOWS

#ifdef ITF_X360
#include <fxl.h>
#include <xgraphics.h>
#endif

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

/* 
#ifndef _ITF_DIRECTX9_HLSL_H_
#include "adapters/GFXAdapter_Directx9/GFXAdapter_HLSL.h"
#endif //_ITF_DIRECTX9_HLSL_H_
*/
#ifdef ITF_WINDOWS
#include "glfw/include/gl/glfw.h"
#endif //ITF_WINDOWS

#ifndef _ITF_DIRECTX9_DRAWPRIM_H_
#include "adapters/GFXAdapter_Directx9/GFXAdapter_Directx9.h"
#endif //_ITF_DIRECTX9_DRAWPRIM_H_

#ifndef _ITF_VEC2D_H_
#include "core/math/Vec2d.h"
#endif //_ITF_VEC2D_H_
#ifndef _ITF_TEXTURE_H_
#include "engine/display/Texture.h"
#endif //_ITF_TEXTURE_H_
#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_

#ifndef _ITF_CODECIMAGEADAPTER_H_
#include "engine/AdaptersInterfaces/CodecImageAdapter.h"
#endif //_ITF_CODECIMAGEADAPTER_H_

#ifndef _ITF_FILE_H_
#include "core/file/File.h"
#endif //_ITF_FILE_H_

#ifndef _ITF_FILEMANAGER_ITF_H_
#include "adapters/FileManager_ITF/FileManager_ITF.h"
#endif //_ITF_FILEMANAGER_ITF_H_

#ifndef _ITF_SYSTEMADAPTER_
#include "core/AdaptersInterfaces/SystemAdapter.h"
#endif //_ITF_SYSTEMADAPTER_

#ifndef _ITF_SCENE_MANAGER_H_
#include "engine/scene/sceneManager.h"
#endif //_ITF_SCENE_MANAGER_H_

#ifndef _ITF_DIRECTORY_H_
#include "core/file/Directory.h"
#endif //_ITF_DIRECTORY_H_

#include <d3dx9.h>
#include <algorithm>

#ifndef FREEIMAGE_H
#include "FreeImage/Source/FreeImage.h"
#endif // FREEIMAGE_H

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_THREADMANAGER_H_
#include "core/AdaptersInterfaces/ThreadManager.h"
#endif //_ITF_THREADMANAGER_H_

#ifndef _ITF_GFXADAPTER_RENDERTARGET_H_
#include "engine/AdaptersInterfaces/GFXAdapter_RenderTarget.h"
#endif //_ITF_GFXADAPTER_RENDERTARGET_H_

#ifndef _ITF_DDSFORMAT_H_
#include "engine/display/ddsformat.h"
#endif //_ITF_DDSFORMAT_H_

#ifndef _ITF_WORLD_MANAGER_H_
#include "engine/scene/worldManager.h"
#endif //_ITF_WORLD_MANAGER_H_

#ifndef _ITF_DEBUGINFO_H_
#include "engine/debug/debugInfo.h"
#endif //_ITF_DEBUGINFO_H_

#ifndef _ITF_RASTERSMANAGER_H_
#include "engine/rasters/RastersManager.h"
#endif //_ITF_RASTERSMANAGER_H_

#ifndef _ITF_STATSMANAGER_H_
#include "engine/stats/statsManager.h"
#endif //_ITF_STATSMANAGER_H_

#ifdef ITF_X360
#define USE_SHADER_BIN
#define USE_TEXTURE_TILE
#endif //ITF_X360

#ifndef _ITF_VIDEOADAPTER_H_
#include "engine/AdaptersInterfaces/VideoAdapter.h"
#endif //_ITF_VIDEOADAPTER_H_

#include "engine/video/videoHandle.h"

//#ifdef ITF_WINDOWS 
#define SHADERTEST
//#endif

#ifndef _ITF_PLUGINGATEWAY_H_
#include "PluginGateway/PluginGateway.h"
#endif //_ITF_PLUGINGATEWAY_H_

#ifndef _ITF_ENGINEMONITORPLUGIN_H_
#include "tools/plugins/EngineMonitorPlugin/EngineMonitorPlugin.h"
#endif //_ITF_ENGINEMONITORPLUGIN_H_

#include "core/UnicodeTools.h"

namespace ITF
{
    DECLARE_RASTER(drawMesh, RasterGroup_Misc, Color::red());
    DECLARE_RASTER(drawMeshPT, RasterGroup_Misc, Color::pink());
    DECLARE_RASTER(afterFX, RasterGroup_Misc, Color::violet());
    DECLARE_RASTER(doScreenCapture, RasterGroup_Misc, Color::violet());
    DECLARE_RASTER(present, RasterGroup_Misc, Color::green());

    #define SHADERGR_DEFAULT 0
    #define SHADERGR_MOVIE   1
    #define SHADERGR_FONT    2
    #define SHADERGR_AFTERFX 3

bbool GFXAdapter_Directx9::createDXDevice(int _alphaBits, int _depthBits, int _stencilBits, bbool _fullscreen  , void* _hwnd)
{
    // Create the D3D object, which is needed to create the D3DDevice.
    if( NULL == ( m_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
        return 0;

#ifdef ITF_X360
    XVIDEO_MODE VideoMode; 
    XMemSet( &VideoMode, 0, sizeof(XVIDEO_MODE) ); 
    XGetVideoMode( &VideoMode );

    u32 width   = VideoMode.dwDisplayWidth;
    u32 height  = VideoMode.dwDisplayHeight;
    if (!VideoMode.fIsWideScreen)
    {
        // compute the useful height to stay with a 16/9 display aspect ratio
        height = u32(float(VideoMode.dwDisplayHeight) * (9.f/16.f) * (4.f/3.f));
    }

    // We choose between two different resolution in order to avoid
    // too much reduction (to keep text readable)
    if (width > 1280 || height > 720)
    {
        width   = 1920;
        height  = 1080;
    }
    else
    {
        width   = 1280;
        height  = 720;
    }

    setResolution(width, height);
#endif //ITF_X360

    m_hwnd = (HWND)_hwnd;
    m_fullscreen = _fullscreen;

#ifdef ITF_WINDOWS
    m_displayModeCount = m_pD3D->GetAdapterModeCount(D3DADAPTER_DEFAULT,D3DFMT_X8R8G8B8);

    m_displayMode = newAlloc(mId_GfxAdapter,D3DDISPLAYMODE[m_displayModeCount]);

    for( u32 mode = 0; mode < m_displayModeCount; mode++ )
        m_pD3D->EnumAdapterModes(D3DADAPTER_DEFAULT,D3DFMT_X8R8G8B8,mode,&m_displayMode[mode]);
#endif //ITF_WINDOWS

    BuildPresentParams();

    DWORD bevahiorFlags = 0;

#ifdef ITF_WINDOWS
    bevahiorFlags = D3DCREATE_PUREDEVICE | D3DCREATE_HARDWARE_VERTEXPROCESSING|D3DCREATE_MULTITHREADED;
#endif //ITF_WINDOWS

#ifdef _USE_ZBUFFER_TILING
    bevahiorFlags = D3DCREATE_BUFFER_2_FRAMES;
#endif  

    if( FAILED( m_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hwnd,
                                      bevahiorFlags,&m_ppars, &m_pd3dDevice ) ) )
    {
        ITF_ASSERT_MSG(0, "FAILED( m_pD3D->CreateDevice...");
        return bfalse;
    }

    D3DCAPS9 caps;
    m_pd3dDevice->GetDeviceCaps(&caps);

    if (!(caps.RasterCaps & D3DPRASTERCAPS_SCISSORTEST))
    {
        ITF_FATAL_ERROR("Direct3D9Renderer: Hardware does not support D3DPRASTERCAPS_SCISSORTEST.  Unable to proceed." );
    }

    m_maxTextureSize = std::min(caps.MaxTextureHeight, caps.MaxTextureWidth);

    m_supportNonSquareTex = !(caps.TextureCaps & D3DPTEXTURECAPS_SQUAREONLY);

    m_supportNPOTTex = !(caps.TextureCaps & D3DPTEXTURECAPS_POW2) ||
                       (caps.TextureCaps & D3DPTEXTURECAPS_NONPOW2CONDITIONAL);
/*
#ifdef ITF_X360
    m_supportNPOTTex = 0;
#endif 
*/
    /// Core Shaders.    
	loadCoreShaders();
	if (!createVertexDeclaration())
		return bfalse;

    slotAllocatorManager::enablePhysicalAllocation();
    return btrue;
}

//----------------------------------------------------------------------------//

#ifdef _DEBUG
void printMatrix(const D3DXMATRIX& _matrix,char* name)
{
    LOG("%s:",name);
    for (int i=0;i<4;++i)
        LOG("( %0.08f %0.08f %0.08f %0.08f )",_matrix(i,0),_matrix(i,1),_matrix(i,2),_matrix(i,3));
}
#endif //_DEBUG


#ifdef _USE_FONTDEBUG
void GFXAdapter_Directx9::createDebugFonts(u32 _screenwidth, u32 _screenHeight)
{
#ifdef ITF_X360
    /// Create a font.
    String finalPath;
    m_FontDebug.Create( "default/x360/Arial_16.xpr",  m_pd3dDevice );
#else
    SF_RELEASE( mp_FontDebug );
	SF_RELEASE( mp_FontDebug_Monospace );
    SF_RELEASE( mp_FontDebug_Big );

    D3DXCreateFont( m_pd3dDevice, getDebugFontSize(), 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET,
                              OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
                              L"Arial", &mp_FontDebug );

    ITF_ASSERT(mp_FontDebug);

	D3DXCreateFont( m_pd3dDevice, getDebugFontSize(), 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
		L"Courier", &mp_FontDebug_Monospace );

	ITF_ASSERT(mp_FontDebug_Monospace);


    D3DXCreateFont( m_pd3dDevice, getBigDebugFontSize(), 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET,
                              OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
                              L"Arial", &mp_FontDebug_Big );

    ITF_ASSERT(mp_FontDebug_Big);
#endif
}
#else
void GFXAdapter_Directx9::createDebugFonts(u32 _screenwidth, u32 _screenHeight)
{
    // do nothing if not in debug mode.
    // We prefer to not remove the virtual method under defines to get API/ABI stability.
}
#endif

//----------------------------------------------------------------------------//

GFXAdapter_Directx9::GFXAdapter_Directx9()
{
    m_pD3D = 0;
    m_pd3dDevice = 0;
    m_depthStencilFormat = D3DFMT_D16;

#ifdef ITF_WINDOWS
    m_displayMode = 0;
    m_displayModeCount = 0;
#endif // ITF_WINDOWS

    mp_currentShader = 0;
    mp_VDcl_PCT = 0;
    mp_VDcl_PC = 0;
    mp_VDcl_PT = 0;
    mp_VDcl_PCBT = 0;
    mp_VDcl_PNCT = 0;
    mp_VDcl_PNC3T = 0;

    m_textureBinded.reserve(MAX_SAMPLER);
    
    for (u32 index = 0;index < m_textureBinded.capacity();++index)
        m_textureBinded.push_back((void *)uPtr(-1));
    mp_oldtexBindOnShader = NULL;

#if defined( _USE_FONTDEBUG) && defined(ITF_WINDOWS)
    mp_FontDebug = 0;
    mp_FontDebug_Big = 0;
	mp_FontDebug_Monospace = 0;
#endif

    D3DXMatrixIdentity( &mg_UVmat );

    m_useAFTERFX = 1;
    m_lostdevice = 0;

    m_currentRenderingSurface = 0;
    m_pCurrentSwapSourceTexture = 0;
    m_pCurrentSwapRenderSurf = 0;
    m_pBackBufferSurf = 0;
    m_curswap = 0;

    m_pSwapTextureDown2x2[0] = 0;
    m_pSwapTextureSurfDown2x2[0] = 0;
    m_pSwapTextureDown2x2[1] = 0;
    m_pSwapTextureSurfDown2x2[1] = 0;
    m_pSwapTextureDown4x4[0] = 0;
    m_pSwapTextureSurfDown4x4[0] = 0;
    m_pSwapTextureDown4x4[1] = 0;
    m_pSwapTextureSurfDown4x4[1] = 0;

    m_pPickingOffScreenBufferSurf = NULL;
    m_pRenderTargetSurfacePicking = NULL;

    m_pFullRTtex[0] = NULL;
    m_pFullRTsurf[0] = NULL;
    m_pFullRTtex[1] = NULL;
    m_pFullRTsurf[1] = NULL;

    setUseDepthBuffer(btrue);

#ifdef ITF_X360
    Synchronize::createCriticalSection(&m_csPresent);
    m_askToReleaseOwnerShip = bfalse;
    Synchronize::createEvent(&m_ReleaseOwnerShipEvent,"releaseOwnership");
    Synchronize::createEvent(&m_AcquireOwnerShipEvent,"acquireOwnership");
#endif //ITF_X360

    m_refractUse2x2 = bfalse;

    m_overDrawRamptexture = NULL;

    m_shaderPassLastState = S_SHADERPASS_NONE;
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::destroy()
{
    /// cleanup vertex buffer.    
    removeInternalBuffers();

    AFTERFX_destroyTarget();

    /// cleanup textures and surface.
    SF_RELEASE(m_pBackBufferSurf);
    SF_RELEASE(m_pFullRTtex[0]);
    SF_RELEASE(m_pFullRTtex[1]);
    SF_RELEASE(m_pFullRTsurf[0]);
    SF_RELEASE(m_pFullRTsurf[1]);
    SF_RELEASE(m_overDrawRamptexture);

#if defined( _USE_FONTDEBUG) && defined(ITF_WINDOWS)
    SF_RELEASE( mp_FontDebug );
    SF_RELEASE( mp_FontDebug_Big );
	SF_RELEASE( mp_FontDebug_Monospace );
#endif // _USE_FONTDEBUG && ITF_WINDOWS

    /// clean up vertex declaration.
    mp_shaderManager.destroyAll();
    m_VertexBufferManager.destroyAllVertexBuffer();
    m_VertexBufferManager.destroyAllIndexBuffer();

    /// clean up vertex declaration.
	releaseVertexDeclaration();

    /// cleanup direct 3d systems.
    SF_RELEASE(m_pD3D);
    SF_RELEASE(m_pd3dDevice);

#ifdef ITF_X360
    Synchronize::destroyCriticalSection(&m_csPresent);
    Synchronize::destroyEvent(&m_ReleaseOwnerShipEvent);
    Synchronize::destroyEvent(&m_AcquireOwnerShipEvent);
#else
    SF_DEL_ARRAY(m_displayMode);
#endif //ITF_X360

    GFXAdapter::destroy();
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::loadCoreShaders()
{
#ifdef SHADERTEST //tmpfix
    mp_shaderManager.createShaderGroup("n0");
    mp_shaderManager.createShaderGroup("n1");
    mp_shaderManager.createShaderGroup("n2");
    mp_shaderManager.createShaderGroup("n3");
    mp_shaderManager.createShaderGroup("n4");
    mp_shaderManager.createShaderGroup("n5");
    mp_shaderManager.createShaderGroup("n6");

	// shader animation with registers.
	mp_shaderManager.loadShaderGroup("renderPCT_Register.fx"); ///default shader.
    mp_shaderManager.loadShaderGroup("movie_Register.fx");
    mp_shaderManager.loadShaderGroup("font_Register.fx");
    mp_shaderManager.loadShaderGroup("AfterFx_Register.fx");

#else
    mp_shaderManager.loadShaderGroup("renderPCT.fx"); ///default shader.
    mp_defaultShader = mp_shaderManager.getShaderByIndex(0);
    mp_shaderManager.loadShaderGroup("afterfx.fx");
    mp_shaderManager.loadShaderGroup("refract.fx");
    mp_shaderManager.loadShaderGroup("font.fx");
    mp_shaderManager.loadShaderGroup("renderOVERDRAW.fx");
    mp_shaderManager.loadShaderGroup("renderFog.fx");
    mp_shaderManager.loadShaderGroup("movie.fx");
#endif

    mp_defaultShader = mp_shaderManager.getShaderByIndex(0);
}

bbool GFXAdapter_Directx9::createVertexDeclaration()
{
    HRESULT hr;

	// Create vertex declaration for scene
    if( FAILED( hr = m_pd3dDevice->CreateVertexDeclaration( VElem_PCT, &mp_VDcl_PCT ) ) )
    {
        return bfalse;
    }

    if( FAILED( hr = m_pd3dDevice->CreateVertexDeclaration( VElem_PC, &mp_VDcl_PC ) ) )
    {
        return bfalse;
    }

    if( FAILED( hr = m_pd3dDevice->CreateVertexDeclaration( VElem_PT, &mp_VDcl_PT ) ) )
    {
        return bfalse;
    }

    if( FAILED( hr = m_pd3dDevice->CreateVertexDeclaration( VElem_PCBT, &mp_VDcl_PCBT ) ) )
    {
        return bfalse;
    }

    if( FAILED( hr = m_pd3dDevice->CreateVertexDeclaration( VElem_PNCT, &mp_VDcl_PNCT ) ) )
    {
        return bfalse;
    }

    if( FAILED( hr = m_pd3dDevice->CreateVertexDeclaration( VElem_PNC3T, &mp_VDcl_PNC3T ) ) )
    {
        return bfalse;
    }

	return btrue;
}

void GFXAdapter_Directx9::releaseVertexDeclaration()
{
	SF_RELEASE( mp_VDcl_PC );
    SF_RELEASE( mp_VDcl_PCT );
    SF_RELEASE( mp_VDcl_PT );
    SF_RELEASE( mp_VDcl_PCBT );
    SF_RELEASE( mp_VDcl_PNCT );
    SF_RELEASE( mp_VDcl_PNC3T );
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::destroyResources()
{
    GFXAdapter::destroyResources();
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::setBiasEyeLookAt(const Vec3d &lookAtDir)
{
    if (!CAMERA->isBiased())
        CAMERA->m_bias_EyeToLookAt = lookAtDir;
    else
    {
        D3DXMATRIX biasRotationMatrix, invBiasRotationMatrix;

        D3DXMatrixRotationYawPitchRoll(&biasRotationMatrix, CAMERA->m_biasHorizAngle, CAMERA->m_biasVertAngle, 0);
        D3DXMatrixTranspose(&invBiasRotationMatrix, &biasRotationMatrix);
        D3DXVec3TransformNormal((D3DXVECTOR3*)&CAMERA->m_bias_EyeToLookAt, (D3DXVECTOR3*)&lookAtDir, &invBiasRotationMatrix);
    }

}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::setCamera(float _x, float _y, float _z, Camera* _cam)
{
    GFXAdapter::setCamera(_x, _y, _z, _cam);

    bbool isBiased = _cam->isBiased();

    ///
    /// WARNING DE LA MORT SI LE PICKING FONCTIONNE MAL ALORS VERIFIER LE NEAR QUI PAR DEFAUT ETAIT DE 1.f
    ///

    f32 nearPlane   = 1.f;
    f32 farPlane    = 1000.f;
    

    Vec2d topLeft = Vec2d::Zero;
    Vec2d botRight = Vec2d::One;
    nearPlane   = _cam->m_near;
    farPlane    = _cam->m_far;
    topLeft     = _cam->m_topLeft;
    botRight    = _cam->m_bottomRight;

    if (isBiased)
        nearPlane = 1.f;

    D3DVIEWPORT9 viewport;
    viewport.X = (DWORD)(topLeft.m_x * getScreenWidth());
    viewport.Y = (DWORD)(topLeft.m_y * getScreenHeight());
    viewport.Width = (DWORD)(fabs(botRight.m_x - topLeft.m_x) * (f32)getScreenWidth());
    viewport.Height = (DWORD)(fabs(botRight.m_y - topLeft.m_y) * (f32)getScreenHeight());
    viewport.MinZ = 0.0f;
    viewport.MaxZ = 1.0f;

    _cam->forcePosition(Vec3d(_x,_y,_z));

    f32 screenratio  = (fabsf(_cam->m_bottomRight.m_x - _cam->m_topLeft.m_x) * (f32)getScreenWidth())/(fabsf(_cam->m_bottomRight.m_y - _cam->m_topLeft.m_y) * (f32)getScreenHeight());

    D3DXMatrixIdentity( &mg_World );

    float viewDistance = _z;

    D3DXVECTOR3 vEyePt( _cam->getX(), _cam->getY(), viewDistance );
    D3DXVECTOR3 vLookatPt(_cam->getX(), _cam->getY(), _z);
    D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );

    D3DXVECTOR3 lookatOffset(0.f, 0.f, 0.f);
    if (isBiased)
    {
        D3DXVECTOR3 transformedRotationCenter;
        D3DXVECTOR3 centerPoint(_cam->m_bias_LookAt_RotationCenter.m_x,
            _cam->m_bias_LookAt_RotationCenter.m_y,
            _cam->m_bias_LookAt_RotationCenter.m_z);

        //Get screeen coordinates of RotationCenter
        if (_cam->hasBiasedCameraChanged())
            D3DXVec3Project(&transformedRotationCenter, &centerPoint, &viewport, &mg_lastSetProj, &mg_lastSetView, &mg_World);
        else
            transformedRotationCenter = m_lastTransformedRotationCenter;

        //get rotated matrix from rotated view vector

        D3DXMATRIX biasRotationMatrix;
        D3DXMatrixRotationYawPitchRoll(&biasRotationMatrix, _cam->m_biasHorizAngle, _cam->m_biasVertAngle, 0);
        
        //make view vector
        Vec3d currentViewVector_tmp = _cam->m_bias_EyeToLookAt;
        D3DXVECTOR3 currentViewVector(currentViewVector_tmp.m_x, currentViewVector_tmp.m_y, currentViewVector_tmp.m_z);
        D3DXVECTOR3 rotatedViewVector;
        D3DXVec3TransformNormal(&rotatedViewVector, &currentViewVector, &biasRotationMatrix);

        D3DXVECTOR3 vRightVec(-rotatedViewVector.z, 0,rotatedViewVector.x);
        if ( (fabs(vRightVec.x)+fabs(vRightVec.z))!=0)
        {
            D3DXVec3Cross(&vUpVec, &vRightVec, &rotatedViewVector);
            D3DXVec3Normalize(&vUpVec, &vUpVec);
            //D3DXVec3Cross(&vRightVec, &rotatedViewVector, &vUpVec); //This line is for checking Right vector
        }

        vEyePt = D3DXVECTOR3(_cam->m_resultingBiasedPosition.m_x, _cam->m_resultingBiasedPosition.m_y, _cam->m_resultingBiasedPosition.m_z);
        vLookatPt =  vEyePt + rotatedViewVector;
        
        //Make camera matrix
        D3DXMatrixLookAtRH( &mg_View, &vEyePt, &vLookatPt, &vUpVec );     
        D3DXMatrixPerspectiveFovRH( &mg_Proj, _cam->getFocale(), screenratio, nearPlane, farPlane );

        
        //We now want to search for the new eye pos, NewEye. NewEye = CurrentEye + T.
        //The camera transform is M<_eye_pos> :
        //   We want M<NewEye>(CenterPoint) = transformedRotationCenter
        //   => M<CurrentEye + T>(Center Point) = transformedRotationCenter
        //   => M<CurrentEye>(CenterPoint-T) = transformedRotationCenter
        //   => T = -InvM<CurrentEye>(transformedRotationCenter) + CenterPoint

        D3DXVECTOR3 T;
        D3DXVECTOR3 dummy;
        D3DXVec3Unproject(&dummy, &transformedRotationCenter, &viewport, &mg_Proj, &mg_View, &mg_World);
        T = centerPoint-dummy;

        T += D3DXVECTOR3(_cam->m_biasDeltaPos.m_x, _cam->m_biasDeltaPos.m_y, _cam->m_biasDeltaPos.m_z);
        _cam->m_biasDeltaPos = Vec3d(0,0,0);
        vEyePt += T;
        vLookatPt += T;

    }
    else
        lookatOffset.z = -1.f;

    vLookatPt += lookatOffset;

    D3DXMatrixLookAtRH( &mg_View, &vEyePt, &vLookatPt, &vUpVec );

    if (_cam->getRotateCam() && !isBiased && _cam->getMode() == Camera::Camera_InGame)
    {
        m_useRotateZCamera = btrue;
        D3DXMATRIX rz;
        D3DXMatrixRotationZ(&rz, _cam->getRotateZAngle());
        D3DXMatrixMultiply(&mg_ViewRender, &mg_View, &rz);
    }
    else
        m_useRotateZCamera = bfalse;

    D3DXMatrixPerspectiveFovRH( &mg_Proj, _cam->getFocale(), screenratio, nearPlane, farPlane );
   
    mg_WorldView = mg_World * mg_View;
    mg_WorldViewProj = mg_WorldView * mg_Proj;

    mg_lastSetView = mg_View;
    mg_lastSetProj = mg_Proj;

    _cam->m_lookAt.m_x = vLookatPt.x;
    _cam->m_lookAt.m_y = vLookatPt.y;
    _cam->m_lookAt.m_z = vLookatPt.z;

    _cam->m_resultingBiasedPosition.m_x = vEyePt.x;
    _cam->m_resultingBiasedPosition.m_y = vEyePt.y;
    _cam->m_resultingBiasedPosition.m_z = vEyePt.z;

    //recompute rotation center in case we have changed parameters meanwhile
    if (isBiased && _cam->hasBiasedCameraChanged())
    {
        D3DXVECTOR3 transformedRotationCenter;
        D3DXVECTOR3 centerPoint(_cam->m_bias_LookAt_RotationCenter.m_x,
            _cam->m_bias_LookAt_RotationCenter.m_y,
            _cam->m_bias_LookAt_RotationCenter.m_z);

        D3DXVec3Project(&transformedRotationCenter, &centerPoint, &viewport, &mg_lastSetProj, &mg_lastSetView, &mg_World);
        m_lastTransformedRotationCenter = transformedRotationCenter;
        _cam->clearChangedBiasedCamera();
    }

    //update parameters
    updateCameraFrustumPlanes(_cam);
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::updateCameraFrustumPlanes(Camera* _cam)
{
    D3DXVECTOR4 leftPlane, rightPlane, topPlane, bottomPlane, nearPlane;
    extractFrustumPlanes(mg_WorldViewProj, leftPlane, rightPlane, topPlane, bottomPlane, nearPlane);

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

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::extractFrustumPlanes(const D3DXMATRIX &_m,
                                               D3DXVECTOR4 &_leftPlane,
                                               D3DXVECTOR4 &_rightPlane,
                                               D3DXVECTOR4 &_topPlane,
                                               D3DXVECTOR4 &_bottomPlane,
                                               D3DXVECTOR4 &_nearPlane)
{
    D3DXVECTOR4 xColumn(_m._11, _m._21, _m._31, _m._41);
    D3DXVECTOR4 yColumn(_m._12, _m._22, _m._32, _m._42);
    D3DXVECTOR4 zColumn(_m._13, _m._23, _m._33, _m._43);
    D3DXVECTOR4 wColumn(_m._14, _m._24, _m._34, _m._44);

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

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::normalizePlane(D3DXVECTOR4 &_plane)
{
    D3DXPLANE &plane = (D3DXPLANE&)_plane;
    D3DXPlaneNormalize(&plane, &plane);
}
//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::init2DRender(   )
{
    /// Set 2D Matrix.
    setOrthoView(0.f, (f32) getScreenWidth(), 0.f, (f32) getScreenHeight());
}

void GFXAdapter_Directx9::init2DGuiRender()
{
    setOrthoView2DYNeg(0.f, (f32) getScreenWidth(), 0.f, (f32) getScreenHeight());
}

void GFXAdapter_Directx9::setOrthoView2DYNeg(f32 _l, f32 _r, f32 _b, f32 _t)
{
    D3DXMatrixIdentity( &mg_World );
    D3DXMatrixIdentity( &mg_View );
    D3DXMatrixIdentity( &mg_ViewRender );
    D3DXMatrixOrthoOffCenterRH(&mg_Proj, _l, _r, _b,_t, 1, -1.f);
    mg_WorldView = mg_World * mg_View;
    mg_WorldViewProj = mg_WorldView * mg_Proj;
    updateCameraFrustumPlanes(CAMERA);

    m_useRotateZCamera = bfalse;
}

void GFXAdapter_Directx9::setOrthoView(f32 _l, f32 _r, f32 _b, f32 _t)
{
    D3DXMatrixIdentity( &mg_World );
    D3DXMatrixIdentity( &mg_View );
    D3DXMatrixIdentity( &mg_ViewRender );
    m_useRotateZCamera = bfalse;

    D3DXMatrixOrthoOffCenterRH(&mg_Proj, _l, _r, _b, _t, -1, 1.f);
    mg_WorldView = mg_World * mg_View;
    mg_WorldViewProj = mg_WorldView * mg_Proj;
    updateCameraFrustumPlanes(CAMERA);
}

void GFXAdapter_Directx9::init()
{
    resetScreenShotRequest();
    m_pUltraHDBackBufferSurf = NULL;
    m_preUHDScreenWidth = 0;
    m_preUHDScreenHeight = 0;

    if (getScreenWidth() == 0 || getScreenHeight() == 0)
        LOG("GFXAdapter init error : resolution = 0, call setResolution(width,height) before init");
    
#ifdef _USE_ZBUFFER_TILING
    //setRenderSceneInTarget(btrue);
    //createRenderTarget(&m_pSceneTexture, &m_pSceneSurf, getScreenWidth(), getScreenHeight(), D3DFMT_X8R8G8B8);
#endif

    GFX_RECT Viewport;
    Viewport.left = 0;
    Viewport.top = 0;
    Viewport.right = getScreenWidth();
    Viewport.bottom = getScreenHeight();
    setupViewport(&Viewport);
    
    setFillMode(GFX_FILL_SOLID);

    // Turn off culling
    m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

    // Turn off D3D lighting
    useHDWLightning(0);

    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
    m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
#ifdef ITF_WINDOWS
    m_pd3dDevice->SetRenderState( D3DRS_ANTIALIASEDLINEENABLE, FALSE );
#endif
    
    setDefaultAlpha();

    renderContext_SetDefault();

    isRender = 0;
    //setCamera(0.f, 0.f, 0.0f, 1.f);

    setShader(mp_defaultShader);
  
    //getNewRenderTarget();
#ifndef _USE_ZBUFFER_TILING
    getBackBuffer();
#endif

    resetSceneResolvedFlag();

    RENDER_TARGET_DEBUG = 0;

    /// create internal GFX VB.
    createInternalBuffers();

#ifdef VBMANAGER_USE_DYNAMICRING_VB
    m_VertexBufferManager.createDynamicRingVB();
#endif

    setGlobalColor(Color::white());

#ifdef ITF_X360
#ifdef _USE_ZBUFFER_TILING
    D3DSURFACE_PARAMETERS DepthSurfParams;
    DepthSurfParams.Base = XGSurfaceSize( getScreenWidth(), getScreenHeight(),/*D3DFMT_X8B8G8R8*/( D3DFORMAT )MAKESRGBFMT( D3DFMT_LE_X8R8G8B8 ), D3DMULTISAMPLE_NONE );
    DepthSurfParams.ColorExpBias = 0;
    DepthSurfParams.HierarchicalZBase = 0;
    DepthSurfParams.HiZFunc = D3DHIZFUNC_DEFAULT;

    m_pd3dDevice->CreateRenderTarget( 512, 512,
                                           D3DFMT_D24S8,
                                           D3DMULTISAMPLE_NONE,
                                           0, FALSE,
                                           &m_pDepthStencil,
                                           &DepthSurfParams );
#endif
#endif

    createOverDrawRamp( 50, 100, 150);
    m_pd3dDevice->SetTexture(0,0);
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::reshape()
{
    if (getScreenWidth() == 0 || getScreenHeight() == 0)
        LOG("GFXAdapter init error : resolution = 0, call setResolution(width,height) before init");
    
    GFX_RECT Viewport;
    Viewport.left = 0;
    Viewport.top = 0;
    Viewport.right = getScreenWidth();
    Viewport.bottom = getScreenHeight();
    setupViewport(&Viewport);
}

//----------------------------------------------------------------------------//

void  GFXAdapter_Directx9::compute3DTo2D(const Vec3d& _in, Vec3d& _out)
{
// for unknown reason the D3DXVec3Project function is sometime returning a nan. So replaced by another available function
#if 1
    GFX_Viewport viewp;
    viewp.m_x = 0;
    viewp.m_y = 0;
    viewp.m_width = getScreenWidth();
    viewp.m_height = getScreenHeight();
    viewp.m_minZ = 0.f;
    viewp.m_maxZ = 1.f;

    D3DXMATRIX viewProj = mg_View * mg_Proj;

    vec3dProject(&_out, &_in, &viewp, (GMatrix44*)&viewProj);

#else
    // Old code not functional
    D3DXVECTOR3 vec2D, vec3D;
    D3DXVECTOR3 vecSrc(_in.m_x, _in.m_y, _in.m_z);
    const D3DXMATRIX * projMat = (const D3DXMATRIX *)&mg_Proj;
    const D3DXMATRIX * viewMat = (const D3DXMATRIX *)&mg_View;
    D3DXMATRIX worldMat;
    D3DXMatrixIdentity(&worldMat);

    D3DVIEWPORT9 viewport;
    viewport.X = viewport.Y = 0;
    viewport.Width = getScreenWidth();
    viewport.Height = getScreenHeight();
    viewport.MinZ = 0.0f;
    viewport.MaxZ = 1.0f;
          
    D3DXVec3Project(
        &vec2D,
        &vecSrc,
        &viewport,
        projMat,
        viewMat,
        &worldMat
        );    
    _out.m_x = vec2D.x;
    _out.m_y = vec2D.y;
    _out.m_z = vec2D.z;
#endif
}
void  GFXAdapter_Directx9::compute2DTo3D(const Vec3d& _in, Vec3d& _out)
{
    D3DXVECTOR3 vec2D, vec3D;
    const D3DXMATRIX * projMat = (const D3DXMATRIX *)&mg_Proj;
    const D3DXMATRIX * viewMat = (const D3DXMATRIX *)&mg_View;
/*#ifdef ASSERT_ENABLED
    const f32 f41 = fabs(mg_View._41);
    const f32 f42 = fabs(mg_View._42);
    const f32 f43 = fabs(mg_View._43);
    if (f41 < MTH_EPSILON && f42 < MTH_EPSILON && f43 < MTH_EPSILON)
    {
        ITF_ASSERT_MSG(bfalse, "calling compute2DTo3D with invalid view matrix. Probably during 2D pass");
    }
#endif*/
    D3DXMATRIX worldMat;
    D3DXMatrixIdentity(&worldMat);

    D3DVIEWPORT9 viewport;
    viewport.X = viewport.Y = 0;
    viewport.Width = getScreenWidth();
    viewport.Height = getScreenHeight();
    viewport.MinZ = 0.0f;
    viewport.MaxZ = 1.0f;

    vec2D.x = _in.m_x;
    vec2D.y = _in.m_y;
    vec2D.z = _in.m_z;

    D3DXVec3Unproject(
        &vec3D,
        &vec2D,
        &viewport,
        projMat,
        viewMat,
        &worldMat
        );
    _out.m_x = vec3D.x;
    _out.m_y = vec3D.y;
    _out.m_z = vec3D.z;
}
//----------------------------------------------------------------------------//


void  GFXAdapter_Directx9::coord2DTo3D( const Vec2d& _in2d, float _zPlane, Vec2d& _out3d, bool noTranslation)
{
    D3DXVECTOR3 in, out;
    D3DVIEWPORT9 vp;
    m_pd3dDevice->GetViewport(&vp);
    in.x = _in2d.m_x;
    in.y = _in2d.m_y;
    in.z = _zPlane;
    D3DXMatrixIdentity(&mg_World);
    
    // no Translation.
    D3DXMATRIX view = mg_View;
    if (noTranslation)
    {
        view._41 = 0;
        view._42 = 0;
        view._43 = 0;
    }
    D3DXVec3Unproject(&out, &in, &vp, &mg_Proj, &view, &mg_World);

    _out3d.m_x = (float)out.x;
    // Directx No Y inversion.
    _out3d.m_y = (float)out.y;
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::drawScreenQuad( f32 _px, f32 _py, f32 _width, f32 _height, f32 _z, u32 _color, bbool _noTex )
{
    if (getOverDrawMode())
        return;

    D3DXMATRIX World;
    D3DXMatrixIdentity(&World);
    setObjectMatrix(World);

    m_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
    m_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

    /// centroid note:
    /// the renderer and matrix view is y inverted,
    /// then the centroid in y must be add not substract.
    f32 centroid = 0.5f;

    if (_noTex)
    {
        VertexPC quad[4];

        quad[0].setData( Vec3d( _px - centroid, _py + centroid, _z ), _color);
        quad[1].setData( Vec3d( _px - centroid, _py + _height + centroid, _z ), _color);
        quad[2].setData( Vec3d( _px + _width - centroid, _py + 0.5f, _z ), _color);
        quad[3].setData( Vec3d( _px + _width - centroid, _py + _height + centroid, _z ), _color);

        setVertexFormat(VertexFormat_PC);
        DrawPrimitive(GFX_TRIANGLE_STRIP, (void*)quad, 4);
    }
    else
    {
        VertexPCT quad[4];

#ifdef test_rotoscreenuv
        anim.m_uvFlagAnim |= UVANIM_F_ROTATE;
        anim.m_uvFlagAnim |= UVANIM_F_TRANS;
        anim.m_speedTrans = Vec2d(2.3f,0.f);
        anim.m_speedRotate = 0.5f;
        computeUVAnim(&anim);
        setUVAnim(anim);
    
        m_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
        m_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
#endif

        quad[0].setData( Vec3d( _px - centroid, _py + centroid, _z ), Vec2d( 0.f, 1.0f ), _color);
        quad[1].setData( Vec3d( _px - centroid, _py + _height + centroid, _z ), Vec2d( 0.f, 0.f ), _color);
        quad[2].setData( Vec3d( _px + _width - centroid, _py + centroid, _z ), Vec2d( 1.0f, 1.0f ), _color);
        quad[3].setData( Vec3d( _px + _width - centroid, _py + _height + centroid, _z ), Vec2d( 1.0f, 0.0f ), _color);
        
        setVertexFormat(VertexFormat_PCT);
        DrawPrimitive(GFX_TRIANGLE_STRIP, (void*)quad, 4);
    }

    m_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
    m_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
}

void GFXAdapter_Directx9::drawScreenQuadC( f32 _px, f32 _py, f32 _width, f32 _height, f32 _z, u32 _color, f32 _centroid, bbool _noTex )
{
    if (getOverDrawMode())
        return;

    D3DXMATRIX World;
    D3DXMatrixIdentity(&World);
    setObjectMatrix(World);

    m_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
    m_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

    if (_noTex)
    {
        VertexPC quad[4];

        quad[0].setData( Vec3d( _px - _centroid, _py + _centroid, _z ), _color);
        quad[1].setData( Vec3d( _px - _centroid, _py + _height + _centroid, _z ), _color);
        quad[2].setData( Vec3d( _px + _width - _centroid, _py + _centroid, _z ), _color);
        quad[3].setData( Vec3d( _px + _width - _centroid, _py + _height + _centroid, _z ), _color);

        setVertexFormat(VertexFormat_PC);
        DrawPrimitive(GFX_TRIANGLE_STRIP, (void*)quad, 4);
    }
    else
    {
        VertexPCT quad[4];

        quad[0].setData( Vec3d( _px - _centroid, _py + _centroid, _z ), Vec2d( 0.f, 1.0f ), _color);
        quad[1].setData( Vec3d( _px - _centroid, _py + _height + _centroid, _z ), Vec2d( 0.f, 0.f ), _color);
        quad[2].setData( Vec3d( _px + _width - _centroid, _py + _centroid, _z ), Vec2d( 1.0f, 1.0f ), _color);
        quad[3].setData( Vec3d( _px + _width - _centroid, _py + _height + _centroid, _z ), Vec2d( 1.0f, 0.0f ), _color);
        
        setVertexFormat(VertexFormat_PCT);
        DrawPrimitive(GFX_TRIANGLE_STRIP, (void*)quad, 4);
    }

    m_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
    m_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::drawQuad2D( f32 _px, f32 _py, f32 _width, f32 _height, f32 _z, u32 _color, const Texture* _tex, const Vec2d* _uvStart,  const Vec2d* _uvEnd )
{
    D3DXMATRIX World;
    D3DXMatrixIdentity(&World);
    setObjectMatrix(World);

    /*
        ------->  +X
        |
        |
        |
        |
        +Y
    */

    const f32 py = (f32)getScreenHeight() - _py - _height;
    Vec2d uvStart, uvEnd;
    if (_uvStart)
        uvStart = *_uvStart;
    else
        uvStart = Vec2d(0.f, 0.f);
    if (_uvEnd)
        uvEnd = *_uvEnd;
    else
        uvEnd = Vec2d(1.f, 1.f);

    setAlphaBlend(GFX_BLEND_ALPHA);

    setShader(mp_defaultShader);

    if (_tex)
    {
        VertexPCT quad[4];
        quad[0].setData( Vec3d( _px, py, _z ), Vec2d( uvStart.m_x, uvEnd.m_y ), _color);
        quad[1].setData( Vec3d( _px, py + _height, _z ), uvStart, _color);
        quad[2].setData( Vec3d( _px + _width, py, _z ), uvEnd, _color);
        quad[3].setData( Vec3d( _px + _width, py + _height, _z ), Vec2d( uvEnd.m_x, uvStart.m_y ), _color);

        SetTextureBind(0, _tex->m_adapterimplementationData);
        setVertexFormat(VertexFormat_PCT);
    
        DrawPrimitive(GFX_TRIANGLE_STRIP, (void*)quad, 4);
    }
    else
    {
        VertexPC quad[4];
        quad[0].setData( Vec3d( _px, py, _z ), _color);
        quad[1].setData( Vec3d( _px, py + _height, _z ), _color);
        quad[2].setData( Vec3d( _px + _width, py, _z ), _color);
        quad[3].setData( Vec3d( _px + _width, py + _height, _z ), _color);

        setVertexFormat(VertexFormat_PC);
        DrawPrimitive(GFX_TRIANGLE_STRIP, (void*)quad, 4);
    }
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::draw3DQuad( GFX_3DQUAD* _t_Quad )
{
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

#ifdef SHADERTEST
    setShader(mp_defaultShader);
#else
    if (getOverDrawMode() > 1)
        setShader( mp_shaderManager.getShaderByIndex(4));
    else
    {
        if (isUseFog())
            setShader( mp_shaderManager.getShaderByIndex(5));
        else
            setShader(mp_defaultShader);
    }
#endif

    setVertexFormat(VertexFormat_PCT);

    setAlphaBlend(_t_Quad->m_blendMode);

    DrawPrimitive(GFX_TRIANGLE_STRIP, (void*)quad, 4);
}

//----------------------------------------------------------------------------//

bbool GFXAdapter_Directx9::loadTextureCooked(Texture* _texture, const char* _path)
{ 
    String filenameCooked = FILESERVER->getCookedName(_path);
    return loadTextureFromFile(_texture,filenameCooked);
}

//----------------------------------------------------------------------------//

bbool GFXAdapter_Directx9::loadTextureFromFile(Texture* _texture, const String& _path)
{ 
    File* fileTexture = FILEMANAGER->openFile(_path,ITF_FILE_ATTR_READ);
    ITF_ASSERT(fileTexture);
    
    u32 sizeFile = (u32) fileTexture->getLength();
    u8 *rawBuf = newAlloc(mId_Temporary,u8[sizeFile]);
    ITF_ASSERT_MSG(rawBuf, "LOAD Texture Failed out of memory %s", UTF16ToUTF8(_path.cStr()).get());

    if (!rawBuf)
        return bfalse;

    u32 bytesRead = 0;
    ITF_VERIFY(fileTexture->read(rawBuf,sizeFile,&bytesRead));
    FILEMANAGER->closeFile(fileTexture);

#ifdef USE_TEXTURE_TILE

    cleanupTexture(_texture);

    D3DTexture* texture = (D3DTexture*)rawBuf;
    // Get description of the texture's mip0
    D3DSURFACE_DESC desc;
    texture->GetLevelDesc( 0, &desc );

    u32 extraDescFlags = 0;//D3DUSAGE_CPU_CACHED_MEMORY;

    u32 baseSize;
    u32 mipSize;
    if(texture->GetType() == D3DRTYPE_TEXTURE)
    {
        D3DTexture* _texadd = new D3DTexture;

        XGSetTextureHeaderEx
            (
            desc.Width,
            desc.Height, 
            texture->GetLevelCount(), 
            desc.Usage | extraDescFlags,
            desc.Format, 
            0, 
            0, 
            0, 
            XGHEADER_CONTIGUOUS_MIP_OFFSET,
            0,
            ( D3DTexture* )_texadd, 
            &baseSize, 
            &mipSize
            );

        //_texadd->Common |= MipDataContinuousInMemory;

        // Compute the required memory for the texture data
        u32 textureDataSize = sizeFile - sizeof(D3DTexture);
 
        ITF_ASSERT(baseSize + mipSize == textureDataSize);
        u8* textureData;

        //bbool flushCache = ( (desc.Usage | extraDescFlags) & D3DUSAGE_CPU_CACHED_MEMORY ) != 0;

        textureData = (u8*)slotAllocatorManager::allocatePhysicalMemory( textureDataSize, MEM_ALIGNMENT_4K,  PAGE_READWRITE|PAGE_WRITECOMBINE );

        ITF_MemcpyWriteCombined(textureData, rawBuf + sizeof(D3DTexture), /*sizeof(u8) **/ textureDataSize);

        XGOffsetBaseTextureAddress( _texadd, textureData, textureData );

        Size size( desc.Width, desc.Height);
        setTextureSize(_texture, size);

        _texture->m_adapterimplementationData = _texadd;

        Size d_dataSize(size);
        updateTextureSize(_texture);
        Size TexSize(_texture->getSizeX(), _texture->getSizeY());
        updateCachedScaleValues(_texture);

        _texture->m_datasizeX = (u32)d_dataSize.d_width;
        _texture->m_datasizeY = (u32)d_dataSize.d_height;
    }
#else

    DDS_HEADER header;
    ITF_Memcpy(&header,rawBuf,sizeof(header));

    // if (header.dwHeight >= 8192 )
    if (header.dwHeight > 16*1024) // $GB changed to allow 16k texture
    {
        ITF_FATAL_ERROR("Texture size too big %d ==> %s ,quit the game and fix ", header.dwHeight, UTF16ToUTF8(_path.cStr()).get());
    }

    Size size( header.dwWidth,header.dwHeight);
    setTextureSize(_texture, size);

    ITF::Texture::PixFormat PixFormat;

    switch (header.format.dwFourCC)
    {
        case MAKEFOURCC('D', 'X', 'T', '1'):
            PixFormat = Texture::PF_DXT1;
            break;
        case MAKEFOURCC('D', 'X', 'T', '2'):
            PixFormat = Texture::PF_DXT2;
            break;
        case MAKEFOURCC('D', 'X', 'T', '3'):
            PixFormat = Texture::PF_DXT3;
            break;
        case MAKEFOURCC('D', 'X', 'T', '4'):
            PixFormat = Texture::PF_DXT4;
            break;
        case MAKEFOURCC('D', 'X', 'T', '5'):
            PixFormat = Texture::PF_DXT5;
            break;
        default:
            PixFormat = Texture::PF_RGBA;
            break;
    }

    loadFromMemory(rawBuf, size,_texture, PixFormat ,sizeFile,btrue,header.dwMipMapCount);

#endif

    SF_DEL_ARRAY(rawBuf);

#ifndef ITF_FINAL
    _texture->setMemoryUsage(sizeFile);
#endif //ITF_FINAL

    RESOURCE_MANAGER->addResourceAsLoaded(_texture);
    return btrue;
}

bbool GFXAdapter_Directx9::loadTexture(Texture* _texture, const char* _path, u32 /*_alhpa*/)
{   
#ifdef MODE_HRESTEXTURE_SUPPORTED
    if (_texture->hasHResVersion())
    {
        const String& hdFileNameCooked = FILESERVER->getCookedHResVersionName(_texture->getPath().getString());
        return loadTextureFromFile(_texture,hdFileNameCooked);
    }
    else
#endif //MODE_HRESTEXTURE_SUPPORTED
    if (_texture->isCooked())
    {
        return loadTextureCooked(_texture,_path);
    }
#ifdef ITF_WINDOWS
    else
    {
        RawDataContainer RawData;
                
        File* textureFile = FILEMANAGER->openFile(_path,ITF_FILE_ATTR_READ);

        if (!textureFile)
            return bfalse;

        u32 size = (u32) textureFile->getLength();

        unsigned char* buffer = newAlloc(mId_Temporary, unsigned char [size]);

        u32 sizeRead = 0;
        textureFile->read(buffer, size,&sizeRead);
        textureFile = FILEMANAGER->closeFile(textureFile);

        if (size!=sizeRead)
        {
            ITF_ASSERT(0);
            SF_DEL_ARRAY(buffer);
            return bfalse;
        }
        
        RawData.setData(buffer);
        RawData.setSize(size);

        CODECIMAGE_ADAPTER->load(RawData, _texture, _path);
    }
#endif  //ITF_WINDOWS

    return btrue;
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::drawTriangle(const Vec2d& _p1, const Vec2d& _p2, const Vec2d& _p3, float _r, float _g, float _b, float _a, float _z)
{
    VertexPC v[3];

    u32 color = D3DCOLOR_ARGB((u32)(_a*255.f), (u32)(_r*255.f), (u32)(_g*255.f), (u32)(_b*255.f));
    v[0].setData(Vec3d(_p1.m_x, _p1.m_y, _z) , color);
    v[1].setData(Vec3d(_p2.m_x, _p2.m_y, _z) , color);
    v[2].setData(Vec3d(_p3.m_x, _p3.m_y, _z) , color);

    /// turn off hardware lightning.
    useHDWLightning(0);

    m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );

    /// set first the shader you want to use.
    setShader(mp_defaultShader);

    /// set the matrix.
    D3DXMATRIX World;
    D3DXMatrixIdentity(&World);
    setObjectMatrix(World);

    //depthTest(0);
    //depthMask(0);
    //depthFunc(GFX_CMP_GREATEREQUAL);

    /// set Vertex format
    setVertexFormat(VertexFormat_PC);
    
    /// draw geometrie.
    DrawPrimitive(GFX_TRIANGLES, (void*)v, 3);
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::draw2dTriangle(const Vec2d& _p1, const Vec2d& _p2, const Vec2d& _p3, u32 _color, i32 _rank)
{
    VertexPC v[3];

    Triangle2D tri;

    const f32 p1y = (f32)getScreenHeight() - _p1.m_y;
    const f32 p2y = (f32)getScreenHeight() - _p2.m_y;
    const f32 p3y = (f32)getScreenHeight() - _p3.m_y;

    tri.v[0].m_pos.m_x  = _p1.m_x;
    tri.v[0].m_pos.m_y  = p1y;
    tri.v[1].m_pos.m_x  = _p2.m_x;
    tri.v[1].m_pos.m_y  = p2y;
    tri.v[2].m_pos.m_x  = _p3.m_x;
    tri.v[2].m_pos.m_y  = p3y;

    tri.v[0].m_color = _color;
    tri.v[1].m_color = _color;
    tri.v[2].m_color = _color;

    tri.m_rank = _rank;
   
    m_Triangle2DList.push_back(tri);
}


//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::drawTriangle( const Vertex& _v1, const Vertex& _v2, const Vertex& _v3, const Texture* _tex, bbool _showWireFrame)
{
    VertexPCT v[3];
    D3DXMATRIX World;
    D3DXMatrixIdentity(&World);
    setObjectMatrix(World);
    //depthTest(0);
    //depthMask(0);
    //depthFunc(GFX_CMP_GREATEREQUAL);

    v[0].setData( Vec3d( _v1.m_pos.m_x, _v1.m_pos.m_y, _v1.m_pos.m_z ), Vec2d( _v1.m_uv.m_x, _v1.m_uv.m_y ), _v1.m_color);
    v[1].setData( Vec3d( _v2.m_pos.m_x, _v2.m_pos.m_y, _v2.m_pos.m_z ), Vec2d( _v2.m_uv.m_x, _v2.m_uv.m_y ), _v2.m_color);
    v[2].setData( Vec3d( _v3.m_pos.m_x, _v3.m_pos.m_y, _v3.m_pos.m_z ), Vec2d( _v3.m_uv.m_x, _v3.m_uv.m_y ), _v3.m_color);

    /// set alpha blend.
    setAlphaBlend(GFX_BLEND_ALPHA);
 
    SetTextureBind(0, _tex->m_adapterimplementationData);

    /// set Vertex format
    setVertexFormat(VertexFormat_PCT);

    /// draw geometrie.
    DrawPrimitive(GFX_TRIANGLES, (void*)v, 3);

    if (!_showWireFrame)
        return;

    VertexPC v1[3];

    v1[0].setData(Vec3d(_v1.m_pos.m_x, _v1.m_pos.m_y, 0.1f), _v1.m_color);
    v1[1].setData(Vec3d(_v2.m_pos.m_x, _v2.m_pos.m_y, 0.1f), _v2.m_color);
    v1[2] = v1[1];

    /// set Vertex format
    setVertexFormat(VertexFormat_PC);

    /// draw geometrie.
    DrawPrimitive(GFX_TRIANGLES, (void*)v1, 3);
}

//----------------------------------------------------------------------------//

#ifndef ITF_CONSOLE_FINAL
void GFXAdapter_Directx9::drawFlatMesh(const ITF_Mesh& _mesh)
{
    setObjectMatrix((D3DXMATRIX&)_mesh.getMatrix());

    /// set alpha blend.
    setAlphaBlend(GFX_BLEND_ALPHA);

    /// set Vertex format
    setVertexFormat(VertexFormat_PC);

    for (u32 i = 0; i < _mesh.m_ElementList.size(); i++)
    {
        if (!_mesh.m_ElementList[i].m_indexBuffer)
            DrawVertexBuffer(GFX_TRIANGLES, _mesh.m_vtxBuffer, _mesh.m_ElementList[i].m_startIndex, _mesh.m_ElementList[i].m_count);
        else
            DrawIndexedVertexBuffer(GFX_TRIANGLES, _mesh.m_vtxBuffer, _mesh.m_ElementList[i].m_indexBuffer, _mesh.m_ElementList[i].m_count);
    } 
}
#endif // ITF_CONSOLE_FINAL


void GFXAdapter_Directx9::drawMeshPT(const ITF_Mesh& _mesh, bbool _showWireFrame)
{
    TIMERASTER_SCOPE(drawMeshPT);

    renderContext_Set();

    setObjectMatrix((D3DXMATRIX&)_mesh.getMatrix());

    /// set Vertex format
    //setVertexFormat(VertexFormat_PT);

    ITF_VertexBuffer* pVertexBuffer =  _mesh.m_vtxBuffer ?   _mesh.m_vtxBuffer : _mesh.m_vtxBufferBuffered[_mesh.m_userBuffer];
    
    if (m_showWireFrame < 2)
    {
    for (u32 i = 0; i < _mesh.m_ElementList.size(); i++)
    {
        const ITF_MeshElement& elem = _mesh.m_ElementList[i];
        if (!elem.m_material.m_textureDiffuse.isValidResourceId())
            continue;

        /// Prepare Material.
        setGfxMaterial(elem.m_material);
        D3DXMatrixIdentity( &mg_UVmat );

        if (!elem.m_indexBuffer)
            DrawVertexBuffer(GFX_TRIANGLES, pVertexBuffer, elem.m_startIndex, elem.m_count);
        else
            DrawIndexedVertexBuffer(GFX_TRIANGLES, pVertexBuffer, elem.m_indexBuffer, elem.m_count);
    } 

    D3DXMatrixIdentity( &mg_UVmat );
    }

    SetTextureBind(1, 0);
    SetTextureBind(2, 0);
    //renderContext_SetDefault();
    m_currentRenderingMaterialType = GFX_MAT_DEFAULT;

    if (!_showWireFrame && !m_showWireFrame)
    {
        return;
    }

    setFillMode(GFX_FILL_WIREFRAME);
    setGlobalColor(Color::white());
    setAlphaBlend(GFX_BLEND_COPY);
    SetTextureBind(0, 0);

    for (u32 i = 0; i < _mesh.m_ElementList.size(); i++)
    {
        if (!SetTextureResource(0, _mesh.m_ElementList[i].m_material.m_textureDiffuse))
            continue;

        if (!_mesh.m_ElementList[i].m_indexBuffer)
            DrawVertexBuffer(GFX_TRIANGLES, pVertexBuffer, _mesh.m_ElementList[i].m_startIndex, _mesh.m_ElementList[i].m_count);
        else
            DrawIndexedVertexBuffer(GFX_TRIANGLES,pVertexBuffer, _mesh.m_ElementList[i].m_indexBuffer, _mesh.m_ElementList[i].m_count);
    } 

    /// Restore state.
    setFillMode(GFX_FILL_SOLID);
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::drawMesh(ITF_Mesh& _mesh, bbool _showWireFrame)
{
    TIMERASTER_SCOPE(drawMesh);

    renderContext_Set();

    setObjectMatrix((D3DXMATRIX&)_mesh.getMatrix());

    ITF_VertexBuffer* pVertexBuffer =  _mesh.m_vtxBuffer ?   _mesh.m_vtxBuffer : _mesh.m_vtxBufferBuffered[_mesh.m_userBuffer];

    /// set Vertex format
    setVertexFormat(pVertexBuffer->m_vertexFormat);

    if (m_showWireFrame < 2)
    {
    for (u32 i = 0; i < _mesh.m_ElementList.size(); i++)
    {
        ITF_MeshElement& elem = _mesh.m_ElementList[i];

        if (!elem.m_material.m_textureDiffuse.isValidResourceId())
            continue;

#ifdef ITF_WINDOWS
        /// Discard some materials for picking
        if (m_bPickingMode)
        {
            switch (elem.m_material.m_matType)
            {
                case GFX_MAT_REFRACTION:    // no picking for refraction
                    continue;
                break;
                default:
                break;
            }
        }
#endif //ITF_WINDOWS

        /// Prepare Material.
        setGfxMaterial(elem.m_material);

        if (elem.m_material.m_uvAnim.m_uvFlagAnim)
        {
            computeUVAnim(&elem.m_material.m_uvAnim);
             setUVAnim(elem.m_material.m_uvAnim);
        }
        else
            D3DXMatrixIdentity( &mg_UVmat );

        if (m_drawFlag & DRWF_COPYTOTARGET)
        {
            GFX_BLENDMODE saveblend = elem.m_material.m_blendMode;
            setAlphaBlend(GFX_BLEND_ADDALPHA);
            m_pd3dDevice->SetRenderTarget( 0, m_pSwapTextureSurfDown4x4[0] );

            if (!elem.m_indexBuffer)
                DrawVertexBuffer(GFX_TRIANGLES, pVertexBuffer, elem.m_startIndex, elem.m_count);
            else
                DrawIndexedVertexBuffer(GFX_TRIANGLES, pVertexBuffer, elem.m_indexBuffer, elem.m_count);

            m_pd3dDevice->SetRenderTarget( 0, m_currentRenderingSurface );

            setAlphaBlend(saveblend);
        }

        if (_mesh.m_pMatrixOverlay)
        {
            m_pd3dDevice->SetVertexShaderConstantF( 17, (f32*)_mesh.m_pMatrixOverlay, 4 );
        }        

        if (!elem.m_indexBuffer)
            DrawVertexBuffer(GFX_TRIANGLES, pVertexBuffer, elem.m_startIndex, elem.m_count);
        else
            DrawIndexedVertexBuffer(GFX_TRIANGLES, pVertexBuffer, elem.m_indexBuffer, elem.m_count);
    
        postGfxMaterial(elem.m_material);
    } 

    SetTextureBind(1, 0);
    SetTextureBind(2, 0);

    D3DXMatrixIdentity( &mg_UVmat );
 
    //renderContext_SetDefault();
    }

    m_currentRenderingMaterialType = GFX_MAT_DEFAULT;

    if (!_showWireFrame && !m_showWireFrame)
    {
        return;
    }

    setFillMode(GFX_FILL_WIREFRAME);

    setGlobalColor(Color::white());
    SetTextureBind(0, 0);
    setAlphaBlend(GFX_BLEND_COPY);

    for (u32 i = 0; i < _mesh.m_ElementList.size(); i++)
    {
        if (!SetTextureResource(0, _mesh.m_ElementList[i].m_material.m_textureDiffuse))
            continue;
        
        if (!_mesh.m_ElementList[i].m_indexBuffer)
            DrawVertexBuffer(GFX_TRIANGLES, pVertexBuffer, _mesh.m_ElementList[i].m_startIndex, _mesh.m_ElementList[i].m_count);
        else
            DrawIndexedVertexBuffer(GFX_TRIANGLES, pVertexBuffer, _mesh.m_ElementList[i].m_indexBuffer, _mesh.m_ElementList[i].m_count);
    } 

    /// Restore state.
    setFillMode(GFX_FILL_SOLID);
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::drawLine(const Vec2d& _p1, const Vec2d& _p2, float _r, float _g, float _b, float _size, float _z, float _alpha)
{
    ITF_ASSERT(0);
}

void GFXAdapter_Directx9::draw2dLine (   const Vec2d& _p1, const Vec2d& _p2, float _size, u32 _color)
{
    f32 screenHeight = (f32)getScreenHeight();
    Vec2d p1(_p1.m_x, screenHeight - _p1.m_y);
    Vec2d p2(_p2.m_x, screenHeight - _p2.m_y);

    Vec2d dir = (p2 - p1).normalize();
    Vec2d perp = dir.getPerpendicular();
    perp *= _size * 0.5f;

    Box2D box;
    box.v[0].m_pos = (p1 + perp).to3d();
    box.v[1].m_pos = (p2 + perp).to3d();
    box.v[2].m_pos = (p1 - perp).to3d();
    box.v[3].m_pos = (p2 - perp).to3d();

    box.v[0].m_color = _color;
    box.v[1].m_color = _color;
    box.v[2].m_color = _color;
    box.v[3].m_color = _color;

    m_Box2DList.push_back(box);
}

void GFXAdapter_Directx9::draw2dBox(const Vec2d& _p, f32 _width, f32 _height, u32 _color0, u32 _color1, u32 _color2, u32 _color3, i32 _rank)
{
    if (hasHideFlags(HideFlag_2DBox))
        return;
    Box2D box;

    const f32 py = (f32)getScreenHeight() - _p.m_y - _height;

    /// strip:
    /// 0 -- 2
    /// |    |
    /// 1 -- 3

    box.v[0].m_pos.m_x = _p.m_x;
    box.v[0].m_pos.m_y = py;
    box.v[1].m_pos.m_x = _p.m_x;
    box.v[1].m_pos.m_y = py + _height;
    box.v[2].m_pos.m_x = _p.m_x + _width;
    box.v[2].m_pos.m_y = py;
    box.v[3].m_pos.m_x = _p.m_x + _width;
    box.v[3].m_pos.m_y = py + _height;

    box.v[0].m_color = _color0;
    box.v[1].m_color = _color1;
    box.v[2].m_color = _color2;
    box.v[3].m_color = _color3;
    box.m_rank = _rank;
   
    m_Box2DList.push_back(box);
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::drawCircle(   float _x, float _y, float r, float _r, float _g, float _b, float _z, u32 _segsCount  )
{
    /// set first the shader you want to use.
    setShader(mp_defaultShader);

    /// fill vertex buffer.
    if (_segsCount > 64)
        _segsCount = 64;
    VertexPC v[65];
    u32 color = D3DCOLOR_ARGB(255, (u32)(_r*255.f), (u32)(_g*255.f), (u32)(_b*255.f));
    for(u32 i = 0; i <= _segsCount; i++) 
    {
        f32 fi = i * MTH_2PI / (float)_segsCount;
        v[i].setData(Vec3d((f32)cos(fi) * r + _x, (f32)sin(fi) * r + _y, _z), color);
    }

    D3DXMATRIX World;
    D3DXMatrixIdentity(&World);
    setObjectMatrix(World);

    /// set Vertex format
    setVertexFormat(VertexFormat_PC);

    /// draw geometrie.
    DrawPrimitive(GFX_LINE_STRIP, (void*)v, _segsCount+1);
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::drawSphere(   float _x, float _y, float r, float _r, float _g, float _b, float _z, u32 _segsCount, bbool _volume, bbool _xaxis, bbool _yaxis, bbool _zaxis  )
{
    /// set first the shader you want to use.
    setShader(mp_defaultShader);

    /// set Vertex format
    setVertexFormat(VertexFormat_PC);

    /// fill vertex buffer.
    if (_segsCount > 64)
        _segsCount = 64;
    VertexPC v[65];
    u32 color = D3DCOLOR_ARGB(255, (u32)(_r*255.f), (u32)(_g*255.f), (u32)(_b*255.f));

    D3DXMATRIX World;

    if (_volume )
    {
        for(u32 i = 0; i <= _segsCount; i++) 
        {
            f32 fi = i * MTH_2PI / (float)_segsCount;
            v[i].setData(Vec3d((f32)cos(fi) * r, (f32)sin(fi) * r, 0.f), color);
        }   
        
        D3DXMatrixInverse(&World, NULL, &mg_View);
        World._41 = _x;
        World._42 = _y;
        World._43 = _z;
        setObjectMatrix(World);
    
        DrawPrimitive(GFX_LINE_STRIP, (void*)v, _segsCount+1);
    }

    D3DXMatrixIdentity(&World);
    setObjectMatrix(World);

    if (_xaxis)
    {
        for(u32 i = 0; i <= _segsCount; i++) 
        {
            f32 fi = i * MTH_2PI / (float)_segsCount;
            v[i].setData(Vec3d(_x, (f32)cos(fi) * r + _y, (f32)sin(fi) * r + _z), color);
        }

        DrawPrimitive(GFX_LINE_STRIP, (void*)v, _segsCount+1);
    }

    if (_yaxis)
    {
        for(u32 i = 0; i <= _segsCount; i++) 
        {
            f32 fi = i * MTH_2PI / (float)_segsCount;
            v[i].setData(Vec3d((f32)cos(fi) * r + _x, _y, (f32)sin(fi) * r + _z), color);
        }

        DrawPrimitive(GFX_LINE_STRIP, (void*)v, _segsCount+1);
    }

    if (_zaxis)
    {
        for(u32 i = 0; i <= _segsCount; i++) 
        {
            f32 fi = i * MTH_2PI / (float)_segsCount;
            v[i].setData(Vec3d((f32)cos(fi) * r + _x, (f32)sin(fi) * r + _y, _z), color);
        }

        DrawPrimitive(GFX_LINE_STRIP, (void*)v, _segsCount+1);
    }
}


//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::drawBox( const Vec2d& _pos, f32 _angle, const Vec2d& _extent, f32 _r, f32 _g, f32 _b, float _z , bbool _fill)
{
    /// set first the shader you want to use.
    setShader(mp_defaultShader);

    VertexPC v[5];
    u32 color = D3DCOLOR_ARGB(255, (u32)(_r*255.f), (u32)(_g*255.f), (u32)(_b*255.f));

    Vec2d p = Vec2d(-_extent.m_x,_extent.m_y).Rotate(_angle) + _pos;
    v[0].setData(Vec3d(p.m_x, p.m_y, _z), color);
    p = Vec2d(_extent.m_x,_extent.m_y).Rotate(_angle) + _pos;
    v[1].setData(Vec3d(p.m_x, p.m_y, _z), color);
    p = Vec2d(_extent.m_x,-_extent.m_y).Rotate(_angle) + _pos;
    v[2].setData(Vec3d(p.m_x, p.m_y, _z), color);
    p = Vec2d(-_extent.m_x,-_extent.m_y).Rotate(_angle) + _pos;
    v[3].setData(Vec3d(p.m_x, p.m_y, _z), color);
    p = Vec2d(-_extent.m_x,_extent.m_y).Rotate(_angle) + _pos;
    v[4].setData(Vec3d(p.m_x, p.m_y, _z), color);

    D3DXMATRIX World;
    D3DXMatrixIdentity(&World);
    setObjectMatrix(World);

    /// set Vertex format
    setVertexFormat(VertexFormat_PC);

    /// draw geometrie.
    if (_fill)
    {
        v[4] = v[2];
        v[2] = v[3];
        v[3] = v[4];
        for (u32 i=0; i<4; i++)
        {
			v[i].m_color = ITFCOLOR_SET_A(v[i].m_color, (u8)0xaa);
        }
        DrawPrimitive(GFX_TRIANGLE_STRIP, (void*)v, 4);
    }
    else
        DrawPrimitive(GFX_LINE_STRIP, (void*)v, 5);
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::drawSphere(float _x, float _y, float _z, float r, int lats, int longs, int _wireFrame, float _r, float _g, float _b) 
{
    /// set first the shader you want to use.
    setShader(mp_defaultShader);

    /// fill vertex buffer.
    VertexPC v[65];
    u32 color = D3DCOLOR_ARGB(255, (u32)(_r*255.f), (u32)(_g*255.f), (u32)(_b*255.f));
    for(u32 i = 0; i <= 64; i++) 
    {
        f32 fi = i * MTH_2PI / 64.f;
        v[i].setData(Vec3d((f32)cos(fi), (f32)sin(fi), 0.f), color);
    }   

    /// set the matrix.
    D3DXMATRIX World, Scale;
    D3DXMatrixTranslation(&World, _x, _y, _z);
    D3DXMatrixScaling(&Scale, r, r, r);
    D3DXMatrixMultiply(&World, &Scale, &World);
    setObjectMatrix(World);
    
    /// set Vertex format
    setVertexFormat(VertexFormat_PC);

    /// draw geometrie.
    DrawPrimitive(GFX_LINE_STRIP, (void*)v, 65);
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::clear(u32 _buffer, f32 _r, f32 _g, f32 _b, f32 _a)
{
/*
    Camera fake;
    setCamera(0, 0, 0, &fake);
*/

    u32 Buf = 0;

    if(_buffer & GFX_CLEAR_COLOR)
    {
        Buf |= D3DCLEAR_TARGET;
    }

    if(_buffer & GFX_CLEAR_ZBUFFER)
    {
        Buf |= D3DCLEAR_ZBUFFER;
    }

    if(_buffer & GFX_CLEAR_STENCIL)
    { 
        Buf |= D3DCLEAR_STENCIL;
    }

    if (getOverDrawMode() > 1)
    {
        _a = _r = _g = _b = 0.f;
    }

    m_pd3dDevice->Clear( 0, NULL, Buf,D3DCOLOR_ARGB((u32)(_a*255.f), (u32)(_r*255.f),(u32)(_g*255.f),(u32)(_b*255.f)), 1.0f, 0 );
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::setAlphaBlend(GFX_BLENDMODE _Blend)
{
    //if (u_CurrentBlendingMode == _Blend) return;

    u_CurrentBlendingMode = _Blend;

    if (getOverDrawMode() > 1)
        _Blend = GFX_BLEND_ALPHA;

    switch(_Blend)
    {
        case GFX_BLEND_COPY:
        m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
        m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
        m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ZERO );
        m_pd3dDevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
        break;

        default:
        case GFX_BLEND_UNKNOWN:
            ITF_ASSERT_MSG(bfalse, "Incorrect blend mode");
            //missing break intentionally
        case GFX_BLEND_ALPHA:
        m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
        m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
        m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
        m_pd3dDevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
        break;

        case GFX_BLEND_ALPHAPREMULT:
        m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
        m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
        m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
        m_pd3dDevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
        break;

        case GFX_BLEND_ALPHADEST:
        m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
        m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_DESTALPHA );
        m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVDESTALPHA );
        m_pd3dDevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
        break;

        case GFX_BLEND_ALPHADESTPREMULT:
        m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
        m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_DESTALPHA );
        m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
        m_pd3dDevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
        break;

        case GFX_BLEND_ADD:
        m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
        m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
        m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
        m_pd3dDevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
        break;

        case GFX_BLEND_ADDALPHA:
        m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
        m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
        m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
        m_pd3dDevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
        break;

        case GFX_BLEND_SUBALPHA:
         m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
        m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
        m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
        m_pd3dDevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_REVSUBTRACT );
        break;

        case GFX_BLEND_SUB:
        m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
        m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ZERO );
        m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR );
        m_pd3dDevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
        break;

        case GFX_BLEND_MUL:
        m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
        m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ZERO );
        m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR );
        m_pd3dDevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
        break;

        case GFX_BLEND_ALPHAMUL:
        m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
        m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ZERO );
        m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_SRCALPHA );
        m_pd3dDevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
        break;

        case GFX_BLEND_IALPHAMUL:
        m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
        m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ZERO );
        m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
        m_pd3dDevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
        break;

        case GFX_BLEND_IALPHA:
        m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
        m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_INVSRCALPHA );
        m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_SRCALPHA );
        m_pd3dDevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
        break;

        case GFX_BLEND_IALPHAPREMULT:
        m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
        m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
        m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_SRCALPHA );
        m_pd3dDevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
        break;

        case GFX_BLEND_IALPHADEST:
        m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
        m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_INVDESTALPHA );
        m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_DESTALPHA );
        m_pd3dDevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
        break;

        case GFX_BLEND_IALPHADESTPREMULT:
        m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
        m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_INVDESTALPHA );
        m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
        m_pd3dDevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
        break;
    
        case GFX_BLEND_MUL2X:
        m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
        m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR );
        m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR );
        m_pd3dDevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
        break;

        case GFX_BLEND_ALPHATOCOLOR:
        m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
        m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_DESTALPHA );
        m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ZERO );
        m_pd3dDevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
        break;

        case GFX_BLEND_IALPHATOCOLOR:
        m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
        m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_INVDESTALPHA );
        m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ZERO );
        m_pd3dDevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
        break;

        case GFX_BLEND_SETTOCOLOR:
        m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
        m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR );
        m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ZERO );
        m_pd3dDevice->SetRenderState( D3DRS_BLENDOP, D3DBLENDOP_ADD );
        break;

    }
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::setCullMode(GFX_CULLMODE _Cull)
{
    switch(_Cull)
    {
        case GFX_CULL_NONE:     m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE );break;
        case GFX_CALL_CCW:      m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW );break;
        case GFX_CULL_CW:       m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW );break;
    }
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::setFillMode(GFX_FILLMODE _fillmode)
{
    switch(_fillmode)
    {
        case GFX_FILL_SOLID:        m_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID );break;
        case GFX_FILL_WIREFRAME:    m_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME );break;
        case GFX_FILL_POINT:        m_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_POINT );break;
    }
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::depthMask(u32 _depthMask)
{
    if (_depthMask)
        m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
    else
        m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::depthFunc(u32 _Func)
{
    switch(_Func)
    {
    case GFX_CMP_NEVER:          m_pd3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_NEVER); break;
    case GFX_CMP_LESS:           m_pd3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESS); break;
    case GFX_CMP_EQUAL:          m_pd3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_EQUAL); break;
    case GFX_CMP_LESSEQUAL:      m_pd3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL); break;
    case GFX_CMP_GREATER:        m_pd3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_GREATER); break;
    case GFX_CMP_NOTEQUAL:       m_pd3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_NOTEQUAL); break;
    case GFX_CMP_GREATEREQUAL:   m_pd3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_GREATEREQUAL); break;
    case GFX_CMP_ALWAYS:         m_pd3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS); break;
    }
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::depthTest(u32 _Test)
{
    if (_Test)
        m_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
    else
        m_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::setScissorRect(GFX_RECT* _clipRect)
{
    // setup clip region
    RECT clip;
    clip.left   = static_cast<LONG>(_clipRect->left);
    clip.top    = static_cast<LONG>(_clipRect->top);
    clip.right  = static_cast<LONG>(_clipRect->right);
    clip.bottom = static_cast<LONG>(_clipRect->bottom);
    m_pd3dDevice->SetScissorRect(&clip);

    m_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE ,TRUE );
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::SetTexture(int _Sampler, Texture* _Texture, bbool _linearFiltering)
{
    void * adrtexture = NULL;
    if (_Texture) 
        adrtexture = _Texture->m_adapterimplementationData;
    SetTextureBind(_Sampler, adrtexture, _linearFiltering);
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::SetTextureBind(int _Sampler, void * _Bind, bbool _linearFiltering)
{
    ITF_ASSERT_CRASH(_Sampler<MAX_SAMPLER, "Incorrect sampler index");

    if (mp_oldtexBindOnShader == (void *)mp_currentShader && m_textureBinded[_Sampler] == _Bind)
        return;

    int filtering = D3DTEXF_LINEAR;
    if (!_linearFiltering)
        filtering = D3DTEXF_POINT;
    m_pd3dDevice->SetSamplerState(_Sampler, D3DSAMP_MINFILTER, filtering);
    m_pd3dDevice->SetSamplerState(_Sampler, D3DSAMP_MAGFILTER, filtering);
	m_pd3dDevice->SetSamplerState(_Sampler, D3DSAMP_MIPFILTER, filtering);

    if (mp_currentShader && mp_currentShader->m_TextureHandle[_Sampler])
    {
#ifdef ITF_X360
        //D3DTRILINEAR_THREEEIGHTHS
        m_pd3dDevice->SetSamplerState(0,D3DSAMP_TRILINEARTHRESHOLD,D3DTRILINEAR_THREEEIGHTHS);
        ((ITF_EFFECTP)mp_currentShader->mp_effect)->SetSampler((ITF_HANDLE)mp_currentShader->m_TextureHandle[_Sampler], (LPDIRECT3DBASETEXTURE9)_Bind);
#else
        ((ITF_EFFECTP)mp_currentShader->mp_effect)->SetTexture((ITF_HANDLE)mp_currentShader->m_TextureHandle[_Sampler], (LPDIRECT3DBASETEXTURE9)_Bind);
#endif
    }

    if (!_Bind)
        m_pd3dDevice->SetTexture( _Sampler, 0 );
#ifdef SHADERTEST
	else
		m_pd3dDevice->SetTexture( _Sampler, (LPDIRECT3DBASETEXTURE9)_Bind );
#endif

    m_textureBinded[_Sampler] = _Bind;
    mp_oldtexBindOnShader = mp_currentShader;
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::lockTexture( Texture* _tex, LOCKED_TEXTURE* _lockTex, u32 _flag )
{
    if ( !_tex || !_tex->m_adapterimplementationData ) return;

    D3DLOCKED_RECT lockTextureRect;
    IDirect3DTexture9*  pDXTexture =(IDirect3DTexture9*) _tex->m_adapterimplementationData;
    pDXTexture->LockRect( 0, &lockTextureRect, NULL, 0 );

    _lockTex->mp_Bits = (DWORD*)(lockTextureRect.pBits);
    _lockTex->m_pitch = lockTextureRect.Pitch;
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::unlockTexture( Texture* _tex)
{
    IDirect3DTexture9*  pDXTexture =(IDirect3DTexture9*) _tex->m_adapterimplementationData;
    pDXTexture->UnlockRect( 0 );

}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::DrawPrimitive(PRIMITIVETYPE _type, const void* _p_Vertex, u32 _NumberVertex)
{
    i32 type;
    u32 numberprimitive = 0;

    switch(_type)
    {
    default:
    case GFX_TRIANGLES: 
        type = D3DPT_TRIANGLELIST;
        numberprimitive = _NumberVertex / 3;
        break;
    case GFX_POINTS:
        type = D3DPT_POINTLIST;
        numberprimitive = _NumberVertex;
        break;
    case GFX_LINES:
        type = D3DPT_LINELIST;
        numberprimitive = _NumberVertex / 2;
       break;
    case GFX_LINE_STRIP:
        type = D3DPT_LINESTRIP;
        numberprimitive = _NumberVertex - 1;
       break;
    case GFX_TRIANGLE_STRIP:
        type = D3DPT_TRIANGLESTRIP;
        numberprimitive = _NumberVertex - 2;
       break;
    }

#ifdef SHADERTEST
if ( m_ShaderMode == 1 )
{
    if (mp_currentShader == mp_defaultShader)
    {
        setShaderFormat(m_currentVertexFormat);
    }
    else
        setShaderGlobalConstant(mp_currentShader);

    m_pd3dDevice->DrawPrimitiveUP((D3DPRIMITIVETYPE)type, numberprimitive, _p_Vertex, m_currentVertexSize);
}
else
#endif
{
    beginShader(mp_currentShader);
    m_pd3dDevice->DrawPrimitiveUP((D3DPRIMITIVETYPE)type, numberprimitive, _p_Vertex, m_currentVertexSize);
    endShader(mp_currentShader);
}

}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::setMatrixTransform(MATRIXMODE _type, const GMatrix44* _Matrix)
{
    switch(_type)
    {
    default:
    case GFX_MWORLDTOWIEW:
    case GFX_MWORLD:
        mg_World = *(D3DXMATRIX*)_Matrix;
        break;
    case GFX_MVIEW:
        mg_View = *(D3DXMATRIX*)_Matrix;
        break;
    case GFX_PROJECTION:
        mg_Proj = *(D3DXMATRIX*)_Matrix;
        break;
    case GFX_TEXTURE:
        mg_UVmat = *(D3DXMATRIX*)_Matrix;
        break;
    }

    /// shader matrix.
    mg_WorldViewProj = mg_World * mg_View * mg_Proj;
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::setupViewport(GFX_RECT* _Viewport)
{
    D3DVIEWPORT9 vp;

    vp.X = static_cast<DWORD>(_Viewport->left);
    vp.Y = static_cast<DWORD>(_Viewport->top);
    vp.Width = static_cast<DWORD>(_Viewport->right - _Viewport->left);
    vp.Height = static_cast<DWORD>(_Viewport->bottom - _Viewport->top);
    vp.MinZ = 0.0f;
    vp.MaxZ = 1.0f;
    m_pd3dDevice->SetViewport(&vp);
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::unprojectPoint(const Vec2d& _in, float _viewDistance, const GMatrix44* _MatrixModel, const GMatrix44* _MatrixProj, const int* _View, Vec2d& _out)
{
    D3DVIEWPORT9 vp;
    vp.X = static_cast<DWORD>(_View[0]);
    vp.Y = static_cast<DWORD>(_View[1]);
    vp.Width = static_cast<DWORD>(_View[2]);
    vp.Height = static_cast<DWORD>(_View[3]);
    vp.MinZ = 0.0f;
    vp.MaxZ = 1.0f;

    D3DXVECTOR3 in_vec;
    in_vec.z = 0.0f;

    // project points to create a plane orientated with GeometryBuffer's data
    D3DXVECTOR3 p1;
    D3DXVECTOR3 p2;
    D3DXVECTOR3 p3;
    in_vec.x = 0;
    in_vec.y = 0;
    D3DXVec3Project(&p1, &in_vec, &vp, (D3DXMATRIX*)_MatrixProj, 0, (D3DXMATRIX*)_MatrixModel); 

    in_vec.x = 1;
    in_vec.y = 0;
    D3DXVec3Project(&p2, &in_vec, &vp, (D3DXMATRIX*)_MatrixProj, 0, (D3DXMATRIX*)_MatrixModel); 

    in_vec.x = 0;
    in_vec.y = 1;
    D3DXVec3Project(&p3, &in_vec, &vp, (D3DXMATRIX*)_MatrixProj, 0, (D3DXMATRIX*)_MatrixModel); 

    // create plane from projected points
    D3DXPLANE surface_plane;
    D3DXPlaneFromPoints(&surface_plane, &p1, &p2, &p3);

    // unproject ends of ray
    in_vec.x = vp.Width * 0.5f;
    in_vec.y = vp.Height * 0.5f;
    in_vec.z = -_viewDistance;
    D3DXVECTOR3 t1;
    D3DXVec3Unproject(&t1, &in_vec, &vp, (D3DXMATRIX*)_MatrixProj, 0, (D3DXMATRIX*)_MatrixModel); 

    in_vec.x = _in.m_x;
    in_vec.y = _in.m_y;
    in_vec.z = 0.0f;
    D3DXVECTOR3 t2;
    D3DXVec3Unproject(&t2, &in_vec, &vp, (D3DXMATRIX*)_MatrixProj, 0, (D3DXMATRIX*)_MatrixModel); 

    // get intersection of ray and plane
    D3DXVECTOR3 intersect;
    D3DXPlaneIntersectLine(&intersect, &surface_plane, &t1, &t2);

    _out.m_x = intersect.x;
    _out.m_y = intersect.y;
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::setLookAtMatrix(GMatrix44* _MatrixOut, double _midx, double _midy, float _viewDistance, float _aspect)
{
    const float fov = 0.523598776f;

    f32 midx =(f32)_midx; 
    f32 midy =(f32)_midy; 
    f32 viewDistance = (f32)  _viewDistance;
    f32 aspect = (f32) _aspect;
    
    D3DXVECTOR3 eye(midx, midy, -viewDistance);
    D3DXVECTOR3 at(midx, midy, 1);
    D3DXVECTOR3 up(0, -1, 0);

    D3DXMATRIX tmp;
    D3DXMatrixMultiply((D3DXMATRIX*)_MatrixOut,
        D3DXMatrixLookAtRH((D3DXMATRIX*)_MatrixOut, &eye, &at, &up),
        D3DXMatrixPerspectiveFovRH(&tmp, fov, aspect,
                                   viewDistance * 0.5f,
                                   viewDistance * 2.0f));
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::getViewport(GFX_RECT* _RectOut)
{
    D3DVIEWPORT9 vp;
    m_pd3dDevice->GetViewport(&vp);

    _RectOut->left = vp.X;
    _RectOut->top =  vp.Y;
    _RectOut->right =  vp.Width;
    _RectOut->bottom = vp.Height;
}


//----------------------------------------------------------------------------//
// generateTexture.
//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::generateTexture(Texture* _texture)
{
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::createTexture( Texture* _texture, u32 _sizeX, u32 _sizeY, u32 _mipLevel, Texture::PixFormat _pixformat, u32 _pool, bbool _dynamic )
{
    LPDIRECT3DTEXTURE9 d3dTex = (LPDIRECT3DTEXTURE9)_texture->m_adapterimplementationData;
    
    D3DFORMAT pformat = D3DFMT_A8B8G8R8;
    D3DPOOL pool = D3DPOOL_MANAGED;

    if (_pool == 0)
        pool = D3DPOOL_DEFAULT;
    else if (_pool == 2)
        pool = D3DPOOL_SYSTEMMEM;

    switch (_pixformat)
    {
    case Texture::PF_RGB:
        ITF_ASSERT(0);
        break;

    case Texture::PF_DXT1:
#ifdef ITF_WINDOWS
        pformat = D3DFMT_DXT1;
#else
        pformat = D3DFMT_LIN_DXT1;
#endif //ITF_WINDOWS
        break;
    case Texture::PF_DXT2:
#ifdef ITF_WINDOWS
        pformat = D3DFMT_DXT2;
#else
        pformat = D3DFMT_LIN_DXT2;
#endif //ITF_WINDOWS
        break;
    case Texture::PF_DXT3:
#ifdef ITF_WINDOWS
        pformat = D3DFMT_DXT3;
#else
        pformat = D3DFMT_LIN_DXT3;
#endif //ITF_WINDOWS
        break;
    case Texture::PF_DXT4:
#ifdef ITF_WINDOWS
        pformat = D3DFMT_DXT4;
#else
        pformat = D3DFMT_LIN_DXT4;
#endif //ITF_WINDOWS
        break;
    case Texture::PF_DXT5:
#ifdef ITF_WINDOWS
        pformat = D3DFMT_DXT5;
#else
        pformat = D3DFMT_LIN_DXT5;
#endif //ITF_WINDOWS
        break;

    case Texture::PF_RGBA:
#ifdef ITF_WINDOWS
        pformat = D3DFMT_A8B8G8R8;
#else
        pformat = D3DFMT_LIN_A8R8G8B8;
#endif //ITF_WINDOWS
        break;
    case Texture::PF_L8:
#ifdef ITF_WINDOWS
        pformat = D3DFMT_L8;
#else
        pformat = D3DFMT_LIN_L8;
#endif //ITF_WINDOWS
        break;
    case Texture::PF_A8:
#ifdef ITF_WINDOWS
        pformat = D3DFMT_A8;
#else
        pformat = D3DFMT_LIN_A8;
#endif //ITF_WINDOWS
        break;
    }

    u32 usage = 0;
#ifdef ITF_WINDOWS
    if (_dynamic)
        usage = D3DUSAGE_DYNAMIC;
#endif
    HRESULT hr;
    hr = D3DXCreateTexture(m_pd3dDevice,
                                   _sizeX,
                                   _sizeY,
                                   _mipLevel, usage, pformat, pool, &d3dTex);

    if (FAILED(hr))
    {
        LOG("Direct3D9Texture::createTexture failed."); 
        return;
    }

    _texture->setSize(_sizeX, _sizeY);
    _texture->m_pixFormat = _pixformat;
    _texture->m_mipMapLevels  = _mipLevel;
    _texture->m_adapterimplementationData = d3dTex;
}

void GFXAdapter_Directx9::cleanupTexture(Texture* _texture)
{
    if (!_texture) return;

    if (_texture->m_adapterimplementationData)
    {
#ifdef ITF_SUPPORT_COOKING
        if (!_texture->isShared())
#endif //ITF_SUPPORT_COOKING
        {
            HRESULT hr;
#ifdef USE_TEXTURE_TILE
            if (_texture->isCooked())
            {
                IDirect3DBaseTexture9* basetexture = static_cast< IDirect3DBaseTexture9* >( (D3DTexture*)_texture->m_adapterimplementationData );
                void* textureData = (void*)(basetexture->Format.BaseAddress << 12);
        
                slotAllocatorManager::deallocatePhysicalMemory( textureData );
                //basetexture->Release();
                delete basetexture;
            }
            else
#endif           
                hr = ((LPDIRECT3DTEXTURE9)_texture->m_adapterimplementationData)->Release();
        }
        _texture->m_adapterimplementationData = NULL;
    }
}
//----------------------------------------------------------------------------//
void GFXAdapter_Directx9::cleanupRenderTarget(renderTarget* _target)
{
    if ( _target )
    {
        HRESULT hr;

        if (_target->getSurface())
        {
            ((LPDIRECT3DSURFACE9)_target->getSurface())->Release();
            _target->setSurface(0);
        }

        if ( _target->getTexture()->m_adapterimplementationData )
        {
            hr = ((LPDIRECT3DTEXTURE9)_target->getTexture()->m_adapterimplementationData)->Release();
            _target->getTexture()->m_adapterimplementationData = NULL;
        }
    }
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::cleanupBaseTexture(Texture* _texture)
{
    HRESULT hr;
    if (_texture->m_adapterimplementationData)
    {
        hr = ((LPDIRECT3DTEXTURE9)_texture->m_adapterimplementationData)->Release();
        _texture->m_adapterimplementationData = NULL;
    }
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::setTextureSize(Texture* _texture, const Size& _size )
{
     _texture->setSize((u32)_size.d_width, (u32)_size.d_height);
}


//----------------------------------------------------------------------------//

unsigned char* GFXAdapter_Directx9::grabTexture(Texture* _texture)
{
    return 0;
}


//----------------------------------------------------------------------------//

u32 GFXAdapter_Directx9::getMaxTextureSize()
{
    return m_maxTextureSize;
}

//----------------------------------------------------------------------------//

Size GFXAdapter_Directx9::getAdjustedSize(const Size& sz)
{
    Size s(sz);

    if (!m_supportNPOTTex)
    {
        s.d_width  = static_cast<u32> (getSizeNextPOT((f32)sz.d_width));
        s.d_height = static_cast<u32> (getSizeNextPOT((f32)sz.d_height));
    }
    if (!m_supportNonSquareTex)
        s.d_width = s.d_height = std::max(s.d_width, s.d_height);
    return s;
}

//----------------------------------------------------------------------------//

#define USE_MIPMAP

#ifndef ITF_FINAL
ITF_INLINE bbool isPowerOfTwo( u32 n )
{
    return (n & (n-1)) == 0;
}
#endif //ITF_FINAL

u32 getPixFmtBPP(u32 _pixFmt)
{
    switch(_pixFmt)
    {
        case D3DFMT_DXT1:				return 4;
        case D3DFMT_DXT3:				return 8;
        case D3DFMT_DXT5:				return 8;

        default:						return 0;
    }
}

u32 getBufferSize(u32 _width, u32 _height, u32 _pixelFormat, bbool _bTile32 = bfalse)
{
    u32 BPP = getPixFmtBPP(_pixelFormat);

    switch(_pixelFormat)
    {
        case D3DFMT_DXT1:
        case D3DFMT_DXT3:
        case D3DFMT_DXT5:
            {
                u8	limit = _bTile32 ? 8 : 4;
                u8	minTiles = _bTile32 ? 2 : 1;

                _width = (_width >= limit) ? _width >> 2 : minTiles;
                _height = (_height >= limit) ? _height >> 2 : minTiles;
            }

            return(_width * _height) * BPP * 2;
            break;
    }

    return 0;
}


void GFXAdapter_Directx9::loadFromMemory(   const void* _buffer, const Size& _texture_size, Texture* _texture, Texture::PixFormat _pixel_format ,const u32 _buffer_size,bbool bCooked,u32 _dwMipMapCount )
{
    cleanupTexture(_texture);

    D3DFORMAT pixfmt = D3DFMT_UNKNOWN;
    u32 byte = 8;
    switch (_pixel_format)
    {
    case Texture::PF_RGB:
        ITF_ASSERT(0);
        break;

    case Texture::PF_DXT1:
         byte = 8;
        pixfmt = D3DFMT_DXT1;
        break;
    case Texture::PF_DXT2:
        pixfmt = D3DFMT_DXT2;
            byte = 16;
        break;
    case Texture::PF_DXT3:
        pixfmt = D3DFMT_DXT3;
            byte = 16;
        break;
    case Texture::PF_DXT4:
        pixfmt = D3DFMT_DXT4;
            byte = 16;
        break;
    case Texture::PF_DXT5:
        pixfmt = D3DFMT_DXT5;
            byte = 16;
        break;

    case Texture::PF_RGBA:
#ifdef ITF_WINDOWS
        pixfmt = D3DFMT_A8B8G8R8;
#else
        pixfmt = D3DFMT_LIN_A8R8G8B8;
#endif //ITF_WINDOWS
        break;
    default:
        LOG("Direct3D9Texture::loadFromMemory failed: Invalid PixelFormat value specified."); 
    }

    u32 levelMipmap = 1;

#ifdef USE_MIPMAP
        levelMipmap = 0;
#endif 

    Size tex_sz(getAdjustedSize(_texture_size));

    LPDIRECT3DTEXTURE9 d3dTex = (LPDIRECT3DTEXTURE9)_texture->m_adapterimplementationData;

    HRESULT hr = S_OK;

    if (bCooked)//textureDDS_pixel_format >= Texture::PF_DXT1 && _pixel_format <=Texture::PF_DXT5)
    {
        //Linear format x360
        D3DFORMAT formatCreate = pixfmt;
        
        u32 * currentBuffer = (u32*)((u8*)_buffer+sizeof(ITF::DDS_HEADER));

#ifdef ITF_X360
        switch (pixfmt)
        {
        case D3DFMT_DXT1:
            formatCreate = D3DFMT_LIN_DXT1;
            break;
        case D3DFMT_DXT2:
            formatCreate = D3DFMT_LIN_DXT2;
            break;
    
        case D3DFMT_DXT4:
            formatCreate = D3DFMT_LIN_DXT4;
            break;
        }
#endif //ITF_X360
        hr = D3DXCreateTexture(m_pd3dDevice,
            static_cast<u32>(tex_sz.d_width),
            static_cast<u32>(tex_sz.d_height),
            _dwMipMapCount > 0 ? _dwMipMapCount : 1, 0, formatCreate, D3DPOOL_MANAGED, &d3dTex);

        u32 levelCount = d3dTex->GetLevelCount();

        for( u32 mipLevel = 0; mipLevel != levelCount; ++mipLevel )
        {
            IDirect3DSurface9* mip_surface;
            hr = d3dTex->GetSurfaceLevel(mipLevel, &mip_surface);
            ITF_ASSERT(hr == S_OK);

            D3DSURFACE_DESC pDesc;
            mip_surface->GetDesc(&pDesc);

            u32 width = pDesc.Width;
            u32 height = pDesc.Height;

            ITF_ASSERT(isPowerOfTwo(width));
            ITF_ASSERT(isPowerOfTwo(height));


            D3DLOCKED_RECT lr;
            hr = mip_surface->LockRect(&lr, 0,0);
            ITF_ASSERT(hr == S_OK);

            u32* dst = static_cast<u32*>(lr.pBits);

            u32 count = (std::max((u32) 1, (u32)(width / 4)) * std::max((u32)1, (u32) (height / 4)) *byte)/sizeof(u32); //8(DXT1) or 16(DXT2-5)

            u32* src = static_cast<u32*>(currentBuffer);
            //produce here

            switch ( _pixel_format)
            {
            case Texture::PF_RGBA:
                {
                    ITF_MemcpyWriteCombined(dst,src,(width*height)*(sizeof(u32)));
                    currentBuffer+= (width*height);
                }
                break;

            default:
                {
                   
#ifdef ITF_WINDOWS
                    ITF_MemcpyWriteCombined(dst,src,count*sizeof(u32));
#else
                    ITF_ASSERT(count!=0);

                    u32 imageSize   = getBufferSize(width, height, pixfmt);
                    u32 lineSize    = getBufferSize(width,1,pixfmt);

                    if (lineSize == (u32) lr.Pitch)
                    {
                        ITF_MemcpyWriteCombined(dst, src, imageSize);
                    }
                    else if (lineSize <(u32)  lr.Pitch)
                    {
                        u8* locked = (u8*)lr.pBits;
                        for(u32 i = 0; i < imageSize; i += lineSize)
                        {
                            u8* p = ((u8*)src) + i;
                            ITF_MemcpyWriteCombined(locked, p, lineSize);
                            locked = ((u8 *)locked) + lr.Pitch;
                        }
                    }
                    else
                    {
                        ITF_ASSERT(0);
                    }

#endif //ITF_WINDOWS
                    currentBuffer+= count;
                }
                break;
            }

            mip_surface->UnlockRect();
            mip_surface->Release();
        }

        switch (hr)
        {
            case S_OK:
                hr=hr;
                break;
            default:
                ITF_ASSERT(0);//Wrong DDS Format;
                break;
        }

        _texture->m_adapterimplementationData = d3dTex;

        Size d_dataSize(_texture_size);
        updateTextureSize(_texture);
        Size TexSize(_texture->getSizeX(), _texture->getSizeY());
        updateCachedScaleValues(_texture);

        _texture->m_datasizeX = (u32)d_dataSize.d_width;
        _texture->m_datasizeY = (u32)d_dataSize.d_height;
        
   }
   else
   {
       D3DXCreateTexture(m_pd3dDevice,
                                       static_cast<u32>(tex_sz.d_width),
                                       static_cast<u32>(tex_sz.d_height),
                                       levelMipmap, 0, pixfmt, D3DPOOL_MANAGED, &d3dTex);

        if (FAILED(hr))
        {
            LOG("Direct3D9Texture::loadFromMemory failed: Direct3D9 texture creation failed."); 
            return;
        }
        _texture->m_adapterimplementationData = d3dTex;

        Size d_dataSize(_texture_size);
        updateTextureSize(_texture);
        Size TexSize(_texture->getSizeX(), _texture->getSizeY());
        updateCachedScaleValues(_texture);

        _texture->m_datasizeX = (u32)d_dataSize.d_width;
        _texture->m_datasizeY = (u32)d_dataSize.d_height;

        // lock the D3D texture

        D3DLOCKED_RECT rect;

        {
            hr = d3dTex->LockRect(0, &rect, 0, 0);

            if (FAILED(hr))
            {
                SF_RELEASE(d3dTex);

                LOG("Direct3D9Texture::loadFromMemory failed: IDirect3DTexture9::LockRect failed."); 
            }
            else
            {
            // copy data from buffer into texture
                u32 * dst = static_cast<u32*>(rect.pBits);
            const u32 * src = static_cast<const u32*>(_buffer);

            // RGBA
            if (_pixel_format == Texture::PF_RGBA)
            {
                for (u32 i = 0; i < _texture_size.d_height; ++i)
                {
                    for (u32 j = 0; j < _texture_size.d_width; ++j)
                    {
                        // we dont need endian safety on microsoft
                        u8 r = static_cast<u8>(src[j] & 0xFF);
                        u8 g = static_cast<u8>((src[j] >> 8) & 0xFF);
                        u8 b = static_cast<u8>((src[j] >> 16)  & 0xFF);
                        u8 a = static_cast<u8>((src[j] >> 24) & 0xFF);

                        dst[j] = D3DCOLOR_ARGB(a, r, g, b);
                    }

                    dst += rect.Pitch / sizeof(u32);
                    src += static_cast<u32>(_texture_size.d_width);
                }
            }
            // RGB
            else
            {
                for (u32 i = 0; i < _texture_size.d_height; ++i)
                {
                    for (u32 j = 0; j < _texture_size.d_width; ++j)
                    {
                        u8 r = reinterpret_cast<const u8*>(src)[j * 3];
                        u8 g = reinterpret_cast<const u8*>(src)[j * 3 + 1];
                        u8 b = reinterpret_cast<const u8*>(src)[j * 3 + 2];
                        u8 a = 0xFF;

                        dst[j] = D3DCOLOR_ARGB(a, r, g, b);
                    }

                    dst += rect.Pitch / sizeof(u32);
                    src = reinterpret_cast<const u32*>
                          (reinterpret_cast<const u8*>(src) +
                           static_cast<int>(_texture_size.d_width) * 3);
                }
            }
            d3dTex->UnlockRect(0);
        }
        }

        if (levelMipmap != 1)
        {
            hr = D3DXFilterTexture(d3dTex,NULL,D3DX_DEFAULT , D3DX_FILTER_BOX | D3DX_FILTER_DITHER  );
            ITF_ASSERT(hr == S_OK);
        }
    }
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::updateTextureSize(Texture* _Tex)
{
    D3DSURFACE_DESC surfDesc;

    // obtain details of the size of the texture
    if (_Tex->m_adapterimplementationData && SUCCEEDED(((LPDIRECT3DTEXTURE9)_Tex->m_adapterimplementationData)->GetLevelDesc(0, &surfDesc)))
    {
        _Tex->setSize(static_cast<u32>(surfDesc.Width),static_cast<u32>(surfDesc.Height));
    }
    // use the original size if query failed.
    // NB: This should probably be an exception.
    else
    {
        _Tex->setSize(static_cast<u32>( _Tex->m_datasizeX),static_cast<u32>(_Tex->m_datasizeY));
    }
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::saveToMemory(void* _buffer, Texture* _Tex)
{
    LOG("Direct3D9Texture::saveToMemory - Unimplemented!"); 
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::setSamplerState( u32 _samp, GFX_SAMPLERSTATETYPE _type, i32 _value )
{
    i32 value = 0;
    i32 type = 0;

    switch (_type)
    {
    case GFXSAMP_ADDRESSU:
        type = D3DSAMP_ADDRESSU;
        break;
    case GFXSAMP_ADDRESSV:
        type = D3DSAMP_ADDRESSV;
        break;
    case GFXSAMP_MAGFILTER:
        type = D3DSAMP_MAGFILTER;
        break;
    case GFXSAMP_MINFILTER:
        type = D3DSAMP_MINFILTER;
        break;
    case GFXSAMP_MIPFILTER:
        type = D3DSAMP_MIPFILTER;
        break;
    default:
        // not supported !!.
        ITF_ASSERT(0);
        break;
    }

    switch (_value)
    {
    default:
    case GFXTADDRESS_WRAP:
        value = D3DTADDRESS_WRAP;
        break;
    case GFXTADDRESS_MIRROR:
        value = D3DTADDRESS_MIRROR;
        break;
    case GFXTADDRESS_CLAMP:
        value = D3DTADDRESS_CLAMP;
        break;
    case GFXTADDRESS_BORDER:
        value = D3DTADDRESS_BORDER;
        break;
    }

    m_pd3dDevice->SetSamplerState((DWORD)_samp, (D3DSAMPLERSTATETYPE)type, (DWORD)value);
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::enableRenderTarget(renderTarget* _target)
{
    if (!_target || !_target->getSurface()) return;

    HRESULT hr;
    LPDIRECT3DSURFACE9 oldSurface;
    m_pd3dDevice->GetRenderTarget(0, &oldSurface);
    if (oldSurface && oldSurface != (LPDIRECT3DSURFACE9)_target->getSurface())
    {
        _target->m_prevColourSurface = (uPtr)oldSurface;
        m_currentRenderingSurface = (LPDIRECT3DSURFACE9)_target->getSurface();
        hr = m_pd3dDevice->SetRenderTarget(0, m_currentRenderingSurface);
        RENDER_TARGET_DEBUG = _target->getTexture();
    }
    else if (oldSurface)
        hr = oldSurface->Release();
}

//----------------------------------------------------------------------------//
void GFXAdapter_Directx9::disableRenderTarget(renderTarget* _target)
{
    if (!_target || !_target->getSurface()) return;

    HRESULT hr;

    resolve( (LPDIRECT3DTEXTURE9)_target->getTexture()->m_adapterimplementationData );

    m_pd3dDevice->SetRenderTarget(0, (LPDIRECT3DSURFACE9)_target->m_prevColourSurface);
    m_currentRenderingSurface = (LPDIRECT3DSURFACE9)_target->m_prevColourSurface;

    hr = ((LPDIRECT3DSURFACE9)_target->m_prevColourSurface)->Release();
    //_target->m_prevColourSurface = 0;
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::clearRenderTarget( renderTarget* _target, u32 _color)
{
    // switch to targetting our texture
    enableRenderTarget(_target);
    // Clear it.
    m_pd3dDevice->Clear(0, 0, D3DCLEAR_TARGET, _color, 1.0f, 0);
    // switch back to rendering to previous target
    disableRenderTarget(_target);
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::initialiseRenderTarget( renderTarget* _target)
{
    if (_target->getTexture()->m_adapterimplementationData) return;

    u32    getWidth =  (_target->m_area[2] - _target->m_area[0]);
    u32    getHeight = (_target->m_area[3] - _target->m_area[1]);

#ifdef ITF_X360
    /// max 512*512
    if (getWidth > 512) getWidth = 512;
    if (getHeight > 512) getHeight = 512;
#endif

    Size    tex_sz(getAdjustedSize(Size(getWidth, getHeight)));

    if (!tex_sz.d_height || !tex_sz.d_width) return;

    LPDIRECT3DTEXTURE9 d3dTex = (LPDIRECT3DTEXTURE9)_target->getTexture()->m_adapterimplementationData;
    LPDIRECT3DSURFACE9 surf = (LPDIRECT3DSURFACE9)_target->getSurface();

    createRenderTarget(&d3dTex, &surf, static_cast<UINT>(tex_sz.d_width), static_cast<UINT>(tex_sz.d_height), D3DFMT_A8R8G8B8, btrue);

    if (!d3dTex) 
        return;

    _target->setSurface(uPtr(surf));


    _target->getTexture()->m_adapterimplementationData = d3dTex;

     // Set Original data Size.
    Texture *texture = _target->getTexture();
    
    texture->m_datasizeX = (u32)getWidth;
    texture->m_datasizeY = (u32)getHeight;

    texture->setSize(getWidth, getHeight);
    updateCachedScaleValues(_target->getTexture());
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::resizeRenderTarget( renderTarget *_target , const Size& _size)
{
    _target->clean();

    _target->m_area[0] = 0;
    _target->m_area[1] = 0;
    _target->m_area[2] = (int)_size.d_width;
    _target->m_area[3] = (int)_size.d_height;

    initialiseRenderTarget(_target);
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::createRenderTarget(LPDIRECT3DTEXTURE9* _ptex, LPDIRECT3DSURFACE9* _psurf, u32 _width, u32 height, D3DFORMAT _format, bbool _shift)
{
    HRESULT hr;
#ifdef ITF_WINDOWS
    hr = D3DXCreateTexture( m_pd3dDevice, _width, height,
                        1, D3DUSAGE_RENDERTARGET, _format, D3DPOOL_DEFAULT, _ptex );
    
    (*_ptex)->GetSurfaceLevel( 0, _psurf );

    /// clear rendertarget.
    LPDIRECT3DSURFACE9 oldSurface;
    m_pd3dDevice->GetRenderTarget(0, &oldSurface);

    m_pd3dDevice->SetRenderTarget(0, *_psurf);
    m_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, 0x00000000,  1.0f, 0 );
    m_pd3dDevice->SetRenderTarget(0, oldSurface);
    oldSurface->Release();

#else
    hr = m_pd3dDevice->CreateTexture( _width, height, 1, 0,
                                         _format,
                                         D3DPOOL_DEFAULT, _ptex,
                                         NULL );

    D3DSURFACE_PARAMETERS SurfaceParameters;
    ITF_MemSet( &SurfaceParameters, 0, sizeof( D3DSURFACE_PARAMETERS ) );

    if (_shift)
    {
        u32 surfacesize = XGSurfaceSize( getScreenWidth(), getScreenHeight(),
                                            D3DFMT_A8B8G8R8, D3DMULTISAMPLE_NONE );
        
        SurfaceParameters.Base = surfacesize;
 
        if (isUseDepthBuffer())
        {

#ifdef _USE_ZBUFFER_TILING
        SurfaceParameters.Base += surfacesize;/*XGHierarchicalZSize( getScreenWidth(),
                                                               getScreenHeight(),
                                                               D3DMULTISAMPLE_NONE );*/
#else
        SurfaceParameters.Base += surfacesize;
        
            
            /*SurfaceParameters.HierarchicalZBase = XGHierarchicalZSize( getScreenWidth(),
                                                               getScreenHeight(),
                                                               D3DMULTISAMPLE_NONE );*/
#endif 
        }
           
        SurfaceParameters.HierarchicalZBase = 0;
     }

    hr = m_pd3dDevice->CreateRenderTarget( _width, height, _format,
                                  D3DMULTISAMPLE_NONE, 0, FALSE, _psurf,
                                  &SurfaceParameters );
#endif
}

//----------------------------------------------------------------------------//

#ifdef ITF_X360
void  GFXAdapter_Directx9::acquireDeviceOwnerShip()
{

    if (Synchronize::getCurrentThreadId() != ThreadSettings::m_settings[eThreadId_mainThread].m_threadID && m_pd3dDevice)
    {
        //ask the main thread to release the owner ship
        {
            ITF_MemoryBarrier();
            {
                csAutoLock cs(m_csPresent);
                m_askToReleaseOwnerShip = btrue;
            }
            //wait until is done;
            Synchronize::waitEvent(&m_ReleaseOwnerShipEvent);
    
            Synchronize::resetEvent(&m_ReleaseOwnerShipEvent);
            m_pd3dDevice->AcquireThreadOwnership();
        }
    }

}

void GFXAdapter_Directx9::releaseDeviceOwnerShip()
{
    if (Synchronize::getCurrentThreadId() != ThreadSettings::m_settings[eThreadId_mainThread].m_threadID && m_pd3dDevice)
    {
        Synchronize::setEvent(&m_AcquireOwnerShipEvent);
        m_pd3dDevice->ReleaseThreadOwnership();
    }
 
    
}

void GFXAdapter_Directx9::minimal_present()
{
    if (m_pd3dDevice)
    {
        clear( GFX_CLEAR_COLOR, 0, 0, 0, 0); 
        m_pd3dDevice->Present(0, 0, 0, 0);
    }
}
#endif //ITF_X360

void GFXAdapter_Directx9::present()
{
    TIMERASTER_SCOPE(present);
    m_pd3dDevice->BeginScene();
    m_pd3dDevice->SetVertexShader( NULL );
    setUseFog(bfalse);
    setShader(mp_defaultShader);
    setGlobalColor(Color::white());

    if (getOverDrawMode())
        showOverDrawRecord();

    setAlphaBlend(GFX_BLEND_ALPHA);

    /// raster.
#ifdef ITF_X360
    //if (!isShowRasters()) toggleRaster();
#endif

	M_RASTER_DRAW();

#ifndef ITF_FINAL
    if (isShowDebugInfo())
        getVertexBufferManager().drawDebugVBInfo();
#endif // ITF_FINAL
    
#if ITF_DEBUG_LEVEL > 0
    {
        for (u32 it = 0; it < m_DBGTextureQuads.size();)
        {
            DBGTextureQuad& t = m_DBGTextureQuads[it];
            for (int i = 0; i < 4; i++)
            {
                t.mVtx[i].m_pos.m_y = getScreenHeight() - t.mVtx[i].m_pos.m_y;
                t.mVtx[i].m_pos.m_z = 0.f;
            }
            drawTriangle(t.mVtx[0], t.mVtx[1], t.mVtx[2], t.t);
            drawTriangle(t.mVtx[1], t.mVtx[2], t.mVtx[3], t.t);
            t.duration -= LOGICDT;
            if (t.duration <= 0.0f)
                m_DBGTextureQuads.eraseKeepOrder(it);
            else
                it++;
        }
    }
#endif //  ITF_DEBUG_LEVEL > 0

    {
#ifdef ITF_WINDOWS
        m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE); 
#endif // 
        //m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
        //m_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
        //m_pd3dDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);

        /// set first the shader you want to use.
        setShader(mp_defaultShader);

        /// set the matrix.
        D3DXMATRIX World;
        D3DXMatrixIdentity(&World);
        setObjectMatrix(World);

        /// set Vertex format
        setVertexFormat(VertexFormat_PC);

        for (u32 it = 0; it < m_Box2DList.size(); ++it)
        {
            const Box2D& rect = m_Box2DList[it];

            DrawPrimitive(GFX_TRIANGLE_STRIP, (void*)rect.v, 4);
        }
        m_Box2DList.clear();
    }

    for (u32 it = 0; it < m_Triangle2DList.size(); ++it)
    {
        const Triangle2D& tri = m_Triangle2DList[it];

        DrawPrimitive(GFX_TRIANGLES, (void*)tri.v, 3);
    }
    m_Triangle2DList.clear();

#ifndef ITF_CONSOLE_FINAL
    // safe Frame.
    if (safeFrameDrawRequested())
        drawSafeFrame();
#endif // ITF_CONSOLE_FINAL

#ifdef _USE_FONTDEBUG

#ifdef ITF_X360
    /// Debug info.
    /// Show fps.

    m_FontDebug.SetScaleFactors( 1.f, 1.f );

    RECT rct;
    rct.left=80;
    rct.right=780;
    rct.top=40;
    rct.bottom=rct.top+20;

#ifndef ITF_DISABLE_DEBUGINFO
    if (getfPs()>0.0f && getDisplayDebugStats())
    {
        wchar_t cfps[128];
        #define MB	(1024*1024)

        MEMORYSTATUS stat;
        // Get the memory status.
        GlobalMemoryStatus( &stat );
        f32 fps = getfPs();
        swprintf_s( cfps, 128, L"%0.02f fps Mem free :%d Mb %0.02f ms", fps,stat.dwAvailPhys / MB ,fps>=0.01f ? 1000.0f/fps : 0.0f);
        m_FontDebug.DrawText( (f32) (m_screenWidth- rct.left-390), (f32)rct.top+20, COLOR_WHITE, cfps); 
        m_FontDebug.DrawText( (f32) (m_screenWidth- rct.left-550), (f32)rct.top+40, COLOR_WHITE, (wchar_t*)m_extraInfo.cStr()); 
        m_FontDebug.DrawText( (f32) (m_screenWidth- rct.left-390), (f32)rct.top+60, COLOR_WHITE, (wchar_t*)m_engineDataVersion.cStr()); 
    }
#endif //ITF_DISABLE_DEBUGINFO

    rct.top=getScreenHeight()-30;
    rct.bottom=rct.top+20;

    ITF_VECTOR<DBGText>::iterator it = m_DBGTexts.begin();
    ITF_VECTOR<DBGText>::iterator end = m_DBGTexts.end();
    for (; it != end; ++it)
    {
        u32 color = D3DCOLOR_ARGB(255, (u32)(it->m_r*255.f), (u32)(it->m_g*255.f), (u32)(it->m_b*255.f));

        if ((*it).m_x < 0.0f && (*it).m_y < 0.0f)
        {
            rct.top -= 20;
            rct.bottom = rct.top+20;
            m_FontDebug.DrawText( (f32)rct.left, (f32)rct.top, color, (wchar_t *)((*it).m_text.cStr()));
        }
        else
        {
            RECT localRect;
            localRect.left=(i32)((*it).m_x);
            localRect.right=getScreenWidth();
            localRect.top=(i32)((*it).m_y);
            localRect.bottom=rct.top+20;
            m_FontDebug.DrawText( (f32)localRect.left, (f32)localRect.top, color, (wchar_t *)((*it).m_text.cStr()));
        }
    }
    m_DBGTexts.clear();    

#else
    /// Debug info.
    /// Show fps.
#ifndef ITF_FINAL
    RECT rct;
    rct.left=10;
    rct.right=780;
    rct.top=10;
    rct.bottom=rct.top+20;

    if (isShowDebugInfo())
    {
        wchar_t fps[128];
        swprintf_s( fps,128,L"%0.02f fps %ls", getfPs(),(wchar_t*)m_extraInfo.cStr());
        mp_FontDebug->DrawText(NULL, fps, -1 ,&rct, 0, COLOR_WHITE);

        u32 color = 0x80000000;
        GFX_ADAPTER->draw2dBox( Vec2d(0.1f , (f32) rct.top), 250,20, color, color, color, color);
    }

    rct.top=getScreenHeight()-30;
    rct.bottom=rct.top+20;

    static ITF_VECTOR<DBGText> stackDBGTexts;

    ITF_VECTOR<DBGText>::iterator it = m_DBGTexts.begin();
    ITF_VECTOR<DBGText>::iterator end = m_DBGTexts.end();
    for (; it != end; ++it)
    {
        if ((*it).m_x < (ITF_POS2D_AUTO+0.1f) && (*it).m_y < (ITF_POS2D_AUTO+0.1f) && (it->m_alignment == FONT_ALIGN_LEFT))
        {
            stackDBGTexts.push_back(*it);
        }
        else
        {
            const u32 color = D3DCOLOR_ARGB(255, (u32)(it->m_r*255.f), (u32)(it->m_g*255.f), (u32)(it->m_b*255.f));
            RECT localRect;
            DWORD alignment = fontAlignmentToDirectXFormat(it->m_alignment);
            localRect.left=(i32)((*it).m_x);
            localRect.right=getScreenWidth();
            localRect.top=(i32)((*it).m_y);
            localRect.bottom=rct.top+20;
			if ((*it).m_useMonospaceFont)
				mp_FontDebug_Monospace->DrawTextW(NULL, (LPCWSTR)(*it).m_text.cStr(), -1 ,&localRect, alignment, color);
			else
            if ((*it).m_useBigFont)
                mp_FontDebug_Big->DrawTextW(NULL, (LPCWSTR)(*it).m_text.cStr(), -1 ,&localRect, alignment, color);
            else
                mp_FontDebug->DrawTextW(NULL, (LPCWSTR)(*it).m_text.cStr(), -1 ,&localRect, alignment, color);
        }
    }
    m_DBGTexts.clear();

    if(stackDBGTexts.size())
    {
        const u32 uColor     = ITFCOLOR_TO_U32(150, 100, 100, 100);

        it = stackDBGTexts.begin();
        end = stackDBGTexts.end();
        while(it != end)
        {
            const DBGText& txt = (*it);
            const u32 color = D3DCOLOR_ARGB(255, (u32)(txt.m_r*255.f), (u32)(txt.m_g*255.f), (u32)(txt.m_b*255.f));

            rct.top -= 15;
            rct.bottom = rct.top+15;
            
            draw2dBox(Vec2d(f32(rct.left - 2), f32(rct.top)), txt.m_text.getLen() * 7.0f + 2, 15.0f, uColor, uColor, uColor, uColor);
            DWORD alignment = fontAlignmentToDirectXFormat(txt.m_alignment);

			if ((*it).m_useMonospaceFont)
				mp_FontDebug_Monospace->DrawTextW(NULL, (LPCWSTR)txt.m_text.cStr(), -1 ,&rct, alignment, color);
			else
            if ((*it).m_useBigFont)
                mp_FontDebug_Big->DrawTextW(NULL, (LPCWSTR)txt.m_text.cStr(), -1 ,&rct, alignment, color);
            else
                mp_FontDebug->DrawTextW(NULL, (LPCWSTR)txt.m_text.cStr(), -1 ,&rct, alignment, color);

            ++it;
        }
        stackDBGTexts.clear();
    }
#endif //ITF_FINAL
#endif
#endif

#if 0
     /// test impostor.
    if (RENDER_TARGET_DEBUG)
    {
        setAlphaBlend(GFX_BLEND_COPY);
        SetTextureBind(0,(u32)RENDER_TARGET_DEBUG->m_adapterimplementationData);
        m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
        m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
        setShader(mp_defaultShader);
        drawScreenQuad(0,0,(f32)getScreenWidth(), (f32)getScreenHeight(),0, COLOR_WHITE );
    }
#endif

    m_pd3dDevice->EndScene();
    resetSceneResolvedFlag();

#ifdef VBMANAGER_USE_DYNAMICRING_VB
    m_VertexBufferManager.DynamicRingVBendDraw();
#endif

#ifndef _USE_ZBUFFER_TILING
    /// Present frame buffer
    HRESULT hr;
    hr = m_pd3dDevice->Present(0, 0, 0, 0);
#else
    m_pd3dDevice->SetRenderTarget( 0, m_pSceneSurf );
    m_pd3dDevice->SynchronizeToPresentationInterval();

    D3DVECTOR4 ClearColor = { 0, 0, 0, 0 };

    // Resolve the rendered scene back to the front buffer.
    m_pd3dDevice->Resolve( D3DRESOLVE_RENDERTARGET0,// | D3DRESOLVE_CLEARRENDERTARGET,
                           NULL,
                           m_pSceneTexture,
                           NULL,
                           0, 0,
                           &ClearColor, 1.0f, 0, NULL );

    // Swap to the current front buffer, so we can see it on screen.
    m_pd3dDevice->Swap( m_pSceneTexture, NULL );
#endif

    
#ifdef ITF_X360
    MemoryBarrier();
    bbool valRelease = bfalse;//TODO interlock
    csAutoLock cs(m_csPresent);     
    {
        valRelease = m_askToReleaseOwnerShip;
    }

    if (valRelease)
    {
        m_pd3dDevice->ReleaseThreadOwnership();
        Synchronize::setEvent(&m_ReleaseOwnerShipEvent);
        Synchronize::waitEvent(&m_AcquireOwnerShipEvent);
        Synchronize::resetEvent(&m_AcquireOwnerShipEvent);
        {   
            csAutoLock cs(m_csPresent);  
            m_askToReleaseOwnerShip = bfalse;
        }

        m_pd3dDevice->AcquireThreadOwnership();
    }
        
#endif  //ITF_X360
}

DWORD GFXAdapter_Directx9::fontAlignmentToDirectXFormat(FONT_ALIGNMENT _alignment)
{
#ifdef ITF_WINDOWS
    switch (_alignment)
    {
        case FONT_ALIGN_LEFT: return DT_LEFT;
        case FONT_ALIGN_CENTER : return DT_CENTER;
        case FONT_ALIGN_RIGHT : return DT_RIGHT;
        default : ITF_ASSERT_MSG(bfalse, "Unknown font alignment"); return DT_LEFT;
    }
#else
    return 0;
#endif //ITF_WINDOWS
}


#ifdef ITF_SUPPORT_VIDEOCAPTURE

void   GFXAdapter_Directx9::startScreenCapture()
{
    if (m_videoCapture)
    {
    IDirect3DSurface9* BackBuffer = NULL;

    m_pd3dDevice->GetRenderTarget(0, &BackBuffer);
    D3DSURFACE_DESC desc;
    HRESULT            hr;
    hr = BackBuffer->GetDesc(&desc);
    ITF_ASSERT(hr == S_OK);

        m_videoCapture->start(desc.Width,desc.Height);
    }


}


class VideoCaptureBuffer_Directx9 : public VideoCaptureBuffer
{
public:

    void release();
    void lock();
    void unlock();

};

void GFXAdapter_Directx9::doScreenCapture()
{
   TIMERASTER_SCOPE(doScreenCapture);
   if (m_videoCapture && m_videoCapture->isEnable())
       {
           bbool hasCaptureBuffer = m_videoCapture->hasCaptureBuffer();
           bbool resetMode        = (hasCaptureBuffer && !m_videoCapture->getCaptureBuffer()->getOffscreenBuffer());
           if (!hasCaptureBuffer || resetMode)
           {
               IDirect3DSurface9* BackBuffer = NULL;
               LPDIRECT3DSURFACE9 offscreen  = NULL;
               LPDIRECT3DSURFACE9 stretchscreen = NULL;
               m_pd3dDevice->GetRenderTarget(0, &BackBuffer);
               D3DSURFACE_DESC desc;
               HRESULT            hr;
               hr = BackBuffer->GetDesc(&desc);
               ITF_ASSERT(hr == S_OK);

               LPDIRECT3DTEXTURE9 texture = NULL;

               if (m_videoCapture->useLowResolution())
               {
                   desc.Height = (u32)((f32)((desc.Height*640.0f)/desc.Width));
                   desc.Width = 640;

                   if (desc.Height&1)
                        desc.Height = desc.Height+1;

                  
                   hr = D3DXCreateTexture(m_pd3dDevice, desc.Width,desc.Height, 1, D3DUSAGE_RENDERTARGET, desc.Format, D3DPOOL_DEFAULT, &texture);
                   ITF_ASSERT(hr == S_OK);

                   hr = texture->GetSurfaceLevel(0, &stretchscreen);
                   ITF_ASSERT(hr == S_OK);

                   texture->Release();
               }
           
               
               hr = m_pd3dDevice->CreateOffscreenPlainSurface(desc.Width, desc.Height,desc.Format,  D3DPOOL_SYSTEMMEM , &offscreen, NULL);


               VideoCaptureBuffer_Directx9* videoCaptureBuffer = new VideoCaptureBuffer_Directx9();
               m_videoCapture->assignCaptureOffscreenBuffer(videoCaptureBuffer);
               videoCaptureBuffer->setOffscreenBuffer(desc.Width,desc.Height,offscreen,stretchscreen,texture);

               BackBuffer->Release();
           }

           ITF_ASSERT(m_videoCapture->hasCaptureBuffer());
           VideoCaptureBuffer* pVideoCaptureBuffer =  m_videoCapture->getCaptureBuffer();

           IDirect3DSurface9* BackBuffer = NULL;

           LPDIRECT3DSURFACE9 offscreen = (LPDIRECT3DSURFACE9) m_videoCapture->getCaptureBuffer()->getOffscreenBuffer();
         
           HRESULT hr = m_pd3dDevice->GetRenderTarget(0, &BackBuffer);
           ITF_ASSERT(hr == S_OK);

           if (m_videoCapture->useLowResolution())
           {
               LPDIRECT3DSURFACE9 stretchscreen = (LPDIRECT3DSURFACE9) m_videoCapture->getCaptureBuffer()->getStretchScreenBuffer();
               ITF_ASSERT(stretchscreen);

               D3DSURFACE_DESC desc;
               hr = stretchscreen->GetDesc(&desc);
               ITF_ASSERT(hr == S_OK);

               RECT rcDest= {0, 0, static_cast<LONG>(desc.Width), static_cast<LONG>(desc.Height)};
               hr= m_pd3dDevice->StretchRect(BackBuffer,NULL,stretchscreen,&rcDest,D3DTEXF_LINEAR);
               ITF_ASSERT(hr == S_OK);

               hr = m_pd3dDevice->GetRenderTargetData(stretchscreen, offscreen);
               ITF_ASSERT(hr == S_OK);
           }
           else
           {
               hr = m_pd3dDevice->GetRenderTargetData(  BackBuffer,offscreen);
           }

           if (hr==S_OK) //if for any reasons ,we failed ( screensaver..) ,we skip this frame
           {
               
               pVideoCaptureBuffer->lock();

               m_videoCapture->resolveBufferScreen();

               pVideoCaptureBuffer->unlock(); 
               
           }

           BackBuffer->Release();
           
       }
 }

#endif //ITF_SUPPORT_VIDEOCAPTURE


#ifdef ITF_WINDOWS
#define PICKING_RENDER_WIDTH 2048

void GFXAdapter_Directx9::setRenderTarget_Picking()
{
    if (!m_pRenderTargetSurfacePicking)
    {
        IDirect3DSurface9* origTarget_ = NULL;

        m_pd3dDevice->GetRenderTarget( 0, &origTarget_ );
        if (!origTarget_)
            return;

        D3DSURFACE_DESC desc;
        origTarget_->GetDesc( &desc );

        //create a render target with the half size of the backbuffer (height)
        desc.Width  = desc.Width;
        desc.Height = desc.Height>>1;

        IDirect3DSurface9* surface_ = NULL;

        HRESULT hr = m_pd3dDevice->CreateRenderTarget(desc.Width,desc.Height,desc.Format, desc.MultiSampleType, desc.MultiSampleQuality,TRUE,&surface_,NULL);
        if (hr!=S_OK)
            messageD3DError(hr);

        m_pRenderTargetSurfacePicking = surface_;
        origTarget_->Release();

        hr = m_pd3dDevice->CreateOffscreenPlainSurface(desc.Width, desc.Height,desc.Format,  D3DPOOL_SYSTEMMEM, &m_pPickingOffScreenBufferSurf, NULL);
        if (hr!=S_OK)
            messageD3DError(hr);
    }

    ITF_ASSERT(m_pRenderTargetSurfacePicking);


    if (m_pRenderTargetSurfacePicking)
    {
        D3DSURFACE_DESC desc;
        m_pRenderTargetSurfacePicking->GetDesc( &desc );

        GFX_RECT rcViewport;
        rcViewport.left = 0;
        rcViewport.right = desc.Width;
        rcViewport.top =  0;
        rcViewport.bottom =  desc.Height;

        setupViewport(&rcViewport);
        m_pd3dDevice->SetRenderTarget(0,m_pRenderTargetSurfacePicking);
    }
}   

void* GFXAdapter_Directx9::resolvePicking()
{
    if (isDeviceLost())
        return NULL;

    m_pd3dDevice->GetRenderTargetData(m_pRenderTargetSurfacePicking,m_pPickingOffScreenBufferSurf);

    return m_pPickingOffScreenBufferSurf;
}

void  GFXAdapter_Directx9::setRenderTarget_BackBuffer()
{
    GFX_RECT rcViewport;
    rcViewport.left = 0;
    rcViewport.right = getScreenWidth();
    rcViewport.top =  0;
    rcViewport.bottom = getScreenHeight();

    setupViewport(&rcViewport);
    if (m_pUltraHDBackBufferSurf)
        m_pd3dDevice->SetRenderTarget(0,m_pUltraHDBackBufferSurf);
    else
    m_pd3dDevice->SetRenderTarget(0,m_pBackBufferSurf);
}

Color GFXAdapter_Directx9::getBufferValue(void* _pBackBuffer,u32 _posX, u32 _posY)
{
    if (_posX>=(u32)getScreenWidth() || _posY>=(u32)getScreenHeight() )
        return Color::zero();

    IDirect3DSurface9* BackBuffer =(IDirect3DSurface9*) _pBackBuffer;

    D3DSURFACE_DESC desc;
    HRESULT hr  = BackBuffer->GetDesc(&desc);
    ITF_ASSERT_MSG(hr == S_OK, "[GFXAdapter_Directx9::getBackBufferValue] BackBuffer->GetDesc(&desc) FAILED");

    RECT rect;
    rect.left = _posX;
    rect.right = _posX+1;
    rect.top = _posY;
    rect.bottom = _posY+1;


    D3DLOCKED_RECT lockedrect;
    hr = BackBuffer->LockRect(&lockedrect,&rect,D3DLOCK_READONLY);
    ITF_ASSERT_MSG(D3D_OK == hr, "DX9::LockRect failed");
    if(D3D_OK == hr)
    {
        u32 pixel = *(u32*)lockedrect.pBits;
        //format is A8R8G8B8
        const u8 a = ITFCOLOR_U32_A(pixel);
        const u8 r = ITFCOLOR_U32_R(pixel);
        const u8 g = ITFCOLOR_U32_G(pixel);
        const u8 b = ITFCOLOR_U32_B(pixel);

        BackBuffer->UnlockRect();

        return Color(ITFCOLOR_U8_TO_F32(a), ITFCOLOR_U8_TO_F32(r), ITFCOLOR_U8_TO_F32(g), ITFCOLOR_U8_TO_F32(b));
    }
    else
        return Color::zero();
}

#endif //ITF_WINDOWS

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::AFTERFX_startRenderTarget(Color _color)
{
    /// already set ?
    if (m_currentRenderingSurface == m_pFullRTsurf[0]) return;
    
    if (!m_pFullRTtex[0])
    {
        createRenderTarget(&m_pFullRTtex[0], &m_pFullRTsurf[0], getScreenWidth(), getScreenHeight(), D3DFMT_A8R8G8B8);
    }
    if (!m_pFullRTtex[1])
    {
        createRenderTarget(&m_pFullRTtex[1], &m_pFullRTsurf[1], getScreenWidth(), getScreenHeight(), D3DFMT_A8R8G8B8);
    }

#ifdef ITF_X360
    // save current backbuffer image to fulltex1
    if (m_currentRenderingSurface == m_pBackBufferSurf)
          copyCurrentColorBuffer(0);
#endif

//#ifndef ITF_X360
    /// afterFx switch Target.
    m_pd3dDevice->SetRenderTarget( 0, m_pFullRTsurf[1] );
    m_currentRenderingSurface = m_pFullRTsurf[1];

    clear(GFX_CLEAR_COLOR, _color.getRed(), _color.getGreen(), _color.getBlue(), _color.getAlpha()); 
//#endif
    /// z write and alpha setting.
    setDefaultAlpha();
    //rC_setZWRITE(btrue);
    //rC_setZTEST(btrue);
}

void GFXAdapter_Directx9::setDefaultAlpha()
{
    setAlphaTest(bfalse);
    setAlphaRef();
    m_pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
}

void GFXAdapter_Directx9::setAlphaTest(bbool _enabled)
{
    m_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, (_enabled?TRUE:FALSE));
    m_isAlphaRefEnabled = _enabled;
}


void GFXAdapter_Directx9::setAlphaRef(int _ref)
{
    m_pd3dDevice->SetRenderState(D3DRS_ALPHAREF, _ref);
}

void GFXAdapter_Directx9::startScreenshotMode()
{
#ifdef ITF_WINDOWS
    HRESULT hr;

    m_preUHDScreenWidth = getScreenWidth();
    m_preUHDScreenHeight = getScreenHeight();

    /// compute UHD backbuffer size, taking current aspect ration into account
    u32 UHDWidth = 4096;
    u32 UHDHeight = 4096;
 
    if (getStrSceenshotResquest() == strScreenshotUHD())
    {
        if (m_preUHDScreenWidth>m_preUHDScreenHeight)
            UHDHeight = (UHDWidth*m_preUHDScreenHeight)/m_preUHDScreenWidth;
        else
            UHDWidth = (UHDHeight*m_preUHDScreenWidth)/m_preUHDScreenHeight;
    }
    else // strScreenshotNormal() || strScreenshotProfileFPS()
    {
        UHDWidth = m_preUHDScreenWidth;
        UHDHeight = m_preUHDScreenHeight;
    }

    ITF_ASSERT_MSG(m_pUltraHDBackBufferSurf == NULL, "Ultra HD backbuffer is already in use");

    hr = m_pd3dDevice->CreateRenderTarget(UHDWidth, UHDHeight, D3DFMT_A8R8G8B8, D3DMULTISAMPLE_NONE, 0, FALSE, &m_pUltraHDBackBufferSurf, NULL );
    if(SUCCEEDED(hr))
    {
        m_pd3dDevice->SetRenderTarget(0, m_pUltraHDBackBufferSurf);
        m_currentRenderingSurface = m_pUltraHDBackBufferSurf;

        forceScreenSize(UHDWidth,UHDHeight);
        m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET , D3DCOLOR_ARGB(0,0,0,0), 0, 0);

        reshape();
    }
    else
        m_pUltraHDBackBufferSurf = NULL;


    setSeparateAlpha(btrue);
#endif //ITF_WINDOWS
}

void GFXAdapter_Directx9::endScreenshotMode()
{
#ifdef ITF_SUPPORT_EDITOR
    HRESULT hr;
    //restore normal render target
    ITF_ASSERT(m_pBackBufferSurf);
    m_pd3dDevice->SetRenderTarget(0, m_pBackBufferSurf);
    //get and save surface
    IDirect3DSurface9 *pSystemMemSurface=NULL;
    hr = m_pd3dDevice->CreateOffscreenPlainSurface(getScreenWidth(), getScreenHeight(),
        D3DFMT_A8R8G8B8,
        D3DPOOL_SYSTEMMEM,
        &pSystemMemSurface,
        NULL);
    if (SUCCEEDED(hr))
    {

        hr = m_pd3dDevice->GetRenderTargetData(m_pUltraHDBackBufferSurf, pSystemMemSurface);
        if (SUCCEEDED(hr))
        {
            if (SUCCEEDED(hr))
            {
                if (getStrSceenshotResquest() == strScreenshotNormal() || getStrSceenshotResquest() == strScreenshotUHD())
                {
                    char myDocumentPath[MAX_PATH];
                    strcpy(myDocumentPath, "c:\\");
                    PIDLIST_ABSOLUTE idList=NULL;
                    SHGetFolderLocation(NULL, CSIDL_MYPICTURES, NULL, 0, &idList);
                    ITF_ASSERT(idList!=NULL);
                    SHGetPathFromIDListA(idList, myDocumentPath);

                    String path, path2;
                    int screenshotIndex=1;

                    for(;;) 
                    {
                        char name[256];
                        path = myDocumentPath;
                        
                        if (getStrSceenshotResquest() == strScreenshotNormal())
                        {
                            sprintf(name, "\\UBI screenshot %03d.jpg", screenshotIndex);
                            path+=name;
                            if (FILEMANAGER->fileExists(path))
                                screenshotIndex++;
                            else
                            {
                                break;
                            }
                        }
                        else if (getStrSceenshotResquest() == strScreenshotUHD())
                        {
                            sprintf(name, "\\UBI Art framework - screenshot %03d.png", screenshotIndex);
                            path+=name;
                            //sprintf(name, "\\UBI Art framework - screenshot %03d.bmp", screenshotIndex);
                            //path2 = String(myDocumentPath) + name;
                            if (FILEMANAGER->fileExists(path))// || FILEMANAGER->fileExists(path2))
                                screenshotIndex++;
                            else
                            {
                                break;
                            }
                        }
                    }

                    if (getStrSceenshotResquest() == strScreenshotUHD())
                    {
                        if (FAILED(D3DXSaveSurfaceToFileW((wchar_t*)path.cStr(), D3DXIFF_PNG, pSystemMemSurface, NULL, NULL)))
                        {
                            //ITF_WARNING(NULL, bfalse, "could not write : %ls", path.cStr());
                        }
                        /*if (FAILED(D3DXSaveSurfaceToFileW((wchar_t*)path2.cStr(), D3DXIFF_BMP, pSystemMemSurface, NULL, NULL)))
                        {
                            //ITF_WARNING(NULL, bfalse, "could not write : %ls", path.cStr());
                        }*/
                    }
                    else if (getStrSceenshotResquest() == strScreenshotNormal())
                    {
                        if (FAILED(D3DXSaveSurfaceToFileW((wchar_t*)path.cStr(), D3DXIFF_JPG, pSystemMemSurface, NULL, NULL)))
                        {
                            //ITF_WARNING(NULL, bfalse, "could not write : %ls", path.cStr());
                        }
                    }

                    if (DEBUGINFO)
                    {
                        String info("Saved ScreenShot to: ");
                        info += path.cStr();
                        DEBUGINFO->addDebugInfoDisplay(info, 5.0f);
                    }
                }
              
                else if (getStrSceenshotResquest() == strScreenshotProfileFPS())
                {
                
                    LPD3DXBUFFER pBuffer;
                    if (!FAILED(D3DXSaveSurfaceToFileInMemory(&pBuffer , D3DXIFF_JPG, pSystemMemSurface, NULL, NULL)))
                    {
                        void* ptr = (pBuffer )->GetBufferPointer();
                        UINT len = (pBuffer )->GetBufferSize();
        
        
                        if(!PLUGINGATEWAY)
                            return;
                        EngineMonitorPlugin *plug = (EngineMonitorPlugin*) PLUGINGATEWAY->getPluginByName(EngineMonitorPlugin::getPluginName());
                        if(!plug)
                            return;
        
                        char szFullFileName[256];
                        sprintf(szFullFileName, "%s.jpg", getStrSceenshotResquest());
                        
                        plug->sendFile(szFullFileName, (char*) ptr, len, false, true);

                    }        
                    pBuffer ->Release();
                }

            }


        }
        pSystemMemSurface->Release();
    }

    // restore sizes
    forceScreenSize(m_preUHDScreenWidth, m_preUHDScreenHeight);
    reshape();

    //release and NULL UHD backbuffer
    m_pUltraHDBackBufferSurf->Release();
    m_pUltraHDBackBufferSurf = NULL;

    resetScreenShotRequest();

    setSeparateAlpha(bfalse);
#endif // ITF_SUPPORT_EDITOR
}

void GFXAdapter_Directx9::startRendering()
{
    HRESULT hr;
    if (isRender) return;

    m_currentRenderingSurface = m_pBackBufferSurf;

#if defined(ITF_WINDOWS) 
    if (getStrSceenshotResquest())
        startScreenshotMode();
#endif //ITF_WINDOWS

    hr = m_pd3dDevice->BeginScene();
    isRender = 1;
 
    if (getOverDrawMode() == 1)
    {
        startOverDrawRecord();
    }
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::endRendering()
{
    if (!isRender) return;
    HRESULT hr;
    
    hr = m_pd3dDevice->EndScene();

    if (m_pUltraHDBackBufferSurf)
    {
        endScreenshotMode();
    }

    isRender = 0;
}

void GFXAdapter_Directx9::getBackBuffer()
{
    if (!m_pBackBufferSurf)
    {
        m_pd3dDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &m_pBackBufferSurf);

        D3DSURFACE_DESC desc;
        m_pBackBufferSurf->GetDesc(&desc);
        ITF_ASSERT_CRASH(desc.Width == getScreenWidth(), "Invalid width for back buffer");
        ITF_ASSERT_CRASH(desc.Height == getScreenHeight(), "Invalid height for back buffer");
    }
}

void GFXAdapter_Directx9::setBackBuffer()
{
    if (m_pUltraHDBackBufferSurf)
    {
        m_pd3dDevice->SetRenderTarget( 0, m_pUltraHDBackBufferSurf );
        m_currentRenderingSurface = m_pUltraHDBackBufferSurf;
    }
    else
    {
        m_pd3dDevice->SetRenderTarget( 0, m_pBackBufferSurf );
        m_currentRenderingSurface = m_pBackBufferSurf;
    }
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::BuildPresentParams()
{
    //POPO CHECK
#ifdef ITF_WINDOWS
    ZeroMemory( &m_ppars, sizeof( m_ppars ) );

    m_ppars.Windowed                = !m_fullscreen;

    m_ppars.BackBufferFormat        = D3DFMT_A8R8G8B8;//D3DFMT_UNKNOWN;
    m_ppars.EnableAutoDepthStencil  = TRUE;//TRUE;
    m_ppars.AutoDepthStencilFormat  = D3DFMT_D24S8;
    //m_ppars.AutoDepthStencilFormat  = m_depthStencilFormat;//D3DFMT_D32F_LOCKABLE;//D3DFMT_D16;
    m_ppars.MultiSampleType         = D3DMULTISAMPLE_NONE;

    m_ppars.Flags                   = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER ;

  
    /// synchro ??.
    if (m_waitVBL)
    {
    m_ppars.PresentationInterval   = D3DPRESENT_INTERVAL_ONE;
    }
    else
    {
    m_ppars.PresentationInterval   = D3DPRESENT_INTERVAL_IMMEDIATE;
    }

    if (m_fullscreen)
    {
        //scan the res

       
        bbool resolutionSupported = bfalse;

        for( u32 mode = 0; mode < m_displayModeCount; mode++ )
        {
            if (m_displayMode[mode].Height == m_screenHeight && m_displayMode[mode].Width == m_screenWidth)
            {
                resolutionSupported = btrue;
                break;
            }
        }

        if (resolutionSupported == bfalse)
        {
            if (m_displayModeCount && m_displayMode[m_displayModeCount-1].Format!=0)
            {  
                const D3DDISPLAYMODE& displayMode = m_displayMode[m_displayModeCount-1];
                m_screenWidth   = displayMode.Width;
                m_screenHeight  = displayMode.Height;
            }
            else
            {
                SYSTEM_ADAPTER->messageBox("Error","Fullscreen mode not supported",MB_OK,ITF_IDYES);
            }
        }

        m_ppars.BackBufferWidth               =   getScreenWidth();
        m_ppars.BackBufferHeight              =   getScreenHeight();
        m_ppars.BackBufferCount               =   2;
        m_ppars.FullScreen_RefreshRateInHz    =   60;
        m_ppars.SwapEffect = D3DSWAPEFFECT_FLIP;//D3DSWAPEFFECT_DISCARD;
    }
    else
    {
        /// multiple monitor.
        m_ppars.BackBufferWidth               =   getScreenWidth();
        m_ppars.BackBufferHeight              =   getScreenHeight();
        m_ppars.FullScreen_RefreshRateInHz    =   0;
        m_ppars.BackBufferCount               =   1;
        m_ppars.SwapEffect                    = D3DSWAPEFFECT_COPY;
    }

#else

    ZeroMemory(&m_ppars, sizeof(m_ppars));

    if (getScreenWidth()  == 1920)
        setUseDepthBuffer(bfalse);


    XVIDEO_MODE videoMode; 
    XMemSet( &videoMode, 0, sizeof(XVIDEO_MODE) ); 
    XGetVideoMode( &videoMode );


    m_ppars.BackBufferWidth = getScreenWidth();
    m_ppars.BackBufferHeight = getScreenHeight();
    m_ppars.BackBufferFormat = D3DFMT_A8R8G8B8;
    m_ppars.BackBufferCount = 0;
    m_ppars.MultiSampleType = D3DMULTISAMPLE_NONE;
    m_ppars.MultiSampleQuality = 0;
    m_ppars.SwapEffect = D3DSWAPEFFECT_DISCARD;
    m_ppars.hDeviceWindow = NULL;
    m_ppars.Windowed = FALSE;
    m_ppars.EnableAutoDepthStencil = isUseDepthBuffer();
    m_ppars.AutoDepthStencilFormat = D3DFMT_D24S8;
    m_ppars.Flags = 0;
    m_ppars.FullScreen_RefreshRateInHz = 0;
    m_ppars.PresentationInterval = m_waitVBL ? D3DPRESENT_INTERVAL_ONE : D3DPRESENT_INTERVAL_IMMEDIATE;

    setMonitorRefreshRate(videoMode.RefreshRate);
    
#ifdef _USE_ZBUFFER_TILING    
    m_ppars.BackBufferFormat = ( D3DFORMAT )MAKESRGBFMT( D3DFMT_A8R8G8B8 );
    m_ppars.FrontBufferFormat = ( D3DFORMAT )MAKESRGBFMT( D3DFMT_LE_X8R8G8B8 );

    m_ppars.DisableAutoBackBuffer = TRUE;
    m_ppars.DisableAutoFrontBuffer = TRUE;
#endif

#endif //ITF_WINDOWS

}

//----------------------------------------------------------------------------//

bbool GFXAdapter_Directx9::resetDevice(bbool _forceReset)
{
    if (!m_pd3dDevice) return bfalse;

    preD3DReset();
    
    BuildPresentParams();

    HRESULT hr;
    hr = m_pd3dDevice->Reset(&m_ppars);

   
    if (hr == D3DERR_DEVICELOST)
    {
        Sleep(1000);
        return false;
    }
    else  if (hr!=S_OK)
    {
       messageD3DError(hr);
    }

    if (SUCCEEDED(hr) || _forceReset)
    {
        Sleep(10);

        postD3DReset();
        reshape();
        init();

        cleanFontCache();

        return true;
    }
  
    

    return false;
}

//----------------------------------------------------------------------------//

bbool GFXAdapter_Directx9::isDeviceLost()
{
#ifdef ITF_WINDOWS
    if (!m_pd3dDevice) return bfalse;

    HRESULT hr;

    hr = m_pd3dDevice->TestCooperativeLevel();

    if (hr == D3DERR_DEVICELOST)
    {
        return btrue;
    }
    else if (hr == D3DERR_DEVICENOTRESET)
    {
        if (!resetDevice(bfalse))
            return btrue;
    }
#endif //ITF_WINDOWS
    return bfalse;

}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::lostDevice()
{

}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::preD3DReset()
{
#ifdef ITF_WINDOWS
    HRESULT hr;

    /// RT.
    if (m_pBackBufferSurf) m_pBackBufferSurf->Release();
        m_pBackBufferSurf = 0;

    if ( m_pFullRTtex[0] )
    {
        m_pFullRTtex[0]->Release();
        m_pFullRTtex[0] = 0;
    }
    if ( m_pFullRTtex[1] )
    {
        m_pFullRTtex[1]->Release();
        m_pFullRTtex[1] = 0;
    }
    if ( m_pFullRTsurf[0] )
    {
        m_pFullRTsurf[0]->Release();
        m_pFullRTsurf[0] = 0;
    }
    if ( m_pFullRTsurf[1] )
    {
        m_pFullRTsurf[1]->Release();
        m_pFullRTsurf[1] = 0;
    }

    if (m_pRenderTargetSurfacePicking)
    {
        m_pRenderTargetSurfacePicking->Release();
        m_pRenderTargetSurfacePicking = NULL;
    }

    if (m_pPickingOffScreenBufferSurf)
    {
        m_pPickingOffScreenBufferSurf->Release();
        m_pPickingOffScreenBufferSurf = NULL;
    }

#ifdef ITF_SUPPORT_VIDEOCAPTURE
    VideoCapture* videoCapture = VideoCapture::getInstance();
    if ( videoCapture && videoCapture->getCaptureBuffer())
        videoCapture->getCaptureBuffer()->release();
#endif //ITF_SUPPORT_VIDEOCAPTURE

    AFTERFX_onLostDevice();

    /// clear Render Target.
    if (getTargetManager())
        getTargetManager()->cleanAllRenderTarget();

    /// shaders.
    for (u32 i = 0; i < mp_shaderManager.getNumberShader(); i++)
	{    
		if (mp_shaderManager.getShaderByIndex(i)->mp_effect)
			hr = ((ITF_EFFECTP)mp_shaderManager.getShaderByIndex(i)->mp_effect)->OnLostDevice();
	}

    /// Vertex Buffers.
    VertexBufferManagerOnLostDevice();
    /// Index Buffer
    IndexBufferManagerOnLostDevice();

    if (VIDEO_ADAPTER)
    {
        u32 nhandle = VIDEO_ADAPTER->getNumberHandle();
        for (u32 i =0; i< nhandle ; i++)
        {
            videoHandle* ph = VIDEO_ADAPTER->getHandleAt(i);
            ph->OnLostDevice();
        }
    }

#ifdef _USE_FONTDEBUG
    /// font debug.
    if( mp_FontDebug )
        mp_FontDebug->OnLostDevice();

	if( mp_FontDebug_Monospace )
		mp_FontDebug_Monospace->OnLostDevice();

    if( mp_FontDebug_Big )
        mp_FontDebug_Big->OnLostDevice();
#endif
#endif

#ifdef SHADERTEST
	mp_shaderManager.removeShaderGroup(mp_shaderManager.getShaderByIndex(10));
    mp_shaderManager.removeShaderGroup(mp_shaderManager.getShaderByIndex(9));
    mp_shaderManager.removeShaderGroup(mp_shaderManager.getShaderByIndex(8));
    mp_shaderManager.removeShaderGroup(mp_shaderManager.getShaderByIndex(7));
#endif
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::postD3DReset()
{
#ifdef ITF_WINDOWS
    HRESULT hr;
    
    /// backBuffer and mainTarget.
    getBackBuffer();

    /// recreate Render Target.
    if (getTargetManager())
        getTargetManager()->recreateAllRenderTarget();

    /// shaders.
    for (u32 i = 0; i < mp_shaderManager.getNumberShader(); i++)
    {
        if (mp_shaderManager.getShaderByIndex(i)->mp_effect)
            hr = ((ITF_EFFECTP)mp_shaderManager.getShaderByIndex(i)->mp_effect)->OnResetDevice();
    }

    /// Vertex Buffers.
    VertexBufferManagerOnResetDevice();

    /// Index Buffer.
    IndexBufferManagerOnResetDevice();

    /// Video
    if (VIDEO_ADAPTER)
    {
        u32 nhandle = VIDEO_ADAPTER->getNumberHandle();
        for (u32 i =0; i< nhandle ; i++)
        {
            videoHandle* ph = VIDEO_ADAPTER->getHandleAt(i);
            ph->OnResetDevice();
        }
    }

#ifdef _USE_FONTDEBUG 
    /// font debug.
    if( mp_FontDebug )
        hr = mp_FontDebug->OnResetDevice();

	if( mp_FontDebug_Monospace )
		hr = mp_FontDebug_Monospace->OnResetDevice();

    if( mp_FontDebug_Big )
        hr = mp_FontDebug_Big->OnResetDevice();
#endif

    WORLD_MANAGER->reinit(bfalse);
#endif

#ifdef SHADERTEST
	mp_shaderManager.loadShaderGroup("renderPCT_Register.fx"); ///default shader.
    mp_shaderManager.loadShaderGroup("movie_Register.fx");
    mp_shaderManager.loadShaderGroup("font_Register.fx");
    mp_shaderManager.loadShaderGroup("AfterFx_Register.fx");
#endif
}

//----------------------------------------------------------------------------//

void GFXAdapter_Directx9::useHDWLightning(u32 _activate)
{
#ifdef ITF_WINDOWS
   m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, _activate );
#endif
}

///----------------------------------------------------------------------------//
/// Shaders.
///----------------------------------------------------------------------------//

HRESULT LoadFile( const CHAR* strFileName, u8** ppFileData, DWORD* pdwFileSize )
{
    File* shaderFile = FILEMANAGER->openFile(strFileName,ITF_FILE_ATTR_READ);

    if (shaderFile)
    {
        u32 lenght = 0;
        u8 * pBuffer = NULL;
    
        lenght = (u32) shaderFile->getLength();
        pBuffer = newAlloc(mId_Temporary,u8[lenght]);

        shaderFile->read(pBuffer,lenght);
        FILEMANAGER->closeFile(shaderFile);

        if( pdwFileSize )
            *pdwFileSize = lenght;
        *ppFileData = pBuffer;

        return S_OK;
    }

    return S_FALSE;
}

VOID UnloadFile( u8* pFileData )
{
    ITF_ASSERT( pFileData != NULL );
    SF_DEL_ARRAY( pFileData );
}

void GFXAdapter_Directx9::createShader( ITF_shader* _shaderGroup )
{
}

void GFXAdapter_Directx9::releaseShader( ITF_shader* _shaderGroup )
{
	if (_shaderGroup->mp_effect)
	{
#ifdef ITF_X360//yo
    ((FXLEffect*)_shaderGroup->mp_effect )->Release();
#else
    SF_RELEASE( (ITF_EFFECTP&)_shaderGroup->mp_effect );
#endif
	}

	u32 np = _shaderGroup->mp_Ps.size();
	for (u32 i = 0 ; i < np ; i++)
	{
		IDirect3DPixelShader9* Ps = (IDirect3DPixelShader9*)_shaderGroup->mp_Ps[i]; 
		Ps->Release();
	}
	u32 nv = _shaderGroup->mp_Vs.size();
	for (u32 i = 0 ; i < nv ; i++)
	{
		IDirect3DVertexShader9* Vs = (IDirect3DVertexShader9*)_shaderGroup->mp_Vs[i]; 
		Vs->Release();
	}
}

void* GFXAdapter_Directx9::loadBinPShader( const String& _name )
{
    String shaderpath;

#ifdef ITF_X360
    shaderpath.setTextFormat("Shaders/compiled/x360/%ls", _name.cStr());
#else
    shaderpath.setTextFormat("Shaders/compiled/win32/%ls", _name.cStr());
#endif
    shaderpath += ".ckd";

    u8* pCode;
    DWORD dwSize;

    HRESULT res = LoadFile( (LPCSTR)StringConverter(shaderpath.cStr()).getChar(), &pCode, &dwSize );
    if (res != S_OK)
    {
        ITF_FATAL_ERROR("Error loading bin shader %ls ",shaderpath.cStr());
        return bfalse;
	}

    IDirect3DPixelShader9  *pPShaderResult = 0;

    HRESULT hr = m_pd3dDevice->CreatePixelShader( (DWORD*) pCode, &pPShaderResult );
    if ( FAILED( hr ) )
    {
        UnloadFile( pCode );
        return( 0 );
    }

    UnloadFile( pCode );
    return pPShaderResult;
}

void* GFXAdapter_Directx9::loadBinVShader(const String& _name )
{
    String shaderpath;

#ifdef ITF_X360
    shaderpath.setTextFormat("Shaders/compiled/x360/%ls", _name.cStr());
#else
    shaderpath.setTextFormat("Shaders/compiled/win32/%ls", _name.cStr());
#endif
    shaderpath += ".ckd";

    u8* pCode;
    DWORD dwSize;

    HRESULT res = LoadFile( (LPCSTR)StringConverter(shaderpath.cStr()).getChar(), &pCode, &dwSize );
    if (res != S_OK)
    {
        ITF_FATAL_ERROR("Error loading bin shader %ls ",shaderpath.cStr());
        return bfalse;
	}

    IDirect3DVertexShader9  *pVShaderResult = 0;

    HRESULT hr = m_pd3dDevice->CreateVertexShader( (DWORD*) pCode, &pVShaderResult );
    if ( FAILED( hr ) )
    {
        UnloadFile( pCode );
        return( 0 );
    }
 
    UnloadFile( pCode );
    return pVShaderResult;
}

bbool GFXAdapter_Directx9::loadShader( ITF_shader* _shaderGroup )
{

#ifdef ITF_WINDOWS
    HRESULT hr;
    DWORD ShaderFlags = D3DXFX_NOT_CLONEABLE;
#endif

#ifdef DEBUG_SHADER
    ShaderFlags |= D3DXSHADER_DEBUG;
#ifdef DEBUG_VS
    ShaderFlags |= D3DXSHADER_FORCE_VS_SOFTWARE_NOOPT;
#endif
#ifdef DEBUG_PS
    ShaderFlags |= D3DXSHADER_FORCE_PS_SOFTWARE_NOOPT;
#endif
#endif ///DEBUG_SHADER

    String shaderpath;
    String shadername;
    u8* pCode = 0;
    DWORD dwSize = 0;

    shadername = _shaderGroup->m_name.cStr();

#ifndef USE_SHADER_BIN
    shaderpath = "Shaders/unified/";
    shaderpath += shadername;

    HRESULT res = LoadFile( (LPCSTR)StringConverter(shaderpath.cStr()).getChar(), &pCode, &dwSize );
    if (res != S_OK)
    {
        ITF_FATAL_ERROR("Error loading shader %ls ",shaderpath.cStr());
        return bfalse;
	}
#endif

	bbool createeffect = btrue;

#ifdef SHADERTEST

	if (shadername=="renderPCT_Register.fx")
    {
		uPtr shadercodeP = 0;
        uPtr shadercodeV = 0;

		// --- vertexShader ---
#ifdef USE_SHADER_BIN
        shadercodeV = (uPtr)loadBinVShader("renderPCT_vsh1");
#else
		shadercodeV = (uPtr)compileVertexShader((const char*)pCode, dwSize, "default_PCT_VS");
#endif
        _shaderGroup->mp_Vs.push_back(shadercodeV);//0

#ifdef USE_SHADER_BIN
        shadercodeV = (uPtr)loadBinVShader("renderPCT_vsh2");
#else
        shadercodeV = (uPtr)compileVertexShader((const char*)pCode, dwSize, "default_PC_VS");
#endif		
        _shaderGroup->mp_Vs.push_back(shadercodeV);//1

#ifdef USE_SHADER_BIN
		shadercodeV = (uPtr)loadBinVShader("renderPCT_vsh3");
#else		
        shadercodeV = (uPtr)compileVertexShader((const char*)pCode, dwSize, "default_PT_VS");
#endif		
        _shaderGroup->mp_Vs.push_back(shadercodeV);

#ifdef USE_SHADER_BIN
		shadercodeV = (uPtr)loadBinVShader("renderPCT_vsh4");
#else
        shadercodeV = (uPtr)compileVertexShader((const char*)pCode, dwSize, "default_PCT_Patch_VS");
#endif		
        _shaderGroup->mp_Vs.push_back(shadercodeV);//3

#ifdef USE_SHADER_BIN
		shadercodeV = (uPtr)loadBinVShader("renderPCT_vsh5");
#else
        shadercodeV = (uPtr)compileVertexShader((const char*)pCode, dwSize, "default_PNCT_VS");
#endif		
        _shaderGroup->mp_Vs.push_back(shadercodeV);

#ifdef USE_SHADER_BIN
		shadercodeV = (uPtr)loadBinVShader("renderPCT_vsh6");
#else
        shadercodeV = (uPtr)compileVertexShader((const char*)pCode, dwSize, "frize_PNC3T_VS");
#endif		
        _shaderGroup->mp_Vs.push_back(shadercodeV);//5

#ifdef USE_SHADER_BIN
		shadercodeV = (uPtr)loadBinVShader("renderPCT_vsh7");
#else
        shadercodeV = (uPtr)compileVertexShader((const char*)pCode, dwSize, "fluid_PCT_VS");
#endif		
        _shaderGroup->mp_Vs.push_back(shadercodeV);

#ifdef USE_SHADER_BIN
		shadercodeV = (uPtr)loadBinVShader("renderPCT_vsh8");
#else
        shadercodeV = (uPtr)compileVertexShader((const char*)pCode, dwSize, "fluid2_PCT_VS");
#endif		
        _shaderGroup->mp_Vs.push_back(shadercodeV);//7

#ifdef USE_SHADER_BIN
		shadercodeV = (uPtr)loadBinVShader("renderPCT_vsh9");
#else
        shadercodeV = (uPtr)compileVertexShader((const char*)pCode, dwSize, "trail_PCT_VS");
#endif		
        _shaderGroup->mp_Vs.push_back(shadercodeV);

#ifdef USE_SHADER_BIN
		shadercodeV = (uPtr)loadBinVShader("renderPCT_vsh10");
#else
        shadercodeV = (uPtr)compileVertexShader((const char*)pCode, dwSize, "spline_PCT_VS");
#endif		
        _shaderGroup->mp_Vs.push_back(shadercodeV);//9

#ifdef USE_SHADER_BIN
		shadercodeV = (uPtr)loadBinVShader("renderPCT_vsh11");
#else
        shadercodeV = (uPtr)compileVertexShader((const char*)pCode, dwSize, "default_PCT_BezierPatch_VS");
#endif		
        _shaderGroup->mp_Vs.push_back(shadercodeV);

#ifdef USE_SHADER_BIN
		shadercodeV = (uPtr)loadBinVShader("renderPCT_vsh12");
#else
        shadercodeV = (uPtr)compileVertexShader((const char*)pCode, dwSize, "default_PTambiant_VS");
#endif		
        _shaderGroup->mp_Vs.push_back(shadercodeV);//11

#ifdef USE_SHADER_BIN
		shadercodeV = (uPtr)loadBinVShader("renderPCT_vsh13");
#else
        shadercodeV = (uPtr)compileVertexShader((const char*)pCode, dwSize, "refraction_PCT_VS");
#endif		
        _shaderGroup->mp_Vs.push_back(shadercodeV);//12

#ifdef USE_SHADER_BIN
        shadercodeV = (uPtr)loadBinVShader("renderPCT_vsh14");
#else
        shadercodeV = (uPtr)compileVertexShader((const char*)pCode, dwSize, "OVERLAY_PCBT_VS");
#endif        
        _shaderGroup->mp_Vs.push_back(shadercodeV);//13

		/// --- pixel shader ---
#ifdef USE_SHADER_BIN
        shadercodeP = (uPtr)loadBinPShader("renderPCT_psh1");
#else
        shadercodeP = (uPtr)compilePixelShader((const char*)pCode, dwSize, "default_PCT_PS");
#endif		
        _shaderGroup->mp_Ps.push_back(shadercodeP);//0

#ifdef USE_SHADER_BIN
		shadercodeP = (uPtr)loadBinPShader("renderPCT_psh2");
#else
        shadercodeP = (uPtr)compilePixelShader((const char*)pCode, dwSize, "default_PC_PS");
#endif		
        _shaderGroup->mp_Ps.push_back(shadercodeP);

#ifdef USE_SHADER_BIN
		shadercodeP = (uPtr)loadBinPShader("renderPCT_psh3");
#else
        shadercodeP = (uPtr)compilePixelShader((const char*)pCode, dwSize, "default_PT_PS");
#endif		
        _shaderGroup->mp_Ps.push_back(shadercodeP);//2

		// fog: 3
#ifdef USE_SHADER_BIN
		shadercodeP = (uPtr)loadBinPShader("renderPCT_psh4");
#else
        shadercodeP = (uPtr)compilePixelShader((const char*)pCode, dwSize, "fog_PCT_PS");
#endif		
        _shaderGroup->mp_Ps.push_back(shadercodeP);

#ifdef USE_SHADER_BIN
		shadercodeP = (uPtr)loadBinPShader("renderPCT_psh5");
#else
        shadercodeP = (uPtr)compilePixelShader((const char*)pCode, dwSize, "fog_PC_PS");
#endif		
        _shaderGroup->mp_Ps.push_back(shadercodeP);//4

#ifdef USE_SHADER_BIN
		shadercodeP = (uPtr)loadBinPShader("renderPCT_psh6");
#else
        shadercodeP = (uPtr)compilePixelShader((const char*)pCode, dwSize, "fog_PT_PS");
#endif		
        _shaderGroup->mp_Ps.push_back(shadercodeP);

		// overdraw: 6
#ifdef USE_SHADER_BIN
        shadercodeP = (uPtr)loadBinPShader("renderPCT_psh7");
#else
        shadercodeP = (uPtr)compilePixelShader((const char*)pCode, dwSize, "overDraw_PS");
#endif		
        _shaderGroup->mp_Ps.push_back(shadercodeP);

		// refraction: 7
#ifdef USE_SHADER_BIN
        shadercodeP = (uPtr)loadBinPShader("renderPCT_psh8");
#else
        shadercodeP = (uPtr)compilePixelShader((const char*)pCode, dwSize, "refraction_PCT_PS");
#endif
        _shaderGroup->mp_Ps.push_back(shadercodeP);

		createeffect = bfalse;
	}
    else if (shadername=="movie_Register.fx")
    {
		uPtr shadercodeP = 0;
        uPtr shadercodeV = 0;
        
		// --- vertexShader ---
#ifdef USE_SHADER_BIN
        shadercodeV = (uPtr)loadBinVShader("movie_vsh1");
#else		
        shadercodeV = (uPtr)compileVertexShader((const char*)pCode, dwSize, "VS_Movie");
#endif		
        _shaderGroup->mp_Vs.push_back(shadercodeV);//0

		/// --- pixel shader ---
#ifdef ITF_X360
#ifdef USE_SHADER_BIN        
        shadercodeP = (uPtr)loadBinPShader("movie_psh1");
#else        
        shadercodeP = (uPtr)compilePixelShader((const char*)pCode, dwSize, "PS_Movie");
#endif		
        _shaderGroup->mp_Ps.push_back(shadercodeP);
#else
#ifdef USE_SHADER_BIN        
        shadercodeP = (uPtr)loadBinPShader("movie_psh1");
#else        
        shadercodeP = (uPtr)compilePixelShader((const char*)pCode, dwSize, "PS_MovieWIN32");
#endif		
        _shaderGroup->mp_Ps.push_back(shadercodeP);
#endif
        createeffect = bfalse;
    }
    else if (shadername=="font_Register.fx")
    {
		uPtr shadercodeP = 0;
        uPtr shadercodeV = 0;
        
		// --- vertexShader ---
#ifdef USE_SHADER_BIN
        shadercodeV = (u32)loadBinVShader("font_vsh1");
#else        
        shadercodeV = (uPtr)compileVertexShader((const char*)pCode, dwSize, "VertScene");
#endif		
        _shaderGroup->mp_Vs.push_back(shadercodeV);//0

        /// --- pixel shader ---
#ifdef USE_SHADER_BIN
        shadercodeP = (uPtr)loadBinPShader("font_psh1");
#else        
        shadercodeP = (uPtr)compilePixelShader((const char*)pCode, dwSize, "PixWithoutOutline");
#endif		
        _shaderGroup->mp_Ps.push_back(shadercodeP);

#ifdef USE_SHADER_BIN
        shadercodeP = (uPtr)loadBinPShader("font_psh2");
#else
        shadercodeP = (uPtr)compilePixelShader((const char*)pCode, dwSize, "PixWithOutline");
#endif		
        _shaderGroup->mp_Ps.push_back(shadercodeP);

        createeffect = bfalse;
    }
    else if (shadername=="AfterFx_Register.fx")
    {
        uPtr shadercodeP = 0;
        uPtr shadercodeV = 0;
        
		// --- vertexShader ---
#ifdef USE_SHADER_BIN
        shadercodeV = (uPtr)loadBinVShader("AfterFx_vsh1");
#else
        shadercodeV = (uPtr)compileVertexShader((const char*)pCode, dwSize, "blur_VS");
#endif		
        _shaderGroup->mp_Vs.push_back(shadercodeV);//0

#ifdef USE_SHADER_BIN
        shadercodeV = (uPtr)loadBinVShader("AfterFx_vsh2");
#else
        shadercodeV = (uPtr)compileVertexShader((const char*)pCode, dwSize, "PCT1_VS");
#endif		
        _shaderGroup->mp_Vs.push_back(shadercodeV);//0

        /// --- pixel shader ---
#ifdef USE_SHADER_BIN
        shadercodeP = (uPtr)loadBinPShader("AfterFx_psh1");
#else
        shadercodeP = (uPtr)compilePixelShader((const char*)pCode, dwSize, "BigBlur_PS");
#endif		
        _shaderGroup->mp_Ps.push_back(shadercodeP);

#ifdef USE_SHADER_BIN
        shadercodeP = (uPtr)loadBinPShader("AfterFx_psh2");
#else
        shadercodeP = (uPtr)compilePixelShader((const char*)pCode, dwSize, "AddMul_PS");
#endif
        _shaderGroup->mp_Ps.push_back(shadercodeP);

        createeffect = bfalse;
    }
#endif

if (createeffect)
{
    //tmp fix
#ifdef ITF_X360

#ifndef USE_SHADER_BIN
    ID3DXBuffer* pEffectData;
    ID3DXBuffer* pErrorList;

    DWORD CompileFlags = D3DXSHADER_FXLPARAMETERS_AS_VARIABLE_NAMES;
    FXLCompileEffect( ( CHAR* )pCode, dwSize, NULL, NULL, CompileFlags, &pEffectData, &pErrorList );
    FXLCreateEffect( m_pd3dDevice, pEffectData->GetBufferPointer(), NULL, (FXLEffect**)&_shaderGroup->mp_effect );
    pEffectData->Release();
#else
    FXLCreateEffect( m_pd3dDevice, pCode, NULL, (FXLEffect**)&_shaderGroup->mp_effect );
#endif
  
   

#else
    LPD3DXBUFFER pCompilationErrors = 0;

    hr = D3DXCreateEffect(m_pd3dDevice, ( CHAR* )pCode, dwSize,NULL,NULL,ShaderFlags|D3DXFX_LARGEADDRESSAWARE,NULL,(ITF_EFFECTP*)&_shaderGroup->mp_effect, &pCompilationErrors);
 
        if (hr != S_OK)
        {
            LOG("Direct3D9Renderer: CreateShader failed : %s\n", pCompilationErrors->GetBufferPointer() );
            return bfalse;
        }

#endif


#ifdef ITF_X360//yo
    
    /// get Texture Handle
    _shaderGroup->m_TextureHandle[0] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetParameterHandle("TextureSampler0");
    _shaderGroup->m_TextureHandle[1] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetParameterHandle("TextureSampler1");
    _shaderGroup->m_TextureHandle[2] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetParameterHandle("TextureSampler2");
        
    /// get Matrix Handle.
    _shaderGroup->m_MatrixHandle[0] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetParameterHandle("g_mWorldViewProjection");
    _shaderGroup->m_MatrixHandle[1] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetParameterHandle("g_mUVmat");
    _shaderGroup->m_MatrixHandle[2] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetParameterHandle("g_mView");
    _shaderGroup->m_MatrixHandle[3] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetParameterHandle("g_mPatchVx");
    _shaderGroup->m_MatrixHandle[4] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetParameterHandle("g_mPatchUv");

    _shaderGroup->m_FloatHandle[0] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetParameterHandle("g_fconst0");
    _shaderGroup->m_FloatHandle[1] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetParameterHandle("g_fconst1");
    _shaderGroup->m_FloatHandle[2] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetParameterHandle("g_fconst2");
    _shaderGroup->m_FloatHandle[3] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetParameterHandle("g_fconst3");
    _shaderGroup->m_FloatHandle[4] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetParameterHandle("g_fconst4");
    _shaderGroup->m_FloatHandle[5] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetParameterHandle("g_fconst5");
    _shaderGroup->m_FloatHandle[6] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetParameterHandle("g_fconst6");
    _shaderGroup->m_FloatHandle[7] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetParameterHandle("g_fconst7");

    _shaderGroup->m_VectorArrayHandle[0] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetParameterHandle("g_va0");
    _shaderGroup->m_VectorArrayHandle[1] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetParameterHandle("g_va1");

    _shaderGroup->m_IntHandle[0] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetParameterHandle("g_iconst0");

    _shaderGroup->m_VecHandle[0] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetParameterHandle("g_vconst0");
    _shaderGroup->m_VecHandle[1] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetParameterHandle("g_vconst1");
    _shaderGroup->m_VecHandle[2] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetParameterHandle("g_vconst2");
    _shaderGroup->m_VecHandle[3] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetParameterHandle("g_vconst3");
    _shaderGroup->m_VecHandle[4] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetParameterHandle("g_vconst4");
    _shaderGroup->m_VecHandle[5] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetParameterHandle("g_vconst5");
    _shaderGroup->m_VecHandle[6] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetParameterHandle("g_vconst6");
    _shaderGroup->m_VecHandle[7] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetParameterHandle("g_vconst7");


    /// get Tech Handle
    _shaderGroup->m_TechHandle[0] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetTechniqueHandle("Tech_0");
    _shaderGroup->m_TechHandle[1] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetTechniqueHandle("Tech_1");
    _shaderGroup->m_TechHandle[2] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetTechniqueHandle("Tech_2");
    _shaderGroup->m_TechHandle[3] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetTechniqueHandle("Tech_3");
    _shaderGroup->m_TechHandle[4] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetTechniqueHandle("Tech_4");
    _shaderGroup->m_TechHandle[5] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetTechniqueHandle("Tech_5");
    _shaderGroup->m_TechHandle[6] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetTechniqueHandle("Tech_6");
    _shaderGroup->m_TechHandle[7] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetTechniqueHandle("Tech_7");
    _shaderGroup->m_TechHandle[8] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetTechniqueHandle("Tech_8");
    _shaderGroup->m_TechHandle[9] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetTechniqueHandle("Tech_9");
    _shaderGroup->m_TechHandle[10] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetTechniqueHandle("Tech_10");
    _shaderGroup->m_TechHandle[11] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetTechniqueHandle("Tech_11");
    _shaderGroup->m_TechHandle[12] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetTechniqueHandle("Tech_12");
    _shaderGroup->m_TechHandle[13] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetTechniqueHandle("Tech_13");
    _shaderGroup->m_TechHandle[14] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetTechniqueHandle("Tech_14");
    _shaderGroup->m_TechHandle[15] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetTechniqueHandle("Tech_15");

#else

    /// get Texture Handle
    _shaderGroup->m_TextureHandle[0] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetParameterByName(NULL, "g_diffuseTexture");
    _shaderGroup->m_TextureHandle[1] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetParameterByName(NULL, "g_sceneTexture");
    _shaderGroup->m_TextureHandle[2] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetParameterByName(NULL, "g_normalTexture");
    _shaderGroup->m_TextureHandle[3] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetParameterByName(NULL, "g_custom1Texture");
    _shaderGroup->m_TextureHandle[4] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetParameterByName(NULL, "g_custom2Texture");
    _shaderGroup->m_TextureHandle[5] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetParameterByName(NULL, "g_custom3Texture");
    _shaderGroup->m_TextureHandle[6] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetParameterByName(NULL, "g_custom4Texture");
    _shaderGroup->m_TextureHandle[7] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetParameterByName(NULL, "g_custom5Texture");
    
    /// get Matrix Handle.
    _shaderGroup->m_MatrixHandle[0] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetParameterByName(NULL, "g_mWorldViewProjection");
    _shaderGroup->m_MatrixHandle[1] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetParameterByName(NULL, "g_mUVmat");
    _shaderGroup->m_MatrixHandle[2] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetParameterByName(NULL, "g_mView");
    _shaderGroup->m_MatrixHandle[3] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetParameterByName(NULL, "g_mPatchVx");
    _shaderGroup->m_MatrixHandle[4] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetParameterByName(NULL, "g_mPatchUv");

    _shaderGroup->m_FloatHandle[0] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetParameterByName(NULL, "g_fconst0");
    _shaderGroup->m_FloatHandle[1] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetParameterByName(NULL, "g_fconst1");
    _shaderGroup->m_FloatHandle[2] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetParameterByName(NULL, "g_fconst2");
    _shaderGroup->m_FloatHandle[3] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetParameterByName(NULL, "g_fconst3");
    _shaderGroup->m_FloatHandle[4] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetParameterByName(NULL, "g_fconst4");
    _shaderGroup->m_FloatHandle[5] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetParameterByName(NULL, "g_fconst5");
    _shaderGroup->m_FloatHandle[6] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetParameterByName(NULL, "g_fconst6");
    _shaderGroup->m_FloatHandle[7] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetParameterByName(NULL, "g_fconst7");

    _shaderGroup->m_VectorArrayHandle[0] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetParameterByName(NULL, "g_va0");
    _shaderGroup->m_VectorArrayHandle[1] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetParameterByName(NULL, "g_va1");

    _shaderGroup->m_IntHandle[0] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetParameterByName(NULL, "g_iconst0");

    _shaderGroup->m_VecHandle[0] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetParameterByName(NULL, "g_vconst0");
    _shaderGroup->m_VecHandle[1] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetParameterByName(NULL, "g_vconst1");
    _shaderGroup->m_VecHandle[2] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetParameterByName(NULL, "g_vconst2");
    _shaderGroup->m_VecHandle[3] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetParameterByName(NULL, "g_vconst3");
    _shaderGroup->m_VecHandle[4] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetParameterByName(NULL, "g_vconst4");
    _shaderGroup->m_VecHandle[5] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetParameterByName(NULL, "g_vconst5");
    _shaderGroup->m_VecHandle[6] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetParameterByName(NULL, "g_vconst6");
    _shaderGroup->m_VecHandle[7] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetParameterByName(NULL, "g_vconst7");

    /// get Tech Handle
    _shaderGroup->m_TechHandle[0] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetTechniqueByName("Tech_0");
    _shaderGroup->m_TechHandle[1] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetTechniqueByName("Tech_1");
    _shaderGroup->m_TechHandle[2] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetTechniqueByName("Tech_2");
    _shaderGroup->m_TechHandle[3] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetTechniqueByName("Tech_3");
    _shaderGroup->m_TechHandle[4] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetTechniqueByName("Tech_4");
    _shaderGroup->m_TechHandle[5] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetTechniqueByName("Tech_5");
    _shaderGroup->m_TechHandle[6] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetTechniqueByName("Tech_6");
    _shaderGroup->m_TechHandle[7] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetTechniqueByName("Tech_7");
    _shaderGroup->m_TechHandle[8] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetTechniqueByName("Tech_8");
    _shaderGroup->m_TechHandle[9] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetTechniqueByName("Tech_9");
    _shaderGroup->m_TechHandle[10] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetTechniqueByName("Tech_10");
    _shaderGroup->m_TechHandle[11] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetTechniqueByName("Tech_11");
    _shaderGroup->m_TechHandle[12] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetTechniqueByName("Tech_12");
    _shaderGroup->m_TechHandle[13] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetTechniqueByName("Tech_13");
    _shaderGroup->m_TechHandle[14] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetTechniqueByName("Tech_14");
    _shaderGroup->m_TechHandle[15] = (void*)((ITF_EFFECTP)_shaderGroup->mp_effect)->GetTechniqueByName("Tech_15");

#endif
}
    if (pCode) 
        UnloadFile( pCode );

    return btrue;
}

///----------------------------------------------------------------------------//

void GFXAdapter_Directx9::beginShader( ITF_shader* _shader )
{
    ITF_ASSERT(m_shaderPassLastState != S_SHADERPASS_BEGIN);

    setShaderConstant(mp_currentShader);
    setShaderMatrix(mp_currentShader);
    ITF_EFFECTP effect = (ITF_EFFECTP)mp_currentShader->mp_effect;

#ifdef ITF_X360//yo
    //effect->BeginTechniqueFromIndex(mp_currentShader->m_selectedTech,FXL_COMMIT_DYNAMIC_RENDER_STATE );
    ITF_HANDLE handle = (ITF_HANDLE)mp_currentShader->m_TechHandle[mp_currentShader->m_selectedTech];
    effect->BeginTechnique( handle, 0 );
    effect->BeginPass( handle );
    //effect->BeginPass( 0 );
    effect->Commit();
#else
    UINT cPasses;
    setShaderTechnique(mp_currentShader);
    effect->Begin( &cPasses, 0 );
    effect->BeginPass( 0 );
#endif

    m_shaderPassLastState = S_SHADERPASS_BEGIN;
}

///----------------------------------------------------------------------------//

void GFXAdapter_Directx9::endShader( ITF_shader* _shader )
{
    ITF_ASSERT(m_shaderPassLastState == S_SHADERPASS_BEGIN);
    
    ITF_EFFECTP effect = (ITF_EFFECTP)mp_currentShader->mp_effect;
    effect->EndPass();

#ifdef ITF_X360//yo
    effect->EndTechnique();
#else
    effect->End();
#endif

    m_shaderPassLastState = S_SHADERPASS_END;
}

///----------------------------------------------------------------------------//

void GFXAdapter_Directx9::setShaderConstant( ITF_shader* _shader )
{
}

///----------------------------------------------------------------------------//

void GFXAdapter_Directx9::setShaderMatrix( ITF_shader* _shader )
{
    if ( m_useRotateZCamera )
        mg_WorldViewProj = mg_World * mg_ViewRender * mg_Proj;
    else
        mg_WorldViewProj = mg_World * mg_View * mg_Proj;

#ifdef ITF_WINDOWS
   if (m_bPickingMode)
   {
        D3DXMATRIX matrixTranslation;
        f32 width   = (f32) getScreenWidth();
        f32 height  = (f32) getScreenHeight();

        f32 offsetNormalizeX = ((-m_PickingPos.m_x+0.5f+m_deltaPicking)/width)*2.0f;
        f32 offsetNormalizeY = -((-m_PickingPos.m_y+0.5f)/height)*2.0f;

        D3DXMatrixTranslation(&matrixTranslation,offsetNormalizeX,offsetNormalizeY,0.0f);

        mg_WorldViewProj =  mg_WorldViewProj*matrixTranslation;
   }    
#endif //ITF_WINDOWS

   ITF_EFFECTP effect = (ITF_EFFECTP)_shader->mp_effect;

#ifdef SHADERTEST
	if (m_ShaderMode && !effect)
	{
		D3DXMATRIX trans;
		D3DXMatrixTranspose(&trans, &mg_WorldViewProj);
		m_pd3dDevice->SetVertexShaderConstantF( 0, (f32*)&trans, 4 );//wvp
		D3DXMatrixTranspose(&trans, &mg_UVmat);
		m_pd3dDevice->SetVertexShaderConstantF( 4, (f32*)&trans, 4 );//wvp
	}
#endif
   
if (effect)
{
#ifdef ITF_X360//yo
    effect->SetMatrixF4x4((ITF_HANDLE)mp_currentShader->m_MatrixHandle[0], ( f32* )&mg_WorldViewProj);
    if (mp_currentShader->m_MatrixHandle[1])
        effect->SetMatrixF4x4((ITF_HANDLE)mp_currentShader->m_MatrixHandle[1], ( f32* )&mg_UVmat);
    if (mp_currentShader->m_MatrixHandle[2])
        effect->SetMatrixF4x4((ITF_HANDLE)mp_currentShader->m_MatrixHandle[2], ( f32* )&mg_View);
#else
    effect->SetMatrix((ITF_HANDLE)mp_currentShader->m_MatrixHandle[0], &mg_WorldViewProj);
    if (mp_currentShader->m_MatrixHandle[1])
        effect->SetMatrix((ITF_HANDLE)mp_currentShader->m_MatrixHandle[1], &mg_UVmat);
    if (mp_currentShader->m_MatrixHandle[2])
        effect->SetMatrix((ITF_HANDLE)mp_currentShader->m_MatrixHandle[2], &mg_View);
#endif
}
}

///----------------------------------------------------------------------------//

void GFXAdapter_Directx9::setShaderTechnique( ITF_shader* _shader )
{
#ifdef ITF_WINDOWS
    HRESULT hr;
    hr = ((ITF_EFFECTP)_shader->mp_effect)->SetTechnique((ITF_HANDLE)mp_currentShader->m_TechHandle[mp_currentShader->m_selectedTech]);
#endif
}

///----------------------------------------------------------------------------//

void GFXAdapter_Directx9::setShader( ITF_shader* _shader)
{
    mp_currentShader = _shader;
}

///----------------------------------------------------------------------------//
/// Vertex Format.
///----------------------------------------------------------------------------//

void GFXAdapter_Directx9::setVertexFormat( u32 _vformat )
{
    m_currentVertexFormat = _vformat;
    
    switch(_vformat)
    {
        default:
        case VertexFormat_PCT:
        m_pd3dDevice->SetVertexDeclaration( mp_VDcl_PCT ) ;
        m_currentVertexSize = sizeof(VertexPCT);
        if (mp_currentShader == mp_defaultShader || getOverDrawMode() > 1 || isUseFog())
        {
            mp_currentShader->m_selectedTech = 0;
        }
        break;
        case VertexFormat_PC:
        m_pd3dDevice->SetVertexDeclaration( mp_VDcl_PC ) ;
        m_currentVertexSize = sizeof(VertexPC);
        if (mp_currentShader == mp_defaultShader || getOverDrawMode() > 1 || isUseFog())
        {
            mp_currentShader->m_selectedTech = 1;
        }
        break;
        case VertexFormat_PT:
            m_pd3dDevice->SetVertexDeclaration( mp_VDcl_PT ) ;
            m_currentVertexSize = sizeof(VertexPT);
            if (mp_currentShader == mp_defaultShader || getOverDrawMode() > 1 || isUseFog())
            {
                mp_currentShader->m_selectedTech = 2;
            }
            break;
        case VertexFormat_PTa:
            m_pd3dDevice->SetVertexDeclaration( mp_VDcl_PT ) ;
            m_currentVertexSize = sizeof(VertexPT);
            if (mp_currentShader == mp_defaultShader || getOverDrawMode() > 1 || isUseFog())
            {
                mp_currentShader->m_selectedTech = 11;
            }
            break;
        case VertexFormat_PCBT:
            m_pd3dDevice->SetVertexDeclaration( mp_VDcl_PCBT ) ;
            m_currentVertexSize = sizeof(VertexPCBT);
            break;
        case VertexFormat_PNCT:
            m_pd3dDevice->SetVertexDeclaration( mp_VDcl_PNCT ) ;
            m_currentVertexSize = sizeof(VertexPNCT);
            mp_currentShader->m_selectedTech = 4;
            break;
        case VertexFormat_PNC3T:
            m_pd3dDevice->SetVertexDeclaration( mp_VDcl_PNC3T ) ;
            m_currentVertexSize = sizeof(VertexPNC3T);
            mp_currentShader->m_selectedTech = 5;
            break;
       case VertexFormat_P:
            m_currentVertexSize = sizeof(VertexP);
            if (mp_currentShader == mp_defaultShader)
                mp_currentShader->m_selectedTech = 1;
        break;
    }
}

///----------------------------------------------------------------------------//
/// VertexBuffer.
///----------------------------------------------------------------------------//

void GFXAdapter_Directx9::createVertexBuffer( ITF_VertexBuffer* _vertexBuffer )
{
    HRESULT hr;
    D3DPOOL memPool = D3DPOOL_MANAGED;
    u32 usage = 0;

#ifdef ITF_WINDOWS
    if (_vertexBuffer->bIsDynamic())
    {
        usage |= D3DUSAGE_DYNAMIC;
        memPool = D3DPOOL_DEFAULT;
        usage |= D3DUSAGE_WRITEONLY;
    }
#endif

    hr = m_pd3dDevice->CreateVertexBuffer( _vertexBuffer->m_size,
                                                      usage, 0,
                                                      memPool, (LPDIRECT3DVERTEXBUFFER9*)&_vertexBuffer->mp_VertexBuffer, NULL );


    if (hr!=S_OK)
        messageD3DError(hr);

}

void GFXAdapter_Directx9::releaseVertexBuffer( ITF_VertexBuffer* _vertexBuffer )
{
    if (!_vertexBuffer->m_size) return;

    if (_vertexBuffer->mp_VertexBuffer)
    {
         m_pd3dDevice->SetStreamSource(0,0,0,0);
        ((LPDIRECT3DVERTEXBUFFER9)_vertexBuffer->mp_VertexBuffer)->Release();
        _vertexBuffer->mp_VertexBuffer = 0;
    }
}

void GFXAdapter_Directx9::setVertexBuffer( ITF_VertexBuffer* _vertexBuffer )
{

    //You must in the main thread to set the texture as available...
    
    ITF_ASSERT(Synchronize::getCurrentThreadId() == ThreadSettings::m_settings[eThreadId_mainThread].m_threadID);

    if(!_vertexBuffer->mp_VertexBuffer) return;
    
    setVertexFormat(_vertexBuffer->m_vertexFormat);

    m_pd3dDevice->SetStreamSource( 0, ((IDirect3DVertexBuffer9 *)_vertexBuffer->mp_VertexBuffer), _vertexBuffer->m_offset, _vertexBuffer->m_structVertexSize );
}

void GFXAdapter_Directx9::LockVertexBuffer( ITF_VertexBuffer* _vertexBuffer, void** _data, u32 _offset, u32 _size, u32 _flag )
{
    HRESULT hr = S_OK;
        
    u32 flag = 0;
#if  _DEBUG
    if (Synchronize::getCurrentThreadId() == ThreadSettings::m_settings[eThreadId_mainThread].m_threadID)
    {
        m_pd3dDevice->SetStreamSource(0,0,0,0);
    }
#endif // (_DEBUG)

#ifdef ITF_WINDOWS
    if (_flag & VB_LOCK_DISCARD)
    {
        /// only dynamic can discard
        if (_vertexBuffer->bIsDynamic())
        {
            flag |= D3DLOCK_DISCARD;
        }
    }

    if (_flag & VB_LOCK_NOOVERWRITE)
    {
        /// only dynamic can nooverwrite
        if (_vertexBuffer->bIsDynamic())
        {
            flag |= D3DLOCK_NOOVERWRITE;
        }
    }
#endif

    if (_vertexBuffer->mp_VertexBuffer)
        hr = ((LPDIRECT3DVERTEXBUFFER9)_vertexBuffer->mp_VertexBuffer)->Lock( _offset, _size, _data, flag);
   
     if (hr!=S_OK)
        messageD3DError(hr);
}

void GFXAdapter_Directx9::UnlockVertexBuffer( ITF_VertexBuffer* _vertexBuffer )
{
    if (_vertexBuffer->mp_VertexBuffer)
        ((LPDIRECT3DVERTEXBUFFER9)_vertexBuffer->mp_VertexBuffer)->Unlock();
}

void GFXAdapter_Directx9::VertexBufferManagerOnLostDevice()
{
    m_VertexBufferManager.UnallocAllDynamicVB();
#ifdef VBMANAGER_USE_DYNAMICRING_VB
    m_VertexBufferManager.deleteDynamicRingVB();
#endif
}
void GFXAdapter_Directx9::VertexBufferManagerOnResetDevice()
{
    m_VertexBufferManager.ReallocAllDynamicVB();
}
void GFXAdapter_Directx9::VertexBufferOnResetDevice(ITF_VertexBuffer* _vertexBuffer )
{
    releaseVertexBuffer(_vertexBuffer);
}
void GFXAdapter_Directx9::VertexBufferOnLostDevice(ITF_VertexBuffer* _vertexBuffer )
{
    createVertexBuffer(_vertexBuffer);
}

///----------------------------------------------------------------------------//
/// IndexBuffer.
///----------------------------------------------------------------------------//

void GFXAdapter_Directx9::createIndexBuffer( ITF_IndexBuffer* _indexBuffer )
{
    HRESULT hr;
    D3DPOOL memPool = D3DPOOL_MANAGED;
    u32 usage = 0;
#ifdef ITF_WINDOWS
    if (_indexBuffer->bIsDynamic())
    {
        usage |= D3DUSAGE_DYNAMIC;
        memPool = D3DPOOL_DEFAULT;
        usage |= D3DUSAGE_WRITEONLY;
    }
#endif

    hr = m_pd3dDevice->CreateIndexBuffer( _indexBuffer->m_nIndices * sizeof(u16),
                                                      usage, D3DFMT_INDEX16,
                                                      memPool, (LPDIRECT3DINDEXBUFFER9*)&_indexBuffer->mp_IndexBuffer, NULL );
    if (FAILED(hr))
        messageD3DError(hr);
}

void GFXAdapter_Directx9::releaseIndexBuffer( ITF_IndexBuffer* _indexBuffer )
{
    if (_indexBuffer->mp_IndexBuffer)
    {
        m_pd3dDevice->SetIndices((IDirect3DIndexBuffer9 *)0 );
        ((LPDIRECT3DINDEXBUFFER9)_indexBuffer->mp_IndexBuffer)->Release();
    }
    _indexBuffer->mp_IndexBuffer = 0;
}

void GFXAdapter_Directx9::setIndexBuffer( ITF_IndexBuffer* _indexBuffer )
{
    if(!_indexBuffer->mp_IndexBuffer) return;
    
    m_pd3dDevice->SetIndices((IDirect3DIndexBuffer9 *)_indexBuffer->mp_IndexBuffer );
}

void GFXAdapter_Directx9::LockIndexBuffer( ITF_IndexBuffer* _indexBuffer, void** _data )
{
    HRESULT hr;
    hr = ((LPDIRECT3DINDEXBUFFER9)_indexBuffer->mp_IndexBuffer)->Lock( 0, _indexBuffer->m_nIndices * sizeof(u16), _data, 0);
    
    if (FAILED(hr))
        messageD3DError(hr);
}

void GFXAdapter_Directx9::UnlockIndexBuffer( ITF_IndexBuffer* _indexBuffer )
{
    ((LPDIRECT3DINDEXBUFFER9)_indexBuffer->mp_IndexBuffer)->Unlock();
}

void GFXAdapter_Directx9::IndexBufferManagerOnLostDevice()
{
    m_VertexBufferManager.UnallocAllDynamicIB();
}
void GFXAdapter_Directx9::IndexBufferManagerOnResetDevice()
{
    m_VertexBufferManager.ReallocAllDynamicIB();
}
void GFXAdapter_Directx9::IndexBufferOnResetDevice(ITF_IndexBuffer* _indexBuffer )
{
    releaseIndexBuffer(_indexBuffer);
}
void GFXAdapter_Directx9::IndexBufferOnLostDevice(ITF_IndexBuffer* _indexBuffer )
{
    createIndexBuffer(_indexBuffer);
}

///----------------------------------------------------------------------------//
/// Draw Vertex/IndexBuffer.
///----------------------------------------------------------------------------//

void GFXAdapter_Directx9::DrawVertexBuffer( u32 _type, ITF_VertexBuffer* _vertexBuffer, u32 _vertexStart, u32 _vertexNumber)
{
    if(!_vertexBuffer || !_vertexBuffer->mp_VertexBuffer || !_vertexNumber) return;

    i32 type = 0;
    u32 numberprimitive = 0;

    switch(_type)
    {
    default:
    case GFX_TRIANGLES: 
        type = D3DPT_TRIANGLELIST;
        numberprimitive = _vertexNumber / 3;
        break;
    case GFX_POINTS:
        type = D3DPT_POINTLIST;
        numberprimitive = _vertexNumber;
        break;
    case GFX_LINES:
        type = D3DPT_LINELIST;
        numberprimitive = _vertexNumber / 2;
       break;
    case GFX_LINE_STRIP:
        type = D3DPT_LINESTRIP;
        numberprimitive = _vertexNumber - 1;
       break;
    case GFX_TRIANGLE_STRIP:
        type = D3DPT_TRIANGLESTRIP;
        numberprimitive = _vertexNumber - 2;
       break;
    case GFX_TRIANGLE_FAN:
        type = D3DPT_TRIANGLEFAN;
        numberprimitive = _vertexNumber - 2;
       break;
    }
    
    setVertexBuffer( _vertexBuffer );

#ifndef SHADERTEST
	beginShader(mp_currentShader);
	m_pd3dDevice->DrawPrimitive((D3DPRIMITIVETYPE)type, _vertexStart, numberprimitive );
	endShader(mp_currentShader);
#else
	u32 tech = mp_currentShader->m_selectedTech;
	if (m_ShaderMode == 0 || (tech != 0 && tech != 1 && tech != 2 && tech != 5) || mp_currentShader->m_type == 1)
	{
		beginShader(mp_currentShader);
		m_pd3dDevice->DrawPrimitive((D3DPRIMITIVETYPE)type, _vertexStart, numberprimitive );
		endShader(mp_currentShader);
	}
	else
	{
        if (m_currentRenderingMaterialType != GFX_MAT_REFRACTION && mp_currentShader->m_type != 2 && mp_currentShader->m_type != 3)
            setShaderFormat(_vertexBuffer->m_vertexFormat);
        else if (mp_currentShader->m_type != 2)
           	setShaderGlobalConstant(mp_shaderManager.getShaderByIndex(7));

		m_pd3dDevice->DrawPrimitive((D3DPRIMITIVETYPE)type, _vertexStart, numberprimitive );
	}
#endif


    m_pd3dDevice->SetStreamSource(0,0,0,0);
}

void GFXAdapter_Directx9::setShaderGlobalConstant(ITF_shader* _shadergroup)
{
	setShaderMatrix(_shadergroup);

	if (isUseFog())
	{
        GFX_Vector4 fog;
		fog.m_x = m_colorFog.getRed();
		fog.m_y = m_colorFog.getGreen();
		fog.m_z = m_colorFog.getBlue();
		fog.m_w = m_colorFog.getAlpha(); // blend factor in alpha

		//m_pd3dDevice->SetVertexShaderConstantF( 3, (f32*)&fog, 1 );//uvm
		m_pd3dDevice->SetPixelShaderConstantF( 16, (f32*)&fog, 1 );//uvm
	}
}

void GFXAdapter_Directx9::setShaderFormat(i32 _format)
{
	ITF_shader* shadergroup = mp_shaderManager.getShaderByIndex(7);    

	u32 vno = 0;
    u32 pno = 0;

	switch (_format)
	{
	default:
	case VertexFormat_PCT:
		vno = 0;
        pno = 0;
		break;
	case VertexFormat_PC:
		vno = 1;
        pno = 1;
		break;
	case VertexFormat_PT:
		vno = 2;
        pno = 2;
		break;
    case VertexFormat_PTa:
        vno = 11;
        pno = 2;
        break;
	case VertexFormat_PNC3T:
		vno = 5;
        pno = 0;
		break;
	}

	m_pd3dDevice->SetVertexShader( (IDirect3DVertexShader9*)shadergroup->mp_Vs[vno] );

	if (getOverDrawMode() > 1)
		m_pd3dDevice->SetPixelShader( (IDirect3DPixelShader9*)shadergroup->mp_Ps[6] );
	else
	{
		if (isUseFog())
			m_pd3dDevice->SetPixelShader( (IDirect3DPixelShader9*)shadergroup->mp_Ps[3] );
		else
			m_pd3dDevice->SetPixelShader( (IDirect3DPixelShader9*)shadergroup->mp_Ps[pno] );
	}

	setShaderGlobalConstant(shadergroup);
}

void GFXAdapter_Directx9::DrawIndexedVertexBuffer( u32 _type, ITF_VertexBuffer* _vertexBuffer, ITF_IndexBuffer* _indexBuffer, u32 _indexNumber )
{
    if(!_vertexBuffer || !_vertexBuffer->mp_VertexBuffer) 
        return;

    if (!_indexNumber)
        return;

    i32 type = 0;
    u32 numberprimitive = 0;

    switch(_type)
    {
    default:
    case GFX_TRIANGLES: 
        type = D3DPT_TRIANGLELIST;
        numberprimitive = _indexNumber/ 3;
        break;
    case GFX_POINTS:
        type = D3DPT_POINTLIST;
        numberprimitive = _indexNumber;
        break;
    case GFX_LINES:
        type = D3DPT_LINELIST;
        numberprimitive = _indexNumber / 2;
       break;
    case GFX_LINE_STRIP:
        type = D3DPT_LINESTRIP;
        numberprimitive = _indexNumber - 1;
       break;
    case GFX_TRIANGLE_STRIP:
        type = D3DPT_TRIANGLESTRIP;
        numberprimitive = _indexNumber - 2;
       break;
    case GFX_TRIANGLE_FAN:
        type = D3DPT_TRIANGLEFAN;
        numberprimitive = _indexNumber - 2;
       break;
    }
    
    setIndexBuffer( _indexBuffer );
    setVertexBuffer( _vertexBuffer );

#ifndef SHADERTEST
	beginShader(mp_currentShader);
	m_pd3dDevice->DrawIndexedPrimitive((D3DPRIMITIVETYPE)type, 0, 0, _vertexBuffer->m_nVertex, 0, numberprimitive );
	endShader(mp_currentShader);
#else
	u32 tech = mp_currentShader->m_selectedTech;
	if (m_ShaderMode == 0 || (tech != 0 && tech != 1 && tech != 2 && tech != 5 && tech != 11) || mp_currentShader->m_type == 1)
	{
		beginShader(mp_currentShader);
		m_pd3dDevice->DrawIndexedPrimitive((D3DPRIMITIVETYPE)_type, 0, 0, _vertexBuffer->m_nVertex, 0, numberprimitive );
		endShader(mp_currentShader);
	}
	else
	{
		if (m_currentRenderingMaterialType != GFX_MAT_REFRACTION && m_currentRenderingMaterialType != GFX_MAT_FRIEZEOVERLAY)
            setShaderFormat(_vertexBuffer->m_vertexFormat);
        else
           	setShaderGlobalConstant(mp_shaderManager.getShaderByIndex(7));

		m_pd3dDevice->DrawIndexedPrimitive((D3DPRIMITIVETYPE)_type, 0, 0, _vertexBuffer->m_nVertex, 0, numberprimitive );
		m_pd3dDevice->SetVertexShader( 0 );
		m_pd3dDevice->SetPixelShader( 0 );	
	}
#endif     
}

///----------------------------------------------------------------------------//
/// Shader Patch.
///----------------------------------------------------------------------------//
#define SHADER_SIZE 4

#define INDEX_BUFFER(a,b)       m_internalIdxBuffer##a##x##b
#define VERTEX_BUFFER(a,b)      m_internalVtxBuffer##a##x##b

#define CURENT_INDEX_BUFFER_(n)     INDEX_BUFFER (n, n)
#define CURENT_VERTEX_BUFFER_(n)    VERTEX_BUFFER(n, n)

#define CURENT_INDEX_BUFFER         CURENT_INDEX_BUFFER_(SHADER_SIZE)
#define CURENT_VERTEX_BUFFER        CURENT_VERTEX_BUFFER_(SHADER_SIZE)

void GFXAdapter_Directx9::setShaderPatchFlush()
{
	ITF_shader* shadergroup = 0;
	shadergroup = mp_shaderManager.getShaderByIndex(7);

	setShaderGlobalConstant(shadergroup);

	m_pd3dDevice->SetVertexShader( (IDirect3DVertexShader9*)shadergroup->mp_Vs[shadergroup->m_selectedVS] );
	m_pd3dDevice->SetPixelShader( (IDirect3DPixelShader9*)shadergroup->mp_Ps[shadergroup->m_selectedPS] );

	m_pd3dDevice->SetVertexShaderConstantF( 17, (f32*)mp_shaderManager.getBufferFloat(0), mp_shaderManager.getNumberVector4ConstToFlush(0) );//colorglob
	m_pd3dDevice->SetVertexShaderConstantF( 145, (f32*)mp_shaderManager.getBufferFloat(1), mp_shaderManager.getNumberVector4ConstToFlush(1) );//colorglob
}

void GFXAdapter_Directx9::shaderPatchFlushSize( u32 _hdiv, u32 _vdiv, u32 countPoint, u32 _pass, u32 _vdivToDraw /*= U32_INVALID*/ )
{
    if (!_vdivToDraw)//don't draw empty patch
        return;

    ITF_ASSERT_MSG( _hdiv != 0,"shaderPatchFlushSize hdiv is 0");
    ITF_ASSERT_MSG( _vdiv != 0,"shaderPatchFlushSize vdiv is 0");
    
    if (!mp_shaderManager.getNumberVector4ConstToFlush(0)) return;

#ifdef SHADERTEST
	if (m_ShaderMode == 0)
#endif	
	{
		/// Flush control Points.
		if (mp_currentShader->m_VectorArrayHandle[0])
		{
			mp_currentShader->setVectorArray(0, (GFX_Vector4*)mp_shaderManager.getBufferFloat(0), mp_shaderManager.getNumberVector4ConstToFlush(0));
		}
		else
		{
			ITF_ASSERT(0);
		}

		/// Flush vector color.
		if (mp_currentShader->m_VectorArrayHandle[1])
		{
			mp_currentShader->setVectorArray(1, (GFX_Vector4*)mp_shaderManager.getBufferFloat(1), mp_shaderManager.getNumberVector4ConstToFlush(1));
		}
		else
		{
			ITF_ASSERT(0);
		}
	}
#ifdef SHADERTEST
	else
	{
		setShaderPatchFlush();
	}
#endif
    /// draw patch.
    u32 nb = (mp_shaderManager.getNumberVector4ConstToFlush(0)/countPoint);
   
    if (nb == 1)
    {
        u32 prevVdiv = _vdiv;
        _vdiv = Min(_vdiv, _vdivToDraw);
#ifdef SHADERTEST
	if (m_ShaderMode)
	{
		f32 fdiv = (f32)prevVdiv / (f32)_vdiv;
		GFX_Vector4 vdiv(fdiv, fdiv, fdiv, fdiv);
		m_pd3dDevice->SetVertexShaderConstantF( 10, (f32*)&vdiv, 1 );
	}
	else
#endif    
		mp_currentShader->setFloat(2, (f32)prevVdiv / (f32)_vdiv);
	}
    else
    {
#ifdef SHADERTEST
	if (m_ShaderMode)
	{
		GFX_Vector4 vdiv(1.f, 1.f, 1.f, 1.f);
		m_pd3dDevice->SetVertexShaderConstantF( 10, (f32*)&vdiv, 1 );
	}
	else
#endif    
        mp_currentShader->setFloat(2, 1.f);
    }

#ifdef SHADERTEST
    if (m_ShaderMode == 0)
#endif
	{
		if (_pass == 0)
			beginShader(mp_currentShader);
		else
			mp_currentShader->commitChange();
	}

    if (m_showWireFrame < 2)
    {
        if ( m_drawFlag & DRWF_COPYTOTARGET )
        {
            setAlphaBlend(GFX_BLEND_ADDALPHA);
            m_pd3dDevice->SetRenderTarget( 0, m_pSwapTextureSurfDown4x4[0] );

            m_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, (_hdiv + 1)*(_vdiv + 1)*nb, 0, (_hdiv*_vdiv*2)*nb );

            setAlphaBlend(GFX_BLEND_ALPHA);
            m_pd3dDevice->SetRenderTarget( 0, m_currentRenderingSurface );
        }

        m_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, (_hdiv + 1)*(_vdiv + 1)*nb, 0, (_hdiv*_vdiv*2)*nb );
    }

    if (m_showWireFrame)
    {
        setFillMode(GFX_FILL_WIREFRAME);
        setAlphaBlend(GFX_BLEND_COPY);
        SetTextureBind(0, 0);
        m_pd3dDevice->SetTexture(0, 0);
        setGlobalColor(Color::white());
        m_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, (_hdiv + 1)*(_vdiv + 1)*nb, 0, (_hdiv*_vdiv*2)*nb );
        setFillMode(GFX_FILL_SOLID);
        setAlphaBlend(GFX_BLEND_ALPHA);
    }

    mp_shaderManager.setCurBuffFloatPos(0, 0);
    mp_shaderManager.setVector4ConstToFlush(0, 0);
    mp_shaderManager.setCurBuffFloatPos(1, 0);
    mp_shaderManager.setVector4ConstToFlush(1, 0);

#ifdef SHADERTEST
	if (m_ShaderMode != 0)
	{
		m_pd3dDevice->SetVertexShader( 0 );
		m_pd3dDevice->SetPixelShader( 0 );
	}
#endif
}

void GFXAdapter_Directx9::prepareShaderPatch(GMatrix44* _matrix, f32 _z, u32 & _hdiv, u32 & _vdiv, GFX_BLENDMODE _blendMode /*= GFX_BLEND_ALPHA*/)
{
    prepareShaderPatchTech(_matrix, _z, _hdiv, _vdiv, 3, _blendMode);
}


void GFXAdapter_Directx9::setShaderPreparePatch(u32 _idTech, f32 _z)
{
	ITF_shader* shadergroup = 0;
	shadergroup = mp_shaderManager.getShaderByIndex(7);    

	shadergroup->m_selectedVS = _idTech;

	if (getOverDrawMode() > 1)
		shadergroup->m_selectedPS = 6;
	else
	{
		if (isUseFog())
			shadergroup->m_selectedPS = 3;
		else
			shadergroup->m_selectedPS = 0;
	}

	m_pd3dDevice->SetVertexShader( (IDirect3DVertexShader9*)shadergroup->mp_Vs[shadergroup->m_selectedVS] );
	m_pd3dDevice->SetPixelShader( (IDirect3DPixelShader9*)shadergroup->mp_Ps[shadergroup->m_selectedPS] );

	setShaderGlobalConstant(shadergroup);

	GFX_Vector4 zz(_z, _z, _z, _z);
	m_pd3dDevice->SetVertexShaderConstantF( 9, (f32*)&zz, 1 );//uvm
}

void GFXAdapter_Directx9::prepareShaderPatchTech(GMatrix44* _matrix, f32 _z, u32 & _hdiv, u32 & _vdiv, u32 _idTech, GFX_BLENDMODE _blendMode /*= GFX_BLEND_ALPHA*/)
{
	setAlphaBlend(_blendMode);

    if (getOverDrawMode() > 1)
        setShader( mp_shaderManager.getShaderByIndex(4));
    else
    {
        if (isUseFog())
            setShader( mp_shaderManager.getShaderByIndex(5));
        else
            setShader(mp_defaultShader);
    }

    renderContext_Set();

    setObjectMatrix((D3DXMATRIX&)*_matrix);

    D3DXMatrixIdentity( &mg_UVmat );

    ITF_IndexBuffer *  indexBuffer  = getIndexBuffer(_hdiv, _vdiv);
    ITF_VertexBuffer * vertexBuffer = getVertexBuffer(_hdiv, _vdiv);
    ITF_ASSERT(indexBuffer && vertexBuffer);

    setIndexBuffer(indexBuffer);
    setVertexBuffer(vertexBuffer);

    /// set Vertex format
    setVertexFormat(VertexFormat_PT);

    mp_currentShader->m_selectedTech = _idTech;

    mp_shaderManager.setCurBuffFloatPos(0, 0);
    mp_shaderManager.setVector4ConstToFlush(0, 0);
    mp_shaderManager.setCurBuffFloatPos(1, 0);
    mp_shaderManager.setVector4ConstToFlush(1, 0);

#ifdef SHADERTEST
	if (m_ShaderMode == 0)
#endif	
	{
		/// set pos z to const.
		mp_currentShader->setFloat(1, _z);
	}
#ifdef SHADERTEST
	else
	{
		setShaderPreparePatch(_idTech, _z);
	}
#endif
}

void GFXAdapter_Directx9::shaderPatchFlush(u32 _hdiv, u32 _vdiv, u32 _pass)
{
    shaderPatchFlushSize( _hdiv, _vdiv, 8, _pass );   
}

void GFXAdapter_Directx9::shaderBezierPatchFlush(u32 _hdiv, u32 _vdiv, u32 _pass, u32 _vdivToDraw )
{
    shaderPatchFlushSize( _hdiv, _vdiv, 4, _pass, _vdivToDraw );   
}

void GFXAdapter_Directx9::endShaderPatch()
{
    // Make sure that a begin pass is called before !!
    if (getShaderPassLastState() == S_SHADERPASS_BEGIN)
        endShader(mp_currentShader);

    mp_shaderManager.setCurBuffFloatPos(0, 0);
    mp_shaderManager.setVector4ConstToFlush(0, 0);
    mp_shaderManager.setCurBuffFloatPos(0, 0);
    mp_shaderManager.setVector4ConstToFlush(0, 0);
    mp_shaderManager.setCurBuffFloatPos(1, 0);
    mp_shaderManager.setVector4ConstToFlush(1, 0);
    mp_shaderManager.setCurBuffFloatPos(1, 0);
    mp_shaderManager.setVector4ConstToFlush(1, 0);

    //renderContext_SetDefault();
}

///----------------------------------------------------------------------------//
/// Shader Bezier Patch.
///----------------------------------------------------------------------------//

void GFXAdapter_Directx9::prepareShaderBezierPatch(GMatrix44* _matrix, f32 _z, u32 & _hdiv, u32 & _vdiv, GFX_BLENDMODE _blendMode /*= GFX_BLEND_ALPHA*/)
{
      prepareShaderPatchTech(_matrix, _z, _hdiv, _vdiv, 10, _blendMode);
}

bbool GFXAdapter_Directx9::shaderDrawBezierPatch32(Texture *texture, Vec2d *PointsNS, ColorInteger* _lighttab, f32* _widths, f32* _alphas, f32* _widths2)
{
    ITF_ASSERT(_lighttab);

    f32 ratio = 1.f;

    /// set Texture;
    if (texture)
    {
        SetTextureBind(0, texture->m_adapterimplementationData);
        ratio = texture->getRatioXY();
        setTextureAdressingMode(0, GFXTADDRESS_WRAP, GFXTADDRESS_WRAP);
    }

    /// The stride of float Buffer is:
    /// Vector2 PointsNS for patch Vertex, Vector2 PointsNS + 8 that is for UV.
    /// this is for Vector4 shader: xy = vertex zw = uv.
    Vec2d * curPointNS  = PointsNS;
    Vec2d * curPointNS4 = PointsNS + 4;
    for (u32 i = 0; i < 4; i++, curPointNS++, curPointNS4++)
    {
        mp_shaderManager.setBufferFloatAt(0, mp_shaderManager.getCurBuffFloatPos(0), curPointNS->m_x);
        mp_shaderManager.addCurBuffFloatPos(0, 1);
        mp_shaderManager.setBufferFloatAt(0, mp_shaderManager.getCurBuffFloatPos(0), curPointNS->m_y);
        mp_shaderManager.addCurBuffFloatPos(0, 1);
        mp_shaderManager.setBufferFloatAt(0, mp_shaderManager.getCurBuffFloatPos(0), curPointNS4->m_x);
        mp_shaderManager.addCurBuffFloatPos(0, 1);
        mp_shaderManager.setBufferFloatAt(0, mp_shaderManager.getCurBuffFloatPos(0), curPointNS4->m_y * ratio);
        mp_shaderManager.addCurBuffFloatPos(0, 1);
    }

    /// number constant for control points is: f32 to vector4 -> 32 float / 4
    mp_shaderManager.addVector4ConstToFlush(0, 4);

    /// color.
    ColorInteger *curLight = _lighttab;
    for (u32 i = 0; i < 2; i++, curLight++)
    {
        mp_shaderManager.setBufferFloatAt(1, mp_shaderManager.getCurBuffFloatPos(1), curLight->getF32Red());
        mp_shaderManager.addCurBuffFloatPos(1, 1);
        mp_shaderManager.setBufferFloatAt(1, mp_shaderManager.getCurBuffFloatPos(1), curLight->getF32Green());
        mp_shaderManager.addCurBuffFloatPos(1, 1);
        mp_shaderManager.setBufferFloatAt(1, mp_shaderManager.getCurBuffFloatPos(1), curLight->getF32Blue());
        mp_shaderManager.addCurBuffFloatPos(1, 1);
        mp_shaderManager.setBufferFloatAt(1, mp_shaderManager.getCurBuffFloatPos(1), curLight->getF32Alpha());
        mp_shaderManager.addCurBuffFloatPos(1, 1);
    }

    mp_shaderManager.addVector4ConstToFlush(1, 2);

    /// Widths.
    for (u32 i = 0; i < 4; i++)
    {
        mp_shaderManager.setBufferFloatAt(1, mp_shaderManager.getCurBuffFloatPos(1), _widths[i]);
        mp_shaderManager.addCurBuffFloatPos(1, 1);
    }

    mp_shaderManager.addVector4ConstToFlush(1, 1);
 

    /// Alphas
    for (u32 i = 0; i < 4; i++)
    {
        mp_shaderManager.setBufferFloatAt(1, mp_shaderManager.getCurBuffFloatPos(1), _alphas[i]);
        mp_shaderManager.addCurBuffFloatPos(1, 1);
    }

    mp_shaderManager.addVector4ConstToFlush(1, 1);

    /// width2
    for (u32 i = 0; i < 4; i++)
    {
        mp_shaderManager.setBufferFloatAt(1, mp_shaderManager.getCurBuffFloatPos(1), _widths2[i]);
        mp_shaderManager.addCurBuffFloatPos(1, 1);
    }

    mp_shaderManager.addVector4ConstToFlush(1, 1);


#ifndef SHADERTEST
    if (isUseFog())
    {
        GFX_Vector4 fog;
        fog.m_x = m_colorFog.getRed();
        fog.m_y = m_colorFog.getGreen();
        fog.m_z = m_colorFog.getBlue();
        fog.m_w = m_colorFog.getAlpha(); // blend factor in alpha

        // color Fog.
        if (mp_currentShader->m_VecHandle[3])
            mp_currentShader->setVector(3, &fog);
    }
#endif

    return 1;
}

///----------------------------------------------------------------------------//
/// Shader Spline.
///----------------------------------------------------------------------------//

#define FILL_BUFFER_VECT(bufferVect, bufferPoint, _spline, splinePoint, _pos) \
    bufferVect[bufferPoint].m_x = _spline->GetNthPoint(splinePoint).m_x + _pos.m_x; \
    bufferVect[bufferPoint].m_y = _spline->GetNthPoint(splinePoint).m_y + _pos.m_y; \
    bufferVect[bufferPoint].m_z = _spline->GetNthPoint(splinePoint).m_z + _pos.m_z; \
    bufferVect[bufferPoint].m_w = _spline->getInterpAt(splinePoint) == Spline::interp_linear ? _spline->getTimeAt(splinePoint) : -_spline->getTimeAt(splinePoint); \
    bufferPoint++;

#define MAX_SPLINE_EFFECTIVE_POINTS 16
#define SPLINE_BUFFER_SIZE (MAX_SPLINE_EFFECTIVE_POINTS + 2) // 1 for the begin, 1 for the end

void GFXAdapter_Directx9::drawSpline(GMatrix44* _matrix, Texture* _texture, const Spline * _spline, const Vec3d &_pos, f32 _height )
{
    if (!_spline || _spline->GetNumPoints() < 2)
        return;
    /// set Texture;
    setShader( mp_defaultShader );

    if (_texture)
    {
        SetTextureBind(0, _texture->m_adapterimplementationData);
        setTextureAdressingMode(0, GFXTADDRESS_WRAP, GFXTADDRESS_WRAP);
    }

    renderContext_Set();

    setObjectMatrix((D3DXMATRIX&)*_matrix);

    D3DXMatrixIdentity( &mg_UVmat );

    u32 hdiv = MESH_DEFAULT_VDIV;
    u32 vdiv = MESH_DEFAULT_HDIV;

    ITF_IndexBuffer *  indexBuffer  = getIndexBuffer(hdiv, vdiv);
    ITF_VertexBuffer * vertexBuffer = getVertexBuffer(hdiv, vdiv);
    ITF_ASSERT(indexBuffer && vertexBuffer);

    setIndexBuffer(indexBuffer);
    setVertexBuffer(vertexBuffer);

    /// set Vertex format
    setVertexFormat(VertexFormat_PT);
    mp_currentShader->m_selectedTech = 9;

    GFX_Vector4 v0(m_globalColor.getRed(), m_globalColor.getGreen(), m_globalColor.getBlue(), m_globalColor.getAlpha());
    GFX_Vector4 bufferVect[SPLINE_BUFFER_SIZE];

    // max 16 points by spline
    i32 point       = _spline->GetNumPoints();
    i32 splinePoint = 0;
    i32 bufferPoint = 0;

    FILL_BUFFER_VECT(bufferVect, bufferPoint, _spline, splinePoint, _pos);

    if (point < 2)
        return;

    while (point)
    {
        FILL_BUFFER_VECT(bufferVect, bufferPoint, _spline, splinePoint, _pos);

        if (bufferPoint >= SPLINE_BUFFER_SIZE || point == 1)
        {
            if (point == 1 && bufferPoint < SPLINE_BUFFER_SIZE)
            {
                 FILL_BUFFER_VECT(bufferVect, bufferPoint, _spline, splinePoint, _pos);
            }

#ifdef SHADERTEST
            if (m_ShaderMode == 1)
            {
		        ITF_shader* shadergroup = mp_shaderManager.getShaderByIndex(7);
                m_pd3dDevice->SetVertexShader( (IDirect3DVertexShader9*)shadergroup->mp_Vs[9] );
				m_pd3dDevice->SetPixelShader( (IDirect3DPixelShader9*)shadergroup->mp_Ps[0] );

        		m_pd3dDevice->SetVertexShaderConstantF( 17, (f32*)&bufferVect, bufferPoint );
                m_pd3dDevice->SetVertexShaderConstantF( 13, (f32*)&v0, 1 );
               
                GFX_Vector4 vconsts((f32)bufferPoint, (f32)_height, 0.f, 0.f);
                m_pd3dDevice->SetVertexShaderConstantF( 14, (f32*)&vconsts, 1 );

                setShaderGlobalConstant(shadergroup);

                m_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, (hdiv + 1)*(vdiv + 1), 0, (hdiv*vdiv*2) );

		        m_pd3dDevice->SetVertexShader( 0 );
		        m_pd3dDevice->SetPixelShader( 0 );
            }
            else
#endif
            {
                if (mp_currentShader->m_VectorArrayHandle[0])
                {
                    mp_currentShader->setVectorArray(0, bufferVect, bufferPoint);
                    mp_currentShader->setVector(0, &v0);
                    mp_currentShader->setFloat(0, (f32)bufferPoint);  
                    mp_currentShader->setFloat(1, (f32)_height);  
                }
                else
                {
                    ITF_ASSERT(0);
                }

                beginShader(mp_currentShader);
                m_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, (hdiv + 1)*(vdiv + 1), 0, (hdiv*vdiv*2) );
                endShader(mp_currentShader);
            }

            if (point == 1)
                break;

            bufferPoint = 0;
            splinePoint -= 2;
        } else
        {
            splinePoint++;
            point--;
        }
    }
}

///----------------------------------------------------------------------------//
/// Shader Fluid.
///----------------------------------------------------------------------------//
void GFXAdapter_Directx9::drawFluid(   GMatrix44* _matrix, Texture* _texture, const FixedArray<u32, GFXADAPTER_FLUID_COLORTAB_ENTRY_COUNT> &_colorTab, const Vec2d _heightTab[], const u32 _heightTabSize, const FixedArray<Vec2d, 4> &_uvTab, u32 mode, f32 _delta1, f32 _delta2, f32 _stepCount  )
{
    /// set Texture;
    if (getOverDrawMode() > 1)
        setShader( mp_shaderManager.getShaderByIndex(4));
    else
    {
        if (isUseFog())
            setShader( mp_shaderManager.getShaderByIndex(5));
        else
            setShader(mp_defaultShader);
    }

    if (!_texture)
    {
        return;
    }

    if (!_texture->isPhysicallyLoaded())
    {
        return;
    }

        SetTextureBind(0, _texture->m_adapterimplementationData);
        setTextureAdressingMode(0, GFXTADDRESS_WRAP, GFXTADDRESS_WRAP);

    renderContext_Set();
 
    setObjectMatrix((D3DXMATRIX&)*_matrix);

    D3DXMatrixIdentity( &mg_UVmat );

    u32 hdiv = MESH_DEFAULT_HDIV;
    u32 vdiv = MESH_DEFAULT_VDIV;
    
    ITF_IndexBuffer *  indexBuffer  = getIndexBuffer(hdiv, vdiv);
    ITF_VertexBuffer * vertexBuffer = getVertexBuffer(hdiv, vdiv);
    ITF_ASSERT(indexBuffer && vertexBuffer);

    setIndexBuffer(indexBuffer);
    setVertexBuffer(vertexBuffer);

    /// set Vertex format
    setVertexFormat(VertexFormat_PT);

	if (!mode)
        mp_currentShader->m_selectedTech = 6;
    else
        mp_currentShader->m_selectedTech = 7;

    /// set pos z to const.

    GFX_Vector4 v0(m_globalColor.getRed(), m_globalColor.getGreen(), m_globalColor.getBlue(), m_globalColor.getAlpha());
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

    GFX_Vector4 colorVect[GFXADAPTER_FLUID_COLORTAB_ENTRY_COUNT];
    u32 colorCount = _colorTab.size();
    ITF_ASSERT_CRASH(colorCount<=GFXADAPTER_FLUID_COLORTAB_ENTRY_COUNT, "TOO MANY COLORS");
    for (u32 i=0; i<colorCount; i++)
    {
        Color           color(_colorTab[i]);
        GFX_Vector4     &colorVectElmt = colorVect[i];
        colorVectElmt.m_x = color.getRed(); 
        colorVectElmt.m_y = color.getGreen();
        colorVectElmt.m_z = color.getBlue();
        colorVectElmt.m_w = color.getAlpha();
    }


    GFX_Vector4 bufferVect[64 + 2];

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

#ifdef SHADERTEST	
	if (m_ShaderMode)
	{
		m_pd3dDevice->SetVertexShaderConstantF( 17, (f32*)bufferVect, hdivCount );
		m_pd3dDevice->SetVertexShaderConstantF( 145, (f32*)colorVect, GFXADAPTER_FLUID_COLORTAB_ENTRY_COUNT );
		m_pd3dDevice->SetVertexShaderConstantF( 13, (f32*)&v0, 1 );
		m_pd3dDevice->SetVertexShaderConstantF( 14, (f32*)&v1, 1 );
		m_pd3dDevice->SetVertexShaderConstantF( 15, (f32*)&v2, 1 );

		f32 fvdid = (f32)vdiv;
		f32 flen = (f32)vLen;

		GFX_Vector4 vvdiv(fvdid, fvdid, fvdid, fvdid);
		GFX_Vector4 vvLen(flen, flen, flen, flen);
		GFX_Vector4 vstepCount(_stepCount, _stepCount, _stepCount, _stepCount);
		GFX_Vector4 vdelta1(_delta1, _delta1, _delta1, _delta1);
		GFX_Vector4 vdelta2(_delta2, _delta2, _delta2, _delta2);

		m_pd3dDevice->SetVertexShaderConstantF( 8, (f32*)&vvdiv, 1 );
		m_pd3dDevice->SetVertexShaderConstantF( 9, (f32*)&vvLen, 1 );
		m_pd3dDevice->SetVertexShaderConstantF( 10, (f32*)&vstepCount, 1 );
		m_pd3dDevice->SetVertexShaderConstantF( 11, (f32*)&vdelta1, 1 );
		m_pd3dDevice->SetVertexShaderConstantF( 12, (f32*)&vdelta2, 1 );

		ITF_shader* shadergroup = 0;
		shadergroup = mp_shaderManager.getShaderByIndex(7);

		setShaderGlobalConstant(shadergroup);

		u32 no = 7;
		if (!mode)
			no = 6;
		
		m_pd3dDevice->SetVertexShader( (IDirect3DVertexShader9*)shadergroup->mp_Vs[no] );

		if (getOverDrawMode() > 1)
			m_pd3dDevice->SetPixelShader( (IDirect3DPixelShader9*)shadergroup->mp_Ps[6] );
		else
		{
			if (isUseFog())
				m_pd3dDevice->SetPixelShader( (IDirect3DPixelShader9*)shadergroup->mp_Ps[3] );
			else
				m_pd3dDevice->SetPixelShader( (IDirect3DPixelShader9*)shadergroup->mp_Ps[0] );
		}
		
		m_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, (hdiv + 1)*(vLen + 1), 0, (hdiv*vLen*2) );

		m_pd3dDevice->SetVertexShader( 0 );
		m_pd3dDevice->SetPixelShader( 0 );
	}
	else
#endif
{
    /// Flush control Points.
    if (mp_currentShader->m_VectorArrayHandle[0])
    {
        mp_currentShader->setVectorArray(0, bufferVect, hdivCount);
        mp_currentShader->setVectorArray(1, colorVect, GFXADAPTER_FLUID_COLORTAB_ENTRY_COUNT);
        mp_currentShader->setVector(0, &v0);  
        mp_currentShader->setVector(1, &v1);  
        mp_currentShader->setVector(2, &v2);
        mp_currentShader->setFloat(0, (f32)vdiv);  
        mp_currentShader->setFloat(1, (f32)vLen);  
        mp_currentShader->setFloat(2, (f32)_stepCount);
        mp_currentShader->setFloat(3, (f32)_delta1);
        mp_currentShader->setFloat(4, (f32)_delta2);
    }
    else
    {
        ITF_ASSERT(0);
    }

    beginShader(mp_currentShader);
    m_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, (hdiv + 1)*(vLen + 1), 0, (hdiv*vLen*2) );
    endShader(mp_currentShader);
}

}

///----------------------------------------------------------------------------//
//Movie
///----------------------------------------------------------------------------//

void GFXAdapter_Directx9::drawMovie( GMatrix44* _matrix,f32 _alpha,ITF_VertexBuffer* _pVertexBuffer,Texture** _pTextureArray,u32 _countTexture)
{
    setVertexBuffer(_pVertexBuffer);
    setVertexFormat(VertexFormat_PT);

#ifndef SHADERTEST
    ITF_shader *pMovieShader = mp_shaderManager.getShaderByIndex(6);
    setShader( pMovieShader);

#ifdef ITF_X360
    pMovieShader->m_selectedTech = 0;
#else
    if (_countTexture == 4)
        pMovieShader->m_selectedTech = 2;
    else
        pMovieShader->m_selectedTech = 1;
#endif

    pMovieShader->setFloat(0,_alpha);
#endif

    D3DXMATRIX   mgViewSaved         = mg_View;
    D3DXMATRIX   mgWorldSaved        = mg_World;
    D3DXMATRIX   mgViewRenderSaved   = mg_ViewRender;
        
    D3DXMatrixIdentity( &mg_World );
    D3DXMatrixIdentity( &mg_View );
    D3DXMatrixIdentity( &mg_ViewRender );

#ifdef SHADERTEST
    ITF_shader* shadergroup = mp_shaderManager.getShaderByIndex(8);
    setShader( shadergroup);
    
    HRESULT hr;
    hr = m_pd3dDevice->SetVertexShader( (IDirect3DVertexShader9*)shadergroup->mp_Vs[0] );
    hr = m_pd3dDevice->SetPixelShader( (IDirect3DPixelShader9*)shadergroup->mp_Ps[0] );

    setShaderMatrix(shadergroup);
#else
    setShaderMatrix(pMovieShader);
#endif

    for (u32 index = 0; index < _countTexture; index++)
        SetTextureBind(index,_pTextureArray[index]->m_adapterimplementationData);

    for( u32 i = 0 ; i < _countTexture ; i++ )
    {
        m_pd3dDevice->SetSamplerState( i, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
        m_pd3dDevice->SetSamplerState( i, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );
        m_pd3dDevice->SetSamplerState( i, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
        m_pd3dDevice->SetSamplerState( i, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
    }

#ifdef SHADERTEST
    GFX_Vector4 vconsts(_alpha, _alpha, _alpha, _alpha);
	m_pd3dDevice->SetPixelShaderConstantF( 8, (f32*)&vconsts, 1 );
#else
    beginShader(pMovieShader);
#endif

#ifdef ITF_X360
    m_pd3dDevice->SetRenderState( D3DRS_VIEWPORTENABLE, TRUE );
    m_pd3dDevice->DrawPrimitive( D3DPT_QUADLIST, 0, 1 );
#else
    m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
#endif //ITF_X360

#ifdef SHADERTEST
    m_pd3dDevice->SetVertexShader( 0 );
    m_pd3dDevice->SetPixelShader( 0 );
#else
    endShader( pMovieShader);
#endif

    mg_View         = mgViewSaved;
    mg_World        = mgWorldSaved;
    mg_ViewRender   = mgViewRenderSaved;

    for (u32 index = 0;index<_countTexture;index++)
        SetTextureBind(index, 0);
}


///----------------------------------------------------------------------------//
/// Shader Trail.
///----------------------------------------------------------------------------//
void GFXAdapter_Directx9::drawTrail3D(Texture* _texture, Color _color, const SafeArray<Vec3d> & _points, f32 _alphaBegin, f32 _alphaEnd, f32 _fadeLength )
{
    if (_points.size() <= 2)
        return;

    if (getOverDrawMode() > 1)
        setShader( mp_shaderManager.getShaderByIndex(4));
    else
    {
        if (isUseFog())
            setShader( mp_shaderManager.getShaderByIndex(5));
        else
            setShader(mp_defaultShader);
    }
    
    /// set Texture;
    if (_texture)
    {
        SetTextureBind(0, _texture->m_adapterimplementationData);
        setTextureAdressingMode(0, GFXTADDRESS_WRAP, GFXTADDRESS_WRAP);
    }

    renderContext_Set();

    D3DXMATRIX World;
    D3DXMatrixIdentity(&World);
    setObjectMatrix(World);
    D3DXMatrixIdentity( &mg_UVmat );


    u32 hdiv = MESH_DEFAULT_HDIV;
    u32 vdiv = MESH_DEFAULT_VDIV;

    ITF_IndexBuffer *  indexBuffer  = getIndexBuffer(hdiv, vdiv);
    ITF_VertexBuffer * vertexBuffer = getVertexBuffer(hdiv, vdiv);
    ITF_ASSERT(indexBuffer && vertexBuffer);

    setIndexBuffer(indexBuffer);
    setVertexBuffer(vertexBuffer);

    /// set Vertex format
    setVertexFormat(VertexFormat_PT);
    mp_currentShader->m_selectedTech = 8;

    /// set pos z to const.

    GFX_Vector4 v0(_color.m_r, _color.m_g, _color.m_b, _color.m_a);
    GFX_Vector4 v1(1.f, 1.f, 1.f, _alphaBegin);
    GFX_Vector4 v2(1.f, 1.f, 1.f, _alphaEnd);

    GFX_Vector4 bufferVectXY[128];
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

    GFX_Vector4 bufferVectZ[32];
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

#ifdef SHADERTEST
if (m_ShaderMode)
{
		m_pd3dDevice->SetVertexShaderConstantF( 17, (f32*)&bufferVectXY, bufferVectXYCount );
		m_pd3dDevice->SetVertexShaderConstantF( 145, (f32*)&bufferVectZ, bufferVectZCount );
		m_pd3dDevice->SetVertexShaderConstantF( 13, (f32*)&v0, 1 );
		m_pd3dDevice->SetVertexShaderConstantF( 14, (f32*)&v1, 1 );
		m_pd3dDevice->SetVertexShaderConstantF( 15, (f32*)&v2, 1 );

		f32 fvid = (f32)vdiv;
		GFX_Vector4 vvdiv(fvid, fvid, fvid, fvid);
		f32 count = (f32)bufferVectXYCount;
		GFX_Vector4 vcount(count, count, count, count);
		GFX_Vector4 vfade(_fadeLength, _fadeLength, _fadeLength, _fadeLength);

		m_pd3dDevice->SetVertexShaderConstantF( 8, (f32*)&vvdiv, 1 );
		m_pd3dDevice->SetVertexShaderConstantF( 9, (f32*)&vcount, 1 );
		m_pd3dDevice->SetVertexShaderConstantF( 10, (f32*)&vfade, 1 );
	
		ITF_shader* shadergroup = mp_shaderManager.getShaderByIndex(7);    
		m_pd3dDevice->SetVertexShader( (IDirect3DVertexShader9*)shadergroup->mp_Vs[8] );

		if (getOverDrawMode() > 1)
			m_pd3dDevice->SetPixelShader( (IDirect3DPixelShader9*)shadergroup->mp_Ps[6] );
		else
		{
			if (isUseFog())
				m_pd3dDevice->SetPixelShader( (IDirect3DPixelShader9*)shadergroup->mp_Ps[3] );
			else
				m_pd3dDevice->SetPixelShader( (IDirect3DPixelShader9*)shadergroup->mp_Ps[0] );
		}

		setShaderGlobalConstant(shadergroup);

		m_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, (hdiv + 1)*(nbPoints/2), 0, (hdiv*(nbPoints/2-1)*2) );

		m_pd3dDevice->SetVertexShader( 0 );
		m_pd3dDevice->SetPixelShader( 0 );

		// restore color
		static const GFX_Vector4 defaultColor(1.f, 1.f, 1.f, 1.f);
	
		m_pd3dDevice->SetVertexShaderConstantF( 13, (f32*)&defaultColor, 1 );
}
	else
#endif
{
    /// Flush control Points.
    if (mp_currentShader->m_VectorArrayHandle[0])
    {
        mp_currentShader->setVectorArray(0, bufferVectXY, bufferVectXYCount);
        mp_currentShader->setVectorArray(1, bufferVectZ, bufferVectZCount);
        mp_currentShader->setVector(0, &v0);  
        mp_currentShader->setVector(1, &v1);  
        mp_currentShader->setVector(2, &v2);
        mp_currentShader->setFloat(0, (f32)vdiv);  
        mp_currentShader->setFloat(1, (f32)bufferVectXYCount);  
        mp_currentShader->setFloat(2, _fadeLength);
    }
    else
    {
        ITF_ASSERT(0);
    }

    beginShader(mp_currentShader);
    m_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, (hdiv + 1)*(nbPoints/2), 0, (hdiv*(nbPoints/2-1)*2) );
    endShader(mp_currentShader);

    // restore Default Color constant:
    static const GFX_Vector4 defaultColor(1.f, 1.f, 1.f, 1.f);
    mp_currentShader->setVector(0, &defaultColor);
}

}

///----------------------------------------------------------------------------//
/// Fx.
///----------------------------------------------------------------------------//

void GFXAdapter_Directx9::fx_setVectorArray(uPtr _fx, void* _h, const GFX_Vector4* _va, u32 _count)
{
//#ifdef NO_FX
#ifdef ITF_X360
    ((ITF_EFFECTP)_fx)->SetVectorArrayF((ITF_HANDLE)_h, (float*)_va, _count);
#else
    ((ITF_EFFECTP)_fx)->SetVectorArray((ITF_HANDLE)_h, (D3DXVECTOR4*)_va, _count);
#endif
//#endif
}

void GFXAdapter_Directx9::fx_setVector(uPtr _fx, void* _h, const GFX_Vector4* _v)
{
//#ifdef NO_FX
#ifdef ITF_X360
    ((ITF_EFFECTP)_fx)->SetVector((ITF_HANDLE)_h, *(FXLVECTOR*) _v);
#else
    ((ITF_EFFECTP)_fx)->SetVector((ITF_HANDLE)_h, (D3DXVECTOR4*) _v);
#endif
//#endif
}

void GFXAdapter_Directx9::fx_setFloat(uPtr _fx, void* _h, float _f)
{
//#ifdef NO_FX
    ((ITF_EFFECTP)_fx)->SetFloat((ITF_HANDLE)_h, _f);
//#endif
}

void GFXAdapter_Directx9::fx_setInt(uPtr _fx, void* _h, i32 _i)
{
//#ifdef NO_FX
    ((ITF_EFFECTP)_fx)->SetInt((ITF_HANDLE)_h, _i);
//#endif
}

void GFXAdapter_Directx9::fx_commitChange(uPtr _fx )
{
//#ifdef NO_FX
#ifdef ITF_X360
    ((ITF_EFFECTP)_fx)->Commit();
#else
    ((ITF_EFFECTP)_fx)->CommitChanges();
#endif
//#endif
}

///----------------------------------------------------------------------------//
/// AfterFx.
///----------------------------------------------------------------------------//
void GFXAdapter_Directx9::prepareInternalRT()
{
    if (!m_pFullRTtex[1])
    {
        createRenderTarget(&m_pFullRTtex[1], &m_pFullRTsurf[1], getScreenWidth(), getScreenHeight(), D3DFMT_A8R8G8B8);
    }

    m_pd3dDevice->SetRenderTarget( 0, m_pFullRTsurf[1]);
    clear(GFX_CLEAR_COLOR, m_internRTColor.getRed(), m_internRTColor.getGreen(), m_internRTColor.getBlue(), m_internRTColor.getAlpha()); 
}

void GFXAdapter_Directx9::endInternalRT()
{
    setBackBuffer();
}

void GFXAdapter_Directx9::prepareAfterFx(bbool _nocopy)
{
        GFX_mWorldViewProj = mg_WorldViewProj;

        init2DRender();
        if (_nocopy) return;

        setAlphaRef(1);

        if (m_useInternalRT)
        {
            resolve( m_pFullRTtex[1] );
            m_pCurrentSwapSourceTexture = m_pFullRTtex[1];
        }
        else
        {
            if (!m_pFullRTtex[1])
            {
                createRenderTarget(&m_pFullRTtex[1], &m_pFullRTsurf[1], getScreenWidth(), getScreenHeight(), D3DFMT_A8R8G8B8);
            }        

            /// if the current rendering target is the backbuffer then resolve it to texture.
            /// to the m_pFullRTtex[0].
            if (m_currentRenderingSurface == m_pBackBufferSurf)
            {
                copyCurrentColorBuffer(0);
                m_pd3dDevice->SetRenderTarget( 0, m_pFullRTsurf[1]);
                m_pCurrentSwapSourceTexture = m_pFullRTtex[0];
                m_pCurrentSwapRenderSurf = m_pFullRTsurf[1];
            }

            else if (m_currentRenderingSurface == m_pFullRTsurf[1])
            {
                resolve( m_pFullRTtex[1] );
                m_pd3dDevice->SetRenderTarget( 0, m_pFullRTsurf[0]);
    #ifndef ITF_X360 
                clear( GFX_CLEAR_COLOR , 0, 0, 0, 0); 
    #endif 
                m_pCurrentSwapSourceTexture = m_pFullRTtex[1];
                m_pCurrentSwapRenderSurf = m_pFullRTsurf[0];
            }
        }
        //else if (m_currentRenderingSurface == m_pFullRTsurf[1])
        //    resolve( m_pFullRTtex[1] );
        setShader(mp_shaderManager.getShaderByIndex(1));
}

void GFXAdapter_Directx9::endAfterfx(bbool _nocopy, GFX_BLENDMODE _finalBlend)
{
        /// restore default shader renderer.
        setShader(mp_defaultShader);

        if (!_nocopy)
        {
            setBackBuffer();
#ifdef ITF_X360
            /// must restitute backbuffer save before blend.
            SetTextureBind(0,(u32)m_pFullRTtex[0]);
            m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
            m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
            setAlphaBlend(GFX_BLEND_COPY);
            drawScreenQuad(0,0,(f32)getScreenWidth(), (f32)getScreenHeight(),0, COLOR_WHITE );
#endif
            SetTextureBind(0, (void *)m_pCurrentSwapSourceTexture);
            m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
            m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
            setAlphaBlend(_finalBlend);
         
            drawScreenQuad(0,0,(f32)getScreenWidth(), (f32)getScreenHeight(),0, COLOR_WHITE );

            setAlphaBlend(GFX_BLEND_ALPHA);

            /// Restore View Matrix.
            mg_WorldViewProj = GFX_mWorldViewProj;
        }

        CAMERA->apply();
}

void GFXAdapter_Directx9::AFTERFX_draw(AFX_Group* _group)
{
    if (m_useAFTERFX)
    {
        TIMERASTER_SCOPE(afterFX);

        prepareAfterFx(bfalse);
#if 0
        init2DRender();
        setAlphaRef(1);

        /// if the current rendering target is the backbuffer then resolve it to texture.
        /// to the m_pFullRTtex[0].
        if (m_currentRenderingSurface == m_pBackBufferSurf)
            copyCurrentColorBuffer(0);
        
        if (!m_pFullRTtex[1])
        {
            createRenderTarget(&m_pFullRTtex[1], &m_pFullRTsurf[1], getScreenWidth(), getScreenHeight(), D3DFMT_A8R8G8B8);
        }        

        m_pd3dDevice->SetRenderTarget( 0, m_pFullRTsurf[1]);
#ifndef ITF_X360 
        clear( GFX_CLEAR_COLOR , 0, 0, 0, 0); 
#endif        
        m_pCurrentSwapSourceTexture = m_pFullRTtex[0];
        
        
        m_pCurrentSwapRenderSurf = m_pFullRTsurf[1];

        setShader(mp_shaderManager.getShaderByIndex(1));
#endif


        u32 size = _group->getNumberFx();
        for (u32 i = 0; i< size ;i++)
        {
            AFTERFX* afx = _group->getFxAt(i);
            AFTERFX_drawOne(afx);
        }

#if 0
        /// restore default shader renderer.
        setShader(mp_defaultShader);

 #ifndef ITF_X360
        setBackBuffer();

        SetTextureBind(0,(u32)m_pCurrentSwapSourceTexture);
        m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
        m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
        setAlphaBlend((GFX_BLENDMODE)_group->getFinalBlend());
         
        drawScreenQuad(0,0,(f32)getScreenWidth(), (f32)getScreenHeight(),0, COLOR_WHITE );


#endif
        /// Restore View Matrix.
        D3DXMATRIX mg_WorldViewProj = saveView;

        CAMERA->apply();
#endif    
    
        endAfterfx(bfalse, (GFX_BLENDMODE)_group->getFinalBlend());
    }

    setAlphaBlend(GFX_BLEND_COPY);
    setDefaultAlpha();
}

void GFXAdapter_Directx9::AFTERFX_onResetDevice()
{
    //createFxTarget();
}

void GFXAdapter_Directx9::AFTERFX_onLostDevice()
{
    AFTERFX_destroyTarget();
}

void GFXAdapter_Directx9::createFxTargetDown2x2()
{
    u32 targetsizeX = (u32)(getScreenWidth() * 0.5f);
    u32 targetsizeY = (u32)(getScreenHeight() * 0.5f);

    for (u32 i = 0; i < 2;i++)
    {
        if (!m_pSwapTextureDown2x2[i])
        {
            // Create fullscreen renders target texture
            createRenderTarget(&m_pSwapTextureDown2x2[i], &m_pSwapTextureSurfDown2x2[i], targetsizeX, targetsizeY, D3DFMT_A8R8G8B8, btrue);

            m_pd3dDevice->SetRenderTarget( 0, m_pSwapTextureSurfDown2x2[i]);
            clear( GFX_CLEAR_COLOR , 0, 0, 0, 0); 
        }
    }

    /// restore back buffer.
    setBackBuffer();
}

void GFXAdapter_Directx9::createFxTargetDown4x4()
{
    u32 targetsizeX = (u32)(getScreenWidth() * 0.25f);
    u32 targetsizeY = (u32)(getScreenHeight() * 0.25f);

    for (u32 i = 0; i < 2;i++)
    {
        if (!m_pSwapTextureDown4x4[i])
        {
            // Create fullscreen renders target texture
            createRenderTarget(&m_pSwapTextureDown4x4[i], &m_pSwapTextureSurfDown4x4[i], targetsizeX, targetsizeY, D3DFMT_A8R8G8B8, btrue);

            m_pd3dDevice->SetRenderTarget( 0, m_pSwapTextureSurfDown4x4[i]);
            clear( GFX_CLEAR_COLOR , 0, 0, 0, 0); 
        }
    }

    /// restore back buffer.
    setBackBuffer();
}

void GFXAdapter_Directx9::AFTERFX_destroyTarget()
{
    if (m_pSwapTextureDown2x2[0])
        m_pSwapTextureDown2x2[0]->Release();
    if (m_pSwapTextureDown2x2[1])
        m_pSwapTextureDown2x2[1]->Release();

    if (m_pSwapTextureSurfDown2x2[0])
        m_pSwapTextureSurfDown2x2[0]->Release();
    if (m_pSwapTextureSurfDown2x2[1])
        m_pSwapTextureSurfDown2x2[1]->Release();

    m_pSwapTextureSurfDown2x2[0] = 0;
    m_pSwapTextureSurfDown2x2[1] = 0;
    m_pSwapTextureDown2x2[0] = 0;
    m_pSwapTextureDown2x2[1] = 0;


    if (m_pSwapTextureDown4x4[0])
        m_pSwapTextureDown4x4[0]->Release();
    if (m_pSwapTextureDown4x4[1])
        m_pSwapTextureDown4x4[1]->Release();

    if (m_pSwapTextureSurfDown4x4[0])
        m_pSwapTextureSurfDown4x4[0]->Release();
    if (m_pSwapTextureSurfDown4x4[1])
        m_pSwapTextureSurfDown4x4[1]->Release();

    m_pSwapTextureSurfDown4x4[0] = 0;
    m_pSwapTextureSurfDown4x4[1] = 0;
    m_pSwapTextureDown4x4[0] = 0;
    m_pSwapTextureDown4x4[1] = 0;
}

void GFXAdapter_Directx9::restoreDefaultShader()
{
#ifdef SHADERTEST
	m_pd3dDevice->SetVertexShader( 0 );
	m_pd3dDevice->SetPixelShader( 0 );
#else
    setShader(mp_defaultShader);
    mp_currentShader->m_selectedTech = 0;
#endif
}

void GFXAdapter_Directx9::setShaderForFont(bbool bOutline)
{
    //TODO change POPO ;need a name instead of index
#ifdef SHADERTEST
    ITF_shader* shadergroup = mp_shaderManager.getShaderByIndex(9);
    setShader(shadergroup);
    shadergroup->m_type = 3;
    m_pd3dDevice->SetVertexShader( (IDirect3DVertexShader9*)shadergroup->mp_Vs[0] );
    m_pd3dDevice->SetPixelShader( (IDirect3DPixelShader9*)shadergroup->mp_Ps[bOutline] );
    setShaderMatrix(shadergroup);
#else
    setShader(mp_shaderManager.getShaderByIndex(3));
    mp_currentShader->m_selectedTech  = 4+bOutline*1;
#endif
}

void GFXAdapter_Directx9::copyCurrentColorBuffer(u32 _rt)
{
    if (!m_refractUse2x2)
    {
        /// create target rendering if needed.
        if (!m_pFullRTtex[_rt])
        {
            createRenderTarget(&m_pFullRTtex[_rt], &m_pFullRTsurf[_rt], getScreenWidth(), getScreenHeight(), D3DFMT_A8R8G8B8);
        }
    }
    else
    {
        if (!m_pSwapTextureDown2x2[0])
        {
            u32 targetsizeX = (u32)(getScreenWidth() * 0.5f);
            u32 targetsizeY = (u32)(getScreenHeight() * 0.5f);
            createRenderTarget(&m_pSwapTextureDown2x2[0], &m_pSwapTextureSurfDown2x2[0], targetsizeX, targetsizeY, D3DFMT_A8R8G8B8, btrue);
        }
    }

#ifdef ITF_WINDOWS
    depthMask(0);
    depthTest(0);
 
    HRESULT hr;
    if (!m_refractUse2x2)
    {
        RECT rect;
        rect.left=0;
        rect.right=getScreenWidth();
        rect.top=0;
        rect.bottom=getScreenHeight();

        hr = m_pd3dDevice->StretchRect(m_currentRenderingSurface,
            &rect, m_pFullRTsurf[_rt], &rect, D3DTEXF_POINT);    
    }
    else
    {
        RECT rect;
        rect.left=0;
        rect.right=(u32)(getScreenWidth() * 0.5f);
        rect.top=0;
        rect.bottom=(u32)(getScreenHeight() * 0.5f);

        hr = m_pd3dDevice->StretchRect(m_currentRenderingSurface,
            &rect, m_pSwapTextureSurfDown2x2[0], &rect, D3DTEXF_POINT);    
    }

#elif defined (ITF_X360)
    resolve( m_pFullRTtex[_rt] );
#endif
}

void GFXAdapter_Directx9::copyTexture(int *TexHan)
{
    
}

void GFXAdapter_Directx9::resolve(LPDIRECT3DTEXTURE9 _ptex)
{
    if (!_ptex) return;

#ifdef ITF_X360
    HRESULT hr;
    hr = m_pd3dDevice->Resolve( D3DRESOLVE_RENDERTARGET0 , NULL, _ptex,
         NULL, 0, 0, NULL, 1.0f, 0L, NULL );
    ITF_ASSERT(hr == S_OK);
#endif
}

///----------------------------------------------------------------------------//

void GFXAdapter_Directx9::validateTexture(int *TexHan,int Mode,int ScreenSize,int ScreenSize_V)
{

}

///----------------------------------------------------------------------------//

void GFXAdapter_Directx9::AFTERFX_SwapTargetDown4x4()
{
    m_pCurrentSwapSourceTexture = m_pSwapTextureDown4x4[m_curswap];

    resolve( m_pCurrentSwapSourceTexture );

    /// next Target.
    m_curswap += 1;
    m_curswap %= 2;
    m_pCurrentSwapRenderSurf = m_pSwapTextureSurfDown4x4[m_curswap];
}

void GFXAdapter_Directx9::AFTERFX_SwapTargetDown2x2()
{
    m_pCurrentSwapSourceTexture = m_pSwapTextureDown2x2[m_curswap];

    resolve( m_pCurrentSwapSourceTexture );

    /// next Target.
    m_curswap += 1;
    m_curswap %= 2;
    m_pCurrentSwapRenderSurf = m_pSwapTextureSurfDown2x2[m_curswap];
}

void GFXAdapter_Directx9::AFTERFX_SwapFullScreenTarget()
{
    if (m_pCurrentSwapRenderSurf == m_pFullRTsurf[0])
    {
        m_pCurrentSwapRenderSurf = m_pFullRTsurf[1];
        m_pCurrentSwapSourceTexture = m_pFullRTtex[0];
    }
    else if (m_pCurrentSwapRenderSurf == m_pFullRTsurf[1])
    {
        m_pCurrentSwapRenderSurf = m_pFullRTsurf[0];
        m_pCurrentSwapSourceTexture = m_pFullRTtex[1];
    }
}

///----------------------------------------------------------------------------//

void GFXAdapter_Directx9::AFTERFX_PrepareSwapRTDown2x2()
{
    if (!m_pSwapTextureDown2x2[0])
        createFxTargetDown2x2();
    m_pd3dDevice->SetRenderTarget( 0, m_pSwapTextureSurfDown2x2[1]);
    clear( GFX_CLEAR_COLOR , 0, 0, 0, 0); 
    m_pd3dDevice->SetRenderTarget( 0, m_pSwapTextureSurfDown2x2[0]);
    clear( GFX_CLEAR_COLOR , 0, 0, 0, 0); 
    m_curswap = 0;
    m_pCurrentSwapRenderSurf = m_pSwapTextureSurfDown2x2[0];
}

void GFXAdapter_Directx9::AFTERFX_PrepareSwapRTDown4x4()
{
    if (!m_pSwapTextureDown4x4[0])
        createFxTargetDown4x4();
    m_pd3dDevice->SetRenderTarget( 0, m_pSwapTextureSurfDown4x4[1]);
    clear( GFX_CLEAR_COLOR , 0, 0, 0, 0); 
    m_pd3dDevice->SetRenderTarget( 0, m_pSwapTextureSurfDown4x4[0]);
    clear( GFX_CLEAR_COLOR , 0, 0, 0, 0); 
    m_curswap = 0;
    m_pCurrentSwapRenderSurf = m_pSwapTextureSurfDown4x4[0];
}

///----------------------------------------------------------------------------//

void GFXAdapter_Directx9::AFTERFX_BigBlur(f32 _factor, u32 _MaxPass, bbool _clearRT, GFX_QUALITY _quality)
{
    //resolve( m_pCurrentSwapSourceTexture );

    f32 W = (f32)getScreenWidth();
    f32 H = (f32)getScreenHeight();
    f32 ConvoleRevolution = 0;
   
    f32 displace = 1.f*(_factor/1000.f);
    f32 ratiopixel = 1.f/W;

    ConvoleRevolution = 0.75f;
    
#ifdef SHADERTEST
    ITF_shader* shadergroup = 0;
	shadergroup = mp_shaderManager.getShaderByIndex(10);
    setShader( shadergroup);
    m_pd3dDevice->SetVertexShader( (IDirect3DVertexShader9*)shadergroup->mp_Vs[0] );
    m_pd3dDevice->SetPixelShader( (IDirect3DPixelShader9*)shadergroup->mp_Ps[0] );
#else
    mp_currentShader->m_selectedTech = 0;
#endif

    /// save old Current RT.
    LPDIRECT3DSURFACE9 savecur = m_pCurrentSwapRenderSurf;
    
    if ( _clearRT )
    {
        if (_quality == GFX_QUALITY_MEDIUM)
            AFTERFX_PrepareSwapRTDown2x2();
        else if (_quality == GFX_QUALITY_LOW)
            AFTERFX_PrepareSwapRTDown4x4();
    }

    setAlphaBlend(GFX_BLEND_COPY);
    
    u32 passo = 0;
    f32 centrof = 0.5f;
    if (_quality == GFX_QUALITY_MEDIUM) centrof = 0.5f * 2;
    else if (_quality == GFX_QUALITY_LOW) centrof = 0.5f  * 4;

    while ( displace > ratiopixel)
    {
        m_pd3dDevice->SetRenderTarget( 0, m_pCurrentSwapRenderSurf );

        SetTextureBind(0, (void*)m_pCurrentSwapSourceTexture);

        /// shader const.
#ifdef SHADERTEST
        GFX_Vector4 vconst1(displace, displace, displace, displace);
	    m_pd3dDevice->SetVertexShaderConstantF( 8, (f32*)&vconst1, 1 );
#else        
        mp_currentShader->setFloat(0, displace);
#endif

        drawScreenQuadC(0.f, 0.f, W, H, 0.f, COLOR_WHITE, centrof);

        displace *= 0.5f;
        passo++;

        if (_quality == GFX_QUALITY_MEDIUM)
            AFTERFX_SwapTargetDown2x2();
        else if (_quality == GFX_QUALITY_LOW)
            AFTERFX_SwapTargetDown4x4();

        if (_MaxPass && passo > _MaxPass) 
            break;
    }

    /// Go back to full RT by restoring old current RT.
    m_pCurrentSwapRenderSurf = savecur;
}

void GFXAdapter_Directx9::AFTERFX_Blur(f32 _factor, u32 _MaxPass, bbool _clearRT, GFX_QUALITY _quality)
{
    AFTERFX_BigBlur(_factor, _MaxPass, _clearRT, _quality);
}

///----------------------------------------------------------------------------//

void GFXAdapter_Directx9::AFTERFX_Remanence(f32 _factor, f32 _glowfactor, f32 _addalpha)
{
        /// Save Scene before apply mask.
        depthTest(0);
        depthMask(0);
        copyCurrentColorBuffer(0);
        m_pd3dDevice->SetRenderTarget( 0, m_pCurrentSwapRenderSurf);

        /// Write Mask to depth.
        setShader(mp_defaultShader);
        static f32 zz = -0.98f;
        depthTest(1);
        depthMask(0);//0
        depthFunc(GFX_CMP_GREATEREQUAL);
        setAlphaBlend(GFX_BLEND_COPY);
        //m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
        //m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR );
        //m_pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
        drawScreenQuad(0,0,(f32)getScreenWidth(), (f32)getScreenHeight(), zz, COLOR_BLACK, btrue );
        
        renderContext_SetDefault();

        setShader(mp_shaderManager.getShaderByIndex(1));

        AFTERFX_SwapTargetDown2x2();

        /// do blur + add scene
        //AFTERFX_Glow(_factor,_glowfactor);
        AFTERFX_BigBlur(_factor,0, btrue, GFX_QUALITY_MEDIUM);

        /// Mul factor. <-TODO->
        mp_currentShader->setFloat(1, _glowfactor);
        /// Mul factor.
        mp_currentShader->setFloat(2, _addalpha);

#if 0
        /// set scene texture to shader to add with bigblur result.
#ifdef ITF_X360//yo
        ((ITF_EFFECTP)mp_currentShader->mp_effect)->SetSampler((ITF_HANDLE)mp_currentShader->m_TextureHandle[1], m_pSceneTexture);
#else
        ((ITF_EFFECTP)mp_currentShader->mp_effect)->SetTexture((ITF_HANDLE)mp_currentShader->m_TextureHandle[1], m_pSceneTexture);
#endif
#endif

#if 0
        
        setAlphaBlend(GFX_BLEND_ADDALPHA);
        //setAlphaBlend(GFX_BLEND_COPY);
        mp_currentShader->m_selectedTech = 1;
        //m_pd3dDevice->SetRenderTarget( 0, m_pCurrentRenderSurf );
        m_pd3dDevice->SetRenderTarget( 0, m_pSceneSurf);
        SetTextureBind(0, (u32)m_pCurrentSourceTexture);
        drawScreenQuad(0.f, 0.f, (f32)getScreenWidth(), (f32)getScreenHeight(), 0.f, COLOR_WHITE);

        m_curswap = 0;
        m_pCurrentSourceTexture = m_pSceneTexture;
        m_pCurrentRenderSurf = m_pFullScreenSwapTextureSurf[m_curswap];
#else
        
        setAlphaBlend(GFX_BLEND_ALPHA);
        mp_currentShader->m_selectedTech = 1;
        m_pd3dDevice->SetRenderTarget( 0, m_pCurrentSwapRenderSurf );
        SetTextureBind(0, (void*)m_pCurrentSwapSourceTexture);
        drawScreenQuad(0.f, 0.f, (f32)getScreenWidth(), (f32)getScreenHeight(), 0.f, COLOR_WHITE);

        AFTERFX_SwapTargetDown2x2();
#endif
}

void GFXAdapter_Directx9::AFTERFX_RayCenter( f32 _v1, f32 _v2, f32 _v3, f32 _v4, f32 _v5, f32 _v6, f32 _v7)
{
    /// Save Scene before apply mask.
    //depthTest(0);
    //depthMask(0);
    GFX_QUALITY _quality = GFX_QUALITY_LOW;

    LPDIRECT3DTEXTURE9 save = m_pCurrentSwapSourceTexture;
    /// save old Current RT.
    LPDIRECT3DSURFACE9 savecur = m_pCurrentSwapRenderSurf;

#if 0
    copyCurrentColorBuffer(0);
    //m_pd3dDevice->SetRenderTarget( 0, m_pSceneSurf);

    /// Write Mask to depth.
    setShader(mp_defaultShader);
    f32 static zz = -_v1;//-0.79f
    depthTest(1);
    depthMask(0);//0
    depthFunc(GFX_CMP_GREATEREQUAL);
    setAlphaBlend(GFX_BLEND_COPY);
    drawScreenQuad(0,0,(f32)getScreenWidth(), (f32)getScreenHeight(), zz, COLOR_BLUE, btrue );

    depthFunc(GFX_CMP_LESSEQUAL);
    depthTest(0);
    depthMask(0);

    setShader(mp_shaderManager.getShaderByIndex(1));
#endif
    //resolve( m_pSceneTexture );

    f32 W = (f32)getScreenWidth();
    f32 H = (f32)getScreenHeight();
       
        if (_quality == GFX_QUALITY_MEDIUM)
            AFTERFX_PrepareSwapRTDown2x2();
        else if (_quality == GFX_QUALITY_LOW)
            AFTERFX_PrepareSwapRTDown4x4();

    setAlphaBlend(GFX_BLEND_COPY);
    
    f32 centrof = 0.5f;
    if (_quality == GFX_QUALITY_MEDIUM) centrof = 0.5f * 2;
    else if (_quality == GFX_QUALITY_LOW) centrof = 0.5f  * 4;

    setAlphaBlend(GFX_BLEND_ALPHA);

    mp_currentShader->m_selectedTech = 8;
    f32 ratiofullscreen = (1.f / W);
    f32 value = ratiofullscreen * 4;
    //<-TODO->
    mp_currentShader->setFloat(3, _v4);
    mp_currentShader->setFloat(4, _v5);
    mp_currentShader->setFloat(5, _v6);

    //m_curswap = 0;
    //m_pCurrentSourceTexture = m_pSceneTexture;
    //m_pCurrentRenderSurf = m_pSwapTextureSurf[m_curswap];

    u32 passo = 0;
    //while (    value > raptarg)
    for (u32 i = 0; i < 8 ; i++)
    {
        //m_pd3dDevice->SetRenderTarget( 0, m_pCurrentRenderSurf );
        m_pd3dDevice->SetRenderTarget( 0, m_pCurrentSwapRenderSurf );

        SetTextureBind(0, m_pCurrentSwapSourceTexture);
    
        /// shader const. <-TODO->
        mp_currentShader->setFloat(1, value);
        mp_currentShader->setInt(0, passo + 1 );

        //drawScreenQuad(0.f, 0.f, W, H, 0.f, COLOR_WHITE);
        drawScreenQuadC(0.f, 0.f, W, H, 0.f, COLOR_WHITE, centrof);

        value *= 0.5f;
        passo++;

        if (_quality == GFX_QUALITY_MEDIUM)
            AFTERFX_SwapTargetDown2x2();
        else if (_quality == GFX_QUALITY_LOW)
            AFTERFX_SwapTargetDown4x4();
    }

    /// Go back to full RT by restoring old current RT.
    m_pCurrentSwapRenderSurf = savecur;


// -------------------

    /// Mul factor.<-TODO->
    mp_currentShader->setFloat(1, _v2);
    /// Mul factor.
    mp_currentShader->setFloat(2, _v3);

    /// set scene texture to shader to add with bigblur result.
#ifdef ITF_X360//yo
    ((ITF_EFFECTP)mp_currentShader->mp_effect)->SetSampler((ITF_HANDLE)mp_currentShader->m_TextureHandle[1], save);
#else
    ((ITF_EFFECTP)mp_currentShader->mp_effect)->SetTexture((ITF_HANDLE)mp_currentShader->m_TextureHandle[1], save);
#endif

    //m_pd3dDevice->SetRenderTarget( 0, m_pSceneSurf);

    setAlphaBlend(GFX_BLEND_COPY);
    mp_currentShader->m_selectedTech = 1;
    m_pd3dDevice->SetRenderTarget( 0, m_pCurrentSwapRenderSurf );
    SetTextureBind(0, m_pCurrentSwapSourceTexture);
    drawScreenQuad(0.f, 0.f, W, H, 0.f, COLOR_WHITE);
 
    AFTERFX_SwapFullScreenTarget();

    /*setAlphaBlend(GFX_BLEND_COPY);
    mp_currentShader->m_selectedTech = 1;
    SetTextureBind(0, (u32)m_pCurrentSourceTexture);
    drawScreenQuad(0.f, 0.f, (f32)getScreenWidth(), (f32)getScreenHeight(), 0.f, COLOR_WHITE);*/
}

///----------------------------------------------------------------------------//

void GFXAdapter_Directx9::AFTERFX_zoomCenter(f32 _factor, Vec3d _dir )
{
#if 0
    f32 scrW = (f32)getScreenWidth();
    f32 scrH = (f32)getScreenHeight();

    f32 X,Y,fCurrentFocale ;
	f32 ConvoleRevolution;
	Vec3d MotionSmoothPoint, ZSDir; 
	//IDirect3DSurface8	*ppSurfaceLevel;
	//HRESULT	hr;
	//UINT uiVStride;

	// render the rendered target
	//i32	i;
	f32 resizeStep;
	ConvoleRevolution =	0.666666f;

	ZSDir =	Vec3d(0.f, 0.f, -1.f);
	_factor = 10.f;//gAE_Params.ZoomSmoothFactor;

    ZSDir.normalize();

    fCurrentFocale = 1.0f / tanf(/*_Camera.getFocale() / */ 2.f);		
    //MATH_TransformVector(&MotionSmoothPoint, &GDI_gpst_CurDD->st_Camera.st_InverseMatrix , &ZSDir);
	
    MotionSmoothPoint.m_x =	((fCurrentFocale) *	MotionSmoothPoint.m_x) / MotionSmoothPoint.m_z;
	MotionSmoothPoint.m_y =	((fCurrentFocale) *	MotionSmoothPoint.m_y) / MotionSmoothPoint.m_z;
	X =	(MotionSmoothPoint.m_x);
	Y =	(MotionSmoothPoint.m_y);
	_factor *= MotionSmoothPoint.m_z	* MotionSmoothPoint.m_z;

	X += 1.f;
	Y += 1.f;
	X *= scrW * 0.5f;
	Y *= scrH * 0.5f;

	resizeStep = _factor;
#endif
}

///----------------------------------------------------------------------------//

void GFXAdapter_Directx9::AFTERFX_prepareObjectGlow()
{
    AFTERFX_PrepareSwapRTDown4x4();
}

///----------------------------------------------------------------------------//

void GFXAdapter_Directx9::AFTERFX_objectsGlow(f32 _factor)
{
    f32 W = (f32)getScreenWidth();
    f32 H = (f32)getScreenHeight();

    setShader(mp_defaultShader);

    /// go back to rendertarget for apply mask
    m_pd3dDevice->SetRenderTarget( 0, m_pSwapTextureSurfDown4x4[0] );

    setAlphaBlend(GFX_BLEND_ALPHATOCOLOR);
    drawScreenQuad(0.f, 0.f, W, H, 0.f, COLOR_WHITE, btrue);
    
    setShader(mp_shaderManager.getShaderByIndex(1));

    /// switch rendertarget to texture and resolve it.
    AFTERFX_SwapTargetDown4x4();

    /// apply blur.
    AFTERFX_BigBlur(_factor, 3, bfalse, GFX_QUALITY_LOW);

    setShader(mp_defaultShader);

    /// go back to principal target and apply result on it.
    setAlphaBlend(GFX_BLEND_ADD);
    mp_currentShader->m_selectedTech = 1;
    m_pd3dDevice->SetRenderTarget( 0, m_currentRenderingSurface );
    SetTextureBind(0, m_pCurrentSwapSourceTexture);
    
    drawScreenQuad(0.f, 0.f, W, H, 0.f, COLOR_WHITE);
}

///----------------------------------------------------------------------------//

void GFXAdapter_Directx9::AFTERFX_Glow(f32 _factor, f32 _glowfactor, f32 _addalpha, GFX_BLENDMODE _finalBlendMode)
{
    f32 W = (f32)getScreenWidth();
    f32 H = (f32)getScreenHeight();

    LPDIRECT3DTEXTURE9 save = m_pCurrentSwapSourceTexture;

    u32 maxPass = 0;
#ifdef ITF_CONSOLE
    maxPass = 3; // 3 pass is enough for 1080p
#endif

    AFTERFX_BigBlur(_factor, maxPass, btrue, GFX_QUALITY_MEDIUM);

#ifdef SHADERTEST

    ITF_shader* shadergroup = 0;
	shadergroup = mp_shaderManager.getShaderByIndex(10);
    setShader( shadergroup);
    m_pd3dDevice->SetVertexShader( (IDirect3DVertexShader9*)shadergroup->mp_Vs[1] );
    m_pd3dDevice->SetPixelShader( (IDirect3DPixelShader9*)shadergroup->mp_Ps[1] );

    /// Mul factor.r.
    GFX_Vector4 vconst1(_glowfactor, _addalpha, 0.f, 0.f);
	m_pd3dDevice->SetPixelShaderConstantF( 16, (f32*)&vconst1, 1 );

	m_pd3dDevice->SetTexture( 1, save );

#else    
    mp_currentShader->setFloat(1, _glowfactor);
    mp_currentShader->setFloat(2, _addalpha);
    mp_currentShader->m_selectedTech = 1;

    /// set scene texture to shader to add with bigblur result.
#ifdef ITF_X360//yo
    ((ITF_EFFECTP)mp_currentShader->mp_effect)->SetSampler((ITF_HANDLE)mp_currentShader->m_TextureHandle[1], save);
#else
    ((ITF_EFFECTP)mp_currentShader->mp_effect)->SetTexture((ITF_HANDLE)mp_currentShader->m_TextureHandle[1], save);
#endif

#endif

    setAlphaBlend(GFX_BLEND_COPY);
    m_pd3dDevice->SetRenderTarget( 0, m_pCurrentSwapRenderSurf );
    SetTextureBind(0, m_pCurrentSwapSourceTexture);
    drawScreenQuad(0.f, 0.f, W, H, 0.f, COLOR_WHITE);

    AFTERFX_SwapFullScreenTarget();
    resolve( m_pCurrentSwapSourceTexture );
}

void GFXAdapter_Directx9::AFTERFX_ColorSetting(f32 _sat, f32 _contrast, f32 _contrastscale, f32 _bright)
{
#ifdef TODOSHADER
    f32 W = (f32)getScreenWidth();
    f32 H = (f32)getScreenHeight();

    mp_currentShader->m_selectedTech = 3;

    m_pd3dDevice->SetRenderTarget( 0, m_pCurrentSwapRenderSurf );
    SetTextureBind(0, (u32)m_pCurrentSwapSourceTexture);
    setAlphaBlend(GFX_BLEND_COPY);
    m_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
    m_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);

    //<-TODO->
    mp_currentShader->setFloat(2, _sat);
    mp_currentShader->setFloat(3, _contrast);
    mp_currentShader->setFloat(4, _contrastscale);
    mp_currentShader->setFloat(5, _bright);

    setShader(mp_defaultShader); ??
    drawScreenQuad(0.f, 0.f, W, H, 0.f, COLOR_WHITE);
    
    AFTERFX_SwapFullScreenTarget();
#endif
}

///----------------------------------------------------------------------------//

void GFXAdapter_Directx9::AFTERFX_ColorRemap(Texture* _tex)
{
#ifdef TODOSHADER
    // setColor Ramp to shader custom texture.
    if (_tex)
    {
        SetTextureBind(3, (u32)_tex->m_adapterimplementationData, 0);
        m_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
        m_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
    }
    else 
        return;

    f32 W = (f32)getScreenWidth();
    f32 H = (f32)getScreenHeight();

    mp_currentShader->m_selectedTech = 9;
    m_pd3dDevice->SetRenderTarget( 0, m_pCurrentSwapRenderSurf );
    SetTextureBind(0, (u32)m_pCurrentSwapSourceTexture);
    setAlphaBlend(GFX_BLEND_COPY);
    m_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
    m_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);

    setShader(mp_defaultShader);??
    drawScreenQuad(0.f, 0.f, W, H, 0.f, COLOR_WHITE);
    
    AFTERFX_SwapFullScreenTarget();
#endif
}

///----------------------------------------------------------------------------//

void GFXAdapter_Directx9::AFTERFX_ColorLevels(f32 _inBlack,  f32 _inGamma, f32 _inWhite, f32 _outBlack, f32 _outWhite)
{
#ifdef TODOSHADER
    f32 W = (f32)getScreenWidth();
    f32 H = (f32)getScreenHeight();

    mp_currentShader->m_selectedTech = 10;
    m_pd3dDevice->SetRenderTarget( 0, m_pCurrentSwapRenderSurf );
    SetTextureBind(0, (u32)m_pCurrentSwapSourceTexture);
    setAlphaBlend(GFX_BLEND_COPY);
    m_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
    m_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);

    //<-TODO->
    mp_currentShader->setFloat(1, _inBlack);
    mp_currentShader->setFloat(2, _inGamma);
    mp_currentShader->setFloat(3, _inWhite);
    mp_currentShader->setFloat(4, _outBlack);
    mp_currentShader->setFloat(5, _outWhite);

    setShader(mp_defaultShader); ??
    drawScreenQuad(0.f, 0.f, W, H, 0.f, COLOR_WHITE);
    
    AFTERFX_SwapFullScreenTarget();
#endif
}

///----------------------------------------------------------------------------//

void GFXAdapter_Directx9::AFTERFX_addSceneAndMul(f32 _mul)
{
#ifdef TODOSHADER
    f32 W = (f32)getScreenWidth();
    f32 H = (f32)getScreenHeight();

    /// Mul factor.<-TODO->
    mp_currentShader->setFloat(1, _mul);
    mp_currentShader->setFloat(2, 1.f);

    /// set scene texture to shader to add with bigblur result.
#ifdef ITF_X360//yo
//    ((ITF_EFFECTP)mp_currentShader->mp_effect)->SetSampler((ITF_HANDLE)mp_currentShader->m_TextureHandle[1], m_pSceneTexture);
#else
 //   ((ITF_EFFECTP)mp_currentShader->mp_effect)->SetTexture((ITF_HANDLE)mp_currentShader->m_TextureHandle[1], m_pSceneTexture);
#endif

    mp_currentShader->m_selectedTech = 1;
    m_pd3dDevice->SetRenderTarget( 0, m_pCurrentSwapRenderSurf );
    SetTextureBind(0, (u32)m_pCurrentSwapSourceTexture);
    m_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
    m_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);

    setShader(mp_defaultShader);
    drawScreenQuad(0.f, 0.f, W, H, 0.f, COLOR_WHITE);
    
    AFTERFX_SwapTargetDown4x4();
#endif
}

///----------------------------------------------------------------------------//

void GFXAdapter_Directx9::AFTERFX_byTech(u32 _tech)
{
#if 0
    f32 W = (f32)getScreenWidth();
    f32 H = (f32)getScreenHeight();

    mp_currentShader->m_selectedTech = _tech;
    m_pd3dDevice->SetRenderTarget( 0, m_pCurrentSwapRenderSurf );
    SetTextureBind(0, (u32)m_pCurrentSwapSourceTexture);
    m_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
    m_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);

    drawScreenQuad(0.f, 0.f, W, H, 0.f, COLOR_WHITE);
    
    AFTERFX_SwapTargetDown4x4();
#endif
}

///----------------------------------------------------------------------------//

void GFXAdapter_Directx9::AFTERFX_byTechParams(u32 _tech, f32 _p0, f32 _p1, f32 _p2, f32 _p3)
{
#if 0
    f32 W = (f32)getScreenWidth();
    f32 H = (f32)getScreenHeight();

    depthTest(0);
    depthMask(0);

    setAlphaBlend(GFX_BLEND_COPY);
    mp_currentShader->m_selectedTech = _tech;
    m_pd3dDevice->SetRenderTarget( 0, m_pCurrentSwapRenderSurf );
    SetTextureBind(0, (u32)m_pCurrentSwapSourceTexture);
    m_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
    m_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);

    if (mp_currentShader->m_FloatHandle[1])
        mp_currentShader->setFloat(1, _p0);
    if (mp_currentShader->m_FloatHandle[2])
        mp_currentShader->setFloat(2, _p1);
    if (mp_currentShader->m_FloatHandle[3])
       mp_currentShader->setFloat(3, _p2);
    if (mp_currentShader->m_FloatHandle[4])
        mp_currentShader->setFloat(4, _p3);

    drawScreenQuad(0.f, 0.f, W, H, 0.f, COLOR_WHITE);
    
    AFTERFX_SwapTargetDown4x4();
#endif
}


///----------------------------------------------------------------------------//
/// UV.
///----------------------------------------------------------------------------//

void GFXAdapter_Directx9::setUVAnim(GFX_UVANIM& _uvanim)
{
    setMatrixTransform(GFX_TEXTURE, &_uvanim.m_UVMat);
}

///----------------------------------------------------------------------------//

void GFXAdapter_Directx9::project(const Vec3d& _in3d, Vec2d & _out2d)
{
    D3DVIEWPORT9 vp;
    m_pd3dDevice->GetViewport(&vp);
    D3DXMatrixIdentity(&mg_World);
    // no Translation.
    D3DXMATRIX view = mg_View;
/*    view._41 = 0;
    view._42 = 0;
    view._43 = 0;*/
    D3DXVECTOR3 in, out;

    in.x = _in3d.m_x;
    in.y = _in3d.m_y;
    in.z = _in3d.m_z;
    D3DXVec3Project(&out, &in, &vp, &mg_Proj, &view, &mg_World);
    _out2d.m_x = out.x;
    _out2d.m_y = out.y;    
    ITF_ASSERT(out.z>=0);
}

///----------------------------------------------------------------------------//

f32 GFXAdapter_Directx9::ReadSurfaceFloat(void* _surf, u32 _posX, u32 _posY)
{
#ifdef ITF_WINDOWS
    D3DSURFACE_DESC desc;
    HRESULT            hr;
    D3DLOCKED_RECT lockedRect;
    LPDIRECT3DSURFACE9 surface = (LPDIRECT3DSURFACE9)_surf;
 
    hr = surface->GetDesc(&desc);

    RECT rect;
    rect.left = 0;
    rect.right = desc.Width;
    rect.top = 0;
    rect.bottom = desc.Height;

    f32 returnVal = 9999.f;

#if 1
    LPDIRECT3DSURFACE9 offscreenSurface;
    
    D3DFORMAT newFormat = desc.Format;

    hr = m_pd3dDevice->CreateOffscreenPlainSurface(desc.Width, desc.Height,newFormat, D3DPOOL_SYSTEMMEM, &offscreenSurface, NULL);

    hr = m_pd3dDevice->GetRenderTargetData(  surface, offscreenSurface);

    hr = offscreenSurface->LockRect(&lockedRect, &rect, NULL );
    f32* pRead = (f32*)lockedRect.pBits;

    returnVal = pRead[(_posY * (lockedRect.Pitch/4)) + _posX];
    
    offscreenSurface->UnlockRect();
    offscreenSurface->Release();

#else
    LPDIRECT3DTEXTURE9 temp;
    hr = m_pd3dDevice->CreateTexture(desc.Width, desc.Height,
                            1, D3DUSAGE_RENDERTARGET, desc.Format,
                            D3DPOOL_SYSTEMMEM, &temp, 0);

    LPDIRECT3DSURFACE9 offscreenSurface;
    hr = temp->GetSurfaceLevel(0, &offscreenSurface);

    hr = m_pd3dDevice->GetRenderTargetData(  surface, offscreenSurface);

    temp->LockRect(0, &lockedRect, &rect, NULL);
    f32* pRead = (f32*)lockedRect.pBits;

    returnVal = pRead[(_posY * (lockedRect.Pitch/4)) + _posX];
    
    temp->UnlockRect(0);
    offscreenSurface->Release();
    temp->Release();

#endif

    return returnVal;
 #else
   return 0.0f;
#endif //ITF_WINDOWS
}

void GFXAdapter_Directx9::startImpostorRendering(renderTarget* _rt, f32 xmin, f32 ymin, f32 xmax, f32 ymax, bbool _mustClear)
{
    _rt->set();
    //_rt->clear(COLOR_ZERO);
    if (_mustClear)
    {
        m_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, _rt->getBackGroundColor(),  1.0f, 0 );
    }
    
    setOrthoView(xmin, xmax, ymin, ymax);

    GFX_RECT Viewport;
    Viewport.left = 0;
    Viewport.top = 0;
    Viewport.right = _rt->m_area[2];
    Viewport.bottom = _rt->m_area[3];
    setupViewport(&Viewport);

    /// Ready to go !!!.
}

void GFXAdapter_Directx9::stopImpostorRendering(renderTarget* _rt)
{
    _rt->unSet();
    /// restore backbuffer and copy result of afterfx texture.
    //setBackBuffer();

    /// restore 3d View.
    CAMERA->apply();
}

///----------------------------------------------------------------------------//
/// Render Context.
///----------------------------------------------------------------------------//
void GFXAdapter_Directx9::renderContext_Set()
{
    if (rC_getZWRITE())
        depthMask(1);
    if (rC_getZWRITE())
        depthTest(1);
}

void GFXAdapter_Directx9::renderContext_SetDefault()
{
    depthMask(0);
    depthTest(0);
    depthFunc(GFX_CMP_ALWAYS);
}

///----------------------------------------------------------------------------//
/// Gfx Material.
///----------------------------------------------------------------------------//
void GFXAdapter_Directx9::setGfxMaterial( const GFX_MATERIAL& _gfxMat )
{
    m_drawFlag = 0;

    switch (_gfxMat.m_matType)
    {
    default:
    case GFX_MAT_DEFAULT:
        setGfxMatDefault();
        m_currentRenderingMaterialType = GFX_MAT_DEFAULT;
        break;
    case GFX_MAT_FRIEZEANIM:
    {    
        setGfxMatDefault();

        GFX_Vector4 vec4;

#ifdef SHADERTEST
		if (m_ShaderMode)
		{
            vec4 = _gfxMat.m_matParams.getvParamsAt(0);
			m_pd3dDevice->SetVertexShaderConstantF( 14, (f32*)&vec4, 1 );//uvm
		}
		else
#endif
        if (mp_currentShader->m_VecHandle[1])
        {
            vec4 = _gfxMat.m_matParams.getvParamsAt(0);
            mp_currentShader->setVector(1, &vec4);
        }
    
        m_currentRenderingMaterialType = GFX_MAT_FRIEZEANIM;
    }
        break;
    case GFX_MAT_FRIEZEOVERLAY:
        {    
            setGfxMatDefault();

#ifdef SHADERTEST
            ITF_shader* shadergroup = mp_shaderManager.getShaderByIndex(7);
            m_pd3dDevice->SetVertexShader( (IDirect3DVertexShader9*)shadergroup->mp_Vs[13] );

		    if (isUseFog())
			    m_pd3dDevice->SetPixelShader( (IDirect3DPixelShader9*)shadergroup->mp_Ps[3] );
		    else
			    m_pd3dDevice->SetPixelShader( (IDirect3DPixelShader9*)shadergroup->mp_Ps[0] );
#endif

            m_currentRenderingMaterialType = GFX_MAT_FRIEZEOVERLAY;
            mp_currentShader->m_selectedTech = 0;
        }
        break;
    case GFX_MAT_REFRACTION:
        setGfxMatRefraction(_gfxMat);
        m_currentRenderingMaterialType = GFX_MAT_REFRACTION;
        break;

    case GFX_MAT_GLOW:
        AFTERFX_prepareObjectGlow();
        setGfxMatDefault();
        m_drawFlag |= DRWF_COPYTOTARGET;
        m_currentRenderingMaterialType = GFX_MAT_GLOW;
		break;

    case GFX_MAT_ALPHAFADE:
        setGfxMatAlphaFade(_gfxMat);
        m_currentRenderingMaterialType = GFX_MAT_ALPHAFADE;
        break;
    }

    /// set alpha blend.
    setAlphaBlend(_gfxMat.m_blendMode);

    /// set Texture if present
    SetTextureResource(0, _gfxMat.m_textureDiffuse);

    setTextureAdressingMode(0, _gfxMat.m_TexAdressingModeU, _gfxMat.m_TexAdressingModeV);

#ifdef SHADERTEST
    if (isUseFog())
    {
        //f32 f = 1.f - computeFogFactor();

        GFX_Vector4 fog;
        fog.m_x = m_colorFog.getRed();
        fog.m_y = m_colorFog.getGreen();
        fog.m_z = m_colorFog.getBlue();
        fog.m_w = m_colorFog.getAlpha(); // blend factor in alpha

        // color Fog.
        if (mp_currentShader->m_VecHandle[3])
            mp_currentShader->setVector(3, &fog);
    }
#endif
}

void GFXAdapter_Directx9::setBlendSeparateAlpha()
{
    if (m_useSeparateAlpha)
    {
        m_pd3dDevice->SetRenderState(D3DRS_DESTBLENDALPHA, D3DBLEND_ONE);
        m_pd3dDevice->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, TRUE);
    }
    else
        m_pd3dDevice->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, FALSE);
}

void GFXAdapter_Directx9::setGfxMatDefault( )
{
#ifdef SHADERTEST
    setShader(mp_defaultShader);
#else
	if (getOverDrawMode() > 1)
        setShader( mp_shaderManager.getShaderByIndex(4));
    else
    {
        if (isUseFog())
            setShader( mp_shaderManager.getShaderByIndex(5));
        else
            setShader(mp_defaultShader);
    }
#endif
}

void GFXAdapter_Directx9::setGfxMatRefraction( const GFX_MATERIAL& _gfxMat )
{
    if (!getSceneResolvedFlag())
    {
        /// Copy SceneTexture To TexBuffer.
        copyCurrentColorBuffer(1);
        setSceneResolvedFlag(btrue);
    }

#ifdef SHADERTEST
    setShader( mp_shaderManager.getShaderByIndex(7));
#else
    setShader( mp_shaderManager.getShaderByIndex(2));
    mp_currentShader->m_selectedTech = 0;
	mp_currentShader->m_type = 1;
#endif

    /// backbuffer_copy/refraction texture.
    if (!m_refractUse2x2)
        SetTextureBind(1, m_pFullRTtex[1]);
    else
        SetTextureBind(1, m_pSwapTextureDown2x2[0]);
    setTextureAdressingMode(1, GFXTADDRESS_CLAMP, GFXTADDRESS_CLAMP);

    /// try normal texture.
    if (!SetTextureResource(2, _gfxMat.m_textureNormal))
    {
        // if normal doesn't work
        // try texture Diffuse
        SetTextureResource(2, _gfxMat.m_textureDiffuse);
    }

    setTextureAdressingMode(2, GFXTADDRESS_WRAP, GFXTADDRESS_WRAP);

#ifdef SHADERTEST
    ITF_shader* shadergroup = mp_shaderManager.getShaderByIndex(7);
    m_pd3dDevice->SetVertexShader( (IDirect3DVertexShader9*)shadergroup->mp_Vs[12] );
    m_pd3dDevice->SetPixelShader( (IDirect3DPixelShader9*)shadergroup->mp_Ps[7] );

    GFX_Vector4 vconst(_gfxMat.m_matParams.getfParamsAt(0), _gfxMat.m_matParams.getfParamsAt(1), _gfxMat.m_matParams.getfParamsAt(2), _gfxMat.m_matParams.getfParamsAt(3));
	m_pd3dDevice->SetPixelShaderConstantF( 14, (f32*)&vconst, 1 );
#else
    /// decal factor
    mp_currentShader->setFloat(0, _gfxMat.m_matParams.getfParamsAt(0));
    mp_currentShader->setFloat(1, _gfxMat.m_matParams.getfParamsAt(1));
    mp_currentShader->setFloat(2, _gfxMat.m_matParams.getfParamsAt(2));
    mp_currentShader->setFloat(3, _gfxMat.m_matParams.getfParamsAt(3));
#endif
}

///----------------------------------------------------------------------------//
/// Overdraw.
///----------------------------------------------------------------------------//

void GFXAdapter_Directx9::createOverDrawRamp( u32 _greenPos, u32 _yellowPos, u32 _redPos)
{
    if (m_overDrawRamptexture)
        m_overDrawRamptexture->Release();
    
    /// internal OverDraw Texture.
    HRESULT hr;
    hr = D3DXCreateTexture(m_pd3dDevice,
        256,
        1,
        0, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &m_overDrawRamptexture);

    D3DLOCKED_RECT lockedRect;
    m_overDrawRamptexture->LockRect( 0, &lockedRect, NULL, 0 );
  	DWORD* pData=(DWORD*)(lockedRect.pBits);
    Color ramp;
    u32 marker1 = _greenPos;
    u32 marker2 = _yellowPos;
    u32 marker3 = _redPos;

    for (u32 i = 0; i < 256; i++)
    {
        if (i == 0)
            ramp = Color::black();
        else
        {
            if (i < marker1) // blue/green.
            {
                u32 dist = marker1 - 1;
                f32 lerp = ((f32)(marker1 - i)/(f32)dist);
                ramp = Color::Interpolate(Color::green(),Color::blue(), lerp);
            }
            else if (i < marker2) // green/Yellow.
            {
                u32 dist = marker2 - marker1;
                f32 lerp = ((f32)(marker2 - i)/(f32)dist);
                ramp = Color::Interpolate(Color::yellow(), Color::green(), lerp);
            }
            else if (i < marker3) // Yellow/Red.
            {
                u32 dist = marker3 - marker2;
                f32 lerp = ((f32)(marker3 - i)/(f32)dist);
                ramp = Color::Interpolate(Color::red(), Color::yellow(), lerp);
            }
            else
                ramp = Color::red();
        }
        
        ramp.m_a = 255.f;
        pData[i] = ramp.getAsU32();
    }
    m_overDrawRamptexture->UnlockRect(0);
}

void GFXAdapter_Directx9::startOverDrawRecord()
{
  m_pd3dDevice->Clear(0, NULL, D3DCLEAR_STENCIL, 0x00000000, 1.0, 0);

  m_pd3dDevice->SetRenderState(D3DRS_STENCILENABLE,    1);
  m_pd3dDevice->SetRenderState(D3DRS_STENCILFUNC,      D3DCMP_ALWAYS);
  m_pd3dDevice->SetRenderState(D3DRS_STENCILREF,       0);
  m_pd3dDevice->SetRenderState(D3DRS_STENCILMASK,      0x00000000);
  m_pd3dDevice->SetRenderState(D3DRS_STENCILWRITEMASK, 0xffffffff);

  m_pd3dDevice->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_INCRSAT);
  m_pd3dDevice->SetRenderState(D3DRS_STENCILFAIL,  D3DSTENCILOP_KEEP);
  m_pd3dDevice->SetRenderState(D3DRS_STENCILPASS,  D3DSTENCILOP_INCRSAT);

  //m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE );

}

void GFXAdapter_Directx9::showOverDrawRecord()
{
    if (getOverDrawMode() == 1)
        OverDraw_StencilBuffer();
    else if (getOverDrawMode() == 3)
        OverDraw_ColorRemap();
}

void GFXAdapter_Directx9::OverDraw_StencilBuffer()
{
    m_pd3dDevice->SetRenderState(D3DRS_ZENABLE,          0);

    m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, 0);

    // Set up the stencil states
    m_pd3dDevice->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
    m_pd3dDevice->SetRenderState(D3DRS_STENCILFAIL,  D3DSTENCILOP_KEEP);
    m_pd3dDevice->SetRenderState(D3DRS_STENCILPASS,  D3DSTENCILOP_KEEP);
    m_pd3dDevice->SetRenderState(D3DRS_STENCILFUNC,  D3DCMP_NOTEQUAL);
    m_pd3dDevice->SetRenderState(D3DRS_STENCILREF,   0);

    m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, 0x00000000, 1.0, 0);

    VertexPC quad[4];
    f32 sx = (f32)getScreenWidth();
    f32 sy = (f32)getScreenHeight();

    quad[0].setData( Vec3d( 0.f, 0.f, 0.f ), COLOR_WHITE);
    quad[1].setData( Vec3d( 0.f, sy, 0.f ), COLOR_WHITE);
    quad[2].setData( Vec3d( sx, 0.f, 0.f ), COLOR_WHITE);
    quad[3].setData( Vec3d( sx, sy, 0.f ), COLOR_WHITE);

    setVertexFormat(VertexFormat_PC);

    /// OVERDRAW 1.
    m_pd3dDevice->SetRenderState(D3DRS_STENCILMASK, 0x01);
    quad[0].m_color = quad[1].m_color = quad[2].m_color = quad[3].m_color = COLOR_BLUE;
    DrawPrimitive(GFX_TRIANGLE_STRIP, (void*)quad, 4);

    /// OVERDRAW 2.
    m_pd3dDevice->SetRenderState(D3DRS_STENCILMASK, 0x02);
    quad[0].m_color = quad[1].m_color = quad[2].m_color = quad[3].m_color = COLOR_GREEN;
    DrawPrimitive(GFX_TRIANGLE_STRIP, (void*)quad, 4);

    /// OVERDRAW 3.
    m_pd3dDevice->SetRenderState(D3DRS_STENCILMASK, 0x04);
    quad[0].m_color = quad[1].m_color = quad[2].m_color = quad[3].m_color = COLOR_RED | COLOR_GREEN;
    DrawPrimitive(GFX_TRIANGLE_STRIP, (void*)quad, 4);

    /// OVERDRAW 4.
    m_pd3dDevice->SetRenderState(D3DRS_STENCILMASK, 0x08);
    quad[0].m_color = quad[1].m_color = quad[2].m_color = quad[3].m_color = COLOR_RED | (0x88<<COLOR_SHIFT_GREEN);
    DrawPrimitive(GFX_TRIANGLE_STRIP, (void*)quad, 4);

    /// OVERDRAW +4.
    m_pd3dDevice->SetRenderState(D3DRS_STENCILMASK, 0xF0);
    quad[0].m_color = quad[1].m_color = quad[2].m_color = quad[3].m_color = COLOR_RED;
    DrawPrimitive(GFX_TRIANGLE_STRIP, (void*)quad, 4);

    // Restore states
    m_pd3dDevice->SetRenderState(D3DRS_STENCILENABLE,    bfalse);
}

void GFXAdapter_Directx9::OverDraw_ColorRemap()
{
    D3DXMATRIX World;
    D3DXMatrixIdentity(&World);
    setObjectMatrix(World);

    setShader(mp_shaderManager.getShaderByIndex(1));
    SetTextureBind(3, m_overDrawRamptexture, 0);
    m_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
    m_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);

    f32 W = (f32)getScreenWidth();
    f32 H = (f32)getScreenHeight();

    if (m_currentRenderingSurface == m_pBackBufferSurf)
          copyCurrentColorBuffer(0);

    SetTextureBind(0, m_pFullRTtex[0]);
    setAlphaBlend(GFX_BLEND_COPY);
    m_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
    m_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);

    //drawScreenQuad(0.f, 0.f, W, H, 0.f, COLOR_WHITE);
    VertexPCT quad[4];
    f32 centroid = 0.5f;
    quad[0].setData( Vec3d( 0.f - centroid, 0.f + centroid, 0.f ), Vec2d( 0.f, 1.0f ), COLOR_WHITE);
    quad[1].setData( Vec3d( 0.f - centroid, 0.f + H + centroid, 0.f ), Vec2d( 0.f, 0.f ), COLOR_WHITE);
    quad[2].setData( Vec3d( 0.f + W - centroid, 0.f + centroid, 0.f ), Vec2d( 1.0f, 1.0f ), COLOR_WHITE);
    quad[3].setData( Vec3d( 0.f + W - centroid, 0.f + H + centroid, 0.f ), Vec2d( 1.0f, 0.0f ), COLOR_WHITE);
    
    setVertexFormat(VertexFormat_PCT);
    mp_currentShader->m_selectedTech = 11;
    DrawPrimitive(GFX_TRIANGLE_STRIP, (void*)quad, 4);

    setShader(mp_defaultShader);
}

///----------------------------------------------------------------------------//
/// Light.
///----------------------------------------------------------------------------//

void GFXAdapter_Directx9::setGlobalColor(const Color& _color)
{
    setGFXGlobalColor(_color);

    GFX_Vector4 light;
    light.m_x = getGFXGlobalColor().getRed();
    light.m_y = getGFXGlobalColor().getGreen();
    light.m_z = getGFXGlobalColor().getBlue();
    light.m_w = getGFXGlobalColor().getAlpha();

#ifdef SHADERTEST
	if (m_ShaderMode)
		m_pd3dDevice->SetVertexShaderConstantF( 13, (f32*)&light, 1 );//colorglob
    else
#endif
    if (mp_currentShader->m_VecHandle[0])
        mp_currentShader->setVector(0, &light);
}
///----------------------------------------------------------------------------//
/// Fog.
///----------------------------------------------------------------------------//

void GFXAdapter_Directx9::setFogColor(const Color& _color)
{
    m_colorFog = _color;
    if (_color.getAlpha())
    {
        setUseFog(btrue);
        setShader( mp_shaderManager.getShaderByIndex(5));
    }
    else
    {
        setUseFog(bfalse);
        if (getOverDrawMode() > 1)
            setShader( mp_shaderManager.getShaderByIndex(4));
        else
            setShader(mp_defaultShader);
    }
}

const GMatrix44* GFXAdapter_Directx9::getViewMatrix()
{
    return (const GMatrix44*)&mg_View;
}

#ifdef ITF_SUPPORT_VIDEOCAPTURE
void VideoCaptureBuffer_Directx9::release()
{

    if (m_captureOffscreenSurface)
    {
        LPDIRECT3DSURFACE9 surface = (LPDIRECT3DSURFACE9) m_captureOffscreenSurface;
        u32 count = surface->Release();
        ITF_ASSERT(count == 0);
        m_captureOffscreenSurface = NULL;
    }
    
    if (m_captureRenderTarget)
    {
        LPDIRECT3DTEXTURE9 texture = (LPDIRECT3DTEXTURE9) m_captureRenderTarget;
        u32 count = texture->Release();
        ITF_ASSERT(count == 0);
        m_captureRenderTarget = NULL;

        m_captureStretchSurface = NULL;
    }
}


void VideoCaptureBuffer_Directx9::lock()
{
    ITF_ASSERT(m_dataPtr == NULL);
    D3DLOCKED_RECT lockedrect;
    LPDIRECT3DSURFACE9 surface = (LPDIRECT3DSURFACE9) m_captureOffscreenSurface;
    HRESULT hr = surface->LockRect(&lockedrect,NULL,D3DLOCK_READONLY);
    ITF_ASSERT(hr==S_OK);
    
    m_dataPtr = lockedrect.pBits;
    m_pitch = lockedrect.Pitch;
}

void VideoCaptureBuffer_Directx9::unlock()
{
    ITF_ASSERT(m_dataPtr != NULL);

    LPDIRECT3DSURFACE9 surface = (LPDIRECT3DSURFACE9) m_captureOffscreenSurface;
    surface->UnlockRect();

    
    m_dataPtr = NULL;
}

#endif //ITF_SUPPORT_VIDEOCAPTURE

void* GFXAdapter_Directx9::compilePixelShader(const char* _bufferText, u32 len, const char* _functionName)
{
	ID3DXBuffer* buffer = 0;
    IDirect3DPixelShader9  * pShaderResult = 0;
    HRESULT   hr;

	hr = D3DXCompileShader( (LPCSTR)_bufferText, len, 0, 0, (LPCSTR)_functionName,  "ps_2_0", 0, &buffer, NULL, NULL );

	if ( SUCCEEDED( hr ) )
    {
        hr = m_pd3dDevice->CreatePixelShader( (DWORD*) buffer->GetBufferPointer(), &pShaderResult );
        buffer->Release();

        if ( FAILED( hr ) )
        {
            return( 0 );
        }
    }

    return pShaderResult;
}

void* GFXAdapter_Directx9::compileVertexShader(const char* _bufferText, u32 len, const char* _functionName)
{
	ID3DXBuffer* buffer = 0;
    IDirect3DVertexShader9  * pShaderResult = 0;
    HRESULT hr;
	
	hr = D3DXCompileShader( (LPCSTR)_bufferText, len, 0, 0, (LPCSTR)_functionName,  "vs_2_0", 0, &buffer, NULL, NULL );
	
    if ( SUCCEEDED( hr ) )
    {
        hr = m_pd3dDevice->CreateVertexShader( (DWORD*) buffer->GetBufferPointer(), &pShaderResult );
        buffer->Release();

        if ( FAILED( hr ) )
        {
            return( 0 );
        }
    }

    return pShaderResult;
}

void GFXAdapter_Directx9::messageD3DError(HRESULT _hr)
{
    switch(_hr)
    {
        case D3DERR_INVALIDCALL:
            ITF_ASSERT_CRASH(0, "messageD3DError D3DERR_INVALIDCALL");
        break;
        case D3DERR_INVALIDDEVICE:
            ITF_ASSERT_CRASH(0, "messageD3DError D3DERR_INVALIDDEVICE");
        break;
        case D3DERR_OUTOFVIDEOMEMORY:
            ITF_ASSERT_CRASH(0, "messageD3DError D3DERR_OUTOFVIDEOMEMORY");
        break;
        case D3DERR_NOTAVAILABLE:
            ITF_ASSERT_CRASH(0, "messageD3DError D3DERR_NOTAVAILABLE");
        break;
        case D3DERR_DEVICENOTRESET:
            ITF_ASSERT_CRASH(0, "messageD3DError D3DERR_DEVICENOTRESET");
        break;
        case D3DERR_DEVICELOST:
            //this error could happen,the engine needs to handle it
        break;
        case D3DERR_MOREDATA:
            ITF_ASSERT_CRASH(0, "messageD3DError D3DERR_MOREDATA");
        break;
        case D3DERR_NOTFOUND:
            ITF_ASSERT_CRASH(0, "messageD3DError D3DERR_NOTFOUND");
        break;
        default:
            ITF_ASSERT_CRASH(0, "messageD3DError unknow error");
            break;
    }
}

#ifdef ITF_X360
void GFXAdapter_Directx9::setGammaRamp(Vec3d gamma,f32 brigthnessValue,f32 contrastValue)
{
    D3DGAMMARAMP scaledRamp;
    buildGammaRamp( scaledRamp, gamma, brigthnessValue, contrastValue );

    m_pd3dDevice->SetGammaRamp(0, 0, &scaledRamp);

}


void GFXAdapter_Directx9::buildGammaRamp( D3DGAMMARAMP& scaledRamp, Vec3d gamma, f32 brigthnessValue, f32 contrastValue )
{
    gamma.m_x = 1.0f / gamma.m_x;
    gamma.m_y = 1.0f / gamma.m_y;
    gamma.m_z = 1.0f / gamma.m_z;

    brigthnessValue = Clamp(brigthnessValue-1.0f,-1.0f,1.0f);
    contrastValue = Clamp(contrastValue-1.0f,-1.0f,1.0f);

    brigthnessValue = brigthnessValue * ( 1.0f / 6.0f );
    contrastValue = 0.5f * contrastValue + 1.0f;

    for(u32 rampIndex = 0; rampIndex < 256; ++rampIndex)
    {
        f32 redFct   = Clamp(powf(static_cast<f32>(rampIndex) / 255.f, gamma.getX()) + brigthnessValue,0.0f,1.0f);
        f32 greenFct = Clamp(powf(static_cast<f32>(rampIndex) / 255.f, gamma.getY()) + brigthnessValue,0.0f,1.0f);
        f32 blueFct  = Clamp(powf(static_cast<f32>(rampIndex) / 255.f, gamma.getZ()) + brigthnessValue,0.0f,1.0f);

        redFct   = Clamp(redFct  *contrastValue,0.0f,1.0f);
        greenFct = Clamp(greenFct*contrastValue,0.0f,1.0f);
        blueFct  = Clamp(redFct  *contrastValue,0.0f,1.0f);

        scaledRamp.red  [rampIndex] = static_cast<u16>( redFct   * 65535.f );
        scaledRamp.green[rampIndex] = static_cast<u16>( greenFct * 65535.f );
        scaledRamp.blue [rampIndex] = static_cast<u16>( blueFct  * 65535.f );
    }
}
#endif //ITF_X360


} // namespace ITF
