#include "precompiled_engine.h"

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_RESOURCELOADER_H_
#include "engine/resources/ResourceLoader.h"
#endif //_ITF_RESOURCELOADER_H_

#ifndef _ITF_RESOURCEGROUP_H_
#include "engine/resources/ResourceGroup.h"
#endif //_ITF_RESOURCEGROUP_H_

#ifndef _ITF_RESOURCE_H_
#include "engine/resources/Resource.h"
#endif //_ITF_RESOURCE_H_

#ifndef _ITF_XMLALL_H_
#include "engine/xml/XML_All.h"
#endif //_ITF_XMLALL_H_

#ifndef _ITF_TEXTURE_H_
#include "engine/display/Texture.h"
#endif //_ITF_TEXTURE_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_PUGIXMLWRAP_H_
#include "core/XML/PugiXMLWrap.h"
#endif //_ITF_PUGIXMLWRAP_H_

#ifndef _ITF_OUTPUTDEBUGGER_H_
#include "core/OutputDebugger.h"
#endif //_ITF_OUTPUTDEBUGGER_H_

#ifndef _ITF_FILEMANAGER_H_
#include "engine/AdaptersInterfaces/FileManager.h"
#endif //_ITF_FILEMANAGER_H_

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_ANIMATIONRES_H_
#include "engine/animation/AnimationRes.h"
#endif //_ITF_ANIMATIONRES_H_

#ifndef _ITF_SOUND_H_
#include "engine/sound/Sound.h"
#endif //_ITF_SOUND_H_

#ifndef _ITF_ANIMATIONMANAGER_H_
#include "engine/animation/AnimationManager.h"
#endif //_ITF_ANIMATIONMANAGER_H_

#ifndef _ITF_SYSTEMADAPTER_
#include "core/AdaptersInterfaces/SystemAdapter.h"
#endif //_ITF_SYSTEMADAPTER_

#ifndef _ITF_FONTRESOURCE_H_
#include "engine/display/FontResource.h"
#endif //_ITF_FONTRESOURCE_H_

#ifndef _ITF_FRISECONFIG_H_
#include "engine/display/Frieze/FriseConfig.h"
#endif //_ITF_FRISECONFIG_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_THREAD_H_
#include "core/system/Thread.h"
#endif //_ITF_THREAD_H_

#ifndef _ITF_ALIASMANAGER_H_
#include "engine/aliasmanager/aliasmanager.h"
#endif //_ITF_ALIASMANAGER_H_

#ifndef _ITF_BUNDLEMANAGER_H_
#include "engine/file/BundleManager.h"
#endif //_ITF_BUNDLEMANAGER_H_

#include "core/UnicodeTools.h"

#define ITF_NO_SEEK_LOG

//Could have a way to handle custom lifetime for different resources type or specfic resources...
#define RESOURCE_LOGICAL_DEFAULTLIFETIME  1 //keep it for 1 FRAMES

