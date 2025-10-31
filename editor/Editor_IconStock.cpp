#include "precompiled_editor.h"

#ifndef ITF_CONSOLE_FINAL

#ifndef _ITF_EDITOR_ICONSTOCK_H_
#include "editor/Editor_IconStock.h"
#endif //_ITF_EDITOR_ICONSTOCK_H_

#ifndef _ITF_RESOURCEGROUP_H_
#include "engine/resources/ResourceGroup.h"
#endif //_ITF_RESOURCEGROUP_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_

namespace ITF
{

Editor_IconStock::Editor_IconStock()
{
    m_mesh.m_vtxBuffer = NULL;
    m_iconsGroup.invalidateResourceId();
    m_atlasTextureID.invalidateResourceId();
}
//////////////////////////////////////////////////////////////////////////
void Editor_IconStock::createMeshVertexBuffer()
{
    if (m_mesh.m_vtxBuffer) return;

    m_mesh.m_vtxBuffer = GFX_ADAPTER->createVertexBuffer
        (
        6 * m_maxIconsDisplayed,
        VertexFormat_PCT,
        sizeof(VertexPCT),
        btrue,
        VB_T_ICONS
        );
}

void Editor_IconStock::loadIcons()
{
#ifndef ITF_CONSOLE
    if (!m_iconsGroup.isValidResourceId())
    {
        String dataFolder = "editor/icons/";
        String atlasTexture = "EditorIcons.tga";
        m_iconsGroup = RESOURCE_MANAGER->newResourceIDFromFile(Resource::ResourceType_ResourceGroup, Path::EmptyPath);
        m_atlasTextureID = ((ResourceGroup*)(m_iconsGroup.getResource()))->addResource(Resource::ResourceType_Texture, Path(dataFolder + atlasTexture));

        String trajectoryTexture = "Trajectory.tga";
        m_trajectoryTextureID = ((ResourceGroup*)(m_iconsGroup.getResource()))->addResource(Resource::ResourceType_Texture, Path(dataFolder + trajectoryTexture));

        RESOURCE_MANAGER->addUserToResourcePhysicalData(m_iconsGroup);
        
        createMeshVertexBuffer();

        m_mesh.addElement();
        ITF_MeshElement& elem = m_mesh.m_ElementList[0];
        elem.m_material.m_matType = GFX_MAT_DEFAULT;
        elem.m_indexBuffer = NULL;
        elem.m_startIndex = 0;
        elem.m_count = 0;                
        elem.m_material.m_textureDiffuse = m_atlasTextureID;
    }

#endif //ITF_CONSOLE
}

//////////////////////////////////////////////////////////////////////////
Texture *Editor_IconStock::getAtlasTexture()
{
    return (Texture*)m_atlasTextureID.getResource();
}

//////////////////////////////////////////////////////////////////////////
Texture *Editor_IconStock::getTrajectoryTexture()
{
    return (Texture*)m_trajectoryTextureID.getResource();
}

//////////////////////////////////////////////////////////////////////////
void Editor_IconStock::unloadIcons()
{
#ifndef ITF_CONSOLE
    RESOURCE_MANAGER->removeUserToResourcePhysicalData(m_iconsGroup);
    RESOURCE_MANAGER->removeUserToResourceLogicalData(m_iconsGroup);
    m_iconsGroup.invalidateResourceId();
    if (m_mesh.m_vtxBuffer)
    {
        GFX_ADAPTER->removeVertexBuffer(m_mesh.m_vtxBuffer);
        m_mesh.m_ElementList.clear();
        m_mesh.m_vtxBuffer = NULL;
    }
#endif // ITF_CONSOLE
}

//////////////////////////////////////////////////////////////////////////
void Editor_IconStock::drawIcon2D(EditorIconID _id, const Vec3d& _pt, f32 _w, f32 _h, f32 _angle, bbool _flipH, bbool _flipV, u32 _color)
{
    Texture* t = (Texture*)m_atlasTextureID.getResource();
    if (!t || !t->isPhysicallyLoaded())
        return;
    drawIcon2D(_pt, t, (int)_id, _w, _h, _angle, _flipH, _flipV, _color);
}

//////////////////////////////////////////////////////////////////////////
void Editor_IconStock::drawIcon2D(const Vec3d& _pt, Texture* _t, int _indexInAtlas, f32 _w, f32 _h, f32 _angle, bbool _flipH, bbool _flipV, u32 _color)
{
    ITF_ASSERT(_t!=NULL);
    if (!_t || !_t->isPhysicallyLoaded())
        return;

    Vec3d proj;
    GFX_ADAPTER->compute3DTo2D(_pt, proj);
    GFX_ADAPTER->drawDBGQuad(Vec2d(proj.m_x, proj.m_y), _w, _h, _t, _angle, 0.f, 0.f, _indexInAtlas, _flipH, _flipV, _color);
}

//////////////////////////////////////////////////////////////////////////
void Editor_IconStock::drawIcon(const Vec3d& _pt, Texture* _t, int _indexInAtlas, f32 _size, f32 _angle, bbool _flipH, bbool _flipV, u32 _color)
{
    ITF_ASSERT(_t!=NULL);
    if (!_t || !_t->isPhysicallyLoaded())
        return;

    if (CAMERA->isBiased())
    {
        Vec3d camPos = CAMERA->getCorrectedPos();
        Vec3d vcamToIcon = _pt - camPos;
        Vec3d camLookAt = CAMERA->getUnnormalizedLookAtDirection();
        if (vcamToIcon.dot(camLookAt) <= 0.f)
        {
           // _color = 0xffff0000;
            return;
        }
    }
    else
    {
        Vec3d camPos = CAMERA->getPos();
        if (_pt.m_z >= CAMERA->getZ())
            return;
    }

    Vec3d proj;
    GFX_ADAPTER->compute3DTo2D(_pt, proj);

    if (proj.m_z < 0.f || proj.m_z > 1.f)
    {
//            _color = 0xffff0000;
        return;
    }

    Vec2d pos(proj.m_x, proj.m_y);

    VertexPCT* vtx = &m_vertex.incrSize(6);
    Vec2d HorizAxis(cosf(_angle), -sinf(_angle));
    
    HorizAxis *= 0.5f;
        
    Vec2d vertAxis = HorizAxis.getPerpendicular();

    HorizAxis *= _size;
    vertAxis *= _size;

    const UVAtlas *atlas = _t->getUVAtlas();
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
    if (_flipH)
    {
        std::swap(uv[0].m_x, uv[2].m_x);
        std::swap(uv[1].m_x, uv[3].m_x);
    }
    if (_flipV)
    {
        std::swap(uv[0].m_y, uv[1].m_y);
        std::swap(uv[2].m_y, uv[3].m_y);
    }
    vtx[0].setData(pos - HorizAxis - vertAxis, uv[0], _color, 0);
    vtx[1].setData(pos - HorizAxis + vertAxis, uv[1], _color, 0);
    vtx[2].setData(pos + HorizAxis - vertAxis, uv[3], _color, 0);

    vtx[3].setData(pos + HorizAxis - vertAxis, uv[3], _color, 0);
    vtx[4].setData(pos - HorizAxis + vertAxis, uv[1], _color, 0);
    vtx[5].setData(pos + HorizAxis + vertAxis, uv[2], _color, 0);
}

//////////////////////////////////////////////////////////////////////////
void Editor_IconStock::drawIcon(EditorIconID _id, const Vec2d& _pt, f32 _z, f32 _size, f32 _angle, bbool _flipH, bbool _flipV, u32 _color)
{    
     drawIcon(_pt, _z, (Texture*)m_atlasTextureID.getResource(), (int)_id, _size, _angle, _flipH, _flipV, _color);
}
//////////////////////////////////////////////////////////////////////////
void Editor_IconStock::drawIcon(const Vec2d& _pt, f32 _z, Texture* _t, int _indexInAtlas, f32 _size, f32 _angle, bbool _flipH, bbool _flipV, u32 _color)
{
    Vec3d anchorpt(_pt.to3d());
    anchorpt.m_z = _z;
    drawIcon(anchorpt, _t, _indexInAtlas, _size, _angle, _flipH, _flipV, _color);
}

//////////////////////////////////////////////////////////////////////////
void Editor_IconStock::drawIcon(EditorIconID _id, const Vec3d& _pt, f32 _size, f32 _angle, bbool _flipH, bbool _flipV, u32 _color)
{
    drawIcon(_pt, (Texture*)m_atlasTextureID.getResource(), (int)_id, _size, _angle, _flipH, _flipV, _color);
}
//////////////////////////////////////////////////////////////////////////
void Editor_IconStock::showMouseIcon   (EditorIconID _id, const Vec2d &_mousePos, f32 _angle)
{
    if (m_mouseIconVtx.size()<4)
    {
        for (u32 i=0;i<4;i++)
        {
            Vertex v;
            m_mouseIconVtx.push_back(v);
        }
    }

    Texture* t  = (Texture*)m_atlasTextureID.getResource();
    ITF_ASSERT(t!=NULL);
    if (t && t->isPhysicallyLoaded())
        GFX_ADAPTER->drawDBGQuad(_mousePos, 64.f, 64.f, t, _angle, 0.f, 0.f, (int)_id);
}


void Editor_IconStock::draw()
{
    u32 count = m_vertex.size();
    if (!count)
        return;

    if (GFX_ADAPTER->isDeviceLost())
    {
        m_vertex.clear();
        return;
    }

    if (count > m_maxIconsDisplayed * 6)
        count = m_maxIconsDisplayed * 6;

    // Lost device ?? then try to recreate VB.
    if (!m_mesh.m_vtxBuffer)
    {
        createMeshVertexBuffer();
    }

    if (m_mesh.m_vtxBuffer) //now it's safe to use
    {
        VertexPCT    *pdata = 0;
        GFX_ADAPTER->getVertexBufferManager().LockVertexBuffer(m_mesh.m_vtxBuffer,(void **) &pdata);
        if (pdata)
        {
            ITF_MemcpyWriteCombined(pdata, &m_vertex[0], sizeof(VertexPCT) * count);
            GFX_ADAPTER->getVertexBufferManager().UnlockVertexBuffer(m_mesh.m_vtxBuffer);

            ITF_MeshElement& elem = m_mesh.m_ElementList[0];
            elem.m_count = count;

            GFX_ADAPTER->addPrimitive2d(Vec2d::Zero, Vec2d::One, 0, GFX_ITF_MESH, &m_mesh, NULL, NULL);
        }
        m_vertex.clear();
    }
} 

}

#endif //ITF_CONSOLE_FINAL
