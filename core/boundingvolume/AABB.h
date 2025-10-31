#ifndef _ITF_BV_AABB_H_
#define _ITF_BV_AABB_H_

#ifndef _ITF_VEC2D_H_
#include "core/math/Vec2d.h"
#endif //_ITF_VEC2D_H_

#ifndef _ITF_VEC3D_H_
#include "core/math/Vec3d.h"
#endif //_ITF_VEC3D_H_

#ifndef _ITF_MATHTOOLS_H_
#include "core/math/MathTools.h"
#endif //_ITF_MATHTOOLS_H_

#ifndef SERIALIZEROBJECT_H
#include "core/serializer/ZSerializerObject.h"
#endif //SERIALIZEROBJECT_H

#ifndef _ITF_CORE_MACROS_H_
#include "core/Macros.h"
#endif //_ITF_CORE_MACROS_H_

#ifndef _ITF_WARNINGMANAGER_H_
#include "engine/common/WarningManager.h"
#endif //_ITF_WARNINGMANAGER_H_

namespace ITF
{
    class AABB
    {
        DECLARE_SERIALIZE();

    public:
        AABB():m_Min(Vec2d::Zero),m_Max(Vec2d::Zero)    {}

        // From Vec2d
        AABB(const Vec2d& _pos) : m_Min(_pos), m_Max(_pos)
        {
#ifdef ITF_WINDOWS
			ITF_WARNING( NULL, isValid(), "Invalid AABB" );
#endif
		}

        AABB(const Vec2d& _minpos, const Vec2d& _maxpos) : m_Min(_minpos), m_Max(_maxpos)
        {
#ifdef ITF_WINDOWS
			ITF_WARNING( NULL, isValid(), "Invalid AABB" );
#endif
		}
        ///

        // From Vec3d
        AABB(const Vec3d& _pos)
        {
            setMin(_pos);
            setMax(_pos);
        }

        AABB(const Vec3d& _minpos, const Vec3d& _maxpos)
        {
            setMin(_minpos);
            setMax(_maxpos);
            ITF_ASSERT(isValid());
        }
        ///

        // From AABB
        AABB(const AABB& _aabb)
        {
            setMin(_aabb.getMin());
            setMax(_aabb.getMax());
        }
        ///

        ITF_INLINE void grow(const register Vec2d* _pos)
        {
#if defined( ITF_PSI )
			register __vec2x32float__ f_p0,f_p1,f_p2,f_p3,f_p4;
		    register float * pmin = (float*)&this->m_Min.m_x;
		    register float * pmax = (float*)&this->m_Max.m_x;
			asm
			{
				psq_l     f_p0,  0(pmin), 0, 0;    /* fp0 <- pmin */
				psq_l     f_p2,  0(_pos), 0, 0;    /* fp2 <- ppos */
				psq_l     f_p1,  0(pmax), 0, 0;    /* fp1 <- pmax */

				ps_sub    f_p3, f_p0, f_p2;        /* fp4 <- m_min - _pos */
				ps_sub    f_p4, f_p1, f_p2;        /* fp5 <- m_max - _pos */

				ps_sel    f_p0, f_p3, f_p2, f_p0;  /* m_min = (m_min - _pos) >= 0 ? _pos : m_min */
				ps_sel    f_p1, f_p4, f_p1, f_p2;  /* m_max = (m_max - _pos) >= 0 ? m_max : _pos */

				// Store result
				psq_st    f_p0,  0(pmin), 0, 0;
				psq_st    f_p1,  0(pmax), 0, 0;
			}
#else
            m_Min.m_x = Min(_pos->m_x, m_Min.m_x);
            m_Min.m_y = Min(_pos->m_y, m_Min.m_y);

            m_Max.m_x = Max(_pos->m_x, m_Max.m_x);
            m_Max.m_y = Max(_pos->m_y, m_Max.m_y);
#endif
        }
        
