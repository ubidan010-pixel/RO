#include "precompiled_engine.h"

#ifndef _ITF_RESOURCE_H_   
#include "engine/resources/Resource.h"
#endif //_ITF_RESOURCE_H_   

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_ID_SERVER_H_
#include "core/IDserver.h"
#endif //_ITF_ID_SERVER_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifdef ITF_WII
#ifndef __MEM_MACROS_WII_H__
#include "core/memory/WII/MEM_macros_WII.h"
#endif //__MEM_MACROS_WII_H__
#endif

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Resource)

ITF_VECTOR<ResourceListener*> Resource::m_listeners;

ResourceID ResourceID::Invalid;

#ifdef RESOURCE_IS_BASEOBJECT
    const ObjectRef& ResourceID::getResourceRef() const {return m_resource;}
#else
    Resource* ResourceID::getResourceRef() const {return m_resource;}
#endif //RESOURCE_IS_BASEOBJECT

//may should have its own class
#ifdef ENABLE_RESOURCE_HISTORY
ResourceHistoryManager Resource::resourceHistoryManager;

ResourceHistoryManager::ResourceHistoryManager()
{
    Synchronize::createCriticalSection(&m_cs);
};

ResourceHistoryManager::~ResourceHistoryManager()
{
    Synchronize::destroyCriticalSection(&m_cs);
}

void ResourceHistoryManager::dumpResourceHistory(ObjectRef _or)
{
    csAutoLock cs(m_cs);
#ifdef ITF_WINDOWS
    {
        u32 frame = CURRENTFRAME;
        FILE* pFic = fopen("c:/ITF_ResourceHistory.txt", "wt");
        if (pFic)
        {
            const ITF_VECTOR<ResourceHistory>& history = getResourcesHistory();
            ITF_VECTOR<ResourceHistory>::const_iterator it = history.begin();
            ITF_VECTOR<ResourceHistory>::const_iterator end = history.end();
            fprintf(pFic, "\n--- Looking for Resource History, ObjectRef: %d at frame %d---\n", _or, frame);
            for (; it != end; ++it)
            {
                const ResourceHistory& h = *it;
                if (h.m_resourceRef == _or)
                {
                    switch (h.m_opType)
                    {
                    case ResourceHistoryOp_CreateResource :
                        fprintf(pFic, "CreateResource %s at frame %d\n", h.m_path.cStr(), h.m_opTime);
                        break;
                    case ResourceHistoryOp_DeleteResource :
                        fprintf(pFic, "DeleteResource %s at frame %d\n", h.m_path.cStr(), h.m_opTime);
                        break;
                    default:
                        break;
                    }
                }
            }
            fprintf(pFic, "\n--- End of Resource History, now dumping full history---\n\n\n");
            it = history.begin();
            for (; it != end; ++it)
            {
                const ResourceHistory& h = *it;
                switch (h.m_opType)
                {
                case ResourceHistoryOp_CreateResource :
                    fprintf(pFic, "CreateResource %s at frame %d\n", h.m_path.cStr(), h.m_opTime);
                    break;
                case ResourceHistoryOp_DeleteResource :
                    fprintf(pFic, "DeleteResource %s at frame %d\n", h.m_path.cStr(), h.m_opTime);
                    break;
                default:
                    break;
                }
            }
            fclose(pFic);
        }
    }
#endif ITF_WINDOWS
}


void ResourceHistoryManager::addResourceHistory(const ResourceHistory& _resourceHistory)
{
    csAutoLock cs(m_cs);
    if (m_history.size() < 1000)
        m_history.push_back(_resourceHistory);

}

#endif //ENABLE_RESOURCE_HISTORY


const Path* ResourceID::getPath() const
{
    if (getResource())
        return &getResource()->getPath();
    return NULL;
}

void ResourceID::invalidateResourceId()
{
    InvalidateResourceIdentifier(m_resource);
}

bbool ResourceID::isValidResourceId()const 
{
    #ifdef RESOURCE_IS_BASEOBJECT
        return m_resource.isValid();
    #else
        return (NULL != m_resource);
    #endif //RESOURCE_IS_BASEOBJECT          
}

