#ifndef _ITF_AFTERFX_H_
#define _ITF_AFTERFX_H_

#ifndef _ITF_RESOURCEGROUP_H_
#include "engine/resources/ResourceGroup.h"
#endif //_ITF_RESOURCEGROUP_H_

#ifndef _ITF_COLOR_H_
#include "core/Color.h"
#endif //_ITF_COLOR_H_

namespace ITF
{

enum AFTERFX_Type
{
    AFX_None = 0,
    AFX_Blur,
    AFX_Glow,
    AFX_Remanence,
    AFX_DOF,
    AFX_RayCenter,
    AFX_ColorSetting,
    AFX_ColorRemap,
    AFX_ColorLevels,
    AFX_Fade,
    AFX_Bright,
    AFX_ToneMap,
    AFX_AddSceneAndMul,
    AFX_objectsGlow,
    AFX_simpleBlend,
    AFX_zoomCenter,
    AFX_BorderBright,
    ENUM_FORCE_SIZE_32(AFTERFX_Type)
};

class Texture;

class AFTERFX
{

public:
    
    DECLARE_SERIALIZE()
    AFTERFX()
    {
        m_type = AFX_None;
        m_lifeTime = 0.f;
        m_apply = 1;
        for (u32 i=0;i<8;i++)
            m_Paramf.push_back(1.f);
        for (u32 i=0;i<8;i++)
            m_Parami.push_back(0);
        for (u32 i=0;i<8;i++)
            m_Paramv.push_back(Vec3d::Zero);
        for (u32 i=0;i<2;i++)
            m_Paramc.push_back(Color::zero());

        m_customval = 0;
        m_colorTarget = Color::zero();

        for (u32 i=0;i<4;i++)
            m_customTextureID[i].invalidateResourceId();

        m_renderintarget = 0;

        m_zStart = 0.f;
        m_zRender = 0.f;

		m_isPost2D = bfalse;
    };

    AFTERFX(const AFTERFX& _source) {copy(_source);}
 
    ~AFTERFX()
    {
        destroy();
    }

    ITF_INLINE void copy(const AFTERFX& _from)
    {
        m_type = _from.m_type;
        m_lifeTime = _from.m_lifeTime;
        m_apply = _from.m_apply;
        m_customval = _from.m_customval;

        m_Paramf = _from.m_Paramf;
        m_Parami = _from.m_Parami;
        m_Paramv = _from.m_Paramv;

        Path path;
        /*for (u32 i = 0 ; i < 4 ; i++)
        {
            Resource * res = _from.m_customTextureID[i].getResource();
            if (res)
            {
                path = res->getPath();
                LoadTexture(path, i);
            }
        }
        loadResources();*/
    }

    static AFTERFX_Type getTypeByName(String& _name);
    
    ITF_INLINE void setType( AFTERFX_Type _type ) { m_type = _type;};
    ITF_INLINE void setActiveFx( u32 _active ) { m_apply = _active;};
    ITF_INLINE void setLifeTime( f32 _lifeTime   ) { m_lifeTime = _lifeTime;};

    ITF_INLINE AFTERFX_Type getType( ) { return m_type;};
    
    ITF_INLINE void setParamfAt( u32 _n, f32 _fval   ) { m_Paramf[_n] = _fval;};
    ITF_INLINE f32 getParamfAt( u32 _n) { return m_Paramf[_n];};

    ITF_INLINE void setParamiAt( u32 _n, i32 _ival   ) { m_Parami[_n] = _ival;};
    ITF_INLINE i32 getParamiAt( u32 _n) { return m_Parami[_n];};

    ITF_INLINE void setParamvAt( u32 _n, Vec3d _vec   ) { m_Paramv[_n] = _vec;};
    ITF_INLINE Vec3d getParamvAt( u32 _n) { return m_Paramv[_n];};

    ITF_INLINE void setParamcAt( u32 _n, Color _col   ) { m_Paramc[_n] = _col;};
    ITF_INLINE Color getParamcAt( u32 _n) { return m_Paramc[_n];};

    ITF_INLINE void setCustomVal( i32 _val ) { m_customval = _val;};
    ITF_INLINE i32 getCustomVal() { return m_customval;}

    ITF_INLINE void setColorTarget( Color _color ) { m_colorTarget = _color;};
    ITF_INLINE Color getColorTarget() { return m_colorTarget;}

