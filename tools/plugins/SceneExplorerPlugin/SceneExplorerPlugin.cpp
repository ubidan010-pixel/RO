// GLOBAL ENGINE CONFIG FILE. Must be the first included file
#ifndef _ENGINECOMMON_H_
    #include "core/enginecommon.h"
#endif // _ENGINECOMMON_H_

#ifndef _ITF_SCENEEXPLORERPLUGIN_H_
#include "tools/plugins/SceneExplorerPlugin/SceneExplorerPlugin.h"
#endif //_ITF_SCENEEXPLORERPLUGIN_H_

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

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_SCENE_MANAGER_H_
#include "engine/scene/sceneManager.h"
#endif //_ITF_SCENE_MANAGER_H_

#ifndef _ITF_SUBSCENECOMPONENT_H_
#include "engine/actors/components/SubSceneComponent.h"
#endif //_ITF_SUBSCENECOMPONENT_H_

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_

#ifndef _ITF_EDITOR_H_
#include "editor/editor.h"
#endif //_ITF_EDITOR_H_

#ifndef _ITF_WORLD_H_
#include "engine/scene/world.h"
#endif //_ITF_WORLD_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/Managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef ITF_CONSOLE_FINAL    
    #include <io.h>
#endif // ITF_CONSOLE_FINAL

#ifndef _ITF_WORLD_MANAGER_H_
#include "engine/scene/worldManager.h"
#endif //_ITF_WORLD_MANAGER_H_

#ifndef _ITF_WORLD_H_
#include "engine/scene/world.h"
#endif //_ITF_WORLD_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_SUBSCENEACTOR_H_
#include "engine/actors/SubSceneActor.h"
#endif //_ITF_SUBSCENEACTOR_H_

namespace ITF
{

