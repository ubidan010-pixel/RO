#include "precompiled_engine.h"

#ifndef _ITF_ANIMATIONMANAGER_H_
#include "engine/animation/AnimationManager.h"
#endif //_ITF_ANIMATIONMANAGER_H_

#ifndef _ITF_ANIMATIONRES_H_
#include "engine/animation/AnimationRes.h"
#endif //_ITF_ANIMATIONRES_H_

#ifndef _ITF_XMLALL_H_
#include "engine/XML/XML_All.h"
#endif //_ITF_XMLALL_H_

#ifndef _ITF_ANIMATIONSKELETON_H_
#include    "engine/animation/AnimationSkeleton.h"
#endif // _ITF_ANIMATIONSKELETON_H_

#ifndef _ITF_ANIMATIONPATCHBANK_H_
#include    "engine/animation/AnimationPatchBank.h"
#endif // _ITF_ANIMATIONPATCHBANK_H_

#ifndef _ITF_ANIMATIONTRACK_H_
#include    "engine/animation/AnimationTrack.h"
#endif // _ITF_ANIMATIONTRACK_H_

#ifndef _ITF_ANIMLIGHTCOMPONENT_H_
#include "engine/actors/components/AnimLightComponent.h"
#endif //_ITF_ANIMLIGHTCOMPONENT_H_

#ifndef _ITF_RESOURCEGROUP_H_
#include "engine/resources/ResourceGroup.h"
#endif // _ITF_RESOURCEGROUP_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif // _ITF_FILESERVER_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_ANIMATION_H_
#include "engine/animation/Animation.h"
#endif //_ITF_ANIMATION_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifdef ITF_COMPRESSED_ANIM
#include "engine/lzo/minilzo.h"
#endif // ITF_COMPRESSED_ANIM

