#include "precompiled_engine.h"

#ifndef _ITF_SCENE_MANAGER_H_
#include "engine/scene/sceneManager.h"
#endif //_ITF_SCENE_MANAGER_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_THREAD_H_
#include "core/system/thread.h"
#endif //_ITF_THREAD_H_

#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_XMLALL_H_
#include "engine/XML/XML_All.h"
#endif //_ITF_XMLALL_H_

#ifndef _ITF_PICKABLE_H_
#include "engine/picking/Pickable.h"
#endif //_ITF_PICKABLE_H_

#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_XMLALL_H_
#include "engine/XML/XML_All.h"
#endif //_ITF_XMLALL_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef SERIALIZEROBJECTBINARY_H 
#include "core/serializer/ZSerializerObjectBinary.h"
#endif //SERIALIZEROBJECTBINARY_H

#ifndef _ITF_RESOURCEGROUP_H_
#include "engine/resources/ResourceGroup.h"
#endif //_ITF_RESOURCEGROUP_H_

#ifndef _ITF_OBJECTID_SERVER_H_
#include "core/ObjectIDserver.h"
#endif //_ITF_OBJECTID_SERVER_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_SYSTEMADAPTER_
#include "core/AdaptersInterfaces/SystemAdapter.h"
#endif //_ITF_SYSTEMADAPTER_

#ifndef _ITF_LOADING_SEQUENCE_H_
#include "engine/boot/LoadingSequence.h"
#endif //_ITF_LOADING_SEQUENCE_H_

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

namespace ITF
{

    //////////////////////////////////////////////////////////////////////////
    // Thread receiving scene load orders
    //////////////////////////////////////////////////////////////////////////

    class AsyncSceneLoader
    {
    public:
        // main thread API
        AsyncSceneLoader();
        ~AsyncSceneLoader();

        void start();
        void stop();
        bool isLoading();
        void requestLoad(Scene & _scene, bbool prefetch); // should only be called if !isLoading()

    private:
        static u32 WINAPI run(void* user);
        void run();

        Thread * m_sceneLoaderThread;

        ITF_THREAD_EVENT                m_event;
        ITF_THREAD_CRITICAL_SECTION     m_eventDataLocker; // CS to protect quit and add
        ITF_THREAD_EVENT                m_exitEvent;
        bool m_quit;
        Scene * m_sceneToLoad;
        bbool m_prefetchLoad;
    };

    AsyncSceneLoader::AsyncSceneLoader()
        : m_sceneLoaderThread(NULL)
        , m_quit(false)
        , m_sceneToLoad(NULL)
        , m_prefetchLoad(bfalse)
    {
        Synchronize::createCriticalSection(&m_eventDataLocker);
        Synchronize::createEvent(&m_event, "AsyncSceneLoader_Event");
        Synchronize::createEvent(&m_exitEvent, "AsyncSceneLoader_ExitEvent");
    }

    AsyncSceneLoader::~AsyncSceneLoader()
    {
        Synchronize::destroyCriticalSection(&m_eventDataLocker);
        Synchronize::destroyEvent(&m_event);
        Synchronize::destroyEvent(&m_exitEvent);
    }


    void AsyncSceneLoader::start()
    {
        ITF_ASSERT(m_sceneLoaderThread == NULL);
        m_sceneLoaderThread = THREADMANAGER->createThread(AsyncSceneLoader::run, (void *)this, ThreadSettings::m_settings[eThreadId_SceneManager]);
    }

    void AsyncSceneLoader::stop()
    {
        if (m_sceneLoaderThread)
        {
            {
                csAutoLock autoLock(&m_eventDataLocker);
                m_quit = true;
                Synchronize::setEvent(&m_event);
            }
            Synchronize::waitEvent(&m_exitEvent);
            m_sceneLoaderThread = NULL;
        }
    }

    bool AsyncSceneLoader::isLoading()
    {
        csAutoLock autoLock(&m_eventDataLocker);
        return m_sceneToLoad != NULL;

    }