        ITF_INLINE void grow(const Vec2d& _pos)
        {
            grow(&_pos);
        }        

#if defined( ITF_WII )
		ITF_INLINE void grow(ps::Float2 PS_PARAM Vector2d)
		{
			ps::Store(ps::Min(ps::Load((float*)&m_Min),Vector2d),(float*)&m_Min);
			ps::Store(ps::Max(ps::Load((float*)&m_Max),Vector2d),(float*)&m_Max);
		}
#endif

        ITF_INLINE void grow(const Vec3d& _pos)
        {
            grow(*(const Vec2d*)&_pos);
        }

        ITF_INLINE void grow(const AABB& _aabb)
        {
#if defined( ITF_PSI )
			register __vec2x32float__ f_p0,f_p1,f_p2,f_p3,f_p4,f_p5;
		    register float * amin = (float*)&this->m_Min.m_x;
		    register float * amax = (float*)&this->m_Max.m_x;
		    const register float * bmin = (float*)&_aabb.m_Min.m_x;
		    const register float * bmax = (float*)&_aabb.m_Max.m_x;
			asm
			{
				// Load a in fr0 and fr1, load b in fr2 and fr3
				psq_l     f_p0,  0(amin), 0, 0;
				psq_l     f_p2,  0(bmin), 0, 0;
				psq_l     f_p1,  0(amax), 0, 0;
				psq_l     f_p3,  0(bmax), 0, 0;

				ps_sub    f_p4, f_p0, f_p2; /* amin - bmin */
				ps_sub    f_p5, f_p1, f_p3; /* amax - bmax */

				ps_sel    f_p0, f_p4, f_p2, f_p0;  /* amin = (amin - bmin) >= 0 ? bmin : amin */
				ps_sel    f_p1, f_p5, f_p1, f_p3;  /* amax = (amax - bmax) >= 0 ? amax : bmax */

				// Store result
				psq_st    f_p0,  0(amin), 0, 0;
				psq_st    f_p1,  0(amax), 0, 0;
			}
#else
            grow(_aabb.getMin());
            grow(_aabb.getMax());
#endif
        }

        ITF_INLINE f32 getHeight()const
        {
            return m_Max.m_y - m_Min.m_y;
        }

        ITF_INLINE f32 getWidth()const
        {
            return m_Max.m_x - m_Min.m_x;
        }

        ITF_INLINE const Vec2d& getMin()const
        {
            return m_Min;
        }

        ITF_INLINE const Vec2d& getMax()const
        {
            return m_Max;
        }

        ITF_INLINE void setMin(const Vec2d& _min)
        {
            m_Min = _min;
        }

        ITF_INLINE void setMax(const Vec2d& _max)
        {
            m_Max = _max;
        }

        ITF_INLINE void setMin(const Vec3d& _min)
        {
            setMin(_min.truncateTo2D());
        }

        ITF_INLINE void setMax(const Vec3d& _max)
        {
            setMax(_max.truncateTo2D());
        }

        ITF_INLINE void moveCenter(const Vec2d& _center)
        {
            Vec2d vHalfDiag;
            Vec2d::Sub(&vHalfDiag, &m_Max, &m_Min);
            Vec2d::Scale(&vHalfDiag, &vHalfDiag, 0.5f);

            Vec2d::Sub(&m_Min, &_center, &vHalfDiag);
            Vec2d::Add(&m_Max, &_center, &vHalfDiag);
        }

        ITF_INLINE Vec2d getMinXMaxY()const
        {
            return Vec2d::mergeXY(&m_Min, &m_Max);
        }

        ITF_INLINE Vec2d getMaxXMinY()const
        {           
            return Vec2d::mergeXY(&m_Max, &m_Min);
        }

        ITF_INLINE Vec2d getCenter() const
        {
            Vec2d center;
            Vec2d::Add(&center, &m_Min, &m_Max);
            Vec2d::Scale(&center, &center, 0.5f);
            return center;
        }

        ITF_INLINE f32 getArea() const
        {
            return getWidth() * getHeight();
        }

        ITF_INLINE bbool contains(const Vec2d& _pos) const
        {
            return m_Min.m_x <= _pos.m_x
                && m_Min.m_y <= _pos.m_y
                && m_Max.m_x >= _pos.m_x
                && m_Max.m_y >= _pos.m_y;
        }

