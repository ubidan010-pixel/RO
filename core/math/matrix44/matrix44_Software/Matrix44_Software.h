#ifndef _ITF_MATRIX44_SOFTWARE_H_
#define _ITF_MATRIX44_SOFTWARE_H_

//----------------------------------------------------------------------------//
// matrix44 class functions.
//----------------------------------------------------------------------------//
ITF_INLINE void Matrix44::reset()
{
    ITF_MemSet(fa16, 0, sizeof(f32) * 16);
};

ITF_INLINE void Matrix44::setFromFloat(
    const f32 _v11, const f32 _v12, const f32 _v13, const f32 _v14,
    const f32 _v21, const f32 _v22, const f32 _v23, const f32 _v24,
    const f32 _v31, const f32 _v32, const f32 _v33, const f32 _v34,
    const f32 _v41, const f32 _v42, const f32 _v43, const f32 _v44
            )
{
    _m11 = _v11;
    _m12 = _v12;
    _m13 = _v13;
    _m14 = _v14;
    _m21 = _v21;
    _m22 = _v22;
    _m23 = _v23;
    _m24 = _v24;
    _m31 = _v31;
    _m32 = _v32;
    _m33 = _v33;
    _m34 = _v34;
    _m41 = _v41;
    _m42 = _v42;
    _m43 = _v43;
    _m44 = _v44;
};

ITF_INLINE void Matrix44::setTranslation(const Vec3d& _trans) 
{
    _m41 = _trans.m_x;
    _m42 = _trans.m_y;
    _m43 = _trans.m_z;
};

ITF_INLINE void Matrix44::mulScale(const Vec3d& _scale) 
{
    _m11 *= _scale.m_x;
    _m12 *= _scale.m_x;
    _m13 *= _scale.m_x;

    _m21 *= _scale.m_y;
    _m22 *= _scale.m_y;
    _m23 *= _scale.m_y;

    _m31 *= _scale.m_z;
    _m32 *= _scale.m_z;
    _m33 *= _scale.m_z;
}

ITF_INLINE Vec3d Matrix44::getXAxis() const
{
    return Vec3d(_m11, _m12, _m13);
}

ITF_INLINE Vec3d Matrix44::getYAxis() const
{
    return Vec3d(_m21, _m22, _m23);
}

ITF_INLINE Vec3d Matrix44::getZAxis() const
{
    return Vec3d(_m31, _m32, _m33);
}

ITF_INLINE Vec3d Matrix44::getTranslation() const 
{
    return Vec3d(_m41, _m42, _m43);
}

ITF_INLINE void Matrix44::setLineAt(u32 _line, f32 _x, f32 _y, f32 _z, f32 _w)
{
    setFloat4(m_l[_line], _x, _y, _z, _w);
}

//----------------------------------------------------------------------------//
// externals functions.
//----------------------------------------------------------------------------//
ITF_INLINE void create(Matrix44** _m)
{
    *_m = newAlloc(mId_Misc, Matrix44);
}

ITF_INLINE void destroy(Matrix44** _m)
{
    if (*_m)
    {
        SF_DEL(*_m);
    }
}

ITF_INLINE void M44_store(float* RESTRICT _fa16, const Matrix44* _m)
{
    ITF_Memcpy(_fa16, _m->fa16, sizeof(f32) * 16);
}

ITF_INLINE void M44_load(const float* RESTRICT _fa16, Matrix44* _m)
{
    ITF_Memcpy(_m->fa16, _fa16, sizeof(f32) * 16);
}

ITF_INLINE void M44_setMatrixIdentity(Matrix44* _mOut)
{
    ITF_MemSet(_mOut->fa16, 0, sizeof(f32) * 16);
    _mOut->_m11 = 1.f;
    _mOut->_m22 = 1.f;
    _mOut->_m33 = 1.f;
    _mOut->_m44 = 1.f;
}

