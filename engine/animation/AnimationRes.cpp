#include "precompiled_engine.h"

#ifndef _ITF_ANIMATIONRES_H_
#include "engine/animation/AnimationRes.h"
#endif //_ITF_ANIMATIONRES_H_

#ifndef _ITF_ANIMATIONMANAGER_H_
#include "engine/animation/AnimationManager.h"
#endif //_ITF_ANIMATIONMANAGER_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_RESOURCEGROUP_H_
#include "engine/resources/ResourceGroup.h"
#endif //_ITF_RESOURCEGROUP_H_


#ifndef _ITF_ANIMATIONSKELETON_H_
#include    "engine/animation/AnimationSkeleton.h"
#endif // _ITF_ANIMATIONSKELETON_H_

#ifndef _ITF_ANIMATIONPATCHBANK_H_
#include    "engine/animation/AnimationPatchBank.h"
#endif // _ITF_ANIMATIONPATCHBANK_H_

#ifndef _ITF_ANIMATIONTRACK_H_
#include    "engine/animation/AnimationTrack.h"
#endif // _ITF_ANIMATIONTRACK_H_

#ifndef _ITF_ANIMATIONDEPENDENCIES_H_
#include "engine/animation/AnimationDependencies.h"
#endif //_ITF_ANIMATIONDEPENDENCIES_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif // _ITF_FILESERVER_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_XMLALL_H_
#include "engine/XML/XML_All.h"
#endif //_ITF_XMLALL_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(AnimSkeletonResource)

bool PrefetchDep( const Path& path, PrefetchFactory::Params& params )
{
    AnimationDependenciesMap*       mapPtr = NULL;
    ResourceID                      mapRes;
    AnimDependenciesMapResource*    mapResPtr = NULL;
    PathString_t pathFilename;

    params.type = Resource::ResourceType_DependenciesMap;
    params.refType = PrefetchFactory::Params::Logical;
    path.getString(pathFilename);

    {
        csAutoLock cs( RESOURCE_MANAGER->getQueryOpsCs() );

        if ( RESOURCE_MANAGER->getResourceIdFromFile( mapRes, pathFilename, Resource::ResourceType_DependenciesMap ) )
        {
            RESOURCE_MANAGER->queryAddUserToResourceLogicalData( mapRes );
            params.res = mapRes.getResource();
            return btrue;
        }

        mapRes = RESOURCE_MANAGER->newResourceIDFromFile( Resource::ResourceType_DependenciesMap, path );
    }
    mapResPtr = static_cast< AnimDependenciesMapResource* >( mapRes.getResource() );

    mapPtr = newAlloc( mId_Animation, AnimationDependenciesMap );
    if ( !mapPtr->openBinary( pathFilename ) )
    {
        mapPtr->clear();
        RESOURCE_MANAGER->removeUserToResourceLogicalData( mapRes );
        return bfalse;
    }
    mapResPtr->setMap( mapPtr );
    mapResPtr->hijackPhysicalLoaded( btrue );
    params.res = mapResPtr;
    return btrue;
}

AnimSkeletonResource::AnimSkeletonResource(const Path &path) : Resource(path),  m_pOwnerData(0), m_canLoadBin(btrue)
{
#ifdef ENABLE_RESOURCE_HISTORY
        ResourceHistory record;
        record.m_opType = ResourceHistoryOp_CreateResource;
        record.m_opTime = CURRENTFRAME;
        record.m_resourceRef = getRef();
        record.m_path = path.cStr();
        ResourceHistoryManager::instance()->addResourceHistory(record);
#endif // ENABLE_RESOURCE_HISTORY
}

AnimSkeletonResource::~AnimSkeletonResource()
{
#ifdef ENABLE_RESOURCE_HISTORY
        ResourceHistory record;
        record.m_opType = ResourceHistoryOp_DeleteResource;
        record.m_opTime = CURRENTFRAME;
        record.m_resourceRef = getRef();
        record.m_path = m_path.getString().cStr();
        ResourceHistoryManager::instance()->addResourceHistory(record);
#endif // ENABLE_RESOURCE_HISTORY
}

