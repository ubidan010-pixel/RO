#ifndef _ITF_GFX_ADAPTER_H_
#define _ITF_GFX_ADAPTER_H_

#ifndef _ITF_TEMPLATESINGLETON_H_
#include "core/templateSingleton.h"
#endif //_ITF_TEMPLATESINGLETON_H_

#ifndef _ITF_VEC2D_H_
#include "core/math/Vec2d.h"
#endif //_ITF_VEC2D_H_

#ifndef _ITF_VEC3D_H_
#include "core/math/Vec3d.h"
#endif //_ITF_VEC3D_H_

#ifndef _ITF_MATRIX44_H_
#include "core/math/matrix44/matrix44.h"
#endif //_ITF_MATRIX44_H_

#ifndef _ITF_TEXTURE_H_
#include "engine/display/Texture.h"
#endif //_ITF_TEXTURE_H_

#ifndef _ITF_AFTERFX_H_
#include "engine/display/AfterFx.h"
#endif //_ITF_AFTERFX_H_

#ifndef _ITF_GFXADAPTER_VERTEXBUFFERMANAGER_H_
#include "engine/AdaptersInterfaces/GFXAdapter_VertexBufferManager.h"
#endif //_ITF_GFXADAPTER_VERTEXBUFFERMANAGER_H_

#ifndef _ITF_SYSTEMADAPTER_
#include "core/AdaptersInterfaces/SystemAdapter.h"
#endif //_ITF_SYSTEMADAPTER_

#ifndef _ITF_SAFEARRAY_H_
#include "core/container/SafeArray.h"
#endif //_ITF_SAFEARRAY_H

#ifndef _ITF_SPRITE_H_
#include "engine/display/UVAtlas.h"
#endif //_ITF_SPRITE_H_

#ifndef _ITF_BV_AABB_H_
#include "Core/boundingvolume/AABB.h"
#endif //_ITF_BV_AABB_H_

#ifndef _ITF_COLOR_H_
#include "core/Color.h"
#endif //_ITF_COLOR_H_

#ifndef _ITF_COLORINTEGER_H_
#include "core/ColorInteger.h"
#endif //_ITF_COLORINTEGER_H_

#ifndef _ITF_SPLINE_H_
#include "core/math/spline.h"
#endif //_ITF_SPLINE_H_

#ifndef _LOCALISATION_ID_H_
#include "core/LocalisationId.h"
#endif // _LOCALISATION_ID_H_

#ifndef PATCH32_WORKINGBUFF
# define PATCH32_WORKINGBUFF    1
#endif // PATCH32_WORKINGBUFF



#ifdef ITF_WII
// can also be defined in the project settings
#define ITF_GFX_PLATFORM_COMMON_FUNCTIONS
#endif

namespace ITF
{

/// drawFlag
const int DRWF_COPYTOTARGET =  0x00000001;

const int VF_XYZ            =  0x00000001;
const int VF_DIFFUSE        =  0x00000002;
const int VF_TEX1           =  0x00000004;
const int VF_NORMAL         =  0x00000008;
const int VF_DWORD          =  0x00000010;
const int VF_BINORMAL       =  0x00000020;
const int VF_TANGENT        =  0x00000040;
const int VF_TEX2           =  0x00000080;
const int VF_TEX3           =  0x00000100;
const int VF_PATCH_POS      =  0x00000200;
const int VF_PATCH_POSNRM   =  0x00000400;
const int VF_PATCH_UV       =  0x00000800;
const int VF_PATCH_UVNRM    =  0x00001000;
const int VF_PATCH_BONEIDX  =  0x00002000;
const int VF_AMBIANT        =  0x00004000;
const int VF_TEX4           =  0x00008000;
const int GFXADAPTER_FLUID_COLORTAB_ENTRY_COUNT = 12;

enum eSafeFrame
{
    SF_NOSAFEFRAME = 0,
    SF_WII_4x3,
    SF_WII_16x9,
    SF_XBOX360,
    SF_PS3,
    SF_MINIMUM,
    SF_COUNT,
    ENUM_FORCE_SIZE_32(eSafeFrame)
};

enum eOffsetVF
{   
    VF_OFFSET_XYZ = 0,
    VF_OFFSET_DIFFUSE, 
    VF_OFFSET_TEX1,
    VF_OFFSET_NORMAL,
    VF_OFFSET_DWORD,
    VF_OFFSET_BLENDINDICES,
    VF_OFFSET_BINORMAL,
    VF_OFFSET_TANGENT ,
    VF_OFFSET_TEX2,
    VF_OFFSET_TEX3,
    VF_OFFSET_MAX, //always the lastest,
    ENUM_FORCE_SIZE_32(eOffsetVF)
};

// Options for clearing.
const int GFX_CLEAR_COLOR    = 0x00000001;
const int GFX_CLEAR_ZBUFFER  = 0x00000002;
const int GFX_CLEAR_STENCIL  = 0x00000004;
const int GFX_CLEAR_DEFERRED = 0x00000008;

#define ITF_POS2D_AUTO  -10000.f
#define AFTERFXLIST

class AnimMeshScene;
class Font;
class TrailComponent;
//class UVdata;
class renderTarget;
class RenderTargetManager;
class ITF_shader;
class Trail;
//class GroupMeshManager;

enum GFX_QUALITY
{
    GFX_QUALITY_FULL = 0,
    GFX_QUALITY_MEDIUM,
    GFX_QUALITY_LOW,
    ENUM_FORCE_SIZE_32(GFX_QUALITY)
};

enum GFX_TEXADRESSMODE : u32
{
    GFXTADDRESS_WRAP = 0,
    GFXTADDRESS_MIRROR,
    GFXTADDRESS_CLAMP,
    GFXTADDRESS_BORDER,
    GFXTADDRESS_COUNT,
};

enum GFX_SAMPLERSTATETYPE
{
    GFXSAMP_ADDRESSU = 0,
    GFXSAMP_ADDRESSV,
    GFXSAMP_MAGFILTER,
    GFXSAMP_MINFILTER,
    GFXSAMP_MIPFILTER,
    ENUM_FORCE_SIZE_32(GFX_SAMPLERSTATETYPE)
};

enum GFX_MATERIAL_TYPE
{
    GFX_MAT_DEFAULT = 0,
    GFX_MAT_REFRACTION,
    GFX_MAT_PATCH,
    GFX_MAT_FRIEZEANIM,
    GFX_MAT_GLOW,
    GFX_MAT_ALPHAFADE,
    GFX_MAT_FRIEZEOVERLAY,
    ENUM_FORCE_SIZE_32(GFX_MATERIAL_TYPE)
};

enum GFX_CMPFUNC
{
    GFX_CMP_NEVER                = 1,
    GFX_CMP_LESS                 = 2,
    GFX_CMP_EQUAL                = 3,
    GFX_CMP_LESSEQUAL            = 4,
    GFX_CMP_GREATER              = 5,
    GFX_CMP_NOTEQUAL             = 6,
    GFX_CMP_GREATEREQUAL         = 7,
    GFX_CMP_ALWAYS               = 8,
    ENUM_FORCE_SIZE_32(GFX_CMPFUNC)    
};

enum GFX_BLENDMODE 
{
    GFX_BLEND_UNKNOWN = 0,
    GFX_BLEND_COPY,
    GFX_BLEND_ALPHA,
    GFX_BLEND_ALPHAPREMULT,
    GFX_BLEND_ALPHADEST,
    GFX_BLEND_ALPHADESTPREMULT, 
    GFX_BLEND_ADD,
    GFX_BLEND_ADDALPHA,
    GFX_BLEND_SUBALPHA,
    GFX_BLEND_SUB,
    GFX_BLEND_MUL,
    GFX_BLEND_ALPHAMUL,
    GFX_BLEND_IALPHAMUL,
    GFX_BLEND_IALPHA,
    GFX_BLEND_IALPHAPREMULT,
    GFX_BLEND_IALPHADEST,
    GFX_BLEND_IALPHADESTPREMULT,
    GFX_BLEND_MUL2X,
    GFX_BLEND_ALPHATOCOLOR,
    GFX_BLEND_IALPHATOCOLOR,
    GFX_BLEND_SETTOCOLOR,
    GFX_BLEND_NUMBER,
    ENUM_FORCE_SIZE_32(GFX_BLENDMODE)
};

enum GFX_CULLMODE : u32
{
    GFX_CULL_NONE = 0,
    GFX_CALL_CCW,
    GFX_CULL_CW,
};

enum GFX_FILLMODE : u32
{
    GFX_FILL_SOLID = 0,
    GFX_FILL_WIREFRAME,
    GFX_FILL_POINT,
};

enum PRIMITIVETYPE
{
    GFX_POINTS = 0,
    GFX_LINES,
    GFX_LINE_LOOP,
    GFX_LINE_STRIP,
    GFX_TRIANGLES,
    GFX_TRIANGLE_STRIP,
    GFX_TRIANGLE_FAN,
    GFX_QUADS,
    GFX_QUAD_STRIP,
    GFX_POLYGON,
    GFX_ITF_MESH,
    GFX_ITF_ANIMSCENE,
    GFX_ITF_STARTRENDERTEX,
    GFX_ITF_AFTERFX,
    GFX_ITF_AFTERFXGROUP,
    GFX_ITF_FONT,
    GFX_ITF_TRAIL,
    GFX_ITF_3DQUAD,
    GFX_ITF_FLUID,
    GFX_ITF_PROCEDURALPATCH,
    GFX_ITF_BEZIERPATCH,
    GFX_ITF_SPLINE,
    GFX_ITF_UNDEFINED,
    GFX_ITF_REMOVED,
    ENUM_FORCE_SIZE_32(PRIMITIVETYPE)
};

enum MATRIXMODE
{
    GFX_MVIEW = 0,
    GFX_PROJECTION,
    GFX_TEXTURE,
    GFX_MWORLD,
    GFX_MWORLDVIEW,
    GFX_MWORLDVIEWPROJ,
    GFX_MWORLDTOWIEW, // specific convertion GL/DIRECTX.,
    ENUM_FORCE_SIZE_32(MATRIXMODE)
};

enum FONT_ALIGNMENT
{
    FONT_ALIGN_LEFT    = 0,
    FONT_ALIGN_CENTER  = 1,
    FONT_ALIGN_RIGHT   = 2,
    FONT_ALIGN_JUSTIFY = 3,
    ENUM_FORCE_SIZE_32(FONT_ALIGNMENT)
};

struct LightInfo
{
    enum Type : i32
    {
        AMBIENT = 0,
        LINEAR,
    };

    Color   m_color;
    Vec3d   m_position;
    Type    m_type;
    f32     m_typeData;
    f32     m_radNear;
    f32     m_radFar;
    bbool   m_useBV;
};

struct TextActorInfo
{
    TextActorInfo( const String8& _path, f32 _width, f32 _height ) : m_path(_path), m_width(_width), m_height(_height) {}
    TextActorInfo() : m_width(0.f), m_height(0.f), m_pos(Vec3d::Zero) {}

    String8         m_path;
    f32             m_width;
    f32             m_height;
    Vec3d           m_pos;
    ObjectRef       m_ref;
};

typedef ITF_VECTOR <TextActorInfo> TextActorInfoList;

struct TextInfo
{
    TextInfo(const String& _str, u32 _color, f32 _size, const StringID& _friendly, LocalisationId _lineId ) {m_text = _str; m_color = _color; m_size = _size; m_friendly = _friendly; m_lineId = _lineId; }
    TextInfo(const String& _str, u32 _color, f32 _size, const StringID& _friendly, LocalisationId _lineId, const TextActorInfoList& _list ) {m_text = _str; m_color = _color; m_size = _size; m_friendly = _friendly; m_lineId = _lineId; m_actors = _list; }
    TextInfo() {m_color = COLOR_WHITE; m_size = 17.f; m_lineId = U32_INVALID; }

    StringID                    m_friendly;
    String                      m_text;
    LocalisationId              m_lineId; // used for localization. when set, then it will automatically gather text from localization manager
    u32                         m_color;
    f32                         m_size;
    TextActorInfoList           m_actors;
};

struct GFX_RECT
{
    i32    left;
    i32    top;
    i32    right;
    i32    bottom;
};

struct GFX_Viewport
{
    u32    m_x;
    u32    m_y;
    u32    m_width;
    u32    m_height;
    f32    m_minZ;
    f32    m_maxZ;
};

struct GFX_3DQUAD
{
    GFX_3DQUAD()
        : m_fogColor(Color::zero())
    {
        m_blendMode = GFX_BLEND_ALPHA;
        m_rotation = Vec3d::Zero;
    }
    Vec3d m_pos;
    Vec2d m_pivot;
    Vec2d m_size;
    u32 m_color;
    Color m_fogColor;
    UVdata m_uvData;
    GFX_BLENDMODE   m_blendMode;
    Vec3d  m_rotation;
};

class GFX_Vector4
{
public:
    f32 m_x, m_y, m_z, m_w;

    GFX_Vector4() : m_x(0.0f), m_y(0.0f), m_z(0.0f), m_w(0.0f)  {}

    GFX_Vector4(const f32 x, const f32 y, const f32 z, const f32 w)
    {
        m_x = x;
        m_y = y;
        m_z = z;
        m_w = w;
    }

