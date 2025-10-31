#ifndef _ITF_MEMORYVIEWERPLUGIN_H_
#define _ITF_MEMORYVIEWERPLUGIN_H_

#ifndef _ITF_PLUGIN_H_
#include "PluginGateway/Plugin.h"
#endif //_ITF_PLUGIN_H_

namespace ITF
{

class MemoryViewerPlugin : public Plugin
{
public:
    MemoryViewerPlugin();
    virtual ~MemoryViewerPlugin();

    static const char*      getPluginName() {return  "MemoryViewer_Plugin";}
    static const float      getUpdateRate() {return 1.0f;} // in seconds

    // Plugin inherited
    virtual void            receive( class Blob& _blob, const class NETPeer* _peer, u32 _senderID );
    virtual void            update();
 
private:

	typedef void (*MemoryDumpFunction)(MemoryViewerPlugin* _plugin,Blob& _blob,const NETPeer* _peer, u32 _senderID);

    MemoryDumpFunction			memoryDumpCaptureStart;
	MemoryDumpFunction			memoryDumpCapture;
	MemoryDumpFunction			memoryDumpAllocatorID;
	MemoryDumpFunction			memoryDumpFrameAllocation;
	MemoryDumpFunction			memoryDumpTopAllocation;

    f32                         m_lastSentTime;
};

} // namespace ITF


#endif // _ITF_MEMORYVIEWERPLUGIN_H_
