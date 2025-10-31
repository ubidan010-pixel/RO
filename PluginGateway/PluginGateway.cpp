#include "precompiled_plugingateway.h"

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef ITF_CONSOLE_FINAL

#ifndef _ITF_PLUGINGATEWAY_H_
#include "PluginGateway/PluginGateway.h"
#endif //_ITF_PLUGINGATEWAY_H_

#ifndef _ITF_PLUGIN_H_
#include "PluginGateway/Plugin.h"
#endif //_ITF_PLUGIN_H_

#ifndef _ITF_SINGLETONS_H_
#include "core/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#if !defined( ITF_CTR)
    #ifndef _ITF_TCPADAPTER_H_
    #include "engine/AdaptersInterfaces/TCPAdapter.h"
    #endif //_ITF_TCPADAPTER_H_

    #ifndef _ITF_TCPADAPTER_ITF_H_
    #include "adapters/TCPAdapter_ITF/TCPAdapter_ITF.h"
    #endif //_ITF_TCPADAPTER_ITF_H_

    #ifndef _ITF_NETPACKETHANDLER_H_
    #include "engine/network/NETPacketHandler.h"
    #endif //_ITF_NETPACKETHANDLER_H_

#endif // ! ITF_CTR

#ifndef _ITF_INPUTADAPTER_H_
#include "engine/AdaptersInterfaces/InputAdapter.h"
#endif //_ITF_INPUTADAPTER_H_

#ifndef _ITF_THREAD_H_
#include "engine/system/Thread.h"
#endif //_ITF_THREAD_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_EDITOR_H_
#include "editor/editor.h"
#endif //_ITF_EDITOR_H_

#ifndef _ITF_PICKINGSHAPE_H_
#include "editor/PickingShape.h"
#endif //_ITF_PICKINGSHAPE_H_

namespace ITF
{
#define CALL_PLUGINS(_expr_)\
{\
ITF_VECTOR<PlugEntry>::const_iterator it;\
for (it = m_plugins.begin(); it != m_plugins.end(); ++it)\
{\
Plugin* pPlugin = (*it).m_plug;\
if (pPlugin)\
pPlugin->_expr_;\
}\
}

    bbool isHiddenBecauseOfEditorDefault(const Pickable* /*_pick*/)
    {
        return bfalse;  // default implementeation
    }

    bbool (*isHiddenBecauseOfEditorFilter)(const Pickable*) = isHiddenBecauseOfEditorDefault;


    u16 PluginGateWay::m_defaultPort = 1001;

    void PluginGateWay::createSingleton()
    {
        ITF_ASSERT(!PLUGINGATEWAY);
        SINGLETONS.setPluginGateWay(newAlloc(mId_System, PluginGateWay()));
        if (INPUT_ADAPTER)
            INPUT_ADAPTER->addListener(PLUGINGATEWAY, 2);
    }

    PluginGateWay* PluginGateWay::getSingleton (   )
    {
        return SINGLETONS.getPluginGateWay();
    }

    PluginGateWay::PluginGateWay()
    {
        m_TCPAdapter = NULL;
        startReceive(NETPACKET_HANDLER);
    }

    const NETPeer*  PluginGateWay::getHost() const
    {
#ifdef ITF_CTR
        return NULL;
#else
        return m_TCPAdapter->getHost();
#endif
    }

    PluginGateWay::~PluginGateWay()
    {
        if(SCENE_MANAGER)
            SCENE_MANAGER->removeListener(this);
        WorldManager::removeListener(this);
        shutdown();
    }

    void PluginGateWay::destroySingleton()
    {
        if(PLUGINGATEWAY)
        {
            if (INPUT_ADAPTER)
                INPUT_ADAPTER->removeListener(PLUGINGATEWAY);

            delete SINGLETONS.getPluginGateWay();
            SINGLETONS.setPluginGateWay(NULL);
        }
    }

