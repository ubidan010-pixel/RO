// GLOBAL ENGINE CONFIG FILE. Must be the first included file
#ifndef _ENGINECOMMON_H_
#include "core/enginecommon.h"
#endif // _ENGINECOMMON_H_

#if !defined(ITF_FINAL) && defined(ITF_DEBUG_STRINGID)


#ifndef _ITF_ACTORCREATEPLUGIN_H_
#include "tools/plugins/ActorCreatePlugin/ActorCreatePlugin.h"
#endif //_ITF_ACTORCREATEPLUGIN_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_


#ifndef SERIALIZERTOOLXML_H
#include "core/serializer/ZSerializerToolXML.h"
#endif // SERIALIZERTOOLXML_H

#ifndef SERIALIZEROBJECTBINARY_H
#include "core/serializer/ZSerializerObjectBinary.h"
#endif // SERIALIZEROBJECTBINARY_H

#ifndef SERIALIZEROBJECTLOGICDB_H
#include "engine/serializer/ZSerializerObjectLogicDB.h"
#endif //SERIALIZEROBJECTLOGICDB_H

#ifndef SERIALIZEROBJECTLUA_H
#include "core/serializer/ZSerializerObjectLUA.h"
#endif // SERIALIZEROBJECTLUA_H

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_BLOBUTILS_H_
#include "core/blobUtils.h"
#endif //_ITF_BLOBUTILS_H_

#ifndef _ITF_PLUGINGATEWAY_H_
#include "PluginGateway/PluginGateway.h"
#endif //_ITF_PLUGINGATEWAY_H_

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_


#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_DIRECTORY_H_
#include "core/file/Directory.h"
#endif //_ITF_DIRECTORY_H_

#ifndef _ITF_FILEMANAGER_ITF_H_
#include "adapters/FileManager_ITF/FileManager_ITF.h"
#endif //_ITF_FILEMANAGER_ITF_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_ACTORCREATE_ANIMLIGHT_H_
#include "tools/plugins/ActorCreatePlugin/ActorCreate_AnimLight.h"
#endif //_ITF_ACTORCREATE_ANIMLIGHT_H_

#ifndef _ITF_ACTORCREATE_ANIMATEDCOMPONENT_H_
#include "tools/plugins/ActorCreatePlugin/ActorCreate_AnimatedComponent.h"
#endif //_ITF_ACTORCREATE_ANIMATEDCOMPONENT_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_EDITOR_H_
#include "editor/Editor.h"
#endif //_ITF_EDITOR_H_

#ifndef _ITF_PICKINGSHAPE_H_
#include "editor/PickingShape.h"
#endif //_ITF_PICKINGSHAPE_H_

#ifndef _ITF_SUBSCENEACTOR_H_
#include "engine/actors/SubSceneActor.h"
#endif //_ITF_SUBSCENEACTOR_H_

#ifndef _ITF_UNDOREDOMANAGER_H_
#include "editor/UndoRedoManager.h"
#endif //_ITF_UNDOREDOMANAGER_H_

#ifndef _ITF_ACTOREDITOR_H_
#include "tools/plugins/ActorEditor/ActorEditor.h"
#endif //_ITF_ACTOREDITOR_H_

namespace ITF
{
    BEGIN_SERIALIZATION(ActorEditorParamsFiles)
        SERIALIZE_MEMBER("component",m_component);
        SERIALIZE_MEMBER("path",m_path);
        SERIALIZE_ENUM_BEGIN("save",m_saveEnable);
            SERIALIZE_ENUM_VAR(noSave);
            SERIALIZE_ENUM_VAR(saveInActorBase);
            SERIALIZE_ENUM_VAR(saveInComponent);
        SERIALIZE_ENUM_END();
    END_SERIALIZATION()

    BEGIN_SERIALIZATION(ActorEditorParams)
        SERIALIZE_CONTAINER_OBJECT("componentFiles", m_componentFiles);
        SERIALIZE_MEMBER("indentationNbChars",m_indentationNbChars);

        SERIALIZE_MEMBER("actorPath",m_actorFile);
        SERIALIZE_ENUM_BEGIN("actorSave",m_saveActorEnable);
            SERIALIZE_ENUM_VAR(noSave);
            SERIALIZE_ENUM_VAR(saveAllInActor);
            SERIALIZE_ENUM_VAR(save);
        SERIALIZE_ENUM_END();

        SERIALIZE_MEMBER("actorBasePath",m_actorBaseFile);
        SERIALIZE_MEMBER("actorBaseSave",m_saveActorBaseEnable);        
        SERIALIZE_MEMBER("useActorBaseSave",m_useActorBaseEnable);
    END_SERIALIZATION()


    ActorCreatePlugin::ActorCreatePlugin()
        : m_currentSelection(ITF_INVALID_OBJREF)
        , m_toolIsAware(bfalse)
        , m_actorIndex(-1)
        , m_actorNewCount(0)
        , m_state(Creator_Idle)
    {
        // register Specific views
        //ActorCreateSpProcess::Register(m_spViewMap, newAlloc(mId_Editor, ActorCreate_AnimLight()), this);
        ActorCreateSpProcess::Register(m_spViewMap, newAlloc(mId_Editor, ActorCreate_AnimatedComponent()), this);
    }

    ActorCreatePlugin::~ActorCreatePlugin()
    {
        for (u32 i=0; i<m_actorList.size(); i++)
        {
            clearActorIn(&m_actorList[i]);
        }
        SpProcessMap::iterator spIt = m_spViewMap.begin();
        for (; spIt != m_spViewMap.end(); spIt++)
        {
            SF_DEL(spIt->second);
        }
    }

    void ActorCreatePlugin::onMouseButton(InputAdapter::MouseButton _but, InputAdapter::PressStatus _status )
    {
        if (_but == InputAdapter::MB_Left)
        {
            switch (m_state)
            {
            case Creator_Copy:
                manageCopy();
                break;
            }
        }
    }

    void ActorCreatePlugin::setActorIndex(int _index)
    {
        if (m_actorIndex == _index)
            return;

        m_state = Creator_Idle; // reset creation mode if actor changed
        if (_index >= (i32)m_actorList.size())
        {
            u32 prevSize = m_actorList.size();
            m_actorList.resize(_index+1);

            for (i32 i = prevSize; i <= _index; i++)
            {
                ActorIn actorIn;
                m_actorList[i]  = actorIn;
            }
        }
        m_actorIndex = _index;
    }


    void ActorCreatePlugin::cleanActorList()
    {
        for (ITF_VECTOR<ActorIn>::iterator actorIter = m_actorList.begin();
            actorIter != m_actorList.end(); )
        {
            if (!GETOBJECT((*actorIter).m_actor))
                actorIter = m_actorList.erase(actorIter);
            else
                ++actorIter;
        }
        if (m_actorIndex >= (int)m_actorList.size())
            m_actorIndex = (int)m_actorList.size() - 1;
    }

    Actor_Template * ActorCreatePlugin::getActorTemplate(int _index, bbool forceSet)
    {
        if (_index < 0)
            _index = m_actorIndex;

        if (forceSet)
            setActorIndex(_index);

        if (0 <= _index && _index < (i32)m_actorList.size())
        {
            // 
            if(m_actorList[_index].m_myActorTemplateIsValid)
                return m_actorList[_index].m_myActorTemplate;
            else
            {
                
                Actor * actor = static_cast<Actor *>(GETOBJECT(m_actorList[_index].m_actor));
                ITF_ASSERT(actor);
                if(actor)
                {
                    if (!m_actorList[_index].m_myActorTemplate)
                        m_actorList[_index].m_myActorTemplate = (Actor_Template*)newAlloc(mId_Editor, Actor_Template());

                    const Actor_Template* actorTemplate = actor->getTemplate();
                    ITF_ASSERT_MSG(actorTemplate, "actor template is null!!");
                    if(actorTemplate)
                    {
                        BinaryClone(const_cast<Actor_Template*>(actorTemplate), (m_actorList[_index].m_myActorTemplate));

                        m_actorList[_index].m_myActorTemplateIsValid = btrue;
                        return m_actorList[_index].m_myActorTemplate;
                    }
                }
            }
        }

        return NULL;
    }


    //Actor_Template * ActorCreatePlugin::getDummyActorTemplate(int _index)
    //{
    //    if (_index < 0)
    //        _index = m_actorIndex;

