#include "precompiled_GFXAdapter_GCM.h"

// PS3
#include <algorithm>
#include <fastmath.h>
#include <sys/memory.h>
#include <sysutil/sysutil_sysparam.h>
#include <sys/paths.h>
#include <cell/dbgfont.h>

// Engine
#include "adapters/GFXAdapter_GCM/GFXAdapter_GCM.h"
#include "engine/rasters/RastersManager.h"
#include "adapters/FileManager_ITF/FileManager_ITF.h"
#include "core/AdaptersInterfaces/SystemAdapter.h"
#include "engine/resources/ResourceManager.h"
#include "core/file/FileServer.h"
#include "engine/display/Camera.h"
#include "engine/display/View.h"

// GCM package
#include "VRAMAllocator_GCM.h"
#include "ShaderProgram_GCM.h"
#include "Technique_GCM.h"
#include "Texture_GCM.h"
#include "VertexBuffer_GCM.h"
#include "IndexBuffer_GCM.h"

#ifdef _DEBUG
// uncomment to debug rsx crashes (sync on each drawcall)
// #define GCM_SYNCH_DRAWCALLS
#endif

#ifdef GCM_SYNCH_DRAWCALLS
#define DEBUG_SYNCH_RSX() m_rdrCtxt.gcmCtxt->Finish(m_finishCount++)
#else
#define DEBUG_SYNCH_RSX() do {} while (0)
#endif

namespace ITF
{

