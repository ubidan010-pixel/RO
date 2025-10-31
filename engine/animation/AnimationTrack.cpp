#include "precompiled_engine.h"

#ifndef _ITF_ANIMATION_H_
#include "engine/animation/Animation.h"
#endif //_ITF_ANIMATION_H_

#ifndef _ITF_ANIMATIONTRACK_H_
#include "engine/animation/AnimationTrack.h"
#endif //_ITF_ANIMATIONTRACK_H_

#ifndef _ITF_ANIMATIONSKELETON_H_
#include    "engine/animation/AnimationSkeleton.h"
#endif // _ITF_ANIMATIONSKELETON_H_

#ifndef _ITF_ANIMATIONPATCHBANK_H_
#include    "engine/animation/AnimationPatchBank.h"
#endif // _ITF_ANIMATIONPATCHBANK_H_

#ifndef _ITF_ANIMATIONPOLYLINE_H_
#include    "engine/animation/AnimationPolyline.h"
#endif // _ITF_ANIMATIONPOLYLINE_H_

#ifndef _ITF_ANIMATIONRES_H_
#include "engine/animation/AnimationRes.h"
#endif // _ITF_ANIMATIONRES_H_

#ifndef _ITF_ANIMATIONMANAGER_H_
#include    "engine/animation/AnimationManager.h"
#endif // _ITF_ANIMATIONMANAGER_H_

#ifndef _ITF_PUGIXMLWRAP_H_
#include "core/XML/PugiXMLWrap.h"
#endif //_ITF_PUGIXMLWRAP_H_

#ifndef _ITF_VERSIONING_H_
#include "core/versioning.h"
#endif  //_ITF_VERSIONING_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_STATSMANAGER_H_
#include "engine/stats/statsManager.h"
#endif //_ITF_STATSMANAGER_H_

#ifdef ITF_COMPRESSED_ANIM
#include "engine/lzo/minilzo.h"
#endif // ITF_COMPRESSED_ANIM