String8 ResourceID::getDebugString() const
{
    const Path* path = getPath();
    if (!path)
    {
        String8 res;
        res.setTextFormat("0x%016zX", uPtr(m_resource));
        return res;
    }

    PathString8_t pathFilename;
    path->getString(pathFilename);
    return pathFilename;
}

void ResourceID::serialize( ArchiveMemory& _archive )
{
    #ifdef RESOURCE_IS_BASEOBJECT
        _archive.serialize(m_resource.m_directValue);
    #else
        _archive.serialize((u32&)m_resource);
    #endif //RESOURCE_IS_BASEOBJECT
}


void Resource::keepOnlyOneLogicalUser()
{
    while(m_physicalDataRefCount)
        removePhysicalDataUser();

    while(m_logicalDataRefCount > 1)
        removeLogicalDataUser(this);
}

void Resource::forceDeletion(Resource *_res)
{
    _res->keepOnlyOneLogicalUser();
    removeLogicalDataUser(_res);
}


Resource::Resource(const Path &path) : constructResourceParent() m_path(path), m_physicalDataRefCount(0), m_logicalDataRefCount(1),
                                        m_bPhysicalLoaded(bfalse),m_TimeLifeBeforeDestroy(0),m_isCooked(bfalse),m_bDeleteMe(0),m_bPending(0), m_canBeFreed(0), m_isStatic(0), m_isSubResource(0)

#ifndef ITF_FINAL
                                        ,m_memoryUsage(0),m_fTimeToLoad(0.0f)
#endif //!ITF_FINAL
#ifdef ITF_SUPPORT_COOKING
                                        , m_bUseAsShared(bfalse)
#endif
{
#if defined _DEBUG && defined (ITF_WINDOWS)
    u16* ptrdoubleSlsh = (u16*)wcsstr((wchar_t*) m_path.getString().cStr(),L"//");
    ITF_ASSERT(ptrdoubleSlsh == NULL);
#endif //_DEBUG
 
    ITF_ASSERT(getPath().isEmpty() || FilePath::isFilePath(getPath().getString()));
    ITF_ASSERT_MSG(!m_path.getString().strstr(getTracedResourceName()), "traced resource - constructor");
#ifdef ASSERT_ENABLED
    m_reloading = bfalse;
#endif

    #ifdef RESOURCE_IS_BASEOBJECT
        setObjectType(BaseObject::eRessource);
       m_ID.m_resource = getRef();
    #else
        m_ID.m_resource = this;
    #endif //RESOURCE_IS_BASEOBJECT
/*
#ifdef ENABLE_RESOURCE_HISTORY
    if (ResourceHistoryManager::instance())
    {
        ResourceHistory record;
        record.m_opType = ResourceHistoryOp_CreateResource;
        record.m_opTime = CURRENTFRAME;
        record.m_resourceRef = getRef();
        record.m_resourcePointer = this;
        record.m_path = path.cStr();

        ResourceHistoryManager::instance()->addResourceHistory(record);
    }
#endif // ENABLE_RESOURCE_HISTORY
*/

}


Resource::Resource(const String8& _ID) : constructResourceParent() m_path(_ID.cStr()), m_physicalDataRefCount(0), m_logicalDataRefCount(1), m_bPhysicalLoaded(bfalse),
                                        m_TimeLifeBeforeDestroy(0),m_isCooked(bfalse),m_bDeleteMe(0),m_bPending(0), m_canBeFreed(0), m_isStatic(0), m_isSubResource(0)
#ifndef ITF_FINAL
                                        ,m_memoryUsage(0),m_fTimeToLoad(0.0f)
#endif //!ITF_FINAL
#ifdef ITF_SUPPORT_COOKING
                                        , m_bUseAsShared(bfalse)
