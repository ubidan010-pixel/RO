#include "precompiled_engine.h"

#ifndef _ITF_ATLASDYNAMICOBJECT_H_
#include "engine/display/AtlasDynamicObject.h"
#endif //_ITF_ATLASDYNAMICOBJECT_H_

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

AtlasDynamicObject::AtlasDynamicObject()
: m_maxQuad(0)
{
    m_color = Color::white();
    m_fogColor = Color::zero();
    m_atlasTextureID.invalidateResourceId();
}
//////////////////////////////////////////////////////////////////////////
void AtlasDynamicObject::createMeshVertexBuffer()
{
#ifndef VBMANAGER_USE_DYNAMICRING_VB
    if (m_mesh.m_vtxBufferBuffered[0]) return;
    for(ux i = 0; i<2; ++i)
    {
        m_mesh.m_vtxBufferBuffered[i] = GFX_ADAPTER->createVertexBuffer(
            4 * m_maxQuad,
            VertexFormat_PCT,
            sizeof(VertexPCT),
            btrue,
            VB_T_MESH
            );
    }
#else
    if (m_mesh.m_vtxBuffer) return;
    m_mesh.m_vtxBuffer = GFX_ADAPTER->createVertexBuffer
        (
        4 * m_maxQuad,
        VertexFormat_PCT,
        sizeof(VertexPCT),
        btrue,
        VB_T_MESH
        );
#endif
  }

void AtlasDynamicObject::initialize(ResourceGroup* _group, const Path& _texture, u32 _maxQuad)
{
    createTexture(_group, _texture);
    initVertexBuffer(_maxQuad);
}


void AtlasDynamicObject::createTexture(ResourceGroup* _group, const Path& _texture)
{
    m_atlasTextureID = _group->addResource(Resource::ResourceType_Texture, _texture);
}

GFX_MATERIAL& AtlasDynamicObject::getMaterial()
{
    return m_mesh.m_ElementList[0].m_material;
}

void AtlasDynamicObject::initVertexBuffer(u32 _maxQuad)
{
    m_maxQuad = _maxQuad;

    createMeshVertexBuffer();

    m_mesh.addElement();
    ITF_MeshElement& elem = m_mesh.m_ElementList[0];
    elem.m_material.m_matType = GFX_MAT_DEFAULT;
    elem.m_indexBuffer = GFX_ADAPTER->needSpriteIndexBuffer(6 * m_maxQuad);
    elem.m_startIndex = 0;
    elem.m_count = 0;                
    elem.m_material.m_textureDiffuse = m_atlasTextureID;
}


//////////////////////////////////////////////////////////////////////////
Texture *AtlasDynamicObject::getAtlasTexture()
{
    return (Texture*)m_atlasTextureID.getResource();
}


//////////////////////////////////////////////////////////////////////////
void AtlasDynamicObject::clear()
{
#ifndef VBMANAGER_USE_DYNAMICRING_VB
    for(ux i = 0; i<2; ++i)
    {
        if (m_mesh.m_vtxBufferBuffered[i] != NULL)
        {
            GFX_ADAPTER->removeVertexBuffer(m_mesh.m_vtxBufferBuffered[i]);
            m_mesh.m_vtxBufferBuffered[i] = NULL;
        }
    }
#else
    if (m_mesh.m_vtxBuffer)
    {
        GFX_ADAPTER->removeVertexBuffer(m_mesh.m_vtxBuffer);
        m_mesh.m_vtxBuffer = NULL;
    }
#endif
  
    m_mesh.m_ElementList.clear();
}


//////////////////////////////////////////////////////////////////////////

ITF_INLINE bbool AtlasDynamicObject::getUVAt( u32 _indexInAtlas , Vec2d* _uvOut)
{
    if (!isValid())
        return bfalse;

    Texture* t = getAtlasTexture();
    const UVAtlas *atlas = t->getUVAtlas();

    if (!atlas)
    {
        f32 halfPixelX = 0.5f/t->getSizeX();
        f32 halfPixelY = 0.5f/t->getSizeY();
        Vec2d uv0(halfPixelX,halfPixelY);
        Vec2d uv1(1-halfPixelX,1-halfPixelY);
        _uvOut[0] = uv0;
        _uvOut[1] = Vec2d(uv0.m_x, uv1.m_y);
        _uvOut[2] = uv1;
        _uvOut[3] = Vec2d(uv1.m_x, uv0.m_y);

    }
    else
    {
        atlas->get4UVAt(_indexInAtlas, _uvOut);
    }

    return btrue;
}

