#include "precompiled_engine.h"

#ifndef _ITF_ANIMATIONPATCHES_H_
#include    "engine/animation/AnimationPatches.h"
#endif // _ITF_ANIMATIONPATCHES_H_

#ifndef _ITF_ANIMATION_H_
#include    "engine/animation/Animation.h"
#endif // _ITF_ANIMATION_H_

#ifndef _ITF_ANIMATIONTRACK_H_
#include    "engine/animation/AnimationTrack.h"
#endif // _ITF_ANIMATIONTRACK_H_

#ifndef _ITF_ANIMATIONPATCHBANK_H_
#include    "engine/animation/AnimationPatchBank.h"
#endif // _ITF_ANIMATIONPATCHBANK_H_

#ifndef _ITF_ANIMATIONSKELETON_H_
#include    "engine/animation/AnimationSkeleton.h"
#endif // _ITF_ANIMATIONSKELETON_H_

#ifndef _ITF_ANIMATIONMANAGER_H_
#include    "engine/animation/AnimationManager.h"
#endif // _ITF_ANIMATIONMANAGER_H_

#ifndef _ITF_GFXADAPTER_RENDERTARGET_H_
#include "engine/AdaptersInterfaces/GFXAdapter_RenderTarget.h"
#endif // _ITF_GFXADAPTER_RENDERTARGET_H_

#ifndef _ITF_STATSMANAGER_H_
#include "engine/stats/statsManager.h"
#endif //_ITF_STATSMANAGER_H_

namespace ITF
{

    struct AnimPatchZLess
    {
        bool operator()(const AnimPatchZ & _r, const AnimPatchZ & _l)
        {
            return _r.m_z < _l.m_z;
        }
    };

    AnimPatches::AnimPatches()
        : m_lastState(U32_INVALID)
    {
    }

    AnimPatches::~AnimPatches()
    {

    }
//#pragma optimize("",off)
    void AnimPatches::setPatchs(ITF_VECTOR<AnimBMLInfo> & _bmlTab, const SafeArray<const AnimTrackExt*> & _trackExtList, u32 _state)
    {
#ifdef SUPPORT_LASTBML
        if (_state == m_lastState && m_lastBMLTab == _bmlTab)
            return;
#endif //SUPPORT_LASTBML

        m_allPatchs.clear();
        u32 bmlTabSize = _bmlTab.size();
        if(!bmlTabSize)
        {
#ifdef SUPPORT_LASTBML
			m_lastBMLTab.clear();
            m_lastState = _state;
#endif //SUPPORT_LASTBML
            return;
        }

        AnimBMLInfo * pAnimBMLInfo = &_bmlTab[0];
        const AnimTrackExt *const* pAnimTrackExt = _trackExtList.getDataBuffer();

        for (u32 i=0; i < bmlTabSize; i++, pAnimBMLInfo++,pAnimTrackExt++)
        {
            if (pAnimBMLInfo->m_bml)
                pAnimBMLInfo->m_bml->getCurrentPatchs(*pAnimTrackExt, pAnimBMLInfo->m_templateUsed, m_allPatchs, _state);
        }

#ifdef SUPPORT_LASTBML
        m_lastBMLTab = _bmlTab;
        m_lastState = _state;
#endif //SUPPORT_LASTBML
    }

    void AnimPatches::processPatchs(AnimMeshScene* _pAnimMeshScene, bbool _onWorking)
    {
        STATS_PATCHES_UPDATEDINC
        // TODO YOUS dirty flag on Z order !
        ITF_ASSERT(_pAnimMeshScene);

        AnimInfo        *pAnimInfo = &_pAnimMeshScene->m_AnimInfo;
        AnimFrameData   *pAnimFrameData = _onWorking ? pAnimInfo->getWorkingFrameData() : pAnimInfo->getCurrentFrameData();

        ITF_VECTOR<AnimBMLInfo>         &curBMLTab  = pAnimFrameData->m_bmlInfo;
        SafeArray<AnimBoneDyn>          &bonesList  = pAnimFrameData->m_bonesList;
        SafeArray<const AnimTrackExt *> &trackExt   = pAnimFrameData->m_trackExtList;

        pAnimInfo->m_globalData.process();
        setPatchs(curBMLTab, trackExt, pAnimInfo->m_state);

        // Always reset renderPassFrontierPatchIndex if necessary
        if (pAnimInfo->m_renderPassFrontierPatchIndex != U32_INVALID)
            pAnimInfo->m_renderPassFrontierPatchIndex = 0;

#ifndef ANIMATION_FIX_ABB
        pAnimInfo->m_patchAABB = AABB(pAnimInfo->m_Pos);
#endif
        if (curBMLTab.size()   == 0 ||
            bonesList.size()   == 0)
        {
            return;
        }

#ifdef ANIMUSEGEOMETRYSHADER
        pAnimInfo->m_finalBoneList.clear();
#endif

        pAnimInfo->clearTemplatesBuffer();
        for (u32 i=0; i<curBMLTab.size(); i++)
        {
            if (curBMLTab[i].m_bml)
                curBMLTab[i].m_bml->dynamicBonesToTemplate(pAnimInfo, trackExt[i]);
        }

        // set Z order to patches
        u32 count = m_allPatchs.size();
        if (count == 0)
            return;

        for (u32 i=0; i<count; i++)
        {
            AnimPatch*          PL          = m_allPatchs[i].m_patch;
            AnimPatchBank       *PLBank     = PL->m_PatchBankPtr;
            ITF_ASSERT(PLBank);
            AnimTemplate        *PLTemplate = PLBank->m_templateList[PL->m_TemplateIdx];
            ITF_ASSERT(PLTemplate);

#ifndef ITF_SUPPORT_MULTITHREADED
            if (!PLTemplate->IsBoneComputedOK())
                continue;
#endif
            AnimPatchPointDyn*  ppDyn   = pAnimInfo->getTemplatePatchPointBuffer(PLTemplate, bfalse);
            
            m_allPatchs[i].m_z = 0.f;
            for (int j=0; j<PL->m_nbPatchPoints; j++)
            {
                AnimPatchPoint      *pp  = PL->m_AnimPatchPoints[j];
                AnimPatchPointDyn   *ppd = &ppDyn[pp->m_Index];
                m_allPatchs[i].m_z += ppd->m_Zorder;
            }
            ITF_ASSERT(PL->m_nbPatchPoints == 4 || PL->m_nbPatchPoints == 2);
            if (PL->m_nbPatchPoints == 2)
            {
                m_allPatchs[i].m_z *= 2.f;
            }
        }

        std::sort(&m_allPatchs[0], &m_allPatchs[0]+m_allPatchs.size(), AnimPatchZLess());

        if (pAnimInfo->m_renderPassFrontierPatchIndex != U32_INVALID)
        {
            u32 i = 0;
            for (; i<count && m_allPatchs[i].m_z < 0.f; i++) {}
            pAnimInfo->m_renderPassFrontierPatchIndex = i;
        }
    }


} // namespace ITF


