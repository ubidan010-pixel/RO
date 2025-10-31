#ifndef _ITF_ANIMATIONBONES_H_
#define _ITF_ANIMATIONBONES_H_

#ifndef _ITF_KEYARRAY_H_
#include "core/container/KeyArray.h"
#endif //_ITF_KEYARRAY_H_

#ifndef _ITF_RESOURCE_H_   
#include "engine/resources/Resource.h"
#endif //_ITF_RESOURCE_H_   

#define NO_COMPUTECOSSIN
#define NO_RUNTIME_LOCK
#define ANIM_TRACK_CACHE
#define ANIMATION_FIX_ABB

namespace ITF
{  
#define BONE_NO_BEGIN               0x1
#define BONE_NO_END                 0x2
#define BONE_PARENT_CUT             0x4
#define BONE_MINIMAL_COMPUTE        0x8

    class AnimPatchBank;
    class AnimPatchPoint;
    class ArchiveMemory;

    ITF_INLINE Vec2d        Turn90(const Vec2d& _v)
    {
        Vec2d ret;
        Vec2d::SwapmXY(&ret, &_v);
        return ret;
    }
    ITF_INLINE Vec2d        Turn270(const Vec2d& _v)
    {
        Vec2d ret;
        Vec2d::SwapXmY(&ret, &_v);
        return ret;
    }

    ITF_INLINE Vec2d        AddAngleBTW(const Vec2d& _v, f32 _angle)
    {
        Vec2d ComplexVect;
        Vec2d::CosSin(&ComplexVect, _angle);
        return Vec2d(_v.dot(ComplexVect), Turn90(_v).dot(ComplexVect));        
    }

    ITF_INLINE void FillAngleBTW(Vec2d & _dst, const Vec2d& _v, f32 _angle)
    {
        Vec2d ComplexVect;
        Vec2d::CosSin(&ComplexVect, _angle);
        _dst.m_x = _v.dot(ComplexVect);
        _dst.m_y = Turn90(_v).dot(ComplexVect);        
    }

    ITF_INLINE void FillAngleBTWCosSin(Vec2d & _dst, const Vec2d& _v, const Vec2d & _cosSin)
    {
        _dst.m_x = _v.dot(_cosSin);
        _dst.m_y = Turn90(_v).dot(_cosSin);        
    }



    ///////////////////////////////////////////////
    //Location part of the bone (is included in each instance of same character)
    class AnimBoneDyn
    {
    public:
        Vec2d           m_Pos;
        Vec2d           m_PosEnd;
        Vec2d           m_CosSin;
        Vec2d           m_PosLocal;
        Vec2d           m_Scale;

#ifndef NO_RUNTIME_LOCK
        u32             m_LockedIndex; // if the bone is locked give index on lock list, else, -1
#endif

        f32             m_Angle;
        f32             m_Length;

#ifndef NO_COMPUTECOSSIN
        f32             m_CosSinAngle;
#endif
        f32             m_AngleLocal;
        f32             m_Alpha;

        f32             m_Zorder;


        ITF_INLINE f32          GetScaledLength()   const           { return m_Length * m_Scale.m_x; }
        ITF_INLINE Vec2d        GetLocalEnd()       const;
        #ifndef NO_COMPUTECOSSIN
        ITF_INLINE void         ComputeCosSin()
        { 
            //if (f32_Abs(m_CosSinAngle - m_Angle) > MTH_EPSILON)
            if(m_CosSinAngle != m_Angle)
            {
                Vec2d::CosSinOpt(&m_CosSin, m_Angle);
                m_CosSinAngle = m_Angle;
            }
        }
        #else
        ITF_INLINE void         ComputeCosSin() const {}
        #endif
             
        ITF_INLINE void         ComputeEnd()
        {
            ComputeCosSin();
            FillAngleBTWCosSin(m_PosEnd, Vec2d(GetScaledLength(), 0.f), m_CosSin);
            m_PosEnd += m_Pos;
        }


        void            serialize(ArchiveMemory& _archive);    
        void            ComputeLocal(const AnimBoneDyn *_BoneParent);
        void            ComputeGlobal();
#ifndef NO_RUNTIME_LOCK
        void            ComputeGlobal(AnimBoneDyn *_BoneParent, const f32 *_lockAngle);
#else
        void            ComputeGlobal(AnimBoneDyn *_BoneParent);
#endif //NO_RUNTIME_LOCK