void    AnimSkeletonResource::flushPhysicalData()
{
    SF_DEL(m_pOwnerData);
    m_pOwnerData = 0;
}

bbool    AnimSkeletonResource::tryLoadFile()
{
    ITF_ASSERT_MSG(!isPhysicallyLoaded(), "duplicate resource loading can lead to crashes");
    return tryLoadFileByPlatform(FILESERVER->getPlatform());
} // namespace ITF


bbool    AnimSkeletonResource::tryLoadFileByPlatform(const String& platform)
{
    ITF_ASSERT(!isPhysicallyLoaded());
    const bbool res = ANIM_MANAGER->loadAnimSkeleton(this, getPath(),platform, m_canLoadBin);
    if (res)
        RESOURCE_MANAGER->addResourceAsLoaded(this);

    ITF_WARNING_CATEGORY(Anim,NULL, res, "Could not load skeleton : '%s'", getPath().getString8().cStr());

    return res;
} // namespace ITF



IMPLEMENT_OBJECT_RTTI(AnimPatchBankResource)

void    AnimPatchBankResource::flushPhysicalData()
{
    SF_DEL(m_pOwnerData);
    m_pOwnerData = 0;
}

bbool    AnimPatchBankResource::tryLoadFile()
{
    ITF_ASSERT_MSG(!isPhysicallyLoaded(), "duplicate resource loading can lead to crashes");
    const bbool res = ANIM_MANAGER->loadAnimPatchBank(this, getPath(), m_canLoadBin);
    if (res)
        RESOURCE_MANAGER->addResourceAsLoaded(this);
    return res;
} // namespace ITF




//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
///// ANIM TRACK RES
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
u32 AnimTrackResource::s_groupCount = 0;

#ifndef ITF_FINAL
bbool AnimTrackResource::m_loadRessourcesOnActor = btrue;
#endif //ITF_FINAL


AnimTrackResource::AnimTrackResource(const Path &path) : Resource(path), m_trackPtr(0)
{
#ifndef ITF_FINAL
    if (!m_loadRessourcesOnActor)
        m_resourceGroup = RESOURCE_MANAGER->newResourceIDFromFile(Resource::ResourceType_ResourceGroup, Path::EmptyPath);
#endif
    m_canLoadBin = btrue;
}

AnimTrackResource::~AnimTrackResource()
{
    if (m_trackPtr)
        flushPhysicalData();
    
#ifndef ITF_FINAL
    if (!m_loadRessourcesOnActor)
        RESOURCE_MANAGER->removeUserToResourceLogicalData(m_resourceGroup);
#endif
}


bbool AnimTrackResource::changeBank(const StringID &_name, const String &_path)
{
    StringID str = ANIM_MANAGER->getStringID(_path);
    NamesMap::Iterator nameIter(m_nameToPatch);
    nameIter.findKey(_name);
    if (nameIter.isEnd())
    {
        m_nameToPatch.setID(_name, m_texturePathList.size());
        m_texturePathList.push_back(str);
    } else
    {
        m_texturePathList[nameIter.second()] = str;
    }

    return btrue;
}


#ifndef ITF_FINAL
ResourceGroup* AnimTrackResource::getResourceGroup() const
{
    if (!m_loadRessourcesOnActor)
        return (ResourceGroup*)(m_resourceGroup.getResource());
    else
        return NULL;
}
#endif

void AnimTrackResource::clearLists()
{
#ifndef ITF_FINAL
    if (m_loadRessourcesOnActor)
#endif //ITF_FINAL
    {
        ITF_VECTOR<ResourceID>::iterator textIt = m_textureResourceIDList.begin();
        ITF_VECTOR<ResourceID>::iterator patchIt = m_patchBankResourceIDList.begin();
        for (; textIt != m_textureResourceIDList.end() &&
            patchIt != m_patchBankResourceIDList.end(); 
            ++textIt, ++patchIt)
        {
            RESOURCE_MANAGER->removeUserToResourceLogicalData(*textIt);
            RESOURCE_MANAGER->removeUserToResourceLogicalData(*patchIt);
        }
    }
    m_textureResourceIDList.clear();
    m_patchBankResourceIDList.clear();
}