    DECLARE_RASTER(drawMesh, RasterGroup_Misc, Color::red());
    DECLARE_RASTER(drawMeshPT, RasterGroup_Misc, Color::pink());

void GFXAdapter_GCM::setRenderTarget(const GCM::Surface * _rtSurface)
{
    ITF_ASSERT(_rtSurface!=NULL);
    if (m_currentRenderTarget != _rtSurface)
    {
        m_currentRenderTarget = _rtSurface;
        m_rdrCtxt.gcmCtxt->SetSurface(m_currentRenderTarget);
    }
    else
    {
        // nothing to do -> already set
    }
}

//----------------------------------------------------------------------------//

void GFXAdapter_GCM::setCamera(float _x, float _y, float _z, Camera* _cam)
{
    GFXAdapter::setCamera(_x, _y, _z, _cam);

    ITF_ASSERT(!_cam->isBiased()); // Right now, bias is only editor

    f32 nearPlane   = 1.f;
    f32 farPlane    = 1000.f;
    
    Vec2d topLeft = Vec2d::Zero;
    Vec2d botRight = Vec2d::One;
    nearPlane   = _cam->m_near;
    farPlane    = _cam->m_far;
    topLeft     = _cam->m_topLeft;
    botRight    = _cam->m_bottomRight;

    _cam->forcePosition(Vec3d(_x,_y,_z));

    f32 screenratio  = (fabs(_cam->m_bottomRight.m_x - _cam->m_topLeft.m_x) * (f32)getScreenWidth())/(fabs(_cam->m_bottomRight.m_y - _cam->m_topLeft.m_y) * (f32)getScreenHeight());

    float viewDistance = _z;

    Vec3d vEyePt( _cam->getX(), _cam->getY(), viewDistance);
    Vec3d vLookatPt(_cam->getX(), _cam->getY(), _z);
    Vec3d vUpVec( 0.0f, 1.0f, 0.0f );
    Vec3d lookatOffset(0.f, 0.f, -1.f);
    vLookatPt += lookatOffset;

    m_worldViewProj.setWorld(GMatrixIdentity);
    m_worldViewProj.setViewLookAtRH((const Vec3d &)vEyePt, (const Vec3d &)vLookatPt, (const Vec3d &)vUpVec);
    m_worldViewProj.setProjPerspectiveRH(_cam->getFocale(), screenratio, nearPlane, farPlane);

    if (_cam->getRotateCam() && _cam->getMode() == Camera::Camera_InGame)
    {
        m_useRotateZCamera = btrue;

        Matrix44 matRotZ;
        M44_setMatrixRotationZ(&matRotZ, _cam->getRotateZAngle());
        m_worldViewProj.setViewRender(matRotZ);
    }
    else
    {
        m_worldViewProj.setViewRender(GMatrixIdentity);
        m_useRotateZCamera = bfalse;
    }



    _cam->m_lookAt.m_x = vLookatPt.getX();
    _cam->m_lookAt.m_y = vLookatPt.getY();
    _cam->m_lookAt.m_z = vLookatPt.getZ();

    _cam->m_resultingBiasedPosition.m_x = vEyePt.getX();
    _cam->m_resultingBiasedPosition.m_y = vEyePt.getY();
    _cam->m_resultingBiasedPosition.m_z = vEyePt.getZ();

    //update parameters
    updateCameraFrustumPlanes(_cam);
}

//----------------------------------------------------------------------------//

void GFXAdapter_GCM::init2DRender(   )
{
    setGlobalColor(Color::white());
    setAlphaBlend(GFX_BLEND_ALPHA);
    setOrthoView(0.f, (f32) getScreenWidth(), 0.f, (f32) getScreenHeight());
}

void GFXAdapter_GCM::init2DGuiRender()
{
	setOrthoView2DYNeg(0.f, (f32) getScreenWidth(), 0.f, (f32) getScreenHeight());
}

void GFXAdapter_GCM::setOrthoView(f32 _l, f32 _r, f32 _b, f32 _t)
{
    m_worldViewProj.setWorld(GMatrixIdentity);
    m_worldViewProj.setView(GMatrixIdentity);
    m_worldViewProj.setProjOrtho(_l, _r, _b, _t);
    m_useRotateZCamera = bfalse;

    updateCameraFrustumPlanes(CAMERA);
}

//----------------------------------------------------------------------------//

void  GFXAdapter_GCM::compute3DTo2D(const Vec3d& _in, Vec3d& _out)
{
    GFX_Viewport viewp;
    viewp.m_x = 0;
    viewp.m_y = 0;
    viewp.m_width = getScreenWidth();
    viewp.m_height = getScreenHeight();
    viewp.m_minZ = 0.f;
    viewp.m_maxZ = 1.f;

    vec3dProject(&_out, &_in, &viewp, (GMatrix44*)&m_worldViewProj.getViewProj());
}

void  GFXAdapter_GCM::compute2DTo3D(const Vec3d& _in, Vec3d& _out)
{
    GFX_Viewport viewp;
    viewp.m_x = 0;
    viewp.m_y = 0;
    viewp.m_width = getScreenWidth();
    viewp.m_height = getScreenHeight();
    viewp.m_minZ = 0.f;
    viewp.m_maxZ = 1.f;

    vec3dUnProject(&_out, &_in, &viewp, (const GMatrix44 *)&m_worldViewProj.getViewProjInv());
}

void  GFXAdapter_GCM::coord2DTo3D( const Vec2d& _in2d, float _zPlane, Vec2d& _out3d, bool noTranslation)
{
    GFX_Viewport viewp;
    viewp.m_x = 0;
    viewp.m_y = 0;
    viewp.m_width = getScreenWidth();
    viewp.m_height = getScreenHeight();
    viewp.m_minZ = 0.f;
    viewp.m_maxZ = 1.f;

    Vec3d vout;
    Vec3d vin(_in2d.m_x, _in2d.m_y, (f32)_zPlane);
    vec3dUnProject(&vout, &vin, &viewp, (const GMatrix44 *)(noTranslation ? &m_worldViewProj.getViewNoTProjInv(): &m_worldViewProj.getViewProjInv()));

    _out3d.m_x = (f32)vout.m_x;
    _out3d.m_y = (f32)vout.m_y;
}

//----------------------------------------------------------------------------//
void GFXAdapter_GCM::drawScreenQuad( f32 _px, f32 _py, f32 _width, f32 _height, f32 _z, u32 _color, bbool _noTex )
{
    if (getOverDrawMode())
        return;

    m_worldViewProj.setWorld(GMatrixIdentity);

    setSamplerAddressState(0, CELL_GCM_TEXTURE_CLAMP_TO_EDGE, CELL_GCM_TEXTURE_CLAMP_TO_EDGE);

    if (_noTex)
    {
        VertexPC quad[4];

        quad[0].setData( Vec3d( _px, _py, _z ), _color);
        quad[1].setData( Vec3d( _px, _py + _height, _z ), _color);
        quad[2].setData( Vec3d( _px + _width, _py, _z ), _color);
        quad[3].setData( Vec3d( _px + _width, _py + _height, _z ), _color);

        setVertexFormat(VertexFormat_PC);
        DrawPrimitive(GFX_TRIANGLE_STRIP, (void*)quad, 4);
    }
    else
    {
        VertexPCT quad[4];

        quad[0].setData( Vec3d( _px, _py, _z ), Vec2d( 0.f, 1.0f ), _color);
        quad[1].setData( Vec3d( _px, _py + _height, _z ), Vec2d( 0.f, 0.f ), _color);
        quad[2].setData( Vec3d( _px + _width, _py , _z ), Vec2d( 1.0f, 1.0f ), _color);
        quad[3].setData( Vec3d( _px + _width, _py + _height , _z ), Vec2d( 1.0f, 0.0f ), _color);

        setVertexFormat(VertexFormat_PCT);
        DrawPrimitive(GFX_TRIANGLE_STRIP, (void*)quad, 4);
    }

    setSamplerAddressState(0, CELL_GCM_TEXTURE_WRAP, CELL_GCM_TEXTURE_WRAP);
}

void GFXAdapter_GCM::drawScreenQuadC( f32 _px, f32 _py, f32 _width, f32 _height, f32 _z, u32 _color, f32 _centroid, bbool _noTex )
{
    if (getOverDrawMode())
        return;

    m_worldViewProj.setWorld(GMatrixIdentity);

    setSamplerAddressState(0, CELL_GCM_TEXTURE_CLAMP_TO_EDGE, CELL_GCM_TEXTURE_CLAMP_TO_EDGE);

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

    setSamplerAddressState(0, CELL_GCM_TEXTURE_WRAP, CELL_GCM_TEXTURE_WRAP);
}



//----------------------------------------------------------------------------//
    
void GFXAdapter_GCM::drawQuad2D( f32 _px, f32 _py, f32 _width, f32 _height, f32 _z, u32 _color, const Texture* _tex, const Vec2d* _uvStart,  const Vec2d* _uvEnd )
{
    m_worldViewProj.setWorld(GMatrixIdentity);

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

        internSetTextureBind(0, _tex->m_adapterimplementationData);
        setVertexFormat(VertexFormat_PCT);
        m_UVmat = GMatrixIdentity; // PCT so no translation (done with the vertex uv)
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

void GFXAdapter_GCM::draw3DQuad( GFX_3DQUAD* _t_Quad )
{
    VertexPCT quad[4];

    float sx = _t_Quad->m_size.m_x * 0.5f;
    float sy = _t_Quad->m_size.m_y * 0.5f;

    quad[0].setData( Vec3d( -sx, -sy, 0.f ), Vec2d( 0.0f, 1.0f ), _t_Quad->m_color);
    quad[1].setData( Vec3d( sx, -sy, 0.f ), Vec2d( 1.0f, 1.0f ), _t_Quad->m_color);
    quad[3].setData( Vec3d( sx, sy, 0.f ), Vec2d( 1.0f, 0.0f ), _t_Quad->m_color);
    quad[2].setData( Vec3d( -sx, sy, 0.f ), Vec2d( 0.0f, 0.0f ), _t_Quad->m_color);

    Matrix44 worldMat;
    M44_setMatrixTranslation(&worldMat, _t_Quad->m_pos.getX(), _t_Quad->m_pos.getY(), _t_Quad->m_pos.getZ());

    if (!_t_Quad->m_rotation.IsEqual(Vec3d::Zero,MTH_EPSILON))
    {
        //reorient dir
        Matrix44 rotZ;
        Matrix44 rotX;
        Matrix44 rotY;

        M44_setMatrixRotationX(&rotX, _t_Quad->m_rotation.m_x);
        M44_setMatrixRotationY(&rotY, _t_Quad->m_rotation.m_y);
        M44_setMatrixRotationZ(&rotZ, _t_Quad->m_rotation.m_z);
        worldMat = rotX * rotY * rotZ * worldMat;
    }

    m_worldViewProj.setWorld(worldMat);

    m_UVmat = GMatrixIdentity;

    if (getOverDrawMode() > 1)
        setShader( mp_shaderManager.getShaderByIndex(4));
    else
    {
        if (isUseFog())
            setShader( mp_shaderManager.getShaderByIndex(5));
        else
            setShader(mp_defaultShader);
    }

    setVertexFormat(VertexFormat_PCT);

    setAlphaBlend(_t_Quad->m_blendMode);
    DrawPrimitive(GFX_TRIANGLE_STRIP, (void*)quad, 4);
}

//----------------------------------------------------------------------------//

bbool GFXAdapter_GCM::loadTexture(Texture* _texture, const char* _path, u32 _alhpa)
{   
    String filenameCooked = FILESERVER->getCookedName(_path);
    ITF::File* pBinary = ITF::FILEMANAGER->openFile(filenameCooked, ITF_FILE_ATTR_READ);
    
    if (!pBinary)
    {
        _texture->m_adapterimplementationData = NULL;
        return bfalse;
    }

    u64 lenFile = pBinary->getLength();
    u8 * rawData = (u8*)Memory::alignedMalloc(size_t(lenFile), 128);
    pBinary->read(rawData, lenFile); // first copy
    ITF::FILEMANAGER->closeFile(pBinary);
    GCM::Texture * gcmTexture = GCM::CreateTextureFromGTFRawData(rawData); // second copy to VRAM (4GB/s)
    Memory::alignedFree(rawData);
    if (!gcmTexture)
    {
        _texture->m_adapterimplementationData = NULL;
        return bfalse;
    }

    _texture->m_adapterimplementationData = reinterpret_cast<u32>(gcmTexture);
    setTextureSize(_texture, Size(gcmTexture->width, gcmTexture->height));
    RESOURCE_MANAGER->addResourceAsLoaded(_texture);


    return btrue;
}

//----------------------------------------------------------------------------//

void GFXAdapter_GCM::drawTriangle(const Vec2d& _p1, const Vec2d& _p2, const Vec2d& _p3, float _r, float _g, float _b, float _a, float _z)
{
    VertexPC v[3];

    Color color(_a, _r, _g, _b);
    u32 intColor = color.getAsU32();
    v[0].setData(Vec3d(_p1.m_x, _p1.m_y, _z) , intColor);
    v[1].setData(Vec3d(_p2.m_x, _p2.m_y, _z) , intColor);
    v[2].setData(Vec3d(_p3.m_x, _p3.m_y, _z) , intColor);

    setAlphaBlend(GFX_BLEND_ALPHA);
    setShader(mp_defaultShader);
    m_worldViewProj.setWorld(GMatrixIdentity);
    setVertexFormat(VertexFormat_PC);
    DrawPrimitive(GFX_TRIANGLES, (void*)v, 3);
}

//----------------------------------------------------------------------------//

void GFXAdapter_GCM::drawTriangle( const Vertex& _v1, const Vertex& _v2, const Vertex& _v3, const Texture* _tex, bbool _showWireFrame)
{
    IMPLEMENTED_NOT_TESTED;

    VertexPCT v[3];

    m_worldViewProj.setWorld(GMatrixIdentity);

    v[0].setData( Vec3d( _v1.m_pos.m_x, _v1.m_pos.m_y, _v1.m_pos.m_z ), Vec2d( _v1.m_uv.m_x, _v1.m_uv.m_y ), _v1.m_color);
    v[1].setData( Vec3d( _v2.m_pos.m_x, _v2.m_pos.m_y, _v2.m_pos.m_z ), Vec2d( _v2.m_uv.m_x, _v2.m_uv.m_y ), _v2.m_color);
    v[2].setData( Vec3d( _v3.m_pos.m_x, _v3.m_pos.m_y, _v3.m_pos.m_z ), Vec2d( _v3.m_uv.m_x, _v3.m_uv.m_y ), _v3.m_color);

    setAlphaBlend(GFX_BLEND_ALPHA);
    internSetTextureBind(0, _tex->m_adapterimplementationData);
    setVertexFormat(VertexFormat_PCT);
    DrawPrimitive(GFX_TRIANGLES, (void*)v, 3);
    ITF_ASSERT(!_showWireFrame); // wireframe mode not supported
}

//----------------------------------------------------------------------------//

#ifndef ITF_CONSOLE_FINAL
void GFXAdapter_GCM::drawFlatMesh(const ITF_Mesh& _mesh)
{
    m_worldViewProj.setWorld(_mesh.getMatrix());

    setAlphaBlend(GFX_BLEND_ALPHA);

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

void GFXAdapter_GCM::setGlobalColor(const Color& _color)
{
    setGFXGlobalColor(_color);

    FxParameterDB::Vector4 globalColorVec;
    globalColorVec.m_x = _color.getRed();
    globalColorVec.m_y = _color.getGreen();
    globalColorVec.m_z = _color.getBlue();
    globalColorVec.m_w = _color.getAlpha(); // blend factor in alpha

    if (FxParameterDB::Float4Hdl(mp_currentShader->m_VecHandle[0]) != FxParameterDB::InvalidHandle)
        m_rdrCtxt.paramDB.setFloat4(FxParameterDB::Float4Hdl(mp_currentShader->m_VecHandle[0]), globalColorVec);
}

void GFXAdapter_GCM::setFogColor(const Color & _color)
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

void GFXAdapter_GCM::drawMesh(ITF_Mesh& _mesh, bbool _showWireFrame)
{
    ITF_ASSERT(!_showWireFrame); // Not implemented

    m_worldViewProj.setWorld(_mesh.getMatrix());

#ifndef VBMANAGER_USE_DYNAMICRING_VB
    ITF_VertexBuffer* pVertexBuffer =  _mesh.m_vtxBuffer ?   _mesh.m_vtxBuffer : _mesh.m_vtxBufferBuffered[_mesh.m_userBuffer];
#else
    ITF_VertexBuffer* pVertexBuffer = _mesh.m_vtxBuffer;
#endif // VBMANAGER_USE_DYNAMICRING_VB

    setVertexFormat(pVertexBuffer->m_vertexFormat);

    for (u32 i = 0; i < _mesh.m_ElementList.size(); i++)
    {
        ITF_MeshElement& elem = _mesh.m_ElementList[i];

        if (!elem.m_material.m_textureDiffuse.isValidResourceId())
            continue;

        /// Prepare Material.
        setGfxMaterial(elem.m_material);

        if (elem.m_material.m_uvAnim.m_uvFlagAnim)
        {
            computeUVAnim(&elem.m_material.m_uvAnim);
            m_UVmat = elem.m_material.m_uvAnim.m_UVMat;
        }
        else
        {
            m_UVmat = GMatrixIdentity;
        } 
        ITF_ASSERT((m_drawFlag & DRWF_COPYTOTARGET) == 0); // TODO

        if (_mesh.m_pMatrixOverlay)
        {
            if (mp_currentShader->m_VectorArrayHandle[0] != (void *)FxParameterDB::InvalidHandle)
            {
                mp_currentShader->setVectorArray(0, (GFX_Vector4*)_mesh.m_pMatrixOverlay, 4);
            }
        }        

        if (!elem.m_indexBuffer)
            DrawVertexBuffer(GFX_TRIANGLES, pVertexBuffer, elem.m_startIndex, elem.m_count);
        else
            DrawIndexedVertexBuffer(GFX_TRIANGLES, pVertexBuffer, elem.m_indexBuffer, elem.m_count);
        postGfxMaterial(elem.m_material);
    } 
}

void GFXAdapter_GCM::drawMeshPT(const ITF_Mesh& _mesh, bbool _showWireFrame)
{
    TIMERASTER_SCOPE(drawMeshPT);

    renderContext_Set();

    m_worldViewProj.setWorld(_mesh.getMatrix());

    /// set Vertex format
    // setVertexFormat(VertexFormat_PT); // can be PT or PTa

#ifndef VBMANAGER_USE_DYNAMICRING_VB
    ITF_VertexBuffer* pVertexBuffer =  _mesh.m_vtxBuffer ?   _mesh.m_vtxBuffer : _mesh.m_vtxBufferBuffered[_mesh.m_userBuffer];
#else
    ITF_VertexBuffer* pVertexBuffer = _mesh.m_vtxBuffer;
#endif // VBMANAGER_USE_DYNAMICRING_VB


    for (u32 i = 0; i < _mesh.m_ElementList.size(); i++)
    {
        const ITF_MeshElement& elem = _mesh.m_ElementList[i];
        if (!elem.m_material.m_textureDiffuse.isValidResourceId())
            continue;

        /// Prepare Material.
        setGfxMaterial(_mesh.m_ElementList[i].m_material);
        m_UVmat = GMatrixIdentity;

        if (!elem.m_indexBuffer)
            DrawVertexBuffer(GFX_TRIANGLES, pVertexBuffer, elem.m_startIndex, elem.m_count);
        else
            DrawIndexedVertexBuffer(GFX_TRIANGLES, pVertexBuffer, elem.m_indexBuffer, elem.m_count);
    } 
}

void GFXAdapter_GCM::draw2dLine (   const Vec2d& _p1, const Vec2d& _p2, float _size, u32 _color)
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

void GFXAdapter_GCM::draw2dTriangle(const Vec2d& _p1, const Vec2d& _p2, const Vec2d& _p3, u32 _color, i32 _rank)
{
    IMPLEMENTED_NOT_TESTED;

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

void GFXAdapter_GCM::draw2dBox(const Vec2d& _p, f32 _width, f32 _height, u32 _color0, u32 _color1, u32 _color2, u32 _color3, i32 _rank)
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
void GFXAdapter_GCM::drawCircle(   float _x, float _y, float r, float _r, float _g, float _b, float _z, u32 _segsCount  )
{
    IMPLEMENTED_NOT_TESTED;
    /// set first the shader you want to use.
    setShader(mp_defaultShader);

    /// fill vertex buffer.
    if (_segsCount > 64)
        _segsCount = 64;
    VertexPC v[65];
    Color color(1.f, _r, _g, _b);
    u32 intColor = color.getAsU32();
    for(u32 i = 0; i <= _segsCount; i++) 
    {
        f32 fi = i * MTH_2PI / (float)_segsCount;
        v[i].setData(Vec3d((f32)cos(fi) * r + _x, (f32)sin(fi) * r + _y, _z), intColor);
    }

    m_worldViewProj.setWorld(GMatrixIdentity);

    /// set Vertex format
    setVertexFormat(VertexFormat_PC);

    /// draw geometrie.
    DrawPrimitive(GFX_LINE_STRIP, (void*)v, _segsCount+1);
}

//----------------------------------------------------------------------------//
void GFXAdapter_GCM::drawBox( const Vec2d& _pos, f32 _angle, const Vec2d& _extent, f32 _r, f32 _g, f32 _b, float _z,bbool _fill  )
{
    /// set first the shader you want to use.
    setShader(mp_defaultShader);

    VertexPC v[5];
    Color color(1.f, _r, _g, _b);
    u32 intColor = color.getAsU32();

    Vec2d p = Vec2d(-_extent.m_x,_extent.m_y).Rotate(_angle) + _pos;
    v[0].setData(Vec3d(p.m_x, p.m_y, _z), intColor);
    p = Vec2d(_extent.m_x,_extent.m_y).Rotate(_angle) + _pos;
    v[1].setData(Vec3d(p.m_x, p.m_y, _z), intColor);
    p = Vec2d(_extent.m_x,-_extent.m_y).Rotate(_angle) + _pos;
    v[2].setData(Vec3d(p.m_x, p.m_y, _z), intColor);
    p = Vec2d(-_extent.m_x,-_extent.m_y).Rotate(_angle) + _pos;
    v[3].setData(Vec3d(p.m_x, p.m_y, _z), intColor);
    p = Vec2d(-_extent.m_x,_extent.m_y).Rotate(_angle) + _pos;
    v[4].setData(Vec3d(p.m_x, p.m_y, _z), intColor);

    m_worldViewProj.setWorld(GMatrixIdentity);

    /// set Vertex format
    setVertexFormat(VertexFormat_PC);

    /// draw geometry
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

void GFXAdapter_GCM::drawSphere(float _x, float _y, float _z, float r, int lats, int longs, int _wireFrame, float _r, float _g, float _b) 
{
    IMPLEMENTED_NOT_TESTED;

    /// set first the shader you want to use.
    setShader(mp_defaultShader);

    /// fill vertex buffer.
    VertexPC v[65];
    Color color(1.f, _r, _g, _b);
    u32 intColor = color.getAsU32();
    
    for(u32 i = 0; i <= 64; i++) 
    {
        f32 fi = i * MTH_2PI / 64.f;
        v[i].setData(Vec3d((f32)cos(fi), (f32)sin(fi), 0.f), intColor);
    }

    // compute scale and translation
    Matrix44 matScale;
    M44_setMatrixScaling(&matScale, r, r, r);
    Matrix44 matTrans;
    M44_setMatrixTranslation(&matTrans, _x, _y, _z);
    Matrix44 matWorld = matScale * matTrans;
    m_worldViewProj.setWorld(matWorld);

    setVertexFormat(VertexFormat_PC);

    DrawPrimitive(GFX_LINE_STRIP, (void*)v, 65);
}

void GFXAdapter_GCM::drawSphere(float _x, float _y, float _radius, float _r, float _g, float _b, float _z, u32 _segsCount, bbool _volume, bbool _xaxis, bbool _yaxis, bbool _zaxis)
{
    IMPLEMENTED_NOT_TESTED;

    setShader(mp_defaultShader);

    setVertexFormat(VertexFormat_PC);

    /// fill vertex buffer.
    if (_segsCount > 64)
        _segsCount = 64;
    VertexPC v[65];
    Color color(1.f, _r, _g, _b);
    u32 intColor = color.getAsU32();

    if (_volume )
    {
        for(u32 i = 0; i <= _segsCount; i++) 
        {
            f32 fi = i * MTH_2PI / (float)_segsCount;
            v[i].setData(Vec3d((f32)cos(fi) * _radius, (f32)sin(fi) * _radius, 0.f), intColor);
        }

        Matrix44 viewMat = m_worldViewProj.getView();
        Matrix44 invViewMat;
        M44_matrixInverse(invViewMat.fa16, viewMat.fa16);
        invViewMat.setTranslation(Vec3d(_x, _y, _z));

        m_worldViewProj.setWorld(invViewMat);

        DrawPrimitive(GFX_LINE_STRIP, (void*)v, _segsCount+1);
    }

    m_worldViewProj.setWorld(GMatrixIdentity);

    if (_xaxis)
    {
        for(u32 i = 0; i <= _segsCount; i++) 
        {
            f32 fi = i * MTH_2PI / (float)_segsCount;
            v[i].setData(Vec3d(_x, (f32)cos(fi) * _radius + _y, (f32)sin(fi) * _radius + _z), intColor);
        }

        DrawPrimitive(GFX_LINE_STRIP, (void*)v, _segsCount+1);
    }

    if (_yaxis)
    {
        for(u32 i = 0; i <= _segsCount; i++) 
        {
            f32 fi = i * MTH_2PI / (float)_segsCount;
            v[i].setData(Vec3d((f32)cos(fi) * _radius + _x, _y, (f32)sin(fi) * _radius + _z), intColor);
        }

        DrawPrimitive(GFX_LINE_STRIP, (void*)v, _segsCount+1);
    }

    if (_zaxis)
    {
        for(u32 i = 0; i <= _segsCount; i++) 
        {
            f32 fi = i * MTH_2PI / (float)_segsCount;
            v[i].setData(Vec3d((f32)cos(fi) * _radius + _x, (f32)sin(fi) * _radius + _y, _z), intColor);
        }

        DrawPrimitive(GFX_LINE_STRIP, (void*)v, _segsCount+1);
    }
}

//----------------------------------------------------------------------------//

void GFXAdapter_GCM::setAlphaBlend(GFX_BLENDMODE _Blend)
{
    u_CurrentBlendingMode = _Blend;
    
    if (getOverDrawMode() > 1)
        _Blend = GFX_BLEND_ALPHA;

    if (_Blend == GFX_BLEND_COPY)
    {
        m_rdrCtxt.gcmCtxt->SetBlendEnable(CELL_GCM_FALSE);
        return;
    }

    // blend active
    m_rdrCtxt.gcmCtxt->SetBlendEnable(CELL_GCM_TRUE);
    u32 blendOp = CELL_GCM_FUNC_ADD;
    u32 srcCoef;
    u32 dstCoef;

    switch(_Blend)
    {
    default:
    case GFX_BLEND_UNKNOWN:
        ITF_ASSERT_MSG(bfalse, "Incorrect blend mode");
        //missing break intentionally
    case GFX_BLEND_ALPHA:
        srcCoef = CELL_GCM_SRC_ALPHA;
        dstCoef = CELL_GCM_ONE_MINUS_SRC_ALPHA;
        break;

    case GFX_BLEND_ALPHAPREMULT:
        srcCoef = CELL_GCM_ONE;
        dstCoef = CELL_GCM_ONE_MINUS_SRC_ALPHA;
        break;

    case GFX_BLEND_ALPHADEST:
        srcCoef = CELL_GCM_DST_ALPHA;
        dstCoef = CELL_GCM_ONE_MINUS_DST_ALPHA;
        break;

    case GFX_BLEND_ALPHADESTPREMULT:
        srcCoef = CELL_GCM_DST_ALPHA;
        dstCoef = CELL_GCM_ONE;
        break;

    case GFX_BLEND_ADD:
        srcCoef = CELL_GCM_ONE;
        dstCoef = CELL_GCM_ONE;
        break;

    case GFX_BLEND_ADDALPHA:
        srcCoef = CELL_GCM_SRC_ALPHA;
        dstCoef = CELL_GCM_ONE;
        break;

    case GFX_BLEND_SUBALPHA:
        srcCoef = CELL_GCM_SRC_ALPHA;
        dstCoef = CELL_GCM_ONE;
        blendOp = CELL_GCM_FUNC_REVERSE_SUBTRACT;
        break;

    case GFX_BLEND_SUB:
        srcCoef = CELL_GCM_ZERO;
        dstCoef = CELL_GCM_ONE_MINUS_SRC_COLOR;
        break;

    case GFX_BLEND_MUL:
        srcCoef = CELL_GCM_ZERO;
        dstCoef = CELL_GCM_SRC_COLOR;
        break;

    case GFX_BLEND_ALPHAMUL:
        srcCoef = CELL_GCM_ZERO;
        dstCoef = CELL_GCM_SRC_ALPHA;
        break;

    case GFX_BLEND_IALPHAMUL:
        srcCoef = CELL_GCM_ZERO;
        dstCoef = CELL_GCM_ONE_MINUS_SRC_ALPHA;
        break;

    case GFX_BLEND_IALPHA:
        srcCoef = CELL_GCM_ONE_MINUS_SRC_ALPHA;
        dstCoef = CELL_GCM_SRC_ALPHA;
        break;

    case GFX_BLEND_IALPHAPREMULT:
        srcCoef = CELL_GCM_ONE;
        dstCoef = CELL_GCM_SRC_ALPHA;
        break;

    case GFX_BLEND_IALPHADEST:
        srcCoef = CELL_GCM_ONE_MINUS_DST_ALPHA;
        dstCoef = CELL_GCM_DST_ALPHA;
        break;

    case GFX_BLEND_IALPHADESTPREMULT:
        srcCoef = CELL_GCM_ONE_MINUS_DST_ALPHA;
        dstCoef = CELL_GCM_ONE;
        break;

    case GFX_BLEND_MUL2X:
        srcCoef = CELL_GCM_DST_COLOR;
        dstCoef = CELL_GCM_SRC_COLOR;
        break;

    case GFX_BLEND_ALPHATOCOLOR:
        srcCoef = CELL_GCM_DST_ALPHA;
        dstCoef = CELL_GCM_ZERO;
        break;

    case GFX_BLEND_IALPHATOCOLOR:
        srcCoef = CELL_GCM_ONE_MINUS_DST_ALPHA;
        dstCoef = CELL_GCM_ZERO;
        break;

    case GFX_BLEND_SETTOCOLOR:
        srcCoef = CELL_GCM_DST_COLOR;
        dstCoef = CELL_GCM_ZERO;
        break;
    }

    m_rdrCtxt.gcmCtxt->SetBlendFunc(srcCoef, dstCoef, srcCoef, dstCoef);
    m_rdrCtxt.gcmCtxt->SetBlendEquation(blendOp, blendOp);
}

void GFXAdapter_GCM::setFillMode(GFX_FILLMODE _fillmode)
{
    switch(_fillmode)
    {
    case GFX_FILL_SOLID:
        m_rdrCtxt.gcmCtxt->SetFrontPolygonMode(CELL_GCM_POLYGON_MODE_FILL);
        m_rdrCtxt.gcmCtxt->SetBackPolygonMode(CELL_GCM_POLYGON_MODE_FILL);
        break;
    case GFX_FILL_WIREFRAME:
        m_rdrCtxt.gcmCtxt->SetFrontPolygonMode(CELL_GCM_POLYGON_MODE_LINE);
        m_rdrCtxt.gcmCtxt->SetBackPolygonMode(CELL_GCM_POLYGON_MODE_LINE);
        break;
    case GFX_FILL_POINT:
        m_rdrCtxt.gcmCtxt->SetFrontPolygonMode(CELL_GCM_POLYGON_MODE_POINT);
        m_rdrCtxt.gcmCtxt->SetBackPolygonMode(CELL_GCM_POLYGON_MODE_POINT);
        break;
    }
}
 

//----------------------------------------------------------------------------//

void GFXAdapter_GCM::depthMask(u32 _depthMask)
{
    if (_depthMask)
    {
        m_rdrCtxt.gcmCtxt->SetDepthMask(CELL_GCM_TRUE);
    }
    else
    {
        m_rdrCtxt.gcmCtxt->SetDepthMask(CELL_GCM_FALSE);
    }
}

//----------------------------------------------------------------------------//

void GFXAdapter_GCM::depthTest(u32 _Test)
{
    if (_Test)
    {
        ITF_ASSERT(0); // not supposed to be set!
        m_rdrCtxt.gcmCtxt->SetDepthTestEnable(CELL_GCM_TRUE);
    }
    else
    {
        m_rdrCtxt.gcmCtxt->SetDepthTestEnable(CELL_GCM_FALSE);
    }
}

//----------------------------------------------------------------------------//

void GFXAdapter_GCM::SetTexture(int _Sampler, Texture* _Texture, bbool _linearFiltering)
{
    u32 adrtexture = 0;
    if (_Texture) 
        adrtexture = (u32)_Texture->m_adapterimplementationData;
    internSetTextureBind(_Sampler, adrtexture, _linearFiltering);
}

//----------------------------------------------------------------------------//

void GFXAdapter_GCM::SetTextureBind(int _Sampler, unsigned int _Bind, bbool _linearFiltering)
{
    internSetTextureBind(u32(_Sampler), _Bind, _linearFiltering);
}

void GFXAdapter_GCM::internSetTextureBind    (u32 _Sampler, u32 _Bind, bbool _linearFiltering)
{
    const GCM::Texture * gcmtexture = (const GCM::Texture *)_Bind;
    m_rdrCtxt.paramDB.setTexture(FxParameterDB::TexSamplerHdl(mp_currentShader->m_TextureHandle[_Sampler]), (void *)gcmtexture);

    u8 minFiltering = m_minLinearFilteringMode;
    u8 magFiltering = CELL_GCM_TEXTURE_LINEAR;
    if (!_linearFiltering)
    {
        minFiltering = CELL_GCM_TEXTURE_NEAREST_NEAREST;
        magFiltering = CELL_GCM_TEXTURE_NEAREST;
    }
    setSamplerFilterState(_Sampler, minFiltering, magFiltering);

    if (!m_currentTexSamplerState[_Sampler].activated)
    {
        m_rdrCtxt.gcmCtxt->SetTextureControlAlphaKill(_Sampler, CELL_GCM_TRUE, 0, 12<<8, CELL_GCM_TEXTURE_MAX_ANISO_1, m_currentTexSamplerState[_Sampler].alphaKill);
        m_currentTexSamplerState[_Sampler].activated = true;
    }
}


//----------------------------------------------------------------------------//

void GFXAdapter_GCM::lockTexture( Texture* _tex, LOCKED_TEXTURE* _lockTex, u32 _flag )
{
    IMPROVE_THIS
    // may need a rsx stall !
    const GCM::Texture * gcmtexture = (const GCM::Texture *)_tex->m_adapterimplementationData;
    _lockTex->mp_Bits = gcmtexture->vramHdl.GetAddress();
    _lockTex->m_pitch = i32(gcmtexture->pitch);
}

//----------------------------------------------------------------------------//

void GFXAdapter_GCM::unlockTexture( Texture* _tex)
{
    IMPROVE_THIS
}

//----------------------------------------------------------------------------//

void GFXAdapter_GCM::DrawPrimitive(PRIMITIVETYPE _type, const void* _p_Vertex, u32 _NumberVertex)
{
    i32 type;

    switch(_type)
    {
    default:
    case GFX_TRIANGLES: 
        type = CELL_GCM_PRIMITIVE_TRIANGLES;
        break;
    case GFX_POINTS:
        type = CELL_GCM_PRIMITIVE_POINTS;
        break;
    case GFX_LINES:
        type = CELL_GCM_PRIMITIVE_LINES;
        break;
    case GFX_LINE_STRIP:
        type = CELL_GCM_PRIMITIVE_LINE_STRIP;
        break;
    case GFX_TRIANGLE_STRIP:
        type = CELL_GCM_PRIMITIVE_TRIANGLE_STRIP;
        break;
    }

    if (isUseFog())
    {
        GFX_Vector4 fog;
        fog.m_x = m_colorFog.getRed();
        fog.m_y = m_colorFog.getGreen();
        fog.m_z = m_colorFog.getBlue();
        fog.m_w = m_colorFog.getAlpha(); // blend factor in alpha

        // color Fog.
        FxParameterDB::Float4Hdl vhdl = FxParameterDB::Float4Hdl(mp_currentShader->m_VecHandle[3]);
        if (vhdl)
        {
            m_rdrCtxt.paramDB.setFloat4(vhdl, (const FxParameterDB::Vector4&)fog);
        }
    }

    // setVertexBuffer( _vertexBuffer ); -> supposed to have been set by the caller
    beginShader(mp_currentShader);
    applyVertexDecl(m_currentVertexStream.stride, *m_currentVertexDecl);
    m_rdrCtxt.gcmCtxt->SetDrawInlineArray(type, _NumberVertex * m_currentVertexStream.stride / 4, _p_Vertex);
    DEBUG_SYNCH_RSX();
    endShader(mp_currentShader);
}

//----------------------------------------------------------------------------//

void GFXAdapter_GCM::setMatrixTransform(MATRIXMODE _type, const GMatrix44* _Matrix)
{
    switch(_type)
    {
    default:
    case GFX_MWORLDTOWIEW:
    case GFX_MWORLD:
        m_worldViewProj.setWorld(*_Matrix);
        break;
    case GFX_MVIEW:
        m_worldViewProj.setView(*_Matrix);
        break;
    case GFX_PROJECTION:
        m_worldViewProj.setProj(*_Matrix);
        break;
    case GFX_TEXTURE:
        m_UVmat = *_Matrix;
        break;
    }
}


//----------------------------------------------------------------------------//

void GFXAdapter_GCM::setupViewport(GFX_RECT* _Viewport)
{
    IMPROVE_THIS

    m_lastSetViewPort = *_Viewport;
    
    u32 x,y,w,h;
    f32 min, max;
    f32 scale[4],offset[4];

    x = _Viewport->left;
    y = _Viewport->top;
    w = _Viewport->right;
    h = _Viewport->bottom;
    min = 0.0f;
    max = 1.0f;
  
    scale[0] = w * 0.5f;
    scale[1] = h * -0.5f;
    scale[2] = 0.0f;//(max - min);// * 0.5f;
    scale[3] = 0.0f;
    offset[0] = x + scale[0];
    offset[1] = y + h * 0.5f;
    offset[2] = min;//(max + min) * 0.5f;
    offset[3] = 0.0f;

    m_rdrCtxt.gcmCtxt->SetViewport(x, y, w, h, min, max, scale, offset);
    m_rdrCtxt.gcmCtxt->SetScissor(x, y, w, h);
}

//----------------------------------------------------------------------------//

void GFXAdapter_GCM::getViewport(GFX_RECT* _RectOut)
{
    if(_RectOut)
    {
        (*_RectOut) = m_lastSetViewPort;
    }
}

void GFXAdapter_GCM::setScissorRect(GFX_RECT* _clipRect)
{
    m_rdrCtxt.gcmCtxt->SetScissor(  Clamp(_clipRect->left, 0, 4095),
                                    Clamp(_clipRect->top, 0, 4095),
                                    Clamp(_clipRect->right - _clipRect->left, 0, 4096),
                                    Clamp(_clipRect->bottom - _clipRect->top, 0, 4096));
}

//----------------------------------------------------------------------------//
// generateTexture.
//----------------------------------------------------------------------------//
void GFXAdapter_GCM::generateTexture(Texture* _texture)
{
    // nothing to do ?

}

void GFXAdapter_GCM::createTexture(Texture * _texture, u32 _sizeX, u32 _sizeY, u32 _mipLevel, Texture::PixFormat _pixformat, u32 _pool, bbool _dynamic)
{
    _texture->setSize(_sizeX, _sizeY);
    _texture->m_pixFormat = _pixformat;
    _texture->m_mipMapLevels  = _mipLevel;

    GCM::TextureFormat::Enum gcmTexFormat = GCM::TextureFormat::ARGB32Linear;

    switch (_pixformat)
    {
    case Texture::PF_RGB:
        gcmTexFormat = GCM::TextureFormat::ARGB32Linear;
        break;
    case Texture::PF_DXT1:
        gcmTexFormat = GCM::TextureFormat::DXT1Linear;
        break;
    case Texture::PF_DXT2:
    case Texture::PF_DXT3:
        gcmTexFormat = GCM::TextureFormat::DXT3Linear;
        break;
    case Texture::PF_DXT4:
    case Texture::PF_DXT5:
        gcmTexFormat = GCM::TextureFormat::DXT5Linear;
        break;
    case Texture::PF_RGBA:
        gcmTexFormat = GCM::TextureFormat::ARGB32Linear;
        break;
    case Texture::PF_L8:
        gcmTexFormat = GCM::TextureFormat::L8;
        break;
    }

    // alloc in main ram, as it's a texture requested to be CPU accessed
    _texture->m_adapterimplementationData = u32(GCM::CreateTexture(_sizeX, _sizeY, 1, 0, gcmTexFormat));
}

//----------------------------------------------------------------------------//

void GFXAdapter_GCM::cleanupTexture(Texture* _texture)
{
    if (!_texture) return;

    GCM::Texture * gcmTex = (GCM::Texture *)_texture->m_adapterimplementationData;
    delete gcmTex;
    _texture->m_adapterimplementationData = 0;
}

void GFXAdapter_GCM::cleanupBaseTexture(Texture* _texture)
{
    cleanupTexture(_texture);
}

//----------------------------------------------------------------------------//

void GFXAdapter_GCM::setTextureSize(Texture* _texture, const Size& _size )
{
    _texture->setSize((u32)_size.d_width ,(u32)_size.d_height);
}


//----------------------------------------------------------------------------//

unsigned char* GFXAdapter_GCM::grabTexture(Texture* _texture)
{
    return 0;
}

 
//----------------------------------------------------------------------------//


u32 GFXAdapter_GCM::getMaxTextureSize()
{
    return 4096;
}

//----------------------------------------------------------------------------//

void GFXAdapter_GCM::loadFromMemory(const void* _buffer, const Size& _texture_size, Texture* _texture, Texture::PixFormat _pixel_format, const u32 _buffer_size, bbool bCooked, u32 _mipmapCount)
{
    ITF_ASSERT(0)
    IMPLEMENT_THIS;
}


//----------------------------------------------------------------------------//

static u32 GFXToGCM_SamplerState[] =
{
        CELL_GCM_TEXTURE_WRAP,   // GFXTADDRESS_WRAP = 0,
        CELL_GCM_TEXTURE_MIRROR, // GFXTADDRESS_MIRROR,
        CELL_GCM_TEXTURE_CLAMP_TO_EDGE,  // GFXTADDRESS_CLAMP,
        CELL_GCM_TEXTURE_BORDER  // GFXTADDRESS_BORDER,
};

void GFXAdapter_GCM::setSamplerState( u32 _samp, GFX_SAMPLERSTATETYPE _type, i32 _value )
{
    ITF_ASSERT(_value < sizeof(GFXToGCM_SamplerState)/sizeof(GFXToGCM_SamplerState[0]));
    u32 gcmAddressValue = GFXToGCM_SamplerState[_value];

    if (_type == GFXSAMP_ADDRESSU)
    {
        setSamplerAddressState(_samp, gcmAddressValue, m_currentTexSamplerState[_samp].wrapV);
    }
    else if (_type == GFXSAMP_ADDRESSV)
    {
        setSamplerAddressState(_samp, m_currentTexSamplerState[_samp].wrapU, gcmAddressValue);
    }
    else
    {
        // not supported !!.
        ITF_ASSERT(0);
    }
}

inline void GFXAdapter_GCM::internSetTextureAdressingMode(u32 _samp, GFX_TEXADRESSMODE _u, GFX_TEXADRESSMODE _v)
{
    ITF_ASSERT(_u < sizeof(GFXToGCM_SamplerState)/sizeof(GFXToGCM_SamplerState[0]));
    ITF_ASSERT(_v < sizeof(GFXToGCM_SamplerState)/sizeof(GFXToGCM_SamplerState[0]));
    u32 gcmAddressU = GFXToGCM_SamplerState[_u];
    u32 gcmAddressV = GFXToGCM_SamplerState[_v];
    setSamplerAddressState(_samp, gcmAddressU, gcmAddressV);
}

inline void GFXAdapter_GCM::setSamplerAddressState(u32 _samp, u32 _uWrap, u32 _vWrap)
{
    SamplerState & sampState = m_currentTexSamplerState[_samp];
    if (sampState.wrapU != _uWrap
        || sampState.wrapV != _vWrap)
    {
        m_rdrCtxt.gcmCtxt->SetTextureAddress(_samp, _uWrap, _vWrap, CELL_GCM_TEXTURE_CLAMP_TO_EDGE, CELL_GCM_TEXTURE_UNSIGNED_REMAP_NORMAL, CELL_GCM_TEXTURE_ZFUNC_NEVER, 0);
        sampState.wrapU = _uWrap;
        sampState.wrapV = _vWrap;
    }
}

void GFXAdapter_GCM::setSamplerFilterState(u32 _samp, u32 _minFilter, u32 _magFilter)
{
    SamplerState & sampState = m_currentTexSamplerState[_samp];
    if (sampState.magFilter!=_magFilter
        || sampState.minFilter!=_minFilter)
    {
        m_rdrCtxt.gcmCtxt->SetTextureFilter(_samp, sampState.bias, _minFilter, _magFilter, CELL_GCM_TEXTURE_CONVOLUTION_QUINCUNX);
        sampState.magFilter = _magFilter;
        sampState.minFilter = _minFilter;
    }
}

void GFXAdapter_GCM::clear(u32 _buffer,  f32 _r, f32 _g, f32 _b, f32 _a)
{
    ITF_ASSERT((_buffer & GFX_CLEAR_COLOR) == GFX_CLEAR_COLOR); // only supported mode right now
    m_rdrCtxt.gcmCtxt->SetClearColor(ITFCOLOR_TO_ARGB(Color(_a,_r,_g,_b).getAsU32())); // color is in ARGB format !
    m_rdrCtxt.gcmCtxt->SetClearSurface(CELL_GCM_CLEAR_R | CELL_GCM_CLEAR_G | CELL_GCM_CLEAR_B | CELL_GCM_CLEAR_A);
}

//----------------------------------------------------------------------------//

void GFXAdapter_GCM::printDebugInfoOnScreen()
{
#ifndef ITF_DISABLE_DEBUGINFO
    m_worldViewProj.setWorld(GMatrixIdentity);

#ifndef ITF_FINAL
    // safe Frame.
    if (getSafeFrameMode())
        drawSafeFrame();
#endif // ITF_FINAL

    setAlphaBlend(GFX_BLEND_ALPHA);

#ifdef RASTER_ENABLED
    RastersManager::get().rastersDraw();
#endif //RASTER_ENABLED

#ifndef ITF_FINAL
    if (isShowDebugInfo())
    {
        getVertexBufferManager().drawDebugVBInfo();
    }
#endif // ITF_FINAL
    
    setShader(mp_defaultShader);

    {
        // Flush 2D boxes
        setVertexFormat(VertexFormat_PC);

        for (u32 it = 0; it < m_Box2DList.size(); ++it)
        {
            const Box2D& rect = m_Box2DList[it];

            DrawPrimitive(GFX_TRIANGLE_STRIP, (void*)rect.v, 4);
        }
     }

    // and triangle 2D list
    {
        for (u32 it = 0; it < m_Triangle2DList.size(); ++it)
        {
            const Triangle2D& tri = m_Triangle2DList[it];

            DrawPrimitive(GFX_TRIANGLES, (void*)tri.v, 3);
        }
    }

#if defined(_USE_FONTDEBUG)
    if (getDisplayDebugStats())
    {
        resetVertexDeclCache();

        // Warning/error stuff
        f32 invW = 1.f/f32(getScreenWidth());
        f32 invH = 1.f/f32(getScreenHeight());

        f32 scaleFontHeightRatio = 40.f; // Height of the font in pixel
        u32 pixelCharHeight = 12;

        //f32 dbgHOffset = 0.067f;
        f32 dbgHOffset = 0.0f;

        GFX_RECT rct;
        rct.left = 80;
        rct.right = 780;
        rct.top = getScreenHeight() - pixelCharHeight - pixelCharHeight/2;
        rct.bottom = rct.top + pixelCharHeight;

        for (ITF_VECTOR<DBGText>::const_iterator it = m_DBGTexts.begin(), itEnd = m_DBGTexts.end();
            it != itEnd;
            ++it)
        {
            const DBGText & txt = *it;
            u32 color = Color(1.f, txt.m_r, txt.m_g, txt.m_b).getAsU32();
            StringConverter str(txt.m_text.cStr());

            if (txt.m_x < 0.0f && txt.m_y < 0.0f)
            {
                rct.top -= pixelCharHeight;
                rct.bottom = rct.top + pixelCharHeight;
                cellDbgFontPrintf( f32(rct.left)*invW, dbgHOffset + f32(rct.top)*invH, pixelCharHeight*invH*scaleFontHeightRatio, color, "%s", str.getChar());
            }
            else
            {
                u32 localPixelCharHeight = 12;
                GFX_RECT localRect;
                localRect.left = i32(txt.m_x);
                localRect.right = getScreenWidth();
                localRect.top = i32(txt.m_y);
                localRect.bottom = rct.top + localPixelCharHeight;
                cellDbgFontPrintf(f32(localRect.left)*invW, dbgHOffset + f32(localRect.top)*invH, localPixelCharHeight*invH*scaleFontHeightRatio, color, "%s", str.getChar());
            }
        }

        // FPS stuff
        f32 lastFrameTime = m_cpuTimeProfiler.Stop()*1000.f;
        m_cpuTimeProfiler.Start();

        f32 cpuWaitForCBAvailable = ms_cpuWaitForCBAvailable * 1000.f;
        ms_cpuWaitForCBAvailable = 0.f;
        f32 cpuWaitForGPUFrameCount = m_cpuWaitForGPUFrameCount * 1000.f;

        // ABGR format
        const u32 warningColor  = 0xff1C9Effu;
        const u32 errorColor    = 0xff0000ffu;
        const u32 allOKColor    = 0xff00ff00u;

        const f32 warningFrameTime = 13.f;
        const f32 errorFrameTime = 16.66f;
        const f32 referenceFps = 59.f; // 60 +/- 1 frame

        static f32 fpsXPos = 0.69f;

        bool gpuBound = cpuWaitForCBAvailable>0.1f || cpuWaitForGPUFrameCount>0.1f;
        f32 smoothedFps = getfPs();
        u32 fpsColor = smoothedFps<referenceFps?errorColor:allOKColor;
        cellDbgFontPrintf( fpsXPos, 0.025f , 0.6f, fpsColor, "%5.2f fps (%5.2fms)", smoothedFps, lastFrameTime);
        f32 cpuTimeNoWait = lastFrameTime - cpuWaitForCBAvailable - cpuWaitForGPUFrameCount;
        u32 cpuColor = cpuTimeNoWait>errorFrameTime?errorColor:(cpuTimeNoWait>warningFrameTime?warningColor:allOKColor);
        if(!gpuBound)
        {
            cellDbgFontPuts( fpsXPos - 0.0076f, 0.025f + 0.014f, 0.6f, cpuColor, ">");
        }
        cellDbgFontPrintf( fpsXPos, 0.025f + 0.014f, 0.6f, cpuColor, "CPU %5.2fms Sync CB %5.2fms Frame %5.2fms",
                                                         cpuTimeNoWait,
                                                         cpuWaitForCBAvailable,
                                                         cpuWaitForGPUFrameCount);

        f32 gpuRenderTime = m_gpuRenderTimer.getTimeMilliSec();
        f32 gpuFlipTime = m_gpuPresentTimer.getTimeMilliSec();
        u32 gpuColor = gpuRenderTime>errorFrameTime?errorColor:(gpuRenderTime>warningFrameTime?warningColor:allOKColor);
        if(gpuBound)
        {
            cellDbgFontPuts(fpsXPos - 0.0074f, 0.025f + 0.028f, 0.6f, gpuColor, ">");
        }
        cellDbgFontPrintf( fpsXPos, 0.025f + 0.028f, 0.6f, gpuColor, "GPU %5.2fms Flip %5.2fms",
                                                          gpuRenderTime,
                                                          gpuFlipTime);
        
        // print mem info
        sys_memory_info_t memInfo;
        sys_memory_get_user_memory_size(&memInfo);

        GCM::VRAMStats vramStats;
        GCM::VRAMAllocator::GetStats(vramStats);

        u32 memColor = (vramStats.vramFreeSize < 29*1024*1024 || memInfo.available_user_memory < 20*1024*1024)?
                        errorColor:allOKColor;
        cellDbgFontPrintf(  fpsXPos, 0.025f + 0.028f + 0.014f, 0.6f, memColor, "MEM %6.1f VRAM %6.2f",
                            f32((memInfo.total_user_memory - memInfo.available_user_memory)/1024) / 1024.f,
                            f32((vramStats.vramTotalSize - vramStats.vramFreeSize)/1024) /1024.f);
        cellDbgFontPrintf(  fpsXPos, 0.025f + 0.028f + 0.014f + 0.014f, 0.6f, 0xff00ffffu,
                            "Version %ls",
                            m_engineDataVersion.cStr());
        cellDbgFontPrintf(  fpsXPos, 0.025f + 0.028f + 0.042f, 0.6f, allOKColor,"%ls", m_extraInfo.cStr());
#ifndef ITF_FINAL
        extern ux g_quitTargetCount;
        extern ux g_quitCount;
        cellDbgFontPrintf(  fpsXPos, 0.025f + 0.028f + 0.042f + 0.014f, 0.6f, allOKColor,"End in %d events", int(g_quitTargetCount) - int(g_quitCount));
#endif

        cellDbgFontDrawGcm();
        resetVertexDeclCache(); // reset vertex decl as the font changed it internally
    }
#endif // Dbg font
#endif // ITF_DEBUG_LEVEL>0 && !define(ITF_DISABLE_DEBUGINFO)


    // clean every array
    m_DBGTexts.clear();
    m_Box2DList.clear();
    m_Triangle2DList.clear();
}

void GFXAdapter_GCM::minimalDisplay()
{
    m_rdrCtxt.gcmCtxt->SetClearColor(ITFCOLOR_TO_ARGB(Color(1.f, 0.f, 0.f, 0.f).getAsU32())); // color is in ARGB format !
    m_rdrCtxt.gcmCtxt->SetClearSurface(CELL_GCM_CLEAR_R | CELL_GCM_CLEAR_G | CELL_GCM_CLEAR_B | CELL_GCM_CLEAR_A);

    if (m_nbFrameBuffer>2
        || m_usingLibResc)
    {
        m_flipMgr->setWaitFlip(m_rdrCtxt);
    }
    m_flipMgr->setFlip(m_rdrCtxt, m_backBufferIndex);
    if (m_nbFrameBuffer<=2
        && !m_usingLibResc)
    {
        m_flipMgr->setWaitFlip(m_rdrCtxt); // double buffering -> sync the gpu with the flip
    }

    // switch to the next render target 
    m_backBufferIndex = (m_backBufferIndex + 1u) % m_nbFrameBuffer;
    m_currentRenderTarget = NULL; // force reset of the render target as the flip changed it
    setRenderTarget(m_frameBuffers[m_backBufferIndex]);

    m_rdrCtxt.gcmCtxt->Finish(m_finishCount++);
}

void GFXAdapter_GCM::present()
{
    printDebugInfoOnScreen();

    // Synchro GPU/CPU
    m_cpuFrameCount++; 
    
    m_gpuRenderTimer.stop(m_rdrCtxt);
    // validate end of rendering
    m_gpuRenderFrameCountLabel.setWriteOnBackEnd(m_rdrCtxt, m_cpuFrameCount);
    m_gpuPresentTimer.start(m_rdrCtxt);
    // In triple buffering (or using libresc, that's triple buffering anyway) :
    // We can reuse the buffer immediately after flip, so the GPU has to wait for flip just before the next flip
    // With double buffering, the gpu cannot render until teh flip, so we need to issue the wait immediately
    if (m_nbFrameBuffer>2
        || m_usingLibResc)
    {
        m_flipMgr->setWaitFlip(m_rdrCtxt);
    }
    m_flipMgr->setFlip(m_rdrCtxt, m_backBufferIndex);
    if (m_nbFrameBuffer<=2
        && !m_usingLibResc)
    {
        m_flipMgr->setWaitFlip(m_rdrCtxt); // double buffering -> sync the gpu with the flip
    }
    m_rdrCtxt.gcmCtxt->SetReportLocation(REPORT_LOCATION); // set the report location back to our chosen location
    m_gpuFlipFrameCountLabel.setWriteOnBackEnd(m_rdrCtxt, m_cpuFrameCount);
    m_gpuPresentTimer.stop(m_rdrCtxt);
    m_rdrCtxt.gcmCtxt->Flush(); // validate the just pushed command
    // check that the cpu is not too much in advance
    m_cpuWaitForGPUFrameCount = 0.f; // timer for frame wait
    // We can wait for the render or the flip frame count label.
    // Waiting on render is a less strong CPU-GPU sync.
    // It can get up to 1 frame of latency if GPU has nothing to do (not supposed to be the case).
    GCM::Label * selectedSyncLabel = &m_gpuRenderFrameCountLabel;
    if (CPUSyncOnFlip)
    {
        selectedSyncLabel = &m_gpuFlipFrameCountLabel;
    }
    u32 currentGpuFrame = 0;
    do 
    {
        currentGpuFrame = selectedSyncLabel->getValue();
        if (m_cpuFrameCount-currentGpuFrame <= MaxCPUFrameInAdvance)
        {
            break;
        }
        sys_timer_usleep(300); // value used in samples
        m_cpuWaitForGPUFrameCount += 0.0003f;
    } while (1);

    m_gpuRenderTimer.start(m_rdrCtxt);

    // Now, our CPU can continue...
    GCM::VRAMAllocator::Update(m_cpuFrameCount, currentGpuFrame);

    // switch to the next render target 
    m_backBufferIndex = (m_backBufferIndex + 1u) % m_nbFrameBuffer;
    m_currentRenderTarget = NULL; // force reset of the render target as the flip changed it
    setRenderTarget(m_frameBuffers[m_backBufferIndex]);
    resetRenderStateAfterFlip();
}

void GFXAdapter_GCM::resetVertexDeclCache()
{
    // reset vertex decl
    for(u32 i = 0; i<GCM::VertexDecl::NbMaxVertexInputAttributes; ++i)
    {
        m_lastUsageSet[i] = false;
        m_rdrCtxt.gcmCtxt->SetVertexDataArrayFormat(i, 0, 0, 0, CELL_GCM_VERTEX_F);
    }
    m_lastVertexDecl = NULL;
    m_lastVertexStride = 0;
}

void GFXAdapter_GCM::resetRenderStateAfterFlip()
{
    // after a flip, render states were reset by the OS.
    // Update internal render state caches and set rendering to our default state
    m_rdrCtxt.reset();

    // set the sampler state to default
    std::copy(m_defaultTexSamplerState, m_defaultTexSamplerState+NB_MAX_TEX_SAMPLER, m_currentTexSamplerState);
    for(u32 i = 0; i<NB_MAX_TEX_SAMPLER; ++i)
    {
        SamplerState & sampState = m_defaultTexSamplerState[i];
        sampState.minFilter = m_minLinearFilteringMode;
        sampState.bias = 0x1FFFu & u32(m_texLodBiasFloat*256.f);
        m_rdrCtxt.gcmCtxt->SetTextureControlAlphaKill(i, sampState.activated, 0, 12<<8, CELL_GCM_TEXTURE_MAX_ANISO_1, sampState.alphaKill);
        m_rdrCtxt.gcmCtxt->SetTextureAddress(i, sampState.wrapU, sampState.wrapV, CELL_GCM_TEXTURE_CLAMP_TO_EDGE, CELL_GCM_TEXTURE_UNSIGNED_REMAP_NORMAL, CELL_GCM_TEXTURE_ZFUNC_NEVER, 0);
        m_rdrCtxt.gcmCtxt->SetTextureFilter(i, sampState.bias, sampState.minFilter, sampState.magFilter, CELL_GCM_TEXTURE_CONVOLUTION_QUINCUNX);
        m_rdrCtxt.gcmCtxt->SetTextureOptimization(i, m_linearFilteringOptimization, CELL_GCM_TEXTURE_ISO_HIGH, CELL_GCM_TEXTURE_ANISO_HIGH);
    }

    resetVertexDeclCache();

    m_alphaTestState = bfalse;
    m_rdrCtxt.gcmCtxt->SetAlphaFunc(CELL_GCM_GEQUAL, 1);

    m_currentVertexStream = VertexStream();

    restoreDefaultShader();
    resetSceneResolvedFlag();
}

//----------------------------------------------------------------------------//

void GFXAdapter_GCM::startRendering()
{
    setRenderTarget(m_frameBuffers[m_backBufferIndex]);
}

//----------------------------------------------------------------------------//

void GFXAdapter_GCM::endRendering()
{
 
}

//----------------------------------------------------------------------------//

bbool GFXAdapter_GCM::resetDevice(bbool _forceReset)
{
    return btrue;
}

//----------------------------------------------------------------------------//

bbool GFXAdapter_GCM::isDeviceLost()
{
    return bfalse;
}

///----------------------------------------------------------------------------//
/// Vertex Format.
///----------------------------------------------------------------------------//

void GFXAdapter_GCM::setVertexFormat( u32 _vformat )
{
    switch(_vformat)
    {
    default:
    case VertexFormat_PCT:
        m_currentVertexDecl = &mp_VDcl_PCT;
        m_currentVertexStream.stride = sizeof(VertexPCT);
        if (mp_currentShader == mp_defaultShader || getOverDrawMode() > 1 || isUseFog())
        {
            mp_currentShader->m_selectedTech = 0;
        }
        break;
    case VertexFormat_PC:
        m_currentVertexDecl = &mp_VDcl_PC;
        m_currentVertexStream.stride = sizeof(VertexPC);
        if (mp_currentShader == mp_defaultShader || getOverDrawMode() > 1 || isUseFog())
        {
            mp_currentShader->m_selectedTech = 1;
        }
        break;
    case VertexFormat_PT:
        m_currentVertexDecl = &mp_VDcl_PT;
        m_currentVertexStream.stride = sizeof(VertexPT);
        if (mp_currentShader == mp_defaultShader || getOverDrawMode() > 1 || isUseFog())
        {
            mp_currentShader->m_selectedTech = 2;
        }
        break;
    case VertexFormat_PTa:
        m_currentVertexDecl = &mp_VDcl_PT;
        m_currentVertexStream.stride = sizeof(VertexPT);
        if (mp_currentShader == mp_defaultShader || getOverDrawMode() > 1 || isUseFog())
        {
            mp_currentShader->m_selectedTech = 11;
        }
        break;
    case VertexFormat_PCBT:
        m_currentVertexDecl = &mp_VDcl_PCBT;
        m_currentVertexStream.stride = sizeof(VertexPCBT);
        break;
    case VertexFormat_PNCT:
        m_currentVertexDecl = &mp_VDcl_PNCT;
        m_currentVertexStream.stride = sizeof(VertexPNCT);
        mp_currentShader->m_selectedTech = 4;
        break;
    case VertexFormat_PNC3T:
        m_currentVertexDecl = &mp_VDcl_PNC3T;
        m_currentVertexStream.stride = sizeof(VertexPNC3T);
        mp_currentShader->m_selectedTech = 5;
        break;
    case VertexFormat_P:
        m_currentVertexDecl = &mp_VDcl_P;
        m_currentVertexStream.stride = sizeof(VertexP);
        if (mp_currentShader == mp_defaultShader)
            mp_currentShader->m_selectedTech = 1;
        break;
    }
}

void GFXAdapter_GCM::createVertexBuffer(ITF_VertexBuffer* _vertexBuffer)
{
    ITF_ASSERT(_vertexBuffer->mp_VertexBuffer == 0);
    GCM::VertexBuffer * newVB = new GCM::VertexBuffer();
    newVB->vramHdl.AllocAligned(_vertexBuffer->m_nVertex * _vertexBuffer->m_structVertexSize, 128, GCM::VRAM_VertexBuffer);
    ITF_ASSERT(newVB->vramHdl.IsValid()); // OOM
    _vertexBuffer->mp_VertexBuffer = reinterpret_cast<u32>(newVB);
}

void GFXAdapter_GCM::releaseVertexBuffer(ITF_VertexBuffer* _vertexBuffer)
{
    ITF_ASSERT(_vertexBuffer);
    ITF_ASSERT(_vertexBuffer->mp_VertexBuffer);
    GCM::VertexBuffer * vb = reinterpret_cast<GCM::VertexBuffer *>(_vertexBuffer->mp_VertexBuffer);
    delete vb;
    _vertexBuffer->mp_VertexBuffer = NULL;
}

inline void GFXAdapter_GCM::setVertexBuffer(ITF_VertexBuffer* _vertexBuffer)
{
    ITF_ASSERT(_vertexBuffer);
    ITF_ASSERT(_vertexBuffer->mp_VertexBuffer);
    setVertexFormat(_vertexBuffer->m_vertexFormat);
    m_currentVertexStream.stride = _vertexBuffer->m_structVertexSize;
    GCM::VertexBuffer * vb = reinterpret_cast<GCM::VertexBuffer *>(_vertexBuffer->mp_VertexBuffer);
    if (_vertexBuffer->bIsDynamic())
        vb->markAsUsedAtFrame(m_cpuFrameCount);
    m_currentVertexStream.rsxOffset =  vb->vramHdl.GetOffset() + _vertexBuffer->m_offset;
    m_currentVertexStream.rsxLocation = vb->vramHdl.GetLocation();
}

void GFXAdapter_GCM::LockVertexBuffer(ITF_VertexBuffer* _vertexBuffer, void** _data, u32 _offset, u32 _size, u32 _flag)
{
    ITF_ASSERT(_vertexBuffer);
    // 0 size vertex buffer -> return NULL
    if (!_vertexBuffer->mp_VertexBuffer)
    {
        if (_data)
        {
            (*_data) = NULL;
        }
        return;
    }

    GCM::VertexBuffer * vb = reinterpret_cast<GCM::VertexBuffer *>(_vertexBuffer->mp_VertexBuffer);
    *_data = reinterpret_cast<void *>(vb->vramHdl.GetAddress());

    // be sure to wait until the GPU has finished the rendering
    if (_vertexBuffer->bIsDynamic())
    {
        u32 lastUsedFrame = vb->getLastUsedFrame();
        bool hasToWait = m_gpuRenderFrameCountLabel.getValue() <= lastUsedFrame;
        if (hasToWait)
        {
            do
            {
                sys_timer_usleep(300);
            } while (m_gpuRenderFrameCountLabel.getValue() <= lastUsedFrame);
            ITF_ASSERT_MSG(0, "The vertex buffer is locked while still being in the rendering commands. A gpu-cpu synchronisation is needed. Please use double or triple buffering to avoid that.");
        }
    }
}

void GFXAdapter_GCM::UnlockVertexBuffer(ITF_VertexBuffer* _vertexBuffer)
{

}

///----------------------------------------------------------------------------//
/// IndexBuffer.
///----------------------------------------------------------------------------//

void GFXAdapter_GCM::createIndexBuffer(ITF_IndexBuffer* _indexBuffer)
{
    ITF_ASSERT(_indexBuffer->mp_IndexBuffer == 0);
    GCM::IndexBuffer * newIB = new GCM::IndexBuffer();
    newIB->vramHdl.AllocAligned(_indexBuffer->m_nIndices * sizeof(u16), 128, GCM::VRAM_IndexBuffer);
    ITF_ASSERT(newIB->vramHdl.IsValid()); // OOM
    _indexBuffer->mp_IndexBuffer = reinterpret_cast<u32>(newIB);
}

void GFXAdapter_GCM::releaseIndexBuffer(ITF_IndexBuffer* _indexBuffer)
{
    ITF_ASSERT(_indexBuffer);
    ITF_ASSERT(_indexBuffer->mp_IndexBuffer);
    GCM::IndexBuffer * ib = reinterpret_cast<GCM::IndexBuffer *>(_indexBuffer->mp_IndexBuffer);
    delete ib;
    _indexBuffer->mp_IndexBuffer = NULL;
    _indexBuffer->m_nIndices = 0;
}

inline void GFXAdapter_GCM::setIndexBuffer(ITF_IndexBuffer* _indexBuffer)
{
    ITF_ASSERT(_indexBuffer);
    ITF_ASSERT(_indexBuffer->mp_IndexBuffer);
    GCM::IndexBuffer * ib = reinterpret_cast<GCM::IndexBuffer *>(_indexBuffer->mp_IndexBuffer);
    m_currentIBufferRSXOffset = ib->vramHdl.GetOffset();
    m_currentIBufferRSXLocation = ib->vramHdl.GetLocation();
}

void GFXAdapter_GCM::LockIndexBuffer( ITF_IndexBuffer* _indexBuffer, void** _data )
{
    ITF_ASSERT(_indexBuffer);
    ITF_ASSERT(_indexBuffer->mp_IndexBuffer);
    GCM::IndexBuffer * ib = reinterpret_cast<GCM::IndexBuffer *>(_indexBuffer->mp_IndexBuffer);
    *_data = (void*)ib->vramHdl.GetAddress();
     IMPROVE_THIS
    // missing gpu stall to be sure it's not used
}

void GFXAdapter_GCM::UnlockIndexBuffer( ITF_IndexBuffer* _indexBuffer )
{
    IMPLEMENT_THIS;
}

///----------------------------------------------------------------------------//
/// Draw Vertex/IndexBuffer.
///----------------------------------------------------------------------------//

void  GFXAdapter_GCM::DrawVertexBuffer( u32 _type, ITF_VertexBuffer* _vertexBuffer, u32 _vertexStart, u32 _vertexNumber)
{
    if(!_vertexBuffer || !_vertexBuffer->mp_VertexBuffer || !_vertexNumber)
        return;

    i32 type = 0;
    switch(_type)
    {
    default:
    case GFX_TRIANGLES: 
        type = CELL_GCM_PRIMITIVE_TRIANGLES;
        break;
    case GFX_POINTS:
        type = CELL_GCM_PRIMITIVE_POINTS;
        break;
    case GFX_LINES:
        type = CELL_GCM_PRIMITIVE_LINES;
        break;
    case GFX_LINE_STRIP:
        type = CELL_GCM_PRIMITIVE_LINE_STRIP;
        break;
    case GFX_TRIANGLE_STRIP:
        type = CELL_GCM_PRIMITIVE_TRIANGLE_STRIP;
        break;
    case GFX_TRIANGLE_FAN:
        type = CELL_GCM_PRIMITIVE_TRIANGLE_FAN;
        break;
    case GFX_QUADS:
        type = CELL_GCM_PRIMITIVE_QUADS;
        break;
    case GFX_QUAD_STRIP:
        type = CELL_GCM_PRIMITIVE_QUAD_STRIP;
        break;
    }

    setVertexBuffer( _vertexBuffer );
    beginShader(mp_currentShader);
    applyVertexDecl(m_currentVertexStream, *m_currentVertexDecl);
    m_rdrCtxt.gcmCtxt->SetDrawArrays(type, _vertexStart, _vertexNumber);
    DEBUG_SYNCH_RSX();
    endShader(mp_currentShader);
}

void GFXAdapter_GCM::DrawIndexedVertexBuffer( u32 _type, ITF_VertexBuffer* _vertexBuffer, ITF_IndexBuffer* _indexBuffer, u32 _indexNumber )
{
    if(!_vertexBuffer || !_vertexBuffer->mp_VertexBuffer || !_indexNumber) 
        return;

    i32 type = 0;
    switch(_type)
    {
    default:
    case GFX_TRIANGLES: 
        type = CELL_GCM_PRIMITIVE_TRIANGLES;
        break;
    case GFX_POINTS:
        type = CELL_GCM_PRIMITIVE_POINTS;
        break;
    case GFX_LINES:
        type = CELL_GCM_PRIMITIVE_LINES;
        break;
    case GFX_LINE_STRIP:
        type = CELL_GCM_PRIMITIVE_LINE_STRIP;
        break;
    case GFX_TRIANGLE_STRIP:
        type = CELL_GCM_PRIMITIVE_TRIANGLE_STRIP;
        break;
    case GFX_TRIANGLE_FAN:
        type = CELL_GCM_PRIMITIVE_TRIANGLE_FAN;
        break;
    }

    setIndexBuffer(_indexBuffer);
    setVertexBuffer( _vertexBuffer );
    beginShader(mp_currentShader);
    applyVertexDecl(m_currentVertexStream, *m_currentVertexDecl);
    m_rdrCtxt.gcmCtxt->SetDrawIndexArray(type, _indexNumber, CELL_GCM_DRAW_INDEX_ARRAY_TYPE_16, m_currentIBufferRSXLocation, m_currentIBufferRSXOffset);
    DEBUG_SYNCH_RSX();
    endShader(mp_currentShader);
}

void GFXAdapter_GCM::applyVertexDecl(const VertexStream & _stream, 
                                     const GCM::VertexDecl & _vertexDecl)
{
    applyVertexDecl(_stream.stride, _vertexDecl);

    // set the VB
    ITF_ASSERT(_stream.rsxOffset != u32(-1));
    ITF_ASSERT(_stream.stride != u32(-1));

    for (u32 i = 0, n = _vertexDecl.nbElements; i != n; ++i)
    {
        const GCM::VertexDecl::Element & vdeclElt = _vertexDecl.elementArray[i];
        m_rdrCtxt.gcmCtxt->SetVertexDataArrayOffset(vdeclElt.usage,
                                                    _stream.rsxLocation,
                                                    _stream.rsxOffset + vdeclElt.offset);
    }
}

void GFXAdapter_GCM::applyVertexDecl(u32 _stride, 
                                     const GCM::VertexDecl & _vertexDecl)
{
    if (m_lastVertexDecl != &_vertexDecl || m_lastVertexStride != _stride) // cache
    {
        m_lastVertexDecl = &_vertexDecl;
        m_lastVertexStride = _stride;

        bool usageSet[GCM::VertexDecl::NbMaxVertexInputAttributes];
        ITF_ASSERT(GCM::VertexDecl::lastUsageUsed<GCM::VertexDecl::NbMaxVertexInputAttributes);
        std::fill(usageSet, usageSet+GCM::VertexDecl::lastUsageUsed+1, false);

        for (u32 i = 0, n = _vertexDecl.nbElements; i != n; ++i)
        {
            const GCM::VertexDecl::Element & vdeclElt = _vertexDecl.elementArray[i];
            m_rdrCtxt.gcmCtxt->SetVertexDataArrayFormat(  vdeclElt.usage,
                                                          0,
                                                          _stride,
                                                          vdeclElt.declType.size,
                                                          vdeclElt.declType.baseType);
            usageSet[vdeclElt.usage] = true;
        }

        for(u32 i = 0; i<GCM::VertexDecl::lastUsageUsed+1; ++i)
        {
            if (m_lastUsageSet[i] && !usageSet[i])
            {
                // invalidate it
                m_rdrCtxt.gcmCtxt->SetVertexDataArrayFormat(i, 0, 0, 0, CELL_GCM_VERTEX_F);
            }
        }

        std::copy(usageSet, usageSet+GCM::VertexDecl::NbMaxVertexInputAttributes, m_lastUsageSet);
    }
}


///----------------------------------------------------------------------------//
/// Shader Patch.
///----------------------------------------------------------------------------//

void GFXAdapter_GCM::shaderPatchFlushSize(u32 _hdiv, u32 _vdiv, u32 _countPoint, u32 _pass, u32 _vdivToDraw /*= U32_INVALID*/)
{
    if (!_vdivToDraw)//don't draw empty patch
        return;

    ITF_ASSERT_MSG( _hdiv != 0,"shaderPatchFlushSize hdiv is 0");
    ITF_ASSERT_MSG( _vdiv != 0,"shaderPatchFlushSize vdiv is 0");

    if (!mp_shaderManager.getNumberVector4ConstToFlush(0)) return;

    /// Flush control Points.
    if (mp_currentShader->m_VectorArrayHandle[0] != (void *)FxParameterDB::InvalidHandle)
    {
        mp_currentShader->setVectorArray(0, (GFX_Vector4*)mp_shaderManager.getBufferFloat(0), mp_shaderManager.getNumberVector4ConstToFlush(0));
        mp_currentShader->commitChange();
    }
    else
    {
        ITF_ASSERT(0);
    }
    /// Flush vector color.
    if (mp_currentShader->m_VectorArrayHandle[1] != (void *)FxParameterDB::InvalidHandle)
    {
        mp_currentShader->setVectorArray(1, (GFX_Vector4*)mp_shaderManager.getBufferFloat(1), mp_shaderManager.getNumberVector4ConstToFlush(1));
        mp_currentShader->commitChange();
    }
    else
    {
        ITF_ASSERT(0);    
    }

    /// draw patch.
    u32 nb = (mp_shaderManager.getNumberVector4ConstToFlush(0)/_countPoint);
    if (nb == 1)
    {
        u32 prevVdiv = _vdiv;
        _vdiv = Min(_vdiv, _vdivToDraw);
        mp_currentShader->setFloat(2, (f32)prevVdiv / (f32)_vdiv);
    }
    else
    {
        mp_currentShader->setFloat(2, 1.f);
    }

    if (m_showWireFrame < 2)
    {
        ITF_ASSERT(0 == (m_drawFlag & DRWF_COPYTOTARGET));
        beginShader(mp_currentShader);
        applyVertexDecl(m_currentVertexStream, *m_currentVertexDecl);
        m_rdrCtxt.gcmCtxt->SetDrawIndexArray(CELL_GCM_PRIMITIVE_TRIANGLES, (_hdiv*_vdiv*2)*nb*3, CELL_GCM_DRAW_INDEX_ARRAY_TYPE_16, m_currentIBufferRSXLocation, m_currentIBufferRSXOffset);
        DEBUG_SYNCH_RSX();
        endShader(mp_currentShader);
    }

    mp_shaderManager.setCurBuffFloatPos(0, 0);
    mp_shaderManager.setVector4ConstToFlush(0, 0);
    mp_shaderManager.setCurBuffFloatPos(1, 0);
    mp_shaderManager.setVector4ConstToFlush(1, 0);
}

void GFXAdapter_GCM::prepareShaderPatch(GMatrix44* _matrix, f32 _z, u32 & _hdiv, u32 & _vdiv, GFX_BLENDMODE _blendMode /*= GFX_BLEND_ALPHA*/)
{
    prepareShaderPatchTech(_matrix, _z, _hdiv, _vdiv, 3, _blendMode);
}

void GFXAdapter_GCM::prepareShaderPatchTech(GMatrix44* _matrix, f32 _z, u32 & _hdiv, u32 & _vdiv, u32 _idTech, GFX_BLENDMODE _blendMode /*= GFX_BLEND_ALPHA*/ )
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

    m_worldViewProj.setWorld(*_matrix);

    m_UVmat = GMatrixIdentity;

    ITF_IndexBuffer *  indexBuffer  = getIndexBuffer(_hdiv, _vdiv);
    ITF_VertexBuffer * vertexBuffer = getVertexBuffer(_hdiv, _vdiv);
    ITF_ASSERT(indexBuffer && vertexBuffer);
    if (indexBuffer && vertexBuffer)
    {
        setIndexBuffer(indexBuffer);
        setVertexBuffer(vertexBuffer);
    } else // default _hdiv == 2 && _vdiv == 2
    {
        _hdiv = bufferTypes[0][0];
        _vdiv = bufferTypes[0][1];
        setIndexBuffer( m_internalIdxBufferTab[0]);
        setVertexBuffer(m_internalVtxBufferTab[0]);
    }

    /// set Vertex format
    setVertexFormat(VertexFormat_PT);

    mp_currentShader->m_selectedTech = _idTech;

    mp_shaderManager.setCurBuffFloatPos(0, 0);
    mp_shaderManager.setVector4ConstToFlush(0, 0);
    mp_shaderManager.setCurBuffFloatPos(1, 0);
    mp_shaderManager.setVector4ConstToFlush(1, 0);

    /// set pos z to const.
    mp_currentShader->setFloat(1, _z);
}


#define USE_OPTIMIZED_PATCH_VERSION 1  //prototype optimization

//Assume this function is optimized with LHS Removed
//-----------------------------------------------------------------------------------------------------------------------------------------    
#ifdef USE_OPTIMIZED_PATCH_VERSION 

void GFXAdapter_GCM::shaderDrawPatch32(const Texture *texture, const Vec2d *PointsNS, const ColorInteger* _lighttab)
{
    ITF_ASSERT(_lighttab);

    f32 ratio = 1.f;
    /// set Texture;
    if (texture)
    {
        internSetTextureBind(0, texture->m_adapterimplementationData);
        ratio = texture->getRatioXY();
        setSamplerAddressState(0, CELL_GCM_TEXTURE_WRAP, CELL_GCM_TEXTURE_WRAP);
    }

    /// The stride of float Buffer is:
    /// Vector2 PointsNS for patch Vertex, Vector2 PointsNS+16 that is for UV.
    /// this is for Vector4 shader: xy = vertex zw = uv.

    //        shaderBuffer localShaderBuffer;
    u32 moveOp = mp_shaderManager.getCurBuffFloatPos(0);
    for (u32 i = 0; i < 8; i++)
    {
        mp_shaderManager.setBufferFloatAt(0, moveOp++, PointsNS[i].m_x);
        mp_shaderManager.setBufferFloatAt(0, moveOp++, PointsNS[i].m_y);
        mp_shaderManager.setBufferFloatAt(0, moveOp++, PointsNS[i + 8].m_x);
        mp_shaderManager.setBufferFloatAt(0, moveOp++, PointsNS[i + 8].m_y * ratio);

    }

    mp_shaderManager.setCurBuffFloatPos(0,moveOp);

    /// number constant for control points is: f32 to vector4 -> 32 float / 4
    mp_shaderManager.addVector4ConstToFlush(0, 8);

    /// color.
    moveOp= mp_shaderManager.getCurBuffFloatPos(1);
    for (u32 i = 0; i < 4; i++)
    {
        mp_shaderManager.setBufferFloatAt(1, moveOp++, _lighttab[i].getF32Red());
        mp_shaderManager.setBufferFloatAt(1, moveOp++, _lighttab[i].getF32Green());
        mp_shaderManager.setBufferFloatAt(1, moveOp++, _lighttab[i].getF32Blue());
        mp_shaderManager.setBufferFloatAt(1, moveOp++ ,_lighttab[i].getF32Alpha());
    }

    mp_shaderManager.setCurBuffFloatPos(1,moveOp);

    mp_shaderManager.addVector4ConstToFlush(1, 4);
}

#else

/* Original version*/
void GFXAdapter_GCM::shaderDrawPatch32(const Texture *texture, const Vec2d *PointsNS, const ColorInteger* _lighttab)
{
    ITF_ASSERT(_lighttab);

    f32 ratio = 1.f;

    /// set Texture;
    if (texture)
    {
        internSetTextureBind(0, texture->m_adapterimplementationData);
        ratio = texture->getRatioXY();
        setSamplerAddressState(0, CELL_GCM_TEXTURE_WRAP, CELL_GCM_TEXTURE_WRAP);
    }

    /// The stride of float Buffer is:
    /// Vector2 PointsNS for patch Vertex, Vector2 PointsNS+16 that is for UV.
    /// this is for Vector4 shader: xy = vertex zw = uv.
    for (u32 i = 0; i < 8; i++)
    {
        mp_shaderManager.setBufferFloatAt(0, mp_shaderManager.getCurBuffFloatPos(0), PointsNS[i].m_x);
        mp_shaderManager.addCurBuffFloatPos(0, 1);
        mp_shaderManager.setBufferFloatAt(0, mp_shaderManager.getCurBuffFloatPos(0), PointsNS[i].m_y);
        mp_shaderManager.addCurBuffFloatPos(0, 1);
        mp_shaderManager.setBufferFloatAt(0, mp_shaderManager.getCurBuffFloatPos(0), PointsNS[i + 8].m_x);
        mp_shaderManager.addCurBuffFloatPos(0, 1);
        mp_shaderManager.setBufferFloatAt(0, mp_shaderManager.getCurBuffFloatPos(0), PointsNS[i + 8].m_y * ratio);
        mp_shaderManager.addCurBuffFloatPos(0, 1);
    }

    /// number constant for control points is: f32 to vector4 -> 32 float / 4
    mp_shaderManager.addVector4ConstToFlush(0, 8);

    /// color.
    for (u32 i = 0; i < 4; i++)
    {
        mp_shaderManager.setBufferFloatAt(1, mp_shaderManager.getCurBuffFloatPos(1), _lighttab[i].getF32Red());
        mp_shaderManager.addCurBuffFloatPos(1, 1);
        mp_shaderManager.setBufferFloatAt(1, mp_shaderManager.getCurBuffFloatPos(1), _lighttab[i].getF32Green());
        mp_shaderManager.addCurBuffFloatPos(1, 1);
        mp_shaderManager.setBufferFloatAt(1, mp_shaderManager.getCurBuffFloatPos(1), _lighttab[i].getF32Blue());
        mp_shaderManager.addCurBuffFloatPos(1, 1);
        mp_shaderManager.setBufferFloatAt(1, mp_shaderManager.getCurBuffFloatPos(1), _lighttab[i].getF32Alpha());
        mp_shaderManager.addCurBuffFloatPos(1, 1);
    }

    mp_shaderManager.addVector4ConstToFlush(1, 4);
}
#endif // USE_OPTIMIZED_PATCH_VERSION

void GFXAdapter_GCM::shaderPatchFlush(u32 _hdiv, u32 _vdiv, u32 _pass)
{
    shaderPatchFlushSize(_hdiv, _vdiv, 8, _pass);   
}

void GFXAdapter_GCM::shaderBezierPatchFlush(u32 _hdiv, u32 _vdiv, u32 _pass, u32 _vDivToDraw)
{
    shaderPatchFlushSize(_hdiv, _vdiv, 4, _pass, _vDivToDraw);   
}

void GFXAdapter_GCM::endShaderPatch()
{
    //endShader(mp_currentShader);

    mp_shaderManager.setCurBuffFloatPos(0, 0);
    mp_shaderManager.setVector4ConstToFlush(0, 0);
    mp_shaderManager.setCurBuffFloatPos(0, 0);
    mp_shaderManager.setVector4ConstToFlush(0, 0);
    mp_shaderManager.setCurBuffFloatPos(1, 0);
    mp_shaderManager.setVector4ConstToFlush(1, 0);
    mp_shaderManager.setCurBuffFloatPos(1, 0);
    mp_shaderManager.setVector4ConstToFlush(1, 0);
}

///----------------------------------------------------------------------------//
/// Shader Bezier Patch.
///----------------------------------------------------------------------------//

void GFXAdapter_GCM::prepareShaderBezierPatch(GMatrix44* _matrix, f32 _z, u32 & _hdiv, u32 & _vdiv, GFX_BLENDMODE _blendMode /*= GFX_BLEND_ALPHA*/)
{
    prepareShaderPatchTech(_matrix, _z, _hdiv, _vdiv, 10, _blendMode);
}

bbool GFXAdapter_GCM::shaderDrawBezierPatch32(Texture *texture, Vec2d *PointsNS, ColorInteger* _lighttab, f32* _widths, f32* _alphas, f32* _widths2 )
{
    ITF_ASSERT(_lighttab);

    f32 ratio = 1.f;

    /// set Texture;
    if (texture)
    {
        internSetTextureBind(0, texture->m_adapterimplementationData);
        ratio = texture->getRatioXY();
        setSamplerAddressState(0, CELL_GCM_TEXTURE_WRAP, CELL_GCM_TEXTURE_WRAP);
    }

    /// The stride of float Buffer is:
    /// Vector2 PointsNS for patch Vertex, Vector2 PointsNS + 8 that is for UV.
    /// this is for Vector4 shader: xy = vertex zw = uv.
    for (u32 i = 0; i < 4; i++)
    {
        mp_shaderManager.setBufferFloatAt(0, mp_shaderManager.getCurBuffFloatPos(0), PointsNS[i].m_x);
        mp_shaderManager.addCurBuffFloatPos(0, 1);
        mp_shaderManager.setBufferFloatAt(0, mp_shaderManager.getCurBuffFloatPos(0), PointsNS[i].m_y);
        mp_shaderManager.addCurBuffFloatPos(0, 1);
        mp_shaderManager.setBufferFloatAt(0, mp_shaderManager.getCurBuffFloatPos(0), PointsNS[i + 4].m_x);
        mp_shaderManager.addCurBuffFloatPos(0, 1);
        mp_shaderManager.setBufferFloatAt(0, mp_shaderManager.getCurBuffFloatPos(0), PointsNS[i + 4].m_y * ratio);
        mp_shaderManager.addCurBuffFloatPos(0, 1);
    }

    /// number constant for control points is: f32 to vector4 -> 32 float / 4
    mp_shaderManager.addVector4ConstToFlush(0, 4);

    /// color.
    for (u32 i = 0; i < 2; i++)
    {
        mp_shaderManager.setBufferFloatAt(1, mp_shaderManager.getCurBuffFloatPos(1), _lighttab[i].getF32Red());
        mp_shaderManager.addCurBuffFloatPos(1, 1);
        mp_shaderManager.setBufferFloatAt(1, mp_shaderManager.getCurBuffFloatPos(1), _lighttab[i].getF32Green());
        mp_shaderManager.addCurBuffFloatPos(1, 1);
        mp_shaderManager.setBufferFloatAt(1, mp_shaderManager.getCurBuffFloatPos(1), _lighttab[i].getF32Blue());
        mp_shaderManager.addCurBuffFloatPos(1, 1);
        mp_shaderManager.setBufferFloatAt(1, mp_shaderManager.getCurBuffFloatPos(1), _lighttab[i].getF32Alpha());
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

    return 1;
}

#define FILL_BUFFER_VECT(bufferVect, bufferPoint, _spline, splinePoint, _pos) \
    bufferVect[bufferPoint].m_x = _spline->GetNthPoint(splinePoint).m_x + _pos.m_x; \
    bufferVect[bufferPoint].m_y = _spline->GetNthPoint(splinePoint).m_y + _pos.m_y; \
    bufferVect[bufferPoint].m_z = _spline->GetNthPoint(splinePoint).m_z + _pos.m_z; \
    bufferVect[bufferPoint].m_w = _spline->getInterpAt(splinePoint) == Spline::interp_linear ? _spline->getTimeAt(splinePoint) : -_spline->getTimeAt(splinePoint); \
    bufferPoint++;

#define MAX_SPLINE_EFFECTIVE_POINTS 16
#define SPLINE_BUFFER_SIZE (MAX_SPLINE_EFFECTIVE_POINTS + 2) // 1 for the begin, 1 for the end

/// Spline Shader
void GFXAdapter_GCM::drawSpline(GMatrix44* _matrix, Texture* _texture, const Spline * _spline, const Vec3d &_pos, f32 _height)
{
    IMPLEMENTED_NOT_TESTED;
    if (!_spline || _spline->GetNumPoints() < 2)
        return;
    /// set Texture;
    setShader( mp_defaultShader );

    if (_texture)
    {
        internSetTextureBind(0, _texture->m_adapterimplementationData);
        setSamplerAddressState(0, CELL_GCM_TEXTURE_WRAP, CELL_GCM_TEXTURE_WRAP);
    }

    renderContext_Set();

    m_worldViewProj.setWorld(*_matrix);

    m_UVmat = GMatrixIdentity;

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

            if (mp_currentShader->m_VectorArrayHandle[0] != (void *)FxParameterDB::InvalidHandle)
            {
                mp_currentShader->setVectorArray(0, bufferVect, bufferPoint);
                mp_currentShader->setVector(0, &v0);
                mp_currentShader->setFloat(0, (f32)bufferPoint);  
                mp_currentShader->setFloat(1, (f32)_height);  
                mp_currentShader->commitChange();
            }
            else
            {
                ITF_ASSERT(0);
            }
            
            beginShader(mp_currentShader);
            applyVertexDecl(m_currentVertexStream, *m_currentVertexDecl);
            m_rdrCtxt.gcmCtxt->SetDrawIndexArray(CELL_GCM_PRIMITIVE_TRIANGLES, (hdiv*vdiv*2)*3, CELL_GCM_DRAW_INDEX_ARRAY_TYPE_16, m_currentIBufferRSXLocation, m_currentIBufferRSXOffset);
            DEBUG_SYNCH_RSX();
            endShader(mp_currentShader);

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

/// Fluid Shader.
void GFXAdapter_GCM::drawFluid(GMatrix44* _matrix, Texture* _texture, const FixedArray<u32, GFXADAPTER_FLUID_COLORTAB_ENTRY_COUNT> &_colorTab, const Vec2d _heightTab[], const u32 _heightTabSize, const FixedArray<Vec2d, 4> &_uvTab, u32 mode, f32 _delta1, f32 _delta2, f32 _stepCount )
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

    if (_texture)
    {
        internSetTextureBind(0, _texture->m_adapterimplementationData);
        setSamplerAddressState(0, CELL_GCM_TEXTURE_WRAP, CELL_GCM_TEXTURE_WRAP);
    }

    renderContext_Set();

    m_worldViewProj.setWorld(*_matrix);

    m_UVmat = GMatrixIdentity;

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
        mp_currentShader->commitChange();
    }
    else
    {
        ITF_ASSERT(0);
    }

    beginShader(mp_currentShader);
    applyVertexDecl(m_currentVertexStream, *m_currentVertexDecl);
    m_rdrCtxt.gcmCtxt->SetDrawIndexArray(CELL_GCM_PRIMITIVE_TRIANGLES, (hdiv*vLen*2)*3, CELL_GCM_DRAW_INDEX_ARRAY_TYPE_16, m_currentIBufferRSXLocation, m_currentIBufferRSXOffset);
    DEBUG_SYNCH_RSX();
    endShader(mp_currentShader);
}

///----------------------------------------------------------------------------//
//Movie
///----------------------------------------------------------------------------//

void GFXAdapter_GCM::drawMovie(GMatrix44* _matrix, f32 _alpha, ITF_VertexBuffer* _pVertexBuffer, Texture** _pTextureArray, u32 _countTexture)
{
    ITF_shader *pMovieShader = mp_shaderManager.getShaderByIndex(6);

    m_worldViewProj.push();
    
    setOrthoView(0.f, f32(getScreenWidth()), 0.f, f32(getScreenHeight()));
    if (_matrix)
    {
        m_worldViewProj.setWorld(*_matrix);
    }

    m_rdrCtxt.gcmCtxt->SetInvalidateTextureCache(CELL_GCM_INVALIDATE_TEXTURE );

    setAlphaBlend(GFX_BLEND_ALPHA);

    setShader( pMovieShader);
    pMovieShader->m_selectedTech = 0;
    pMovieShader->setFloat(0, _alpha);

    for (u32 index = 0;index<_countTexture;index++)
    {
        SetTextureBind(index, u32(_pTextureArray[index]));
        internSetTextureAdressingMode(index, GFXTADDRESS_CLAMP, GFXTADDRESS_CLAMP);
    }

    DrawVertexBuffer(GFX_QUADS, _pVertexBuffer, 0, 4); // just 1 quad

    m_worldViewProj.pop();
}

/// Trail 3d Shader.
void GFXAdapter_GCM::drawTrail3D(Texture* _texture, Color _color, const SafeArray<Vec3d> & _points, f32 _alphaBegin, f32 _alphaEnd, f32 _fadeLength)
{
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
        internSetTextureBind(0, _texture->m_adapterimplementationData);
        setSamplerAddressState(0, CELL_GCM_TEXTURE_WRAP, CELL_GCM_TEXTURE_WRAP);
    }

    renderContext_Set();

    m_worldViewProj.setWorld(GMatrixIdentity);
    m_UVmat = GMatrixIdentity;

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
        mp_currentShader->commitChange();
    }
    else
    {
        ITF_ASSERT(0);
    }

    beginShader(mp_currentShader);
    applyVertexDecl(m_currentVertexStream, *m_currentVertexDecl);
    m_rdrCtxt.gcmCtxt->SetDrawIndexArray(CELL_GCM_PRIMITIVE_TRIANGLES, (hdiv*(nbPoints/2-1)*2)*3, CELL_GCM_DRAW_INDEX_ARRAY_TYPE_16, m_currentIBufferRSXLocation, m_currentIBufferRSXOffset);
    DEBUG_SYNCH_RSX();
    endShader(mp_currentShader);

    // restore Default Color constant:
    static const GFX_Vector4 defaultColor(1.f, 1.f, 1.f, 1.f);
    mp_currentShader->setVector(0, &defaultColor);
}

///----------------------------------------------------------------------------//
/// UV.
///----------------------------------------------------------------------------//

void GFXAdapter_GCM::setUVAnim(GFX_UVANIM& _uvanim)
{
    setMatrixTransform(GFX_TEXTURE, &_uvanim.m_UVMat);
}

///----------------------------------------------------------------------------//
/// Render Context.
///----------------------------------------------------------------------------//
void GFXAdapter_GCM::renderContext_Set()
{
    if (rC_getZWRITE())
        depthMask(1);
    if (rC_getZWRITE())
        depthTest(1);
}

void GFXAdapter_GCM::renderContext_SetDefault()
{
    depthMask(0);
    depthTest(0);
    depthFunc(GFX_CMP_ALWAYS);
}


void GFXAdapter_GCM::setGfxMaterial( const GFX_MATERIAL& _gfxMat ) 
{
    m_drawFlag = 0;

    switch (_gfxMat.m_matType)
    {
    default:
    case GFX_MAT_DEFAULT:
        GFXAdapter_GCM::setGfxMatDefault();
        break;
    case GFX_MAT_FRIEZEANIM:
        {    
            GFXAdapter_GCM::setGfxMatDefault();

            FxParameterDB::Float4Hdl vhdl = FxParameterDB::Float4Hdl(mp_currentShader->m_VecHandle[1]);
            if (vhdl)
            {
                GFX_Vector4 vec4 = _gfxMat.m_matParams.getvParamsAt(0);
                m_rdrCtxt.paramDB.setFloat4(vhdl, (const FxParameterDB::Vector4&)vec4);
            }
        }
        break;

    case GFX_MAT_FRIEZEOVERLAY:
        {    
            setGfxMatDefault();
            mp_currentShader->m_selectedTech = 12;
        }
        break;

    case GFX_MAT_REFRACTION:
        GFXAdapter_GCM::setGfxMatRefraction(_gfxMat);
        break;

    case GFX_MAT_GLOW:
        AFTERFX_prepareObjectGlow();
        GFXAdapter_GCM::setGfxMatDefault();
        ITF_ASSERT(0); // not supported
        m_drawFlag |= DRWF_COPYTOTARGET;
        break;

    case GFX_MAT_ALPHAFADE:
        setGfxMatAlphaFade(_gfxMat);
        break;
    }

    /// set alpha blend.
    setAlphaBlend(_gfxMat.m_blendMode);

    /// set Texture if present
    if (_gfxMat.m_textureDiffuse.isValidResourceId())
    {
        Texture * texture = (Texture *)_gfxMat.m_textureDiffuse.getResource();
        ITF_ASSERT(texture);
        GFXAdapter_GCM::SetTexture(0, texture);
    }

    internSetTextureAdressingMode(0, _gfxMat.m_TexAdressingModeU, _gfxMat.m_TexAdressingModeV);

    if (isUseFog())
    {
        GFX_Vector4 fog;
        fog.m_x = m_colorFog.getRed();
        fog.m_y = m_colorFog.getGreen();
        fog.m_z = m_colorFog.getBlue();
        fog.m_w = m_colorFog.getAlpha(); // blend factor in alpha

        // color Fog.
        FxParameterDB::Float4Hdl vhdl = FxParameterDB::Float4Hdl(mp_currentShader->m_VecHandle[3]);
        if (vhdl)
        {
            m_rdrCtxt.paramDB.setFloat4(vhdl, (const FxParameterDB::Vector4&)fog);
        }
    }
}

void GFXAdapter_GCM::setBlendSeparateAlpha()
{
    ITF_ASSERT(0); // bugged -> need to keep the last blend state change and reset it when it changed

    if (m_useSeparateAlpha)
    {
        m_rdrCtxt.gcmCtxt->SetBlendFunc(CELL_GCM_SRC_ALPHA, CELL_GCM_ONE_MINUS_SRC_ALPHA, CELL_GCM_SRC_ALPHA, CELL_GCM_ONE);
    }
}

void GFXAdapter_GCM::setDefaultAlphaTest()
{
    setAlphaTest(bfalse);
    setAlphaRef();
}

void GFXAdapter_GCM::setAlphaTest(bbool _on)
{
    m_alphaTestState = _on;
    m_rdrCtxt.gcmCtxt->SetAlphaTestEnable(_on);
}

void GFXAdapter_GCM::setAlphaRef(int _ref)
{
    m_rdrCtxt.gcmCtxt->SetAlphaFunc(CELL_GCM_GEQUAL, _ref);
}

void GFXAdapter_GCM::setGfxMatDefault() 
{
    if (getOverDrawMode() > 1)
        setShader( mp_shaderManager.getShaderByIndex(4));
    else
    {
        if (isUseFog())
            setShader( mp_shaderManager.getShaderByIndex(5));
        else
            setShader(mp_defaultShader);
    }
}

void GFXAdapter_GCM::setGfxMatRefraction( const GFX_MATERIAL& _gfxMat ) 
{
    if (!getSceneResolvedFlag())
    {
        /// Copy SceneTexture To TexBuffer.
        copyCurrentColorBuffer(1);
        setSceneResolvedFlag(btrue);
    }

    setShader( mp_shaderManager.getShaderByIndex(2));
    mp_currentShader->m_selectedTech = 0;

    /// backbuffer_copy/refraction texture.
    if (!UseRefract2x2)
        internSetTextureBind(1, (u32)m_pFullRTtex[1]);
    else
        internSetTextureBind(1, (u32)m_pSwapTextureDown2x2[0]);
    setSamplerAddressState(1, CELL_GCM_TEXTURE_CLAMP_TO_EDGE, CELL_GCM_TEXTURE_CLAMP_TO_EDGE);

    /// try normal texture.
    if (!SetTextureResource(2, _gfxMat.m_textureNormal))
    {
        // if normal doesn't work
        // try texture Diffuse
        SetTextureResource(2, _gfxMat.m_textureDiffuse);
    }

    setSamplerAddressState(2, CELL_GCM_TEXTURE_WRAP, CELL_GCM_TEXTURE_WRAP);

    /// decal factor
    mp_currentShader->setFloat(0, _gfxMat.m_matParams.getfParamsAt(0));
    mp_currentShader->setFloat(1, _gfxMat.m_matParams.getfParamsAt(1));
    mp_currentShader->setFloat(2, _gfxMat.m_matParams.getfParamsAt(2));
    mp_currentShader->setFloat(3, _gfxMat.m_matParams.getfParamsAt(3));
}


const GMatrix44*  GFXAdapter_GCM::getViewMatrix()
{
    return (const GMatrix44*)&m_worldViewProj.getView();
}

static void	normalizePlane(Plane & _plane)
{
    float a = _plane.m_normal.getX();
    float b = _plane.m_normal.getY();
    float c = _plane.m_normal.getZ();
    // float d = _plane.m_w;
    // plane eq : ax + by + cz + d = 0

    float norm = sqrt(a*a + b*b + c*c);
    if (norm)
    {
        f32 invNorm = 1.f / norm;
        _plane.m_normal*= invNorm;
        _plane.m_constant *= invNorm;
    }
    else
    {
        _plane.m_normal.set(0.f, 0.f, 0.f);
        _plane.m_constant = 0.f;
    }
}

static void extractFrustumPlanes(   const Matrix44 & _mProj,
                                    Plane & _leftPlane,
                                    Plane & _rightPlane,
                                    Plane & _topPlane,
                                    Plane & _bottomPlane,
                                    Plane & _nearPlane)
{
    Vec3d xColumn(_mProj._m11, _mProj._m21, _mProj._m31);
    f32 xColumnConst = _mProj._m41;
    Vec3d yColumn(_mProj._m12, _mProj._m22, _mProj._m32);
    f32 yColumnConst = _mProj._m42;
    Vec3d zColumn(_mProj._m13, _mProj._m23, _mProj._m33);
    f32 zColumnConst = _mProj._m43;
    Vec3d wColumn(_mProj._m14, _mProj._m24, _mProj._m34);
    f32 wColumnConst = _mProj._m44;

    _leftPlane.m_normal = -wColumn-xColumn;
    _leftPlane.m_constant = -(-wColumnConst-xColumnConst);

    _rightPlane.m_normal = xColumn-wColumn;
    _rightPlane.m_constant = -(xColumnConst-wColumnConst);

    _bottomPlane.m_normal = -wColumn-yColumn;
    _bottomPlane.m_constant = -(-wColumnConst-yColumnConst);
    
    _topPlane.m_normal = yColumn-wColumn;
    _topPlane.m_constant = -(yColumnConst-wColumnConst);

    _nearPlane.m_normal = -zColumn;
    _nearPlane.m_constant = -(-zColumnConst);

    normalizePlane(_leftPlane);
    normalizePlane(_rightPlane);
    normalizePlane(_bottomPlane);
    normalizePlane(_topPlane);
    normalizePlane(_nearPlane);
}

void			GFXAdapter_GCM::updateCameraFrustumPlanes(Camera *_cam)
{
    Plane & nearP = _cam->m_frustumPlanes[Camera::CameraPlane_Near];

    extractFrustumPlanes(m_worldViewProj.getWorldViewProj(), 
                        _cam->m_frustumPlanes[Camera::CameraPlane_Left],
                        _cam->m_frustumPlanes[Camera::CameraPlane_Right],
                        _cam->m_frustumPlanes[Camera::CameraPlane_Top],
                        _cam->m_frustumPlanes[Camera::CameraPlane_Bottom],
                        nearP);
    // adjust with camera pos
    nearP.m_constant = nearP.m_normal.dot(_cam->getCorrectedPos());
}

} // namespace ITF
