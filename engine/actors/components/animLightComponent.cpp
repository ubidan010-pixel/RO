#include "precompiled_engine.h"

#ifndef _ITF_ANIMLIGHTCOMPONENT_H_
#include "engine/actors/components/AnimLightComponent.h"
#endif //_ITF_ANIMLIGHTCOMPONENT_H_

#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_

#ifndef _ITF_ANIMATIONSKELETON_H_
#include    "engine/animation/AnimationSkeleton.h"
#endif // _ITF_ANIMATIONSKELETON_H_

#ifndef _ITF_ANIMATIONPOLYLINE_H_
#include    "engine/animation/AnimationPolyline.h"
#endif // _ITF_ANIMATIONPOLYLINE_H_

#ifndef _ITF_ANIMATIONPATCHES_H_
#include    "engine/animation/AnimationPatches.h"
#endif // _ITF_ANIMATIONPATCHES_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_ANIMATIONRES_H_
#include "engine/animation/AnimationRes.h"
#endif //_ITF_ANIMATIONRES_H_

#ifndef _ITF_SCENE_MANAGER_H_
#include "engine/scene/sceneManager.h"
#endif //_ITF_SCENE_MANAGER_H_

#ifndef _ITF_GAMEPLAYEVENTS_H_
#include "gameplay/GameplayEvents.h"
#endif //_ITF_GAMEPLAYEVENTS_H_

#ifndef _ITF_JOB_H_
#include "engine/scheduler/job.h"
#endif //_ITF_JOB_H_

#ifndef _ITF_SCHEDULER_H_
#include "engine/scheduler/scheduler.h"
#endif //_ITF_SCHEDULER_H_

#ifndef _ITF_JOBUPDATEVISUAL_H_
#include "engine/actors/components/jobUpdateVisual.h"
#endif //_ITF_JOBUPDATEVISUAL_H_

#ifndef _ITF_ANIMATIONMANAGER_H_
#include "engine/animation/AnimationManager.h"
#endif //_ITF_ANIMATIONMANAGER_H_

#ifndef _ITF_METRONOMEMANAGER_H_
#include "engine/sound/MetronomeManager.h"
#endif //_ITF_METRONOME_H_

#ifndef _ITF_STATSMANAGER_H_
#include "engine/stats/statsManager.h"
#endif //_ITF_STATSMANAGER_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

namespace ITF
{

BEGIN_SERIALIZATION(BoneMapping)
	SERIALIZE_MEMBER("inputBone",m_inputBone);
	SERIALIZE_MEMBER("outputBone",m_outputBone);
END_SERIALIZATION()

BEGIN_SERIALIZATION(AnimLightFrameInfo)
    SERIALIZE_OBJECT("subAnimFrameInfo", m_subAnimFrameInfo);
    SERIALIZE_MEMBER("weight", m_weight);
    SERIALIZE_MEMBER("usePatches", m_usePatches);
END_SERIALIZATION()

IMPLEMENT_OBJECT_RTTI(AnimLightComponent)
BEGIN_SERIALIZATION_CHILD(AnimLightComponent)

    BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
        SERIALIZE_MEMBER("syncOffset", m_syncOffset);
        SERIALIZE_MEMBER("animInstance", m_animInstance);
        SERIALIZE_FUNCTION(postChangeProperties, ESerialize_PropertyEdit_Load);
    END_CONDITION_BLOCK()

    BEGIN_CONDITION_BLOCK(ESerializeGroup_Checkpoint)
        SERIALIZE_MEMBER("playAnim",m_playAnimName);
        SERIALIZE_MEMBER("playAnimFrames",m_playAnimNumFrames);
        SERIALIZE_CONTAINER_OBJECT("currentFrameSubAnims", m_currentFrameSubAnims);
    END_CONDITION_BLOCK()

END_SERIALIZATION()

BEGIN_VALIDATE_COMPONENT(AnimLightComponent)
    VALIDATE_COMPONENT_PARAM("frontZOffset", getTemplate()->getFrontZOffset() >= getTemplate()->getBackZOffset(), "must be positive greater than back !");
    VALIDATE_COMPONENT_PARAM("", m_subAnimSet.getSubAnimCount(), "Error on animation component in actor (bad version ?): '%s'", m_actor->getLua().getString8().cStr());
    VALIDATE_COMPONENT_PARAM("", m_subAnimSet.isResolveMarkersOk(), "Error on animation component in actor (anim error): '%s'", m_actor->getLua().getString8().cStr());
END_VALIDATE_COMPONENT()

AnimLightComponent::AnimLightComponent()
: Super()
, m_AnimMeshScene(NULL)
, m_animID()
, m_rootIndex(U32_INVALID)
, m_ignoreEvents(bfalse)
, m_currentDt(0.f)
, m_playAnimNumFrames(U32_INVALID)
, m_previousLookRight(bfalse)
, m_loadingOK(bfalse)
, m_currentColor(Color::white())
, m_colorSrc(Color::white())
, m_colorDst(Color::white())
, m_currentBlendFrames(0)
#ifdef USEPCT
, m_vertexFormat(VertexFormat_PCT)
#else
, m_vertexFormat(VertexFormat_PTa)
#endif
, m_drawEnabled(btrue)
, m_isProcedural(bfalse)
, m_updateAnimTime(btrue)
, m_isLooped(bfalse)
, m_useRootRotation(bfalse)
, m_animDefaultFlip(bfalse)
, m_forceDirectUpdate(0)
, m_semiForcedDirectUpdate(bfalse)
, m_isVisiblePrediction(btrue)
, m_disableVisiblePrediction(bfalse)
, m_useScissor(bfalse)
, m_syncOffset(0.f)
, m_scaleMultiplier(1.0f)
, m_rank2D(0)
, m_instancePositionOffset(0,0)
{
    m_jobUpdateVisual = newAlloc(mId_Animation, JobUpdateVisual);
    m_jobUpdateVisual->setAsManaged(btrue);
}


AnimLightComponent::~AnimLightComponent()
{
    clear();
    SF_DEL(m_jobUpdateVisual);
}

void AnimLightComponent::clear()
{
    u32 numNewSubAnims = m_newSubAnims.size();

    for ( u32 i = 0; i < numNewSubAnims; i++ )
    {
        delete m_newSubAnims[i];
    }

    m_newSubAnims.clear();
    m_subAnimSet.clear();

    SF_DEL(m_AnimMeshScene);
}

void AnimLightComponent::fillBoneMap()
{
	m_boneList = getTemplate()->getBoneList();
	for (ITF_VECTOR<BoneMapping>::const_iterator it = m_boneList.begin(); it != m_boneList.end(); ++it)
	{
		m_boneMap[it->m_inputBone] = it->m_outputBone;
	}
}

void AnimLightComponent::onCheckpointLoaded()
{
    Super::onCheckpointLoaded();

    if (!getTemplate()->getRestoreStateAtCheckpoint())
        m_currentFrameSubAnims.clear();

    if ( m_currentFrameSubAnims.size() == 0 )
    {
        setFirstAnim();
    }
    else
    {
        for (u32 i=0; i<m_currentFrameSubAnims.size(); i++)
        {
            m_currentFrameSubAnims[i].m_subAnimFrameInfo.setSubAnimSet(&m_subAnimSet);
        }
    }
}

void AnimLightComponent::addAnimByPath(const String & path, const String & name, i32 beginMarker, i32 endMarker)
{
    SubAnim_Template* newTemplate = newAlloc(mId_Animation,SubAnim_Template);

    // animation name
    newTemplate->setAnimPath(path);
    newTemplate->setFriendlyName(name);
    newTemplate->setIsLooped(btrue);

    SubAnim* subanim = newAlloc(mId_Animation,SubAnim);
    newTemplate->setStart(beginMarker * ANIMATION_FRAMERATE);
    newTemplate->setStop(endMarker * ANIMATION_FRAMERATE);
    subanim->setInternalTemplate(newTemplate);

    PathString_t animPath;

    getTemplate()->getAnimPath().getString(animPath);

    m_subAnimSet.addSubAnimList(subanim, animPath);
}


void AnimLightComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    m_instancePositionOffset.clear();
    m_subAnimSet.setTemplate(getTemplate()->getSubAnimTemplate());
    m_subAnimSet.setGlobalSyncOffset(m_syncOffset);

    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventReset,2551104629),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventActivate,2639596100),this);

    m_subAnimSet.linkSubAnimToTrack(m_actor->getResourceGroup(), getTemplate()->getAnimPath());
    m_currentColor = getTemplate()->getDefaultColor();

    if ( !m_animInstance.isEmpty() )
    {
        String animInstance;
        m_animInstance.getString(animInstance);
        addAnimByPath(animInstance, FilePath::getFilenameWithoutExtension(animInstance), -1, -1);
    }

    setFirstAnim();

	fillBoneMap();
    AABB aabb = m_subAnimSet.getGlobalBoundingBox();
    if (aabb.isValid())
    {
        aabb.Scale(getScale());
        GetActor()->growRelativeAABB(aabb);
    }
}