    bbool PluginGateWay::init( const char* _ip, u16 _port, bbool _isServer ,bbool bRaiseError /*true*/ )
    {
#ifndef ITF_CTR
        //because we allow to retry a connection...
        if (!m_TCPAdapter)
        {
            m_TCPAdapter = newAlloc(mId_System, TCPAdapter_ITF());

            SCENE_MANAGER->addListener(this);
            WorldManager::addListener(this);
        }

        if (_isServer)
        {
            m_isRunningAsServer = btrue;
            if (!m_TCPAdapter->setHost(_ip, _port))
            {
                if (bRaiseError)
                {
                    LOG("ITF_FATAL_ERROR: Could not connect PluginGateWay as Host to IP %s, port %d", _ip, _port);
                }
                return bfalse;
            }
        }
        else
        {
            m_isRunningAsServer = bfalse;
            m_TCPAdapter->initialize();
            m_TCPAdapter->setClient();
            if (!m_TCPAdapter->hostConnection(_ip, _port))
            {
                /*
                if (bRaiseError)
                {
                    ITF_FATAL_ERROR("ITF_FATAL_ERROR: Could not connect PluginGateWay as Client to IP %s, port %d\nLaunch Techno first", _ip, _port);
                }
                */
                
                return bfalse;
            }
        }

#else
        m_isRunningAsServer = bfalse;
#endif
        return btrue;

    }

    void PluginGateWay::shutdown( )
    {
#ifndef ITF_CTR
        if(m_TCPAdapter)
        {
            m_TCPAdapter->destroy();
            SF_DEL(m_TCPAdapter);
        }
#endif
        stopReceive(NETPACKET_HANDLER);
        ITF_VECTOR<PlugEntry>::iterator it;
        for (it = m_plugins.begin(); it != m_plugins.end(); ++it)
        {
            SF_DEL((*it).m_plug);
        }
        m_plugins.clear();
    }


    //////////////////////////////////////////////////////////////////////////
    void PluginGateWay::addPlugin( const String& _name, Plugin* _plug, BaseObject::ObjectType _isMainEditorforType, const SafeArray<BaseObject::ObjectType>* _getEventsForTypes )
    {
        if(getPluginByName(_name) != NULL)
        {
            LOG("[PluginGateWay] adding a plugin already added : %S", _name.cStr());
            return;
        }

        m_plugins.push_back(PlugEntry(_name, _plug, _isMainEditorforType, _getEventsForTypes));
    }
    
    //////////////////////////////////////////////////////////////////////////
    void PluginGateWay::getPluginList(ITF_VECTOR<Plugin*> &_pluginList)
    {
        _pluginList.resize(m_plugins.size());
        ITF_VECTOR<PlugEntry>::iterator iter;
        int index=0;
        for (iter=m_plugins.begin(); iter!=m_plugins.end(); iter++)
            _pluginList[index++] = iter->m_plug;
    }

    //////////////////////////////////////////////////////////////////////////
    Plugin* PluginGateWay::getPluginByName ( const String& _name   )
    {
        ITF_VECTOR<PlugEntry>::iterator it;
        for (it = m_plugins.begin(); it != m_plugins.end(); ++it)
        {
            if ((*it).m_name == _name)
                return (*it).m_plug;
        }
        return NULL;
    }

    //////////////////////////////////////////////////////////////////////////
    void PluginGateWay::update()    
    {
#ifndef ITF_CTR
        ITF::NETPACKET_HANDLER->update(m_TCPAdapter);
#endif
        ITF_VECTOR<PlugEntry>::iterator it;
        for (it = m_plugins.begin(); it != m_plugins.end(); ++it)
        {
            if (!((*it).m_isPaused))
            {
                if ((*it).m_plug)
                    (*it).m_plug->update();
            }
        }
    }
    
    void PluginGateWay::draw()    
    {
        CALL_PLUGINS(draw())
    }

    void PluginGateWay::onFrisePicked(ObjectRef _frise)
    {
        CALL_PLUGINS(onFrisePicked(_frise))
    }

    void PluginGateWay::onActorPicked(ObjectRef _actor)
    {
        CALL_PLUGINS(onActorPicked(_actor))
    }

    void PluginGateWay::onActorSubEdit(ObjectRef _actor)
    {
        CALL_PLUGINS(onActorSubEdit(_actor))
    }

    void PluginGateWay::onStartRotation()
    {
        CALL_PLUGINS(onStartRotation())
    }

    void PluginGateWay::onEndRotation()
    {
        CALL_PLUGINS(onEndRotation())
    }