    const GFX_Vector4& operator = (const GFX_Vector4& src)
    {
        m_x = src.m_x;
        m_y = src.m_y;
        m_z = src.m_z;
        m_w = src.m_w;
        return *this;
    }

    void set(const f32 x, const f32 y, const f32 z, const f32 w)
    {
        m_x = x;
        m_y = y;
        m_z = z;
        m_w = w;
    }

    ITF_INLINE GFX_Vector4 operator/(const f32 s) const
    {
        f32 invs = 1.f / s;
        return GFX_Vector4(m_x * invs, m_y * invs, m_z * invs, m_w * invs);
    }
};

const int VertexFormat_P = VF_XYZ;
struct VertexP 
{
    void    setData (const Vec3d &_pos)
    {
        m_pos   = _pos;
    }
    void    setData (const Vec2d &_pos)
    {
        m_pos = Vec3d(_pos.m_x, _pos.m_y, 0.0f);
    }
    Vec3d   m_pos;
};


const int VertexFormat_PC = VF_XYZ | VF_DIFFUSE;
struct VertexPC 
{
public:
    void    setData (const Vec3d &_pos, u32 _color)
    {
        m_pos   = _pos;
        m_color = _color;
    }
    void    setData (const Vec2d &_pos, u32 _color)
    {
        m_pos = Vec3d(_pos.m_x, _pos.m_y, 0.0f);
        m_color = _color;
    }
    Vec3d   m_pos;
    u32     m_color;
};

const int VertexFormat_PCT = VF_XYZ | VF_DIFFUSE | VF_TEX1;
class VertexPCT
{
public:
    void    setData (const Vec3d& _pos, const Vec2d& _uv, u32 _color)
    {
        m_pos   = _pos;
        m_uv    = _uv;
        m_color = _color;
    }
    ITF_INLINE void    setData (const Vec2d& _pos, const Vec2d& _uv, u32 _color, float _z = 0.f)
    {
        m_pos.set(_pos.m_x, _pos.m_y, _z);
        m_uv = _uv;
        m_color = _color;
    }

    ITF_INLINE void setColor(u32 _color)
    {
        m_color = _color;
    }

    ITF_INLINE u32 getColor() const {return m_color;}

    ITF_INLINE void setPosition(const Vec3d& _pos)
    {
        m_pos = _pos;
    }

    ITF_INLINE void setUV(const Vec2d& _uv)
    {
        m_uv = _uv;
    }

    Vec3d   m_pos;
    u32     m_color;
    Vec2d   m_uv;
};

const int VertexFormat_PT = VF_XYZ | VF_TEX1;
class   VertexPT
{
public:
    void    setData (const Vec3d& _pos, const Vec2d& _uv)
    {
        m_pos   = _pos;
        m_uv    = _uv;
    }

    void    setData (const Vec2d& _pos, const Vec2d& _uv ,f32 _z = 0.f)
    {
        m_pos = Vec3d(_pos.m_x, _pos.m_y, _z);
        m_uv = _uv;
      
    }
    Vec3d   m_pos;
    Vec2d   m_uv;
};

typedef VertexPCT Vertex;


//VertexPCBT
const int VertexFormat_PCBT = VF_XYZ | VF_TEX1 | VF_DIFFUSE | VF_DWORD;
// According to the vertex buffer ubyte4 format
inline u32 BuildUByte4(u8 _x, u8 _y, u8 _z, u8 _w)
{
#ifdef ITF_PS3
    return u32(_w) + (u32(_z) << 8u) + (u32(_y) << 16u) + (u32(_x) << 24u);
#else
    return u32(_x) + (u32(_y) << 8u) + (u32(_z) << 16u) + (u32(_w) << 24u);
#endif
}

class VertexPCBT
{
public:
    void setData(const Vec3d& _pos, const Vec3d& _normal,const Vec2d& _uv, u32 _color, u32 _blendindices)
    {
        m_pos           = _pos;
        m_uv            = _uv;
        m_color         = _color;
        m_blendindices  = BuildUByte4(u8(_blendindices), 0, 0, 0);
    }

    void setData(const Vec2d& _pos, u32 _color, u32 _blendindices, const Vec2d& _uv)
    {
        m_pos.m_x       = _pos.m_x;
        m_pos.m_y       = _pos.m_y;
        m_pos.m_z       = 0.f;
        m_color         = _color;
        m_blendindices  = BuildUByte4(u8(_blendindices), 0, 0, 0);
        m_uv            = _uv;
    }

    Vec3d   m_pos;
    u32     m_color;
    u32     m_blendindices;
    Vec2d   m_uv;
};

//VertexPNCT (not used, old code?)
const int VertexFormat_PNCT = VF_XYZ | VF_TEX1 | VF_NORMAL | VF_DIFFUSE;

class VertexPNCT
{
public:
    void    setData(const Vec3d& _pos, const Vec3d& _normal, const Vec2d& _uv, u32 _color)
    {
        m_pos           = _pos;
        m_uv            = _uv;
        m_normal        = _normal;
        m_color         = _color;
    }


    ITF_INLINE void setColor(u32 _color)
    {
        m_color = _color;
    }

    Vec3d   m_pos;
    Vec3d   m_normal;
    u32     m_color;
    Vec2d   m_uv;
};

//VertexPNCTBiTg
const int VertexFormat_PNC3T = VF_XYZ | VF_DIFFUSE | VF_TEX1 | VF_TEX2 | VF_TEX3 | VF_TEX4;

class VertexPNC3T
{
public:
    void  setData(const Vec3d& _pos, u32 _color, const Vec2d& _uv, const GFX_Vector4& _uv2, const GFX_Vector4& _uv3, const Vec2d& _uv4 )
    {
        m_pos           = _pos;       
        m_color         = _color;
        m_uv            = _uv;
        m_uv2           = _uv2;
        m_uv3           = _uv3;        
        m_uv4           = _uv4;
    }

    ITF_INLINE void setColor(u32 _color)
    {
        m_color = _color;
    }

    ITF_INLINE void setUv(const Vec2d& _uv)
    {
        m_uv = _uv;        
    }

    Vec3d   m_pos;
    u32     m_color;
    Vec2d   m_uv;
    GFX_Vector4 m_uv2;
    GFX_Vector4 m_uv3;
    Vec2d       m_uv4;
};

// VertexFormat_PatchAnim use depends on define ANIMUSEGEOMETRYSHADER
const int VertexFormat_PatchAnim = VF_PATCH_POS | VF_PATCH_POSNRM | VF_PATCH_UV | VF_PATCH_UVNRM | VF_PATCH_BONEIDX;
struct VertexPatchAnim
{
    void    setData (const Vec2d& _pos, const Vec2d& _posNrm, const Vec2d& _uv, const Vec2d& _uvNrm, u8 _boneIx)
    {
        m_pos       = _pos;
        m_posNrm    = _posNrm;
        m_uv        = _uv;
        m_uvNrm     = _uvNrm;
        m_boneIdx   = _boneIx;
    }

    Vec2d   m_pos;
    Vec2d   m_posNrm;
    Vec2d   m_uv;
    Vec2d   m_uvNrm;
    f32     m_boneIdx;
};


const int VertexFormat_PTa = VF_XYZ | VF_TEX1 | VF_AMBIANT;

class PolyLine;
class SpriteAtomicObject;
class SpriteWorld;
class AnimScene;

/////////////////////////////////////////////////////////////

    //  vertexs buffer

struct Triangle
{
    Triangle(const ITF_VECTOR<Vertex>* _vertexList, u32 _i, u32 _ta, const ITF_VECTOR<u32>* _indexes=NULL) : m_vertexs(_vertexList)
    {
        index       = _i;
        taille      = _ta;
        m_indexes   = _indexes;
    }

    const ITF_VECTOR<Vertex>*   m_vertexs;
    const ITF_VECTOR<u32>*      m_indexes;
    u32     index;
    u32     taille;

    const Triangle& operator = (const Triangle& _p)
    {
        m_vertexs   = _p.m_vertexs;
        m_indexes   = _p.m_indexes;
        index       = _p.index;
        taille      = _p.taille;
        return *this;
    }    
};

struct Strip
{
    Strip(const ITF_VECTOR<Vertex>* _vertexList, u32 _i, u32 _ta, const ITF_VECTOR<u32>* _indexes=NULL) : m_vertexs(_vertexList)
    {
        index       = _i;
        taille      = _ta;
        m_indexes   = _indexes;
    }
    
    const ITF_VECTOR<Vertex>*   m_vertexs;
    const ITF_VECTOR<u32>*      m_indexes;
    u32     index;
    u32     taille;

    const Strip& operator = (const Strip& _p)
    {
        m_vertexs   = _p.m_vertexs;
        m_indexes   = _p.m_indexes;
        index       = _p.index;
        taille      = _p.taille;
        return *this;
    }    
};

struct Fan
{
    Fan(const ITF_VECTOR<Vertex>* _t, u32 _i, u32 _ta, const ITF_VECTOR<u32>* _indexes=NULL) : m_vertexs(_t)
    {
        index       = _i;
        taille      = _ta;
        m_indexes   = _indexes;
    }

    const ITF_VECTOR<Vertex>* m_vertexs;
    const ITF_VECTOR<u32>*    m_indexes;
    u32                 index;
    u32                 taille;

    const Fan& operator = (const Fan& _p)
    {
        m_vertexs   = _p.m_vertexs;
        m_indexes   = _p.m_indexes;
        index       = _p.index;
        taille      = _p.taille;
        return *this;
    }    
};

const int UVANIM_F_TRANS =  0x00000001;
const int UVANIM_F_ROTATE =  0x00000002;

struct GFX_UVANIM
{
    GFX_UVANIM()
    {
        m_UVMat = GMatrixIdentity;
        m_uvFlagAnim = 0;
        m_speedRotate = 0.f;
        m_currentangle = 0.f;
        m_pos = Vec2d::Zero;
        m_pivot = Vec2d::Zero;
        m_speedTrans = Vec2d::Zero;
    };

    GMatrix44       m_UVMat;
    Vec2d           m_pos;
    Vec2d           m_pivot;
    u32             m_uvFlagAnim;
    Vec2d           m_speedTrans;
    f32             m_speedRotate;
    f32             m_currentangle;
};

const u32 MAX_IPARAMS =  1;
const u32 MAX_FPARAMS =  4;
const u32 MAX_VPARAMS =  1;

struct GFX_MaterialParams
{
    enum AlphaFadeType {AlphaFade_Distance=0, AlphaFade_ExpDistance, AlphaFade_ExpSquaredDistance, AlphaFade_Constant};

    GFX_MaterialParams()
    {
        for (u32 i = 0 ; i < MAX_IPARAMS; i++)
           m_iparams[i] = 0;
        for (u32 i = 0 ; i < MAX_FPARAMS; i++)
            m_fparams[i] = 0;
        for (u32 i = 0 ; i < MAX_VPARAMS; i++)
            m_vparams[i] = GFX_Vector4(0.f, 0.f, 0.f, 0.f);
    };

    ITF_INLINE void setfParamsAt(u32 _at, f32 _val) {ITF_ASSERT(_at < MAX_FPARAMS); m_fparams[_at] = _val;};
    ITF_INLINE f32 getfParamsAt(u32 _at) const {ITF_ASSERT(_at < MAX_FPARAMS); return m_fparams[_at];};
    ITF_INLINE void setiParamsAt(u32 _at, i32 _val) {ITF_ASSERT(_at < MAX_IPARAMS); m_iparams[_at] = _val;};
    ITF_INLINE i32 getiParamsAt(u32 _at) const {ITF_ASSERT(_at < MAX_IPARAMS); return m_iparams[_at];};
    
    ITF_INLINE void setvParamsAt(u32 _at, const GFX_Vector4& _vec) {ITF_ASSERT(_at < MAX_VPARAMS); m_vparams[_at] = _vec;};
    ITF_INLINE GFX_Vector4 getvParamsAt(u32 _at) const {ITF_ASSERT(_at < MAX_VPARAMS); return m_vparams[_at];};
    ITF_INLINE void getvParamsAt(u32 _at, f32* _out) const {ITF_ASSERT(_at < MAX_VPARAMS); _out[0] = m_vparams[_at].m_x;_out[1] = m_vparams[_at].m_y;_out[2] = m_vparams[_at].m_z;_out[3] = m_vparams[_at].m_w;};
    ITF_INLINE f32* getfParams() {return m_fparams;}
    ITF_INLINE i32* getiParams() {return m_iparams;}
private:
        f32         m_fparams[MAX_FPARAMS];
        i32         m_iparams[MAX_IPARAMS];
        GFX_Vector4 m_vparams[MAX_VPARAMS];
};

/// material flags.
const int MAT_F_RENDERINTARGET =  0x00000001;

struct GFX_MATERIAL
{
    GFX_MATERIAL() 
    {
        m_blendMode = GFX_BLEND_ALPHA;
        m_matType = GFX_MAT_DEFAULT;

        m_TexAdressingModeU = GFXTADDRESS_WRAP;
        m_TexAdressingModeV = GFXTADDRESS_WRAP;

        m_flags = 0;
    };