    void AsyncSceneLoader::requestLoad(Scene & _scene, bbool prefetch) // should only be called if !isLoading()
    {
        csAutoLock autoLock(&m_eventDataLocker);
        ITF_ASSERT_CRASH(m_sceneToLoad == NULL, "Already loading !");
        m_sceneToLoad = &_scene;
        m_prefetchLoad = prefetch;
        Synchronize::setEvent(&m_event);
    }

    u32 WINAPI AsyncSceneLoader::run(void* user)
    {
        ((AsyncSceneLoader *)user)->run();
        return 0;
    }

    void AsyncSceneLoader::run()
    {
        csAutoLock autoLock(&m_eventDataLocker);

        while (!m_quit) // break at middle
        {
            while (!m_quit && m_sceneToLoad) // loop while we get levels to load
            {
                Scene * sceneToLoad = m_sceneToLoad;
                {
                    csAutoUnlock autoUnlock(&m_eventDataLocker);
                    if (m_prefetchLoad)
                        SCENE_MANAGER->asyncOneScenePrefetched(sceneToLoad);
                    else
                        SCENE_MANAGER->asyncOneSceneLoad(sceneToLoad);
                    ThreadSettings::threadStartFrame(); // One loaded scene is a "frame" for the async loader
                }
                m_prefetchLoad = bfalse;
                m_sceneToLoad = NULL; // allow new load
                SCENE_MANAGER->addSceneAsyncLoadedDone_async(sceneToLoad);
            }
            if (!m_quit)
            {
                // condition is now false, wait for condition change
                csAutoUnlock autoUnlock(&m_eventDataLocker);
                Synchronize::waitEvent(&m_event);
                Synchronize::resetEvent(&m_event);
            }
        }

        Synchronize::setEvent(&m_exitEvent);
    }


    ITF_MAP<Path, Scene*>   currentAsyncLoadingHistory;

    SceneManager::SceneManager()
        : m_uniqueCountId(0)
        , m_isInMinimalLoadingMode(bfalse)
        , m_asyncSceneLoaderThread(NULL)
    {
        Synchronize::createCriticalSection(&m_csSceneLoad);

        m_asyncSceneLoaderThread = newAlloc(mId_System, AsyncSceneLoader());
        m_asyncSceneLoaderThread->start();
    }

    SceneManager::~SceneManager()
    {
        destroy();
        Synchronize::destroyCriticalSection(&m_csSceneLoad);
    }

    Scene*  SceneManager::createScene(const Path& szPathName, bbool insertInList, ObjectRef _worldRef /*= ITF_INVALID_OBJREF*/)
    {
        Scene* pNewScene = newAlloc(mId_Scene,Scene());

        String uniqueName;
        buildUniqueSceneName(uniqueName,szPathName);
        pNewScene->setUniqueName(uniqueName);
        pNewScene->setPath(szPathName);
        pNewScene->setWorldID(_worldRef);
        
        if (insertInList)
        {
            registerExistingScene(pNewScene);
        }

        return pNewScene;
    }


    Scene*  SceneManager::getLoadedSceneAt(u32 _index)  
    {
        return static_cast<Scene*>(GETOBJECT(m_loadedScene[_index]));
    }

    bbool SceneManager::unloadScene(Scene* _pScene)
    {
        for(u32 i = 0; i < m_loadedScene.size(); ++i)
        {
            Scene* pScene = static_cast<Scene*>(GETOBJECT(m_loadedScene[i]));
            ITF_ASSERT_MSG(pScene, "There is a deleted scene in the scene list");
            if(!pScene)
                continue;

            if(pScene == _pScene)
            {
                m_loadedScene.eraseNoOrder(i);
                SF_DEL(_pScene);

                return btrue;
            }
        }
        return bfalse;
    }

    void SceneManager::registerExistingScene(Scene* _pScene)
    {
        ITF_ASSERT(_pScene);
        
        if(m_loadedScene.find(_pScene->getRef()) == -1)
        m_loadedScene.push_back(_pScene->getRef());
    }