    //    if (0 <= _index && _index < (i32)m_actorList.size())
    //    {
    //        // 
    //        if(m_actorList[_index].m_myDummyActorTemplateIsValid)
    //            return m_actorList[_index].m_myDummyActorTemplate;
    //        else
    //        {
    //            Actor_Template* actorTemplate = getActorTemplate(_index);
    //            if(actorTemplate)
    //            {
    //                if (!m_actorList[_index].m_myDummyActorTemplate)
    //                    m_actorList[_index].m_myDummyActorTemplate = (Actor_Template*)newAlloc(mId_Editor, Actor_Template());

    //                BinaryClone(const_cast<Actor_Template*>(actorTemplate), (m_actorList[_index].m_myDummyActorTemplate));
    //                ((SafeArray <class Actor_Template*>&)m_actorList[_index].m_myDummyActorTemplate->getComponents()).clear();

    //                m_actorList[_index].m_myDummyActorTemplateIsValid = btrue;
    //                return m_actorList[_index].m_myDummyActorTemplate;                    
    //            }
    //        }
    //    }

    //    return NULL;
    //}



    Actor* ActorCreatePlugin::getActor(int _index, bbool forceSet)
    {
        if (_index < 0)
            _index = m_actorIndex;

        if (forceSet)
            setActorIndex(_index);

        if (0 <= _index && _index < (i32)m_actorList.size())
        {
            return static_cast<Actor *>(GETOBJECT(m_actorList[_index].m_actor));
        }

        return NULL;
    }



    ActorEditorParams * ActorCreatePlugin::getActorEditorParams(int _index)
    {
        if (_index < 0)
            _index = m_actorIndex;

        if (0 <= _index && _index < (i32)m_actorList.size())
            return &m_actorList[_index].m_params;

        return NULL;
    }


    u32 ActorCreatePlugin::getActorEditorFlags(int _index)
    {
        if (_index < 0)
            _index = m_actorIndex;

        if (0 <= _index && _index < (i32)m_actorList.size())
            return m_actorList[_index].m_flags;

        return 0;
    }

    void ActorCreatePlugin::setActorEditorFlags(u32 flag, int _index)
    {
        if (_index < 0)
            _index = m_actorIndex;

        if (0 <= _index && _index < (i32)m_actorList.size())
        {
            m_actorList[_index].m_flags = flag;
        }
    }

    void ActorCreatePlugin::addActorEditorFlags(u32 flag, int _index)
    {
        if (_index < 0)
            _index = m_actorIndex;

        if (0 <= _index && _index < (i32)m_actorList.size())
        {
            m_actorList[_index].m_flags |= flag;
        }
    }

    void ActorCreatePlugin::removeActorEditorFlags(u32 flag, int _index)
    {
        if (_index < 0)
            _index = m_actorIndex;

        if (0 <= _index && _index < (i32)m_actorList.size())
        {
            m_actorList[_index].m_flags &= ~flag;
        }
    }


    void ActorCreatePlugin::setActorComponent(const String & _compoName)
    {
        if (m_actorIndex < 0 || m_actorIndex >= (i32)m_actorList.size())
            return;

        if (getActorComponent(_compoName))
        {
            m_actorList[m_actorIndex].m_actorCurComponent = _compoName;
            m_actorList[m_actorIndex].m_actorInMain = bfalse;
        }
        //else if(_compoName == "Actor_Template")//::GetObjectClassName())
        //{
        //    Actor_Template* actTemplate = getDummyActorTemplate(m_actorIndex);
        //    if(actTemplate)
        //    {
        //        m_actorList[m_actorIndex].m_actorCurComponent = _compoName;
        //    }
        //}
    }

    void ActorCreatePlugin::setActorInMain(bbool _actorInMain)
    {
        if (m_actorIndex < 0 || m_actorIndex >= (i32)m_actorList.size())
            return;
        
        m_actorList[m_actorIndex].m_actorInMain = _actorInMain;
    }

    TemplateActorComponent * ActorCreatePlugin::getActorComponent(const String & _compoName, int _index)
    {
        Actor_Template * actorTemplate = getActorTemplate(_index);
        if (!actorTemplate)
            return NULL;

        return actorTemplate->getTemplateComponentFromString(_compoName);
    }

    AnimTree_Template* ActorCreatePlugin::getAnimTreeTemplate(int _index, bbool forceSet)
    {
        if (_index < 0)
            _index = m_actorIndex;

        if (forceSet)
            setActorIndex(_index);

        if (0 <= _index && _index < (i32)m_actorList.size())
        {
            // get copy of actor template
            Actor_Template * actorTemplate = getActorTemplate(_index, forceSet);
            if(actorTemplate)
            {
                AnimatedComponent_Template *animComp = actorTemplate->GetComponent<AnimatedComponent_Template>();
                ITF_ASSERT(animComp);
                if (!animComp)
                    return NULL;

                return &(AnimTree_Template&)animComp->getAnimTreeTemplate(); // remove "const" by I use a copy of template
            }
            else
            {
                return NULL;
            }
        }
        return NULL;
    }

    void ActorCreatePlugin::clearActorIn(ActorIn *actorIn)
    {
        Actor * actor = static_cast<Actor *>(GETOBJECT(actorIn->m_actor));
        if (actor)
        {
            if ((actorIn->m_flags & ACTOR_IN_CREATED) )
            {
                Scene* pScene = actor->getScene();
                if (pScene)
                {
                    if(UNDOREDO_MANAGER)
                        UNDOREDO_MANAGER->preDelete(actor);
                    pScene->unregisterPickable(actor);
                    if(EDITOR)
                        EDITOR->setSceneAsModified(pScene);
                }
                else
                {
                    ITF_ASSERT_MSG(0, "Actor destroyed not in any scene");
                    SF_DEL(actor);
                }
            }
            else
            {
                actor->setOverrideTemplate(NULL);
                actor->oneStepHotReload();
            }
        } 
        actorIn->m_actor   = ITF_INVALID_OBJREF;
        actorIn->m_flags   = 0;


        SF_DEL(actorIn->m_myActorTemplate);
        SF_DEL(actorIn->m_myActorTemplateOverride);
    }

    void ActorCreatePlugin::reloadActor(int _index)
    {
        if (_index < 0)
            _index = m_actorIndex;
        Actor * actor = getActor(_index);
        if (!actor || !actor->isPhysicalReady())
            return;
        
        //if (UNDOREDO_MANAGER)
        //    UNDOREDO_MANAGER->preChangeTemplate(actor->getOverrideTemplate(), actor->getRef(), ActorProcessUndoRedoChange);

        // reload
        Actor_Template* actorTemplate = getActorTemplate(_index);
        ITF_ASSERT_MSG(actorTemplate, "template is null!");
        if(actorTemplate)
        {
            String pathStr = actor->getLua().getString();

            if(!actor->getOverrideTemplate())
            {
                m_actorList[_index].m_myActorTemplateOverride = newAlloc(mId_Editor, Actor_Template);
                BinaryClone(actorTemplate, (m_actorList[_index].m_myActorTemplateOverride));

                m_actorList[_index].m_myActorTemplateOverride->setFile(pathStr);
                m_actorList[_index].m_myActorTemplateOverride->onTemplateLoaded();

                ActorEditor::getInstance()->changeTemplate(pathStr, m_actorList[_index].m_myActorTemplateOverride, btrue);
            } 
            else
            {
                ActorEditor::getInstance()->modifyOverridedTemplate(pathStr, actorTemplate,
                    (ESerializeFlags)(ESerialize_Data_Save|ESerialize_Editor_Save),
                    (ESerializeFlags)(ESerialize_Data_Load|ESerialize_Editor_Load));

            }

            m_actorList[_index].m_actor = actor->getRef();
        }
    }

    void ActorCreatePlugin::pauseActor(int _index, bbool _pause)
    {        
        Actor * actor = getActor(_index);
        if (!actor)
            return;

        ITF_VECTOR<ObjectRef>  tempObjList;
        ITF_VECTOR<ObjectRef>* objList = NULL;

            String8 path8(actor->getLua().getString().cStr());
            objList = LUA_HANDLER->getLUAFileClients(path8);

        if (!objList)
            return;

        ITF_VECTOR<ObjectRef>::const_iterator sit = objList->begin();
        ITF_VECTOR<ObjectRef>::const_iterator send = objList->end();
        for (; sit != send; ++sit)
        {
            Actor* act = static_cast<Actor*>(GETOBJECT(*sit));
            if (act)
            {
                if (_pause)
                {
                    //act->disable();
                    //const u32 uCompCount = act->GetAllComponents().size();
                    //for(u32 j = 0; j < uCompCount; ++j)
                    //{
                    //    ActorComponent* comp = act->GetAllComponents()[j];
                    //    if(comp && !comp->needsDraw())
                    //    {
                    //        comp->pause();
                    //    }
                    //}
                }
                else
                {
                    //act->unpause();
                    //const u32 uCompCount = act->GetAllComponents().size();
                    //for(u32 j = 0; j < uCompCount; ++j)
                    //{
                    //    ActorComponent* comp = act->GetAllComponents()[j];
                    //    if(comp)
                    //    {
                    //        comp->unpause();
                    //    }
                    //}
                    reloadActor(_index);
                    

                    SpProcessMap::iterator spIterBegin = m_spViewMap.begin();
                    SpProcessMap::iterator spIterEnd = m_spViewMap.end();
                    for (SpProcessMap::iterator iter = spIterBegin; iter!= spIterEnd; iter++)
                    {
                        iter->second->ReceiveActorReload();
                    }
                }
            }
        }
    }