    u32                 m_flags;
    ResourceID          m_textureDiffuse;
    ResourceID          m_textureNormal;

    GFX_BLENDMODE       m_blendMode;
    GFX_UVANIM          m_uvAnim;

    GFX_MATERIAL_TYPE   m_matType;

    GFX_MaterialParams  m_matParams;

    GFX_TEXADRESSMODE   m_TexAdressingModeU;
    GFX_TEXADRESSMODE   m_TexAdressingModeV;
};

struct ITF_MeshElement
{
    ITF_MeshElement()
    {
        m_indexBuffer = NULL;
        m_startIndex = 0;
        m_count = 0;
    };

    ITF_IndexBuffer*    m_indexBuffer;
    GFX_MATERIAL        m_material;
    u32                 m_startIndex;
    u32                 m_count;
};

class ITF_Mesh
{
public:
    enum MeshType
    {
        meshType_PTC,
        meshType_PT,
        meshType_PTa,
        meshType_Patch,

        ENUM_FORCE_SIZE_32(0)
    };
    ITF_Mesh()
        :m_globalColor(Color::white())
        ,m_colorFog(Color::zero())
#if PATCH32_WORKINGBUFF != 0
        ,m_userBuffer(0)
#endif        
        ,m_vtxBuffer(NULL)     
        ,m_pMatrixOverlay(NULL)
    {
#if PATCH32_WORKINGBUFF != 0
        m_vtxBufferBuffered[0] = NULL;
        m_vtxBufferBuffered[1] = NULL;
#endif // PATCH32_WORKINGBUFF != 0        
        m_type = meshType_PTC;
        //Matrix44::create(&m_matrix);
        //m_matrix->setIdentity();
        m_matrix = GMatrixIdentity;        
    };
    
    ~ITF_Mesh()
    {
        //Matrix44::destroy(&m_matrix);
    }

    MeshType                        m_type;
    ITF_VertexBuffer*               m_vtxBuffer;
#if PATCH32_WORKINGBUFF != 0
    ITF_VertexBuffer*               m_vtxBufferBuffered[2];
    u32                             m_userBuffer;
#endif // 
    ITF_VECTOR<ITF_MeshElement>     m_ElementList;
    Color                           m_globalColor;
    Color                           m_colorFog;
    //ITF_INLINE Matrix44*              getMatrix() {return &m_matrix;};
    //ITF_INLINE const    Matrix44&     getMatrix() const {return m_matrix;};
    //ITF_INLINE void                 setMatrix(const Matrix& _matrix) {m_matrix = _matrix;};
    
    //ITF_INLINE Matrix44*              getMatrix() {return m_matrix;};
    //ITF_INLINE const    Matrix44&     getMatrix() const {return *m_matrix;};
    //ITF_INLINE void                 setMatrix(const Matrix44& _matrix) {*m_matrix = _matrix;};
    
    ITF_INLINE GMatrix44&           getMatrix() {return m_matrix;};
    ITF_INLINE const GMatrix44&     getMatrix() const {return m_matrix;};
    ITF_INLINE void                 setMatrix(const GMatrix44& _matrix) {m_matrix = _matrix;};
    
    Texture*                        getDefaultTexture() const;
    bbool                           isDataReady() const;

    void                            initStaticIndexMesh(int _vertexFormat, ITF_IndexBuffer *_staticIndex, u32 _bufferSize = 0);
    void                            emptyIndexedMesh();
    void                            addElement();

    ///////////////////////////////////////////////////////////////////////////////////////////
    /// Create Vertex buffers for dynamic mesh
    /// @param _vertexFormat
    /// @param _staticIndex
    /// @param _bufferSize : Vertex count
    void                            initDynamicIndexedMesh(int _vertexFormat, ITF_IndexBuffer *_staticIndex, u32 _bufferSize);
    ///////////////////////////////////////////////////////////////////////////////////////////
    /// if created a dynamic vertex buffer, alternate vertex buffers. Some implementations may not need it.
    void                            swapVBForDynamicMesh();
    ///////////////////////////////////////////////////////////////////////////////////////////
    /// get current VertexBuffer
    ITF_VertexBuffer                *getCurrentVB();
    
    GMatrix44*                      m_pMatrixOverlay;

private:
    GMatrix44                       m_matrix;
};

struct FontCall
{

public:
    FontCall() : m_backGroundColour(COLOR_WHITE)
               , m_textTab(NULL)
               , m_textTabStop(U32_INVALID)
               , m_textTabStart(0)
               , m_append(bfalse)
               , m_bUseStyle(bfalse)
               , m_boxHeight(0.f)
               , m_boxWidth(0.f)
			   , m_boxPosition(Vec3d::Zero)
				, m_boxRotation(0.f)
				, m_boxPivot(Vec3d::Zero)
				, m_boxScale(Vec3d::Zero)
               , m_color(COLOR_WHITE)
               , m_count(0)
               , m_croppingMode(1)
               , m_height(-1.f)
               , m_isWriteML(bfalse)
               , m_mode(0)
               , m_modeY(0)
               , m_scale(1.f)
               , m_useBackground(bfalse)
			   , m_useTransBox(bfalse)
               , m_auto2dScale(btrue)
               , m_lineSpacingFactor(1.f)
			   , m_is2D(btrue)
               , m_write(btrue)
               , m_shadowOffset(Vec2d::Zero)
               , m_shadowColor(COLOR_BLACK)
    {
    }

	ITF_INLINE void setFont(ResourceID _font)			        {m_fontId = _font;}
	ITF_INLINE void setColor(u32 _color)				        {m_color = _color;}
	ITF_INLINE void setPosition(const Vec3d& _pos)		        {m_Position = _pos;}
	ITF_INLINE const Vec3d& getPosition() const 		        {return m_Position;}
	ITF_INLINE void setText(const String& _text)		        {m_text = _text;}
	ITF_INLINE void setCount(i32 _count)				        {m_count = _count;}
	ITF_INLINE void setMode(u32 _mode)					        {m_mode = _mode;}
    ITF_INLINE void setModeY(u32 _mode)					        {m_modeY = _mode;}
	ITF_INLINE void setAppend(bbool _append)			        {m_append = _append;}
	ITF_INLINE void setBackGroundTexture(ResourceID _backgroundTexture)   {m_backGroundTexture = _backgroundTexture;}
	ITF_INLINE void setBackGroundTextureStartUV(const Vec2d& _uv)   {m_backGroundTextureStartUV = _uv;}
	ITF_INLINE void setBackGroundTextureEndUV(const Vec2d& _uv) {m_backGroundTextureEndUV = _uv;}
	ITF_INLINE const Vec2d& getBackGroundTextureStartUV() const {return m_backGroundTextureStartUV;}
	ITF_INLINE const Vec2d& getBackGroundTextureEndUV() const   {return m_backGroundTextureEndUV;}
	ITF_INLINE void setBackGroundColour(u32 _colour)            {m_backGroundColour = _colour;}
	ITF_INLINE u32  getBackGroundColour() const                 {return m_backGroundColour;}

    ITF_INLINE void setHeight(f32 _height)			            {m_height = _height;}
    ITF_INLINE f32  getHeight() const   			            {return m_height;}
    ITF_INLINE void setBoxPosition (const Vec3d& _pos)          {m_boxPosition = _pos;}
	ITF_INLINE const Vec3d& getBoxPosition () const				{return m_boxPosition;}
    
	ITF_INLINE void setBoxWidth (f32 _boxWidth)    	            {m_boxWidth = _boxWidth;}
    ITF_INLINE void setBoxHeight (f32 _boxHeight)    	        {m_boxHeight = _boxHeight;}
    ITF_INLINE f32  getBoxWidth () const          	            {return m_boxWidth;}
    ITF_INLINE f32  getBoxHeight () const                       {return m_boxHeight;}
    
	ITF_INLINE void	setBoxRotation(f32 _rot)					{m_boxRotation = _rot;}
	ITF_INLINE f32  getBoxRotation () const                     {return m_boxRotation;}
	ITF_INLINE void	setBoxPivot(const Vec3d& _vPivot)			{m_boxPivot = _vPivot;}
	ITF_INLINE const Vec3d& getBoxPivot () const				{return m_boxPivot;}
	ITF_INLINE void	setBoxScale(const Vec3d& _vScale)			{m_boxScale = _vScale;}
	ITF_INLINE const Vec3d& getBoxScale () const				{return m_boxScale;}

	ITF_INLINE void setUseTransBox(bbool _use)    	            {m_useTransBox = _use;}
    ITF_INLINE bbool getUseTransBox() const        	            {return m_useTransBox;}

	ITF_INLINE void setAutoScale(bbool _use)    	            {m_auto2dScale = _use;}
    ITF_INLINE bbool getAutoScale() const        	            {return m_auto2dScale;}

	ITF_INLINE void setlineSpacingFactor(f32 _val)              {m_lineSpacingFactor = _val;}
    ITF_INLINE f32 getlineSpacingFactor() const                 {return m_lineSpacingFactor;}

	ITF_INLINE void setIs2D(bbool _v)    						{m_is2D = _v;}
    ITF_INLINE bbool getIs2D() const        					{return m_is2D;}
	
	ITF_INLINE void setTextTab(ITF_VECTOR<TextInfo> *_textTab)  {m_textTab = _textTab;}
    ITF_INLINE void setTextTabStart(u32 _start)                 {m_textTabStart = _start;}
    ITF_INLINE void setTextTabStop(u32 _stop)                   {m_textTabStop = _stop;}

    ITF_INLINE void setTextCroppingMode(u32 _croppingMode)      {m_croppingMode = _croppingMode;}
    ITF_INLINE void setUseStyle(bbool _useStyle)                {m_bUseStyle = _useStyle;}
    ITF_INLINE void setWriteML (bbool _isUseML)                 {m_isWriteML = _isUseML;}
    ITF_INLINE Texture* getBackGroundTexture() const            {return (Texture *)m_backGroundTexture.getResource();}
    ITF_INLINE void setTextureOffset2D (Vec2d _offset)          {m_textureOffset2D = _offset;}
    ITF_INLINE Vec2d getTextureOffset2D () const                {return m_textureOffset2D;}
    ITF_INLINE void setScale (f32 _scale)                       {m_scale = _scale;}
    ITF_INLINE f32 getScale () const                            {return m_scale;}
    ITF_INLINE bbool getUseBackground () const                  {return m_useBackground;}
    ITF_INLINE void setUseBackground (bbool _useBackground)     { m_useBackground = _useBackground;}
    ITF_INLINE void setWrite( bbool _val )                      { m_write = _val; }

    ITF_INLINE void setShadowOffset(Vec2d _voffset)             {m_shadowOffset = _voffset;}
    ITF_INLINE void setShadowColor(u32 _col)                    {m_shadowColor = _col;}

    bbool           isAppend()    const                         {return m_append;}

    f32 write() const;

private:
    ResourceID          m_fontId;
    u32                 m_color;
    Vec3d               m_Position;
    String              m_text;
    i32                 m_count;
    u32                 m_mode;
    u32                 m_modeY;
    bbool               m_append;
    ResourceID          m_backGroundTexture;
    Vec2d               m_backGroundTextureStartUV;
    Vec2d               m_backGroundTextureEndUV;
    u32                 m_backGroundColour;

    f32                 m_height;
    f32                 m_boxWidth;
    f32                 m_boxHeight;
	Vec3d				m_boxPosition;
	f32                 m_boxRotation;
	Vec3d				m_boxPivot;
	Vec3d				m_boxScale;
	bbool				m_useTransBox;
	bbool				m_is2D;
    bbool				m_auto2dScale;
    f32                 m_lineSpacingFactor;

	u32                 m_croppingMode;
    ITF_VECTOR<TextInfo> *m_textTab;
    u32                 m_textTabStart;
    u32                 m_textTabStop;

    bbool               m_bUseStyle;
    bbool               m_isWriteML;
    Vec2d               m_textureOffset2D; 
    f32                 m_scale; 
    bbool               m_useBackground;
    bbool               m_write;

    Vec2d               m_shadowOffset;
    u32                 m_shadowColor;
};

struct Quad2DInfo
{
    Texture*            m_texture;
    f32                 m_width;
    f32                 m_height;
    u32                 m_color;
    Vec2d               m_uvStart;
    Vec2d               m_uvEnd;
    Vec3d               m_pos2D;
    f32                 m_angle;
};

struct PrimMeshInfo
{
    Vec2d       Pos;
    Vec2d       Scale;
    ITF_Mesh*   mesh;
};

struct PrimAnimMeshInfo
{
    Vec2d       Pos;
    Vec2d       Scale;
    AnimMeshScene*   mesh;
};

struct PrimTrailInfo
{
    Trail *trail;
};

struct PrimAFXInfo
{
    AFTERFX* afx;
};

struct PrimitivesContainer2d
{
    ITF_VECTOR<PrimMeshInfo>        MeshList;
    ITF_VECTOR<PrimAnimMeshInfo>    AnimSceneList;
    ITF_VECTOR<FontCall>            FontsCallList;
    ITF_VECTOR<Quad2DInfo>          Quad2DInfoList;
    ITF_VECTOR<PrimTrailInfo>       TrailList;
	ITF_VECTOR<PrimAFXInfo>			AfterfxList;
};

#define DEFAULT_HDIV            2
#define DEFAULT_VDIV            2

class Patch32Data
{
public:
    Patch32Data(int _vertexFormat, u32 _maxVertex = MAX_VERTEX_BY_MESH);
    ~Patch32Data();

