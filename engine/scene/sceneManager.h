#ifndef _ITF_SCENE_MANAGER_H_
#define _ITF_SCENE_MANAGER_H_

#ifndef _ITF_TEMPLATESINGLETON_H_
#include "core/templateSingleton.h"
#endif //_ITF_TEMPLATESINGLETON_H_

#ifndef _ITF_ID_SERVER_H_
#include "core/IDserver.h"
#endif //_ITF_ID_SERVER_H_

#ifndef _ITF_SEEDER_H_
#include "core/Seeder.h"
#endif //_ITF_SEEDER_H_

#ifndef _ITF_LOAD_INFO_H_
#include "engine/scene/loadInfo.h"
#endif //_ITF_LOAD_INFO_H_

#ifndef _ITF_SCENEEVENT_LISTENER_H_
#include "engine/scene/sceneEventListener.h"
#endif //_ITF_SCENEEVENT_LISTENER_H_

#ifndef ITF_TEMPLATECLIENTHANDLER_H
#include "engine/TemplateManager/TemplateClientHandler.h"
#endif // ITF_TEMPLATECLIENTHANDLER_H


namespace ITF
{
    class World;
    class Scene;
    class Thread;
    class Player;
    class ArchiveMemory;



    class  SceneSplitOnLoad
    {

    public:

        SceneSplitOnLoad() {clear();}
        bbool isIdle()              {return (m_ProcessStatus == ProcessStatusOff);}
        bbool isProcessing()        {return (m_ProcessStatus == ProcessStatusProcessing);}
        bbool isProcessingDone()    {return (m_ProcessStatus == ProcessStatusFinish);}
        void clear();
        void startProcessing();
        void onCollect(ITF_VECTOR<Scene*> &_asyncLoadedSceneDone);
        void process();
        void onFinish();

        const ITF_VECTOR<class Scene*>& getProcessedScenes()const { return m_processedScenes; }

    private:

        enum eProcessStatus  {

            ProcessStatusOff = 0,
            ProcessStatusCollecting,
            ProcessStatusProcessing,
            ProcessStatusFinish,
            ENUM_FORCE_SIZE_32(0)

        };

        ITF_VECTOR<class Pickable*> m_vActor;
        ITF_VECTOR<class Pickable*> m_vFrieze;
        ITF_VECTOR<class Pickable*> m_vSubScene;

        ITF_VECTOR<class Scene*>    m_processedScenes;

        eProcessStatus  m_ProcessStatus;

        bbool           m_isProcessingActor;
        bbool           m_isProcessingFrieze;

        u32             m_currentActorIndex;
        u32             m_currentFriezeIndex;

        TemplateClientHandler   m_templateClientHandler;
    };

    class SceneManager:public TemplateSingleton<SceneManager>
    {
        friend class Scene;

    public:

        typedef void (*startLoadingProfileCB)();
        typedef void (*stopLoadingProfileCB)();

        SceneManager();
        virtual ~SceneManager();

        void        destroy();

        bbool       isInMinimalLoadingMode() {return m_isInMinimalLoadingMode;}
        void        setIsInMinimalLoadingMode(bbool _minimalLoading) {m_isInMinimalLoadingMode = _minimalLoading;}

        Scene*      createScene(const Path& szPathName, bbool _binsertInList, ObjectRef _worldRef = ITF_INVALID_OBJREF);

        String      preloadScene(LoadInfo& _LoadInfo);
        
        Scene*      loadSceneForCooking(const String& _Path);
        bbool       unloadScene(Scene* _pScene);

        void        registerExistingScene(Scene* _pScene);
        
        Scene*      getSceneByUniqueName(const String & _uniqueName);
      
        void        buildUniqueSceneName(String& _uniqueSceneName,const Path& _sceneName);

        void        updateAsynchronousLoading();


        // Used in cooking
        ArchiveMemory*       saveBinaryScene(Scene* _pScene);


        void                    loadSceneAsynchronous(const LoadInfo& _preloadInfo);
        Scene*                  loadSceneSynchronous(LoadInfo& _loadInfo, bbool _binsertInList, bbool _bstartFrame = btrue, bbool _bCallOnLoaded = btrue);
        void                    addSceneAsyncLoadedDone_async(Scene* _pScene);
        void                    addListener(Interface_SceneEventsListener* _listener);
        void                    removeListener(Interface_SceneEventsListener* _listener);
        void                    addAlwaysActiveScene(Scene * _scene);
        void                    removeAlwaysActiveScene(Scene * _scene);
        Scene*                  prefetchScene( const String& mapPath );

        void                    onScenePreDestroy(Scene* _pScene) const;
        void                    onSceneDestroyed(Scene* _pScene) const;
        void                    broadcastObjectAdded(const Scene* _pScene, BaseObject* _pObj)const;
        void                    broadcastObjectRemoved(const Scene* _pScene, BaseObject* _pObj, bbool _delete)const;
        void                    broadcastPostSceneActivation(const Scene* _pScene, bbool _pActive)const;

        bbool                   isLoadingInProgress() const;
        ITF_INLINE bbool        isLoadingPending() const        { return isLoadingInProgress() || m_vPreloadedScene.size(); }
        u32                     getLoadedSceneCount()const      { return m_loadedScene.size(); }
        const ObjectRefList&    getLoadedScenes() const      { return m_loadedScene; }

        Scene*                  getLoadedSceneAt(u32 _index);

        void                    prepareOnLoad();
        void                    processOnLoad();
        void                    finishOnLoad();

        SceneSplitOnLoad        m_sceneSplitOnLoad;
    private:
        friend class AsyncSceneLoader;
        static void             asyncOneSceneLoad(Scene* _scene);
        static void             asyncOneScenePrefetched(Scene* _scene);

        Scene*                  loadScene(LoadInfo& _loadInfo, bbool _binsertInList, bbool _bstartFrame = btrue, bbool _bCallOnLoaded = btrue);

        SafeArray<Interface_SceneEventsListener*> m_pListeners;

        u32                             m_uniqueCountId; // used to build unique scene names
        ObjectRefList                   m_loadedScene;

        ITF_VECTOR<LoadInfo>            m_vPreloadedScene;
        ITF_VECTOR<Scene*>              m_asyncLoadedSceneDone; //The async thread tells that that the list scene is completed;
        ITF_THREAD_CRITICAL_SECTION     m_csSceneLoad;//CS to protect thread id

        bbool                           m_isInMinimalLoadingMode;
        ITF_MAP< Path, Scene* >         m_prefetchedScene;

        class AsyncSceneLoader *        m_asyncSceneLoaderThread;

    };

#define SCENE_MANAGER SceneManager::getptr()

} // namespace ITF

#endif // _ITF_SCENE_MANAGER_H_