    void ActorCreatePlugin::receive(Blob& _blob,const NETPeer* _peer, u32 _senderID)
    {
        String message;

        _blob.extractString(message);

        if ( message == getSpViewName() )
        {
            onSpView(_blob);
        }
        else if ( message == getRecvCommandPropertiesName() )
        {
            onPropertiesCmd(_blob);
        }
        else if(message == getRecvSessionInfoCmdName())
        {
            onGetSessionInfoCmd(_blob);
        }
        else if ( message == getRecvCommandDumpToFileName() )
        {
            onDumpToFileCmd(_blob);
        }
        else if ( message == getRecvActorListName() )
        {
            onGetActorList(_blob);
        }
        else if ( message == getRecvSetActorMainName() )
        {
            onSetActorMain(_blob);
        }
        else if ( message == getRecvComponentListName() )
        {
            onGetComponentList(_blob);
        }
        else if ( message == getRecvSetActorName() )
        {
            onSetActor(_blob);
        }
        else if ( message == getRecvSetActorRefName() )
        {
            onSetActorRef(_blob);
        }
        else if ( message == getRecvActorComponentListName() )
        {
            onGetActorComponentList(_blob);
        }
        else if ( message == getRecvSetActorMainDataName() )
        {
            onSetActorMainData(_blob);
        }
        else if ( message == getRecvGetActorMainDataName() )
        {
            onGetActorMainData(_blob);
        }
        else if ( message == getRecvSetActorComponentName() )
        {
            onSetActorComponent(_blob);
        }
        else if ( message == getRecvSetActorComponentDataName() )
        {
            onSetActorComponentData(_blob);
        }
        else if ( message == getRecvSetActorComponentPosName() )
        {
            onSetActorComponentPos(_blob);
        }
        else if ( message == getRecvAddActorComponentName() )
        {
            onAddActorComponent(_blob);
        }
        else if ( message == getRecvDelActorComponentName() )
        {
            onDelActorComponent(_blob);
        }
        else if ( message == getRecvSaveActorName() )
        {
            onSaveActor(_blob);
        }
        else if ( message == getRecvSpawnActorName() )
        {
            onSpawnActor(_blob);
        }
        else if ( message == getRecvChangeActorPauseName() )
        {
            onChangeActorPause(_blob);
        }
        else if ( message == getRecvSetActorPauseName() )
        {
            onSetActorPause(_blob);
        }
        else if ( message == getRecvCloseActorName() )
        {
            onCloseActor(_blob);
        }
        else if ( message == getRecvActorEditorParamsName() )
        {
            onGetActorEditorParams(_blob);
        }
        else if( message == getRecvSetActorEditorParamsName( ))
        {
            onSetActorEditorParams(_blob);
        }
        else if( message == getRecvNewActorName())
        {
            onNewActor(_blob);
        }
    }

    void ActorCreatePlugin::onSpView( Blob& _blob )
    {
        String spComponent;
        _blob.extractString(spComponent);

        SpProcessMap::iterator spIter = m_spViewMap.find(spComponent);
        if (spIter != m_spViewMap.end())
        {
            spIter->second->ReceiveMsg(_blob);
        }
    }

    void ActorCreatePlugin::onPropertiesCmd( Blob& _blob )
    {
        String val;
        ObjectRef objRef = _blob.extractUint32();
        _blob.extractString(val);

        BaseObject* obj = GETOBJECT(objRef);

        if ( obj )
        {
            SerializerToolXML serializer;

            StringConverter converted(val);

            serializer.OpenToRead(converted.getChar());
            obj->Serialize(&serializer,ESerialize_Data_Load);
            serializer.Close();

            //size_t size;
            //char* buffer = NULL;
            //serializer.OpenToWrite(&buffer,&size);
            //obj->Serialize(&serializer,ESerialize_Template_Save);
            //serializer.Close();
            //sendProperties(buffer);
            //SF_DEL(buffer);
        }
    }

    void ActorCreatePlugin::onGetSessionInfoCmd(Blob& _blob )
    {
        Blob b(BLOB_EXTRACT_AND_WRITE_ENABLED);
        String path = DATA_FOLDER;
        path.replace('/', '\\');
        path.truncate(path.getLen()-1);
        b.pushString(ActorCreatePlugin::getPluginName());  // plugin ID
        b.pushString(getSendSessionInfoCmdName()); 
        b.pushString(path);        
        PLUGINGATEWAY->sendPacket(b, m_editorPeer, m_editorSenderID);
    }

    void ActorCreatePlugin::onGetActorList( class Blob& _blob )
    {
        // Only send the actor list
        sendActorList();
        sendActorCurrentData();
    }

    void ActorCreatePlugin::onGetComponentList( class Blob& _blob )
    {
        // Only send the component list
        sendComponentList();
    }
    
    void ActorCreatePlugin::onGetActorComponentList( class Blob& _blob )
    {
        // Only send the component list
        int actorIndex = _blob.extractInt32();        
        sendActorComponentList(actorIndex);
    }

    void ActorCreatePlugin::manageForceReload(i32 _index, const String & _path, i32 _mainInfo)
    {
    //    const Actor * actor = getActor(_index, false);
    //    if (!actor || _path.isEmpty())
    //        return;

    //    String  relativePath;
    //    if (!FILESERVER->getPathFromDataFolder(relativePath, _path))
    //        return;
    //    ITF_ASSERT(actor->getLua().getString() == relativePath);

    //    CSerializerObjectLogicDB serializer(actor->getLua().getString());
    //    actor->Serialize(&serializer,ESerialize_Template_Load);
    //    reloadActor(_index);

    //    setActorIndex(_index);
    //    setActorInMain(_mainInfo);
    //    sendActorCurrentData();
    }

    void ActorCreatePlugin::manageSetActor(i32 _index, const String & _path, ObjectRef _ref, i32 _mainInfo, i32 _tags)
    {
        Actor * actorSrc = (Actor *)(GETOBJECT(_ref));
        bbool   isACT = _path.getLen() > 0 && FilePath::getExtension(_path) == "act";
        String  relativePath;
        i32     actorFound = -1;

        if (actorSrc)
        {
            isACT = btrue;
            relativePath = actorSrc->getLua().getString();
        }

        if (isACT)
        {
            if (!actorSrc && !FILESERVER->getPathFromDataFolder(relativePath, _path))
                return;

            //verify if actor not already present
            for (u32 i=0; i<m_actorList.size(); i++)
            {
                const Actor * actor = getActor(i, false);
                if (actor && actor->getLua().getString() == relativePath)
                {
                    actorFound = i;
                    break;
                }
            }
        }

        if (actorFound >= 0)
        {
            // actor already present, send it
            setActorIndex(actorFound);
            sendActorCurrentData();
            return;
        }

        if (_index < 0)
            _index = m_actorList.size();
        Actor* actor = getActor(_index);

        if (actor && isACT)
        {
            if (!(actor->getLua().getString() == relativePath))
            {
                clearActorIn(&m_actorList[_index]);
                actor = NULL;
            }
        }

        if (!actor)
        {
            // if not actor set, create new one -> dummy actor ...
            Scene* pScene =  EDITOR->getEditedScene();
            ITF_ASSERT(pScene);
            Vec3d pos(Vec3d::Zero);

            if (actorSrc)
            {
                actor = actorSrc;
            } else 
            {
                if (isACT)
                {
                    actor = ACTORSMANAGER->spawnActor(pos, pScene, relativePath, FILEMANAGER->fileExists(relativePath));
                    _tags |= ACTOR_IN_CREATED;
                } else
                {
                    ITF_ASSERT(0);
                }
                //// put actor in pause -> no need for draw nor update in edition mode
                //if (actor)
                //{
                //    actor->disable();
                //    actor->setSerializable(bfalse);
                //}
            }

            ITF_ASSERT(actor);
            if (actor)
            {
                m_actorList[_index].m_actor   = actor->getRef();
                m_actorList[_index].m_flags   = _tags;

                // get actor editor params
                String path = actor->getLua().getString();
                if (!path.isEmpty())
                {
                    CSerializerObjectLogicDB serializer(path);
                    if (serializer.isValid())
                        m_actorList[_index].m_params.Serialize(&serializer,ESerialize_Data_Load);
                }

            }
            sendActorList();
        }

        setActorIndex(_index);
        setActorInMain(_mainInfo);
        sendActorCurrentData();
    }

