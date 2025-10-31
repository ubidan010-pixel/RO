#ifndef _ITF_MATRIX44_H_
#define _ITF_MATRIX44_H_

#ifndef _ITF_VEC3D_H_
#include "core/math/vec3d.h"
#endif //_ITF_VEC3D_H_

#ifndef _ITF_VEC2D_H_
#include "core/math/vec2d.h"
#endif //_ITF_VEC2D_H_

//----------------------------------------------------------------------------//
// Platform define.
//----------------------------------------------------------------------------//
#if defined(ITF_WINDOWS)
    #if defined(WRAPPER_C)
        #define _Software
    #else
        #define _x86
    #endif
#elif defined(ITF_X360)
    #define _Software
#else
    #define _Software
#endif

//----------------------------------------------------------------------------//
// header from platform.
//----------------------------------------------------------------------------//
#if defined(_Software)
#include "matrix44_Software/matrix44_Software_header.h"
#elif defined(_x86)
#include "matrix44_x86/matrix44_x86_header.h"
#endif

namespace ITF
{
    //----------------------------------------------------------------------------//
    // macros.
    //----------------------------------------------------------------------------//
    #define _m11 fa16[0]
    #define _m12 fa16[1]
    #define _m13 fa16[2]
    #define _m14 fa16[3]
    #define _m21 fa16[4]
    #define _m22 fa16[5]
    #define _m23 fa16[6]
    #define _m24 fa16[7]
    #define _m31 fa16[8]
    #define _m32 fa16[9]
    #define _m33 fa16[10]
    #define _m34 fa16[11]
    #define _m41 fa16[12]
    #define _m42 fa16[13]
    #define _m43 fa16[14]
    #define _m44 fa16[15]

    class Matrix44;