    void PluginGateWay::onCameraModeChange (int _prevMode, int newMode)
    {
        CALL_PLUGINS(onCameraModeChange(_prevMode, newMode))
    }

    bbool PluginGateWay::isSubEditAllowed ()
    {
        ITF_VECTOR<PlugEntry>::iterator it;
        for (it = m_plugins.begin(); it != m_plugins.end(); ++it)
        {// All plugins should be asked INCLUDING those who don't have the focus.
            Plugin* pPlugin = (*it).m_plug;
            if (pPlugin && !pPlugin->isSubEditAllowed())
                return bfalse;
        }
        return btrue;
    }

    void PluginGateWay::onSceneSave()
    {
        ITF_VECTOR<PlugEntry>::iterator it;
        for (it = m_plugins.begin(); it != m_plugins.end(); ++it)
        {// All plugins should be warned INCLUDING those who don't have the focus.
            Plugin* pPlugin = (*it).m_plug;
            if (pPlugin)
                pPlugin->onSceneSave();
        }
    }


    bbool PluginGateWay::onPickShape(PickingShape* _shape)
    {
        bbool bPickingHandled = bfalse;
        ITF_VECTOR<PlugEntry>::iterator it;
        for (it = m_plugins.begin(); it != m_plugins.end(); ++it)
        {
            if ((*it).m_hasFocus)
            {
                Plugin* pPlugin = (*it).m_plug;
                if (pPlugin)
                    bPickingHandled |= pPlugin->onPickShape(_shape);
            }
        }
        return bPickingHandled;
    }

    bbool PluginGateWay::onEditorRemove(const SafeArray<PickingShape*>& _shapesWithSameOwner)
    {
        bbool bResult = bfalse;
        ITF_VECTOR<PlugEntry>::iterator it;
        for (it = m_plugins.begin(); it != m_plugins.end(); ++it)
        {
            if ((*it).m_hasFocus)
            {
                Plugin* pPlugin = (*it).m_plug;

                if(pPlugin && pPlugin->onEditorRemove(_shapesWithSameOwner))
                {
                    bResult = btrue;
                    break;
                }
            }
        }
        return bResult;
    }

    bbool PluginGateWay::receive(Blob& _blob, const NETPeer* _peer, u32 _senderID, bbool _unknowListener)
    {
        String pluginName;
        _blob.extractString(pluginName);
        if (pluginName == "PluginId")
        {
            _blob.extractString(pluginName);
            Plugin* plug = getPluginByName(pluginName);
            if(plug)
            {
                plug->onToolConnect(_peer, _senderID);
    
                // Send the depot path to the tool
                Blob bob(BLOB_EXTRACT_AND_WRITE_ENABLED);
                bob.pushString(pluginName);
                bob.pushString(COMMAND_DEPOTPATH);
                bob.pushString(DATA_FOLDER);
                ITF_VERIFY(sendPacket(bob, _peer, _senderID));
            }            
            return btrue;
        }
        else
        {
            Plugin* plug = getPluginByName(pluginName);
            if(plug)
            {
                plug->receive(_blob, _peer, _senderID);
                return btrue;
            }
        }
        return bfalse;
    }


    bbool PluginGateWay::sendPacket(Blob& _blob,const NETPeer* _peer, u32 _receiverID )const
    {
         return send(NETPACKET_HANDLER,_blob,_peer,_receiverID);
    }
    
    bbool PluginGateWay::sendToHost( Blob& _blob  )
    {
#ifndef ITF_CTR
        ITF_ASSERT(m_TCPAdapter);
        ITF_ASSERT(!m_isRunningAsServer);

        return send(NETPACKET_HANDLER,_blob, m_TCPAdapter->getHost());
#else
        return bfalse;
#endif
    }


    void PluginGateWay::onPostSceneActivation(const Scene* _pScene, bbool _bActive)
    {
        ITF_VECTOR<PlugEntry>::iterator it;
        for (it = m_plugins.begin(); it != m_plugins.end(); ++it)
        {
            if ((*it).m_plug)
                (*it).m_plug->onPostSceneActivation(_pScene, _bActive);
        }
    }