    void ActorCreatePlugin::manageCopy()
    {
    //    const Actor *actor = getActor();
    //    if (!actor)
    //        return;

    //    // non duplicable actors
    //    if(GAMEMANAGER->isPlayerActor(actor->getRef()) || actor->DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817)))
    //        return;
    //    
    //    Actor* pNewActor = NULL;
    //    PickingShape* pPickingShape = EDITOR->getMainPickingShapeForObject(actor->getRef());
    //    Vec2d anchor = pPickingShape->getPickingInfoPtr()->m_anchor;

    //        pNewActor = (Actor *)actor->clone(actor->getScene(), btrue);

    //    ITF_ASSERT(pNewActor && pPickingShape);
    //    if(!pNewActor || !pPickingShape)
    //        return;

    //    UNDOREDO_MANAGER->postAdd(pNewActor);
    //    // new actor IS serialisable !
    //    pNewActor->resetCurrentInfos();
    //    pNewActor->setSerializable(btrue);
    //    pNewActor->enable();

    //    PickingShape* pNewPickableShape = EDITOR->getMainPickingShapeForObject(pNewActor->getRef());
    //    ITF_ASSERT(pNewPickableShape);
    //    EDITOR->selectObject(pNewActor->getRef(), bfalse);
    //    pNewPickableShape->fillPickingInfo(pNewPickableShape->getPickingInfoPtr());
    //    pNewPickableShape->getPickingInfoPtr()->m_anchor = anchor;
    //    pNewPickableShape->dragByMouse(EDITOR->getMouse2d());

    //    m_state = Creator_Idle;
    }

    void ActorCreatePlugin::onSetActor(class Blob& _blob )
    {
        i32         actorIndex = 0;
        String      actorPath;
        i32         mainInfo;
        i32         forceReload;
        ObjectRef   actorRef = ITF_INVALID_OBJREF;

        actorIndex = _blob.extractInt32();
        _blob.extractString(actorPath);
        mainInfo        = _blob.extractInt32();
        forceReload     = _blob.extractInt32();

        if (actorIndex >= 0)
        {
            const Actor *actor = getActor(actorIndex, false);
            if (actor)
                actorRef = actor->getRef();
        }
        FilePath::normPath(actorPath);

        if (forceReload)
        {
            manageForceReload(actorIndex, actorPath, mainInfo);
        } else
        {
            manageSetActor(actorIndex, actorPath, actorRef, mainInfo, 0);
        }

    }

    void ActorCreatePlugin::onNewActor(class Blob& _blob)
    {
        String                actorPath;
        ITF_VECTOR<String>  listComponentForAdding;
        
        _blob.extractString(actorPath);
        int count = _blob.extractInt32();
        for (i32 i = 0; i < count; i++)
        {
            String compName;
            _blob.extractString(compName);
            listComponentForAdding.push_back(compName);
        }


        String relativePath;
        FilePath::normPath(actorPath);
        if (!FILESERVER->getPathFromDataFolder(relativePath, actorPath))
            return;


        // if not actor set, create new one -> dummy actor ...
        Scene* pScene =  EDITOR->getEditedScene();
        ITF_ASSERT(pScene);
        Vec3d pos(Vec3d::Zero);

        // create a new actor
        Actor* newActor = new Actor();
        Actor_Template* newTemplate = new Actor_Template(relativePath);

        ObjectFactory * actorFactory = TEMPLATEDATABASE->getComponentsFactory();
        ITF_ASSERT(actorFactory);
        if (actorFactory)
        {
            for (u32 i = 0; i < listComponentForAdding.size(); i++)
            {            
                TemplateActorComponent * actorCompo = actorFactory->CreateObject<TemplateActorComponent>(StringID(listComponentForAdding[i]).GetValue());
                if (actorCompo)
                    newTemplate->addTemplateComponent(actorCompo);
            }
        }
        newActor->setOverrideTemplate(newTemplate);

        // save actor
        {
            CSerializerObjectLua serializerLUA;
            if ( serializerLUA.OpenBufferToWrite("params", btrue, newTemplate->GetObjectClassName()) )
            {
                newTemplate->Serialize(&serializerLUA,ESerialize_Data_Save);
                ITF_STDSTRING buffer = serializerLUA.CloseBuffer();

                File* file = FILEMANAGER->openFile(relativePath, ITF_FILE_ATTR_WRITE|ITF_FILE_ATTR_CREATE_NEW);
                if ( file )
                {
                    file->write(buffer.c_str(), u32(sizeof(u8)*buffer.length()));
                    FILEMANAGER->closeFile(file);
                }
            }
        }
        ITF_DELETE(newActor);
        ITF_DELETE(newTemplate);


        Actor* actor = ACTORSMANAGER->spawnActor(pos, pScene, relativePath, FILEMANAGER->fileExists(relativePath));


        if (actor)
        {
            actor->disable();
            actor->setSerializable(bfalse);
        }

        ITF_ASSERT(actor);
        if (actor)
        {
            u32 index = m_actorList.size();

            setActorIndex(index);

            EDITOR->setSceneAsModified(pScene);
            UNDOREDO_MANAGER->postAdd(actor);
            m_actorList[index].m_actor   = actor->getRef();
            m_actorList[index].m_flags  |= ACTOR_IN_CREATED;;


            // get actor editor params
            String path = actor->getLua().getString();
            if (!path.isEmpty())
            {
                CSerializerObjectLogicDB serializer(path);
                if (serializer.isValid())
                    m_actorList[index].m_params.Serialize(&serializer,ESerialize_Data_Load);
            }

        
            sendActorList();
            setActorIndex(index);
            setActorInMain(bfalse);
            sendActorCurrentData();
        }
    }

    void ActorCreatePlugin::onSetActorRef(class Blob& _blob )
    {
        i32         actorIndex = 0;
        ObjectRef   actorRef;
        i32         mainInfo;
    

        actorIndex = _blob.extractInt32();
        actorRef = (ObjectRef)_blob.extractUint32();
        mainInfo = _blob.extractInt32();

        Actor *actorFromRef = (Actor *)GETOBJECT(actorRef);
        if (!actorFromRef)
            return;

        manageSetActor(actorIndex, "", actorRef, mainInfo, ACTOR_IN_VISIBLE);
    }

    void ActorCreatePlugin::onGetActorMainData( class Blob& _blob )
    {
        i32     actorIndex = 0;
        actorIndex = _blob.extractInt32();

        sendActorMainData(actorIndex);
    }

    void ActorCreatePlugin::onSetActorMain( class Blob& _blob )
    {
        i32     actorIndex = 0;
        actorIndex = _blob.extractInt32();

        setActorIndex(actorIndex);
        setActorInMain(btrue);
        sendActorMainData(actorIndex);
    }

    void ActorCreatePlugin::onSetActorMainData(class Blob& _blob)
    {
        i32     actorIndex = 0;
        String  srcXML;

        actorIndex = _blob.extractInt32();
        _blob.extractString(srcXML);

        Actor_Template * actorTemplate = getActorTemplate(actorIndex);
        ActorEditorParams * editorParams = getActorEditorParams(actorIndex);
        blobToActorEditorParams(_blob, editorParams);

        if (!actorTemplate || !editorParams)
            return;


        SerializerToolXML   serializer;
        StringConverter converted(srcXML);
        serializer.OpenToRead(converted.getChar());
        //UNDOREDO_MANAGER->preChange(getActor(actorIndex));
       
        actorTemplate->Serialize(&serializer,ESerialize_PropertyEdit_Load); // ESerialize_PropertyEdit_Load
        serializer.Close();
        

        //actorDummy.setToActor(actorTemplate);
        //reloadActor(actorIndex);

        setActorIndex(actorIndex);
        setActorInMain(btrue);
        sendActorMainData(actorIndex);
    }