    //----------------------------------------------------------------------------//
    // generic matrix. (Minimum operation)
    //----------------------------------------------------------------------------//
    struct GMatrix44
    {
        GMatrix44()
        {
            fa16[0] = 0.0f;
            fa16[1] = 0.0f;
            fa16[2] = 0.0f;
            fa16[3] = 0.0f;
            fa16[4] = 0.0f;
            fa16[5] = 0.0f;
            fa16[6] = 0.0f;
            fa16[7] = 0.0f;
            fa16[8] = 0.0f;
            fa16[9] = 0.0f;
            fa16[10] = 0.0f;
            fa16[11] = 0.0f;
            fa16[12] = 0.0f;
            fa16[13] = 0.0f;
            fa16[14] = 0.0f;
            fa16[15] = 0.0f;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// construct from float.
        GMatrix44( const f32 _v11, const f32 _v12, const f32 _v13, const f32 _v14,
                    const f32 _v21, const f32 _v22, const f32 _v23, const f32 _v24,
                    const f32 _v31, const f32 _v32, const f32 _v33, const f32 _v34,
                    const f32 _v41, const f32 _v42, const f32 _v43, const f32 _v44
                    )
        {
            fa16[0] = _v11;
            fa16[1] = _v12;
            fa16[2] = _v13;
            fa16[3] = _v14;
            fa16[4] = _v21;
            fa16[5] = _v22;
            fa16[6] = _v23;
            fa16[7] = _v24;
            fa16[8] = _v31;
            fa16[9] = _v32;
            fa16[10] = _v33;
            fa16[11] = _v34;
            fa16[12] = _v41;
            fa16[13] = _v42;
            fa16[14] = _v43;
            fa16[15] = _v44;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Constructor, for 44 matrix
        /// @param _m
        GMatrix44(const Matrix44 &_m)  {makeFromMatrix44(_m);}

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// copyToMatrix44
        /// @param _m : the 44 matrix which receives the transform
        ITF_INLINE void copyToMatrix44(Matrix44 &_m) const;

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// from 44 matrix
        /// @param _m : the 44 source matrix 
        ITF_INLINE void makeFromMatrix44(const Matrix44 &_m) {ITF_Memcpy(fa16, &_m, sizeof(GMatrix44));}

ITF_INLINE void setIdentity()
        {
            fa16[0] = 1.0f;
            fa16[1] = 0.0f;
            fa16[2] = 0.0f;
            fa16[3] = 0.0f;
            fa16[4] = 0.0f;
            fa16[5] = 1.0f;
            fa16[6] = 0.0f;
            fa16[7] = 0.0f;
            fa16[8] = 0.0f;
            fa16[9] = 0.0f;
            fa16[10] = 1.0f;
            fa16[11] = 0.0f;
            fa16[12] = 0.0f;
            fa16[13] = 0.0f;
            fa16[14] = 0.0f;
            fa16[15] = 1.0f;        }

        ITF_INLINE void set(
            f32 _11, f32 _12, f32 _13, f32 _14,
            f32 _21, f32 _22, f32 _23, f32 _24,
            f32 _31, f32 _32, f32 _33, f32 _34,
            f32 _41, f32 _42, f32 _43, f32 _44
            )
        {
            fa16[0] = _11;
            fa16[1] = _12;
            fa16[2] = _13;
            fa16[3] = _14;
            fa16[4] = _21;
            fa16[5] = _22;
            fa16[6] = _23;
            fa16[7] = _24;
            fa16[8] = _31;
            fa16[9] = _32;
            fa16[10] = _33;
            fa16[11] = _34;
            fa16[12] = _41;
            fa16[13] = _42;
            fa16[14] = _43;
            fa16[15] = _44;
        }

        ///////////////////////////////////////////////////////////////////////////////////////////
        ///Storage
        float fa16[16];
    };

    class Matrix44;

    //----------------------------------------------------------------------------//
    // external fuctions.
    //----------------------------------------------------------------------------//
    ITF_INLINE void M44_create(Matrix44** _m);

    ITF_INLINE void M44_destroy(Matrix44** _m);

    ITF_INLINE void M44_copy(Matrix44* _Out, const Matrix44* _In);

    ITF_INLINE void M44_store(float* RESTRICT _fa16, const Matrix44* _m);

    ITF_INLINE void M44_load(const float* RESTRICT _fa16, Matrix44* _m);

    ITF_INLINE void M44_setMatrixIdentity(Matrix44* _mOut);

    ITF_INLINE void M44_setMatrixRotationX(Matrix44* _mOut, f32 _angle);

    ITF_INLINE void M44_setMatrixRotationY(Matrix44* _mOut, f32 _angle);

    ITF_INLINE void M44_setMatrixRotationZ(Matrix44* __restrict _mOut, f32 _angle);
    
    ITF_INLINE void M44_setMatrixRotationYawPithRoll(Matrix44* _mOut, f32 _yaw, f32 _pitch, f32 _roll);

    ITF_INLINE void M44_setMatrixScaling(Matrix44* _mOut, f32 _x, f32 _y, f32 _z);

    ITF_INLINE void M44_setMatrixTranslation(Matrix44* _mOut, f32 _x, f32 _y, f32 _z);

    ITF_INLINE Matrix44 M44_mul4x4(const Matrix44* _mA, const Matrix44* _mB);

    ITF_INLINE void M44_mul4x4Out(Matrix44* _Out, const Matrix44* _mInA, const Matrix44* _mInB);

    ITF_INLINE void M44_mul3x3Out(Matrix44* _Out, const Matrix44* _mInA, const Matrix44* _mInB);

    ITF_INLINE void M44_matrixTranspose(Matrix44* _mOut, const Matrix44* _mIn);

    ITF_INLINE void M44_matrixCoordTransform(Float4* _f4Out, const Matrix44* _m, const Float4* _f4In);
    
    ITF_INLINE void M44_matrixCoordTransform(Vec3d* _vOut, const Matrix44* _m, const Vec3d* _vIn);

    ITF_INLINE void M44_matrixCoordTransform(Vec2d* _vOut, const Matrix44* _m, const Vec2d* _vIn);

    ITF_INLINE void M44_matrixFloat4Transform(Float4* _f4Out, const Matrix44* _m, const Float4* _f4In);

    ITF_INLINE void M44_matrixVec3dTransform(Vec3d* _vOut, const Matrix44* _m, const Vec3d* _vIn);

    ITF_INLINE void M44_matrixVec2dTransform(Vec2d* _vOut, const Matrix44* _m, const Vec2d* _vIn);

    ITF_INLINE void Vec3dToFloat4(Float4* _f4Out, const Vec3d* _vIn);

    ITF_INLINE void Float4ToVec3d(Vec3d* _vOut, const Float4* _f4In);

    //----------------------------------------------------------------------------//
    // class Matrix44.
    //----------------------------------------------------------------------------//
    class Matrix44
    {
    public:

        /// default constructor.
        Matrix44()
        {
            reset();
        }

        /// construct from float.
        Matrix44( const f32 _v11, const f32 _v12, const f32 _v13, const f32 _v14,
                    const f32 _v21, const f32 _v22, const f32 _v23, const f32 _v24,
                    const f32 _v31, const f32 _v32, const f32 _v33, const f32 _v34,
                    const f32 _v41, const f32 _v42, const f32 _v43, const f32 _v44
                    )
        {
            setFromFloat(
                _v11, _v12, _v13, _v14,
                _v21, _v22, _v23, _v24,
                _v31, _v32, _v33, _v34,
                _v41, _v42, _v43, _v44
                );
        }

        /// construct from array.
        Matrix44( const f32* RESTRICT _array )
        {
            M44_load(_array, this);
        }

        Matrix44( const GMatrix44 &_m)
        {
            M44_load(_m.fa16, this);
        }
        
        const Matrix44& operator = (const Matrix44& src)
        {
            M44_copy(this, &src);
            return *this;
        }    
        
        ITF_INLINE Matrix44 operator*(const Matrix44& _mat) const
        {
            return M44_mul4x4(this, &_mat);
        }

        ITF_INLINE Matrix44 operator*=(const Matrix44& _mat)
        {
            *this = M44_mul4x4(this, &_mat);
            return *this;
        }

        ITF_INLINE void reset();

        ITF_INLINE void setFromFloat( 
                    const f32 _v11, const f32 _v12, const f32 _v13, const f32 _v14,
                    const f32 _v21, const f32 _v22, const f32 _v23, const f32 _v24,
                    const f32 _v31, const f32 _v32, const f32 _v33, const f32 _v34,
                    const f32 _v41, const f32 _v42, const f32 _v43, const f32 _v44
                    );

        ITF_INLINE void setIdentity()
        {
            M44_setMatrixIdentity(this);
        }

        ITF_INLINE void setTranslation(const Vec3d& _trans);
        ITF_INLINE void mulScale(const Vec3d& _scale);
        ITF_INLINE Vec3d getXAxis() const;
        ITF_INLINE Vec3d getYAxis() const;
        ITF_INLINE Vec3d getZAxis() const;
        ITF_INLINE Vec3d getTranslation() const; 
        ITF_INLINE void setLineAt(u32 _line, f32 _x, f32 _y, f32 _z, f32 _w);

        union 
        {
            Float4 m_l[4];
            float fa16[16];
        };
    };

    //----------------------------------------------------------------------------//
    // platform functions.
    //----------------------------------------------------------------------------//
#if defined(_Software)
    #include "matrix44_Software/matrix44_Software.h"
#elif defined(_x86)
    #include "matrix44_x86/matrix44_x86.h"
#endif

    //----------------------------------------------------------------------------//
    // global platform functions.
    //----------------------------------------------------------------------------//
    ITF_INLINE Matrix44 M44_mul4x4(const Matrix44* _mA, const Matrix44* _mB)
    {
        Matrix44 out;
        M44_mul4x4Out(&out, _mA, _mB);
        return out;
    }

    ITF_INLINE void M44_copy(Matrix44* _Out, const Matrix44* _In)
    {
        _Out->m_l[0] = _In->m_l[0];
	    _Out->m_l[1] = _In->m_l[1];
	    _Out->m_l[2] = _In->m_l[2];
	    _Out->m_l[3] = _In->m_l[3];
    }

    //
    ITF_INLINE void GMatrix44::copyToMatrix44(Matrix44 &_m) const {M44_load(fa16, &_m);}

    //----------------------------------------------------------------------------//
    // generic Matrix identity.
    //----------------------------------------------------------------------------//
    extern const GMatrix44 GMatrixIdentity;

} // namespace ITF

#if defined(_Software)
    #undef _Software
#elif defined(_x86)
    #undef _x86
#endif

#endif // _ITF_MATRIX44_H_