#endif
{
    ITF_ASSERT_MSG(!m_path.getString().strstr(getTracedResourceName()), "traced resource - constructor");
    ITF_ASSERT(FilePath::isFilePath(getPath().getString()));
#ifdef ASSERT_ENABLED
    m_reloading = bfalse;
#endif

    #ifdef RESOURCE_IS_BASEOBJECT
        setObjectType(BaseObject::eRessource);
        m_ID.m_resource = getRef();
    #else
        m_ID.m_resource = this;
    #endif //RESOURCE_IS_BASEOBJECT

    /*
#ifdef ENABLE_RESOURCE_HISTORY
    if (ResourceHistoryManager::instance())
    {
        ResourceHistory record;
        record.m_opType = ResourceHistoryOp_CreateResource;
        record.m_opTime = CURRENTFRAME;
        record.m_resourceRef = getRef();
        record.m_resourcePointer = this;
        record.m_path = _ID;
        ResourceHistoryManager::instance()->addResourceHistory(record);
    }

#endif // ENABLE_RESOURCE_HISTORY
    */
}


Resource::Resource(const char* _ID) : constructResourceParent() m_path(_ID), m_physicalDataRefCount(0), m_logicalDataRefCount(1), m_bPhysicalLoaded(bfalse),
                                        m_TimeLifeBeforeDestroy(0),m_isCooked(bfalse),m_bDeleteMe(0),m_bPending(0), m_canBeFreed(0), m_isStatic(0), m_isSubResource(0)
#ifndef ITF_FINAL
                                        ,m_memoryUsage(0),m_fTimeToLoad(0.0f)
#endif //!ITF_FINAL
#ifdef ITF_SUPPORT_COOKING
                                        , m_bUseAsShared(bfalse)
#endif

{
    ITF_ASSERT_MSG(!m_path.getString().strstr(getTracedResourceName()), "traced resource - constructor");
    ITF_ASSERT(FilePath::isFilePath(getPath().getString()));
#ifdef ASSERT_ENABLED
    m_reloading = bfalse;
#endif

    #ifdef RESOURCE_IS_BASEOBJECT
        setObjectType(BaseObject::eRessource);
        m_ID.m_resource = getRef();
    #else
        m_ID.m_resource = this;
    #endif //RESOURCE_IS_BASEOBJECT

    /*
#ifdef ENABLE_RESOURCE_HISTORY
    if (ResourceHistoryManager::instance())
    {
        ResourceHistory record;
        record.m_opType = ResourceHistoryOp_CreateResource;
        record.m_opTime = CURRENTFRAME;
        record.m_resourceRef = getRef();
        record.m_resourcePointer = this;
        record.m_path = _ID;
        ResourceHistoryManager::instance()->addResourceHistory(record);
    }
#endif // ENABLE_RESOURCE_HISTORY
    */
}

//---------------------------------------------------------------------------------------------------------------------

Resource::~Resource()
{
#ifdef ASSERT_ENABLED
    ITF_ASSERT_MSG(!m_path.getString().strstr(getTracedResourceName()), "traced resource - destructor");
    ITF_ASSERT(RESOURCE_MANAGER->canResourceBeDeleted(getID())); // Call RESOURCE_MANAGER->unregisterResource(id) before delete
    ITF_ASSERT(!m_physicalDataRefCount);
    ITF_ASSERT(!m_logicalDataRefCount);
#endif
/*
#ifdef ENABLE_RESOURCE_HISTORY
    if (ResourceHistoryManager::instance())
    {
        ResourceHistory record;
        record.m_opType = ResourceHistoryOp_DeleteResource;
        record.m_opTime = CURRENTFRAME;
        record.m_resourceRef = getRef();
        record.m_resourcePointer = this;
        record.m_path = m_path.getString().cStr();
        ResourceHistoryManager::instance()->addResourceHistory(record);
    }
#endif // ENABLE_RESOURCE_HISTORY
    */
    ITF_ASSERT_CRASH(0 == m_logicalDataRefCount, "Resource is destroyed while is still has users");
    ITF_ASSERT(m_canBeFreed);
}

void Resource::addListener(ResourceListener* _listener)
{
    ITF_ASSERT(_listener);
#ifdef ITF_WII
    MEM_M_PushExt(MEM_C_MEM1_ALLOC);
#endif
    m_listeners.push_back(_listener);
#ifdef ITF_WII
    MEM_M_PopExt();
#endif
}

