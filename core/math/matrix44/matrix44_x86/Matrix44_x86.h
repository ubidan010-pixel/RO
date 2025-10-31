#ifndef _ITF_MATRIX44_X86_H_
#define _ITF_MATRIX44_X86_H_

//----------------------------------------------------------------------------//
// matrix44 class functions.
//----------------------------------------------------------------------------//
ITF_INLINE void Matrix44::reset()
{
    m_l[0] = _mm_setzero_ps();
    m_l[1] = _mm_setzero_ps();
    m_l[2] = _mm_setzero_ps();
    m_l[3] = _mm_setzero_ps();
};

ITF_INLINE void Matrix44::setFromFloat(
    const f32 _v11, const f32 _v12, const f32 _v13, const f32 _v14,
    const f32 _v21, const f32 _v22, const f32 _v23, const f32 _v24,
    const f32 _v31, const f32 _v32, const f32 _v33, const f32 _v34,
    const f32 _v41, const f32 _v42, const f32 _v43, const f32 _v44
            )
{
    f32 ma[16];
    ma[0] = _v11;
    ma[1] = _v12;
    ma[2] = _v13;
    ma[3] = _v14;
    ma[4] = _v21;
    ma[5] = _v22;
    ma[6] = _v23;
    ma[7] = _v24;
    ma[8] = _v31;
    ma[9] = _v32;
    ma[10] = _v33;
    ma[11] = _v34;
    ma[12] = _v41;
    ma[13] = _v42;
    ma[14] = _v43;
    ma[15] = _v44;

    M44_load(ma, this);
};

ITF_INLINE void Matrix44::setTranslation(const Vec3d& _trans) 
{
    m_l[3] = _mm_set_ps(1.f, _trans.m_z, _trans.m_y, _trans.m_x);
};

ITF_INLINE void Matrix44::mulScale(const Vec3d& _scale) 
{
    Float4 scale;
    
    scale = _mm_set_ps(0.f, _scale.m_z, _scale.m_y, _scale.m_x);
    Float4 scaleX = _mm_mul_ps( m_l[0], splatX(scale) );
    Float4 scaleY = _mm_mul_ps( m_l[1], splatY(scale) );
    Float4 scaleZ = _mm_mul_ps( m_l[2], splatZ(scale) );

    m_l[0] = scaleX;
    m_l[1] = scaleY;
    m_l[2] = scaleZ;
}

ITF_INLINE Vec3d Matrix44::getXAxis() const
{
    float s[4];
    _mm_storeu_ps(s, m_l[0]);
    return Vec3d(s[0], s[1], s[2]);
}

ITF_INLINE Vec3d Matrix44::getYAxis() const
{
    float s[4];
    _mm_storeu_ps(s, m_l[1]);
    return Vec3d(s[0], s[1], s[2]);
}

ITF_INLINE Vec3d Matrix44::getZAxis() const
{
    float s[4];
    _mm_storeu_ps(s, m_l[2]);
    return Vec3d(s[0], s[1], s[2]);
}

ITF_INLINE Vec3d Matrix44::getTranslation() const 
{
    float s[4];
    _mm_storeu_ps(s, m_l[3]);
    return Vec3d(s[0], s[1], s[2]);
}

ITF_INLINE void Matrix44::setLineAt(u32 _line, f32 _x, f32 _y, f32 _z, f32 _w)
{
    m_l[_line] = _mm_set_ps(_w, _z, _y, _x);
}

//----------------------------------------------------------------------------//
// externals functions.
//----------------------------------------------------------------------------//
ITF_INLINE void M44_create(Matrix44** _m)
{
    *_m = (Matrix44*)_aligned_malloc(sizeof(Matrix44), 16);
}

ITF_INLINE void M44_destroy(Matrix44** _m)
{
    if (*_m)
    {
        _aligned_free(*_m);
        *_m = NULL;
    }
}

ITF_INLINE void M44_store(float* RESTRICT _fa16, const Matrix44* _m)
{
    _mm_storeu_ps(_fa16, _m->m_l[0]);
    _mm_storeu_ps(_fa16+4, _m->m_l[1]);
    _mm_storeu_ps(_fa16+8, _m->m_l[2]);
    _mm_storeu_ps(_fa16+12, _m->m_l[3]);
}

