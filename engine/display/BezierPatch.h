#ifndef _ITF_BEZIERPATCH_H_
#define _ITF_BEZIERPATCH_H_

#ifndef _ITF_CORETYPES_H_
#include "core/types.h"
#endif //_ITF_CORETYPES_H_

namespace ITF
{

    enum BEZIER_PATCH_UVMODE 
    {
        UV_MODE_OPTIMUM = 0,
        UV_MODE_SPEED,
        UV_MODE_COUNT,
        ENUM_FORCE_SIZE_32(BEZIER_PATCH_UVMODE)
    };

    #define SERIALIZE_UVMODE(name,val) \
    SERIALIZE_ENUM_BEGIN(name,val); \
        SERIALIZE_ENUM_VAR(UV_MODE_OPTIMUM); \
        SERIALIZE_ENUM_VAR(UV_MODE_SPEED); \
    SERIALIZE_ENUM_END();

    class Texture;

    struct BezierPatchParams
    {
        BezierPatchParams()
            : m_startUV(0.0f, 0.0f)
            , m_endUV(1.0f, 1.0f)
            , m_startWidth(1.f)
            , m_endWidth(1.f)
            , m_startColor(Color::white())
            , m_endColor(Color::white())
            , m_fogColor(0.f, Color::white().m_r, Color::white().m_g, Color::white().m_b)
            , m_alphaLimitA(0.f)
            , m_alphaValueA(1.f)
            , m_alphaLimitB(1.f)
            , m_alphaValueB(1.f)
            , m_widthLimitA(0.f)
            , m_widthValueA(1.f)
            , m_widthLimitB(1.f)
            , m_widthValueB(1.f)
            , m_blendMode(GFX_BLEND_ALPHA)
            , m_uvMode(UV_MODE_OPTIMUM)
            , m_texture(NULL)
            , m_z(0.f)
            , m_hdiv(MESH_DEFAULT_HDIV)
            , m_vdiv(MESH_DEFAULT_VDIV)
            , m_vdivToDraw(U32_INVALID)
        {
        }

        Vec2d m_p0;
        Vec2d m_p1;
        Vec2d m_p2;
        Vec2d m_p3;
        Vec2d m_startUV;
        Vec2d m_endUV;
        f32 m_startWidth;
        f32 m_endWidth;
        f32 m_widthLimitA;
        f32 m_widthValueA;
        f32 m_widthLimitB;
        f32 m_widthValueB;

        Color m_startColor;
        Color m_endColor;
        Color m_fogColor;
        f32 m_alphaLimitA;
        f32 m_alphaValueA;
        f32 m_alphaLimitB;
        f32 m_alphaValueB;

        GFX_BLENDMODE m_blendMode;
        BEZIER_PATCH_UVMODE m_uvMode;
        Texture* m_texture;
        f32 m_z;

        u32 m_hdiv;
        u32 m_vdiv;
        u32 m_vdivToDraw;
    };


    class BezierPatch
    {
    public:

        BezierPatch();
        BezierPatch(const BezierPatch&);
        virtual ~BezierPatch();

        BezierPatch& operator =(const BezierPatch& _src);

        void        drawPatch( const BezierPatchParams& _params );

        void        Draw( const Vec2d* _pointsAndUVs, const f32* _widths,
                          const f32* _widths2,
                          const f32* _alphas, const ColorInteger* _colors,
                          const BezierPatchParams& _params );
        void        DrawShader();

#ifdef ANIMUSESHADER
        const Color&    getFogColor() const { return m_fogColor; }
#else
        ITF_Mesh*       getMesh()           { return &m_mesh; }
        const Color&    getFogColor() const { return m_mesh.m_colorFog; }
#endif

    protected:
#ifdef ANIMUSESHADER
        Texture*        m_texture;
        Vec2d           m_pointsAndUVs[8];
        f32             m_z;
        Color           m_fogColor;
        ColorInteger    m_colors[2];
        f32             m_widths[4];
        f32             m_alphas[4]; // 2 Alpha and 2 Percentage
        f32             m_widths2[4]; // 2 Size and 2 Percentage

        GFX_BLENDMODE  m_blendMode;

        u32         m_hdiv;
        u32         m_vdiv;
        u32         m_vdivToDraw;
#else
        ITF_Mesh        m_mesh;
        Patch32Data*    mp_Data;
#endif
    };
}

#endif // _ITF_BEZIERPATCH_H_