ITF_INLINE void M44_setMatrixRotationX(Matrix44* _mOut, f32 _angle)
{
    ITF_MemSet(_mOut->fa16, 0, sizeof(f32) * 16);

    f32 c, s;
    f32_CosSin(_angle, &c, &s);

    _mOut->_m11 = 1.f;
    _mOut->_m22 = c;
    _mOut->_m23 = s;
    _mOut->_m32 = -s;
    _mOut->_m33 = c;
    _mOut->_m44 = 1.f;
}

ITF_INLINE void M44_setMatrixRotationY(Matrix44* _mOut, f32 _angle)
{
    ITF_MemSet(_mOut->fa16, 0, sizeof(f32) * 16);

    f32 c, s;
    f32_CosSin(_angle, &c, &s);

    _mOut->_m11 = c;
    _mOut->_m13 = -s;
    _mOut->_m22 = 1.f;
    _mOut->_m31 = s;
    _mOut->_m33 = c;
    _mOut->_m44 = 1.f;
}

ITF_INLINE void M44_setMatrixRotationZ(Matrix44* __restrict _mOut, f32 _angle)
{
    ITF_MemSet(_mOut->fa16, 0, sizeof(f32) * 16);

    f32 c, s;
    f32_CosSin(_angle, &c, &s);

    _mOut->_m11 = c;
    _mOut->_m12 = s;
    _mOut->_m21 = -s;
    _mOut->_m22 = c;
    _mOut->_m33 = 1.f;
    _mOut->_m44 = 1.f;
}

ITF_INLINE void setMatrixRotationYawPithRoll(Matrix44* _mOut, f32 _yaw, f32 _pitch, f32 _roll)
{
    f32 sa, sb, sc, ca, cb, cc;
    f32_CosSin(_roll,  &ca, &sa);
    f32_CosSin(_pitch, &cb, &sb);
    f32_CosSin(_yaw,   &cc, &sc);

    _mOut->_m11 = ca*cc+sa*sb*sc;
    _mOut->_m12 = -sa*cc+ca*sb*sc;
    _mOut->_m13 = cb*sc;  
    _mOut->_m14 = 0.f;
    _mOut->_m21 = sa*cb;
    _mOut->_m22 = ca*cb;
    _mOut->_m23 = -sb;
    _mOut->_m24 = 0.f;
    _mOut->_m31 = -ca*sc+sa*sb*cc;
    _mOut->_m32 = sa*sc+ca*sb*cc;
    _mOut->_m33 = cb*cc;
    _mOut->_m34 = 0.f;
    _mOut->_m41 = 0.f;
    _mOut->_m42 = 0.f;
    _mOut->_m43 = 0.f;
    _mOut->_m44 = 1.f;
}

ITF_INLINE void M44_setMatrixScaling(Matrix44* _mOut, f32 _x, f32 _y, f32 _z)
{
    ITF_MemSet(_mOut->fa16, 0, sizeof(f32) * 16);

    _mOut->_m11 = _x;
    _mOut->_m22 = _y;
    _mOut->_m33 = _z;
    _mOut->_m44 = 1.f;
}

ITF_INLINE void M44_setMatrixTranslation(Matrix44* _mOut, f32 _x, f32 _y, f32 _z)
{
    ITF_MemSet(_mOut->fa16, 0, sizeof(f32) * 16);

    _mOut->_m11 = 1.f;
    _mOut->_m22 = 1.f;
    _mOut->_m33 = 1.f;
    _mOut->_m41 = _x;
    _mOut->_m42 = _y;
    _mOut->_m43 = _z;
    _mOut->_m44 = 1.f;
}