ITF_INLINE void M44_load(const float* RESTRICT _fa16, Matrix44* _m)
{
    _m->m_l[0] = _mm_loadu_ps( _fa16 );
    _m->m_l[1] = _mm_loadu_ps( _fa16 + 4 );
    _m->m_l[2] = _mm_loadu_ps( _fa16 + 8 );
    _m->m_l[3] = _mm_loadu_ps( _fa16 + 12 );
 }

ITF_INLINE void M44_setMatrixIdentity(Matrix44* _mOut)
{
    _mOut->m_l[0] = _mm_set_ps(0.f, 0.f, 0.f, 1.f);
    _mOut->m_l[1] = _mm_set_ps(0.f, 0.f, 1.f, 0.f);
    _mOut->m_l[2] = _mm_set_ps(0.f, 1.f, 0.f, 0.f);
    _mOut->m_l[3] = _mm_set_ps(1.f, 0.f, 0.f, 0.f);
}

ITF_INLINE void M44_setMatrixRotationX(Matrix44* _mOut, f32 _angle)
{
    f32 s = sinf(_angle);
    f32 c = cosf(_angle);

    _mOut->m_l[0] = _mm_set_ps(0.f, 0.f, 0.f, 1.f);
    _mOut->m_l[1] = _mm_set_ps(0.f, s, c, 0.f);
    _mOut->m_l[2] = _mm_set_ps(0.f, c, -s, 0.f);
    _mOut->m_l[3] = _mm_set_ps(1.f, 0.f, 0.f, 0.f);
}

ITF_INLINE void M44_setMatrixRotationY(Matrix44* _mOut, f32 _angle)
{
    f32 s = sinf(_angle);
    f32 c = cosf(_angle);

    _mOut->m_l[0] = _mm_set_ps(0.f, -s, 0.f, c);
    _mOut->m_l[1] = _mm_set_ps(0.f, 0.f, 1.f, 0.f);
    _mOut->m_l[2] = _mm_set_ps(0.f, c, 0.f, s);
    _mOut->m_l[3] = _mm_set_ps(1.f, 0.f, 0.f, 0.f);
}

ITF_INLINE void M44_setMatrixRotationZ(Matrix44* __restrict _mOut, f32 _angle)
{
    f32 s = sinf(_angle);
    f32 c = cosf(_angle);

    _mOut->m_l[0] = _mm_set_ps(0.f, 0.f, s, c);
    _mOut->m_l[1] = _mm_set_ps(0.f, 0.f, c, -s);
    _mOut->m_l[2] = _mm_set_ps(0.f, 1.f, 0.f, 0.f);
    _mOut->m_l[3] = _mm_set_ps(1.f, 0.f, 0.f, 0.f);
}

ITF_INLINE void M44_setMatrixRotationYawPithRoll(Matrix44* _mOut, f32 _yaw, f32 _pitch, f32 _roll)
{
    f32 sa = sinf(_roll);
    f32 sb = sinf(_pitch);
    f32 sc = sinf(_yaw);
    f32 ca = cosf(_roll);
    f32 cb = cosf(_pitch);
    f32 cc = cosf(_yaw);

    _mOut->m_l[0] = _mm_set_ps(0.f, cb*sc, -sa*cc+ca*sb*sc, ca*cc+sa*sb*sc);
    _mOut->m_l[1] = _mm_set_ps(0.f, -sb, ca*cb, sa*cb);
    _mOut->m_l[2] = _mm_set_ps(0.f, cb*cc, sa*sc+ca*sb*cc, -ca*sc+sa*sb*cc);
    _mOut->m_l[3] = _mm_set_ps(1.f, 0.f, 0.f, 0.f);
}

ITF_INLINE void M44_setMatrixScaling(Matrix44* _mOut, f32 _x, f32 _y, f32 _z)
{
    _mOut->m_l[0] = _mm_set_ps(0.f, 0.f, 0.f, _x);
    _mOut->m_l[1] = _mm_set_ps(0.f, 0.f, _y, 0.f);
    _mOut->m_l[2] = _mm_set_ps(0.f, _z, 0.f, 0.f);
    _mOut->m_l[3] = _mm_set_ps(1.f, 0.f, 0.f, 0.f);
}