void AnimLightComponent::postChangeProperties()
{
    m_subAnimSet.setGlobalSyncOffset(m_syncOffset);
}


void AnimLightComponent::setFirstAnim()
{
    if ( getTemplate()->getDefaultAnim().isValid() )
    {
        setAnim(getTemplate()->getDefaultAnim());
    }
    else if ( m_subAnimSet.getSubAnimCount() > 0 )
    {
        setAnim(m_subAnimSet.getSubAnim(0)->getTemplate()->getFriendlyName());
    }
}

void AnimLightComponent::onResourceLoaded()
{
    Super::onResourceLoaded();
    ITF_ASSERT(m_actor->isPhysicalReady());

    m_rootIndex = U32_INVALID;
    if (m_subAnimSet.getSubAnimCount() == 0)
    {
        // validated later
        return;
    }

    if (!m_subAnimSet.isResolveMarkersOk())
    {
        PathString_t animPath;
        getTemplate()->getAnimPath().getString(animPath);
        m_subAnimSet.resolveMarkers(animPath);
    }


     // Verify if resources are the same ...
     //const ITF_VECTOR<SubAnim_Template> &tmplt = m_subAnimSet.getTemplate()->getSubAnimList();
     //for (u32 i=0; i<tmplt.size(); i++)
     //{
     //   const SubAnim_Template & tmp = tmplt[i];
     //   const AnimTrackExt* animTrack =  tmp.getTrackExt();
     //   
     //   ITF_ASSERT(m_actor->getResourceGroup()->usesResource(animTrack->m_trackResourceID, true));
     //}

    if (!m_subAnimSet.isResolveMarkersOk())
        return;

    const NamesObjectRefMap& map  = m_subAnimSet.m_nameToTrack;
    NamesObjectRefMap::Iterator it(map);
    for (;  !it.isEnd(); it.increment())
    {
        AnimTrack *track = m_subAnimSet.getTrack(it.first());
        ITF_ASSERT(track);
        if (track && track->m_tags & ANIM_TRACK_USE_ALPHA)
        {
            m_vertexFormat = VertexFormat_PCT;
            break;
        }
    }

    NamesObjectRefMap::Iterator itScissor(map);
    for (;  !itScissor.isEnd(); itScissor.increment())
    {
        AnimTrack *track = m_subAnimSet.getTrack(itScissor.first());
        ITF_ASSERT(track);
        if (track && track->m_scissor.isValid())
        {
            m_useScissor = btrue;
            break;
        }
    }


    if ( m_currentFrameSubAnims.size() == 0 )
    {
        processAnim();
    }

    if ( m_currentFrameSubAnims.size() == 0 )
    {
        SubAnimFrameInfo frameInfo(&m_subAnimSet);
        frameInfo.setSubAnimIndex(0);
        frameInfo.resetCurTime();
        addSubAnimToPlay(frameInfo,1.f,btrue);
    }
    else
    {
       resetSubAnimFrameInfoFlags();
       for (u32 i=0; i<m_currentFrameSubAnims.size(); i++)
       {
           const SubAnimFrameInfo& subframeinfo = m_currentFrameSubAnims[i].m_subAnimFrameInfo;
           updateSubAnimFrameInfoFlag(subframeinfo);
       }
    }

    commitSubAnimsToPlay();

    m_loadingOK = btrue;
    createAnimMeshScene();

    m_rootIndex = getBoneIndex(ITF_GET_STRINGID_CRC(root,170057116));

    if (m_visualAABB.isZero())
    {
        if ( getTemplate()->getVisualAABBanimID().isValid())
        {
            AnimTrack *track = m_subAnimSet.getTrack(getTemplate()->getVisualAABBanimID());
            if (track)
                m_visualAABB = track->m_bounding;
        }
    }

    if (m_visualAABB.isZero())
    {
        m_visualAABB = AABB(Vec2d(-0.5f, -0.5f), Vec2d(0.5f, 0.5f));
    }
#ifndef ANIMATION_FIX_ABB
    m_drawPredictionAABB = m_visualAABB;
#endif
}

void AnimLightComponent::createAnimMeshScene()
{
    ANIM_MANAGER->unregisterAnimForResourceUnload( m_actor->getRef() );
    
    SF_DEL(m_AnimMeshScene);

    // Force re creation of anim mesh !!
#if defined(ANIMUSEGEOMETRYSHADER)
    m_AnimMeshScene = AnimMeshScene::AllocNewMesh(VertexFormat_PatchAnim, getMyStaticIndexBuffer());
#else
    m_AnimMeshScene = AnimMeshScene::AllocNewMesh(m_vertexFormat, getMyStaticIndexBuffer());
#endif
    m_AnimMeshScene->m_pAnimDraw.setGlobalMaterialParams(m_matFlags, getTemplate()->getMaterialType(), getTemplate()->getBlendMode());
    if (getTemplate()->getFrontZOffset() != getTemplate()->getBackZOffset() )
        m_AnimMeshScene->m_AnimInfo.m_renderPassFrontierPatchIndex = 0; // This will allow use of mid draw
    
#ifndef ITF_FINAL
    m_AnimMeshScene->m_owner = m_actor->getRef();
#endif //!ITF_FINAL

    forceSynchronousFrame();
}

void AnimLightComponent::processColor()
{
    if ( m_colorSrc == m_colorDst )
    {
        return;
    }

    f32 t = m_faidingTime ? 1.f - ( m_faidingValue / m_faidingTime ) : 1.f;

    if ( t == 1 )
    {
        m_currentColor = m_colorSrc = m_colorDst;
    }
    else
    {
        m_currentColor = Color::Interpolate(m_colorSrc,m_colorDst,t);
    }
}

bool AnimLightComponent::updateData()
{
    // VERIFY IF IT WORK HERE BEFORE update
    AnimInfo & animInfo = m_AnimMeshScene->m_AnimInfo;
    if (animInfo.m_prevPercent > 0.f)
    {
        animInfo.m_prevPercent -= m_currentDt/(m_currentBlendFrames/ANIMATION_FPS);
        if (animInfo.m_prevPercent <= 0.f)
        {
            animInfo.clearPrev();
            m_currentBlendFrames = 0;
        }
    }

    return btrue;
}

bool AnimLightComponent::postUpdateData()
{
    return btrue;
}

void AnimLightComponent::processAnim()
{
    if ( m_playAnimName.isValid() )
    {
        commitPlayAnim();
    }
}

void AnimLightComponent::Update( f32 _deltaTime )
{
    STATS_ANIMLIGHTCOMPONENT_UPDATEINC

    m_currentDt = _deltaTime;

    ITF_ASSERT_MSG(m_AnimMeshScene, "Updating non initialized actor !");
    if (!m_AnimMeshScene || !m_actor->isPhysicalReady() )
    {
        return;
    }

	computeVisiblePrediction();
    
	if ( useShadow() )
	{
		f32 angle = 0.f;

		if (getTemplate()->getUseRootBone())
		{
			if (getNumPlayingSubAnims())
			{
				const SubAnim* currentSubAnim = getCurSubAnim(0);

				if ( currentSubAnim )
				{
					AnimSkeleton* skeleton = currentSubAnim->getSkeleton();
					u32 rootIndex = skeleton->getRootIndex();
					SafeArray<AnimBoneDyn> & boneDynList = m_AnimMeshScene->m_AnimInfo.getWorkingBoneList();

					if ( rootIndex < boneDynList.size() )
					{
						AnimBoneDyn *rootBone = &boneDynList[rootIndex];
						angle = rootBone->m_Angle;
					}
				}
			}
		}

		processShadow(_deltaTime, angle);
	}
	processAlpha(_deltaTime);
    processColor();
    
    // update visual AABB
    m_AnimMeshScene->m_AnimInfo.m_visualAABB = getVisualAABB();
#ifdef ANIMATION_FIX_ABB
    updateAABB();
#endif

    if (!updateData())
        return;

    processAnim();

    if ( m_updateAnimTime )
    {
        updateAnimTime(_deltaTime);
    }

    fillAnimInfo();

    if ( isSynchronous() )
    {
        mainJobUpdateVisuals(this);

        // When procedural the update is done later !
#ifndef ANIMATION_FIX_ABB
        updateAABB();
#endif
    }
    else
    {
        JobUpdateVisualContext context(this);
        AnimInfo & animInfo = m_AnimMeshScene->m_AnimInfo;

        m_jobUpdateVisual->setContext(context);
        ANIM_MANAGER->registerAnimForSyncro(&animInfo, m_actor->getRef());
        Scheduler::get()->pushJob(m_jobUpdateVisual);
    }
}