bbool   Resource::isPhysicallyLoaded()
{
    if (m_bPhysicalLoaded && getGroup())
        return getGroup()->isPhysicallyLoaded();

    return m_bPhysicalLoaded;
}


void    Resource::setPhysicalLoaded(bbool _bPhysicalLoaded)
{
    //You must in the main thread to set the texture as available...
    //Disable until a way is found to load lua in no async mode
    //ITF_ASSERT(Synchronize::getCurrentThreadId() == THREADMANAGER->getMainThreadId());
    ITF_ASSERT_MSG(!_bPhysicalLoaded || getPhysicalUsersCount() > 0, "hijacked physical ready !!! ignore if cooking");
    ITF_ASSERT_MSG(_bPhysicalLoaded || getPhysicalUsersCount() == 0, "hijacked physical ready !!! ignore if cooking");
    ITF_ASSERT_MSG(!m_path.getString().strstr(getTracedResourceName()), "traced resource - physical ready");
        
    m_bPhysicalLoaded = _bPhysicalLoaded;
};

bbool Resource::addPhysicalDataUser()
{
    ITF_ASSERT(Synchronize::getCurrentThreadId() == ThreadSettings::m_settings[eThreadId_mainThread].m_threadID);
  //  ITF_ASSERT(m_logicalDataRefCount >= m_physicalDataRefCount);

    ITF_ASSERT_MSG(!m_path.getString().strstr(getTracedResourceName()), "traced resource - add phys user");
    bbool success = btrue;
    if (m_physicalDataRefCount == 0)
    {
#ifdef DEVELOPER_FRED
        ITF_ASSERT_MSG(!isPending(), "loading a resource that is already being loaded");
#endif
        load();
    }

    m_physicalDataRefCount++;

    return success;
}

void Resource::reload()
{
    ITF_ASSERT_MSG(!m_path.getString().strstr(getTracedResourceName()), "traced resource - reload");
    ITF_ASSERT(Synchronize::getCurrentThreadId() == ThreadSettings::m_settings[eThreadId_mainThread].m_threadID);

    if (m_physicalDataRefCount > 0)
    {
#ifdef ASSERT_ENABLED
        m_reloading = btrue;
#endif

        // Synchronization must be done BEFORE reloading !

        flushPhysical();
        load();

        // Update of resource manager must Be done AFTER reload -> not inside

#ifdef ASSERT_ENABLED
        m_reloading = bfalse;
#endif
    }
}

void Resource::removePhysicalDataUser()
{
    if (m_isStatic)
        return;

    ITF_ASSERT(Synchronize::getCurrentThreadId() == ThreadSettings::m_settings[eThreadId_mainThread].m_threadID);

    ITF_ASSERT_MSG(!m_path.getString().strstr(getTracedResourceName()), "traced resource - remove phys user");
    if (m_physicalDataRefCount == 0)
    {
        ITF_ASSERT(!isPhysicallyLoaded());
        return;
    }

    ITF_ASSERT(m_physicalDataRefCount>0);

    m_physicalDataRefCount--;
    if (m_physicalDataRefCount <= 0)
    {
#ifdef DEVELOPER_FRED
        ITF_ASSERT_MSG(!isPending(), "unloading a resource that is being loaded");
#endif
        unload();
    }
}

void Resource::removeLogicalDataUser    ( Resource * _res )
{
    ITF_ASSERT(Synchronize::getCurrentThreadId() == ThreadSettings::m_settings[eThreadId_mainThread].m_threadID);

    ITF_ASSERT(_res);
    if (!_res)
        return;
    if (_res->isStatic())
        return;

    ITF_ASSERT_MSG(!_res->m_path.getString().strstr(getTracedResourceName()), "traced resource - remove logic user");
    ITF_ASSERT(_res->m_logicalDataRefCount > 0);
    if (_res->m_logicalDataRefCount > 0)
        _res->m_logicalDataRefCount--;

    ITF_ASSERT(_res->m_logicalDataRefCount >= _res->m_physicalDataRefCount);
    if (_res->m_logicalDataRefCount == 0)
    {
        ITF_ASSERT(Synchronize::getCurrentThreadId() == ThreadSettings::m_settings[eThreadId_mainThread].m_threadID);
        if (_res->isPending())
        {
            _res->setDeleteMe(1);
        }
        else
        {
            RESOURCE_MANAGER->requestResourceLogicalRelease(_res);
        }
    }
}


