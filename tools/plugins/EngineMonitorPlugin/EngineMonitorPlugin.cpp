// GLOBAL ENGINE CONFIG FILE. Must be the first included file
#ifndef _ENGINECOMMON_H_
#include "core/enginecommon.h"
#endif // _ENGINECOMMON_H_

#if !defined(ITF_FINAL)


#ifndef _ITF_ENGINEMONITORPLUGIN_H_
#include "tools/plugins/EngineMonitorPlugin/EngineMonitorPlugin.h"
#endif //_ITF_ENGINEMONITORPLUGIN_H_

#ifndef _ITF_BLOBUTILS_H_
#include "core/blobUtils.h"
#endif //_ITF_BLOBUTILS_H_

#ifndef _ITF_PLUGINGATEWAY_H_
#include "PluginGateway/PluginGateway.h"
#endif //_ITF_PLUGINGATEWAY_H_

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

#ifndef _ITF_SYSTEMADAPTER_
#include "core/AdaptersInterfaces/SystemAdapter.h"
#endif //_ITF_SYSTEMADAPTER_

#ifndef _ITF_WARNINGMANAGER_H_
#include "engine/common/WarningManager.h"
#endif // _ITF_WARNINGMANAGER_H_

#ifndef _ITF_CONFIG_H_
#include "core/Config.h"
#endif //_ITF_CONFIG_H_

#ifndef _ITF_STATSMANAGER_H_
#include "engine/stats/statsManager.h"
#endif //_ITF_STATSMANAGER_H_

#ifndef ITF_MEMORYSTATSMANAGER_H_
#include "core/memory/memoryStatsManager.h"
#endif //ITF_MEMORYSTATSMANAGER_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_INPUTADAPTER_H_
#include "engine/AdaptersInterfaces/InputAdapter.h"
#endif //_ITF_INPUTADAPTER_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

namespace ITF
{
    EngineMonitorPlugin* EngineMonitorPlugin::m_singleton = NULL;

    EngineMonitorPlugin::EngineMonitorPlugin()
    : m_monitoring(bfalse)
    , m_connected(bfalse)
    , m_memoryMonitoring(0)
    , m_gamma(1.0f, 1.0f, 1.0f)
    , m_brightness(1.0f)
    , m_contrast(1.0f)
    , m_FPS(0.0)
    , m_LastFPSTime(0.0)
    , m_trakingFPS(bfalse)
    , m_inhibitionTimeCapture(0.0)
    {
        ITF_ASSERT(!m_singleton);
        m_singleton = this;
        m_noWarnings_oldState = CONFIG->m_noWarnings;
        
        m_FPScaptureLimit = CONFIG->m_ProfileFPS;
    }

    EngineMonitorPlugin::~EngineMonitorPlugin()
    {
    }

    void EngineMonitorPlugin::receive( class Blob& _blob, const class NETPeer* _peer, u32 _senderID )
    {
        m_connected = btrue;        

        String message;
        _blob.extractString(message);
        
        if (message == getRecvSessionInfoCmdName())
        {
            
        }
        else if (message == getRecvSetMonitoringCmdName())
        {
            onSetMonitoring(_blob);
        }
        else if (message == getRecvSetMemoryMonitoringCmdName())
        {
            onSetMemoryMonitoring(_blob);
        }
        else if (message == getRecvGetMemoryCategoryStatsNameCmdName())
        {
            sendMemoryCategoryStatsName();
        }
        else if (message == getRecvDragDropFileCmdName())
        {
            onDragDropFileOnTool(_blob);
        }
        else if (message == getRecvGetRenderColorsCmdName())
        {
            sendRenderColors();
        }
        else if (message == getRecvSetRenderColorsCmdName())
        {
            onSetRenderColors(_blob);
        }
        else if (message == getRecvSetKeyPressedCmdName())
        {
            onSetKeyPressed(_blob);
        }
        else if (message == getRecvSetKeyReleasedCmdName())
        {
            onSetKeyReleased(_blob);
        }
        else if (message == getRecvGetTrackingFPSCmdName())
        {
            sendTrackingFPS();
        }
        else if (message == getRecvSetTrackingFPSCmdName())
        {
            onTrackingFPS(_blob);
        }
        else if (message == getRecvForceFPScaptureCmdName())
        {
            onForceFPScapture(_blob);
        }
        else
            ITF_ASSERT_MSG(0, "unknown command");
    }