ITF_INLINE void M44_setMatrixTranslation(Matrix44* _mOut, f32 _x, f32 _y, f32 _z)
{
    _mOut->m_l[0] = _mm_set_ps(0.f, 0.f, 0.f, 1.f);
    _mOut->m_l[1] = _mm_set_ps(0.f, 0.f, 1.f, 0.f);
    _mOut->m_l[2] = _mm_set_ps(0.f, 1.f, 0.f, 0.f);
    _mOut->m_l[3] = _mm_set_ps(1.f, _z, _y, _x);
}

ITF_INLINE void M44_mul4x4Out(Matrix44* _Out, const Matrix44* _mInA, const Matrix44* _mInB)
{
    Float4 mbl0 = _mInB->m_l[0];
    Float4 mbl1 = _mInB->m_l[1];
    Float4 mbl2 = _mInB->m_l[2];
    Float4 mbl3 = _mInB->m_l[3];

    Float4 tmp1, tmp2;

    tmp1 = _mm_mul_ps( splatX(_mInA->m_l[0]), mbl0 );
    tmp2 = _mm_mul_ps( splatY(_mInA->m_l[0]), mbl1 );
    tmp1 = _mm_add_ps( tmp1, tmp2 );
    tmp2 = _mm_mul_ps( splatZ(_mInA->m_l[0]), mbl2 );
    tmp1 = _mm_add_ps( tmp1, tmp2 );
    tmp2 = _mm_mul_ps( splatW(_mInA->m_l[0]), mbl3 );
    _Out->m_l[0] = _mm_add_ps( tmp1, tmp2 );

    tmp1 = _mm_mul_ps( splatX(_mInA->m_l[1]), mbl0 );
    tmp2 = _mm_mul_ps( splatY(_mInA->m_l[1]), mbl1 );
    tmp1 = _mm_add_ps( tmp1, tmp2 );
    tmp2 = _mm_mul_ps( splatZ(_mInA->m_l[1]), mbl2 );
    tmp1 = _mm_add_ps( tmp1, tmp2 );
    tmp2 = _mm_mul_ps( splatW(_mInA->m_l[1]), mbl3 );
    _Out->m_l[1] = _mm_add_ps( tmp1, tmp2 );

    tmp1 = _mm_mul_ps( splatX(_mInA->m_l[2]), mbl0 );
    tmp2 = _mm_mul_ps( splatY(_mInA->m_l[2]), mbl1 );
    tmp1 = _mm_add_ps( tmp1, tmp2 );
    tmp2 = _mm_mul_ps( splatZ(_mInA->m_l[2]), mbl2 );
    tmp1 = _mm_add_ps( tmp1, tmp2 );
    tmp2 = _mm_mul_ps( splatW(_mInA->m_l[2]), mbl3 );
    _Out->m_l[2] = _mm_add_ps( tmp1, tmp2 );

    tmp1 = _mm_mul_ps( splatX(_mInA->m_l[3]), mbl0 );
    tmp2 = _mm_mul_ps( splatY(_mInA->m_l[3]), mbl1 );
    tmp1 = _mm_add_ps( tmp1, tmp2 );
    tmp2 = _mm_mul_ps( splatZ(_mInA->m_l[3]), mbl2 );
    tmp1 = _mm_add_ps( tmp1, tmp2 );
    tmp2 = _mm_mul_ps( splatW(_mInA->m_l[3]), mbl3 );
   _Out->m_l[3] = _mm_add_ps( tmp1, tmp2 );
}