bbool AnimLightComponent::isLookingRight() const
{
    bbool isRight = !m_actor->isFlipped();

    if ( getDefaultAnimFlip() )
    {
        isRight = !isRight;
    }

    if ( getTemplate()->getStartFlip() )
    {
        isRight = !isRight;
    }

    return isRight;
}


void AnimLightComponent::updateScissor()
{
    ITF_ASSERT_CRASH(m_AnimMeshScene, "we enter here only when animMesh is valid!");

    AnimInfo *  pAnimInfo           = &m_AnimMeshScene->m_AnimInfo;

    pAnimInfo->m_scissor.invalidate();
    ITF_VECTOR<AnimFrameInfo>::const_iterator iter = pAnimInfo->m_frameInfo.begin();
    for (; iter != pAnimInfo->m_frameInfo.end(); ++iter)
    {
        AnimTrack *trk = (*iter).m_pTrackExt->getTrack();
        if (trk->m_scissor.isValid())
        {
            if (pAnimInfo->m_scissor.isValid())
            {
                pAnimInfo->m_scissor.grow(trk->m_scissor);
            } else
            {
                pAnimInfo->m_scissor = trk->m_scissor;
            }
        }
    }
    if (pAnimInfo->m_scissor.isValid())
    {
        pAnimInfo->m_scissor.Scale(getScale());
        if (m_actor->isFlipped())
            pAnimInfo->m_scissor.FlipHorizontaly(bfalse);
        pAnimInfo->m_scissor.Rotate(getOffsettedAngle(), bfalse);
        pAnimInfo->m_scissor.Translate(getOffsettedPos().truncateTo2D());
    }
}

bbool AnimLightComponent::fillAnimInfo()
{
    ITF_ASSERT(m_AnimMeshScene);
    if (!m_AnimMeshScene)
        return bfalse;

    AnimInfo *  pAnimInfo           = &m_AnimMeshScene->m_AnimInfo;

    if (getTemplate()->getDraw2d())
    {
        pAnimInfo->m_Pos   = Vec3d::Zero;
        pAnimInfo->m_Scale = Vec2d::One;
    }
    else
    {
        pAnimInfo->m_Pos   = getOffsettedPos();
        pAnimInfo->m_Scale = getScale();
    }

    bbool lookRight = isLookingRight();

    if (m_previousLookRight != lookRight )
    {
        if (pAnimInfo->m_prevPercent > 0.f)
        {
            pAnimInfo->clearPrev();
        }
        m_previousLookRight = lookRight;
    }

    pAnimInfo->m_frameInfo              = getAnimFrameInfo();
    ITF_ASSERT_MSG(pAnimInfo->m_frameInfo.size(), "trying to play empty anim");
    pAnimInfo->m_useWorking             = !isSynchronous();
    pAnimInfo->m_IsLookingRight         = lookRight;
    pAnimInfo->m_Angle                  = getOffsettedAngle();
    pAnimInfo->m_useRootRotation        = getUseRootRotation();
    pAnimInfo->m_vertexFormat           = getVertexFormat();
    pAnimInfo->m_hDiv                   = getHDiv();
    pAnimInfo->m_vDiv                   = getVDiv();
    pAnimInfo->m_canonizeTransitions    = getCanonizeTransitions();
    pAnimInfo->m_color                  = getColor() * m_colorFactor;
    pAnimInfo->m_color.m_a              *= getAlpha();
    pAnimInfo->m_selfIllumColor         = getTemplate()->getSelfIllumColor();
    pAnimInfo->m_fogColor               = getColorFog();
    pAnimInfo->m_minimalBonesUpdate     = !isVisiblePrediction() && !getDisableVisiblePrediction();
    pAnimInfo->m_globalData.clear();
#ifndef ANIMATION_FIX_ABB
    pAnimInfo->m_patchAABB.invalidate();
#endif

    if (m_useScissor)
        updateScissor();

    return btrue;
}

void AnimLightComponent::forceSynchronousFrame()
{
    ITF_ASSERT_MSG(Synchronize::getCurrentThreadId() == ThreadSettings::m_settings[eThreadId_mainThread].m_threadID, "direct update must be done in main thread!");
    ITF_ASSERT_MSG(m_AnimMeshScene, "Anim light component not loaded correctly");

    if (!m_AnimMeshScene)
        return;

    // clear events ...
    m_frameEvents.clear();

    // Only process Bone, this is for bones data, no visual !!
    JobUpdateVisualContext context(this);
    fillAnimInfo();
    mainJobUpdateVisuals(&context);

    // for non procedural anims, copy bone list from working
    if (m_AnimMeshScene->m_AnimInfo.m_useWorking)
    {
        m_AnimMeshScene->m_AnimInfo.copyWorkingToCurrent();
    }
}

void AnimLightComponent::forceUpdateFlip()
{
    if ( m_AnimMeshScene )
    {
        AnimInfo* pAnimInfo = &m_AnimMeshScene->m_AnimInfo;

        if ( pAnimInfo )
        {
            bbool lookRight = isLookingRight();

            if (m_previousLookRight != lookRight )
            {
                if (pAnimInfo->m_prevPercent > 0.f)
                {
                    pAnimInfo->clearPrev();
                }
                m_previousLookRight = lookRight;
            }

            pAnimInfo->m_IsLookingRight = lookRight;
            pAnimInfo->SetFrame();
        }
    }
}

void AnimLightComponent::updateAnimTime( f32 _dt )
{
    u32 numAnims = m_currentFrameSubAnims.size();

    m_frameEvents.clear();

    for ( u32 i = 0; i < numAnims; i++ )
    {
        AnimLightFrameInfo& anim = m_currentFrameSubAnims[i];

        SubAnim* subAnim = anim.m_subAnimFrameInfo.getSubAnim();
        if ( subAnim && subAnim->getTrack() )
        {
            if (!m_forceDirectUpdate || m_semiForcedDirectUpdate)
                anim.m_subAnimFrameInfo.updateAnimTime(_dt,&m_frameEvents);
        }
    }

    commitSubAnimsToPlay();
}

void AnimLightComponent::updateEvents()
{
    if ( m_frameEvents.size() == 0 || !getNumPlayingSubAnims() )
        return;

    const SubAnim* currentSubAnim = getCurSubAnim(0);

    if ( !currentSubAnim )
    {
        return;
    }

    AnimSkeleton* skeleton = currentSubAnim->getSkeleton();
    u32 rootIndex = skeleton->getRootIndex();
    SafeArray<AnimBoneDyn> & boneDynList = m_AnimMeshScene->m_AnimInfo.getWorkingBoneList();

    if ( rootIndex >= boneDynList.size() )
    {
        return;
    }

    AnimBoneDyn *rootBone = &boneDynList[rootIndex];
    AnimBoneDyn *eventBone;
    u32 numEvents = m_frameEvents.size();

    for ( u32 i = 0; i < numEvents; i++ )
    {
        AnimMarkerEvent* trackEvent = m_frameEvents[i];
        eventBone = trackEvent->getBoneParent() != U32_INVALID ? &boneDynList[trackEvent->getBoneParent()] : rootBone;
        
        Vec2d pos = eventBone->m_Pos + AddAngleBTW(Vec2d(eventBone->m_Length*eventBone->m_Scale.m_x,0) + trackEvent->getPosLocal(), eventBone->m_Angle);

        trackEvent->setPos(pos);

        if ( trackEvent->GetObjectClassCRC() == ITF_GET_STRINGID_CRC(AnimPolylineEvent,4083589773) )
        {
            AnimPolylineEvent* polyEvent = static_cast<AnimPolylineEvent*>(trackEvent);
            AnimPolyline *  pPolyline   = getCurrentPolyline(polyEvent->getPolylineID(), btrue);
            if (pPolyline)
            {
                u32             numPoints   = pPolyline->m_PolylinePointList.size();
                const Vec2d*    points      = m_AnimMeshScene->m_AnimInfo.getPolylinePointBuffer(polyEvent->getPolylineID(), btrue);
                polyEvent->setPoints(points,numPoints);
            }
            else
            {
#ifndef ITF_FINAL
                if ( m_currentFrameSubAnims.size() )
                {
                    String warningString;
                    
                    for (u32 animIdx=0; animIdx<m_currentFrameSubAnims.size(); animIdx++ )
                    {
                        const AnimLightFrameInfo& info = m_currentFrameSubAnims[animIdx];
                        const SubAnim* subAnim = info.m_subAnimFrameInfo.getSubAnim();
                        if (subAnim)
                        {
                            if (animIdx != 0)
                                warningString += " or ";
                            warningString += "'";
                            warningString += subAnim->getAnimPath().getString();
                            warningString += "'";
                        }
                    }

                    if ( !warningString.isEmpty() )
                    {
#if defined(ITF_DEBUG_STRINGID)
                        ITF_WARNING_CATEGORY(Anim,m_actor, 0, "Actor '%s' has an invalid polyline event '%s' in animation %s: ", m_actor->getLua().getString8().cStr(),
                                                                                                                       trackEvent->getName().getDebugString(),
                                                                                                                       StringToUTF8(warningString).get());
#else
                        ITF_WARNING_CATEGORY(Anim,m_actor, 0, "Actor '%s' has an invalid polyline event in animation '%s': ", m_actor->getLua().getString8().cStr(),
                                                                                                                  StringToUTF8(warningString).get());
#endif
                    }
                }
                else
                {
                    ITF_WARNING_CATEGORY(Anim,m_actor, 0, "Actor '%s' has an invalid polyline event: ", m_actor->getLua().getString8().cStr());
                }
#endif // ITF_FINAL
                continue;
            }
        }

        trackEvent->setSender(m_actor->getRef());
        m_actor->onEvent(trackEvent);
    }
}

