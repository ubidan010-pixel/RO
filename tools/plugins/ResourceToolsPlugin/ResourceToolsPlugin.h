#ifndef _ITF_RESOURCETOOLSPLUGIN_H_
#define _ITF_RESOURCETOOLSPLUGIN_H_

#ifndef _ITF_PLUGIN_H_
#include "PluginGateway/Plugin.h"
#endif //_ITF_PLUGIN_H_

namespace ITF
{

class ResourceToolsPlugin : public Plugin
{
public:
    ResourceToolsPlugin();
    virtual ~ResourceToolsPlugin();

    static const char*      getPluginName() {return  "ResourceTools_Plugin";}
    static const float      getUpdateRate() {return 0.5f;} // in seconds

    void sendDumpActivity(const String& _cmdName,Blob& _blobReader);

    // Plugin inherited
    virtual void            receive( class Blob& _blob, const class NETPeer* _peer, u32 _senderID );
    virtual void            UnselectAll(bbool _force);
    virtual void            update();
 
private:

    void                    sendClearSelection();

};

} // namespace ITF


#endif // _ITF_RESOURCETOOLSPLUGIN_H_