    ITF_Mesh    *mesh;
    union
    {
        VertexPT     *vertexListPT;
        VertexPCT     *vertexListPCT;
        VertexPatchAnim   *vertexListPatch;
    };
    u32         vertexCount;

    u32         maxVertexListSize;
    int         vertexFormat;

    void Init(ITF_Mesh    *_mesh, int _vertexType);
#if PATCH32_WORKINGBUFF != 0
    void CopyToMesh();
#else
    void *LockVBuff();
    void UnlockVBuff();
#endif
};

/////////////////////////////////////////////////////////////

class Size
{
public:
    Size(void) {}
    Size(u32 width, u32 height) : d_width(width), d_height(height) {}

    bool operator==(const Size& other) const;
    bool operator!=(const Size& other) const;

    u32 d_width, d_height;
};

struct LOCKED_TEXTURE
{
    i32     m_pitch;
    void*   mp_Bits;
};

#if ITF_DEBUG_LEVEL > 0
struct DBGCircle
{
    float x, y, rad, r, g, b, duration, z;
    u32 segsCount;
};

struct DBGSphere
{
    float x, y, rad, r, g, b, duration, z;
    u32 segsCount;
    bbool volume;
    bbool xaxis;
    bbool yaxis;
    bbool zaxis;
};

struct DBGBox
{
    Vec2d pos, extent;
    f32 angle, r, g, b, duration, z;
    bbool fill;
};

struct DBGLine
{
    Vec2d p1, p2;
    float r, g, b, duration, size, z, alpha;
};

struct DBG3DLine
{
    Vec3d p1, p2;
    float r, g, b, duration, size;
};

struct DBGTriangle
{
    Vec2d p1, p2, p3;
    float r, g, b, a, duration, z; 

};

struct DBGTriangle3D
{
    Vec3d p1, p2, p3;
    float r, g, b;
};

struct DBGTextureTriangle
{
    ITF_VECTOR<Vertex>* pVtx;
    float duration, z;
    Texture* t;
};

struct DBGTextureQuad
{
    Vertex mVtx[4];
    float duration, z;
    const Texture* t;
};
#endif // ITF_DEBUG_LEVEL > 0

//#ifdef ITF_SUPPORT_CHEAT
struct DBGText
{
    String  m_text;
    float   m_x;
    float   m_y;
    f32     m_r;
    f32     m_g;
    f32     m_b;
    bbool   m_useBigFont;
	bbool	m_useMonospaceFont;
    FONT_ALIGNMENT m_alignment;

    DBGText()
        : m_useBigFont(bfalse)
		, m_useMonospaceFont(bfalse)
        , m_x(F32_INFINITY)
        , m_y(F32_INFINITY)
        , m_r(1.0f)
        , m_g(1.0f)
        , m_b(1.0f)
        , m_alignment(FONT_ALIGN_LEFT)
    {
    }
};
//#endif // ITF_SUPPORT_CHEAT

struct Box2D
{
    Box2D() {m_rank = 0;}
    VertexPC v[4];
    i32     m_rank;
};

struct Triangle2D
{
    Triangle2D() {m_rank = 0;}
    VertexPC v[3];
    i32     m_rank;
};

struct PrimitiveInfo
{
	PrimitiveInfo()
    {
        mp_Primitive = NULL;
        m_Texture = NULL;
        m_type = GFX_ITF_UNDEFINED;
    }
    
    void*           mp_Primitive;
    Texture*        m_Texture;
    PRIMITIVETYPE   m_type;
};

struct ZList_Node
{
    PrimitiveInfo	pPrimitiveInfo;
    f32 m_depth;
    ObjectRef m_ref;
};

struct Spline_Info
{
    Spline          m_spline;
    GMatrix44       m_mat;
    Texture *       m_texture;
    f32             m_height;
    Vec3d           m_pos;
    Color           m_color;

    Spline_Info()
    : m_texture(NULL)
    , m_mat(GMatrixIdentity), m_height(0.1f)
    {}
};

class GFX_Zlist
{
public:
    GFX_Zlist()
    {
        m_NumberEntry = 0;
        m_maxSegNode = 256;
        m_currentMax = 0;
        m_reorderingTable = NULL;
        ZList_init();
    }

    ~GFX_Zlist()
    {
        if (m_reorderingTable)
            delete[] m_reorderingTable;
    }

    void            AddPrimitiveInZList(void* _pPrimitive, PRIMITIVETYPE _type, Texture* _texture, f32 _depth,const ObjectRef& _ref);
    void            transferNodeTOZList(const ZList_Node& _node);
    void            ZList_remove(PRIMITIVETYPE _type, void* _primitive);

    ITF_INLINE u32  getNumberEntry() {return m_NumberEntry;};
    void            sort();
    ITF_INLINE      ZList_Node* getOrderNodeAt(u32 _val) {return m_reorderingTable[_val];};
    //ITF_INLINE      ZList_Node* getNodeAt(u32 _val) {return &m_zListBufferNode[_val];};
    void            ZList_reset();
    ZList_Node*     ZList_find(PRIMITIVETYPE _type);
    void            removeNode(ZList_Node* _node);

    ZList_Node&     getNodeAt(u32 _index) {return m_zListBufferNode[_index];}

protected:

    void            ZList_init();
    void            ZList_expandsize();

    ZList_Node*     ZList_AddNode(f32 _zordering);

    SafeArray<ZList_Node> m_zListBufferNode;
    ZList_Node** m_reorderingTable;

    u32 m_NumberEntry;
    u32 m_maxSegNode;
    u32 m_currentMax;
};

typedef u32 UVSubdiv[2];

struct AnimBoneRenderable;

class GFXAdapter : public TemplateSingleton<GFXAdapter>
{
public:
            // HideFlags  --> powers of 2
            static const u32   HideFlag_DBGPrimitive = 1;
            static const u32   HideFlag_2DBox        = 2;
            static const u32   HideFlag_Font         = 4;

            static const u32        m_PriorityCount2d   = 16;
           
#define m_PatchsDegree          4
#define m_PatchsNumberOfCurves  2

            void                    render2D                (   );
            void                    drawPrimitives          (   );
            void                    drawZlistPrimitives     (   GFX_Zlist* _zlist  );
            void                    drawPrimitives2d        ( PrimitivesContainer2d* _container2d );
            void                    drawStrip               (   const Strip& _strip, const Texture* _txt);
            void                    drawTriangles           (   const Triangle& _triangle, const Texture* _txt);
            void                    drawFan                 (   const Fan& _fan, const Texture* _txt);
            void                    addPrimitive2d          (   Vec2d _Pos, Vec2d _scale, int _prio, PRIMITIVETYPE _type, void* _mesh = NULL, FontCall* _font = NULL, Quad2DInfo* _texInfo = NULL, AFTERFX* _afx = NULL, bbool _UpScreen = btrue);
            void                    addPrimitiveInZList     (   void* _pPrimitive, PRIMITIVETYPE _type, Texture* _texture, f32 _depth, bbool _target ,const ObjectRef& _ref);
            i32                     getDebugFontSize        ();
            i32                     getBigDebugFontSize     ();
#ifndef ITF_CONSOLE_FINAL
            void                    drawDBGPrimitives       (                           );
            void                    drawPolyLine            (   const PolyLine* _pl   );
#endif // ITF_CONSOLE_FINAL
            void                    cancelAllTasks          (     );
#if !defined(ITF_WII) || !defined(ITF_FINAL)
    virtual void                    project                 (   const Vec3d& /*_in3d*/, Vec2d & /*_out2d*/) {ITF_ASSERT(0);}
#endif // !defined(ITF_WII) || !defined(ITF_FINAL)
#ifndef ITF_CONSOLE_FINAL
    virtual void                    drawSphere              (   float /*_x*/, float /*_y*/, float /*_z*/, float /*_radius*/, int /*lats*/, int /*longs*/, int /*_wireFrame*/, float /*_r*/=1, float /*_g*/=1, float /*_b*/=1  ) {ITF_ASSERT(0);}
    virtual void                    drawSphere              (   float /*_x*/, float /*_y*/, float /*_radius*/, float /*_r*/=1, float /*_g*/=1, float /*_b*/=1, float /*_z*/ = 0.f, u32 /*_segsCount*/ = 16, bbool _volume = btrue, bbool _xaxis = btrue, bbool _yaxis = btrue, bbool _zaxis = btrue ){ITF_ASSERT(0);}
    virtual void                    drawCircle              (   float /*_x*/, float /*_y*/, float /*_radius*/, float /*_r*/=1, float /*_g*/=1, float /*_b*/=1, float /*_z*/ = 0.f, u32 /*_segsCount*/ = 16  ){ITF_ASSERT(0);}
    virtual void                    drawBox                 (   const Vec2d& /*_pos*/, f32 /*_angle*/, const Vec2d& /*_extent*/, f32 /*_r*/=1, f32 /*_g*/=1, f32 /*_b*/=1, float /*_z*/ = 0.f, bbool /* _fill */ = bfalse ) {ITF_ASSERT(0);}
    virtual void                    drawLine                (   const Vec2d& /*_p1*/, const Vec2d& /*_p2*/, float /*_r*/=1, float /*_g*/=1, float /*_b*/=1, float /*_size*/ = 1.f, float /*_z*/ = 0.f, float _alpha = 1.f ) {ITF_ASSERT(0);}
#endif // ITF_CONSOLE_FINAL
    virtual void                    draw2dLine              (   const Vec2d& /*_p1*/, const Vec2d& /*_p2*/, float /*_size*/=1.f, u32 /*_color*/ = COLOR_WHITE) {ITF_ASSERT(0);}
    virtual void                    draw2dTriangle          (   const Vec2d& /*_p1*/, const Vec2d& /*_p2*/, const Vec2d& /*_p3*/, u32 /*_color*/ = COLOR_WHITE, i32 /*_rank*/ = 0) {ITF_ASSERT(0);}
    virtual void                    draw2dBox               (   const Vec2d& /*_p*/, f32 /*_width*/, f32 /*_height*/, u32 /*_color0*/ = COLOR_WHITE, u32 /*_color1*/ = COLOR_WHITE, u32 /*_color2*/ = COLOR_WHITE, u32 /*_color3*/ = COLOR_WHITE, i32 /*_rank*/ = 0) {ITF_ASSERT(0);}
    virtual void                    drawTriangle            (   const Vec2d& /*_p1*/, const Vec2d& /*_p2*/, const Vec2d& /*_p3*/, float /*_r*/=1, float /*_g*/=1, float /*_b*/=1, float /*_a*/=1, float /*_z*/ = 0.f    ) {ITF_ASSERT(0);}
    virtual void                    drawTriangle            (   const Vertex& /*_v1*/, const Vertex& /*_v2*/, const Vertex& /*_v3*/, const Texture* /*_tex*/, bbool /*_showWireFrame*/ = 0) {ITF_ASSERT(0);}

#ifndef ITF_CONSOLE_FINAL
            void                    drawArrow               (   const Vec2d& /*pos1*/, const Vec2d& /*pos2*/, float /*_r*/, float /*_g*/, float /*_b*/, float /*_rad*/, float /*_z*/ = 0.0f, float /*_duration*/ = 0.f, bbool /*_forceRad*/ = bfalse  );
            void                    drawArrow               (   const Vec3d& /*pos1*/, const Vec3d& /*pos2*/, float /*_r*/, float /*_g*/, float /*_b*/, float /*_rad*/, float /*_duration*/ = 0.f, bbool /*_forceRad*/ = bfalse  );
    ITF_INLINE void                 drawArrow               (   const Vec2d& pos1, const Vec2d& pos2, Color _color = Color::white(), float _rad = 0.1f, float _z = 0.f, float _duration = 0.f, bbool _forceRad = bfalse ) { drawArrow(pos1, pos2, _color.m_r, _color.m_g, _color.m_b, _rad, _z, _duration, _forceRad ); }
    ITF_INLINE void                 drawArrow               (   const Vec3d& pos1, const Vec3d& pos2, Color _color = Color::white(), float _rad = 0.1f, float _duration = 0.f, bbool _forceRad = bfalse ) { drawArrow(pos1, pos2, _color.m_r, _color.m_g, _color.m_b, _rad, _duration, _forceRad ); }
#endif // ITF_CONSOLE_FINAL
    virtual void                    drawQuad2D              (   f32 /*_px*/, f32 /*_py*/, f32 /*_width*/, f32 /*_height*/, f32 /*_z*/, u32 /*_color*/ = COLOR_WHITE , const Texture* /*_tex*/ = NULL, const Vec2d* /*_uvStart */= NULL,  const Vec2d* /*_uvEnd */= NULL) {ITF_ASSERT(0);}
    virtual void                    draw3DQuad              (    GFX_3DQUAD* /*_t_Quad*/ ) {ITF_ASSERT(0);}
    virtual bbool                   loadTexture             (   Texture* /*_tex*/, const char* /*_path*/, u32 /*_alpha*/ = 0 ) {ITF_ASSERT(0);return bfalse;}
            Texture*                loadTexture             (   const char* /*_path*/, u32 /*_alpha*/   );

#ifdef ITF_GFX_PLATFORM_COMMON_FUNCTIONS
    void                            coord2DTo3D             (const Vec2d& /*_in2d*/, float /*_zPlane*/, Vec2d& /*_out3d*/, bool /*noTranslation*/ = btrue);
    void                            compute3DTo2D           (const Vec3d& /*_in*/, Vec3d& /*_out*/);
    void                            compute2DTo3D           (const Vec3d& /*_in*/, Vec3d& /*_out*/);
    void                            compute2DTo3D           (const Vec2d& /*_in*/, const f32 /*_worldZ*/, Vec3d& /*_out*/);
#else // ITF_GFX_PLATFORM_COMMON_FUNCTIONS
    virtual void                    coord2DTo3D             (const Vec2d& /*_in2d*/, float /*_zPlane*/, Vec2d& /*_out3d*/, bool /*noTranslation*/ = btrue);
    virtual void                    compute3DTo2D           (const Vec3d& /*_in*/, Vec3d& /*_out*/);
    virtual void                    compute2DTo3D           (const Vec3d& /*_in*/, Vec3d& /*_out*/);
            void                    compute2DTo3D           (const Vec2d& /*_in*/, const f32 /*_worldZ*/, Vec3d& /*_out*/);
#endif // ITF_GFX_PLATFORM_COMMON_FUNCTIONS