ITF_INLINE void M44_mul3x3Out(Matrix44* _Out, const Matrix44* _mInA, const Matrix44* _mInB)
{
    Float4 mbl0 = _mInB->m_l[0];
    Float4 mBl1 = _mInB->m_l[1];
    Float4 mBl2 = _mInB->m_l[2];

    Float4 tmp1, tmp2;

    tmp1 = _mm_mul_ps( splatX(_mInA->m_l[0]), mbl0 );
    tmp2 = _mm_mul_ps( splatY(_mInA->m_l[0]), mBl1 );
    tmp1 = _mm_add_ps( tmp1, tmp2 );
    tmp2 = _mm_mul_ps( splatZ(_mInA->m_l[0]), mBl2 );
    _Out->m_l[0] = _mm_add_ps( tmp1, tmp2 );

    tmp1 = _mm_mul_ps( splatX(_mInA->m_l[1]), mbl0 );
    tmp2 = _mm_mul_ps( splatY(_mInA->m_l[1]), mBl1 );
    tmp1 = _mm_add_ps( tmp1, tmp2 );
    tmp2 = _mm_mul_ps( splatZ(_mInA->m_l[1]), mBl2 );
    _Out->m_l[1] = _mm_add_ps( tmp1, tmp2 );

    tmp1 = _mm_mul_ps( splatX(_mInA->m_l[2]), mbl0 );
    tmp2 = _mm_mul_ps( splatY(_mInA->m_l[2]), mBl1 );
    tmp1 = _mm_add_ps( tmp1, tmp2 );
    tmp2 = _mm_mul_ps( splatZ(_mInA->m_l[2]), mBl2 );
    _Out->m_l[2] = _mm_add_ps( tmp1, tmp2 );
}

ITF_INLINE void M44_matrixTranspose(Matrix44* _mOut, const Matrix44* _mIn)
{
    Float4 temp0 = _mm_unpacklo_ps( _mIn->m_l[0], _mIn->m_l[2] );
    Float4 temp1 = _mm_unpacklo_ps( _mIn->m_l[1], _mIn->m_l[3] );
    Float4 temp2 = _mm_unpackhi_ps( _mIn->m_l[0], _mIn->m_l[2] );
    Float4 temp3 = _mm_unpackhi_ps( _mIn->m_l[1], _mIn->m_l[3] );

    _mOut->m_l[0] = _mm_unpacklo_ps( temp0, temp1 );
    _mOut->m_l[1] = _mm_unpackhi_ps( temp0, temp1 );
    _mOut->m_l[2] = _mm_unpacklo_ps( temp2, temp3 );
    _mOut->m_l[3] = _mm_unpackhi_ps( temp2, temp3 );
}


