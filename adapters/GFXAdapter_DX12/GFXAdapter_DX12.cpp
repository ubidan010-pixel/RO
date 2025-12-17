#include "precompiled_GFXAdapter_DX12.h"

#include "GFXAdapter_DX12.h"
#include "engine/display/View.h"
#include "engine/display/Camera.h"
#include "GFXAdapter_device_context_DX12.h"
#include "VRAMAllocator/VRAMAllocator.h"
#include "FxParameterDB_DX12.h"
#include "TextureDescriptorPool_DX12.h"
#include "SamplerDescriptorPool_DX12.h"
#include "Texture_DX12.h"
#include "RenderTarget_DX12.h"
#include "ITFToDX12_Helpers.h"
#include "VertexBuffer_DX12.h"
#include "core/file/FileServer.h"
#include "core/AdaptersInterfaces/FileManager.h"
#include "core/file/File.h"
#include "engine/resources/ResourceManager.h"
#include "engine/rasters/RastersManager.h"

#define IMPLEMENTED_NOT_TESTED ITF_ASSERT(0)

namespace ITF
{
    DECLARE_RASTER(drawMesh, RasterGroup_Misc, Color::red());
    DECLARE_RASTER(drawMeshPT, RasterGroup_Misc, Color::pink());

    void GFXAdapter_DX12::setCamera(float _x, float _y, float _z, Camera* _cam)
    {
        GFXAdapter::setCamera(_x, _y, _z, _cam);

        ITF_ASSERT(!_cam->isBiased()); // Right now, bias is only editor

        f32 nearPlane = 1.f;
        f32 farPlane = 1000.f;

        Vec2d topLeft = Vec2d::Zero;
        Vec2d botRight = Vec2d::One;
        nearPlane = _cam->m_near;
        farPlane = _cam->m_far;
        topLeft = _cam->m_topLeft;
        botRight = _cam->m_bottomRight;

        _cam->forcePosition(Vec3d(_x, _y, _z));

        f32 screenratio = (fabs(_cam->m_bottomRight.m_x - _cam->m_topLeft.m_x) * (f32)getScreenWidth()) / (fabs(_cam->m_bottomRight.m_y - _cam->m_topLeft.m_y) * (f32)getScreenHeight());

        float viewDistance = _z;

        Vec3d vEyePt(_cam->getX(), _cam->getY(), viewDistance);
        Vec3d vLookatPt(_cam->getX(), _cam->getY(), _z);
        Vec3d vUpVec(0.0f, 1.0f, 0.0f);
        Vec3d lookatOffset(0.f, 0.f, -1.f);
        vLookatPt += lookatOffset;

        m_worldViewProj.setWorld(GMatrixIdentity);
        m_worldViewProj.setViewLookAtRH((const Vec3d&)vEyePt, (const Vec3d&)vLookatPt, (const Vec3d&)vUpVec);
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

    void GFXAdapter_DX12::init2DRender()
    {
        DX12::pushMarker(getRenderingContext(), "init2DRender");

        setGlobalColor(Color::white());
        setAlphaBlend(GFX_BLEND_ALPHA);
        setOrthoView(0.f, (f32)getScreenWidth(), 0.f, (f32)getScreenHeight());
    }

    void GFXAdapter_DX12::end2DRender()
    {
        DX12::popMarker(getRenderingContext());
    }

    void GFXAdapter_DX12::init2DGuiRender()
    {
        DX12::pushMarker(getRenderingContext(), "init2DGuiRender");
        setOrthoView2DYNeg(0.f, (f32)getScreenWidth(), 0.f, (f32)getScreenHeight());
    }

    void GFXAdapter_DX12::setOrthoView(f32 _l, f32 _r, f32 _b, f32 _t)
    {
        m_worldViewProj.setWorld(GMatrixIdentity);
        m_worldViewProj.setView(GMatrixIdentity);
        m_worldViewProj.setProjOrtho(_l, _r, _b, _t);
        m_useRotateZCamera = bfalse;

        updateCameraFrustumPlanes(CAMERA);
    }

    //----------------------------------------------------------------------------//

    void  GFXAdapter_DX12::compute3DTo2D(const Vec3d& _in, Vec3d& _out)
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

    void  GFXAdapter_DX12::compute2DTo3D(const Vec3d& _in, Vec3d& _out)
    {
        GFX_Viewport viewp;
        viewp.m_x = 0;
        viewp.m_y = 0;
        viewp.m_width = getScreenWidth();
        viewp.m_height = getScreenHeight();
        viewp.m_minZ = 0.f;
        viewp.m_maxZ = 1.f;

        vec3dUnProject(&_out, &_in, &viewp, (const GMatrix44*)&m_worldViewProj.getViewProjInv());
    }

    void  GFXAdapter_DX12::coord2DTo3D(const Vec2d& _in2d, float _zPlane, Vec2d& _out3d, bool noTranslation)
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
        vec3dUnProject(&vout, &vin, &viewp, (const GMatrix44*)(noTranslation ? &m_worldViewProj.getViewNoTProjInv() : &m_worldViewProj.getViewProjInv()));

        _out3d.m_x = (f32)vout.m_x;
        _out3d.m_y = (f32)vout.m_y;
    }

    void GFXAdapter_DX12::drawScreenQuad(f32 _px, f32 _py, f32 _width, f32 _height, f32 _z, u32 _color, bbool _noTex)
    {
        drawScreenQuadCentroid(_px, _py, _width, _height, _z, _color, 0.f, _noTex);
    }

    void GFXAdapter_DX12::drawScreenQuadCentroid(f32 _px, f32 _py, f32 _width, f32 _height, f32 _z, u32 _color, f32 _centroid, bbool _noTex)
    {
        if (getOverDrawMode())
            return;

        m_worldViewProj.setWorld(GMatrixIdentity);

        setTextureAdressingMode(0, GFXTADDRESS_CLAMP, GFXTADDRESS_CLAMP);

        if (_noTex)
        {
            VertexPC quad[4];

            quad[0].setData(Vec3d(_px - _centroid, _py + _centroid, _z), _color);
            quad[1].setData(Vec3d(_px - _centroid, _py + _height + _centroid, _z), _color);
            quad[2].setData(Vec3d(_px + _width - _centroid, _py + _centroid, _z), _color);
            quad[3].setData(Vec3d(_px + _width - _centroid, _py + _height + _centroid, _z), _color);

            setVertexFormat(VertexFormat_PC);
            DrawPrimitive(GFX_TRIANGLE_STRIP, (void*)quad, 4);
        }
        else
        {
            VertexPCT quad[4];

            quad[0].setData(Vec3d(_px - _centroid, _py + _centroid, _z), Vec2d(0.f, 1.0f), _color);
            quad[1].setData(Vec3d(_px - _centroid, _py + _height + _centroid, _z), Vec2d(0.f, 0.f), _color);
            quad[2].setData(Vec3d(_px + _width - _centroid, _py + _centroid, _z), Vec2d(1.0f, 1.0f), _color);
            quad[3].setData(Vec3d(_px + _width - _centroid, _py + _height + _centroid, _z), Vec2d(1.0f, 0.0f), _color);

            setVertexFormat(VertexFormat_PCT);
            DrawPrimitive(GFX_TRIANGLE_STRIP, (void*)quad, 4);
        }

        setTextureAdressingMode(0, GFXTADDRESS_WRAP, GFXTADDRESS_WRAP);
    }

    void GFXAdapter_DX12::drawQuad2D(f32 _px, f32 _py, f32 _width, f32 _height, f32 _z, u32 _color, const Texture* _tex, const Vec2d* _uvStart, const Vec2d* _uvEnd)
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
            quad[0].setData(Vec3d(_px, py, _z), Vec2d(uvStart.m_x, uvEnd.m_y), _color);
            quad[1].setData(Vec3d(_px, py + _height, _z), uvStart, _color);
            quad[2].setData(Vec3d(_px + _width, py, _z), uvEnd, _color);
            quad[3].setData(Vec3d(_px + _width, py + _height, _z), Vec2d(uvEnd.m_x, uvStart.m_y), _color);