    virtual void                    drawScreenQuad          (    f32 /*_px*/, f32 /*_py*/, f32 /*_width*/, f32 /*_height*/, f32 /*_z*/, u32 /*_color*/, bbool /*_noTex*/ = 0 )   {ITF_ASSERT(0);}
    virtual                         ~GFXAdapter             (   );// {}// always declare virtual destructor for adapters
    virtual void                    init                    (   )  {ITF_ASSERT(0);}
    virtual void                    setCamera               (   float /*_x*/, float /*_y*/, float /*_z*/,class Camera* /*_cam*/);
    
    void                            GFX_computeMatrixLookAtRH                   (   GMatrix44* _view, Vec3d* _vEyePt, Vec3d* _vLookatPt, Vec3d* _vUpVec );
    void                            GFX_computeMatrixPerspectiveFovRH           (   GMatrix44* _proj, f32 _fov, f32 _aspect, f32 _zn, f32 _zf   );
    void                            GFX_computeMatrixOrthoOffCenterRH           (   GMatrix44* _proj, f32 _l, f32 _r, f32 _b, f32 _t, f32 _zn, f32 _zf );
    void                            computeRenderMatrix     (   );

    void                            vec3dProject            (   Vec3d* _out, const Vec3d* _v, const GFX_Viewport* _viewp, const GMatrix44* _ViewProj);
    void                            vec3dUnProject          (   Vec3d* _out, const Vec3d* _v, const GFX_Viewport* _viewp, const GMatrix44* _ViewProj_InvT);
    
    virtual void                    destroy();                    
    virtual void                    destroyResources();                    
    virtual void                    setBiasEyeLookAt        (const Vec3d &/*lookAtDir*/) {}
    virtual void                    init2DRender            (   )   {ITF_ASSERT(0);}
    virtual void                    end2DRender             (   )   {}
#ifndef ITF_WII
    virtual void                    init2DRenderSubScreen   (   )   {}
    virtual void                    init2DGuiRender         (   )   {ITF_ASSERT(0);}
#endif // ITF_WII
    virtual void                    setOrthoView            (   f32 /*_l*/, f32 /*_r*/, f32 /*_b*/, f32 /*_t*/  )   {ITF_ASSERT(0);}
#ifndef ITF_WII
    virtual void                    setOrthoView2DYNeg      (   f32 /*_l*/, f32 /*_r*/, f32 /*_b*/, f32 /*_t*/  )   {ITF_ASSERT(0);}
    virtual bbool                   isDrawingForScreenshot  (   )   {return bfalse;}
#endif // ITF_WII

    virtual void                    setAlphaBlend           (   GFX_BLENDMODE /*_blend*/    )   {ITF_ASSERT(0);}
    virtual void                    setCullMode             (   GFX_CULLMODE /*_cull*/    )  {ITF_ASSERT(0);}
    virtual void                    setFillMode             (   GFX_FILLMODE /*_fillmode*/  ) {ITF_ASSERT(0);}

    virtual void                    depthMask               (   u32 /*_depthMask*/    )   {ITF_ASSERT(0);}
    virtual void                    depthFunc               (   u32 /*_func*/    )   {ITF_ASSERT(0);}
    virtual void                    depthTest               (   u32 /*_test*/    )   {ITF_ASSERT(0);}

    float                           getSizeNextPOT          (   float sz    ) const;
    virtual void                    setScissorRect          (   GFX_RECT* /*_clipRect*/    )   {ITF_ASSERT(0);}
            bbool                   SetTextureResource      (   int _Sampler, ResourceID _Texture, bbool _linearFiltering = btrue);
    virtual void                    SetTextureBind          (   int /*_Sampler*/, void* /*_Bind*/, bbool /*_linearFiltering*/ = btrue)   {ITF_ASSERT_MSG(0, "not implemented for this platform");}
    virtual void                    lockTexture             (   Texture* /*_tex*/, LOCKED_TEXTURE* /*_lockTex*/, u32 /*_flag*/ = 0 )   {ITF_ASSERT(0);}
    virtual void                    unlockTexture           (   Texture* /*_tex*/   )   {ITF_ASSERT(0);}

    virtual void                    cleanupTexture          (   Texture* /*_texture*/   ) {ITF_ASSERT(0);}
    virtual void                    DrawPrimitive           (   PRIMITIVETYPE /*_type*/, const void * /*_p_Vertex*/, u32 /*_NumberVertex*/    ){ITF_ASSERT(0);}

    // Matrix.
    virtual void                    setMatrixTransform      (   MATRIXMODE /*_type*/, const GMatrix44* /*_Matrix*/    ) {ITF_ASSERT(0);}
    virtual void                    setupViewport           (    GFX_RECT* /*_Viewport*/    ) {ITF_ASSERT(0);}
#if !defined(ITF_WII) || !defined(ITF_FINAL)
    virtual void                    unprojectPoint          (    const Vec2d& /*_in*/, float /*_viewDistance*/, const GMatrix44* /*_MatrixModel*/, const GMatrix44* /*_MatrixProj*/, const int* /*_View*/, Vec2d& /*_out*/) {ITF_ASSERT(0);}
    virtual void                    setLookAtMatrix         (    GMatrix44* /*_MatrixOut*/, double /*_midx*/, double /*_midy*/, float /*_viewDistance*/, float /*_aspect*/) {ITF_ASSERT(0);}
#endif // !defined(ITF_WII) || !defined(ITF_FINAL)
    virtual void                    getViewport             (    GFX_RECT* /*_RectOut*/) {ITF_ASSERT(0);}

    /// GfxMaterials.
    virtual void                    setGfxMaterial          (   const GFX_MATERIAL& /*_gfxMat*/ ) {ITF_ASSERT(0);}
    virtual void                    setGfxMatDefault        (   ) {ITF_ASSERT(0);}
    virtual void                    setGfxMatRefraction     (   const GFX_MATERIAL& /*_gfxMat*/ ) {ITF_ASSERT(0);}
    void                            setGfxMatAlphaFade      (   const GFX_MATERIAL& _gfxMat );

    void                            postGfxMaterial         (   const GFX_MATERIAL& _gfxMat );

    /// Texture.
    virtual void                    generateTexture         (   Texture* /*_texture*/  )  {ITF_ASSERT(0);}
    virtual void                    createTexture           (   Texture* /*_texture*/, u32 /*_sizeX*/, u32 /*_sizeY*/, u32 /*_mipLevel*/, Texture::PixFormat /*_pixformat*/, u32 /*_pool*/ = 1, bbool /*_dynamic*/ = bfalse )  {ITF_ASSERT(0);}
    virtual void                    cleanupBaseTexture      (   Texture* /*_texture*/   ) {ITF_ASSERT(0);}
    virtual void                    setTextureSize          (   Texture* /*_texture*/, const Size& /*_size*/   ) {ITF_ASSERT(0);}
    virtual unsigned char*          grabTexture             (   Texture* /*_texture*/   ) {ITF_ASSERT(0);return NULL;}
    virtual u32                     getMaxTextureSize       (   ) {ITF_ASSERT(0);return 0;}
    virtual void                    loadFromMemory          (   const void* /*_buffer*/, const Size& /*_texture_size*/, Texture* /*_texture*/, Texture::PixFormat /*_pixel_format*/, const u32 /*_buffer_size*/, bbool /*bCooked*/,u32 _mipmapCount)  {ITF_ASSERT(0);}
    virtual bbool                   loadFromFileInMemory    (   Texture *_texture, const void* _buffer, u32 _contentSize) {ITF_ASSERT(0);return bfalse;}
    virtual void                    SetTexture              (   int /*_Sampler*/, Texture* /*_Texture*/, bbool /*_linearFiltering*/ = btrue) {ITF_ASSERT(0);}
    virtual void                    saveToMemory            (    void* /*_buffer*/, Texture* /*_Tex*/ ) {ITF_ASSERT(0);}
    const   Vec2d&                  getTexelScaling         (   ) const;
    void                            updateCachedScaleValues (   Texture* _Tex  );
    virtual float                   getTexelOffset          (   ) {ITF_ASSERT(0);return 0.0f;}

    virtual void                    setTextureAdressingMode (   u32 /*_samp*/, GFX_TEXADRESSMODE _u, GFX_TEXADRESSMODE _v);
    virtual void                    setSamplerState         (   u32 /*_samp*/, GFX_SAMPLERSTATETYPE /*_type*/, i32 /*_value*/ ) {ITF_ASSERT(0);}

    // Texture Target.   
    virtual void                    clearRenderTarget       (   renderTarget* /*_target*/, u32 /*_color*/   ) {ITF_ASSERT(0);}
    virtual void                    initialiseRenderTarget  (   renderTarget* /*_target*/   ) {ITF_ASSERT(0);}
    virtual void                    resizeRenderTarget      (   renderTarget* /*_target*/, const Size& /*_size*/) {ITF_ASSERT(0);}
    virtual void                    enableRenderTarget      (   renderTarget* /*_target*/   ) {ITF_ASSERT(0);}
    virtual void                    disableRenderTarget     (   renderTarget* /*_target*/   ) {ITF_ASSERT(0);}
    virtual void                    cleanupRenderTarget     (   renderTarget* /*_target*/   ) {ITF_ASSERT(0);}
    virtual bbool                   isRenderTargetInverted  (   ) {ITF_ASSERT(0);return bfalse;}
            renderTarget*           getNewRenderTarget      (   );
            void                    deleteRenderTarget      (   renderTarget* _target   );
        
    virtual void                    clear                   (   u32 /*_buffer*/, float /*_r*/=0.f, float /*_g*/=0.f, float /*_b*/=0.f, float /*_a*/=0.f){ITF_ASSERT(0);}

    virtual void                    present                 (   ) {ITF_ASSERT(0);}
    virtual void                    startRendering          (   ) {ITF_ASSERT(0);}
    virtual void                    endRendering            (   ) {ITF_ASSERT(0);}
    virtual bbool                   waitPreviousGPUframeEnd (   ) {ITF_ASSERT(0); return bfalse;}
    virtual void                    syncGPU                 (   ) {ITF_ASSERT(0);}

    virtual bbool                   setScreenProtection(bbool _enable) { return btrue; }

    GFXAdapter();
    GFX_BLENDMODE                   u_CurrentBlendingMode;
    bbool                           mb_WireFrame;
    bbool                           getSceneResolvedFlag()const         { return m_SceneAlreadyResolve; }
    void                            setSceneResolvedFlag(bbool _val)    { m_SceneAlreadyResolve = _val; }
    void                            resetSceneResolvedFlag()            { m_SceneAlreadyResolve = bfalse; }

