// GLOBAL ENGINE CONFIG FILE. Must be the first included file
#ifndef _ENGINECOMMON_H_
    #include "core/enginecommon.h"
#endif // _ENGINECOMMON_H_

#ifndef ITF_FINAL

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_RESOURCETOOLSPLUGIN_H_
#include "tools/plugins/ResourceToolsPlugin/ResourceToolsPlugin.h"
#endif //_ITF_RESOURCETOOLSPLUGIN_H_

#ifndef SERIALIZERTOOLXML_H
#include "core/serializer/ZSerializerToolXML.h"
#endif // SERIALIZERTOOLXML_H

#ifndef _ITF_BLOB_H_
#include "core/Blob.h"
#endif //_ITF_BLOB_H_

#ifndef _ITF_BLOBUTILS_H_
#include "core/blobUtils.h"
#endif //_ITF_BLOBUTILS_H_

#ifndef _ITF_PLUGINGATEWAY_H_
#include "PluginGateway/PluginGateway.h"
#endif //_ITF_PLUGINGATEWAY_H_

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

#ifndef _ITF_ID_SERVER_H_
#include "core/IDserver.h"
#endif //_ITF_ID_SERVER_H_

#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_


namespace ITF
{

ResourceToolsPlugin::ResourceToolsPlugin()
{
}

ResourceToolsPlugin::~ResourceToolsPlugin()
{
}

struct ContextResource
{
    ResourceToolsPlugin*    m_pPlugin;
    ResourceID              m_Resource;
};

static void OnResourceLoaded(void * _context)
{
    ContextResource *pContextResource = (ContextResource*) (_context);
    ResourceID resourceToReload = pContextResource->m_Resource;

    if (resourceToReload.isValidResourceId())
    {
        ITF::Blob blob(ITF::BLOB_EXTRACT_AND_WRITE_ENABLED);

        blob.pushString(pContextResource->m_pPlugin->getPluginName());
        blob.pushString("UpdateResource");
        resourceToReload.getResource()->dumpInfo(blob);


        u32 memory = RESOURCE_MANAGER->getMemoryUsage(Resource::ResourceType_Texture);
        blob.pushUInt32(memory);

        PLUGINGATEWAY->sendPacket(blob, pContextResource->m_pPlugin->m_editorPeer, pContextResource->m_pPlugin->m_editorSenderID);
    }

    SF_DEL(pContextResource);
}

void ResourceToolsPlugin::sendDumpActivity(const String& _cmdName,Blob& _blobReader)
{
    ITF::Blob blobWriter(ITF::BLOB_EXTRACT_AND_WRITE_ENABLED);

    blobWriter.pushString(getPluginName());
    blobWriter.pushString(_cmdName);

    String resourceType;
    bbool bDisplayCooked;
    bbool bDisplayNotCooked;
    _blobReader.extractString(resourceType);
    bDisplayCooked      = _blobReader.extractBool();
    bDisplayNotCooked   = _blobReader.extractBool();

    Resource::ResourceType resourceTypeRequested = Resource::getResourceTypenameByType(resourceType);
    RESOURCE_MANAGER->dumpActivity(blobWriter,resourceTypeRequested,bDisplayCooked,bDisplayNotCooked);
    PLUGINGATEWAY->sendPacket(blobWriter, m_editorPeer, m_editorSenderID);
}

void ResourceToolsPlugin::receive(Blob& _blobReader,const NETPeer* _peer, u32 _senderID)
{
    String _cmdName, val;

    _blobReader.extractString(_cmdName);

    if ( _cmdName == "DumpResources" )
    {
        sendDumpActivity(_cmdName,_blobReader);
    }
    else
    if ( _cmdName == "ReloadResource")
    {
        String path; 
        _blobReader.extractString(path);

        FilePath::normPath(path);
        Resource* pResource = RESOURCE_MANAGER->getResourceByPath_slow(path);
        if (pResource)
        {
            ResourceID resourceToReload = pResource->getID();
            ITF_VECTOR<ResourceID> idList;
            idList.push_back(resourceToReload);
            ContextResource *pContextResource = new ContextResource();
            pContextResource->m_pPlugin     = this;
            pContextResource->m_Resource    = resourceToReload;
            RESOURCE_MANAGER->registerResourceLoadCallback(idList,(void*) pContextResource,OnResourceLoaded);
            RESOURCE_MANAGER->reloadResource(path);
        }
    }
    else
    if ( _cmdName == "NewPlatform")
    {
        String platform; 
        _blobReader.extractString(platform);
        platform.toUpper();
        //on emulation ,we reload all the texture type
        FILESERVER->rebuildCookDepot(platform);
        RESOURCE_MANAGER->reloadResourcesByType( Resource::ResourceType_Texture);

        //main thread
        RESOURCE_MANAGER->loadResourcesPending();
        RESOURCE_MANAGER->synchronizeResourcesAsLoaded();
        RESOURCE_MANAGER->synchronizeResourcesToProcessFromMainThread();

        sendDumpActivity("DumpResources",_blobReader);
    }
}


void ResourceToolsPlugin::UnselectAll(bbool _force)
{
    sendClearSelection();
}

void ResourceToolsPlugin::sendClearSelection()
{
    ITF::Blob blob(ITF::BLOB_EXTRACT_AND_WRITE_ENABLED, 10 * 1024);
    blob.pushString(getPluginName());
    blob.pushString("Clear");

    PLUGINGATEWAY->sendPacket(blob, m_editorPeer, m_editorSenderID);
}

void ResourceToolsPlugin::update()
{
}

} // namespace ITF

#endif //!ITF_FINAL