            internSetTexture(0, _tex);
            setVertexFormat(VertexFormat_PCT);
            m_UVmat = GMatrixIdentity; // PCT so no translation (done with the vertex uv)
            DrawPrimitive(GFX_TRIANGLE_STRIP, (void*)quad, 4);
        }
        else
        {
            VertexPC quad[4];
            quad[0].setData(Vec3d(_px, py, _z), _color);
            quad[1].setData(Vec3d(_px, py + _height, _z), _color);
            quad[2].setData(Vec3d(_px + _width, py, _z), _color);
            quad[3].setData(Vec3d(_px + _width, py + _height, _z), _color);

            setVertexFormat(VertexFormat_PC);
            DrawPrimitive(GFX_TRIANGLE_STRIP, (void*)quad, 4);
        }
    }

    void GFXAdapter_DX12::draw3DQuad(GFX_3DQUAD* _t_Quad)
    {
        VertexPCT quad[4];

        float sx = _t_Quad->m_size.m_x * 0.5f;
        float sy = _t_Quad->m_size.m_y * 0.5f;

        quad[0].setData(Vec3d(-sx, -sy, 0.f), Vec2d(0.0f, 1.0f), _t_Quad->m_color);
        quad[1].setData(Vec3d(sx, -sy, 0.f), Vec2d(1.0f, 1.0f), _t_Quad->m_color);
        quad[3].setData(Vec3d(sx, sy, 0.f), Vec2d(1.0f, 0.0f), _t_Quad->m_color);
        quad[2].setData(Vec3d(-sx, sy, 0.f), Vec2d(0.0f, 0.0f), _t_Quad->m_color);

        Matrix44 worldMat;
        M44_setMatrixTranslation(&worldMat, _t_Quad->m_pos.getX(), _t_Quad->m_pos.getY(), _t_Quad->m_pos.getZ());

        if (!_t_Quad->m_rotation.IsEqual(Vec3d::Zero, MTH_EPSILON))
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
            setShader(mp_shaderManager.getShaderByIndex(4));
        else
        {
            if (isUseFog())
                setShader(mp_shaderManager.getShaderByIndex(5));
            else
                setShader(mp_defaultShader);
        }

        setVertexFormat(VertexFormat_PCT);

        setAlphaBlend(_t_Quad->m_blendMode);
        DrawPrimitive(GFX_TRIANGLE_STRIP, (void*)quad, 4);
    }

    bbool GFXAdapter_DX12::loadTexture(Texture* _texture, const char* _path, u32 _alpha)
    {
        String filenameCooked = FILESERVER->getCookedName(_path);
        ITF::File* pBinary = ITF::FILEMANAGER->openFile(filenameCooked, ITF_FILE_ATTR_READ);

        if (!pBinary)
        {
            _texture->m_adapterimplementationData = NULL;
            return bfalse;
        }

        u64 lenFile = pBinary->getLength();
        u8* rawData = (u8*)Memory::alignedMalloc(size_t(lenFile), 128);
        pBinary->read(rawData, u32(lenFile)); // first copy
        ITF::FILEMANAGER->closeFile(pBinary);

        cleanupTexture(_texture);

        auto * dx12Texture = new DX12::Texture(DX12::Texture::createFromMemoryDDS(*m_textureDescriptorPool, *m_uploadBufferRequestManager, rawData, lenFile, _path));

        Memory::alignedFree(rawData);
        rawData = nullptr;

        _texture->m_adapterimplementationData = dx12Texture;
        _texture->m_pixFormat = dx12ToITFTexFormat(dx12Texture->getDesc().Format);

        setTextureSize(_texture, Size(u32(dx12Texture->getDesc().Width), u32(dx12Texture->getDesc().Height)));

        _texture->m_datasizeX = u32(dx12Texture->getDesc().Width);
        _texture->m_datasizeY = u32(dx12Texture->getDesc().Height);

        updateCachedScaleValues(_texture);

        RESOURCE_MANAGER->addResourceAsLoaded(_texture);

        return btrue;
    }

    void GFXAdapter_DX12::drawTriangle(const Vec2d& _p1, const Vec2d& _p2, const Vec2d& _p3, float _r, float _g, float _b, float _a, float _z)
    {
        VertexPC v[3];

        Color color(_a, _r, _g, _b);
        u32 intColor = color.getAsU32();
        v[0].setData(Vec3d(_p1.m_x, _p1.m_y, _z), intColor);
        v[1].setData(Vec3d(_p2.m_x, _p2.m_y, _z), intColor);
        v[2].setData(Vec3d(_p3.m_x, _p3.m_y, _z), intColor);

        setAlphaBlend(GFX_BLEND_ALPHA);
        setShader(mp_defaultShader);
        m_worldViewProj.setWorld(GMatrixIdentity);
        setVertexFormat(VertexFormat_PC);
        DrawPrimitive(GFX_TRIANGLES, (void*)v, 3);
    }

    //----------------------------------------------------------------------------//

    void GFXAdapter_DX12::drawTriangle(const Vertex& _v1, const Vertex& _v2, const Vertex& _v3, const Texture* _tex, bbool _showWireFrame)
    {
        NOT_IMPLEMENTED_ON_NINTENDO

        VertexPCT v[3];

        m_worldViewProj.setWorld(GMatrixIdentity);

        v[0].setData(Vec3d(_v1.m_pos.m_x, _v1.m_pos.m_y, _v1.m_pos.m_z), Vec2d(_v1.m_uv.m_x, _v1.m_uv.m_y), _v1.m_color);
        v[1].setData(Vec3d(_v2.m_pos.m_x, _v2.m_pos.m_y, _v2.m_pos.m_z), Vec2d(_v2.m_uv.m_x, _v2.m_uv.m_y), _v2.m_color);
        v[2].setData(Vec3d(_v3.m_pos.m_x, _v3.m_pos.m_y, _v3.m_pos.m_z), Vec2d(_v3.m_uv.m_x, _v3.m_uv.m_y), _v3.m_color);

        setAlphaBlend(GFX_BLEND_ALPHA);
        internSetTexture(0, _tex);
        setVertexFormat(VertexFormat_PCT);
        DrawPrimitive(GFX_TRIANGLES, (void*)v, 3);
        ITF_ASSERT(!_showWireFrame); // wireframe mode not supported
    }

    //----------------------------------------------------------------------------//

#ifndef ITF_CONSOLE_FINAL
    void GFXAdapter_DX12::drawFlatMesh(const ITF_Mesh& _mesh)
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

    void GFXAdapter_DX12::setGlobalColor(const Color& _color)
    {
        setGFXGlobalColor(_color);

        Float4 globalColorVec;
        globalColorVec.m_x = _color.getRed();
        globalColorVec.m_y = _color.getGreen();
        globalColorVec.m_z = _color.getBlue();
        globalColorVec.m_w = _color.getAlpha(); // blend factor in alpha

        if (DX12::FxParameterDB::Float4Hdl(mp_currentShader->m_VecHandle[0]).isValid())
            m_paramDB.setFloat4(DX12::FxParameterDB::Float4Hdl(mp_currentShader->m_VecHandle[0]), globalColorVec);
    }

    void GFXAdapter_DX12::setFogColor(const Color& _color)
    {
        m_colorFog = _color;
        if (_color.getAlpha())
        {
            setUseFog(btrue);
            setShader(mp_shaderManager.getShaderByIndex(5));
        }
        else
        {
            setUseFog(bfalse);
            if (getOverDrawMode() > 1)
                setShader(mp_shaderManager.getShaderByIndex(4));
            else
                setShader(mp_defaultShader);
        }
    }

    void GFXAdapter_DX12::drawMesh(ITF_Mesh& _mesh, bbool _showWireFrame)
    {
        ITF_ASSERT(!_showWireFrame); // Not implemented

        m_worldViewProj.setWorld(_mesh.getMatrix());

#ifndef VBMANAGER_USE_DYNAMICRING_VB
        ITF_VertexBuffer* pVertexBuffer = _mesh.m_vtxBuffer ? _mesh.m_vtxBuffer : _mesh.m_vtxBufferBuffered[_mesh.m_userBuffer];
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
                if (mp_currentShader->m_VectorArrayHandle[0])
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

    void GFXAdapter_DX12::drawMeshPT(const ITF_Mesh& _mesh, bbool _showWireFrame)
    {
        TIMERASTER_SCOPE(drawMeshPT);

        renderContext_Set();

        m_worldViewProj.setWorld(_mesh.getMatrix());

        /// set Vertex format
        // setVertexFormat(VertexFormat_PT); // can be PT or PTa

#ifndef VBMANAGER_USE_DYNAMICRING_VB
        ITF_VertexBuffer* pVertexBuffer = _mesh.m_vtxBuffer ? _mesh.m_vtxBuffer : _mesh.m_vtxBufferBuffered[_mesh.m_userBuffer];
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

    void GFXAdapter_DX12::draw2dLine(const Vec2d& _p1, const Vec2d& _p2, float _size, u32 _color)
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

    void GFXAdapter_DX12::draw2dTriangle(const Vec2d& _p1, const Vec2d& _p2, const Vec2d& _p3, u32 _color, i32 _rank)
    {
        IMPLEMENTED_NOT_TESTED;

        VertexPC v[3];

        Triangle2D tri;

        const f32 p1y = (f32)getScreenHeight() - _p1.m_y;
        const f32 p2y = (f32)getScreenHeight() - _p2.m_y;
        const f32 p3y = (f32)getScreenHeight() - _p3.m_y;

        tri.v[0].m_pos.m_x = _p1.m_x;
        tri.v[0].m_pos.m_y = p1y;
        tri.v[1].m_pos.m_x = _p2.m_x;
        tri.v[1].m_pos.m_y = p2y;
        tri.v[2].m_pos.m_x = _p3.m_x;
        tri.v[2].m_pos.m_y = p3y;

        tri.v[0].m_color = _color;
        tri.v[1].m_color = _color;
        tri.v[2].m_color = _color;

        tri.m_rank = _rank;

        m_Triangle2DList.push_back(tri);
    }

    void GFXAdapter_DX12::draw2dBox(const Vec2d& _p, f32 _width, f32 _height, u32 _color0, u32 _color1, u32 _color2, u32 _color3, i32 _rank)
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

#ifndef ITF_CONSOLE_FINAL
    void GFXAdapter_DX12::drawCircle(float _x, float _y, float r, float _r, float _g, float _b, float _z, u32 _segsCount)
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
        for (u32 i = 0; i <= _segsCount; i++)
        {
            f32 fi = i * MTH_2PI / (float)_segsCount;
            v[i].setData(Vec3d((f32)cos(fi) * r + _x, (f32)sin(fi) * r + _y, _z), intColor);
        }

        m_worldViewProj.setWorld(GMatrixIdentity);

        /// set Vertex format
        setVertexFormat(VertexFormat_PC);

        /// draw geometrie.
        DrawPrimitive(GFX_LINE_STRIP, (void*)v, _segsCount + 1);
    }

    //----------------------------------------------------------------------------//
    void GFXAdapter_DX12::drawBox(const Vec2d& _pos, f32 _angle, const Vec2d& _extent, f32 _r, f32 _g, f32 _b, float _z, bbool _fill)
    {
        /// set first the shader you want to use.
        setShader(mp_defaultShader);

        VertexPC v[5];
        Color color(1.f, _r, _g, _b);
        u32 intColor = color.getAsU32();

        Vec2d p = Vec2d(-_extent.m_x, _extent.m_y).Rotate(_angle) + _pos;
        v[0].setData(Vec3d(p.m_x, p.m_y, _z), intColor);
        p = Vec2d(_extent.m_x, _extent.m_y).Rotate(_angle) + _pos;
        v[1].setData(Vec3d(p.m_x, p.m_y, _z), intColor);
        p = Vec2d(_extent.m_x, -_extent.m_y).Rotate(_angle) + _pos;
        v[2].setData(Vec3d(p.m_x, p.m_y, _z), intColor);
        p = Vec2d(-_extent.m_x, -_extent.m_y).Rotate(_angle) + _pos;
        v[3].setData(Vec3d(p.m_x, p.m_y, _z), intColor);
        p = Vec2d(-_extent.m_x, _extent.m_y).Rotate(_angle) + _pos;
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
            for (u32 i = 0; i < 4; i++)
            {
                v[i].m_color = ITFCOLOR_SET_A(v[i].m_color, (u8)0xaa);
            }
            DrawPrimitive(GFX_TRIANGLE_STRIP, (void*)v, 4);
        }
        else
            DrawPrimitive(GFX_LINE_STRIP, (void*)v, 5);
    }
    void GFXAdapter_DX12::drawSphere(float _x, float _y, float _z, float r, int lats, int longs, int _wireFrame, float _r, float _g, float _b)
    {
        IMPLEMENTED_NOT_TESTED;

        /// set first the shader you want to use.
        setShader(mp_defaultShader);

        /// fill vertex buffer.
        VertexPC v[65];
        Color color(1.f, _r, _g, _b);
        u32 intColor = color.getAsU32();

        for (u32 i = 0; i <= 64; i++)
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

    void GFXAdapter_DX12::drawSphere(float _x, float _y, float _radius, float _r, float _g, float _b, float _z, u32 _segsCount, bbool _volume, bbool _xaxis, bbool _yaxis, bbool _zaxis)
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

        if (_volume)
        {
            for (u32 i = 0; i <= _segsCount; i++)
            {
                f32 fi = i * MTH_2PI / (float)_segsCount;
                v[i].setData(Vec3d((f32)cos(fi) * _radius, (f32)sin(fi) * _radius, 0.f), intColor);
            }

            Matrix44 viewMat = m_worldViewProj.getView();
            Matrix44 invViewMat;
            M44_matrixInverse(invViewMat.fa16, viewMat.fa16);
            invViewMat.setTranslation(Vec3d(_x, _y, _z));

            m_worldViewProj.setWorld(invViewMat);

            DrawPrimitive(GFX_LINE_STRIP, (void*)v, _segsCount + 1);
        }

        m_worldViewProj.setWorld(GMatrixIdentity);

        if (_xaxis)
        {
            for (u32 i = 0; i <= _segsCount; i++)
            {
                f32 fi = i * MTH_2PI / (float)_segsCount;
                v[i].setData(Vec3d(_x, (f32)cos(fi) * _radius + _y, (f32)sin(fi) * _radius + _z), intColor);
            }

            DrawPrimitive(GFX_LINE_STRIP, (void*)v, _segsCount + 1);
        }

        if (_yaxis)
        {
            for (u32 i = 0; i <= _segsCount; i++)
            {
                f32 fi = i * MTH_2PI / (float)_segsCount;
                v[i].setData(Vec3d((f32)cos(fi) * _radius + _x, _y, (f32)sin(fi) * _radius + _z), intColor);
            }

            DrawPrimitive(GFX_LINE_STRIP, (void*)v, _segsCount + 1);
        }

        if (_zaxis)
        {
            for (u32 i = 0; i <= _segsCount; i++)
            {
                f32 fi = i * MTH_2PI / (float)_segsCount;
                v[i].setData(Vec3d((f32)cos(fi) * _radius + _x, (f32)sin(fi) * _radius + _y, _z), intColor);
            }

            DrawPrimitive(GFX_LINE_STRIP, (void*)v, _segsCount + 1);
        }
    }