bbool AnimTrackResource::fillResources(ResourceGroup* _resGroup)
{
    //m_skeletonResourceID = _resGroup->addResource(Resource::ResourceType_AnimSkeleton, ANIM_MANAGER->getString(m_skeletonPath));

    const u32 count = m_texturePathList.size();
 
    clearLists();
    m_textureResourceIDList.reserve(count);
    m_patchBankResourceIDList.reserve(count);
    
    String pbkExtStr = AnimationDependencies::PBK_EXT;

    String    pathBankStr;
    for (u32 i=0; i<count; i++)
    {
        const StringID& pathId          = m_texturePathList[i];
        const String  & pathTextureStr  = ANIM_MANAGER->getString(pathId);
        pathBankStr = pathTextureStr;
        FilePath::changeExtension(pathBankStr, pbkExtStr);

        ResourceID localID = RESOURCE_MANAGER->newResourceIDFromFile(Resource::ResourceType_Texture, pathTextureStr);
        m_textureResourceIDList.push_back(localID);
#ifndef ITF_FINAL
        if (!m_loadRessourcesOnActor)
            RESOURCE_MANAGER->queryAddResourceToGroup( _resGroup->getID(), localID );
#endif //ITF_FINAL

        ResourceID patchBankRscID = RESOURCE_MANAGER->newResourceIDFromFile(Resource::ResourceType_AnimPatchBank, pathBankStr);
#ifndef ITF_FINAL
        if (!m_loadRessourcesOnActor)
            RESOURCE_MANAGER->queryAddResourceToGroup( _resGroup->getID(), patchBankRscID );
#endif //ITF_FINAL
        AnimPatchBankResource *patchBank = (AnimPatchBankResource*)patchBankRscID.getResource();
        if (patchBank)
        {
            patchBank->setCanLoadBin(m_canLoadBin && patchBank->getCanLoadBin());
        }
        m_patchBankResourceIDList.push_back(patchBankRscID);
    }
    m_trackPtr->m_textureResourceIDList     = m_textureResourceIDList;
    m_trackPtr->m_patchBankResourceIDList   = m_patchBankResourceIDList;
    return btrue;
}

void AnimTrackResource::flushPhysicalData()
{
    if (m_trackPtr)
    {
#ifndef ITF_FINAL
        if (m_loadRessourcesOnActor)
#endif
        {
            if (m_trackPtr->m_skeletonResourceID.isValidResourceId())
                RESOURCE_MANAGER->removeUserToResourceLogicalData(m_trackPtr->m_skeletonResourceID);
        }
    }
    SF_DEL(m_trackPtr);

    clearLists();

#ifndef ITF_FINAL
    if (!m_loadRessourcesOnActor)
    {
        RESOURCE_MANAGER->removeUserToResourcePhysicalData(m_resourceGroup);

        ResourceGroup *resGroup = getResourceGroup();
        ResourceGroup::ResourceList resList = resGroup->getResourcesList();
        for (u32 i=0; i<resList.size(); i++)
        {
            resGroup->removeResource(resList[i]);
        }
    }
#endif
}

bbool AnimTrackResource::tryLoadFile()
{
    ITF_ASSERT_MSG(!isPhysicallyLoaded(), "duplicate resource loading can lead to crashes");
    if (isPhysicallyLoaded() && m_trackPtr)
        return btrue;

#ifndef ITF_FINAL
    ResourceGroup *resGroup = getGroup();
#else
    ResourceGroup *resGroup = NULL;
#endif

    const bbool res = ANIM_MANAGER->loadAnimTrack(this, getPath(), resGroup, m_canLoadBin);
    if (!res)
    {
        ITF_WARNING_CATEGORY(Anim,NULL, res, "Could not load animation track : %s", getPath().getString8().cStr());
        return bfalse;
    }
    RESOURCE_MANAGER->addResourceAsLoaded(this);

    // load skeleton and patchbanks -> we are sure we using the good ones
    fillResources(resGroup);
    if (resGroup)
        RESOURCE_MANAGER->queryAddUserToResourcePhysicalData(resGroup);
    return res;
};