void Resource::flushPhysical()
{
    ITF_ASSERT_MSG(!m_path.getString().strstr(getTracedResourceName()), "traced resource - flush physical");
    startFlushPhysicalData();
    flushPhysicalData();  
    endFlushPhysicalData();
}


bbool Resource::isPhysicalExist()
{
    String filenameOriginal;
    
    getPath().getString(filenameOriginal);

    String filenameCooked   = FILESERVER->getCookedName(filenameOriginal);
    
    bbool cookedFileExist   = btrue;
    bbool physicalExist     = bfalse;

    if (!FILEMANAGER->isRemote())
    {
#if defined(ITF_SUPPORT_COOKING)
        cookedFileExist = RESOURCE_MANAGER->isCookedFileAllowed(filenameOriginal);
#endif //ITF_SUPPORT_COOKING

#ifdef ITF_SUPPORT_WWISE
        String ext = FilePath::getExtension(filenameOriginal);
        if (ext == "wav")
        {
            cookedFileExist = true;
        }
        else
        {
            if (cookedFileExist)
                cookedFileExist = FILEMANAGER->fileExists(filenameCooked);
        }
#else
        if (cookedFileExist)
            cookedFileExist = FILEMANAGER->fileExists(filenameCooked);
#endif





#if defined(ITF_SUPPORT_COOKING)
	    if (!RESOURCE_MANAGER->isModeFinal())
	    {
            filenameOriginal = ResourceManager::getExistingAlternativeFileNameForCooking(filenameOriginal, FILESERVER->getPlatform());

            u64 cookedTime      = FILEMANAGER->getLastTimeWriteAccess(filenameCooked);
            u64 originalTime    = FILEMANAGER->getLastTimeWriteAccess(filenameOriginal);

            if (originalTime!=cookedTime) //force to load the original if they doesn't match
                cookedFileExist = bfalse;
        }
#endif //ITF_SUPPORT_COOKING
        
        physicalExist = cookedFileExist;
    
        //missing ? try the original
        if (!cookedFileExist)
        {
            physicalExist =  FILEMANAGER->fileExists(filenameOriginal);

            if (!physicalExist)
            {
                String8 msg("Missing file: ");
                msg += filenameOriginal.cStr();
                //msg += "\nTechno may crash or try to load forever if this file is mandatory";
                ITF_WARNING(NULL, NULL, msg.cStr());
            }
        }
    }
    else
    {
        //in remote mode we force to true
        cookedFileExist = btrue;
        physicalExist   = btrue;
    }

    setCooked(cookedFileExist);

    return physicalExist;
}


void Resource::load()
{
    ITF_ASSERT_MSG(!m_path.getString().strstr(getTracedResourceName()), "traced resource - load");
    if (!isPhysicallyLoaded())
    {
        RESOURCE_MANAGER->requestFileLoading(getID());
    }
}

void Resource::unload()
{
    ITF_ASSERT_MSG(!m_path.getString().strstr(getTracedResourceName()), "traced resource - unload");

    ResourceGroup* group = getGroup();

    if ( group )
    {
        group->updatePhysicallyLoaded();
    }

    if( isPhysicallyLoaded())
    {
        RESOURCE_MANAGER->requestResourcePhysicalRelease(this);
    }
}

void Resource::startFlushPhysicalData()
{
    #ifdef ASSERT_ENABLED
        if (!m_reloading)
        {
            ITF_ASSERT(getPhysicalUsersCount() == 0);
            ITF_ASSERT(isPhysicallyLoaded());
        }
    #endif
}

void Resource::endFlushPhysicalData()
{
    setPhysicalLoaded(bfalse);

#ifndef ITF_FINAL
    setMemoryUsage(0);
#endif //ITF_FINAL
}


