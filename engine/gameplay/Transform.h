#ifndef _ITF_TRANSFORM2D3D_H_
#define _ITF_TRANSFORM2D3D_H_

namespace ITF
{
//#define HUMAN_READABLE_CODE_PLEASE

#ifdef HUMAN_READABLE_CODE_PLEASE

    ITF_INLINE void rotate( f32& _x, f32& _y, const f32 _cos, const f32 _sin )
    {
        const f32 x0 = _x;
        _x = x0 * _cos - _y * _sin;
        _y = x0 * _sin + _y * _cos;
    }

    ITF_INLINE void rotate( Vec2d& _v, const Vec2d& _cosSin, const Vec2d& _mSinCos, const f32 _flipSign )
    {
        _v.m_x *= _flipSign;
        rotate(_v.m_x, _v.m_y, _cosSin.m_x, _cosSin.m_y);
    }

    ITF_INLINE void inverseRotate( Vec2d& _v, const Vec2d& _cosSin, const Vec2d& _mSinCos, const f32 _flipSign )
    {
        rotate(_v.m_x, _v.m_y, _cosSin.m_x, -_cosSin.m_y);  // cos(-a) = cos(a), sin(-a) = -sin(a)
        _v.m_x *= _flipSign;
    }

#else

    ITF_INLINE void rotate( Vec2d& _v, const Vec2d& _cosSin, const Vec2d& _mSinCos )
    {
        // x' = x * cos - y * sin;
        // y' = x * sin + y * cos;
        // v' = vXX * cosSin + vYY * mSinCos

        Vec2d vXX, vYY;
        Vec2d::MergeXX(&vXX, &_v, &_v);
        Vec2d::MergeYY(&vYY, &_v, &_v);
        Vec2d::Mul(&_v, &vXX, &_cosSin);
        Vec2d::MulAdd(&_v, &vYY, &_mSinCos, &_v);
    }

    ITF_INLINE void rotate( Vec2d& _v, const Vec2d& _cosSin, const Vec2d& _mSinCos, const f32 _flipSign )
    {
        _v.m_x *= _flipSign;
        rotate(_v, _cosSin, _mSinCos);
    }

    ITF_INLINE void inverseRotate( Vec2d& _v, const Vec2d& _cosSin, const Vec2d& _mSinCos, const f32 _flipSign )
    {
        // x' = x * cos + y * sin;
        // y' = x * -sin + y * cos;
        // v' = vXX * cosMSin + vYY * sinCos

        // negate both sines: cos(-a) = cos(a), sin(-a) = -sin(a)
        Vec2d cosMSin;
        Vec2d::NegY(&cosMSin, &_cosSin);
        Vec2d sinCos = _mSinCos;
        sinCos.m_x = -sinCos.m_x;

        rotate(_v, cosMSin, sinCos);
        _v.m_x *= _flipSign;
    }

#endif // HUMAN_READABLE_CODE_PLEASE


    ITF_INLINE void rotate( Vec3d& _v, const Vec2d& _cosSin, const Vec2d& _mSinCos, const f32 _flipSign )
    {
        rotate(reinterpret_cast<Vec2d&>(_v), _cosSin, _mSinCos, _flipSign);
    }

    ITF_INLINE void inverseRotate( Vec3d& _v, const Vec2d& _cosSin, const Vec2d& _mSinCos, const f32 _flipSign )
    {
        inverseRotate(reinterpret_cast<Vec2d&>(_v), _cosSin, _mSinCos, _flipSign);
    }


    class Transform3d;

    class Transform2d
    {
    public:
        Transform2d()
            : m_pos(Vec2d::Zero)
            , m_angle(0.f)
            , m_cosSin(1.f, 0.f)
            , m_mSinCos(0.f, 1.f)
            , m_flipSign(1.f)
        {
        }
        Transform2d( const Vec2d& _pos, const f32 _angle, const bbool _flip = bfalse )
            : m_pos(_pos)
            , m_angle(_angle)
            , m_flipSign(_flip ? -1.f : 1.f)
        {
            Vec2d::CosSin(&m_cosSin, m_angle);
            Vec2d::SwapXmY(&m_mSinCos, &m_cosSin);
        }

        ITF_INLINE const Vec2d& getPos() const { return m_pos; }
        ITF_INLINE void setPos( const Vec2d& _value ) { m_pos = _value; }