    GFX_Zlist                       m_RenderZlist;
    GFX_Zlist                       m_RenderZlistRTarget;
    GFX_Zlist                       m_AfterFxZlist;

#ifdef ITF_SUPPORT_CHEAT
    void                            drawDBGText             (   const String& _text, float _x = ITF_POS2D_AUTO, float _y = ITF_POS2D_AUTO, f32 _r = 1.f, f32 _g = 1.f, f32 _b = 1.f, bbool _useBigFont = bfalse,FONT_ALIGNMENT _alignment=FONT_ALIGN_LEFT );
    void                            drawDBGTextFixed        (   const String& _text, float _x = ITF_POS2D_AUTO, float _y = ITF_POS2D_AUTO, f32 _r = 1.f, f32 _g = 1.f, f32 _b = 1.f, FONT_ALIGNMENT _alignment=FONT_ALIGN_LEFT );
    ITF_INLINE void                 drawDBGText             (   const String& _text, const Color& _color, float _x = ITF_POS2D_AUTO, float _y = ITF_POS2D_AUTO ) { drawDBGText(_text, _x, _y, _color.m_r, _color.m_g, _color.m_b); }
#endif // ITF_SUPPORT_CHEAT

#ifndef ITF_CONSOLE_FINAL
    void                            drawDBGCircle           (   float _x, float _y, float _radius, float _r=1, float _g=1, float _b=1, float _duration = 0.f, float _z = 0.f, u32 _segsCount = 16);
    ITF_INLINE void                 drawDBGCircle           (   const Vec2d& _pos, float _radius, float _r=1, float _g=1, float _b=1, float _duration = 0.f, u32 _segsCount = 16) { drawDBGCircle(_pos.m_x, _pos.m_y, _radius, _r, _g, _b, _duration, 0.f, _segsCount); }
    ITF_INLINE void                 drawDBGCircle           (   const Vec3d& _pos, float _radius, float _r=1, float _g=1, float _b=1, float _duration = 0.f, u32 _segsCount = 16) { drawDBGCircle(_pos.m_x, _pos.m_y, _radius, _r, _g, _b, _duration, _pos.m_z, _segsCount); }
    void                            drawDBGSphere           (   float _x, float _y, float _radius, float _r=1, float _g=1, float _b=1, float _duration = 0.f, float _z = 0.f, u32 _segsCount = 16, bbool _volume = btrue, bbool _xaxis = btrue, bbool _yaxis = btrue, bbool _zaxis = btrue );
    void                            drawDBGBox              (   const Vec2d& _pos, f32 _angle, const Vec2d& _extent, float _r=1, float _g=1, float _b=1, float _duration = 0.f, float _z = 0.f, bbool _fill = bfalse );
    void                            drawDBGLine             (   const Vec2d& _p1, const Vec2d& _p2, float _r = 1, float _g = 1, float _b = 1, float _duration = 0.f, float _size = 1.f, float _z =0.f, float _alpha = 1.f );
    void                            drawDBG3DLine           (   const Vec3d& _p1, const Vec3d& _p2, float _r = 1, float _g = 1, float _b = 1, float _duration = 0.f, float _size = 1.f);
    void                            drawDBGTriangle         (   const Vec2d& _p1, const Vec2d& _p2, const Vec2d& _p3, float _r=1, float _g=1, float _b=1, float _a=1, float _duration = 1.f, float _z = 0.0f );
    void                            drawDBGTextureTriangle  (   ITF_VECTOR<Vertex>* _pVtx, Texture* _tex, float _duration = 0.f, float _z = 0.0f );
    void                            drawDBGQuad             (   const Vec2d& _pos, f32 _width, f32 _height, const Texture * _tex, f32 _angle = 0.0f, float _duration = 0.f, float _z = 0.0f, int _indexInAtlas=0, bbool _flipX=bfalse, bbool _flipY=bfalse, u32 _color = COLOR_WHITE  );
    void                            drawDBGAABB             (   const AABB& _aabb, float _r = 1, float _g = 1, float _b = 1, float _duration = 0.f, float _size = 1.f, float _z = 0.f );
    DBGLine*                        getDBGLineBuffer        (   u32 _count );
    DBG3DLine*                      getDBG3DLineBuffer      (   u32 _count );

    // Color overloads
    ITF_INLINE void                 drawDBGCircle           (   float _x, float _y, float _radius, const Color& _color, float _duration = 0.f, float _z = 0.f, u32 _segsCount = 16) { drawDBGCircle(_x, _y, _radius, _color.m_r, _color.m_g, _color.m_b, _duration, _z, _segsCount); }
    ITF_INLINE void                 drawDBGCircle           (   const Vec2d& _pos, float _radius, const Color& _color, float _duration = 0.f, u32 _segsCount = 16) { drawDBGCircle(_pos.m_x, _pos.m_y, _radius, _color.m_r, _color.m_g, _color.m_b, _duration, 0.f, _segsCount); }
    ITF_INLINE void                 drawDBGCircle           (   const Vec3d& _pos, float _radius, const Color& _color, float _duration = 0.f, u32 _segsCount = 16) { drawDBGCircle(_pos.m_x, _pos.m_y, _radius, _color.m_r, _color.m_g, _color.m_b, _duration, _pos.m_z, _segsCount); }
    ITF_INLINE void                 drawDBGBox              (   const Vec2d& _pos, f32 _angle, const Vec2d& _extent, const Color& _color, float _duration = 0.f, float _z = 0.f ) { drawDBGBox(_pos, _angle, _extent, _color.m_r, _color.m_g, _color.m_b, _duration, _z); }
    ITF_INLINE void                 drawDBGLine             (   const Vec2d& _p1, const Vec2d& _p2, const Color& _color, float _duration = 0.f, float _size = 1.f, float _z = 0.f ) { drawDBGLine(_p1, _p2, _color.m_r, _color.m_g, _color.m_b, _duration, _size, _z, _color.m_a); }
    ITF_INLINE void                 drawDBG3DLine           (   const Vec3d& _p1, const Vec3d& _p2, const Color& _color, float _duration = 0.f, float _size = 1.f) { drawDBG3DLine(_p1, _p2, _color.m_r, _color.m_g, _color.m_b, _duration, _size); }
    ITF_INLINE void                 drawDBGTriangle         (   const Vec2d& _p1, const Vec2d& _p2, const Vec2d& _p3, const Color& _color, float _duration = 1.f, float _z = 0.0f ) { drawDBGTriangle(_p1, _p2, _p3, _color.m_r, _color.m_g, _color.m_b, _color.m_a, _duration, _z); }
    ITF_INLINE void                 drawDBGAABB             (   const AABB& _aabb, const Color& _color, float _duration = 0.f, float _size = 1.f, float _z = 0.f ) { drawDBGAABB(_aabb, _color.m_r, _color.m_g, _color.m_b, _duration, _size, _z); }
#endif // ITF_CONSOLE_FINAL

    /// extended primitives.

    bbool                           drawPatch32PT(Patch32Data *pData, Texture *texture, const Vec2d *PointsNS, u32 _hdiv, u32 _vdiv, f32 _z);
    bbool                           drawPatch32PCT(Patch32Data *_pData, Texture *texture, Vec2d *PointsNS, u32 _hdiv, u32 _vdiv, ColorInteger *_color, f32 _z);
   // bbool                           drawPatch32PCTSIMD(Patch32Data *_pData, Texture *texture, Vec2d *PointsNS, u32 _hdiv, u32 _vdiv, Color *_color, f32 _z);
    bbool                           drawPatch32PCTOneColor(Patch32Data *_pData, Texture *texture, Vec2d *PointsNS, u32 _hdiv, u32 _vdiv, u32 _generalColor, f32 _z);

    bbool                           drawBezierPatch32PT(Patch32Data *_pData, Texture *texture, const Vec2d *PointsNS, const f32* _widths, const f32* _widths2, const f32* _alphas, u32 _hdiv, u32 _vdiv, f32 _z);
    bbool                           drawBezierPatch32PCT(Patch32Data *_pData, Texture *texture, const Vec2d *PointsNS, const f32* _widths, const f32* _widths2, const f32* _alphas, u32 _hdiv, u32 _vdiv, const ColorInteger *_color, f32 _z);
    bbool                           drawBezierPatch32PCTOneColor(Patch32Data *_pData, Texture *texture, const Vec2d *PointsNS, const f32* _widths, const f32* _widths2, const f32* _alphas, u32 _hdiv, u32 _vdiv, u32 _generalColor, f32 _z);

    Texture*                        RENDER_TARGET_DEBUG;

    void                            removePrimitiveFromRenderLists (   PRIMITIVETYPE _type, void* _primitive    );
    ITF_INLINE u32                  getScreenWidth          (   ) const                 {   return m_screenWidth;}
    ITF_INLINE u32                  getScreenHeight         (   ) const                 {   return m_screenHeight;}
    ITF_INLINE const AABB &         getScreenAABB           (   ) const                 {   return m_screenAABB; }         
    void                            setResolution           (   u32 _screenWidth, u32 _screenHeight    );               
    void                            setWaitVBL              (   bbool _waitVBL  ) {m_waitVBL = _waitVBL;}
    void                            forceScreenSize         (   u32 _screenWidth, u32 _screenHeight    ){m_screenWidth = _screenWidth; m_screenHeight = _screenHeight;}               

    virtual void                    reshape                 (   ) {ITF_ASSERT(0);}

    virtual void                    createDebugFonts        (u32 _screenwidth, u32 _screenHeight) {};

    virtual bbool                   isDeviceLost            (   ) {ITF_ASSERT(0);return bfalse;}
    virtual bbool                   resetDevice             (   bbool /*_forceReset*/   ){ITF_ASSERT(0);return bfalse;}

    // text
    void                            initFont();
    void                            drawText(unsigned char *Txt,float X, float Y, float FX, float FY , u32 ColorRGBA , u32 DrawModeShadow);
    
    ITF_INLINE RenderTargetManager* getTargetManager        (   )   {   return m_RenderTargetManager;   };                     

    /// fps from system information.
    ITF_INLINE f32                  getfPs                  (   )   {  return m_fPs;};
    ITF_INLINE void                 setfPs                  (   f32 _fps) {  m_fPs = _fps;  };

    ITF_INLINE void                 setPause                (   bbool _pause  ) {  m_paused = _pause;  };
    ITF_INLINE bbool                isGFXPaused             (   ) {  return m_paused;  };

    /// Vertex declaration.
    virtual void                    setVertexFormat         (   u32 /*_vformat*/    ) {ITF_ASSERT(0);}

    virtual const String            getRenderName           (   ) = 0;

    u32 getRenderZNumberEntry()     {return m_RenderZlist.getNumberEntry();}

    /// Shaders.
    virtual void                    createShader            (   ITF_shader* /*_shader*/ ) {ITF_ASSERT(0);}
    virtual void                    releaseShader           (   ITF_shader* /*_shader*/ ) {ITF_ASSERT(0);}
    virtual bbool                   loadShader              (   ITF_shader* /*_shader*/ ) {ITF_ASSERT(0);return bfalse;}
    virtual void                    beginShader             (   ITF_shader* /*_shader*/ ) {ITF_ASSERT(0);}
    virtual void                    endShader               (   ITF_shader* /*_shader*/ ) {ITF_ASSERT(0);}

    virtual void                    fx_setVectorArray       (   uPtr /*_fx*/, void* /*_h*/, const GFX_Vector4* /*_va*/, u32 /*_count*/) {ITF_ASSERT(0);}
    virtual void                    fx_setVector            (   uPtr /*_fx*/, void* /*_h*/, const GFX_Vector4* /*_v*/) {ITF_ASSERT(0);}
    virtual void                    fx_setFloat             (   uPtr /*_fx*/, void* /*_h*/, float/*_f*/) {ITF_ASSERT(0);}
    virtual void                    fx_setInt               (   uPtr /*_fx*/, void* /*_h*/, i32/*_i*/) {ITF_ASSERT(0);}
    virtual void                    fx_commitChange         (   uPtr /*_fx*/ ) {ITF_ASSERT(0);}

#if defined(ANIMUSESHADER)
    /// Patch shader.
    virtual void                    prepareShaderPatch      (   GMatrix44* /*_matrix*/, f32 /*_z*/, u32 &/*_hdiv*/, u32 &/*_vdiv*/, GFX_BLENDMODE _blendMode = GFX_BLEND_ALPHA ) {ITF_ASSERT(0);}    
    virtual void                    shaderDrawPatch32       (   const Texture* /*texture*/, const Vec2d* /*PointsNS*/,const ColorInteger* /*_lighttab*/ ){ITF_ASSERT(0);}
    virtual void                    endShaderPatch          (   ) {ITF_ASSERT(0);}
    virtual void                    shaderPatchFlush        (   u32 /*_hdiv*/, u32 /*_vdiv*/, u32 /*_pass*/ ) {ITF_ASSERT(0);}
#endif // defined(ANIMUSESHADER)

    /// Bezier Patch shader.
#ifdef ANIMUSESHADER
    virtual void                    prepareShaderBezierPatch      (   GMatrix44* /*_matrix*/, f32 /*_z*/, u32 &/*_hdiv*/, u32 &/*_vdiv*/, GFX_BLENDMODE _blendMode = GFX_BLEND_ALPHA ) {ITF_ASSERT(0);}    
    virtual bbool                   shaderDrawBezierPatch32       (   Texture* /*texture*/, Vec2d* /*PointsNS*/, ColorInteger* /*_lighttab*/, f32* /*_widths*/, f32* /*_alphas*/, f32* /*_widths2*/ ){ITF_ASSERT(0);return bfalse;}
    virtual void                    shaderBezierPatchFlush        (   u32 /*_hdiv*/, u32 /*_vdiv*/, u32 /*_pass*/, u32 /*_vdivToDraw*/ ) {ITF_ASSERT(0);}
#elif defined(ANIMUSEGEOMETRYSHADER)
    /// Patch drawing using geometry shader.
    virtual bool                    prepareGeomShaderPatch  (   GMatrix44* /*_matrix*/, SafeArray<AnimBoneRenderable>& /*_bones*/, f32 /*_z*/, u32 /*_hdiv*/, u32 /*_vdiv*/, bbool /*_mirror*/ ) {ITF_ASSERT(0); return false;}    
    virtual bbool                   drawAnimatedPatches     (   ITF_Mesh& /*_mesh*/  ){ITF_ASSERT(0);return bfalse;}
    virtual void                    endGeomShaderPatch      (   ) {ITF_ASSERT(0);}
#endif // defined(ANIMUSEGEOMETRYSHADER)

