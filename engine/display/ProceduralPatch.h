#ifndef _ITF_PROCEDURALPATCH_H_
#define _ITF_PROCEDURALPATCH_H_

#ifndef _ITF_CORETYPES_H_
#include "core/types.h"
#endif //_ITF_CORETYPES_H_

namespace ITF
{
    class Texture;

    class ProceduralPatch
    {
    public:

        ProceduralPatch();
        virtual ~ProceduralPatch();
 
        void        Draw( const Vec2d* _pointsAndUVs, Texture* _texture, f32 _z, const ColorInteger* _colors = NULL );
        void        DrawShader();

#ifdef ANIMUSESHADER
#else
        ITF_Mesh*   getMesh() { return &m_mesh; }
#endif

    protected:
#ifdef ANIMUSESHADER
        Texture*        m_texture;
        Vec2d           m_pointsAndUVs[16];
        f32             m_z;
        ColorInteger    m_colors[4];
#else
        ITF_Mesh    m_mesh;
#endif
        u32         m_hdiv;
        u32         m_vdiv;
    };
}

#endif // _ITF_PATCHCURVE_H_