    void EngineMonitorPlugin::update()
    {
        if(m_monitoring && m_connected)
        {            
            processFPS();

            Blob b(BLOB_EXTRACT_AND_WRITE_ENABLED, 100*1024);
            b.pushString(EngineMonitorPlugin::getPluginName());
            b.pushString(getSendUpdateAllInfosCmdName());

            // adding informations to blob
            sendFPS(b);
            sendWarnings(b);
            sendInformations(b);
            sendStats(b);

            if(m_memoryMonitoring)
            {
                sendMemoryCategoryStats(b);
            }

            // end and send
            b.pushString(getSendEndOFMessagesCmdName());   // key of "End of messages"
            m_connected = PLUGINGATEWAY->sendPacket(b, m_editorPeer, m_editorSenderID);            

            if(m_trakingFPS)
            {
                requestProfilerFPScaptureMode();
            }

            applyPendingDragDropOnTool();
        }
        else
        {
            m_trakingFPS = bfalse;
        }
    }

    void EngineMonitorPlugin::processFPS()
    {
        f64 time = SYSTEM_ADAPTER->getTime();

        m_FPS = 1.0f / ( time - m_LastFPSTime );
        m_LastFPSTime = time;
    }

    /************************************************************************/
    /* Sending messages                                                     */
    /************************************************************************/
    void EngineMonitorPlugin::sendMemoryCategoryStatsName()
    {
        Blob b(BLOB_EXTRACT_AND_WRITE_ENABLED);
        b.pushString(EngineMonitorPlugin::getPluginName());
        b.pushString(getSendMemoryCategoryStatsNameCmdName());   // key

        #ifdef ITF_CATEGORY_MEMORY_ENABLE
        memoryStatsManager::get().dumpCategoryStatsName(b);
        #else
        b.pushBool(bfalse);   // disable
        #endif

        m_connected = PLUGINGATEWAY->sendPacket(b, m_editorPeer, m_editorSenderID);
    }

    /************************************************************************/
    /* Sending messages informations                                        */
    /*                                                                      */
    /*   Message structure                                                  */    
    /*      1) push key                                                     */
    /*      2) push informations...                                         */
    /************************************************************************/
    void EngineMonitorPlugin::sendFPS(class Blob& _blob )
    {
        const f64 fps = getFPS();

        _blob.pushString(getSendFPSCmdName());   // key
        _blob.pushFloat32((f32)fps);        
    }

    void EngineMonitorPlugin::sendWarnings(class Blob& _blob )
    {
    #ifndef ITF_FINAL
        _blob.pushString(getSendWarningsCmdName());   // key

        ITF_VECTOR<String> warnings;
        WarningManager::getWarningsList(warnings);    

        const u32 count = warnings.size();
        _blob.pushUInt32(count);
        for (u32 index = 0; index < count; index++)
        {
            _blob.pushString(warnings[index]);
        }       
    #endif
    }

    void EngineMonitorPlugin::sendInformations(class Blob& _blob )
    {

    }

    void EngineMonitorPlugin::sendStats(class Blob& _blob )
    {

#ifdef SUPPORT_STATS_MANAGER

        _blob.pushString(getSendStatsCmdName());

        ITF_VECTOR<String> vStatsStrings;
        STATS_MANAGER->getInfos(vStatsStrings);
        const u32 count = vStatsStrings.size();
        _blob.pushUInt32(count);
        for (u32 index = 0; index < count; index++)
        {
            _blob.pushString( vStatsStrings[index]   );
        }

#endif //SUPPORT_STATS_MANAGER
    }

