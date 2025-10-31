#include "precompiled_engine.h"

#ifndef _ITF_ANIMATIONPATCHBANK_H_
#include    "engine/animation/AnimationPatchBank.h"
#endif // _ITF_ANIMATIONPATCHBANK_H_

#ifndef _ITF_VERSIONING_H_
#include "core/versioning.h"
#endif  //_ITF_VERSIONING_H_

#ifndef _ITF_ARCHIVELINKER_H_
#include "core/ArchiveLinker.h"
#endif //_ITF_ARCHIVELINKER_H_

#ifdef ITF_WII
#ifndef __MEM_MACROS_WII_H__
#include "core/memory/WII/MEM_macros_WII.h"
#endif //__MEM_MACROS_WII_H__
#endif

#ifndef _ITF_ANIMATIONSKELETON_H_
#include    "engine/animation/AnimationSkeleton.h"
#endif // _ITF_ANIMATIONSKELETON_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

namespace ITF
{
    cookFileCallback AnimPatchBank::m_funcCookerPatchBank = NULL;

    bool PrefetchPatchBank( const Path& path, PrefetchFactory::Params& params )
    {
        return ResourceManager::PrefetchResource( path, params, Resource::ResourceType_AnimPatchBank );
    }

    void AnimPatchPointLocal::serialize(ArchiveMemory& _archive)
    {
        _archive.serializePtr(reinterpret_cast<uPtr&>(m_BonePtr));

        m_Pos.serialize(_archive);
        m_Normale.serialize(_archive);
#ifndef ITF_UNIQUE_PATCH_POINT_REF
        _archive.serialize(m_Influence);
#endif
    }


    void AnimPatchPoint::serialize(ArchiveMemory& _archive)
    {
        _archive.serializeLink(reinterpret_cast<uPtr>(this));

        _archive.serialize(m_Index);
        m_PosUV.serialize(_archive);
        m_NormaleUV.serialize(_archive);

#ifndef ITF_UNIQUE_PATCH_POINT_REF
        m_LocalRefs.serialize(_archive); 
#else
        m_mainRefs.serialize(_archive); 
#endif
    }


    AnimPatch::AnimPatch()
    {
        ITF_MemSet(this, 0, sizeof(*this));
    }

    AnimPatch::~AnimPatch()
    {
    }

    void AnimPatch::serialize(ArchiveMemory& _pArchive)
    {
        _pArchive.serialize((void *&)m_PatchBankPtr);
        _pArchive.serialize(m_TemplateIdx);

        _pArchive.serialize(m_nbPatchPoints);
        for (u32 index =0;index<m_nbPatchPoints;index++)
        {
            _pArchive.serialize((void *&)m_AnimPatchPoints[index]);
        }
    }

    AnimTemplate::AnimTemplate()
    : m_lenghtFactor(1.f)
    , m_computedSkeleton(NULL)
    , m_computeBonesOK(bfalse)
    {
    }

    AnimTemplate::~AnimTemplate()
    {
        clearPatchData();
    }

    void AnimTemplate::serialize(ArchiveMemory& _pArchive)
    {
        //resource..
        m_NameToBone.serialize(_pArchive);
        _pArchive.serialize(m_lenghtFactor);

		m_BonesCurFrame.serialize(_pArchive);
        AnimBone::restoreParentLink(&m_BonesCurFrame, _pArchive);
        m_BonesCurFrameDyn.serialize(_pArchive);
		m_PatchPoints.serialize(_pArchive);
        m_Patch.serialize(_pArchive);
    }


    void AnimTemplate::link(AnimPatchBank *_animPatchBank)
    {
        for ( VectorAnim<AnimBone> ::iterator iter = m_BonesCurFrame.begin();iter!=m_BonesCurFrame.end();++iter)
        {
            (*iter).link(_animPatchBank);
        }
        
        ArchiveLinker *archiveLinker = _animPatchBank->m_archiveLinker;
        for (VectorAnim<AnimPatch>::iterator iter = m_Patch.begin(); iter != m_Patch.end(); ++iter)
        {
            AnimPatch* patch = &(*iter);
            for (u32 i=0; i<patch->m_nbPatchPoints; i++)
            {
                patch->m_AnimPatchPoints[i] = (AnimPatchPoint *)(archiveLinker->getLink(uPtr(patch->m_AnimPatchPoints[i])));
            }
            patch->m_PatchBankPtr     = (AnimPatchBank *)(archiveLinker->getLink(uPtr(patch->m_PatchBankPtr)));
        }
    }

    void AnimTemplate::clearPatchData()
    {
        m_Patch.clear();
    }

    bbool AnimTemplate::ComputeBonesInfo()
    {
        VectorAnim<AnimBone>::iterator it = m_BonesCurFrame.begin();
        for (u32 pos = 0; it != m_BonesCurFrame.end(); ++it, ++pos)
        {
            (*it).setIndex(pos);
            (*it).m_OrderedIndex = -1;
        }
        return btrue;
    }


