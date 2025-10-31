#ifndef _ITF_ATLASDYNAMICOBJECT_H_
#define _ITF_ATLASDYNAMICOBJECT_H_

#ifndef _ITF_RESOURCE_H_   
#include "engine/resources/Resource.h"
#endif //_ITF_RESOURCE_H_   

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

namespace ITF
{

class Vec2d;
class Vec3d;
class Texture;

class AtlasDynamicObject
{
public:
    //////////////////////////////////////////////////////////////////////////
    ///Constructor
    AtlasDynamicObject();
    ~AtlasDynamicObject();
    
    //p1 ----- p4
    //|        |
    //|        |
    //p2 ----- p3
    void                    addQuad(int _indexInAtlas, const Vec2d& _pt1, const Vec2d& _pt2, const Vec2d& _pt3, const Vec2d& _pt4,  f32 _zZoom = 0.0f, u32 _color = COLOR_WHITE);

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// get texture atlas (contains all icons)
    Texture                 *getAtlasTexture();

    void                    initialize(ResourceGroup* _group, const Path& _texture, u32 _maxQuad);
    void                    createTexture(ResourceGroup* _group, const Path& _texture);
    void                    initVertexBuffer(u32 _maxQuad);

    void                    clear(); //DEALLOCATES/REINITIALIZES ALL

    void                    clearVertices() {m_vertex.clear();}

    void                    draw(f32 _z);
    void                    draw2d(i32 _priority, bool _clearAfter);
    ITF_VertexBuffer *      getCurrentVertexBuffer() const;
    const ResourceID&       getResourceID() const { return m_atlasTextureID; }
    VertexPCT*              startFillVertexBuffer();
    void                    endFillVertexBuffer();
    void                    drawVertexBuffer(f32 _z, u32 _quadCount);

    ITF_INLINE bbool        getUVAt(u32 _indexInAtlas, Vec2d* _uvOut);
    typedef SafeArray<VertexPCT, 4 * 256> VertexPCTList;
   ITF_INLINE VertexPCTList & getVertex() { return m_vertex; }
    bbool                   isValid();
    ITF_INLINE Color        getColor() const { return m_color; }
    ITF_INLINE void         setColor(Color _val) { m_color = _val; }
    ITF_INLINE Color        getFogColor() const { return m_fogColor; }
    ITF_INLINE void         setFogColor(Color _val) { m_fogColor = _val; }

    u32                     getNumIndices();

    GFX_MATERIAL&           getMaterial(); //You must initialize before calling this function
    ITF_Mesh&               getMesh() {return m_mesh;}
private:
    u32                     m_maxQuad;

    void                    createMeshVertexBuffer();

    ResourceID              m_atlasTextureID;
    ITF_Mesh                m_mesh;
    VertexPCTList m_vertex;
    Color                   m_color;
    Color                   m_fogColor;
};

}

#endif // _ITF_ATLASDYNAMICOBJECT_H_

