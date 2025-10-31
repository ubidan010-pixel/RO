#ifndef _ITF_MATRIX44_HEADER_SOFTWARE_H_
#define _ITF_MATRIX44_HEADER_SOFTWARE_H_

//----------------------------------------------------------------------------//
// defines.
//----------------------------------------------------------------------------//
#define RESTRICT            __restrict

namespace ITF
{
    //----------------------------------------------------------------------------//
    // Float4 definition.
    //----------------------------------------------------------------------------//
    struct Float4
    {
        /*Float4(float _x, float _y, float _z, float _w)
        {
            m_x = _x;
            m_y = _y;
            m_z = _z;
            m_w = _w;
        };*/

        float m_x, m_y, m_z, m_w;
    };

    //----------------------------------------------------------------------------//
    // Float4 functions.
    //----------------------------------------------------------------------------//
    //
    //
    ITF_FORCE_INLINE void setFloat4(Float4 &_f4, f32 _x, f32 _y, f32 _z, f32 _w) {_f4.m_x = _x; _f4.m_y = _y; _f4.m_z = _z; _f4.m_w = _w;}

    ITF_FORCE_INLINE Float4 makeFloat4(f32 _x, f32 _y, f32 _z, f32 _w)  {Float4 v; v.m_x = _x; v.m_y = _y; v.m_z = _z; v.m_w = _w; return v;}

    ITF_FORCE_INLINE Float4 min4(const Float4 &_v1, const Float4 &_v2)
    {
        Float4 r;
        r.m_x = std::min(_v1.m_x, _v2.m_x);
        r.m_y = std::min(_v1.m_y, _v2.m_y);
        r.m_z = std::min(_v1.m_z, _v2.m_z);
        r.m_w = std::min(_v1.m_w, _v2.m_w);
        return r;
    }

    ITF_FORCE_INLINE Float4 max4(const Float4 &_v1, const Float4 &_v2)
    {
        Float4 r;
        r.m_x = std::max(_v1.m_x, _v2.m_x);
        r.m_y = std::max(_v1.m_y, _v2.m_y);
        r.m_z = std::max(_v1.m_z, _v2.m_z);
        r.m_w = std::max(_v1.m_w, _v2.m_w);
        return r;
    }
    ITF_FORCE_INLINE void getFloat4(Float4 &_f4In, f32* _out)
    {
        _out[0] = _f4In.m_x;
        _out[1] = _f4In.m_y;
        _out[2] = _f4In.m_z;
        _out[3] = _f4In.m_w;
	}

} // namespace ITF


#endif // _ITF_MATRIX44_HEADER_SOFTWARE_H_