    volatile int g_reentrancyCount=0;

////////////////////////////////////////////////////////////////////////////////////////////////////////

SceneExplorerPlugin::SceneTree::SceneTree(const SceneTree &_other)
{
    ITF_ASSERT_MSG(0, "Copy constructor is not enabled");
}

SceneExplorerPlugin::SceneTree::~SceneTree()
{
    for (ITF_LIST<SceneTree*>::iterator iter=m_childScenes.begin(); iter!=m_childScenes.end(); iter++)
    {
        delete *iter;
    }
}

//
/*

//
bool SceneExplorerPlugin::SceneTree::areObjectListEqual(const PickableList &_list1, const PickableList &_list2)
{
    if (_list1.size()!=_list2.size())
        return false;
    for (size_t i=0; i<_list1.size(); i++)
        if (_list1[i]!=_list2[i])
            return false;
    return true;
}*/

//
bool SceneExplorerPlugin::SceneTree::isEqual_IncludingObjects(SceneTree *_other)
{
    //Scene comparison
    if (_other==NULL)
        return false;

    if (m_isRoot != _other->m_isRoot)
        return false;
    if (!(m_sceneUniqueName == _other->m_sceneUniqueName))
        return false;
    if (!(m_scenePath == _other->m_scenePath))
        return false;

    //Actors and frises
    if (!(m_actors == _other->m_actors))
        return false;
    if (!(m_frises == _other->m_frises))
        return false;
    
    //Recurse
    if (m_childScenes.size() != _other->m_childScenes.size())
        return false;

    ITF_LIST<SceneTree*>::const_iterator subscene1 = m_childScenes.begin();
    ITF_LIST<SceneTree*>::const_iterator subscene2 = _other->m_childScenes.begin();

    for (size_t subSceneIndex=0; subSceneIndex<m_childScenes.size(); subSceneIndex++, subscene1++, subscene2++)
    {        
        if (!(*subscene1)->isEqual_IncludingObjects(*subscene2))
            return false;
    }
    return true;
}

//
void SceneExplorerPlugin::SceneTree::saveToBlob(ITF::Blob &_blob, bool _offline)
{
    _blob.pushString(m_sceneUniqueName);
    _blob.pushString(m_scenePath);

    //list actors
    
    PathString_t actorPath;
    const PickableList &actorList=m_actors;
    _blob.pushUInt32(actorList.size());
    for (int actorIndex=0; actorIndex<(int)actorList.size(); actorIndex++)
    {
        Actor &actor=*(Actor*)actorList[actorIndex];
        String componentString;

        //push ObjectRef, friendly name, lua
        if (_offline) //Offline => we send ID instead
        {
            String8 objectID;
            actor.getObjectId(objectID);
            _blob.pushString(objectID);
        }
        else
            _blob.pushUInt32(actor.getRef().getValue());

        _blob.pushString(String(actor.getUserFriendly().cStr()));
        const Actor::ComponentsVector& components = actor.GetAllComponents();
        for (int componentIndex=0; componentIndex<(int)components.size(); componentIndex++)
        {
            if (componentString.getLen()!=0)
                componentString += ",";
            ActorComponent *component = components[componentIndex];
            if (component)
                componentString += StringID(component->GetObjectClassCRC()).getDebugString();
        }
        _blob.pushString(componentString);

        actor.getLua().getString(actorPath);

        _blob.pushString(actorPath);
    }

    //list frises    
    const PickableList &friseList=m_frises;
    PathString8_t pathName;
    _blob.pushUInt32(friseList.size());
    for (int friseIndex=0; friseIndex<(int)friseList.size(); friseIndex++)
    {
        Frise &frise=*(Frise*)friseList[friseIndex];

        //push objectref, friendly name, config
        if (_offline) //Offline => we send ID instead
        {
            String8 objectID;
            frise.getObjectId(objectID);
            _blob.pushString(objectID);
        }
        else
            _blob.pushUInt32(frise.getRef().getValue());

        _blob.pushString(frise.getUserFriendly());
        frise.getConfigNamePath().getString(pathName);
        _blob.pushString(pathName.cStr());
    }

    //recurse into child scenes
    _blob.pushUInt32(u32(m_childScenes.size()));
    for (ITF_LIST<SceneTree*>::iterator iter=m_childScenes.begin(); iter!=m_childScenes.end(); iter++)
    {
        (*iter)->saveToBlob(_blob, _offline);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
SceneExplorerPlugin::SceneExplorerPlugin()
: m_actorRef(ITF_INVALID_OBJREF),
m_mustSendSceneTreeASAP(false),
m_lastSceneTree(NULL)
{
}

SceneExplorerPlugin::~SceneExplorerPlugin()
{
    SF_DEL(m_lastSceneTree);
}

///////////////////////////////////////////////////////////////////////////////////////////
void SceneExplorerPlugin::received_SelectScene(Blob& _blob)
{
    String val;
    _blob.extractString(val);
    EDITOR->unselectAll();
    LOG("SceneExplorerPlugin : Select %ls\n", val.cStr());
}
///////////////////////////////////////////////////////////////////////////////////////////
void SceneExplorerPlugin::received_DeleteItem(Blob& _blob)
    {
        u32 objectRef = _blob.extractUint32();
        Pickable *pickable = (Pickable*)GETOBJECT(objectRef);
        if (pickable && 
            (pickable->getObjectType()==BaseObject::eActor || pickable->getObjectType()==BaseObject::eFrise) 
            )
        {
            if (EDITOR)
            {
                EDITOR->selectObject(objectRef, btrue);

                if (pickable)
                {
                    if(UNDOREDO_MANAGER)
                    {
                        if(SubSceneActor* pSSA = pickable->DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817)))
                            UNDOREDO_MANAGER->preDeleteGroup(pSSA);
                        else
                            UNDOREDO_MANAGER->preDelete(pickable);
                    }
                    pickable->requestDestruction();
                    EDITOR->setSceneAsModified(pickable->getScene());
                }
            }
        }

    }
///////////////////////////////////////////////////////////////////////////////////////////
void SceneExplorerPlugin::received_RenameItem(Blob& _blob)
    {
        u32 objectRef = _blob.extractUint32();
        String newName="";
        _blob.extractString(newName);
        if (newName.getLen()!=0)
        {
            Pickable *pickable = (Pickable*)GETOBJECT(objectRef);
            if (pickable && 
                (pickable->getObjectType()==BaseObject::eActor || pickable->getObjectType()==BaseObject::eFrise) 
                )
            {
                pickable->setUserFriendly(String8(newName.cStr()));
            }
    }
}
///////////////////////////////////////////////////////////////////////////////////////////
void SceneExplorerPlugin::received_DuplicateAndMoveItem(Blob& _blob)
{
    ObjectRef objectRef=0;
    Vec3d deltaPos;

    objectRef = _blob.extractUint32();
    deltaPos.m_x = _blob.extractFloat32();
    deltaPos.m_y = _blob.extractFloat32();
    deltaPos.m_z = _blob.extractFloat32();

    Pickable *pickable = (Pickable*)GETOBJECT(objectRef);

    //Duplicate & move
    Pickable *newPickable = NULL;
    if (pickable)
    {
        newPickable = pickable->clone(pickable->getScene(), btrue);
    }

    if (newPickable)
    {
        switch(newPickable->getObjectType())
        {
        case BaseObject::eActor: newPickable->offsetPosition(deltaPos, btrue); break;
        case BaseObject::eFrise: newPickable->offsetPosition(deltaPos, btrue); break;
        }
        //Add to selection list
        m_objectsToSelectASAP.push_back(newPickable->getRef());
    }
}
///////////////////////////////////////////////////////////////////////////////////////////
void SceneExplorerPlugin::received_SelectObjects(Blob& _blob)
    {
        u32 selectionCount=0;
        selectionCount = _blob.extractUint32();
        ObjectRefList selection;
        
        if (selectionCount!=0)
            selection.reserve(selectionCount);

        for (u32 index=0; index<selectionCount; index++)
        {
            ObjectRef ref=0;
            ref = _blob.extractUint32();
            
            selection.push_back(ref);
        }
        CAMERA->setMode(Camera::Camera_Editor);
        EDITOR->unselectAll();

        if (selection.size()!=0)
        {
            g_reentrancyCount ++;
            if (EDITOR)
            {
                EDITOR->selectObjects(selection, btrue);

                setCameraOnMultiSelection(selection);
            }
            g_reentrancyCount--;
            ITF_ASSERT(g_reentrancyCount>=0);
        }
    }

///////////////////////////////////////////////////////////////////////////////////////////
void SceneExplorerPlugin::receive(Blob& _blob,const NETPeer* _peer, u32 _senderID)
{
    String name;

    _blob.extractString(name);

    if ( name == "SelectScene" )
        received_SelectScene(_blob);
    else if (name== "SendSceneTree")
        m_mustSendSceneTreeASAP = true;
    else if (name=="DeleteItem")
        received_DeleteItem(_blob);
    else if (name=="RenameItem")
        received_RenameItem(_blob);
    else if (name=="DuplicateAndMoveItem")
        received_DuplicateAndMoveItem(_blob);
    else if (name=="SelectObjects")
        received_SelectObjects(_blob);
}

///////////////////////////////////////////////////////////////////////////////////////////
void SceneExplorerPlugin::setCameraOnMultiSelection(SafeArray<ObjectRef> &selection)
{
    Vec3d newCameraPos;
    Vec2d mean(0,0);
    int count = 0;

    //we get the mean of all suitable selected items. NB : Frises have no position 
    for (u32 index=0; index<selection.size(); index++)
    {
        ObjectRef ref = selection[index];
        Pickable* pPickable = static_cast<Pickable*>(GETOBJECT(ref));
        if (pPickable)
        {
         //   if (World::get2DObjects().find(ref)<0) //Don't focus on 2d objects
            if (pPickable->getObjectType() != BaseObject::eActor || !((Actor*)pPickable)->is2DActor())
            {
                mean += pPickable->getAnchorPos().truncateTo2D();
                count++;
            }
        }
    }
    if (count)
    {
        newCameraPos = CAMERA->getPos();
        newCameraPos.m_x = mean.m_x/count;
        newCameraPos.m_y = mean.m_y/count;
        CAMERA->forcePosition(newCameraPos);
    }
}
//
void SceneExplorerPlugin::sendUpdateIfInconsistent()
{
    if (!m_editorPeer)
        return;

    SceneTree *sceneTree = getSceneTree(NULL);
    if (m_lastSceneTree && !m_lastSceneTree->isEqual_IncludingObjects(sceneTree))
        sendSceneTree();
    delete sceneTree;
}
//
void SceneExplorerPlugin::add2DActors (PickableList &_actors, Scene *_baseScene)
{
    const PickableList& actors = _baseScene->get2DActors();

    for (u32 index=0; index<actors.size(); index++)
    {
        Actor* pActor = static_cast<Actor*>(actors[index]);
        if (_actors.find(pActor)<0 )
        {
            _actors.push_back(pActor);
        }
    }
}

//
void SceneExplorerPlugin::fillWithNonSpawnableActors(PickableList &_result, const PickableList &_sourceActors)
{
    _result.clear();
    u32 srcSize = _sourceActors.size();
    if (_result.capacity()<srcSize)
    {
        _result.reserve(srcSize);
    }
    for (u32 i=0; i<srcSize; i++)
    {
        Pickable *pickable = _sourceActors[i];
        if (pickable && pickable->getObjectType()==BaseObject::eActor)
        {
            _result.push_back(pickable);
        }
    }
}

//
SceneExplorerPlugin::SceneTree *SceneExplorerPlugin::getSceneTree(Scene *_theScene)
{
    //If _baseScene is NULL, we 1st search in root scene and other base scenes(if any)
    if (_theScene==NULL)
    {

        //We both get the main scene and base scene. Any can be null
        World* pCurrWorld = CURRENTWORLD;
        World* pBaseWorld = static_cast<World*>(GETOBJECT(GAMEMANAGER->getBaseMapWorld()));
        Scene *mainScene = pCurrWorld ? pCurrWorld->getRootScene() : NULL;
        Scene *baseScene = pBaseWorld ? pBaseWorld->getRootScene() : NULL;
        if (pBaseWorld)
        {
            /*if (!(baseScene && baseScene->isPhysicalReady()))
                baseScene = NULL;*/
            if (baseScene == mainScene)
                baseScene = NULL;
        }
        if (mainScene && baseScene)
        {
            //Both scenes are ok : we create a root scene
            SceneTree *rootScene = newAlloc(mId_Plugin,SceneTree);
            rootScene->m_isRoot = true;
            rootScene->m_childScenes.push_back(getSceneTree(baseScene));
            rootScene->m_childScenes.push_back(getSceneTree(mainScene));
            return rootScene;
        }
        else if (mainScene)
            return getSceneTree(mainScene);
        else if (baseScene)
            return getSceneTree(baseScene);
        return NULL;
    }

    //otherwise, search in given scene
    SceneTree *sceneTree = newAlloc(mId_Plugin, SceneTree);

    sceneTree->m_sceneUniqueName = _theScene->getUniqueName();
    
    _theScene->getPath().getString(sceneTree->m_scenePath);

    fillWithNonSpawnableActors(sceneTree->m_actors, _theScene->getActors());
    sceneTree->m_frises = _theScene->getFrises();

    add2DActors (sceneTree->m_actors, _theScene);
    const PickableList& actors = sceneTree->m_actors;

    //For each actor of given scene
    for (u32 actorIndex=0; actorIndex<actors.size(); actorIndex++)
    {
        Actor *actor = (Actor*)actors[actorIndex];
        if (actor)
        {
            //is it an anchor for a referenced scene?
            SubSceneComponent *subsceneComponent = actor->GetComponent<SubSceneComponent>();
            if (subsceneComponent)
            {
                //if yes, recurse
                Scene *subscene = subsceneComponent->getSubScene();
                if (subscene/*is ready*/) 
                {
                    SceneTree *subsceneTree = getSceneTree(subscene);
                    if (subsceneTree)
                        sceneTree->m_childScenes.push_back(subsceneTree);
                }
            }

        }
    }

    return sceneTree;

}

void SceneExplorerPlugin::onPostSceneActivation(const Scene *, bbool)
{
    if(m_editorPeer == NULL) // do nothing if not connected
        return;

    if (g_reentrancyCount==0)
        sendSceneTree();
}

void SceneExplorerPlugin::sendSceneTree()
{
    if (m_editorPeer)
    {
        SceneTree *sceneTree = getSceneTree(NULL);
        if (sceneTree)
        {
            ITF::Blob blob(ITF::BLOB_EXTRACT_AND_WRITE_ENABLED);
            blob.pushString(getPluginName());
            blob.pushString("SceneTree");
            sceneTree->saveToBlob(blob, false);
            PLUGINGATEWAY->sendPacket(blob, m_editorPeer, m_editorSenderID);
            delete m_lastSceneTree;
            m_lastSceneTree = sceneTree;
        }
    }
}


void SceneExplorerPlugin::UnselectAll(bbool _force)
{
    sendClearSelection();
}

void SceneExplorerPlugin::sendClearSelection()
{
}

void SceneExplorerPlugin::update()
{
    //First may update scene
    if (m_mustSendSceneTreeASAP)
    {
        m_mustSendSceneTreeASAP = false;
        sendSceneTree();
    }
    else
        sendUpdateIfInconsistent();


    //SelectObjects ASAP : must be done after scene update
    if (EDITOR && m_objectsToSelectASAP.size()>0)
    {
        EDITOR->selectObjects(m_objectsToSelectASAP, btrue);
        m_objectsToSelectASAP.clear();
    }

}

void  SceneExplorerPlugin::onFrisePicked   (ObjectRef _frise)
{
    onActorOrFrisePicked(_frise);
}

void SceneExplorerPlugin::onActorPicked(ObjectRef _actor)
{
    onActorOrFrisePicked(_actor);
}

void SceneExplorerPlugin::onActorOrFrisePicked(ObjectRef _item)
{
    if (!EDITOR || !m_editorPeer)
        return;


    //Sanity checks, in order to avoid problems with multiple selection in the future
    if (g_reentrancyCount!=0)
        return;

    //
    sendUpdateIfInconsistent();

    //send notification
    ITF::Blob blob(ITF::BLOB_EXTRACT_AND_WRITE_ENABLED);
    blob.pushString(getPluginName());
    blob.pushString("ObjectsSelectedInRuntime");
    blob.pushUInt32(_item.getValue());
    PLUGINGATEWAY->sendPacket(blob, m_editorPeer, m_editorSenderID);

}
//
bool SceneExplorerPlugin::isScenePathAlreadyLoaded(const ITF_LIST<SceneTree*> &_loadedScenes, const String &_scenePath)
{

    for (ITF_LIST<SceneTree*>::const_iterator iter = _loadedScenes.begin();
        iter!=_loadedScenes.end();
        iter++)
    {
        SceneTree *scene = *iter;
        if (wcscmp((const wchar_t*)scene->m_scenePath.cStr(), (const wchar_t*)_scenePath.cStr())==0)
        {
            return true;
        }
    }
    return false;
}

} // namespace ITF

