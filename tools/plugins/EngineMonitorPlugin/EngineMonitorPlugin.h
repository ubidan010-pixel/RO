#ifndef _ITF_ENGINEMONITORPLUGIN_H_
#define _ITF_ENGINEMONITORPLUGIN_H_

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

namespace ITF
{

    class EngineMonitorPlugin : public Plugin
    {
    public:

        enum FileType
        {
            filetype_none = 0,
            filetype_image,
            filetype_profilerData,
            ENUM_FORCE_SIZE_32(0)
        };

        EngineMonitorPlugin();
        virtual ~EngineMonitorPlugin();

        static                      EngineMonitorPlugin*        m_singleton;
        static  const NETPeer*      getNetPeer()                {return m_singleton->m_editorPeer;}
        static  u32                 getSenderID()               {return m_singleton->m_editorSenderID;}

        /************************************************************************/
        /* Messages                                                             */
        /************************************************************************/
        static const char*      getPluginName()                             {return "EngineMonitor_Plugin";}

        // send
        static const char*      getSendSessionInfoCmdName()                 {return "SessionInfo";}
        static const char*      getSendUpdateAllInfosCmdName()              {return "UpdateAllInfos";}
        
        static const char*      getSendFPSCmdName()                         {return "FPS";}
        static const char*      getSendWarningsCmdName()                    {return "Warnings";}
        static const char*      getSendInformationsCmdName()                {return "Informations";}
        static const char*      getSendStatsCmdName()                       {return "Stats";}
        static const char*      getSendMemoryCategoryStatsCmdName()         {return "MemoryCategoryStats";}
        static const char*      getSendMemoryCategoryStatsNameCmdName()     {return "MemoryCategoryStatsName";}        
        static const char*      getSendRenderColorsCmdName()                {return "RenderColors";}

        static const char*      getSendFileCmdName()                        {return "WriteFile";} 
        static const char*      getSendSetTrackingFPSCmdName()              {return "SetTrackingFPS";}
        static const char*      getSendDataTrackingFPSCmdName()             {return "DataTrackingFPS";}

        static const char*      getSendEndOFMessagesCmdName()               {return "END_OF_MESSAGES";}        

        

        // receive
        static const char*      getRecvDragDropFileCmdName()                {return "DragDropFile";}
        static const char*      getRecvSetMonitoringCmdName()               {return "SetMonitoring";}

        static const char*      getRecvSetMemoryMonitoringCmdName()         {return "SetMemoryMonitoring";}
        static const char*      getRecvGetMemoryCategoryStatsNameCmdName()  {return "GetMemoryCategoryStatsName";}

        static const char*      getRecvGetRenderColorsCmdName()             {return "GetRenderColors";}
        static const char*      getRecvSetRenderColorsCmdName()             {return "SetRenderColors";}
        
        static const char*      getRecvSetKeyReleasedCmdName()              {return "SetKeyReleased";}
        static const char*      getRecvSetKeyPressedCmdName()               {return "SetKeyPressed";}

        static const char*      getRecvGetTrackingFPSCmdName()              {return "GetTrackingFPS";}
        static const char*      getRecvSetTrackingFPSCmdName()              {return "SetTrackingFPS";}

        static const char*      getRecvForceFPScaptureCmdName()             {return "ForceFPScapture";}

        static const char*      getRecvSessionInfoCmdName()                 {return "getSessionInfo";}
        /************************************************************************/
        /*                                                                      */
        /************************************************************************/

        // Plugin inherited
        virtual void            receive( class Blob& _blob, const class NETPeer* _peer, u32 _senderID );
        virtual void            update();


        void                    setMonitoring(bbool _monitoring) { m_monitoring = _monitoring; }
        bbool                   getMonitoring() { return m_monitoring; }

        void                    processFPS();
        const f64               getFPS() const { return m_FPS; }
        void                    addInformation();

        void                    sendFile(const String& _fileName, char* _buffer, u32 _bufferSize, bbool _append, bbool _binary);
        void                    sendDataTrackingFPS(const String& _fileName);

    private:        
        void                    sendFPS(class Blob& _blob);
        void                    sendWarnings(class Blob& _blob);
        void                    sendInformations(class Blob& _blob);
        void                    sendStats(class Blob& _blob);
        void                    sendMemoryCategoryStats(class Blob& _blob);
        void                    onSetRenderColors(class Blob& _blob);

        void                    sendMemoryCategoryStatsName();

        void                    onSetMonitoring(class Blob& _blob );
        void                    onSetMemoryMonitoring(class Blob& _blob );
        void                    onDragDropFileOnTool(class Blob& _blob );               

        void                    onSetKeyPressed(class Blob& _blob);
        void                    onSetKeyReleased(class Blob& _blob);
        void                    keyProcess(class Blob& _blob, enum InputAdapter::PressStatus _status);

        void                    sendRenderColors();

        void                    requestProfilerFPScaptureMode();
        void                    sendTrackingFPS();
        void                    onTrackingFPS(class Blob& _blob);
        void                    onForceFPScapture(class Blob& _blob);
        void                    sendDataStrackingFPS(const f32 _fps);

        void                    applyPendingDragDropOnTool();

        bbool                   m_noWarnings_oldState;
        bbool                   m_monitoring;
        bbool                   m_connected;
        bbool                   m_memoryMonitoring;
        ITF_VECTOR<String>      m_pendingDragDropFileOnTool;

        f64                     m_FPS;
        f64                     m_LastFPSTime;

        bbool                   m_trakingFPS;
        f32                     m_FPScaptureLimit;
        f64                     m_inhibitionTimeCapture;

        // Render colors
        Vec3d                   m_gamma;
        float                   m_brightness;
        float                   m_contrast;
    };

} // namespace ITF


#endif // _ITF_ENGINEMONITORPLUGIN_H_
