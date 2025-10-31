#include "precompiled_engine.h"

#ifndef _ITF_RESOURCEGROUP_H_
#include "engine/resources/ResourceGroup.h"
#endif //_ITF_RESOURCEGROUP_H_

#ifndef _ITF_RESOURCE_H_   
#include "engine/resources/Resource.h"
#endif //_ITF_RESOURCE_H_   

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_SYNCHRONIZE_H_
#include "core/system/Synchronize.h"
#endif //_ITF_SYNCHRONIZE_H_

namespace ITF
{


void ResourceGroup::initStaticData()
{
}

void ResourceGroup::deleteStaticData()
{
}

ResourceGroup::ResourceGroup() : Resource(Path::EmptyPath)
{
    m_alreadyLoaded = bfalse;
    m_physicallyReady = bfalse;
}

ResourceGroup::~ResourceGroup()
{
    ITF_ASSERT(!m_alreadyLoaded);
    ITF_ASSERT(!getPhysicalUsersCount());
    clearAll();
}

void ResourceGroup::flushPhysicalData()
{
 //   ITF_ASSERT_MSG(bfalse, "should never be called for non physical resources. Call unload() instead");
    unload();
}

void ResourceGroup::setStatic(bbool _static)
{
    Resource::setStatic(_static);
    for ( ResourceList::iterator it = m_ResourcesIDs.begin(); it != m_ResourcesIDs.end(); ++it )
    {
        Resource* pRes = (*it).getResource();
        ITF_ASSERT_MSG(pRes, "verify resource !!");
        if (pRes)
        {
            ResourceGroup* grp = pRes->getGroup();
            if (grp)
                grp->setStatic(_static);
        }
    }

    for (ResourceList::iterator it = m_ResourcesIDs.begin(); it != m_ResourcesIDs.end(); ++it)
    {
        Resource* res = (*it).getResource();
        ITF_ASSERT_MSG(res, "verify resource !!");

        if ( res )
            res->setStatic(_static);
    }
}

void ResourceGroup::clearAll(bbool _recurseSubGroups)
{
    ITF_ASSERT(Synchronize::getCurrentThreadId() == ThreadSettings::m_settings[eThreadId_mainThread].m_threadID);
#ifdef ASSERT_ENABLED
    ITF_ASSERT_MSG(!getPath().getString().strstr(Resource::getTracedResourceName()), "traced group - clearAll");
#endif

    if (_recurseSubGroups)
    {
        for ( ResourceList::iterator it = m_ResourcesIDs.begin(); it != m_ResourcesIDs.end(); ++it )
        {
            Resource* pRes = (*it).getResource();
            ITF_ASSERT_MSG(pRes, "verify resource !!");
            if (pRes)
            {
                ResourceGroup* grp = pRes->getGroup();
                if (grp)
                    grp->clearAll(btrue);
            }
        }
    }

    unload();
    for (ResourceList::iterator it = m_ResourcesIDs.begin(); it != m_ResourcesIDs.end(); ++it)
    {
        Resource* res = (*it).getResource();
        ITF_ASSERT_MSG(res, "verify resource !!");

        if ( res )
            RESOURCE_MANAGER->removeUserToResourceLogicalData(res);
    }
    m_ResourcesIDs.clear();
    m_alreadyLoaded = bfalse;
    m_physicallyReady = bfalse;
}

bbool ResourceGroup::usesResource(const ResourceID& _id, bbool _recurseSubGroups)
{
    ITF_ASSERT(Synchronize::getCurrentThreadId() == ThreadSettings::m_settings[eThreadId_mainThread].m_threadID);
    for ( ResourceList::iterator it = m_ResourcesIDs.begin(); it != m_ResourcesIDs.end(); ++it )
    {
        if (*it == _id)
            return btrue;
    }

    if (_recurseSubGroups)
    {
        for ( ResourceList::iterator it = m_ResourcesIDs.begin(); it != m_ResourcesIDs.end(); ++it )
        {
            Resource* pRes = (*it).getResource();
            ITF_ASSERT_MSG(pRes, "verify resource !!");
            if (pRes)
            {
                ResourceGroup* grp = pRes->getGroup();
                if (grp && grp->usesResource(_id, btrue))
                        return btrue;
            }
        }
    }

    return bfalse;
}


bbool ResourceGroup::addResource( Resource* _res, bbool _addLogicUser, bbool _checkDoublons )
{
    if (isStatic())
        _res->setStatic(btrue);

    _res->setSubResource(btrue);

    ITF_ASSERT(Synchronize::getCurrentThreadId() == ThreadSettings::m_settings[eThreadId_mainThread].m_threadID);
    m_physicallyReady = bfalse;

#ifdef ASSERT_ENABLED
    ITF_ASSERT_MSG(!getPath().getString().strstr(Resource::getTracedResourceName()), "traced group - add resource");
    ITF_ASSERT_MSG(!_res->getPath().getString().strstr(Resource::getTracedResourceName()), "traced resource added to group");
#endif
    if (_checkDoublons)
    {
        for (ResourceList::iterator it = m_ResourcesIDs.begin(); it != m_ResourcesIDs.end(); ++it)
        {
            if ( *it == _res->getID() )
            {
                return bfalse;
            }
        }
    }

    ITF_ASSERT_CRASH(!isPhysicallyLoaded(), "Adding a resource to an already physical loaded resource is completly forbidden");
    ITF_ASSERT(RESOURCE_MANAGER->isHotReloading() || 0  == getPhysicalUsersCount());   // No new resource in an already loaded group
    m_ResourcesIDs.push_back(_res->getID());
    if (_addLogicUser)
        RESOURCE_MANAGER->queryAddUserToResourceLogicalData(_res);
    if (m_alreadyLoaded)
        RESOURCE_MANAGER->queryAddUserToResourcePhysicalData(_res);

    return btrue;
}

ResourceID ResourceGroup::addResource( Resource::ResourceType _type, const Path& _path )
{
#ifdef ASSERT_ENABLED
    ITF_ASSERT_MSG(!getPath().getString().strstr(Resource::getTracedResourceName()), "traced group - add resource");
#endif
    ITF_ASSERT(Synchronize::getCurrentThreadId() == ThreadSettings::m_settings[eThreadId_mainThread].m_threadID);
    m_physicallyReady = bfalse;

    // check if the resource already exists
    if (ResourceType_ResourceGroup != _type) // doublons can't be checked for groups
    {
        ResourceID existsID;
        if (RESOURCE_MANAGER->getResourceIdFromFile(existsID, _path.getStringID(), _type))
        {
            // first check doublons, otherwise newResourceIDFromFile will increase logical users count
            for (ResourceList::iterator it = m_ResourcesIDs.begin(); it != m_ResourcesIDs.end(); ++it)
            {
                if ( *it == existsID )
                    return existsID;
            }
        }
    }

    ResourceID resID = RESOURCE_MANAGER->newResourceIDFromFile(_type, _path);
    Resource* resource = resID.getResource();

    ITF_ASSERT(resource); // Id pointing to no resource (or deprecated resource)
    addResource(resource, bfalse, bfalse); // don't add logical data user, it's already done in newResourceIDFromFile
    return resID;
}

bbool ResourceGroup::removeResource( const ResourceID &_id, bbool _recurseSubGroups )
{
    ITF_ASSERT(Synchronize::getCurrentThreadId() == ThreadSettings::m_settings[eThreadId_mainThread].m_threadID);
#ifdef ASSERT_ENABLED
    ITF_ASSERT_MSG(!getPath().getString().strstr(Resource::getTracedResourceName()), "traced group - remove resource");
#endif

    bbool ret = bfalse;

    for ( ResourceList::iterator it = m_ResourcesIDs.begin(); it != m_ResourcesIDs.end(); ++it )
    {
        if (*it == _id)
        {
            Resource* res = _id.getResource();
            ITF_ASSERT_MSG(res, "verify resource !!");
            if ( res )
            {
                if ( m_alreadyLoaded )
                {
                    RESOURCE_MANAGER->removeUserToResourcePhysicalData(res);
                }

                RESOURCE_MANAGER->removeUserToResourceLogicalData(res);
            }

            m_ResourcesIDs.erase(it);

            ret = btrue;
            break;
        }
    }

    if (_recurseSubGroups)
    {
        for ( ResourceList::iterator it = m_ResourcesIDs.begin(); it != m_ResourcesIDs.end(); ++it )
        {
            Resource* pRes = (*it).getResource();
            ITF_ASSERT_MSG(pRes, "verify resource !!");
            if (pRes)
            {
                ResourceGroup* pGroup = pRes->getGroup();
                if (pGroup)
                {
                    if (pGroup->removeResource(_id, btrue))
                        ret = btrue;
                }
            }
        }
    }

    if(!m_physicallyReady)
        updatePhysicallyLoaded(); // update m_physicallyReady flag

    return ret;
}

bbool  ResourceGroup::isPhysicallyLoaded()
{
    return m_physicallyReady;
}

void ResourceGroup::updatePhysicallyLoaded()
{
    ITF_ASSERT(Synchronize::getCurrentThreadId() == ThreadSettings::m_settings[eThreadId_mainThread].m_threadID);

    if (m_physicallyReady)
        return;

    if (!m_alreadyLoaded)
    {
        return;
    }
    
    for ( ResourceList::iterator it = m_ResourcesIDs.begin(); it != m_ResourcesIDs.end(); ++it )
    {
        Resource* pRes = (*it).getResource();
        ITF_ASSERT_MSG(pRes, "verify resource !!");
        if (pRes)
        {
            ResourceGroup* grp = pRes->getGroup();
            if (grp)
                grp->updatePhysicallyLoaded();
        }
    }

    for ( ResourceList::const_iterator it = m_ResourcesIDs.begin(); it != m_ResourcesIDs.end(); ++it )
	{
        Resource * res = (*it).getResource();
        ITF_ASSERT_MSG(res, "verify resource !!");
        if (res)
        {
            if (!res->isPhysicallyLoaded())
			    return;
        }
	}

    m_physicallyReady = btrue;
}

void ResourceGroup::dumpUnloadedResources(String8& _msg)
{
    for ( ResourceList::iterator it = m_ResourcesIDs.begin(); it != m_ResourcesIDs.end(); ++it )
    {
        Resource* pRes = (*it).getResource();
        if (pRes)
        {
            ResourceGroup* grp = pRes->getGroup();
            if (grp)
                grp->dumpUnloadedResources(_msg);
        }
    }

    PathString8_t pathName;

    for ( ResourceList::const_iterator it = m_ResourcesIDs.begin(); it != m_ResourcesIDs.end(); ++it )
	{
        Resource * res = (*it).getResource();
        if (res)
        {
            if (!res->isPhysicallyLoaded())
            {
                res->getPath().getString(pathName);
                _msg += "\n - ";
                _msg += pathName;
                _msg += " - is missing";
            }			    
        }
	}
}


void    ResourceGroup::load()
{
    ITF_ASSERT(Synchronize::getCurrentThreadId() == ThreadSettings::m_settings[eThreadId_mainThread].m_threadID);
    if (m_alreadyLoaded)
    {
        ITF_ASSERT(getPhysicalUsersCount());
        return;
    }
    m_physicallyReady = bfalse;

#ifdef ASSERT_ENABLED
    ITF_ASSERT_MSG(!getPath().getString().strstr(Resource::getTracedResourceName()), "traced group - load");
#endif

    m_alreadyLoaded = btrue;

    if (!isEmpty())
    {
        const ResourceGroup::ResourceList& GroupResources = getResourcesList();
        for (ResourceGroup::ResourceList::const_iterator it = GroupResources.begin() ; it != GroupResources.end(); ++it)
        {
            Resource *res = (*it).getResource();
            ITF_ASSERT_MSG(res, "verify resource !!");
            if (res)
                res->addPhysicalDataUser();
        }
    }
}

void    ResourceGroup::unload()
{
    ITF_ASSERT(Synchronize::getCurrentThreadId() == ThreadSettings::m_settings[eThreadId_mainThread].m_threadID);
    m_physicallyReady = bfalse;

    if (!m_alreadyLoaded)
    {
        return;
    }
#ifdef ASSERT_ENABLED
    ITF_ASSERT_MSG(!getPath().getString().strstr(Resource::getTracedResourceName()), "traced group - flushPhysicalData");
#endif

    for (ResourceList::iterator it = m_ResourcesIDs.begin(); it != m_ResourcesIDs.end(); ++it)
    {
        Resource* res = (*it).getResource();
        if ( res )
            RESOURCE_MANAGER->removeUserToResourcePhysicalData(res);
    }

    m_alreadyLoaded = bfalse;
}

void ResourceGroup::addPhysicalUserFromMainThread()
{
    ITF_ASSERT(Synchronize::getCurrentThreadId() == ThreadSettings::m_settings[eThreadId_mainThread].m_threadID);
    addPhysicalDataUser();
}

bbool ResourceGroup::tryLoadFile()
{
    ITF_ASSERT_MSG(!isPhysicallyLoaded(), "duplicate resource loading can lead to crashes");
    ITF_ASSERT_MSG(bfalse, "should never be called for non physical resources. Call load() instead");
    load();
    return btrue;
}

bbool   ResourceGroup::isPhysicalExist()
{
    return btrue; // existence of references files will be solved when loading them
}


} // namespace ITF