    /// Spline Shader
    virtual void                    drawSpline              (   GMatrix44* _matrix, Texture* _texture, const Spline * _spline, const Vec3d &_pos, f32 _height ) {ITF_ASSERT(0);}

    /// Fluid Shader.
    virtual void                    drawFluid               (   GMatrix44* _matrix, Texture* _texture, const FixedArray<u32, GFXADAPTER_FLUID_COLORTAB_ENTRY_COUNT> &_colorTab, const Vec2d _heightTab[], const u32 _heightTabSize, const FixedArray<Vec2d, 4> &_uvTab, u32 mode, f32 _delta1, f32 _delta2, f32 _stepCount ) {ITF_ASSERT(0);}

    /// Trail 3d Shader.
    virtual void                    drawTrail3D             (   Texture* _texture, Color _color, const SafeArray<Vec3d> & points, f32 _alphaBegin, f32 _alphaEnd, f32 _fadeLength ) {ITF_ASSERT(0);}

    //Movie    
    virtual void                    drawMovie               ( GMatrix44* _matrix,f32 _alpha,ITF_VertexBuffer* _pVertexBuffer,Texture** _pTextureArray,u32 _countTexture) {ITF_ASSERT(0);}

    /// VertexBuffer.
    virtual void                    createVertexBuffer      (   ITF_VertexBuffer* /*_vertexBuffer*/ ) {ITF_ASSERT(0);}
    virtual void                    releaseVertexBuffer     (   ITF_VertexBuffer* /*_vertexBuffer*/ ) {ITF_ASSERT(0);}
    virtual void                    LockVertexBuffer        (   ITF_VertexBuffer* /*_vertexBuffer*/, void** /*_data*/, u32 /*_offset*/, u32 /*_size*/, u32 /*_flag*/ ) {ITF_ASSERT(0);}
    virtual void                    UnlockVertexBuffer      (   ITF_VertexBuffer* /*_vertexBuffer*/ ) {ITF_ASSERT(0);}

    /// IndexBuffer.
    virtual void                    createIndexBuffer       (   ITF_IndexBuffer* /*_indexBuffer*/ ) {ITF_ASSERT(0);}
    virtual void                    releaseIndexBuffer      (   ITF_IndexBuffer* /*_indexBuffer*/ ) {ITF_ASSERT(0);}
    virtual void                    LockIndexBuffer         (   ITF_IndexBuffer* /*_indexBuffer*/, void** /*_data*/ )  {ITF_ASSERT(0);}
    virtual void                    UnlockIndexBuffer       (   ITF_IndexBuffer* /*_indexBuffer*/ )  {ITF_ASSERT(0);}
    
    /// Vertex/Index Buffer Draw
    virtual void                    DrawVertexBuffer        (   u32 /*_type*/, ITF_VertexBuffer* /*_vertexBuffer*/, u32 /*_vertexStart*/, u32 /*_vertexNumber*/ )  {ITF_ASSERT(0);}
    virtual void                    DrawIndexedVertexBuffer (   u32 /*_type*/, ITF_VertexBuffer* /*_vertexBuffer*/, ITF_IndexBuffer* /*_indexBuffer*/, u32 /*_indexNumber*/ )  {ITF_ASSERT(0);}
    virtual void                    drawMesh                (   ITF_Mesh& /*_mesh*/, bbool /*_showWireFrame*/){ITF_ASSERT(0);};
    virtual void                    drawMeshPT              (   const ITF_Mesh& /*_mesh*/, bbool /*_showWireFrame*/) {ITF_ASSERT(0);}
#ifndef ITF_CONSOLE_FINAL
    virtual void                    drawFlatMesh(const ITF_Mesh& /*_mesh*/)  {ITF_ASSERT(0);}
#endif // ITF_CONSOLE_FINAL

    virtual void                    setShaderForFont(bbool /*bOutline*/)  {ITF_ASSERT(0);}
    virtual void                    restoreDefaultShader()  {ITF_ASSERT(0);}

    void cleanBufferEndOfFrame();

    /// Matrix.
    ITF_INLINE void                 setMatrixIdentity       (   GMatrix44& _matrix  ) { _matrix = GMatrixIdentity; };

    int                             FBTexture[10];

    virtual const GMatrix44*        getViewMatrix(   ) {ITF_ASSERT(0);return NULL;}

    virtual void					AFTERFX_BigBlur( f32 /*_factor*/, u32 /*_MaxPass*/, bbool /*_clearRT*/, GFX_QUALITY /*_quality*/ = GFX_QUALITY_MEDIUM)  {ITF_ASSERT(0);}
    virtual void        AFTERFX_Glow( f32 /*_factor*/, f32 /*_glowfactor*/, f32 /*_addalpha*/, GFX_BLENDMODE _finalBlendMode = GFX_BLEND_COPY)  {ITF_ASSERT(0);}
    virtual void					AFTERFX_objectsGlow(f32 /*_factor*/)  {ITF_ASSERT(0);}
    virtual void        AFTERFX_RayCenter( f32 /*_v1*/, f32 /*_v2*/, f32 /*_v3*/, f32 /*_v4*/, f32 /*_v5*/, f32 /*_v6*/, f32 /*_v7*/)  {ITF_ASSERT(0);}
    virtual void        AFTERFX_Remanence( f32 /*_factor*/, f32 /*_glowfactor*/, f32 /*_addalpha*/)  {ITF_ASSERT(0);}
    virtual void        AFTERFX_ColorSetting(f32 /*_sat*/, f32 /*_contrast*/, f32 /*_contrastscale*/, f32 /*_bright*/ )  {ITF_ASSERT(0);}
    virtual void        AFTERFX_ColorRemap(Texture* )  {ITF_ASSERT(0);}
    virtual void        AFTERFX_ColorLevels(f32 /*_inBlack*/,  f32 /*_inGamma*/, f32 /*_inWhite*/, f32 /*_outBlack*/, f32 /*_outWhite*/)  {ITF_ASSERT(0);}
    virtual void        AFTERFX_addSceneAndMul(f32 /*_mul*/)  {ITF_ASSERT(0);}
    virtual void        AFTERFX_byTech(u32 /*_tech*/)  {ITF_ASSERT(0);}
    virtual void        AFTERFX_byTechParams(u32 /*_tech*/, f32 /*_p0*/, f32 /*_p1*/, f32 /*_p2*/, f32 /*_p3*/)  {ITF_ASSERT(0);}
    virtual void        AFTERFX_startRenderTarget(Color /*_color*/   ) {ITF_ASSERT(0);}
    void                AFTERFX_colorScreen(Color _color, i32 _blendMode = 1 );
    void                AFTERFX_borderBright(f32 _factor, f32 _attenuation, Color _color, i32 _blendMode);
    virtual void        AFTERFX_zoomCenter(f32 /*_factor*/, Vec3d /*_dir*/ ) {ITF_ASSERT(0);}

    virtual void        prepareAfterFx(bbool /*_nocopy*/) {ITF_ASSERT(0);}
    virtual void        endAfterfx(bbool /*_nocopy*/, GFX_BLENDMODE /*_finalBlend*/) {ITF_ASSERT(0);}
    virtual void        prepareInternalRT() {ITF_ASSERT(0);}
    virtual void        endInternalRT() {ITF_ASSERT(0);}


    virtual void        setGammaRamp(Vec3d gamma,f32 brigthnessValue,f32 contrastValue) {ITF_ASSERT(0);}

    virtual void        AFTERFX_draw(AFX_Group* /*_group*/) {ITF_ASSERT(0);}
    void							AFTERFX_drawOne(AFTERFX* _afx);

    virtual void					AFTERFX_prepareObjectGlow() {ITF_ASSERT(0);}
    void							AFTERFX_processObjectGlow(f32 _factor);

    // afterfx options:
    GFX_Zlist*          getAfterfxList() {return &m_AfterFxZlist;}

    bbool               m_AFX_needCopyBuffer;
    bbool               m_AFX_needFullRt;
    bbool               m_AFX_needRestituteBackBuffer;
    void                setInternalRTcolor(const Color& _color) {m_internRTColor = _color;}
    void                setUseInternalRT(bbool _val) {m_useInternalRT = _val;}
    bbool               isUseInternalRT() {return m_useInternalRT;}

    const Color&        getInternalRTcolor(   )const { return m_internRTColor; }
    Color               m_internRTColor;
    bbool               m_useInternalRT;

    virtual void        startImpostorRendering(renderTarget* /*_rt*/, f32 /*xmin*/, f32 /*ymin*/, f32 /*xmax*/, f32 /*ymax*/, bbool _mustClear=btrue )  {ITF_ASSERT(0);}
    virtual void        stopImpostorRendering(renderTarget* /*_rt*/)  {ITF_ASSERT(0);}

    u32                  loadFont(const Path& name);
    const ResourceID&    getFontByIndex(u32 _index)    {return m_Fonts[_index];};
    const u32            getFontsCount()                {return m_Fonts.size();}

    /// Surface.
    virtual f32         ReadSurfaceFloat(void* /*_surf*/, u32 /*_posX*/, u32 /*_posY*/)  {ITF_ASSERT(0);return 0.0f;}

    //No required on some adapters
#ifdef ITF_WINDOWS
    virtual void        setRenderTarget_Picking() {};
    virtual void        setRenderTarget_BackBuffer() {};
    virtual void*       resolvePicking() {return NULL;};

    void                setPickingMode(bbool bMode,const Vec2d& _PickingPos,f32 _delta) {m_bPickingMode = bMode;m_PickingPos = _PickingPos;m_deltaPicking = _delta;}
    bbool               isPickingMode()             {return m_bPickingMode;}
#endif  //ITF_WINDOWS

    virtual Color       getBufferValue(void* pBackBuffer,u32 /*_posX*/, u32 /*_posY*/) {return Color::black();}
    virtual void        doScreenCapture() {};
    virtual void        setAlphaRef(int /*_ref*/ =1) {}
    
    void                setSeparateAlpha(bbool _val) {m_useSeparateAlpha =_val; setBlendSeparateAlpha();}
    virtual void        setBlendSeparateAlpha() {}
    void                toggleSeparateAlpha() {setSeparateAlpha(!m_useSeparateAlpha);}
    bbool               getSeparateAlpha() {return m_useSeparateAlpha;}

    void                setOverDrawMode(u32 _val) {m_overDrawMode =_val;if (m_overDrawMode>3) {m_overDrawMode = 0;}}
    void                switchOverDrawMode() {setOverDrawMode(++m_overDrawMode);}
    u32                 getOverDrawMode() {return m_overDrawMode;}
    virtual void        startOverDrawRecord() {ITF_ASSERT(0);}
    virtual void        showOverDrawRecord() {ITF_ASSERT(0);}

    void                setSafeFrameMode(i32 _mode) {m_safeFrameMode =_mode;if (m_safeFrameMode>=(i32)SF_COUNT) {m_safeFrameMode = 0;}}
    void                switchSafeFrameMode() {setSafeFrameMode(++m_safeFrameMode);}
    i32                 getSafeFrameMode() {return m_safeFrameMode;}
    const char *        computeSafeFrame(Vec2d &_margin, Vec2d &_safeSize);
#ifndef ITF_CONSOLE_FINAL
    void            requestSafeFrameDraw()      { m_drawSafeFrameRequested = btrue; }
#endif // ITF_CONSOLE_FINAL

    void                setWireFrameMode(u32 _mode) {m_showWireFrame =_mode;if (m_showWireFrame>2) {m_showWireFrame = 0;}}
    void                switchWireFrameMode() {setWireFrameMode(++m_showWireFrame);}
    u32                 getWireFrameMode() {return m_showWireFrame;}

    virtual bbool       isAlphaTestEnabled(void) const {return m_isAlphaRefEnabled;}
    virtual void        setAlphaTest(bbool _enabled) {m_isAlphaRefEnabled = _enabled;}

    virtual void        startScreenCapture() {};
    void                stopScreenCapture();
    void                cancelScreenCapture();

    virtual void        setThreadUsageScreenCapture(u32 _uThreadUsage,bbool _bStepByStep);

    /// UV.
    virtual void        setUVAnim(GFX_UVANIM& /*_uvanim*/)  {ITF_ASSERT(0);}
    void                computeUVAnim(GFX_UVANIM* _uvanim);

    virtual void*       compilePixelShader(const char* _bufferText,u32 len, const char* _functionName) {return NULL;}
	virtual void*       compileVertexShader(const char* _bufferText,u32 len, const char* _functionName) {return NULL;}

#ifndef ITF_FINAL
    void     toggleRaster    (   );
    ITF_INLINE void     toggleAfterFx   (   )   {   m_showAfterFx = !m_showAfterFx;};
    ITF_INLINE  bbool   isShowRasters   (   )   {   return m_showRasters;   };
    ITF_INLINE  bbool   isShowAfterFx   (   )   {   return m_showAfterFx;   };
    
    ITF_INLINE void     toggleDebugInfo (   )   {   m_showDebugInfo = !m_showDebugInfo;};
    ITF_INLINE  bbool   isShowDebugInfo (   )   {   return m_showDebugInfo;   };
#endif // ITF_FINAL