    void EngineMonitorPlugin::sendMemoryCategoryStats(class Blob& _blob )
    {
        _blob.pushString(getSendMemoryCategoryStatsCmdName());   // key

        #ifdef ITF_CATEGORY_MEMORY_ENABLE
        memoryStatsManager::get().dumpCategoryStats(_blob);
        #else
        _blob.pushBool(bfalse);   // disable
        #endif    
    }

    void EngineMonitorPlugin::sendRenderColors()
    {
        Blob b(BLOB_EXTRACT_AND_WRITE_ENABLED);
        b.pushString(EngineMonitorPlugin::getPluginName());
        b.pushString(getSendRenderColorsCmdName());   // key

        b.pushFloat32(m_gamma.m_x);
        b.pushFloat32(m_gamma.m_y);
        b.pushFloat32(m_gamma.m_z);

        b.pushFloat32(m_brightness);

        b.pushFloat32(m_contrast);

        m_connected = PLUGINGATEWAY->sendPacket(b, m_editorPeer, m_editorSenderID);
    }

    void EngineMonitorPlugin::sendDataStrackingFPS(const f32 _fps)
    {
        Blob b(BLOB_EXTRACT_AND_WRITE_ENABLED);
        b.pushString(EngineMonitorPlugin::getPluginName());
        b.pushString(getSendDataTrackingFPSCmdName());   // key

        b.pushFloat32(_fps);

        const World* currentWorld = CURRENTWORLD;
        if(currentWorld && currentWorld->getRootScene())
        {
            Scene* scene = currentWorld->getRootScene();
            if(scene)
                b.pushString(scene->getPath().getString());
            else
                b.pushString("[Error: getRootScene is invalid]");
        }
        else 
            b.pushString("[Error: getCurrentWorld is invalid]");

        #ifdef SUPPORT_STATS_MANAGER
            sendStats(b);
        #else
            b.pushString("SUPPORT_STATS_MANAGER");
        #endif //SUPPORT_STATS_MANAGER


        m_connected = PLUGINGATEWAY->sendPacket(b, m_editorPeer, m_editorSenderID);
    }

    void EngineMonitorPlugin::sendFile(const String& _fileName, char* _buffer, u32 _bufferSize, bbool _append, bbool _binary)
     {
        Blob b(BLOB_EXTRACT_AND_WRITE_ENABLED);
        b.pushString(EngineMonitorPlugin::getPluginName());
        b.pushString(getSendFileCmdName());   // key

        b.pushBool(_append);
        b.pushBool(_binary);
        b.pushString(_fileName);
        //b.pushUInt32((Unit)_fileType);

        b.pushBinaryBlock(_buffer, _bufferSize);

        m_connected = PLUGINGATEWAY->sendPacket(b, m_editorPeer, m_editorSenderID);
    }

    void EngineMonitorPlugin::sendTrackingFPS()
    {
        Blob b(BLOB_EXTRACT_AND_WRITE_ENABLED);
        b.pushString(EngineMonitorPlugin::getPluginName());
        b.pushString(getSendSetTrackingFPSCmdName());   // key

        b.pushBool(m_trakingFPS);
        b.pushFloat32(m_FPScaptureLimit);

        m_connected = PLUGINGATEWAY->sendPacket(b, m_editorPeer, m_editorSenderID);
    }

    void EngineMonitorPlugin::requestProfilerFPScaptureMode()
    {
        f64 time = SYSTEM_ADAPTER->getTime();
        m_inhibitionTimeCapture -= time;
        if(m_inhibitionTimeCapture > 0.0f)
            return;
 
        const f64 fps = getFPS();
        if(fps <= m_FPScaptureLimit)
        {
            sendDataStrackingFPS((f32)fps);

            GFX_ADAPTER->requestProfilerFPScaptureMode();
            m_inhibitionTimeCapture = 100000.0f;
        }
        else
            m_inhibitionTimeCapture = 0.0f;
    }