ITF_INLINE void M44_matrixInverse(f32* dst, const f32* scr)
{
    __m128 minor0, minor1, minor2, minor3;
    __m128 row0, row1, row2, row3;
    __m128 det, tmp1;

    tmp1 = _mm_set_ps(0.f, 0.f, 0.f, 0.f);
    row1 = _mm_set_ps(0.f, 0.f, 0.f, 0.f);
    row3 = _mm_set_ps(0.f, 0.f, 0.f, 0.f);

    tmp1 = _mm_loadh_pi(_mm_loadl_pi(tmp1, (const __m64*)(scr)), (const __m64*)(scr+ 4));
    row1 = _mm_loadh_pi(_mm_loadl_pi(row1, (const __m64*)(scr+8)), (const __m64*)(scr+12));
    row0 = _mm_shuffle_ps(tmp1, row1, 0x88);
    row1 = _mm_shuffle_ps(row1, tmp1, 0xDD);
    tmp1 = _mm_loadh_pi(_mm_loadl_pi(tmp1, (const __m64*)(scr+ 2)), (const __m64*)(scr+ 6));
    row3 = _mm_loadh_pi(_mm_loadl_pi(row3, (const __m64*)(scr+10)), (const __m64*)(scr+14));
    row2 = _mm_shuffle_ps(tmp1, row3, 0x88);
    row3 = _mm_shuffle_ps(row3, tmp1, 0xDD);

    // -----------------------------------------------
    
    tmp1 = _mm_mul_ps(row2, row3);
    tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
    minor0 = _mm_mul_ps(row1, tmp1);
    minor1 = _mm_mul_ps(row0, tmp1);
    tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
    minor0 = _mm_sub_ps(_mm_mul_ps(row1, tmp1), minor0);
    minor1 = _mm_sub_ps(_mm_mul_ps(row0, tmp1), minor1);
    minor1 = _mm_shuffle_ps(minor1, minor1, 0x4E);

    // -----------------------------------------------
    
    tmp1 = _mm_mul_ps(row1, row2);
    tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
    minor0 = _mm_add_ps(_mm_mul_ps(row3, tmp1), minor0);
    minor3 = _mm_mul_ps(row0, tmp1);
    tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
    minor0 = _mm_sub_ps(minor0, _mm_mul_ps(row3, tmp1));
    minor3 = _mm_sub_ps(_mm_mul_ps(row0, tmp1), minor3);
    minor3 = _mm_shuffle_ps(minor3, minor3, 0x4E);

    // -----------------------------------------------
    
    tmp1 = _mm_mul_ps(_mm_shuffle_ps(row1, row1, 0x4E), row3);
    tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
    row2 = _mm_shuffle_ps(row2, row2, 0x4E);
    minor0 = _mm_add_ps(_mm_mul_ps(row2, tmp1), minor0);
    minor2 = _mm_mul_ps(row0, tmp1);
    tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
    minor0 = _mm_sub_ps(minor0, _mm_mul_ps(row2, tmp1));
    minor2 = _mm_sub_ps(_mm_mul_ps(row0, tmp1), minor2);
    minor2 = _mm_shuffle_ps(minor2, minor2, 0x4E);

    // -----------------------------------------------

    tmp1 = _mm_mul_ps(row0, row1);
    tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
    minor2 = _mm_add_ps(_mm_mul_ps(row3, tmp1), minor2);
    minor3 = _mm_sub_ps(_mm_mul_ps(row2, tmp1), minor3);
    tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
    minor2 = _mm_sub_ps(_mm_mul_ps(row3, tmp1), minor2);
    minor3 = _mm_sub_ps(minor3, _mm_mul_ps(row2, tmp1));

    // -----------------------------------------------

    tmp1 = _mm_mul_ps(row0, row3);
    tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
    minor1 = _mm_sub_ps(minor1, _mm_mul_ps(row2, tmp1));
    minor2 = _mm_add_ps(_mm_mul_ps(row1, tmp1), minor2);
    tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
    minor1 = _mm_add_ps(_mm_mul_ps(row2, tmp1), minor1);
    minor2 = _mm_sub_ps(minor2, _mm_mul_ps(row1, tmp1));

    // -----------------------------------------------

    tmp1 = _mm_mul_ps(row0, row2);
    tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
    minor1 = _mm_add_ps(_mm_mul_ps(row3, tmp1), minor1);
    minor3 = _mm_sub_ps(minor3, _mm_mul_ps(row1, tmp1));
    tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
    minor1 = _mm_sub_ps(minor1, _mm_mul_ps(row3, tmp1));
    minor3 = _mm_add_ps(_mm_mul_ps(row1, tmp1), minor3);

    // -----------------------------------------------

    det = _mm_mul_ps(row0, minor0);
    det = _mm_add_ps(_mm_shuffle_ps(det, det, 0x4E), det);
    det = _mm_add_ss(_mm_shuffle_ps(det, det, 0xB1), det);
    tmp1 = _mm_rcp_ss(det);
    det = _mm_sub_ss(_mm_add_ss(tmp1, tmp1), _mm_mul_ss(det, _mm_mul_ss(tmp1, tmp1)));
    det = _mm_shuffle_ps(det, det, 0x00);
    minor0 = _mm_mul_ps(det, minor0);

    _mm_storel_pi((__m64*)(dst), minor0);
    _mm_storeh_pi((__m64*)(dst+2), minor0);
    minor1 = _mm_mul_ps(det, minor1);
    _mm_storel_pi((__m64*)(dst+4), minor1);
    _mm_storeh_pi((__m64*)(dst+6), minor1);
    minor2 = _mm_mul_ps(det, minor2);
    _mm_storel_pi((__m64*)(dst+ 8), minor2);
    _mm_storeh_pi((__m64*)(dst+10), minor2);
    minor3 = _mm_mul_ps(det, minor3);
    _mm_storel_pi((__m64*)(dst+12), minor3);
    _mm_storeh_pi((__m64*)(dst+14), minor3);


}
ITF_INLINE void M44_matrixCoordTransform(Float4* _f4Out, const Matrix44* _m, const Float4* _f4In)
{
    Float4 result = _mm_add_ps( _mm_mul_ps(splatX(*_f4In), _m->m_l[0]), _m->m_l[3] );
    result = _mm_add_ps( _mm_mul_ps(splatY(*_f4In), _m->m_l[1]), result );
    *_f4Out = _mm_add_ps( _mm_mul_ps(splatZ(*_f4In), _m->m_l[2]), result );
}

