#include "precompiled_engine.h"

#ifndef _ITF_ANIMATIONSTATS_H_
#include "engine/animation/AnimationStats.h"
#endif //_ITF_ANIMATIONSTATS_H_

#ifdef ENABLE_ANIMATIONSTATS

#ifndef _ITF_FRISE_H_
#include "engine/animation/Animation.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_ANIMATIONTRACK_H_
#include "engine/animation/AnimationTrack.h"
#endif //_ITF_ANIMATIONTRACK_H_

#ifndef _ITF_ANIMATIONSKELETON_H_
#include    "engine/animation/AnimationSkeleton.h"
#endif // _ITF_ANIMATIONSKELETON_H_

#ifndef _ITF_ANIMATIONPATCHBANK_H_
#include    "engine/animation/AnimationPatchBank.h"
#endif // _ITF_ANIMATIONPATCHBANK_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_ID_SERVER_H_
#include "core/IDserver.h"
#endif //_ITF_ID_SERVER_H_

namespace ITF
{

    AnimationStats* AnimationStats::m_singleton = NULL;


    char * sizeToChar(u32 size, char *buffer)
    {
        if (size < 1024)
        {
            sprintf(buffer, "%io", size);
            return buffer;
        }
        if (size < 1024*1024)
        {
            sprintf(buffer, "%.2fko", ((f32)size)/1024.f);
            return buffer;
        }
        sprintf(buffer, "%.2fMo", ((f32)size)/(1024.f*1024.f));
        return buffer;
    }

    void AnimationStats::createSingleton()
    {
        ITF_ASSERT(NULL == m_singleton);
        m_singleton = newAlloc(mId_Animation,AnimationStats());

        m_singleton->m_animationMemory = 0;
        m_singleton->m_animMeshMemory = 0;

    }

    void AnimationStats::deleteSingleton()
    {
        SF_DEL(m_singleton);
    }

    void AnimationStats::addAnimation(AnimationObject * anmObj)
    {
        ITF_ASSERT(m_animationList.find(anmObj) == -1);
        m_animationList.push_back(anmObj);

    }
    