    bbool AnimTemplate::ComputePatchs(AnimPatchBank *_patchBank, u32 _index)
    {
        clearPatchData();

        ITF_MAP<void *, VectorAnim<AnimPatchPoint *> > ClosingInfo;

        // Helpers to hide the address tricks
        auto isMarkedAddress = [](const AnimBone* ptr) -> bool { return (uPtr(ptr) & 1) != 0; };
        auto buildMarkedAddress = [](AnimBone* ptr) -> void* { return (void*)(~uPtr(ptr)); };
        auto removedMarkedAddress = [](const AnimBone* ptr) -> const AnimBone* { return (const AnimBone*)(~uPtr(ptr)); };

        u32 boneSize = m_BonesCurFrame.size();
        for (u32 i=0; i<boneSize; ++i)
        {
            AnimBone    *B                  = &m_BonesCurFrame[i];
            ITF_ASSERT(!isMarkedAddress(B));
            u32          NumberOfSubBones	= B->m_PatchPointsPtr.size();

            if (NumberOfSubBones > 0) 
            {
                ClosingInfo[B].push_back(B->m_PatchPointsPtr[B->m_PatchPointsPtr.size()-1]);
                ClosingInfo[B].push_back(B->m_PatchPointsPtr[B->m_PatchPointsPtr.size()-2]);
                if (B->m_BoneParent && !(B->m_PatchFlags & BONE_PARENT_CUT) )
                {
                    ClosingInfo[B->m_BoneParent].push_back(B->m_PatchPointsPtr[0]);
                    ClosingInfo[B->m_BoneParent].push_back(B->m_PatchPointsPtr[1]);
                } else
                {
                    void * BMarked = buildMarkedAddress(B);
                    ClosingInfo[BMarked].push_back(B->m_PatchPointsPtr[0]); // ~B to obtain a new non used value
                    ClosingInfo[BMarked].push_back(B->m_PatchPointsPtr[1]);
                }
                if (NumberOfSubBones > 2)
                {
                    AnimPatchPoint * iiI1 = B->m_PatchPointsPtr[0];
                    AnimPatchPoint * iiI2 = B->m_PatchPointsPtr[1];
                    for (u32 ii = 2; ii<B->m_PatchPointsPtr.size(); ii +=2)
                    {
                        AnimPatchPoint * I1 = B->m_PatchPointsPtr[ii];
                        AnimPatchPoint * I2 = B->m_PatchPointsPtr[ii+1];

                        AnimPatch       animPatch;

                        animPatch.m_PatchBankPtr    = _patchBank;
                        animPatch.m_TemplateIdx     = _index;
                        animPatch.m_nbPatchPoints   = 4;
                        animPatch.m_AnimPatchPoints[0]  = iiI1;
                        animPatch.m_AnimPatchPoints[1]  = iiI2;
                        animPatch.m_AnimPatchPoints[2]  = I1;
                        animPatch.m_AnimPatchPoints[3]  = I2;

                        iiI1 = I1;
                        iiI2 = I2;

                        m_Patch.push_back(animPatch);
                    }
                }
            }
        }


        for (ITF_MAP<void *, VectorAnim<AnimPatchPoint *> >::const_iterator iter = ClosingInfo.begin(); iter != ClosingInfo.end(); ++iter)
        {
            const AnimBone *                        BPtr    = (AnimBone*)(*iter).first;
            const VectorAnim<AnimPatchPoint *>          &CInfo  = (*iter).second;
            bbool                                   isBegin = bfalse;
            
            if (isMarkedAddress(BPtr))
            {
                BPtr = removedMarkedAddress(BPtr);
                isBegin     = btrue;
            }

            AnimPatchPoint * iiI1 = CInfo[0];
            AnimPatchPoint * iiI2 = CInfo[1];
            if (CInfo.size() == 4)
            {
                AnimPatchPoint * I1 = CInfo[3];
                AnimPatchPoint * I2 = CInfo[2];

                AnimPatch       animPatch;
                animPatch.m_PatchBankPtr    = _patchBank;
                animPatch.m_TemplateIdx     = _index;
                animPatch.m_nbPatchPoints   = 4;
                animPatch.m_AnimPatchPoints[0]  = I1;
                animPatch.m_AnimPatchPoints[1]  = I2;
                animPatch.m_AnimPatchPoints[2]  = iiI1;
                animPatch.m_AnimPatchPoints[3]  = iiI2;
                
                m_Patch.push_back(animPatch);
            } else if (CInfo.size() == 2)
            {
                if (!( isBegin && (BPtr->m_PatchFlags & BONE_NO_BEGIN)) &&
                    !(!isBegin && (BPtr->m_PatchFlags & BONE_NO_END  )))
                {
                    AnimPatch       animPatch;

                    animPatch.m_PatchBankPtr    = _patchBank;
                    animPatch.m_TemplateIdx     = _index;
                    animPatch.m_nbPatchPoints   = 2;
                    animPatch.m_AnimPatchPoints[0]  = iiI1;
                    animPatch.m_AnimPatchPoints[1]  = iiI2;

                    m_Patch.push_back(animPatch);
                }                
            }
        }
        return true;
    }


    void AnimTemplate::computeBoneOrder( SafeArray<AnimBone *> & bonesOrdered )
    {
        u32 count = m_BonesCurFrame.size();
        u8* done  = new_array(u8,count,mId_Animation);

        ITF_MemSet(done, 0, count);
        VectorAnim<AnimBone>::iterator bIter = m_BonesCurFrame.begin();
        for (i32 pos = 0;bIter != m_BonesCurFrame.end(); ++bIter)
            (*bIter).setIndex(pos++);

        for (bIter = m_BonesCurFrame.begin();
            bIter != m_BonesCurFrame.end(); ++bIter)
        {
            i32 index = (*bIter).getIndex();
            if (!done[index])
            {
                done[index] = 1;
                AnimBone    *B = &(*bIter);
                u32         pos = bonesOrdered.size();
                bonesOrdered.push_back(B);
                while (B && B->m_BoneParent)
                {
                    i32 parentIndex = B->m_BoneParent->getIndex();
                    if (!done[parentIndex])
                    {
                        done[parentIndex] = 1;
                        B = B->m_BoneParent;
                        bonesOrdered.insert(B, pos);
                    } else
                        B = NULL;
                }
            }
        }
        SF_DEL_ARRAY(done);
    }