    void ActorCreatePlugin::onSetActorEditorParams( class Blob& _blob )
    {
        i32     actorIndex = 0;
        actorIndex = _blob.extractInt32();

        ActorEditorParams * editorParams = getActorEditorParams(actorIndex);
        blobToActorEditorParams(_blob, editorParams);
        setActorIndex(actorIndex);
        sendActorEditorParams(actorIndex);
    }


    void ActorCreatePlugin::onSetActorComponent(class Blob& _blob )
    {
        i32     actorIndex = 0;
        String  actorComponent;

        actorIndex = _blob.extractInt32();
        _blob.extractString(actorComponent);
        
        setActorIndex(actorIndex);
        setActorComponent(actorComponent);
        sendActorComponentData(actorComponent);
    }

/*
    void ActorCreatePlugin::onSetActorComponentData(class Blob& _blob )
    {
        i32     actorIndex = 0;
        String  actorComponent;
        String  srcXML;

        actorIndex = _blob.extractInt32();
        _blob.extractString(actorComponent);
        _blob.extractString(srcXML);


        Actor * actor = getActor(actorIndex);
        Actor_Template* actorTemplate = getActorTemplate(actorIndex);
        if(!actorTemplate)
            return;

        // get index of component
        i32 indexComponent = actorTemplate->getIndexTemplateComponentFromString(actorComponent);


        if (actor && indexComponent != -1)
        {            
            actorTemplate->delTemplateComponent(actorComponent, btrue);

            ObjectFactory * actorFactory = TEMPLATEDATABASE->getComponentsFactory();
            ITF_ASSERT(actorFactory);
            if (actorFactory)
            {
                TemplateActorComponent * actorCompo = actorFactory->CreateObject<TemplateActorComponent>(StringConverter(actorComponent).getChar());
                if (actorCompo)
                {
                    SerializerToolXML serializer;

                    StringConverter converted(srcXML);

                    serializer.OpenToRead(converted.getChar());
                    
                    actorCompo->Serialize(&serializer,ESerialize_PropertyEdit_Load);
                    serializer.Close();

                    actorTemplate->insertTemplateComponent(actorCompo, (u32)indexComponent);
                }

            //reloadActor(actorIndex);
            }


            setActorIndex(actorIndex);
            setActorComponent(actorComponent);
            sendActorComponentData(actorComponent);
        }
    }
*/

    void ActorCreatePlugin::onSetActorComponentData(class Blob& _blob )
    {
        i32     actorIndex = 0;
        String  actorComponent;
        String  srcXML;

        actorIndex = _blob.extractInt32();
        _blob.extractString(actorComponent);
        _blob.extractString(srcXML);

        // TODO save and apply
        Actor * actor = getActor(actorIndex);
        TemplateActorComponent *compo = getActorComponent(actorComponent, actorIndex);
        if (actor && compo)
        {
            SerializerToolXML serializer;

            StringConverter converted(srcXML);

            serializer.OpenToRead(converted.getChar());

            compo->Serialize(&serializer,ESerialize_PropertyEdit_Load);
            serializer.Close();


            //reloadActor(actorIndex);

            setActorIndex(actorIndex);
            setActorComponent(actorComponent);
            sendActorComponentData(actorComponent);
        }
    }

    void ActorCreatePlugin::onSetActorComponentPos(class Blob& _blob )
    {
        i32     actorIndex = 0;
        String  actorComponent;
        i32     actorComponentIndex = 0;

        actorIndex = _blob.extractInt32();
        _blob.extractString(actorComponent);
        actorComponentIndex = _blob.extractInt32();

        Actor_Template * actor = getActorTemplate(actorIndex);
        ITF_ASSERT(actor);
        if (actor)
        {
            actor->moveTemplateComponent(actorComponent, actorComponentIndex);
            sendActorComponentList(actorIndex);
            sendActorComponentData(actorComponent);
        }
    }

    void ActorCreatePlugin::onAddActorComponent( Blob& _blob )
    {
        i32     actorIndex = 0;
        String  actorComponent;

        actorIndex = _blob.extractInt32();
        _blob.extractString(actorComponent);

        Actor_Template * actorTemplate = getActorTemplate(actorIndex);
        if (!actorTemplate)
        {
            return;
        }
        
        ObjectFactory * actorFactory = TEMPLATEDATABASE->getComponentsFactory();
        ITF_ASSERT(actorFactory);
        if (actorFactory)
        {
            TemplateActorComponent * actorCompo = actorFactory->CreateObject<TemplateActorComponent>(StringConverter(actorComponent).getChar());

            if (actorCompo)
            {
                SafeArray<TemplateActorComponent*> actorComponents = actorTemplate->getComponents();
                i32 count = (i32)actorComponents.size();
                bbool conflict = bfalse;
                for (i32 i=0; i<count; i++)
                {
                    conflict = actorComponents[i]->IsClassCRC(actorCompo->GetObjectClassCRC()) ||
                               actorCompo->IsClassCRC(actorComponents[i]->GetObjectClassCRC());
                    if (conflict)
                    {
                        String message;
                        message.setTextFormat("Cannot add component %s\nIt conflicts with actor's component %s", actorCompo->GetObjectClassName(), actorComponents[i]->GetObjectClassName());
                        sendError(message);
                        break;
                    }
                }
                
                if (!conflict)
                {
                    // TODO change this!
                    actorTemplate->addTemplateComponent(actorCompo);
                    reloadActor();
                    sendActorComponentList(actorIndex);
                    setActorComponent(actorComponent);
                    sendActorComponentData(actorComponent);
                }
            }
        }
    }

    void ActorCreatePlugin::onDelActorComponent( Blob& _blob )
    {
        i32     actorIndex = 0;
        String  actorComponent;

        actorIndex = _blob.extractInt32();
        _blob.extractString(actorComponent);

        Actor_Template * actorTemplate = getActorTemplate(actorIndex);
        if (!actorTemplate)
        {
            return;
        }
        
        actorTemplate->delTemplateComponent(actorComponent, true);
        reloadActor();
        sendActorComponentList(actorIndex);
    }

    void ActorCreatePlugin::onSaveActor(class Blob& _blob)
    {
        saveActor(_blob);
    }

    // 
    void ActorCreatePlugin::fillComponentsPaths(i32 _actorIndex, bbool _resetPaths)
    {
        Actor *actor = getActor(_actorIndex);
        Actor_Template *actorTemplate = getActorTemplate(_actorIndex);

        ActorEditorParams * editorParams = getActorEditorParams(_actorIndex);
        if (!actor || !editorParams || !actorTemplate)
            return;

        SafeArray<TemplateActorComponent*> actorComponents = actorTemplate->getComponents();
       

        // actor path
        String actorPath;
        if(editorParams->m_actorFile.isEmpty())
        {   
            // initialize path
            actorPath = actor->getLua().getString();
        }
        else
        {
            actorPath = editorParams->m_actorFile;
            FilePath::normPath(actorPath);
            FilePath::transformToRelative(actorPath);
        }
        editorParams->m_actorFile = FilePath::normalizePath(actorPath);


        // default component path
        String componentPath  = FilePath::getDirectory(actorPath) + "components/";
        //componentPath += FilePath::getFilenameWithoutExtension(actorPath) + "_";


        // actor base path
        String actorBasePath;
        if(editorParams->m_actorBaseFile.isEmpty() || _resetPaths)
        {   
            // initialize path
            actorBasePath = componentPath + "_" + FilePath::getFilenameWithoutExtension(actorPath) + "_base.ilu";
        }
        else
        {
            actorBasePath = editorParams->m_actorBaseFile;
            FilePath::normPath(actorBasePath);
            FilePath::transformToRelative(actorBasePath);
        }
        editorParams->m_actorBaseFile = FilePath::normalizePath(actorBasePath);


        if(_resetPaths)
        {
            editorParams->m_componentFiles.clear();
        }

        ITF_VECTOR<ActorEditorParamsFiles> params = editorParams->m_componentFiles;
        
        u32 count = actorComponents.size();
        for (u32 i=0; i<count; i++)
        {
            // found if a component have a file
            TemplateActorComponent *actorComponent = actorComponents[i];
            ITF_VECTOR<ActorEditorParamsFiles>::iterator paramsIter = params.begin();
            for (; paramsIter != params.end(); ++paramsIter)
            {
                if ((*paramsIter).m_component == actorComponent->GetObjectClassName())
                {
                    if(_resetPaths)
                        editorParams->m_componentFiles.push_back(*paramsIter);
                    break;
                }
            }

            // not exist in list => initialize path
            if (paramsIter == params.end())
            {
                ActorEditorParamsFiles fileComp;

                // class name
                fileComp.m_component    = actorComponent->GetObjectClassName();

                // file path
                String classname = actorComponent->GetObjectClassName(); 
                classname.toLower();
                classname.replace("_template", "");
                fileComp.m_path         = componentPath + classname + ".ilu";

                editorParams->m_componentFiles.push_back(fileComp);
            }            
        }        
    }

