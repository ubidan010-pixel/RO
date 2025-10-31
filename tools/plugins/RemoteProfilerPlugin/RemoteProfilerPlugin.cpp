// GLOBAL ENGINE CONFIG FILE. Must be the first included file
#ifndef _ENGINECOMMON_H_
#include "core/enginecommon.h"
#endif // _ENGINECOMMON_H_

#if !defined(ITF_FINAL)


#ifndef _ITF_REMOTEPROFILERPLUGIN_H_
#include "tools/plugins/RemoteProfilerPlugin/RemoteProfilerPlugin.h"
#endif //_ITF_REMOTEPROFILERPLUGIN_H_

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

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_PROFILERMANAGER_H_
#include "engine/profiler/ProfilerManager.h"
#endif //_ITF_PROFILERMANAGER_H_

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_BUNDLEPROFILER_H_
#include "engine/file/bundleProfiler.h"
#endif //_ITF_BUNDLEPROFILER_H_

#ifndef _ITF_CAMERACONTROLLERMANAGER_H_
#include "engine/actors/components/Camera/CameraControllerManager.h"
#endif // _ITF_CAMERACONTROLLERMANAGER_H_

namespace ITF
{
    RemoteProfilerPlugin* RemoteProfilerPlugin::m_singleton = NULL;

    RemoteProfilerPlugin::RemoteProfilerPlugin()
    : m_tabMonitorSelected(TabNone)
    , m_monitoring(bfalse)
    , m_connected(bfalse)
    , m_showAABB(bfalse)
    , m_startLoadCapture(bfalse)
    , m_doOneCapture(bfalse)
    {
        ITF_ASSERT(!m_singleton);
        m_singleton = this;
    }

    RemoteProfilerPlugin::~RemoteProfilerPlugin()
    {
    }

    void RemoteProfilerPlugin::receive( class Blob& _blob, const class NETPeer* _peer, u32 _senderID )
    {
        m_connected = true;
        PROFILERMANAGER->setEnableMonitoring(true, true);
        
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
        else if (message == getRecvSetUpdateCmdName())
        {
            onSetUpdate(_blob);
        }
        else if (message == getRecvSetDrawCmdName())
        {
            onSetDraw(_blob);
        }
        else if (message == getRecvDoOneCaptureCmdName())
        {
            m_doOneCapture = btrue;
        }
        else if (message == getRecvSetShowAABBCmdName())
        {
            onSetShowAABB(_blob);
        }
        else if (message == getRecvBreakUpdateCmdName())
        {
            onBreakUpdate(_blob);
        }
        else if (message == getRecvBreakDrawCmdName())
        {
            onBreakDraw(_blob);
        }
        else if (message == getRecvProfileEventCmdName())
        {
            onProfileEvent(_blob);
        }
        else if (message == getRecvLoadCaptureCmdName())
        {
            onLoadCapture(_blob);
        }
        else if (message == getRecvSetCameraGoto())
        {
            onCameraGoto(_blob);
        }

        else
            ITF_ASSERT_MSG(0, "unknown command");
    }

    void RemoteProfilerPlugin::update()
    {
        if(PROFILERMANAGER == NULL)
            return;

        bbool enable = (m_monitoring || m_doOneCapture) && m_connected;
        PROFILERMANAGER->setEnableMonitoring(enable, m_connected);

        //update camera following
        const Pickable* pObj = static_cast<const Pickable*>(GETOBJECT(m_cameraFollow));
        if (pObj)
        {
            Vec3d pos(pObj->getPos().getX(),pObj->getPos().getY(),pObj->getDepth()+13.0f);
            CAMERACONTROLLERMANAGER->setCameraSequencePosition (pos,13.0f); 
        }
        else
        {
            m_cameraFollow.invalidate();
        }

        if(!enable)
            return;

        m_doOneCapture = bfalse;

        if(m_tabMonitorSelected != TabNone)
        {
            if(m_tabMonitorSelected == TabLoading)
            {
                
            }
            else if(m_tabMonitorSelected == TabRuntime)
            {
                sendList();
              //  sendMoreInformtion();
            }
            else ITF_ASSERT(0);
        }


    }