        ITF_INLINE f32 getAngle() const { return m_angle; }
        ITF_INLINE void setAngle( const f32 _value )
        {
            m_angle = _value;
            Vec2d::CosSin(&m_cosSin, m_angle);
            Vec2d::SwapXmY(&m_mSinCos, &m_cosSin);
        }

        ITF_INLINE bbool isFlipped() const { return m_flipSign < 0; }
        ITF_INLINE void setFlipped( const bbool _value ) { m_flipSign = (_value ? -1.f : 1.f); }

        ITF_INLINE f32 getFlipSign() const { return m_flipSign; }
        ITF_INLINE void setFlipSign( const f32 _value ) { m_flipSign = _value; }


        ITF_INLINE Vec2d transformPos( const Vec2d& _pos ) const
        {
            Vec2d transformedPos = _pos;
            rotate(transformedPos, m_cosSin, m_mSinCos, m_flipSign);
            return m_pos + transformedPos;
        }
        ITF_INLINE Vec2d inverseTransformPos( const Vec2d& _pos ) const
        {
            Vec2d localPos = _pos - m_pos;
            inverseRotate(localPos, m_cosSin, m_mSinCos, m_flipSign);
            return localPos;
        }


        ITF_INLINE Vec2d transformDir( const Vec2d& _dir ) const
        {
            Vec2d transformedDir = _dir;
            rotate(transformedDir, m_cosSin, m_mSinCos, m_flipSign);
            return transformedDir;
        }
        ITF_INLINE Vec2d inverseTransformDir( const Vec2d& _dir ) const
        {
            Vec2d localDir = _dir;
            inverseRotate(localDir, m_cosSin, m_mSinCos, m_flipSign);
            return localDir;
        }


        ITF_INLINE f32 transformAngle( const f32 _angle ) const { return _angle + m_angle; }
        ITF_INLINE f32 inverseTransformAngle( const f32 _angle ) const { return _angle - m_angle; }

        ITF_INLINE Transform2d transform( const Transform2d& _transform ) const { return Transform2d(transformPos(_transform.m_pos), transformAngle(_transform.m_angle)); }
        ITF_INLINE Transform3d to3d( const f32 _z = 0.f ) const;

    private:
        Vec2d m_pos;
        Vec2d m_cosSin;     // (cos, sin)
        Vec2d m_mSinCos;    // (-sin, cos)
        f32 m_flipSign;
        f32 m_angle;
    };

    class Transform3d
    {
    public:
        Transform3d()
            : m_pos(Vec3d::Zero)
            , m_angle(0.f)
            , m_cosSin(1.f, 0.f)
            , m_mSinCos(0.f, 1.f)
            , m_flipSign(1.f)
        {
        }
        Transform3d( const Vec3d& _pos, const f32 _angle, const bbool _flip = bfalse )
            : m_pos(_pos)
            , m_angle(_angle)
            , m_flipSign(_flip ? -1.f : 1.f)
        {
            Vec2d::CosSin(&m_cosSin, m_angle);
            Vec2d::SwapXmY(&m_mSinCos, &m_cosSin);
        }

        ITF_INLINE const Vec3d& getPos() const { return m_pos; }
        ITF_INLINE void setPos( const Vec3d& _value ) { m_pos = _value; }

        ITF_INLINE f32 getAngle() const { return m_angle; }
        ITF_INLINE void setAngle( const f32 _value )
        {
            m_angle = _value;
            Vec2d::CosSin(&m_cosSin, m_angle);
            Vec2d::SwapXmY(&m_mSinCos, &m_cosSin);
        }

        ITF_INLINE bbool isFlipped() const { return m_flipSign < 0; }
        ITF_INLINE void setFlipped( const bbool _value ) { m_flipSign = (_value ? -1.f : 1.f); }

        ITF_INLINE f32 getFlipSign() const { return m_flipSign; }
        ITF_INLINE void setFlipSign( const f32 _value ) { m_flipSign = _value; }


        ITF_INLINE Vec3d transformPos( const Vec3d& _pos ) const
        {
            Vec3d transformedPos = _pos;
            rotate(transformedPos, m_cosSin, m_mSinCos, m_flipSign);
            return m_pos + transformedPos;
        }
        ITF_INLINE Vec3d inverseTransformPos( const Vec3d& _pos ) const
        {
            Vec3d localPos = _pos - m_pos;
            inverseRotate(localPos, m_cosSin, m_mSinCos, m_flipSign);
            return localPos;
        }
        ITF_INLINE Vec3d transformPos( const Vec2d& _pos ) const { return transformPos(_pos.to3d()); }
        ITF_INLINE Vec3d inverseTransformPos( const Vec2d& _pos ) const { return inverseTransformPos(_pos.to3d()); }