//////////////////////////////////////////////////////////////////////////

void AtlasDynamicObject::addQuad(int _indexInAtlas, const Vec2d& _pt1, const Vec2d& _pt2, const Vec2d& _pt3, const Vec2d& _pt4, f32 _zZoom, u32 _color)
{
    if (m_maxQuad <= m_vertex.size() / 4)
    {
        //ITF_WARNING_CATEGORY(LD, NULL, 0, "You try to draw more quad than the maximum allowed ! ");
        ITF_ASSERT(0);
        return;
    }

    Vec2d uv[4];

    if (!getUVAt(_indexInAtlas, uv))
    {
        return;
    }
  
    VertexPCT* vtx = &m_vertex.incrSize(4);
 
   
    vtx[0].setData(_pt1, uv[0], _color, _zZoom);
    vtx[3].setData(_pt2, uv[1], _color, _zZoom);
    vtx[2].setData(_pt3, uv[2], _color, _zZoom);
    vtx[1].setData(_pt4, uv[3], _color, _zZoom);

}

ITF_VertexBuffer *AtlasDynamicObject::getCurrentVertexBuffer() const
{
#ifndef VBMANAGER_USE_DYNAMICRING_VB
    return m_mesh.m_vtxBufferBuffered[m_mesh.m_userBuffer];
#else
    return m_mesh.m_vtxBuffer;
#endif
}

VertexPCT* AtlasDynamicObject::startFillVertexBuffer()
{
    ITF_ASSERT(0 == m_vertex.size());
    createMeshVertexBuffer();

#ifndef VBMANAGER_USE_DYNAMICRING_VB
    //switch mesh user buffer
    m_mesh.m_userBuffer = !m_mesh.m_userBuffer;
    u32 currentBuffer = m_mesh.m_userBuffer;
    //get vertex buffer ptr
    ITF_VertexBuffer * vertexBuffer = m_mesh.m_vtxBufferBuffered[currentBuffer];
#else
    ITF_VertexBuffer * vertexBuffer = m_mesh.m_vtxBuffer;
#endif //VBMANAGER_USE_DYNAMICRING_VB
    void* pret;
    GFX_ADAPTER->getVertexBufferManager().LockVertexBuffer(vertexBuffer, &pret);
    return (VertexPCT*)pret;
}

void AtlasDynamicObject::endFillVertexBuffer()
{
    ITF_ASSERT(0 == m_vertex.size());
#ifndef VBMANAGER_USE_DYNAMICRING_VB
    u32 currentBuffer = m_mesh.m_userBuffer;
    //get vertex buffer ptr
    ITF_VertexBuffer * vertexBuffer = m_mesh.m_vtxBufferBuffered[currentBuffer];
#else
    ITF_VertexBuffer * vertexBuffer = m_mesh.m_vtxBuffer;
#endif //VBMANAGER_USE_DYNAMICRING_VB
    GFX_ADAPTER->getVertexBufferManager().UnlockVertexBuffer(vertexBuffer);
}

void AtlasDynamicObject::drawVertexBuffer(f32 _z, u32 _quadCount)
{
    ITF_ASSERT(0 == m_vertex.size());

    ITF_MeshElement& elem = m_mesh.m_ElementList[0];
    elem.m_count = _quadCount* 6;
    m_mesh.getMatrix().fa16[14] = _z;
    m_mesh.m_colorFog = getFogColor();
    m_mesh.m_globalColor = getColor();

    GFX_ADAPTER->addPrimitiveInZList((void*)&m_mesh, GFX_ITF_MESH, NULL, _z,0,ObjectRef::InvalidRef);
}

