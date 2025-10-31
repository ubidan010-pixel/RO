#ifndef _ITF_REMOTEPROFILERPLUGIN_H_
#define _ITF_REMOTEPROFILERPLUGIN_H_

#ifndef _ITF_PLUGIN_H_
#include "PluginGateway/Plugin.h"
#endif //_ITF_PLUGIN_H_

#ifndef ITF_TEMPLATEDATABASE_H
#include "engine/TemplateManager/TemplateDatabase.h"
#endif // ITF_TEMPLATEDATABASE_H

#ifndef _ITF_ANIMATEDCOMPONENT_H_
#include "engine/actors/components/animatedcomponent.h"
#endif //_ITF_ANIMATEDCOMPONENT_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_PROFILERMANAGER_H_
#include "engine/profiler/ProfilerManager.h"
#endif //_ITF_PROFILERMANAGER_H_

namespace ITF
{
    class RemoteProfilerPlugin : public Plugin
    {
        enum TabMonitor
        {
            TabNone = 0,
            TabLoading,
            TabRuntime,
        };

        struct ProfilerInfo
        {
            ProfilerInfo()
                : objectRef(ITF_INVALID_OBJREF)
                , timeToUpdate(0.0f)
                , timeToDraw(0.0f)
                , className("no define")
                , friendlyName("no define")
                , memoryAllocate(0)
                , fileName("no define")
            {}

            ObjectRef objectRef;
            f32 timeToUpdate;
            f32 timeToDraw;
            String className;
            String8 friendlyName;
            u32 memoryAllocate;
            String fileName;
        };

    public:
        RemoteProfilerPlugin();
        virtual ~RemoteProfilerPlugin();

        static                      RemoteProfilerPlugin*       m_singleton;
        static  const NETPeer*      getNetPeer()                {return m_singleton->m_editorPeer;}
        static  u32                 getSenderID()               {return m_singleton->m_editorSenderID;}

        /************************************************************************/
        /* Messages                                                             */
        /************************************************************************/
        static const char*      getPluginName()                             {return "RemoveProfiler_Plugin";}

        // send
        static const char*      getSendSessionInfoCmdName()                 {return "SessionInfo";}

        static const char*      getSendListCmdName()                        {return "List";}
        static const char*      getSendMoreInformationCmdName()             {return "MoreInformation";}
        static const char*      getSendLoadCapture()                        {return "LoadCaptureResult";}

        // receive
        static const char*      getRecvSetTabMonitorSelectedCmdName()       {return "SetTabMonitorSelected";}
        static const char*      getRecvSetMonitoringCmdName()               {return "SetMonitoring";}
        static const char*      getRecvSetUpdateCmdName()                   {return "SetUpdate";}
        static const char*      getRecvSetDrawCmdName()                     {return "SetDraw";}

        static const char*      getRecvDoOneCaptureCmdName()                {return "DoOneCapture";}
        static const char*      getRecvSetShowAABBCmdName()                 {return "SetShowAABB";}
        static const char*      getRecvSetCameraGoto()                      {return "CameraGoto";}

        static const char*      getRecvBreakUpdateCmdName()                 {return "BreakUpdate";}
        static const char*      getRecvBreakDrawCmdName()                   {return "BreakDraw";}
        static const char*      getRecvProfileEventCmdName()                {return "ProfileEvent";}
        

        static const char*      getRecvSessionInfoCmdName()                 {return "getSessionInfo";}
        static const char*      getRecvLoadCaptureCmdName()                 {return "SetLoadCapture";}
        /************************************************************************/
        /*                                                                      */
        /************************************************************************/

        // Plugin inherited
        virtual void            receive( class Blob& _blob, const class NETPeer* _peer, u32 _senderID );
        virtual void            update();
        virtual void            draw();


        void                    setTabMonitorSelected(TabMonitor _tabMonitorSelected) { m_tabMonitorSelected = _tabMonitorSelected; }
        bbool                   getTabMonitorSelected() { return m_tabMonitorSelected; }
		
    private:
        // message receive
        void                    onSetTabMonitorSelected(class Blob& _blob );
        void                    onSetUpdate(class Blob& _blob );
        void                    onSetDraw(class Blob& _blob );
        void                    onSetShowAABB(class Blob& _blob );
        void                    onCameraGoto(class Blob& _blob );
        void                    onBreakUpdate(class Blob& _blob );
        void                    onBreakDraw(class Blob& _blob );
        void                    onProfileEvent(class Blob& _blob );
        void                    onLoadCapture(class Blob& _blob);


        // message send
        void                    sendList();
        void                    sendMoreInformtion();
        void                    sendSessionInfo();
        


        void                    onSetMonitoring(class Blob& _blob );
        
        void                    fillAndResolve();
        void                    ResolveInfoAndAddingList(const ProfilerData& data,f32 _frequency);

        TabMonitor              m_tabMonitorSelected;
        bbool                   m_monitoring;
        bbool                   m_doOneCapture;
        bbool                   m_connected;
        ITF_VECTOR<ProfilerInfo> listProfilerInfo;

        bbool                   m_showAABB;
        bbool                   m_startLoadCapture;
        ITF_VECTOR<u32>         m_listShowAABB;

        ObjectRef               m_cameraFollow;

    };

} // namespace ITF


#endif // _ITF_SoundMonitorPLUGIN_H_