    void  SceneManager::buildUniqueSceneName(String& _uniqueSceneName,const Path& _sceneName)
    {
        PathString_t sceneFilename;

        _sceneName.getString(sceneFilename);

        m_uniqueCountId++;
        String shortName = FilePath::getFilenameWithoutExtension(sceneFilename);
       _uniqueSceneName.setTextFormat("%s_%d", StringToUTF8(shortName).get(), m_uniqueCountId);
    }

    bbool SceneManager::isLoadingInProgress() const
    {
        return m_asyncSceneLoaderThread != NULL && m_asyncSceneLoaderThread->isLoading();
    }

    void SceneManager::updateAsynchronousLoading()
    {
        //RASTER_SCOPE(__FUNCTION__);

#ifdef ASSERT_ENABLED
        static u32 lastFrame = 666;
        ITF_ASSERT_MSG(lastFrame != CURRENTFRAME, "SceneManager::update is called twice in the main loop");
        lastFrame = CURRENTFRAME;
#endif

        {
            csAutoLock cs(m_csSceneLoad);


            if(m_sceneSplitOnLoad.isIdle())
            {
                if (m_asyncLoadedSceneDone.size())
                {
                    prepareOnLoad();
                    m_asyncLoadedSceneDone.clear();
                }
            }
            else
            {
                if (m_sceneSplitOnLoad.isProcessingDone())  //it's finish
                {   
                   finishOnLoad();
                }
                else if (m_sceneSplitOnLoad.isProcessing()) //execute the processing,call onLoad on pickable when its possible
                {   
                    processOnLoad();
                }
            }
        }
        
        //Preload pending and thread not running ==> Run a new one
        if (m_vPreloadedScene.size() && !isLoadingInProgress())
        {
            ITF_VECTOR<LoadInfo>::iterator iterPreload = m_vPreloadedScene.begin();
            loadSceneAsynchronous(*iterPreload);
            m_vPreloadedScene.erase(iterPreload);
        }
    }

    //--------------------------------------------------------------------------------------------------------------------------------------------------------

    String SceneManager::preloadScene(LoadInfo& _LoadInfo)
    {
        ITF_ASSERT(_LoadInfo.m_szUniqueName.isEmpty());
        buildUniqueSceneName(_LoadInfo.m_szUniqueName,_LoadInfo.m_mapPath);

        m_vPreloadedScene.push_back(_LoadInfo);
        return _LoadInfo.m_szUniqueName;
    }

    void SceneManager::addSceneAsyncLoadedDone_async(Scene* _pScene)
    {
        csAutoLock cs(m_csSceneLoad);
        m_asyncLoadedSceneDone.push_back(_pScene);
    }

    void SceneManager::addListener(Interface_SceneEventsListener* _listener)
    {
        csAutoLock cs(m_csSceneLoad);

        if (m_pListeners.find(_listener) == -1)
            m_pListeners.push_back(_listener);
    }

    void SceneManager::removeListener(Interface_SceneEventsListener* _listener)
    {
        csAutoLock cs(m_csSceneLoad);

        int index = m_pListeners.find(_listener);
        if (index != -1)
            m_pListeners.eraseNoOrder((u32)index);
    }

    Scene*  SceneManager::loadSceneForCooking(const String& _Path)
    {
        Scene* pScene = createScene(_Path,bfalse);
        
        pScene->addSerializationFlag(Scene::flag_isCooking);
        ITF_ASSERT(pScene);
        LoadInfo info;
        info.m_mapPath = _Path;
        if(!XMLAll::readMapFolder(pScene, info, btrue))
        {
            ITF_FATAL_ERROR("[SceneManager::loadSceneForCooking] readMapFolder failed for scene : %s", StringToUTF8(_Path).get());
        }
        pScene->loadSubScenes();

        if (!isInMinimalLoadingMode())
            pScene->onSceneLoaded();


        pScene->flushPending();

        return pScene;
    }