    void AnimationStats::computeAnimation(AnimationObject * anmObj)
    {
        AnimantionData data;
        data.m_mainAnimSceneMemory = sizeof(*anmObj);
        // add basic objects infos
        data.m_mainAnimSceneMemory += anmObj->m_trackPtrList.capacity() * sizeof(AnimTrack*);
        data.m_mainAnimSceneMemory += anmObj->m_allPatchs.capacity() * sizeof(AnimPatch *);

        data.m_mainAnimSceneMemory += (sizeof(StringID) + sizeof(i32))*anmObj->m_animNameToIndex.size();
        data.m_mainAnimSceneFullMemory = data.m_mainAnimSceneMemory;

        data.m_subScene.clear();
        data.m_subSceneMemory = 0;
        u32 count = anmObj->m_sceneList.size();
        for (u32 i=0; i<count; ++i)
        {
            const AnimSubScene* subScene = anmObj->m_sceneList[i];
            AnimSubSceneData subData;
            subData.m_mainAnimSubSceneMemory = sizeof(*subScene);
            subData.m_mainAnimSubSceneMemory += subScene->m_UID.capacity()*sizeof(u16);
            subData.m_mainAnimSubSceneMemory += subScene->m_BackGroundMedia.capacity()*sizeof(u16);

            ITF_ASSERT(subScene->m_UIDToBone.size() == 0);
            ITF_ASSERT(subScene->m_UIDToPatchPoint.size() == 0);
            ITF_ASSERT(subScene->m_PtrToUID.size() == 0);

            subData.m_mainAnimSubSceneMemory += subScene->m_NameToBone.size()*(sizeof(StringID) + sizeof(i32));
            subData.m_mainAnimSubSceneMemory += subScene->m_BonesOrdered.capacity()*sizeof(AnimBone*);
            subData.m_mainAnimSubSceneMemory += subScene->m_BonesCurFrame.capacity()*sizeof(AnimBone*);
            subData.m_mainAnimSubSceneMemory += subScene->m_PatchPoints.capacity()*sizeof(AnimPatchPoint*);
            subData.m_mainAnimSubSceneMemory += subScene->m_Patch.capacity()*sizeof(AnimPatch*);

            subData.m_mainAnimSubSceneFullMemory = subData.m_mainAnimSubSceneMemory;

            subData.m_bonesStatics.clear();
            subData.m_bonesStaticsMemory = 0;
            u32 countIn = subScene->m_BonesCurFrame.size();
            for (u32 j=0; j<countIn; j++)
            {
                const AnimBone *b = subScene->m_BonesCurFrame[j];
                u32             val = sizeof(*b);
                subData.m_bonesStatics.push_back(val);
                subData.m_bonesStaticsMemory += val;
            }
            subData.m_mainAnimSubSceneFullMemory += subData.m_bonesStaticsMemory;

            subData.m_bonesDynamic.clear();
            subData.m_bonesDynamicMemory = 0;
            countIn = subScene->m_BonesCurFrameDyn.size();
            for (u32 j=0; j<countIn; j++)
            {
                const AnimBoneDyn   &b = subScene->m_BonesCurFrameDyn[j];
                u32                 val = sizeof(b);
                subData.m_bonesDynamic.push_back(val);
                subData.m_bonesDynamicMemory += val;
            }
            subData.m_mainAnimSubSceneFullMemory += subData.m_bonesDynamicMemory;

            subData.m_bonesTDynamic.clear();
            subData.m_bonesTDynamicMemory = 0;
            countIn = subScene->m_BonesCurFrameDyn.size();
            for (u32 j=0; j<countIn; j++)
            {
                const AnimBoneDyn   &b = subScene->m_BonesTPauseDyn[j];
                u32                 val = sizeof(b);
                subData.m_bonesTDynamic.push_back(val);
                subData.m_bonesTDynamicMemory += val;
            }
            subData.m_mainAnimSubSceneFullMemory += subData.m_bonesTDynamicMemory;

            subData.m_patchesPoints.clear();
            subData.m_patchesPointsMemory = 0;
            countIn = subScene->m_PatchPoints.size();
            for (u32 j=0; j<countIn; j++)
            {
                const AnimPatchPoint*pp = subScene->m_PatchPoints[j];
#ifndef ITF_UNIQUE_PATCH_POINT_REF
                u32                 val = sizeof(*pp) + pp->m_LocalRefs.capacity()*sizeof(AnimPatchPointLocal);
#else
                u32                 val = sizeof(*pp);
#endif
                subData.m_patchesPoints.push_back(val);
                subData.m_patchesPointsMemory += val;
            }
            subData.m_mainAnimSubSceneFullMemory += subData.m_patchesPointsMemory;

            subData.m_patches.clear();
            subData.m_patchesMemory = 0;
            countIn = subScene->m_Patch.size();
            for (u32 j=0; j<countIn; j++)
            {
                const AnimPatch     *p = subScene->m_Patch[j];
                u32                 val = sizeof(*p);
                subData.m_bonesTDynamic.push_back(val);
                subData.m_patchesMemory += val;
            }
            subData.m_mainAnimSubSceneFullMemory += subData.m_patchesMemory;;

            data.m_subScene.push_back(subData);
            data.m_subSceneMemory += subData.m_mainAnimSubSceneFullMemory;
        }
        data.m_mainAnimSceneFullMemory += data.m_subSceneMemory;

        data.m_tracks.clear();
        data.m_tracksMemory = 0;
        count = anmObj->m_animList.size();
        for (u32 i=0; i<count; ++i)
        {
            const AnimTrack * anm = anmObj->m_animList[i];
            TrackData trkData;

            trkData.m_mainTrackMemory = sizeof(*anm);
            trkData.m_mainTrackFullMemory = trkData.m_mainTrackMemory;

            trkData.m_bmlMemory = 0;
            u32 countIn = anm->m_bmlList.size();
            for (u32 j=0; j<countIn; j++)
            {
                const AnimTrackBML &bml = anm->m_bmlList[j];
                u32                 val = sizeof(bml) + bml.m_animSubScenebml.capacity()*sizeof(AnimSubScene*);
                trkData.m_bml.push_back(val);
                trkData.m_bmlMemory += val;
            }
            trkData.m_mainTrackFullMemory += trkData.m_bmlMemory;

            trkData.m_bonesMemory = 0;
            countIn = anm->m_bonesList.size();
            for (u32 j=0; j<countIn; j++)
            {
                const AnimTrackBonesList &bonesList = anm->m_bonesList[j];
                u32                      val =  sizeof(bonesList) +
                                                bonesList.m_trackList.capacity() * sizeof(AnimTrackBone);
                u32 countInIn = bonesList.m_trackList.size();
                for (u32 k=0; k<countInIn; k++)
                {
                    const AnimTrackBone &anmTrk = bonesList.m_trackList[k];
                    if (anmTrk.m_Lock)
                    {
                        val += sizeof(*anmTrk.m_Lock);
                    }
                }
                trkData.m_bones.push_back(val);
                trkData.m_bonesMemory += val;
            }
            trkData.m_mainTrackFullMemory += trkData.m_bonesMemory;


            trkData.m_eventsMemory = 0;
            countIn = anm->m_eventList.size();
            for (u32 j=0; j<countIn; j++)
            {
                const AnimTrackFrameEvents &eventsList = anm->m_eventList[j];
                u32                         val =  sizeof(eventsList) +
                                                    eventsList.m_frameEvents.capacity()*sizeof(AnimTrackEvent);
                trkData.m_events.push_back(val);
                trkData.m_eventsMemory += val;
            }
            trkData.m_mainTrackFullMemory += trkData.m_eventsMemory;

            data.m_tracks.push_back(trkData);
            data.m_tracksMemory += trkData.m_mainTrackFullMemory;
        }
        data.m_mainAnimSceneFullMemory += data.m_tracksMemory;

        m_animationMemory += data.m_mainAnimSceneFullMemory;
        m_animationDataList[anmObj] = data;
    }

