// GLOBAL ENGINE CONFIG FILE. Must be the first included file
#ifndef _ENGINECOMMON_H_
    #include "core/enginecommon.h"
#endif // _ENGINECOMMON_H_

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_MEMORYVIEWERPLUGIN_H_
#include "tools/plugins/MemoryViewerPlugin/MemoryViewerPlugin.h"
#endif //_ITF_MEMORYVIEWERPLUGIN_H_

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

#ifndef _ITF_SLOTALLOCATORMANAGER_H_
#include "core/memory/slotallocatorManager.h"
#endif //_ITF_SLOTALLOCATORMANAGER_H_

#ifndef _ITF_THREADALLOCATIONTRACKER_H_
#include "core/memory/threadAllocatorStacker.h"
#endif //_ITF_THREADALLOCATIONTRACKER_H_

#ifndef _ITF_SYSTEMADAPTER_
#include "core/AdaptersInterfaces/SystemAdapter.h"
#endif //_ITF_SYSTEMADAPTER_

#ifndef ITF_MEMORYSTATSMANAGER_H_
#include "core/memory/memoryStatsManager.h"
#endif //ITF_MEMORYSTATSMANAGER_H_

namespace ITF
{
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#ifdef ITF_TRACKING_MEMORY_ENABLE
void dumpMemoryCaptureStart(MemoryViewerPlugin* _plugin,Blob& _blob,const NETPeer* _peer, u32 _senderID)
{
    bbool start_state = _blob.extractBool();       
    threadAllocatorTracker::getInstance()->started(start_state);
}

void dumpMemoryCapture(MemoryViewerPlugin* _plugin,Blob& _blob,const NETPeer* _peer, u32 _senderID)
{
	threadAllocatorTracker::getInstance()->dumpMemoryCapture();

	ITF::Blob blobToSend(ITF::BLOB_EXTRACT_AND_WRITE_ENABLED);
	blobToSend.pushString(_plugin->getPluginName());
	blobToSend.pushString("capturedone");
    PLUGINGATEWAY->sendPacket(blobToSend, _peer, _senderID);
}

void dumpAllocatorID(MemoryViewerPlugin* _plugin,Blob& _blob,const NETPeer* _peer, u32 _senderID)
{
	u32 allocatorID = _blob.extractUint32();

	ITF::Blob blobToSend(ITF::BLOB_EXTRACT_AND_WRITE_ENABLED);
	blobToSend.pushString(_plugin->getPluginName());
	blobToSend.pushString("dump_allocationidresult");

	threadAllocatorTracker::getInstance()->dumpAllocatorID(blobToSend,allocatorID);
	PLUGINGATEWAY->sendPacket(blobToSend, _peer, _senderID);

}

void dumpFrameAllocation(MemoryViewerPlugin* _plugin,Blob& _blob,const NETPeer* _peer, u32 _senderID)
{
	u32 allocatorID = _blob.extractUint32();       
	u32 count       = _blob.extractUint32();

	threadAllocatorTracker::getInstance()->requestCaptureFrameAllocation(allocatorID,count);
}

void dumpTopFrameAllocation(MemoryViewerPlugin* _plugin,Blob& _blob,const NETPeer* _peer, u32 _senderID)
{
	ITF::Blob blobToSend(ITF::BLOB_EXTRACT_AND_WRITE_ENABLED);
	blobToSend.pushString(_plugin->getPluginName());
	blobToSend.pushString("dump_capturetopallocation");
	u32 allocatorID = _blob.extractUint32();       
	u32 count       = _blob.extractUint32();

	threadAllocatorTracker::getInstance()->dumpCaptureTopAllocation(blobToSend,allocatorID,count);
	PLUGINGATEWAY->sendPacket(blobToSend, _peer, _senderID);
}

#endif

MemoryViewerPlugin::MemoryViewerPlugin()
{
    m_lastSentTime = 0.f;

#ifdef ITF_TRACKING_MEMORY_ENABLE
    memoryDumpCaptureStart	    = dumpMemoryCaptureStart;
	memoryDumpCapture			= dumpMemoryCapture;
	memoryDumpAllocatorID		= dumpAllocatorID;
	memoryDumpFrameAllocation	= dumpFrameAllocation;
	memoryDumpTopAllocation		= dumpTopFrameAllocation;
#else
    memoryDumpCaptureStart      = NULL;
	memoryDumpCapture			= NULL;
	memoryDumpAllocatorID		= NULL;
	memoryDumpFrameAllocation	= NULL;
	memoryDumpTopAllocation		= NULL;
#endif
}

MemoryViewerPlugin::~MemoryViewerPlugin()
{
}




//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------


#ifdef ITF_CATEGORY_MEMORY_ENABLE

void MemoryViewerPlugin::receive(Blob& _blob,const NETPeer* _peer, u32 _senderID)
{
    String name, val;

    _blob.extractString(name);

    if ( name == "capturestart" && memoryDumpCaptureStart)
    {
        (memoryDumpCaptureStart)(this,_blob,_peer,_senderID);
    }
    else
    if ( name == "capture" && memoryDumpCapture)
    {
		(memoryDumpCapture)(this,_blob,_peer,_senderID);
    }
    else if (name == "dumpallocatorid" && memoryDumpAllocatorID)
    {
		(memoryDumpAllocatorID)(this,_blob,_peer,_senderID);
    }
    else if ( name == "capture_frameallocation" && memoryDumpFrameAllocation)
    {
		(memoryDumpFrameAllocation)(this,_blob,_peer,_senderID);
    }
    else if (  name == "capture_topallocation" && memoryDumpTopAllocation)
    {
		(memoryDumpTopAllocation)(this,_blob,_peer,_senderID);
      
    }
	/*
    else if ( name == "dump_address")
    {
        u32 address = _blob.extractUint32();

        ITF::Blob blobToSend(ITF::BLOB_EXTRACT_AND_WRITE_ENABLED);
        blobToSend.pushString(getPluginName());
        blobToSend.pushString("dump_address_result");
        threadAllocatorTracker::getInstance()->dumpAddress(blobToSend,address);
        PLUGINGATEWAY->send(blobToSend, _peer, _senderID);
    }*/
 }

class ScopeFlushFrame
{
public:
    ScopeFlushFrame(){};
    ~ScopeFlushFrame()
    {

#ifdef ITF_TRACKING_MEMORY_ENABLE
        if (threadAllocatorTracker::getInstance())
            threadAllocatorTracker::getInstance()->flushFrameAllocation();
#endif //ITF_TRACKING_MEMORY_ENABLE
		memoryStatsManager::get().flushFrameAllocation();

	}
};

void MemoryViewerPlugin::update()
{
    ScopeFlushFrame sFlushFrame;//flush on exit function;  

    if (m_editorSenderID == 0)
        return;
	/*
#ifdef ITF_TRACKING_MEMORY_ENABLE
    if (threadAllocatorTracker::getInstance() && threadAllocatorTracker::getInstance()->hasCaptureFrameAllocationDone())
    {
        threadAllocatorTracker::getInstance()->disableCaptureFrameAllocation();
        ITF::Blob blob(ITF::BLOB_EXTRACT_AND_WRITE_ENABLED);
        blob.pushString(getPluginName());

        blob.pushString("dump_captureframeallocation");

        threadAllocatorTracker::getInstance()->dumpCaptureFrameAllocation(blob);

        if (!PLUGINGATEWAY->send(blob, m_editorPeer, m_editorSenderID))
            m_editorSenderID = 0;

        return;
    }
#endif
	*/
    float time = ELAPSEDTIME;
    if (fabs(time - m_lastSentTime) < getUpdateRate())
        return;

    m_lastSentTime = time;

    ITF::Blob blob(ITF::BLOB_EXTRACT_AND_WRITE_ENABLED);
    blob.pushString(getPluginName());
    blob.pushString("stats");
    blob.pushString(SYSTEM_ADAPTER->getPlatformName());

    memoryStatsManager::get().dumpCategoryStats(blob);

    slotAllocatorManager::dumpStats(blob);

    if (!PLUGINGATEWAY->sendPacket(blob, m_editorPeer, m_editorSenderID))
        m_editorSenderID = 0;
}
#else

void MemoryViewerPlugin::receive(Blob& _blob,const NETPeer* _peer, u32 _senderID)
{

}

void MemoryViewerPlugin::update()
{

}

#endif //ITF_CATEGORY_MEMORY_ENABLE

} // namespace ITF