    void SceneManager::asyncOneSceneLoad(Scene* _scene)
    {
        currentAsyncLoadingHistory.clear();

        RESOURCE_MANAGER->pause();//make sure that not resource will be loaded while the scene is

        LoadInfo info;
        info.m_mapPath = _scene->getPath();
        info.m_bAsyncLoad = btrue;
        ITF_ASSERT(_scene->getWorldID().isValid());

        String mapPath;
        info.m_mapPath.getString(mapPath);
        String bundle = FilePath::getFilenameWithoutExtension( mapPath );

        FILEMANAGER->registerBundle( bundle, btrue );
        FILEMANAGER->loadBundles();
        ResourceManager::registerLocalThreadId(ResourceManager::asyncloadsceneThread);

        if(!XMLAll::readMapFolder(_scene, info))
        {
            ITF_FATAL_ERROR("[SceneManager::runAsyncScene] readMapFolder failed for scene : %s", _scene->getPath().getString8().cStr());
        }
        _scene->getPrefetchedList().clear();
        LoadingSequence::load(_scene->getDependenciesRef(), _scene->getPrefetchedList(), _scene->getTemplateClientHandler() );
        ITF_VECTOR< Path >().swap( _scene->getDependenciesRef() );
        _scene->loadSubScenes();

        ITF_MemoryBarrier();

        //FILEMANAGER->unregisterBundle( bundle + "_loading" );
        FILEMANAGER->unloadBundles();
        //FILEMANAGER->registerBundle( bundle );
        //if ( !_scene->getMusicThemePath().isEmpty() )
        //{
        //    String musicThemeBundle = FilePath::getFilenameWithoutExtension( _scene->getMusicThemePath().getString() );

        //    FILEMANAGER->registerBundle( musicThemeBundle );
        //}
        RESOURCE_MANAGER->unpause();

        currentAsyncLoadingHistory.clear();
    }

    void SceneManager::asyncOneScenePrefetched(Scene* _scene)
    {
        currentAsyncLoadingHistory.clear();

        RESOURCE_MANAGER->pause();//make sure that not resource will be loaded while the scene is

        ITF_ASSERT(_scene->getWorldID().isValid());

        ResourceManager::registerLocalThreadId(ResourceManager::asyncloadsceneThread);

        _scene->getDependenciesRef().clear();
        _scene->loadSubScenes();

        ITF_MemoryBarrier();

        RESOURCE_MANAGER->unpause();

        currentAsyncLoadingHistory.clear();
    }

    void SceneManager::loadSceneAsynchronous(const LoadInfo& _preloadInfo)
    {
        ITF_MAP< Path, Scene* >::iterator   it = m_prefetchedScene.find( _preloadInfo.m_mapPath );
        Scene*                              pScene = NULL;
        bbool                               prefetch = bfalse;

        if ( it != m_prefetchedScene.end() )
        {
            prefetch = btrue;
            pScene = it->second;
            pScene->setWorldID( _preloadInfo.m_worldRef );
            m_prefetchedScene.erase( it );
        }
        else
            pScene = createScene(_preloadInfo.m_mapPath, bfalse, _preloadInfo.m_worldRef);

        //Force the unique name
        pScene->setAsyncLoadingPending(btrue);
        pScene->setUniqueName(_preloadInfo.m_szUniqueName);
        pScene->setForceLoadResources(_preloadInfo.m_forceLoadResources);

        World* pWorld = pScene->getWorld();

        pWorld->setIgnoreCells(_preloadInfo.m_disconnectFromCells);

        ITF_ASSERT_CRASH(!m_asyncSceneLoaderThread->isLoading(), "One loading thread at a time");
        m_asyncSceneLoaderThread->requestLoad(*pScene, prefetch);
    }

    Scene* SceneManager::loadSceneSynchronous(LoadInfo& _loadInfo, bbool _binsertInList, bbool _bstartFrame /*btrue*/, bbool _bCallOnLoaded /* = btrue*/)
    {
        Scene* pScene = loadScene( _loadInfo, _binsertInList, _bstartFrame, _bCallOnLoaded );

        if ( pScene != NULL )
            pScene->loadSubScenes();
        return pScene;
    }

