#ifndef _ITF_SOUNDMONITORPLUGIN_H_
#define _ITF_SOUNDMONITORPLUGIN_H_

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

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

namespace ITF
{

    class SoundMonitorPlugin : public Plugin
    {
        enum TabSoundMonitor
        {
            TabSoundMonitor_NONE = 0,
            TabSoundMonitor_SoundCalled = 1,
            TabSoundMonitor_CurrentSound,
            TabSoundMonitor_Bus,
            TabSoundMonitor_SoundLoaded,
            TabSoundMonitor_Warnings,
            TabSoundMonitor_Informations,
            TabSoundMonitor_Count,
        };

    public:
        SoundMonitorPlugin();
        virtual ~SoundMonitorPlugin();

        static                      SoundMonitorPlugin*        m_singleton;
        static  const NETPeer*      getNetPeer()                {return m_singleton->m_editorPeer;}
        static  u32                 getSenderID()               {return m_singleton->m_editorSenderID;}

        /************************************************************************/
        /* Messages                                                             */
        /************************************************************************/
        static const char*      getPluginName()                             {return "SoundMonitor_Plugin";}

        // send
        static const char*      getSendSessionInfoCmdName()                 {return "SessionInfo";}
        static const char*      getSendUpdateAllInfosCmdName()              {return "UpdateAllInfos";}

        static const char*      getSendSoundCalledCmdName()                 {return "SoundCalled";}
        static const char*      getSendCurrentSoundCmdName()                {return "CurrentSound";}
        static const char*      getSendBusCmdName()                         {return "Bus";}
        static const char*      getSendSoundLoadedCmdName()                 {return "SoundLoaded";}
        static const char*      getSendWarningsCmdName()                    {return "Warnings";}
        static const char*      getSendInformationsCmdName()                {return "Informations";}
        static const char*      getSendObjectListWithResourceCmdName()      {return "ObjectListWithResource";}
        

        // receive
        static const char*      getRecvSetTabMonitorSelectedCmdName()       {return "SetTabMonitorSelected";}
        static const char*      getRecvSetMonitoringCmdName()               {return "SetMonitoring";}
        static const char*      getRecvObjectListWithResourceCmdName()      {return "getObjectListWithResource";}


        static const char*      getRecvSessionInfoCmdName()                 {return "getSessionInfo";}
        /************************************************************************/
        /*                                                                      */
        /************************************************************************/

        // Plugin inherited
        virtual void            receive( class Blob& _blob, const class NETPeer* _peer, u32 _senderID );
        virtual void            update();


        void                    setTabMonitorSelected(TabSoundMonitor _tabMonitorSelected) { m_tabMonitorSelected = _tabMonitorSelected; }
        bbool                   getTabMonitorSelected() { return m_tabMonitorSelected; }
		
    private:        
        // message receive
        void                    onSetTabMonitorSelected(class Blob& _blob );
        void                    onObjectListWithResource(class Blob& _blob );

        // message send
        void                    sendSessionInfo();
        void                    sendSoundCalled();
        void                    sendCurrentSound();
        void                    sendBus();

        void                    pushBusMix( Blob &b, const BusMix &busMix );

        void                    sendSoundLoaded();
        void                    sendWarnings();
        void                    sendInformations();
        void                    sendObjectListWithResource(String _resourcePath);

        void                    onSetMonitoring(class Blob& _blob );


        TabSoundMonitor         m_tabMonitorSelected;
        bbool                   m_monitoring;
        bbool                   m_connected;
    };

} // namespace ITF


#endif // _ITF_SoundMonitorPLUGIN_H_