#if defined( ITF_PSI )
ITF_INLINE
void M44PSI_mul4x4Out(register Matrix44 * ITF_RESTRICT ab, const register Matrix44 *a, const register Matrix44 *b)
{
    asm{
	psq_l       fp0 ,  0(a), 0, 0;          // a00,a01
	psq_l       fp2 ,  0(b), 0, 0;          // b00,b01
	ps_muls0    fp6 ,   fp2,  fp0;          // b00a00,b01a00
	psq_l       fp3 , 16(b), 0, 0;          // b10,b11
	psq_l       fp4 , 32(b), 0, 0;          // b20,b21
	ps_madds1   fp6 ,   fp3,  fp0,  fp6;    // b00a00+b10a01,b01a00+b11a01
	psq_l       fp1 ,  8(a), 0, 0;          // a02,a03
	psq_l       fp5 , 48(b), 0, 0;          // b30,b31

	// b00a00+b10a01+b20a02,b01a00+b11a01+b21a02
	ps_madds0   fp6 ,   fp4,  fp1,  fp6;
	psq_l       fp0 , 16(a), 0, 0;          // a10,a11

	// b00a00+b10a01+b20a02+b30a03,b01a00+b11a01+b21a02+b31a03
	ps_madds1   fp6 ,   fp5,  fp1,  fp6;
	psq_l       fp1 , 24(a), 0, 0;          // a12,a13
	ps_muls0    fp8 ,   fp2,  fp0;          // b00a10,b01a10
	ps_madds1   fp8 ,   fp3,  fp0,  fp8;    // b00a10+b10a11,b01a11+b11a11
	psq_l       fp0 , 32(a), 0, 0;          // a20,a21

	// b00a10+b10a11+b20a12,b01a11+b11a11+b21a12
	ps_madds0   fp8 ,   fp4,  fp1,  fp8;

	// b00a10+b10a11+b20a12+b30a13,b01a10+b11a11+b21a12+b31a13
	ps_madds1   fp8 ,   fp5,  fp1,  fp8;
	psq_l       fp1 , 40(a), 0, 0;          // a22,a23
	ps_muls0    fp10,   fp2,  fp0;          // b00a20,b01a20
	ps_madds1   fp10,   fp3,  fp0, fp10;    // b00a20+b10a21,b01a20+b11a21
	psq_l       fp0 , 48(a), 0, 0;          // a30,a31

	// b00a20+b10a21+b20a22,b01a20+b11a21+b21a22
	ps_madds0   fp10,   fp4,  fp1, fp10;

	// b00a20+b10a21+b20a22+b30a23,b01a20+b11a21+b21a22+b31a23
	ps_madds1   fp10,   fp5,  fp1, fp10;
	psq_l       fp1 , 56(a), 0, 0;          // a32,a33

	ps_muls0    fp12,   fp2,  fp0;          // b00a30,b01a30
	psq_l       fp2 ,  8(b), 0, 0;          // b02,b03
	ps_madds1   fp12,   fp3,  fp0, fp12;    // b00a30+b10a31,b01a30+b11a31
	psq_l       fp0 ,  0(a), 0, 0;          // a00,a01

	// b00a30+b10a31+b20a32,b01a30+b11a31+b21a32
	ps_madds0   fp12,   fp4,  fp1, fp12;
	psq_l       fp3 , 24(b), 0, 0;          // b12,b13

	// b00a30+b10a31+b20a32+b30a33,b01a30+b11a31+b21a32+b31a33
	ps_madds1   fp12,   fp5,  fp1, fp12;
	psq_l       fp1 ,  8(a), 0, 0;          // a02,a03

	ps_muls0    fp7 ,   fp2,  fp0;          // b02a00,b03a00
	psq_l       fp4 , 40(b), 0, 0;          // b22,b23
	ps_madds1   fp7 ,   fp3,  fp0, fp7;     // b02a00+b12a01,b03a00+b13a01
	psq_l       fp5 , 56(b), 0, 0;          // b32,b33

	// b02a00+b12a01+b22a02,b03a00+b13a01+b23a02
	ps_madds0   fp7 ,   fp4,  fp1, fp7;

	psq_l       fp0 , 16(a), 0, 0;          // a10,a11

	// b02a00+b12a01+b22a02+b32a03,b03a00+b13a01+b23a02+b33a03
	ps_madds1   fp7 ,   fp5,  fp1, fp7; 
	psq_l       fp1 , 24(a), 0, 0;          // a12,a13

	ps_muls0    fp9 ,   fp2,  fp0;          // b02a10,b03a10
	psq_st      fp6 , 0(ab), 0, 0;          // ab00,ab01
	ps_madds1   fp9 ,   fp3,  fp0, fp9;     // b02a10+b12a11,b03a10+b13a11
	psq_l       fp0 , 32(a), 0, 0;          // a20,a21

	// b02a10+b12a11+b22a12,b03a10+b13a11+b23a12
	ps_madds0   fp9,    fp4,  fp1, fp9;
	psq_st      fp8 ,16(ab), 0, 0;          // ab10,ab11

	// b02a10+b12a11+b22a12+b32a13,b03a10+b13a11+b23a12+b33a13
	ps_madds1   fp9 ,   fp5,  fp1, fp9;
	psq_l       fp1 , 40(a), 0, 0;          // a22,a23
	ps_muls0    fp11,   fp2,  fp0;          // b02a20,b03a20
	psq_st      fp10,32(ab), 0, 0;          // ab20,ab21
	ps_madds1   fp11,   fp3,  fp0, fp11;    // b02a20+b12a21,b03a20+b13a21
	psq_l       fp0 , 48(a), 0, 0;          // a30,a31

	// b02a20+b12a21+b22a22,b03a20+b13a21+b23a22
	ps_madds0   fp11,   fp4,  fp1, fp11;
	psq_st      fp12,48(ab), 0, 0;          // ab30,ab31

	// b02a20+b12a21+b22a22+b32a23,b03a20+b13a21+b23a22+b33a23
	ps_madds1   fp11,   fp5,  fp1, fp11;

	psq_l       fp1,  56(a), 0, 0;          // a32,a33
	ps_muls0    fp13,   fp2,  fp0;          // b02a30,b03a30
	psq_st      fp7 , 8(ab), 0, 0;          // ab02,ab03
	ps_madds1   fp13,   fp3,  fp0, fp13;    // b02a30+b12a31,b03a30+b13a31
	psq_st      fp9 ,24(ab), 0, 0;          // ab12,ab13

	// b02a30+b12a31+b22a32,b03a30+b13a31+b23a32
	ps_madds0   fp13,   fp4,  fp1, fp13;
	psq_st      fp11,40(ab), 0, 0;          // ab22,ab23

	// b02a30+b12a31+b22a32+b32a33,b03a30+b13a31+b23a32+b33a33
	ps_madds1   fp13,   fp5,  fp1, fp13;

	psq_st      fp13,56(ab), 0, 0;          // ab32,ab33
	}
}
#endif // ITF_PSI