    Scene* SceneManager::loadScene(LoadInfo& _loadInfo, bbool _binsertInList, bbool _bstartFrame /*btrue*/, bbool _bCallOnLoaded /* = btrue*/)
    {
        ITF_MAP< Path, Scene* >::iterator   it = m_prefetchedScene.find( _loadInfo.m_mapPath );
        Scene*                              pScene = NULL;
        bbool                               prefetch = bfalse;

        if ( it != m_prefetchedScene.end() )
        {
            prefetch = btrue;
            pScene = it->second;
            pScene->setWorldID( _loadInfo.m_worldRef );
            m_prefetchedScene.erase( it );
            if ( _binsertInList )
                registerExistingScene( pScene );
        }
        else
            pScene = createScene(_loadInfo.m_mapPath, _binsertInList, _loadInfo.m_worldRef);
        ITF_ASSERT(pScene);

        const bbool isLoadingThread = Synchronize::getCurrentThreadId() == ThreadSettings::m_settings[eThreadId_SceneManager].m_threadID;
        if(isLoadingThread)
        {
            ITF_MAP<Path, Scene*>::iterator it = currentAsyncLoadingHistory.find(_loadInfo.m_mapPath);

            if(it != currentAsyncLoadingHistory.end())
            {
                ArchiveMemory archiveMem;   
                CSerializerObjectBinary serializeBin(&archiveMem);
                it->second->Serialize(&serializeBin, ESerialize_Data_Save);
                
                serializeBin.rewindForReading();
                pScene->Serialize(&serializeBin,ESerialize_Data_Load);
                pScene->setPath(_loadInfo.m_mapPath);
            }
            else
            {
                if (!_loadInfo.m_mapPath.isEmpty() && !prefetch )
                {
                    if(XMLAll::readMapFolder(pScene, _loadInfo))
                    {
                        currentAsyncLoadingHistory[_loadInfo.m_mapPath] = pScene;
                    }
                    else
                    {
                        ITF_FATAL_ERROR("[SceneManager::loadScene] readMapFolder failed for scene : %s", pScene->getPath().getString8().cStr());
                    }
                }
            }
        }
        else
        {
            if (!_loadInfo.m_mapPath.isEmpty() && !prefetch )
            {
                if(!XMLAll::readMapFolder(pScene, _loadInfo))
                {
                    ITF_FATAL_ERROR("[SceneManager::loadScene] readMapFolder failed for scene : %s", pScene->getPath().getString8().cStr());
                }
            }
        }

        if (_bCallOnLoaded && !isInMinimalLoadingMode())
            pScene->onSceneLoaded();
        
        if (_bstartFrame)
            pScene->flushPending(btrue);

        return pScene;
    }

    Scene* SceneManager::getSceneByUniqueName(const String & _uniqueName)
    {
        String lowerUniqueSceneName = _uniqueName;
        lowerUniqueSceneName.toLower();

        for(u32 i = 0; i < m_loadedScene.size(); ++i)
        {
            Scene* pScene = static_cast<Scene*>(GETOBJECT(m_loadedScene[i]));
            ITF_ASSERT(pScene);
            if(!pScene)
                continue;

            if (pScene->getUniqueName() == lowerUniqueSceneName)
            {
                return pScene;
            }
        }
        return NULL;
    }

    ArchiveMemory* SceneManager::saveBinaryScene(Scene* _pScene)
    {
        if (_pScene)
        {            
            ArchiveMemory* _ArchiveMem = new ArchiveMemory();     
            CSerializerObjectBinary serializeBin(_ArchiveMem);
            _pScene->Serialize(&serializeBin,ESerialize_Data_Save);

            return _ArchiveMem;
        }

        return NULL;
    }

    void SceneManager::onScenePreDestroy(Scene* _pScene) const
    {
        for (u32 iListeners = 0; iListeners < m_pListeners.size(); iListeners++)
        {
            m_pListeners[iListeners]->onScenePreDestroy(_pScene);
        }
    }

    void SceneManager::onSceneDestroyed(Scene* _pScene) const
    {
        for (u32 iListeners = 0; iListeners < m_pListeners.size(); iListeners++)
        {
             m_pListeners[iListeners]->onDeleteScene(_pScene);
        }
    }


