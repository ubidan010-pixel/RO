// GLOBAL ENGINE CONFIG FILE. Must be the first included file
#ifndef _ENGINECOMMON_H_
#include "core/enginecommon.h"
#endif // _ENGINECOMMON_H_


#ifdef ITF_SUPPORT_RAKI
#if !defined(ITF_FINAL)


#ifndef _ITF_SOUNDMONITORPLUGIN_H_
#include "tools/plugins/SoundMonitorPlugin/SoundMonitorPlugin.h"
#endif //_ITF_SOUNDMONITORPLUGIN_H_

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

#ifndef _ITF_SOUNDMANAGER_H_
#include "engine/sound/SoundManager.h"
#endif //_ITF_SOUNDMANAGER_H_

#ifndef _ITF_SOUNDINSTANCE_H_
#include "engine/sound/SoundInstance.h"
#endif //_ITF_SOUNDINSTANCE_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_PICKABLE_H_
#include "engine/picking/Pickable.h"
#endif // _ITF_PICKABLE_H_

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_FEEDBACKFXMANAGER_H_
#include "engine/actors/managers/FeedbackFXManager.h"
#endif //_ITF_FEEDBACKFXMANAGER_H_

namespace ITF
{
    SoundMonitorPlugin* SoundMonitorPlugin::m_singleton = NULL;

    SoundMonitorPlugin::SoundMonitorPlugin()
    : m_tabMonitorSelected(TabSoundMonitor_NONE)
    , m_monitoring(btrue)
    , m_connected(btrue)
    {
        ITF_ASSERT(!m_singleton);
        m_singleton = this;
    }

    SoundMonitorPlugin::~SoundMonitorPlugin()
    {
    }

    void SoundMonitorPlugin::receive( class Blob& _blob, const class NETPeer* _peer, u32 _senderID )
    {
        m_connected = true;
        
        String message;
        _blob.extractString(message);
        
        if (message == getRecvSessionInfoCmdName())
        {
            sendSessionInfo();
        }
        else if (message == getRecvSetTabMonitorSelectedCmdName())
        {
            onSetTabMonitorSelected(_blob);
        }
        else if (message == getRecvSetMonitoringCmdName())
        {
            onSetMonitoring(_blob);
        }
        else if (message == getRecvObjectListWithResourceCmdName())
        {
            onObjectListWithResource(_blob);
        }
        else
            ITF_ASSERT_MSG(0, "unknown command");
    }

    void SoundMonitorPlugin::update()
    {
        if(!m_monitoring || !m_connected)
            return;
        
        if(m_tabMonitorSelected != TabSoundMonitor_NONE)
        {
            if(m_tabMonitorSelected == TabSoundMonitor_SoundCalled)
            {
                sendSoundCalled();
            }
            else if(m_tabMonitorSelected == TabSoundMonitor_CurrentSound)
            {
                sendCurrentSound();
            }
            else if(m_tabMonitorSelected == TabSoundMonitor_Bus)
            {
                sendBus();
            }
            else if(m_tabMonitorSelected == TabSoundMonitor_SoundLoaded)
            {
                // send one time, when select tab: onSetTabMonitorSelected()
            }
            else if(m_tabMonitorSelected == TabSoundMonitor_Warnings)
            {
                sendWarnings();
            }
            else if(m_tabMonitorSelected == TabSoundMonitor_Informations)
            {
                sendInformations();
            }
            else ITF_ASSERT(0);
        }
    }

    /************************************************************************/
    /* Sending messages                                                     */
    /************************************************************************/
    void SoundMonitorPlugin::sendSessionInfo()
    {
        Blob b(BLOB_EXTRACT_AND_WRITE_ENABLED);
        String path = DATA_FOLDER;
        path.replace('/', '\\');
        path.truncate(path.getLen()-1);
        b.pushString(SoundMonitorPlugin::getPluginName());
        b.pushString(getSendSessionInfoCmdName()); 
        b.pushString(path);
        PLUGINGATEWAY->sendPacket(b, SoundMonitorPlugin::getNetPeer(), SoundMonitorPlugin::getSenderID());
    }