        ITF_INLINE Vec3d transformDir( const Vec3d& _dir ) const
        {
            Vec3d transformedDir = _dir;
            rotate(transformedDir, m_cosSin, m_mSinCos, m_flipSign);
            return transformedDir;
        }
        ITF_INLINE Vec3d inverseTransformDir( const Vec3d& _dir ) const
        {
            Vec3d localDir = _dir;
            inverseRotate(localDir, m_cosSin, m_mSinCos, m_flipSign);
            return localDir;
        }
        ITF_INLINE Vec2d transformDir( const Vec2d& _dir ) const
        {
            Vec2d transformedDir = _dir;
            rotate(transformedDir, m_cosSin, m_mSinCos, m_flipSign);
            return transformedDir;
        }
        ITF_INLINE Vec2d inverseTransformDir( const Vec2d& _dir ) const
        {
            Vec2d localDir = _dir;
            inverseRotate(localDir, m_cosSin, m_mSinCos, m_flipSign);
            return localDir;
        }


        ITF_INLINE f32 transformAngle( const f32 _angle ) const { return _angle + m_angle; }
        ITF_INLINE f32 inverseTransformAngle( const f32 _angle ) const { return _angle - m_angle; }

        ITF_INLINE Transform3d transform( const Transform3d& _transform ) const { return Transform3d(transformPos(_transform.m_pos), transformAngle(_transform.m_angle)); }

    private:
        Vec3d m_pos;
        Vec2d m_cosSin;     // (cos, sin)
        Vec2d m_mSinCos;    // (-sin, cos)
        f32 m_flipSign;
        f32 m_angle;
    };

    ITF_INLINE Transform3d Transform2d::to3d( const f32 _z /*= 0.f */ ) const
    {
        return Transform3d(m_pos.to3d(_z), m_angle, isFlipped());
    }



    // Actor utils

    ITF_INLINE Transform2d getWorldTransform2d( const Actor* _actor, const bbool _useFlip = btrue )
    {
        return Transform2d(_actor->get2DPos(), _actor->getAngle(), _useFlip && _actor->isFlipped());
    }

    ITF_INLINE Transform2d getWorldInitTransform2d( const Actor* _actor, const bbool _useFlip = btrue )
    {
        return Transform2d(_actor->getBoundWorldInitialPos().truncateTo2D(), _actor->getBoundWorldInitialAngle(), _useFlip && _actor->startFlipped());
    }

    ITF_INLINE Transform2d getLocalTransform2d( const Actor* _actor, const bbool _useFlip = btrue )
    {
        return Transform2d(_actor->getBoundLocal2DPos(), _actor->getBoundLocalAngle(), _useFlip && _actor->isFlipped());
    }

    ITF_INLINE Transform2d getLocalInitTransform2d( const Actor* _actor, const bbool _useFlip = btrue )
    {
        return Transform2d(_actor->getBoundLocalInitial2DPos(), _actor->getBoundLocalInitialAngle(), _useFlip && _actor->startFlipped());
    }


    ITF_INLINE Transform3d getWorldTransform( const Actor* _actor, const bbool _useFlip = btrue )
    {
        return Transform3d(_actor->getPos(), _actor->getAngle(), _useFlip && _actor->isFlipped());
    }

    ITF_INLINE Transform3d getWorldInitTransform( const Actor* _actor, const bbool _useFlip = btrue )
    {
        return Transform3d(_actor->getBoundWorldInitialPos(), _actor->getBoundWorldInitialAngle(), _useFlip && _actor->startFlipped());
    }

    ITF_INLINE Transform3d getLocalTransform( const Actor* _actor, const bbool _useFlip = btrue )
    {
        return Transform3d(_actor->getBoundLocalPos(), _actor->getBoundLocalAngle(), _useFlip && _actor->isFlipped());
    }

    ITF_INLINE Transform3d getLocalInitTransform( const Actor* _actor, const bbool _useFlip = btrue )
    {
        return Transform3d(_actor->getBoundLocalInitialPos(), _actor->getBoundLocalInitialAngle(), _useFlip && _actor->startFlipped());
    }

};

#endif //_ITF_TRANSFORM2D3D_H_