ResourceID AnimDependenciesMapResource::s_dependencyGroup;

IMPLEMENT_OBJECT_RTTI(AnimDependenciesMapResource);

ResourceGroup * AnimDependenciesMapResource::getDenpendenciesGroup()
{
    ResourceGroup* pGroup = (ResourceGroup*)s_dependencyGroup.getResource();
    if (pGroup)
        return pGroup;

    s_dependencyGroup =  RESOURCE_MANAGER->newResourceIDFromFile(Resource::ResourceType_ResourceGroup, Path::EmptyPath);
    return (ResourceGroup*)s_dependencyGroup.getResource();
}

AnimationDependenciesMap * AnimDependenciesMapResource::getDependenciesMap(const Path & _animDepFile, ResourceGroup *_grp )
{
    AnimationDependenciesMap*       mapPtr = NULL;
    ResourceID                      mapRes;
    AnimDependenciesMapResource*    mapResPtr = NULL;
    bbool                           justCreated = bfalse;

    if ( RESOURCE_MANAGER->getResourceIdFromFile(mapRes, _animDepFile.getStringID(), Resource::ResourceType_DependenciesMap) )
    {
        mapResPtr = mapRes.getResource()->DynamicCast<AnimDependenciesMapResource>(ITF_GET_STRINGID_CRC(AnimDependenciesMapResource,4011170951));
        mapPtr = mapResPtr->getMap();
    }
    else
    {
        PathString_t animDepStr;
        
        _animDepFile.getString(animDepStr);

#ifdef ITF_SUPPORT_COOKING
        bbool bRes = RESOURCE_MANAGER->cookNeeded(animDepStr);
        if (bRes)
            RESOURCE_MANAGER->requestCooking(animDepStr);
#endif  // ITF_SUPPORT_COOKING

        // If the resource doesn't exist we have to create it and read the file
        mapRes      = RESOURCE_MANAGER->newResourceIDFromFile(Resource::ResourceType_DependenciesMap, _animDepFile);
        mapResPtr   = static_cast<AnimDependenciesMapResource*>(mapRes.getResource());
        
        mapPtr = newAlloc(mId_Animation, AnimationDependenciesMap);
        if (!mapPtr->openBinary(animDepStr))
        {
            mapPtr->clear();
        }
        else
            mapResPtr->hijackPhysicalLoaded( btrue );
        mapResPtr->setMap(mapPtr);
        justCreated = btrue;
    }

    ResourceGroup*  grp = _grp != NULL ? _grp : getDenpendenciesGroup();
    bbool           inMainThread = Synchronize::getCurrentThreadId() == ThreadSettings::m_settings[eThreadId_mainThread].m_threadID;

    if ( inMainThread )
    {
        if ( !grp->usesResource( mapRes ) )
            grp->addResource( mapResPtr, !justCreated, bfalse );
    }
    else
    {
        if ( !justCreated )
            RESOURCE_MANAGER->queryAddUserToResourceLogicalData( mapRes );
        RESOURCE_MANAGER->queryAddResourceToGroup( grp->getID(), mapRes );
    }
    ITF_ASSERT_CRASH(mapPtr, "NULL resource -> big problem");
    return mapResPtr->getMap();

}


void AnimDependenciesMapResource::terminate()
{
    if (!s_dependencyGroup.isValidResourceId())
        return;

    RESOURCE_MANAGER->removeUserToResourceLogicalData(s_dependencyGroup);
    s_dependencyGroup.invalidateResourceId();

}