    void PluginGateWay::onSceneAsyncLoadFinished(const Scene* _pScene)
    {
        ITF_VECTOR<PlugEntry>::iterator it;
        for (it = m_plugins.begin(); it != m_plugins.end(); ++it)
        {
            if ((*it).m_plug)
                (*it).m_plug->onSceneAsyncLoadFinished(_pScene);
        }
    }

    bbool PluginGateWay::onMouseButton(InputAdapter::MouseButton _but, InputAdapter::PressStatus _status )
    {
        if (!m_forwardMouseEvents)
            return btrue;
        ITF_VECTOR<PlugEntry>::iterator it;
        for (it = m_plugins.begin(); it != m_plugins.end(); ++it)
        {
            if ((*it).m_hasFocus && (*it).m_plug)
                (*it).m_plug->onMouseButton(_but, _status);
        }
        return btrue;
    }

    bbool    PluginGateWay::onMousePos(i32 _x, i32 _y)
    {
        if (!m_forwardMouseEvents)
            return btrue;
        ITF_VECTOR<PlugEntry>::iterator it;
        for (it = m_plugins.begin(); it != m_plugins.end(); ++it)
        {
            if ((*it).m_hasFocus && (*it).m_plug)
                (*it).m_plug->onMousePos(_x, _y);
        }
        return btrue;
    }

    bbool PluginGateWay::onMouseWheel(i32 _wheelValue, i32 _wheelDelta )
    {
        if (!m_forwardMouseEvents)
            return btrue;
        ITF_VECTOR<PlugEntry>::iterator it;
        for (it = m_plugins.begin(); it != m_plugins.end(); ++it)
        {
            if ((*it).m_hasFocus && (*it).m_plug)
                (*it).m_plug->onMouseWheel(_wheelValue, _wheelDelta );
        }
        return btrue;
    }


    bbool   PluginGateWay::onLog(const String& szExeName,const String& _szLogText)
    {

        ITF_VECTOR<PlugEntry>::iterator it;
        for (it = m_plugins.begin(); it != m_plugins.end(); ++it)
        {
            if ((*it).m_plug)
                (*it).m_plug->onLog(szExeName,_szLogText);
        }
        return btrue;
    }

    bbool PluginGateWay::onKey(i32 _key, InputAdapter::PressStatus _status )
    {
        ITF_VECTOR<PlugEntry>::iterator it;
        for (it = m_plugins.begin(); it != m_plugins.end(); ++it)
        {
            if ((*it).m_hasFocus && (*it).m_plug)
                (*it).m_plug->onKey(_key, _status);
        }
        return btrue;
    }

    bbool PluginGateWay::isPickingAllowed()
    {
        ITF_VECTOR<PlugEntry>::iterator it;
        for (it = m_plugins.begin(); it != m_plugins.end(); ++it)
        {
            if ((*it).m_hasFocus && (*it).m_plug)
            {
                bbool res = (*it).m_plug->isPickingAllowed();
                if (!res)
                    return bfalse;
            }
        }
        return btrue;
    }

    void  PluginGateWay::onClearScene( )
    {
        UnselectAll(btrue);

        ITF_VECTOR<PlugEntry>::iterator it;
        for (it = m_plugins.begin(); it != m_plugins.end(); ++it)
        {
            if ((*it).m_plug)
                (*it).m_plug->onClearScene();
        }
        setExclusiveFocus(NULL);
    }

    void PluginGateWay::PausePlugin     (  Plugin* _plug        )
    {
        ITF_VECTOR<PlugEntry>::iterator it;
        for (it = m_plugins.begin(); it != m_plugins.end(); ++it)
        {
            (*it).m_isPaused = btrue;
        }
    }
    
    void PluginGateWay::UnpausePlugin   (  Plugin* _plug        )
    {
        ITF_VECTOR<PlugEntry>::iterator it;
        for (it = m_plugins.begin(); it != m_plugins.end(); ++it)
        {
            (*it).m_isPaused = bfalse;
        }
    }
    
    void PluginGateWay::PauseAllExcept  (  Plugin* _plug        )
    {
        ITF_VECTOR<PlugEntry>::iterator it;
        for (it = m_plugins.begin(); it != m_plugins.end(); ++it)
        {
            if ((*it).m_plug == _plug)
                (*it).m_isPaused = bfalse;
            else
                (*it).m_isPaused = btrue;
        }
    }
    