    /************************************************************************/
    /* Process receive messages                                             */
    /************************************************************************/
    void EngineMonitorPlugin::onSetMonitoring(class Blob& _blob )
    {
        m_monitoring = _blob.extractBool();
    }

    void EngineMonitorPlugin::onSetMemoryMonitoring(class Blob& _blob )
    {
        m_memoryMonitoring = _blob.extractBool();
    }

    void EngineMonitorPlugin::onDragDropFileOnTool(class Blob& _blob)
    {
        String fileName = "";
        _blob.extractString(fileName);
        
        FilePath::normPath(fileName);
        FilePath::transformToRelative(fileName);
        
        const String& extension = FilePath::getExtension(fileName);
        if (extension == "isc" || extension == "isd")
        {
            m_pendingDragDropFileOnTool.push_back(fileName);
        }
    }

    void EngineMonitorPlugin::onSetRenderColors(class Blob& _blob)
    {
        m_gamma.m_x = _blob.extractFloat32();
        m_gamma.m_y = _blob.extractFloat32();
        m_gamma.m_z = _blob.extractFloat32();

        m_brightness = _blob.extractFloat32();

        m_contrast = _blob.extractFloat32();


        // apply to engine render
        GFX_ADAPTER->setGammaRamp(m_gamma,m_brightness,m_contrast);
    }

    void EngineMonitorPlugin::onSetKeyPressed(class Blob& _blob)
    {
        keyProcess(_blob, InputAdapter::Pressed);
    }

    void EngineMonitorPlugin::onSetKeyReleased(class Blob& _blob)
    {
        keyProcess(_blob, InputAdapter::Released);
    }

    void EngineMonitorPlugin::keyProcess(class Blob& _blob, InputAdapter::PressStatus _status)
    {
        u32   key       = _blob.extractInt32();
        bbool control   = _blob.extractBool();
        bbool alt       = _blob.extractBool();
        bbool shift     = _blob.extractBool();

        if(control)
        {
            INPUT_ADAPTER->onKey(KEY_LCTRL, _status);
            INPUT_ADAPTER->onKey(KEY_RCTRL, _status);
        }

        if(alt)
        {
            INPUT_ADAPTER->onKey(KEY_LALT, _status);
            INPUT_ADAPTER->onKey(KEY_RALT, _status);
        }

        if(shift)
        {
            INPUT_ADAPTER->onKey(KEY_LSHIFT, _status);
            INPUT_ADAPTER->onKey(KEY_RSHIFT, _status);
        }

        if(key != 0)
        {
            INPUT_ADAPTER->onKey(key, _status);
        }
    }

    void EngineMonitorPlugin::applyPendingDragDropOnTool()
    {
        if(!SCENE_MANAGER->isLoadingPending())
        {
            if(m_pendingDragDropFileOnTool.size() != 0)
            {
                const String& fileName = m_pendingDragDropFileOnTool[0];

                ITF_VERIFY(GAMEMANAGER->loadGameplayMap(fileName, btrue, bfalse));
                m_pendingDragDropFileOnTool.removeAt(0);
            }            
        }
    }

    void EngineMonitorPlugin::onTrackingFPS(class Blob& _blob)
    {
        m_trakingFPS = _blob.extractBool();
        m_FPScaptureLimit = _blob.extractFloat32();
        sendTrackingFPS();
    }

    void EngineMonitorPlugin::onForceFPScapture(class Blob& _blob)
    {
        sendDataStrackingFPS((f32)getFPS());

        GFX_ADAPTER->requestProfilerFPScaptureMode();
        m_inhibitionTimeCapture = 100000.0f;
    }

} // namespace ITF

#endif //!ITF_FINAL