    void AnimTemplate::ComputeBonesGlobalPos()
    {
        SafeArray<AnimBone *> bonesOrdered;
        computeBoneOrder( bonesOrdered );

        u32 count = bonesOrdered.size();
        for (u32 i=0; i<count; ++i)
        {
            AnimBone    *pBone     = bonesOrdered[i];
            AnimBoneDyn &rAnimBone = m_BonesCurFrameDyn[pBone->getIndex()];

            if (pBone->m_BoneParent)
            {
                AnimBoneDyn &rAnimBoneParent = m_BonesCurFrameDyn[pBone->m_BoneParent->getIndex()];
#ifndef NO_RUNTIME_LOCK
                rAnimBone.ComputeGlobal(&rAnimBoneParent, NULL);
#else
                rAnimBone.ComputeGlobal(&rAnimBoneParent);
#endif
            }
            else
            {
                rAnimBone.ComputeGlobal();
            }
        }
    }

    void AnimTemplate::ComputeBonesLocalPos( bbool _visualRootOnly )
    {
        SafeArray<AnimBone *> bonesOrdered;
        computeBoneOrder( bonesOrdered );

        const u32 count = bonesOrdered.size();
        ITF_ASSERT(count);
        for (u32 i=0; i<count; ++i)
        {
            AnimBone * pBone = bonesOrdered[i];
            if ( _visualRootOnly && 
                (pBone->m_BoneParent && !(pBone->m_PatchFlags & BONE_PARENT_CUT)))
                continue;

            if (pBone->m_BoneParent)
            {
                m_BonesCurFrameDyn[pBone->getIndex()].ComputeLocal(&m_BonesCurFrameDyn[pBone->m_BoneParent->getIndex()]);
            }
            else
                m_BonesCurFrameDyn[pBone->getIndex()].ComputeLocal(NULL);
        }

    }

#ifndef ITF_SUPPORT_MULTITHREADED
    void AnimTemplate::ComputeBoneInMainSkeleton( AnimSkeleton *_skeleton )
    {
        VectorAnim<AnimBone>::iterator  boneIter = m_BonesCurFrame.begin();
        m_computeBonesOK = btrue;
        for (; boneIter != m_BonesCurFrame.end(); ++boneIter)
        {
            // compute Bone index
            (*boneIter).m_OrderedIndex = _skeleton->getBoneIndex((*boneIter).m_Name); 
            m_computeBonesOK &= (*boneIter).m_OrderedIndex != U32_INVALID;
        }
        m_computedSkeleton = _skeleton;
    }
#endif    