#endif // ITF_CONSOLE_FINAL


    void GFXAdapter_DX12::setAlphaBlend(GFX_BLENDMODE _blend)
    {
        u_CurrentBlendingMode = _blend;

        if (getOverDrawMode() > 1)
            _blend = GFX_BLEND_ALPHA;

        m_currentBlendState.set(_blend);
    }

    void GFXAdapter_DX12::setFillMode(GFX_FILLMODE _fillmode)
    {
        m_currentFillMode = _fillmode;

        ITF_ASSERT_CRASH(m_currentFillMode == GFX_FILL_SOLID, "Only FILL mode is currently supported (no PSO variant created in DX12::Technique)");
    }


    void GFXAdapter_DX12::depthMask(u32 _depthMask)
    {
        ITF_ASSERT_CRASH(_depthMask == 0, "No depth mask supported (no depth buffer support)");
    }

    void GFXAdapter_DX12::depthTest(u32 _test)
    {
        ITF_ASSERT_CRASH(_test == 0, "No depth test supported (no depth buffer support)");
    }

    void GFXAdapter_DX12::SetTextureBind(int _Sampler, void* _bind, bbool _linearFiltering)
    {
        DX12::Texture* texDX12 = reinterpret_cast<DX12::Texture *>(_bind);
        internSetTextureBind(static_cast<u32>(_Sampler), texDX12, _linearFiltering!=0);
    }

    void GFXAdapter_DX12::SetTexture(int _sampler, Texture* _texture, bbool _linearFiltering)
    {
        internSetTexture(_sampler, _texture, _linearFiltering != 0);
    }

    void GFXAdapter_DX12::internSetTexture(int _sampler, const Texture* _texture, bool _linearFiltering)
    {
        DX12::Texture* texDX12 = nullptr;
        if (_texture != nullptr && _texture->m_adapterimplementationData != nullptr)
            texDX12 = reinterpret_cast<DX12::Texture*>(_texture->m_adapterimplementationData);

        internSetTextureBind(static_cast<u32>(_sampler), texDX12, _linearFiltering);
    }

    void GFXAdapter_DX12::internSetTextureBind(u32 _samplerIdx, DX12::Texture * _textureToBind, bool _linearFiltering)
    {
        ITF_ASSERT(_samplerIdx < GFX_NB_MAX_SAMPLER);

        if (_textureToBind)
        {
            _textureToBind->ensureUploadDone(*m_uploadBufferRequestManager);
            _textureToBind->markAsUsedAtFrame(m_mainContext->getNbSubmittedFrame() + 1);
        }

        DX12::FxParameterDB::TexSamplerHdl texAndSamplerHDl{ mp_currentShader->m_TextureHandle[_samplerIdx] };
        m_paramDB.setTexture(texAndSamplerHDl, _textureToBind);
        auto samplerMode = m_paramDB.getSamplerMode(texAndSamplerHDl);
        samplerMode.linearFiltering = _linearFiltering;
        m_paramDB.setSamplerMode(texAndSamplerHDl, samplerMode);
    }

    void GFXAdapter_DX12::setRTAsTexture(u32 _samplerIdx, DX12::RenderTarget* _rtToBind, bool _linearFiltering)
    {
        ITF_ASSERT(_rtToBind != nullptr);
        if (_rtToBind != nullptr)
        {
            internSetTextureBind(_samplerIdx, _rtToBind->transitionToTexture(getRenderingContext()), _linearFiltering);
        }
    }

    void GFXAdapter_DX12::lockTexture(Texture* _tex, LOCKED_TEXTURE* _lockTex, u32 _flag)
    {
        DX12::Texture* dx12 = static_cast<DX12::Texture*>(_tex->m_adapterimplementationData);
        D3D12_RANGE range{ 0, 0 };
        void* ptr = nullptr;
        HRESULT hr = dx12->getResource()->Map(0, &range, &ptr);
        ITF_ASSERT(SUCCEEDED(hr));
        _lockTex->mp_Bits = ptr;

        const u32 bpp = 1;
        const u32 rowSize = _tex->getSizeX() * bpp;

        constexpr u32 PitchAlign = D3D12_TEXTURE_DATA_PITCH_ALIGNMENT;
        const u32 rowPitch = (rowSize + (PitchAlign - 1)) & ~(PitchAlign - 1);

        _lockTex->m_pitch = rowPitch;
    }

    //----------------------------------------------------------------------------//

    void GFXAdapter_DX12::unlockTexture(Texture* _tex)
    {
        DX12::Texture* dx12 = static_cast<DX12::Texture*>(_tex->m_adapterimplementationData);
        D3D12_RANGE written{ 0, 0 };
        dx12->getResource()->Unmap(0, &written);
    }

    //----------------------------------------------------------------------------//

    void GFXAdapter_DX12::DrawPrimitive(PRIMITIVETYPE _type, const void* _pVertex, u32 _nbVertex)
    {
        if (isUseFog())
        {
            Float4 fog;
            fog.m_x = m_colorFog.getRed();
            fog.m_y = m_colorFog.getGreen();
            fog.m_z = m_colorFog.getBlue();
            fog.m_w = m_colorFog.getAlpha(); // blend factor in alpha

            // color Fog.
            DX12::FxParameterDB::Float4Hdl vhdl = DX12::FxParameterDB::Float4Hdl(mp_currentShader->m_VecHandle[3]);
            if (vhdl.isValid())
            {
                m_paramDB.setFloat4(vhdl, fog);
            }
        }

        // create local vertex buffer which will behind the scenes use DynamicVertexBuffer from
        // VertexBufferManager. Check LockVertexBuffer() function to see how it works.
        ITF_VertexBuffer localVBfr;

        localVBfr.initVertexBuffer(_nbVertex, toUnderlying(m_currentVertexFormat), DX12::getVertexByteSize(m_currentVertexFormat), /* _dynamic */ btrue);
        localVBfr.m_debugType = VB_T_INTERNAL;
#ifndef ITF_FINAL
        // m_bUseRingBuffer is not there in final builds (used for debugging)
        localVBfr.m_bUseRingBuffer = btrue;
#endif

        void* data = nullptr;
        getVertexBufferManager().LockVertexBuffer(&localVBfr, &data);
        ITF_ASSERT(data);
        ITF_Memcpy(data, _pVertex, localVBfr.m_size);
        getVertexBufferManager().UnlockVertexBuffer(&localVBfr);

        setVertexBuffer(&localVBfr);

        beginShader(mp_currentShader);

        if (isDrawAllowed())
        {
            applyVertexBuffer(m_currentVertexBuffer);
            getRenderingContext()->IASetPrimitiveTopology(itfToDX12PrimitiveType(_type));
            getRenderingContext()->DrawInstanced(_nbVertex, 1, 0, 0);
        }

        endShader(mp_currentShader);

        // Avoid vb destruction by object destructor. 
        // VB is a dynamic vertex buffer which belongs to VertexBufferManager.
        localVBfr.mp_VertexBuffer = 0;
    }

    void GFXAdapter_DX12::setMatrixTransform(MATRIXMODE _type, const GMatrix44* _matrix)
    {
        switch (_type)
        {
        default:
        case GFX_MWORLDTOWIEW:
        case GFX_MWORLD:
            m_worldViewProj.setWorld(*_matrix);
            break;
        case GFX_MVIEW:
            m_worldViewProj.setView(*_matrix);
            break;
        case GFX_PROJECTION:
            m_worldViewProj.setProj(*_matrix);
            break;
        case GFX_TEXTURE:
            m_UVmat = *_matrix;
            break;
        }
    }

    void GFXAdapter_DX12::setupViewport(GFX_RECT* _viewportRect)
    {
        ITF_ASSERT(_viewportRect != nullptr);

        if (_viewportRect == nullptr)
            return;

        m_lastSetViewPort = *_viewportRect;

        if (isFrameStarted())
        {
            DX12::setMarker(getRenderingContext(), "setViewport");

            D3D12_VIEWPORT  dx12ViewPort = itfRectToDX12Viewport(*_viewportRect);
            D3D12_RECT dx12Rect = itfRectToDX12Rect(*_viewportRect);

            getRenderingContext()->RSSetViewports(1, &dx12ViewPort);
            getRenderingContext()->RSSetScissorRects(1, &dx12Rect);
        }
    }

    //----------------------------------------------------------------------------//

    void GFXAdapter_DX12::getViewport(GFX_RECT* _RectOut)
    {
        if (_RectOut)
        {
            (*_RectOut) = m_lastSetViewPort;
        }
    }

    static bool isValidScissorForViewport(GFX_RECT _scissorRect, GFX_RECT _viewportRect)
    {
        bool scissorEmpty = (_scissorRect.right <= _scissorRect.left) || (_scissorRect.bottom <= _scissorRect.top);
        bool viewportEmpty = (_viewportRect.right <= _viewportRect.left) || (_viewportRect.bottom <= _viewportRect.top);

        bool scissorIntersectViewport =
            (_scissorRect.left < _viewportRect.right) &&
            (_scissorRect.right > _viewportRect.left) &&
            (_scissorRect.top < _viewportRect.bottom) &&
            (_scissorRect.bottom > _viewportRect.top);

        return !viewportEmpty && !scissorEmpty && scissorIntersectViewport;
    }

    void GFXAdapter_DX12::setScissorRect(GFX_RECT* _clipRect)
    {
        if (_clipRect == nullptr)
        {
            setupViewport(&m_lastSetViewPort);
        }
        else
        {
            if (isValidScissorForViewport(*_clipRect, m_lastSetViewPort))
            {
                D3D12_VIEWPORT  dx12ViewPort = itfRectToDX12Viewport(m_lastSetViewPort);
                getRenderingContext()->RSSetViewports(1, &dx12ViewPort);
                D3D12_RECT dx12Rect = itfRectToDX12Rect(*_clipRect);
                getRenderingContext()->RSSetScissorRects(1, &dx12Rect);
            }
            else
            {
                // If the scissor rect is not valid, set the viewport and scissor to 0.
                D3D12_VIEWPORT  dx12ViewPort = itfRectToDX12Viewport(GFX_RECT{ 0, 0, 0, 0 });
                getRenderingContext()->RSSetViewports(1, &dx12ViewPort);
                D3D12_RECT dx12Rect = itfRectToDX12Rect(GFX_RECT{ 0, 0, 0, 0 });
                getRenderingContext()->RSSetScissorRects(1, &dx12Rect);
            }
        }
    }

    void GFXAdapter_DX12::generateTexture(Texture* _texture)
    {
        // nothing to do ?
        ITF_ASSERT(0);
    }

    void GFXAdapter_DX12::createTexture(Texture* _texture, u32 _sizeX, u32 _sizeY, u32 _mipLevel, Texture::PixFormat _pixformat, u32 _pool, bbool _dynamic)
    {
        ITF_ASSERT(_pool == 1);
        ITF_ASSERT(_dynamic == false);

        DXGI_FORMAT format = itfToDX12TexFormat(_pixformat);
        if (_dynamic)
        {
            ITF_ASSERT(_mipLevel == 1); // Linear textures do not support multiple mipmap levels. 
        }

        String8 debugName;
        debugName.setTextFormat("createdTexture_%s", _texture->getPath().isEmpty() ? "NoName" : _texture->getPath().getBasename());

        auto * dx12Texture = new (MemoryId::mId_Textures) DX12::Texture();

        (*dx12Texture) = DX12::Texture::createCommitted(m_device,
            *m_textureDescriptorPool,
            _sizeX,
            _sizeY,
            format,
            D3D12_HEAP_TYPE_UPLOAD,
            debugName.cStr());

        _texture->m_adapterimplementationData = reinterpret_cast<void*>(dx12Texture);
        _texture->setSize(_sizeX, _sizeY);
        _texture->m_pixFormat = _pixformat;
        _texture->m_mipMapLevels = _mipLevel;
    }

    //----------------------------------------------------------------------------//

    void GFXAdapter_DX12::cleanupTexture(Texture* _texture)
    {
        if (_texture == nullptr || _texture->m_adapterimplementationData == nullptr)
            return;

        DX12::Texture* dx12Tex = reinterpret_cast<DX12::Texture*>(_texture->m_adapterimplementationData);
        delete dx12Tex;
        _texture->m_adapterimplementationData = nullptr;
    }

    void GFXAdapter_DX12::cleanupBaseTexture(Texture* _texture)
    {
        cleanupTexture(_texture);
    }

    void GFXAdapter_DX12::setTextureSize(Texture* _texture, const Size& _size)
    {
        _texture->setSize((u32)_size.d_width, (u32)_size.d_height);
    }

    unsigned char* GFXAdapter_DX12::grabTexture(Texture* _texture)
    {
        ITF_NOT_IMPLEMENTED();
        return 0;
    }

    u32 GFXAdapter_DX12::getMaxTextureSize()
    {
        return 16 * 1024;
    }

    void GFXAdapter_DX12::loadFromMemory(const void* _buffer, const Size& _texture_size, Texture* _texture, Texture::PixFormat _pixel_format, const u32 _buffer_size, bbool bCooked, u32 _mipmapCount)
    {
        ITF_NOT_IMPLEMENTED();
        // used by GFXAdapter::Text for (debug ?) font loading
    }


    void GFXAdapter_DX12::setSamplerState(u32 _samp, GFX_SAMPLERSTATETYPE _type, i32 _value)
    {
        ITF_NOT_IMPLEMENTED();
        // only called by GFXAdapter::setTextureAdressingMode, but setTextureAdressingMode is also overriden
    }

    void GFXAdapter_DX12::setTextureAdressingMode(u32 _samp, GFX_TEXADRESSMODE _texAdressModeU, GFX_TEXADRESSMODE _texAdressModeV)
    {
        DX12::FxParameterDB::TexSamplerHdl texAndSamplerHDl{ mp_currentShader->m_TextureHandle[_samp] };

        DX12::FxParameterDB::SamplerMode samplerMode = m_paramDB.getSamplerMode(texAndSamplerHDl);
        samplerMode.wrapModeU = _texAdressModeU;
        samplerMode.wrapModeV = _texAdressModeV;

        m_paramDB.setSamplerMode(texAndSamplerHDl, samplerMode);
    }

    void GFXAdapter_DX12::clear(u32 _buffer, f32 _r, f32 _g, f32 _b, f32 _a)
    {
        DX12::setMarker(getRenderingContext(), "clear");

        ITF_ASSERT((_buffer & GFX_CLEAR_COLOR) == GFX_CLEAR_COLOR); // only supported mode right now
        m_currentRenderTarget->clear(getRenderingContext(), Color(_a, _r, _g, _b));
    }

    //----------------------------------------------------------------------------//

    void GFXAdapter_DX12::printDebugInfoOnScreen()
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
            f32 invW = 1.f / f32(getScreenWidth());
            f32 invH = 1.f / f32(getScreenHeight());

            f32 scaleFontHeightRatio = 40.f; // Height of the font in pixel
            u32 pixelCharHeight = 12;

            //f32 dbgHOffset = 0.067f;
            f32 dbgHOffset = 0.0f;

            GFX_RECT rct;
            rct.left = 80;
            rct.right = 780;
            rct.top = getScreenHeight() - pixelCharHeight - pixelCharHeight / 2;
            rct.bottom = rct.top + pixelCharHeight;

            for (ITF_VECTOR<DBGText>::const_iterator it = m_DBGTexts.begin(), itEnd = m_DBGTexts.end();
                it != itEnd;
                ++it)
            {
                const DBGText& txt = *it;
                u32 color = Color(1.f, txt.m_r, txt.m_g, txt.m_b).getAsU32();
                StringConverter str(txt.m_text.cStr());

                if (txt.m_x < 0.0f && txt.m_y < 0.0f)
                {
                    rct.top -= pixelCharHeight;
                    rct.bottom = rct.top + pixelCharHeight;
                    cellDbgFontPrintf(f32(rct.left) * invW, dbgHOffset + f32(rct.top) * invH, pixelCharHeight * invH * scaleFontHeightRatio, color, "%s", str.getChar());
                }
                else
                {
                    u32 localPixelCharHeight = 12;
                    GFX_RECT localRect;
                    localRect.left = i32(txt.m_x);
                    localRect.right = getScreenWidth();
                    localRect.top = i32(txt.m_y);
                    localRect.bottom = rct.top + localPixelCharHeight;
                    cellDbgFontPrintf(f32(localRect.left) * invW, dbgHOffset + f32(localRect.top) * invH, localPixelCharHeight * invH * scaleFontHeightRatio, color, "%s", str.getChar());
                }
            }

            // FPS stuff
            f32 lastFrameTime = m_cpuTimeProfiler.Stop() * 1000.f;
            m_cpuTimeProfiler.Start();

            f32 cpuWaitForCBAvailable = ms_cpuWaitForCBAvailable * 1000.f;
            ms_cpuWaitForCBAvailable = 0.f;
            f32 cpuWaitForGPUFrameCount = m_cpuWaitForGPUFrameCount * 1000.f;

            // ABGR format
            const u32 warningColor = 0xff1C9Effu;
            const u32 errorColor = 0xff0000ffu;
            const u32 allOKColor = 0xff00ff00u;

            const f32 warningFrameTime = 13.f;
            const f32 errorFrameTime = 16.66f;
            const f32 referenceFps = 59.f; // 60 +/- 1 frame

            static f32 fpsXPos = 0.69f;

            bool gpuBound = cpuWaitForCBAvailable > 0.1f || cpuWaitForGPUFrameCount > 0.1f;
            f32 smoothedFps = getfPs();
            u32 fpsColor = smoothedFps < referenceFps ? errorColor : allOKColor;
            cellDbgFontPrintf(fpsXPos, 0.025f, 0.6f, fpsColor, "%5.2f fps (%5.2fms)", smoothedFps, lastFrameTime);
            f32 cpuTimeNoWait = lastFrameTime - cpuWaitForCBAvailable - cpuWaitForGPUFrameCount;
            u32 cpuColor = cpuTimeNoWait > errorFrameTime ? errorColor : (cpuTimeNoWait > warningFrameTime ? warningColor : allOKColor);
            if (!gpuBound)
            {
                cellDbgFontPuts(fpsXPos - 0.0076f, 0.025f + 0.014f, 0.6f, cpuColor, ">");
            }
            cellDbgFontPrintf(fpsXPos, 0.025f + 0.014f, 0.6f, cpuColor, "CPU %5.2fms Sync CB %5.2fms Frame %5.2fms",
                cpuTimeNoWait,
                cpuWaitForCBAvailable,
                cpuWaitForGPUFrameCount);

            f32 gpuRenderTime = m_gpuRenderTimer.getTimeMilliSec();
            f32 gpuFlipTime = m_gpuPresentTimer.getTimeMilliSec();
            u32 gpuColor = gpuRenderTime > errorFrameTime ? errorColor : (gpuRenderTime > warningFrameTime ? warningColor : allOKColor);
            if (gpuBound)
            {
                cellDbgFontPuts(fpsXPos - 0.0074f, 0.025f + 0.028f, 0.6f, gpuColor, ">");
            }
            cellDbgFontPrintf(fpsXPos, 0.025f + 0.028f, 0.6f, gpuColor, "GPU %5.2fms Flip %5.2fms",
                gpuRenderTime,
                gpuFlipTime);

            // print mem info
            sys_memory_info_t memInfo;
            sys_memory_get_user_memory_size(&memInfo);

            GCM::VRAMStats vramStats;
            GCM::VRAMAllocator::GetStats(vramStats);

            u32 memColor = (vramStats.vramFreeSize < 29 * 1024 * 1024 || memInfo.available_user_memory < 20 * 1024 * 1024) ?
                errorColor : allOKColor;
            cellDbgFontPrintf(fpsXPos, 0.025f + 0.028f + 0.014f, 0.6f, memColor, "MEM %6.1f VRAM %6.2f",
                f32((memInfo.total_user_memory - memInfo.available_user_memory) / 1024) / 1024.f,
                f32((vramStats.vramTotalSize - vramStats.vramFreeSize) / 1024) / 1024.f);
            cellDbgFontPrintf(fpsXPos, 0.025f + 0.028f + 0.014f + 0.014f, 0.6f, 0xff00ffffu,
                "Version %ls",
                m_engineDataVersion.cStr());
            cellDbgFontPrintf(fpsXPos, 0.025f + 0.028f + 0.042f, 0.6f, allOKColor, "%ls", m_extraInfo.cStr());
