#include "precompiled_engine.h"

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_SUBSCENEACTOR_H_
#include "engine/actors/SubSceneActor.h"
#endif //_ITF_SUBSCENEACTOR_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_LUAHANDLER_H_
#include "core/script/LUAHandler.h"
#endif //_ITF_LUAHANDLER_H_

#ifndef _ITF_LOGICDATABASE_H_
#include "engine/logicdatabase/logicdatabase.h"
#endif //_ITF_LOGICDATABASE_H_

#ifndef SERIALIZEROBJECTBINARY_H
#include "core/serializer/ZSerializerObjectBinary.h"
#endif // SERIALIZEROBJECTBINARY_H

#ifndef _ITF_ANIMATIONMANAGER_H_
#include "engine/animation/AnimationManager.h"
#endif //_ITF_ANIMATIONMANAGER_H_

#ifndef ITF_TEMPLATEDATABASE_H
#include "engine/TemplateManager/TemplateDatabase.h"
#endif // ITF_TEMPLATEDATABASE_H

#ifndef SERIALIZEROBJECTLOGICDB_H
#include "engine/serializer/ZSerializerObjectLogicDB.h"
#endif //SERIALIZEROBJECTLOGICDB_H

#ifndef _ITF_ALIASMANAGER_H_
#include "engine/aliasmanager/aliasmanager.h"
#endif //_ITF_ALIASMANAGER_H_