    bbool PluginGateWay::hasExclusiveFocus(Plugin* _plug        )
    {
        if (_plug)
        {
            ITF_VECTOR<PlugEntry>::iterator it;
            for (it = m_plugins.begin(); it != m_plugins.end(); ++it)
            {
                if ((*it).m_plug == _plug)
                    return (*it).m_hasExclusiveFocus;
            }
        }
        ITF_ASSERT(0); // plug not found, we have a problem
        return bfalse;
    }


    Plugin* PluginGateWay::getExclusiveFocus()
    {
        ITF_VECTOR<PlugEntry>::iterator it;
        for (it = m_plugins.begin(); it != m_plugins.end(); ++it)
        {
            if ((*it).m_hasExclusiveFocus)
                return (*it).m_plug;
        }
        return NULL;
    }


    void PluginGateWay::setExclusiveFocus(Plugin* _plug        )
    {
        if (_plug)
        {
            ITF_VECTOR<PlugEntry>::iterator it;
            for (it = m_plugins.begin(); it != m_plugins.end(); ++it)
            {
                if ((*it).m_plug == _plug)
                {
                    (*it).m_hasFocus = btrue;
                    (*it).m_hasExclusiveFocus = btrue;
                }
                else
                {
                    (*it).m_hasFocus = bfalse;
                    (*it).m_hasExclusiveFocus = bfalse;
                    (*it).m_plug->UnselectAll(bfalse);
                }
            }
        }
        else
        {
            ITF_VECTOR<PlugEntry>::iterator it;
            for (it = m_plugins.begin(); it != m_plugins.end(); ++it)
            {
                (*it).m_hasFocus = btrue;
                (*it).m_hasExclusiveFocus = bfalse;
            }

        }
    }

    bbool PluginGateWay::bCanEnterEditMode(  )
    {
        ITF_VECTOR<PlugEntry>::iterator it;
        for (it = m_plugins.begin(); it != m_plugins.end(); ++it)
        {
            if (!(*it).m_isPaused && (*it).m_plug)
            {
                bbool res = (*it).m_plug->bCanEnterEditMode();
                if (!res)
                    return bfalse;
            }
        }
        return btrue;
    }
    
    bbool PluginGateWay::bCanLeaveEditMode(  )
    {
        ITF_VECTOR<PlugEntry>::iterator it;
        for (it = m_plugins.begin(); it != m_plugins.end(); ++it)
        {
            if (!(*it).m_isPaused && (*it).m_plug)
            {
                bbool res = (*it).m_plug->bCanLeaveEditMode();
                if (!res)
                    return bfalse;
            }
        }
        return btrue;
    }

    void PluginGateWay::UnselectAll(bbool _force, const Plugin* _exception)
    {
        ITF_VECTOR<PlugEntry>::iterator it;
        for (it = m_plugins.begin(); it != m_plugins.end(); ++it)
        {
            if ((*it).m_plug == _exception)
                continue;
            if (!(*it).m_isPaused && (*it).m_plug)
            {
                (*it).m_plug->UnselectAll(_force);
            }
        }
    }

    const String* PluginGateWay::getPluginName( const Plugin* _p) const
    {
        ITF_VECTOR<PlugEntry>::const_iterator it;
        for (it = m_plugins.begin(); it != m_plugins.end(); ++it)
        {
            if ((*it).m_plug == _p)
            {
                return &((*it).m_name);
            }
        }
        return NULL;
    }


    void PluginGateWay::fillEditorActiveList( ObjectRefList &_objects, PickingShapeVector& _orderedShapes, Scene* _pFilterScene ) const
    {
        CALL_PLUGINS(fillEditorActiveList(_objects, _orderedShapes, _pFilterScene))
/*        for (i32 i = 0; i < (i32)_objects.size(); i++)
        {
            Pickable* pick = (Pickable*)_objects[i].getObject();
            if (!pick || isHiddenBecauseOfEditorFilter(pick))
            {
                PickingShapeVector::iterator it = _orderedShapes.begin();
                while (it != _orderedShapes.end())
                {
                    if (_objects[i] == (*it)->getOwner())
                        it = _orderedShapes.erase(it);
                    else
                        ++it;
                }                
                _objects.eraseKeepOrder(i);
                i--;
            }
        }*/
    }