        ITF_INLINE bbool contains(const Vec2d& _pos, f32 _epsilon) const
        {
            return m_Min.m_x <= _pos.m_x +_epsilon
                && m_Min.m_y <= _pos.m_y +_epsilon
                && m_Max.m_x >= _pos.m_x -_epsilon
                && m_Max.m_y >= _pos.m_y -_epsilon;
        }

        ITF_INLINE bbool contains(const Vec3d& _pos) const
        {
            return contains(_pos.truncateTo2D());
        }

        ITF_INLINE bbool contains(const Vec3d& _pos, f32 _epsilon) const
        {
            return contains(_pos.truncateTo2D(), _epsilon );
        }

        ITF_INLINE bbool contains( const AABB& _other) const
        {
            return contains(_other.getMin())
                && contains(_other.getMax());
        }

        ITF_INLINE bbool contains( const AABB& _other, f32 _epsilon) const
        {
            return contains(_other.getMin(), _epsilon )
                && contains(_other.getMax(), _epsilon );
        }

        ITF_INLINE bbool checkOverlap( const AABB& _other) const
        {
            if ( getMin().m_x > (_other.getMax().m_x) ||
                 getMin().m_y > (_other.getMax().m_y) ||
                 getMax().m_x < (_other.getMin().m_x) ||
                 getMax().m_y < (_other.getMin().m_y) )
            {
                return bfalse;
            }

            return btrue;
        }

        ITF_INLINE bbool checkOverlap( const AABB& _other, f32 _epsilon) const
        {
            AABB test(_other.getMin() -_epsilon, _other.getMax() + _epsilon);
            return checkOverlap(test);
        }

        ITF_INLINE void Translate(const Vec2d & _trans)
        {
            Vec2d::Add(&m_Min, &m_Min, &_trans);
            Vec2d::Add(&m_Max, &m_Max, &_trans);
        }

        ITF_INLINE void Scale(const Vec2d & _scale)
        {
            Vec2d::Mul(&m_Min, &m_Min, &_scale);
            Vec2d::Mul(&m_Max, &m_Max, &_scale);

            // check negative scales
            Vec2d diff = m_Max - m_Min;
            Vec2d tMin = m_Min;
            Vec2d::Sel(&m_Min, &diff, &m_Min, &m_Max);
            Vec2d::Sel(&m_Max, &diff, &m_Max, &tMin);
        }

		ITF_INLINE void ScaleFromCenter(const Vec2d & _scale)
        {
            Vec2d center = getCenter();
            moveCenter(Vec2d::Zero);
            Scale(_scale);
            moveCenter(center);
        }

        ITF_INLINE void FlipHorizontaly(bbool _aroundCenter)
        {
            f32 centerAxe = _aroundCenter ? (m_Min.m_x + m_Max.m_x) * 0.5f : 0.f;
            const f32 swapVal = m_Min.m_x;
            m_Min.m_x = centerAxe - (m_Max.m_x - centerAxe);
            m_Max.m_x = centerAxe - (swapVal   - centerAxe);
        }

        ITF_INLINE void FlipVerticaly(bbool _aroundCenter)
        {
            f32 centerAxe = _aroundCenter ? (m_Min.m_y + m_Max.m_y) * 0.5f : 0.f;
            const f32 swapVal = m_Min.m_y;
            m_Min.m_y = centerAxe - (m_Max.m_y - centerAxe);
            m_Max.m_y = centerAxe - (swapVal   - centerAxe);
        }

        void RotateAround(f32 _angle, const Vec2d &center);
        void Rotate(f32 _angle);

        ITF_INLINE void Rotate(f32 _angle, bbool _aroundCenter)
        {
            if (_aroundCenter)
                RotateAround(_angle, getCenter());
            else
                Rotate(_angle);
        }