ITF_INLINE void M44_mul4x4Out(Matrix44* _Out, const Matrix44* _mInA, const Matrix44* _mInB)
{
#if defined( ITF_PSI )
    M44PSI_mul4x4Out(_Out, _mInA, _mInB);
#else
    _Out->_m11 = (_mInA->_m11 * _mInB->_m11) + (_mInA->_m12 * _mInB->_m21) + (_mInA->_m13 * _mInB->_m31) + (_mInA->_m14 * _mInB->_m41);
    _Out->_m12 = (_mInA->_m11 * _mInB->_m12) + (_mInA->_m12 * _mInB->_m22) + (_mInA->_m13 * _mInB->_m32) + (_mInA->_m14 * _mInB->_m42);
    _Out->_m13 = (_mInA->_m11 * _mInB->_m13) + (_mInA->_m12 * _mInB->_m23) + (_mInA->_m13 * _mInB->_m33) + (_mInA->_m14 * _mInB->_m43);
    _Out->_m14 = (_mInA->_m11 * _mInB->_m14) + (_mInA->_m12 * _mInB->_m24) + (_mInA->_m13 * _mInB->_m34) + (_mInA->_m14 * _mInB->_m44);

    _Out->_m21 = (_mInA->_m21 * _mInB->_m11) + (_mInA->_m22 * _mInB->_m21) + (_mInA->_m23 * _mInB->_m31) + (_mInA->_m24 * _mInB->_m41);
    _Out->_m22 = (_mInA->_m21 * _mInB->_m12) + (_mInA->_m22 * _mInB->_m22) + (_mInA->_m23 * _mInB->_m32) + (_mInA->_m24 * _mInB->_m42);
    _Out->_m23 = (_mInA->_m21 * _mInB->_m13) + (_mInA->_m22 * _mInB->_m23) + (_mInA->_m23 * _mInB->_m33) + (_mInA->_m24 * _mInB->_m43);
    _Out->_m24 = (_mInA->_m21 * _mInB->_m14) + (_mInA->_m22 * _mInB->_m24) + (_mInA->_m23 * _mInB->_m34) + (_mInA->_m24 * _mInB->_m44);

    _Out->_m31 = (_mInA->_m31 * _mInB->_m11) + (_mInA->_m32 * _mInB->_m21) + (_mInA->_m33 * _mInB->_m31) + (_mInA->_m34 * _mInB->_m41);
    _Out->_m32 = (_mInA->_m31 * _mInB->_m12) + (_mInA->_m32 * _mInB->_m22) + (_mInA->_m33 * _mInB->_m32) + (_mInA->_m34 * _mInB->_m42);
    _Out->_m33 = (_mInA->_m31 * _mInB->_m13) + (_mInA->_m32 * _mInB->_m23) + (_mInA->_m33 * _mInB->_m33) + (_mInA->_m34 * _mInB->_m43);
    _Out->_m34 = (_mInA->_m31 * _mInB->_m14) + (_mInA->_m32 * _mInB->_m24) + (_mInA->_m33 * _mInB->_m34) + (_mInA->_m34 * _mInB->_m44);

    _Out->_m41 = (_mInA->_m41 * _mInB->_m11) + (_mInA->_m42 * _mInB->_m21) + (_mInA->_m43 * _mInB->_m31) + (_mInA->_m44 * _mInB->_m41);
    _Out->_m42 = (_mInA->_m41 * _mInB->_m12) + (_mInA->_m42 * _mInB->_m22) + (_mInA->_m43 * _mInB->_m32) + (_mInA->_m44 * _mInB->_m42);
    _Out->_m43 = (_mInA->_m41 * _mInB->_m13) + (_mInA->_m42 * _mInB->_m23) + (_mInA->_m43 * _mInB->_m33) + (_mInA->_m44 * _mInB->_m43);
    _Out->_m44 = (_mInA->_m41 * _mInB->_m14) + (_mInA->_m42 * _mInB->_m24) + (_mInA->_m43 * _mInB->_m34) + (_mInA->_m44 * _mInB->_m44);
#endif
}