    void AnimationStats::removeAnimation(AnimationObject * anmObj)
    {
        i32 index = m_animationList.find(anmObj);
        ITF_ASSERT(index >= 0);
        m_animationList.eraseNoOrder(index);
        
        // Remove Data
        ITF_MAP<AnimationObject *, AnimantionData>::iterator ad = m_animationDataList.find(anmObj);
        if (ad != m_animationDataList.end())
        {
            m_animationMemory -= (ad->second).m_mainAnimSceneFullMemory;
            m_animationDataList.erase(ad);
        }
    }

    void AnimationStats::addAnimMesh(AnimMeshScene *_an)
    {
        ITF_ASSERT(m_animMeshList.find(_an) == -1);
        m_animMeshList.push_back(_an);
    }

    void AnimationStats::computeAnimMesh(AnimMeshScene *_an)
    {
        // Process Data
        AnimMeshData data;
        
        data.m_mainAnimMeshMemory = sizeof(*_an);
        data.m_mainAnimMeshMemory += _an->m_pMesh.capacity()*sizeof(ITF_Mesh*);

        // add big one
        data.m_mainAnimMeshMemory += _an->m_AnimInfo.m_frameInfo.capacity() * sizeof(AnimFrameInfo);
        data.m_mainAnimMeshMemory += _an->m_AnimInfo.m_frameInfo.capacity() * sizeof(AnimFrameInfo);
        data.m_mainAnimMeshMemory += _an->m_AnimInfo.m_curBonesList.capacity() * sizeof(AnimBoneDyn);

        data.m_mainAnimMeshFullMemory = data.m_mainAnimMeshMemory;
        data.m_meshes.clear();
        u32 count = _an->m_pMesh.size();
        for (u32 i=0; i<count; i++)
        {
            const ITF_Mesh *mesh = _an->m_pMesh[i];
            u32 val = sizeof(*mesh) + mesh->m_ElementList.capacity() *sizeof(ITF_MeshElement);
            data.m_meshes.push_back(val);
            data.m_mainAnimMeshFullMemory += val;

        }

        m_animMeshMemory += data.m_mainAnimMeshFullMemory;
        m_animMeshDataList[_an] = data;
    }

    void AnimationStats::removeAnimMesh(AnimMeshScene *_an)
    {
        i32 index = m_animMeshList.find(_an);
        ITF_ASSERT(index >= 0);
        m_animMeshList.eraseNoOrder(index);

        // Remove Data
        ITF_MAP<AnimMeshScene *, AnimMeshData>::iterator ad = m_animMeshDataList.find(_an);
        if (ad != m_animMeshDataList.end())
        {
            m_animMeshMemory -= (ad->second).m_mainAnimMeshFullMemory;
            m_animMeshDataList.erase(ad);
        }
    }