    void RemoteProfilerPlugin::draw()
    {
        if(!m_showAABB)
            return;

        for (u32 i = 0; i < m_listShowAABB.size(); i++)
        {        
            ObjectRef objRef(m_listShowAABB[i]);
            const Pickable* pObj = static_cast<const Pickable*>(GETOBJECT(objRef));
            if(pObj)
            {
                GFX_ADAPTER->drawDBGAABB(pObj->getAABB(), 0.1f, 0.8f, 0.1f, 0.0f, 2.0f, pObj->getDepth());
            }
            
        }
    }

    /************************************************************************/
    /* Sending messages                                                     */
    /************************************************************************/



    void RemoteProfilerPlugin::sendList()
    {
        if (PROFILERMANAGER->getProfileSerialized() == 0)
            return;

        ArchiveMemoryWriter& archMem = PROFILERMANAGER->getArchiveMemory();
      
        const ITF_MAP<ObjectRef, ProfilerStatesData*> states = PROFILERMANAGER->getListProfilerStatesData();
        ITF_MAP<ObjectRef, ProfilerStatesData*>::const_iterator it = states.begin();
        ITF_MAP<ObjectRef, ProfilerStatesData*>::const_iterator end = states.end();

        archMem.serialize((u32)states.size());

        for (; it != end; ++it)
        {
           archMem.serialize(it->first.getValue()); // objectRef
           const ProfilerStatesData* state = it->second;
           archMem.serialize(state->enableUpdate);
           archMem.serialize(state->enableDraw);
        }

        PROFILERMANAGER->updateArchiveHeader(btrue);

        Blob b(BLOB_EXTRACT_AND_WRITE_ENABLED,archMem.getSize()+2048);
        b.pushString(RemoteProfilerPlugin::getPluginName());
        b.pushString(getSendListCmdName());
        b.pushBinaryBlock(archMem.getData(),archMem.getSize());

        m_connected = PLUGINGATEWAY->sendPacket(b, m_editorPeer, m_editorSenderID);
    }

    void RemoteProfilerPlugin::sendMoreInformtion()
    {
        Blob b(BLOB_EXTRACT_AND_WRITE_ENABLED);
        b.pushString(RemoteProfilerPlugin::getPluginName());
        b.pushString(getSendMoreInformationCmdName());

        // streaming
        b.pushBool(bfalse);


        m_connected = PLUGINGATEWAY->sendPacket(b, m_editorPeer, m_editorSenderID);
    }

    void RemoteProfilerPlugin::sendSessionInfo()
    {
        Blob b(BLOB_EXTRACT_AND_WRITE_ENABLED);
        String path = DATA_FOLDER;

        if ( !path.isEmpty() )
        {
            path.replace('/', '\\');
            path.truncate(path.getLen()-1);
        }
        b.pushString(RemoteProfilerPlugin::getPluginName());
        b.pushString(getSendSessionInfoCmdName()); 
        b.pushString(path);
        PLUGINGATEWAY->sendPacket(b, RemoteProfilerPlugin::getNetPeer(), RemoteProfilerPlugin::getSenderID());
    }


    /************************************************************************/
    /* Process receive messages                                             */
    /************************************************************************/
    void RemoteProfilerPlugin::onSetTabMonitorSelected(class Blob& _blob )
    {
        m_tabMonitorSelected = (TabMonitor)_blob.extractInt32();

        if(m_tabMonitorSelected == TabRuntime)
        {
            sendList();
            sendMoreInformtion();
        }
        else if (m_tabMonitorSelected == TabLoading)
        {
            
        }
    }

    void RemoteProfilerPlugin::onSetMonitoring(class Blob& _blob )
    {
        m_monitoring = _blob.extractBool();
    }

    void RemoteProfilerPlugin::onSetUpdate(class Blob& _blob )
    {
        u32 objectRef = _blob.extractUint32();
        bbool state = _blob.extractBool();

        PROFILERMANAGER->setEnableUpdateObject(objectRef, state);
    }