ITF_INLINE void M44_mul3x3Out(Matrix44* _Out, const Matrix44* _mInA, const Matrix44* _mInB)
{
    _Out->_m11 = (_mInA->_m11 * _mInB->_m11) + (_mInA->_m12 * _mInB->_m21) + (_mInA->_m13 * _mInB->_m31);
    _Out->_m12 = (_mInA->_m11 * _mInB->_m12) + (_mInA->_m12 * _mInB->_m22) + (_mInA->_m13 * _mInB->_m32);
    _Out->_m13 = (_mInA->_m11 * _mInB->_m13) + (_mInA->_m12 * _mInB->_m23) + (_mInA->_m13 * _mInB->_m33);
    _Out->_m21 = (_mInA->_m21 * _mInB->_m11) + (_mInA->_m22 * _mInB->_m21) + (_mInA->_m23 * _mInB->_m31);
    _Out->_m22 = (_mInA->_m21 * _mInB->_m12) + (_mInA->_m22 * _mInB->_m22) + (_mInA->_m23 * _mInB->_m32);
    _Out->_m23 = (_mInA->_m21 * _mInB->_m13) + (_mInA->_m22 * _mInB->_m23) + (_mInA->_m23 * _mInB->_m33);
    _Out->_m31 = (_mInA->_m31 * _mInB->_m11) + (_mInA->_m32 * _mInB->_m21) + (_mInA->_m33 * _mInB->_m31);
    _Out->_m32 = (_mInA->_m31 * _mInB->_m12) + (_mInA->_m32 * _mInB->_m22) + (_mInA->_m33 * _mInB->_m32);
    _Out->_m23 = (_mInA->_m31 * _mInB->_m13) + (_mInA->_m32 * _mInB->_m23) + (_mInA->_m33 * _mInB->_m33);
}