void AtlasDynamicObject::draw(f32 _z)
{
    u32 count = m_vertex.size();
    if (!count)
        return;

#ifndef ITF_WII
    if (GFX_ADAPTER->isDeviceLost())
    {
        m_vertex.clear();
        return;
    }
#endif // ITF_WII

    // Lost device ?? then try to recreate VB.
    createMeshVertexBuffer();

    m_mesh.m_colorFog = getFogColor();
    m_mesh.m_globalColor = getColor();

#ifndef VBMANAGER_USE_DYNAMICRING_VB
    //switch mesh user buffer
    m_mesh.m_userBuffer = !m_mesh.m_userBuffer;
    u32 currentBuffer = m_mesh.m_userBuffer;
    //get vertex buffer ptr
    ITF_VertexBuffer * vertexBuffer = m_mesh.m_vtxBufferBuffered[currentBuffer];
#else
    ITF_VertexBuffer * vertexBuffer = m_mesh.m_vtxBuffer;
#endif //VBMANAGER_USE_DYNAMICRING_VB

    void *pdata = NULL;
    GFX_ADAPTER->getVertexBufferManager().LockVertexBuffer(vertexBuffer,&pdata);
    if(pdata)
    {
        ITF_MemcpyWriteCombined(pdata, &m_vertex[0], sizeof(VertexPCT) * count);
        GFX_ADAPTER->getVertexBufferManager().UnlockVertexBuffer(vertexBuffer);

        ITF_MeshElement& elem = m_mesh.m_ElementList[0];
        elem.m_count = (count / 4) * 6;

        m_mesh.getMatrix().fa16[14] = _z;


        GFX_ADAPTER->addPrimitiveInZList((void*)&m_mesh, GFX_ITF_MESH, NULL, _z,0,ObjectRef::InvalidRef);
    }
    m_vertex.clear();
}

void AtlasDynamicObject::draw2d(i32 _priority, bool _clearAfter)
{
    u32 count = m_vertex.size();
    if (!count)
        return;

#ifndef ITF_WII
    if (GFX_ADAPTER->isDeviceLost())
    {
        m_vertex.clear();
        return;
    }
#endif // ITF_WII

    // Lost device ?? then try to recreate VB.
    createMeshVertexBuffer();

#ifndef VBMANAGER_USE_DYNAMICRING_VB
    //switch mesh user buffer
    m_mesh.m_userBuffer = !m_mesh.m_userBuffer;
    u32 currentBuffer = m_mesh.m_userBuffer;
    //get vertex buffer ptr
    ITF_VertexBuffer * vertexBuffer = m_mesh.m_vtxBufferBuffered[currentBuffer];
#else
    ITF_VertexBuffer * vertexBuffer = m_mesh.m_vtxBuffer;
#endif //VBMANAGER_USE_DYNAMICRING_VB

    void *pdata = NULL;
    GFX_ADAPTER->getVertexBufferManager().LockVertexBuffer(vertexBuffer,&pdata);
    if(pdata)
    {
        ITF_MemcpyWriteCombined(pdata, &m_vertex[0], sizeof(VertexPCT) * count);
        GFX_ADAPTER->getVertexBufferManager().UnlockVertexBuffer(vertexBuffer);

        ITF_MeshElement& elem = m_mesh.m_ElementList[0];
        elem.m_count = (count / 4) * 6;

        m_mesh.getMatrix().fa16[14] = 0;


        GFX_ADAPTER->addPrimitive2d(Vec2d::Zero, Vec2d::One, _priority, GFX_ITF_MESH, (void*)&m_mesh);
    }
    if (_clearAfter)
    {
        m_vertex.clear();
    }
}

bbool AtlasDynamicObject::isValid()
{
    Texture* t = getAtlasTexture();

    if (!t || !t->isPhysicallyLoaded())
        return bfalse;

    return btrue;
}

u32 AtlasDynamicObject::getNumIndices()
{
    Texture* t = getAtlasTexture();

    if (!t || !t->isPhysicallyLoaded())
        return U32_INVALID;


    const UVAtlas *atlas = t->getUVAtlas();
    if (!atlas)
        return 1;

    return atlas->getNumberUV();
}

AtlasDynamicObject::~AtlasDynamicObject()
{
    clear();
}

}