    // Different algorithms depends on patch influences and scale ratio
#ifndef ANIMATION_FIX_ABB
#define GROWABB patchBounding.grow(_ppDyn->m_Pos);
#define PROC_MIN_MAX Vec2d::Max(&MaxP,&_ppDyn->m_Pos,&MaxP);\
    Vec2d::Min(&MinP,&_ppDyn->m_Pos,&MinP);
#else
#define GROWABB
#define PROC_MIN_MAX
#endif

#ifndef ITF_UNIQUE_PATCH_POINT_REF
#define PROCESS_SINGLE_PATCH\
    f32 totalInfluence= 0.0f;\
    \
    Vec2d posPatchPoint = Vec2d::Zero;\
    Vec2d Normal        = Vec2d::Zero;\
    f32  Zorder	  = 0.0f;\
    \
    for (ITF_VECTOR<AnimPatchPointLocal>::const_iterator ppLocal = patchPoint->m_LocalRefs.begin();\
    ppLocal != patchPoint->m_LocalRefs.end(); ++ppLocal)\
        {\
        const AnimBoneDyn      *B= &(*_dyn)[(*ppLocal).m_BonePtr->getIndex()];;\
        Pos1		    = B->m_Pos;\
        Pos2		    = B->GetEnd();\
        COMPUTE_SCALE_IN(Pos1, Pos2);\
        \
        Vect00	        = (Pos2-Pos1).normalize();\
        Vect90	        = Turn90(Vect00);\
        posPatchPoint	= posPatchPoint	 + (Pos1 +  (Pos2 - Pos1) * (*ppLocal).m_Pos.m_x  + Vect90 * (*ppLocal).m_Pos.m_y * B->m_Scale.m_y)*(*ppLocal).m_Influence;\
        Normal	        = Normal + (Vect00 * (*ppLocal).m_Normale.m_x + Vect90 * (*ppLocal).m_Normale.m_y)*(*ppLocal).m_Influence;\
        Zorder		    = Zorder + B->m_Zorder*(*ppLocal).m_Influence;\
        totalInfluence += (*ppLocal).m_Influence;\
    }\
    \
    _ppDyn->m_Pos		= posPatchPoint * totalInfluence;\
    _ppDyn->m_Normale	= Normal.normalize();\
    COMPUTE_SCALE_OUT(_ppDyn->m_Pos, _ppDyn->m_Normale);\
    GROWABB;\
    _ppDyn->m_Zorder	= Zorder / totalInfluence;\
    _ppDyn->m_Mirror    = fMirror < 0.f;

#else
#define PROCESS_SINGLE_PATCH\
    f32                         fPMirror;\
    f32                         VDirLen;\
    const AnimPatchPointLocal   &curRef = patchPoint->m_mainRefs;\
    \
    const AnimBoneDyn           *B	    = &(*_dyn)[curRef.m_BonePtr->getIndex()];\
    Pos1		    = B->m_Pos;\
    Pos2		    = B->m_PosEnd;\
    COMPUTE_SCALE_IN(Pos1, Pos2);\
    VDir	        = (Pos2-Pos1);\
    Vect00	        = VDir;\
    VDirLen         = VDir.norm();\
    Vect00          *= f32_Inv(VDirLen);\
    fPMirror        = B->m_Scale.m_x * B->m_Scale.m_y >= 0.f ? 1.f : -1.f;\
    \
    Vect90.m_x = Vect00.m_y * fMirror * fPMirror * -1.f;\
    Vect90.m_y = Vect00.m_x * fMirror * fPMirror;\
    \
    _ppDyn->m_Pos       = Pos1 +  VDir * curRef.m_Pos.m_x  + Vect90 * (curRef.m_Pos.m_y * f32_Abs(B->m_Scale.m_y * (VDirLen / B->GetScaledLength())));\
    _ppDyn->m_Normale   = Vect00 * curRef.m_Normale.m_x + Vect90 * curRef.m_Normale.m_y ;\
    _ppDyn->m_Normale.normalize();\
    COMPUTE_SCALE_OUT(_ppDyn->m_Pos, _ppDyn->m_Normale);\
    GROWABB;\
    _ppDyn->m_Zorder    = B->m_Zorder;\
    _ppDyn->m_Alpha     = B->m_Alpha;\
    _ppDyn->m_Mirror    = fPMirror < 0.f;

#endif

#define COMPUTE_PATCH_POINTS \
    const u32 patchCount = m_PatchPoints.size();\
    if (!patchCount)\
    return btrue;\
    \
    AnimPatchPoint      *patchPoint     = &m_PatchPoints[0];\
    Vec2d VDir, Vect00, Vect90, Pos1, Pos2;\
    \
    AnimPatchPoint *patchPointEnd = patchPoint + patchCount;\
    f32     fMirror = _globalData.m_mirror ? -1.f : 1.f;\
    \
    for (;patchPoint < patchPointEnd; patchPoint++, _ppDyn++)\
    {\
    PROCESS_SINGLE_PATCH;\
    }

#ifdef ITF_WII
#ifndef ANIMATION_FIX_ABB
    bbool AnimTemplate::ComputePatchPointGlobalPos(SafeArray<AnimBoneDyn> *_dyn, AnimPatchPointDyn *_ppDyn, const AnimGlobalData & _globalData, AABB &_patchBounding, SafeArray<AnimBoneRenderable>*)
#else
    bbool AnimTemplate::ComputePatchPointGlobalPos(SafeArray<AnimBoneDyn> *_dyn, AnimPatchPointDyn *_ppDyn, const AnimGlobalData & _globalData, SafeArray<AnimBoneRenderable>*)
#endif
    {
#ifndef ANIMATION_FIX_ABB
        AABB patchBounding(_patchBounding);
#endif
		ITF_ASSERT(_globalData.m_isRatioOne);

        const u32 patchCount = m_PatchPoints.size();
        if (!patchCount)
            return btrue;
        if (patchCount>1)
            MEM_M_DCZeroRangeSafe(_ppDyn, sizeof(AnimPatchPointDyn)*m_PatchPoints.size());
        
        AnimPatchPoint      *patchPoint     = &m_PatchPoints[0];
        
        AnimPatchPoint *patchPointEnd = patchPoint + patchCount;        
        f32     fMirror = _globalData.m_mirror ? -1.f : 1.f;

        /*register Vec2d VDir, Vect00, Vect90, Pos1, Pos2;
        for (u32 i=0;patchPoint < patchPointEnd; patchPoint++, i++, _ppDyn++)
        {
            f32                         fPMirror;
            f32                         VDirLen;
            const AnimPatchPointLocal   &curRef = patchPoint->m_mainRefs;
            const AnimBoneDyn           *B	    = &(*_dyn)[curRef.m_BonePtr->getIndex()];
            Pos1		    = B->m_Pos;
            Pos2		    = B->m_PosEnd;
            VDir	        = (Pos2-Pos1);
            Vect00	        = VDir;
            VDirLen         = VDir.norminv();
            Vect00          *= VDirLen;
            VDirLen = f32_Inv(VDirLen);
            fPMirror        = B->m_Scale.m_x * B->m_Scale.m_y >= 0.f ? 1.f : -1.f;
            Vect90.m_x = Vect00.m_y * fMirror * fPMirror * -1.f;
            Vect90.m_y = Vect00.m_x * fMirror * fPMirror;
            _ppDyn->m_Pos       = Pos1 +  VDir * curRef.m_Pos.m_x  + Vect90 * (curRef.m_Pos.m_y * f32_Abs(B->m_Scale.m_y * (VDirLen / B->GetScaledLength())));
            _ppDyn->m_Normale   = Vect00 * curRef.m_Normale.m_x + Vect90 * curRef.m_Normale.m_y ;
            _ppDyn->m_Normale.normalize();
            _ppDyn->m_Zorder    = B->m_Zorder;
            _ppDyn->m_Alpha     = B->m_Alpha;
            _ppDyn->m_Mirror    = fPMirror < 0.f;
#ifndef ANIMATION_FIX_ABB
            patchBounding.grow(_ppDyn->m_Pos);
#endif
        }*/
        register float *pf0, *pf1, *pf2;
        register __vec2x32float__ fVDir, fVect00, fVect90, fPos1, fPos2, fNormale, fTmp0, fTmp1;
        register float fPMirror, VDirLen, Tmp0, Tmp1, fM, fM1;
        for (u32 i=0;patchPoint < patchPointEnd; patchPoint++, i++, _ppDyn++)
        {
            const AnimPatchPointLocal   &curRef = patchPoint->m_mainRefs;
            const AnimBoneDyn           *B	    = &(*_dyn)[curRef.m_BonePtr->getIndex()];
            //Pos1		    = B->m_Pos;
            //Pos2		    = B->m_PosEnd;
            //VDir	        = (Pos2-Pos1);
            pf0 = (float*)&B->m_Pos.m_x;
            pf1 = (float*)&B->m_PosEnd.m_x;            
            asm{ 
                psq_l fPos1, 0(pf0), 0, 0
                psq_l fPos2, 0(pf1), 0, 0
                ps_sub fVDir, fPos2, fPos1
            }
            //Vect00	        = VDir;
            //VDirLen         = VDir.norminv();
            //Vect00          *= VDirLen;
            //VDirLen = f32_Inv(VDirLen);
            asm{
                ps_mul  fVect00, fVDir, fVDir
                ps_sum0 VDirLen, fVect00, fVect00, fVect00
            }
            VDirLen = f32_InvSqrt(VDirLen);
            asm{
                ps_muls0  fVect00, fVDir, VDirLen
                fres VDirLen, VDirLen
            }
            
            fPMirror        = B->m_Scale.m_x * B->m_Scale.m_y >= 0.f ? 1.f : -1.f;
            fM              = fMirror * fPMirror;
            fM1             = fM * -1.f;
                       
            asm {
                ps_merge00 fTmp1, fM, fM1
                ps_mul fVect90, fVect00, fTmp1;
                ps_merge10 fVect90, fVect90, fVect90
            }
            
            Tmp0 = curRef.m_Pos.m_x;
            Tmp1 = curRef.m_Pos.m_y * f32_Abs(B->m_Scale.m_y * (VDirLen / B->GetScaledLength()));
                        
            //_ppDyn->m_Pos       = Pos1 +  VDir * fTmp0  + Vect90 * fTmp1;
            asm {
                ps_madds0 fTmp0, fVDir, Tmp0, fPos1
                ps_madds0 fPos1, fVect90, Tmp1, fTmp0
            }

            pf0 = (float*)&_ppDyn->m_Pos.m_x;
            asm { psq_st fPos1, 0(pf0), 0, 0 }
#ifndef ANIMATION_FIX_ABB
            patchBounding.grow(_ppDyn->m_Pos);            
#endif            
            //_ppDyn->m_Normale   = Vect00 * curRef.m_Normale.m_x + Vect90 * curRef.m_Normale.m_y ;            
            pf1 = (float*)&curRef.m_Normale.m_x;
            pf2 = (float*)&_ppDyn->m_Normale.m_x;            
            asm{
                psq_l fNormale, 0(pf1), 0, 0
                ps_muls0  fTmp1, fVect00, fNormale
                ps_madds1 fTmp1, fVect90, fNormale, fTmp1
                psq_st fTmp1, 0(pf2), 0, 0
            }            
            _ppDyn->m_Normale.normalize();
            
            _ppDyn->m_Zorder    = B->m_Zorder;
            _ppDyn->m_Alpha     = B->m_Alpha;
            _ppDyn->m_Mirror    = fPMirror < 0.f;
        }
#ifndef ANIMATION_FIX_ABB
		_patchBounding = patchBounding;
#endif
        return btrue;
    }
#elif defined (ANIMUSEGEOMETRYSHADER)
#ifndef ANIMATION_FIX_ABB
bbool AnimTemplate::ComputePatchPointGlobalPos(SafeArray<AnimBoneDyn> *_dyn, AnimPatchPointDyn *_ppDyn, const AnimGlobalData & _globalData, AABB &_patchBounding, SafeArray<AnimBoneRenderable>* _finalBoneList)
#else
bbool AnimTemplate::ComputePatchPointGlobalPos(SafeArray<AnimBoneDyn> *_dyn, AnimPatchPointDyn *_ppDyn, const AnimGlobalData & _globalData, SafeArray<AnimBoneRenderable>* _finalBoneList)
#endif
{
    //ITF_ASSERT(_globalData.m_isRatioOne);
#ifndef ANIMATION_FIX_ABB
	AABB patchBounding(_patchBounding);
#endif
    const u32 patchCount = m_PatchPoints.size();
    if (!patchCount)
    {
        return btrue;
    }

    AnimPatchPoint      *patchPoint     = m_PatchPoints[0];

    AnimPatchPoint *patchPointEnd = patchPoint + patchCount;

    u32 initialBoneCount = _finalBoneList->size();
    _finalBoneList->resize(initialBoneCount + _dyn->size());
    for (u32 i = 0; i < _dyn->size(); i++)
    {
        (*_finalBoneList)[i + initialBoneCount].setBone((*_dyn)[i]);
    }

    for (u32 i=0;patchPoint < patchPointEnd; patchPoint++, i++, _ppDyn++)
    {
        const AnimPatchPointLocal   &curRef = patchPoint->m_mainRefs;

        const AnimBoneDyn           *B	    = &(*_dyn)[curRef.m_BonePtr->getIndex()];

        _ppDyn->m_Pos       = curRef.m_Pos;
        _ppDyn->m_Normale   = curRef.m_Normale;
        _ppDyn->m_Zorder    = B->m_Zorder;
        _ppDyn->m_Alpha     = B->m_Alpha;
        _ppDyn->m_boneIdx   = initialBoneCount + curRef.m_BonePtr->getIndex();
#ifndef ANIMATION_FIX_ABB
        patchBounding.grow(B->m_PosEnd);
#endif
    }
#ifndef ANIMATION_FIX_ABB
	_patchBounding = patchBounding;
#endif
    return btrue;
}
#else

#ifndef ANIMATION_FIX_ABB
bbool AnimTemplate::ComputePatchPointGlobalPos(SafeArray<AnimBoneDyn> *_dyn, AnimPatchPointDyn *_ppDyn, const AnimGlobalData & _globalData, AABB &_patchBounding, SafeArray<AnimBoneRenderable>*)
#else
bbool AnimTemplate::ComputePatchPointGlobalPos(SafeArray<AnimBoneDyn> *_dyn, AnimPatchPointDyn *_ppDyn, const AnimGlobalData & _globalData, SafeArray<AnimBoneRenderable>*)
#endif
{
    ITF_ASSERT(_globalData.m_isRatioOne);
#ifndef ANIMATION_FIX_ABB
    AABB patchBounding(_patchBounding);
#endif

#define COMPUTE_SCALE_IN(pos, posEnd)
#define COMPUTE_SCALE_OUT(pos, normal)

    COMPUTE_PATCH_POINTS;

#undef COMPUTE_SCALE_IN
#undef COMPUTE_SCALE_OUT

#ifndef ANIMATION_FIX_ABB
    _patchBounding = patchBounding;
#endif
    return btrue;
}
#endif // !ITF_WII

#ifndef ANIMATION_FIX_ABB
    bbool AnimTemplate::ComputePatchPointGlobalPosRatio(SafeArray<AnimBoneDyn> *_dyn, AnimPatchPointDyn *_ppDyn, const AnimGlobalData & _globalData, AABB &_patchBounding)
#else
    bbool AnimTemplate::ComputePatchPointGlobalPosRatio(SafeArray<AnimBoneDyn> *_dyn, AnimPatchPointDyn *_ppDyn, const AnimGlobalData & _globalData)
#endif
    {
        ITF_ASSERT(!_globalData.m_isRatioOne);
#ifndef ANIMATION_FIX_ABB
        AABB patchBounding(_patchBounding);
#endif

#define COMPUTE_SCALE_IN(pos, posEnd)\
    pos             = _globalData.m_origin\
    + _globalData.m_xAxis*((pos - _globalData.m_origin).dot(_globalData.m_xAxis))\
    + _globalData.m_yAxis*((pos - _globalData.m_origin).dot(_globalData.m_yAxis)*_globalData.m_ratio);\
    \
    posEnd          = _globalData.m_origin\
    + _globalData.m_xAxis*((posEnd - _globalData.m_origin).dot(_globalData.m_xAxis))\
    + _globalData.m_yAxis*((posEnd - _globalData.m_origin).dot(_globalData.m_yAxis)*_globalData.m_ratio);

#define COMPUTE_SCALE_OUT(pos, normal)\
    pos   = _globalData.m_origin\
    + _globalData.m_xAxis*((pos - _globalData.m_origin).dot(_globalData.m_xAxis))\
    + _globalData.m_yAxis*((pos - _globalData.m_origin).dot(_globalData.m_yAxis)/_globalData.m_ratio);\
    \
    normal = \
    _globalData.m_xAxis*((normal).dot(_globalData.m_xAxis))\
    + _globalData.m_yAxis*((normal).dot(_globalData.m_yAxis)*_globalData.m_ratio);\
    normal.normalize();

        COMPUTE_PATCH_POINTS;
#undef COMPUTE_SCALE_IN
#undef COMPUTE_SCALE_OUT

#ifndef ANIMATION_FIX_ABB
		_patchBounding = patchBounding;
#endif
        return btrue;
    }

#ifndef ITF_WII

#define PROCESS_SINGLE_PATCH_MATRIX\
    const AnimPatchPointLocal   &curRef = patchPoint->m_mainRefs;\
    \
    const PatchPointBoneMatrix     *PPMatrix   = &(*_ppmat)[curRef.m_BonePtr->m_OrderedIndex];\
    _ppDyn->m_Pos       = PPMatrix->m_translation + PPMatrix->m_dir * curRef.m_Pos.m_x  + PPMatrix->m_perpendicular * curRef.m_Pos.m_y * PPMatrix->m_ratio;\
    _ppDyn->m_Normale   = PPMatrix->m_dir * curRef.m_Normale.m_x + PPMatrix->m_perpendicular * curRef.m_Normale.m_y ;\
    COMPUTE_SCALE_OUT_MATRIX(_ppDyn->m_Pos, _ppDyn->m_Normale); \
    _ppDyn->m_Zorder    = PPMatrix->m_zOrder;\
    _ppDyn->m_Alpha     = PPMatrix->m_alpha;\
    _ppDyn->m_Mirror    = PPMatrix->m_mirror;\
    PROC_MIN_MAX\
	 _ppDyn->m_Normale.normalize();


#else

#define PROCESS_SINGLE_PATCH_MATRIX\
    const AnimPatchPointLocal   *curRef = &patchPoint->m_mainRefs;\
    const PatchPointBoneMatrix     *PPMatrix   = &(*_ppmat)[curRef->m_BonePtr->m_OrderedIndex];\
	mT = ps::Load((float*)&PPMatrix->m_translation);\
	mD = ps::Load((float*)&PPMatrix->m_dir);\
	mP = ps::Load((float*)&PPMatrix->m_perpendicular);\
	m_Zorder    = PPMatrix->m_zOrder;\
	m_Alpha     = PPMatrix->m_alpha;\
	m_Mirror    = PPMatrix->m_mirror;\
	register ps::Float2 ResP = ps::Load((float*)&curRef->m_Pos);\
	register ps::Float2 ResN1 = ps::Load((float*)&curRef->m_Normale);\
	ResP = ps::Add(mT,ps::Add(ps::Mul(mD,ps::GetX(ResP)),ps::Mul(mP,ps::GetY(ResP) * PPMatrix->m_ratio)));\
	ResN1 = ps::Add(ps::Mul(mD,ps::GetX(ResN1)),ps::Mul(mP,ps::GetY(ResN1)));\
	ps::Store( ResP , (float*)&_ppDyn->m_Pos );\
    _ppDyn->m_Zorder    = m_Zorder;\
    _ppDyn->m_Alpha     = m_Alpha;\
    _ppDyn->m_Mirror    = m_Mirror;\
    PROC_MIN_MAX\
    patchPoint++; _ppDyn++;\
    curRef = &patchPoint->m_mainRefs;\
	ResP = ps::Load((float*)&curRef->m_Pos);\
	ResP = ps::Add(mT,ps::Add(ps::Mul(mD,ps::GetX(ResP)),ps::Mul(mP,ps::GetY(ResP) * PPMatrix->m_ratio)));\
	register ps::Float2 ResN2 = ps::Load((float*)&curRef->m_Normale);\
	ResN2 = ps::Add(ps::Mul(mD,ps::GetX(ResN2)),ps::Mul(mP,ps::GetY(ResN2)));\
	ps::Store( ResP , (float*)&_ppDyn->m_Pos );\
    PROC_MIN_MAX\
	register ps::Float2 ResNNN = ps::Merge(ps::Dot(ResN1,ResN1),ps::Dot(ResN2,ResN2));\
	ResNNN = ps::RsqrtEstimate(ResNNN);\
	ResN1 = ps::Mul(ResN1,ps::GetX(ResNNN));\
	ResN2 = ps::Mul(ResN2,ps::GetY(ResNNN));\
	_ppDyn--;\
	ps::Store( ResN1 , (float*)&_ppDyn->m_Normale );\
    COMPUTE_SCALE_OUT_MATRIX(_ppDyn->m_Pos, _ppDyn->m_Normale); \
	_ppDyn++;\
	ps::Store( ResN2 , (float*)&_ppDyn->m_Normale );\
    COMPUTE_SCALE_OUT_MATRIX(_ppDyn->m_Pos, _ppDyn->m_Normale); \
    _ppDyn->m_Zorder    = m_Zorder;\
    _ppDyn->m_Alpha     = m_Alpha;\
    _ppDyn->m_Mirror    = m_Mirror;

#endif

#define COMPUTE_PATCH_POINTS_MATRIX \
    const u32 patchCount = m_PatchPoints.size();\
    if (!patchCount)\
    return btrue;\
    \
    AnimPatchPoint      *patchPoint     = &m_PatchPoints[0];\
    \
    AnimPatchPoint *patchPointEnd = patchPoint + patchCount;\
    for (;patchPoint < patchPointEnd; patchPoint++, _ppDyn++)\
    {\
    PROCESS_SINGLE_PATCH_MATRIX;\
    }
#ifndef ANIMATION_FIX_ABB
    bbool AnimTemplate::ComputePatchPointGlobalPosMatrix(SafeArray<PatchPointBoneMatrix> *_ppmat, AnimPatchPointDyn *_ppDyn, const AnimGlobalData & _globalData, AABB &_patchBounding, SafeArray<AnimBoneRenderable> *_finalBoneList)
#else
    bbool AnimTemplate::ComputePatchPointGlobalPosMatrix(SafeArray<PatchPointBoneMatrix> *_ppmat, AnimPatchPointDyn *_ppDyn, const AnimGlobalData & _globalData, SafeArray<AnimBoneRenderable> *_finalBoneList)
#endif
    {
#ifndef ANIMATION_FIX_ABB
        Vec2d MinP,MaxP;
		MinP = Vec2d(100000.f,100000.f);
		MaxP = Vec2d(-100000.f,-100000.f);
#endif

#ifdef ITF_WII
		register const PatchPointBoneMatrix     *LastPPMatrix   = NULL;
#ifndef ANIMATION_FIX_ABB
        register ps::Float2 MinPF,MaxPF;
		MinPF = ps::Load(100000.f);
		MaxPF = ps::Neg(MinPF);
#endif
		register ps::Float2 mT;
		register ps::Float2 mD;
		register ps::Float2 mP;
		register float m_Zorder,m_Alpha;
		register bbool m_Mirror;

#endif

// OFTEN
#define COMPUTE_SCALE_OUT_MATRIX(pos, normal)

        COMPUTE_PATCH_POINTS_MATRIX;

#undef COMPUTE_SCALE_OUT_MATRIX

#ifndef ANIMATION_FIX_ABB
#ifdef ITF_WII
		_patchBounding.grow(MinPF);
		_patchBounding.grow(MaxPF);
#else
		_patchBounding.grow(MinP);
		_patchBounding.grow(MaxP);
#endif
#endif
        return btrue;
    }
#ifndef ANIMATION_FIX_ABB
    bbool  AnimTemplate::ComputePatchPointGlobalPosMatrixRatio(SafeArray<PatchPointBoneMatrix> *_ppmat, AnimPatchPointDyn *_ppDyn, const AnimGlobalData & _globalData, AABB &_patchBounding)
#else
    bbool  AnimTemplate::ComputePatchPointGlobalPosMatrixRatio(SafeArray<PatchPointBoneMatrix> *_ppmat, AnimPatchPointDyn *_ppDyn, const AnimGlobalData & _globalData)
#endif
    {
#ifndef ANIMATION_FIX_ABB
		register Vec2d MinP,MaxP;
		MinP = Vec2d(100000,100000);
		MaxP = Vec2d(-100000,-100000);
#endif
#ifdef ITF_WII
		register const PatchPointBoneMatrix     *LastPPMatrix   = NULL;
#ifndef ANIMATION_FIX_ABB
        register ps::Float2 MinPF,MaxPF;
		MinPF = ps::Load(100000);
		MaxPF = ps::Neg(MinPF);
#endif
		register ps::Float2 mT;
		register ps::Float2 mD;
		register ps::Float2 mP;
		register float m_Zorder,m_Alpha;
		register bbool m_Mirror;
#endif

// RARE
#define COMPUTE_SCALE_OUT_MATRIX(pos, normal)\
        pos   = _globalData.m_origin\
        + _globalData.m_xAxis*((pos - _globalData.m_origin).dot(_globalData.m_xAxis))\
        + _globalData.m_yAxis*((pos - _globalData.m_origin).dot(_globalData.m_yAxis)/_globalData.m_ratio);\
		PROC_MIN_MAX\
        normal = \
        _globalData.m_xAxis*((normal).dot(_globalData.m_xAxis))\
        + _globalData.m_yAxis*((normal).dot(_globalData.m_yAxis)*_globalData.m_ratio);


        COMPUTE_PATCH_POINTS_MATRIX;


#undef COMPUTE_SCALE_OUT_MATRIX

#ifndef ANIMATION_FIX_ABB
		_patchBounding.grow(MinP);
		_patchBounding.grow(MaxP);
#endif
        return btrue;
    }



