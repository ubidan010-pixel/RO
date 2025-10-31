#ifndef _ITF_ANIMATIONPATCHBANK_H_
#define _ITF_ANIMATIONPATCHBANK_H_

#ifndef _ITF_ANIMATIONBONES_H_
#include    "engine/animation/AnimationBones.h"
#endif // _ITF_ANIMATIONBONES_H_

#ifndef _ITF_ANIMATION_SERIALIZE_H_
#include "engine/animation/AnimationSerialize.h"
#endif //_ITF_ANIMATION_SERIALIZE_H_

#ifndef _ITF_RESOURCE_H_   
#include "engine/resources/Resource.h"
#endif //_ITF_RESOURCE_H_   

namespace ITF
{  
    class AABB;
    class Texture;
    class AnimBone;
    class AnimPatchBank;

#define MAX_PATCH_POINTS_IN_PATCH   4

    // Use this this define to set only on local ref for animation
#define ITF_UNIQUE_PATCH_POINT_REF


    class AnimPatchPointLocal
    {
    public:

        AnimPatchPointLocal(): m_Pos(Vec2d::Zero),m_Normale(Vec2d::Zero),m_BonePtr(NULL) {}
        AnimBone    *m_BonePtr;

        Vec2d       m_Pos;
        Vec2d       m_Normale;
#ifndef ITF_UNIQUE_PATCH_POINT_REF
        f32         m_Influence;
#endif
        void        serialize(ArchiveMemory& _archive);
    };

    class AnimPatchPointDyn
    {
    public:

        AnimPatchPointDyn():m_Pos(Vec2d::Zero),m_Normale(Vec2d::Zero) {}//TODO CHECK YOUS

        Vec2d                       m_Pos;
        Vec2d                       m_Normale;

        f32                         m_Zorder;
        f32                         m_Alpha;
#ifdef ANIMUSEGEOMETRYSHADER
        u8                          m_boneIdx;
#endif
        bbool                       m_Mirror;
    };

    class AnimPatchPoint
    {
    public:

        AnimPatchPoint():m_PosUV(Vec2d::Zero),m_NormaleUV(Vec2d::Zero),m_Index(0) {}//TODO CHECK YOUS

        u32                         m_Index;
        Vec2d                       m_PosUV;
        Vec2d                       m_NormaleUV;

#ifdef ITF_UNIQUE_PATCH_POINT_REF
        AnimPatchPointLocal         m_mainRefs;
#else
        VectorAnim<AnimPatchPointLocal> m_LocalRefs;
#endif
        void     serialize(ArchiveMemory& _archive);
    };


    class AnimPatch
    {
    public:
        AnimPatchBank   *m_PatchBankPtr;
        u32              m_TemplateIdx;

        u8              m_nbPatchPoints;
        AnimPatchPoint  *m_AnimPatchPoints[MAX_PATCH_POINTS_IN_PATCH]; //

        void            serialize(ArchiveMemory& _pArchive);

        AnimPatch();
        ~AnimPatch();
    };
    VECTAGSET(AnimPatch, ITF::MemoryId::mId_VectorAnimation)

    class AnimTemplate
    {
    public:
        VectorAnim<AnimBone>        m_BonesCurFrame;
        VectorAnim<AnimBoneDyn>     m_BonesCurFrameDyn; //initially set to poseT and after used for repositioning

        // Needed Map
        NamesMap                m_NameToBone;

        VectorAnim<AnimPatchPoint>  m_PatchPoints;
        VectorAnim<AnimPatch>       m_Patch;

        f32                     m_lenghtFactor;

        ITF_INLINE f32          GetLenghtFactor() { return m_lenghtFactor; }
        bbool                   ComputeBonesInfo();
        bbool                   ComputePatchs(AnimPatchBank *_patchBank, u32 _index);
#ifndef ANIMATION_FIX_ABB
        bbool                   ComputePatchPointGlobalPos(SafeArray<AnimBoneDyn> *_dyn, AnimPatchPointDyn *_ppDyn, const AnimGlobalData & _globalData, AABB &patchBounding, SafeArray<AnimBoneRenderable> *_finalBoneList);
        bbool                   ComputePatchPointGlobalPosRatio(SafeArray<AnimBoneDyn> *_dyn, AnimPatchPointDyn *_ppDyn, const AnimGlobalData & _globalData, AABB &patchBounding);
        bbool                   ComputePatchPointGlobalPosMatrix(SafeArray<PatchPointBoneMatrix> *_dyn, AnimPatchPointDyn *_ppDyn, const AnimGlobalData & _globalData, AABB &patchBounding, SafeArray<AnimBoneRenderable> *_finalBoneList);
        bbool                   ComputePatchPointGlobalPosMatrixRatio(SafeArray<PatchPointBoneMatrix> *_dyn, AnimPatchPointDyn *_ppDyn, const AnimGlobalData & _globalData, AABB &patchBounding);
#else
        bbool                   ComputePatchPointGlobalPos(SafeArray<AnimBoneDyn> *_dyn, AnimPatchPointDyn *_ppDyn, const AnimGlobalData & _globalData, SafeArray<AnimBoneRenderable> *_finalBoneList);
        bbool                   ComputePatchPointGlobalPosRatio(SafeArray<AnimBoneDyn> *_dyn, AnimPatchPointDyn *_ppDyn, const AnimGlobalData & _globalData);
        bbool                   ComputePatchPointGlobalPosMatrix(SafeArray<PatchPointBoneMatrix> *_dyn, AnimPatchPointDyn *_ppDyn, const AnimGlobalData & _globalData, SafeArray<AnimBoneRenderable> *_finalBoneList);
        bbool                   ComputePatchPointGlobalPosMatrixRatio(SafeArray<PatchPointBoneMatrix> *_dyn, AnimPatchPointDyn *_ppDyn, const AnimGlobalData & _globalData);
#endif
        void                    ComputeBonesGlobalPos();
        void                    ComputeBonesLocalPos(bbool _visualRootOnly);

#ifndef ITF_SUPPORT_MULTITHREADED
        void                    ComputeBoneInMainSkeleton(class AnimSkeleton *_skeleton);
        ITF_INLINE bbool        IsBoneComputedInMainSkeleton(class AnimSkeleton *_skeleton) { return m_computedSkeleton == _skeleton;  }
        ITF_INLINE bbool        IsBoneComputedOK() { return m_computeBonesOK;  }
#endif
        //computed area

        void serialize(ArchiveMemory& _pArchive);
        void link(AnimPatchBank *_animPatchBank);

        AnimTemplate();
        virtual ~AnimTemplate();

    private:
        void                    clearPatchData();
        void                    computeBoneOrder(SafeArray<AnimBone *> & _boneOrder);
        class AnimSkeleton *    m_computedSkeleton;
        bbool                   m_computeBonesOK;
    };


    class AnimPatchBank : public AnimationSerialize
    {
    public:
        f32                     m_Ratio;

        ResourceID              m_texture;         // outside texture

        ArchiveLinker*          m_archiveLinker;

        VectorAnim<AnimTemplate*> m_templateList;
        NamesMap                m_NameToTemplate;

        bbool                   ComputeBonesInfo();
        bbool                   ComputePatchs();

        void                    serialize(ArchiveMemory& _pArchive, bbool _compressedData=bfalse);
        void                    link();
        virtual void            clear();
        virtual void            postProcess(const String& _szFilename, const String& _szPlatform, bbool _tryLoadingBin);

        AnimPatchBank();
        virtual ~AnimPatchBank();

        static cookFileCallback m_funcCookerPatchBank;
    };
}

#endif // _ITF_ANIMATIONPATCHBANK_H_