    void SoundMonitorPlugin::sendSoundCalled()
    {
        Blob b(BLOB_EXTRACT_AND_WRITE_ENABLED);
        b.pushString(SoundMonitorPlugin::getPluginName());
        b.pushString(getSendSoundCalledCmdName());

        const SafeArray<SoundInstance*>* playingInstances = SOUND_MANAGER()->getPlayingInstances();
        u32 count = playingInstances->size();

        b.pushInt32(count);

        for (u32 i = 0 ; i < count; ++i)
        {
            b.pushString((*playingInstances)[i]->getDebugName().cStr());
            b.pushFloat32((*playingInstances)[i]->m_debugVolume);
            b.pushFloat32((*playingInstances)[i]->m_debugPitch);
            b.pushInt32((i32)(*playingInstances)[i]->getState());
            b.pushBool((*playingInstances)[i]->isPlaying());
            b.pushBool((*playingInstances)[i]->getIsStreamed());
            //b.pushBool(SOUND_ADAPTER->isStreamingSource((*playingInstances)[i]->getHandle()));
        }

        m_connected = PLUGINGATEWAY->sendPacket(b, m_editorPeer, m_editorSenderID);
    }

    void SoundMonitorPlugin::sendCurrentSound()
    {
        Blob b(BLOB_EXTRACT_AND_WRITE_ENABLED);
        b.pushString(SoundMonitorPlugin::getPluginName());
        b.pushString(getSendCurrentSoundCmdName());

        const SafeArray<SoundInstance*>* playingInstances = SOUND_MANAGER()->getPlayingInstances();
        u32 count = playingInstances->size();

        b.pushInt32(count);

        for (u32 i = 0 ; i < count; ++i)
        {
            b.pushString((*playingInstances)[i]->getDebugName().cStr());
            b.pushFloat32((*playingInstances)[i]->m_debugVolume);
            b.pushFloat32((*playingInstances)[i]->m_debugPitch);
            b.pushInt32((i32)(*playingInstances)[i]->getState());
            b.pushBool((*playingInstances)[i]->isPlaying());
            b.pushBool((*playingInstances)[i]->getIsStreamed());
            //b.pushBool(SOUND_ADAPTER->isStreamingSource((*playingInstances)[i]->getHandle()));
        }

        m_connected = PLUGINGATEWAY->sendPacket(b, m_editorPeer, m_editorSenderID);
    }

    void SoundMonitorPlugin::sendBus()
    {
        Blob b(BLOB_EXTRACT_AND_WRITE_ENABLED);
        b.pushString(SoundMonitorPlugin::getPluginName());
        b.pushString(getSendBusCmdName());

        const ITF_VECTOR<BusMix>& busMixList = SOUND_MANAGER()->getBusMix();

        u32 count = busMixList.size();

        b.pushInt32(count + 1);
        const BusMix & busMix  = SOUND_MANAGER()->getRelativeBusMixTarget();
        pushBusMix(b,busMix);
        for (u32 i = 0; i < count; ++i)
        {
            const BusMix &busMix = busMixList[i];

            pushBusMix(b, busMix);

        }

        m_connected = PLUGINGATEWAY->sendPacket(b, m_editorPeer, m_editorSenderID);
    }

    void SoundMonitorPlugin::sendSoundLoaded()
    {
        Blob b(BLOB_EXTRACT_AND_WRITE_ENABLED);
        b.pushString(SoundMonitorPlugin::getPluginName());
        b.pushString(getSendSoundLoadedCmdName());

        RESOURCE_MANAGER->dumpActivity(b, Resource::ResourceType_Sound, btrue /*bDisplayCooked*/, btrue /*bDisplayNotCooked*/);
        
        RESOURCE_MANAGER->dumpActivity(b, Resource::ResourceType_MusicPart, btrue /*bDisplayCooked*/, btrue /*bDisplayNotCooked*/);

        m_connected = PLUGINGATEWAY->sendPacket(b, m_editorPeer, m_editorSenderID);
    }