ITF_INLINE void M44_matrixTranspose(Matrix44* _mOut, const Matrix44* _mIn)
{
    const i32	*Src;
    i32			*Dst;

    Src = (const i32 *) &(_mIn->_m11);
    Dst = (i32 *) &(_mOut->_m11);

    *(Dst) = *(Src);
    *(Dst + 4) = *(Src + 1);
    *(Dst + 8) = *(Src + 2);
    *(Dst + 12) = *(Src + 3);

    *(Dst + 1) = *(Src + 4);
    *(Dst + 5) = *(Src + 5);
    *(Dst + 9) = *(Src + 6);
    *(Dst + 13) = *(Src + 7);

    *(Dst + 2) = *(Src + 8);
    *(Dst + 6) = *(Src + 9);
    *(Dst + 10) = *(Src + 10);
    *(Dst + 14) = *(Src + 11);

    *(Dst + 3) = *(Src + 12);
    *(Dst + 7) = *(Src + 13);
    *(Dst + 11) = *(Src + 14);
    *(Dst + 15) = *(Src + 15);
}

ITF_INLINE void M44_matrixInverse(f32* dst, const f32* scr)
{
    f32 m22_33_23_32 = scr[10] * scr[15] - scr[11] * scr[14];
	f32 m12_33_13_32 = scr[6] * scr[15] - scr[7] * scr[14];
	f32 m12_23_13_22 = scr[6] * scr[11] - scr[7] * scr[10];
	f32 m02_33_03_32 = scr[2] * scr[15] - scr[3] * scr[14];
	f32 m02_23_03_22 = scr[2] * scr[11] - scr[3] * scr[10];
	f32 m02_13_03_12 = scr[2] * scr[7] - scr[3] * scr[6];
	f32 m21_33_23_31 = scr[9] * scr[15] - scr[11] * scr[13];
	f32 m11_33_13_31 = scr[5] * scr[15] - scr[7] * scr[13];
	f32 m11_23_13_21 = scr[5] * scr[11] - scr[7] * scr[9];
	f32 m01_33_03_31 = scr[1] * scr[15] - scr[3] * scr[13];
	f32 m01_23_03_21 = scr[1] * scr[11] - scr[3] * scr[9];
	f32 m01_13_03_11 = scr[1] * scr[7] - scr[3] * scr[5];
	f32 m21_32_22_31 = scr[9] * scr[14] - scr[10] * scr[13];
	f32 m11_32_12_31 = scr[5] * scr[14] - scr[6] * scr[13];
	f32 m11_22_12_21 = scr[5] * scr[10] - scr[6] * scr[9];
	f32 m01_32_02_31 = scr[1] * scr[14] - scr[2] * scr[13];
	f32 m01_22_02_21 = scr[1] * scr[10] - scr[2] * scr[9];
	f32 m01_12_02_11 = scr[1] * scr[6] - scr[2] * scr[5];
	f32 f_Det = scr[0] * (scr[5] * (m22_33_23_32) - scr[9] * (m12_33_13_32) + scr[13] * (m12_23_13_22)) - scr[4] *
		(scr[1] * (m22_33_23_32) - scr[9] * (m02_33_03_32) + scr[13] * (m02_23_03_22)) + scr[8] *
			(scr[1] * (m12_33_13_32) - scr[5] * (m02_33_03_32) + scr[13] * (m02_13_03_12)) - scr[12] *
				(scr[1] * (m12_23_13_22) - scr[5] * (m02_23_03_22) + scr[9] * (m02_13_03_12));

	if(f_Det == 0.0f)
	{
        M44_setMatrixIdentity((Matrix44*)dst);
		return;
	}

	f32 f_RDet = 1.0f / f_Det;

	dst[0] = f_RDet * (scr[5] * (m22_33_23_32) - scr[9] * (m12_33_13_32) + scr[13] * (m12_23_13_22));
	dst[1] = -f_RDet * (scr[1] * (m22_33_23_32) - scr[9] * (m02_33_03_32) + scr[13] * (m02_23_03_22));
	dst[2] = f_RDet * (scr[1] * (m12_33_13_32) - scr[5] * (m02_33_03_32) + scr[13] * (m02_13_03_12));
	dst[3] = -f_RDet * (scr[1] * (m12_23_13_22) - scr[5] * (m02_23_03_22) + scr[9] * (m02_13_03_12));

	dst[4] = -f_RDet * (scr[4] * (m22_33_23_32) - scr[8] * (m12_33_13_32) + scr[12] * (m12_23_13_22));
	dst[5] = f_RDet * (scr[0] * (m22_33_23_32) - scr[8] * (m02_33_03_32) + scr[12] * (m02_23_03_22));
	dst[6] = -f_RDet * (scr[0] * (m12_33_13_32) - scr[4] * (m02_33_03_32) + scr[12] * (m02_13_03_12));
	dst[7] = f_RDet * (scr[0] * (m12_23_13_22) - scr[4] * (m02_23_03_22) + scr[8] * (m02_13_03_12));

	dst[8] = f_RDet * (scr[4] * (m21_33_23_31) - scr[8] * (m11_33_13_31) + scr[12] * (m11_23_13_21));
	dst[9] = -f_RDet * (scr[0] * (m21_33_23_31) - scr[8] * (m01_33_03_31) + scr[12] * (m01_23_03_21));
	dst[10] = f_RDet * (scr[0] * (m11_33_13_31) - scr[4] * (m01_33_03_31) + scr[12] * (m01_13_03_11));
	dst[11] = -f_RDet * (scr[0] * (m11_23_13_21) - scr[4] * (m01_23_03_21) + scr[8] * (m01_13_03_11));

	dst[12] = -f_RDet * (scr[4] * (m21_32_22_31) - scr[8] * (m11_32_12_31) + scr[12] * (m11_22_12_21));
	dst[13] = f_RDet * (scr[0] * (m21_32_22_31) - scr[8] * (m01_32_02_31) + scr[12] * (m01_22_02_21));
	dst[14] = -f_RDet * (scr[0] * (m11_32_12_31) - scr[4] * (m01_32_02_31) + scr[12] * (m01_12_02_11));
	dst[15] = f_RDet * (scr[0] * (m11_22_12_21) - scr[4] * (m01_22_02_21) + scr[8] * (m01_12_02_11));
}