void AnimLightComponent::addLight(const LightInfo &_light)
{
    if (m_AnimMeshScene && !getDisableLight())
        m_AnimMeshScene->m_AnimInfo.m_lights.push_back(_light);
}

void AnimLightComponent::clearLights()
{
    if (m_AnimMeshScene)
        m_AnimMeshScene->m_AnimInfo.m_lights.clear();
}

AABB AnimLightComponent::getVisualAABB() const
{
    if ( !getTemplate() )
    {
        return GetActor()->getAABB();
    }
    else
    {        
        AABB AABBtemp(m_visualAABB);
        AABBtemp.Scale(getScale());
        if (m_actor->isFlipped())
            AABBtemp.FlipHorizontaly(bfalse);
        AABBtemp.Rotate(getOffsettedAngle(), bfalse);
        AABBtemp.Translate(getOffsettedPos().truncateTo2D());
        return AABBtemp;
    }
}


void AnimLightComponent::fillAnimsAABBLocal(AABB &animAABB) const
{
    ITF_VECTOR<AnimFrameInfo>::const_iterator frameInfoInter = m_animFrameInfo.begin();
    for (; frameInfoInter != m_animFrameInfo.end(); ++frameInfoInter)
    {
        AnimTrack * track = (*frameInfoInter).m_pTrackExt->getTrack();
        if (animAABB.isValid())
        {
            animAABB.grow(track->m_bounding);
        } else
        {
            animAABB = track->m_bounding;
        }
    }
}

AABB AnimLightComponent::getAnimsAABB() const
{
    AABB animAABB;
    animAABB.invalidate();

    fillAnimsAABBLocal(animAABB);
    if (!animAABB.isValid())
    {
        return getVisualAABB();
    }

    animAABB.Scale(getScale());
    if (m_actor->isFlipped())
        animAABB.FlipHorizontaly(bfalse);
    animAABB.Rotate(getOffsettedAngle(), bfalse);
    animAABB.Translate(getOffsettedPos().truncateTo2D());
    
    return animAABB;
}

void AnimLightComponent::updateAABB()
{
    ITF_ASSERT(isLoaded());
    Actor * actor = GetActor();

#ifndef ANIMATION_FIX_ABB

    if (!m_AnimMeshScene->m_AnimInfo.m_patchAABB.isValid())
    {
        actor->growRelativeAABBFromAbsolute(getVisualAABB());
        return;
    }

    if (needsDraw2D())
    {
        // move m_patch to 2d screen using mesh info
        // this is done here because before that we don't have mesh info
        const GMatrix44* matrix = &m_AnimMeshScene->m_pMesh[0]->getMatrix();
        // matrix is only used for translation and scale (no rotation)
        m_AnimMeshScene->m_AnimInfo.m_patchAABB.Scale(Vec2d(matrix->fa16[0], matrix->fa16[5]));
        m_AnimMeshScene->m_AnimInfo.m_patchAABB.Translate(Vec2d(matrix->fa16[12], matrix->fa16[13]));
    }
    
    actor->growRelativeAABBFromAbsolute(m_AnimMeshScene->m_AnimInfo.m_patchAABB);

    m_drawPredictionAABB.grow(
        AABB(m_AnimMeshScene->m_AnimInfo.m_patchAABB.getMin() - actor->get2DPos(),
             m_AnimMeshScene->m_AnimInfo.m_patchAABB.getMax() - actor->get2DPos()));
#else
    actor->growRelativeAABBFromAbsolute(getAnimsAABB());
#endif
}

void AnimLightComponent::computeVisiblePrediction()
{
    // QUICK FIX FOR POPPING
    if (m_alpha <= MTH_EPSILON )
    {
        m_isVisiblePrediction =  bfalse;
        return;
    }

    if (needsDraw2D()/* || getDisableVisiblePrediction()*/ )
    {
        m_isVisiblePrediction = btrue;
        return;
    }

    const Actor * actor = GetActor();
#ifndef ANIMATION_FIX_ABB
    AABB aabbVisible = getVisualAABB();

    aabbVisible.grow(
        AABB(m_drawPredictionAABB.getMin() + actor->get2DPos(),
             m_drawPredictionAABB.getMax() + actor->get2DPos()));

    aabbVisible.ScaleFromCenter(Vec2d(1.1f, 1.1f));

    m_isVisiblePrediction = CAMERA->isRectVisible(aabbVisible, actor->getPos().m_z);
#else
    m_isVisiblePrediction = CAMERA->isRectVisible(getAnimsAABB(), actor->getPos().m_z);
#endif

}



Vec2d AnimLightComponent::getOffset() const
{

    if (getUseBase())
    {
        Vec2d offset = getTemplate()->getPosOffset()+m_instancePositionOffset;
        offset.m_y -= m_actor->getRadius();
        return offset.Rotate( getOffsettedAngle() );
    }
    else
    {
        Vec2d offset = getTemplate()->getPosOffset()+m_instancePositionOffset;
        return offset.Rotate( getOffsettedAngle() );
    }
}