        ITF_INLINE void projectTo2D(f32 _deltaX,f32 _deltaY,f32 _zSrc, f32 _fDst = 0.0f)
        {
            const f32 fDeltaZ = _fDst - _zSrc;
            if (fDeltaZ < MTH_EPSILON)
                return;

            Vec2d delta(_deltaX, _deltaY);
            Vec2d::Scale(&delta, &delta, fDeltaZ);
            Vec2d::Sub(&m_Min, &m_Min, &delta);
            Vec2d::Add(&m_Max, &m_Max, &delta);
        }

        ITF_INLINE f32 getDistMinPosAABB (const Vec2d& _pos) const
        {
            Vec2d dist, halfsize, center;

            Vec2d::Add(&center, &m_Min, &m_Max);
            Vec2d::Sub(&halfsize, &m_Max, &m_Min);
            Vec2d::Scale(&center, &center, 0.5f);
            Vec2d::Scale(&halfsize, &halfsize, 0.5f);

            Vec2d::Sub(&dist, &_pos, &center);
            Vec2d::Abs(&dist, &dist);
            Vec2d::Sub(&dist, &dist, &halfsize);
            Vec2d::ClampMin0(&dist, &dist);
            return Max(dist.m_x, dist.m_y);
        }

        bbool isValid()const
        {
            return
                (m_Min.m_x <= m_Max.m_x)
                &&
                (m_Min.m_y <= m_Max.m_y)
            ;
        }


        void invalidate()
        {
            m_Min.m_x = m_Min.m_y = FLT_MAX; m_Max.m_x = m_Max.m_y = -FLT_MAX; //By using FLT_MAX, you can use invalidate as an empty, growable box
        }

        bbool isZero()const
        {
            return
                (m_Min == Vec2d::Zero)
                &&
                (m_Max == Vec2d::Zero)
                ;
        }

        void serialize(ArchiveMemory& _archive)
        {
            m_Min.serialize(_archive);
            m_Max.serialize(_archive);
        }

    private:
        Vec2d m_Min;
        Vec2d m_Max;

    };


    class Margin
    {
        DECLARE_SERIALIZE();
    public:

        Margin()
            : m_left(0)
            , m_right(0)
            , m_top(0)
            , m_bottom(0)
        {}

        Margin( f32 _left, f32 _right, f32 _top, f32 _bottom )
            : m_left(_left)
            , m_right(_right)
            , m_top(_top)
            , m_bottom(_bottom)
        {}

        f32 m_left;
        f32 m_right;
        f32 m_top;
        f32 m_bottom;

        bbool isZero()const
        {
            return m_left == 0 &&
                   m_right == 0 &&
                   m_top == 0 &&
                   m_bottom == 0;
        }

        void grow(const Margin &_m2);

        void scale(f32 _x, f32 _y);

        void addTo(AABB& _aabb) const
        {
            _aabb.setMin(_aabb.getMin() - Vec2d(m_left, m_bottom));
            _aabb.setMax(_aabb.getMax() + Vec2d(m_right, m_top));
            ITF_ASSERT(_aabb.isValid());
        }
    };

    ITF_INLINE void Margin::grow(const Margin &_m2)
    {
        Vec2d::Add((Vec2d*)&((f32*)(this))[0], (Vec2d*)&((f32*)(this))[0], (Vec2d*)&((f32*)(&_m2))[0]);
        Vec2d::Add((Vec2d*)&((f32*)(this))[2], (Vec2d*)&((f32*)(this))[2], (Vec2d*)&((f32*)(&_m2))[2]);
    }

    ITF_INLINE void Margin::scale(f32 _x, f32 _y)
    {
#if defined( ITF_PSI )
        Vec2d::Scale((Vec2d*)&((f32*)(this))[0], (Vec2d*)&((f32*)(this))[0], _x);
        Vec2d::Scale((Vec2d*)&((f32*)(this))[2], (Vec2d*)&((f32*)(this))[2], _y);
#else
        m_left *= _x;
        m_right *= _x;
        m_top *= _y;
        m_bottom *= _y;
#endif
    }

} // namespace ITF

#endif // _ITF_BV_AABB_H_