namespace ITF
{

ActorsManager::SpawnInfo::SpawnInfo()
{
    m_init = btrue;
    m_pos = Vec3d::Zero;
    m_luaPath = Path::EmptyPath;
    m_pScene = NULL;
    m_pGroup = NULL;
    m_isInternal = bfalse;
    m_serializable = bfalse;
}

ActorsManager::ActorsManager()
    : m_hotReloadFct(NULL)
{
    m_ActorComponentsFactory        = newAlloc(mId_GameplayFactory,ObjectFactory);
    m_AIBehaviorsFactory            = newAlloc(mId_GameplayFactory,ObjectFactory);
    m_AIActionsFactory              = newAlloc(mId_GameplayFactory,ObjectFactory);
    m_SeqEventFactory               = newAlloc(mId_GameplayFactory,ObjectFactory);
    m_EventFactory                  = newAlloc(mId_GameplayFactory,ObjectFactory);
    m_AnimTreeNodeFactory           = newAlloc(mId_GameplayFactory,ObjectFactory);
    m_MusicTreeNodeFactory          = newAlloc(mId_GameplayFactory,ObjectFactory);

#ifdef ITF_WINDOWS
    m_actorCooking                  = bfalse;
#endif //ITF_WINDOWS
}

ActorsManager::~ActorsManager()
{
    delete m_AIActionsFactory;
    delete m_AIBehaviorsFactory;
    delete m_ActorComponentsFactory;
    delete m_SeqEventFactory;
    delete m_EventFactory;
    delete m_AnimTreeNodeFactory;
    delete m_MusicTreeNodeFactory;

    for (ITF_MAP<ActorRef,ActorDataContainer>::iterator it = m_serializeActorData.begin(); it != m_serializeActorData.end(); ++it)
    {
        ActorDataContainer& data = it->second;
        data.dispose();
    }
}



void ActorsManager::postload()
{
}

Actor* ActorsManager::spawnActor( const SpawnInfo& _spawnInfo )
{
    return spawnActorImpl<Actor>(_spawnInfo);
}

Actor* ActorsManager::spawnActor( const Vec3d& _pos, Scene* _scene, const Path& _luaPath, bbool _init, ResourceGroup* _group )
{
    SpawnInfo info;
    info.m_pos = _pos;
    info.m_pScene = _scene;
    info.m_luaPath = _luaPath;
    info.m_init = _init;
    info.m_pGroup = _group;

    return spawnActor(info);
}


SubSceneActor* ActorsManager::spawnSubsceneActor( const Vec3d& _pos, Scene* _scene, const Path& _luaPath, bbool _init, ResourceGroup* _group )
{
    SpawnInfo info;
    info.m_pos = _pos;
    info.m_pScene = _scene;
    info.m_luaPath = _luaPath;
    info.m_init = _init;
    info.m_pGroup = _group;

    SubSceneActor* pSSA = spawnActorImpl<SubSceneActor>(info);
    if(pSSA)
        pSSA->setZLock(btrue);

    return pSSA;
}

template <class T>
T* ActorsManager::spawnActorImpl( const SpawnInfo& _spawnInfo )
{
    ITF_ASSERT_MSG(_spawnInfo.m_pScene, "A valid destination scene must be provided");

    if (_spawnInfo.m_init && _spawnInfo.m_luaPath == Path::EmptyPath)
    {
        ITF_ASSERT_MSG(0, "A valid actor path must be provided");
        return NULL;
    }

    T* newActor = newAlloc(mId_Gameplay,T);

    newActor->setSerializable(_spawnInfo.m_serializable);
    newActor->setLua(_spawnInfo.m_luaPath);
    
    if (_spawnInfo.m_pGroup)
        _spawnInfo.m_pGroup->addResource(newActor->getResourceGroup());

#ifdef ITF_WINDOWS
    PathString_t name;
    _spawnInfo.m_luaPath.getBasenameWithoutExtension(name);
    name = String("*") + name;
    newActor->setUserFriendly(StringConverter(name).getChar());
    newActor->generateNewUserFriendly();
#endif //ITF_WINDOWS

    if (_spawnInfo.m_pScene)
    {
        if (newActor->getScene())
        {
            ITF_ASSERT_MSG(bfalse, "Newly spawned actor already bnelongs to a scene");
            newActor->getScene()->unregisterPickable(newActor, bfalse);
            newActor->getScene()->processUnregisterPending();
        }
        _spawnInfo.m_pScene->registerPickable(newActor);
        _spawnInfo.m_pScene->processRegisterPending();
    }

    newActor->setPos(_spawnInfo.m_pos);
    newActor->setWorldInitialPos(_spawnInfo.m_pos);
    newActor->growRelativeAABBFromAbsolute(_spawnInfo.m_pos);
    newActor->updateAbsoluteAABBFromRelative();
    
    if(_spawnInfo.m_init)
        newActor->onLoaded();

    return newActor;
}

void ActorsManager::update(f32 _elapsed)
{
    //RASTER_SCOPE(__FUNCTION__);
    if ( _elapsed == 0.0f )
    {
        return;
    }
}

#ifdef ITF_SUPPORT_EDITOR
void ActorsManager::hotReloadFromFile(const String & _relativeFilePath)
{
    ITF_ASSERT(FilePath::getExtension(_relativeFilePath) == "act");
    ITF_VECTOR<ActorHotReload> reloadActors;

    // SubSceneActor exception skip hotreload
    const Path pParam(_relativeFilePath);
    const Path pSubSceneActPath(GETPATH_ALIAS("subsceneactor"));
    if(pParam == pSubSceneActPath)
        return;

    String fixedRelativeFilePath = FilePath::normalizePath(_relativeFilePath);
    String8 fixedRelativeFilePath8(fixedRelativeFilePath.cStr());
    ITF_VECTOR<ObjectRef>* objList = LUA_HANDLER->getLUAFileClients(fixedRelativeFilePath8);

    if (objList)
    {
        reloadActors.reserve(objList->size());
        ITF_VECTOR<ObjectRef>::const_iterator sit = objList->begin();
        ITF_VECTOR<ObjectRef>::const_iterator send = objList->end();

        for (; sit != send; ++sit)
        {
            Actor* act = static_cast<Actor*>(GETOBJECT(*sit));
            ITF_ASSERT(act); //must be not null;if null the object has been deleted and still used in this list..
#ifndef ITF_FINAL
            if (act && act->getOverrideTemplate() == NULL)
#else
            if (act)
#endif
            {
                if(m_hotReloadFct)
                    m_hotReloadFct(act, btrue);

                reloadActors.resize(reloadActors.size()+1);
                ActorHotReload& curData = reloadActors[reloadActors.size()-1];
                curData.m_actorRef = *sit;
                act->fillHotReloadData(curData);
                act->clear();

                ActorsManager::ActorDataContainer* dataContainer = getSerializeDataActor(act);
                if (dataContainer)
                {
                    // Invalidate the checkpoint data in case a component was added/removed
                    SF_DEL(dataContainer->m_checkpoint);
                }
            }
        }
    }

    TEMPLATEDATABASE->reloadTemplate<Actor_Template>(_relativeFilePath);

    for ( u32 i = 0; i < reloadActors.size(); i++ )
    {
        ActorHotReload& hotreloadInfo = reloadActors[i];

        if(Actor* pActor = hotreloadInfo.m_actorRef.getActor())
        {
            pActor->hotReload(hotreloadInfo);
            if(m_hotReloadFct)
                m_hotReloadFct(pActor, bfalse);
        }
    }
 }

void ActorsManager::hotReloadActorsUsingResource(const ResourceID & _res)
{
    ITF_SET<String> actorPathList;
    PathString_t pathName;

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
            const PickableList&  actors = curScene->getActors();
            ITF_VECTOR<ObjectRef> actorsRefs;
            actorsRefs.reserve(actors.size());
            for (u32 itActor = 0; itActor < actors.size(); itActor++)
            {
                actorsRefs.push_back(actors[itActor]->getRef());
            }
            
            for (u32 itActor = 0; itActor < actorsRefs.size(); itActor++)
            {
                Actor* pActor = static_cast<Actor*>(actorsRefs[itActor].getObject());
                if (!pActor)
                    continue;
                ResourceGroup* pGroup = (ResourceGroup*)pActor->getResourceGroup();
                if (pGroup && pGroup->usesResource(_res, btrue))
                {
                    pActor->getLua().getString(pathName);

                    actorPathList.insert(pathName);
                }
            }
        }
    }

    for (ITF_SET<String> ::const_iterator iter = actorPathList.begin();
        iter != actorPathList.end();iter++)
    {
        hotReloadFromFile(*iter);
    }
}