namespace ITF
{
#ifndef ITF_FINAL
  static int triggerDebugMark = 0;
#endif
ThreadLocalStorage  ResourceManager::m_localThreadId((u32) ResourceManager::invalidThread);
ResourceID ResourceManager::m_InvalidResourceID;
ResourceID ResourceManager::m_cookingGroupID;

u32 resourcePhysicalLifeTime = RESOURCE_PHYSICAL_DEFAULTLIFETIME;

// CS CHECK
//static bbool volatile isCSAdd = bfalse;
//static bbool volatile isCSRemove = bfalse;

#ifdef ASSERT_ENABLED
void checkForStringIDCollision(const Path& _path, const ResourceID _resID)
{
    Resource* res = _resID.getResource();
    ITF_ASSERT_MSG(res, "big bug : resource manager references deleted resources");
    if (res->getPath() != _path)
    {
        String msg("Resource Duplicate ID:\n");
        msg += "New resource path: ";
        msg += _path.getString();
        String crcs;
        crcs.setTextFormat("\nNew resource CRC: %s", _resID.getDebugString().cStr());
        msg += crcs;
        msg += "\nDuplicate resource: ";
        msg += res->getPath().getString();
        ITF_ASSERT_MSG(bfalse, "Yet another CRC collision");
        StringConverter cMsg(msg);
        ITF_ERROR(cMsg.getChar());
    }
}
#endif // ASSERT_ENABLED


ResourceManager::ResourceManager()
{
    m_defaultResourceGroup.invalidateResourceId();
    m_deleteLogicalAllowed = btrue;
    m_bEnableAsync = btrue;
    m_loadAllowed  = bfalse;
	m_bModeFinal   = btrue;
    m_useLinearMode = bfalse;
    m_exitedAsync = bfalse;

    m_bAllowTextureCooked = btrue;

    m_cookFunc     = NULL;
    m_cookNeededFunc    = NULL;
    m_cookFuncHRes =  NULL;

    m_pAsyncThread = NULL;

    Synchronize::createEvent(&m_exitEvent,"ResourceManagerExit");
    Synchronize::resetEvent(&m_exitEvent);

    Synchronize::createEvent(&m_activeloadEvent,"ResourceManagerActiveLoad");
    Synchronize::resetEvent(&m_activeloadEvent);

    Synchronize::createCriticalSection(&m_csResourceAsLoaded);
    Synchronize::createCriticalSection(&m_csResourceToProcess);
    Synchronize::createCriticalSection(&m_csResourceCurrentLoaded);
    Synchronize::createCriticalSection(&m_csResourceAsyncAdded);
    Synchronize::createCriticalSection(&m_csResourceAsyncSceneAdded);
    Synchronize::createCriticalSection(&m_csResourceQueryOps);
    Synchronize::createCriticalSection(&m_csResourceError);
    Synchronize::createCriticalSection(&m_csResourceCreation);

    for (u32 index = 0;index<maxThread;++index)
         Synchronize::createCriticalSection(&m_csResourceCallbackThread[index]);

    mResourceLoader = newAlloc(mId_Resource, ResourceLoader());

    m_DelayToLoad = 0;
    m_bImmediateFlushMode = bfalse;
    m_paused = 0;
    m_bDisableDefaultResourceUsage = bfalse;
    m_hotReloading = bfalse;

    m_ResourceDefaultID = newAlloc(mId_Resource,ResourceID[Resource::ResourceType_Count]);

    ResourceGroup::initStaticData();

    m_queryAddOps.reserve(m_defaultQuerryOpTableSize);
    m_queryRemoveOps.reserve(m_defaultQuerryOpTableSize);

#ifndef ITF_DISABLE_DEBUGINFO
    m_currentCountLoad = 0;
#endif //ITF_DISABLE_DEBUGINFO

#ifndef ITF_FINAL
        m_debugMarkIndex = 0;
#endif //!ITF_FINAL

}

void ResourceManager::init()
{
#ifndef ITF_CONSOLE
    if (!m_defaultResourceGroup.isValidResourceId())
    {
        m_defaultResourceGroup = newResourceIDFromFile(Resource::ResourceType_ResourceGroup, Path::EmptyPath);
        ResourceGroup* grp = (ResourceGroup*) m_defaultResourceGroup.getResource();
        m_ResourceDefaultID[Resource::ResourceType_Texture] = grp->addResource(Resource::ResourceType_Texture, GETPATH_ALIAS("cooking"));
        addUserToResourcePhysicalData(grp);
    }

#endif //ITF_CONSOLE
}

void    ResourceManager::flushStaticResources(bbool _parentsOnly)
{
    ITF_MAP<StringID, Resource*>::iterator  resIter;

    SafeArray<Resource*> staticResources;

    for (u32 itResTable = 0; itResTable < Resource::ResourceType_Count; itResTable++)
    {
        ITF_MAP<StringID, Resource*>& mResourceById = mResourcesById[itResTable];
        for (resIter=mResourceById.begin() ; resIter!=mResourceById.end(); resIter++)
        {
            Resource* pResource = resIter->second;
            if( pResource && pResource->isStatic() && !pResource->m_canBeFreed && (!pResource->isSubResource() || !_parentsOnly))
            {
                staticResources.push_back(pResource);
            }
        }
    }

    for (u32 iStat = 0; iStat < staticResources.size(); iStat++)
    {
        staticResources[iStat]->setStatic(bfalse);
    }

    for (u32 iStat = 0; iStat < staticResources.size(); iStat++)
    {
        Resource* pResource = staticResources[iStat];
        while(pResource->getPhysicalUsersCount() && !pResource->m_canBeFreed)
            pResource->removePhysicalDataUser();
    }

    for (u32 iStat = 0; iStat < staticResources.size(); iStat++)
    {   // Beware: because of resources hierarchy, deleting one may delete others
        Resource* pResource = staticResources[iStat];
        if (pResource->m_canBeFreed)
            continue;
        i32 lcount = pResource->getLogicalRefCount();
        while(lcount > 1 && !pResource->m_canBeFreed) // use a counter because pResource can be deleted at anytime within the loop
        {
            Resource::removeLogicalDataUser(pResource);
            lcount--;
        }
        if (lcount && !pResource->m_canBeFreed)
            Resource::removeLogicalDataUser(pResource);
    }

    staticResources.clear();
}


ResourceManager::~ResourceManager()
{
    manageDestruction(btrue, btrue);

#ifndef ITF_CONSOLE
    removeUserToResourcePhysicalData(m_defaultResourceGroup);
    removeUserToResourceLogicalData(m_defaultResourceGroup);
#endif //!ITF_CONSOLE

    DECLARE_OUTPUTDBG;
    OUTPUTDBG("\n\n Closing Resource Manager \n");

    cleanPostDeletedSingletons();

    flushPendingOps();

    flushStaticResources(btrue);
    flushStaticResources(bfalse);

    // delete all resources:
    ITF_MAP<StringID, Resource*>::iterator  resIter;

    for (u32 itResTable = 0; itResTable < Resource::ResourceType_Count; itResTable++)
    {
        ITF_MAP<StringID, Resource*>& mResourceById = mResourcesById[itResTable];
        for (resIter=mResourceById.begin() ; resIter!=mResourceById.end() ; ++resIter)
        {
            Resource* pResource = resIter->second;

            OUTPUTDBG(" Resource LEAK:");
            OUTPUTDBG("\n");

            ITF_ASSERT_MSG(0, "Please verify your resource leaks!!");

            if( !pResource)
                continue;

            ITF_ASSERT_MSG(!pResource->isStatic(), "Bug in static resources deletion");

            OUTPUTDBG((pResource->getPath().getString() + "\n").cStr());

            // if the resource is loaded
            if (pResource->isPhysicallyLoaded())
            {
                // unload it:
                pResource->removePhysicalDataUser();
                OUTPUTDBG("  --> Physical data still in memory. Check the resource dependencies\n");
                OUTPUTDBG(pResource->getPath().getString().cStr());
                OUTPUTDBG("\n");

                ITF_ASSERT_MSG(0, "Please verify your PHYSICAL resource leaks!!");
            }
        }
    }

    delete[] m_ResourceDefaultID;

    for (u32 itResTable = 0; itResTable < Resource::ResourceType_Count; itResTable++)
    {
        ITF_MAP<StringID, Resource*>& mResourceById = mResourcesById[itResTable];
        resIter = mResourceById.begin();
        while(resIter != mResourceById.end())
        {
            Resource* pResource = resIter->second;

            if (pResource && !pResource->isStatic())
            {
                ResourceID resID;
                resID.m_resource = getIdentifierFromResourcePtr(pResource);
                unregisterResource(resID); // will "normally" delete the resource
                Resource::removeLogicalDataUser(pResource);
                resIter = mResourceById.begin();
            }
            else
                ++resIter;

        }
        mResourceById.clear();
    }


    // unload all resource groups:


    // clear the rest of the data
    SF_DEL(mResourceLoader);

    OUTPUTDBG("Resource Manager closed \n\n");


    Synchronize::destroyEvent(&m_exitEvent);
    Synchronize::destroyEvent(&m_activeloadEvent);

    Synchronize::destroyCriticalSection(&m_csResourceAsLoaded);
    Synchronize::destroyCriticalSection(&m_csResourceToProcess);
    Synchronize::destroyCriticalSection(&m_csResourceCurrentLoaded);
    Synchronize::destroyCriticalSection(&m_csResourceAsyncAdded);
    Synchronize::destroyCriticalSection(&m_csResourceAsyncSceneAdded);
    Synchronize::destroyCriticalSection(&m_csResourceQueryOps);
    Synchronize::destroyCriticalSection(&m_csResourceError);
    Synchronize::destroyCriticalSection(&m_csResourceCreation);

    for (u32 index = 0;index<maxThread;++index)
        Synchronize::destroyCriticalSection(&m_csResourceCallbackThread[index]);

    ResourceGroup::deleteStaticData();
}

bbool ResourceManager::isCookedFileAllowed(const String _filename)
{
    String file = (_filename);
    file = FilePath::normalizePath(file);
    String ext = FilePath::getExtension(file);

    if (ext == "tga" || ext == "png")
    {
        return m_bAllowTextureCooked;
    }

    return btrue;
}

bool ResourceManager::getAlternativeFileNamesForCooking(const String& _resourcePathName, Vector<String>& _altResourcePathNames, const String& _platform)
{
    // On switch, we do not use upscaled textures (too much memory usage)
    if (_platform == "NX")
    {
        return false;
    }

    static String extensions[] = { String("png"), String("tga") };

    String extOfFile = FilePath::getExtension(_resourcePathName);
    extOfFile.toLower();

    bool fileAdded = false;

    for (const String& ext : extensions)
    {
        if (extOfFile != ext)
        {
            continue;
        }

        String szAlternativeName = FilePath::getDirectory(_resourcePathName);
        szAlternativeName += FilePath::getFilenameWithoutExtension(_resourcePathName);
        // upscale manually by Arist
        _altResourcePathNames.push_back(szAlternativeName + "_4kfixed." + ext);
        // upscale with Substance 3D sampler for the RGB and RealESRGAN for the alpha (then remerged)
        _altResourcePathNames.push_back(szAlternativeName + "_rsampler." + ext);
        // upscale with Substance 3D sampler for the RGB and simple lanczos for the alpha (then remerged)
        _altResourcePathNames.push_back(szAlternativeName + "_sampler." + ext);
        if (ext == "tga")
        {
            _altResourcePathNames.push_back(szAlternativeName + "_sampler." + "png"); // search for a png version
        }
        // upscale with Substance 3D sampler for the RGB, and Substance 3D sampler for the alpha (then remerged)
        _altResourcePathNames.push_back(szAlternativeName + "_usampler." + ext);
        // Full ESRGAN upscale. Image tends to be less generative but sometimes it is better like that.
        _altResourcePathNames.push_back(szAlternativeName + "_ESRGAN." + ext);

        fileAdded = true;
    }

    return fileAdded;
}

String ResourceManager::getExistingAlternativeFileNameForCooking(const String& _resourcePathName, const String& _platform)
{
    Vector<String> altResourcePathNames;
    if (getAlternativeFileNamesForCooking(_resourcePathName, altResourcePathNames, _platform))
    {
        for (const String& altName : altResourcePathNames)
        {
            if (FILEMANAGER->fileExists(altName))
            {
                return altName;
            }
        }
    }
    return _resourcePathName;
}

bbool ResourceManager::getResourceIdFromFile(ResourceID& _resourceID, const StringID &_path, Resource::ResourceType _type)
{
    ITF_ASSERT_MSG(Resource::ResourceType_ResourceGroup != _type, "groups can't be found by path");

    if (Resource::ResourceType_All == _type)
    {
        for (u32 itTypeParse = 0; itTypeParse < (u32)Resource::ResourceType_Count; itTypeParse++)
        {
            if ((u32)Resource::ResourceType_ResourceGroup == itTypeParse)
                continue;

            if (getResourceIdFromFile(_resourceID, _path, (Resource::ResourceType)itTypeParse))
                return btrue;
        }
        return bfalse;
    }

    {
        csAutoLock cs(m_csResourceCreation);

        ITF_MAP<StringID, Resource*>& mResourceById = getResourceMapFromType(_type);
        ITF_MAP<StringID, Resource*>::iterator iter = mResourceById.find(_path);
        if (iter == mResourceById.end())
        {
            return bfalse;
        }

        Resource* pResource = (*iter).second;
        _resourceID = pResource->getID();
        ITF_ASSERT_MSG(_resourceID.getResource(), "resource manager is referencing deleted objects");
    }
    return btrue;
}

ResourceID ResourceManager::newResourceIDFromFile(Resource::ResourceType type, const Path &_relativePath)
{
    return newResourceIDFromFile_Normalized(type, _relativePath);
}

//const ResourceID& ResourceManager::newResourceIDFromFile(Resource::ResourceType type, const String &_relativePath)
//{
//    String fixedupPath = FilePath::normalizePath(_relativePath);
//    StringID resourcePathStringID(fixedupPath);
//
//    return ResourceManager::newResourceIDFromFile_Normalized(type, fixedupPath, resourcePathStringID);
//}

ResourceID ResourceManager::newResourceIDFromFile_Normalized(Resource::ResourceType type, const Path &fixedupPath)
{
    Resource* res = NULL;

    {
        csAutoLock cs(m_csResourceCreation);

    //#if defined(ASSERT_ENABLED)
    //    bbool isValidPath = !fixedupPath.isEmpty();

    //    if ( !isValidPath )
    //    {
    //        if ( SYSTEM_ADAPTER )
    //        {
    //            String msg;
    //            msg.setTextFormat("Trying to create a resource from an invalid path : '%ls'", fixedupPath.getString().cStr());
    //            SYSTEM_ADAPTER->messageBox("File error", msg, ITF_MSG_ICONWARNING | ITF_MSG_OK, ITF_IDYES);
    //        }
    //        else
    //        {
    //            ITF_ASSERT_MSG(0, "Trying to create a resource from an invalid path : '%ls'", fixedupPath.getString().cStr());
    //        }
    //    }
    //#endif

        if (Resource::ResourceType_ResourceGroup != type)
        {   // can't check doublons for groups
            ResourceID resourcePathID;
            if (getResourceIdFromFile(resourcePathID, fixedupPath.getStringID(), type))
            {
        #ifdef ASSERT_ENABLED
                checkForStringIDCollision(fixedupPath, resourcePathID);
                Resource* pRes = resourcePathID.getResource();
                ITF_ASSERT_MSG(pRes, "big bug in resourcemanager");
        #endif //ASSERT_ENABLED
                {
                    csAutoUnlock csUnlock(m_csResourceCreation);
                    queryAddUserToResourceLogicalData(resourcePathID);
                }
                return resourcePathID;
            }
        }

    #ifdef ITF_CTR
       // LOG("Loading rsc %ls", fixedupPath.cStr());//CASA::Disable display loading ressource
    #endif


        switch(type)
        {
        case Resource::ResourceType_Texture:
            res = (Resource*)mResourceLoader->createImage(fixedupPath);
            break;
        case Resource::ResourceType_Sound:
            res = (Resource*) mResourceLoader->createSound(fixedupPath);
            break;
#ifdef ITF_SUPPORT_XML
        case Resource::ResourceType_XML:
            res = (Resource*) mResourceLoader->createXML(fixedupPath);
            break;
#endif //ITF_SUPPORT_XML
        case Resource::ResourceType_AnimSkeleton:
            res = (Resource*) mResourceLoader->createAnimSkeletonResource(fixedupPath);
            break;
        case Resource::ResourceType_AnimPatchBank:
            res = (Resource*) mResourceLoader->createAnimPatchBankResource(fixedupPath);
            break;
        case Resource::ResourceType_AnimTrack:
            res = (Resource*) mResourceLoader->createAnimTrackResource(fixedupPath);
            break;
        case Resource::ResourceType_Font:
            res = (Resource*) mResourceLoader->createFontResource(fixedupPath);
            break;
        case Resource::ResourceType_ResourceGroup:
            {
                ITF_ASSERT_MSG(fixedupPath.isEmpty(), "groups must be created with an empty path (Path::EmptyPath)");
                res = (Resource*) newAlloc(mId_Resource, ResourceGroup());
            }
            break;
        case Resource::ResourceType_FriezeConfig: // specific case : the path is actually a name
            {
                String8 path8;

                fixedupPath.getString(path8);
                res = newAlloc(mId_Resource,FriseConfig(path8));
            }
            break;
        case Resource::ResourceType_DependenciesMap: // specific case : the path is actually a name
             res = (Resource*) mResourceLoader->createAnimDependenciesMapResource(fixedupPath);
            break;
        case Resource::ResourceType_MusicPart:
            res = (Resource*) mResourceLoader->createMusicPart(fixedupPath);
            break;
        default:
            {
                ITF_ERROR("unknown resource type");
                return m_InvalidResourceID;
            }
        }

        ITF_ASSERT_CRASH(res,"res couldn't be null");
        ITF_MAP<StringID, Resource*>& mResourceById = getResourceMapFromType(type);
        const StringID& mapID = res->getPath().getStringID();
        #ifdef ASSERT_ENABLED
            if (Resource::ResourceType_ResourceGroup != type)
            {
                ITF_ASSERT_MSG(mResourceById.find(mapID) == mResourceById.end(), "a resource with the same name already exists");
            }
            ITF_ASSERT_MSG(res->getLogicalRefCount() == 1, "all newly created resources should have one logical user");
        #endif // ASSERT_ENABLED

        res->m_ID.setID(res);

        if (Resource::ResourceType_ResourceGroup != type)
            mResourceById[mapID] = res;

    }//close cl

    if (res)
       res->postCreate();


    return res->getID();
}

Resource *ResourceManager::newEmptyResource(Resource::ResourceType type)
{
    Resource *res = NULL;


    switch(type)
    {
    case Resource::ResourceType_Texture:
        res = mResourceLoader->createEmptyImage();
        break;
    default:
        ITF_ASSERT(0); // unknown type does not support empty resource creation
        return NULL;
    }
    res->m_ID.m_resource = getIdentifierFromResourcePtr(res);
    return res;
}





void ResourceManager::reloadResources()
{
    for (u32 itResTable = 0; itResTable < Resource::ResourceType_Count; itResTable++)
    {
        ITF_MAP<StringID, Resource*>& mResourceById = mResourcesById[itResTable];
        for (ITF_MAP<StringID, Resource*>::iterator iter = mResourceById.begin();iter != mResourceById.end();++iter)
        {
            iter->second->reload();
        }
    }
}

void ResourceManager::reloadResourcesByType(Resource::ResourceType _typeResource)
{
    ITF_MAP<StringID, Resource*>& mResourceById = getResourceMapFromType(_typeResource);
    for (ITF_MAP<StringID, Resource*>::iterator iter = mResourceById.begin();iter != mResourceById.end();++iter)
    {
        Resource* resource = (*iter).second;
        if (resource->isPhysicallyLoaded())
        {
            resource->reload();
        }
    }
}



bbool ResourceManager::reloadResource(const String& path)
{
    StringID id(path);
    for (u32 itResTable = 0; itResTable < Resource::ResourceType_Count; itResTable++)
    {
        ITF_MAP<StringID, Resource*>& mResourceById = mResourcesById[itResTable];
        ITF_MAP<StringID, Resource*>::iterator iter = mResourceById.find(id);
        if (iter != mResourceById.end())
        {
            Resource* resource = iter->second;
            if (resource->getPath() == path)
            {
                resource->reload();
                return btrue;
            }
        }
    }
    return bfalse;
}

void ResourceManager::unloadAllResources()
{
    for (u32 itResTable = 0; itResTable < Resource::ResourceType_Count; itResTable++)
    {
        ITF_MAP<StringID, Resource*>& mResourceById = mResourcesById[itResTable];
        for (ITF_MAP<StringID,Resource*>::iterator iter = mResourceById.begin() ;
            iter != mResourceById.end() ;
            ++iter)
        {
            Resource* r = iter->second;
            while(r->isPhysicallyLoaded())
            {
                r->removePhysicalDataUser();
            }
        }
    }
}


Font *ResourceManager::getFont(const ResourceID& id)
{
    FontResource* fr = reinterpret_cast<FontResource*>(id.getResource());
    return fr->getFont();
}



u32 ResourceManager::getTextureCount()
{
    ITF_MAP<StringID, Resource*>& mResourceById = getResourceMapFromType(Resource::ResourceType_Texture);
    return mResourceById.size();
}

Texture* ResourceManager::getTextureAt(u32 _index)
{
    u32 count = 0;
    ITF_MAP<StringID, Resource*>& mResourceById = getResourceMapFromType(Resource::ResourceType_Texture);
    ITF_MAP<StringID, Resource*>::iterator resIter;
    for (resIter = mResourceById.begin(); resIter != mResourceById.end(); ++resIter)
    {
        Resource* res = (Resource*)resIter->second;
        if (res->getType() == Resource::ResourceType_Texture)
        {
            if (count == _index)
            {
                return (Texture*)res;
            }
            count++;
        }
    }
    return NULL;
}


void ResourceManager::getAllTexture(ITF_VECTOR<Texture*> &mVTexture)
{
    // mVTexture.reserve(mResourcesByPath.size() / RESOURCE_TYPE_COUNT);
    ITF_MAP<StringID, Resource*>& mResourceById = getResourceMapFromType(Resource::ResourceType_Texture);
    ITF_MAP<StringID, Resource*>::iterator resIter;
    for (resIter = mResourceById.begin(); resIter != mResourceById.end(); ++resIter)
    {
        Resource* res = (Resource*)resIter->second;
        if (res->getType() == Resource::ResourceType_Texture)
        {
           mVTexture.push_back((Texture*)res);
        }
    }
}



#ifndef ITF_FINAL
ResourceManager::DebugMark* ResourceManager::findMark(Resource* _pResource, u32 _index)
{
    ITF_VECTOR<DebugMark>::iterator it = m_marks[_index].begin();
    ITF_VECTOR<DebugMark>::iterator end = m_marks[_index].end();
    for (; it != end; ++it)
    {
        DebugMark& mark = *it;
        if (mark.m_ptr == _pResource)
        {
            return &mark;
        }
    }
    return NULL;
}

ResourceManager::DebugMark* ResourceManager::findMark(const String8& _name, u32 _index)
{
    ITF_VECTOR<DebugMark>::iterator it = m_marks[_index].begin();
    ITF_VECTOR<DebugMark>::iterator end = m_marks[_index].end();
    for (; it != end; ++it)
    {
        DebugMark& mark = *it;
        if (mark.m_name == _name)
        {
            return &mark;
        }
    }
    return NULL;
}

void ResourceManager::removeFromMarks(Resource* _pResource)
{
    {
        ITF_VECTOR<DebugMark>::iterator it = m_marks[0].begin();
        ITF_VECTOR<DebugMark>::iterator end = m_marks[0].end();
        for (; it != end; ++it)
        {
            DebugMark& mark = *it;
            if (mark.m_ptr == _pResource)
            {
                mark.m_deleted = btrue;
                break;
            }
        }
    }

    {
        ITF_VECTOR<DebugMark>::iterator it = m_marks[1].begin();
        ITF_VECTOR<DebugMark>::iterator end = m_marks[1].end();
        for (; it != end; ++it)
        {
            DebugMark& mark = *it;
            if (mark.m_ptr == _pResource)
            {
                mark.m_deleted = btrue;
                break;
            }
        }
    }
}

void ResourceManager::setDebugMark()
{
    LOG("--------------- [START RESOURCEMANAGER DUMP] ------------");

    ITF_MAP<StringID, Resource*>::iterator  resIter;

    u32 prevMarkIndex = m_debugMarkIndex ^ 1;
    m_marks[m_debugMarkIndex].clear();
    u32 totalRes = 0;
    u32 recreatedRes = 0;
    u32 newRes = 0;
    for (u32 itResTable = 0; itResTable < Resource::ResourceType_Count; itResTable++)
    {
        ITF_MAP<StringID, Resource*>& mResourceById = mResourcesById[itResTable];
        for (resIter=mResourceById.begin() ; resIter!=mResourceById.end(); resIter++)
        {
            Resource* pResource = resIter->second;
            if( pResource )
            {
                totalRes++;
                DebugMark mark;
                mark.m_ptr = pResource;
                mark.m_name = pResource->getPath().getString().cStr();
                mark.m_deleted = bfalse;
                m_marks[m_debugMarkIndex].push_back(mark);

                DebugMark* found = findMark(mark.m_name, prevMarkIndex);
                if (m_marks[prevMarkIndex].size() && !found)
                {
                    newRes++;
                    LOG("NEW RESOURCE %s", mark.m_name.cStr());
                }
                else if (found && found->m_deleted)
                {
                    recreatedRes++;
                    LOG("DELETED AND RECREATED RESOURCE %s", mark.m_name.cStr());
                }
            }
        }
    }

    m_debugMarkIndex = prevMarkIndex;

    LOG("--------------- ResourceManager currently handles %d resources", totalRes);
    LOG("--------------- created %d new resources", newRes);
    LOG("--------------- deleted and recreated %d resources", recreatedRes);
    LOG("--------------- [END RESOURCEMANAGER DUMP] ------------");
}
#endif // ITF_FINAL



void ResourceManager::synchronizeResourcesToProcessFromMainThread()
{
    //
    Synchronize::enterCriticalSection(&m_csResourceToProcess);
    ITF_VECTOR<ResourceID> resourceToProcessFromMainThread = m_ResourceToProcessFromMainThread; //resource is physically loaded;
    m_ResourceToProcessFromMainThread.clear();
    Synchronize::leaveCriticalSection(&m_csResourceToProcess);

    //
    for (ITF_VECTOR<ResourceID>::iterator it = resourceToProcessFromMainThread.begin();it!=resourceToProcessFromMainThread.end();++it)
    {
        Resource* pResource = (*it).getResource();
        if (pResource)
        {
            pResource->onResourceProcessingFromMainThread();
        }
    }
}
void ResourceManager::synchronizeResourcesAsLoaded()
{
    //ITF_VECTOR<Resource*> copyDebug = m_ResourceAsLoaded; //HELP For debug
    {
        csAutoLock cs(m_csResourceAsLoaded);

        for (ITF_VECTOR<ResourceID>::iterator it = m_ResourceAsLoaded.begin();it!=m_ResourceAsLoaded.end();++it)
        {
            Resource* pResource = (*it).getResource();
            if (pResource)
            {
                ITF_ASSERT(!pResource->isPhysicallyLoaded());

                if (pResource->getPhysicalUsersCount() == 0) // if unloaded requested in the same frame
                {
                    requestResourcePhysicalRelease(pResource);
                    pResource->hijackPhysicalLoaded(btrue);
                    continue;
                }

                pResource->setPhysicalLoaded(btrue);

                //if (pResource->getType() == Resource::ResourceType_Animation)
                //{
                //    AnimScene* pAnimationScene = ((Animation*)pResource)->getOwnerData();
                //    ITF_ASSERT(pAnimationScene);
                //    if (pAnimationScene)
                //       ANIM_MANAGER->addScene(pAnimationScene);
                //}
            }
        }

        m_ResourceAsLoaded.clear();
    }
}


#ifdef ASSERT_ENABLED
bbool ResourceManager::checkIfResourceContainsTrackedResource(Resource* res)
{
    csAutoLock cs(m_csResourceQueryOps);
    const u16* traced = Resource::getTracedResourceName();

    ResourceGroup* grp = res->getGroup();
    if (traced != NULL && grp)
    {
        if (!grp->isEmpty())
        {
            const ResourceGroup::ResourceList& GroupResources = grp->getResourcesList();
            for (ResourceGroup::ResourceList::const_iterator it = GroupResources.begin() ; it != GroupResources.end(); ++it)
            {
                Resource *gres = (*it).getResource();
                if (gres)
                {
                    if(gres->getPath().getString().strstr(traced))
                        return btrue;
                    if (checkIfResourceContainsTrackedResource(gres))
                        return btrue;
                }
            }
        }
    }
    return bfalse;
}
#endif // ASSERT_ENABLED

void ResourceManager::addQueryOp(const ResourceID& _target, QueryOpType _op, const ResourceID& _secondArg)
{
#ifdef ASSERT_ENABLED
    Resource* res = _target.getResource();
    const u16* traced = Resource::getTracedResourceName();
    if (traced != NULL && res != NULL)
    {
        bbool viaGrp = checkIfResourceContainsTrackedResource(res);
        if (viaGrp || res->getPath().getString().strstr(traced))
        {
            switch (_op)
            {
            case QueryOp_AddPhysicClient:
                ITF_ASSERT_MSG(bfalse, "traced resource - QueryOp_AddPhysicClient");
                break;
            case QueryOp_RemovePhysicClient:
                ITF_ASSERT_MSG(bfalse, "traced resource - QueryOp_RemovePhysicClient");
                break;
            case QueryOp_AddLogicClient:
                ITF_ASSERT_MSG(bfalse, "traced resource - QueryOp_AddLogicClient");
                break;
            case QueryOp_RemoveLogicClient:
                {
                    ITF_ASSERT_MSG(bfalse, "traced resource - QueryOp_RemoveLogicClient");
                    i32 removeQueryCount = 1;
                    for (u32 i = 0; i<m_queryRemoveOps.size(); i++)
                    {
                        if (m_queryRemoveOps[i].m_target == res && m_queryRemoveOps[i].m_op == QueryOp_RemoveLogicClient)
                        {
                            removeQueryCount++;
                        }
                    }
                    for (u32 i = 0; i<m_queryAddOps.size(); i++)
                    {
                        if (m_queryAddOps[i].m_target == res && m_queryAddOps[i].m_op == QueryOp_AddLogicClient)
                        {
                            removeQueryCount--;
                        }
                    }
                    ITF_ASSERT_MSG((i32)res->m_logicalDataRefCount >= (i32)removeQueryCount, "want to destroy twice a resource");
                    if (removeQueryCount > 0)
                    {
                        ITF_ASSERT_MSG(!isInDestoyList(res), "want to destroy twice a resource");
                    }
                }
                break;
            case QueryOp_AddResourceToGroup:
                ITF_ASSERT_MSG(bfalse, "traced resource - QueryOp_AddResourceToGroup");
                break;
            default:
                ITF_ASSERT_MSG(bfalse, "traced resource - Unknown QueryOp");
                break;
            }
        }
    }
    if (QueryOp_AddResourceToGroup == _op)
    {
        Resource* res2 = _secondArg.getResource();
        if (res2)
        {
            if (res2->getPath().getString().strstr(traced))
            {
                ITF_ASSERT_MSG(bfalse, "traced resource added to group - QueryOp_AddResourceToGroup");
            }
        }
    }

#endif
    if (m_bImmediateFlushMode)
    {
        executeQueryOp(_target.getResource(), _op, _secondArg.getResource());
    }
    else
    {
        // add critical section on the thread !!
        csAutoLock cs(m_csResourceQueryOps);

        switch (_op)
        {
        case QueryOp_AddPhysicClient:
        case QueryOp_AddLogicClient:
        case QueryOp_AddResourceToGroup:
        {
            // CS CHECK
            //bbool isCPC = isCSAdd;
            //if (isCPC)
            //{
            //    bbool multiAccess = btrue;
            //}
            QueryAddOp& localOp = m_queryAddOps.incrSize();
            localOp.m_target = _target.getResource();
            localOp.m_op = _op;
            localOp.m_secondaryArg = _secondArg.getResource();
        }
            break;
        case QueryOp_RemovePhysicClient:
        case QueryOp_RemoveLogicClient:
        {
            // CS CHECK
            //bbool isCPC = isCSRemove;
            //if (isCPC)
            //{
            //    bbool multiAccess = btrue;
            //}
            QueryRemoveOp& localOp = m_queryRemoveOps.incrSize();
            localOp.m_target = _target.getResource();
            localOp.m_op = _op;
        }
            break;
        default:
            ITF_ASSERT_MSG(bfalse, "Unknown QueryOp");
            break;
        }
    }
}

bbool ResourceManager::isValidResource(const Resource* pRes)
{
    for (u32 itResTable = 0; itResTable < Resource::ResourceType_Count; itResTable++)
    {
        const ITF_MAP<StringID, Resource*>& mResourceById = mResourcesById[itResTable];
        ITF_MAP<StringID, Resource*>::const_iterator resIter = mResourceById.begin();
        ITF_MAP<StringID, Resource*>::const_iterator end = mResourceById.end();
        for (; resIter!=end; ++resIter)
        {
            Resource* pResource = resIter->second;
            if( pResource && pResource == pRes)
                return btrue;
        }
    }
    return bfalse;
}

bbool ResourceManager::isValidResource(const Resource::ResourceType _type, const Resource* pRes)
{
    const ITF_MAP<StringID, Resource*>& mResourceById = getResourceMapFromType(_type);
    ITF_MAP<StringID, Resource*>::const_iterator resIter = mResourceById.begin();
    ITF_MAP<StringID, Resource*>::const_iterator end = mResourceById.end();
    for (; resIter!=end; ++resIter)
    {
        Resource* pResource = resIter->second;
        if( pResource && pResource == pRes)
            return btrue;
    }
     return bfalse;
}



//Called on the main thread;
void ResourceManager::synchronize()
{
    ITF_ASSERT(Synchronize::getCurrentThreadId() == ThreadSettings::m_settings[eThreadId_mainThread].m_threadID);

   updateQueryOps();

    static ITF_VECTOR<ResourceID> vAsync;
    vAsync.clear();

    {
        csAutoLock  cs(m_csResourceAsyncAdded);
        vAsync = m_vResourceAsync;
        m_vResourceAsync.clear();
    }

    //from asynchronous scene loading
    {
        csAutoLock  cs(m_csResourceAsyncSceneAdded);
        vAsync.insert(vAsync.end(),m_vResourceAsyncFromScene.begin(),m_vResourceAsyncFromScene.end());
        m_vResourceAsyncFromScene.clear();
    }

    //TODO could use a double buffering to the Critical Section usage
    for (ITF_VECTOR<ResourceID>::iterator it = vAsync.begin();it!=vAsync.end();++it)
    {
        requestFileLoadingInternal(*it);
    }



   //if the element reaches 0 reference make sure ,cancel the pending loading
   {
       ITF_VECTOR<ResourceID>::iterator iter = m_resourcesToLoad.begin();
       while (iter!= m_resourcesToLoad.end())   // m_resourcesToLoad.end() MUST be re-evaluated inside the loop because we are erasing elements
       {
           Resource* res = (*iter).getResource();
           ITF_ASSERT(res);
           if (!res || res->getPhysicalUsersCount() == 0)
           {
               iter =  m_resourcesToLoad.erase(iter);
           }
           else
               ++iter;
       }
   }

   synchronizeResourcesToProcessFromMainThread();
   synchronizeResourcesAsLoaded();

   //m_resourcesToLoad is only in main thread,we don't need to protect it
   if (m_resourcesToLoad.size() && !m_paused)
   {
       //tell to the asynchronous thread that there is a new resource available..
       {
           csAutoLock cs(m_csResourceCurrentLoaded);
           ITF_MemoryBarrier();//make sure that the asynchronous thread flush its memory
           if (!m_ResourceLoadingPending.size())
           {
               m_ResourceLoadingPending.reserve(m_resourcesToLoad.size());

                ITF_VECTOR<ResourceID> mLoadList = m_resourcesToLoad;
                ITF_VECTOR<ResourceID>::iterator it = mLoadList.begin();

                for (; it != mLoadList.end();++it)
                {
                    const ResourceID& rid = (*it);
                    Resource* res = rid.getResource();
                    ITF_ASSERT(res);
                    if (res)
                    {
                        if (res->isDeleteMe())
                        {
                            csAutoLock cs(m_csResourceCreation);

                            updateQueryOpsAddLogical();

                            if (res->getLogicalRefCount() == 0)
                            {
                                RESOURCE_MANAGER->unregisterResource(res->getID());
                                callResourceDestructor(res);

                                continue;
                            } else
                            {
                                res->setDeleteMe(bfalse);
                            }
                        }

                        if (!res->isPhysicallyLoaded())
                        {
                            #ifdef DEVELOPER_FRED
                                ITF_ASSERT_MSG(!res->isPending(), "adding already pending resource --> can lead to crash");
                                ITF_ASSERT_MSG(std::find(m_ResourceLoadingPending.begin(),m_ResourceLoadingPending.end(), *it) == m_ResourceLoadingPending.end(), "adding already pending resource --> can lead to crash");
                            #endif
                            res->setPending(btrue);
                            //check if the resource must cooked
                            m_ResourceLoadingPending.push_back(*it);
                        }
                    }
                }

                m_resourcesToLoad.clear();

                Synchronize::setEvent(&m_activeloadEvent);
           }
       }
   }

   manageDestruction(bfalse, m_deleteLogicalAllowed);

   callResourceCallBack();

}

void ResourceManager::removeFromDestoyList(const Resource* _res)
{
    ITF_ASSERT(Synchronize::getCurrentThreadId() == ThreadSettings::m_settings[eThreadId_mainThread].m_threadID);
    {
        ITF_VECTOR<ResourceID>::iterator iter = m_vResourceForDestroyLogical.begin();
        ITF_VECTOR<ResourceID>::iterator end = m_vResourceForDestroyLogical.end();
        for (; iter != end; ++iter)
        {
            if (*iter == _res->getID())
            {
                m_vResourceForDestroyLogical.erase(iter);
                break;
            }
        }
    }
    {
        ITF_VECTOR<ResourceID>::iterator iter = m_vResourceForDestroy.begin();
        ITF_VECTOR<ResourceID>::iterator end = m_vResourceForDestroy.end();
        for (; iter != end; ++iter)
        {
            if (*iter == _res->getID())
            {
                m_vResourceForDestroy.erase(iter);
                break;
            }
        }
    }
}

bbool ResourceManager::isInDestoyList(const Resource* _res)
{
    ITF_ASSERT(Synchronize::getCurrentThreadId() == ThreadSettings::m_settings[eThreadId_mainThread].m_threadID);
    {
        ITF_VECTOR<ResourceID>::iterator iter = m_vResourceForDestroyLogical.begin();
        ITF_VECTOR<ResourceID>::iterator end = m_vResourceForDestroyLogical.end();
        for (; iter != end; ++iter)
        {
            if (*iter == _res->getID())
                return btrue;
        }
    }
    {
        ITF_VECTOR<ResourceID>::iterator iter = m_vResourceForDestroy.begin();
        ITF_VECTOR<ResourceID>::iterator end = m_vResourceForDestroy.end();
        for (; iter != end; ++iter)
        {
            if (*iter == _res->getID())
                return btrue;
        }
    }
    return bfalse;
}

void ResourceManager::manageDestruction( bbool _shutdown, bbool _deleteLogicalAllowed )
{
    ITF_ASSERT(Synchronize::getCurrentThreadId() == ThreadSettings::m_settings[eThreadId_mainThread].m_threadID);

    //avoid resource insertion in m_vResourceForDestroy while parsing a physical resource destruction
    ITF_VECTOR<ResourceID> vResourceToDestroy = m_vResourceForDestroy;
    m_vResourceForDestroy.clear();

    ITF_VECTOR<ResourceID>::iterator iter = vResourceToDestroy.begin();
    while (iter!=vResourceToDestroy.end())
    {
        const ResourceID& pResID = (*iter);
        Resource* pResource = pResID.getResource();

        if (pResource)
        {
            ITF_ASSERT(!pResource->m_canBeFreed);

            //Finally we decide to keep the resource,someone ask a bind in the life time
            if (pResource->getPhysicalUsersCount() !=0)
               iter=vResourceToDestroy.erase(iter);
            else //Life time expired and no bind anymore ;remove the physical resource
                if (pResource->decreaseLifeTime() == 0 || _shutdown)
                {
                    pResource->flushPhysical();

                    iter=vResourceToDestroy.erase(iter);
                }
                else //try next step
                    ++iter;
        }
        else
        {
            iter=vResourceToDestroy.erase(iter);
        }
    }
    m_vResourceForDestroy.insert(m_vResourceForDestroy.end(),vResourceToDestroy.begin(),vResourceToDestroy.end());

    // Do not manage logical destruction when scene is loading
    if (!_shutdown && !_deleteLogicalAllowed)
        return;

    {
        csAutoLock cs(m_csResourceCreation);


        vResourceToDestroy = m_vResourceForDestroyLogical;
        m_vResourceForDestroyLogical.clear();

        if (vResourceToDestroy.size())
        {
            // Resource creation can add new resource between
            // Use also a cs ...
            updateQueryOpsAddLogical();
        }

        iter = vResourceToDestroy.begin();
        while (iter!=vResourceToDestroy.end())
        {
            const ResourceID& pResID = (*iter);
            Resource* pResource = pResID.getResource();

            if (pResource)
            {
                ITF_ASSERT(!pResource->m_canBeFreed);
                if ( pResource->getLogicalRefCount() > 0 )
                {
                    // someone is using again this resource, skip destruction
                    iter=vResourceToDestroy.erase(iter);
                }
                else if (pResource->decreaseLifeTime() == 0 || _shutdown)
                {
                    if ( pResource->isPhysicallyLoaded() )
                    {
                        pResource->flushPhysical();
                    }

                    RESOURCE_MANAGER->unregisterResource(pResource->getID());
                    callResourceDestructor(pResource);
                    iter=vResourceToDestroy.erase(iter);
                }
                else //try next step
                {
                    ++iter;
                }
            }
            else
            {
                iter=vResourceToDestroy.erase(iter);
            }
        }
        m_vResourceForDestroyLogical.insert(m_vResourceForDestroyLogical.end(),vResourceToDestroy.begin(),vResourceToDestroy.end());
    }
}


void ResourceManager::requestFileLoading(const ResourceID& _res)
{
    ITF_ASSERT(!_res.getResource()->isPhysicallyLoaded());
#ifndef ITF_IPAD
    if (Synchronize::getCurrentThreadId() == ThreadSettings::m_settings[eThreadId_ResourceManager].m_threadID) //we are in the asynchronous resource thread
    {
        {
            csAutoLock  cs(m_csResourceAsyncAdded);
            m_vResourceAsync.push_back(_res);
        }
    }
    else
    if (Synchronize::getCurrentThreadId() == ThreadSettings::m_settings[eThreadId_SceneManager].m_threadID) //we are in the asynchronous scene thread
    {
        {
            csAutoLock  cs(m_csResourceAsyncSceneAdded);
            m_vResourceAsyncFromScene.push_back(_res);
        }
    }
    else
    {
       requestFileLoadingInternal(_res);
    }
#else
	requestFileLoadingInternal(_res);
#endif
}


void ResourceManager::asyncLoad()
{
    registerLocalThreadId(asyncresourceThread);

    m_exitedAsync = bfalse;
    m_stopAsync = bfalse;

    if (!m_loadBundleDynamically)
    {
        FILEMANAGER->loadBundles();
    }

    while (!m_stopAsync)  //the wait event is inside updateCurrentResourcePending;
    {
        ThreadSettings::threadStartFrame();
        updateCurrentResourcePending();
    }

    if (!m_loadBundleDynamically)
    {
        FILEMANAGER->unloadBundles();
    }

    if (m_stopAsync)
    {
        Synchronize::setEvent(&m_exitEvent);
        ITF_MemoryBarrier();
        m_exitedAsync = btrue;
    }

}

void ResourceManager::requestStopAsync()
{
    if (!m_pAsyncThread)
        return;

    if (m_bEnableAsync)
    {
        m_stopAsync = btrue;
        ITF_MemoryBarrier();

        Synchronize::setEvent(&m_activeloadEvent);
    }
}


void ResourceManager::stopAsync()
{
	if (!m_pAsyncThread)
		return;

    if (m_bEnableAsync)
    {
        m_stopAsync = btrue;
        ITF_MemoryBarrier();

        Synchronize::setEvent(&m_activeloadEvent);
        Synchronize::waitEvent(&m_exitEvent);
    }
}


u32 WINAPI ResourceManager_Run(void* user)
{
    ResourceManager* resourceManager = (ResourceManager*)(user);

    resourceManager->asyncLoad();
    return 0;
}

void ResourceManager::launch()
{
    m_pAsyncThread = THREADMANAGER->createThread(ResourceManager_Run, this, ThreadSettings::m_settings[eThreadId_ResourceManager]);
}

bbool ResourceManager::loadResourcesPending()
{
    ITF_VECTOR<ResourceID> resourceCopy  = m_resourcesToLoad;
    m_resourcesToLoad.clear();
    ITF_VECTOR<ResourceID>::iterator it = resourceCopy.begin();
    for (; it != resourceCopy.end(); ++it)
    {
        Resource* pResource = (*it).getResource();
        ITF_ASSERT(pResource);
        //right now we skip all resource not existing until multithreading is done
        if (pResource)
        {
            bbool exists = pResource->isPhysicalExist();
            if (!exists)
            {
                addError(*it, ResourceError::ResourceError_FileDoesNotExist);
            }
            else
            {
                #ifdef DEVELOPER_FRED
                    ITF_ASSERT_MSG(!pResource->isBeingLoaded(), "duplicate resource loading can lead to crashes");
                #endif
                //if (!pResource->isBeingLoaded())
                {
                    if (!pResource->tryLoadFile())
                        addError(*it, ResourceError::ResourceError_TryLoadFileFailed);
                }
            }
        }
    }


   //btrue is finish
   return (m_resourcesToLoad.size()==0);
}

bbool ResourceManager::forceSynchronousLoading(const Resource::ResourceType _type)
{
    bbool somethingChanged=bfalse;

    for (int resourceIndex=0; resourceIndex<(int)m_resourcesToLoad.size(); resourceIndex++)
    {
        Resource* pResource = m_resourcesToLoad[resourceIndex].getResource();
        //right now we skip all resource not existing until multithreading is done
        bbool deleteEntry = bfalse;
        if (!pResource)
        {
            deleteEntry=btrue;
        }
        else if (pResource->getType() == _type)
        {
            if (pResource->isPhysicallyLoaded())
                deleteEntry = btrue;
            else
            {
                if (!pResource->isPhysicalExist())
                {
                    addError(pResource->getID(), ResourceError::ResourceError_FileDoesNotExist);
                    deleteEntry=btrue;
                }
                else
                {
                    #ifdef DEVELOPER_FRED
                        ITF_ASSERT_MSG(!pResource->isBeingLoaded(), "duplicate resource loading can lead to crashes");
                    #endif
                    //if (!pResource->isBeingLoaded())
                    {
                        deleteEntry = pResource->tryLoadFile();
                        if (!deleteEntry)
                            addError(pResource->getID(), ResourceError::ResourceError_TryLoadFileFailed);
                    }
                }
            }
        }
        if (deleteEntry)
        {
            m_resourcesToLoad.erase(m_resourcesToLoad.begin()+resourceIndex);
            somethingChanged = btrue;
            RESOURCE_MANAGER->synchronizeResourcesAsLoaded();
            RESOURCE_MANAGER->synchronizeResourcesToProcessFromMainThread();

            resourceIndex--;
        }
    }
    return somethingChanged;
}

void ResourceManager::forceResourceAnimSkeletonLoading(ResourceID _res,const String& _platform) // only for specific resources in specific conditions !!!
{
    Resource* pResource = _res.getResource();
    ITF_UNUSED(pResource);
    ITF_ASSERT(pResource && pResource->getType() == Resource::ResourceType_AnimSkeleton);

    AnimSkeletonResource* pAnimSkeletonResource =static_cast<AnimSkeletonResource*> (_res.getResource());
    if (pAnimSkeletonResource && !pAnimSkeletonResource->isPhysicallyLoaded())
    {
        if (!pAnimSkeletonResource->tryLoadFileByPlatform(_platform))
            addError(_res, ResourceError::ResourceError_TryLoadFileFailed);
        else
        {
            synchronizeResourcesAsLoaded();
            synchronizeResourcesToProcessFromMainThread();
        }
    }
}

void ResourceManager::callResourceCallBack()
{
    //callbacks
    VResourceLoadCallback::iterator resIter;
    m_CBtoSend.clear();

    //synchronize with loadscene/and resource callback

    for (u32 index = 0;index<maxThread;++index)
    {
        csAutoLock cs(m_csResourceCallbackThread[index]);
        m_resourceLoadCallbacks.insert(m_resourceLoadCallbacks.end(),m_resourceLoadCallbacksThread[index].begin(),
                                       m_resourceLoadCallbacksThread[index].end());
        m_resourceLoadCallbacksThread[index].clear();
    }



    for (resIter=m_resourceLoadCallbacks.begin() ; resIter!=m_resourceLoadCallbacks.end() ; )
    {

        for (ITF_VECTOR<ResourceID>::iterator it = resIter->resList.begin(); it != resIter->resList.end();)
        {
            Resource* pResource = (*it).getResource();
            if (pResource && pResource->isPhysicallyLoaded())
            {

                it = resIter->resList.erase(it);
            }
            else
            {
                ++it;
            }
        }

        if (resIter->resList.size() == 0)
        {
            m_CBtoSend.push_back(*resIter);
            resIter = m_resourceLoadCallbacks.erase(resIter);
        }
        else
        {
            ++resIter;
        }
    }


    for ( VResourceLoadCallback::iterator resSend=m_CBtoSend.begin() ; resSend!= m_CBtoSend.end() ;++resSend )
    {
         resSend->cb(resSend->context);
    }

}

void    ResourceManager::executeQueryOp(Resource* _res, QueryOpType _op, Resource* _secondArg)
{
    ITF_ASSERT(Synchronize::getCurrentThreadId() == ThreadSettings::m_settings[eThreadId_mainThread].m_threadID);

    if (!_res || _res->m_canBeFreed)
    {
        ITF_ASSERT(_op != QueryOp_AddLogicClient);
        ITF_ASSERT(_op != QueryOp_AddResourceToGroup);
        return;
    }

    switch (_op)
    {
    case QueryOp_AddPhysicClient:
        _res->addPhysicalDataUser();
        break;

    case QueryOp_RemovePhysicClient:
            {
                _res->removePhysicalDataUser();
                if (_res->getPhysicalUsersCount() == 0)
                {
                    ITF_VECTOR<ResourceID> ::iterator iter = std::find(m_resourcesToLoad.begin(),m_resourcesToLoad.end(), _res->getID());

                    if (iter != m_resourcesToLoad.end())
                        m_resourcesToLoad.erase(iter);
                }
            }
        break;
    case QueryOp_RemoveLogicClient:
        {
            if (_res->getLogicalRefCount() > 1) // no true delete, no need for logical release bigger process
                Resource::removeLogicalDataUser(_res);
            else
                 deleteResource(_res);
        }
        break;
    case QueryOp_AddLogicClient:
        _res->addLogicalDataUser();
        break;

    case QueryOp_AddResourceToGroup:
        {
            if (Resource::ResourceType_ResourceGroup != _res->getType())
            {
                ITF_ASSERT_MSG(bfalse, "adding a resource to something that is not a group");
                return;
            }
            ResourceGroup* grp = (ResourceGroup*)_res;
            if (!_secondArg || _secondArg->m_canBeFreed)
            {
                ITF_ASSERT_MSG(bfalse, "adding unknown or deleted resource to group");
                return;
            }
            if (!grp->addResource(_secondArg, bfalse))
            {
                // Resource already present in the group
                // We must REMOVE one logical count because
                // when coming here we add a logical to an unreferenced resource
                // -> DONE inside add resource
                queryRemoveUserToResourceLogicalData(_secondArg);
            }
        }

        break;

    default:
        ITF_ASSERT_MSG(bfalse, "unhandled case");
        break;
    }
}

void ResourceManager::updateQueryOps(QueryUpdateType _type)
{
#ifndef ITF_FINAL
    if (triggerDebugMark)
    {
        triggerDebugMark = 0;
        setDebugMark();
    }
#endif
    SafeArray<QueryAddOp> queryAddOps;
    SafeArray<QueryRemoveOp> queryRemoveOps;

    static const u32 maxOpsInARow = 51200;
    bbool addOpsWereTruncated = bfalse;

    csAutoLock cs(m_csResourceQueryOps);

    if (QueryUpdate_RemoveOnly != _type)
    {
        // CS CHECK
        //isCSAdd = btrue;
        u32 size = m_queryAddOps.size();
        if (size <= maxOpsInARow)
        {
            if(size > 0)
                queryAddOps.buildFrom(m_queryAddOps, 0, size);
            m_queryAddOps.clear();
            if (m_queryAddOps.capacity() > m_defaultQuerryOpTableSize)
                m_queryAddOps.setCapacity(m_defaultQuerryOpTableSize);
        }
        else
        {
            addOpsWereTruncated = btrue;
            queryAddOps.clear();
            queryAddOps.reserve(maxOpsInARow);
            QueryAddOp* pWrite = &queryAddOps.incrSize(maxOpsInARow);
            for (u32 i=0; i<maxOpsInARow; i++)
            {
                *pWrite++ = m_queryAddOps[size-1-i];
            }
            m_queryAddOps.resize(size-maxOpsInARow);
        }
        // CS CHECK
        //Sleep(1);
        //isCSAdd = bfalse;
    }

    bbool executeRemoveQueries = (QueryUpdate_AddOnly != _type);
    if (QueryUpdate_All == _type)
        executeRemoveQueries = !addOpsWereTruncated;

    if (executeRemoveQueries)
    {
        // CS CHECK
        //isCSRemove = btrue;

        u32 size = m_queryRemoveOps.size();
        if (size <= maxOpsInARow)
        {
            if(size > 0)
                queryRemoveOps.buildFrom(m_queryRemoveOps, 0, size);
            m_queryRemoveOps.clear();
            if (m_queryRemoveOps.capacity() > m_defaultQuerryOpTableSize)
                m_queryRemoveOps.setCapacity(m_defaultQuerryOpTableSize);
        }
        else
        {
            queryRemoveOps.clear();
            queryRemoveOps.reserve(maxOpsInARow);
            QueryRemoveOp* pWrite = &queryRemoveOps.incrSize(maxOpsInARow);
            for (u32 i=0; i<maxOpsInARow; i++)
            {
                *pWrite++ = m_queryRemoveOps[size-1-i];
            }
            m_queryRemoveOps.resize(size-maxOpsInARow);
        }
        // CS CHECK
        //Sleep(1);
        //isCSRemove = bfalse;
    }

    ITF_ASSERT_MSG((QueryUpdate_RemoveOnly != _type) || (!m_queryAddOps.size()), "Don't solve remove querries while add querries are still pending");

    #define TREAT_ADD_OP(table, opName)\
    {\
        u32 arSize = table.size();\
        for (u32 arIt = 0; arIt < arSize; arIt++)\
        {\
            const QueryAddOp& arOp = table[arIt];\
            if (opName == arOp.m_op)\
            executeQueryOp(arOp.m_target, opName, arOp.m_secondaryArg);\
        }\
    }

    #define TREAT_REMOVE_OP(table, opName)\
    {\
        u32 arSize = table.size();\
        for (u32 arIt = 0; arIt < arSize; arIt++)\
        {\
            const QueryRemoveOp& arOp = table[arIt];\
            if (opName == arOp.m_op)\
            executeQueryOp(arOp.m_target, opName);\
            ITF_ASSERT_MSG((QueryUpdate_RemoveOnly != _type) || (!m_queryAddOps.size()), "How the f#ck can remove querries ADD resources?");\
        }\
    }

    if (QueryUpdate_RemoveOnly != _type)
    {
        TREAT_ADD_OP(queryAddOps, QueryOp_AddResourceToGroup)
        TREAT_ADD_OP(queryAddOps, QueryOp_AddLogicClient)
        TREAT_ADD_OP(queryAddOps, QueryOp_AddPhysicClient)
        if (addOpsWereTruncated)
            return; // don't treat remove ops if all add ops were not achieved
    }
    if (QueryUpdate_AddOnly != _type)
    {
        TREAT_REMOVE_OP(queryRemoveOps, QueryOp_RemovePhysicClient)
        TREAT_REMOVE_OP(queryRemoveOps, QueryOp_RemoveLogicClient)
    }
}

void ResourceManager::updateQueryOpsAddLogical()
{
    csAutoLock cs(m_csResourceQueryOps);

    for (u32 it = 0; it < m_queryAddOps.size();)
    {
        const QueryAddOp& op = m_queryAddOps[it];
        if (QueryOp_AddLogicClient == op.m_op)
        {
            executeQueryOp(op.m_target, QueryOp_AddLogicClient, op.m_secondaryArg);
            m_queryAddOps.eraseKeepOrder(it);
        }
        else
            ++it;
        }
    }

void ResourceManager::requestFileLoadingInternal( const ResourceID& _id )
{
    Resource* _res = _id.getResource();
    ITF_ASSERT(_res);
    if (!_res)
        return;

#ifdef ASSERT_ENABLED
#ifdef TRACE_RESOURCE
    {
        StringConverter strpath(_res->getPath());
        ITF_ASSERT_MSG(!strstr(strpath.getChar(), TRACE_RESOURCE), "traced resource - ResourceManager::requestFileLoadingInternal");
    }
#endif // TRACE_RESOURCE
#endif // ASSERT_ENABLED


    ITF_ASSERT(!_res->isPhysicallyLoaded());
    ITF_ASSERT(!_res->isBeingLoaded());

    ITF_ASSERT(Synchronize::getCurrentThreadId() == ThreadSettings::m_settings[eThreadId_mainThread].m_threadID);

    ITF_VECTOR<ResourceID>::iterator iter = std::find(m_resourcesToLoad.begin(), m_resourcesToLoad.end(), _id);
    if (iter != m_resourcesToLoad.end())
        return;

    ITF_ASSERT_MSG(std::find(m_ResourceLoadingPending.begin(), m_ResourceLoadingPending.end(), _id) == m_ResourceLoadingPending.end(), "adding already pending resource --> can lead to crash - ignore if cooking");

    m_resourcesToLoad.push_back(_id);
}

//--------------------------------------------------------------------------------------------------------------------------------------------


void sortResourcesLoad(ITF_VECTOR<ResourceID>& dst_resourceID,const ITF_VECTOR<ResourceID>& src_resourceID)
{
    //sort this array following the bundle position;
    const u32 resourceCount = src_resourceID.size();

    ITF_VECTOR<FilesPosition> _pathList;
    _pathList.reserve(resourceCount);

    dst_resourceID.reserve(resourceCount);

    //create a path list

    FilesPosition fp;
    for (ITF_VECTOR<ResourceID>::const_iterator iter = src_resourceID.begin();iter!=src_resourceID.end();iter++)
    {
        fp.m_Position = 0;
        fp.m_resourceID = (*iter);
        fp.m_fileSize = 0;

        _pathList.push_back(fp);
    }

    BUNDLEMANAGER->orderLoad(_pathList);

    //resend to the destination
#if !defined( ITF_FINAL ) && !defined( ITF_NO_SEEK_LOG )
    u64 nextPos = _pathList.begin()->m_Position;
    ITF_VECTOR< Path > range;
    ITF_VECTOR<FilesPosition>::const_iterator prev;
#endif //ITF_FINAL

    for (ITF_VECTOR<FilesPosition>::const_iterator iter = _pathList.begin();iter!=_pathList.end();iter++)
    {
#if !defined( ITF_FINAL ) && !defined( ITF_NO_SEEK_LOG )
        if ( iter->m_Position != nextPos )
        {
            prev = iter - 1;
            if ( prev->m_file == iter->m_file )
            {
                LOG( "Seeking %ld bytes from %s", iter->m_Position - ( prev->m_Position + prev->m_fileSize ), prev->m_resourceID.getPath()->getString8().cStr() );
                prev->m_file->getFilesFromRange( prev->m_Position + prev->m_fileSize, iter->m_Position, range );
                for ( u32 i = 0; i < range.size(); ++i )
                {
                    LOG( "  Skipping %s%s", range[i].getString8().cStr(), iter->m_resourceID.isValidResourceId() && iter->m_resourceID.getResource()->isPhysicallyLoaded() ? " already loaded" : "" );
                }
                range.clear();
            }
        }
        nextPos = iter->m_Position + iter->m_fileSize;
#endif //ITF_FINAL
       //LOG("Resource > %d|%ld %ls"  ,(*iter).m_bundleKey,(*iter).m_Position,(*iter).m_resourceID.getPath()->getString().cStr());
       dst_resourceID.push_back((*iter).m_resourceID);
    }

}

//--------------------------------------------------------------------------------------------------------------------------------------------

//One resource by frame is set to be loaded;
void ResourceManager::updateCurrentResourcePending()
{
    Synchronize::waitEvent(&m_activeloadEvent);

#if !defined (ITF_FINAL) //&& defined(ITF_X360)
    f32  timeStart = static_cast<f32>(SYSTEM_ADAPTER->getTime());
#endif //(ITF_FINAL) //&& defined(ITF_X360)

    if (m_loadBundleDynamically)
    {
        FILEMANAGER->loadBundles();
    }

    ITF_VECTOR<ResourceID> resourceLoadingPending;

    ITF_MemoryBarrier();
    {
       csAutoLock cs(m_csResourceCurrentLoaded);

       m_useLinearMode = (BUNDLEMANAGER->getRegistredBundleCount()>0); //under test

       if (isLinearMode())
       {
           sortResourcesLoad(resourceLoadingPending,m_ResourceLoadingPending);
       }
       else
       {
           resourceLoadingPending = m_ResourceLoadingPending;
       }

       SETCURRENTCOUNDLOAD(resourceLoadingPending.size());
    }

    PathString_t pathName;

    for (ITF_VECTOR<ResourceID>::iterator iterResource = resourceLoadingPending.begin();iterResource!=resourceLoadingPending.end();++iterResource)
    {
         Resource* pResource = (*iterResource).getResource();
         if (!pResource || pResource->isDeleteMe())
         {
            continue;
         }

        // LOG("Resource loaded >  %ls"  ,(pResource)->getPath().getString().cStr());

    #ifdef ITF_SUPPORT_COOKING

	    if (!isModeFinal())
	    {
		     if (pResource->mustBeCooked() && !pResource->fileCookAvailable())
		     {
                 pResource->getPath().getString(pathName);

			     bbool bRes = cookNeeded(pathName);
			     if (bRes)
			     {
                    if (!m_bDisableDefaultResourceUsage)
                    {
				        ResourceID defaultID = m_ResourceDefaultID[pResource->getType()];
				        if (defaultID.isValidResourceId())
						    pResource->setUseDefault(defaultID.getResource());
                    }

				     requestCooking(pathName);
			     }
                 else
                 {
                    if( !FILEMANAGER->fileExists(pathName))
                    {
                        addError( *iterResource, ResourceError::ResourceError_FileDoesNotExist);
                    }
                 }
		     }
	    }

	    if (m_DelayToLoad)
		    sleep(m_DelayToLoad); //if you want to simulate a delay

         f64 rs = SYSTEM_ADAPTER->getTime();
    #endif //!ITF_SUPPORT_COOKING

         //resource all ready loaded...
         //ITF_ASSERT(!pResource->isPhysicallyLoaded());
         if (pResource->getPhysicalUsersCount()) // !!!! NOT THREAD SAFE !!! Main thread can be adding or removing users at the same time////
         {
             bbool fileExists = pResource->isPhysicalExist();

             if (fileExists)
             {
                //ITF_ASSERT_MSG(!pResource->isPhysicallyLoaded(), "duplicate resource loading of %s, can lead to crashes",
                //               pResource->getPath().getString8().cStr());
                ITF_ASSERT_MSG(pResource->isPending(), "only pending resources should arrive here");

                bbool fileLoaded = pResource->isPhysicallyLoaded();
                // $GS: load once
                if (!fileLoaded)
                    fileLoaded = pResource->tryLoadFile();

                if (fileLoaded)
                {
            #ifdef ITF_SUPPORT_COOKING
                     f32 fTimeLoaded = (f32)(SYSTEM_ADAPTER->getTime()-rs);
                     //char t[255];
                     //sprintf(t,"Time to load %ls -- %0.02f ms\n",pResource->getPath().cStr(),fTimeLoaded*1000.0f);
                     //OutputDebugStringA(t);
                     pResource->setTimeToLoad(fTimeLoaded);
            #endif //!ITF_FINAL
                 }
                 else
                    addError(pResource->getID(), ResourceError::ResourceError_TryLoadFileFailed);
             }
             else
                addError(pResource->getID(), ResourceError::ResourceError_FileDoesNotExist);
         }

         ADDCURRENTCOUNDLOAD(-1);
         pResource->setPending(bfalse);
    }

    {
        csAutoLock cs(m_csResourceCurrentLoaded);
        m_ResourceLoadingPending.clear();//allow to fill with the new resource array
        SETCURRENTCOUNDLOAD(0)


    #if !defined (ITF_FINAL) //&& defined(ITF_X360)
        f32  delta = static_cast<f32>(SYSTEM_ADAPTER->getTime())-timeStart;
        LOG("Resource Streamed in %0.02f ms",delta*1000);
    #endif //(ITF_FINAL) //&& defined(ITF_X360)

        Synchronize::resetEvent(&m_activeloadEvent);
    }

    if (m_loadBundleDynamically)
    {
        FILEMANAGER->unloadBundles();
    }
}

bbool ResourceManager::hasResourcesToLoad() const
{
    return m_ResourceToProcessFromMainThread.size()!=0
        || m_ResourceLoadingPending.size()!= 0
        || m_resourcesToLoad.size()!=0;
}

void ResourceManager::addResourceAsLoaded(Resource* pResource)
{
     csAutoLock cs(m_csResourceAsLoaded);
     m_ResourceAsLoaded.push_back(pResource->getID());
}

void ResourceManager::addResourceToProcessFromMainThread(Resource* pResource)
{
    csAutoLock cs(m_csResourceToProcess);
    m_ResourceToProcessFromMainThread.push_back(pResource->getID());
}

void ResourceManager::updateFilesToLoad()
{
    ITF_VECTOR<ResourceID>::iterator it = m_resourcesToLoad.begin();
    for (; it != m_resourcesToLoad.end();)
    {
        Resource* pRes = (*it).getResource();

        //right now we skip all resource not existing until multithreading is done
        bbool erase = bfalse;
        if (!pRes)
            erase = btrue;
        else
        {
            if (!pRes->isPhysicalExist())
            {
                addError(*it, ResourceError::ResourceError_FileDoesNotExist);
                erase = btrue;
            }
            else
            {
                #ifdef DEVELOPER_FRED
                    ITF_ASSERT_MSG(!pRes->isBeingLoaded(), "duplicate resource loading can lead to crashes");
                #endif
                //if (!pRes->isBeingLoaded())
                 {
                     if (pRes->tryLoadFile())
                        erase = btrue;
                    else
                        addError(*it, ResourceError::ResourceError_TryLoadFileFailed);
                 }
            }
        }
        if (erase)
            it = m_resourcesToLoad.erase(it);
        else
            it++;
    }
}


void ResourceManager::updateAsyncFilesToLoad()
{
    // We want to process all pending resourceToLoad
    // Because of threading, we have to wait that no pending resources
    // are loading event if this would make the application freeze
    // -> Only done on hotreload for the moment
    do
    {
        //tell to the asynchronous thread that there is a new resource available..
        {
            csAutoLock cs(m_csResourceCurrentLoaded);
            ITF_MemoryBarrier();//make sure that the asynchronous thread flush its memory
            if (!m_ResourceLoadingPending.size())
            {
                updateFilesToLoad();
            }
        }
        if (m_resourcesToLoad.size())
            sleep(1);
    } while(m_resourcesToLoad.size());
}

void ResourceManager::updateFiles()
{
    if (m_bEnableAsync)
    {
        RESOURCE_MANAGER->updateAsyncFilesToLoad();
    } else
    {
        RESOURCE_MANAGER->updateFilesToLoad();
    }
}


void ResourceManager::registerResourceGroupLoadCallback(const ResourceID& _group, void * _context, resourceLoadCallback _cb)
{
    ITF_VECTOR<ResourceID> resList;
    resList.push_back(_group);
    registerResourceLoadCallback(resList,_context,_cb);
}

bbool   ResourceManager::addUserToResourcePhysicalData ( const ResourceID& _resID )
{
    queryAddUserToResourcePhysicalData(_resID);
    return btrue;
}

void    ResourceManager::removeUserToResourcePhysicalData ( const ResourceID& _resID )
{
    queryRemoveUserToResourcePhysicalData(_resID);
}


void ResourceManager::flushResourcesForDestroy()
{
    ITF_ASSERT(Synchronize::getCurrentThreadId() == ThreadSettings::m_settings[eThreadId_mainThread].m_threadID);

    ITF_VECTOR<ResourceID>::iterator iter = m_vResourceForDestroy.begin();
    ITF_VECTOR<ResourceID>::iterator end = m_vResourceForDestroy.end();

    for (; iter != end; ++iter)
    {
        const ResourceID& pResID = (*iter);
        Resource* pResource = pResID.getResource();

        if (pResource)
        {
            LOG_FRED("[FLUSH_RESOURCE] %s", pResource->getPath().getString8().cStr());
            pResource->setTimeLifeDestroy(0);
            pResource->flushPhysical();
        }
    }
    m_vResourceForDestroy.clear();
}

void ResourceManager::requestResourcePhysicalRelease(Resource* _res)
{
    ITF_ASSERT(Synchronize::getCurrentThreadId() == ThreadSettings::m_settings[eThreadId_mainThread].m_threadID);

    //flush the resource right away
    if (m_bImmediateFlushMode || !_res->delayPhysicalFlush())
    {
        _res->flushPhysical();
    }
    else
    {
        _res->setTimeLifeDestroy(resourcePhysicalLifeTime);
        if (!_res->m_canBeFreed) // the rule is that m_canBeFreed is set to true just before calling delete
        {
            bbool isDoublon = false;
            for (u32 i = 0; i < m_vResourceForDestroy.size(); i++)
            {
                if (m_vResourceForDestroy[i] == _res->getID())
                {
                    ITF_ASSERT_MSG(bfalse, "duplicate deletion forces this for() loop that wastes CPU cycles for nothing...");
                    isDoublon = true;
                    break;
                }
            }
            if (!isDoublon)
                m_vResourceForDestroy.push_back(_res->getID());
        }
    }
}

void ResourceManager::requestResourceLogicalRelease(Resource* _res)
{
    //flush the resource right away
    if (m_bImmediateFlushMode || !_res->delayPhysicalFlush())
    {
        csAutoLock cs(m_csResourceCreation);

        if (_res->isPhysicallyLoaded())
        {
            _res->flushPhysical();
        }

        RESOURCE_MANAGER->unregisterResource(_res->getID());

        ITF_ASSERT_CRASH(Synchronize::getCurrentThreadId() == ThreadSettings::m_settings[eThreadId_mainThread].m_threadID, "Must be done in main thread!");
        callResourceDestructor(_res);
    }
    else
    {
        _res->setTimeLifeDestroy(RESOURCE_LOGICAL_DEFAULTLIFETIME);
        if (!_res->m_canBeFreed) // the rule is that m_canBeFreed is set to true just before calling delete
        {
            bbool isDoublon = false;
            for (u32 i = 0; i < m_vResourceForDestroyLogical.size(); i++)
            {
                if (m_vResourceForDestroyLogical[i] == _res->getID())
                {
                  //  ITF_ASSERT_MSG(bfalse, "duplicate deletion forces this for() loop that wastes CPU cycles for nothing...");
                    isDoublon = true;
                    break;
                }
            }
            if (!isDoublon)
                m_vResourceForDestroyLogical.push_back(_res->getID());
        }
    }
}

/*
void ResourceManager::removeUserToResourceLogicalData( const ResourceID& _resID )
{
    Resource* res = _resID.getResource();
    ITF_ASSERT(res);
    if (res)
        removeUserToResourceLogicalData(res);
}

void ResourceManager::removeUserToResourceLogicalData(Resource* _res)
{
    if (_res->getLogicalRefCount() > 1) // no true delete, no need for logical release bigger process
        Resource::removeLogicalDataUser(_res);
    else
         deleteResource(_res);
}



bbool   ResourceManager::addUserToResourcePhysicalData ( Resource* _res )
{
    ITF_ASSERT_MSG(Synchronize::getCurrentThreadId() == ThreadSettings::m_settings[eThreadId_mainThread].m_threadID, "outside of main thread : call queryAddUserToResourcePhysicalData instead");
    if (!_res)
        return bfalse;
    _res->addPhysicalDataUser();
    return btrue;
}

void    ResourceManager::removeUserToResourcePhysicalData ( Resource* _res )
{
    ITF_ASSERT_MSG(Synchronize::getCurrentThreadId() == ThreadSettings::m_settings[eThreadId_mainThread].m_threadID, "outside of main thread : call queryRemoveUserToResourcePhysicalData instead");
    if (!_res)
        return;
    _res->removePhysicalDataUser();

    //if the element reaches 0 reference make sure ,cancel the pending loading
    //TODO fix the case where the resource is loading
    if (_res->getPhysicalUsersCount() == 0)
    {
        ITF_VECTOR<ResourceID> ::iterator iter = std::find(m_resourcesToLoad.begin(),m_resourcesToLoad.end(),_res->getID());

        if (iter != m_resourcesToLoad.end())
            m_resourcesToLoad.erase(iter);
    }
}
*/

void ResourceManager::removeUserToResourceLogicalData( const ResourceID& _resID )
{
    queryRemoveUserToResourceLogicalData(_resID);
}

void ResourceManager::removeUserToResourceLogicalData(Resource* _res)
{
    queryRemoveUserToResourceLogicalData(_res);
}



bbool   ResourceManager::addUserToResourcePhysicalData ( Resource* _res )
{
    queryAddUserToResourcePhysicalData(_res);
    return btrue;
}

void    ResourceManager::removeUserToResourcePhysicalData ( Resource* _res )
{
    queryRemoveUserToResourcePhysicalData(_res);
}

void    ResourceManager::queryAddResourceToGroup( const ResourceID& _groupID, const ResourceID& _resID )
{
    addQueryOp(_groupID, QueryOp_AddResourceToGroup, _resID);
}

void ResourceManager::queryAddUserToResourcePhysicalData ( const ResourceID& _resID )
{
    addQueryOp(_resID, QueryOp_AddPhysicClient);
}

void   ResourceManager::queryAddUserToResourcePhysicalData ( Resource* _res )
{
    queryAddUserToResourcePhysicalData(_res->getID());
}


void    ResourceManager::queryRemoveUserToResourcePhysicalData ( const ResourceID& _resID )
{
    addQueryOp(_resID, QueryOp_RemovePhysicClient);
}

void    ResourceManager::queryRemoveUserToResourcePhysicalData ( Resource* _res )
{
    queryRemoveUserToResourcePhysicalData(_res->getID());
}


void ResourceManager::queryAddUserToResourceLogicalData ( const ResourceID& _resID )
{
    addQueryOp(_resID, QueryOp_AddLogicClient);
}

void   ResourceManager::queryAddUserToResourceLogicalData ( Resource* _res )
{
    queryAddUserToResourceLogicalData(_res->getID());
}


void    ResourceManager::queryRemoveUserToResourceLogicalData ( const ResourceID& _resID )
{
    addQueryOp(_resID, QueryOp_RemoveLogicClient);
}

void    ResourceManager::queryRemoveUserToResourceLogicalData ( Resource* _res )
{
    queryRemoveUserToResourceLogicalData(_res->getID());
}

void ResourceManager::removeUserToResourceData(const ResourceID& _resID)
{
    if (!_resID.isValidResourceId())
        return;

    Resource* res = _resID.getResource();
    ITF_ASSERT(res);
    removeUserToResourceData(res);
}

void ResourceManager::removeUserToResourceData(Resource* _res)
{
    removeUserToResourcePhysicalData(_res);
    removeUserToResourceLogicalData(_res);
}


//------------------------------------------------------------------------------------------------------------
void ResourceManager::deleteResource(Resource *_res)
{
    if (_res->getLogicalRefCount() ) // if refcount bigger than one the delete will just -- the count
    {
        Resource::removeLogicalDataUser(_res);
        return;
    }

    ITF_ASSERT(0); // Must not Happen
}


void ResourceManager::pause()
{
    LOG("ResourceManager::pause()");
    InterlockedIncrement( &m_paused );
}

void ResourceManager::unpause()
{
    LOG("ResourceManager::unpause()");
    ITF_ASSERT_CRASH( m_paused > 0, "Pause counter is going to be negative" );
    InterlockedDecrement( &m_paused );
}

void ResourceManager::unregisterResource( const ResourceID & _rID )
{
    // -> Critical section must be locked by called !
    // csAutoLock cs(m_csResourceCreation);

    ResourceID  rID  = _rID; // the resource ID can be a member of the resource we want to delete
                             // to prevent that, we copy the ResourceId in another object
    Resource*   pRes = rID.getResource();
    ITF_ASSERT_MSG(pRes, "trying to unregister deleted resource");
    if (!pRes)
        return;

    {
        ITF_MAP<StringID, Resource*>& mResourceById = getResourceMapFromType(pRes->getType());
        ITF_MAP<StringID, Resource*>::iterator it = mResourceById.find(pRes->getPath().getStringID());
        if(it == mResourceById.end())
            return;
        mResourceById.erase(it);
    }


    {
        ITF_VECTOR<ResourceID>::iterator iter = std::find(m_resourcesToLoad.begin(), m_resourcesToLoad.end(), rID);
        if (iter != m_resourcesToLoad.end())
            m_resourcesToLoad.erase(iter);
    }

    {
        VResourceLoadCallback ::iterator it = m_resourceLoadCallbacks.begin();
        VResourceLoadCallback ::iterator itend = m_resourceLoadCallbacks.end();
        while(it != itend)
        {
            ResourceLoadCallbackData& _current = *it;

            ITF_VECTOR<ResourceID>::iterator it2 = _current.resList.begin();
            ITF_VECTOR<ResourceID>::iterator it2end = _current.resList.end();
            while(it2 != it2end)
            {
                if(*it2 == pRes->getID())
                {
                    _current.resList.erase(it2);
                    break;
                }
                ++it2;
            }

            ++it;
        }
    }
}

//------------------------------------------------------------------------------------------------------------

#ifndef ITF_FINAL

void ResourceManager::dumpActivity(ITF_VECTOR<String>& strings)
{
	ITF_VECTOR<Resource*> mVresources;

	strings.reserve( Resource::ResourceType_Count);

	u32  MemUsed[Resource::ResourceType_Count];
	ITF_MemSet(&MemUsed,0,sizeof(MemUsed));

	String tmp;
	for (u32 itResTable = 0; itResTable < Resource::ResourceType_Count; itResTable++)
	{
		ITF_MAP<StringID, Resource*>& mResourceById = mResourcesById[itResTable];
		for (ITF_MAP<StringID, Resource*>::iterator iter = mResourceById.begin(); iter != mResourceById.end() ;++iter)
		{
			Resource* pResource = iter->second;
			MemUsed[itResTable]+= pResource->getMemoryUsage();
		}

		tmp.setTextFormat("%d",MemUsed[itResTable]);
		strings.push_back(tmp);
	}






}

void ResourceManager::dumpActivity(Blob& _blob,Resource::ResourceType _resourcetype,bbool bDisplayCooked,bbool bDisplayNotCooked)
{
    ITF_VECTOR<Resource*> mVresources;

    u32 countResourcesGlobal = 0;
    for (u32 itResTable = 0; itResTable < Resource::ResourceType_Count; itResTable++)
    {
        ITF_MAP<StringID, Resource*>& mResourceById = mResourcesById[itResTable];
        for (ITF_MAP<StringID, Resource*>::iterator iter = mResourceById.begin(); iter != mResourceById.end() ;++iter)
        {
            Resource* pResource = iter->second;
            countResourcesGlobal++;
            if (_resourcetype == Resource::ResourceType_All || _resourcetype == pResource->getType())
            {
                bool atleastDisplay = (bDisplayCooked || bDisplayNotCooked);
                if (atleastDisplay && ((pResource->isCooked() == bDisplayCooked) || (bDisplayNotCooked != pResource->isCooked())))
                {
                    mVresources.push_back( pResource);
                }
            }
        }
    }

    _blob.pushUInt32(mVresources.size());

    for (ITF_VECTOR<Resource*>::iterator iter = mVresources.begin() ; iter != mVresources.end() ; ++iter)
    {
        (*iter)->dumpInfo(_blob);
    }

    //TEXTURE MEMORY
    u32 textureMemory = getMemoryUsage(Resource::ResourceType_Texture);
    _blob.pushUInt32(textureMemory);
    _blob.pushUInt32(countResourcesGlobal);

}


u32 ResourceManager::getMemoryUsage(Resource::ResourceType type)
{
    u32 memoryUsage = 0;

    ITF_MAP<StringID, Resource*>& mResourceById = getResourceMapFromType(type);
    for (ITF_MAP<StringID, Resource*>::iterator iter = mResourceById.begin() ;
        iter != mResourceById.end() ;
        ++iter)
    {
        Resource* pResource = iter->second;
        if (pResource->getType() == type)
        {
            memoryUsage += pResource->getMemoryUsage();
        }
    }

    return memoryUsage;
}

#endif //!ITF_FINAL

#ifdef MODE_HRESTEXTURE_SUPPORTED
bbool ResourceManager::cookHResNeeded(const String& _filename)
{
    if (m_cookNeededFunc)
        return m_cookNeededFunc(_filename,btrue);

#ifdef ITF_WINDOWS
    ITF_ASSERT_MSG(bfalse, "don't call this before m_cookNeededFunc is set!");
#endif
    return bfalse;
}

void ResourceManager::cookHRes(const String& _filename)
{
    if (m_cookFuncHRes)
        m_cookFuncHRes(_filename);
}

#endif //MODE_HRESTEXTURE_SUPPORTED

bbool ResourceManager::cookNeeded(const String& _filename)
{
    if (m_cookNeededFunc)
        return m_cookNeededFunc(_filename,bfalse);

#ifdef ITF_WINDOWS
    ITF_ASSERT_MSG(bfalse, "don't call this before m_cookNeededFunc is set!");
#endif
    return bfalse;
}

void ResourceManager::requestCooking(const String& _filename)
{
    if (m_cookFunc)
        m_cookFunc(_filename);
}

void ResourceManager::cleanPostDeletedSingletons()
{
    if (GFX_ADAPTER)
    {
        GFX_ADAPTER->destroyFonts();
    }
}

void ResourceManager::flushPendingOps()
{
    ITF_ASSERT(Synchronize::getCurrentThreadId() == ThreadSettings::m_settings[eThreadId_mainThread].m_threadID);

    while (m_queryAddOps.size())
        updateQueryOps(QueryUpdate_AddOnly);

    while (m_queryRemoveOps.size())
        updateQueryOps(QueryUpdate_RemoveOnly);

    ITF_ASSERT_MSG(!m_queryAddOps.size(), "How the f#ck can remove querries ADD resources?");
}

void ResourceManager::setImmediateFlushMode(bbool _bImmediateFlushMode)
{
    m_bImmediateFlushMode = _bImmediateFlushMode;
    if (_bImmediateFlushMode)
    {
        flushPendingOps();
    }
}

const char* ResourceManager::ResourceError::getErrorLabel() const
{
    switch(m_type)
    {
    case ResourceError_FileDoesNotExist:
        return "file doesn't exist: ";
        break;

    case ResourceError_TryLoadFileFailed:
        return "error in file: ";
        break;

    default:
        return "unknown error. file: ";
        break;
    }
}

void ResourceManager::addError(const ResourceID& _id, ResourceError::ResourceErrorType _type)
{
    csAutoLock cs(m_csResourceError);
    m_errors.push_back(ResourceError(_id, _type));
    LOG("Error %s with resource %s",
        _type == ResourceError::ResourceError_FileDoesNotExist ? "'file not found'" : "'read failed'",
        _id.getDebugString().cStr());
}


void ResourceManager::getErrors(ITF_VECTOR<ResourceError>& _dest, bbool _flush)
{
    csAutoLock cs(m_csResourceError);
    _dest = m_errors;
    if (_flush)
        m_errors.clear();
}

void ResourceManager::flushErrors()
{
    csAutoLock cs(m_csResourceError);
    m_errors.clear();
}

bool ResourceManager::PrefetchResource( const Path& path, PrefetchFactory::Params& params, Resource::ResourceType type )
{
    ResourceID id;
    csAutoLock cs( RESOURCE_MANAGER->getQueryOpsCs() );

    params.type = type;
    if ( !RESOURCE_MANAGER->getResourceIdFromFile( id, path.getStringID(), type ) )
    {
        const ResourceID& newId = RESOURCE_MANAGER->newResourceIDFromFile( type, path );

        RESOURCE_MANAGER->queryAddUserToResourcePhysicalData( newId );
        params.res = newId.getResource();
        params.refType = PrefetchFactory::Params::Logical | PrefetchFactory::Params::Physical;
        return btrue;
    }
    params.res = id.getResource();
    return btrue;
}

#ifdef ASSERT_ENABLED
bbool ResourceManager::canResourceBeDeleted(const ResourceID _id)
{
    const Resource* pRes = _id.getResource();
    if (!pRes)
        return btrue;
    const StringID& sid = pRes->getPath().getStringID();
    for (u32 itResTable = 0; itResTable < Resource::ResourceType_Count; itResTable++) // don't retrieve resource type here: we are called from its destructor
    {
        ITF_MAP<StringID, Resource*>& mResourceById = mResourcesById[itResTable];
        if (mResourceById.find(sid) != mResourceById.end())
            return bfalse;
    }
    return btrue;
}
#endif // ASSERT_ENABLED


Resource* ResourceManager::getResourceByPath_slow(const StringID _path)
{
    ResourceID resourcePathID;
    if (getResourceIdFromFile(resourcePathID, _path, Resource::ResourceType_All))
    {
        Resource* pRes = resourcePathID.getResource();
        return pRes;
    }
    return NULL;
}

void    ResourceManager::callResourceDestructor(Resource *_res)
{
    ITF_ASSERT(Synchronize::getCurrentThreadId() == ThreadSettings::m_settings[eThreadId_mainThread].m_threadID);
    ITF_ASSERT_MSG(!_res->m_canBeFreed, "double deletion = bad bad bad bad...");

    csAutoLock cs(m_csResourceQueryOps);

    {
        for (u32 it = 0; it < m_queryAddOps.size();)
        {
            const QueryAddOp& op = m_queryAddOps[it];
            if (op.m_target == _res)
            {
                ITF_ASSERT_MSG(bfalse, "deleting resource with pending ops on it");
                m_queryAddOps.eraseKeepOrder(it);
            }
            else
                ++it;
        }
    }
    {
        for (u32 it = 0; it < m_queryRemoveOps.size();)
        {
            const QueryRemoveOp& op = m_queryRemoveOps[it];
            if (op.m_target == _res)
            {
                ITF_ASSERT_MSG(bfalse, "deleting resource with pending ops on it");
                m_queryRemoveOps.eraseKeepOrder(it);
            }
            else
                ++it;
        }
    }


    removeFromDestoyList(_res);
    _res->m_canBeFreed = 1;

#ifndef ITF_FINAL
    removeFromMarks(_res);
#endif // ITF_FINAL

    delete _res;
}

bbool ResourceManager::isResourceLoadPending(const Resource* pRes) const
{
    ITF_ASSERT(Synchronize::getCurrentThreadId() == ThreadSettings::m_settings[eThreadId_mainThread].m_threadID);

    ITF_VECTOR<ResourceID>::const_iterator iter = std::find(m_resourcesToLoad.begin(), m_resourcesToLoad.end(), pRes->getID());
    if (iter != m_resourcesToLoad.end())
        return btrue;

    ITF_VECTOR<ResourceID>::const_iterator iter2 = std::find(m_ResourceLoadingPending.begin(), m_ResourceLoadingPending.end(), pRes->getID());
    if (iter2 != m_ResourceLoadingPending.end())
        return btrue;

    return bfalse;
}

} // namespace ITF
