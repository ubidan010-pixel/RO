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

#ifndef _ITF_ANIMATIONPOLYLINE_H_
#include    "engine/animation/AnimationPolyline.h"
#endif // _ITF_ANIMATIONPOLYLINE_H_

#ifndef _ITF_ANIMATIONRES_H_
#include "engine/animation/AnimationRes.h"
#endif //_ITF_ANIMATIONRES_H_

#ifndef _ITF_RESOURCEGROUP_H_
#include "engine/resources/ResourceGroup.h"
#endif //_ITF_RESOURCEGROUP_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_ANIMATIONSTATS_H_
#include "engine/animation/AnimationStats.h"
#endif //_ITF_ANIMATIONSTATS_H_

#ifndef _ITF_ANIMATIONPATCHBANK_H_
#include    "engine/animation/AnimationPatchBank.h"
#endif // _ITF_ANIMATIONPATCHBANK_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifdef ITF_IPAD
#include <algorithm>
#endif

#ifndef ITF_FINAL
#ifndef _ITF_ANIMATIONMANAGER_H_
#include "engine/animation/AnimationManager.h"
#endif //_ITF_ANIMATIONMANAGER_H_
#endif //!ITF_FINAL

#ifndef _ITF_RESOURCE_H_
#include "engine/resources/Resource.h"
#endif //_ITF_RESOURCE_H_

#ifdef __CWCC__
// TODO : hack for CW compiler crash
#pragma optimization_level 2
#endif

#ifndef _ITF_ANIMATIONDEPENDENCIES_H_
#include "engine/animation/AnimationDependencies.h"
#endif //_ITF_ANIMATIONDEPENDENCIES_H_