    void ActorCreatePlugin::saveActor(class Blob& _blob)
    {
        i32     actorIndex = 0;        

        actorIndex = _blob.extractInt32();

        Actor *actor = getActor(actorIndex);
        Actor_Template *actorTemplate = getActorTemplate(actorIndex);

        ActorEditorParams * editorParams = getActorEditorParams(actorIndex);
        if (!actor || !editorParams || !actorTemplate)
            return;

        //if (actorPath.isEmpty())
        //    actorPath = actor->getLua().getString();
        //else
        //{
        //    FilePath::normPath(actorPath);
        //    FilePath::transformToRelative(actorPath);
        //}
        //actorPath = FilePath::normalizePath(actorPath);

        // actor file
        String  actorPath = editorParams->m_actorFile;
        ActorEditorParams::actorSaveType saveActorEnable = editorParams->m_saveActorEnable;
        // actor base file
        String  actorBasePath = editorParams->m_actorBaseFile;
        bbool useActorBaseEnbale = editorParams->m_useActorBaseEnable;
        bbool saveActorBaseEnable = editorParams->m_saveActorBaseEnable;


        CSerializerObjectLua serializerLUA;

        // components save in separated file?
        fillComponentsPaths(actorIndex, bfalse);
        bbool saveInComp = bfalse;
        for (u32 i=0; i<editorParams->m_componentFiles.size(); i++)
        {
            saveInComp = editorParams->m_componentFiles[i].m_saveEnable == ActorEditorParamsFiles::saveInComponent;
            if(saveInComp)
                break;
        }

        // separated components
        if (saveActorEnable != ActorEditorParams::saveAllInActor && (saveActorBaseEnable || saveInComp))
        {
            String8 actEndBuffer, bufferTmp, bufferComponentInActorBase;
            SafeArray<TemplateActorComponent*> actorComponents = actorTemplate->getComponents();
            i32 count = (i32)actorComponents.size();            
            if (count)
            {
                for (i32 i=0; i<count; i++)
                {
                    if(editorParams->m_componentFiles[i].m_saveEnable == ActorEditorParamsFiles::saveInComponent)
                    {
                        String pathStr = editorParams->m_componentFiles[i].m_path.getString();
                        FILEMANAGER->deleteFile(pathStr);
                        File* file = FILEMANAGER->openFile(pathStr, ITF_FILE_ATTR_WRITE|ITF_FILE_ATTR_CREATE_NEW);
                        if (file)
                            FILEMANAGER->closeFile(file);
                    }
                }
                for (i32 i=0; i<count; i++)
                {
                    // found component to save
                    TemplateActorComponent *actorComponent = actorComponents[i];
                    String8         componentName(actorComponent->GetObjectClassName());
                    Path            componentPath;
                    ActorEditorParamsFiles::componentSaveType saveEnable = ActorEditorParamsFiles::saveInComponent;
                    for (u32 j=0; j<editorParams->m_componentFiles.size(); j++)
                    {
                        if (editorParams->m_componentFiles[j].m_component == componentName)
                        {
                            componentPath = editorParams->m_componentFiles[j].m_path;
                            saveEnable = editorParams->m_componentFiles[j].m_saveEnable;
                            break;
                        }
                    }
                    ITF_ASSERT(!componentPath.getString().isEmpty());
                    
                    ITF_STDSTRING buffer;

                    // serialize a specific components and get buffer by saveComponent()                    
                    if(saveEnable == ActorEditorParamsFiles::saveInComponent) 
                    {
                        SpProcessMap::iterator spIterBegin = m_spViewMap.begin();
                        SpProcessMap::iterator spIterEnd = m_spViewMap.end();
                        for (SpProcessMap::iterator iter = spIterBegin; iter!= spIterEnd; iter++)
                        {
                            if(componentName.equals(iter->second->GetComponentName(), btrue))
                            {
                                buffer = iter->second->saveComponent(actor, componentPath, saveEnable, actorComponent);
                                break;
                            }
                        }
                    }

                    // save component
                    {
                        // if not specific component, get buffer
                        if( buffer.empty() )
                        {
                            serializerLUA.OpenBufferToWrite("component", btrue, actorComponent->GetObjectClassName());
                            actorComponent->Serialize(&serializerLUA,ESerialize_Data_Save);
                            buffer = serializerLUA.CloseBuffer();
                        }

                        // write component file
                        if( saveEnable == ActorEditorParamsFiles::saveInComponent)
                        {
                            // create directory
                            String fullPath;
                            FILEMANAGER->TranslatePath(fullPath, componentPath.getString());
                            Directory::create(FilePath::getDirectory(fullPath));

                            File* file = FILEMANAGER->openFile(componentPath.getString(), ITF_FILE_ATTR_WRITE);
                            if ( file )
                            {
                                file->seek(0, FILE_END);

                                file->write(buffer.c_str(), u32(sizeof(u8)*buffer.length()));
                                file->write(iluEnd(), u32(sizeof(u8)*strlen(iluEnd())));
                                FILEMANAGER->closeFile(file);
                                
                                bbool found = bfalse;
                                for (i32 compIdx = 0; !found && compIdx < i; ++compIdx)
                                {
                                    if (editorParams->m_componentFiles[compIdx].m_path == componentPath)
                                        found = btrue;
                                }
                                if (!found)
                                {
                                    bufferTmp.setTextFormat(actInclude(), componentPath.getString().cStr());
                                    //actEndBuffer += bufferTmp.cStr();
                                    bufferComponentInActorBase += bufferTmp;
                                    bufferComponentInActorBase += "\n";
                                }
                            }
                        }
                        // adding component in actor base buffer
                        else if( saveEnable == ActorEditorParamsFiles::saveInActorBase)
                        {
                            bufferComponentInActorBase += buffer.c_str();
                            bufferComponentInActorBase += iluEnd();
                            bufferComponentInActorBase += "\n";
                        }
                        // adding include
                        else if(saveEnable == ActorEditorParamsFiles::noSave)
                        {
                            bbool found = bfalse;
                            for (i32 compIdx = 0; !found && compIdx < i; ++compIdx)
                            {
                                if (editorParams->m_componentFiles[compIdx].m_path == componentPath)
                                    found = btrue;
                            }
                            if (!found)
                            {
                                bufferTmp.setTextFormat(actInclude(), componentPath.getString().cStr());
                                bufferComponentInActorBase += bufferTmp;
                                bufferComponentInActorBase += "\n";
                                //actEndBuffer += bufferTmp;
                            }
                        }
                    }
                }
            }
            
            // save actor base file
            if ( useActorBaseEnbale && saveActorBaseEnable )
            {
                serializerLUA.OpenBufferToWrite("params", btrue, actorTemplate->GetObjectClassName());
                actorTemplate->Serialize(&serializerLUA,(ESerializeFlags)0);
                editorParams->Serialize(&serializerLUA,ESerialize_Data_Save);

                ITF_STDSTRING buffer = serializerLUA.CloseBuffer();

                File* file = FILEMANAGER->openFile(actorBasePath, ITF_FILE_ATTR_WRITE|ITF_FILE_ATTR_CREATE_NEW);
                if ( file )
                {
                    file->write(buffer.c_str(), u32(sizeof(u8)*buffer.length()));
                    file->write(actEnd(), u32(sizeof(u8)*strlen(actEnd())));
                    file->write(bufferComponentInActorBase.cStr(),sizeof(u8)*bufferComponentInActorBase.getLen());                    
                    file->write(actEndBuffer.cStr(), sizeof(u8)*actEndBuffer.getLen());
                    FILEMANAGER->closeFile(file);
                }
            }

            // actor file
            if ( saveActorEnable)
            {
                // save actor file
                if(useActorBaseEnbale)
                {
                    File* file = FILEMANAGER->openFile(actorPath, ITF_FILE_ATTR_WRITE|ITF_FILE_ATTR_CREATE_NEW);
                    if ( file )
                    {
                        bufferTmp.setTextFormat(actInclude(), actorBasePath.cStr());
                        file->write(bufferTmp.cStr(), sizeof(u8)*bufferTmp.getLen());
                        FILEMANAGER->closeFile(file);
                    }
                }
                // components save in actor file
                else
                {
                    serializerLUA.OpenBufferToWrite("params", btrue, actorTemplate->GetObjectClassName());
                    actorTemplate->Serialize(&serializerLUA,(ESerializeFlags)0);
                    editorParams->Serialize(&serializerLUA,ESerialize_Data_Save);

                    ITF_STDSTRING buffer = serializerLUA.CloseBuffer();

                    File* file = FILEMANAGER->openFile(actorPath, ITF_FILE_ATTR_WRITE|ITF_FILE_ATTR_CREATE_NEW);
                    if ( file )
                    {
                        file->write(buffer.c_str(), u32(sizeof(u8)*buffer.length()));
                        file->write(actEnd(), u32(sizeof(u8)*strlen(actEnd())));
                        file->write(bufferComponentInActorBase.cStr(), u32(sizeof(u8)*bufferComponentInActorBase.getLen()));
                        //file->write(actEndBuffer.cStr(), sizeof(u8)*actEndBuffer.getLen());
                        FILEMANAGER->closeFile(file);
                    }
                }
            }
        }
        // save all in actor file
        else if(saveActorEnable == ActorEditorParams::saveAllInActor)
        {
            if ( serializerLUA.OpenBufferToWrite("params", btrue, actorTemplate->GetObjectClassName()) )
            {
                actorTemplate->Serialize(&serializerLUA,ESerialize_Data_Save);
                editorParams->Serialize(&serializerLUA,ESerialize_Data_Save);
                ITF_STDSTRING buffer = serializerLUA.CloseBuffer();

                File* file = FILEMANAGER->openFile(actorPath, ITF_FILE_ATTR_WRITE|ITF_FILE_ATTR_CREATE_NEW);
                if ( file )
                {
                    file->write(buffer.c_str(), u32(sizeof(u8)*buffer.length()));
                    FILEMANAGER->closeFile(file);
                }
            }
        }

        actor->setOverrideTemplate(NULL);
        actor->oneStepHotReload();
   }