void ActorsManager::onModifiedFile(const String& _absoluteFilename)
{
    String relativeFilePath = _absoluteFilename;
    FilePath::transformToRelative(relativeFilePath);

    String extension = FilePath::getExtension(_absoluteFilename);
    ITF_VECTOR<String> actFiles;

    if (extension == "act")
    {
        hotReloadFromFile(relativeFilePath);

    }
    else if (extension == "ilu")
    {
        const ITF_MAP<String_Uncached,LogicDataTableDependencies*> & dependenciesTables = LOGICDATABASE->getDependenciesTables();

        for (ITF_MAP<String_Uncached,LogicDataTableDependencies*>::const_iterator it = dependenciesTables.begin(); it != dependenciesTables.end(); ++it)
        {
            if (it->second->hasDependency(relativeFilePath))
            {
                actFiles.push_back(it->first);
                continue;
            }
        }

        for (ITF_VECTOR<String>::const_iterator it = actFiles.begin(); it != actFiles.end(); ++it)
        {
            hotReloadFromFile(*it);
        }
    }
    else if (extension == "skl" ||
             extension == "anm" ||
             extension == "pbk") // resources only need to reload resource !
    {
        ResourceID res;
        Resource * resPtr;
        // Synchronize data BEFORE getResourceIdFromFile
        bbool oldPaused = RESOURCE_MANAGER->isPaused();

        // Pause resource manager to prevent filling loading resource thread
        RESOURCE_MANAGER->pause();

        // Process all pending resources on loading thread 
        RESOURCE_MANAGER->synchronize();
        RESOURCE_MANAGER->updateFiles();

        if (RESOURCE_MANAGER->getResourceIdFromFile(res,relativeFilePath, Resource::ResourceType_All))
        {
            resPtr = res.getResource();
            ITF_ASSERT(resPtr);
            resPtr->reload();

            RESOURCE_MANAGER->synchronize();
            RESOURCE_MANAGER->updateFiles();

            hotReloadActorsUsingResource(res);
        }
#if defined(ITF_WINDOWS) && !defined(ITF_FINAL)
        // when changeing skl, we must change related anm files
        if (extension == "skl")
        {
            ITF_VECTOR<Path> animList;
            if (ANIM_MANAGER->getAnimTrackUsingSkeleton(relativeFilePath, animList))
            {
                for (ITF_VECTOR<Path>::iterator animIter = animList.begin();
                    animIter != animList.end(); ++animIter)
                {
                    if (!RESOURCE_MANAGER->getResourceIdFromFile(res,animIter->getStringID(), Resource::ResourceType_AnimTrack))
                        continue;

                    resPtr = res.getResource();
                    ITF_ASSERT(resPtr);
                    resPtr->reload();

                    RESOURCE_MANAGER->synchronize();
                    RESOURCE_MANAGER->updateFiles();

                    hotReloadActorsUsingResource(res);
                }
            }
        }
        if (!oldPaused)
        {
            RESOURCE_MANAGER->unpause();
        }

#endif
    }

}
#endif //ITF_SUPPORT_EDITOR