//////////////////////////////////////////////////////////////////////////
AnimBoneDyn* AnimLightComponent::getBone (u32 _boneIndex) const
    //////////////////////////////////////////////////////////////////////////
{
    if (!isLoaded())
        return NULL;

    SafeArray<AnimBoneDyn> & boneDynList = m_AnimMeshScene->m_AnimInfo.getCurrentBoneList();
    if (!boneDynList.size())
        return NULL;

    if (!isBoneDataOk(_boneIndex))
    {
        _boneIndex = m_rootIndex;
    }

    ITF_ASSERT_CRASH( _boneIndex < boneDynList.size(), "Bone index bigger than boneList size !");
    return &boneDynList[_boneIndex];
}
//////////////////////////////////////////////////////////////////////////
bbool AnimLightComponent::getBonePos (u32 _boneIndex, Vec2d& _dest, bbool _getBoneEnd) const
    //////////////////////////////////////////////////////////////////////////
{
    if (!isLoaded())
        return bfalse;

    SafeArray<AnimBoneDyn> & boneDynList = m_AnimMeshScene->m_AnimInfo.getCurrentBoneList();
    if (!boneDynList.size())
        return bfalse;

    if (!isBoneDataOk(_boneIndex))
    {
        _boneIndex = m_rootIndex;
    }

    ITF_ASSERT_CRASH( _boneIndex < boneDynList.size(), "Bone index bigger than boneList size !");

    _dest = _getBoneEnd ? boneDynList[_boneIndex].m_PosEnd : boneDynList[_boneIndex].m_Pos;
    return btrue;
}
//////////////////////////////////////////////////////////////////////////
bbool AnimLightComponent::getBonePos (u32 _boneIndex, Vec3d& _dest, bbool _getBoneEnd) const
    //////////////////////////////////////////////////////////////////////////
{
    Vec2d res;
    if (!getBonePos(_boneIndex, res, _getBoneEnd) )
    {
        return bfalse;
    }
    
    // get bone infos
    SafeArray<AnimBoneDyn> & boneDynList = m_AnimMeshScene->m_AnimInfo.getCurrentBoneList();
    if (!boneDynList.size())
    {
        return bfalse;
    }

    if (!isBoneDataOk(_boneIndex))
    {
        _boneIndex = m_rootIndex;
    }

    ITF_ASSERT_CRASH(_boneIndex < boneDynList.size(), "Bone index bigger than boneList size !");
    
    // use actor's depth and apply bone Z offset
    f32 depth = (boneDynList[_boneIndex].m_Zorder >= 0.0f)
        ? m_actor->getDepth() + getTemplate()->getFrontZOffset()
        : m_actor->getDepth() + getTemplate()->getBackZOffset();
    _dest.set(res.m_x, res.m_y, depth);
    
    return btrue;
}
//////////////////////////////////////////////////////////////////////////
bbool AnimLightComponent::getBoneOrientation (u32 _boneIndex, Vec2d& _orientation) const
    //////////////////////////////////////////////////////////////////////////
{
    if (!isLoaded())
        return bfalse;

    SafeArray<AnimBoneDyn> & boneDynList = m_AnimMeshScene->m_AnimInfo.getCurrentBoneList();
    if (!boneDynList.size())
        return bfalse;

    if (!isBoneDataOk(_boneIndex))
    {
        _boneIndex = m_rootIndex;
    }

    ITF_ASSERT_CRASH( _boneIndex < boneDynList.size(), "Bone index bigger than boneList size !");

    if( boneDynList[_boneIndex].m_PosEnd != boneDynList[_boneIndex].m_Pos )
    {
        _orientation = (boneDynList[_boneIndex].m_PosEnd - boneDynList[_boneIndex].m_Pos).normalize();
    }
    else
    {
        _orientation = Vec2d::Zero;
    }
    return btrue;
}
//////////////////////////////////////////////////////////////////////////
bbool AnimLightComponent::getBoneScale(u32 _boneIndex, Vec2d& _dest) const
    //////////////////////////////////////////////////////////////////////////
{
    if (!isLoaded())
        return bfalse;

    SafeArray<AnimBoneDyn> & boneDynList = m_AnimMeshScene->m_AnimInfo.getCurrentBoneList();
    if (!boneDynList.size())
        return bfalse;
    
    if (!isBoneDataOk(_boneIndex))
    {
        _boneIndex = m_rootIndex;
    }

    ITF_ASSERT_CRASH( _boneIndex < boneDynList.size(), "Bone index bigger than boneList size !");

    _dest = boneDynList[_boneIndex].m_Scale;
    return btrue;
}

//////////////////////////////////////////////////////////////////////////
i32  AnimLightComponent::getBoneIndex (const StringID& _boneName)
    //////////////////////////////////////////////////////////////////////////
{
    if (!isLoaded() || !getNumPlayingSubAnims() )
        return U32_INVALID;
    AnimSkeleton *skeleton = getCurSubAnim(0)->getSkeleton();
    return skeleton->getBoneIndex(_boneName);
}


u32  AnimLightComponent::getBoneCount() const
{
    if (!isLoaded() || !getNumPlayingSubAnims() )
        return U32_INVALID;

    AnimSkeleton *skeleton = getCurSubAnim(0)->getSkeleton();
    return skeleton->m_BonesCurFrame.size();
}

AnimPolyline * AnimLightComponent::getCurrentPolyline(const StringID & _polyId, bbool _useWorking) const
{
     ITF_ASSERT_MSG(isLoaded() && getNumPlayingSubAnims(), "Trying to get polyline on invalid animation");
    if (!isLoaded() || !getNumPlayingSubAnims() )
        return NULL;

    AnimSkeleton                    *skeleton       = _useWorking ? m_AnimMeshScene->m_AnimInfo.getWorkingSkeleton() : m_AnimMeshScene->m_AnimInfo.getCurrentSkeleton();
    ITF_VECTOR<StringID>            &polyRefList    = _useWorking ? m_AnimMeshScene->m_AnimInfo.getWorkingPolyList() : m_AnimMeshScene->m_AnimInfo.getCurrentPolyList();

    for ( ITF_VECTOR<StringID>::const_iterator iter = polyRefList.begin();
        iter != polyRefList.end(); iter++ )
    {
        const StringID& polyRef = *iter;
        if (_polyId != polyRef)
            continue;

        AnimPolyline *pPolyline;
        if ( skeleton->getPolyline(polyRef, pPolyline) )
        {
            return pPolyline;
        }
    }
    return NULL;
}


Vec2d * AnimLightComponent::getCurrentPolylinePointBuffer(const StringID & _polyId, u32 * _size, AnimPolyline::PolylineType * _type) const
{
    AnimPolyline* pPolyline = getCurrentPolyline(_polyId);
    if (!pPolyline)
    {
        if (_size)
            *_size = 0;
        if (_type)
            *_type = AnimPolyline::shape_error;
        return NULL;
    }

    if (_size)
        *_size = pPolyline->m_PolylinePointList.size();
    if (_type)
        *_type = pPolyline->m_type;
    return m_AnimMeshScene->m_AnimInfo.getPolylinePointBuffer(_polyId, btrue);
}

Vec2d * AnimLightComponent::getCurrentPolylinePoint(const StringID & _polyId, const StringID & _polyPointName, u32 * _index) const
{
    AnimPolyline* pPolyline = getCurrentPolyline(_polyId);
    if (!pPolyline)
        return NULL;

    u32 numPoints = pPolyline->m_PolylinePointList.size();
    u32 pointIndex = pPolyline->m_NameToPolylinePoint.find(_polyPointName);
    Vec2d* points = m_AnimMeshScene->m_AnimInfo.getPolylinePointBuffer(_polyPointName, btrue);

    if (!points || numPoints == 0 || pointIndex >= numPoints)
        return NULL;

    if (_index)
        *_index = pointIndex;
    return &points[pointIndex];
}

Vec2d * AnimLightComponent::getCurrentFirstPolylinePoint(const StringID & _polyPointName, AnimPolyline ** _polyline, u32 * _index) const
{
    ITF_ASSERT_MSG(isLoaded() && getNumPlayingSubAnims(), "Trying to get polyline on invalid animation");
    if (!isLoaded() || !getNumPlayingSubAnims() )
        return NULL;

    AnimSkeleton                    *skeleton   = m_AnimMeshScene->m_AnimInfo.getCurrentSkeleton();
    ITF_VECTOR<StringID>&           polyRefList = m_AnimMeshScene->m_AnimInfo.getCurrentPolyList();

    for ( ITF_VECTOR<StringID>::const_iterator iter = polyRefList.begin();
        iter != polyRefList.end(); iter++ )
    {
        const StringID& polyRef = *iter;
        AnimPolyline *pPolyline;
        if ( skeleton->getPolyline(polyRef, pPolyline) )
        {
            for ( NamesMap::Iterator it(pPolyline->m_NameToPolylinePoint); !it.isEnd(); it.increment() )
            {
                if ( it.first() == _polyPointName )
                {
                    if (_polyline)
                        *_polyline = pPolyline;
                    if (_index)
                        *_index = it.second();

                    Vec2d* points = m_AnimMeshScene->m_AnimInfo.getPolylinePointBuffer(polyRef, btrue);
                    return &points[it.second()];
                }
            }
        }
    }
    return NULL;
}



void AnimLightComponent::Draw()
{
    STATS_ANIMLIGHTCOMPONENT_DRAWINC

    if (isDrawEnabled())
    {
        if ( m_alpha > MTH_EPSILON && isLoaded() && getNumPlayingSubAnims() && isVisiblePrediction() )
        {
/*
            const Layer* layer = LAYER_MANAGER->findLayer(GetActor()->getLayerID());
            ITF_ASSERT(layer);
            m_AnimMeshScene->m_AnimInfo.m_lights.push_back(layer->getAmbientLight());
*/
            
			if ( useShadow() )
				drawShadow();

            if ( getTemplate()->getFrontZOffset() != getTemplate()->getBackZOffset() && m_AnimMeshScene->m_AnimInfo.m_renderPassFrontierPatchIndex)
            {
                m_AnimMeshScene->m_AnimInfo.m_renderPass = 0;
                GFX_ADAPTER->addPrimitiveInZList((void*)m_AnimMeshScene, GFX_ITF_ANIMSCENE, NULL, m_actor->getDepth() + getTemplate()->getBackZOffset(), m_renderInTarget,m_actor->getRef());
            }
            GFX_ADAPTER->addPrimitiveInZList((void*)m_AnimMeshScene, GFX_ITF_ANIMSCENE, NULL, m_actor->getDepth() + getTemplate()->getFrontZOffset(), m_renderInTarget,m_actor->getRef());
        }
    }
}
void AnimLightComponent::Draw2D()
{
    if (isDrawEnabled())
    {
        if (m_alpha > MTH_EPSILON && isLoaded() && getNumPlayingSubAnims() && isVisiblePrediction() )
        {
            /*
            const Layer* layer = LAYER_MANAGER->findLayer(GetActor()->getLayerID());
            m_AnimMeshScene->m_AnimInfo.m_lights.push_back(layer->getAmbientLight());
            */
            if (!getTemplate()->getDraw2dSubScreen()) // CASA::CTR
            {
                GFX_ADAPTER->addPrimitive2d(m_actor->get2DPos(), getScale(),m_rank2D, GFX_ITF_ANIMSCENE,m_AnimMeshScene);
            } 
            else
            {
                // $GB there was a bug here, bfalse was given to the 8th parameter while it was expecting a pointer (probably NULL).
                // So the 9th parameter, _UpScreen, was set to btrue (the default). Maybe the intent was to set to bfalse?
                // For compatibility I set it to btrue, but might need to reconsider.
                ITF_ASSERT_CRASH(0, "Check that this is the correct behavior");
                GFX_ADAPTER->addPrimitive2d(m_actor->get2DPos(), getScale(),m_rank2D, GFX_ITF_ANIMSCENE, m_AnimMeshScene, NULL, NULL, NULL, btrue);
            }
        }
    }
}

