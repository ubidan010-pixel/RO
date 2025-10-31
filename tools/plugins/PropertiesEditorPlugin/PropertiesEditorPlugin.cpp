// GLOBAL ENGINE CONFIG FILE. Must be the first included file
#ifndef _ENGINECOMMON_H_
    #include "core/enginecommon.h"
#endif // _ENGINECOMMON_H_

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_PROPERTIESEDITORPLUGIN_H_
#include "tools/plugins/PropertiesEditorPlugin/PropertiesEditorPlugin.h"
#endif //_ITF_PROPERTIESEDITORPLUGIN_H_

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

#ifndef _ITF_ID_SERVER_H_
#include "core/IDserver.h"
#endif //_ITF_ID_SERVER_H_

#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef SERIALIZEROBJECTLUA_H
#include "core/serializer/ZSerializerObjectLUA.h"
#endif // SERIALIZEROBJECTLUA_H

#ifndef _ITF_UNDOREDOMANAGER_H_
#include "editor/UndoRedoManager.h"
#endif //_ITF_UNDOREDOMANAGER_H_

namespace ITF
{

    PropertiesEditorPlugin::PropertiesEditorPlugin()
        : m_currentSelection(ITF_INVALID_OBJREF)
        , m_toolIsAware(bfalse)
    {
    }

    PropertiesEditorPlugin::~PropertiesEditorPlugin()
    {
    }

    void PropertiesEditorPlugin::receive(Blob& _blob,const NETPeer* _peer, u32 _senderID)
    {
        String message;

        _blob.extractString(message);

        if ( message == getCommandPropertiesName() )
        {
            onPropertiesCmd(_blob);
        }
        else if(message == getSessionInfoCmdName())
        {
            onGetSessionInfoCmd(_blob);
        }
        else if ( message == getCommandDumpToFileName() )
        {
            onDumpToFileCmd(_blob);
        }
    }

    void PropertiesEditorPlugin::onPropertiesCmd( Blob& _blob )
    {
        String val;
        ObjectRef objRef = _blob.extractUint32();
        _blob.extractString(val);

        BaseObject* obj = GETOBJECT(objRef);

        if ( obj )
        {
            Pickable* pPick = static_cast<Pickable*>(obj);
            if (pPick->getScene())
                UNDOREDO_MANAGER->preChange(pPick);

            SerializerToolXML serializer;
            uSize size = 0;
            char* buffer = NULL;

            StringConverter converted(val);

            serializer.OpenToRead(converted.getChar());

            obj->Serialize(&serializer,ESerialize_PropertyEdit_Load);

            serializer.Close();
            serializer.OpenToWrite(&buffer, &size);

            obj->Serialize(&serializer,ESerialize_PropertyEdit_Save);
            serializer.Close();

            PLUGINGATEWAY->onObjectChanged(obj);
            pPick->notifyParentsAndChildren(ChangeType_Changed);

            if(size)
            {
                SF_DEL_ARRAY(buffer);
            }
        }
    }

    void PropertiesEditorPlugin::onGetSessionInfoCmd(Blob& _blob )
    {
        Blob b(BLOB_EXTRACT_AND_WRITE_ENABLED);
        String path = DATA_FOLDER;
        path.replace('/', '\\');
        path.truncate(path.getLen()-1);
        b.pushString(PropertiesEditorPlugin::getPluginName());  // plugin ID
        b.pushString(getSessionInfoCmdName()); 
        b.pushString(path);        
        PLUGINGATEWAY->sendPacket(b, m_editorPeer, m_editorSenderID);
    }

    void PropertiesEditorPlugin::onDumpToFileCmd( Blob& _blob )
    {
        String val;

        _blob.extractString(val);

        BaseObject* obj = GETOBJECT(m_currentSelection);

        if ( obj )
        {
            CSerializerObjectLua serializer;

            if ( serializer.OpenToWrite(val) )
            {
                obj->Serialize(&serializer,ESerialize_Data_Save);
                serializer.Close();
            }
        }
    }

    void PropertiesEditorPlugin::onFrisePicked(ObjectRef _frise)
    {
        const BaseObject* pObj = GETOBJECT(_frise);
        ITF_ASSERT(pObj && pObj->getObjectType() == BaseObject::eFrise);

        onBaseObjectPicked(_frise);
    }

    void PropertiesEditorPlugin::onActorPicked( ObjectRef _actor )
    {
        const BaseObject* pObj = GETOBJECT(_actor);
        ITF_ASSERT(pObj && pObj->getObjectType() == BaseObject::eActor);

        onBaseObjectPicked(_actor);
    }

    void PropertiesEditorPlugin::onBaseObjectPicked( ObjectRef _baseobj )
    {
        BaseObject* obj = GETOBJECT(_baseobj);
        if ( obj )
        {
            m_currentSelection = _baseobj;

            if ( m_editorPeer )
            {
                SerializerToolXML serializer;
                char* buffer = NULL;
                uSize size = 0;

                serializer.OpenToWrite(&buffer, &size);

                obj->Serialize(&serializer,ESerialize_PropertyEdit_Save);

                serializer.Close();

                sendProperties(_baseobj, buffer);

                if(size)
                {
                    SF_DEL_ARRAY(buffer);
                }


                return;
            }
        }

		if(m_editorPeer)
			sendClearSelection();
    }

    void PropertiesEditorPlugin::UnselectAll(bbool /*_force*/)
    {
        m_currentSelection = ITF_INVALID_OBJREF;
        if(m_editorPeer)
            sendClearSelection();
    }

    void PropertiesEditorPlugin::sendProperties(ObjectRef _objref, const String8& _data)
    {
        ITF_ASSERT(m_editorPeer);

        ITF::Blob blob(ITF::BLOB_EXTRACT_AND_WRITE_ENABLED);
        blob.pushString(getPluginName());
        blob.pushString(getCommandPropertiesName());
        blob.pushUInt32(_objref.getValue());
        blob.pushString8(_data);

        const bbool bSendOk = PLUGINGATEWAY->sendPacket(blob, m_editorPeer, m_editorSenderID);
        if(!bSendOk)
            m_editorPeer = NULL;

        m_toolIsAware = bSendOk;
    }

    void PropertiesEditorPlugin::sendClearSelection()
    {
        ITF_ASSERT(m_editorPeer);

        ITF::Blob blob(ITF::BLOB_EXTRACT_AND_WRITE_ENABLED);
        blob.pushString(getPluginName());
        blob.pushString(getCommandClearName());

        const bbool bSendOk = PLUGINGATEWAY->sendPacket(blob, m_editorPeer, m_editorSenderID);
        if(!bSendOk)
            m_editorPeer = NULL;

        m_toolIsAware = bSendOk;
    }

    void PropertiesEditorPlugin::update()
    {
        // If we have a selection and the tool don't knows anything about it
        if(m_editorPeer && !m_toolIsAware && m_currentSelection != ITF_INVALID_OBJREF)
            onBaseObjectPicked(m_currentSelection);
    }

    void PropertiesEditorPlugin::onObjectChanged( BaseObject* _pObject )
    {
        if(_pObject->getRef() == m_currentSelection)
            m_toolIsAware = bfalse;
    }

    void PropertiesEditorPlugin::onToolConnect( const NETPeer* _peer, u32 _senderID )
    {
        Plugin::onToolConnect(_peer, _senderID);

        m_toolIsAware = bfalse;
    }

} // namespace ITF