namespace ITF
{

AnimManager::AnimManager()
{
    m_PData = newAlloc(mId_Animation,Patch32Data(VertexFormat_PT));
};

AnimManager::~AnimManager()
{   
    SF_DEL(m_PData);    
    m_idToString.clear();
} 

StringID    AnimManager::getStringID(const String & _str)
{
    String normalized = FilePath::normalizePath(_str);
    normalized.replace("//", "/");

    StringID strId(normalized);
    ITF_MAP<StringID, String>::iterator str_id =  m_idToString.find(strId);
    if (str_id == m_idToString.end())
    {
        m_idToString[strId] = normalized;
    } else
    {
        ITF_ASSERT(normalized == str_id->second);
    }
    return strId;
}

const String & AnimManager::getString(const StringID & _str)
{
    ITF_MAP<StringID, String>::iterator str_id =  m_idToString.find(_str);
    if (str_id != m_idToString.end())
    {
        return str_id->second;
    } else
    {
        return String::emptyString;
    }
}

void AnimManager::setStringStringID(const String & _str, const StringID & _strId)
{
    m_idToString[_strId] = _str;
}



void    AnimManager::dumpData()
{
    //OutputDebugger dbg;
    //dbg << "\n\n  AnimManager::dumpData\n";
    //for (u32 i = 0; i < m_scenesList.size(); i++)
    //{
    //    AnimScene* scn = m_scenesList[i];
    //    dbg << "\nSCENE\n-----\n";
    //    String str;
    //    str.setTextFormat("  UnicBasssss: %d\n  SceneVersion: %d\n  BMLname:", scn->m_UnicBasssss, scn->m_SceneVersion);
    //    dbg << str;
    //    for (u32 j = 0; j < scn->m_sceneList.size(); j++)
    //    {
    //        dbg << "\n  SUBSCENE\n  -----\n    ";
    //    }
    //    for (u32 j = 0; j < scn->m_animList.size(); j++)
    //    {
    //        dbg << "\n  ANIM\n  -----\n    ";
    //    }
    //}
}

/////////////////////// Skeleton

bbool AnimManager::loadAnimSkeleton(AnimSkeletonResource * _animSkeletonRes, const Path& _path,const String& _platform, bbool _tryLoadingBin)
{
    AnimSkeleton * pSkeleton = newAlloc(mId_Animation, AnimSkeleton);
    PathString_t pathFilename;

    _path.getString(pathFilename);

    if (!pSkeleton->loadFile(pathFilename,_platform,_tryLoadingBin,btrue))
    {
        SF_DEL(pSkeleton);
        return bfalse;
    }

    _animSkeletonRes->setOwnerData(pSkeleton);

#ifndef ITF_FINAL
    u32 dwChunkSize = sizeof(*pSkeleton);
    dwChunkSize += pSkeleton->m_BonesCurFrame.capacity() * sizeof(AnimBone);
    dwChunkSize += pSkeleton->m_BonesTPauseDyn.capacity() * sizeof(AnimBoneDyn);
    _animSkeletonRes->setMemoryUsage(dwChunkSize );
#endif
    return btrue;
}

void AnimManager::unloadAnimSkeleton(AnimSkeletonResource * _animSkeletonRes)
{
    AnimSkeleton* pSkeleton = _animSkeletonRes->getOwnerData();
    _animSkeletonRes->setOwnerData(NULL);
    SF_DEL(pSkeleton);
}



/////////////////////// PatchBank
bbool AnimManager::loadAnimPatchBank(AnimPatchBankResource * _animPatchBank, const Path& _path, bbool _tryLoadingBin)
{
    AnimPatchBank * pPatchBank = newAlloc(mId_Animation, AnimPatchBank);
    PathString_t pathFilename;

    _path.getString(pathFilename);

    if (!pPatchBank->loadFile(pathFilename,FILESERVER->getPlatform(), _tryLoadingBin, btrue))
    {
        SF_DEL(pPatchBank);
        return bfalse;
    }

    _animPatchBank->setOwnerData(pPatchBank);
#ifndef ITF_FINAL
    u32 dwChunkSize = sizeof(*pPatchBank);
    dwChunkSize += pPatchBank->m_NameToTemplate.size()*(sizeof(StringID) + sizeof(i32));
    dwChunkSize += pPatchBank->m_templateList.capacity()*sizeof(AnimTemplate*);
    for (u32 i=0; i<pPatchBank->m_templateList.size(); i++)
    {
        AnimTemplate * tmplt = pPatchBank->m_templateList[i];
        dwChunkSize += sizeof(*tmplt);
        dwChunkSize += tmplt->m_BonesCurFrame.capacity()*sizeof(AnimBone);

        dwChunkSize += tmplt->m_BonesCurFrameDyn.capacity()*sizeof(AnimBoneDyn);
        dwChunkSize += tmplt->m_PatchPoints.capacity()*sizeof(AnimPatchPoint);

        dwChunkSize += tmplt->m_Patch.capacity()*sizeof(AnimPatch);
    }
    _animPatchBank->setMemoryUsage(dwChunkSize);
#endif
    return true;
}

void AnimManager::unloadAnimPatchBank(AnimPatchBankResource * _animPatchBank)
{
    AnimPatchBank* pPatchBank = _animPatchBank->getOwnerData();
    _animPatchBank->setOwnerData(NULL);
    SF_DEL(pPatchBank);
}




/////////////////////// Track
bbool isMapKeysInMap(const NamesMap &_map1, const NamesMap &_map2)
{
    NamesMap::Iterator it(_map1);
    for (; !it.isEnd(); it.increment())
    {
        if (_map2.find(it.first()) < 0)
            return bfalse;
    }
    return btrue;
}

bbool AnimManager::loadAnimTrack(AnimTrackResource * _animTrackRes, const Path& _path, ResourceGroup *_resGroup, bbool _tryLoadingBin)
{
    AnimTrack * pTrack = newAlloc(mId_Animation, AnimTrack);
    PathString_t pathFilename;

    _path.getString(pathFilename);

    if (!pTrack->loadFile(pathFilename,FILESERVER->getPlatform(), _tryLoadingBin, btrue))
    {
        SF_DEL(pTrack);
        return bfalse;
    }

    ITF_ASSERT(isMapKeysInMap(_animTrackRes->m_nameToPatch, pTrack->m_nameToPatch));

    // add on AnimTrackResource default tracks infos
    NamesMap::Iterator it(pTrack->m_nameToPatch);
    for (; !it.isEnd(); it.increment())
    {
        NamesMap::Iterator it2(_animTrackRes->m_nameToPatch);
        it2.findKey(it.first());
        if (it2.isEnd())
        {
            #ifdef ITF_DEBUG_STRINGID
                _animTrackRes->m_nameToPatch.setDBG(it.first(), _animTrackRes->m_texturePathList .size(), it.dbg());
            #else
                _animTrackRes->m_nameToPatch.set(it.first(), _animTrackRes->m_texturePathList.size());
            #endif
            _animTrackRes->m_texturePathList.push_back(pTrack->m_patchBankPathList[it.second()]);
        }
    }
    // AnimTrackResource are ok, so now, we can set the track with resource
    pTrack->m_nameToPatch           = _animTrackRes->m_nameToPatch;
    pTrack->m_patchBankPathList     = _animTrackRes->m_texturePathList;

    // for the skeleton, use loaded one
    _animTrackRes->m_skeletonPath   = pTrack->m_skeletonPath;

    if (!pTrack->m_skeletonResourceID.isValidResourceId())
    {
        if (!pTrack->m_skeletonPath.isValid())
        {
            //ITF_FATAL_ERROR("Invalid skeleton on track %s", _path);
            return bfalse;
        }
        const String &skeletonPath = ANIM_MANAGER->getString(pTrack->m_skeletonPath);
        if (skeletonPath.isEmpty())
        {
            //ITF_FATAL_ERROR("Invalid skeleton on track %s", _path);
            return bfalse;
        }
        pTrack->m_skeletonResourceID = RESOURCE_MANAGER->newResourceIDFromFile(Resource::ResourceType_AnimSkeleton, skeletonPath);
        if (_resGroup)
            RESOURCE_MANAGER->queryAddResourceToGroup( _resGroup->getID(), pTrack->m_skeletonResourceID );
    }

    _animTrackRes->setTrack(pTrack);

#if defined(ITF_WINDOWS) && !defined(ITF_FINAL)
    ITF_MAP<StringID, ITF_VECTOR<Path>>::iterator sklFind = m_skeletonToTrack.find(pTrack->m_skeletonPath);
    if (sklFind != m_skeletonToTrack.end())
    {
        bbool found = bfalse;
        ITF_VECTOR<Path> & animList = sklFind->second;
        for (ITF_VECTOR<Path>::iterator iter = animList.begin(); !found && iter != animList.end(); iter++)
        {
            if ((*iter) == _path)
                found = btrue;
        }
        if (!found)
            animList.push_back(_path);
    } else
    {
        ITF_VECTOR<Path>  animList;
        animList.push_back(_path);
        m_skeletonToTrack[pTrack->m_skeletonPath] = animList;
    }
#endif

#ifndef ITF_FINAL
    u32 dwChunkSize = sizeof(*pTrack);
    dwChunkSize += pTrack->m_patchBankPathList.capacity()*sizeof(StringID);
    dwChunkSize += pTrack->m_nameToPatch.size()*(sizeof(StringID) + sizeof(i32));
    dwChunkSize += pTrack->m_bmlList.capacity()*sizeof(AnimTrackBML);
    dwChunkSize += pTrack->m_bonesList.capacity()*sizeof(AnimTrackBonesList);
    dwChunkSize += pTrack->m_eventList.capacity()*sizeof(AnimTrackFrameEvents);
    for (u32 i=0; i<pTrack->m_bmlList.size(); i++)
    {
        AnimTrackBML & bml = pTrack->m_bmlList[i];
        dwChunkSize += bml.m_templateRefs.capacity()*sizeof(TemplateRef);
    }
    dwChunkSize += pTrack->m_trackListPAS.capacity()*sizeof(AnimTrackBonePAS);
    dwChunkSize += pTrack->m_trackListZAL.capacity()*sizeof(AnimTrackBoneZAL);
    for (u32 i=0; i<pTrack->m_eventList.size(); i++)
    {
        AnimTrackFrameEvents & eList = pTrack->m_eventList[i];
        dwChunkSize += eList.m_frameEvents.capacity()*sizeof(AnimMarkerEvent);
    }
    _animTrackRes->setMemoryUsage(dwChunkSize);
#endif
    return true;
}

void AnimManager::unloadAnimTrack(AnimTrackResource * _animTrackRes)
{
    AnimTrack* pTrack = _animTrackRes->getTrack();
    _animTrackRes->setTrack(NULL);
    SF_DEL(pTrack);
}

void AnimManager::synchronize()
{
    for (u32 i = 0; i < m_animSyncroList.size(); )
    {
        AnimData & animData = m_animSyncroList[i];
        if (animData.m_type == AnimData::anim_synchronize)
        {
            AnimInfo * info = m_animSyncroList[i].m_info;
            ITF_ASSERT_CRASH(info, "wrong synchronisation type");
            info->copyWorkingToCurrent();

#ifndef ANIMATION_FIX_ABB
            ITF_ASSERT_CRASH(bfalse, "deprecated, if you hit this assert, contact smathieu/ysalas");
            Actor * actor = (Actor *)m_animSyncroList[i].m_actor.getObject();
            if (actor)
            {
                actor->GetComponent<AnimLightComponent>()->updateAABB();
            }
#endif
            m_animSyncroList.eraseNoOrder(i);
        } else
        {
            ++i;
        }
    }

    u32 size = m_animSyncroList.size();
    for (u32 i = 0; i < size; ++i)
    {
        AnimData & animData = m_animSyncroList[i];
        if (animData.m_type == AnimData::anim_unloadResource)
        {
            Actor * actor = (Actor *)m_animSyncroList[i].m_actor.getObject();
            if (actor)
            {
                AnimLightComponent *animLightComponent = actor->GetComponent<AnimLightComponent>();
                ITF_ASSERT(animLightComponent);
                if (animLightComponent)
                {
                    animLightComponent->deleteAnimMesh();
                }
            }
        }
    }

    m_animSyncroList.clear();

#ifdef ITF_COMPRESSED_ANIM
	// Free unused animations from cache
	i32 i;
	for (i=m_compressionCache.size()-1; i>=0;--i)
	{
		CachedAnim *cachedAnim = &m_compressionCache[i];
		if (cachedAnim->m_frameCounter != m_frameCounter)
		{
			cachedAnim->m_track->clearDecompressed();
			u32 cacheSize = m_compressionCache.size();
			m_compressionCache[i] = m_compressionCache[cacheSize-1];
			m_compressionCache.resize(cacheSize-1);
		}
	}
	m_frameCounter++;
#endif //ITF_COMPRESSED_ANIM
}

#ifdef ITF_COMPRESSED_ANIM
void AnimManager::UseTrack(AnimTrack *_track)
{
	// Case where the track is not compressed
	if (!_track->m_compressedTrackListPAS)
		return;

	// Search track in cache
	u32 i;
	for (i=0; i<m_compressionCache.size(); i++)
	{
		CachedAnim *pCachedAnim = &m_compressionCache[i];
		if (pCachedAnim->m_track == _track)
		{
			// found track is in decompressed cache
			pCachedAnim->m_frameCounter = m_frameCounter;
			return;
		}
	}

	// Track is not decompressed -> decompress in MEM1.
	MEM_M_PushExt(MEM_C_MEM1_ALLOC);
	void *pDecompressedData = ITF::Memory::mallocCategory(_track->m_trackListPASSize,MemoryId::mId_Animation);
	MEM_M_PopExt();

	lzo_uint decompressedSize;
	int error = lzo1x_decompress((lzo_bytep)_track->m_compressedTrackListPAS,(lzo_uint)_track->m_trackListPASCompressedSize,(lzo_bytep)pDecompressedData,(lzo_uintp)&decompressedSize,NULL);
	ITF_ASSERT((error == LZO_E_OK) && (decompressedSize == _track->m_trackListPASSize));

	_track->m_trackListPAS.setData((AnimTrackBonePAS *)pDecompressedData,_track->m_trackListPASSize/sizeof(AnimTrackBonePAS));

	// Add track to cache
	CachedAnim cachedAnim;
	cachedAnim.m_frameCounter = m_frameCounter;
	cachedAnim.m_track = _track;
	m_compressionCache.push_back(cachedAnim);
}
#endif //ITF_COMPRESSED_ANIM

void AnimManager::unregisterAnimForResourceUnload( ObjectRef _actor )
{
    for (u32 i=0; i<m_animSyncroList.size();) 
    {
        if (m_animSyncroList[i].m_type == AnimData::anim_unloadResource &&
            m_animSyncroList[i].m_actor == _actor)
        {
            m_animSyncroList.eraseNoOrder(i);
        } else
        {
            i++;
        }
    }
}

bbool AnimManager::IsInfoInAnimForSyncro( AnimInfo * _info, ObjectRef * _actor )
{
    for (u32 i=0; i<m_animSyncroList.size(); i++) 
    {
        if (m_animSyncroList[i].m_type == AnimData::anim_synchronize &&
            m_animSyncroList[i].m_info == _info)
        {
            if (_actor)
                *_actor = m_animSyncroList[i].m_actor;
            return btrue; 
        }
    }
    
    return bfalse; 
}


};