void ActorsManager::registerSerializeDataActor(Actor * _actor)
{
    ITF_ASSERT_MSG(_actor, "trying to registerSerializeDataActor a null actor");

    if (!_actor->isSerializable())
        return;

    ITF_ASSERT_CRASH(!_actor->isSpawned(),"Trying to register a spawned actor to F5 list");
    ITF_MAP<ActorRef,ActorDataContainer>::iterator it = m_serializeActorData.find(_actor->getRef());
    if (it == m_serializeActorData.end())
    {
        ActorDataContainer actorData;
        
        actorData.m_instance = newAlloc(mId_Editor,ArchiveMemory);
        CSerializerObjectBinary serializeBin(actorData.m_instance);
        _actor->Serialize(&serializeBin,ESerialize_Data_Save);

        m_serializeActorData[_actor->getRef()] = actorData;
    }

}
void ActorsManager::unregisterSerializeDataActor(Actor * _actor)
{
    ITF_ASSERT_MSG(_actor, "trying to unregister a null actor");

    if (!_actor->isSerializable())
        return;

    ITF_ASSERT_MSG(!_actor->isSpawned(),"Trying to unregister a spawned actor from F5 list");
    ITF_MAP<ActorRef,ActorDataContainer>::iterator it = m_serializeActorData.find(_actor->getRef());
    ITF_ASSERT_MSG(it != m_serializeActorData.end(),"Actor %s already unregistered ",_actor->getUserFriendly().cStr());
    if (it != m_serializeActorData.end())
    {
        it->second.dispose();
        m_serializeActorData.erase(it);
    }
}
ActorsManager::ActorDataContainer* ActorsManager::getSerializeDataActor(Actor * _actor)
{
    ITF_ASSERT_MSG(_actor, "trying to getSerializeDataActor a null actor");

    ITF_MAP<ActorRef,ActorDataContainer>::iterator it = m_serializeActorData.find(_actor->getRef());
    if (it!= m_serializeActorData.end())
    {
        return &it->second;
    }
    return NULL;
}


bbool ActorsManager::isActorRegistered( Actor * _actor ) const
{
    ITF_ASSERT_MSG(_actor, "trying to isActorRegistered a null actor");
    return m_serializeActorData.find(_actor->getRef()) != m_serializeActorData.end();
}

}
