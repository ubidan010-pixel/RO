#include "precompiled_engine.h"

#include <algorithm>
#include <math.h>

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

#ifndef _ITF_CONFIG_H_
#include "core/Config.h"
#endif //_ITF_CONFIG_H_

#ifndef _ITF_ANIMATION_H_
#include "engine/animation/Animation.h"
#endif //_ITF_ANIMATION_H_

#ifdef DEACTRAIL
#ifndef _ITF_TRAILCOMPONENT_H_
#include "gameplay/components/misc/TrailComponent.h"
#endif //_ITF_TRAILCOMPONENT_H_
#endif

#ifndef _ITF_TRAIL_H_
#include "engine/display/Trail.h"
#endif //_ITF_TRAIL_H_

#ifndef _ITF_PROCEDURALPATCH_H_
#include "engine/display/ProceduralPatch.h"
#endif //_ITF_PROCEDURALPATCH_H_

#ifndef _ITF_BEZIERPATCH_H_
#include "engine/display/BezierPatch.h"
#endif //_ITF_BEZIERPATCH_H_

#ifndef _ITF_MATHTOOLS_H_
#include "core/math/MathTools.h"
#endif //_ITF_MATHTOOLS_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_


#ifndef _ITF_FONT_H_
#include "engine/display/font.h"
#endif //_ITF_FONT_H_

#ifndef _ITF_FONTRESOURCE_H_
#include "engine/display/FontResource.h"
#endif //_ITF_FONTRESOURCE_H_

#ifndef _ITF_GFXADAPTER_RENDERTARGET_H_
#include "engine/AdaptersInterfaces/GFXAdapter_RenderTarget.h"
#endif //_ITF_GFXADAPTER_RENDERTARGET_H_

#ifndef _ITF_SPLINE_H_
#include "core/math/spline.h"
#endif //_ITF_SPLINE_H_

#ifndef _ITF_VIDEOCAPTURE_H_
#include "engine/display/videoCapture.h"
#endif //_ITF_VIDEOCAPTURE_H_

#ifndef _ITF_FRIEZE_DRAW_H_
#include "engine/display/Frieze/FriezeDraw.h"
#endif //_ITF_FRIEZE_DRAW_H_

#ifndef _ITF_ALIASMANAGER_H_
#include "engine/aliasmanager/aliasmanager.h"
#endif //_ITF_ALIASMANAGER_H_

#ifndef _ITF_PROFILERMANAGER_H_
#include "engine/profiler/ProfilerManager.h"
#endif //_ITF_PROFILERMANAGER_H_

/*
#ifndef _ITF_GROUPMESH_H_
#include "engine/display//GroupMesh.h"
#endif //_ITF_GROUPMESH_H_
*/