ITF_INLINE void M44_matrixCoordTransform(Float4* _f4Out, const Matrix44* _m, const Float4* _f4In)
{
    _f4Out->m_x = (_m->_m11 * _f4In->m_x) + (_m->_m21 * _f4In->m_y) + (_m->_m31 * _f4In->m_z) + _m->_m41;
    _f4Out->m_y = (_m->_m12 * _f4In->m_x) + (_m->_m22 * _f4In->m_y) + (_m->_m32 * _f4In->m_z) + _m->_m42;
    _f4Out->m_z = (_m->_m13 * _f4In->m_x) + (_m->_m23 * _f4In->m_y) + (_m->_m33 * _f4In->m_z) + _m->_m43;
    _f4Out->m_w = (_m->_m14 * _f4In->m_x) + (_m->_m24 * _f4In->m_y) + (_m->_m34 * _f4In->m_z) + _m->_m44;
}

ITF_INLINE void M44_matrixCoordTransform(Vec3d* _vOut, const Matrix44* _m, const Vec3d* _vIn)
{
    M44_matrixVec3dTransform(_vOut, _m, _vIn);
    _vOut->m_x += _m->_m41;
    _vOut->m_y += _m->_m42;
    _vOut->m_z += _m->_m43;
}

ITF_INLINE void M44_matrixCoordTransform(Vec2d* _vOut, const Matrix44* _m, const Vec2d* _vIn)
{
    M44_matrixVec2dTransform(_vOut, _m, _vIn);
    _vOut->m_x += _m->_m41;
    _vOut->m_y += _m->_m42;
}