#ifdef ITF_SUPPORT_COOKING
bbool Resource::fileCookAvailable()
{   
    PathString_t pathName;
    getPath().getString(pathName);
    String cookFile = FILESERVER->getCookedName(pathName);
    //return a file path if it's valid cook file
    return !(cookFile.getLen()); //false ==> ask a cook
}
#endif //ITF_SUPPORT_COOKING

#ifndef ITF_FINAL



#define GETRESOURCETYPE(resourceType) if (_typename == #resourceType) return resourceType;

Resource::ResourceType  Resource::getResourceTypenameByType(const String &_typename)
{
    GETRESOURCETYPE(ResourceType_Texture);
    GETRESOURCETYPE(ResourceType_Lua);
    GETRESOURCETYPE(ResourceType_XML);
    GETRESOURCETYPE(ResourceType_AnimSkeleton);
    GETRESOURCETYPE(ResourceType_AnimPatchBank);
    GETRESOURCETYPE(ResourceType_AnimTrack);
    GETRESOURCETYPE(ResourceType_FriezeConfig);
    GETRESOURCETYPE(ResourceType_Sound);
    GETRESOURCETYPE(ResourceType_Font);
    GETRESOURCETYPE(ResourceType_MusicPart);
    return ResourceType_All;
}

void    Resource::getTypeName(String& _typename)
{
    switch  (getType())
    {
        case ResourceType_Texture:
            _typename = "ResourceType_Texture";
            break;
        case ResourceType_Lua:
            _typename = "ResourceType_Lua";
            break;
        case ResourceType_XML:
            _typename = "ResourceType_XML";
            break;
        case ResourceType_AnimSkeleton:
            _typename = "ResourceType_AnimSkeleton";
            break;
        case ResourceType_AnimPatchBank:
            _typename = "ResourceType_AnimPatchBank";
            break;
        case ResourceType_AnimTrack:
            _typename = "ResourceType_AnimTrack";
            break;
        case ResourceType_FriezeConfig:
            _typename = "ResourceType_FriezeConfig";
            break;
        case ResourceType_Sound:
            _typename = "ResourceType_Sound";
            break;
        case ResourceType_Font:
            _typename = "ResourceType_Font";
            break;
        case ResourceType_MusicPart:
            _typename = "ResourceType_MusicPart";
            break;

        default:
            _typename = "UNKNOW";
            break;
    }
}

void   Resource::dumpInfo(Blob& _blob)
{
    String rtypename = "";
    getTypeName(rtypename);

    _blob.pushString(getPath().getString());
    _blob.pushString(FilePath::getFilename(getPath().getString()));
    _blob.pushString(rtypename);
    _blob.pushFloat32(m_fTimeToLoad);
    _blob.pushBool(m_bPhysicalLoaded);
    _blob.pushUInt32(m_memoryUsage);
    _blob.pushBool(m_isCooked);

    String customInfo = getCustomInfo();
    _blob.pushString(customInfo);

}
#endif //!ITF_FINAL

void    Resource::setStatic(bbool _static)
{
    m_isStatic = _static;
    ResourceGroup* grp = getGroup();
    if (grp && grp != this)
        grp->setStatic(_static);
}

#ifdef ASSERT_ENABLED
void  Resource::addLogicalDataUser() 
{
    ITF_ASSERT(Synchronize::getCurrentThreadId() == ThreadSettings::m_settings[eThreadId_mainThread].m_threadID);
   ITF_ASSERT_MSG(!m_path.getString().strstr(getTracedResourceName()), "traced resource - add logic user");
    m_logicalDataRefCount++;
}
#endif

#ifdef ASSERT_ENABLED
    const u16* Resource::getTracedResourceName()
    {
        //return (u16*)(L"misc/fonts/font02.fnt");
        return NULL;
    }
#endif // ASSERT_ENABLED

#ifdef ENABLE_RESOURCE_HISTORY
    void Resource::dumpResourceHistory(ObjectRef _or)
    {
        if (ResourceHistoryManager::instance())
        {
            ResourceHistoryManager::instance()->dumpResourceHistory(_or);
        }

    }
#endif // ENABLE_RESOURCE_HISTORY

} // namespace ITF
 