    void SoundMonitorPlugin::sendWarnings()
    {
        Blob b(BLOB_EXTRACT_AND_WRITE_ENABLED);
        b.pushString(SoundMonitorPlugin::getPluginName());
        b.pushString(getSendWarningsCmdName());

        ITF_VECTOR<String> warnings;
        WarningManager::getWarningsList(warnings);    

        const u32 count = warnings.size();
        b.pushUInt32(count);
        for (u32 index = 0; index < count; index++)
        {
            b.pushString(warnings[index]);
        }       

        m_connected = PLUGINGATEWAY->sendPacket(b, m_editorPeer, m_editorSenderID);
    }

    void SoundMonitorPlugin::sendInformations()
    {
        Blob b(BLOB_EXTRACT_AND_WRITE_ENABLED);
        b.pushString(SoundMonitorPlugin::getPluginName());
        b.pushString(getSendInformationsCmdName());

        String infos;
        SOUND_ADAPTER->getPerformanceDebugInfo(infos);

        b.pushString(infos);

        m_connected = PLUGINGATEWAY->sendPacket(b, m_editorPeer, m_editorSenderID);
    }

    void SoundMonitorPlugin::sendObjectListWithResource(String _resourcePath)
    {
        Blob b(BLOB_EXTRACT_AND_WRITE_ENABLED);
        b.pushString(SoundMonitorPlugin::getPluginName());
        b.pushString(getSendObjectListWithResourceCmdName());

        Resource* resource = RESOURCE_MANAGER->getResourceByPath_slow(_resourcePath);
        
        if (!resource)
        {
            b.pushUInt32(0);
            return;
        }

        ITF_VECTOR<String> objectList;

        // found actor with this resource
        for (u32 iWorld = 0; iWorld < WORLD_MANAGER->getWorldCount(); iWorld++)
        {
            World* curWorld = WORLD_MANAGER->getWorldAt(iWorld);
            if (!curWorld)
                continue;

            for (u32 iScene = 0; iScene < curWorld->getSceneCount(); iScene++)
            {
                Scene* curScene = curWorld->getSceneAt(iScene);
                if (!curScene)
                    continue;

                const PickableList&  friezes = curScene->getFrises();
                for (u32 itFrieze = 0; itFrieze < friezes.size(); itFrieze++)
                {
                    Frise* pFrieze = static_cast<Frise*>(friezes[itFrieze]);
                    ResourceGroup* pGroup = pFrieze->getResourceGroup();
                    if (pGroup)
                    {
                        if(pGroup->usesResource(resource->getID(), btrue))
                        {
                            objectList.push_back(pFrieze->getConfigNamePath().getString());
                        }
                    }
                }
                const PickableList&  actors = curScene->getActors();
                for (u32 itActor = 0; itActor < actors.size(); itActor++)
                {
                    Actor* pActor = static_cast<Actor*>(actors[itActor]);
                    ResourceGroup* pGroup = (ResourceGroup*)pActor->getResourceGroup();
                    if (pGroup)
                    {
                        if (pGroup->usesResource(resource->getID(), btrue))
                        {
                            objectList.push_back(pActor->getLua().getString());
                        }
                    }
                }
            }
        }

        const ITF_VECTOR<SoundDescriptor_Template *>* soundDescriptorTemplates = FEEDBACKFX_MANAGER->getSoundDescriptorTemplates();
        ITF_VECTOR<SoundDescriptor_Template *>::const_iterator sound_it = soundDescriptorTemplates->begin();
        ITF_VECTOR<SoundDescriptor_Template *>::const_iterator sound_end = soundDescriptorTemplates->end();

        for (; sound_it != sound_end; ++sound_it)
        {
            const ITF_VECTOR<Path> &files = (*sound_it)->getFiles();
            for (u32 i = 0; i < files.size(); i++)
            {
                String path = files[i].getString();
                if(path.equals(_resourcePath, btrue))
                {
#ifdef ITF_DEBUG_STRINGID
                    char buf[255];
                    _snprintf(buf, 255, "[FeedbackFX]     Name: %s     category: %s", (*sound_it)->GetName().getDebugString(), (*sound_it)->getCategory().getDebugString());
                    String name(buf);
                    objectList.push_back(name);
#else
                    objectList.push_back("[FeedbackFX]     Name: StringDebug no supported     category: StringDebug no supported");
#endif
                }
            }
            
            
        }

        //{ 
        //    ActorMap map = soundDescriptorTemplates = FEEDBACKFX_MANAGER->m_actors;
        //    ITF_VECTOR<SoundDescriptor_Template *>::const_iterator sound_it = soundDescriptorTemplates->begin();
        //    ITF_VECTOR<SoundDescriptor_Template *>::const_iterator sound_end = soundDescriptorTemplates->end();

        //    for (; sound_it != sound_end; ++sound_it)
        //    {

        //    }
        //}



        u32 count = objectList.size();
        b.pushUInt32(count);
        for (u32 i = 0; i < count; i++)
        {
            b.pushString(objectList[i]);
        }

        m_connected = PLUGINGATEWAY->sendPacket(b, m_editorPeer, m_editorSenderID);
    }