bbool AnimDependenciesMapResource::fillGroupDependencies( ResourceGroup *_group, const String & _anim, const Path & _depenencyFile, AABB *_aabb )
{
    ITF_ASSERT(Synchronize::getCurrentThreadId() == ThreadSettings::m_settings[eThreadId_mainThread].m_threadID);
    static String tmpString; // to avoid excessive allocations -> code only running in main thread

    AnimationDependenciesMap * mapPtr = NULL;
    AnimationDependencies    * animDep = NULL;

    if (AnimationDependenciesMap::s_computeDependenciesCallback)
    {
        if (!AnimationDependenciesMap::s_computeDependenciesCallback(mapPtr, animDep, _anim, _depenencyFile, _group))
            return bfalse;
    } else
    {
        mapPtr                        = getDependenciesMap(_depenencyFile, _group );
        if (!mapPtr)
            return bfalse;

        MapDependencies & depMap          = mapPtr->getMap();
        MapDependencies::iterator mapIter = depMap.find(_anim);

        if (mapIter == depMap.end())
            return bfalse;

        animDep = &mapIter->second;
    }

    ResourceID localID;
    animDep->getSkeletonPath(tmpString, mapPtr->getPathMap());
    _group->addResource(Resource::ResourceType_AnimSkeleton, tmpString);

    u32 patchCount =  animDep->getTextureCount();
    for (u32 i = 0; i < patchCount; i++)
    {
        animDep->getTextureAt(tmpString, mapPtr->getPathMap(), i);
        _group->addResource(Resource::ResourceType_Texture, tmpString);
        animDep->getPatchBankAt(tmpString, mapPtr->getPathMap(), i);
        _group->addResource(Resource::ResourceType_AnimPatchBank, tmpString);
    }

    if (_aabb)
        *_aabb = animDep->getBoundingBox();

    return animDep->isBoundingFound();
}

bbool AnimDependenciesMapResource::fillPathList( ITF_VECTOR<Path> & _pathList, ITF_VECTOR<u32> & _pathTypeList, const String & _anim, const Path & _depenencyFile, AABB *_aabb )
{
    ITF_ASSERT(Synchronize::getCurrentThreadId() == ThreadSettings::m_settings[eThreadId_mainThread].m_threadID);

    AnimationDependenciesMap * mapPtr = NULL;
    AnimationDependencies    * animDep = NULL;

    if (AnimationDependenciesMap::s_computeDependenciesCallback)
    {
        if (!AnimationDependenciesMap::s_computeDependenciesCallback(mapPtr, animDep, _anim, _depenencyFile, NULL))
            return bfalse;
    } else
    {
        mapPtr                        = getDependenciesMap(_depenencyFile, NULL );
        if (!mapPtr)
            return bfalse;

        MapDependencies & depMap          = mapPtr->getMap();
        MapDependencies::iterator mapIter = depMap.find(_anim);

        if (mapIter == depMap.end())
            return bfalse;

        animDep = &mapIter->second;
    }

    ResourceID localID;
    Path skeleton = animDep->getSkeletonPath(mapPtr->getPathMap());
    if (std::find(_pathList.begin(), _pathList.end(), skeleton) == _pathList.end())
    {
        _pathList.push_back(skeleton);
        _pathTypeList.push_back((u32)Resource::ResourceType_AnimSkeleton);
    }

    u32 patchCount =  animDep->getTextureCount();
    for (u32 i = 0; i < patchCount; i++)
    {
        Path texture = animDep->getTextureAt(mapPtr->getPathMap(), i);
        if (std::find(_pathList.begin(), _pathList.end(), texture) == _pathList.end())
        {
            _pathList.push_back(texture);
            _pathTypeList.push_back((u32)Resource::ResourceType_Texture);
        }

        Path patchBank = animDep->getPatchBankAt(mapPtr->getPathMap(), i);
        if (std::find(_pathList.begin(), _pathList.end(), patchBank) == _pathList.end())
        {
            _pathList.push_back(patchBank);
            _pathTypeList.push_back((u32)Resource::ResourceType_AnimPatchBank);
        }
    }

    if (_aabb)
        *_aabb = animDep->getBoundingBox();

    return animDep->isBoundingFound();
}

AnimDependenciesMapResource::~AnimDependenciesMapResource()
{
    SF_DEL(m_animDependenciesMap);
}

bbool AnimDependenciesMapResource::tryLoadFile()
{
    ITF_ASSERT_MSG(!isPhysicallyLoaded(), "duplicate resource loading can lead to crashes");
    RESOURCE_MANAGER->addResourceAsLoaded(this);
    return btrue;
}




}