    void PluginGateWay::fillEditorForcedList( ObjectRefList &_objects, PickingShapeVector& _orderedShapes ) const
    {
        CALL_PLUGINS(fillEditorForcedList(_objects, _orderedShapes))
    }

    void PluginGateWay::onFileDrop (const String& _fileName)
    {
        CALL_PLUGINS(onFileDrop(_fileName))
    }

    bbool PluginGateWay::fillContextMenu(EditorContextMenu& _menu, ObjectRef _target, bbool _topItems)
    {
        bbool bShowMenu = btrue;
        ITF_VECTOR<PlugEntry>::iterator it;
        for (it = m_plugins.begin(); it != m_plugins.end(); ++it)
        {// All plugins should be warned INCLUDING those who don't have the focus.
            Plugin* pPlugin = (*it).m_plug;
            if (pPlugin)
                bShowMenu |= pPlugin->fillContextMenu(_menu, _target, _topItems);
        }
        return bShowMenu;
    }

    void PluginGateWay::fillAltContextMenu(EditorContextMenu& _menu)
    {
        ITF_VECTOR<PlugEntry>::iterator it;
        for (it = m_plugins.begin(); it != m_plugins.end(); ++it)
        {// All plugins should be warned INCLUDING those who don't have the focus.
            Plugin* pPlugin = (*it).m_plug;
            if (pPlugin)
                pPlugin->fillAltContextMenu(_menu);
        }
    }

    void PluginGateWay::onContextMenuItemSelected(ContextMenuItem* _item, ObjectRef _menuTarget)
    {
        CALL_PLUGINS(onContextMenuItemSelected(_item, _menuTarget))
    }
    
    void PluginGateWay::onDeleteScene(const Scene* _pScene)
    {
        if (!EDITOR)
            return; // can be called after editor destruction
        CALL_PLUGINS(onDeleteScene(_pScene))
    }

    void PluginGateWay::onSwitchToWorld(World* _pWorld, bbool _bSwitchDone)
    {
        CALL_PLUGINS(onSwitchToWorld(_pWorld, _bSwitchDone))
    }
    
    void PluginGateWay::onDeleteWorld(World* _pWorld)
    {
        if (!EDITOR)
            return; // can be called after editor destruction
        CALL_PLUGINS(onDeleteWorld(_pWorld))
    }

    void PluginGateWay::cancelCurrentAction(bbool _keepEditorStuff)
    {
        CALL_PLUGINS(cancelCurrentAction(_keepEditorStuff))
    }

    Plugin* PluginGateWay::getPluginForObject ( const BaseObject* _obj    )
    {
        ITF_VECTOR<PlugEntry>::iterator it;
        for (it = m_plugins.begin(); it != m_plugins.end(); ++it)
        {
            if ((*it).m_primaryObjType == _obj->getObjectType())
                return (*it).m_plug;
        }
        return NULL;
    }

    void PluginGateWay::onObjectCreated(BaseObject* pBO)const
    {
        ITF_VECTOR<PlugEntry>::const_iterator it;
        for (it = m_plugins.begin(); it != m_plugins.end(); ++it)
        {
            const PlugEntry& entry = *it;
            if (entry.m_primaryObjType == pBO->getObjectType() ||
                entry.m_secondaryObjTypes.find(pBO->getObjectType()) >= 0)
            {
                if (entry.m_plug)
                    entry.m_plug->onObjectCreated(pBO);
            }
        }

        if(EDITOR)
        {
            if(Pickable* pPickable = pBO->DynamicCast<Pickable>(ITF_GET_STRINGID_CRC(Pickable,594660361)))
            {
                EDITOR->setSceneAsModified(pPickable->getScene());
            }
        }
    }