    /************************************************************************/
    /* Process receive messages                                             */
    /************************************************************************/
    void SoundMonitorPlugin::onSetTabMonitorSelected(class Blob& _blob )
    {
        m_tabMonitorSelected = (TabSoundMonitor)_blob.extractInt32();

        if(m_tabMonitorSelected == TabSoundMonitor_SoundLoaded)
        {
            sendSoundLoaded();
        }
    }

    void SoundMonitorPlugin::onSetMonitoring(class Blob& _blob )
    {
        m_monitoring = _blob.extractBool();
    }

    void SoundMonitorPlugin::onObjectListWithResource(class Blob& _blob )
    {
        String pathFile = "";
        _blob.extractString(pathFile);

        sendObjectListWithResource(pathFile);
    }

    void SoundMonitorPlugin::pushBusMix( Blob &b, const BusMix  &busMix )
    {
        b.pushUInt32(busMix.m_priority);
        b.pushFloat32(busMix.m_duration);
        b.pushFloat32(busMix.m_fadeIn);
        b.pushFloat32(busMix.m_fadeOut);

        u32 countBusDef = busMix.m_busDefs.size();
        b.pushInt32(countBusDef);
        for (u32 iBdef = 0; iBdef < countBusDef; ++iBdef)
        {
            const BusDef & bus = busMix.m_busDefs[iBdef];

            //f32 volume = 0.0f;
            //bus.getVoice()->GetVolume(&volume);
            //f32 dBVolume = XAudio2AmplitudeRatioToDecibels (volume);

            //b.pushFloat32(dBVolume);

#ifdef ITF_DEBUG_STRINGID
            b.pushString(bus.m_name.getDebugString());

            b.pushInt32(bus.m_outputs.size());
            for (u32 o = 0; o < bus.m_outputs.size(); o++)
            {
                b.pushString(bus.m_outputs[o].getDebugString());
            }
#else
            b.pushString("StringDebug no supported");
            b.pushInt32(1);
            b.pushString("StringDebug no supported");
#endif

            b.pushFloat32(bus.m_volume.dB());
            b.pushFloat32(bus.m_filterFrequency);
            b.pushFloat32(bus.m_filterQ);
            b.pushInt32((int)bus.m_filterType);
            b.pushBool(bus.m_reverbActive);
            b.pushInt32((int)bus.m_reverbPreset);

            //b.pushInt32(bus.m_modifiers.size());
            //for (u32 o = 0; o < bus.m_outputs.size(); o++)
            //{
            //    b.pushString(bus.m_modifiers[o].);
            //}
        }
    }

} // namespace ITF

#endif //!ITF_FINAL
#endif