namespace ITF
{

static Color defaultColor_Animation[4] = {Color(1.f,1.f,1.f,1.f), Color(1.f,1.f,1.f,1.f), Color(1.f,1.f,1.f,1.f), Color(1.f,1.f,1.f,1.f)};
i32 TrackSet::s_trackCount = 0;

bool PrefetchAnim( const Path& path, PrefetchFactory::Params& params )
{
    return ResourceManager::PrefetchResource( path, params, Resource::ResourceType_AnimTrack );
}

TrackSet::TrackSet()
{
    m_resourceGroup.invalidateResourceId();
    m_parentResourceGroup.invalidateResourceId();

    m_globalBoundingBox.invalidate();
}

TrackSet::~TrackSet()
{
    destroy();
}

void TrackSet::setParentResourceGroup(ResourceGroup* _parent) 
{
    m_parentResourceGroup.setID(_parent);
    if ( !m_resourceGroup.isValidResourceId() )
    {
        m_resourceGroup = _parent->addResource(Resource::ResourceType_ResourceGroup, Path::EmptyPath);
    }
    else
    {
        ITF_ASSERT_MSG( find(_parent->getResourcesList().begin(), _parent->getResourcesList().end(), m_resourceGroup) != _parent->getResourcesList().end(), "the resource is valid but not contained in the parent" );
    }
}

void TrackSet::destroy()
{
    ResourceGroup* parentGrp = (ResourceGroup*)m_parentResourceGroup.getResource();
    if (parentGrp)
    {
        parentGrp->removeResource(m_resourceGroup);
    }
    m_resourceGroup.invalidateResourceId();
    m_nameToTrack.clear();
}


void TrackSet::addTrack(const StringID &_name, const String &_path, const Path & _dependencyFile)
{
    ResourceGroup* grp = (ResourceGroup*)m_resourceGroup.getResource();
    ITF_ASSERT(grp);
    if (m_nameToTrack.find(_name) >= 0 || !grp)
        return;

    
    if (!_dependencyFile.isEmpty())
    {
        AABB trackAABB;

        // Also resources contained inside animation BEFORE the animation to prevent cook problems
        bbool aabbFound = AnimDependenciesMapResource::fillGroupDependencies( grp, _path, _dependencyFile, &trackAABB );
        if (!aabbFound)
            aabbFound = AnimDependenciesMapResource::fillGroupDependencies( grp, _path, AnimationDependenciesMap::getDependenciesFile(_path, FILESERVER->getPlatform()), &trackAABB );

        ITF_WARNING_CATEGORY(Anim,NULL, aabbFound, "Animation %s: Invalid bounding box - please reload data with GenAnim to compute bounding box", StringToUTF8(_path).get());
        if (trackAABB.isValid())
        {
            if (m_globalBoundingBox.isValid())
                m_globalBoundingBox.grow(trackAABB);
            else
                m_globalBoundingBox = trackAABB;
        }
    }
    
    ResourceID trkRes = grp->addResource(Resource::ResourceType_AnimTrack, _path);
    m_nameToTrack.setID(_name, trkRes.getResourceRef());

}

bbool TrackSet::getTrackResource(const StringID &_name, ResourceID &_res, bbool _useDefault) const
{
    ResourceIdentifierType trackRes;
    InvalidateResourceIdentifier(trackRes);
    NamesObjectRefMap::Iterator nameIter(m_nameToTrack);
    nameIter.findKey(_name);

    if (nameIter.isValid())
    {
        trackRes = nameIter.second();
    } else if (!_useDefault)
    {
        return bfalse;
    }

    Resource *res = getResourceFromIdentifier(trackRes);
    if (!res)
        return bfalse;

    _res = res->getID();
    ITF_ASSERT_MSG(((ResourceGroup*)m_resourceGroup.getResource())->usesResource(_res), "m_trackResource and group DO NOT MATCH !!!!");
    return btrue;
}


AnimTrack * TrackSet::getTrack(const StringID &_name, bbool _useDefault) const
{
    ResourceID res;
    if (!getTrackResource(_name, res, _useDefault))
        return NULL;

    AnimTrackResource* pAnimTrackRes = static_cast<AnimTrackResource*>(res.getResource());
    ITF_ASSERT(pAnimTrackRes && pAnimTrackRes->isPhysicallyLoaded());
    return pAnimTrackRes->getTrack();
}

AnimMeshScene::AnimMeshScene() : m_vertexFormat(VertexFormat_PTa)
{
    ANIMATIONSTAT_ADD_ANIMINSTANCE(this);
}

AnimMeshScene::~AnimMeshScene()
{
    for (u32 i=0; i<m_pMesh.size(); ++i)
    {
        m_pMesh[i]->emptyIndexedMesh();
        SF_DEL(m_pMesh[i]);
    }
    m_pMesh.clear();
    ANIMATIONSTAT_DEL_ANIMINSTANCE(this);
}

AnimMeshScene *AnimMeshScene::AllocNewMesh(int _VtxFormat, ITF_IndexBuffer *_staticIndex, u32 _bufferSize)
{
    AnimMeshScene *animMeshScene = newAlloc(mId_SafeArrayCache,AnimMeshScene());
    animMeshScene->m_vertexFormat = _VtxFormat;
    if (animMeshScene->AllocAddMesh(_staticIndex, _bufferSize))
        return animMeshScene;

    delete animMeshScene;
    return NULL;
}

bbool AnimMeshScene::AllocAddMesh()
{
    if (m_pMesh.size() == 0) return bfalse;
    return AllocAddMesh(m_pMesh[0]->m_ElementList[0].m_indexBuffer);
}

bbool AnimMeshScene::AllocAddMesh(ITF_IndexBuffer *_staticIndex, u32 _bufferSize)
{
    if (m_pMesh.size() >= MAX_MESHES)
        return bfalse;

    ITF_Mesh *pMesh = newAlloc(mId_Mesh, ITF_Mesh());
#if defined(ANIMUSESHADER)
    pMesh->addElement();
#elif defined(ANIMUSEGEOMETRYSHADER)
    pMesh->initStaticIndexMesh(VertexFormat_PatchAnim, _staticIndex, _bufferSize);
#else
    pMesh->initStaticIndexMesh(m_vertexFormat, _staticIndex, _bufferSize);
#endif
    m_pMesh.push_back(pMesh);
    return btrue;
}

AnimInfo::AnimInfo() :
    m_color(Color::white()),
    m_selfIllumColor(Color::zero()),
    m_fogColor(Color::zero()),
    m_Pos(0.f, 0.f, 0.0f),
    m_Angle(0.f),
    m_Scale(1.f, 1.f),
    m_IsLookingRight(bfalse),
    m_useRootRotation(btrue),
    m_prevPercent(0.f),
    m_vertexFormat(VertexFormat_PTa),
    m_canonizeTransitions(btrue),
    m_hDiv(2),
    m_vDiv(2),
    m_patchPointDynAlloc(16),
    m_polylineDynAlloc(2),
    m_forceBMLRecompute(bfalse),
    m_useWorking(bfalse),
    m_state(0),
    m_templateBufferFreeIndex(0),
#ifdef ITF_SUPPORT_MULTITHREADED
    m_currFrameDataIndex(0),
#endif
    m_renderPassFrontierPatchIndex(U32_INVALID),
    m_renderPass(0),
    m_minimalBonesUpdate(bfalse)
    {
        AnimFrameInfo   emptyFrame;
        m_frameInfo.push_back(emptyFrame);
        m_patchPointDyn.reserve(m_patchPointDynAlloc);
        m_scissor.invalidate();

        for (u32 i=0; i<FRAMEDATA_NB; i++)
            m_frameDataList[i].m_polylineDyn.reserve(m_polylineDynAlloc);
    }

AnimInfo::~AnimInfo()
{
    clearPrev();
}

bbool AnimInfo::ComputeBonesFromLocalToWorld(bbool _forceCompute)
{
    AnimFrameData *pFrameData = getWorkingFrameData();
    SafeArray<AnimBoneDyn>  *mainBoneDynList= &pFrameData->m_bonesList;
#ifndef NO_RUNTIME_LOCK
    ITF_VECTOR<AnimLock>    *mainLocked     = &pFrameData->m_locked;
#endif

    AnimTrack               *baseTrack      = getWorkingTrackReal();
    ITF_ASSERT(baseTrack);
    AnimSkeleton            *skeletonPtr    = getWorkingSkeleton();
    ITF_ASSERT(skeletonPtr);

    i32 rootIndex = skeletonPtr->getRootIndex();
    if (rootIndex < 0)
        return bfalse;
    AnimBoneDyn *mainBoneDyn    = &(*mainBoneDynList)[rootIndex];

    Vec2d   translateValue;
    bbool   isDefaultRight  = (baseTrack->m_tags & ANIM_TRACK_IS_DEFAULT_RIGHT) != 0;
    bbool   inverted        = isDefaultRight != m_IsLookingRight;

    Vec2d origin(0, 1);
    if (_forceCompute)
    {
        mainBoneDyn->m_PosLocal     = origin;
#ifndef NO_RUNTIME_LOCK
        skeletonPtr->ComputeBonesGlobalPos(mainBoneDynList, mainLocked, m_minimalBonesUpdate);
#else
        skeletonPtr->ComputeBonesGlobalPos(mainBoneDynList, m_minimalBonesUpdate);
#endif
        translateValue              = m_Pos.truncateTo2D();
    } else
    {
        Vec2d   translateIn = origin - mainBoneDyn->m_PosLocal;
        translateIn.m_y *= -1.f;
        translateIn     *= m_Scale;
        if (inverted)   translateIn.m_x *= -1.f;
        translateValue  = m_Pos.truncateTo2D() + translateIn;
    }

    skeletonPtr->ComputeBonesEnd(mainBoneDynList, true, m_minimalBonesUpdate);
    skeletonPtr->Scale(mainBoneDynList, m_Scale, m_globalData, m_minimalBonesUpdate);
    skeletonPtr->Rotate(mainBoneDynList, inverted ? -m_Angle : m_Angle, m_globalData, m_minimalBonesUpdate);
    if (inverted)
        skeletonPtr->SwapHorizontal(mainBoneDynList, m_globalData, m_minimalBonesUpdate);
    skeletonPtr->Translate(mainBoneDynList, translateValue, m_globalData, m_minimalBonesUpdate);

    return btrue;
}

Vec2d AnimInfo::ComputePointFromLocalToWorld(const Vec2d &_point, AnimTrack* _baseTrack) const
{
    bbool           isDefaultRight  = (_baseTrack->m_tags & ANIM_TRACK_IS_DEFAULT_RIGHT) != 0;
    bbool           inverted        = isDefaultRight != m_IsLookingRight;

    Vec2d gPos(_point.m_x, 1.f - _point.m_y);
    gPos      *= m_Scale;
    gPos.Rotate(inverted ? -m_Angle : m_Angle);
    if (inverted)
        gPos.m_x  *= -1.0f;

    gPos += m_Pos.truncateTo2D();
    return gPos;
}

void AnimInfo::ComputePolylinePoints()
{
    AnimSkeleton            *skeleton       = getWorkingSkeleton();
    SafeArray<AnimBoneDyn>  &bonesList      = getWorkingBoneList();
    ITF_VECTOR<StringID>    &polyRefList    = getWorkingPolyList();

    for (ITF_VECTOR<StringID>::const_iterator iter =  polyRefList.begin();
        iter != polyRefList.end(); iter++)
    {
        AnimPolyline *pPolyline;
        if (skeleton->getPolyline(*iter, pPolyline))
        {
            pPolyline->ComputeGlobalPos(&bonesList, getPolylinePointBuffer(*iter, bfalse), m_globalData);
        }
    }
}

void AnimInfo::updateCurFrameData()
{
    AnimFrameData *pFrameData = getWorkingFrameData();
    u32 frameInfoSize = m_frameInfo.size();

    if (frameInfoSize != pFrameData->m_bmlInfo.size())
        pFrameData->m_bmlInfo.resize(frameInfoSize);

    // Polylines
    ITF_VECTOR<StringID> &polyRefList = getWorkingPolyList();

    polyRefList.clear();
    pFrameData->m_trackExtList.resize(frameInfoSize);
    for (u32 i=0; i<frameInfoSize; i++)
    {
        AnimFrameInfo   &frameInfo  = m_frameInfo[i];
        AnimBMLInfo     &bmlInfo    = pFrameData->m_bmlInfo[i];

        pFrameData->m_trackExtList[i] = frameInfo.m_pTrackExt;
        if (frameInfo.m_useVisuals)
        {
            bmlInfo.m_bml = getFrameTrackReal(i)->GetAnimBMLFrame(getFrame(i));
            if (bmlInfo.m_bml && frameInfo.m_bonesUsed.size() > 0)
            {
                bmlInfo.m_bml->setComputeTab(pFrameData->m_trackExtList[i],
                    frameInfo.m_bonesUsed,
                    bmlInfo.m_templateUsed,
                    m_state);
            }
            AnimTrackPolyline * polylinePtr = getFrameTrackReal(i)->GetAnimPolylineFrame(getFrame(i));
            if (polylinePtr)
            {
                for (VectorAnim<StringID>::const_iterator iter = polylinePtr->m_polylineRefs.begin();
                     iter != polylinePtr->m_polylineRefs.end(); iter++)
                {
                    polyRefList.push_back(*iter);
                }
            }
        } else
        {
            bmlInfo.m_bml = NULL;
        }
    }
}

void AnimInfo::SetFrame()
{
    AnimFrameData           *pFrameData     = getWorkingFrameData();
    SafeArray<AnimBoneDyn>  *mainBoneDynList= &pFrameData->m_bonesList;
#ifndef NO_RUNTIME_LOCK
    ITF_VECTOR<AnimLock>    *mainLocked     = &pFrameData->m_locked;
#endif
    AnimSkeleton            *skeletonPtr    = NULL;

    u32 frameInfoSize = m_frameInfo.size();
    ITF_ASSERT_MSG(frameInfoSize, "Playing animation with no frameInfo !");
    if (frameInfoSize == 0)
        return;

    for (u32 i=0; i<frameInfoSize; i++)
    {
        AnimFrameInfo   &frameInfo = m_frameInfo[i];
        AnimTrack       *curTrack  = getFrameTrackReal(i);
        
        if (!curTrack)
            return;

        if (i == 0)
        {
            skeletonPtr = curTrack->getSkeleton();
        }
        else
        {
            ITF_ASSERT(skeletonPtr == curTrack->getSkeleton());
        }

		frameInfo.m_fFrame = f32_Max(frameInfo.m_fFrame,0.0f);
    }

    ITF_ASSERT_MSG(skeletonPtr, "Error finding skeleton, please check data !");
    if (!skeletonPtr)
    {
        return;
    }

    u32 rootIndex = skeletonPtr->getRootIndex(); // create bone order
    if (mainBoneDynList->size() < skeletonPtr->m_BonesTPauseDyn.size())
        mainBoneDynList->resize(skeletonPtr->m_BonesTPauseDyn.size());

    skeletonPtr->SetTPause(mainBoneDynList);

#ifndef NO_RUNTIME_LOCK
    mainLocked->clear();
#endif
    for (u32 i=0; i<m_frameInfo.size(); i++)
    {
        AnimTrack       *mainAnim  = getFrameTrackReal(i); // test of mainAnim != NULL make before
		AnimFrameInfo &frameInfo = m_frameInfo[i];
        AnimFrameInfo::BonesArray* bonesUsed = frameInfo.m_bonesUsed.size() ? &frameInfo.m_bonesUsed : NULL;
#ifdef ANIM_TRACK_CACHE
        if (i >= m_frameCache.size())
        {
            m_frameCache.resize(i+1);
            m_frameCache[i].m_pTrackExt = NULL;
        }
		AnimFrameCache &frameCache = m_frameCache[i];
        if (frameCache.m_pTrackExt != getFrameTrackExt(i))
        {
            u32 cacheSize = ANIM_TRACK_CACHE_SIZE*skeletonPtr->m_BonesCurFrame.size();
            // if the skeleton change, resize the cache
            frameCache.m_cashTab.resize(cacheSize);

            AnimTrackBonePASCut *cachePtr =  &frameCache.m_cashTab[0];
            AnimTrackBonePASCut *cachePtrEnd = cachePtr + cacheSize;
            for (; cachePtr < cachePtrEnd; cachePtr++)
                cachePtr->m_frame = -1;
            frameCache.m_pTrackExt = getFrameTrackExt(i);
        }
#endif
#ifdef ANIM_TRACK_CACHE
#ifndef NO_RUNTIME_LOCK
        mainAnim->ComputeAnimFrame(mainBoneDynList, frameCache.m_cashTab, mainLocked, getFrame(i), frameInfo.m_percent, frameInfo.m_useVisuals, bonesUsed, m_minimalBonesUpdate);
#else
        mainAnim->ComputeAnimFrame(mainBoneDynList, frameCache.m_cashTab, getFrame(i), frameInfo.m_percent, frameInfo.m_useVisuals, bonesUsed, m_minimalBonesUpdate);
#endif //NO_RUNTIME_LOCK
#else
#ifndef NO_RUNTIME_LOCK
        mainAnim->ComputeAnimFrame(mainBoneDynList, mainLocked, getFrame(i), frameInfo.m_percent, frameInfo.m_useVisuals, bonesUsed, m_minimalBonesUpdate);
#else
        mainAnim->ComputeAnimFrame(mainBoneDynList, getFrame(i), frameInfo.m_percent, frameInfo.m_useVisuals, bonesUsed, m_minimalBonesUpdate);
#endif //NO_RUNTIME_LOCK
#endif
    }

    if (!m_minimalBonesUpdate)
    {
        if (m_prevPercent > 0.5f)
        {
            AnimFrameData *prevFrameData = getPreviousFrameData();

            pFrameData->m_bmlInfo        = prevFrameData->m_bmlInfo;
            pFrameData->m_polyRefList    = prevFrameData->m_polyRefList;
            pFrameData->m_trackExtList   = prevFrameData->m_trackExtList;
        } else
        {
            updateCurFrameData();
        }

        u32 bmlInfoSize = pFrameData->m_bmlInfo.size();
        for (u32 i=0; i<bmlInfoSize; i++)
        {
			AnimTrackBML *bml = pFrameData->m_bmlInfo[i].m_bml;
            if (bml)
            {
                bml->templateToDynamicsBonesLen(this, pFrameData->m_trackExtList[i]);
            }
        }

        // Blend - Local position method
        if (m_prevPercent > 0.f)
        {
            AnimFrameData *prevFrameData = getPreviousFrameData();
            // Blend Must NOT affect object position !!!
            f32                     curPercent  = 1.f - m_prevPercent;
            u32                     dynCount = mainBoneDynList->size();
            SafeArray<AnimBoneDyn>  *prevBoneDynList= &prevFrameData->m_bonesList;
#ifndef NO_RUNTIME_LOCK
            ITF_VECTOR<AnimLock>    *prevLocked     = &prevFrameData->m_locked;
#endif
            AnimBoneDyn       *curBone    = &(*mainBoneDynList)[0];
            const AnimBoneDyn *prevBone   = &(*prevBoneDynList)[0];
            for (u32 d=0; d<dynCount; d++,++curBone,++prevBone)
            {
                f32         angleDiff   = m_canonizeTransitions ?
                    getShortestAngleDelta(curBone->m_AngleLocal, prevBone->m_AngleLocal) :
                (prevBone->m_AngleLocal - curBone->m_AngleLocal);

                //curBone->m_PosLocal     = prevBone->m_PosLocal*m_prevPercent + curBone->m_PosLocal*curPercent;
                Vec2d::Scale(&curBone->m_PosLocal, &curBone->m_PosLocal, curPercent);
                Vec2d::ScaleAdd(&curBone->m_PosLocal, &prevBone->m_PosLocal, m_prevPercent, &curBone->m_PosLocal);
                //curBone->m_Scale        = prevBone->m_Scale*m_prevPercent + curBone->m_Scale*curPercent;
                Vec2d::Scale(&curBone->m_Scale, &curBone->m_Scale, curPercent);
                Vec2d::ScaleAdd(&curBone->m_Scale, &prevBone->m_Scale, m_prevPercent, &curBone->m_Scale);

				curBone->m_AngleLocal = f32_MulAdd(angleDiff,m_prevPercent,curBone->m_AngleLocal);
                curBone->m_Alpha      = f32_MulAdd(prevBone->m_Alpha,m_prevPercent,curBone->m_Alpha*curPercent);
            }
#ifndef NO_RUNTIME_LOCK
            // blend of locks
            ITF_VECTOR<AnimLock> finalLockList;
            ITF_VECTOR<AnimLock>::iterator  prevIter    = (*prevLocked).begin();
            ITF_VECTOR<AnimLock>::iterator  curIter     = (*mainLocked).begin();

            AnimLock    anm;
            for (;prevIter != (*prevLocked).end(); ++prevIter)
            {
                for (; curIter != (*mainLocked).end() &&
                    (*curIter).m_details.m_Index < (*prevIter).m_details.m_Index; ++curIter)
                    finalLockList.push_back(*curIter);

                if (curIter != (*mainLocked).end() && (*prevIter).m_details.m_Index == (*curIter).m_details.m_Index)
                {
                    // blend locks
                    anm.m_details.m_Index = (*curIter).m_details.m_Index;
                    ITF_ASSERT((*curIter).m_details.m_OrderedIndex == (*prevIter).m_details.m_OrderedIndex);
                    anm.m_details.m_OrderedIndex  = (*curIter).m_details.m_OrderedIndex;
                    anm.m_details.m_AngleSign = (*prevIter).m_details.m_AngleSign | (*curIter).m_details.m_AngleSign;
                    if (anm.m_details.m_AngleSign == 0x3)
                        anm.m_details.m_AngleSign = 0x0;
                    //anm.m_Locked        = (*prevIter).m_Locked*m_prevPercent + (*curIter).m_Locked*curPercent;
                    Vec2d::Scale(&anm.m_Locked, &(*curIter).m_Locked, curPercent);
                    Vec2d::ScaleAdd(&anm.m_Locked, &(*prevIter).m_Locked, m_prevPercent, &anm.m_Locked);

                    anm.m_LockedAngle   = (*prevIter).m_LockedAngle*m_prevPercent + (*curIter).m_LockedAngle*curPercent;
                } else
                    finalLockList.push_back(*prevIter);
            }
            for (; curIter != (*mainLocked).end(); ++curIter)
                finalLockList.push_back(*curIter);

            for (u32 i=0; i<finalLockList.size(); i++)
                (*mainBoneDynList)[finalLockList[i].m_details.m_Index].m_LockedIndex = i;

            *mainLocked = finalLockList;
#endif
            //////////////////////////////////////////////////////////////////////////
        }
    } else
    {
        updateCurFrameData();
        u32 bmlInfoSize = pFrameData->m_bmlInfo.size();
        for (u32 i=0; i<bmlInfoSize; i++)
        {
			AnimTrackBML *bml = pFrameData->m_bmlInfo[i].m_bml;
            if (bml)
            {
                bml->templateToDynamicsBonesLen(this, pFrameData->m_trackExtList[i]);
            }
        }
    }

    if (rootIndex != U32_INVALID)
    {
        AnimBoneDyn     *mainBoneDyn    = &(*mainBoneDynList)[rootIndex];
        bbool            forceRecompute = bfalse;
#ifndef NO_RUNTIME_LOCK
        if (mainLocked->size())
        {
            skeletonPtr->ComputeBonesGlobalPos(mainBoneDynList, mainLocked, m_minimalBonesUpdate);
            skeletonPtr->ComputeIK(mainBoneDynList, mainLocked);
            if (!m_useRootRotation)
            {
                mainBoneDyn->m_AngleLocal   = 0.f;
                forceRecompute              = btrue;
            } else
            {
                skeletonPtr->ComputeBonesGlobalPos(mainBoneDynList, mainLocked, m_minimalBonesUpdate);
            }
        }
        else
#endif
        {
            if (!m_useRootRotation)
            {
                mainBoneDyn->m_AngleLocal   = 0.f;
            }
#ifndef NO_RUNTIME_LOCK
            skeletonPtr->ComputeBonesGlobalPos(mainBoneDynList, mainLocked, m_minimalBonesUpdate);
#else
            skeletonPtr->ComputeBonesGlobalPos(mainBoneDynList, m_minimalBonesUpdate);
#endif
        }
        ComputeBonesFromLocalToWorld(forceRecompute);
        ComputePolylinePoints();
    }
}

void AnimInfo::clearTemplatesBuffer()
{
    m_templateBufferFreeIndex = 0;
    m_templateToPatchPointDyn.clear();
}

AnimPatchPointDyn * AnimInfo::getTemplatePatchPointBuffer(AnimTemplate* _template, bbool _allowAddOnBuffer)
{
    if (!_allowAddOnBuffer)
    {
        KeyArray<IndexSize, uPtr>::Iterator iter(m_templateToPatchPointDyn);
        iter.findKey(uPtr(_template));
        if (!iter.isEnd())
        {
            ITF_ASSERT_CRASH(iter.second().m_size == _template->m_PatchPoints.size() && iter.second().m_index + _template->m_PatchPoints.size() <= m_patchPointDyn.size(), "Accessing bad patchPoint buffer");
            return &m_patchPointDyn[iter.second().m_index];
        }
    }
    ITF_ASSERT_MSG(_allowAddOnBuffer, "Template on buffer not added before used !!!" );

    IndexSize idxSize;
    idxSize.m_index = m_templateBufferFreeIndex;
    idxSize.m_size  = _template->m_PatchPoints.size();
    m_templateBufferFreeIndex = idxSize.m_index + _template->m_PatchPoints.size();

    m_templateToPatchPointDyn.setNewValue(uPtr(_template), idxSize);
    while (m_patchPointDynAlloc < m_templateBufferFreeIndex)
        m_patchPointDynAlloc *= 2;
    m_patchPointDyn.reserve(m_patchPointDynAlloc);
    m_patchPointDyn.resize(m_templateBufferFreeIndex);

    return &m_patchPointDyn[idxSize.m_index];
}

void AnimInfo::copyWorkingToCurrent()
{
    ITF_ASSERT_MSG(m_useWorking, "Do synchronization only for working process!");
    ITF_ASSERT_MSG(getWorkingBoneList().size(), "working bone list is not set, verify if SetFrame has been called correctly");

#ifdef ITF_SUPPORT_MULTITHREADED
    if (!getWorkingBoneList().size())
        return;

    //Instead of doing a copy, just change between working index and currentIndex
    m_currFrameDataIndex = 1 - m_currFrameDataIndex;
#endif
}


Vec2d* AnimInfo::getPolylinePointBuffer(const StringID & _polyID, bbool forReading)
{
    SafeArray<Vec2d> * polylineDyn;
#ifndef ITF_SUPPORT_MULTITHREADED
     polylineDyn = &m_frameDataList[CUR_FRAME_DATA_INDEX].m_polylineDyn;
#else
    if (m_useWorking && !forReading)
        polylineDyn = &m_frameDataList[1-m_currFrameDataIndex].m_polylineDyn;
    else
        polylineDyn = &m_frameDataList[m_currFrameDataIndex].m_polylineDyn;
#endif
#ifdef ITF_WII
	i32 IDX = m_polylineToPolylineDyn.find(_polyID);
	if (IDX >= 0) return &(*polylineDyn)[m_polylineToPolylineDyn.getAt(IDX)];
#else
    KeyArray<u32>::Iterator iter(m_polylineToPolylineDyn);
    iter.findKey(_polyID);
    if (!iter.isEnd())
        return &(*polylineDyn)[iter.second()];
#endif

    ITF_ASSERT_MSG(!forReading, "If we reallocate data for reading that mean we are reading bad data !");

    AnimSkeleton * skeleton = getWorkingSkeleton(); // here we are not on reading mode, so we use working polyline
    if (!skeleton)
        return NULL;

    AnimPolyline * pPolyline;
    if (!skeleton->getPolyline(_polyID, pPolyline) || pPolyline->m_PolylinePointList.size() == 0)
        return NULL;

    u32 newPos = (*polylineDyn).size(); // poly or working poly size is the same
    u32 newSize= newPos + pPolyline->m_PolylinePointList.size();

    m_polylineToPolylineDyn.set(_polyID, newPos);
    while (m_polylineDynAlloc < newSize)
        m_polylineDynAlloc *= 2;

    // reallocate polyline AND working polyline 
    for (u32 i=0; i<FRAMEDATA_NB; i++)
    {
        m_frameDataList[i].m_polylineDyn.reserve(m_polylineDynAlloc);
        m_frameDataList[i].m_polylineDyn.resize(newSize);
    }
    return &(*polylineDyn)[newPos];
}


void AnimInfo::setPrev(f32 _percent)
{
#ifndef ITF_FINAL
    if (!verifyJob())
        return;
#endif //!ITF_FINAL

    clearPrev();
    
    AnimFrameData *pFrameData = getCurrentFrameData();

    ITF_ASSERT_MSG(pFrameData->m_bonesList.size(), "current boneList Empty !");
    ITF_ASSERT_MSG(pFrameData->m_trackExtList.size(), "no animation is playing !");
    if (pFrameData->m_bonesList.size() == 0 || pFrameData->m_trackExtList.size() == 0)
        return;

    m_prevPercent = _percent;
    // before setting previous frame, update curFrame data
    updateCurFrameData();
    m_frameDataList[PREV_FRAME_DATA_INDEX] = *pFrameData;
}


void AnimInfo::clearPrev()
{
#ifndef ITF_FINAL
    if (!verifyJob())
        return;
#endif //!ITF_FINAL

    m_frameDataList[PREV_FRAME_DATA_INDEX].clear();
    m_prevPercent       = -1.f;
}

#ifndef ITF_FINAL
ITF::bbool AnimInfo::verifyJob()
{

    ObjectRef   actorRef;
    if (ANIM_MANAGER->IsInfoInAnimForSyncro(this, &actorRef))
    {
        Actor *actor = (Actor *)actorRef.getObject();
        if (actor)
        {
            String8 errorStr =  "Parallel actor is doing a setAnim AFTER updating anim component\nPlease verify component order";
            ITF_ASSERT_CRASH(0, errorStr.cStr());

            ITF_WARNING_CATEGORY(GPP,actor, true, errorStr.cStr());
            actor->setDataError(errorStr);
        }
        return bfalse;
    }
    return btrue;
}
#endif //!ITF_FINAL
} // namespace ITF