void AnimLightComponent::onUnloadResources()
{
    ANIM_MANAGER->registerAnimForResourceUnload(m_actor->getRef());
}

bbool AnimLightComponent::isLoaded() const
{
    return m_actor && m_AnimMeshScene != NULL && m_actor->isPhysicalReady();
}

void AnimLightComponent::onEvent( Event* _event )
{
    Super::onEvent(_event);

    if ( EventReset* eventReset = _event->DynamicCast<EventReset>(ITF_GET_STRINGID_CRC(EventReset,2551104629)) )
    {
        processEventReset(eventReset);
    }
    else if ( EventShow* eventShow = _event->DynamicCast<EventShow>(ITF_GET_STRINGID_CRC(EventShow,111532112)) )
    {
        processEventShow(eventShow);
    }
    else if ( EventActivate* eventActivate = _event->DynamicCast<EventActivate>(ITF_GET_STRINGID_CRC(EventActivate,2639596100)) )
    {
        processEventActivate(eventActivate);
    }
}

void  AnimLightComponent::processEventReset( EventReset* _resetEvent )
{
    resetColor();
    m_colorSrc = m_colorDst = m_currentColor;
}

void  AnimLightComponent::processEventShow( EventShow* _eventShow )
{
    if ( _eventShow->getIsOverrideColor() )
    {
        m_colorSrc = m_currentColor;
        m_colorDst = _eventShow->getOverrideColor();
    }
}

void AnimLightComponent::processEventActivate( EventActivate* _eventActivate )
{
    if (!_eventActivate->getActivated())
    {
        return;
    }

    const u32 subAnimCount = m_subAnimSet.getSubAnimCount();
    for ( u32 i = 0; i < subAnimCount; i++ )
    {
        SubAnim* subAnim = m_subAnimSet.getSubAnim(i);
        if (subAnim->allowSyncOffset())
        {
            f32 syncOffset = METRONOME_MANAGER->getBarPercentage(0.f, subAnim->getSyncRatio(),
                btrue, subAnim->getMetronome());
            subAnim->setSyncOffset(m_syncOffset + syncOffset);
        }
    }
}

void AnimLightComponent::setAnim(const StringID & _subAnim, u32 _blendFrames /*= U32_INVALID*/)
{
    m_playAnimName = _subAnim;
    m_playAnimNumFrames = _blendFrames;
}

void AnimLightComponent::commitPlayAnim()
{
    setCurrentBlendFrames(m_playAnimNumFrames);

    u32 numAnims = m_currentFrameSubAnims.size();

    for ( u32 i = 0; i < numAnims; i++ )
    {
        AnimLightFrameInfo& anim = m_currentFrameSubAnims[i];
        SubAnim* pSubAnim = anim.m_subAnimFrameInfo.getSubAnim();
        ITF_ASSERT(pSubAnim);
        if(pSubAnim)
        {
            EventAnimChanged onAnimChanged(pSubAnim->getFriendlyName());
            m_actor->onEvent(&onAnimChanged);
        }
    }

    resetSubAnimsToPlay();

    i32 newsubAnimIndex = m_subAnimSet.getSubAnimIndex(m_playAnimName);

    if ( newsubAnimIndex != -1 )
    {
        SubAnimFrameInfo frameInfo(&m_subAnimSet);
        frameInfo.setSubAnimIndex(newsubAnimIndex);
        frameInfo.resetCurTime();
        addSubAnimToPlay(frameInfo,1.f,btrue);
    }

    ITF_WARNING(m_actor, getNumPlayingSubAnims() > 0, "Trying to play an unexisting anim %s", m_playAnimName.getDebugString());

    m_playAnimName.invalidate();
} 


void AnimLightComponent::resetSubAnimsToPlay()
{
    m_currentFrameSubAnims.clear();

    resetSubAnimFrameInfoFlags();
}

i32 AnimLightComponent::addSubAnimToPlay(const StringID & _subAnim, f32 _weight, bbool _usePatches )
{
    i32 curAnimIndex = m_subAnimSet.getSubAnimIndex(_subAnim);

    ITF_WARNING(m_actor, curAnimIndex != -1, "trying to set invalid animation %s", _subAnim.getDebugString());
    if ( curAnimIndex != -1 )
    {
        SubAnimFrameInfo frameInfo(&m_subAnimSet);
        frameInfo.setSubAnimIndex(curAnimIndex);
        frameInfo.resetCurTime();
        return addSubAnimToPlay(frameInfo,_weight,_usePatches);
    }
    return -1;
}

void AnimLightComponent::setFrameForSubAnim(u32 _subAnimIndex, f32 _time)
{
    ITF_ASSERT(_subAnimIndex<m_currentFrameSubAnims.size());

    SubAnimFrameInfo &frameInfo = m_currentFrameSubAnims[_subAnimIndex].m_subAnimFrameInfo;
    frameInfo.setCurTime(frameInfo.getSubAnim()->getStart() + _time);
}

void AnimLightComponent::changeSubAnimToPlay(u32 _subAnimIndex, const StringID & _subAnim, f32 _weight, bbool _usePatches)
{
    i32 curAnimIndex = m_subAnimSet.getSubAnimIndex(_subAnim);

    if ( curAnimIndex != -1 )
    {
        resetSubAnimFrameInfoFlags();
        ITF_ASSERT(_subAnimIndex<m_currentFrameSubAnims.size());

        SubAnimFrameInfo frameInfo(&m_subAnimSet);
        frameInfo.setSubAnimIndex(curAnimIndex);
        frameInfo.resetCurTime();
        m_currentFrameSubAnims[_subAnimIndex] = AnimLightFrameInfo(frameInfo,_weight,_usePatches);

        for (u32 i=0; i<m_currentFrameSubAnims.size(); i++)
        {
            const SubAnimFrameInfo& subframeinfo = m_currentFrameSubAnims[i].m_subAnimFrameInfo;
            updateSubAnimFrameInfoFlag(subframeinfo);
        }
    }
}

void AnimLightComponent::resetSubAnimFrameInfoFlags()
{
    m_useRootRotation = bfalse;
    m_animDefaultFlip = bfalse;
    m_isLooped = bfalse;

	if (useShadow())
		setShadowMul(1.f);
}

void AnimLightComponent::updateSubAnimFrameInfoFlag(const SubAnimFrameInfo& _subAnimFrame)
{
    m_useRootRotation |= _subAnimFrame.getSubAnim()->getUseRootRotation();
    m_animDefaultFlip |= _subAnimFrame.getSubAnim()->getDefaultFlip();
    m_isLooped |= _subAnimFrame.getIsLooped();
	
	if (useShadow())
		setShadowMul(_subAnimFrame.getSubAnim()->getShadowMul());
}

i32 AnimLightComponent::addSubAnimToPlay( const SubAnimFrameInfo& _subAnimFrame, f32 _weight, bbool _usePatches )
{
    i32 index = (i32)m_currentFrameSubAnims.size();
    updateSubAnimFrameInfoFlag(_subAnimFrame);

    ITF_ASSERT_MSG(_subAnimFrame.getSubAnim(), "adding null sub anim !");
    m_currentFrameSubAnims.push_back(AnimLightFrameInfo(_subAnimFrame,_weight,_usePatches));
    return index;
}

