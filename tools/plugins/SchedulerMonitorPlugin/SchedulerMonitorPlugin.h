#ifndef _ITF_SCHEDULERMONITOR_PLUGIN_H_
#define _ITF_SCHEDULERMONITOR_PLUGIN_H_

#ifndef _ITF_PLUGIN_H_
#include "PluginGateway/Plugin.h"
#endif //_ITF_PLUGIN_H_

namespace ITF
{

class SchedulerMonitor_Plugin : public Plugin
{
public:
    SchedulerMonitor_Plugin();
    virtual ~SchedulerMonitor_Plugin();

    static const char*      getPluginName() {return  "SchedulerMonitor_Plugin";}
    static const float      getUpdateRate() {return 1.0f;} // in seconds

    void send(class Blob& blob);

    // Plugin inherited
    virtual void            receive( class Blob& _blob, const class NETPeer* _peer, u32 _senderID );
    virtual void            update();
 
private:


    f32                         m_lastSentTime;
};

} // namespace ITF


#endif // _ITF_SCHEDULERMONITOR_PLUGIN_H_
