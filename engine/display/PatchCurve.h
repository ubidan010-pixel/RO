#ifndef _ITF_PATCHCURVE_H_
#define _ITF_PATCHCURVE_H_

#ifndef _ITF_PROCEDURALPATCH_H_
#include "engine/display/ProceduralPatch.h"
#endif //_ITF_PROCEDURALPATCH_H_

namespace ITF
{
    class PatchCurve : public ProceduralPatch
    {
    public:
        PatchCurve();
        virtual ~PatchCurve();

        struct Params 
        {
            Vec2d   m_p1;
            Vec2d   m_n1;
            Vec2d   m_p2;
            Vec2d   m_n2;
            f32     m_width;
            Vec2d   m_startUV;
            Vec2d   m_endUV;
        };

        void        Draw( const Params& _params, Texture* _texture, const f32 _z, const ColorInteger* _colors = NULL );

    private:
        void        computePoints( Vec2d* _pointsAndUVs, const Params& _params );
    };

}

#endif // _ITF_PATCHCURVE_H_