    void ActorCreatePlugin::onCloseActor(class Blob& _blob )
    {
        i32     actorIndex = 0;
        String  actorPath;

        actorIndex = _blob.extractInt32();
        if (actorIndex < 0 || actorIndex >= (i32)m_actorList.size() )
            return;

        clearActorIn(&m_actorList[actorIndex]);
        m_actorList.erase(m_actorList.begin() + actorIndex);

        sendActorList();

        if (m_actorIndex >= (i32)m_actorList.size())
            m_actorIndex--;
        
        if (m_actorIndex >= 0)
            sendActorComponentList(m_actorIndex);
    }

    void ActorCreatePlugin::onSpawnActor(class Blob& _blob )
    {
        i32     actorIndex = 0;

        actorIndex = _blob.extractInt32();
        // spawn only current actor
        if (actorIndex < 0 || actorIndex >= (i32)m_actorList.size() || actorIndex != m_actorIndex)
            return;

        Plugin* plugin = PLUGINGATEWAY->getPluginByName(ActorEditor::getPluginName());
        if(plugin) 
        {
            Actor * actor = getActor(actorIndex, bfalse);
            if (actor)
            {
                String pathTechno = DATA_FOLDER;
                pathTechno.replace('/', '\\');

                String path = pathTechno + actor->getLua().getString();
                ((ActorEditor*)plugin)->enterCreateActorState(path);
                m_state = Creator_Copy;
            }
        }
        
    }

    void ActorCreatePlugin::onGetActorEditorParams(class Blob& _blob)
    {
        i32 actorIndex = 0;
        actorIndex = _blob.extractInt32();

        bbool resetPath;
        resetPath = _blob.extractBool();

        fillComponentsPaths(actorIndex, resetPath);
        sendActorEditorParams(actorIndex);
    }

    void ActorCreatePlugin::onSetActorPause(class Blob& _blob )
    {
        i32 actorIndex = 0;
        actorIndex = _blob.extractInt32();

        i32 setpause = 0;
        setpause = _blob.extractInt32();

        // pause only current actor
        if (actorIndex < 0 || actorIndex >= (i32)m_actorList.size() || actorIndex != m_actorIndex)
            return;

        if (!setpause)
        {
            removeActorEditorFlags(ACTOR_IN_PAUSE);
            //reloadActor(actorIndex);
        } else
        {
            addActorEditorFlags(ACTOR_IN_PAUSE);
        }

        pauseActor(actorIndex, getActorEditorFlags() & ACTOR_IN_PAUSE);
        
        // dbg
        //reloadActor(actorIndex);
        
        sendActorList();
        sendActorCurrentData();
    }

    void ActorCreatePlugin::onChangeActorPause(class Blob& _blob )
    {
        i32 actorIndex = 0;
        actorIndex = _blob.extractInt32();

        // pause only current actor
        if (actorIndex < 0 || actorIndex >= (i32)m_actorList.size() || actorIndex != m_actorIndex)
            return;

        if (getActorEditorFlags() & ACTOR_IN_PAUSE)
        {
            removeActorEditorFlags(ACTOR_IN_PAUSE);
        } else
        {
            addActorEditorFlags(ACTOR_IN_PAUSE);
        }


        pauseActor(actorIndex, getActorEditorFlags() & ACTOR_IN_PAUSE);
        reloadActor(actorIndex);
        sendActorList();
        sendActorCurrentData();
    }


    void ActorCreatePlugin::onDumpToFileCmd( Blob& _blob )
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


    void ActorCreatePlugin::onActorPicked( ObjectRef _actor )
    {
        const BaseObject* pObj = GETOBJECT(_actor);
        ITF_ASSERT(pObj && pObj->getObjectType() == BaseObject::eActor);

        onBaseObjectPicked(_actor);
    }

    void ActorCreatePlugin::onBaseObjectPicked( ObjectRef _baseobj )
    {
        //BaseObject* obj = GETOBJECT(_baseobj);
        //if ( obj )
        //{
        //    m_currentSelection = _baseobj;

        //    if ( m_editorPeer )
        //    {
        //        SerializerToolXML serializer;
        //        char* buffer = NULL;
        //        size_t size;

        //        serializer.OpenToWrite(&buffer,&size);
        //        obj->Serialize(&serializer,ESerialize_Template_Save);
        //        serializer.Close();
        //        //sendProperties(buffer);
        //        SF_DEL(buffer);

        //        return;
        //    }
        //}

        //if(m_editorPeer)
        //    sendClearSelection();
    }

    void ActorCreatePlugin::UnselectAll(bbool /*_force*/)
    {
        //m_currentSelection = ITF_INVALID_OBJREF;
        //if(m_editorPeer)
        //    sendClearSelection();
    }

    
    // Send functions
    void ActorCreatePlugin::sendComponentList()
    {
        Blob b(BLOB_EXTRACT_AND_WRITE_ENABLED);
        b.pushString(ActorCreatePlugin::getPluginName());  // plugin ID
        b.pushString(getSendComponentListName()); 

        ObjectFactory * actorFactory = TEMPLATEDATABASE->getComponentsFactory();
        ITF_ASSERT(actorFactory);
        if (actorFactory)
        {
            ObjectFactory::ObjectsNamesContainer names = actorFactory->GetObjectsNames();

            b.pushInt32((i32)names.size());
            for (ObjectFactory::ObjectsNamesContainer::const_iterator iter = names.begin();
                iter != names.end(); ++iter)
            {
                b.pushString(iter->getDebugString());
            }
            PLUGINGATEWAY->sendPacket(b, m_editorPeer, m_editorSenderID);
        }
    }


    void ActorCreatePlugin::sendActorList()
    {
        Blob b(BLOB_EXTRACT_AND_WRITE_ENABLED);
        b.pushString(ActorCreatePlugin::getPluginName());  // plugin ID
        b.pushString(getSendActorListName());
        
        cleanActorList();
        i32 count = m_actorList.size();
        b.pushInt32(count);
        for (i32 i=0; i<count; i++)
        {
            Actor * actor = getActor(i, bfalse);
            if (actor)
            {
                String unicID;
                String name;
                    unicID = actor->getLua().getString();
                    name =  FilePath::getFilename(unicID);
                b.pushString(unicID);
                b.pushString(name);
                b.pushInt32(getActorEditorFlags(i));
            }
        }
        PLUGINGATEWAY->sendPacket(b, m_editorPeer, m_editorSenderID);
    }