    void RemoteProfilerPlugin::onSetDraw(class Blob& _blob )
    {
        u32 objectRef = _blob.extractUint32();
        bbool state = _blob.extractBool();

        PROFILERMANAGER->setEnableDrawObject(objectRef, state);
    }

    void RemoteProfilerPlugin::onBreakUpdate(class Blob& _blob )
    {
        u32 objectRef = _blob.extractUint32();
        PROFILERMANAGER->setEnableBreakObject(objectRef);
    }

    void RemoteProfilerPlugin::onBreakDraw(class Blob& _blob )
    {
        /*u32 objectRef =*/ _blob.extractUint32();

    }

    void RemoteProfilerPlugin::onLoadCapture(class Blob& _blob)
    {
         m_startLoadCapture = _blob.extractBool();
         if (m_startLoadCapture == btrue)
         {
             //start a capture
             BundleProfileManager::get().setEnable(btrue);
         }
         else
         {
             //stop a capture
             Blob bWrite(BLOB_EXTRACT_AND_WRITE_ENABLED);
             bWrite.pushString(RemoteProfilerPlugin::getPluginName());
             bWrite.pushString(getSendLoadCapture()); 
             BundleProfileManager::get().setEnable(bfalse);
             BundleProfileManager::get().writeBlob(bWrite);

             PLUGINGATEWAY->sendPacket(bWrite, RemoteProfilerPlugin::getNetPeer(), RemoteProfilerPlugin::getSenderID());
         }
    }

    void RemoteProfilerPlugin::onProfileEvent(class Blob& _blob )
    {
        u32 objectRef = _blob.extractUint32();
        PROFILERMANAGER->setEnableEventObject(objectRef);

    }

    void RemoteProfilerPlugin::onSetShowAABB(class Blob& _blob)
    {
        m_showAABB = _blob.extractBool();
        
        if(m_showAABB)
        {            
            m_listShowAABB.clear();

            u32 count = (u32)_blob.extractInt32();
            for (u32 i = 0; i < count; i++)
            {
                u32 objectRef = _blob.extractUint32();
                m_listShowAABB.push_back(objectRef);
            }
        }
    }


    void RemoteProfilerPlugin::onCameraGoto(class Blob& _blob)
    {
        u32 obj = _blob.extractUint32();
        ObjectRef objref(obj);

        const Pickable* pObj = static_cast<const Pickable*>(GETOBJECT(objref));
        if (pObj)
        {
            Vec3d pos(pObj->getPos().getX(),pObj->getPos().getY(),pObj->getDepth()+13.0f);
            CAMERACONTROLLERMANAGER->startCameraSequence(pObj->getPos(),1.0f,13.0f); 
            m_cameraFollow = objref;
        }

    }


    /************************************************************************/
    /* Pressing                                                             */
    /************************************************************************/
    void RemoteProfilerPlugin::fillAndResolve()
    {
        if(PROFILERMANAGER == NULL)
            return;
    }

    void RemoteProfilerPlugin::ResolveInfoAndAddingList( const ProfilerData& data,f32 _frequency)
    {

        ProfilerInfo info;

        info.objectRef = data.objectRef;
        info.timeToUpdate = static_cast<f32> (PROFILER_CONVERT_TOMS(data.deltaTimeToUpdate,_frequency));

        BaseObject* baseObject = data.objectRef.getObject();
        if(baseObject)
        {
            // type and file name
            if (baseObject->getObjectType() == BaseObject::eActor)
            {
                Actor* act =(Actor*)baseObject;
                info.friendlyName = act->getUserFriendly();
                //info.fileName   = "act->getLua().getString().cStr()";
            }
            else if (baseObject->getObjectType() == BaseObject::eFrise)
            {
                Frise* frize = (Frise*)baseObject;
                info.friendlyName = frize->getUserFriendly();
                //info.fileName   = "frize->getConfigNamePath().getString().cStr()";
            }
            listProfilerInfo.push_back(info);
        }
    }

} // namespace ITF

#endif //!ITF_FINAL