namespace ITF
{
    #define MAX_VERTEX_ALLOWED 30000

const ColorInteger GFXAdapter::m_default4WhiteColorTab[4] = {ColorInteger(0xFFFFFFFF), ColorInteger(0xFFFFFFFF), ColorInteger(0xFFFFFFFF), ColorInteger(0xFFFFFFFF) };

const UVSubdiv  GFXAdapter::bufferTypes[] = {
#ifdef ITF_WII
	{ 2, 2},
    { 2, 2},
    { 4, 1}, 
#else
    { 2, 2},
    { 4, 4},
    { 8, 2}, 
#endif
    { MESH_DEFAULT_HDIV, MESH_DEFAULT_VDIV } , 
    { MESH_DEFAULT_VDIV , MESH_DEFAULT_HDIV} 
};
const u32       GFXAdapter::bufferTypesNb = sizeof(GFXAdapter::bufferTypes)/sizeof(UVSubdiv);

String8 GFXAdapter::getAllowedBufferTypesStr()
{
    char str[bufferTypesNb*10];
    char *strPtr = str;
    for (u32 i=0; i<bufferTypesNb; i++)
    {
        if (i != 0 )
        {
            (*strPtr++) = ',';
        }
        sprintf(strPtr, "%ix%i", bufferTypes[i][0], bufferTypes[i][1]);
        strPtr += strlen(strPtr);
    }

    return String8(str);
}


//----------------------------------------------------------------------------//

GFXAdapter::GFXAdapter()
: m_SceneAlreadyResolve(bfalse)
{
    m_HideFlags = 0;
    u_CurrentBlendingMode = GFX_BLEND_UNKNOWN;
    mb_WireFrame = bfalse;
    m_screenWidth = 0;
    m_screenHeight = 0;
    m_fontTexture = 0;
    m_RenderTargetManager = 0;
    m_isAlphaRefEnabled = bfalse;

#ifndef ITF_FINAL
    m_showRasters = bfalse;
    m_showAfterFx = btrue;
    m_showDebugInfo = bfalse;
#endif // ITF_FINAL

    m_RenderSceneInTarget = bfalse;

    m_RContext_ZWRITE = bfalse;
    m_RContext_ZTEST = bfalse;

    m_internalIdxBufferTab = newAlloc(mId_GfxAdapter, ITF_IndexBuffer*[bufferTypesNb]);
    m_internalVtxBufferTab = newAlloc(mId_GfxAdapter, ITF_VertexBuffer*[bufferTypesNb]);
    ITF_MemSet(m_internalIdxBufferTab, 0, bufferTypesNb*sizeof(ITF_IndexBuffer*));
    ITF_MemSet(m_internalVtxBufferTab, 0, bufferTypesNb*sizeof(ITF_VertexBuffer*));

    m_internalSpritesIndexBuffer = NULL;
    m_spriteIndexBufferSize = 0;

    m_videoCapture         = NULL;

    m_useDepthBuffer = bfalse;
    m_fPs = 0.f;

    for (u32 i = 0; i < 10; i++)
        FBTexture[i] = 0;

    RENDER_TARGET_DEBUG = NULL;

    m_drawFlag = 0;
    m_currentDrawZ = 0.f;
#ifdef ITF_WINDOWS
    m_bPickingMode = bfalse;
#endif //ITF_WINDOWS
    m_AFX_needCopyBuffer = bfalse;
    m_AFX_needFullRt = bfalse;
    m_AFX_needRestituteBackBuffer = bfalse;
    //m_groupMeshManager = new GroupMeshManager;

    m_paused = bfalse;
    m_useFog = bfalse;
    m_overDrawMode = 0;
	m_ShaderMode = 1;
    m_showWireFrame = 0;
    m_useSeparateAlpha = bfalse;
    m_safeFrameMode = SF_NOSAFEFRAME;

    m_colorFog = Color::zero();
    m_internRTColor = Color::zero();
    m_useInternalRT = bfalse;
    m_useRotateZCamera = bfalse;

	m_currentRenderingMaterialType = GFX_MAT_DEFAULT;
    m_currentVertexFormat = VertexFormat_PCT;

#ifndef ITF_CONSOLE_FINAL
    m_drawSafeFrameRequested = bfalse;
#endif // ITF_CONSOLE_FINAL

    resetScreenShotRequest();

    m_monitorRefreshRate = 60.0f;
    m_displaydebugstats = btrue;
}

void GFXAdapter::GFX_computeMatrixLookAtRH( GMatrix44* _view, Vec3d* _vEyePt, Vec3d* _vLookatPt, Vec3d* _vUpVec )
{
    // compute Look At Matrix RH.
    Vec3d zaxis = (*_vEyePt - *_vLookatPt);
    zaxis.normalize();
    Vec3d xaxis;
    _vUpVec->crossProduct(zaxis, xaxis);
    xaxis.normalize();
    Vec3d yaxis;
    zaxis.crossProduct(xaxis, yaxis);

    _view->set(
        xaxis.m_x, yaxis.m_x, zaxis.m_x, 0.f,
        xaxis.m_y, yaxis.m_y, zaxis.m_y, 0.f,
        xaxis.m_z, yaxis.m_z, zaxis.m_z, 0.f,
        -xaxis.dot(*_vEyePt), -yaxis.dot(*_vEyePt), -zaxis.dot(*_vEyePt), 1.f
        );
}

void GFXAdapter::GFX_computeMatrixPerspectiveFovRH( GMatrix44* _proj, f32 _fov, f32 _aspect, f32 _zn, f32 _zf )
{
    // compute perspective projection Matrix RH.
    f32 Q = _zf / (_zn-_zf);
    f32 y = 1.f/tanf(_fov*0.5f);
    _proj->set(
        y/_aspect,  0.f,    0.f,    0.f,
        0.f,        y,      0.f,    0.f,
        0.f,        0.f,    Q,      -1.f,
        0.f,        0.f,    Q*_zn,  0.f
        );
}

void GFXAdapter::GFX_computeMatrixOrthoOffCenterRH( GMatrix44* _proj, f32 _l, f32 _r, f32 _b, f32 _t, f32 _zn, f32 _zf )
{
    // compute orthographic projection Matrix RH.
    f32 Qz = 1.f / (_zn-_zf);
    f32 Qw = 1.f / (_r - _l);
    f32 Qh = 1.f / (_t - _b);
    _proj->set(
        2.f*Qw,         0.f,            0.f,    0.f,
        0.f,            2.f*Qh,         0.f,    0.f,
        0.f,            0.f,            Qz,     0.f,
        -(_r+_l)*Qw,    -(_t+_b)*Qh,    _zn*Qz, 1.f
        );
}

void GFXAdapter::computeRenderMatrix()
{
#ifdef ITF_GFX_PLATFORM_COMMON_FUNCTIONS
    // compute Matrix rendering from mg_World, mg_View, mg_Proj.
    GMatrix44 WorldView;
    M44_mul4x4Out((Matrix44*)&WorldView,        (const Matrix44*)&mg_World,  (const Matrix44*)&mg_View);
    M44_mul4x4Out((Matrix44*)&mg_WorldViewProj, (const Matrix44*)&WorldView, (const Matrix44*)&mg_Proj);

    //----
    
    M44_mul4x4Out((Matrix44*)&mg_ViewProj,      (const Matrix44*)&mg_View,   (const Matrix44*)&mg_Proj);
    M44_matrixInverse(mg_ViewProj_Inv.fa16, mg_ViewProj.fa16); 

    //----
    
    GMatrix44 viewNoT = mg_View;
    viewNoT._m41 = viewNoT._m42 = viewNoT._m43 = 0.f;
    
    M44_mul4x4Out((Matrix44*)&mg_ViewNoTProj,   (const Matrix44*)&viewNoT,   (const Matrix44*)&mg_Proj);
    M44_matrixInverse(mg_ViewNoTProj_Inv.fa16, mg_ViewNoTProj.fa16); 
#endif // ITF_GFX_PLATFORM_COMMON_FUNCTIONS
}

void GFXAdapter::setCamera(float _x, float _y, float _z, Camera* _cam)
{
    GFX_RECT Viewport;
    Vec2d topLeft = Vec2d::Zero;
    Vec2d botRight = Vec2d::One;
    if (_cam)
    {
        topLeft = _cam->m_topLeft;
        botRight = _cam->m_bottomRight;
    }
    Viewport.left = (i32)(topLeft.m_x * getScreenWidth());
    Viewport.top = (i32)(topLeft.m_y * getScreenHeight());
    Viewport.right = (i32)(botRight.m_x * getScreenWidth());
    Viewport.bottom = (i32)(botRight.m_y * getScreenHeight());
    setupViewport(&Viewport);
}


void GFXAdapter::destroyFonts()
{
    for (u32 index = 0;index < m_Fonts.size();++index)
    {
        RESOURCE_MANAGER->removeUserToResourceData(m_Fonts[index]);
    }

    m_Fonts.clear();

}

void GFXAdapter::setThreadUsageScreenCapture(u32 _uThreadUsage,bbool _bStepByStep)  
{
#ifdef ITF_SUPPORT_VIDEOCAPTURE
    if (!m_videoCapture)
        m_videoCapture = new VideoCapture(_uThreadUsage,_bStepByStep);
#endif //ITF_SUPPORT_VIDEOCAPTURE
}

void GFXAdapter::destroy()
{
#ifdef ITF_SUPPORT_VIDEOCAPTURE
    SF_DEL(m_videoCapture);
#endif //ITF_SUPPORT_VIDEOCAPTURE

    //SF_DEL(m_groupMeshManager);

    SF_DEL_ARRAY(m_internalIdxBufferTab);
    SF_DEL_ARRAY(m_internalVtxBufferTab);
}
   
void   GFXAdapter::destroyResources()
{
    SF_DEL(m_RenderTargetManager);
}

GFXAdapter::~GFXAdapter()
{
    destroy();
}


void GFXAdapter::setResolution(u32 _screenWidth, u32 _screenHeight)
{
    if ((_screenWidth != m_screenWidth || m_screenHeight != _screenHeight))
    {
        m_screenWidth = _screenWidth;
        m_screenHeight = _screenHeight;
#ifndef ITF_WII        
        resetDevice(0);
#endif // ITF_WII        
    }
    else
    {
        m_screenWidth = _screenWidth;
        m_screenHeight = _screenHeight;
    }
    Vec2d topLeft = Vec2d(0, 0);
    Vec2d botRight = Vec2d((f32)m_screenWidth, (f32)m_screenHeight);
    m_screenAABB = AABB(topLeft, botRight);

}

//----------------------------------------------------------------------------//

Texture* GFXAdapter::loadTexture(   const char* _path, u32 _alpha   )
{    
    const ResourceID& id = RESOURCE_MANAGER->newResourceIDFromFile(Resource::ResourceType_Texture, Path(_path));
    Texture* texture = (Texture*)id.getResource();
    loadTexture(texture, _path, _alpha);
    return texture;
}

//----------------------------------------------------------------------------//
/*
bbool isPointBehindCamera(const Vec3d& _pt)
{
    if (CAMERA->isBiased())
    {
        Vec3d camPos = CAMERA->getCorrectedPos();
        Vec3d vcamToIcon = _pt - camPos;
        Vec3d camLookAt = CAMERA->getUnnormalizedLookAtDirection();
        if (vcamToIcon.dot(camLookAt) <= 0.f)
        {
            return btrue;
        }
    }
    else
    {
        Vec3d camPos = CAMERA->getPos();
        if (_pt.m_z >= CAMERA->getZ())
            return btrue;
    }
    return bfalse;
}

bbool clipLineToCam(Vec3d& _p1, Vec3d& _p2, bbool _clipP2)
{
    if (CAMERA->isBiased())
    {
        Vec3d pos, lookat, V, startPt;
        if (_clipP2)
        {
            startPt = _p2;
            V = _p1 - _p2;
        }
        else
        {
            startPt = _p1;
            V = _p2 - _p1;
        }

        f32 vLen = V.norm();
        if (vLen == 0.f)
            return bfalse;
        V.normalize();

        pos = CAMERA->getCorrectedPos();
        lookat = CAMERA->getUnnormalizedLookAtDirection();
        Vec3d nLookat = lookat;
        nLookat.normalize();

        // calculate camera plane equation Ax + By + Cz + D = 0
        f32 A = nLookat.m_x;
        f32 B = nLookat.m_y;
        f32 C = nLookat.m_z;
        f32 D = -(A * pos.m_x + B * pos.m_y + C * pos.m_z);

        // resolve line ray VS camera plane equation
        f32 denom = A * V.m_x + B * V.m_y + C * V.m_z;
        if (denom == 0.f)
            return bfalse;
        f32 k = (-D - A*startPt.m_x - B * startPt.m_y - C * startPt.m_z) / denom;
        if (k > 0.f && k < vLen)
        {
            if (_clipP2)
                _p2 = startPt + k * V;
            else
                _p1 = startPt + k * V;
            return btrue;
        }
        return bfalse;
    }
    return bfalse;
}
*/
void GFXAdapter::fillTriangleList(const DBGLine& l)
{
    Vec3d p13d(l.p1.m_x, l.p1.m_y, l.z);
    Vec3d p23d(l.p2.m_x, l.p2.m_y, l.z);

 /*   
    bbool clipP1 = isPointBehindCamera(p13d);
    bbool clipP2 = isPointBehindCamera(p23d);

    if (clipP1 && clipP2)
        return;
 
    if (clipP1)
        clipLineToCam(p13d, p23d);
    else if (clipP2)
        clipLineToCam(p23d, p13d);

    bbool clipP1 = clipLineToCam(p13d, p23d, bfalse);
    bbool clipP2 = clipLineToCam(p13d, p23d, btrue);
*/
    Vec3d p12d, p22d;

    compute3DTo2D(p13d, p12d);
    compute3DTo2D(p23d, p22d);


    bbool clip1 = (p12d.m_z < 0.f) || (p12d.m_z > 1.f);
    bbool clip2 = (p22d.m_z < 0.f) || (p22d.m_z > 1.f);
    if (clip1 && clip2)
        return;

    Vec3d perp = (p22d - p12d).getPerpendicular();
    perp.setLength(l.size * 0.5f);
    p12d += perp;
    p22d += perp;

    Vec3d p33dZ0, p43dZ0;
    compute2DTo3D(p12d, p33dZ0);
    compute2DTo3D(p22d, p43dZ0);

    Vec3d p3Final = p33dZ0;
    p3Final.m_z = l.z;
    Vec3d p4Final = p43dZ0;
    p3Final.m_z = l.z;

    /// fill vertex buffer.
    u32 color = Color(l.alpha, l.r, l.g, l.b).getAsU32();
    VertexPC v[6];
    Vec3d perp3d = p3Final - p13d;
    p13d -= perp3d;
    p23d -= perp3d;
    v[0].setData(p13d, color);
    v[1].setData(p4Final, color);
    v[2].setData(p3Final, color);
    v[3].setData(p13d, color);
    v[4].setData(p23d, color);
    v[5].setData(p4Final, color);

    for (i32 i=0;i<6;++i)
    {
        m_DBGTriangleList.push_back(v[i]);
    }
}

void GFXAdapter::fillTriangleList(const DBG3DLine& l)
{
    Vec3d p13d = l.p1;
    Vec3d p23d = l.p2;

    Vec3d p12d, p22d;

    compute3DTo2D(p13d, p12d);
    compute3DTo2D(p23d, p22d);

    bbool clip1 = (p12d.m_z < 0.f) || (p12d.m_z > 1.f);
    bbool clip2 = (p22d.m_z < 0.f) || (p22d.m_z > 1.f);
    if (clip1 && clip2)
        return;

    Vec3d perp = (p22d - p12d).getPerpendicular();
    perp.m_z = 0.f;
    perp.setLength(l.size * 0.5f);
    p12d += perp;
    p22d += perp;

    Vec3d p33dZ0, p43dZ0;
    compute2DTo3D(p12d, p33dZ0);
    compute2DTo3D(p22d, p43dZ0);

    Vec3d p3Final = p33dZ0;
//    p3Final.m_z = l.z;
    Vec3d p4Final = p43dZ0;
  //  p3Final.m_z = l.z;


    /// fill vertex buffer.
    u32 color = Color(1.f, l.r, l.g, l.b).getAsU32();
    VertexPC v[6];
    Vec3d perp3d = p3Final - p13d;
    p13d -= perp3d;
    p23d -= perp3d;
    v[0].setData(p13d, color);
    v[1].setData(p4Final, color);
    v[2].setData(p3Final, color);
    v[3].setData(p13d, color);
    v[4].setData(p23d, color);
    v[5].setData(p4Final, color);

    for (i32 i=0;i<6;++i)
    {
        m_DBGTriangleList.push_back(v[i]);
    }
}

void GFXAdapter::fillTriangleList( const DBGTriangle3D& _t )
{
    /// fill vertex buffer.
    u32 color = Color(1.f, _t.r, _t.g, _t.b).getAsU32();

    VertexPC v[3];
    v[0].setData(_t.p1, color);
    v[1].setData(_t.p2, color);
    v[2].setData(_t.p3, color);

    for (i32 i=0;i<3;++i)
        m_DBGTriangleList.push_back(v[i]);
}

void GFXAdapter::render2D()
{
    init2DRender();
    drawPrimitives2d(m_container2d);

#ifdef ITF_CTR
    init2DRenderSubScreen();
    drawPrimitives2d(m_container2dSubScreen);
#endif

    end2DRender();
}

#ifndef ITF_CONSOLE_FINAL
void GFXAdapter::drawDBGPrimitives()
{
#if ITF_DEBUG_LEVEL > 0
    setGlobalColor(Color::white());
    {
        for (u32 it = 0; it < m_DBGSpheres.size();)
        {
            DBGSphere& c = m_DBGSpheres[it];
            drawSphere(c.x, c.y, c.rad, c.r, c.g, c.b, c.z, c.segsCount, c.volume, c.xaxis, c.yaxis, c.zaxis);
            c.duration -= LOGICDT;
            if (c.duration <= 0.0f)
                m_DBGSpheres.eraseNoOrder(it);
            else
                it++;
        }

        for (u32 it = 0; it < m_DBGCircles.size();)
        {
            DBGCircle& c = m_DBGCircles[it];
            drawCircle(c.x, c.y, c.rad, c.r, c.g, c.b, c.z, c.segsCount);
            c.duration -= LOGICDT;
            if (c.duration <= 0.0f)
                m_DBGCircles.eraseNoOrder(it);
            else
                it++;
        }

        for (u32 it = 0; it < m_DBGBoxes.size();)
        {
            DBGBox& b = m_DBGBoxes[it];
            drawBox(b.pos, b.angle, b.extent, b.r, b.g, b.b, b.z, b.fill );
            b.duration -= LOGICDT;
            if (b.duration <= 0.0f)
                m_DBGBoxes.eraseNoOrder(it);
            else
                it++;
        }
    }
    
    {
        m_DBGTriangleList.reserve((m_DBGLines.size()+m_DBGLinesDuration.size())*6);
     
        for (u32 it = 0; it < m_DBGLines.size();it++)
        {
            fillTriangleList(m_DBGLines[it]);
        }

        for (u32 it = 0; it < m_DBGLinesDuration.size();)
        {
            DBGLine& l = m_DBGLinesDuration[it];

            l.duration -= LOGICDT;
            fillTriangleList(l);

            if (l.duration <= 0.0f)
                m_DBGLinesDuration.eraseNoOrder(it);
            else
                it++;
        }

        for (u32 it = 0; it < m_DBG3DLines.size();)
        {
            DBG3DLine& l = m_DBG3DLines[it];

            l.duration -= LOGICDT;
            fillTriangleList(l);

            if (l.duration <= 0.0f)
                m_DBG3DLines.eraseNoOrder(it);
            else
                it++;
        }

        m_DBGLines.clear();
        
    }

    
    {
        for (u32 it = 0; it < m_DBGTriangles.size();)
        {
            DBGTriangle& t = m_DBGTriangles[it];
            drawTriangle(t.p1, t.p2, t.p3, t.r, t.g, t.b, t.a, t.z);
            t.duration -= LOGICDT;
            if (t.duration <= 0.0f)
                m_DBGTriangles.eraseNoOrder(it);
            else
                it++;
        }
    }
    {
        for (u32 it = 0; it < m_DBGTextureTriangles.size();)
        {
            DBGTextureTriangle& t = m_DBGTextureTriangles[it];
            ITF_VECTOR<Vertex>& vtx = *t.pVtx;
            drawTriangle(vtx[0], vtx[1], vtx[2], t.t);
            drawTriangle(vtx[1], vtx[2], vtx[3], t.t);
            t.duration -= LOGICDT;
            if (t.duration <= 0.0f)
                m_DBGTextureTriangles.eraseNoOrder(it);
            else
                it++;
        }
    }
 

#ifndef ITF_WII
    if (!isDeviceLost())
#endif // ITF_WII
    { 
 
        i32 listSize = m_DBGTriangleList.size();
        i32 startPos = 0;
        i32 currentSize = listSize;

        if (currentSize>MAX_VERTEX_ALLOWED)
            currentSize = MAX_VERTEX_ALLOWED;

        while (currentSize>0)
        {
                ITF_Mesh meshLines;
                VertexPC    *pdata = 0;
                meshLines.m_vtxBuffer = m_VertexBufferManager.createVertexBuffer
                    (
                    currentSize,
                    VertexFormat_PC,
                    sizeof(VertexPC),
                    btrue
                    );

                ITF_MeshElement elem;

                elem.m_material.m_textureDiffuse.invalidateResourceId();
                elem.m_startIndex                   = 0;
                elem.m_count                        = currentSize;

                meshLines.m_ElementList.push_back(elem);

                getVertexBufferManager().LockVertexBuffer(meshLines.m_vtxBuffer, (void **) &pdata);
                VertexPC    *write = pdata;
              
                ITF_MemcpyWriteCombined(write,&m_DBGTriangleList[startPos],sizeof(VertexPC)*currentSize);

                getVertexBufferManager().UnlockVertexBuffer(meshLines.m_vtxBuffer);

                drawFlatMesh(meshLines);

                startPos += MAX_VERTEX_ALLOWED;
                listSize -= MAX_VERTEX_ALLOWED;
                currentSize  = listSize;

                if (currentSize>MAX_VERTEX_ALLOWED)
                    currentSize = MAX_VERTEX_ALLOWED;

                m_VertexBufferManager.removeVertexBuffer(meshLines.m_vtxBuffer);
        }
    }
       
    m_DBGTriangleList.clear();
#endif // ITF_DEBUG_LEVEL > 0
}

//----------------------------------------------------------------------------//

void GFXAdapter::drawPolyLine(const PolyLine* _pl)
{
    i32 iPosCount = _pl->getPosCount();
    for (u32 i = 0; i < u32(iPosCount); i++)
    {
        const PolyLineEdge& thisData = _pl->getEdgeAt(i);
/*
        if ((_pl->m_pickingInfo) && ((u32)_pl->m_pickingInfo->m_extraInformation == i))
            drawSphere(thisData.m_pos.m_x, thisData.m_pos.m_y, 0.f, POLYLINE_SPHERE_RADIUS * 8.f, 8, 8, 1, 1, 0, 1);
        else
            drawSphere(thisData.m_pos.m_x, thisData.m_pos.m_y, 0.f, POLYLINE_SPHERE_RADIUS, 8, 8, 1);
*/
        if (i < u32(iPosCount-1))
        {
            const PolyLineEdge& nextData = _pl->getEdgeAt(i+1);
            drawArrow(thisData.getPos(), nextData.getPos(), 1, 1, 1, 0.2f);
/*
            const Vec2d& n = thisData.m_normalizedVector.getPerpendicular() * 4.f;
            Vec2d center = thisData.m_pos + thisData.m_vector * 0.5; 
            drawLine(center, center + n, 1, 0, 0 , 1.f);*/
        }
    }
}
#endif // ITF_CONSOLE_FINAL

//----------------------------------------------------------------------------//

void GFXAdapter::updateCachedScaleValues (Texture* _Tex)
{
    //
    // calculate what to use for x scale
    //
    const float orgW = (float)_Tex->m_datasizeX;
    const float texW = (float)_Tex->getSizeX();

    // if texture and original data width are the same, scale is based
    // on the original size.
    // if texture is wider (and source data was not stretched), scale
    // is based on the size of the resulting texture.
    _Tex->m_texelScaling.m_x = 1.0f / ((orgW == texW) ? orgW : texW);

    //
    // calculate what to use for y scale
    //
    const float orgH = (float)_Tex->m_datasizeY;
    const float texH = (float)_Tex->getSizeY();

    // if texture and original data height are the same, scale is based
    // on the original size.
    // if texture is taller (and source data was not stretched), scale
    // is based on the size of the resulting texture.
    _Tex->m_texelScaling.m_y = 1.0f / ((orgH == texH) ? orgH : texH);
}

//----------------------------------------------------------------------------//

float GFXAdapter::getSizeNextPOT(float sz) const
{
    u32 size = static_cast<u32>(sz);

    // if not power of 2
    if ((size & (size - 1)) || !size)
    {
        int log = 0;

        // get integer log of 'size' to base 2
        while (size >>= 1)
            ++log;

        // use log to calculate value to use as size.
        size = (2 << log);
    }

    return static_cast<float>(size);
}

//----------------------------------------------------------------------------//
#ifndef ITF_CONSOLE_FINAL
void GFXAdapter::drawDBGCircle(float _x, float _y, float _radius, float _r, float _g, float _b, float _duration, float _z, u32 _segsCount)
{
#ifndef ITF_FINAL
    if (hasHideFlags(HideFlag_DBGPrimitive))
        return;
#endif
    #if ITF_DEBUG_LEVEL > 0
        DBGCircle c;
        c.x = _x;
        c.y = _y;
        c.rad = _radius;
        c.r = _r;
        c.g = _g;
        c.b = _b;
        c.duration = _duration;
        c.z = _z;
        c.segsCount = _segsCount;
        m_DBGCircles.push_back(c);
    #else
        drawCircle(_x, _y, _radius, _r, _g, _b, _z, _segsCount);
    #endif
}
#endif // ITF_CONSOLE_FINAL

#ifndef ITF_CONSOLE_FINAL
void GFXAdapter::drawDBGSphere(float _x, float _y, float _radius, float _r, float _g, float _b, float _duration, float _z, u32 _segsCount, bbool _volume, bbool _xaxis, bbool _yaxis, bbool _zaxis)
{
#ifndef ITF_FINAL
    if (hasHideFlags(HideFlag_DBGPrimitive))
        return;
#endif
    #if ITF_DEBUG_LEVEL > 0
        DBGSphere c;
        c.x = _x;
        c.y = _y;
        c.rad = _radius;
        c.r = _r;
        c.g = _g;
        c.b = _b;
        c.duration = _duration;
        c.z = _z;
        c.segsCount = _segsCount;
        c.volume = _volume;
        c.xaxis = _xaxis;
        c.yaxis = _yaxis;
        c.zaxis = _zaxis;
        m_DBGSpheres.push_back(c);
    #else
        drawSphere(_x, _y, _radius, _r, _g, _b, _z, _segsCount, _volume, _xaxis, _yaxis, _zaxis);
    #endif
}
#endif // ITF_CONSOLE_FINAL

//----------------------------------------------------------------------------//
#ifndef ITF_CONSOLE_FINAL
void GFXAdapter::drawDBGBox( const Vec2d& _pos, f32 _angle, const Vec2d& _extent, float _r, float _g, float _b, float _duration, float _z, bbool _fill )
{
#ifndef ITF_FINAL
    if (hasHideFlags(HideFlag_DBGPrimitive))
        return;
#endif
    #if ITF_DEBUG_LEVEL > 0
        DBGBox b;
        b.pos = _pos;
        b.angle = _angle;
        b.extent = _extent;
        b.r = _r;
        b.g = _g;
        b.b = _b;
        b.duration = _duration;
        b.z = _z;
        b.fill = _fill;
        m_DBGBoxes.push_back(b);
    #else
        drawBox(_pos, _angle, _extent, _r, _g, _b, _z);
    #endif
}
#endif // ITF_CONSOLE_FINAL

//----------------------------------------------------------------------------//
#ifndef ITF_CONSOLE_FINAL
void GFXAdapter::drawDBGTriangle(   const Vec2d& _p1, const Vec2d& _p2, const Vec2d& _p3, float _r, float _g, float _b, float _a, float _duration, float _z)
{
#ifndef ITF_FINAL
    if (hasHideFlags(HideFlag_DBGPrimitive))
        return;
#endif
    #if ITF_DEBUG_LEVEL > 0
        DBGTriangle t;
        t.p1 = _p1;
        t.p2 = _p2;
        t.p3 = _p3;
        t.r = _r;
        t.g = _g;
        t.b = _b;
        t.a = _a;
        t.duration = _duration;
        t.z = _z;
        m_DBGTriangles.push_back(t);
    #else
        drawTriangle(_p1, _p2, _p3, _r, _g, _b, _a, _z);
    #endif
}
#endif // ITF_CONSOLE_FINAL

//----------------------------------------------------------------------------//
#ifndef ITF_CONSOLE_FINAL
void GFXAdapter::drawDBGTextureTriangle(   ITF_VECTOR<Vertex>* _pVtx, Texture* _tex, float _duration, float _z )
{
#ifndef ITF_FINAL
    if (hasHideFlags(HideFlag_DBGPrimitive))
        return;
#endif
    #if ITF_DEBUG_LEVEL > 0
        DBGTextureTriangle t;
        t.pVtx = _pVtx;
        t.duration = _duration;
        t.z = _z;
        t.t = _tex;
        m_DBGTextureTriangles.push_back(t);
    #else
        ITF_VECTOR<Vertex>& vtx = *_pVtx;
        drawTriangle(vtx[0], vtx[1], vtx[2], _tex);
        drawTriangle(vtx[1], vtx[2], vtx[3], _tex);
    #endif
}
#endif // ITF_CONSOLE_FINAL

//----------------------------------------------------------------------------//
#ifndef ITF_CONSOLE_FINAL
void GFXAdapter::drawDBGQuad (   const Vec2d& _pos, f32 _width, f32 _height, const Texture * _tex, f32 _angle, float _duration , float _z, int _indexInAtlas, bbool _flipX, bbool _flipY, u32 _color )
{
#ifndef ITF_FINAL
    if (hasHideFlags(HideFlag_DBGPrimitive))
        return;
#endif
    #if ITF_DEBUG_LEVEL > 0
        f32 c,s;
        f32_CosSin(_angle, &c, &s);
        Vec2d HorizAxis(c, -s);
        
        HorizAxis *= 0.5f;
            
        Vec2d vertAxis = HorizAxis.getPerpendicular();

        HorizAxis *= _width;
        vertAxis *= _height;

        DBGTextureQuad q;
        q.duration = _duration;
        q.z = _z;
        q.t = _tex;
        const UVAtlas *atlas = _tex->getUVAtlas();
        Vec2d uv[4];

        if (atlas)
            atlas->get4UVAt(_indexInAtlas, uv);
        else
        {
            //TODO : order may be incorrect
            uv[0] = Vec2d(0.f,1.f);
            uv[1] = Vec2d(0.f,0.f);
            uv[2] = Vec2d(1.f,0.f);
            uv[3] = Vec2d(1.f,1.f);
        }
        if (_flipX)
        {
            std::swap(uv[0].m_x, uv[2].m_x);
            std::swap(uv[1].m_x, uv[3].m_x);
        }
        if (_flipY)
        {
            std::swap(uv[0].m_y, uv[1].m_y);
            std::swap(uv[2].m_y, uv[3].m_y);
        }
        q.mVtx[0].setData(_pos - HorizAxis - vertAxis, uv[0], _color, _z);
        q.mVtx[1].setData(_pos - HorizAxis + vertAxis, uv[1], _color, _z);
        q.mVtx[2].setData(_pos + HorizAxis - vertAxis, uv[3], _color, _z);
        q.mVtx[3].setData(_pos + HorizAxis + vertAxis, uv[2], _color, _z);

        m_DBGTextureQuads.push_back(q);
    #endif // ITF_DEBUG_LEVEL > 0
}
#endif // ITF_CONSOLE_FINAL

//----------------------------------------------------------------------------//
#ifndef ITF_CONSOLE_FINAL
void GFXAdapter::drawDBGLine(const Vec2d& _p1, const Vec2d& _p2, float _r, float _g, float _b, float _duration, float _size, float _z, float _alpha)
{
#ifndef ITF_FINAL
    if (hasHideFlags(HideFlag_DBGPrimitive))
        return;
#endif
    #if ITF_DEBUG_LEVEL > 0
       DBGLine l;
        l.p1 = _p1;
        l.p2 = _p2;
        l.r = _r;
        l.g = _g;
        l.b = _b;
        l.size = _size;
        l.duration = _duration;
        l.z  =_z;
        l.alpha = _alpha;

        if (_duration>0.0f)
            m_DBGLinesDuration.push_back(l);
        else
            m_DBGLines.push_back(l);
       

    #else
        drawLine(_p1, _p2, _r, _g, _b, _size, _alpha);
    #endif
}
#endif // ITF_CONSOLE_FINAL

#ifndef ITF_CONSOLE_FINAL
void GFXAdapter::drawDBG3DLine(const Vec3d& _p1, const Vec3d& _p2, float _r, float _g, float _b, float _duration, float _size)
{
#ifndef ITF_FINAL
    if (hasHideFlags(HideFlag_DBGPrimitive))
        return;
#endif
    #if ITF_DEBUG_LEVEL > 0
        DBG3DLine l;
        l.p1 = _p1;
        l.p2 = _p2;
        l.r = _r;
        l.g = _g;
        l.b = _b;
        l.size = _size;
        l.duration = _duration;
        m_DBG3DLines.push_back(l);
    #endif
}
#endif // ITF_CONSOLE_FINAL

#ifndef ITF_CONSOLE_FINAL
void GFXAdapter::drawDBGAABB(const AABB& _aabb, float _r, float _g, float _b, float _duration, float _size, float _z)
{
#ifndef ITF_FINAL
    if (hasHideFlags(HideFlag_DBGPrimitive))
        return;
#endif
    const Vec2d v1(_aabb.getMin());
    const Vec2d v2(_aabb.getMaxXMinY());
    const Vec2d v3(_aabb.getMax());
    const Vec2d v4(_aabb.getMinXMaxY());

    drawDBGLine(v1, v2, _r, _g, _b, _duration, _size, _z);
    drawDBGLine(v2, v3, _r, _g, _b, _duration, _size, _z);
    drawDBGLine(v3, v4, _r, _g, _b, _duration, _size, _z);
    drawDBGLine(v4, v1, _r, _g, _b, _duration, _size, _z);
}
#endif // ITF_CONSOLE_FINAL

//----------------------------------------------------------------------------//
bbool GFXAdapter::SetTextureResource( int _Sampler, ResourceID _Texture, bbool _linearFiltering)
{
    if (!_Texture.isValidResourceId())
        return bfalse;

    Texture * texture = (Texture *)_Texture.getResource();
    ITF_ASSERT(texture);
    if (!texture)
        return bfalse;

    SetTexture(_Sampler, texture, _linearFiltering);
    return btrue;
}

//----------------------------------------------------------------------------//

void GFXAdapter::drawTriangles(const Triangle& _triangle, const Texture* _txt)
{
    const ITF_VECTOR<Vertex>& v = *_triangle.m_vertexs;

    //  with tab index
    if ( _triangle.m_indexes )
    {
        const ITF_VECTOR<u32>&  idx = *_triangle.m_indexes;

        for (u32 i=_triangle.index; i<_triangle.index +_triangle.taille; i+=3)   
            drawTriangle(v[idx[i]], v[idx[i+2]], v[idx[i+1]], _txt);

        return;
    }

    //  without tab index
    for (u32 i=_triangle.index; i<_triangle.index+_triangle.taille; i+=3)     
        drawTriangle(v[i], v[i+2], v[i+1], _txt);
}

//----------------------------------------------------------------------------//

void GFXAdapter::drawStrip(const Strip& _strip, const Texture* _txt)
{
    const ITF_VECTOR<Vertex>& v = *_strip.m_vertexs;

        //  with tab index
    if ( _strip.m_indexes )
    {
        const ITF_VECTOR<u32>&  idx = *_strip.m_indexes;

        for (u32 i=_strip.index; i<_strip.index+_strip.taille -2; i+=2)   
        {
            //ITF_ASSERT(i+3 < (int)v.size());
//             if (i+3 < (int)v.size())
//             {
                drawTriangle(v[idx[i]], v[idx[i+2]], v[idx[i+1]], _txt);
                drawTriangle(v[idx[i+2]], v[idx[i+3]], v[idx[i+1]], _txt);
//             }
        }
        return;
    }

        //  without tab index
    for (u32 i=_strip.index; i<_strip.index+_strip.taille -2; i+=2)   
    {
        //ITF_ASSERT(i+3 < (int)v.size());
//         if (i+3 < (int)v.size())
//         {
            drawTriangle(v[i], v[i+2], v[i+1], _txt);
            drawTriangle(v[i+2], v[i+3], v[i+1], _txt);
//         }
    }

}

//----------------------------------------------------------------------------//

void GFXAdapter::drawFan(const Fan& _fan, const Texture* _txt)
{
    const ITF_VECTOR<Vertex>& v = *_fan.m_vertexs;



        //  with tab index
    if ( _fan.m_indexes )
    {

        const ITF_VECTOR<u32>&  idx = *_fan.m_indexes;

        //  debug
//          if ( _fan.index +_fan.taille -2 >= int(idx.size()) )
//              return;
//         if ( idx[_fan.index]>=int(v.size()) )
//             return;


        for (u32 i=_fan.index +1; i<_fan.index +_fan.taille -1; i++)  
        {
/*            if(_fan.index < 0)
                _asm int 3;
            if(_fan.index >= idx.size())
                _asm int 3;
            if((i+1) >= idx.size())
                _asm int 3;*/
                //  debug
//             if ( idx[i]>=int(v.size()) || idx[i+1]>=int(v.size())  )
//                 return;

            drawTriangle(v[idx[_fan.index]], v[idx[i+1]], v[idx[i]], _txt);
        }
        return;
    }

        //  debug
//     if ( _fan.index +_fan.taille -2 >=int(v.size()) )
//         return;

        //  without tab index
    for (u32 i=_fan.index +1; i<_fan.index +_fan.taille -1; i++)   
        drawTriangle(v[_fan.index], v[i+1], v[i], _txt);

}

//----------------------------------------------------------------------------//
void GFXAdapter::cancelAllTasks()
{
#if ITF_DEBUG_LEVEL > 0
    m_DBGSpheres.clear();
    m_DBGCircles.clear();
    m_DBGBoxes.clear();
    m_DBGLines.clear();
    m_DBGTriangles.clear();
    m_DBGTextureTriangles.clear();
    m_DBGTextureQuads.clear();
    m_DBGTexts.clear();
#endif // ITF_DEBUG_LEVEL > 0
}

//----------------------------------------------------------------------------//

void GFXAdapter::drawPrimitives()
{
    //m_groupMeshManager->draw();
#ifdef AFTERFXLIST    
    //transfer afterfxlist to renderlist
    u32 nFx = m_AfterFxZlist.getNumberEntry();
    if (nFx)
    {
        for (u32 i = 0; i < nFx; i++)
        {
            ZList_Node* sourcenode = m_AfterFxZlist.getOrderNodeAt(i);
            if (sourcenode->pPrimitiveInfo.m_type == GFX_ITF_REMOVED) continue;
            //m_RenderZlist.transferNodeTOZList(*sourcenode);
            AFTERFX* afx = (AFTERFX*)sourcenode->pPrimitiveInfo.mp_Primitive;

            if ( afx->isRenderInTarget() == 1 )
                m_RenderZlist.AddPrimitiveInZList((void*)afx, GFX_ITF_STARTRENDERTEX, NULL, afx->getZStart(),ObjectRef::InvalidRef);

            if (!(afx->isRenderInTarget() == 2 && !GFX_ADAPTER->isUseInternalRT()))
				m_RenderZlist.AddPrimitiveInZList((void*)afx, GFX_ITF_AFTERFX, NULL, afx->getZRender(),ObjectRef::InvalidRef);
		}
   
        m_AfterFxZlist.ZList_reset();
    }
#endif
    if (isUseInternalRT())
    {
        prepareInternalRT();
        
        if (m_RenderZlistRTarget.getNumberEntry())
        {
            m_RenderZlistRTarget.sort();
            drawZlistPrimitives(&m_RenderZlistRTarget);
            m_RenderZlistRTarget.ZList_reset();
        }
            
        endInternalRT();
    }

    if (!m_RenderZlist.getNumberEntry()) return;

    m_RenderZlist.sort();
    drawZlistPrimitives(&m_RenderZlist);

    m_RenderZlist.ZList_reset();

    /// Reset Sampler.
    SetTextureBind(0, 0);

    setUseInternalRT(bfalse);
}

//----------------------------------------------------------------------------//

void GFXAdapter::drawPrimitives2d( PrimitivesContainer2d* _container2d )
{
    Matrix44 matrix;
    matrix.setIdentity();

    Vec3d position;
    position.m_z = 0;
    Vec3d lastPosWidth;
    f32 width = 0.0f;

    /// Draw all containers.
    u32 count = m_PriorityCount2d;

    while (count--)
    {
        for (u32 meshId = 0; meshId < _container2d[count].MeshList.size() ; meshId++)
        {
            const PrimMeshInfo& prim = _container2d[count].MeshList[meshId];
            position.m_x = prim.Pos.m_x;
            position.m_y = (f32)getScreenHeight() - prim.Pos.m_y;
            M44_setMatrixTranslation(&matrix, position.m_x, position.m_y, position.m_z);
            // scale by negative, because y ortho view inversed.
            matrix.mulScale(Vec3d(prim.Scale.m_x, -prim.Scale.m_y, 1.f));
            
            ITF_Mesh& mesh  = *prim.mesh;
            mesh.setMatrix((GMatrix44&)matrix);

            switch(mesh.m_type)
            {
            case ITF_Mesh::meshType_PT:
                drawMeshPT(mesh, bfalse);
                break;
            default:
                drawMesh(mesh, bfalse);
                break;
            }
        }

        for (u32 AnimSceneId = 0; AnimSceneId < _container2d[count].AnimSceneList.size(); AnimSceneId++)
        {
            const PrimAnimMeshInfo& prim = _container2d[count].AnimSceneList[AnimSceneId];
            position.m_x = prim.Pos.m_x;
            position.m_y = (f32)getScreenHeight() - prim.Pos.m_y;
            M44_setMatrixTranslation(&matrix, position.m_x, position.m_y, position.m_z);
            matrix.mulScale(Vec3d(prim.Scale.m_x, prim.Scale.m_y, 1.f));

            // test rotation matrix.
            // Matrix44 rot;
            // M44_setMatrixRotationZ(&rot, ELLASPEDTIME * 2.f);
            // matrix = M44_mul4x4(&rot, &matrix);

            AnimMeshScene* pAnimMeshScene = prim.mesh;
            ITF_Mesh* mesh  = pAnimMeshScene->m_pMesh[0];
            mesh->setMatrix((GMatrix44&)matrix);

            pAnimMeshScene->m_pAnimDraw.Draw(pAnimMeshScene);
            pAnimMeshScene->m_AnimInfo.m_lights.clear();

        }

        for (u32 TrailIndex=0; TrailIndex<_container2d[count].TrailList.size(); TrailIndex++)
        {
            Trail *trail = _container2d[count].TrailList[TrailIndex].trail;
            trail->directDraw();
        }

        for (u32 FontId = 0; FontId < _container2d[count].FontsCallList.size(); FontId++)
        {
            const FontCall& fontcall = _container2d[count].FontsCallList[FontId];
            Texture* backGroundTexture = fontcall.getBackGroundTexture();
            if (fontcall.getUseBackground())
            {
                const Vec3d offset = fontcall.getTextureOffset2D().to3d();
                const Vec3d& pos = fontcall.getPosition() + offset;
                const f32 scale =  fontcall.getScale();

                f32 boxWidth;
                if (backGroundTexture)
                    boxWidth = fontcall.getBoxWidth() - offset.m_x + (10.0f * scale);
                else
                    boxWidth = fontcall.getBoxWidth();

                drawQuad2D(pos.m_x, pos.m_y, boxWidth, fontcall.getBoxHeight(), pos.m_z, fontcall.getBackGroundColour(), backGroundTexture, &fontcall.getBackGroundTextureStartUV(), &fontcall.getBackGroundTextureEndUV());
            }
            if (!fontcall.isAppend())
            {
                width = drawTextInternal(fontcall);
                lastPosWidth = fontcall.getPosition();
            }
            else
            {
                width = drawTextInternalAppend(fontcall,lastPosWidth);//sum the appends
            }

            lastPosWidth.m_x += width;            
        }


        for (u32 Texture2DId = 0; Texture2DId < _container2d[count].Quad2DInfoList.size(); Texture2DId++)
        {
            const Quad2DInfo& texInfo = _container2d[count].Quad2DInfoList[Texture2DId];
            if (texInfo.m_angle == 0.f)
            {
                drawQuad2D(texInfo.m_pos2D.m_x, texInfo.m_pos2D.m_y, texInfo.m_width, texInfo.m_height, texInfo.m_pos2D.m_z, texInfo.m_color, texInfo.m_texture, &texInfo.m_uvStart, &texInfo.m_uvEnd );
            }
            else
            {
                const f32 w2 = texInfo.m_width * 0.5f;
                const f32 h2 = texInfo.m_height * 0.5f;
                const f32 cx = texInfo.m_pos2D.m_x + w2;
                const f32 cy = texInfo.m_pos2D.m_y + h2;
                const f32 ang = texInfo.m_angle;
                const f32 c = cosf(ang);
                const f32 s = sinf(ang);
                auto rot = [&](f32 lx, f32 ly)->Vec2d { return Vec2d(cx + lx * c - ly * s, cy + lx * s + ly * c); };

                Vec2d pTL2 = rot(-w2, -h2);
                Vec2d pTR2 = rot( w2, -h2);
                Vec2d pBR2 = rot( w2,  h2);
                Vec2d pBL2 = rot(-w2,  h2);

                Vec3d pTL, pTR, pBR, pBL;
                compute2DTo3D(pTL2, texInfo.m_pos2D.m_z, pTL);
                compute2DTo3D(pTR2, texInfo.m_pos2D.m_z, pTR);
                compute2DTo3D(pBR2, texInfo.m_pos2D.m_z, pBR);
                compute2DTo3D(pBL2, texInfo.m_pos2D.m_z, pBL);
                Vec2d uvTL(texInfo.m_uvStart.m_x, texInfo.m_uvStart.m_y);
                Vec2d uvTR(texInfo.m_uvEnd.m_x,   texInfo.m_uvStart.m_y);
                Vec2d uvBR(texInfo.m_uvEnd.m_x,   texInfo.m_uvEnd.m_y);
                Vec2d uvBL(texInfo.m_uvStart.m_x, texInfo.m_uvEnd.m_y);
                Vertex vTL, vTR, vBR, vBL;
                vTL.setData(pTL, uvTL, texInfo.m_color);
                vTR.setData(pTR, uvTR, texInfo.m_color);
                vBR.setData(pBR, uvBR, texInfo.m_color);
                vBL.setData(pBL, uvBL, texInfo.m_color);
                drawTriangle(vTL, vTR, vBR, texInfo.m_texture, 0);
                drawTriangle(vTL, vBR, vBL, texInfo.m_texture, 0);
            }
        }

#ifndef ITF_FINAL
        if (isShowAfterFx()/* && allowRendering*/)             
#endif // ITF_FINAL
        {
			for (u32 AFXId = 0; AFXId < _container2d[count].AfterfxList.size(); AFXId++)
			{
				const PrimAFXInfo& priomInfo = _container2d[count].AfterfxList[AFXId];
                
				AFTERFX* afx = priomInfo.afx;
                bbool nobackbuffercopy = bfalse;
                if (afx->getType() == AFX_Fade || afx->getType() == AFX_BorderBright)
                    nobackbuffercopy = btrue;
#if defined(ITF_PS3) || defined(ITF_WII)
                // On PS3, we can directly sample the backbuffer
                // On WII, we can avoid the last backbuffer copy
                if (afx->getType() == AFX_Glow)
                    nobackbuffercopy = btrue;
#endif // defined(ITF_PS3) || defined(ITF_WII)

                prepareAfterFx(nobackbuffercopy);
                AFTERFX_drawOne(afx);
                endAfterfx(nobackbuffercopy, (GFX_BLENDMODE)afx->getCustomVal());
            }
		}

        _container2d[count].MeshList.clear();
        _container2d[count].AnimSceneList.clear();
        _container2d[count].FontsCallList.clear();
        _container2d[count].Quad2DInfoList.clear();
        _container2d[count].TrailList.clear();
		_container2d[count].AfterfxList.clear();
    }
}

void GFXAdapter::addPrimitive2d(Vec2d _Pos, Vec2d _scale, int _prio, PRIMITIVETYPE _type, void* _mesh, FontCall* _font, Quad2DInfo* _texInfo, AFTERFX* _afx, bbool _UpScreen)
{
    if (_prio >= (int)m_PriorityCount2d)
        _prio   = m_PriorityCount2d-1;
    
    if (_prio<0)
        _prio   = 0;

    switch (_type)
    {
        case GFX_ITF_MESH:
        {
            ITF_ASSERT (((ITF_Mesh*)_mesh)->isDataReady()); // only loaded meshes should be sent to the gfx adapter
            PrimMeshInfo prim;
            prim.mesh = (ITF_Mesh*)_mesh;
            prim.Pos = _Pos;
            prim.Scale = _scale;
            if (_UpScreen)
            {
                ITF_VECTOR<PrimMeshInfo>* ptr = &m_container2d[_prio].MeshList;
                ptr->push_back(prim);
            } 
            else
            {
                ITF_VECTOR<PrimMeshInfo>* ptr = &m_container2dSubScreen[_prio].MeshList;
                ptr->push_back(prim);
            }
        }
        break;

        case GFX_ITF_ANIMSCENE:
        {
            ITF_ASSERT(_mesh);
            PrimAnimMeshInfo prim;
            prim.mesh = (AnimMeshScene*)_mesh;
            prim.Pos = _Pos;
            prim.Scale = _scale;
            if (_UpScreen)
            {
                ITF_VECTOR<PrimAnimMeshInfo>* ptr = &m_container2d[_prio].AnimSceneList;
                ptr->push_back(prim);
            } 
            else
            {
                ITF_VECTOR<PrimAnimMeshInfo>* ptr = &m_container2dSubScreen[_prio].AnimSceneList;
                ptr->push_back(prim);
            }
         }
         break;              

        case GFX_ITF_TRAIL:
        {
            ITF_ASSERT(_mesh);
            PrimTrailInfo prim;
            prim.trail = (Trail*)_mesh;
            if (_UpScreen)
            {
                ITF_VECTOR<PrimTrailInfo>* ptr = &m_container2d[_prio].TrailList;
                ptr->push_back(prim);
            } 
            else
            {
                ITF_VECTOR<PrimTrailInfo>* ptr = &m_container2dSubScreen[_prio].TrailList;
                ptr->push_back(prim);
            }
        }
        break;

        case GFX_ITF_FONT:
        {
            ITF_ASSERT(_font);
            if (_UpScreen)
            {
                ITF_VECTOR<FontCall>* ptr = &m_container2d[_prio].FontsCallList;
                ptr->push_back(*_font);
            } 
            else
            {
                ITF_VECTOR<FontCall>* ptr = &m_container2dSubScreen[_prio].FontsCallList;
                ptr->push_back(*_font);
            }
         }
         break;  

        case GFX_QUADS:
        {
            ITF_ASSERT(_texInfo);
            if (_UpScreen)
            {
                ITF_VECTOR<Quad2DInfo>* ptr = &m_container2d[_prio].Quad2DInfoList;
                ptr->push_back(*_texInfo);
            } 
            else
            {
                ITF_VECTOR<Quad2DInfo>* ptr = &m_container2dSubScreen[_prio].Quad2DInfoList;
                ptr->push_back(*_texInfo);
            }
        }
        break;

        case GFX_ITF_AFTERFX:
        {
            ITF_ASSERT(_afx);

            PrimAFXInfo prim;
			prim.afx = _afx;
			
			if (_UpScreen)
            {
                ITF_VECTOR<PrimAFXInfo>* ptr = &m_container2d[_prio].AfterfxList;
                ptr->push_back(prim);
            } 
            else
            {
                ITF_VECTOR<PrimAFXInfo>* ptr = &m_container2dSubScreen[_prio].AfterfxList;
                ptr->push_back(prim);
            }
        }
        break;

    default:
        ITF_ASSERT(0);
        break;
    }
}

void GFXAdapter::computeUVAnim(GFX_UVANIM* _uvanim)
{
    if (!_uvanim->m_uvFlagAnim) return;

    Matrix44 mat;
    mat.setIdentity();

    if (_uvanim->m_uvFlagAnim & UVANIM_F_TRANS)
    {
        // get last post from matrix.
        Vec3d tr = Vec3d(_uvanim->m_UVMat._m41, _uvanim->m_UVMat._m42, 0.f);
        
        if (!isGFXPaused())
        {
            tr.m_x += LOGICDT * _uvanim->m_speedTrans.m_x;
            tr.m_y += LOGICDT * _uvanim->m_speedTrans.m_y;
        }
        mat.setTranslation(tr);
    }

    if (_uvanim->m_uvFlagAnim & UVANIM_F_ROTATE)
    {
       // Rotation about the Z axis.
        _uvanim->m_currentangle += LOGICDT * _uvanim->m_speedRotate;
        f32 c, s;
        f32_CosSin(_uvanim->m_currentangle, &c, &s);
        mat._m11 = c;
        mat._m12 = -s;
        mat._m14 = _uvanim->m_pivot.m_x;
        mat._m21 = s;
        mat._m22 = c;
        mat._m24 = _uvanim->m_pivot.m_y;
    }

    _uvanim->m_UVMat = *(GMatrix44*)&mat;

}

void GFXAdapter::vec3dProject(Vec3d* _out, const Vec3d* _v, const GFX_Viewport* _viewp, const GMatrix44* _ViewProj)
{
    Float4 v_in;
    setFloat4(v_in, _v->m_x, _v->m_y, _v->m_z, 1.f);

	Float4 v_out;
    M44_matrixCoordTransform(&v_out, (const Matrix44*)_ViewProj, &v_in);

	GFX_Vector4 getv_out;
	getFloat4(v_out, (f32*)&getv_out);

    const f32 invbw = (getv_out.m_w == 0.f) ? 0.f : 1.f/getv_out.m_w;
    getv_out.m_x *= invbw;
    getv_out.m_y *= invbw;
    getv_out.m_z *= invbw;

    _out->m_x = _viewp->m_x + _viewp->m_width  * (1.f + getv_out.m_x) / 2.f;
    _out->m_y = _viewp->m_y + _viewp->m_height * (1.f - getv_out.m_y) / 2.f;
    _out->m_z = _viewp->m_minZ + getv_out.m_z * (_viewp->m_maxZ - _viewp->m_minZ);
}

void GFXAdapter::vec3dUnProject(Vec3d* _out, const Vec3d* _v, const GFX_Viewport* _viewp, const GMatrix44* _ViewProj_InvT)
{
    Float4 v_in;
    setFloat4(v_in,
        - 1.0f + (_v->m_x - _viewp->m_x) * 2.f / _viewp->m_width,
          1.0f - (_v->m_y - _viewp->m_y) * 2.f / _viewp->m_height,
        (_v->m_z - _viewp->m_minZ) / (_viewp->m_maxZ - _viewp->m_minZ),
        1.0f
        );

    Float4 v_out;
    M44_matrixCoordTransform(&v_out, (const Matrix44*)_ViewProj_InvT, &v_in);

	GFX_Vector4 getv_out;
	getFloat4(v_out, (f32*)&getv_out);

    const f32 invbw = (getv_out.m_w == 0.f) ? 0.f : 1.f/getv_out.m_w;
    _out->m_x = getv_out.m_x * invbw;
    _out->m_y = getv_out.m_y * invbw;
    _out->m_z = getv_out.m_z * invbw;
}

void GFXAdapter::coord2DTo3D( const Vec2d& _in2d, float _zPlane, Vec2d& _out3d, bool noTranslation )
{
#ifdef ITF_GFX_PLATFORM_COMMON_FUNCTIONS
	GFX_Viewport viewp;
	viewp.m_x = 0;
	viewp.m_y = 0;
	viewp.m_width = getScreenWidth();
	viewp.m_height = getScreenHeight();
	viewp.m_minZ = 0.f;
	viewp.m_maxZ = 1.f;

	Vec3d vout;
	Vec3d vin(_in2d.m_x, _in2d.m_y, (f32)_zPlane);
	vec3dUnProject(&vout, &vin, &viewp, noTranslation ? &mg_ViewNoTProj_Inv : &mg_ViewProj_Inv);

	_out3d.m_x = (f32)vout.m_x;
	_out3d.m_y = (f32)vout.m_y;
#else // ITF_GFX_PLATFORM_COMMON_FUNCTIONS
    ITF_ASSERT(0);
#endif // ITF_GFX_PLATFORM_COMMON_FUNCTIONS
}

void GFXAdapter::compute3DTo2D(const Vec3d& _in, Vec3d& _out)
{
#ifdef ITF_GFX_PLATFORM_COMMON_FUNCTIONS
	GFX_Viewport viewp;
	viewp.m_x = 0;
	viewp.m_y = 0;
	viewp.m_width = getScreenWidth();
	viewp.m_height = getScreenHeight();
	viewp.m_minZ = 0.f;
	viewp.m_maxZ = 1.f;

	vec3dProject(&_out, &_in, &viewp, &mg_ViewProj);
#else // ITF_GFX_PLATFORM_COMMON_FUNCTIONS
    ITF_ASSERT(0);
#endif // ITF_GFX_PLATFORM_COMMON_FUNCTIONS
}

void GFXAdapter::compute2DTo3D(const Vec3d& _in, Vec3d& _out)
{
#ifdef ITF_GFX_PLATFORM_COMMON_FUNCTIONS

	GFX_Viewport viewp;
	viewp.m_x = 0;
	viewp.m_y = 0;
	viewp.m_width = getScreenWidth();
	viewp.m_height = getScreenHeight();
	viewp.m_minZ = 0.f;
	viewp.m_maxZ = 1.f;

	vec3dUnProject(&_out, &_in, &viewp, &mg_ViewProj_Inv);
#else // ITF_GFX_PLATFORM_COMMON_FUNCTIONS
    ITF_ASSERT(0);
#endif // ITF_GFX_PLATFORM_COMMON_FUNCTIONS
}

void GFXAdapter::compute2DTo3D(const Vec2d& _in, const f32 _worldZ, Vec3d& _out)
{
    Vec3d camCenterAtZ = Vec3d(CAMERA->getX(), CAMERA->getY(), _worldZ);
    if (CAMERA->m_bUseBias)
    {
        camCenterAtZ.m_x = CAMERA->getCorrectedPos().m_x;
        camCenterAtZ.m_y = CAMERA->getCorrectedPos().m_y;
    }
    Vec3d screen;
    compute3DTo2D(camCenterAtZ, screen);
    screen.m_x = _in.m_x;
    screen.m_y = _in.m_y;
    compute2DTo3D(screen, _out);
    _out.m_z = _worldZ; // projected z should be nearly the same as requested, but not quite: fix it here*/
}

#ifdef ITF_SUPPORT_CHEAT
void GFXAdapter::drawDBGText( const String& _text, float _x, float _y, f32 _r, f32 _g, f32 _b, bbool _useBigFont, FONT_ALIGNMENT _alignment )
{
#ifndef ITF_FINAL
    if (hasHideFlags(HideFlag_DBGPrimitive))
        return;
#endif
    DBGText txt;
    txt.m_text = _text;
    txt.m_x = _x;
    txt.m_y = _y;
    txt.m_r = _r;
    txt.m_g = _g;
    txt.m_b = _b;
    txt.m_alignment = _alignment;
    txt.m_useBigFont = _useBigFont;
    m_DBGTexts.push_back(txt);
}

void GFXAdapter::drawDBGTextFixed( const String& _text, float _x, float _y, f32 _r, f32 _g, f32 _b, FONT_ALIGNMENT _alignment )
{
#ifndef ITF_FINAL
	if (hasHideFlags(HideFlag_DBGPrimitive))
		return;
#endif
    DBGText txt;
    txt.m_text = _text;
    txt.m_x = _x;
    txt.m_y = _y;
    txt.m_r = _r;
    txt.m_g = _g;
    txt.m_b = _b;
    txt.m_alignment = _alignment;
    txt.m_useBigFont = bfalse;
    txt.m_useMonospaceFont = btrue;
    m_DBGTexts.push_back(txt);
}
#endif // ITF_SUPPORT_CHEAT

bbool GFXAdapter::isFontLoaded(u32 _index)
{
    const ResourceID& _fontResourceID = m_Fonts[_index];
    Resource* pFontResource = _fontResourceID.getResource();

    if (pFontResource && pFontResource->isPhysicallyLoaded())
    {
        return ((FontResource*) pFontResource)->getFont()->texturePageIsPhysicalReady();
    }

    return bfalse;
}

f32 FontCall::write() const
{
    Font* pFont = RESOURCE_MANAGER->getFont(m_fontId);
    if (!pFont)
        return 0.0f;

    if (m_height!= -1.f)
         pFont->setHeight(m_height);

    //TODO Batch them !
    pFont->prepareEffect();
    pFont->m_shadowOffset = m_shadowOffset;
    pFont->m_shadowcolor = m_shadowColor;
    pFont->m_autoScale = m_auto2dScale;
    pFont->m_lineSpacingFactor = m_lineSpacingFactor;

    if (!m_bUseStyle)
    {
        if (m_isWriteML)
            return pFont->writeML(m_color, m_Position.m_x, m_Position.m_y, m_Position.m_z, m_text, m_count, m_mode, m_write);
        else
            return pFont->write(m_color, m_Position.m_x, m_Position.m_y, m_Position.m_z, m_text, m_count, m_mode, m_write);
    }
    else
    {
        bbool isUseScale = bfalse;
        if (m_croppingMode == 0)
            isUseScale = btrue;

        pFont->writeBoxWithStyle(m_Position.m_x, m_Position.m_y, m_Position.m_z, m_is2D, m_boxPosition, m_boxPivot, m_boxScale, m_boxRotation, m_useTransBox, m_boxWidth,m_boxHeight, m_shadowOffset, m_shadowColor, m_count, m_mode, m_modeY, (*m_textTab), isUseScale, m_textTabStart, m_textTabStop, m_write);
        return m_boxWidth;
    }
}

f32 GFXAdapter::drawTextInternalAppend(const FontCall &_fontcall,const Vec3d& _positionWitdh)
{
    FontCall newone = _fontcall;
    newone.setPosition(newone.getPosition()+_positionWitdh);
    f32 width = newone.write();

    restoreDefaultShader();
    return width;
}

f32 GFXAdapter::drawTextInternal(const FontCall &_fontcall)
{
    f32 width = _fontcall.write();
    restoreDefaultShader();
    return width;
}

void    GFXAdapter::cleanFontCache()
{
    for (u32 index = 0;index<m_Fonts.size();++index)
    {
        Font* pFont = RESOURCE_MANAGER->getFont(m_Fonts[index]);
        if (pFont)
            pFont->cleanFontCache();       
    }
}

void GFXAdapter::appendText(const ResourceID& _fontResource, i32 _depthrank, u32 _color,const char* _text)
{
    FontCall fontcall;
    fontcall.setFont(_fontResource);
    fontcall.setColor(_color);
    fontcall.setText(_text);
    fontcall.setCount(0);
    fontcall.setAppend(btrue);

    addPrimitive2d(Vec2d::Zero, Vec2d::One, _depthrank, GFX_ITF_FONT, NULL, &fontcall);
}

void GFXAdapter::drawText(const ResourceID& _fontResource, i32 _depthrank, u32 _color,f32 _x, f32 _y, f32 _z, const Vec2d& _shadowOffset, u32 _shadowColor, const String& _text, i32 _count, u32 _mode, f32 _height, bbool _isWriteML)
{
#ifndef ITF_FINAL
    if (hasHideFlags(HideFlag_Font))
        return;
#endif
    FontCall fontcall;
    fontcall.setFont(_fontResource);
    fontcall.setColor(_color);
    fontcall.setPosition(Vec3d(_x,_y,_z));
    fontcall.setText(_text);
    fontcall.setCount(_count);
    fontcall.setMode(_mode);
    fontcall.setAppend(bfalse);

    fontcall.setShadowOffset(_shadowOffset);
    fontcall.setShadowColor(_shadowColor);

    fontcall.setHeight(_height);
    fontcall.setUseStyle(bfalse);
    fontcall.setWriteML(_isWriteML);

    addPrimitive2d(Vec2d::Zero, Vec2d::One, _depthrank, GFX_ITF_FONT, NULL, &fontcall);
}

void GFXAdapter::drawTextWithStyle(FontCall* _call, i32 _depthrank)
{
#ifndef ITF_FINAL
    if (hasHideFlags(HideFlag_Font))
        return;
#endif

	if (_call->getIs2D())
		addPrimitive2d(Vec2d::Zero, Vec2d::One, _depthrank, GFX_ITF_FONT, NULL, _call);
	else
	{
		FontCall* _newFc;
		_newFc = newAlloc(mId_GfxAdapter, FontCall);
		*_newFc = *_call;
        addPrimitiveInZList( _newFc, GFX_ITF_FONT, NULL, _newFc->getPosition().m_z,0,ObjectRef::InvalidRef);
	}
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void GFXAdapter::reloadFont()
{
    for (u32 i=0; i<m_Fonts.size(); i++)
    {
        FontResource* fr = reinterpret_cast<FontResource*>(m_Fonts[i].getResource());
       if (fr!=nullptr) fr->reload();
    }
}
u32 GFXAdapter::loadFont(const Path& name)
{
    ResourceID fontId;

    if (RESOURCE_MANAGER->getResourceIdFromFile(fontId, name.getStringID(), Resource::ResourceType_Font))
    {
        for (u32 i=0; i<m_Fonts.size(); i++)
        {
            if (m_Fonts[i] == fontId) return i;
        }
    }
    
    // Font not loaded or not in font list
    fontId = RESOURCE_MANAGER->newResourceIDFromFile(Resource::ResourceType_Font,name);
    RESOURCE_MANAGER->addUserToResourcePhysicalData(fontId);
    m_Fonts.push_back(fontId);
    return (m_Fonts.size()-1);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifndef ITF_FINAL
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void GFXAdapter::toggleRaster()
{
#if defined(RASTER_ENABLED)
    if (!m_showRasters)
    {
        RastersManager& rasterManager = RastersManager::get();
        if (!rasterManager.isHistoryEnabled())
        {
            rasterManager.setEnableHistory(btrue);
            rasterManager.setEnable(btrue);
        }
    }

    m_showRasters = !m_showRasters; 
#endif //RASTER_ENABLED
};
#endif //!ITF_FINAL

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ITF_VertexBuffer* GFXAdapter::createVertexBuffer(u32 _vertexNumber, int _vertexformat, u32 _structVertexSize, bbool _dynamic, dbEnumType _type )
{
    return m_VertexBufferManager.createVertexBuffer(_vertexNumber,  _vertexformat, _structVertexSize, _dynamic, _type );
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ITF_IndexBuffer* GFXAdapter::createIndexBuffer(u32 _indexNumber, bbool _dynamic)
{
    return m_VertexBufferManager.createIndexBuffer(_indexNumber, _dynamic);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void GFXAdapter::removeVertexBuffer(ITF_VertexBuffer* _VertexBuffer)
{
    if (_VertexBuffer)
        m_VertexBufferManager.removeVertexBuffer(_VertexBuffer);
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void GFXAdapter::removeIndexBuffer(ITF_IndexBuffer* _IndexBuffer)
{
    if (_IndexBuffer)
        m_VertexBufferManager.removeIndexBuffer(_IndexBuffer);
}

void GFXAdapter::removeVertexBufferDelay(   ITF_VertexBuffer* _VertexBuffer ,u32 _frameCount  )
{
    if (_VertexBuffer)
        m_VertexBufferManager.removeVertexBufferDelay(_VertexBuffer,_frameCount);
}

void GFXAdapter::removeIndexBufferDelay (   ITF_IndexBuffer* _IndexBuffer ,u32 _frameCount  )
{
    if (_IndexBuffer)
        m_VertexBufferManager.removeIndexBufferDelay(_IndexBuffer,_frameCount);
}

void GFXAdapter::cleanBufferEndOfFrame()
{
    m_VertexBufferManager.cleanEndOfFrame();

    /// Manage time life font.
    for (u32 index = 0;index<m_Fonts.size();++index)
    {
        Font* pFont = RESOURCE_MANAGER->getFont(m_Fonts[index]);
        if (pFont)
        {
            pFont->manageFontCache();
        }
    }
}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


Patch32Data::Patch32Data(int _vertexFormat, u32 _maxVertex)
{
    ITF_MemSet(this, 0, sizeof(*this));
    switch (_vertexFormat)
    {
    case VertexFormat_PCT:
#if PATCH32_WORKINGBUFF != 0
        vertexListPCT       = newAlloc(mId_Vertex,VertexPCT[_maxVertex]);
#endif // PATCH32_WORKINGBUFF
        vertexFormat        = VertexFormat_PCT;
        break;

    case VertexFormat_PatchAnim:
        vertexListPatch = newAlloc(mId_Vertex, VertexPatchAnim[_maxVertex]);
        vertexFormat = VertexFormat_PatchAnim;
        break;

    default:
#if PATCH32_WORKINGBUFF != 0
        vertexListPT        = newAlloc(mId_Vertex,VertexPT[_maxVertex]);
#endif // PATCH32_WORKINGBUFF
        vertexFormat        = VertexFormat_PTa;
        break;
    }

    maxVertexListSize   = _maxVertex;
}

Patch32Data::~Patch32Data()
{
    switch (vertexFormat)
    {
    case VertexFormat_PCT:
#if PATCH32_WORKINGBUFF != 0
        SF_DEL_ARRAY(vertexListPCT);
#endif // PATCH32_WORKINGBUFF
        break;

    case VertexFormat_PatchAnim:
        SF_DEL_ARRAY(vertexListPatch);
        break;

    default:
#if PATCH32_WORKINGBUFF != 0
        SF_DEL_ARRAY(vertexListPT);
#endif // PATCH32_WORKINGBUFF
        break;
    }
}

void Patch32Data::Init(ITF_Mesh    *_mesh, int _vertexFormat)
{
    vertexFormat                                        = _vertexFormat;
    vertexCount                                         = 0;
    mesh                                                = _mesh;
    mesh->m_ElementList[0].m_material.m_textureDiffuse.invalidateResourceId();
    mesh->m_ElementList[0].m_startIndex                 = 0;
    mesh->m_ElementList[0].m_count                      = 0;
}

#if PATCH32_WORKINGBUFF != 0
void Patch32Data::CopyToMesh()
{
    mesh->m_userBuffer = !mesh->m_userBuffer;
    u32 currentBuffer = mesh->m_userBuffer;
    void *pdata;

    GFX_ADAPTER->getVertexBufferManager().LockVertexBuffer(mesh->m_vtxBufferBuffered[currentBuffer],&pdata);
    
    switch(vertexFormat)
    {
    case VertexFormat_PCT:
        ITF_MemcpyWriteCombined(pdata,vertexListPCT,vertexCount*sizeof(VertexPCT));
        break;

    case VertexFormat_PatchAnim:
        ITF_MemcpyWriteCombined(pdata, vertexListPatch,vertexCount*sizeof(VertexPatchAnim));
        break;

    default:
        ITF_MemcpyWriteCombined(pdata,vertexListPT,vertexCount*sizeof(VertexPT));
        break;
    }

    GFX_ADAPTER->getVertexBufferManager().UnlockVertexBuffer(mesh->m_vtxBufferBuffered[currentBuffer]);
}
#else
void * Patch32Data::LockVBuff()
{
#ifndef VBMANAGER_USE_DYNAMICRING_VB
    mesh->m_userBuffer = !mesh->m_userBuffer;
    u32 currentBuffer = mesh->m_userBuffer;
    void *pdata;

    GFX_ADAPTER->getVertexBufferManager().LockVertexBuffer(mesh->m_vtxBufferBuffered[currentBuffer],&pdata);
    return pdata;
#else
    void *pdata;
    GFX_ADAPTER->getVertexBufferManager().LockVertexBuffer(mesh->m_vtxBuffer,&pdata);
    return pdata;
#endif // VBMANAGER_USE_DYNAMICRING_VB    
}

void Patch32Data::UnlockVBuff()
{
#ifndef VBMANAGER_USE_DYNAMICRING_VB
    GFX_ADAPTER->getVertexBufferManager().UnlockVertexBuffer(mesh->m_vtxBufferBuffered[mesh->m_userBuffer]);
#else
    GFX_ADAPTER->getVertexBufferManager().UnlockVertexBufferUsage(mesh->m_vtxBuffer, vertexCount * mesh->m_vtxBuffer->m_structVertexSize);
#endif // VBMANAGER_USE_DYNAMICRING_VB    
}
#endif // PATCH32_WORKINGBUFF

void ITF_Mesh::addElement()
{
    ITF_MeshElement elem;
    elem.m_material.m_textureDiffuse.invalidateResourceId();
    elem.m_startIndex                   = 0;
    elem.m_count                        = 0;
    elem.m_indexBuffer                  = NULL;
    m_ElementList.push_back(elem);
}

void ITF_Mesh::initStaticIndexMesh(int _vertexFormat, ITF_IndexBuffer *_staticIndex, u32 _bufferSize)
{
    if (!_bufferSize) _bufferSize = MAX_VERTEX_BY_MESH;

    switch(_vertexFormat)
    {
    case VertexFormat_PCT:
#ifndef VBMANAGER_USE_DYNAMICRING_VB
        for (u32 index =0;index<2;++index)
        {
            m_vtxBufferBuffered[index]   =  GFX_ADAPTER->createVertexBuffer(_bufferSize,
                VertexFormat_PCT, sizeof(VertexPCT),1, VB_T_MESH);
        }
#else
            m_vtxBuffer = GFX_ADAPTER->createVertexBuffer(_bufferSize,
                VertexFormat_PCT, sizeof(VertexPCT),1, VB_T_MESH);
#endif // VBMANAGER_USE_DYNAMICRING_VB
        m_type = meshType_PTC;
        break;

    case VertexFormat_PatchAnim:
#ifndef VBMANAGER_USE_DYNAMICRING_VB
        for (u32 index =0;index<2;++index)
        {
            m_vtxBufferBuffered[index]   =  GFX_ADAPTER->createVertexBuffer(_bufferSize,
                VertexFormat_PatchAnim, sizeof(VertexPatchAnim),1, VB_T_MESH);
        }
#else
            m_vtxBuffer = GFX_ADAPTER->createVertexBuffer(_bufferSize,
                VertexFormat_PatchAnim, sizeof(VertexPatchAnim),1, VB_T_MESH);
#endif // VBMANAGER_USE_DYNAMICRING_VB
        m_type = meshType_Patch;
        break;

    default: // VertexFormat_PTa
#ifndef VBMANAGER_USE_DYNAMICRING_VB
        for (u32 index =0;index<2;++index)
        {
            m_vtxBufferBuffered[index]   =  GFX_ADAPTER->createVertexBuffer(_bufferSize,
                VertexFormat_PTa, sizeof(VertexPT),1, VB_T_MESH);
        }
#else
            m_vtxBuffer = GFX_ADAPTER->createVertexBuffer(_bufferSize,
                VertexFormat_PTa, sizeof(VertexPT),1, VB_T_MESH);
#endif // VBMANAGER_USE_DYNAMICRING_VB        
        m_type = meshType_PTa;
        break;
    }

    addElement();
    m_ElementList[0].m_indexBuffer = _staticIndex;
}

void ITF_Mesh::initDynamicIndexedMesh(int _vertexFormat, ITF_IndexBuffer *_staticIndex, u32 _bufferSize)
{
    if (!_bufferSize) _bufferSize = MAX_VERTEX_BY_MESH;

    switch(_vertexFormat)
    {
    case VertexFormat_PCT:
#ifndef VBMANAGER_USE_DYNAMICRING_VB
        for (u32 index =0;index<2;++index)
        {
            m_vtxBufferBuffered[index]   =  GFX_ADAPTER->createVertexBuffer(_bufferSize,
                VertexFormat_PCT, sizeof(VertexPCT),1, VB_T_MESH);
        }
#else
        m_vtxBuffer = GFX_ADAPTER->createVertexBuffer(_bufferSize,
            VertexFormat_PCT, sizeof(VertexPCT),1, VB_T_MESH);
#endif // VBMANAGER_USE_DYNAMICRING_VB
        m_type = meshType_PTC;
        break;

    case VertexFormat_PatchAnim:
#ifndef VBMANAGER_USE_DYNAMICRING_VB
        for (u32 index =0;index<2;++index)
        {
            m_vtxBufferBuffered[index]   =  GFX_ADAPTER->createVertexBuffer(_bufferSize,
                VertexFormat_PatchAnim, sizeof(VertexPatchAnim),1, VB_T_MESH);
        }
#else
        m_vtxBuffer = GFX_ADAPTER->createVertexBuffer(_bufferSize,
            VertexFormat_PatchAnim, sizeof(VertexPatchAnim),1, VB_T_MESH);
#endif // VBMANAGER_USE_DYNAMICRING_VB
        m_type = meshType_Patch;
        break;

    default: // VertexFormat_PTa
#ifndef VBMANAGER_USE_DYNAMICRING_VB
        for (u32 index =0;index<2;++index)
        {
            m_vtxBufferBuffered[index]   =  GFX_ADAPTER->createVertexBuffer(_bufferSize,
                VertexFormat_PTa, sizeof(VertexPT),1, VB_T_MESH);
        }
#else
        m_vtxBuffer = GFX_ADAPTER->createVertexBuffer(_bufferSize,
            VertexFormat_PTa, sizeof(VertexPT),1, VB_T_MESH);
#endif // VBMANAGER_USE_DYNAMICRING_VB        
        m_type = meshType_PTa;
        break;
    }

    addElement();
    m_ElementList[0].m_indexBuffer = _staticIndex;
}

void ITF_Mesh::swapVBForDynamicMesh()
{
#ifndef VBMANAGER_USE_DYNAMICRING_VB
    m_userBuffer = !m_userBuffer;
#endif
}

ITF_VertexBuffer *ITF_Mesh::getCurrentVB()
{
#ifndef VBMANAGER_USE_DYNAMICRING_VB
    ITF_VertexBuffer * pVB = m_vtxBufferBuffered[m_userBuffer];
#else
    ITF_VertexBuffer * pVB = m_vtxBuffer;
#endif
    return pVB;
}

void ITF_Mesh::emptyIndexedMesh()
{
#ifndef VBMANAGER_USE_DYNAMICRING_VB
    for (u32 indexVertex = 0;indexVertex<2;++indexVertex)
    {
        if (m_vtxBufferBuffered[indexVertex])
        {
            GFX_ADAPTER->removeVertexBuffer(m_vtxBufferBuffered[indexVertex]);
            m_vtxBufferBuffered[indexVertex] = NULL;
        }
    }
#else
    GFX_ADAPTER->removeVertexBuffer(m_vtxBuffer);
    m_vtxBuffer = NULL;
#endif // VBMANAGER_USE_DYNAMICRING_VB
}


Texture* ITF_Mesh::getDefaultTexture() const
{
    for (u32 i = 0; i < m_ElementList.size(); i++)
    {
        ResourceID t = m_ElementList[i].m_material.m_textureDiffuse;
        if (t.isValidResourceId())
            return (Texture *)t.getResource();
    }
    return NULL;
}

bbool ITF_Mesh::isDataReady() const
{
    for (u32 i = 0; i < m_ElementList.size(); i++)
    {
        ResourceID t = m_ElementList[i].m_material.m_textureDiffuse;
        if (t.isValidResourceId())
        {
            Texture* pT = (Texture*)t.getResource();
            ITF_ASSERT(pT);
            if (pT && (!pT->isPhysicallyLoaded()))
                return bfalse;
        }
    }
    return btrue;
}


// Use max size to be compatible with software use
ITF_IndexBuffer* GFXAdapter::createInternalIB(u32 _hdiv, u32 _vdiv)
{
    u32 nbElements;
    if (_hdiv == 2 && _vdiv == 2)
    {
        nbElements= 32736/(_hdiv*_vdiv*6);
    } else
    {
        nbElements= MAX_INDEX_BY_ELEMENT/(_hdiv*_vdiv*6);
    }
    ITF_IndexBuffer* internalIdxBuffer = createIndexBuffer(  (_hdiv*_vdiv*6)*nbElements, bfalse);

    u16        *idxBuff;
    internalIdxBuffer->Lock((void **) &idxBuff);
    u16        * __restrict idx = idxBuff;
    u16 idxPos  = 0;
    u16 idxLine = (u16)(_hdiv+1);

    for (u32 k = 0 ; k < nbElements; k++)
    {
        for (u32 y=0; y<_vdiv; y++)
        {
            for (u32 x=0; x<_hdiv; x++, idxPos++)
            {
                *(idx++) = idxPos;
                *(idx++) = idxPos + 1;
                *(idx++) = idxPos + idxLine + 1;
                *(idx++) = idxPos;
                *(idx++) = idxPos + idxLine + 1;
                *(idx++) = idxPos + idxLine;
            }
            idxPos++;
        }
        idxPos += idxLine;
    }
    internalIdxBuffer->Unlock();
    return internalIdxBuffer;
}

ITF_VertexBuffer* GFXAdapter::createInternalVB(u32 _hdiv, u32 _vdiv)
{
    ITF_VertexBuffer* internalVtxBuffer = createVertexBuffer(  ((_hdiv + 1)*(_vdiv + 1))*NB_ELEMENT_IN_VERTEX_BUFFER,
    VertexFormat_PT, sizeof(VertexPT),bfalse, VB_T_INTERNAL);
    
    VertexPT    *pdata;
    internalVtxBuffer->Lock((void **) &pdata);
    VertexPT    * __restrict write = pdata;

    f32 divx = 1.f / _hdiv;
    f32 divy = 1.f / _vdiv;
    f32 UVu = 0.f;
    f32 UVv = 0.f;
    for (u32 k = 0 ; k < NB_ELEMENT_IN_VERTEX_BUFFER ; k++)
    {
        UVv = 0.f;
        for (u32 j = 0 ; j < (_vdiv + 1); j++)
        {
            UVu = 0.f;
            for (u32 i = 0 ; i < (_hdiv + 1) ; i++)
            {
                write->m_pos.m_x    = UVu;
                write->m_pos.m_y    = UVv;
                write->m_pos.m_z    = (f32)k;
                write->m_uv.m_x     = UVu;
                write->m_uv.m_y     = UVv;
                UVu  += divx;
                write++;
            }
            UVv += divy;
        }
    }
    internalVtxBuffer->Unlock();
    return internalVtxBuffer;
}

void GFXAdapter::createInternalBuffers()
{
    for (u32 i=0; i<bufferTypesNb; i++)
    {
        m_internalIdxBufferTab[i] = createInternalIB(bufferTypes[i][0], bufferTypes[i][1]);
        m_internalVtxBufferTab[i] = createInternalVB(bufferTypes[i][0], bufferTypes[i][1]);
    }
}

void GFXAdapter::removeInternalBuffers()
{
    for (u32 i=0; i<bufferTypesNb; i++)
    {
        if (m_internalIdxBufferTab && m_internalIdxBufferTab[i])
        {
            removeIndexBuffer(m_internalIdxBufferTab[i]);
            m_internalIdxBufferTab[i] = NULL;
        }
        if (m_internalVtxBufferTab && m_internalVtxBufferTab[i])
        {
            removeVertexBuffer(m_internalVtxBufferTab[i]);
            m_internalVtxBufferTab[i] = NULL;
        }
    }
}

ITF_IndexBuffer*    GFXAdapter::getIndexBuffer(u32 & _hdiv, u32 & _vdiv)
{
    for (u32 i=0; i<bufferTypesNb; i++)
    {
        if (bufferTypes[i][0] == _hdiv && bufferTypes[i][1] == _vdiv)
        {
            return m_internalIdxBufferTab[i];
        }
    }
#ifndef ITF_WII
    ITF_ASSERT(0);
#endif // ITF_WII
    _hdiv = bufferTypes[0][0];
    _vdiv = bufferTypes[0][1];
    return m_internalIdxBufferTab[0];
}

ITF_VertexBuffer*   GFXAdapter::getVertexBuffer(u32 & _hdiv, u32 & _vdiv)
{
    for (u32 i=0; i<bufferTypesNb; i++)
    {
        if (bufferTypes[i][0] == _hdiv && bufferTypes[i][1] == _vdiv)
        {
            return m_internalVtxBufferTab[i];
        }
    }
    ITF_ASSERT(0);
    _hdiv = bufferTypes[0][0];
    _vdiv = bufferTypes[0][1];
    return m_internalVtxBufferTab[0];
}

void FillSpriteIndexBuffer(ITF_IndexBuffer* _indexbuffer, u32 _numberSprite)
{
    /// Fill Element index Buffer.
    u32 i;
    u16* index;
    _indexbuffer->Lock((void**)&index);
    u16 vertex = 0;

    for (i = 0; i < _numberSprite ; i++)
    {
        index[i * 6 + 0] = vertex + 0;
        index[i * 6 + 1] = vertex + 1;
        index[i * 6 + 2] = vertex + 2;
        index[i * 6 + 3] = vertex + 2;
        index[i * 6 + 4] = vertex + 3;
        index[i * 6 + 5] = vertex + 0;
        vertex += 4;
    }

    _indexbuffer->Unlock();
}

ITF_IndexBuffer* GFXAdapter::needSpriteIndexBuffer(u32 _numberIndex)
{
    /// Create.
    if (!m_internalSpritesIndexBuffer)
    {
        m_internalSpritesIndexBuffer = createIndexBuffer(  _numberIndex, bfalse);
        m_spriteIndexBufferSize = _numberIndex;
        FillSpriteIndexBuffer(m_internalSpritesIndexBuffer, _numberIndex / 6);
        return m_internalSpritesIndexBuffer;
    }
    
    /// Resize.
    if ( _numberIndex > m_spriteIndexBufferSize )
    {
        /// destroy.
        releaseIndexBuffer(m_internalSpritesIndexBuffer);
        /// recreate with good size.
        m_internalSpritesIndexBuffer->createIndexBuffer(  _numberIndex, bfalse);
        
        m_spriteIndexBufferSize = _numberIndex;
    
        FillSpriteIndexBuffer(m_internalSpritesIndexBuffer, _numberIndex / 6);
    }

    return m_internalSpritesIndexBuffer;
}

void  GFXAdapter::stopScreenCapture()
{
#ifdef ITF_SUPPORT_VIDEOCAPTURE
    if (m_videoCapture)
        m_videoCapture->stop();
#endif //ITF_SUPPORT_VIDEOCAPTURE
}

void  GFXAdapter::cancelScreenCapture()
{
#ifdef ITF_SUPPORT_VIDEOCAPTURE
    if (m_videoCapture)
        m_videoCapture->cancel();
#endif //ITF_SUPPORT_VIDEOCAPTURE
}

void GFX_Zlist::AddPrimitiveInZList(void* _pPrimitive, PRIMITIVETYPE _type, Texture* _texture, f32 _depth,const ObjectRef& _ref)
{
    f32 zordering = _depth;
    
    // reject same AFX:
    if (_type == GFX_ITF_AFTERFX)
    {
        if (ZList_Node* node = ZList_find(_type))
        {
            if (node->m_depth > _depth && node->m_depth < _depth + 0.000065535f)
                return;
        }
    }

    // compute and add unique epsilon from primitive adress
    // to avoid egality in z zlist.
    f32 epsilon = f32(uPtr(_pPrimitive) & 0x0000ffff) * 0.000000001f;
    zordering += epsilon;

    ZList_Node	*pNode;
    pNode = ZList_AddNode(zordering);

    if (!pNode) return;

    pNode->pPrimitiveInfo.mp_Primitive = _pPrimitive;
    pNode->pPrimitiveInfo.m_type = _type;
    pNode->pPrimitiveInfo.m_Texture = _texture;
    pNode->m_ref = _ref;
    // ITF_ASSERT_MSG(_ref.isValid(), "Provide a valid ref so we can debug...");
}

void GFX_Zlist::transferNodeTOZList(const ZList_Node& _node)
{
    ZList_Node* pNewNode = ZList_AddNode(_node.m_depth);
    pNewNode->pPrimitiveInfo = _node.pPrimitiveInfo;
}

ZList_Node* GFX_Zlist::ZList_AddNode(f32 _zordering)
{
    /// Must Expand ?
    if (m_NumberEntry + 1 > m_currentMax )
        ZList_expandsize();

    ZList_Node* pNewNode = &m_zListBufferNode[m_NumberEntry];
    m_reorderingTable[m_NumberEntry] = pNewNode;

    pNewNode->m_depth = _zordering;
    
    m_NumberEntry++;

    return pNewNode;
}

void GFX_Zlist::ZList_reset()
{
    m_NumberEntry = 0;
}

void GFX_Zlist::ZList_init()
{
    ZList_expandsize();
}

ZList_Node* GFX_Zlist::ZList_find(PRIMITIVETYPE _type)
{
    ZList_Node* node;
    for (u32 i=0; i < m_NumberEntry; i++)
    {
        node = &m_zListBufferNode[i];
        if (node->pPrimitiveInfo.m_type == _type)
            return node;
    }
    
    return NULL;
}

void GFX_Zlist::ZList_remove(PRIMITIVETYPE _type, void* _primitive)
{
    for (u32 i=0; i < getNumberEntry(); i++)
    {
        ZList_Node& node = getNodeAt(i);
        if (node.pPrimitiveInfo.mp_Primitive == _primitive)
        {
            node.pPrimitiveInfo.m_type = GFX_ITF_REMOVED;
            return;
        }
    }
}

void GFXAdapter::removePrimitiveFromRenderLists(PRIMITIVETYPE _type, void* _primitive)
{
    m_RenderZlist.ZList_remove(_type, _primitive);
    m_RenderZlistRTarget.ZList_remove(_type, _primitive);
}


void GFX_Zlist::removeNode(ZList_Node* _node)
{
    if (!m_NumberEntry) return;

    ZList_Node* node;
    for (u32 i = 0; i < m_NumberEntry; i++)
    {
        node = &m_zListBufferNode[i];
        if (_node == node)
        {
            _node->pPrimitiveInfo.m_type = GFX_ITF_REMOVED;
            break;
        }
    }
}

struct ListNodePtrDepthLess
{
    bool operator()(ZList_Node * _r, ZList_Node * _l)
    {
        return _r->m_depth < _l->m_depth;
    }
};

void GFX_Zlist::sort()
{
    std::sort(m_reorderingTable, m_reorderingTable+m_NumberEntry, ListNodePtrDepthLess());
}

void GFX_Zlist::ZList_expandsize()
{
    m_currentMax += m_maxSegNode;
    m_zListBufferNode.resize(m_currentMax);

    if (m_reorderingTable)
        delete[] m_reorderingTable;

    m_reorderingTable = newAlloc(mId_GfxAdapter, ZList_Node*[m_currentMax]);
    for (u32 i = 0; i < m_currentMax;i++)
    {
        m_reorderingTable[i] = &m_zListBufferNode[i];
    }
}

void GFXAdapter::drawZlistPrimitives(GFX_Zlist* _zlist)
{
    Vec3d lastPosWidth;
    f32 width = 0.0f;
	
	ZList_Node* pCurNode = NULL;
#ifndef ITF_FINAL
#ifdef ITF_WINDOWS
    bbool allowRendering = !m_bPickingMode;
#else
    bbool allowRendering = btrue;
#endif //ITF_WINDOWS
#endif // ITF_FINAL

    for (u32 i = 0; i < _zlist->getNumberEntry(); i++)
    {   
        pCurNode = _zlist->getOrderNodeAt(i);

        PROFILER_DRAWGPU(pCurNode->m_ref);

        m_currentDrawZ = pCurNode->m_depth; // inform the renderer the actual z position in world.

        switch(pCurNode->pPrimitiveInfo.m_type)
        {
        case GFX_ITF_REMOVED:
            // skip.
            break;
        case GFX_TRIANGLE_STRIP:
            SetTexture(0, pCurNode->pPrimitiveInfo.m_Texture);
            drawStrip((Strip&)pCurNode->pPrimitiveInfo.mp_Primitive, pCurNode->pPrimitiveInfo.m_Texture);
            break;
        case GFX_TRIANGLE_FAN:
            SetTexture(0, pCurNode->pPrimitiveInfo.m_Texture);
            drawFan((Fan&)pCurNode->pPrimitiveInfo.mp_Primitive, pCurNode->pPrimitiveInfo.m_Texture);
            break;
        case GFX_TRIANGLES:
            SetTexture(0, pCurNode->pPrimitiveInfo.m_Texture);
            drawTriangles((Triangle&)pCurNode->pPrimitiveInfo.mp_Primitive, pCurNode->pPrimitiveInfo.m_Texture);
            break;
        case GFX_ITF_MESH:
        {
            ITF_Mesh* pMesh = (ITF_Mesh*)pCurNode->pPrimitiveInfo.mp_Primitive;
            setFogColor(pMesh->m_colorFog);
            setGlobalColor(pMesh->m_globalColor);
            switch(pMesh->m_type)
            {
            case ITF_Mesh::meshType_PT:
            case ITF_Mesh::meshType_PTa:
                drawMeshPT(*pMesh, bfalse);
                break;
            default:
                drawMesh(*pMesh, bfalse);
                break;
            }
            setGlobalColor(Color::white());
            setFogColor(Color::zero());
        } 
        break;
        case GFX_ITF_ANIMSCENE:
        {
            AnimMeshScene* pAnimMeshScene  = (AnimMeshScene*)pCurNode->pPrimitiveInfo.mp_Primitive;
            pAnimMeshScene->m_pAnimDraw.Draw(pAnimMeshScene);
            pAnimMeshScene->m_AnimInfo.m_renderPass++;
            break;
        }
        case GFX_ITF_TRAIL:
        {
            Trail* pTrail  = (Trail*)pCurNode->pPrimitiveInfo.mp_Primitive;
            pTrail->directDraw();
            break;
        }
        case GFX_ITF_STARTRENDERTEX:
#ifndef ITF_FINAL
            if (isShowAfterFx() && allowRendering)
#endif // ITF_FINAL
            {
                AFTERFX* afx = (AFTERFX*)pCurNode->pPrimitiveInfo.mp_Primitive;
#ifdef TESTAFX
                GFX_QUALITY quality = GFX_QUALITY_LOW;
                if (afx->getType() == AFX_simpleBlend)
                    quality = GFX_QUALITY_FULL;
                AFTERFX_startRenderTarget(quality, afx->getColorTarget());
#else
                AFTERFX_startRenderTarget(afx->getColorTarget());
#endif
            }
            break;

        case GFX_ITF_AFTERFX:
#ifndef ITF_FINAL
            if (isShowAfterFx() && allowRendering)
#endif // ITF_FINAL
            {
                AFTERFX* afx = (AFTERFX*)pCurNode->pPrimitiveInfo.mp_Primitive;
                bbool nobackbuffercopy = bfalse;
                if (afx->getType() == AFX_Fade || afx->getType() == AFX_BorderBright)
                    nobackbuffercopy = btrue;
#if defined(ITF_PS3) || defined(ITF_WII)
                // On PS3, we can directly sample the backbuffer
                // On WII, we can avoid the last backbuffer copy
                if (afx->getType() == AFX_Glow)
                    nobackbuffercopy = btrue;
#endif // defined(ITF_PS3) || defined(ITF_WII)
                
                prepareAfterFx(nobackbuffercopy);
                AFTERFX_drawOne(afx);
                endAfterfx(nobackbuffercopy, (GFX_BLENDMODE)afx->getCustomVal());
            }
            break;

        case GFX_ITF_AFTERFXGROUP:
#ifndef ITF_FINAL
            if (isShowAfterFx() && allowRendering)             
#endif // ITF_FINAL
            {
                AFX_Group* group = (AFX_Group*)pCurNode->pPrimitiveInfo.mp_Primitive;
                AFTERFX_draw(group);
            }
            break;
        case GFX_ITF_3DQUAD:
            {
                GFX_3DQUAD* quad = (GFX_3DQUAD*)pCurNode->pPrimitiveInfo.mp_Primitive;
                setFogColor(quad->m_fogColor);
                setGlobalColor(Color::white());
                SetTexture(0, pCurNode->pPrimitiveInfo.m_Texture);
                draw3DQuad(quad);
                SetTextureBind(0, 0);
                setFogColor(Color::zero());
                break;
            }
        case GFX_ITF_FLUID:
            {
                EdgeFluidLevel* pEdgeFluidLevel  = (EdgeFluidLevel*)pCurNode->pPrimitiveInfo.mp_Primitive;
                setGlobalColor(pEdgeFluidLevel->m_color);
                pEdgeFluidLevel->m_edgeFluid->Draw(&pEdgeFluidLevel->m_data, pEdgeFluidLevel->m_pivot, pEdgeFluidLevel->m_layerInfos);
                break;
            }
        case GFX_ITF_PROCEDURALPATCH:
            {
                ProceduralPatch* pProceduralPatch = (ProceduralPatch*)pCurNode->pPrimitiveInfo.mp_Primitive;
                pProceduralPatch->DrawShader();
                break;
            }
        case GFX_ITF_BEZIERPATCH:
            {
                BezierPatch* pBezierPatch = (BezierPatch*)pCurNode->pPrimitiveInfo.mp_Primitive;
                setFogColor(pBezierPatch->getFogColor());
                setGlobalColor(Color::white());
                pBezierPatch->DrawShader();
                setFogColor(Color::zero());
                break;
            }
        case GFX_ITF_SPLINE:
            {
                Spline_Info* pSpline  = (Spline_Info*)pCurNode->pPrimitiveInfo.mp_Primitive;
                setGlobalColor(pSpline->m_color);
                drawSpline((GMatrix44 *)&pSpline->m_mat, pSpline->m_texture, &pSpline->m_spline, pSpline->m_pos, pSpline->m_height);
                break;
            }
        case GFX_ITF_FONT:
            {
                FontCall* font = (FontCall*)pCurNode->pPrimitiveInfo.mp_Primitive;

                //Texture* backGroundTexture = font->getBackGroundTexture();
                /*if (font->getUseBackground())
                {
                    const Vec3d offset = font->getTextureOffset2D().to3d();
                    const Vec3d& pos = font->getPosition() + offset;
                    const f32 scale =  font->getScale();

                    f32 boxWidth;
                    if (backGroundTexture)
                        boxWidth = fontcall.getBoxWidth() - offset.m_x + (10.0f * scale);
                    else
                        boxWidth = fontcall.getBoxWidth();

                    drawQuad2D(pos.m_x, pos.m_y, boxWidth, fontcall.getBoxHeight(), pos.m_z, fontcall.getBackGroundColour(), backGroundTexture, &fontcall.getBackGroundTextureStartUV(), &fontcall.getBackGroundTextureEndUV());
                }*/
                if (!font->isAppend())
                {
                    width = drawTextInternal(*font);
                    lastPosWidth = font->getPosition();
                }
                else
                {
                    width = drawTextInternalAppend(*font,lastPosWidth);//sum the appends
                }

                lastPosWidth.m_x += width;

			    SF_DEL(font);

                break;
		    }
        default:
            {
                ITF_ASSERT_CRASH(0, "Check that this is acceptable to get this primitive type here");
                break;
            }
		}
    }
}

void GFXAdapter::addPrimitiveInZList(void* _pPrimitive, PRIMITIVETYPE _type, Texture* _texture, f32 _depth, bbool _target,const ObjectRef& _ref)
{
#ifdef AFTERFXLIST
    if (_type == GFX_ITF_AFTERFX)
        m_AfterFxZlist.AddPrimitiveInZList(_pPrimitive, _type, _texture, _depth,_ref);
    else
#endif    
    {
        if (_target && isUseInternalRT())
            m_RenderZlistRTarget.AddPrimitiveInZList(_pPrimitive, _type, _texture, _depth,_ref);
        else
            m_RenderZlist.AddPrimitiveInZList(_pPrimitive, _type, _texture, _depth,_ref);

    }
}

void GFXAdapter::AFTERFX_drawOne(AFTERFX* _afx)
{
    switch((AFTERFX_Type)_afx->getType())
    {
        case AFX_Blur:
        AFTERFX_BigBlur(_afx->getParamfAt(0),0, btrue, GFX_QUALITY_MEDIUM);// + sinf(SYSTEM_ADAPTER->getTime())*v, 0, btrue);// 0.008f
        break;
        case AFX_Glow:
        AFTERFX_Glow(_afx->getParamfAt(0), _afx->getParamfAt(1), _afx->getParamfAt(2), (GFX_BLENDMODE)_afx->getCustomVal());
        break;
        case AFX_objectsGlow:
        AFTERFX_objectsGlow(_afx->getParamfAt(0));
        break;
        case AFX_Remanence:
        AFTERFX_Remanence(_afx->getParamfAt(0), _afx->getParamfAt(1), _afx->getParamfAt(2));
        break;
        case AFX_DOF:
        break;
        case AFX_RayCenter:
        AFTERFX_RayCenter(_afx->getParamfAt(0), _afx->getParamfAt(1), _afx->getParamfAt(2), _afx->getParamfAt(3), _afx->getParamfAt(4), _afx->getParamfAt(5), _afx->getParamfAt(6));
        break;
        case AFX_Fade:
        AFTERFX_colorScreen(_afx->getParamcAt(0), _afx->getCustomVal());
        break;
        case AFX_ColorSetting:
        AFTERFX_ColorSetting(_afx->getParamfAt(0), _afx->getParamfAt(1), _afx->getParamfAt(2), _afx->getParamfAt(3));
        break;
        case AFX_BorderBright:
        AFTERFX_borderBright(_afx->getParamfAt(0), _afx->getParamfAt(1), _afx->getParamcAt(0), _afx->getCustomVal());
        break;
        case AFX_ColorRemap:
        AFTERFX_ColorRemap(_afx->getTexture(0));
        break;
        case AFX_ColorLevels:
        AFTERFX_ColorLevels(_afx->getParamfAt(0), _afx->getParamfAt(1), _afx->getParamfAt(2), _afx->getParamfAt(3), _afx->getParamfAt(4));
        break;
        case AFX_Bright:
        AFTERFX_byTechParams(4, _afx->getParamfAt(0), _afx->getParamfAt(1), _afx->getParamfAt(2), _afx->getParamfAt(3));
        break;
        case AFX_ToneMap:
        AFTERFX_byTechParams(5, _afx->getParamfAt(0), _afx->getParamfAt(1), _afx->getParamfAt(2), _afx->getParamfAt(3));
        break;
        case AFX_AddSceneAndMul:
        AFTERFX_addSceneAndMul(_afx->getParamfAt(0));
        break;
        case AFX_simpleBlend:
        // no render for this afx.
           break;
        case AFX_zoomCenter:
        AFTERFX_zoomCenter(_afx->getParamfAt(0), _afx->getParamvAt(0) );
        break;
        default:
        case AFX_None:
        break;
    }
}

void GFXAdapter::AFTERFX_colorScreen(Color _color, i32 _blendMode)
{
    f32 W = (f32)getScreenWidth();
    f32 H = (f32)getScreenHeight();

    depthTest(0);
    depthMask(0);

	if (_blendMode < 2)
		_blendMode = 2;

    setGfxMatDefault();
	setAlphaBlend((GFX_BLENDMODE)_blendMode);
    drawScreenQuad(0.f, 0.f, W, H, 0.f, _color.getAsU32(), btrue);
}

void GFXAdapter::AFTERFX_borderBright(f32 _factor, f32 _attenuation, Color _color, i32 _blendMode)
{
    const f32 W = (f32)getScreenWidth();
    const f32 H = (f32)getScreenHeight();

#ifdef VBMANAGER_USE_DYNAMICRING_VB
    const u32 div = 16;
    ITF_VertexBuffer vbuffer;
    VertexPC* vertex;
    getVertexBufferManager().LockDynamicRingVB((6*div*div) * sizeof(VertexPC), &vbuffer, (void**)&vertex);
#else // VBMANAGER_USE_DYNAMICRING_VB
    const u32 div = 32;
    VertexPC vertex[6*32*32];
#endif // VBMANAGER_USE_DYNAMICRING_VB

    const u32 colorparam = (_color == Color::zero()) ? COLOR_BLACK : _color.getAsU32();

    _attenuation = Clamp(_attenuation, 0.f, 1.f);
    const f32 maxAlpha = 255.f * _attenuation;

    const f32 Wi = W / div;
	const f32 Hi = H / div;

    const f32 Wby2 = W * 0.5f;
    const f32 Hby2 = H * 0.5f;
    const f32 twoByW = f32_Inv(Wby2);
    const f32 twoByH = f32_Inv(Hby2);

    // v0 --- v1/v3
    // |     /   |
    // |    /    |
    // |   /     |
    // v2/v5 --- v4 
    i32 vcount = 0;
    f32 Wb = 0.0f;
    u32 countX = div;
    while(countX--)
	{
		f32 Hb = 0.0f;
		u32 countY = div;
		while(countY--)
		{
			f32 X = (Wb - Wby2) * twoByW;
            f32 Y = (Hb - Hby2) * twoByH;
			Y *= W/H;
			X = Clamp((X * X + Y * Y) * _factor, 0.f, 1.f);

            u32 color = ((i32)(X * maxAlpha)) << COLOR_SHIFT_ALPHA;
            color += colorparam & COLOR_RGB_MASK;

            vertex[vcount].setData(Vec3d(Wb, Hb, 0.f), color);

            //----

			X = (Wb + Wi - Wby2) * twoByW;
            Y = (Hb - Hby2) * twoByH;
			Y *= W/H;
			X = Clamp((X * X + Y * Y) * _factor, 0.f, 1.f);

            color = ((i32)(X * maxAlpha)) << COLOR_SHIFT_ALPHA;
            color += colorparam & COLOR_RGB_MASK;

            vertex[vcount+1].setData(Vec3d(Wb+Wi, Hb, 0.f), color);
            vertex[vcount+3].setData(Vec3d(Wb+Wi, Hb, 0.f), color);

            //----

			X = (Wb - Wby2) * twoByW;
            Y = (Hb + Hi - Hby2) * twoByH;
			Y *= W/ H;
			X = Clamp((X * X + Y * Y) * _factor, 0.f, 1.f);

            color = ((i32)(X * maxAlpha)) << COLOR_SHIFT_ALPHA;
            color += colorparam & COLOR_RGB_MASK;

            vertex[vcount+2].setData(Vec3d(Wb, Hb+Hi, 0.f), color);
			vertex[vcount+5].setData(Vec3d(Wb, Hb+Hi, 0.f), color);
            
            //----

            X = (Wb + Wi - Wby2) * twoByW;
            Y = (Hb + Hi - Hby2) * twoByH;
			Y *= W / H;
			X = Clamp((X * X + Y * Y) * _factor, 0.f, 1.f);

            color = ((i32)(X * maxAlpha)) << COLOR_SHIFT_ALPHA;
            color += colorparam & COLOR_RGB_MASK;

            vertex[vcount+4].setData(Vec3d(Wb+Wi, Hb+Hi, 0.f), color);

            //----

			vcount+=6;
			Hb += Hi;
		}

		Wb += Wi;
    }

#ifdef VBMANAGER_USE_DYNAMICRING_VB
    getVertexBufferManager().UnlockDynamicRingVB(&vbuffer);
    vbuffer.mp_VertexBuffer = NULL; // avoid vb destruction by object destructor.
#endif // VBMANAGER_USE_DYNAMICRING_VB

    depthTest(0);
    depthMask(0);

    setGfxMatDefault();
    setAlphaBlend((GFX_BLENDMODE)_blendMode);

    setVertexFormat(VertexFormat_PC);
    DrawPrimitive(GFX_TRIANGLES, (void*)vertex, 6*div*div);
}

void GFXAdapter::AFTERFX_processObjectGlow(f32 _factor)
{
    AFTERFX *newAFX = newAlloc(mId_GfxAdapter, AFTERFX);
    newAFX->setType(AFX_objectsGlow);
    newAFX->setParamfAt(0, _factor);
    GFX_ADAPTER->init2DRender();
    GFX_ADAPTER->AFTERFX_drawOne(newAFX);
    SF_DEL(newAFX);
    CAMERA->apply();
}

void GFXAdapter::postGfxMaterial(const GFX_MATERIAL& _gfxMat)
{
    switch (_gfxMat.m_matType)
    {
        case GFX_MAT_GLOW:
        {
            /// compute size
            f32 sizepixel = _gfxMat.m_matParams.getfParamsAt(0);
            f32 factor = 1.f/f32_Abs(m_currentDrawZ - CAMERA->getPos().m_z) * sizepixel;
            AFTERFX_processObjectGlow(factor);
            break;
        }

    default:
        break;
    }

    m_drawFlag = 0;
}

i32 GFXAdapter::getDebugFontSize()
{
/*    if (getScreenWidth() > 1250)
        return 25;*/
    return 15;
}

i32 GFXAdapter::getBigDebugFontSize()
{
  /*  if (getScreenWidth() > 1250)
        return 30;*/
    return 20;
}

f32 GFXAdapter::computeFogFactor()
{
    f32 factor;
    f32 startFog = 100.f;
    f32 endFog = 600.f;

    //if (GFX_FOG == FOG_T_LINEAR)
    {
        f32 camDist = f32_Abs(CAMERA->getPos().m_z - m_currentDrawZ);
        factor = (endFog - camDist)/(endFog - startFog);
        factor = Clamp(factor,0.f, 1.f);
    }

    return factor;
}

void GFXAdapter::setGfxMatAlphaFade(const GFX_MATERIAL& _gfxMat)
{
	setGfxMatDefault();

	Color val = getGFXGlobalColor();
	f32 currentalpha = val.getAlpha();

	f32 dmin = _gfxMat.m_matParams.getfParamsAt(0);
	f32 dmax = _gfxMat.m_matParams.getfParamsAt(1);

	f32 coeff = 1.0f;
	f32 dist = f32_Abs(CAMERA->getPos().m_z - m_currentDrawZ);

	i32 type = _gfxMat.m_matParams.getiParamsAt(0);

	/// compute pos in range 0 -> 1
	f32 d = dmax - dmin;
	f32 d1 = dist - dmin;
	
    coeff =  1.f - ((d - d1)/d);
	   
	switch(type)
	{
    case GFX_MaterialParams::AlphaFade_Distance:
        break;
    case GFX_MaterialParams::AlphaFade_ExpDistance:
    case GFX_MaterialParams::AlphaFade_ExpSquaredDistance:
        {
		    f32 density = _gfxMat.m_matParams.getfParamsAt(2);
		    f32 dd = 1.f / coeff;
		    f32 co = (dd * density);
            if (type == GFX_MaterialParams::AlphaFade_ExpSquaredDistance)
			    co *= co;
		    coeff =  expf(-co);
        } break;
    case GFX_MaterialParams::AlphaFade_Constant:
        coeff = _gfxMat.m_matParams.getfParamsAt(2);
        break;
	}

	/// clip: do not > original alpha.
	if (coeff > 1.f)
	   coeff = 1.f;

	currentalpha *= coeff;

	val.m_a = currentalpha;
	setGlobalColor(val);
}

void GFXAdapter::setTextureAdressingMode(u32 _samp, GFX_TEXADRESSMODE _u, GFX_TEXADRESSMODE _v)
{
    setSamplerState(_samp, GFXSAMP_ADDRESSU ,_u);
    setSamplerState(_samp, GFXSAMP_ADDRESSV ,_v);
}

#ifndef ITF_CONSOLE_FINAL
DBGLine* GFXAdapter::getDBGLineBuffer(   u32 _count )
{
    return &m_DBGLines.incrSize(_count);
}

DBG3DLine* GFXAdapter::getDBG3DLineBuffer(   u32 _count )
{
    return &m_DBG3DLines.incrSize(_count);
}
#endif // ITF_CONSOLE_FINAL

const char *GFXAdapter::computeSafeFrame(Vec2d &_margin, Vec2d &_safeSize)
{
    f32 safeFrameH = 85.f;
    f32 safeFrameV = 85.f;
    const char *safeName = "";

    switch((eSafeFrame)m_safeFrameMode)
    {
    default:
    case SF_NOSAFEFRAME:
        break;
    case SF_WII_4x3:
        safeFrameH = 80.8f;
        safeFrameV = 84.4f;
        safeName = "WII 4/3 safe Frame";
        break;
    case SF_WII_16x9:
        safeFrameH = 87.f;
        safeFrameV = 87.f;
        safeName = "WII 16/9 safe Frame";
        break;
    case SF_XBOX360:
        safeFrameH = 85.f;
        safeFrameV = 85.f;
        safeName = "X360 safe Frame";
        break;
    case SF_PS3:
        safeFrameH = 85.f;
        safeFrameV = 85.f;
        safeName = "PS3 safe Frame";
        break;
    case SF_MINIMUM:
        safeFrameH = 80.8f;
        safeFrameV = 84.4f;
        safeName = "Minimum safe Frame";
        break;
    };

    f32 screenx = (f32)getScreenWidth();
    f32 screeny = (f32)getScreenHeight();

    f32 sizex = screenx * safeFrameH / 100.f;
    f32 sizey = screeny * safeFrameV / 100.f;

    f32 posy = (screeny - sizey) * 0.5f;
    f32 posx = (screenx - sizex) * 0.5f;

    _margin.set(posx,posy);
    _safeSize.set(sizex,sizey);

    return safeName;
}

#ifndef ITF_CONSOLE_FINAL
void GFXAdapter::drawSafeFrame()
{
    if(CONFIG && CONFIG->m_noSafeFrame)
        return;

    VertexPC v[4];

    Vec2d margin, safeSize;
    const char *safeName = computeSafeFrame(margin, safeSize);

    f32 screeny = (f32)getScreenHeight();

    f32 width = safeSize.m_x;
    f32 height = safeSize.m_y;
    f32 posx = margin.m_x;
    f32 posy = margin.m_y;
    u32 color = COLOR_RED;
    f32 lsize = 2.f;

    const f32 py = screeny - posy - height;

    setVertexFormat(VertexFormat_PC);

    v[0].m_color = color;
    v[1].m_color = color;
    v[2].m_color = color;
    v[3].m_color = color;

    v[0].m_pos.m_x = posx;
    v[0].m_pos.m_y = py;
    v[1].m_pos.m_x = posx;
    v[1].m_pos.m_y = py + lsize;
    v[2].m_pos.m_x = posx + width;
    v[2].m_pos.m_y = py;
    v[3].m_pos.m_x = posx + width;
    v[3].m_pos.m_y = py + lsize;
    DrawPrimitive(GFX_TRIANGLE_STRIP, (void*)v, 4);

    v[0].m_pos.m_x = posx;
    v[0].m_pos.m_y = py + height;
    v[1].m_pos.m_x = posx;
    v[1].m_pos.m_y = py + height + lsize;
    v[2].m_pos.m_x = posx + width;
    v[2].m_pos.m_y = py + height;
    v[3].m_pos.m_x = posx + width;
    v[3].m_pos.m_y = py + height + lsize;
    DrawPrimitive(GFX_TRIANGLE_STRIP, (void*)v, 4);

    v[0].m_pos.m_x = posx;
    v[0].m_pos.m_y = py;
    v[1].m_pos.m_x = posx;
    v[1].m_pos.m_y = py + height;
    v[2].m_pos.m_x = posx + lsize;
    v[2].m_pos.m_y = py;
    v[3].m_pos.m_x = posx + lsize;
    v[3].m_pos.m_y = py + height;
    DrawPrimitive(GFX_TRIANGLE_STRIP, (void*)v, 4);

    v[0].m_pos.m_x = posx + width;
    v[0].m_pos.m_y = py;
    v[1].m_pos.m_x = posx + width;
    v[1].m_pos.m_y = py + height;
    v[2].m_pos.m_x = posx + width + lsize;
    v[2].m_pos.m_y = py;
    v[3].m_pos.m_x = posx + width + lsize;
    v[3].m_pos.m_y = py + height;
    DrawPrimitive(GFX_TRIANGLE_STRIP, (void*)v, 4);

#ifdef ITF_SUPPORT_CHEAT
    drawDBGText(safeName, posx + 9 , posy + 9, 0.f, 0.f, 0.f, bfalse);
    drawDBGText(safeName, posx + 8 , posy + 8, 1.f, 1.f, 0.f, bfalse);
#endif // ITF_SUPPORT_CHEAT
    m_drawSafeFrameRequested = bfalse;
}
#endif // ITF_CONSOLE_FINAL

} // namespace ITF


