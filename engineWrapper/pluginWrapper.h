#pragma once

#ifndef _ITF_MEMORY_H_
#include "core/memory/memory.h"
#endif //_ITF_MEMORY_H_

#ifndef _ITF_CORETYPES_H_   
#include "core/types.h"  
#endif

#ifndef _ITF_ARCHIVE_H_
#include "core/Archive.h"
#endif //_ITF_ARCHIVE_H_


#ifndef _ITF_PLUGIN_H_
#include "PluginGateway/Plugin.h"
#endif // _ITF_PLUGIN_H_

#include "blobWrapper.h"

class PluginReceiver;

public ref class pluginWrapper
{
public:

    pluginWrapper() {};

    bool dispatch(blobWrapper^ blob);
    void attach( PluginReceiver* _plugin) {m_plugin = _plugin;}

protected:

    PluginReceiver* m_plugin;
};



class PluginReceiver : public ITF::Plugin
{
public:

    PluginReceiver() {};

    void receive        (ITF::Blob& _blob, const ITF::NETPeer* _peer, ITF::u32 _senderID);

    ITF::Blob* getFrontBlob();

private:
    ITF_VECTOR<ITF::Blob*> m_vBlobs;
};
