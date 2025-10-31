// GLOBAL ENGINE CONFIG FILE. Must be the first included file
#ifndef _ENGINECOMMON_H_
    #include "core/enginecommon.h"
#endif // _ENGINECOMMON_H_

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef ITF_FINAL

#ifndef _ITF_SCHEDULERMONITOR_PLUGIN_H_
#include "tools/plugins/SchedulerMonitorPlugin/SchedulerMonitorPlugin.h"
#endif //_ITF_SCHEDULERMONITOR_PLUGIN_H_

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

#ifndef _ITF_SCHEDULER_H_
#include "engine/scheduler/scheduler.h"
#endif //_ITF_SCHEDULER_H_


namespace ITF
{

SchedulerMonitor_Plugin::SchedulerMonitor_Plugin()
{
    m_lastSentTime = 0.f;
}

SchedulerMonitor_Plugin::~SchedulerMonitor_Plugin()
{
}

void SchedulerMonitor_Plugin::receive(Blob& _blob,const NETPeer* _peer, u32 _senderID)
{
    String name, val;

    _blob.extractString(name);

    if ( name == "changestate" )
    {
        Scheduler::get()->swapImmediateMode();
    }
    else    
    if (name == "capture")
    {
        Scheduler::get()->startCapture();
    }

}

void SchedulerMonitor_Plugin::send(Blob& blob)
{
    if (m_editorSenderID == 0)
        return;

    if (!PLUGINGATEWAY->sendPacket(blob, m_editorPeer, m_editorSenderID))
        m_editorSenderID = 0;
}

void SchedulerMonitor_Plugin::update()
{
    if (m_editorSenderID == 0)
        return;

    float time = ELAPSEDTIME;
    if (fabs(time - m_lastSentTime) < getUpdateRate())
        return;

    m_lastSentTime = time;
/*
    if (!PLUGINGATEWAY->send(blob, m_editorPeer, m_editorSenderID))
        m_editorSenderID = 0;*/
}

} // namespace ITF

#endif //!ITF_FINAL