void AnimLightComponent::commitSubAnimsToPlay()
{

    u32 numAnims = m_currentFrameSubAnims.size();

    if (m_animFrameInfo.size() != numAnims)
        m_animFrameInfo.resize(numAnims);

    for ( u32 i = 0; i < numAnims; i++ )
    {
        AnimLightFrameInfo& anim = m_currentFrameSubAnims[i];
        AnimFrameInfo& frameInfo = m_animFrameInfo[i];

        if ( getTemplate()->getSmoothAnim() ) 
            frameInfo.m_fFrame = anim.m_subAnimFrameInfo.getCurrentTime() / ANIMATION_FRAMERATE;
        else
            frameInfo.m_fFrame = timeToFrame(anim.m_subAnimFrameInfo.getCurrentTime(),ANIMATION_FRAMERATE);
        frameInfo.m_pTrackExt = anim.m_subAnimFrameInfo.getSubAnim()->getTrackExt();
        frameInfo.m_percent = anim.m_weight;
        frameInfo.m_useVisuals = anim.m_usePatches;
    }
}

void AnimLightComponent::setPlayRate( f32 _rate )
{
    u32 numAnims = m_currentFrameSubAnims.size();

    for ( u32 i = 0; i < numAnims; i++ )
    {
        AnimLightFrameInfo& anim = m_currentFrameSubAnims[i];
        anim.m_subAnimFrameInfo.setPlayRate(_rate);
    }
}

void AnimLightComponent::setCurFrame(f32 _frame, AnimEventContainer* _eventList)
{
    u32 numAnims = m_currentFrameSubAnims.size();

    for ( u32 i = 0; i < numAnims; i++ )
    {
        AnimLightFrameInfo& anim = m_currentFrameSubAnims[i];
        anim.m_subAnimFrameInfo.setCurTime(_frame * ANIMATION_FRAMERATE,_eventList);
    }
}

void AnimLightComponent::setCurFrameFromStart(f32 _frame, AnimEventContainer* _eventList)
{
    u32 numAnims = m_currentFrameSubAnims.size();

    for ( u32 i = 0; i < numAnims; i++ )
    {
        AnimLightFrameInfo& anim = m_currentFrameSubAnims[i];
        anim.m_subAnimFrameInfo.setCurTime((anim.m_subAnimFrameInfo.getSubAnim()->getStart()+_frame) * ANIMATION_FRAMERATE,_eventList);
    }
}

void AnimLightComponent::resetCurTime()
{
    u32 numAnims = m_currentFrameSubAnims.size();

    for ( u32 i = 0; i < numAnims; i++ )
    {
        AnimLightFrameInfo& anim = m_currentFrameSubAnims[i];
        anim.m_subAnimFrameInfo.resetCurTime();
    }
}

void AnimLightComponent::setProceduralCursor(f32 _cursor)
{
    u32 numAnims = m_currentFrameSubAnims.size();

    for ( u32 i = 0; i < numAnims; i++ )
    {
        AnimLightFrameInfo& anim = m_currentFrameSubAnims[i];
        ITF_ASSERT(anim.m_subAnimFrameInfo.getIsProcedural());
        anim.m_subAnimFrameInfo.setCursor(_cursor);
    }
}

bbool AnimLightComponent::isSubAnimFinished() const
{
    u32 numAnims = m_currentFrameSubAnims.size();

    for ( u32 i = 0; i < numAnims; i++ )
    {
        const AnimLightFrameInfo& anim = m_currentFrameSubAnims[i];

#if !defined(ITF_DISABLE_WARNING)
        SubAnim* subAnim = anim.m_subAnimFrameInfo.getSubAnim();
        ITF_WARNING_CATEGORY(GPP,m_actor, !subAnim->getIsLooped(), "Testing isSubAnimFinished on looping animation: %s",
            subAnim->getFriendlyName().getDebugString());
#endif // ITF_DISABLE_WARNING

        if ( anim.m_subAnimFrameInfo.isSubAnimFinished() )
        {
            return btrue;
        }
    }

    return bfalse;
}

bbool AnimLightComponent::isAnimLooped() const
{
    u32 numAnims = m_currentFrameSubAnims.size();

    for ( u32 i = 0; i < numAnims; i++ )
    {
        const AnimLightFrameInfo& anim = m_currentFrameSubAnims[i];

        if ( anim.m_subAnimFrameInfo.getIsLooped() )
        {
            return btrue;
        }
    }

    return bfalse;
}

void AnimLightComponent::setAnimLoop(bbool _loop)
{
    u32 numAnims = m_currentFrameSubAnims.size();

    for ( u32 i = 0; i < numAnims; i++ )
    {
        AnimLightFrameInfo& anim = m_currentFrameSubAnims[i];
        anim.m_subAnimFrameInfo.setIsLooped(_loop);
    }
}

void AnimLightComponent::dbgGetCurAnims( u32 _index, String8 & _text)
{
#if !defined(ITF_FINAL) && defined(ITF_DEBUG_STRINGID)

    if ( _index < m_currentFrameSubAnims.size() )
    {
        AnimLightFrameInfo& anim = m_currentFrameSubAnims[_index];

        _text.setTextFormat("%s (weight: %.f%%, time %.2f, frame: %d)",
            anim.m_subAnimFrameInfo.getSubAnim()->getFriendlyName().getDebugString(),
            anim.m_weight * 100,
            anim.m_subAnimFrameInfo.getCurrentTime() - anim.m_subAnimFrameInfo.getSubAnim()->getStart(),
            static_cast<u32>(timeToFrame(anim.m_subAnimFrameInfo.getCurrentTime(),ANIMATION_FRAMERATE)) );
    }
#endif
}

void AnimLightComponent::setBankState(u32 _state)
{
    if (m_AnimMeshScene)
        m_AnimMeshScene->m_AnimInfo.m_state = _state;
}

Vec2d AnimLightComponent::getRootDelta()
{
    if (m_AnimMeshScene)
    {
        ITF_VECTOR<AnimFrameInfo> &frameInfo = m_AnimMeshScene->m_AnimInfo.m_frameInfo;
        ITF_ASSERT_MSG(frameInfo.size() == 1, "Delta root has only a meaning with one frameInfo");
        if (frameInfo.size() > 0)
        {
            AnimFrameInfo &firstFrameInfo = frameInfo[0];
            AnimTrack * track = firstFrameInfo.m_pTrackExt->getTrack();
            ITF_ASSERT(track);
            ITF_WARNING_CATEGORY(Anim,m_actor, track->m_rootPos.size(), "get delta on animation with Root Pos not set, please update data");
            if (track->m_rootPos.size() == 0)
                return Vec2d::Zero;
            u32 frame       = (u32)(firstFrameInfo.m_fFrame + 0.5f);
            const SubAnim* subAnim = getCurSubAnim(0);
            u32 nextFrame   = frame+1;
            if (subAnim->getIsReversed())
                nextFrame   = frame-1;
            u32 startFrame  = (u32)timeToFrame(subAnim->getStart(), ANIMATION_FRAMERATE);
            ITF_UNUSED(startFrame);
            u32 stopFrame   = (u32)timeToFrame(subAnim->getStop(), ANIMATION_FRAMERATE);

            if (frame >= stopFrame)
                return Vec2d::Zero;

            ITF_ASSERT( startFrame <= frame && frame < stopFrame);
            frame = Min(frame, (u32)track->m_rootPos.size() - 1);
            if (nextFrame >= stopFrame)
                nextFrame = frame;

            Vec2d           rootDelta       = track->m_rootPos[nextFrame] - track->m_rootPos[frame];
            bbool           isDefaultRight  = (track->m_tags & ANIM_TRACK_IS_DEFAULT_RIGHT) != 0;
            bbool           inverted        = isDefaultRight != isLookingRight();

            rootDelta      *= getScale();
            rootDelta.Rotate(inverted ? -m_actor->getAngle() : m_actor->getAngle());
            if (inverted)
                rootDelta.m_x  *= -1.0f;

            return rootDelta;
        }
    }

    ITF_ASSERT_MSG(0, "trying to get root delta without loading animation");
    return Vec2d::Zero;
}