    void PluginGateWay::onObjectDeleted(BaseObject* pBO)const
    {
        ITF_VECTOR<PlugEntry>::const_iterator it;
        for (it = m_plugins.begin(); it != m_plugins.end(); ++it)
        {
            const PlugEntry& entry = *it;
            if (entry.m_primaryObjType == pBO->getObjectType() ||
                entry.m_secondaryObjTypes.find(pBO->getObjectType()) >= 0)
            {
                if (entry.m_plug)
                    entry.m_plug->onObjectDeleted(pBO);
            }
        }

        if(EDITOR)
        {
            if(Pickable* pPickable = pBO->DynamicCast<Pickable>(ITF_GET_STRINGID_CRC(Pickable,594660361)))
            {
                EDITOR->setSceneAsModified(pPickable->getScene());
            }
        }
    }

    void PluginGateWay::onObjectChanged(BaseObject* pBO)const
    {
        ITF_VECTOR<PlugEntry>::const_iterator it;
        for (it = m_plugins.begin(); it != m_plugins.end(); ++it)
        {
            const PlugEntry& entry = *it;
            if (entry.m_primaryObjType == pBO->getObjectType() ||
                entry.m_secondaryObjTypes.find(pBO->getObjectType()) >= 0)
            {
                if (entry.m_plug)
                    entry.m_plug->onObjectChanged(pBO);
            }
        }

        if(EDITOR)
        {
            if(Pickable* pPickable = pBO->DynamicCast<Pickable>(ITF_GET_STRINGID_CRC(Pickable,594660361)))
            {
                EDITOR->setSceneAsModified(pPickable->getScene());
            }
        }
    }

    void PluginGateWay::onObjectSpawned( const BaseObject* _newObject ) const
    {
        ITF_VECTOR<PlugEntry>::const_iterator it;
        for (it = m_plugins.begin(); it != m_plugins.end(); ++it)
        {
            const PlugEntry& entry = *it;
            if (entry.m_primaryObjType == _newObject->getObjectType() ||
                entry.m_secondaryObjTypes.find(_newObject->getObjectType()) >= 0)
            {
                if (entry.m_plug)
                    entry.m_plug->onObjectSpawned(_newObject);
            }
        }

        if(EDITOR)
        {
            if(const Pickable* pPickable = _newObject->DynamicCast<Pickable>(ITF_GET_STRINGID_CRC(Pickable,594660361)))
            {
                EDITOR->setSceneAsModified(pPickable->getScene());
            }
        }
    }

    void PluginGateWay::onObjectsDuplicated( const SafeArray <BaseObject*>& _from, const SafeArray <BaseObject*>& _to ) const
    {
        ITF_ASSERT_CRASH(_from.size() == _to.size(), "Size must match ! Read the comment of the function...");
        CALL_PLUGINS(onObjectsDuplicated(_from, _to))

        if(EDITOR)
        {
            for(u32 i = 0; i < _to.size(); ++i)
            {
                if(Pickable* pPick = _to[i]->DynamicCast<Pickable>(ITF_GET_STRINGID_CRC(Pickable,594660361)))
                {
                    EDITOR->setSceneAsModified(pPick->getScene());
                }
            }
        }
    }

    void PluginGateWay::onCloseApp( ) const
    {
        CALL_PLUGINS(onCloseApp())
    }

    void PluginGateWay::enterSubEdit( ObjectRef _or )
    {
        CALL_PLUGINS(enterSubEdit(_or))
    }

    bbool PluginGateWay::isSubEditable( const BaseObject* _pObj ) const
    {
        bbool result = bfalse;
        ITF_VECTOR<PlugEntry>::const_iterator it;
        for (it = m_plugins.begin(); it != m_plugins.end(); ++it)
        {// All plugins should be warned INCLUDING those who don't have the focus.
            const Plugin* pPlugin = (*it).m_plug;
            if(pPlugin && pPlugin->isSubEditable(_pObj))
            {
                result = btrue;
                break;
            }
        }
        return result;
    }

    void PluginGateWay::onObjectAdded( const Scene* _pScene, BaseObject* _pObj )
    {
        CALL_PLUGINS(onObjectAdded(_pScene, _pObj))
    }
    
    void PluginGateWay::onObjectRemoved( const Scene* _pScene, BaseObject* _pObj, bbool _delete )
    {
        CALL_PLUGINS(onObjectRemoved(_pScene, _pObj, _delete))
    }

    void PluginGateWay::pushInfoFlags(bbool _clear)
    {
        CALL_PLUGINS(pushInfoFlags(_clear))
    }
    
