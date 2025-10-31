#ifndef _ITF_ANIMATIONSKELETON_H_
#define _ITF_ANIMATIONSKELETON_H_


#ifndef _ITF_ANIMATIONBONES_H_
#include    "engine/animation/AnimationBones.h"
#endif // _ITF_ANIMATIONBONES_H_

#ifndef _ITF_ANIMATION_SERIALIZE_H_
#include "engine/animation/AnimationSerialize.h"
#endif //_ITF_ANIMATION_SERIALIZE_H_

namespace ITF
{  
    /////////////////////////////////////////////////////////////////////////////////////
    ///  Component including a bone list, and patches (1st one has no patch) (also called "template")
    class AnimSkeleton : public AnimationSerialize
    {
    public:
        // file flush time
        u64                     m_flushTime;

        VectorAnim<AnimBone>       m_BonesCurFrame;
        VectorAnim<AnimBoneDyn>     m_BonesTPauseDyn;

        SafeArray<AnimBone*>        m_BonesOrdered;
        SafeArray<AnimBone*>        m_BonesMinimumUpdateOrdered;

        // Needed Map
        NamesMap                    m_NameToBone;
        NamesMap                    m_UIDToBone;

        class AnimPolylineBank *    m_PolylineBank;

        AnimBone*               getBoneAt(i32 _index);
        i32                     getBoneIndex(const StringID& _name);
        i32                     getRootIndex();

        void                    ComputeBonesOrder();

#ifndef NO_RUNTIME_LOCK
        void                    SetTPause(SafeArray<AnimBoneDyn> *_dyn);
#else
        void                    SetTPause(SafeArray<AnimBoneDyn> *_dyn);
        void                    SetTPauseCook(SafeArray<AnimBoneDynWithLock> *_dyn);
#endif
        void                    ComputeBonesLocalPos(SafeArray<AnimBoneDyn> *_dyn);
        void                    ComputeIK(SafeArray<AnimBoneDynWithLock> *_dyn, ITF_VECTOR<AnimLock> *_lock);
        void                    ComputeBonesEnd(SafeArray<AnimBoneDyn> *_dyn, bbool processInvert, bbool _minimalBonesUpdate = bfalse);
        void                    ComputePolylineFile();

#ifndef NO_RUNTIME_LOCK
        void                    ComputeBonesGlobalPos(SafeArray<AnimBoneDyn> *_dyn, ITF_VECTOR<AnimLock> *_lock, bbool _minimalBonesUpdate = bfalse);
#else
        void                    ComputeBonesGlobalPos(SafeArray<AnimBoneDyn> *_dyn, bbool _minimalBonesUpdate = bfalse);
        void                    ComputeBonesGlobalPosCook(SafeArray<AnimBoneDynWithLock> *_dyn, ITF_VECTOR<AnimLock> *_lock, bbool _minimalBonesUpdate = bfalse);
#endif

        void                    Translate(SafeArray<AnimBoneDyn> *_dyn, const Vec2d &_move, AnimGlobalData & _globalData, bbool _minimalBonesUpdate = bfalse);
        void                    Scale(SafeArray<AnimBoneDyn> *_dyn, const Vec2d &_scale, AnimGlobalData & _globalData, bbool _minimalBonesUpdate = bfalse);
        void                    Rotate(SafeArray<AnimBoneDyn> *_dyn, f32 _angle, AnimGlobalData & _globalData, bbool _minimalBonesUpdate = bfalse);
        void                    SwapHorizontal(SafeArray<AnimBoneDyn> *_dyn, AnimGlobalData & _globalData, bbool _minimalBonesUpdate = bfalse);

        void                    RotateBone(SafeArray<AnimBoneDyn> *_dyn, i32 _boneIndex, f32 _angle);
        i32                     GetChildrenOf(u32 _index, SafeArray<char> &boneCheckBuffer);

        void                    serialize(ArchiveMemory& _pArchive, bbool _compressedData=bfalse);
        bbool                   getPolyline(StringID _polyID, class AnimPolyline * &_pPolyline);

        virtual void            clear();
        virtual void            postProcess(const String& _szFilename, const String& _szPlatform, bbool _tryLoadingBin);
        void                    ProcessMinimalBonesUpdateList();
        bbool                   isBoneInMinimalBonesList(i32 _index );

        AnimSkeleton();
        virtual ~AnimSkeleton();

        static cookFileCallback m_funcCookerSkeleton;
    };
}

#endif // _ITF_ANIMATIONSKELETON_H_