    void fillTriangleList(const DBGLine& l);
    void fillTriangleList(const DBG3DLine& l);
    void fillTriangleList(const DBGTriangle3D& t);

	void drawText(const ResourceID& _fontResource, i32 _depthrank, u32 _color, f32 _x, f32 _y, f32 _z, const Vec2d& _shadowOffset, u32 _shadowColor, const String& _text, i32 _count, u32 _mode, f32 _height = -1.f, bbool _isWriteML = bfalse);
    void drawTextWithStyle(FontCall* _call, i32 _depthrank);
            void reloadFont();
    void appendText(const ResourceID& _fontResource, i32 _depthrank, u32 _color, const char* _text);
    
    void                cleanFontCache();
    void                destroyFonts();
    bbool               isFontLoaded(u32 index);

    ITF_INLINE  bbool isRenderSceneInTarget() {return m_RenderSceneInTarget;};
    ITF_INLINE  void setRenderSceneInTarget(bbool _val) {m_RenderSceneInTarget = _val;};

    /// Render Context.
    virtual void        renderContext_Set()  {ITF_ASSERT(0);}
    virtual void        renderContext_SetDefault()  {ITF_ASSERT(0);}
    ITF_INLINE  bbool rC_getZWRITE() {return m_RContext_ZWRITE;};
    ITF_INLINE  void rC_setZWRITE(bbool _val) {m_RContext_ZWRITE = _val;};
    
    ITF_INLINE  bbool rC_getZTEST() {return m_RContext_ZTEST;};
    ITF_INLINE  void rC_setZTEST(bbool _val) {m_RContext_ZTEST = _val;};

    /// static vb.
    static const UVSubdiv       bufferTypes[];
    static const u32            bufferTypesNb;

    static String8              getAllowedBufferTypesStr();


    ITF_IndexBuffer*            createInternalIB(u32 _hdiv, u32 _vdiv);
    ITF_VertexBuffer*           createInternalVB(u32 _hdiv, u32 _vdiv);

    void                        createInternalBuffers();
    void                        removeInternalBuffers();

    ITF_IndexBuffer*            getIndexBuffer(u32 & _hdiv, u32 & _vdiv);
    ITF_VertexBuffer*           getVertexBuffer(u32 & _hdiv, u32 & _vdiv);

    /// Fog.
    f32                         computeFogFactor();
    ITF_INLINE void             setUseFog(bbool _v) { m_useFog = _v;}
    ITF_INLINE bbool            isUseFog() { return m_useFog;}

    /// Global Color.
    virtual void        setGlobalColor(const Color& ) {ITF_ASSERT(0);}
    virtual void        setFogColor(const Color& ) {ITF_ASSERT(0);}
    ITF_INLINE void     setGFXGlobalColor(const Color& _color) {m_globalColor = _color;}
    ITF_INLINE Color    getGFXGlobalColor() {return m_globalColor;};

    ITF_VertexBuffer* createVertexBuffer        (   u32 _vertexNumber, int _vertexformat, u32 _structVertexSize, bbool _dynamic, dbEnumType _type = VB_T_NOTDEFINED );
    ITF_IndexBuffer* createIndexBuffer          (   u32 _indexNumber, bbool _dynamic    );
    void removeVertexBuffer                     (   ITF_VertexBuffer* _VertexBuffer     );
    void removeVertexBufferDelay                (   ITF_VertexBuffer* _VertexBuffer ,u32 _frameCount    );
    void removeIndexBuffer                      (   ITF_IndexBuffer* _IndexBuffer   );
    void removeIndexBufferDelay                (   ITF_IndexBuffer* _IndexBuffer ,u32 _frameCount     );

    ITF_INLINE GFXAdapter_VertexBufferManager& getVertexBufferManager() {return m_VertexBufferManager;};

    ITF_INLINE  void setUseDepthBuffer(bbool _val) {m_useDepthBuffer = _val;};
    ITF_INLINE  bbool isUseDepthBuffer() {return m_useDepthBuffer;};

    ITF_INLINE  u32     getSpriteIndexBufferSize() {return m_spriteIndexBufferSize;};
    ITF_IndexBuffer*    needSpriteIndexBuffer(u32 _numberIndex);

    ITF_INLINE const ColorInteger* getDefault4WhiteColorTab() {return m_default4WhiteColorTab;};


    virtual void        acquireDeviceOwnerShip()         {ITF_ASSERT(0);}
    virtual void        releaseDeviceOwnerShip()         {ITF_ASSERT(0);}
    virtual void        minimal_present()                {ITF_ASSERT(0);}

    void                addHideFlags(const u32 _flags) {m_HideFlags |= _flags;}
    void                removeHideFlags(const u32 _flags) {m_HideFlags &= ~_flags;}
    bbool               hasHideFlags(const u32 _flags) const {return m_HideFlags & _flags;}
    u32                 getHideFlags() const {return m_HideFlags;}
    void                setHideFlags(u32 _flags) {m_HideFlags = _flags;}

    void                setUseRotateCamera(bbool _v) {m_useRotateZCamera = _v;}
    bbool               getUseRotateCamera() {return m_useRotateZCamera;}

    void                setMonitorRefreshRate(f32 _monitor) {m_monitorRefreshRate= _monitor;}
    ITF_INLINE f32      getMonitorRefreshRate()    const    {return m_monitorRefreshRate;}

    void                setDisplayDebugStats(bbool _val)    {m_displaydebugstats = _val;}
    ITF_INLINE bbool    getDisplayDebugStats()              {return m_displaydebugstats;}


#ifndef ITF_DISABLE_DEBUGINFO
    void                setDebugExtraInfo(const String& _extraInfo)         {m_extraInfo = _extraInfo;}
    void                setEngineDataVersion(const String& _extraInfo)      {m_engineDataVersion = _extraInfo;}
#endif //ITF_DISABLE_DEBUGINFO

   // GroupMeshManager*   getGroupMeshManager() {return m_groupMeshManager;}

#ifdef ITF_GFX_PLATFORM_COMMON_FUNCTIONS
    // Matrix.
    GMatrix44           mg_World;
    GMatrix44           mg_View;
    GMatrix44           mg_ViewRender;
    GMatrix44           mg_Proj;
    GMatrix44           mg_WorldViewProj;       // mg_World * mg_View * mg_Proj

    // Pre-computed Matrix for 2D<->3D tranformations
    GMatrix44           mg_ViewProj;            // mg_View * mg_Proj
    GMatrix44           mg_ViewProj_Inv;        // inverse of mg_ViewProj
    GMatrix44           mg_ViewNoTProj;         // (mg_View without translation) * mg_Proj
    GMatrix44           mg_ViewNoTProj_Inv;     // inverse of mg_ViewNoTProj

    // UV animated Matrix
    GMatrix44           mg_UVmat;
#endif // ITF_GFX_PLATFORM_COMMON_FUNCTIONS

#ifndef ITF_DISABLE_DEBUGINFO
    String            m_extraInfo;
    String            m_engineDataVersion;
#endif //ITF_DISABLE_DEBUGINFO

// take a screenshot [begin]
private:
    const char* m_strSceenshotResquest;
    

public:
#ifdef ITF_SUPPORT_EDITOR
    void requestProfilerFPScaptureMode() { m_strSceenshotResquest = strScreenshotProfileFPS(); }
#endif //ITF_SUPPORT_EDITOR

    void requestScreenshotUHDMode() { m_strSceenshotResquest = strScreenshotUHD(); }
    void requestScreenshotNormalMode() { m_strSceenshotResquest = strScreenshotNormal(); }

    void resetScreenShotRequest() { m_strSceenshotResquest = NULL; }
    const char* getStrSceenshotResquest() const { return m_strSceenshotResquest; }


    static const char* strScreenshotNormal() {return "ScreenshotNormal";}
    static const char* strScreenshotUHD() {return "ScreenshotUHD";}

#ifdef ITF_SUPPORT_EDITOR
    static const char* strScreenshotProfileFPS() {return "ScreenshotProfileFPS";}
#endif //ITF_SUPPORT_EDITOR

// take a screenshot [end]


private:
    
    static const ColorInteger  m_default4WhiteColorTab[4];
    
    ITF_IndexBuffer*    m_internalSpritesIndexBuffer;
    u32                 m_spriteIndexBufferSize;

    bbool               m_useDepthBuffer;
    bbool               m_SceneAlreadyResolve;

    bbool               m_RenderSceneInTarget;
    bbool               m_RContext_ZWRITE;
    bbool               m_RContext_ZTEST;

    f32                    drawTextInternal(const FontCall &_fontcall);
    f32                    drawTextInternalAppend(const FontCall &_fontcall,const Vec3d& _positionWitdh);

    RenderTargetManager*            m_RenderTargetManager;

    AABB                            m_screenAABB;
    f32                             m_fPs;
    bbool                           m_paused;
#ifndef ITF_FINAL
    bbool                           m_showRasters;
    bbool                           m_showAfterFx;
    bbool                           m_showDebugInfo;
#endif // ITF_FINAL

    #if ITF_DEBUG_LEVEL > 0
        SafeArray<DBGSphere, 16>            m_DBGSpheres;
        SafeArray<DBGCircle, 16>            m_DBGCircles;
        SafeArray<DBGBox, 16>               m_DBGBoxes;
        SafeArray<DBGLine, 256>             m_DBGLines;
        SafeArray<DBG3DLine, 16>            m_DBG3DLines;
        SafeArray<DBGLine, 8>               m_DBGLinesDuration;
        SafeArray<DBGTriangle, 8>           m_DBGTriangles;
        SafeArray<DBGTextureTriangle, 8>    m_DBGTextureTriangles;
        SafeArray<VertexPC, 8>              m_DBGTriangleList;
    #endif

    ITF_VECTOR<ResourceID>                  m_Fonts;

#ifndef ITF_FINAL
    ResourceID                              m_CommonGroup;
#endif //ITF_FINAL
    
    ///  vertexs buffer.
    Texture                                 *m_fontTexture;

    PrimitivesContainer2d                   m_container2d[m_PriorityCount2d];
    PrimitivesContainer2d                   m_container2dSubScreen[m_PriorityCount2d];

    #if ITF_DEBUG_LEVEL > 0
protected:
        SafeArray<DBGTextureQuad, 8>    m_DBGTextureQuads;
        ITF_VECTOR<DBGText>             m_DBGTexts;     
#endif // ITF_DEBUG_LEVEL > 0

#if !defined(ITF_WII) || (ITF_DEBUG_LEVEL > 0)
    SafeArray<Box2D, 10>            m_Box2DList;   
    SafeArray<Triangle2D, 10>       m_Triangle2DList;   
#endif // !defined(ITF_WII) || (ITF_DEBUG_LEVEL > 0)

    bbool                           m_isAlphaRefEnabled;

protected:
#ifndef ITF_CONSOLE_FINAL
    bbool           safeFrameDrawRequested()    { return m_drawSafeFrameRequested; }
    bbool           m_drawSafeFrameRequested;

    void            drawSafeFrame();
#endif // ITF_CONSOLE_FINAL

    /// Manager Vertex/Index
    GFXAdapter_VertexBufferManager  m_VertexBufferManager;

    /// static vb.
    ITF_IndexBuffer**   m_internalIdxBufferTab;
    ITF_VertexBuffer**  m_internalVtxBufferTab;

    class VideoCapture*       m_videoCapture;

    //GroupMeshManager*               m_groupMeshManager;

    u32                             m_drawFlag;
    f32                             m_currentDrawZ;
	GFX_MATERIAL_TYPE				m_currentRenderingMaterialType;
    i32                             m_currentVertexFormat;

    u32                             m_HideFlags;
    u32                             m_screenWidth;
    u32                             m_screenHeight;

#ifdef ITF_WINDOWS
    bbool                           m_bPickingMode;
    Vec2d                           m_PickingPos;
    f32                             m_deltaPicking;
#endif //ITF_WINDOWS
    protected:

    Color               m_globalColor;
    Color               m_colorFog;
    bbool               m_waitVBL = btrue;
    bbool               m_useFog;
    u32                 m_overDrawMode;
	u32                 m_ShaderMode;//temp
    u32                 m_showWireFrame;
    bbool               m_useSeparateAlpha;
    i32                 m_safeFrameMode;
    bbool               m_useRotateZCamera;
    f32                 m_monitorRefreshRate;
    bbool               m_displaydebugstats;
};

#define DRAWLINE( _p1, _p2, _r, _g, _b , _size)                             GFX_ADAPTER->drawLine(_p1, _p2, _r, _g, _b, _size)
#define DRAWSPHERE(_x, _y, _z, r1, lats, longs, _wireFrame, _r, _g, _b )    GFX_ADAPTER->drawSphere(_x, _y, _z, r1, lats, longs, _wireFrame, _r, _g, _b )
#define DRAWCIRCLE(_x, _y, r1, _r, _g, _b )                                 GFX_ADAPTER->drawCircle(_x, _y, r1, _r, _g, _b )

#define GFX_ADAPTER         GFXAdapter::getptr()

} // namespace ITF



#endif // _ITF_GFX_ADAPTER_H_