#ifndef ITF_FINAL
            extern ux g_quitTargetCount;
            extern ux g_quitCount;
            cellDbgFontPrintf(fpsXPos, 0.025f + 0.028f + 0.042f + 0.014f, 0.6f, allOKColor, "End in %d events", int(g_quitTargetCount) - int(g_quitCount));
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

    void GFXAdapter_DX12::syncGPU()
    {
        ITF_ASSERT_MSG(false, "syncing with GPU, this will stall the CPU frame until GPU finishes! Check why we are calling this function and fix the cause.");
        // This call is currently stalling the GPU, but the command buffer that is being filled is not submit.
        // So if you expect this method to reuse a buffer, it is not guaranteed.
        m_mainContext->waitUntilGPUStalled();
    }

    void GFXAdapter_DX12::present()
    {
        printDebugInfoOnScreen();

        m_textureDescriptorPool->recycleUnused(m_mainContext->getNBGPURenderedFrame());
        m_uniformBufferPool->advanceToNext(m_mainContext->getQueue());

        m_VertexBufferManager.DynamicRingVBEndDraw();

        getBackBufferRT()->transitionToPresent(getRenderingContext());

        // Copy during a potential CPU - GPU sync
        m_uploadBufferRequestManager->pushRequestedCopyCommands(); // trigger copy commands before flip
        // Ensure copy are done before rendering in case of lock => render scenario
        m_uploadBufferRequestManager->syncCopyCommandsWithGFX(getRenderingContext(), m_mainContext->getQueue());

    #ifdef ITF_WIN64
        m_mainContext->endFrameAndFlip(m_swapChain.Get(), m_waitVBL, m_frameBufferIndex);
    #else
        m_mainContext->endFrameAndFlip(getBackBufferRT()->getResource(), m_frameBufferIndex);
    #endif

        // Copy while not rendering
        m_uploadBufferRequestManager->pushRequestedCopyCommands(); // trigger copy commands after flip

        // flip buffers
        m_frameBufferIndex = (m_frameBufferIndex + 1) % FRAME_BUFFER_COUNT;
        ITF_ASSERT(m_frameBufferIndex == m_mainContext->getCurrentBufferIndex());
    }

    void GFXAdapter_DX12::startRendering()
    {
    #if defined(ITF_ENABLE_DX12_GRAPHICS_DEBUGGING) && ITF_ENABLE_DX12_GRAPHICS_DEBUGGING
        m_currentDrawCallIndexInFrame = 0;
    #endif

        u64 cpuFrameCount = m_mainContext->getNbSubmittedFrame();
        u64 gpuFrameCount = m_mainContext->getNBGPURenderedFrame();
        DX12::VRAMAllocator::update(cpuFrameCount, gpuFrameCount);

        checkShaderReload();

        m_mainContext->startNewFrame();

        m_uploadBufferRequestManager->pushRequestedCopyCommands();
        m_uploadBufferRequestManager->syncCopyCommandsWithGFX(getRenderingContext(),  m_mainContext->getQueue());


        DX12::pushMarker(getRenderingContext(), "startRendering");

        resetRenderingStates();
        resetSceneResolvedFlag(); // inform the parent (GFXAdapter) that a scene resolving will be needed at this new frame
        setRenderTarget(*getBackBufferRT());

        ID3D12DescriptorHeap* heaps[] = { m_textureDescriptorPool->getHeap(), m_samplerPool->getHeap()};
        getRenderingContext()->SetDescriptorHeaps(ITF_ARRAY_SIZE(heaps), heaps);

        m_VertexBufferManager.DynamicRingVBBeginDraw();
    }

    void GFXAdapter_DX12::endRendering()
    {
        // everything done in present()
        DX12::popMarker(getRenderingContext());

        // We sync here also to trigger copy command often
        m_uploadBufferRequestManager->pushRequestedCopyCommands();
    }

    void GFXAdapter_DX12::resetRenderingStates()
    {
        // Called at the beginning of each frame
        m_worldViewProj.setWorld(GMatrixIdentity);
        mp_currentShader = mp_defaultShader;
        m_currentAlphaTestRef = 0.f;
        m_currentVertexFormat = DX12::VertexFormat::PCT;
        m_currentVertexBuffer = nullptr;
        m_currentIndexBuffer = nullptr;
        m_UVmat = GMatrixIdentity;
        m_lastSetViewPort = GFX_RECT{ 0, 0, int(getScreenWidth()), int(getScreenHeight()) };
        setupViewport(&m_lastSetViewPort);

        m_currentBlendState.reset();
        m_psoCache.reset();

        m_paramDB.setFloat(m_texelRatioBlendHdl, m_texelRatioBlend);

        setFillMode(GFX_FILL_SOLID);
    }

    bbool GFXAdapter_DX12::isDeviceLost()
    {
    #ifdef ITF_WIN64
        // Todo for PC
        ITF_ASSERT(0);
    #endif // ITF_WIN64
        return bfalse;
    }

    bbool GFXAdapter_DX12::resetDevice(bbool _forceReset)
    {
    #ifdef ITF_WIN64
        // Todo for PC
        ITF_ASSERT(0);
    #endif // ITF_WIN64
        // Resolution change not supported yet
        return btrue;
    }

    void GFXAdapter_DX12::onSuspendApp()
    {
        if (m_mainContext != nullptr)
        {
            m_mainContext->waitUntilGPUStalled();
            m_uploadBufferRequestManager->waitUntilAllCopyDone(getRenderingContext(), m_mainContext->getQueue());
            m_mainContext->getQueue()->SuspendX(0);
        }
    }

    void GFXAdapter_DX12::onResumeApp()
    {
        if (m_mainContext != nullptr)
        {
            m_mainContext->getQueue()->ResumeX();
        }
    }

    ///----------------------------------------------------------------------------//
    /// Vertex Buffer
    ///----------------------------------------------------------------------------//

    void GFXAdapter_DX12::setVertexFormat(u32 _vformat)
    {
        m_currentVertexFormat = static_cast<DX12::VertexFormat>(_vformat);
        if (mp_currentShader == mp_defaultShader || getOverDrawMode() > 1 || isUseFog())
        {
            mp_currentShader->m_selectedTech = DX12::getFXTechInDefaultShader(m_currentVertexFormat);
        }
    }

    void GFXAdapter_DX12::createVertexBuffer(ITF_VertexBuffer* _vertexBuffer)
    {
        ITF_ASSERT(_vertexBuffer->mp_VertexBuffer == 0);
        DX12::VertexBuffer* newVB = new (MemoryId::mId_Vertex) DX12::VertexBuffer();

        newVB->initDynamic(_vertexBuffer->m_size + _vertexBuffer->m_structVertexSize);

        ITF_ASSERT(newVB->getResource() != nullptr); // OOM?

        _vertexBuffer->mp_VertexBuffer = reinterpret_cast<uPtr>(newVB);
    }

    void GFXAdapter_DX12::releaseVertexBuffer(ITF_VertexBuffer* _vertexBuffer)
    {
        ITF_ASSERT(_vertexBuffer);
        ITF_ASSERT(_vertexBuffer->mp_VertexBuffer);
        DX12::VertexBuffer* vb = reinterpret_cast<DX12::VertexBuffer*>(_vertexBuffer->mp_VertexBuffer);
        delete vb;
        _vertexBuffer->mp_VertexBuffer = 0;
    }

    void GFXAdapter_DX12::setVertexBuffer(ITF_VertexBuffer* _vertexBuffer)
    {
        ITF_ASSERT(_vertexBuffer);
        ITF_ASSERT(_vertexBuffer->mp_VertexBuffer != 0);

        setVertexFormat(_vertexBuffer->m_vertexFormat);
        m_currentVertexBuffer = _vertexBuffer;
    }

    void GFXAdapter_DX12::applyVertexBuffer(ITF_VertexBuffer* _vb) // set the vertex buffer in the context (to call once the VS is set, so after beginShader)
    {
        ITF_ASSERT(_vb != nullptr && _vb->mp_VertexBuffer != 0);
        if (_vb != nullptr && _vb->mp_VertexBuffer != 0)
        {
            ITF_ASSERT(toUnderlying(m_currentVertexFormat) == _vb->m_vertexFormat); // $GB assert added, as it seems to be assumed
            DX12::VertexBuffer * dx12VB = reinterpret_cast<DX12::VertexBuffer*>(_vb->mp_VertexBuffer);

            dx12VB->ensureUploadDone(*m_uploadBufferRequestManager);
            dx12VB->markAsUsedAtFrame(m_mainContext->getNbSubmittedFrame() + 1u);

            D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
            vertexBufferView.StrideInBytes = DX12::getVertexByteSize(m_currentVertexFormat);
            vertexBufferView.BufferLocation = dx12VB->getResource()->GetGPUVirtualAddress() + _vb->m_offset;
            vertexBufferView.SizeInBytes = _vb->m_size;
            ITF_ASSERT(_vb->m_offset + _vb->m_size <= dx12VB->getByteSize());
            getRenderingContext()->IASetVertexBuffers(0, 1, &vertexBufferView);
        }
    }

    void GFXAdapter_DX12::LockVertexBuffer(ITF_VertexBuffer* _vertexBuffer, void** _data, u32 _offset, u32 _size, u32 _flag)
    {
        ITF_ASSERT(_vertexBuffer);

        // 0 size vertex buffer -> return NULL
        if (!_vertexBuffer->mp_VertexBuffer)
        {
            if (_data)
            {
                (*_data) = nullptr;
            }
            return;
        }

        ITF_ASSERT(_data);

        DX12::VertexBuffer* vb = reinterpret_cast<DX12::VertexBuffer*>(_vertexBuffer->mp_VertexBuffer);
        ITF_ASSERT_MSG(vb->isDynamic(), "This VB should have been created as Dynamic. Are you locking twice a VB not dynamic?");
        ITF_ASSERT_MSG((_flag & VB_LOCK_NOOVERWRITE)!=0 || i64(m_mainContext->getNBGPURenderedFrame()) >= vb->getLastUsedFrame(), "The vertex buffer is locked while still being in the rendering commands.");

        // Map only the requested range in bytes
        D3D12_RANGE range{};
        range.Begin = _offset;
        range.End = _offset + _size;
        void* mappedPtr = nullptr;
        vb->getResource()->Map(0, &range, &mappedPtr);
        *(_data) = mappedPtr != nullptr ? static_cast<u8*>(mappedPtr) + _offset : nullptr;
    }

    void GFXAdapter_DX12::UnlockVertexBuffer(ITF_VertexBuffer* _vertexBuffer)
    {
        ITF_ASSERT(_vertexBuffer);
        DX12::VertexBuffer* vb = reinterpret_cast<DX12::VertexBuffer*>(_vertexBuffer->mp_VertexBuffer);
        ITF_ASSERT_MSG(vb->isDynamic(), "This VB should have been created as Dynamic");
        vb->getResource()->Unmap(0, nullptr);

        if (!_vertexBuffer->bIsDynamic())
        {
            // not dynamic VB can be uploaded after unlock.
            // Dynamic VB are used directly from an UPLOAD heap, so no need to upload.
            vb->upload(*m_uploadBufferRequestManager);
        }
    }

    ///----------------------------------------------------------------------------//
    /// IndexBuffer.
    ///----------------------------------------------------------------------------//

    void GFXAdapter_DX12::createIndexBuffer(ITF_IndexBuffer* _indexBuffer)
    {
        ITF_ASSERT(_indexBuffer->mp_IndexBuffer == 0);
        DX12::IndexBuffer* newIB = new (ITF::MemoryId::mId_Vertex) DX12::IndexBuffer{};

        const uSize indexBufferSize = _indexBuffer->m_nIndices * sizeof(u16);
        newIB->initDynamic(indexBufferSize);

        ITF_ASSERT(newIB->getResource() != nullptr); // OOM?
        _indexBuffer->mp_IndexBuffer = reinterpret_cast<uPtr>(newIB);
    }

    void GFXAdapter_DX12::releaseIndexBuffer(ITF_IndexBuffer* _indexBuffer)
    {
        DX12::IndexBuffer * ib = reinterpret_cast<DX12::IndexBuffer *>(_indexBuffer->mp_IndexBuffer);
        delete ib;
        _indexBuffer->mp_IndexBuffer = 0;
        _indexBuffer->m_nIndices = 0;
    }

    void GFXAdapter_DX12::setIndexBuffer(ITF_IndexBuffer* _indexBuffer)
    {
        if (_indexBuffer != NULL)
        {
            m_currentIndexBuffer = reinterpret_cast<DX12::IndexBuffer*>(_indexBuffer->mp_IndexBuffer);
        }
    }

    void GFXAdapter_DX12::applyIndexBuffer(DX12::IndexBuffer* _ib)
    {
        ITF_ASSERT(_ib != nullptr);
        if (_ib != nullptr)
        {
            _ib->ensureUploadDone(*m_uploadBufferRequestManager);
            _ib->markAsUsedAtFrame(m_mainContext->getNbSubmittedFrame() + 1u);

            D3D12_INDEX_BUFFER_VIEW indexBufferView = {};
            indexBufferView.BufferLocation = _ib->getResource()->GetGPUVirtualAddress();
            indexBufferView.SizeInBytes = _ib->getByteSize();
            indexBufferView.Format = DXGI_FORMAT_R16_UINT;
            getRenderingContext()->IASetIndexBuffer(&indexBufferView);
        }
    }

    void GFXAdapter_DX12::LockIndexBuffer(ITF_IndexBuffer* _indexBuffer, void** _data)
    {
        ITF_ASSERT(_indexBuffer);
        ITF_ASSERT(_indexBuffer->mp_IndexBuffer);

        DX12::IndexBuffer* ib = reinterpret_cast<DX12::IndexBuffer*>(_indexBuffer->mp_IndexBuffer);
        ITF_ASSERT_MSG(ib->isDynamic(), "This IB should have been created as Dynamic. Are you locking twice an IB not dynamic?");
        ITF_ASSERT_MSG(i64(m_mainContext->getNBGPURenderedFrame()) >= ib->getLastUsedFrame(), "The index buffer is locked while still being in the rendering commands.");

        D3D12_RANGE range{};
        ib->getResource()->Map(0, &range, _data);
    }

    void GFXAdapter_DX12::UnlockIndexBuffer(ITF_IndexBuffer* _indexBuffer)
    {
        ITF_ASSERT(_indexBuffer);
        DX12::IndexBuffer* ib = reinterpret_cast<DX12::IndexBuffer*>(_indexBuffer->mp_IndexBuffer);
        ITF_ASSERT_MSG(ib->isDynamic(), "This IB should have been created as Dynamic");
        ib->getResource()->Unmap(0, nullptr);

        if (!_indexBuffer->bIsDynamic())
        {
            // not dynamic IB can be uploaded after unlock.
            // Dynamic IB are used directly from an UPLOAD heap, so no need to upload.
            ib->upload(*m_uploadBufferRequestManager);
        }
    }

    ///----------------------------------------------------------------------------//
    /// Draw Vertex/IndexBuffer.
    ///----------------------------------------------------------------------------//

    void GFXAdapter_DX12::DrawVertexBuffer(u32 _type, ITF_VertexBuffer* _vertexBuffer, u32 _vertexStart, u32 _vertexNumber)
    {
        if (_vertexBuffer==nullptr || _vertexBuffer->mp_VertexBuffer==0 || _vertexNumber==0)
            return;

        setVertexBuffer(_vertexBuffer);

        beginShader(mp_currentShader);

        if (isDrawAllowed())
        {
            applyVertexBuffer(m_currentVertexBuffer);
            auto dx12PrimitiveType = itfToDX12PrimitiveType(static_cast<PRIMITIVETYPE>(_type));
            getRenderingContext()->IASetPrimitiveTopology(dx12PrimitiveType);
            getRenderingContext()->DrawInstanced(_vertexNumber, 1, _vertexStart, 0);
        }

        endShader(mp_currentShader);
    }

    void GFXAdapter_DX12::DrawIndexedVertexBuffer(u32 _type, ITF_VertexBuffer* _vertexBuffer, ITF_IndexBuffer* _indexBuffer, u32 _indexNumber)
    {
        if (   _vertexBuffer==nullptr || _vertexBuffer->mp_VertexBuffer==0
            || _indexBuffer == nullptr || _indexBuffer->mp_IndexBuffer==0
            || _indexNumber==0)
            return;

        setVertexBuffer(_vertexBuffer);
        setIndexBuffer(_indexBuffer);

        beginShader(mp_currentShader);
        drawElements(_indexNumber);
        endShader(mp_currentShader);
    }

    ///----------------------------------------------------------------------------//
    /// Shader Patch.
    ///----------------------------------------------------------------------------//

    void GFXAdapter_DX12::prepareShaderPatch(GMatrix44* _matrix, f32 _z, u32& _hdiv, u32& _vdiv, GFX_BLENDMODE _blendMode /*= GFX_BLEND_ALPHA*/)
    {
        prepareShaderPatchTech(_matrix, _z, _hdiv, _vdiv, 3, _blendMode);
    }

    void GFXAdapter_DX12::prepareShaderPatchTech(GMatrix44* _matrix, f32 _z, u32& _hdiv, u32& _vdiv, u32 _idTech, GFX_BLENDMODE _blendMode /*= GFX_BLEND_ALPHA*/)
    {
        setAlphaBlend(_blendMode);

        if (getOverDrawMode() > 1)
            setShader(mp_shaderManager.getShaderByIndex(4));
        else
        {
            if (isUseFog())
                setShader(mp_shaderManager.getShaderByIndex(5));
            else
                setShader(mp_defaultShader);
        }

        renderContext_Set();

        m_worldViewProj.setWorld(*_matrix);

        m_UVmat = GMatrixIdentity;

        ITF_IndexBuffer* indexBuffer = getIndexBuffer(_hdiv, _vdiv);
        ITF_VertexBuffer* vertexBuffer = getVertexBuffer(_hdiv, _vdiv);
        ITF_ASSERT(indexBuffer && vertexBuffer);
        if (indexBuffer && vertexBuffer)
        {
            setIndexBuffer(indexBuffer);
            setVertexBuffer(vertexBuffer);
            ITF_ASSERT(vertexBuffer->m_vertexFormat == VertexFormat_PT);
        }
        else // default _hdiv == 2 && _vdiv == 2
        {
            _hdiv = bufferTypes[0][0];
            _vdiv = bufferTypes[0][1];
            setIndexBuffer(m_internalIdxBufferTab[0]);
            setVertexBuffer(m_internalVtxBufferTab[0]);
            ITF_ASSERT(m_internalVtxBufferTab[0]->m_vertexFormat == VertexFormat_PT);
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

    void GFXAdapter_DX12::shaderPatchFlushSize(u32 _hdiv, u32 _vdiv, u32 _countPoint, u32 _pass, u32 _vdivToDraw /*= U32_INVALID*/)
    {
        if (!_vdivToDraw)//don't draw empty patch
            return;

        ITF_ASSERT_MSG(_hdiv != 0, "shaderPatchFlushSize hdiv is 0");
        ITF_ASSERT_MSG(_vdiv != 0, "shaderPatchFlushSize vdiv is 0");

        if (!mp_shaderManager.getNumberVector4ConstToFlush(0)) return;

        /// Flush control Points.
        if (mp_currentShader->m_VectorArrayHandle[0] != nullptr)
        {
            mp_currentShader->setVectorArray(0, (GFX_Vector4*)mp_shaderManager.getBufferFloat(0), mp_shaderManager.getNumberVector4ConstToFlush(0));
            mp_currentShader->commitChange();
        }
        else
        {
            ITF_ASSERT(0);
        }
        /// Flush vector color.
        if (mp_currentShader->m_VectorArrayHandle[1] != nullptr)
        {
            mp_currentShader->setVectorArray(1, (GFX_Vector4*)mp_shaderManager.getBufferFloat(1), mp_shaderManager.getNumberVector4ConstToFlush(1));
            mp_currentShader->commitChange();
        }
        else
        {
            ITF_ASSERT(0);
        }

        /// draw patch.
        u32 nb = (mp_shaderManager.getNumberVector4ConstToFlush(0) / _countPoint);
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
            drawElements((_hdiv * _vdiv * 2) * nb * 3);
            endShader(mp_currentShader);
        }

        mp_shaderManager.setCurBuffFloatPos(0, 0);
        mp_shaderManager.setVector4ConstToFlush(0, 0);
        mp_shaderManager.setCurBuffFloatPos(1, 0);
        mp_shaderManager.setVector4ConstToFlush(1, 0);
    }

    void GFXAdapter_DX12::shaderDrawPatch32(const Texture* texture, const Vec2d* PointsNS, const ColorInteger* _lighttab)
    {
        ITF_ASSERT(_lighttab);

        f32 ratio = 1.f;
        /// set Texture;
        if (texture)
        {
            internSetTexture(0, texture);
            ratio = texture->getRatioXY();
            setTextureAdressingMode(0, GFXTADDRESS_WRAP, GFXTADDRESS_WRAP);
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

        mp_shaderManager.setCurBuffFloatPos(0, moveOp);

        /// number constant for control points is: f32 to vector4 -> 32 float / 4
        mp_shaderManager.addVector4ConstToFlush(0, 8);

        /// color.
        moveOp = mp_shaderManager.getCurBuffFloatPos(1);
        for (u32 i = 0; i < 4; i++)
        {
            mp_shaderManager.setBufferFloatAt(1, moveOp++, _lighttab[i].getF32Red());
            mp_shaderManager.setBufferFloatAt(1, moveOp++, _lighttab[i].getF32Green());
            mp_shaderManager.setBufferFloatAt(1, moveOp++, _lighttab[i].getF32Blue());
            mp_shaderManager.setBufferFloatAt(1, moveOp++, _lighttab[i].getF32Alpha());
        }

        mp_shaderManager.setCurBuffFloatPos(1, moveOp);

        mp_shaderManager.addVector4ConstToFlush(1, 4);
    }

    void GFXAdapter_DX12::shaderPatchFlush(u32 _hdiv, u32 _vdiv, u32 _pass)
    {
        shaderPatchFlushSize(_hdiv, _vdiv, 8, _pass);
    }

    void GFXAdapter_DX12::shaderBezierPatchFlush(u32 _hdiv, u32 _vdiv, u32 _pass, u32 _vDivToDraw)
    {
        shaderPatchFlushSize(_hdiv, _vdiv, 4, _pass, _vDivToDraw);
    }

    void GFXAdapter_DX12::endShaderPatch()
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

    void GFXAdapter_DX12::prepareShaderBezierPatch(GMatrix44* _matrix, f32 _z, u32& _hdiv, u32& _vdiv, GFX_BLENDMODE _blendMode /*= GFX_BLEND_ALPHA*/)
    {
        prepareShaderPatchTech(_matrix, _z, _hdiv, _vdiv, 10, _blendMode);
    }

    bbool GFXAdapter_DX12::shaderDrawBezierPatch32(Texture* texture, Vec2d* PointsNS, ColorInteger* _lighttab, f32* _widths, f32* _alphas, f32* _widths2)
    {
        ITF_ASSERT(_lighttab);

        f32 ratio = 1.f;

        /// set Texture;
        if (texture)
        {
            internSetTexture(0, texture);
            ratio = texture->getRatioXY();
            setTextureAdressingMode(0, GFXTADDRESS_WRAP, GFXTADDRESS_WRAP);
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
    void GFXAdapter_DX12::drawSpline(GMatrix44* _matrix, Texture* _texture, const Spline* _spline, const Vec3d& _pos, f32 _height)
    {
        IMPLEMENTED_NOT_TESTED;
        if (!_spline || _spline->GetNumPoints() < 2)
            return;
        /// set Texture;
        setShader(mp_defaultShader);

        if (_texture)
        {
            internSetTexture(0, _texture);
            setTextureAdressingMode(0, GFXTADDRESS_WRAP, GFXTADDRESS_WRAP);
        }

        renderContext_Set();

        m_worldViewProj.setWorld(*_matrix);

        m_UVmat = GMatrixIdentity;

        u32 hdiv = MESH_DEFAULT_VDIV;
        u32 vdiv = MESH_DEFAULT_HDIV;

        ITF_IndexBuffer* indexBuffer = getIndexBuffer(hdiv, vdiv);
        ITF_VertexBuffer* vertexBuffer = getVertexBuffer(hdiv, vdiv);
        ITF_ASSERT(indexBuffer && vertexBuffer);

        setIndexBuffer(indexBuffer);
        setVertexBuffer(vertexBuffer);

        /// set Vertex format
        setVertexFormat(VertexFormat_PT);
        mp_currentShader->m_selectedTech = 9;

        GFX_Vector4 v0(m_globalColor.getRed(), m_globalColor.getGreen(), m_globalColor.getBlue(), m_globalColor.getAlpha());
        GFX_Vector4 bufferVect[SPLINE_BUFFER_SIZE];

        // max 16 points by spline
        i32 point = _spline->GetNumPoints();
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

                if (mp_currentShader->m_VectorArrayHandle[0] != nullptr)
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
                drawElements((hdiv * vdiv * 2) * 3);
                endShader(mp_currentShader);

                if (point == 1)
                    break;

                bufferPoint = 0;
                splinePoint -= 2;
            }
            else
            {
                splinePoint++;
                point--;
            }
        }
    }

    /// Fluid Shader.
    void GFXAdapter_DX12::drawFluid(GMatrix44* _matrix, Texture* _texture, const FixedArray<u32, GFXADAPTER_FLUID_COLORTAB_ENTRY_COUNT>& _colorTab, const Vec2d _heightTab[], const u32 _heightTabSize, const FixedArray<Vec2d, 4>& _uvTab, u32 mode, f32 _delta1, f32 _delta2, f32 _stepCount)
    {
        /// set Texture;
        if (getOverDrawMode() > 1)
            setShader(mp_shaderManager.getShaderByIndex(4));
        else
        {
            if (isUseFog())
                setShader(mp_shaderManager.getShaderByIndex(5));
            else
                setShader(mp_defaultShader);
        }

        if (_texture)
        {
            internSetTexture(0, _texture);
            setTextureAdressingMode(0, GFXTADDRESS_WRAP, GFXTADDRESS_WRAP);
        }

        renderContext_Set();

        m_worldViewProj.setWorld(*_matrix);

        m_UVmat = GMatrixIdentity;

        u32 hdiv = MESH_DEFAULT_HDIV;
        u32 vdiv = MESH_DEFAULT_VDIV;

        ITF_IndexBuffer* indexBuffer = getIndexBuffer(hdiv, vdiv);
        ITF_VertexBuffer* vertexBuffer = getVertexBuffer(hdiv, vdiv);
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
        ITF_ASSERT_CRASH(colorCount <= GFXADAPTER_FLUID_COLORTAB_ENTRY_COUNT, "TOO MANY COLORS");
        for (u32 i = 0; i < colorCount; i++)
        {
            Color           color(_colorTab[i]);
            GFX_Vector4& colorVectElmt = colorVect[i];
            colorVectElmt.m_x = color.getRed();
            colorVectElmt.m_y = color.getGreen();
            colorVectElmt.m_z = color.getBlue();
            colorVectElmt.m_w = color.getAlpha();
        }


        GFX_Vector4 bufferVect[64 + 2];

        int hdivCount = 0;
        int heightTabSize = _heightTabSize;
        int rest = (heightTabSize % 2);
        int vLen = heightTabSize - 1;
        if (rest)
            heightTabSize--;
        for (int i = 0; i < (int)heightTabSize; hdivCount++)
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
        drawElements((hdiv * vLen * 2) * 3);
        endShader(mp_currentShader);
    }

    void GFXAdapter_DX12::drawElements(u32 _nbIndices)
    {
        if (isDrawAllowed())
        {
            applyVertexBuffer(m_currentVertexBuffer);
            applyIndexBuffer(m_currentIndexBuffer);
            getRenderingContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            getRenderingContext()->DrawIndexedInstanced(
                _nbIndices,    // nbIndices to draw
                1,             // nb Instances
                0,             // first Index in the index buffer
                0,             // vertex buffer first vertex
                0              // first Instance
            );
        }
    }

    ///----------------------------------------------------------------------------//
    //Movie
    ///----------------------------------------------------------------------------//

    void GFXAdapter_DX12::drawMovie(GMatrix44* _matrix, f32 _alpha, ITF_VertexBuffer* _pVertexBuffer, Texture** _pTextureArray, u32 _countTexture)
    {
        ITF_shader* pMovieShader = mp_shaderManager.getShaderByIndex(6);

        m_worldViewProj.push();

        setGlobalColor(Color::white());
        setAlphaBlend(GFX_BLEND_ALPHA);
        setOrthoView(0.f, (f32)getScreenWidth(), 0.f, (f32)getScreenHeight());

        setOrthoView(0.f, f32(getScreenWidth()), 0.f, f32(getScreenHeight()));
        if (_matrix)
        {
            m_worldViewProj.setWorld(*_matrix);
        }

        setAlphaBlend(GFX_BLEND_ALPHA);

        setShader(pMovieShader);
        pMovieShader->m_selectedTech = 0;
        pMovieShader->setFloat(0, _alpha);

        for (u32 index = 0; index < _countTexture; index++)
        {
            internSetTexture(index, _pTextureArray[index]);
            setTextureAdressingMode(index, GFXTADDRESS_CLAMP, GFXTADDRESS_CLAMP);
        }

        DrawVertexBuffer(GFX_TRIANGLE_STRIP, _pVertexBuffer, 0, 4); // just 1 quad

        m_worldViewProj.pop();
    }

    /// Trail 3d Shader.
    void GFXAdapter_DX12::drawTrail3D(Texture* _texture, Color _color, const SafeArray<Vec3d>& _points, f32 _alphaBegin, f32 _alphaEnd, f32 _fadeLength)
    {
        if (getOverDrawMode() > 1)
            setShader(mp_shaderManager.getShaderByIndex(4));
        else
        {
            if (isUseFog())
                setShader(mp_shaderManager.getShaderByIndex(5));
            else
                setShader(mp_defaultShader);
        }

        /// set Texture;
        if (_texture)
        {
            internSetTexture(0, _texture);
            setTextureAdressingMode(0, GFXTADDRESS_CLAMP, GFXTADDRESS_CLAMP);
        }

        renderContext_Set();

        m_worldViewProj.setWorld(GMatrixIdentity);
        m_UVmat = GMatrixIdentity;

        u32 hdiv = MESH_DEFAULT_HDIV;
        u32 vdiv = MESH_DEFAULT_VDIV;
        ITF_IndexBuffer* indexBuffer = getIndexBuffer(hdiv, vdiv);
        ITF_VertexBuffer* vertexBuffer = getVertexBuffer(hdiv, vdiv);
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

        for (idx = 0; idx < (int)nbPoints; bufferVectXYCount++)
        {
            bufferVectXY[bufferVectXYCount].m_x = _points[idx].m_x;
            bufferVectXY[bufferVectXYCount].m_y = _points[idx++].m_y;
            bufferVectXY[bufferVectXYCount].m_z = _points[idx].m_x;
            bufferVectXY[bufferVectXYCount].m_w = _points[idx++].m_y;
        }

        u32 count = nbPoints;
        u32 rest = nbPoints % 8;
        if (rest > 0)
            count -= rest;

        GFX_Vector4 bufferVectZ[32];
        u32         bufferVectZCount = 0;
        for (idx = 0; idx < (int)count; bufferVectZCount++)
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
        drawElements((hdiv* (nbPoints / 2 - 1) * 2) * 3);
        endShader(mp_currentShader);

        // restore Default Color constant:
        static const GFX_Vector4 defaultColor(1.f, 1.f, 1.f, 1.f);
        mp_currentShader->setVector(0, &defaultColor);
    }

    ///----------------------------------------------------------------------------//
    /// UV.
    ///----------------------------------------------------------------------------//

    void GFXAdapter_DX12::setUVAnim(GFX_UVANIM& _uvanim)
    {
        setMatrixTransform(GFX_TEXTURE, &_uvanim.m_UVMat);
    }

    ///----------------------------------------------------------------------------//
    /// Render Context.
    ///----------------------------------------------------------------------------//
    void GFXAdapter_DX12::renderContext_Set()
    {
        if (rC_getZWRITE())
            depthMask(1);
        if (rC_getZWRITE())
            depthTest(1);
    }

    void GFXAdapter_DX12::renderContext_SetDefault()
    {
        depthMask(0);
        depthTest(0);
        depthFunc(GFX_CMP_ALWAYS);
    }


    void GFXAdapter_DX12::setGfxMaterial(const GFX_MATERIAL& _gfxMat)
    {
        m_drawFlag = 0;

        switch (_gfxMat.m_matType)
        {
        default:
        case GFX_MAT_DEFAULT:
            GFXAdapter_DX12::setGfxMatDefault();
            break;
        case GFX_MAT_FRIEZEANIM:
        {
            GFXAdapter_DX12::setGfxMatDefault();

            DX12::FxParameterDB::Float4Hdl vhdl = DX12::FxParameterDB::Float4Hdl(mp_currentShader->m_VecHandle[1]);
            if (vhdl.isValid())
            {
                GFX_Vector4 vec4 = _gfxMat.m_matParams.getvParamsAt(0);
                m_paramDB.setFloat4(vhdl, Float4{ vec4.m_x, vec4.m_y, vec4.m_z, vec4.m_w });
            }
        }
        break;

        case GFX_MAT_FRIEZEOVERLAY:
        {
            GFXAdapter_DX12::setGfxMatDefault();
            mp_currentShader->m_selectedTech = 12;
        }
        break;

        case GFX_MAT_REFRACTION:
            GFXAdapter_DX12::setGfxMatRefraction(_gfxMat);
            break;

        case GFX_MAT_GLOW:
            AFTERFX_prepareObjectGlow();
            GFXAdapter_DX12::setGfxMatDefault();
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
            Texture* texture = (Texture*)_gfxMat.m_textureDiffuse.getResource();
            ITF_ASSERT(texture);
            GFXAdapter_DX12::SetTexture(0, texture);
        }

        setTextureAdressingMode(0, _gfxMat.m_TexAdressingModeU, _gfxMat.m_TexAdressingModeV);

        if (isUseFog())
        {
            Float4 fog;
            fog.m_x = m_colorFog.getRed();
            fog.m_y = m_colorFog.getGreen();
            fog.m_z = m_colorFog.getBlue();
            fog.m_w = m_colorFog.getAlpha(); // blend factor in alpha

            // color Fog.
            DX12::FxParameterDB::Float4Hdl vhdl = DX12::FxParameterDB::Float4Hdl(mp_currentShader->m_VecHandle[3]);
            if (vhdl.isValid())
            {
                m_paramDB.setFloat4(vhdl, fog);
            }
        }
    }

    void GFXAdapter_DX12::setBlendSeparateAlpha()
    {
        ITF_ASSERT(0); // bugged -> need to keep the last blend state change and reset it when it changed
    }

    void GFXAdapter_DX12::setDefaultAlphaTest()
    {
        setAlphaTest(bfalse);
        setAlphaRef(0);
    }

    void GFXAdapter_DX12::setAlphaTest(bbool _on)
    {
        GFXAdapter::setAlphaTest(_on); // set isAlphaTestEnabled()
        setAlphaTestInParamDB();
    }


    void GFXAdapter_DX12::setAlphaRef(int _ref)
    {
        m_currentAlphaTestRef = f32_Clamp(f32(_ref) / 255.f, 0.f, 1.f);
        setAlphaTestInParamDB();
    }

    void GFXAdapter_DX12::setAlphaTestInParamDB()
    {
        if (mp_currentShader->m_alphaTestRef != nullptr)
        {
            if (isAlphaTestEnabled())
                m_paramDB.setFloat(DX12::FxParameterDB::FloatHdl(mp_currentShader->m_alphaTestRef), m_currentAlphaTestRef);
            else
                m_paramDB.setFloat(DX12::FxParameterDB::FloatHdl(mp_currentShader->m_alphaTestRef), 0.f);
        }
        else
        {
            ITF_ASSERT(!isAlphaTestEnabled());
        }

    }

    void GFXAdapter_DX12::setGfxMatDefault()
    {
        if (getOverDrawMode() > 1)
            setShader(mp_shaderManager.getShaderByIndex(4));
        else
        {
            if (isUseFog())
                setShader(mp_shaderManager.getShaderByIndex(5));
            else
                setShader(mp_defaultShader);
        }
    }

    const GMatrix44* GFXAdapter_DX12::getViewMatrix()
    {
        return (const GMatrix44*)&m_worldViewProj.getView();
    }

    static void	normalizePlane(Plane& _plane)
    {
        float a = _plane.m_normal.getX();
        float b = _plane.m_normal.getY();
        float c = _plane.m_normal.getZ();
        // float d = _plane.m_w;
        // plane eq : ax + by + cz + d = 0

        float norm = sqrt(a * a + b * b + c * c);
        if (norm)
        {
            f32 invNorm = 1.f / norm;
            _plane.m_normal *= invNorm;
            _plane.m_constant *= invNorm;
        }
        else
        {
            _plane.m_normal.set(0.f, 0.f, 0.f);
            _plane.m_constant = 0.f;
        }
    }

    static void extractFrustumPlanes(const Matrix44& _mProj,
        Plane& _leftPlane,
        Plane& _rightPlane,
        Plane& _topPlane,
        Plane& _bottomPlane,
        Plane& _nearPlane)
    {
        Vec3d xColumn(_mProj._m11, _mProj._m21, _mProj._m31);
        f32 xColumnConst = _mProj._m41;
        Vec3d yColumn(_mProj._m12, _mProj._m22, _mProj._m32);
        f32 yColumnConst = _mProj._m42;
        Vec3d zColumn(_mProj._m13, _mProj._m23, _mProj._m33);
        f32 zColumnConst = _mProj._m43;
        Vec3d wColumn(_mProj._m14, _mProj._m24, _mProj._m34);
        f32 wColumnConst = _mProj._m44;

        _leftPlane.m_normal = -wColumn - xColumn;
        _leftPlane.m_constant = -(-wColumnConst - xColumnConst);

        _rightPlane.m_normal = xColumn - wColumn;
        _rightPlane.m_constant = -(xColumnConst - wColumnConst);

        _bottomPlane.m_normal = -wColumn - yColumn;
        _bottomPlane.m_constant = -(-wColumnConst - yColumnConst);

        _topPlane.m_normal = yColumn - wColumn;
        _topPlane.m_constant = -(yColumnConst - wColumnConst);

        _nearPlane.m_normal = -zColumn;
        _nearPlane.m_constant = -(-zColumnConst);

        normalizePlane(_leftPlane);
        normalizePlane(_rightPlane);
        normalizePlane(_bottomPlane);
        normalizePlane(_topPlane);
        normalizePlane(_nearPlane);
    }

    void GFXAdapter_DX12::updateCameraFrustumPlanes(Camera* _cam)
    {
        Plane& nearP = _cam->m_frustumPlanes[Camera::CameraPlane_Near];

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