        Vec2d           getDirection() const;
        void            setDirection(const Vec2d& _direction);
        // get current angle in world-space (m_Angle is actor-relative --I think?!)
        f32             getAngle() const;

        AnimBoneDyn();
    };
    VECTAGSET(AnimBoneDyn, ITF::MemoryId::mId_SafeArrayCache)

#ifdef NO_RUNTIME_LOCK
    class AnimBoneDynWithLock : public AnimBoneDyn
    {
    public:
        AnimBoneDynWithLock() : AnimBoneDyn(), m_LockedIndex(U32_INVALID) {}
        u32             m_LockedIndex; // if the bone is locked give index on lock list, else, -1

        void            ComputeGlobalWithLock(AnimBoneDyn *_BoneParent, const f32 *_lockAngle);
    };
#else
    typedef AnimBoneDyn AnimBoneDynWithLock;
#endif //NO_RUNTIME_LOCK

    ITF_INLINE Vec2d AnimBoneDyn::GetLocalEnd() const
    {
#ifndef NO_COMPUTECOSSIN
        Vec2d CS;
        f32 len = m_Length * m_Scale.m_x;
        Vec2d::CosSinOpt(&CS, m_Angle);
        return Vec2d(len*CS.m_x, -len*CS.m_y);
#else
        f32 len = m_Length * m_Scale.m_x;
        Vec2d ret;
        Vec2d::NegY(&ret, &m_CosSin);
        ret *= len;
        return ret;
#endif
    }

    ITF_INLINE void AnimBoneDyn::ComputeLocal(const AnimBoneDyn *_BoneParent)
    {
        if (!_BoneParent)
        {
            m_PosLocal		= m_Pos;
            m_AngleLocal    = m_Angle;
        } else
        {
            // OLD
            // m_PosLocal		= AddAngleBTW(m_Pos - _BoneParent->m_Pos, -_BoneParent->m_Angle) - Vec2d(_BoneParent->GetScaledLength(),0);
            // NEW
            m_PosLocal		= AddAngleBTW(m_Pos - _BoneParent->m_Pos, -_BoneParent->m_Angle);
            if (f32_Abs(_BoneParent->m_Scale.m_x) > MTH_EPSILON)
            {
                m_PosLocal.m_x    /= _BoneParent->m_Scale.m_x;
            } else
            {
                m_PosLocal.m_x     = 0.f;
            }
            if (f32_Abs(_BoneParent->m_Scale.m_y) > MTH_EPSILON)
            {
                m_PosLocal.m_y    /= _BoneParent->m_Scale.m_y;
            } else
            {
                m_PosLocal.m_y     = 0.f;
            }
            m_PosLocal	   -= Vec2d(_BoneParent->m_Length,0);
            m_AngleLocal	= m_Angle - _BoneParent->m_Angle;
        }
    }