    void PluginGateWay::popInfoFlags()
    {
        CALL_PLUGINS(popInfoFlags())
    }

    void PluginGateWay::switchInfoFlags()
    {
        CALL_PLUGINS(switchInfoFlags())
    }

    void PluginGateWay::onPickableRenamed( Pickable* _pObj, const String8& _previousUserFriendly )
    {
        CALL_PLUGINS(onPickableRenamed(_pObj, _previousUserFriendly))

        onObjectChanged(_pObj);
    }

    void PluginGateWay::startEditor()
    {
        CALL_PLUGINS(startEditor())
    }

    void PluginGateWay::stopEditor()
    {
        CALL_PLUGINS(stopEditor())
    }

    void PluginGateWay::onSetShapePosition( PickingShape* _pPickingShape, const Vec2d& _screenspacepos )
    {
        Pickable* obj = static_cast<Pickable*>(_pPickingShape->getOwnerPtr());

        ITF_VECTOR<PlugEntry>::const_iterator it;
        for (it = m_plugins.begin(); it != m_plugins.end(); ++it)
        {
            const PlugEntry& entry = *it;
            if (entry.m_primaryObjType == obj->getObjectType())
            {
                if (entry.m_plug)
                {
                    if(entry.m_plug->onSetShapePosition(_pPickingShape, _screenspacepos))
                    {
                        if(EDITOR)
                        {
                            EDITOR->setSceneAsModified(obj->getScene());
                        }
                        return;
                    }
                }
            }
        }

        ITF_ASSERT_MSG(0, "No plugin was able to manage onSetShapePosition call");
    }

    void PluginGateWay::onOffsetShapePosition( PickingShape* _pPickingShape, const Vec3d& _deltaPos )
    {
        Pickable* obj = static_cast<Pickable*>(_pPickingShape->getOwnerPtr());

        ITF_VECTOR<PlugEntry>::const_iterator it;
        for (it = m_plugins.begin(); it != m_plugins.end(); ++it)
        {
            const PlugEntry& entry = *it;
            if (entry.m_primaryObjType == obj->getObjectType())
            {
                if (entry.m_plug)
                {
                    if(entry.m_plug->onOffsetShapePosition(_pPickingShape, _deltaPos))
                    {
                        if(EDITOR)
                        {
                            EDITOR->setSceneAsModified(obj->getScene());
                        }
                        return;
                    }
                }
            }
        }

        ITF_ASSERT_MSG(0, "No plugin was able to manage onOffsetShapePosition call");
    }
	void PluginGateWay::OnFileMissing(const String& _actorFileName, Path& _luaPath )
	{
		CALL_PLUGINS(OnFileMissing(_actorFileName,_luaPath));
	}

	void PluginGateWay::OnRefMissing(const char* filename,const String& reference)
	{
		CALL_PLUGINS(OnRefMissing(filename,reference));
	}

	void PluginGateWay::OnDataError(const String& _str,Pickable* pickable)
	{
		CALL_PLUGINS(OnDataError(_str,pickable));
	}

	void PluginGateWay::OnMapLoaded(Scene* rootScene)
	{
		CALL_PLUGINS(OnMapLoaded(rootScene));
	}

	void  PluginGateWay::OnFatalError(const char* msg)
	{
		CALL_PLUGINS(OnFatalError(msg));
	}
	void  PluginGateWay::OnWarning(const char* msg)
	{
		CALL_PLUGINS(OnWarning(msg));
	}

	void  PluginGateWay::OnError(const char* msg)
	{
		CALL_PLUGINS(OnError(msg));
	}

    void  PluginGateWay::OnWorldUpdated()
    {
        CALL_PLUGINS(OnWorldUpdated());
    }

  void PluginGateWay::onPickableSceneChanged( Pickable* _pPickable, Scene* _src, Scene *_dst )
    {
        CALL_PLUGINS(onPickableSceneChanged(_pPickable, _src, _dst));
#ifdef ITF_SUPPORT_EDITOR
        EDITOR->setSceneAsModified(_src);
        EDITOR->setSceneAsModified(_dst);
#endif //ITF_SUPPORT_EDITOR
    }
} // namespace ITF

#endif //!ITF_CONSOLE_FINAL