ITF_INLINE void M44_matrixFloat4Transform(Float4* _f4Out, const Matrix44* _m, const Float4* _f4In)
{
    _f4Out->m_x = (_m->_m11 * _f4In->m_x) + (_m->_m21 * _f4In->m_y) + (_m->_m31 * _f4In->m_z);
    _f4Out->m_y = (_m->_m12 * _f4In->m_x) + (_m->_m22 * _f4In->m_y) + (_m->_m32 * _f4In->m_z);
    _f4Out->m_z = (_m->_m13 * _f4In->m_x) + (_m->_m23 * _f4In->m_y) + (_m->_m33 * _f4In->m_z);
    _f4Out->m_w = (_m->_m14 * _f4In->m_x) + (_m->_m24 * _f4In->m_y) + (_m->_m34 * _f4In->m_z);
}

ITF_INLINE void M44_matrixVec3dTransform(Vec3d* _vOut, const Matrix44* _m, const Vec3d* _vIn)
{
    _vOut->m_x = (_m->_m11 * _vIn->m_x) + (_m->_m21 * _vIn->m_y) + (_m->_m31 * _vIn->m_z);
    _vOut->m_y = (_m->_m12 * _vIn->m_x) + (_m->_m22 * _vIn->m_y) + (_m->_m32 * _vIn->m_z);
    _vOut->m_z = (_m->_m13 * _vIn->m_x) + (_m->_m23 * _vIn->m_y) + (_m->_m33 * _vIn->m_z);
}

ITF_INLINE void M44_matrixVec2dTransform(Vec2d* _vOut, const Matrix44* _m, const Vec2d* _vIn)
{
    _vOut->m_x = (_m->_m11 * _vIn->m_x) + (_m->_m21 * _vIn->m_y);
    _vOut->m_y = (_m->_m12 * _vIn->m_x) + (_m->_m22 * _vIn->m_y);
}

ITF_INLINE void Vec3dToFloat4(Float4* _f4Out, const Vec3d* _vIn)
{
    _f4Out->m_x = _vIn->m_x;
    _f4Out->m_y = _vIn->m_y;
    _f4Out->m_z = _vIn->m_z;
    _f4Out->m_w = 0.f;
}
ITF_INLINE void Float4ToVec3d(Vec3d* _vOut, const Float4* _f4In)
{
    _vOut->m_x = _f4In->m_x;
    _vOut->m_y = _f4In->m_y;
    _vOut->m_z = _f4In->m_z;
}

//----------------------------------------------------------------------------//
// M44 matrix identity.
//----------------------------------------------------------------------------//
extern const Matrix44 MatrixIdentity;

#endif // _ITF_MATRIX44_SOFTWARE_H_