    ITF_INLINE void AnimBoneDyn::ComputeGlobal()
    {
        m_Pos		= m_PosLocal;
        m_Angle	    = m_AngleLocal;
#ifdef NO_COMPUTECOSSIN
        Vec2d::CosSinOpt(&m_CosSin, m_Angle);
#endif
    }

#ifndef NO_RUNTIME_LOCK
    ITF_INLINE void AnimBoneDyn::ComputeGlobal(AnimBoneDyn *_BoneParent, const f32 *_lockAngle)
#else
    ITF_INLINE void AnimBoneDynWithLock::ComputeGlobalWithLock(AnimBoneDyn *_BoneParent, const f32 *_lockAngle)
#endif //NO_RUNTIME_LOCK
    {
        ITF_ASSERT(_BoneParent);
        // OLD
        //m_Pos		= _BoneParent->m_Pos + AddAngleBTW(Vec2d(_BoneParent->GetScaledLength(),0) + m_PosLocal, _BoneParent->m_Angle);
        // NEW
        //m_Pos		= _BoneParent->m_Pos + AddAngleBTW((Vec2d(_BoneParent->m_Length,0) + m_PosLocal)*_BoneParent->m_Scale, _BoneParent->m_Angle);

        _BoneParent->ComputeCosSin();
        Vec2d tmp0(m_PosLocal.m_x + _BoneParent->m_Length, m_PosLocal.m_y);
        Vec2d::Mul(&tmp0, &tmp0, &_BoneParent->m_Scale);
        
        Vec2d tmp1,tmp2;
        Vec2d::SwapmXY(&tmp1, &tmp0); // y, -x
        tmp2.m_x = Vec2d::Dot(&tmp0, &_BoneParent->m_CosSin);
        tmp2.m_y = Vec2d::Dot(&tmp1, &_BoneParent->m_CosSin);
        
        Vec2d::Add(&m_Pos, &_BoneParent->m_Pos, &tmp2);

        if (_lockAngle)
            m_Angle = *_lockAngle + m_AngleLocal;
        else
            m_Angle = _BoneParent->m_Angle + m_AngleLocal;
#ifdef NO_COMPUTECOSSIN
        Vec2d::CosSinOpt(&m_CosSin, m_Angle);
#endif // NO_COMPUTECOSSIN
    }

#ifdef NO_RUNTIME_LOCK
    ITF_INLINE void AnimBoneDyn::ComputeGlobal(AnimBoneDyn *_BoneParent)
    {
        ITF_ASSERT(_BoneParent);
        // OLD
        //m_Pos		= _BoneParent->m_Pos + AddAngleBTW(Vec2d(_BoneParent->GetScaledLength(),0) + m_PosLocal, _BoneParent->m_Angle);
        // NEW
        //m_Pos		= _BoneParent->m_Pos + AddAngleBTW((Vec2d(_BoneParent->m_Length,0) + m_PosLocal)*_BoneParent->m_Scale, _BoneParent->m_Angle);

        _BoneParent->ComputeCosSin();
        Vec2d tmp0(m_PosLocal.m_x + _BoneParent->m_Length, m_PosLocal.m_y);
        Vec2d::Mul(&tmp0, &tmp0, &_BoneParent->m_Scale);

        Vec2d tmp1,tmp2;
        Vec2d::SwapmXY(&tmp1, &tmp0); // y, -x
        tmp2.m_x = Vec2d::Dot(&tmp0, &_BoneParent->m_CosSin);
        tmp2.m_y = Vec2d::Dot(&tmp1, &_BoneParent->m_CosSin);

        Vec2d::Add(&m_Pos, &_BoneParent->m_Pos, &tmp2);
        m_Angle = _BoneParent->m_Angle + m_AngleLocal;
#ifdef NO_COMPUTECOSSIN
        Vec2d::CosSinOpt(&m_CosSin, m_Angle);
#endif // NO_COMPUTECOSSIN
    }
#endif //NO_RUNTIME_LOCK

    ITF_INLINE Vec2d AnimBoneDyn::getDirection() const
    {
        return (m_PosEnd - m_Pos).normalize();
    }

    ITF_INLINE void AnimBoneDyn::setDirection(const Vec2d& _direction)
    {
        Vec2d::Sub(&m_PosEnd, &m_PosEnd, &m_Pos);
        f32 length = m_PosEnd.norm();
        Vec2d::ScaleAdd(&m_PosEnd, &_direction, length, &m_Pos);
    }

    ITF_INLINE f32 AnimBoneDyn::getAngle() const
    {
        return getDirection().getOrientedAngle(Vec2d::Right);
    }

    ///////////////////////////////////////////////
    //Location part of the bone (is included in each instance of same character)
    struct AnimBoneRenderable
    {
        AnimBoneRenderable():m_Pos(Vec2d::Zero),m_PosEnd(Vec2d::Zero) {} //TODO check Yous
        Vec2d           m_Pos;
        Vec2d           m_PosEnd;
        f32             m_ScaleY;
        f32             m_ScaledLenthInv;
        f32             m_Alpha;
        f32             m_Mirror;

        void setBone(AnimBoneDyn& _bone)
        {
            m_Pos = _bone.m_Pos;
            m_PosEnd = _bone.m_PosEnd;
            m_ScaleY = _bone.m_Scale.m_y;
            m_ScaledLenthInv = f32_Inv(_bone.GetScaledLength());
            m_Alpha = _bone.m_Alpha;
            m_Mirror = _bone.m_Scale.m_x * _bone.m_Scale.m_y >= 0.f ? 1.0f : -1.0f;
        }
    };