// get the root delta compared to frame 0
Vec2d AnimLightComponent::getRootDeltaCumulated( bbool _useFloatingFrame )
{
    if (m_AnimMeshScene)
    {
        ITF_VECTOR<AnimFrameInfo> &frameInfo = m_AnimMeshScene->m_AnimInfo.m_frameInfo;
        ITF_ASSERT_MSG(frameInfo.size() == 1, "Delta root has only a meaning with one frameInfo");
        if (frameInfo.size() > 0)
        {
            AnimFrameInfo &firstFrameInfo = frameInfo[0];
            AnimTrack * track = firstFrameInfo.m_pTrackExt->getTrack();
            ITF_ASSERT(track);
            ITF_WARNING_CATEGORY(Anim,m_actor, track->m_rootPos.size(), "get delta on animation with Root Pos not set, please update data");
            if (track->m_rootPos.size() == 0)
                return Vec2d::Zero;

            Vec2d rootDelta;
            if (!_useFloatingFrame)
            {
                u32 frame = (u32)(firstFrameInfo.m_fFrame + 0.5f);
                const SubAnim* subAnim = getCurSubAnim(0);
                u32 startFrame = (u32)timeToFrame(subAnim->getStart(), ANIMATION_FRAMERATE);
                u32 stopFrame  = (u32)timeToFrame(subAnim->getStop(), ANIMATION_FRAMERATE);

                ITF_ASSERT( startFrame <= frame && frame <= stopFrame);
                frame = Clamp(frame, startFrame, stopFrame - 1);

                rootDelta = track->m_rootPos[frame] - track->m_rootPos[startFrame];
            }
            else
            {
                f32 frameBegin = floorf(firstFrameInfo.m_fFrame + 0.5f);
                f32 frameEnd = ceilf(firstFrameInfo.m_fFrame + 0.5f);
                f32 ratio = firstFrameInfo.m_fFrame - frameBegin;

                const SubAnim* subAnim = getCurSubAnim(0);
                f32 startFrame = timeToFrame(subAnim->getStart(), ANIMATION_FRAMERATE);
                f32 stopFrame  = timeToFrame(subAnim->getStop(), ANIMATION_FRAMERATE);

                ITF_ASSERT( startFrame <= frameBegin && frameEnd <= stopFrame );
                frameBegin = Clamp(frameBegin, startFrame, stopFrame - 1);
                frameEnd = Clamp(frameEnd, startFrame, stopFrame - 1);

                rootDelta = track->m_rootPos[(u32)frameBegin]*(1-ratio) + track->m_rootPos[(u32)frameEnd]*ratio - track->m_rootPos[(u32)startFrame];
            }

            bbool           isDefaultRight  = (track->m_tags & ANIM_TRACK_IS_DEFAULT_RIGHT) != 0;
            bbool           inverted        = isDefaultRight != isLookingRight();

            rootDelta      *= getScale();
            rootDelta.Rotate(inverted ? -m_actor->getAngle() : m_actor->getAngle());
            if (inverted)
                rootDelta.m_x  *= -1.0f;

            return rootDelta;
        }
    }

    ITF_ASSERT_MSG(0, "trying to get root delta without loading animation");
    return Vec2d::Zero;
}

void AnimLightComponent::setCurrentBlendFrames( u32 _blendFrames )
{
    if (!isLoaded())
    {
        return;
    }

    if ( _blendFrames != U32_INVALID )
    {
        if ( _blendFrames )
        {
            m_currentBlendFrames = static_cast<f32>(_blendFrames);
            m_AnimMeshScene->m_AnimInfo.setPrev(1.0f);
        }
        else
        {
            if ( m_currentBlendFrames )
            {
                m_AnimMeshScene->m_AnimInfo.clearPrev();
                m_currentBlendFrames = 0;
            }
        }
    }
    else
    {
        setCurrentBlendFrames(getTemplate()->getDefaultBlendFrames());
    }
}

f32 AnimLightComponent::getCurTime( u32 _index /*= 0*/ ) const
{
    ITF_ASSERT(_index < m_currentFrameSubAnims.size());

    const SubAnimFrameInfo& frameInfo = m_currentFrameSubAnims[_index].m_subAnimFrameInfo;
    SubAnim* subAnim = frameInfo.getSubAnim();

    if (frameInfo.getIsReversed())
    {
        return subAnim->getStop() - frameInfo.getCurrentTime();
    }
    else
    {
        return subAnim->getStart() + frameInfo.getCurrentTime();
    }
}

void AnimLightComponent::setCurTime( f32 _time, u32 _index /*= 0*/ )
{
    ITF_ASSERT(_index < m_currentFrameSubAnims.size());

    SubAnimFrameInfo& frameInfo = m_currentFrameSubAnims[_index].m_subAnimFrameInfo;
    SubAnim* subAnim = frameInfo.getSubAnim();

    if (frameInfo.getIsReversed())
    {
        frameInfo.setCurTime(subAnim->getStop() - _time);
    }
    else
    {
        frameInfo.setCurTime(subAnim->getStart() + _time);
    }
}

bbool AnimLightComponent::canGetRootDelta() const
{
    if (m_AnimMeshScene)
    {
        ITF_VECTOR<AnimFrameInfo> &frameInfo = m_AnimMeshScene->m_AnimInfo.m_frameInfo;
        ITF_ASSERT_MSG(frameInfo.size() == 1, "Delta root has only a meaning with one frameInfo");
        if (frameInfo.size() > 0)
        {
            AnimFrameInfo &firstFrameInfo = frameInfo[0];
            AnimTrack * track = firstFrameInfo.m_pTrackExt->getTrack();
            ITF_ASSERT(track);
            return track->m_rootPos.size() > 0;
        }
    }
    return bfalse;
}

ITF::AABB AnimLightComponent::getPatchAABB() const
{
    AABB aabb;
    fillAnimsAABBLocal(aabb);
    return aabb;
}

bbool AnimLightComponent::isBoneDataOk( i32 _boneIndex ) const
{
    if (isVisiblePrediction() || getDisableVisiblePrediction() || !getNumPlayingSubAnims())
        return btrue;

    AnimSkeleton *skeleton = getCurSubAnim(0)->getSkeleton();
    return skeleton->m_BonesCurFrame[_boneIndex].m_PatchFlags & BONE_MINIMAL_COMPUTE;
}


void AnimLightComponent::updatePatchAndAABB() 
{
    if ( isVisiblePrediction() )
    {
        updatePatches();
    }
#ifndef ANIMATION_FIX_ABB
    updateAABB();
#endif
}


#ifdef ITF_SUPPORT_EDITOR
void AnimLightComponent::onEditorMove(bbool _modifyInitialPos)
{
    if (!GetActor()->isUpdateAllowed() && GetActor()->getLastUpdatedFrame() < CURRENTFRAME)
        forceSynchronousFrame();
}
#endif // ITF_SUPPORT_EDITOR


///////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_OBJECT_RTTI(AnimLightComponent_Template)
BEGIN_SERIALIZATION_CHILD(AnimLightComponent_Template)

    SERIALIZE_MEMBER("animationPath", m_animPath); 
    SERIALIZE_MEMBER("useBase",m_useBase); 
    SERIALIZE_MEMBER("smoothAnim",m_smoothAnim);
    SERIALIZE_MEMBER("canonizeTransitions",m_canonizeTransitions); 
    SERIALIZE_MEMBER("defaultBlendFrames",m_defaultBlendFrames); 
    SERIALIZE_MEMBER("draw2D",m_draw2D); 
    SERIALIZE_MEMBER("visualAABBanimID",m_visualAABBanimID); 
    BEGIN_CONDITION_BLOCK(ESerializeGroup_Data)
        SERIALIZE_OBJECT("animSet",m_subAnimSet);
    END_CONDITION_BLOCK()
    SERIALIZE_MEMBER("defaultAnimation",m_defaultAnimation);
    SERIALIZE_MEMBER("defaultColor",m_defaultColor); 
    SERIALIZE_MEMBER("flip",m_startFlip); 
    SERIALIZE_MEMBER("backZOffset",m_backZOffset); 
    SERIALIZE_MEMBER("frontZOffset",m_frontZOffset); 
    SERIALIZE_MEMBER("isSynchronous",m_isSynchronous);
    SERIALIZE_MEMBER("restoreStateAtCheckpoint",m_restoreStateAtCheckpoint);
    SERIALIZE_CONTAINER_OBJECT("boneList",m_boneList);
    SERIALIZE_MEMBER("disableVisiblePrediction", m_disableVisiblePrediction );
    SERIALIZE_FUNCTION(postSerialize, ESerialize_Data_Load);

END_SERIALIZATION()

AnimLightComponent_Template::AnimLightComponent_Template()
: m_useBase(bfalse)
, m_smoothAnim(bfalse)
, m_canonizeTransitions(btrue)
, m_defaultBlendFrames(0)
, m_draw2D(bfalse)
, m_defaultColor(Color::white())
, m_startFlip(bfalse)
, m_backZOffset(0.f)
, m_frontZOffset(0.f)
, m_isSynchronous(bfalse)
, m_restoreStateAtCheckpoint(bfalse)
, m_disableVisiblePrediction( bfalse )
{
}

void AnimLightComponent_Template::postSerialize()
{
    PathString_t animPath;
    m_animPath.getString(animPath);
    m_subAnimSet.postSerialize(animPath);
}

} //namespace ITF