    void ActorCreatePlugin::sendActorCurrentData()
    {
        if (m_actorIndex < 0 || m_actorIndex >= (i32)m_actorList.size())
            return;

        if (m_actorList[m_actorIndex].m_actorInMain)
        {
            sendActorMainData(m_actorIndex);
        } else
        {
            sendActorComponentList(m_actorIndex);
            if (!m_actorList[m_actorIndex].m_actorCurComponent.isEmpty())
            {
                sendActorComponentData(getActorComponent(m_actorList[m_actorIndex].m_actorCurComponent));
            }
        }
    }

    void ActorCreatePlugin::sendActorEditorParams(i32 _index)
    {
        ActorEditorParams * editorParams = getActorEditorParams(_index);
        if (!editorParams)
            return;

        ITF::Blob blob(ITF::BLOB_EXTRACT_AND_WRITE_ENABLED);
        blob.pushString(getPluginName());
        blob.pushString(getSendActorEditorParamsName());
        blob.pushInt32(_index);
        fillComponentsPaths(_index, bfalse);

        actorEditorParamsToBlob(editorParams, blob);

        const bbool bSendOk = PLUGINGATEWAY->sendPacket(blob, m_editorPeer, m_editorSenderID);
        if(!bSendOk)
            m_editorPeer = NULL;

        m_toolIsAware = bSendOk;
    }

    void ActorCreatePlugin::sendActorMainData(i32 _index)
    {
        Actor_Template * actorTemplate = getActorTemplate(_index);
        ActorEditorParams * editorParams = getActorEditorParams(_index);
        if (!actorTemplate || !editorParams || !actorTemplate)
            return;

        SerializerToolXML serializer;
        
        char* buffer = NULL;
        uSize size;
        serializer.OpenToWrite(&buffer,&size);
        //DummyActorMain dummyActor;
        //dummyActor.initFromActor(actorTemplate);


        actorTemplate->Serialize(&serializer,(ESerializeFlags)0); //ESerialize_Data_Save
        serializer.Close();
        
        ITF::Blob blob(ITF::BLOB_EXTRACT_AND_WRITE_ENABLED);
        blob.pushString(getPluginName());
        blob.pushString(getSendActorMainDataName());
        blob.pushInt32(_index);
        blob.pushString(buffer);
        SF_DEL(buffer);
        
        fillComponentsPaths(_index, bfalse);

        actorEditorParamsToBlob(editorParams, blob);

        const bbool bSendOk = PLUGINGATEWAY->sendPacket(blob, m_editorPeer, m_editorSenderID);
        if(!bSendOk)
            m_editorPeer = NULL;

        m_toolIsAware = bSendOk;
    }

    void ActorCreatePlugin::sendActorComponentList(i32 _index)
    {
        Actor_Template * actorTemplate = getActorTemplate(_index);
        if (!actorTemplate)
            return;

        Blob b(BLOB_EXTRACT_AND_WRITE_ENABLED);
        b.pushString(ActorCreatePlugin::getPluginName());  // plugin ID
        b.pushString(getSendActorComponentListName());
        b.pushInt32(_index);
        SafeArray<TemplateActorComponent*> actorComponents = actorTemplate->getComponents();
        i32 count = (i32)actorComponents.size();
        b.pushInt32(count);
        for (i32 i=0; i<count; i++)
        {
            String compoName = actorComponents[i]->GetObjectClassName();
            b.pushString(compoName);
        }
        PLUGINGATEWAY->sendPacket(b, m_editorPeer, m_editorSenderID);
    }

    void ActorCreatePlugin::sendActorComponentData(const String & _compoName)
    {
        if(_compoName == "Actor_Template")
        {
            sendActorMainData(m_actorIndex);
        }
        else 
        {
            sendActorComponentData(getActorComponent(_compoName));
        }
    }

    void ActorCreatePlugin::sendActorComponentData(TemplateActorComponent *_component)
    {
        if (!_component)
            return;

        SerializerToolXML serializer;
        
        char* buffer = NULL;
        uSize size;
        serializer.OpenToWrite(&buffer, &size);
        _component->Serialize(&serializer,(ESerializeFlags)0);   //ESerialize_Data_Save
        serializer.Close();

        sendActorComponentDataBuffer(_component->GetObjectClassName(), buffer);
        SF_DEL(buffer);
    }

    void ActorCreatePlugin::sendActorComponentDataBuffer(const String & componentName, const String8& _data)
    {
        ITF_ASSERT(m_editorPeer);

        ITF::Blob blob(ITF::BLOB_EXTRACT_AND_WRITE_ENABLED);
        blob.pushString(getPluginName());
        blob.pushString(getSendActorComponentDataBufferName());
        blob.pushString(componentName);
        blob.pushString(_data);

        const bbool bSendOk = PLUGINGATEWAY->sendPacket(blob, m_editorPeer, m_editorSenderID);
        if(!bSendOk)
            m_editorPeer = NULL;

        m_toolIsAware = bSendOk;
    }

    void ActorCreatePlugin::sendClearSelection()
    {
        ITF_ASSERT(m_editorPeer);

        ITF::Blob blob(ITF::BLOB_EXTRACT_AND_WRITE_ENABLED);
        blob.pushString(getPluginName());
        blob.pushString(getSendCommandClearName());

        const bbool bSendOk = PLUGINGATEWAY->sendPacket(blob, m_editorPeer, m_editorSenderID);
        if(!bSendOk)
            m_editorPeer = NULL;

        m_toolIsAware = bSendOk;
    }

    void ActorCreatePlugin::sendError(const String &_errorMsg)
    {
        ITF::Blob blob(ITF::BLOB_EXTRACT_AND_WRITE_ENABLED);
        blob.pushString(getPluginName());
        blob.pushString(getSendErrorName());
        blob.pushString(_errorMsg);

        PLUGINGATEWAY->sendPacket(blob, m_editorPeer, m_editorSenderID);
    }

    void ActorCreatePlugin::update()
    {
        // If we have a selection and the tool don't knows anything about it
        if(m_editorPeer && !m_toolIsAware && m_currentSelection != ITF_INVALID_OBJREF)
            onBaseObjectPicked(m_currentSelection);

        //if (m_state == Creator_Copy)
        //{
        //    EDITOR->showMouseIcon(Editor_IconStock::Icon_Plus);
        //}
    }

    void ActorCreatePlugin::blobToActorEditorParams(class Blob& _blob, ActorEditorParams * _params)
    {
        if (_params)
        {
            _params->m_indentationNbChars = _blob.extractUint32();
            
            // actor file
            _blob.extractString(_params->m_actorFile);
            _params->m_saveActorEnable = (ActorEditorParams::actorSaveType)_blob.extractInt32();
            
            // actor base file
            _blob.extractString(_params->m_actorBaseFile);
            _params->m_useActorBaseEnable = _blob.extractBool();
            _params->m_saveActorBaseEnable = _blob.extractBool();

            // component files
            u32 count = _blob.extractUint32();
            for (u32 i = 0; i < count; i++)
            {
                _params->m_componentFiles[i].blobToParams(_blob);
            }
        }
        // empty _blob
        else
        {
            _blob.extractBool();
            _blob.extractUint32();
            String out;
            // actor file
            _blob.extractString(out);
            _blob.extractBool();
            // actor base file
            _blob.extractString(out);
            _blob.extractBool();
            // component files
            u32 count = _blob.extractUint32();   ITF_ASSERT(count == 0);
        }
    }

    void ActorCreatePlugin::actorEditorParamsToBlob(const ActorEditorParams * _params, class Blob& _blob)
    {        
        if (_params)
        {
            _blob.pushUInt32(_params->m_indentationNbChars);

            // actor file
            _blob.pushString(_params->m_actorFile);
            _blob.pushInt32(_params->m_saveActorEnable);

            // actor base file
            _blob.pushString(_params->m_actorBaseFile);
            _blob.pushBool(_params->m_useActorBaseEnable);
            _blob.pushBool(_params->m_saveActorBaseEnable);

            // component files
            u32 count = _params->m_componentFiles.size();
            _blob.pushUInt32(count);
            for (u32 i = 0; i < count; i++)
            {
                _params->m_componentFiles[i].paramsToBlob(_blob);
            }
        } 
        // empty _blob
        else
        {            
            _blob.pushBool(bfalse);
            _blob.pushUInt32(0);
            // actor file
            _blob.pushUInt32(bfalse);
            _blob.pushString("Error");
            _blob.pushString("Error");
            // actor base file
            _blob.pushUInt32(bfalse);
            _blob.pushString("Error");
            // 0 files paths
            _blob.pushUInt32(0);
        }
    }

} // namespace ITF

#endif //!ITF_FINAL