    //////////////////////////////////////////////////////////////////////////////
    ///static part of a bone (hierarchy). Not duplicated in instances
    class AnimBone
    {
    public:
        AnimBone():
          m_BoneParent(0)
        , m_PatchFlags(0)
        , m_index(0)
        , m_OrderedIndex(0)
        {};

        StringID        m_Name;

        u8              m_PatchFlags;
        AnimBone*       m_BoneParent;

        void            serialize(ArchiveMemory& _archive);
        void            link(AnimPatchBank *_animPatchBank);

        VectorAnim<AnimPatchPoint *>    m_PatchPointsPtr;	//Source information as loaded from model for this bone

        //computed area
        i32             m_OrderedIndex;
        ITF_INLINE void setIndex(i32 _index) {ITF_ASSERT(_index>=0);m_index  =_index;}
        ITF_INLINE i32  getIndex() const {return m_index;}

        static  void    restoreParentLink(VectorAnim<AnimBone> *_animBoneList, ArchiveMemory& _Archive);

    private:
        i32             m_index;
    };
    VECTAGSET(AnimBone, ITF::MemoryId::mId_VectorAnimation)

    class AnimLock
    {
    public:

        AnimLock() :m_Locked(Vec2d::Zero) {};//TODO check Yous
        union
        {
            u32 m_serializeField;
            struct {
                u32 m_AngleSign : 2,
                    m_Index         : 15,
                    m_OrderedIndex  : 15;
            } m_details;
        };

        f32             m_LockedAngle;      // locked angle
        Vec2d           m_Locked;           // locked point

        void serialize(ArchiveMemory& _archive);
    };
    VECTAGSET(AnimLock, ITF::MemoryId::mId_VectorAnimation)

    struct AnimGlobalData
    {
        bbool       m_mirror;
        bbool       m_isRatioOne;
        bbool       m_processDone;
        f32         m_ratio;
        Vec2d       m_origin;
        Vec2d       m_xAxis;
        Vec2d       m_yAxis;

        AnimGlobalData() { clear(); }
        void clear()
        {
            m_mirror        = bfalse;
            m_origin        = Vec2d::Zero;
            m_xAxis         = Vec2d::Right;
            m_yAxis         = Vec2d::Up;
            m_ratio         = 1.f;
            m_isRatioOne    = btrue;
            m_processDone   = bfalse;
        }

        void process()
        {
            if (m_processDone)
                return;

            Vec2d   xAxis, yAxis;
            Vec2d::Sub(&xAxis, &m_xAxis, &m_origin);
            Vec2d::Sub(&yAxis, &m_yAxis, &m_origin);
            f32     xNorm = Vec2d::Normalize(&m_xAxis, &xAxis);
            f32     yNorm = Vec2d::Normalize(&m_yAxis, &yAxis);
            m_ratio = xNorm/yNorm;

            //m_isRatioOne    = f32_Abs(m_ratio - 1.f) < MTH_EPSILON;
            // For optimization purpose, change ratio limit
            m_isRatioOne    = f32_Abs(m_ratio - 1.f) < 1.e-3f;
            m_processDone   = btrue;
        }

    };
    VECTAGSET(AnimGlobalData, ITF::MemoryId::mId_VectorAnimation)

    typedef KeyArray<i32> NamesMap;
    typedef KeyArray<ResourceIdentifierType> NamesObjectRefMap;


    #define ANIM_TRACK_CACHE_SIZE 3
    struct AnimTrackBonePASCut
    {
        f32                     m_frame;
        f32                     m_AngleLocal;

        Vec2d                   m_PosLocal;
        Vec2d                   m_Scale;

		u32						OriginalTrackIndex;
    };


    struct PatchPointBoneMatrix
    {
        Vec2d   m_translation;
        Vec2d   m_dir;
        Vec2d   m_perpendicular;

        f32     m_ratio;
        f32     m_zOrder;
        f32     m_alpha;
        bbool   m_mirror;

#ifdef ITF_WII
		f32		m_padding[6];
#endif
	};

};

#endif // _ITF_ANIMATIONBONES_H_