    void SceneManager::destroy()
    {
        for(u32 i = 0; i < m_loadedScene.size(); ++i)
        {
            Scene* pScene = static_cast<Scene*>(GETOBJECT(m_loadedScene[i]));
            ITF_ASSERT(pScene);
            if(!pScene)
                continue;

            pScene->destroy();
            SF_DEL(pScene);
        }
        m_loadedScene.clear();

        m_asyncSceneLoaderThread->stop();
        SF_DEL(m_asyncSceneLoaderThread);
    }

    void SceneManager::addAlwaysActiveScene(Scene * _scene)
    {
    }

    void SceneManager::removeAlwaysActiveScene(Scene * _scene)
    {
    }

    Scene* SceneManager::prefetchScene( const String& mapPath )
    {
        ITF_MAP< Path, Scene* >::iterator it = m_prefetchedScene.find( mapPath );

        if ( it != m_prefetchedScene.end() )
            return it->second;

        Scene*      pScene = createScene( mapPath, bfalse, ITF_INVALID_OBJREF );
        LoadInfo    info;

        info.m_mapPath = mapPath;
        if ( !XMLAll::readMapFolder( pScene, info ) )
        {
            ITF_FATAL_ERROR("[SceneManager::runAsyncScene] readMapFolder failed for scene : %s", pScene->getPath().getString8().cStr());
        }
        m_prefetchedScene.insert( ITF_MAP< Path, Scene* >::value_type( mapPath, pScene ) );
        return pScene;
    }

    void SceneManager::broadcastObjectAdded( const Scene* _pScene, BaseObject* _pObj ) const
    {
        for (u32 iListeners = 0; iListeners < m_pListeners.size(); iListeners++)
        {
             m_pListeners[iListeners]->onObjectAdded(_pScene, _pObj);
        }
    }

    void SceneManager::broadcastObjectRemoved( const Scene* _pScene, BaseObject* _pObj, bbool _delete ) const
    {
        for (u32 iListeners = 0; iListeners < m_pListeners.size(); iListeners++)
        {
             m_pListeners[iListeners]->onObjectRemoved(_pScene, _pObj, _delete);
        }
    }

    void SceneManager::broadcastPostSceneActivation( const Scene* _pScene, bbool _pActive ) const
    {
        for (u32 iListeners = 0; iListeners < m_pListeners.size(); iListeners++)
        {
             m_pListeners[iListeners]->onPostSceneActivation(_pScene, _pActive);
        }
    }


//--------------------------------------------------------------------------------------------------------------------------------------------------------

    void SceneManager::prepareOnLoad()
    {
        RESOURCE_MANAGER->pause();
        m_sceneSplitOnLoad.onCollect(m_asyncLoadedSceneDone);
    }
    //--------------------------------------------------------------------------------------------------------------------------------------------------------

    void SceneManager::processOnLoad()
    {
        m_sceneSplitOnLoad.process();
    }
    //--------------------------------------------------------------------------------------------------------------------------------------------------------

    void SceneManager::finishOnLoad()
    {
        m_sceneSplitOnLoad.onFinish();
        const ITF_VECTOR<Scene*>& scenes = m_sceneSplitOnLoad.getProcessedScenes();

        for (ITF_VECTOR<Scene*>::const_iterator iterScene = scenes.begin() ;iterScene!=scenes.end();++iterScene)
        {
            Scene* pScene = *iterScene;

            /*     if (!isInMinimalLoadingMode())
            pScene->onSceneLoaded();
            */
            if (pScene->getForceLoadResources())
            {
                LOG("SCENE FORCED TO BE LOAD %s", pScene->getPath().getString8().cStr());
                pScene->forceLoadResourcesAndDisableUnload();
            }

            pScene->setAsyncLoadingPending(bfalse);

            m_loadedScene.push_back(pScene->getRef());

            for (u32 i = 0; i < m_pListeners.size(); i++)
                m_pListeners[i]->onSceneAsyncLoadFinished(pScene);
            LoadingSequence::releaseResources( pScene->getPrefetchedList() );
        }
        m_sceneSplitOnLoad.clear();
        RESOURCE_MANAGER->unpause();
    }

} // namespace ITF