    ITF_INLINE void setZStart( f32 _z ) { m_zStart = _z;};
    ITF_INLINE f32 getZStart() { return m_zStart;}

    ITF_INLINE void setZRender( f32 _z ) { m_zRender = _z;};
    ITF_INLINE f32 getZRender() { return m_zRender;}

	ITF_INLINE void setPost2D( bbool _v ) { m_isPost2D = _v;};
	ITF_INLINE bbool isPost2D() { return m_isPost2D;}

    ResourceGroup* getResourceGroup();
    //ResourceID addResource( Resource::ResourceType _type, const Path& _path );
    void removeResource(const ResourceID &_resource);
    void loadResources();

    void destroy();

    Texture* getTexture(u32 _n);
    void setTexture(u32 _n, ResourceID _texture);

    void LoadTexture(const Path& path, u32 _n);
    
    ITF_INLINE u32              isRenderInTarget(   )               {   return m_renderintarget;};
    ITF_INLINE void             SetRenderInTarget( u32 _v )       {   m_renderintarget = _v;  };

private:
    ResourceID          m_resourceGroup;

    ResourceID          m_customTextureID[4];

    AFTERFX_Type        m_type;
    SafeArray<f32,8>    m_Paramf;
    SafeArray<i32,8>    m_Parami;
    SafeArray<Vec3d,8>  m_Paramv;
    SafeArray<Color,2>  m_Paramc;
    f32                 m_lifeTime;
    u32                 m_apply;
    i32                 m_customval;
    Color               m_colorTarget;
    u32                 m_renderintarget;

    f32                 m_zStart;
    f32                 m_zRender;

	bbool				m_isPost2D;
};

class AFX_Group
{
public:
    AFX_Group()
    {
        m_renderintarget = 0;
        m_startRank = -1000.f;
        m_renderRank = 1000.f;
        m_finalblend = 0;
        m_depth = 0.f;
    };

    AFX_Group(const AFX_Group& _source) {copy(_source);}
 
    void copy(const AFX_Group& _from);

    ~AFX_Group();

    void addAFXtoGroup( AFTERFX* _afx );
    void destroy();

    //ITF_INLINE AFTERFX*         getList()      {   return m_AFXList;};
    ITF_INLINE AFTERFX*         getFxAt(u32 _n)      {   return m_AFXList[_n];};
    ITF_INLINE u32              getNumberFx()  {   return m_AFXList.size();};

    ITF_INLINE void             setDepth(   f32 _depth   )          {   m_depth = _depth;    };
    ITF_INLINE f32              getDepth(   )                       {   return m_depth;    };
    ITF_INLINE void             setBeginRank(   f32 _rank   )       {   m_startRank = _rank;    };
    ITF_INLINE void             setRenderRank(  f32 _rank   )       {   m_renderRank = _rank;   };
    ITF_INLINE f32              getStartRank(   )                   {   return m_startRank;     };
    ITF_INLINE f32              getRenderRank(  )                   {   return m_renderRank;    };

    ITF_INLINE void             SetRenderInTarget( u32 _v )       {   m_renderintarget = _v;  };
    ITF_INLINE u32              isRenderInTarget(   )               {   return m_renderintarget;};
    ITF_INLINE void             SetFinalBlend( i32 _v )             {   m_finalblend = _v;      };
    ITF_INLINE i32              getFinalBlend(   )                  {   return m_finalblend;    };


private:
    f32                     m_depth;
    f32                     m_startRank;
    f32                     m_renderRank;
    u32                     m_renderintarget;
    i32                     m_finalblend;
    ITF_VECTOR<AFTERFX*>    m_AFXList;
};

class AFTERFXManager
{
public:
    AFTERFXManager()
    {
        m_render = 1;
    };

    ~AFTERFXManager();

    void update();
    void addFxGroup( AFX_Group* _afxg );
    void reinit();
    void render();
    void addFxPrimitive();
    void deleteAll();

    ITF_INLINE u32              getNumberGroup() {return m_AFXGroupList.size();};
 
private:
    u32                         m_render;
    ITF_VECTOR<AFX_Group*>      m_AFXGroupList;
};


} // namespace ITF

#endif // _ITF_AFTERFX_H_