ITF_INLINE void M44_matrixCoordTransform(Vec3d* _vOut, const Matrix44* _m, const Vec3d* _vIn)
{
    Float4 vIn;
    float vo[4];
    Vec3dToFloat4(&vIn, _vIn);

    Float4 result = _mm_add_ps( _mm_mul_ps(splatX(vIn), _m->m_l[0]), _m->m_l[3] );
    result = _mm_add_ps( _mm_mul_ps(splatY(vIn), _m->m_l[1]), result );
    _mm_storeu_ps(vo, _mm_add_ps( _mm_mul_ps(splatZ(vIn), _m->m_l[2]), result ));
    _vOut->m_x = vo[0];
    _vOut->m_y = vo[1];
    _vOut->m_z = vo[2];
}

ITF_INLINE void M44_matrixCoordTransform(Vec2d* _vOut, const Matrix44* _m, const Vec2d* _vIn)
{
    M44_matrixVec2dTransform(_vOut, _m, _vIn);
    _vOut->m_x += _m->_m41;
    _vOut->m_y += _m->_m42;
}

ITF_INLINE void M44_matrixFloat4Transform(Float4* _f4Out, const Matrix44* _m, const Float4* _f4In)
{
    Float4 result = _mm_mul_ps( splatX(*_f4In), _m->m_l[0] );
    result = _mm_add_ps( _mm_mul_ps(splatY(*_f4In), _m->m_l[1]), result );
    *_f4Out = _mm_add_ps( _mm_mul_ps(splatZ(*_f4In), _m->m_l[2]), result );
}

ITF_INLINE void M44_matrixVec3dTransform(Vec3d* _vOut, const Matrix44* _m, const Vec3d* _vIn)
{
    Float4 vIn;
    float vo[4];
    Vec3dToFloat4(&vIn, _vIn);

    Float4 result = _mm_mul_ps( splatX(vIn), _m->m_l[0] );
    result = _mm_add_ps( _mm_mul_ps(splatY(vIn), _m->m_l[1]), result );
    _mm_storeu_ps(vo, _mm_add_ps( _mm_mul_ps(splatZ(vIn), _m->m_l[2]), result ));

    _vOut->m_x = vo[0];
    _vOut->m_y = vo[1];
    _vOut->m_z = vo[2];
}

ITF_INLINE void M44_matrixVec2dTransform(Vec2d* _vOut, const Matrix44* _m, const Vec2d* _vIn)
{
    _vOut->m_x = (_m->_m11 * _vIn->m_x) + (_m->_m21 * _vIn->m_y);
    _vOut->m_y = (_m->_m12 * _vIn->m_x) + (_m->_m22 * _vIn->m_y);
}

ITF_INLINE void Vec3dToFloat4(Float4* _f4Out, const Vec3d* _vIn)
{
    float vin[4];
    vin[0] = _vIn->m_x;
    vin[1] = _vIn->m_y;
    vin[2] = _vIn->m_z;
    vin[3] = 0.f;
   *_f4Out = _mm_loadu_ps(vin);

}
ITF_INLINE void Float4ToVec3d(Vec3d* _vOut, const Float4* _f4In)
{
    float vo[4];
    _mm_storeu_ps(vo, *_f4In);
    _vOut->m_x = vo[0];
    _vOut->m_y = vo[1];
    _vOut->m_z = vo[2];
}

//----------------------------------------------------------------------------//
// M44 matrix identity.
//----------------------------------------------------------------------------//
extern const Matrix44 MatrixIdentity;


#endif // _ITF_MATRIX44_X86_H_