    AnimPatchBank::AnimPatchBank() :
      m_Ratio(1.0f)
    , m_archiveLinker(NULL)
    {
        m_cookCallback = m_funcCookerPatchBank;
    }

    AnimPatchBank::~AnimPatchBank()
    {
        clear();
    }

    void AnimPatchBank::clear()
    {
		u32 size = m_templateList.size();
		for (u32 i=0; i<size; ++i)
		{
			SF_DEL(m_templateList[i]);
		}
        m_templateList.clear();
    }

    void AnimPatchBank::serialize(ArchiveMemory& _pArchive, bbool )
    {
        m_archiveLinker = _pArchive.getLinker();

        ITF_VERIFY(Versioning::serializeVersion(_pArchive,Versioning::animPatchBankVer()));

        _pArchive.serializeLink(reinterpret_cast<uPtr>(this));
        _pArchive.serialize(m_Ratio);
        m_NameToTemplate.serialize(_pArchive);

        u32 size;
        if (_pArchive.isReading())
        {
            _pArchive.serialize(size);
            m_templateList.reserve(size);
            for (u32 i=0; i<size; ++i)
            {
                AnimTemplate *tmplt = newAlloc(mId_Animation, AnimTemplate());
                tmplt->serialize(_pArchive);
                m_templateList.push_back(tmplt);
            }
        } else
        {
            size = m_templateList.size();
            _pArchive.serialize(size);
            for (u32 i=0; i<size; ++i)
            {
                AnimTemplate *tmplt = m_templateList[i];
                tmplt->serialize(_pArchive);
            }
        }
    }


	bbool AnimPatchBank::ComputeBonesInfo()
	{
		VectorAnim<AnimTemplate*>::iterator iter = m_templateList.begin();
		for (; iter != m_templateList.end(); ++iter)
		{
			if (!(*iter)->ComputeBonesInfo())
				return bfalse;
		}
		return btrue;
	}


	bbool AnimPatchBank::ComputePatchs()
	{
		VectorAnim<AnimTemplate*>::iterator iter = m_templateList.begin();
		for (u32 i=0; iter != m_templateList.end(); ++iter, ++i)
		{
			if (!(*iter)->ComputePatchs(this, i))
				return bfalse;
		}
		return btrue;
	}

	void AnimPatchBank::link()
	{
		for ( VectorAnim<AnimTemplate*>::iterator iter = m_templateList.begin();iter!=m_templateList.end();++iter)
		{
			(*iter)->link(this);
		}
	}

    void AnimPatchBank::postProcess( const String& _szFilename, const String& _szPlatform, bbool _tryLoadingBin )
    {
        ComputeBonesInfo();
    }

};