    void AnimationStats::dumpToFile()
    {
        u32 count = m_animationList.size();
        for (u32 i=0; i<count; i++)
        {
            ITF_MAP<AnimationObject *, AnimantionData>::iterator ad = m_animationDataList.find(m_animationList[i]);
            if (ad == m_animationDataList.end())
                computeAnimation(m_animationList[i]);
        }

        count = m_animMeshList.size();
        for (u32 i=0; i<count; i++)
        {
            ITF_MAP<AnimMeshScene *, AnimMeshData>::iterator ad = m_animMeshDataList.find(m_animMeshList[i]);
            if (ad == m_animMeshDataList.end())
                computeAnimMesh(m_animMeshList[i]);
        }

#ifdef ITF_WINDOWS
        FILE* animf = fopen("c:\\animdump.txt", "wt");
        char  buffer[512];
        char  buffer2[512];

        fprintf(animf, "------------------ animation ------------------\n\n");
        fprintf(animf, "Nb animation: %d\n", (i32)m_animationList.size());
        fprintf(animf, "Total animation memory used: %s\n", sizeToChar(m_animationMemory, buffer));
        
        count = m_animationList.size();
        for (u32 i=0; i<count; i++)
        {
            AnimationObject *       animScene= m_animationList[i];
            const AnimantionData &  animData = m_animationDataList[animScene];
            fprintf(animf, "-- Anim %s - local mem %s - global mem %s\n", StringConverter(animScene->m_GroupName).getChar(), sizeToChar(animData.m_mainAnimSceneMemory, buffer), sizeToChar(animData.m_mainAnimSceneFullMemory, buffer2));
            fprintf(animf, "\t° SubScenes - Nb: %i, Memory: %s\n", (i32)animData.m_subScene.size(), sizeToChar(animData.m_subSceneMemory, buffer));
            u32 countIn = animData.m_subScene.size();
            for (u32 j=0; j<countIn; j++)
            {
                const AnimSubSceneData & assData = animData.m_subScene[j];
                fprintf(animf, "\t\t- SubScenes - local mem: %s - global mem: %s\n", sizeToChar(assData.m_mainAnimSubSceneMemory, buffer), sizeToChar(assData.m_mainAnimSubSceneFullMemory, buffer2));
                fprintf(animf, "\t\t\t- bonesStatics memory: %s\n", sizeToChar(assData.m_bonesStaticsMemory, buffer));
                fprintf(animf, "\t\t\t- bonesDynamic memory: %s\n", sizeToChar(assData.m_bonesDynamicMemory, buffer));
                fprintf(animf, "\t\t\t- bonesTDynamic memory: %s\n", sizeToChar(assData.m_bonesTDynamicMemory, buffer));
                fprintf(animf, "\t\t\t- patches memory: %s\n", sizeToChar(assData.m_patchesMemory, buffer));
                fprintf(animf, "\t\t\t- patchesPoints memory: %s\n", sizeToChar(assData.m_patchesPointsMemory, buffer));
            }
            fprintf(animf, "\t° Tracks - Nb: %i, Memory: %s\n", (i32)animData.m_tracks.size(), sizeToChar(animData.m_tracksMemory, buffer));
            
            countIn = animData.m_tracks.size();
            for (u32 j=0; j<countIn; j++)
            {
                const TrackData & trkData = animData.m_tracks[j];
                fprintf(animf, "\t\t- Tracks - local mem: %s - global mem: %s\n", sizeToChar(trkData.m_mainTrackMemory, buffer), sizeToChar(trkData.m_mainTrackFullMemory, buffer2));
                fprintf(animf, "\t\t\t- bml memory: %s\n", sizeToChar(trkData.m_bmlMemory, buffer));
                fprintf(animf, "\t\t\t- bones memory: %s\n", sizeToChar(trkData.m_bonesMemory, buffer));
                fprintf(animf, "\t\t\t- events memory: %s\n", sizeToChar(trkData.m_eventsMemory, buffer));
            }
        }

        fprintf(animf, "\n------------------ animMeshe ------------------\n\n");
        fprintf(animf, "Nb animMeshe: %d\n", (i32)m_animMeshList.size());
        fprintf(animf, "Total animMeshe memory used: %s\n", sizeToChar(m_animMeshMemory, buffer));

        count = m_animMeshList.size();
        for (u32 i=0; i<count; i++)
        {
            AnimMeshScene *   animMesh= m_animMeshList[i];
            const AnimMeshData &    animMeshData = m_animMeshDataList[animMesh];
            fprintf(animf, "-- AnimMesh for %s size: %s\n", StringConverter(animMesh->m_pAnimScene->m_GroupName).getChar(), sizeToChar(animMeshData.m_mainAnimMeshFullMemory, buffer));
        }

        fclose(animf);
#endif

    }

} // namespace ITF

#endif //ENABLE_ANIMATIONSTATS

