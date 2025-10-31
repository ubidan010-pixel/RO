#ifndef _ITF_ANIMATIONPOLYLINE_H_
#define _ITF_ANIMATIONPOLYLINE_H_

#ifndef _ITF_ANIMATIONBONES_H_
#include    "engine/animation/AnimationBones.h"
#endif // _ITF_ANIMATIONBONES_H_

#ifndef _ITF_ANIMATION_SERIALIZE_H_
#include "engine/animation/AnimationSerialize.h"
#endif //_ITF_ANIMATION_SERIALIZE_H_

namespace ITF
{
    class AnimPolylinePoint
    {
    public:
        Vec2d       m_PosLocal;

        StringID    m_BoneUID;
        AnimBone    *m_BonePtr;

        AnimPolylinePoint() : m_BonePtr(NULL) {}

        Vec2d ComputeGlobalPos(SafeArray<AnimBoneDyn> *_dyn, const AnimGlobalData & _globalData);
        Vec2d ComputeRelativePos(Vec2d _scale, const AnimGlobalData & _globalData);
        void serialize(ArchiveMemory& _pArchive);

    private:
        Vec2d ComputeGlobalPosInternally(AnimBoneDyn *_boneDyn, const AnimGlobalData & _globalData);
    };
    VECTAGSET(AnimPolylinePoint, ITF::MemoryId::mId_VectorAnimation)

    class AnimPolyline
    {
    public:
        enum PolylineType
        {
            shape_error     = 0,
            shape_polyline  = 1, // Begin at 1 to match with editor
            shape_circle    = 2
        };
        
        VectorAnim<AnimPolylinePoint>   m_PolylinePointList;
        NamesMap                    m_NameToPolylinePoint;
        
        StringID                    m_Name;
        bbool                       m_isLooping;
        PolylineType                m_type;

        AnimPolyline() : m_isLooping(bfalse), m_type(shape_polyline) {}

        bbool   resolvePtr(class AnimSkeleton *_skeleton);
        void    ComputeGlobalPos(SafeArray<AnimBoneDyn> *_dyn, Vec2d* _posList, const AnimGlobalData & _globalData);
        void    ComputeRelativePos(Vec2d* _posList, Vec2d _scale, const AnimGlobalData & _globalData);
        
        void serialize(ArchiveMemory& _pArchive);
    };
    VECTAGSET(AnimPolyline, ITF::MemoryId::mId_VectorAnimation)

    class AnimPolylineBank : public AnimationSerialize
    {
    public:
        AnimPolylineBank();
        
        VectorAnim<AnimPolyline>    m_PolylineList;
        NamesMap                    m_NameToPolyline;

        bbool           resolvePtr(class AnimSkeleton *_skeleton);
        void            serialize(ArchiveMemory& _pArchive, bbool _compressedData=bfalse);

        virtual void    clear() {}
    };
}


#endif //_ITF_ANIMATIONPOLYLINE_H_