namespace ITF
{
    
cookFileCallback         AnimTrack::m_funcCookerTrack = NULL;

AnimTrackBoneWithLock::AnimTrackBoneWithLock(const AnimTrackBoneWithLock &_trk)
{
    ITF_Memcpy(this, &_trk, sizeof(AnimTrackBoneWithLock));
    if (_trk.m_Lock)
    {
        m_Lock = newAlloc(mId_Animation,AnimLock());
        ITF_Memcpy(m_Lock, _trk.m_Lock, sizeof(AnimLock));
    }
}


AnimTrackBoneWithLock::~AnimTrackBoneWithLock()
{
    SF_DEL(m_Lock);
}

AnimTrackBoneWithLock& AnimTrackBoneWithLock::operator=(const AnimTrackBoneWithLock& _trk)
{
    AnimLock *prevLock = m_Lock;
    ITF_Memcpy(this, &_trk, sizeof(AnimTrackBoneWithLock));
    m_Lock = prevLock;

    if (_trk.m_Lock)
    {
        if (!m_Lock)
            m_Lock = newAlloc(mId_Animation,AnimLock());
        ITF_Memcpy(m_Lock, _trk.m_Lock, sizeof(AnimLock));
    } else
    {
        SF_DEL(m_Lock);
    }
    return *this;
}



AnimTrackBonePAS::AnimTrackBonePAS() :
  m_frame(0)
, m_AngleLocal(0)
, m_PosLocalX(0)
, m_PosLocalY(0)
, m_ScaleX(1)
, m_ScaleY(1)
{
}

bbool AnimTrackBonePAS::isEpsilonEqual(AnimTrackBonePAS *_trkBone)
{
    return   m_AngleLocal == _trkBone->m_AngleLocal &&
            m_PosLocalX == _trkBone->m_PosLocalX &&
            m_PosLocalY == _trkBone->m_PosLocalY &&
            m_ScaleX == _trkBone->m_ScaleX &&
            m_ScaleY == _trkBone->m_ScaleY;
}

AnimTrackBonePAS::AnimTrackBonePAS(const AnimTrackBonePAS &_trk)
{
    ITF_Memcpy(this, &_trk, sizeof(AnimTrackBonePAS));
}


AnimTrackBonePAS::~AnimTrackBonePAS()
{
}



void  AnimTrackBonePAS::serialize(ArchiveMemory& _archive)
{
    _archive.serialize(m_frame);

    _archive.serialize(m_AngleLocal);
    _archive.serialize(m_PosLocalX);
    _archive.serialize(m_PosLocalY);
    _archive.serialize(m_ScaleX);
    _archive.serialize(m_ScaleY);
}


AnimTrackBoneZAL::AnimTrackBoneZAL() 
: m_frame(0)
, m_Alpha(0)
, m_Zorder(0.f)
#ifndef NO_RUNTIME_LOCK
, m_Lock(NULL)
#endif
{
}

bbool AnimTrackBoneZAL::isEpsilonEqual(AnimTrackBoneZAL *_trkBone)
{
    return   
        m_Alpha  == _trkBone->m_Alpha &&
        f32_Abs(m_Zorder       - _trkBone->m_Zorder)               < MTH_EPSILON
#ifndef NO_RUNTIME_LOCK
        &&
        ((!m_Lock         && !_trkBone->m_Lock) ||
        ( m_Lock         &&  _trkBone->m_Lock &&
        m_Lock->m_Locked.IsEqual(_trkBone->m_Lock->m_Locked,   MTH_EPSILON) &&
        f32_Abs(m_Lock->m_LockedAngle    - _trkBone->m_Lock->m_LockedAngle)      < MTH_EPSILON));
#else
        ;
#endif
}


AnimTrackBoneZAL::AnimTrackBoneZAL(const AnimTrackBoneZAL &_trk)
{
    ITF_Memcpy(this, &_trk, sizeof(AnimTrackBoneZAL));
#ifndef NO_RUNTIME_LOCK
    if (_trk.m_Lock)
    {
        m_Lock = newAlloc(mId_Animation,AnimLock());
        ITF_Memcpy(m_Lock, _trk.m_Lock, sizeof(AnimLock));
    }
#endif
}


AnimTrackBoneZAL::~AnimTrackBoneZAL()
{
#ifndef NO_RUNTIME_LOCK
    SF_DEL(m_Lock);
#endif
}

AnimTrackBoneZAL& AnimTrackBoneZAL::operator=(const AnimTrackBoneZAL& _trk)
{
#ifndef NO_RUNTIME_LOCK
    AnimLock *prevLock = m_Lock;
    ITF_Memcpy(this, &_trk, sizeof(AnimTrackBoneZAL));
    m_Lock = prevLock;

    if (_trk.m_Lock)
    {
        if (!m_Lock)
            m_Lock = newAlloc(mId_Animation,AnimLock());
        ITF_Memcpy(m_Lock, _trk.m_Lock, sizeof(AnimLock));
    } else
    {
        SF_DEL(m_Lock);
    }
#else
    ITF_Memcpy(this, &_trk, sizeof(AnimTrackBoneZAL));
#endif
    return *this;
}

void  AnimTrackBoneZAL::serialize(ArchiveMemory& _archive)
{
    _archive.serialize(m_frame);

    _archive.serialize(m_Zorder);
    _archive.serialize(m_Alpha);

#ifndef NO_RUNTIME_LOCK
    if (_archive.isReading())
    {
        bbool locked;
        _archive.serialize(locked);
        if (locked)
        {
            if (!m_Lock) m_Lock = newAlloc(mId_Animation,AnimLock());
            m_Lock->serialize(_archive);

        } else
        {
            SF_DEL(m_Lock);
        }
    } else
    {
        bbool locked = m_Lock != NULL;
        _archive.serialize(locked);
        if (m_Lock)
        {
            m_Lock->serialize(_archive);
        }
    }
#else
    bbool locked = bfalse;
    _archive.serialize(locked);
    ITF_ASSERT(!locked);
#endif
}


void AnimTrackBonesList::serialize(ArchiveMemory& _pArchive)
{
    _pArchive.serialize(m_trackPASIndex);
    _pArchive.serialize(m_trackPASSize);

    _pArchive.serialize(m_trackZALIndex);
    _pArchive.serialize(m_trackZALSize);
}

void AnimTrackBonesList::setTrack(VectorAnim<AnimTrackBoneWithLock> &_trackList, const AnimTrackBoneWithLock & _track)
{
    VectorAnim<AnimTrackBoneWithLock>::iterator iter = _trackList.begin();
    for (; iter != _trackList.end(); ++iter)
    {
        if ((*iter).m_frame ==  _track.m_frame)
        {
            (*iter) = _track;
            return;
        } else if ((*iter).m_frame > _track.m_frame)
        {
            _trackList.insert(iter, _track);
            return;
        }
    }
    _trackList.push_back(_track);
}


void AnimTrackBML::serialize(ArchiveMemory& _archive)
{
    _archive.serialize(m_frame);
    m_templateRefs.serialize(_archive);
}

void AnimTrackBML::templateToDynamicsBonesLen(AnimInfo * _animInfo, const AnimTrackExt * _animTrack)
{
    AnimSkeleton *skeleton = _animTrack->getSkeleton();
    if (!skeleton)
        return;

    SafeArray<AnimBoneDyn>  &bonesList  = _animInfo->getWorkingBoneList();

    VectorAnim<AnimBone>::const_iterator boneIter;
    VectorAnim<AnimBoneDyn>::const_iterator   boneDynIter;

    u32 count = m_templateRefs.size();
    for (u32 i=0; i<count; ++i)
    {
        AnimTemplate *pTemplate;
        if (_animTrack->getTemplate(m_templateRefs[i], pTemplate, _animInfo->m_state))
        {
            const f32 lenghtFactor = pTemplate->GetLenghtFactor();

#ifndef ITF_SUPPORT_MULTITHREADED
            if (!pTemplate->IsBoneComputedInMainSkeleton( skeleton ))
                pTemplate->ComputeBoneInMainSkeleton( skeleton );

            if (!pTemplate->IsBoneComputedOK())
                continue;
#endif

            boneIter        = pTemplate->m_BonesCurFrame.begin();
            boneDynIter     = pTemplate->m_BonesCurFrameDyn.begin();
            for (; boneIter != pTemplate->m_BonesCurFrame.end(); ++boneIter, ++boneDynIter)
            {
                // compute Bone index
#ifndef ITF_SUPPORT_MULTITHREADED
                i32          BSIndex = (*boneIter).m_OrderedIndex;
#else
                i32          BSIndex = skeleton->getBoneIndex((*boneIter).m_Name);
                if (BSIndex >= 0)
#endif
                {
                    AnimBoneDyn  *BDyn    = &bonesList[BSIndex];
                    BDyn->m_Length        = boneDynIter->m_Length * lenghtFactor;
                }
            }
        }
    }
}

#if defined (ANIMUSEGEOMETRYSHADER) || defined (ITF_SUPPORT_MULTITHREADED)
void AnimTrackBML::dynamicBonesToTemplate(AnimInfo * _animInfo, const AnimTrackExt * _animTrack)
{
    u32 count = m_templateRefs.size();
    AnimSkeleton *skeleton = _animTrack->getSkeleton();
    if (!skeleton)
        return;

    SafeArray<AnimBoneDyn>           &bonesList  = _animInfo->getWorkingBoneList();

    VectorAnim<AnimBone>::iterator   boneIter;
    AnimBoneDyn                      *BSDynTab;

    for (u32 i=0; i<count; ++i)
    {
        AnimTemplate *pTemplate;

        if (_animTrack->getTemplate(m_templateRefs[i], pTemplate, _animInfo->m_state) && pTemplate->m_PatchPoints.size() > 0)
        {
            const f32 templateLenghtFactor = pTemplate->GetLenghtFactor();
            _animInfo->m_tmpTemplateBonesList.resize(pTemplate->m_BonesCurFrame.size());
            SafeArray<AnimBoneDyn> *_dyn = &_animInfo->m_tmpTemplateBonesList;

#ifndef ITF_SUPPORT_MULTITHREADED
            if (!pTemplate->IsBoneComputedInMainSkeleton( skeleton ))
                pTemplate->ComputeBoneInMainSkeleton( skeleton );

            if (!pTemplate->IsBoneComputedOK())
                continue;
#endif
            boneIter = pTemplate->m_BonesCurFrame.begin();
            BSDynTab     = &(*_dyn)[0];
            for (; boneIter != pTemplate->m_BonesCurFrame.end(); ++boneIter, ++BSDynTab)
            {
                // compute Bone index
                //i32          BSIndex = (*boneIter)->m_BoneInMainSkeleton;
#ifndef ITF_SUPPORT_MULTITHREADED
                i32          BSIndex = (*boneIter).m_OrderedIndex;
#else
                i32          BSIndex = skeleton->getBoneIndex((*boneIter).m_Name);
                if (BSIndex >= 0)
#endif
                {
                    AnimBoneDyn  *BDyn    = &bonesList[BSIndex];
                    *BSDynTab             = *BDyn;
                    // We affect the X with the length, now we have to affect Y with the scale
                    BSDynTab->m_Scale.m_y*= templateLenghtFactor;
                }
            }
            AnimPatchPointDyn * ppDyn = _animInfo->getTemplatePatchPointBuffer(pTemplate, btrue);
#ifndef ANIMATION_FIX_ABB
#ifndef ANIMUSEGEOMETRYSHADER
            if (_animInfo->m_globalData.m_isRatioOne)
                pTemplate->ComputePatchPointGlobalPos(_dyn, ppDyn, _animInfo->m_globalData, _animInfo->m_patchAABB, NULL);
            else
                pTemplate->ComputePatchPointGlobalPosRatio(_dyn, ppDyn, _animInfo->m_globalData, _animInfo->m_patchAABB);
#else
            pTemplate->ComputePatchPointGlobalPos(_dyn, ppDyn, _animInfo->m_globalData, _animInfo->m_patchAABB, &_animInfo->m_finalBoneList);
#endif
#else
#ifndef ANIMUSEGEOMETRYSHADER
            if (_animInfo->m_globalData.m_isRatioOne)
                pTemplate->ComputePatchPointGlobalPos(_dyn, ppDyn, _animInfo->m_globalData, NULL);
            else
                pTemplate->ComputePatchPointGlobalPosRatio(_dyn, ppDyn, _animInfo->m_globalData);
#else
            pTemplate->ComputePatchPointGlobalPos(_dyn, ppDyn, _animInfo->m_globalData, &_animInfo->m_finalBoneList);
#endif
#endif

        }
    }
}
#else

static SafeArray<PatchPointBoneMatrix>  patchPointBoneMatrix;

void AnimTrackBML::dynamicBonesToTemplate(AnimInfo * _animInfo, const AnimTrackExt * _animTrack)
{
    u32 count = m_templateRefs.size();
    AnimSkeleton *skeleton = _animTrack->getSkeleton();
    if (!skeleton)
        return;

    SafeArray<AnimBoneDyn>           &bonesList  = _animInfo->getWorkingBoneList();
	SafeArray<PatchPointBoneMatrix>  &ppMatrix   = patchPointBoneMatrix;

    VectorAnim<AnimBone>::iterator  boneIter;
    ppMatrix.resize(bonesList.size());

    f32     fMirror = _animInfo->m_globalData.m_mirror ? -1.f : 1.f;
	u32		uMirror = _animInfo->m_globalData.m_mirror ? 0x80000000 : 0;
    if (_animInfo->m_globalData.m_isRatioOne)
    {
        for (u32 i=0; i<count; ++i)
        {
            AnimTemplate *pTemplate;

            if (_animTrack->getTemplate(m_templateRefs[i], pTemplate, _animInfo->m_state) && pTemplate->m_PatchPoints.size() > 0)
            {
                const f32 lenghtFactor = pTemplate->GetLenghtFactor();
                if (!pTemplate->IsBoneComputedInMainSkeleton( skeleton ))
                    pTemplate->ComputeBoneInMainSkeleton( skeleton );

                if (!pTemplate->IsBoneComputedOK(  ))
                    continue;

                boneIter = pTemplate->m_BonesCurFrame.begin();
                for (; boneIter != pTemplate->m_BonesCurFrame.end(); ++boneIter/*, ++BSDynTab*/)
                {
#ifdef ITF_WII // TODO : check eff
					i32          BSIndex = (*boneIter).m_OrderedIndex;
                    AnimBoneDyn          &BDyn  = bonesList[BSIndex];
                    PatchPointBoneMatrix &ppmat = ppMatrix[BSIndex];

					register ps::Float2 mT = ps::Load((float*)&BDyn.m_Pos);
					register ps::Float2 mD = ps::Sub(ps::Load((float*)&BDyn.m_PosEnd),mT);
					ps::Store( mT , (float*)&ppmat.m_translation);
					ps::Store( mD , (float*)&ppmat.m_dir);


					u32 xorS = ((*((u32 *)&(BDyn.m_Scale.m_x))) ^ (*((u32 *)&(BDyn.m_Scale.m_y))))>>31;

                    ppmat.m_mirror              = xorS;
					xorS = uMirror ^ (xorS<<31);
					*((u32*)&ppmat.m_perpendicular.m_y)   = *((u32*)&ppmat.m_dir.m_x) ^ xorS;
					xorS ^= 0x80000000;
                    *((u32*)&ppmat.m_perpendicular.m_x)   = *((u32*)&ppmat.m_dir.m_y) ^ xorS;
                    ppmat.m_ratio = f32_Abs(lenghtFactor * BDyn.m_Scale.m_y / (BDyn.m_Length * BDyn.m_Scale.m_x));
                    ppmat.m_alpha = BDyn.m_Alpha;
                    ppmat.m_zOrder= BDyn.m_Zorder;
#else
					i32          BSIndex = (*boneIter).m_OrderedIndex;
                    AnimBoneDyn          &BDyn  = bonesList[BSIndex];
                    PatchPointBoneMatrix &ppmat = ppMatrix[BSIndex];

                    ppmat.m_translation = BDyn.m_Pos;
                    ppmat.m_dir         = BDyn.m_PosEnd - BDyn.m_Pos;
                    if (BDyn.m_Scale.m_x * BDyn.m_Scale.m_y >= 0.f)
                    {
                        ppmat.m_mirror              = bfalse;
                        Vec2d::SwapXmY(&ppmat.m_perpendicular, &ppmat.m_dir);
                        ppmat.m_perpendicular *= fMirror;
                    } else
                    {
                        ppmat.m_mirror              = btrue;
                        Vec2d::SwapmXY(&ppmat.m_perpendicular, &ppmat.m_dir);
                        ppmat.m_perpendicular *= fMirror;
                    }

                    ppmat.m_ratio = f32_Abs(lenghtFactor * BDyn.m_Scale.m_y / (BDyn.m_Length * BDyn.m_Scale.m_x));
                    ppmat.m_alpha = BDyn.m_Alpha;
                    ppmat.m_zOrder= BDyn.m_Zorder;
#endif
                }
				AnimPatchPointDyn * ppDyn = _animInfo->getTemplatePatchPointBuffer(pTemplate, btrue);
#ifndef ANIMATION_FIX_ABB
                pTemplate->ComputePatchPointGlobalPosMatrix(&ppMatrix, ppDyn, _animInfo->m_globalData, _animInfo->m_patchAABB, NULL);
#else
                pTemplate->ComputePatchPointGlobalPosMatrix(&ppMatrix, ppDyn, _animInfo->m_globalData, NULL);
#endif
            }
        }
    } else
    {
        for (u32 i=0; i<count; ++i)
        {
            AnimTemplate *pTemplate;

            if (_animTrack->getTemplate(m_templateRefs[i], pTemplate, _animInfo->m_state) && pTemplate->m_PatchPoints.size() > 0)
            {
                const f32 lenghtFactor = pTemplate->GetLenghtFactor();
                if (!pTemplate->IsBoneComputedInMainSkeleton( skeleton ))
                    pTemplate->ComputeBoneInMainSkeleton( skeleton );

                if (!pTemplate->IsBoneComputedOK(  ))
                    continue;

                boneIter = pTemplate->m_BonesCurFrame.begin();
                for (; boneIter != pTemplate->m_BonesCurFrame.end(); ++boneIter/*, ++BSDynTab*/)
                {
                    i32          BSIndex = (*boneIter).m_OrderedIndex;
                    AnimBoneDyn          &BDyn  = bonesList[BSIndex];
                    PatchPointBoneMatrix &ppmat = ppMatrix[BSIndex];

                    ppmat.m_translation = BDyn.m_Pos;
                    ppmat.m_dir         = BDyn.m_PosEnd - BDyn.m_Pos;
                    Vec2d vtmp          = _animInfo->m_globalData.m_xAxis*(ppmat.m_dir.dot(_animInfo->m_globalData.m_xAxis));
                    Vec2d::ScaleAdd(&ppmat.m_dir, &_animInfo->m_globalData.m_yAxis, ppmat.m_dir.dot(_animInfo->m_globalData.m_yAxis) * _animInfo->m_globalData.m_ratio, &vtmp);
                    Vec2d::ScaleAdd(&vtmp, &_animInfo->m_globalData.m_xAxis, (ppmat.m_translation-_animInfo->m_globalData.m_origin).dot(_animInfo->m_globalData.m_xAxis), &_animInfo->m_globalData.m_origin);
                    Vec2d::ScaleAdd(&ppmat.m_translation, &_animInfo->m_globalData.m_yAxis, (ppmat.m_translation-_animInfo->m_globalData.m_origin).dot(_animInfo->m_globalData.m_yAxis) * _animInfo->m_globalData.m_ratio, &vtmp);
                    if (BDyn.m_Scale.m_x * BDyn.m_Scale.m_y >= 0.f)
                    {
                        ppmat.m_mirror              = bfalse;
                        Vec2d::SwapXmY(&ppmat.m_perpendicular, &ppmat.m_dir);
                        ppmat.m_perpendicular *= fMirror;
                    } else
                    {
                        ppmat.m_mirror              = btrue;
                        Vec2d::SwapmXY(&ppmat.m_perpendicular, &ppmat.m_dir);
                        ppmat.m_perpendicular *= fMirror;
                    }

                    ppmat.m_ratio = f32_Abs(lenghtFactor * BDyn.m_Scale.m_y / (BDyn.m_Length * BDyn.m_Scale.m_x));
                    ppmat.m_alpha = BDyn.m_Alpha;
                    ppmat.m_zOrder= BDyn.m_Zorder;
                }
				AnimPatchPointDyn * ppDyn = _animInfo->getTemplatePatchPointBuffer(pTemplate, btrue);
#ifndef ANIMATION_FIX_ABB
                pTemplate->ComputePatchPointGlobalPosMatrixRatio(&ppMatrix, ppDyn, _animInfo->m_globalData, _animInfo->m_patchAABB);
#else
                pTemplate->ComputePatchPointGlobalPosMatrixRatio(&ppMatrix, ppDyn, _animInfo->m_globalData);
#endif
            }
        }
    }

}
#endif

void AnimTrackBML::setComputeTab(const AnimTrackExt * _animTrack, const AnimFrameInfo::BonesArray& _boneUsed, SafeArray<char> &_templateUsed, u32 _state)
{
    u32 count = m_templateRefs.size();
    AnimSkeleton *skeleton = (AnimSkeleton *)_animTrack->getSkeleton();
    if (!skeleton || !_boneUsed.size())
        return;

    _templateUsed.resize(count);
    ITF_MemSet(&_templateUsed[0], 0, count);

    char *pTemplateUsed = &_templateUsed[0];
    const TemplateRef* pTemplateRef = &m_templateRefs[0];
    for (u32 i=0; i<count; ++i, ++pTemplateUsed, ++pTemplateRef)
    {
        AnimTemplate *pTemplate;
        if (_animTrack->getTemplate(*pTemplateRef, pTemplate, _state))
        {
            for (u32 boneIndex=0; !*pTemplateUsed && boneIndex<pTemplate->m_BonesCurFrame.size(); ++boneIndex)
            { 
                AnimBone    *BS      = &pTemplate->m_BonesCurFrame[boneIndex];

				// compute Bone index
				ITF_ASSERT(BS);
                i32 BSIndex = skeleton->getBoneIndex(BS->m_Name);
                if (BSIndex >= 0)
                {
                    if (_boneUsed[BSIndex])
                        *pTemplateUsed = 1;
                }
            }
        }
    }
}

u32 AnimTrackBML::getCurrentPatchs(const AnimTrackExt *_animTrack, const SafeArray<char> &_templateUsed, SafeArray<AnimPatchZ> &_patchList, u32 _state)
{
    _patchList.reserve(_patchList.size() + m_templateRefs.size()*4);
    u32 count = m_templateRefs.size();
    for (u32 i=0; i<count; ++i)
    {
        if (_templateUsed.size() && !_templateUsed[i])
            continue;
        AnimTemplate *pTemplate;
        if (_animTrack->getTemplate(m_templateRefs[i], pTemplate, _state))
        {
            for (VectorAnim<AnimPatch>::iterator p = pTemplate->m_Patch.begin(); 
                p != pTemplate->m_Patch.end(); ++p)
            {
                _patchList.push_back(AnimPatchZ(&(*p)));
            }
        }
    }

    return _patchList.size();
}

void AnimTrackPolyline::serialize(ArchiveMemory& _Archive)
{
    _Archive.serialize(m_frame);
    m_polylineRefs.serialize(_Archive);
}

void AnimTrackPolyline::dynamicBonesToPolyline(AnimInfo * _animInfo, AnimTrackExt * _animTrack)
{
    u32 count = m_polylineRefs.size();
    AnimSkeleton *skeleton = _animTrack->getSkeleton();
    if (!skeleton || !count)
        return;

    SafeArray<AnimBoneDyn>  &bonesList  = _animInfo->getWorkingBoneList();
    for (u32 i=0; i<count; ++i)
    {
        AnimPolyline *pPolyline;
        if (skeleton->getPolyline(m_polylineRefs[i], pPolyline))
        {
            pPolyline->ComputeGlobalPos(&bonesList, _animInfo->getPolylinePointBuffer(m_polylineRefs[i], bfalse),_animInfo->m_globalData);
        }
    }
}


String AnimTrack::s_redirectPatchBank("redirectPatchBank.rpb");

AnimTrack::AnimTrack() :
  m_endFrame(0.f)
, m_tags(0)
, m_dummySkeleton(NULL)
, m_skeletonFlushTime(0)
, m_bonesMaxAngle(0.f)
, m_bonesMaxPos(0.f)
, m_bonesMaxScale(0.f)
{
    m_cookCallback = m_funcCookerTrack;
    m_scissor.invalidate();
    m_bounding.invalidate();
}

AnimTrack::~AnimTrack()
{
    clear();
}


void AnimTrack::clear()
{
    for (VectorAnim<AnimTrackFrameEvents>::iterator it = m_eventList.begin(); it != m_eventList.end();++it)
    {
        AnimTrackFrameEvents& frameEvents = *it;
        u32 numEvents = frameEvents.m_frameEvents.size();

        for (u32 j=0; j<numEvents; j++)
        {
            delete frameEvents.m_frameEvents[j];
        }
    }
    m_eventList.clear();

#ifdef ITF_COMPRESSED_ANIM
	if (m_compressedTrackListPAS)
	{
		ITF::Memory::free(m_compressedTrackListPAS);
		m_compressedTrackListPAS = NULL;
	}
#endif //ITF_COMPRESSED_ANIM
}

f32  AnimTrack::getDuration()
{ 
    return (m_endFrame+1.0f) * ANIMATION_FRAMERATE;
}

void AnimTrack::serialize(ArchiveMemory& _Archive, bbool _compressedData)
{
    ITF_ASSERT_CRASH(!_compressedData, "Compression no more supported");
    Versioning::serializeVersion(_Archive,Versioning::animTrackVer());

    _Archive.serialize(m_endFrame);

    m_bmlList.serialize(_Archive);

#ifdef ITF_WII
	MEM_M_PushExt(MEM_C_MEM2_ALLOC);
#endif //ITF_WII

#ifdef ITF_COMPRESSED_ANIM
	ITF_ASSERT(_compressedData && _Archive.isReading());

	_Archive.serialize(m_trackListPASSize);
	_Archive.serialize(m_trackListPASCompressedSize);

	if (m_trackListPASCompressedSize)
	{
		MEM_M_PushExt(MEM_C_MEM2_ALLOC);
		m_compressedTrackListPAS = ITF::Memory::mallocCategory(m_trackListPASCompressedSize, ITF::MemoryId::mId_Animation);
		MEM_M_PopExt();

		_Archive.serializeBlock8((u8 *)m_compressedTrackListPAS,m_trackListPASCompressedSize);

		ITF_ASSERT(m_trackListPAS.size() == 0);
	}
	else 
	{
		m_compressedTrackListPAS = NULL;
		m_trackListPAS.serialize(_Archive);
	}
#else // ITF_COMPRESSED_ANIM
    m_trackListPAS.serialize(_Archive);
#endif // ITF_COMPRESSED_ANIM

	m_trackListZAL.serialize(_Archive);

#ifdef ITF_WII
	MEM_M_PopExt();
#endif //ITF_WII

    _Archive.serialize(m_bonesMaxAngle);
    _Archive.serialize(m_bonesMaxPos);
    _Archive.serialize(m_bonesMaxScale);

    m_polylineList.serialize(_Archive);
    
    m_bonesList.serialize(_Archive);
    m_eventList.serialize(_Archive);
    m_soundList.serialize(_Archive);

    m_rootPos.serialize(_Archive);
    m_bounding.serialize(_Archive);
    m_scissor.serialize(_Archive);

    String serializeStr;
    if (!_Archive.isReading())
    {
        serializeStr = ANIM_MANAGER->getString(m_skeletonPath);
        m_skeletonPath.serialize(_Archive);
        serializeStr.serialize(_Archive);

        m_nameToPatch.serialize(_Archive);
        u32 count = m_patchBankPathList.size();
        _Archive.serialize(count);
        for (u32 i=0; i<count; ++i)
        {
            StringID strId = m_patchBankPathList[i];
            serializeStr   = ANIM_MANAGER->getString(strId);
            strId.serialize(_Archive);
            serializeStr.serialize(_Archive);
        }
    } else
    {
        m_skeletonPath.serialize(_Archive);
        serializeStr.serialize(_Archive);
        ANIM_MANAGER->setStringStringID(serializeStr, m_skeletonPath);

        m_nameToPatch.serialize(_Archive);
        u32 count;
        _Archive.serialize(count);
        m_patchBankPathList.clear();
        m_patchBankPathList.reserve(count);
        
        StringID strId;
        for (u32 i=0; i<count; ++i)
        {
            strId.serialize(_Archive);
            serializeStr.serialize(_Archive);
            m_patchBankPathList.push_back(strId);
            ANIM_MANAGER->setStringStringID(serializeStr, strId);
        }
    }
    _Archive.serialize(m_skeletonFlushTime);
    m_redirectPatchBankFlushTimeList.serialize(_Archive);
    _Archive.serialize(m_tags);
}

AnimSkeleton *AnimTrack::getSkeleton()
{
    if (m_dummySkeleton)
        return m_dummySkeleton;

    AnimSkeletonResource *res = static_cast<AnimSkeletonResource *>(m_skeletonResourceID.getResource());
    // [BEGIN] TEMPORARY CODE TO TRACK BUG
    #ifdef ENABLE_RESOURCE_HISTORY
        if (!res)
        {
            Resource::dumpResourceHistory(m_skeletonResourceID.getResourceRef());
        }
    #endif //ENABLE_RESOURCE_HISTORY
    // [END] TEMPORARY CODE TO TRACK BUG
    ITF_ASSERT_CRASH(res, "verify resource !!");

    if (res)
    {
        ITF_ASSERT_CRASH(res->getOwnerData(), "resource not loaded !");
        return res->getOwnerData();
    }
    return NULL;
}

ResourceID AnimTrack::getTexture(u32 _index)
{
    ITF_ASSERT(_index < m_textureResourceIDList.size());
    if (_index  < m_textureResourceIDList.size())
        return m_textureResourceIDList[_index];
    return ResourceID::Invalid;  
}

AnimPatchBank *AnimTrack::getPatchBank(u32 _index)
{
    ITF_ASSERT(_index < m_patchBankResourceIDList.size());
    AnimPatchBankResource *res = static_cast<AnimPatchBankResource *>(m_patchBankResourceIDList[_index].getResource());
    ITF_ASSERT_MSG(res, "verify resource !!");

    if (res)
    {
        AnimPatchBank * pbk = res->getOwnerData();
        if (pbk)
        {
            pbk->m_texture = getTexture(_index);
            return pbk;
        }
    }
    return NULL;
}

AnimTrackBoneWithLock AnimTrack::getTrackBoneFromDyn(SafeArray<AnimBoneDynWithLock> & dyn, int _index, f32 _frame)
{
    AnimSkeleton *  skeleton = getSkeleton();
    AnimBone *      pBone = skeleton->getBoneAt(_index);
    if (pBone->m_BoneParent)
    {
        dyn[pBone->getIndex()].ComputeLocal(&(dyn[pBone->m_BoneParent->getIndex()]));
    }
    else
    {
        dyn[pBone->getIndex()].ComputeLocal(NULL);
    }
    AnimBoneDynWithLock & pBoneDyn = dyn[pBone->getIndex()];
    AnimBoneDyn & tBoneDyn = skeleton->m_BonesTPauseDyn[_index];
    AnimTrackBoneWithLock trck;

    trck.m_frame = _frame;
    trck.m_PosLocal     = pBoneDyn.m_PosLocal - tBoneDyn.m_PosLocal;
    trck.m_AngleLocal   = pBoneDyn.m_AngleLocal - tBoneDyn.m_AngleLocal;
    trck.m_Scale        = pBoneDyn.m_Scale/tBoneDyn.m_Scale;
    trck.m_Zorder       = pBoneDyn.m_Zorder - tBoneDyn.m_Zorder;
    trck.m_Alpha        = pBoneDyn.m_Alpha - tBoneDyn.m_Alpha;
    trck.m_Lock         = NULL;
    return trck;
}


void AnimTrack::removeLock(VectorAnim<VectorAnimTrackBoneWithLock> & _tempTrackList, bbool useDefaultImages)
{
    SafeArray<AnimBoneDynWithLock>  dyn;
    ITF_VECTOR<AnimLock>    lock;
    SafeArray<AnimPatch*>   patches;
    AnimSkeleton *          skeleton;

    skeleton = getSkeleton();

    dyn.resize(skeleton->m_BonesCurFrame.size());
    VectorAnim<VectorAnimTrackBoneWithLock> newTrackList = _tempTrackList;

    for (f32 j=0.0f; j<=m_endFrame; ++j)
    {
        lock.clear();
#ifndef NO_RUNTIME_LOCK
        skeleton->SetTPause(&dyn);
#else
        skeleton->SetTPauseCook(&dyn);
#endif
        ComputeAnimFrameCook(_tempTrackList, &dyn, &lock, j, 1.f, btrue);

        if (lock.size() > 0)
        {
#ifndef NO_RUNTIME_LOCK
            skeleton->ComputeBonesGlobalPos(&dyn, &lock, bfalse);
            skeleton->ComputeIK(&dyn, &lock);
            skeleton->ComputeBonesGlobalPos(&dyn, &lock, bfalse);
#else
            skeleton->ComputeBonesGlobalPosCook(&dyn, &lock, bfalse);
            skeleton->ComputeIK(&dyn, &lock);
            skeleton->ComputeBonesGlobalPosCook(&dyn, &lock, bfalse);
#endif
            for (ITF_VECTOR<AnimLock>::iterator lockIter = lock.begin();
                 lockIter != lock.end(); lockIter++)
            {
                AnimBone *bone;                
                int boneIndex;

                boneIndex = (*lockIter).m_details.m_Index;
                AnimTrackBoneWithLock trck = getTrackBoneFromDyn(dyn, boneIndex, j);
                AnimTrackBonesList::setTrack(newTrackList[boneIndex], trck);

                bone  = skeleton->getBoneAt((*lockIter).m_details.m_Index);
                if (bone->m_BoneParent)
                {
                    bone        = bone->m_BoneParent;
                    boneIndex   = bone->getIndex();

                    trck        = getTrackBoneFromDyn(dyn, boneIndex, j);
                    AnimTrackBonesList::setTrack(newTrackList[boneIndex], trck);

                }

                for (u32 i=0; i<dyn.size(); i++)
                {
                    bone = skeleton->getBoneAt(i);
                    if (bone && bone->m_BoneParent && (u32)bone->m_BoneParent->getIndex() == (*lockIter).m_details.m_Index)
                    {
                        trck = getTrackBoneFromDyn(dyn, i, j);
                        AnimTrackBonesList::setTrack(newTrackList[i], trck);
                    }
                }
            }
        }
    }

    _tempTrackList = newTrackList;
}



void AnimTrack::useMagicBoxAsRoot(VectorAnim<VectorAnimTrackBoneWithLock> & _tempTrackList)
{
    AnimSkeleton *skeleton = getSkeleton();

    i32 rootIndex = skeleton->getRootIndex();
    i32 virtualRootIndex = skeleton->getBoneIndex(ITF_GET_STRINGID_CRC(MagicBox,4279953898));
    
    if (rootIndex == -1 || virtualRootIndex == -1)
        return;

    SafeArray<AnimBoneDynWithLock>  dyn;
    ITF_VECTOR<AnimLock>    lock;
    SafeArray<AnimPatch*>   patches;
    VectorAnim<VectorAnimTrackBoneWithLock> newTrackList = _tempTrackList;
    SafeArray<i32>  boneIndexToChange; // root and it's children

    u32 boneCount = skeleton->m_BonesCurFrame.size();
    dyn.resize(boneCount);
    
    boneIndexToChange.push_back(rootIndex);
    for (u32 i=0; i<boneCount; i++)
    {
        AnimBone * bonePtr = &skeleton->m_BonesCurFrame[i];
        if (bonePtr->m_BoneParent && bonePtr->m_BoneParent->getIndex() == rootIndex)
            boneIndexToChange.push_back(i);
    }


    for (f32 j=0.0f; j<=m_endFrame; ++j)
    {
        lock.clear();
#ifndef NO_RUNTIME_LOCK
        skeleton->SetTPause(&dyn);
#else
        skeleton->SetTPauseCook(&dyn);
#endif
        ComputeAnimFrameCook(_tempTrackList, &dyn, &lock, j, 1.f, btrue);

#ifndef NO_RUNTIME_LOCK
        skeleton->ComputeBonesGlobalPos(&dyn, &lock, bfalse);
        skeleton->ComputeIK(&dyn, &lock);
        skeleton->ComputeBonesGlobalPos(&dyn, &lock, bfalse);
#else
        skeleton->ComputeBonesGlobalPosCook(&dyn, &lock, bfalse);
        skeleton->ComputeIK(&dyn, &lock);
        skeleton->ComputeBonesGlobalPosCook(&dyn, &lock, bfalse);
#endif
        // move root to Virtual Root
        dyn[rootIndex].m_Pos    = dyn[virtualRootIndex].m_Pos;
        dyn[rootIndex].m_Angle  = dyn[virtualRootIndex].m_Angle;
        
        for (u32 k=0; k<boneIndexToChange.size(); k++)
        {
            AnimTrackBoneWithLock trck = getTrackBoneFromDyn(dyn, boneIndexToChange[k], j);
            AnimTrackBonesList::setTrack(newTrackList[boneIndexToChange[k]], trck);
        }
    }

    _tempTrackList = newTrackList;
}


// To a simple version of animation setFrame
void AnimTrack::processTagAndBounding(bbool saveRootPos, bbool _addSkeletonABB)
{
    bbool           AABBSet = m_bounding.isValid();
    bbool           useAlpha= bfalse;
    m_tags  = 0;
    
    SafeArray<AnimBoneDyn>  dyn;
    SafeArray<AnimPatch*>   patches;
    AnimSkeleton *          skeleton;
    AnimGlobalData          globalData;

#ifndef NO_RUNTIME_LOCK
    ITF_VECTOR<AnimLock>    lock;
#endif

    skeleton = getSkeleton();

    u32 rootIndex = skeleton->getRootIndex();
    u32 u32EndFrame = (u32)m_endFrame;
    dyn.resize(skeleton->m_BonesCurFrame.size());
#ifdef ANIM_TRACK_CACHE
    SAFEARRAY_CACHE_TRCKBONE_PASCUT pasCache;
    pasCache.resize(skeleton->m_BonesCurFrame.size()*ANIM_TRACK_CACHE_SIZE);
    for (u32 i=0; i<pasCache.size(); i++)
    {
        pasCache[i].m_frame = -1.f;
    }
#endif

    if (saveRootPos)
        m_rootPos.resize(u32EndFrame+1);

    for (f32 j=0.0f; j<=m_endFrame; ++j)
    {
        skeleton->SetTPause(&dyn);
#ifdef ANIM_TRACK_CACHE
    #ifndef NO_RUNTIME_LOCK
        lock.clear();
        ComputeAnimFrame(&dyn, pasCache, &lock, j, 1.f, btrue, NULL, bfalse);

        if (lock.size() > 0)
        {
            skeleton->ComputeBonesGlobalPos(&dyn, &lock);
            skeleton->ComputeIK(&dyn, &lock);
        }
        skeleton->ComputeBonesGlobalPos(&dyn, &lock);
        skeleton->ComputeBonesEnd(&dyn, true);
    #else
        ComputeAnimFrame(&dyn, pasCache, j, 1.f, btrue, NULL, bfalse);
        skeleton->ComputeBonesGlobalPos(&dyn);
        skeleton->ComputeBonesEnd(&dyn, true);
    #endif
#else
    #ifndef NO_RUNTIME_LOCK
        lock.clear();
        ComputeAnimFrame(&dyn, &lock, j, 1.f, btrue, NULL, bfalse);

        if (lock.size() > 0)
        {
            skeleton->ComputeBonesGlobalPos(&dyn, &lock);
            skeleton->ComputeIK(&dyn, &lock);
        }
        skeleton->ComputeBonesGlobalPos(&dyn, &lock);
        skeleton->ComputeBonesEnd(&dyn, true);
    #else
        ComputeAnimFrame(&dyn, j, 1.f, btrue, NULL, bfalse);
        skeleton->ComputeBonesGlobalPos(&dyn);
        skeleton->ComputeBonesEnd(&dyn, true);
    #endif
#endif
        // force root pos to 0
        Vec2d move = dyn[rootIndex].m_Pos * -1.f;
        if (saveRootPos)
            m_rootPos[(u32)j] = dyn[rootIndex].m_Pos;
        skeleton->Translate(&dyn, move, globalData);

        if (j == 0.f && f32_Cos(dyn[rootIndex].m_Angle) > 0.f)
        {
            m_tags = m_tags | ANIM_TRACK_IS_DEFAULT_RIGHT;
        }
        u32 dynCount =  dyn.size();
        for (u32 d=0; d<dynCount; d++)
        {
            AnimBoneDyn * pBoneDyn = &dyn[d];
            if (_addSkeletonABB)
            {
                if (!AABBSet)
                {
                    m_bounding = AABB(pBoneDyn->m_Pos);
                    m_bounding.grow(pBoneDyn->m_PosEnd);
                    AABBSet = btrue;
                } else
                {
                    m_bounding.grow(pBoneDyn->m_Pos);
                    m_bounding.grow(pBoneDyn->m_PosEnd);
                }
            }

            useAlpha = useAlpha || pBoneDyn->m_Alpha < 0.99999f;
        }
    }

    if (useAlpha)
    {
        m_tags = m_tags | ANIM_TRACK_USE_ALPHA;
    }
}


//////////////////////////////////////////////////////////////
////// ANIM
//////////////////////////////////////////////////////////////
ITF_INLINE bbool UpdateBoneWithLock(const AnimBone *_bone, AnimBoneDynWithLock *_boneDyn, const AnimTrackBoneWithLock *_trackBone, ITF_VECTOR<AnimLock> *lock_list, f32 percent, bbool isMain)
{
    STATS_ANIM_UPDATEDBONESINC;

    f32 oldZorder           = _boneDyn->m_Zorder;
    _boneDyn->m_LockedIndex = U32_INVALID;
    if (percent == 1.f) // special case for 1, most common with less cost
    {
        // Dynamic part, may grow
        _boneDyn->m_PosLocal           += _trackBone->m_PosLocal;
        _boneDyn->m_AngleLocal         += _trackBone->m_AngleLocal;
        _boneDyn->m_Alpha              += _trackBone->m_Alpha;
        _boneDyn->m_Scale              *= _trackBone->m_Scale;
    } else
    {
        Vec2d::ScaleAdd(&_boneDyn->m_PosLocal, &_trackBone->m_PosLocal, percent, &_boneDyn->m_PosLocal);
        _boneDyn->m_AngleLocal         += _trackBone->m_AngleLocal*percent;
        _boneDyn->m_Alpha              += _trackBone->m_Alpha*percent;
        _boneDyn->m_Scale.m_x          *= powf(f32_Abs(_trackBone->m_Scale.m_x), percent);
        _boneDyn->m_Scale.m_y          *= powf(f32_Abs(_trackBone->m_Scale.m_y), percent);
    }

    // Used to calculate Global Pos so, no need to save
    if (isMain)
    {
        _boneDyn->m_Zorder             += _trackBone->m_Zorder;
        if (_boneDyn->m_Scale.m_x * _trackBone->m_Scale.m_x < 0.f) _boneDyn->m_Scale.m_x *= -1.f;
        if (_boneDyn->m_Scale.m_y * _trackBone->m_Scale.m_y < 0.f) _boneDyn->m_Scale.m_y *= -1.f;
        if (_trackBone->m_Lock)
        {
            AnimLock anmLock;
            anmLock.m_details.m_Index         = _bone->getIndex();
            anmLock.m_details.m_OrderedIndex  = _bone->m_OrderedIndex;
            anmLock.m_Locked                  = _trackBone->m_Lock->m_Locked;
            anmLock.m_LockedAngle             = _trackBone->m_Lock->m_LockedAngle;
            anmLock.m_details.m_AngleSign     = _trackBone->m_Lock->m_details.m_AngleSign;

            lock_list->push_back(anmLock);
            _boneDyn->m_LockedIndex = lock_list->size() - 1;
        }
    }

    return oldZorder != _boneDyn->m_Zorder;
}

ITF_INLINE bbool UpdateBoneWithoutLock(const AnimBone *_bone, AnimBoneDyn *_boneDyn, const AnimTrackBone *_trackBone, f32 percent, bbool isMain)
{
    STATS_ANIM_UPDATEDBONESINC;

    f32 oldZorder           = _boneDyn->m_Zorder;
    if (percent == 1.f) // special case for 1, most common with less cost
    {
        // Dynamic part, may grow
        _boneDyn->m_PosLocal           += _trackBone->m_PosLocal;
        _boneDyn->m_AngleLocal         += _trackBone->m_AngleLocal;
        _boneDyn->m_Alpha              += _trackBone->m_Alpha;
        _boneDyn->m_Scale              *= _trackBone->m_Scale;
    } else
    {
        Vec2d::ScaleAdd(&_boneDyn->m_PosLocal, &_trackBone->m_PosLocal, percent, &_boneDyn->m_PosLocal);
        _boneDyn->m_AngleLocal         += _trackBone->m_AngleLocal*percent;
        _boneDyn->m_Alpha              += _trackBone->m_Alpha*percent;
        _boneDyn->m_Scale.m_x          *= powf(f32_Abs(_trackBone->m_Scale.m_x), percent);
        _boneDyn->m_Scale.m_y          *= powf(f32_Abs(_trackBone->m_Scale.m_y), percent);
    }

    // Used to calculate Global Pos so, no need to save
    if (isMain)
    {
        _boneDyn->m_Zorder             += _trackBone->m_Zorder;
        if (_boneDyn->m_Scale.m_x * _trackBone->m_Scale.m_x < 0.f) _boneDyn->m_Scale.m_x *= -1.f;
        if (_boneDyn->m_Scale.m_y * _trackBone->m_Scale.m_y < 0.f) _boneDyn->m_Scale.m_y *= -1.f;
    }

    return oldZorder != _boneDyn->m_Zorder;
}

ITF_INLINE bbool BlendAnims(AnimTrackBoneWithLock *_trackBone1, AnimTrackBoneWithLock *_trackBone2, f32 factor, AnimTrackBoneWithLock *_trackBoneDst, AnimLock *_lock)
{
    float opFactor  = (1.0f-factor);
    Vec2d::Scale(&_trackBoneDst->m_PosLocal, &_trackBone1->m_PosLocal, opFactor);
    Vec2d::ScaleAdd(&_trackBoneDst->m_PosLocal, &_trackBone2->m_PosLocal, factor, &_trackBoneDst->m_PosLocal);
    _trackBoneDst->m_AngleLocal     = _trackBone1->m_AngleLocal*opFactor + _trackBone2->m_AngleLocal*factor;
    Vec2d::Scale(&_trackBoneDst->m_Scale, &_trackBone1->m_Scale, opFactor);
    Vec2d::ScaleAdd(&_trackBoneDst->m_Scale, &_trackBone2->m_Scale, factor, &_trackBoneDst->m_Scale);
    _trackBoneDst->m_Zorder         = _trackBone1->m_Zorder*opFactor + _trackBone2->m_Zorder*factor;
    _trackBoneDst->m_Alpha          = _trackBone1->m_Alpha*opFactor + _trackBone2->m_Alpha*factor;
    if (_trackBone1->m_Lock && _trackBone2->m_Lock)
    {
        _lock->m_details.m_AngleSign    = _trackBone1->m_Lock->m_details.m_AngleSign;
        Vec2d::Scale(&_lock->m_Locked, &_trackBone1->m_Lock->m_Locked, opFactor);
        Vec2d::ScaleAdd(&_lock->m_Locked, &_trackBone2->m_Lock->m_Locked, factor, &_lock->m_Locked);
        _lock->m_LockedAngle  =         _trackBone1->m_Lock->m_LockedAngle*opFactor + _trackBone2->m_Lock->m_LockedAngle*factor;

        _trackBoneDst->m_Lock       = _lock;
    }else if (_trackBone1->m_Lock)
    {
        _trackBoneDst->m_Lock       = _trackBone1->m_Lock;
    } else
        _trackBoneDst->m_Lock       = NULL;
    return bfalse;
}

#ifdef ANIM_TRACK_CACHE
#ifndef NO_RUNTIME_LOCK
bbool AnimTrack::fillTrackAtIndex(AnimTrackBone & track, AnimTrackBonePASCut * _pasCache, const AnimTrackBonesList & animBoneLst, f32 index, AnimLock *lock )
{
    if (animBoneLst.m_trackPASSize == 0 &&  animBoneLst.m_trackZALSize == 0)
        return bfalse;

    u16                 uIndex = (u16)index;
    Vec2d               PB, PE, SB, SE;

    // search current index with dichotomy
    if (animBoneLst.m_trackPASSize > 0)
    {
        int cacheIdx = 0;
        for (; cacheIdx<ANIM_TRACK_CACHE_SIZE-1; cacheIdx++)
        {
            AnimTrackBonePASCut & pasCache0 = _pasCache[cacheIdx];
            AnimTrackBonePASCut & pasCache1 = _pasCache[cacheIdx+1];
            if (pasCache0.m_frame < 0)
            {
                cacheIdx = ANIM_TRACK_CACHE_SIZE-1;
                break;
            }
            if (pasCache1.m_frame < 0)
            {
                if (pasCache0.m_frame <= index)
                {
                    track.m_PosLocal    = pasCache0.m_PosLocal;
                    track.m_AngleLocal  = pasCache0.m_AngleLocal;
                    track.m_Scale       = pasCache0.m_Scale;
                    break;
                }
            } else
            {
                if (pasCache0.m_frame <= index && index <= pasCache1.m_frame )
                {
                    if (pasCache0.m_frame == index)
                    {
                        track.m_PosLocal    = pasCache0.m_PosLocal;
                        track.m_AngleLocal  = pasCache0.m_AngleLocal;
                        track.m_Scale       = pasCache0.m_Scale;
                    } else if (pasCache1.m_frame == index)
                    {
                        track.m_PosLocal    = pasCache1.m_PosLocal;
                        track.m_AngleLocal  = pasCache1.m_AngleLocal;
                        track.m_Scale       = pasCache1.m_Scale;
                    } else
                    {
                        f32 factor    = (index - pasCache0.m_frame) / (pasCache1.m_frame - pasCache0.m_frame);
                        f32 opFactor  = (1.0f-factor);

                        track.m_PosLocal    = pasCache0.m_PosLocal * opFactor + pasCache1.m_PosLocal * factor;
                        track.m_AngleLocal  = pasCache0.m_AngleLocal * opFactor + pasCache1.m_AngleLocal * factor;
                        track.m_Scale       = pasCache0.m_Scale * opFactor + pasCache1.m_Scale * factor;
                    }
                    break;
                }
            }
        }

        if (cacheIdx == ANIM_TRACK_CACHE_SIZE-1)
        {
            u32             begin = animBoneLst.m_trackPASIndex;
            u32             end   = begin + animBoneLst.m_trackPASSize - 1;
            u32             lastPos = end;

            while (end - begin > 1)
            {
                u32 center = (begin + end)/2;
                if (m_trackListPAS[center].m_frame < uIndex)
                {
                    begin = center;
                } else
                {
                    end = center;
                }
            }

            u32 pos = begin;
            int cacheIdx = 0;
            AnimTrackBonePASCut *pasCache = _pasCache;
            for (; cacheIdx<ANIM_TRACK_CACHE_SIZE && pos <= lastPos; cacheIdx++, pos++, pasCache++)
            {
                AnimTrackBonePAS &pas = m_trackListPAS[pos];
                pasCache->m_frame = pas.m_frame;
                pas.getPos(&pasCache->m_PosLocal, m_bonesMaxPos);
                pasCache->m_AngleLocal  = pas.getAngle(m_bonesMaxAngle);
                pas.getScale(&pasCache->m_Scale, m_bonesMaxScale);
            }
            AnimTrackBonePASCut & pasCache0 = _pasCache[0];
            AnimTrackBonePASCut & pasCache1 = cacheIdx > 1 ? _pasCache[1] : _pasCache[0];

            for (; cacheIdx<ANIM_TRACK_CACHE_SIZE; cacheIdx++, pasCache++)
            {
                pasCache->m_frame = -1.f;
            }

            if (pasCache0.m_frame >= index)
            {
                track.m_PosLocal    = pasCache0.m_PosLocal;
                track.m_AngleLocal  = pasCache0.m_AngleLocal;
                track.m_Scale       = pasCache0.m_Scale;
            } else if (pasCache1.m_frame <= index)
            {
                track.m_PosLocal    = pasCache1.m_PosLocal;
                track.m_AngleLocal  = pasCache1.m_AngleLocal;
                track.m_Scale       = pasCache1.m_Scale;
            } else
            {
                f32 factor    = (index - pasCache0.m_frame) / (pasCache1.m_frame - pasCache0.m_frame);
                f32 opFactor  = (1.0f-factor);

                track.m_PosLocal    = pasCache0.m_PosLocal * opFactor + pasCache1.m_PosLocal * factor;
                track.m_AngleLocal  = pasCache0.m_AngleLocal * opFactor + pasCache1.m_AngleLocal * factor;
                track.m_Scale       = pasCache0.m_Scale * opFactor + pasCache1.m_Scale * factor;
            }
        }
    }

    if (animBoneLst.m_trackZALSize > 0)
    {
        u32             begin = animBoneLst.m_trackZALIndex;
        u32             end   = begin + animBoneLst.m_trackZALSize - 1;

        while (end - begin > 1)
        {
            u32 center = (begin + end)/2;
            if (m_trackListZAL[center].m_frame < uIndex)
            {
                begin = center;
            } else
            {
                end = center;
            }
        }


        if (m_trackListZAL[begin].m_frame >= index)
        {
            AnimTrackBoneZAL &zal = m_trackListZAL[begin];
            track.m_Alpha       = (f32)zal.m_Alpha / 255.f;
            track.m_Zorder      = zal.m_Zorder;
            track.m_Lock        = zal.m_Lock;
        } else if (m_trackListZAL[end].m_frame <= index)
        {
            AnimTrackBoneZAL &zal = m_trackListZAL[end];
            track.m_Alpha       = (f32)zal.m_Alpha / 255.f;
            track.m_Zorder      = zal.m_Zorder;
            track.m_Lock        = zal.m_Lock;
        } else
        {
            AnimTrackBoneZAL &zalBegin  = m_trackListZAL[begin];
            AnimTrackBoneZAL &zalEnd    = m_trackListZAL[end];

            f32 factor    = (index - zalBegin.m_frame) / (zalEnd.m_frame - zalBegin.m_frame);
            f32 opFactor  = (1.0f-factor);


            track.m_Zorder         = zalBegin.m_Zorder*opFactor + zalEnd.m_Zorder*factor;
            track.m_Alpha          = ((f32)zalBegin.m_Alpha * opFactor + (f32)zalEnd.m_Alpha *factor) / 255;
            if (zalBegin.m_Lock && zalEnd.m_Lock && lock)
            {
                lock->m_details.m_AngleSign    = zalBegin.m_Lock->m_details.m_AngleSign;
                Vec2d::Scale(&lock->m_Locked, &zalBegin.m_Lock->m_Locked, opFactor);
                Vec2d::ScaleAdd(&lock->m_Locked, &zalEnd.m_Lock->m_Locked, factor, &lock->m_Locked);
                lock->m_LockedAngle            = zalBegin.m_Lock->m_LockedAngle*opFactor + zalEnd.m_Lock->m_LockedAngle*factor;

                track.m_Lock                    = lock;
            }else if (zalBegin.m_Lock)
            {
                track.m_Lock                    = zalBegin.m_Lock;
            } else
                track.m_Lock                    = NULL;
        }
    }
    return btrue;
}

void AnimTrack::ComputeAnimFrame(SafeArray<AnimBoneDyn> *_dyn,SAFEARRAY_CACHE_TRCKBONE_PASCUT & _pasCache, ITF_VECTOR<AnimLock> *_lock, f32 index, f32 percent, bbool isMain, const AnimFrameInfo::BonesArray* _templateUsed, bbool _minimalBones)
{
    ITF_ASSERT(_dyn && _lock);

    AnimSkeleton * skeleton = getSkeleton();
    ITF_ASSERT(skeleton);
    if (!skeleton)
        return;

    AnimLock            lock;

    if (_minimalBones)
    {
        u32 count = skeleton->m_BonesMinimumUpdateOrdered.size();
        for (u32 minI=0; minI<count; ++minI)
        {
            int i = skeleton->m_BonesMinimumUpdateOrdered[minI]->getIndex();
            if (_templateUsed && !(*_templateUsed)[i])
                continue;

            AnimTrackBone       track;

            if (!fillTrackAtIndex(track, &_pasCache[i*ANIM_TRACK_CACHE_SIZE], m_bonesList[i], index, &lock ))
                continue;

            AnimBoneDyn     *pBoneDyn = &(*_dyn)[i];
            UpdateBoneWithLock(skeleton->m_BonesCurFrame[i], pBoneDyn, &track, _lock, percent, isMain);
            track.m_Lock = NULL; // force null to avoid deleting a non allocated Lock
        }
    } else
    {
        VectorAnim<AnimTrackBonesList>::iterator    animBone    = m_bonesList.begin();
        VectorAnim<AnimBone*>::iterator             Bone        = skeleton->m_BonesCurFrame.begin();

        u32 count = _dyn->size();
        for (u32 i=0; i<count && animBone != m_bonesList.end(); ++animBone, ++Bone, ++i)
        {
            if (_templateUsed && !(*_templateUsed)[i])
                continue;

            AnimTrackBone       track;

            if (!fillTrackAtIndex(track, &_pasCache[i*ANIM_TRACK_CACHE_SIZE], (*animBone), index, &lock ))
                continue;

            AnimBoneDyn     *pBoneDyn = &(*_dyn)[i];
            UpdateBoneWithLock((*Bone), pBoneDyn, &track, _lock, percent, isMain);
            track.m_Lock = NULL; // force null to avoid deleting a non allocated Lock
        }
    }
}

#else
#ifndef ITF_WII
ITF_INLINE void CacheInterpolate(AnimTrackBone & track,AnimTrackBonePASCut & pasCache0,AnimTrackBonePASCut & pasCache1,float factor)
{
	f32 opFactor  = (1.0f-factor);
    track.m_PosLocal    = pasCache0.m_PosLocal * opFactor + pasCache1.m_PosLocal * factor;
    track.m_AngleLocal  = pasCache0.m_AngleLocal * opFactor + pasCache1.m_AngleLocal * factor;
    track.m_Scale       = pasCache0.m_Scale * opFactor + pasCache1.m_Scale * factor;
}
#else
ITF_INLINE void CacheInterpolate(AnimTrackBone & track,AnimTrackBonePASCut & pasCache0,AnimTrackBonePASCut & pasCache1,float factor)
{
	f32 opFactor  = (1.0f-factor);
	*((ps::Float2 *)&track.m_PosLocal)    = *((ps::Float2 *)&pasCache0.m_PosLocal) * opFactor + *((ps::Float2 *)&pasCache1.m_PosLocal) * factor;
    *((ps::Float2 *)&track.m_Scale)       = *((ps::Float2 *)&pasCache0.m_Scale) * opFactor + *((ps::Float2 *)&pasCache1.m_Scale) * factor;
    track.m_AngleLocal  = pasCache0.m_AngleLocal * opFactor + pasCache1.m_AngleLocal * factor;
}
#endif
bbool AnimTrack::fillTrackAtIndex(AnimTrackBone & track, AnimTrackBonePASCut * _pasCache, const AnimTrackBonesList & animBoneLst, f32 index )
{
    if (animBoneLst.m_trackPASSize == 0 &&  animBoneLst.m_trackZALSize == 0)
        return bfalse;

    u16                 uIndex = (u16)index;
    Vec2d               PB, PE, SB, SE;

#ifdef ITF_COMPRESSED_ANIM
	ANIM_MANAGER->UseTrack(this);
#endif //ITF_COMPRESSED_ANIM

	// search current index with dichotomy
	if (animBoneLst.m_trackPASSize > 0)
    {
        int cacheIdx = 0;
		u32 GuessedIndex = 0;
		{
			AnimTrackBonePASCut * pasCache0 = _pasCache;
			for (; cacheIdx<ANIM_TRACK_CACHE_SIZE-1; cacheIdx++,pasCache0++)
			{
				AnimTrackBonePASCut * pasCache1 = pasCache0+1;
				GuessedIndex = pasCache0->OriginalTrackIndex;
				if (pasCache0->m_frame < 0)
				{
					cacheIdx = ANIM_TRACK_CACHE_SIZE-1;
					break;
				}
				if (pasCache1->m_frame < 0)
				{
					if (pasCache0->m_frame <= index)
					{
						track.m_PosLocal    = pasCache0->m_PosLocal;
						track.m_AngleLocal  = pasCache0->m_AngleLocal;
						track.m_Scale       = pasCache0->m_Scale;
						break;
					}
				} else
				{
					if (pasCache0->m_frame <= index && index <= pasCache1->m_frame )
					{
						if (pasCache0->m_frame == index)
						{
							track.m_PosLocal    = pasCache0->m_PosLocal;
							track.m_AngleLocal  = pasCache0->m_AngleLocal;
							track.m_Scale       = pasCache0->m_Scale;
						} else if (pasCache1->m_frame == index)
						{
							track.m_PosLocal    = pasCache1->m_PosLocal;
							track.m_AngleLocal  = pasCache1->m_AngleLocal;
							track.m_Scale       = pasCache1->m_Scale;
						} else
						{
							f32 factor    = (index - pasCache0->m_frame) / (pasCache1->m_frame - pasCache0->m_frame);
							CacheInterpolate(track,*pasCache0,*pasCache1,factor);
						}
						break;
					}
				}
			}
		}

        if (cacheIdx == ANIM_TRACK_CACHE_SIZE-1)
        {
            u32             begin   = animBoneLst.m_trackPASIndex;
            u32             end     = begin + animBoneLst.m_trackPASSize - 1;
            u32             lastPos = end;
			if (((int)GuessedIndex>=(int)begin) && (GuessedIndex +1 < end))
		    {
			    if (m_trackListPAS[GuessedIndex].m_frame <= uIndex)
			    {
				    begin = GuessedIndex;
			    }
			    if (m_trackListPAS[GuessedIndex+1].m_frame >= uIndex) 
			    {
				    end = GuessedIndex+1;
			    }
		    }

            while (end - begin > 1)
            {
                u32 center = (begin + end)/2;
                if (m_trackListPAS[center].m_frame < uIndex)
                {
                    begin = center;
                } else
                {
                    end = center;
                }
            }

            u32 pos = begin;
            int cacheIdx = 0;
            AnimTrackBonePASCut *pasCache = _pasCache;
            for (; cacheIdx<ANIM_TRACK_CACHE_SIZE && pos <= lastPos; cacheIdx++, pos++, pasCache++)
            {
                AnimTrackBonePAS &pas = m_trackListPAS[pos];
                pasCache->m_frame = pas.m_frame;
                pas.getPos(&pasCache->m_PosLocal, m_bonesMaxPos);
                pasCache->m_AngleLocal  = pas.getAngle(m_bonesMaxAngle);
                pas.getScale(&pasCache->m_Scale, m_bonesMaxScale);
				pasCache->OriginalTrackIndex = pos;
            }
            AnimTrackBonePASCut & pasCache0 = _pasCache[0];
            AnimTrackBonePASCut & pasCache1 = cacheIdx > 1 ? _pasCache[1] : _pasCache[0];

            for (; cacheIdx<ANIM_TRACK_CACHE_SIZE; cacheIdx++, pasCache++)
            {
                pasCache->m_frame = -1.f;
            }

            if (pasCache0.m_frame >= index)
            {
                track.m_PosLocal    = pasCache0.m_PosLocal;
                track.m_AngleLocal  = pasCache0.m_AngleLocal;
                track.m_Scale       = pasCache0.m_Scale;
            } else if (pasCache1.m_frame <= index)
            {
                track.m_PosLocal    = pasCache1.m_PosLocal;
                track.m_AngleLocal  = pasCache1.m_AngleLocal;
                track.m_Scale       = pasCache1.m_Scale;
            } else
            {
                f32 factor    = (index - pasCache0.m_frame) / (pasCache1.m_frame - pasCache0.m_frame);
                

				CacheInterpolate(track,pasCache0,pasCache1,factor);

            }
        }
    }

    if (animBoneLst.m_trackZALSize > 0)
    {
        u32             begin = animBoneLst.m_trackZALIndex;
        u32             end   = begin + animBoneLst.m_trackZALSize - 1;
		u32 GuessedIndex = _pasCache[0].OriginalTrackIndex;
		if (((int)GuessedIndex>=(int)begin) && (GuessedIndex + 1 < end))
		{
			if (m_trackListZAL[GuessedIndex].m_frame <= uIndex)
			{
				begin = GuessedIndex;
			}
			if (m_trackListZAL[GuessedIndex+1].m_frame > uIndex) 
			{
				end = GuessedIndex+1;
			}
		}

        while (end - begin > 1)
        {
            u32 center = (begin + end)/2;
            if (m_trackListZAL[center].m_frame < uIndex)
            {
                begin = center;
            } else
            {
                end = center;
            }
        }

		_pasCache[0].OriginalTrackIndex = begin;

        if (m_trackListZAL[begin].m_frame >= index)
        {
            AnimTrackBoneZAL &zal = m_trackListZAL[begin];
            track.m_Alpha       = (f32)zal.m_Alpha * (1.0f / 255.f);
            track.m_Zorder      = zal.m_Zorder;
        } else if (m_trackListZAL[end].m_frame <= index)
        {
            AnimTrackBoneZAL &zal = m_trackListZAL[end];
            track.m_Alpha       = (f32)zal.m_Alpha * (1.0f / 255.f);
            track.m_Zorder      = zal.m_Zorder;
        } else
        {
            AnimTrackBoneZAL &zalBegin  = m_trackListZAL[begin];
            AnimTrackBoneZAL &zalEnd    = m_trackListZAL[end];

            f32 factor    = (index - zalBegin.m_frame) / (zalEnd.m_frame - zalBegin.m_frame);
            f32 opFactor  = (1.0f-factor);


            track.m_Zorder         = zalBegin.m_Zorder*opFactor + zalEnd.m_Zorder*factor;
            track.m_Alpha          = ((f32)zalBegin.m_Alpha * opFactor + (f32)zalEnd.m_Alpha *factor) * (1.0f / 255.f);
        }
    }//*/
    return btrue;
}

void AnimTrack::ComputeAnimFrame(SafeArray<AnimBoneDyn> *_dyn,SAFEARRAY_CACHE_TRCKBONE_PASCUT & _pasCache, f32 index, f32 percent, bbool isMain, const AnimFrameInfo::BonesArray* _templateUsed, bbool _minimalBones)
{
    ITF_ASSERT(_dyn);

    AnimSkeleton * skeleton = getSkeleton();
    ITF_ASSERT(skeleton);
    if (!skeleton)
        return;

    if (_minimalBones)
    {
        u32 count = skeleton->m_BonesMinimumUpdateOrdered.size();
        for (u32 minI=0; minI<count; ++minI)
        {
            int i = skeleton->m_BonesMinimumUpdateOrdered[minI]->getIndex();
            if (_templateUsed && !(*_templateUsed)[i])
                continue;

            AnimTrackBone       track;

            if (!fillTrackAtIndex(track, &_pasCache[i*ANIM_TRACK_CACHE_SIZE], m_bonesList[i], index ))
                continue;

            AnimBoneDyn     *pBoneDyn = &(*_dyn)[i];
            UpdateBoneWithoutLock(&skeleton->m_BonesCurFrame[i], pBoneDyn, &track, percent, isMain);
        }
    } else
    {
        VectorAnim<AnimTrackBonesList>::iterator    animBone    = m_bonesList.begin();
        VectorAnim<AnimBone>::iterator              Bone        = skeleton->m_BonesCurFrame.begin();

        u32 count = _dyn->size();
        for (u32 i=0; i<count && animBone != m_bonesList.end(); ++animBone, ++Bone, ++i)
        {
            if (_templateUsed && !(*_templateUsed)[i])
                continue;

            AnimTrackBone       track;

            if (!fillTrackAtIndex(track, &_pasCache[i*ANIM_TRACK_CACHE_SIZE], (*animBone), index ))
                continue;

            AnimBoneDyn     *pBoneDyn = &(*_dyn)[i];
            UpdateBoneWithoutLock(&(*Bone), pBoneDyn, &track, percent, isMain);
        }
    }
}

#endif
#else
#ifndef NO_RUNTIME_LOCK
bbool AnimTrack::fillTrackAtIndex(AnimTrackBone & track, const AnimTrackBonesList & animBoneLst, f32 index, AnimLock *lock )
{
    if (animBoneLst.m_trackPASSize == 0 &&  animBoneLst.m_trackZALSize == 0)
        return bfalse;

    u16                 uIndex = (u16)index;
    Vec2d               PB, PE, SB, SE;

    // search current index with dichotomy
    if (animBoneLst.m_trackPASSize > 0)
    {
        u32             begin = animBoneLst.m_trackPASIndex;
        u32             end   = begin + animBoneLst.m_trackPASSize - 1;

        while (end - begin > 1)
        {
            u32 center = (begin + end)/2;
            if (m_trackListPAS[center].m_frame < uIndex)
            {
                begin = center;
            } else
            {
                end = center;
            }
        }

        if (m_trackListPAS[begin].m_frame >= index)
        {
            const AnimTrackBonePAS &pas = m_trackListPAS[begin];
            pas.getPos(&track.m_PosLocal, m_bonesMaxPos);
            track.m_AngleLocal  = pas.getAngle(m_bonesMaxAngle);
            pas.getScale(&track.m_Scale, m_bonesMaxScale);
        } else if (m_trackListPAS[end].m_frame <= index)
        {
            const AnimTrackBonePAS &pas = m_trackListPAS[end];
            pas.getPos(&track.m_PosLocal, m_bonesMaxPos);
            track.m_AngleLocal  = pas.getAngle(m_bonesMaxAngle);
            pas.getScale(&track.m_Scale, m_bonesMaxScale);
        } else
        {
            const AnimTrackBonePAS &pasBegin  = m_trackListPAS[begin];
            const AnimTrackBonePAS &pasEnd    = m_trackListPAS[end];

            f32 factor    = (index - pasBegin.m_frame) / (pasEnd.m_frame - pasBegin.m_frame);
            f32 opFactor  = (1.0f-factor);


            //track.m_PosLocal    = pasBegin.getPos(m_bonesMaxPos)     * opFactor + pasEnd.getPos(m_bonesMaxPos) * factor;
            PB = Vec2d(pasBegin.m_PosLocalX, pasBegin.m_PosLocalY);                
            PE = Vec2d(pasEnd.  m_PosLocalX, pasEnd.  m_PosLocalY);
            Vec2d::Scale(&PB, &PB, opFactor*m_bonesMaxPos*RATIO_DIV_I16);
            Vec2d::ScaleAdd(&track.m_PosLocal, &PE, factor*m_bonesMaxPos*RATIO_DIV_I16, &PB);

            track.m_AngleLocal  = pasBegin.getAngle(m_bonesMaxAngle) * opFactor + pasEnd.getAngle(m_bonesMaxAngle) * factor;

            //track.m_Scale       = pasBegin.getScale(m_bonesMaxScale) * opFactor + pasEnd.getScale(m_bonesMaxScale) * factor;
            SB = Vec2d(pasBegin.m_ScaleX, pasBegin.m_ScaleY);                
            SE = Vec2d(pasEnd.  m_ScaleX, pasEnd.  m_ScaleY);
            Vec2d::Scale(&SB, &SB, opFactor*m_bonesMaxScale*RATIO_DIV_I16);
            Vec2d::ScaleAdd(&track.m_Scale, &SE, factor*m_bonesMaxScale*RATIO_DIV_I16, &SB);
        }
    }
    else
    {
        track.m_PosLocal   = Vec2d::Zero;
        track.m_AngleLocal = 0.0f;
        track.m_Scale      = Vec2d::One;
    }

    if (animBoneLst.m_trackZALSize > 0)
    {
        u32             begin = animBoneLst.m_trackZALIndex;
        u32             end   = begin + animBoneLst.m_trackZALSize - 1;

        while (end - begin > 1)
        {
            u32 center = (begin + end)/2;
            if (m_trackListZAL[center].m_frame < uIndex)
            {
                begin = center;
            } else
            {
                end = center;
            }
        }


        if (m_trackListZAL[begin].m_frame >= index)
        {
            AnimTrackBoneZAL &zal = m_trackListZAL[begin];
            track.m_Alpha       = (f32)zal.m_Alpha / 255.f;
            track.m_Zorder      = zal.m_Zorder;
            track.m_Lock        = zal.m_Lock;
        } else if (m_trackListZAL[end].m_frame <= index)
        {
            AnimTrackBoneZAL &zal = m_trackListZAL[end];
            track.m_Alpha       = (f32)zal.m_Alpha / 255.f;
            track.m_Zorder      = zal.m_Zorder;
            track.m_Lock        = zal.m_Lock;
        } else
        {
            AnimTrackBoneZAL &zalBegin  = m_trackListZAL[begin];
            AnimTrackBoneZAL &zalEnd    = m_trackListZAL[end];

            f32 factor    = (index - zalBegin.m_frame) / (zalEnd.m_frame - zalBegin.m_frame);
            f32 opFactor  = (1.0f-factor);


            track.m_Zorder         = zalBegin.m_Zorder*opFactor + zalEnd.m_Zorder*factor;
            track.m_Alpha          = ((f32)zalBegin.m_Alpha * opFactor + (f32)zalEnd.m_Alpha *factor) / 255;
           /* if (zalBegin.m_Lock && zalEnd.m_Lock && lock)
            {
                lock->m_details.m_AngleSign    = zalBegin.m_Lock->m_details.m_AngleSign;
                Vec2d::Scale(&lock->m_Locked, &zalBegin.m_Lock->m_Locked, opFactor);
                Vec2d::ScaleAdd(&lock->m_Locked, &zalEnd.m_Lock->m_Locked, factor, &lock->m_Locked);
                lock->m_LockedAngle            = zalBegin.m_Lock->m_LockedAngle*opFactor + zalEnd.m_Lock->m_LockedAngle*factor;

                track.m_Lock                    = lock;
            }else if (zalBegin.m_Lock)
            {
                track.m_Lock                    = zalBegin.m_Lock;
            } else*/
                track.m_Lock                    = NULL;
        }
    }
    else
    {
        track.m_Zorder = 0.0f;
        track.m_Alpha  = 0.0f;
        track.m_Lock   = NULL;
    }

    return btrue;
}

void AnimTrack::ComputeAnimFrame(SafeArray<AnimBoneDyn> *_dyn, ITF_VECTOR<AnimLock> *_lock, f32 index, f32 percent, bbool isMain, const AnimFrameInfo::BonesArray* _templateUsed, bbool _minimalBones)
{
    ITF_ASSERT(_dyn && _lock);

    AnimSkeleton * skeleton = getSkeleton();
    ITF_ASSERT(skeleton);
    if (!skeleton)
        return;

    AnimLock            lock;

    if (_minimalBones)
    {
        u32 count = skeleton->m_BonesMinimumUpdateOrdered.size();
        for (u32 minI=0; minI<count; ++minI)
        {
            int i = skeleton->m_BonesMinimumUpdateOrdered[minI]->getIndex();
            if (_templateUsed && !(*_templateUsed)[i])
                continue;

            AnimTrackBone       track;

            if (!fillTrackAtIndex(track, m_bonesList[i], index, &lock ))
                continue;

            AnimBoneDyn     *pBoneDyn = &(*_dyn)[i];
            UpdateBoneWithLock(skeleton->m_BonesCurFrame[i], pBoneDyn, &track, _lock, percent, isMain);
            track.m_Lock = NULL; // force null to avoid deleting a non allocated Lock
        }
    } else
    {
        VectorAnim<AnimTrackBonesList>::iterator    animBone    = m_bonesList.begin();
        VectorAnim<AnimBone*>::iterator             Bone        = skeleton->m_BonesCurFrame.begin();

        u32 count = _dyn->size();
        for (u32 i=0; i<count && animBone != m_bonesList.end(); ++animBone, ++Bone, ++i)
        {
            if (_templateUsed && !(*_templateUsed)[i])
                continue;

            AnimTrackBone       track;

            if (!fillTrackAtIndex(track, (*animBone), index, &lock ))
                continue;

            AnimBoneDyn     *pBoneDyn = &(*_dyn)[i];
            UpdateBoneWithLock((*Bone), pBoneDyn, &track, _lock, percent, isMain);
            track.m_Lock = NULL; // force null to avoid deleting a non allocated Lock
        }
    }
}
#else
bbool AnimTrack::fillTrackAtIndex(AnimTrackBone & track, const AnimTrackBonesList & animBoneLst, f32 index )
{
    if (animBoneLst.m_trackPASSize == 0 &&  animBoneLst.m_trackZALSize == 0)
        return bfalse;

    u16                 uIndex = (u16)index;
    Vec2d               PB, PE, SB, SE;

    // search current index with dichotomy
    if (animBoneLst.m_trackPASSize > 0)
    {
        u32             begin = animBoneLst.m_trackPASIndex;
        u32             end   = begin + animBoneLst.m_trackPASSize - 1;

        while (end - begin > 1)
        {
            u32 center = (begin + end)/2;
            if (m_trackListPAS[center].m_frame < uIndex)
            {
                begin = center;
            } else
            {
                end = center;
            }
        }

        if (m_trackListPAS[begin].m_frame >= index)
        {
            const AnimTrackBonePAS &pas = m_trackListPAS[begin];
            pas.getPos(&track.m_PosLocal, m_bonesMaxPos);
            track.m_AngleLocal  = pas.getAngle(m_bonesMaxAngle);
            pas.getScale(&track.m_Scale, m_bonesMaxScale);
        } else if (m_trackListPAS[end].m_frame <= index)
        {
            const AnimTrackBonePAS &pas = m_trackListPAS[end];
            pas.getPos(&track.m_PosLocal, m_bonesMaxPos);
            track.m_AngleLocal  = pas.getAngle(m_bonesMaxAngle);
            pas.getScale(&track.m_Scale, m_bonesMaxScale);
        } else
        {
            const AnimTrackBonePAS &pasBegin  = m_trackListPAS[begin];
            const AnimTrackBonePAS &pasEnd    = m_trackListPAS[end];

            f32 factor    = (index - pasBegin.m_frame) / (pasEnd.m_frame - pasBegin.m_frame);
            f32 opFactor  = (1.0f-factor);


            //track.m_PosLocal    = pasBegin.getPos(m_bonesMaxPos)     * opFactor + pasEnd.getPos(m_bonesMaxPos) * factor;
            PB = Vec2d(pasBegin.m_PosLocalX, pasBegin.m_PosLocalY);                
            PE = Vec2d(pasEnd.  m_PosLocalX, pasEnd.  m_PosLocalY);
            Vec2d::Scale(&PB, &PB, opFactor*m_bonesMaxPos*RATIO_DIV_I16);
            Vec2d::ScaleAdd(&track.m_PosLocal, &PE, factor*m_bonesMaxPos*RATIO_DIV_I16, &PB);

            track.m_AngleLocal  = pasBegin.getAngle(m_bonesMaxAngle) * opFactor + pasEnd.getAngle(m_bonesMaxAngle) * factor;

            //track.m_Scale       = pasBegin.getScale(m_bonesMaxScale) * opFactor + pasEnd.getScale(m_bonesMaxScale) * factor;
            SB = Vec2d(pasBegin.m_ScaleX, pasBegin.m_ScaleY);                
            SE = Vec2d(pasEnd.  m_ScaleX, pasEnd.  m_ScaleY);
            Vec2d::Scale(&SB, &SB, opFactor*m_bonesMaxScale*RATIO_DIV_I16);
            Vec2d::ScaleAdd(&track.m_Scale, &SE, factor*m_bonesMaxScale*RATIO_DIV_I16, &SB);
        }
    }

    if (animBoneLst.m_trackZALSize > 0)
    {
        u32             begin = animBoneLst.m_trackZALIndex;
        u32             end   = begin + animBoneLst.m_trackZALSize - 1;

        while (end - begin > 1)
        {
            u32 center = (begin + end)/2;
            if (m_trackListZAL[center].m_frame < uIndex)
            {
                begin = center;
            } else
            {
                end = center;
            }
        }


        if (m_trackListZAL[begin].m_frame >= index)
        {
            AnimTrackBoneZAL &zal = m_trackListZAL[begin];
            track.m_Alpha       = (f32)zal.m_Alpha / 255.f;
            track.m_Zorder      = zal.m_Zorder;
        } else if (m_trackListZAL[end].m_frame <= index)
        {
            AnimTrackBoneZAL &zal = m_trackListZAL[end];
            track.m_Alpha       = (f32)zal.m_Alpha / 255.f;
            track.m_Zorder      = zal.m_Zorder;
        } else
        {
            AnimTrackBoneZAL &zalBegin  = m_trackListZAL[begin];
            AnimTrackBoneZAL &zalEnd    = m_trackListZAL[end];

            f32 factor    = (index - zalBegin.m_frame) / (zalEnd.m_frame - zalBegin.m_frame);
            f32 opFactor  = (1.0f-factor);


            track.m_Zorder         = zalBegin.m_Zorder*opFactor + zalEnd.m_Zorder*factor;
            track.m_Alpha          = ((f32)zalBegin.m_Alpha * opFactor + (f32)zalEnd.m_Alpha *factor) / 255;
        }
    }
    return btrue;
}

void AnimTrack::ComputeAnimFrame(SafeArray<AnimBoneDyn> *_dyn, f32 index, f32 percent, bbool isMain, const AnimFrameInfo::BonesArray* _templateUsed, bbool _minimalBones)
{
    ITF_ASSERT(_dyn);

    AnimSkeleton * skeleton = getSkeleton();
    ITF_ASSERT(skeleton);
    if (!skeleton)
        return;

    if (_minimalBones)
    {
        u32 count = skeleton->m_BonesMinimumUpdateOrdered.size();
        for (u32 minI=0; minI<count; ++minI)
        {
            int i = skeleton->m_BonesMinimumUpdateOrdered[minI]->getIndex();
            if (_templateUsed && !(*_templateUsed)[i])
                continue;

            AnimTrackBone       track;

            if (!fillTrackAtIndex(track, m_bonesList[i], index ))
                continue;

            AnimBoneDyn     *pBoneDyn = &(*_dyn)[i];
            UpdateBoneWithoutLock(skeleton->m_BonesCurFrame[i], pBoneDyn, &track, percent, isMain);
        }
    } else
    {
        VectorAnim<AnimTrackBonesList>::iterator    animBone    = m_bonesList.begin();
        VectorAnim<AnimBone*>::iterator             Bone        = skeleton->m_BonesCurFrame.begin();

        u32 count = _dyn->size();
        for (u32 i=0; i<count && animBone != m_bonesList.end(); ++animBone, ++Bone, ++i)
        {
            if (_templateUsed && !(*_templateUsed)[i])
                continue;

            AnimTrackBone       track;

            if (!fillTrackAtIndex(track, (*animBone), index ))
                continue;

            AnimBoneDyn     *pBoneDyn = &(*_dyn)[i];
            UpdateBoneWithoutLock((*Bone), pBoneDyn, &track, percent, isMain);
        }
    }
}
#endif
#endif

//#ifdef ITF_SUPPORT_COOKING
void AnimTrack::ComputeAnimFrameCook(VectorAnim<VectorAnimTrackBoneWithLock> _trackList, SafeArray<AnimBoneDynWithLock> *_dyn, ITF_VECTOR<AnimLock> *_lock, f32 index, f32 percent, bbool isMain, const AnimFrameInfo::BonesArray* _templateUsed)
{
    ITF_ASSERT(_dyn && _lock);

    AnimSkeleton * skeleton = getSkeleton();
    ITF_ASSERT(skeleton);
    if (!skeleton)
        return;

    VectorAnim<VectorAnimTrackBoneWithLock>::iterator   trackBone   = _trackList.begin();
    VectorAnim<AnimBone >::iterator                     Bone        = skeleton->m_BonesCurFrame.begin();

    u32 count = _dyn->size();
    for (u32 i=0; i<count && trackBone != _trackList.end(); ++trackBone, ++Bone, ++i)
    {
        VectorAnim<AnimTrackBoneWithLock> & currentList = (*trackBone);
        if (currentList.size() == 0 || (_templateUsed && !(*_templateUsed)[i]))
            continue;

        // search current index with dychotomy
        u32             begin = 0;
        u32             end   = currentList.size() - 1;

        while (end - begin > 1)
        {
            u32 center = (begin + end)/2;
            if (currentList[center].m_frame < index)
            {
                begin = center;
            } else
            {
                end = center;
            }
        }

        AnimBoneDynWithLock     *pBoneDyn = &(*_dyn)[i];
        if (currentList[begin].m_frame >= index)
        {
            UpdateBoneWithLock(&(*Bone), pBoneDyn, &currentList[begin], _lock, percent, isMain);
        } else if (currentList[end].m_frame <= index)
        {
            UpdateBoneWithLock(&(*Bone), pBoneDyn, &currentList[end], _lock, percent, isMain);
        } else
        {
            AnimTrackBoneWithLock   track;
            AnimLock                lock; // used for temporary blend lock info
            BlendAnims(&currentList[begin], &currentList[end],
                (index - currentList[begin].m_frame)/
                (currentList[end].m_frame - currentList[begin].m_frame),
                &track, &lock);
            UpdateBoneWithLock(&(*Bone), pBoneDyn, &track, _lock, percent, isMain);
            track.m_Lock = NULL; // force null to avoid deleting a non allocated Lock
        }
    }
}
//#endif // ITF_SUPPORT_COOKING

//////////////////////////////////////////////////////////////
////// BML
//////////////////////////////////////////////////////////////


AnimTrackBML * AnimTrack::GetAnimBMLFrame(float index)
{
    if (m_bmlList.size() == 0)
        return NULL;

    index = floorf(index + 0.5f);
    u32 begin = 0;
    u32 end   = m_bmlList.size() - 1;
    while (end - begin > 1)
    {
        u32 mid = (begin + end)/2;
        if (m_bmlList[mid].m_frame > index)
        {
            end = mid;
        } else
        {
            begin = mid;
        }
    }
    if (m_bmlList[end].m_frame <= index)
    {
        return &m_bmlList[end];
    } else if (m_bmlList[begin].m_frame <= index)
    {
        return &m_bmlList[begin];
    } else
    {
        return NULL;
    }
}



//////////////////////////////////////////////////////////////
////// POLYLINE
//////////////////////////////////////////////////////////////


AnimTrackPolyline * AnimTrack::GetAnimPolylineFrame(float index)
{
    if (m_polylineList.size() == 0)
        return NULL;

    index = floorf(index + 0.5f);
    u32 begin = 0;
    u32 end   = m_polylineList.size() - 1;
    while (end - begin > 1)
    {
        u32 mid = (begin + end)/2;
        if (m_polylineList[mid].m_frame > index)
        {
            end = mid;
        } else
        {
            begin = mid;
        }
    }
    if (m_polylineList[end].m_frame <= index)
    {
        return &m_polylineList[end];
    } else if (m_polylineList[begin].m_frame <= index)
    {
        return &m_polylineList[begin];
    } else
    {
        return NULL;
    }
}

//////////////////////////////////////////////////////////////
////// EVENTS
//////////////////////////////////////////////////////////////


int AnimTrack::GetEvents(f32 prevIndex, f32 index, AnimEventContainer &eventList, bbool _getAll )
{
    if (m_eventList.size() == 0)
        return 0;

    //find first event greater than previous frame
    VectorAnim<AnimTrackFrameEvents>::iterator begin = m_eventList.end();
    for (VectorAnim<AnimTrackFrameEvents>::iterator it = m_eventList.begin(); it != m_eventList.end();++it)
    {
        if (it->m_frame >= prevIndex)
        {
            begin = it;
            break;
        }
    }
    if (begin == m_eventList.end() || begin->m_frame > index )
        return 0;

    //find event smaller than current frame
    VectorAnim<AnimTrackFrameEvents>::iterator end = m_eventList.end();
    for (VectorAnim<AnimTrackFrameEvents>::iterator it = begin; it != m_eventList.end();++it)
    {
        if (it->m_frame >= index)
        {
            end = it; // end should be the last valid + 1
            break;
        }
    }
    u32 nbEvents    = 0;
    for (VectorAnim<AnimTrackFrameEvents>::iterator it = begin; it != end;++it)
    {
        AnimEventContainer& frameEvents = it->m_frameEvents;
        u32 numEvents = frameEvents.size();

        for (u32 j=0; j<numEvents; j++)
        {
            AnimMarkerEvent* trackEvent = frameEvents[j];

            if ( !_getAll && trackEvent->GetObjectClassCRC() == AnimAnimationEvent::GetClassCRCStatic() )
            {
                continue;
            }

            eventList.push_back(trackEvent);
            nbEvents++;
        }
    }
    return nbEvents;
}


f32 AnimTrack::GetEventFrame(StringID::StringIdValueType _crc, const StringID & _eventName )const
{
    for (VectorAnim<AnimTrackFrameEvents>::const_iterator it = m_eventList.begin(); it != m_eventList.end();++it)
    {
        const AnimEventContainer& frameEvents = it->m_frameEvents;
        u32 numEvents = frameEvents.size();

        for (u32 j=0; j<numEvents; j++)
        {
            const AnimMarkerEvent* animEvent = frameEvents[j];

            if ( animEvent->GetObjectClassCRC() == _crc && _eventName == animEvent->getName() )
                return it->m_frame;
        }
    }

#ifndef ITF_FINAL
    LOG_YOUS("Missing Event [ %s ] in Animation [ %s ]",(_eventName.c_str(),StringConverter(m_path).getChar()));
#endif
    return -1.f;
}


bbool AnimTrack::searchSubAnims(VectorAnim<SubAnimBeginEnd> &subAnims)
{
    VectorAnim<AnimTrackFrameEvents>::iterator eventIter = m_eventList.begin();
    for (; eventIter != m_eventList.end(); ++eventIter)
    {
        AnimTrackFrameEvents & evtFrame = *eventIter;

        u32 count = evtFrame.m_frameEvents.size();
        for (u32 evtIdx = 0; evtIdx < count; ++evtIdx)
        {
            AnimMarkerEvent * evt = evtFrame.m_frameEvents[evtIdx];
            if (evt->getAnimEvtType() == AnimEvtType_Animation)
            {
                String8 trueName = evt->getName().getDebugString();
                if (trueName.getLen() > 6 &&
                    trueName.substr(trueName.getLen()-6, 6).equals("_start", btrue))
                {
                    SubAnimBeginEnd newZone;
                    newZone.m_begin     = evtFrame.m_frame;
                    newZone.m_subAnim   = trueName.substr(0, trueName.getLen()-6);
                    subAnims.push_back(newZone);
                }
            }
        }
    }

    // Verify subAnims
    bbool subAnimsOk = btrue;

    u32 subAnimsSize = subAnims.size();
    if (subAnimsSize > 0)
    {
        // Fill end
        eventIter = m_eventList.begin();
        for (; eventIter != m_eventList.end(); ++eventIter)
        {
            AnimTrackFrameEvents & evtFrame = *eventIter;
            u32 count = evtFrame.m_frameEvents.size();
            for (u32 evtIdx = 0; evtIdx < count; ++evtIdx)
            {
                AnimMarkerEvent * evt = evtFrame.m_frameEvents[evtIdx];
                if (evt->getAnimEvtType() == AnimEvtType_Animation)
                {
                    String8 trueName = evt->getName().getDebugString();
                    if (trueName.getLen() > 5 &&
                        trueName.substr(trueName.getLen()-5, 5).equals("_stop", btrue))
                    {
                        StringID animName = trueName.substr(0, trueName.getLen()-5);
                        for (u32 subIdx = 0; subIdx < subAnimsSize; ++subIdx)
                        {
                            if (subAnims[subIdx].m_subAnim == animName)
                            {
                                ITF_WARNING_CATEGORY(Anim,NULL, evtFrame.m_frame >= subAnims[subIdx].m_begin, "Track %s has subanim event %s with end < begin", m_name.getDebugString(), subAnims[subIdx].m_subAnim.getDebugString());
                                subAnimsOk &= evtFrame.m_frame >= subAnims[subIdx].m_begin;
                                subAnims[subIdx].m_end = evtFrame.m_frame;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

    if (!subAnimsOk)
        return bfalse;

    for (u32 subIdx = 0; subIdx < subAnimsSize; ++subIdx)
    {
        if (subAnims[subIdx].m_end < 0)
        {
            subAnimsOk = bfalse;
            ITF_WARNING_CATEGORY(Anim,NULL, subAnimsOk, "Track %s has subanim event %s with no end", m_name.getDebugString(), subAnims[subIdx].m_subAnim.getDebugString());
            break;
        }
    }            

    if (!subAnimsOk)
        return bfalse;

    return btrue;
}

bbool AnimTrack::fillSoundFromEvents()
{
    // compute partition event -> processed separately
    VectorAnim<SubAnimBeginEnd> subAnims;

    bbool trackHasPartition = bfalse;
    VectorAnim<AnimTrackFrameEvents>::iterator eventIter = m_eventList.begin();
    for (; eventIter != m_eventList.end() && !trackHasPartition; ++eventIter)
    {
        AnimTrackFrameEvents & evtFrame = *eventIter;

        u32 count = evtFrame.m_frameEvents.size();
        for (u32 evtIdx = 0; evtIdx < count; evtIdx++)
        {
            AnimMarkerEvent * evt = evtFrame.m_frameEvents[evtIdx];
            if (evt->getAnimEvtType() == AnimEvtType_Partition)
            {
                trackHasPartition = btrue;
                break;
            }
        }
    }

    if (!trackHasPartition)
        return btrue;

#ifdef ITF_DEBUG_STRINGID
    // first search subAnims ...
    // Fill begin

    if (!searchSubAnims(subAnims))
        return bfalse;

#endif //ITF_DEBUG_STRINGID

    eventIter = m_eventList.begin();
    for (; eventIter != m_eventList.end(); )
    {
        AnimTrackFrameEvents & evtFrame = *eventIter;

        u32 count = evtFrame.m_frameEvents.size();
        for (u32 evtIdx = 0; evtIdx < count; )
        {
            AnimMarkerEvent * evt = evtFrame.m_frameEvents[evtIdx];
            if (evt->getAnimEvtType() == AnimEvtType_Partition)
            {
                f32 percent = ((AnimPartitionEvent *)evt)->getPercent();
                if (percent < 0.f)
                {
                    u32 subAnimIndex = U32_INVALID;
                    for (u32 subIdx = 0; subIdx < subAnims.size(); ++subIdx)
                    {
                        if (subAnims[subIdx].m_begin <= evtFrame.m_frame &&
                            evtFrame.m_frame <= subAnims[subIdx].m_end )
                        {
                            subAnimIndex = subIdx;
                            break;
                        }
                    }
                    if (subAnimIndex == U32_INVALID)
                    {
                        percent = evtFrame.m_frame / (m_endFrame + 1.f);
                    } else
                    {
                        percent = (evtFrame.m_frame - subAnims[subAnimIndex].m_begin) / (subAnims[subAnimIndex].m_end - subAnims[subAnimIndex].m_begin  + 1.f);
                    }
                }

                AnimTrackFrameSoundEvents sound;

                sound.m_name            = evt->getName();
                sound.m_startRatio      = percent;
                sound.m_parentBoneIndex = evt->getBoneParent();

                m_soundList.push_back(sound);
                evtFrame.m_frameEvents.eraseKeepOrder(evtIdx);
                SF_DEL(evt);

                count = evtFrame.m_frameEvents.size();
            } else
            {
                ++evtIdx;
            }
        }
        if (count == 0)
        {
            eventIter = m_eventList.erase(eventIter);
        } else
        {
            eventIter++;
        }
    }

    return btrue;
}


void AnimTrack::computeMaxValues( const VectorAnim<VectorAnimTrackBoneWithLock> & _tempTrackList )
{
    m_bonesMaxPos    = MTH_EPSILON * RATIO_MULT_I16; // no need more precision than epsilon
    m_bonesMaxAngle  = MTH_EPSILON * RATIO_MULT_I16;
    
    VectorAnim<VectorAnimTrackBoneWithLock>::const_iterator trackListIter = _tempTrackList.begin();
    for (;trackListIter != _tempTrackList.end(); trackListIter++)
    {
        const VectorAnim<AnimTrackBoneWithLock> & trackList = *trackListIter;
        VectorAnim<AnimTrackBoneWithLock>::const_iterator trackInIter = trackList.begin();
        for (; trackInIter != trackList.end(); trackInIter++)
        {
            const AnimTrackBoneWithLock & tBone = *trackInIter;
            if (f32_Abs(tBone.m_AngleLocal) > m_bonesMaxAngle )
                m_bonesMaxAngle = f32_Abs(tBone.m_AngleLocal);
            if (f32_Abs(tBone.m_PosLocal.m_x) > m_bonesMaxPos )
                m_bonesMaxPos = f32_Abs(tBone.m_PosLocal.m_x);
            if (f32_Abs(tBone.m_PosLocal.m_y) > m_bonesMaxPos )
                m_bonesMaxPos = f32_Abs(tBone.m_PosLocal.m_y);
            if (f32_Abs(tBone.m_Scale.m_x) > m_bonesMaxScale)
                m_bonesMaxScale = f32_Abs(tBone.m_Scale.m_x);
            if (f32_Abs(tBone.m_Scale.m_y) > m_bonesMaxScale )
                m_bonesMaxScale = f32_Abs(tBone.m_Scale.m_y);
        }
    }
    m_bonesMaxPos   *= 1.01f; // add 1% to prevent border effects
    m_bonesMaxAngle *= 1.01f; // add 1% to prevent border effects
    m_bonesMaxScale *= 1.01f; // add 1% to prevent border effects
}


void AnimTrack::fillPASZALListFromBoneList( const VectorAnim<VectorAnimTrackBoneWithLock> & _tempTrackList, VectorAnim<VectorAnimTrackBonePAS> & _pasList, VectorAnim<VectorAnimTrackBoneZAL> & _zalList )
{
    VectorAnim<VectorAnimTrackBoneWithLock>::const_iterator trackListIter = _tempTrackList.begin();
    VectorAnim<VectorAnimTrackBonePAS>::iterator    trackListPASIter = _pasList.begin();
    VectorAnim<VectorAnimTrackBoneZAL>::iterator    trackListZALIter = _zalList.begin();

    for (;trackListIter != _tempTrackList.end(); trackListIter++, trackListPASIter++, trackListZALIter++)
    {
        const VectorAnim<AnimTrackBoneWithLock> & trackList = *trackListIter;
        VectorAnim<AnimTrackBonePAS> & trackListPASIn = *trackListPASIter;
        VectorAnim<AnimTrackBoneZAL> & trackListZALIn = *trackListZALIter;
        VectorAnim<AnimTrackBoneWithLock>::const_iterator trackInIter = trackList.begin();
        for (; trackInIter != trackList.end(); trackInIter++)
        {
            const AnimTrackBoneWithLock & tBone = *trackInIter;
            AnimTrackBonePAS pas;
            pas.m_frame = (u16)tBone.m_frame;
            pas.setAngle(tBone.m_AngleLocal, m_bonesMaxAngle);
            pas.setPos(tBone.m_PosLocal, m_bonesMaxPos);
            pas.setScale(tBone.m_Scale, m_bonesMaxScale);

            trackListPASIn.push_back(pas);

            AnimTrackBoneZAL zal;
            zal.m_frame     = (u16)tBone.m_frame;
            zal.m_Zorder    = tBone.m_Zorder;
            zal.m_Alpha     = (i16)(tBone.m_Alpha * 255);
#ifndef NO_RUNTIME_LOCK

            if (tBone.m_Lock)
            {
                zal.m_Lock = newAlloc(mId_Animation,AnimLock());
                ITF_Memcpy(zal.m_Lock, tBone.m_Lock, sizeof(AnimLock));
            } else
            {
                zal.m_Lock      = tBone.m_Lock;
            }
#else
            ITF_ASSERT(!tBone.m_Lock);
#endif

            trackListZALIn.push_back(zal);
        }
    }
}

void AnimTrack::cleanPASKeys( VectorAnim<VectorAnimTrackBonePAS> & _pasList )
{
    VectorAnim<VectorAnimTrackBonePAS>::iterator    trackListPASIter = _pasList.begin();
    for (;trackListPASIter != _pasList.end(); trackListPASIter++)
    {
        VectorAnim<AnimTrackBonePAS> & trackListPASIn = *trackListPASIter;
        // clean unnecessary keys
        u32 count = trackListPASIn.size();
        bbool repeat;
        do 
        {
            repeat = bfalse;
            for (u32 i=2; i<count; i++)
            {
                AnimTrackBonePAS *trkBone0 = &trackListPASIn[i-2];
                AnimTrackBonePAS *trkBone1 = &trackListPASIn[i-1];
                AnimTrackBonePAS *trkBone2 = &trackListPASIn[i-0];
                if (trkBone0->isEpsilonEqual(trkBone1) &&
                    trkBone1->isEpsilonEqual(trkBone2))
                {
                    // unnecessary middle key
                    trackListPASIn.erase(trackListPASIn.begin() + (i-1));
                    count   = trackListPASIn.size();
                    repeat  = btrue;
                    break;
                }

                // Interp Key test
                AnimTrackBonePAS interpKey;
                f32 percent = ((f32)trkBone1->m_frame - (f32)trkBone0->m_frame)/((f32)trkBone2->m_frame - (f32)trkBone0->m_frame);
                f32 invPercent = 1.f - percent;
                interpKey.m_AngleLocal  = (i16)(trkBone0->m_AngleLocal*invPercent + trkBone2->m_AngleLocal*percent);
                interpKey.m_PosLocalX   = (i16)(trkBone0->m_PosLocalX*invPercent + trkBone2->m_PosLocalX*percent);
                interpKey.m_PosLocalY   = (i16)(trkBone0->m_PosLocalY*invPercent + trkBone2->m_PosLocalY*percent);
                interpKey.m_ScaleX      = (i16)(trkBone0->m_ScaleX*invPercent + trkBone2->m_ScaleX*percent);
                interpKey.m_ScaleY      = (i16)(trkBone0->m_ScaleY*invPercent + trkBone2->m_ScaleY*percent);
                if (interpKey.isEpsilonEqual(trkBone1))
                {
                    // track is blend of prev and next key -> non useful
                    trackListPASIn.erase(trackListPASIn.begin() + (i-1));
                    count   = trackListPASIn.size();
                    repeat  = btrue;
                    break;
                }
            }
        } while(repeat);

        if (count > 1)
        {
            AnimTrackBonePAS *trkBone0 = &trackListPASIn[count-2];
            AnimTrackBonePAS *trkBone1 = &trackListPASIn[count-1];
            if (trkBone0->isEpsilonEqual(trkBone1))
            {
                trackListPASIn.pop_back();
            }
        }
        count = trackListPASIn.size();
        if (count == 1) // only one element
        {
            AnimTrackBonePAS trkEmpty;
            trkEmpty.setScale(Vec2d::One, m_bonesMaxScale);
            AnimTrackBonePAS *trkBone = &trackListPASIn[0];
            if (trkEmpty.isEpsilonEqual(trkBone))
            {
                trackListPASIn.clear();
            }
        }
    }
}

void AnimTrack::cleanZALKeys( VectorAnim<VectorAnimTrackBoneZAL> & _zalList )
{
    VectorAnim<VectorAnimTrackBoneZAL>::iterator    trackListZALIter = _zalList.begin();
    for (;trackListZALIter != _zalList.end(); trackListZALIter++)
    {
        VectorAnim<AnimTrackBoneZAL> & trackListZALIn = *trackListZALIter;
        u32 count = trackListZALIn.size();
        bbool repeat;
        do 
        {
            repeat = bfalse;
            for (u32 i=2; i<count; i++)
            {
                AnimTrackBoneZAL *trkBone0 = &trackListZALIn[i-2];
                AnimTrackBoneZAL *trkBone1 = &trackListZALIn[i-1];
                AnimTrackBoneZAL *trkBone2 = &trackListZALIn[i-0];
                if (trkBone0->isEpsilonEqual(trkBone1) &&
                    trkBone1->isEpsilonEqual(trkBone2))
                {
                    // unnecessary middle key
                    trackListZALIn.erase(trackListZALIn.begin() + (i-1));
                    count   = trackListZALIn.size();
                    repeat  = btrue;
                    break;
                }

                // Interp Key test
                AnimTrackBoneZAL interpKey;
                f32 percent = ((f32)trkBone1->m_frame - (f32)trkBone0->m_frame)/((f32)trkBone2->m_frame - (f32)trkBone0->m_frame);
                f32 invPercent = 1.f - percent;
                interpKey.m_Zorder      = trkBone0->m_Zorder*invPercent     + trkBone2->m_Zorder*percent;
                interpKey.m_Alpha       = (i16)(trkBone0->m_Alpha*invPercent      + trkBone2->m_Alpha*percent);
                if (interpKey.isEpsilonEqual(trkBone1))
                {
                    // track is blend of prev and next key -> non useful
                    trackListZALIn.erase(trackListZALIn.begin() + (i-1));
                    count   = trackListZALIn.size();
                    repeat  = btrue;
                    break;
                }
            }
        } while(repeat);

        if (count > 1)
        {
            AnimTrackBoneZAL *trkBone0 = &trackListZALIn[count-2];
            AnimTrackBoneZAL *trkBone1 = &trackListZALIn[count-1];
            if (trkBone0->isEpsilonEqual(trkBone1))
            {
                trackListZALIn.pop_back();
            }
        }
        count = trackListZALIn.size();
        if (count == 1) // only one element
        {
            AnimTrackBoneZAL trkEmpty;
            AnimTrackBoneZAL *trkBone = &trackListZALIn[0];
            if (trkEmpty.isEpsilonEqual(trkBone))
            {
                trackListZALIn.clear();
            }
        }
    }
}

void AnimTrack::fillPASZAL( const VectorAnim<VectorAnimTrackBonePAS> & _pasList, const VectorAnim<VectorAnimTrackBoneZAL> & _zalList )
{
    VectorAnim<VectorAnimTrackBonePAS>::const_iterator trackListPASIter = _pasList.begin();
    VectorAnim<VectorAnimTrackBoneZAL>::const_iterator trackListZALIter = _zalList.begin();
    for (VectorAnim<AnimTrackBonesList>::iterator iter = m_bonesList.begin();
        iter != m_bonesList.end(); ++iter, ++trackListPASIter, ++trackListZALIter)
    {
        AnimTrackBonesList &boneTrack = *iter;
        const VectorAnim<AnimTrackBonePAS> & trackListPASIn = *trackListPASIter;
        const VectorAnim<AnimTrackBoneZAL> & trackListZALIn = *trackListZALIter;

        boneTrack.m_trackPASIndex = (u16)m_trackListPAS.size();
        boneTrack.m_trackPASSize  = (u16)trackListPASIn.size();
        if (boneTrack.m_trackPASSize)
		{
			VectorAnim<AnimTrackBonePAS>::const_iterator itB = trackListPASIn.begin();
			VectorAnim<AnimTrackBonePAS>::const_iterator itE = trackListPASIn.end();
			for (;itB !=itE; ++itB )
			{
				m_trackListPAS.push_back(*itB);
			}
		}

        boneTrack.m_trackZALIndex = (u16)m_trackListZAL.size();
        boneTrack.m_trackZALSize  = (u16)trackListZALIn.size();
        if (boneTrack.m_trackZALSize)
            m_trackListZAL.insert(m_trackListZAL.end(), trackListZALIn.begin(), trackListZALIn.end());
    }
}

bbool AnimTrack::needRecook()
{
    const String & skeletonRelativePath = ANIM_MANAGER->getString(m_skeletonPath);
    u64            fileTime             = FILEMANAGER->getLastTimeWriteAccess(skeletonRelativePath);

    if (fileTime != m_skeletonFlushTime)
        return btrue;


    // also recook when redirectPatchBank has changed
    u32 count = m_patchBankPathList.size();
    for (u32 i=0; i<count; ++i)
    {
        const String & patchBankPath= ANIM_MANAGER->getString(m_patchBankPathList[i]);
        String redirectPatchBank    = FilePath::getDirectory(patchBankPath) +  AnimTrack::s_redirectPatchBank;
        fileTime                    = FILEMANAGER->getLastTimeWriteAccess(redirectPatchBank);

        if (fileTime != m_redirectPatchBankFlushTimeList[i])
            return btrue;
    }

    return bfalse;
}


ResourceID BankRedirect::getExtTexture(u32 _index) const
{
    ITF_ASSERT(_index < m_textureResourceIDList.size());
    return m_textureResourceIDList[_index];
}

AnimPatchBank*  BankRedirect::getExtPatchBank(u32 _index) const
{
    ITF_ASSERT(_index < m_patchBankResourceIDList.size());
    AnimPatchBankResource *res = static_cast<AnimPatchBankResource *>(m_patchBankResourceIDList[_index].getResource());
    if (res)
    {
        AnimPatchBank* pbk = res->getOwnerData();
        if (pbk)
        {
            pbk->m_texture = getExtTexture(_index);
            return pbk;
        }
    }
    return NULL;
}


bbool BankRedirect::getTemplate(const TemplateRef &_ref, AnimTemplate * &_template) const
{
#ifdef ITF_WII
	i32 IDX = m_nameToPatch.find(_ref.m_bankName);
    if (IDX>=0)
    {
		AnimPatchBank *bank = getExtPatchBank(m_nameToPatch.getAt(IDX));
        if (bank)
        {
            i32 IDX2 = bank->m_NameToTemplate.find(_ref.m_patchName);
            if (IDX2 < 0)
                return bfalse;
			_template = bank->m_templateList[bank->m_NameToTemplate.getAt(IDX2)];
            return _template != NULL;
        }
    }
    return bfalse;
#else
    NamesMap::Iterator ntp(m_nameToPatch);
    ntp.findKey(_ref.m_bankName);
    if (!ntp.isEnd())
    {
        AnimPatchBank *bank = getExtPatchBank(ntp.second());
        if (bank)
        {
            NamesMap::Iterator ntt(bank->m_NameToTemplate);
            ntt.findKey(_ref.m_patchName);
            if (ntt.isEnd())
                return bfalse;
            _template = bank->m_templateList[ntt.second()];
            return _template != NULL;
        }
    }
    return bfalse;
#endif
}

bbool BankRedirect::getTexture(const TemplateRef &_ref, ResourceID &_texture) const
{
    NamesMap::Iterator ntp(m_nameToPatch);
    ntp.findKey(_ref.m_bankName);
    if (!ntp.isEnd())
    {
        _texture = getExtTexture(ntp.second());
        return _texture.isValidResourceId();
    }
    return bfalse;
}

AnimTrackExt::AnimTrackExt()
{

}

AnimTrack *AnimTrackExt::getTrack() const
{
    AnimTrackResource * res = static_cast<AnimTrackResource *>(m_trackResourceID.getResource());
    ITF_ASSERT_CRASH(res, "verify resource !!");

    AnimTrack * track = res->getTrack();
    ITF_ASSERT_CRASH(track, "resource not loaded !");

    return track;
}


AnimSkeleton *AnimTrackExt::getSkeleton() const
{
    AnimTrack *trk = getTrack();
    if (trk)
    {
        return trk->getSkeleton();
    }
    return NULL;
}


bbool AnimTrackExt::getTexture(const TemplateRef &_ref, ResourceID &_texture, u32 _state) const
{
#ifdef ITF_WII
    {
        i32 IDX  = m_stateToRedirect.find(_state);
        if ((IDX >= 0) && (m_redirectStates[m_stateToRedirect.getAt(IDX)].getTexture(_ref, _texture)))
                return btrue;
        // if not found in specify, search in default
		IDX  = m_stateToRedirect.find(U32_INVALID);
        if ((IDX >= 0) && (m_redirectStates[m_stateToRedirect.getAt(IDX)].getTexture(_ref, _texture)))
                return btrue;
    }
    // if not found in redirect bank, search in track
    AnimTrack * track = getTrack();
    if (!track)        return bfalse;
    {
		i32 IDX = track->m_nameToPatch.find(_ref.m_bankName);
        if (IDX>=0)
        {
			_texture = track->getTexture(track->m_nameToPatch.getAt(IDX));
            return _texture.isValidResourceId();
        }
    }
    return bfalse;
#else
    {
        KeyArray<u32>::Iterator ntp(m_stateToRedirect);
        // search in specific state
        ntp.findKey(_state);
        if (!ntp.isEnd())
        {
            if (m_redirectStates[ntp.second()].getTexture(_ref, _texture))
                return btrue;
        }
        // if not found in specify, search in default
        ntp.findKey(U32_INVALID);
        if (!ntp.isEnd())
        {
            if (m_redirectStates[ntp.second()].getTexture(_ref, _texture))
                return btrue;
        }
    }

    // if not found in redirect bank, search in track
    AnimTrack * track = getTrack();
    if (!track)
        return bfalse;

    {
        NamesMap::Iterator ntp(track->m_nameToPatch);
        ntp.findKey(_ref.m_bankName);
        if (!ntp.isEnd())
        {
            _texture = track->getTexture(ntp.second());
            return _texture.isValidResourceId();
        }
    }

    return bfalse;
#endif
}

bbool AnimTrackExt::getTemplate(const TemplateRef &_ref, AnimTemplate * &_template, u32 _state) const
{
    {
//#ifdef ITF_WII
#if 1
		i32 IDX = m_stateToRedirect.find(_state);
		if ((IDX >= 0)&&(m_redirectStates[m_stateToRedirect.getAt(IDX)].getTemplate(_ref, _template)))
                return btrue;
        // if not found in specify, search in default
		IDX = m_stateToRedirect.find(U32_INVALID);
        //ntp.findKey(U32_INVALID);
        if ((IDX >= 0) && (m_redirectStates[m_stateToRedirect.getAt(IDX)].getTemplate(_ref, _template)))
                return btrue;
    }

    AnimTrack * track = getTrack();
    if (!track)
        return bfalse;

    {
		i32 IDX = track->m_nameToPatch.find(_ref.m_bankName);
        if (IDX >= 0)
        {
            AnimPatchBank *bank = track->getPatchBank(track->m_nameToPatch.getAt(IDX));
            if (bank)
            {
				i32 IDX2 = bank->m_NameToTemplate.find(_ref.m_patchName);
			    if (IDX2 < 0)                  return bfalse;
				_template = bank->m_templateList[bank->m_NameToTemplate.getAt(IDX2)];
                return _template != NULL;
            }
        }
    }

    return bfalse;
#else
        KeyArray<u32>::Iterator ntp(m_stateToRedirect);
        // search in specific state
        ntp.findKey(_state);
        if (!ntp.isEnd())
        {
            if (m_redirectStates[ntp.second()].getTemplate(_ref, _template))
                return btrue;
        }
        // if not found in specify, search in default
        ntp.findKey(U32_INVALID);
        if (!ntp.isEnd())
        {
            if (m_redirectStates[ntp.second()].getTemplate(_ref, _template))
                return btrue;
        }
    }

    AnimTrack * track = getTrack();
    if (!track)
        return bfalse;

    {
        NamesMap::Iterator ntp(track->m_nameToPatch);
        ntp.findKey(_ref.m_bankName);
        if (!ntp.isEnd())
        {
            AnimPatchBank *bank = track->getPatchBank(ntp.second());
            if (bank)
            {
                NamesMap::Iterator ntt(bank->m_NameToTemplate);
                ntt.findKey(_ref.m_patchName);
                if (ntt.isEnd())
                    return bfalse;
                _template = bank->m_templateList[ntt.second()];
                return _template != NULL;
            }
        }
    }

    return bfalse;
#endif
}

AnimMarkerEvent* AnimMarkerEvent::createEvent( AnimEvtType _type )
{
    AnimMarkerEvent* ret;

    switch(_type)
    {
    case AnimEvtType_FX:
        ret = newAlloc(mId_Animation,AnimFXEvent());
        break;
    case AnimEvtType_Animation:
        ret = newAlloc(mId_Animation,AnimAnimationEvent());
        break;
    case AnimEvtType_Gameplay:
        ret = newAlloc(mId_Animation,AnimGameplayEvent());
        break;
    case AnimEvtType_Polyline:
        ret = newAlloc(mId_Animation,AnimPolylineEvent());
        break;
    case AnimEvtType_Partition:
        ret = newAlloc(mId_Animation,AnimPartitionEvent());
        break;
    default:
        ITF_ASSERT(0);
        ret = NULL;
        break;
    }

    return ret;
}

void AnimTrackFrameEvents::serialize(ArchiveMemory& _archive)
{
    _archive.serialize(m_frame);

    if (_archive.isReading())
    {
        u32 _size = 0;
        _archive.serialize(_size);

        m_frameEvents.clear();
        m_frameEvents.resize(_size);

        AnimMarkerEvent* val;
        u32 type;

        for (u32 index = 0;index<_size;++index)
        {                    
            _archive.serialize(type);

            val = AnimMarkerEvent::createEvent(static_cast<AnimEvtType>(type));

            if ( val )
            {
                val->serialize(_archive);
                m_frameEvents[index] = val;
            }
        }

    }
    else
    {
        u32 type;
        u32 _size = m_frameEvents.size();
        _archive.serialize(_size);

        u32 numEvents = m_frameEvents.size();

        for (u32 j=0; j<numEvents; j++)
        {
            AnimMarkerEvent*  val = m_frameEvents[j];
            type = val->getAnimEvtType();
            _archive.serialize(type);
            val->serialize(_archive);
        }
    }
}

// THIS FUNCTIONS IS DUPLICATED TO AVOID plug-in dependencies ...

#ifdef ITF_SUPPORT_XML
pugi::xml_node LocalxmlWriteString(pugi::xml_node _parentNode, const char *_name, const char *_string)
{
    pugi::xml_node elem = _parentNode.append_child(_name);
    elem.append_attribute("type").set_value("string");
    elem.append_attribute("value").set_value(_string);
    return elem;
}
///////////////////////////////////////////////////////////////////////////////////////////
pugi::xml_node LocalxmlWriteFloat(pugi::xml_node _parentNode, const char *_name, f32 _value)
{
    pugi::xml_node elem = _parentNode.append_child(_name);
    elem.append_attribute("type").set_value("number");
    if (_value==(f32)(int)_value)
        elem.append_attribute("value").set_value(_value);
    else
        elem.append_attribute("value").set_value(_value);
    return elem;
}

///////////////////////////////////////////////////////////////////////////////////////////
pugi::xml_node LocalxmlWriteVector(pugi::xml_node _parentNode, const char *_name, const Vec2d &_vec)
{
    pugi::xml_node elem = _parentNode.append_child(_name);
    elem.append_attribute("type").set_value("vector");
    elem.append_attribute("x").set_value(_vec.m_x);
    elem.append_attribute("y").set_value(_vec.m_y);
    elem.append_attribute("z").set_value(0);
    return elem;
}

#endif //ITF_SUPPORT_XML


IMPLEMENT_OBJECT_RTTI(AnimMarkerEvent)

BEGIN_SERIALIZATION_CHILD(AnimMarkerEvent)

    SERIALIZE_MEMBER("name",m_Name);
    SERIALIZE_MEMBER("posLocal",m_PosLocal);

END_SERIALIZATION()

void AnimMarkerEvent::serialize(ArchiveMemory& _archive)
{
    if (_archive.isReading())
    {
        StringID::StringIdValueType nameId;
        _archive.serialize(nameId);
        m_Name = StringID(nameId);
    }
    else
    {
        StringID::StringIdValueType nameId = m_Name.GetValue();
        _archive.serialize(nameId);
    }

    m_PosLocal.serialize(_archive);
    _archive.serialize(m_boneParent); 
}

#ifdef ITF_SUPPORT_XML

bbool AnimMarkerEvent::ReadXML(PugiXMLWrap *pXML, NamesMap *_uidToBone, int errBehavior)
{
    Vec2d pos;
    pos.m_x       = pXML->readFloatAttribute("PosLocal/x", 0, (PugiXMLWrap::ErrorBehavior)errBehavior);
    pos.m_y       = pXML->readFloatAttribute("PosLocal/y", 0, (PugiXMLWrap::ErrorBehavior)errBehavior);
    setPosLocal(pos);

    const char * boneName = pXML->readAttribute("BoneUID/value", NULL, PugiXMLWrap::DoNothing);
    if (_uidToBone && boneName)
    {
        NamesMap::Iterator keyUID(*_uidToBone);
        keyUID.findKey(StringID(boneName));
        if (!keyUID.isEnd())
            m_boneParent = keyUID.second();
    }

    const char* eventName = pXML->readAttribute("Name/value", "");
    if ( eventName && eventName[0] != '\0' )
    {
        setName(eventName);
        return btrue;
    }
    
    return bfalse;
}

void  AnimMarkerEvent::WriteXML(pugi::xml_node node, AnimSkeleton *skeleton)
{
    LocalxmlWriteVector(node, "PosLocal", getPosLocal());
    
    if (m_boneParent != U32_INVALID)
    {
        StringID boneID; ///////////////////
        for (u32 idIndex=0; idIndex<skeleton->m_UIDToBone.size(); idIndex++)
        {
            if ((i32)m_boneParent==skeleton->m_UIDToBone.getAt(idIndex))
            {
                boneID = skeleton->m_UIDToBone.getKeyAt(idIndex);
                break;
            }
        }

        LocalxmlWriteString(node, "UID", boneID.getDebugString());
    }

    if (m_Name.isValid())
        LocalxmlWriteString(node, "Name", m_Name.getDebugString());

    LocalxmlWriteFloat(node, "Type", (f32)getAnimEvtType());
    
    String8 uid;
    uid.setTextFormat("%p", this);
    LocalxmlWriteString(node, "UID", uid.cStr());
}
#endif //ITF_SUPPORT_XML


IMPLEMENT_OBJECT_RTTI(AnimFXEvent)
BEGIN_SERIALIZATION_CHILD(AnimFXEvent)
END_SERIALIZATION()

void AnimFXEvent::serialize(ArchiveMemory& _archive)
{
    Super::serialize(_archive);

    _archive.serialize((u32 &)m_action);
}

#ifdef ITF_SUPPORT_XML
bbool AnimFXEvent::ReadXML(PugiXMLWrap *pXML, NamesMap *_uidToBone,  int errBehavior)
{
    bbool ret = Super::ReadXML(pXML, _uidToBone, errBehavior);

    if (pXML->enterElement("Params"))
    {
        m_action = !strcmp(pXML->readAttribute("Action/value", "1"), "1") ? FX_Start : FX_Stop;
        pXML->exitElement();
        return ret;
    } else
    {
        m_action = FX_Start;
    }
    return ret;
}


void AnimFXEvent::WriteXML(pugi::xml_node node, AnimSkeleton * skeleton)
{
    Super::WriteXML(node, skeleton);

    pugi::xml_node params = node.append_child("Params");
    {
        LocalxmlWriteFloat(params, "Action", (f32)m_action);
    }
}
#endif //ITF_SUPPORT_XML



IMPLEMENT_OBJECT_RTTI(AnimAnimationEvent)
BEGIN_SERIALIZATION_CHILD(AnimAnimationEvent)
END_SERIALIZATION()

IMPLEMENT_OBJECT_RTTI(AnimGameplayEvent)
BEGIN_SERIALIZATION_CHILD(AnimGameplayEvent)
END_SERIALIZATION()

IMPLEMENT_OBJECT_RTTI(AnimPolylineEvent)
BEGIN_SERIALIZATION_CHILD(AnimPolylineEvent)
END_SERIALIZATION()

void AnimPolylineEvent::serialize(ArchiveMemory& _archive)
{
    Super::serialize(_archive);

    if (_archive.isReading())
    {
        StringID::StringIdValueType nameId;
        _archive.serialize(nameId);
        m_polylineID = StringID(nameId);
    }
    else
    {
        StringID::StringIdValueType nameId = m_polylineID.GetValue();
        _archive.serialize(nameId);
    }
}

void AnimPolylineEvent::setPoints( const Vec2d* _points, u32 _numPoints )
{
    ITF_ASSERT_MSG(_points, "Setting NULL points to AnimPolylineEvent!");
    m_points = _points;
    m_numPoints = _numPoints;
}

#ifdef ITF_SUPPORT_XML
bbool AnimPolylineEvent::ReadXML(PugiXMLWrap *pXML, NamesMap *_uidToBone,  int errBehavior)
{
    bbool ret = Super::ReadXML(pXML, _uidToBone, errBehavior);

    if (pXML->enterElement("Params"))
    {
        m_polylineID = pXML->readAttribute("PolylineID/value", "", (PugiXMLWrap::ErrorBehavior)errBehavior);
        pXML->exitElement();
        return ret;
    }
    return bfalse;
}

void AnimPolylineEvent::WriteXML(pugi::xml_node node, AnimSkeleton * skeleton)
{
    Super::WriteXML(node, skeleton);

    pugi::xml_node params = node.append_child("Params");
    {
        LocalxmlWriteString(params, "PolylineID", m_polylineID.getDebugString());
    }
}
#endif //ITF_SUPPORT_XML


IMPLEMENT_OBJECT_RTTI(AnimPartitionEvent)
BEGIN_SERIALIZATION_CHILD(AnimPartitionEvent)
END_SERIALIZATION()

void AnimPartitionEvent::serialize(ArchiveMemory& _archive)
{
    Super::serialize(_archive);
    _archive.serialize(m_percent);
}


#ifdef ITF_SUPPORT_XML
bbool AnimPartitionEvent::ReadXML(PugiXMLWrap *pXML, NamesMap *_uidToBone,  int errBehavior)
{
    bbool ret = Super::ReadXML(pXML, _uidToBone, errBehavior);

    if (pXML->enterElement("Params"))
    {
        m_percent = pXML->readFloatAttribute("Ratio/value", -1.f, (PugiXMLWrap::ErrorBehavior)errBehavior);
        pXML->exitElement();
    }
    return ret;
}

void AnimPartitionEvent::WriteXML(pugi::xml_node node, AnimSkeleton * skeleton)
{
    Super::WriteXML(node, skeleton);

    pugi::xml_node params = node.append_child("Params");
    {
        LocalxmlWriteFloat(params, "Ratio", m_percent);
    }
}
#endif //ITF_SUPPORT_XML

void AnimTrackFrameSoundEvents::serialize( ArchiveMemory& _pArchive )
{
    m_name.serialize(_pArchive);
    _pArchive.serialize(m_startRatio);
    _pArchive.serialize(m_parentBoneIndex);
}

void PbkRedirect::serialize(ArchiveMemory& _Archive, bbool )
{
    m_patchBankPathList.serialize(_Archive);
    m_oldPatchBankToNew.serialize(_Archive);
}

void PbkRedirect::clear()
{
    m_patchBankPathList.clear();
    m_oldPatchBankToNew.clear();
}


bbool PbkRedirect::findPatchBank( const String &_currentPath, String &_path )
{
    StringID id = _currentPath;
    KeyArray<i32>::Iterator it(m_oldPatchBankToNew);
    it.findKey(id);
    if (it.isEnd())
        return bfalse;

    _path       = m_patchBankPathList[it.second()];
    return btrue;
}

}

