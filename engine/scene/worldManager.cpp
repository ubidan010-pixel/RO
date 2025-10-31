#include "precompiled_engine.h"

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_WORLD_MANAGER_H_
#include "engine/scene/worldManager.h"
#endif //_ITF_WORLD_MANAGER_H_

#ifndef _ITF_WORLD_H_
#include "engine/scene/world.h"
#endif //_ITF_WORLD_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef SERIALIZEROBJECTBINARY_H
#include "core/serializer/ZSerializerObjectBinary.h"
#endif // SERIALIZEROBJECTBINARY_H

#ifndef _ITF_XMLALL_H_
#include "engine/XML/XML_All.h"
#endif //_ITF_XMLALL_H_

#ifndef _ITF_OBJECTID_SERVER_H_
#include "core/ObjectIDserver.h"
#endif //_ITF_OBJECTID_SERVER_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_UIMENUMANAGER_H_
#include "engine/actors/managers/UIMenuManager.h"
#endif //_ITF_UIMENUMANAGER_H_

#ifndef _ITF_MENUMANAGER_H_
#include "engine/actors/managers/MenuManager.h"
#endif //_ITF_MENUMANAGER_H_

#ifndef _ITF_STIMSMANAGER_H_
#include "gameplay/managers/stimsmanager.h"
#endif //_ITF_STIMSMANAGER_H_

#ifndef _ITF_TESTMANAGER_H_
#include "engine/test/testmanager.h"
#endif //_ITF_TESTMANAGER_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_WORLD_EVENT_INTERFACE_H_
#include "engine/scene/worldEventInterface.h"
#endif //_ITF_WORLD_EVENT_INTERFACE_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_WORLDFILE_H_
#include "engine/scene/worldFile.h"
#endif //_ITF_WORLD_H_

#ifndef ITF_FINAL
    #ifndef _ITF_PROFILERMANAGER_H_
    #include "engine/profiler/ProfilerManager.h"
    #endif //_ITF_PROFILERMANAGER_H_
#endif

#ifndef _ITF_LIGHTCOMPONENT_H_
#include "gameplay/components/misc/LightComponent.h"
#endif //_ITF_LIGHTCOMPONENT_H_


#ifndef _ITF_SCENE_MANAGER_H_
#include "engine/scene/sceneManager.h"
#endif //_ITF_SCENE_MANAGER_H_

#ifndef _ITF_PARTICULEGENERATOR_H_
#include "engine/display/particle/ParticuleGenerator.h"
#endif //_ITF_PARTICULEGENERATOR_H_

#ifndef _ITF_CHEATMANAGER_H_
#include "engine/gameplay/CheatManager.h"
#endif //_ITF_CHEATMANAGER_H_

#ifndef _ITF_LOADINGSCREEN_H_
#include "engine/loading/loadingscreen.h"
#endif //_ITF_LOADINGSCREEN_H_

namespace ITF
{

    bbool WorldManager::m_is2DPass = bfalse;

    WorldManager::WorldManager()
        : m_rGlobalWorld(ITF_INVALID_OBJREF)
        , m_rSleepWorld(ITF_INVALID_OBJREF)
        , m_rCurrentWorld(ITF_INVALID_OBJREF)
        , m_bInitStaticWorldsCalledOnce(bfalse)
        , m_autoUnloadPreviousWorld(btrue)
        , m_defaultWorldFile(NULL)
    {
        Synchronize::createCriticalSection(&m_csWorldEvents);
    }

    WorldManager::~WorldManager()
    {
        Synchronize::destroyCriticalSection(&m_csWorldEvents);
        destroy();
    }

    ObjectRef WorldManager::createAndLoadEmptyWorld(bbool _ignoreCells)
    {
        LoadInfo info;
        info.m_bNodata = btrue;
        info.m_disconnectFromCells = _ignoreCells;

        ObjectRef rWorld =WORLD_MANAGER->newWorld();
        World* pWorld = static_cast<World*>(GETOBJECT(rWorld));
        if (pWorld)
        {
            if (!loadWorld(rWorld, info))
                return ITF_INVALID_OBJREF;
            return pWorld->getRef();
        }
        return ITF_INVALID_OBJREF;
    }

    ObjectRef WorldManager::createAndLoadWorld(LoadInfo& _sLoadInfo)
    {
        ObjectRef rWorld = newWorld();
        if(rWorld.isValid() && loadWorld(rWorld, _sLoadInfo))        
            return rWorld;
        return ITF_INVALID_OBJREF;
    }

    ObjectRef WorldManager::newWorld()
    {
        // Create a world 
        World* pWorld = newAlloc(mId_Scene,World());
        return pWorld->getRef();
    }

    bbool WorldManager::loadWorld(ObjectRef& _worldToLoad, LoadInfo& _loadInfo)
    {
        World* pWorld = (World*)GETOBJECT(_worldToLoad);
        ITF_ASSERT(pWorld);
        if(!pWorld)
            return bfalse;

        // and reference it
        if (!_loadInfo.m_disconnectFromCells)
            pWorld->createCellIndexes();

        addWorldToList(pWorld);

        WorldFile* pWorldFile = loadWorldFile(_loadInfo.m_mapPath);
        if(pWorldFile)
        {
            pWorld->setWorldFile(pWorldFile);

            // Override mapPath to continue with scene loading
            _loadInfo.m_mapPath = pWorldFile->getScenePath();
        }

        LOG_OLIV("[WM] New world from : %s", _loadInfo.m_mapPath.getString8().cStr());

        if(!_loadInfo.m_bNodata)
        {
            // This will set the worldID in the scene, before serialization process
            _loadInfo.m_worldRef = pWorld->getRef();

            // If the pointer is set then start asynchronous loading of the requested scene
            if(_loadInfo.m_bAsyncLoad)
            {
                pWorld->setSceneAsyncLoadRunning(btrue);

                // Call for asynchronous loading, will be call after the next call to SceneManager::update(...)
                SCENE_MANAGER->preloadScene(_loadInfo);
            }
            else
            {
                Scene* pRootScene = SCENE_MANAGER->loadSceneSynchronous(_loadInfo, btrue, bfalse, bfalse);
                ITF_ASSERT_MSG(pRootScene, "The scene could not be load");

                if(pRootScene)
                {
                    pWorld->addScene(pRootScene);

                    if(!SCENE_MANAGER->isInMinimalLoadingMode())
                        pRootScene->onSceneLoaded();

                    pRootScene->setForceLoadResources(_loadInfo.m_forceLoadResources);
                    if (_loadInfo.m_forceLoadResources)
                        pRootScene->forceLoadResourcesAndDisableUnload();
                }
            }
        }
        else
        {
            Scene* pRootScene = SCENE_MANAGER->createScene(_loadInfo.m_mapPath, btrue, pWorld->getRef());
            ITF_ASSERT_MSG(pRootScene, "The scene could not be created");

            if(pRootScene)
            {
                pWorld->addScene(pRootScene);
                pRootScene->setForceLoadResources(_loadInfo.m_forceLoadResources);
                if (_loadInfo.m_forceLoadResources)
                    pRootScene->forceLoadResourcesAndDisableUnload();
            }
        }
        
        //Register infos
        pWorld->setIgnoreCells(_loadInfo.m_disconnectFromCells);        
        pWorld->setAsyncLoadedWorld(_loadInfo.m_bAsyncLoad);

#if defined(_DEBUG)
        check();
#endif // _DEBUG

        return btrue;
    }

    bbool WorldManager::deleteWorld(World* _pWorld)
    {
        bbool bresult = bfalse;
        if(_pWorld)
        {
                ITF_ASSERT_MSG(!_pWorld->isSceneAsyncLoadRunning(),"Trying to delete a world that is in the middle of an async loading !");

                _pWorld->setActive(bfalse);

                for (u32 i = 0; i < m_pWorldEventsListeners.size(); i++)
                    m_pWorldEventsListeners[i]->onDeleteWorld(_pWorld);

                LOG_OLIV("[WM] Delete world : %s", _pWorld->getRootScene()->getPath().getString8().cStr());
                Scene* pScene = _pWorld->getRootScene();
                ITF_ASSERT_MSG(pScene, "The scene is already deleted");
                // This will unload all hierarchy recursively
                ITF_VERIFY_MSG(SCENE_MANAGER->unloadScene(pScene), "Can't unload unregistered scene");
                pScene = NULL;

                removeWorldFromList(_pWorld);
                SF_DEL(_pWorld);
                bresult = btrue;

#if defined(_DEBUG)
                check();
#endif // _DEBUG
        }

        return bresult;
    }

    World* WorldManager::getWorldFromStringID( const StringID& _uName ) const
    {
        const u32 worldCount = getWorldCount();
        for(u32 iWorld = 0; iWorld < worldCount; ++iWorld)
        {
            World* pWorld = getWorldAt(iWorld);
            if(StringID(pWorld->getUniqueName()) == _uName)
                return pWorld;
        }
        return NULL;
    }

    World* WorldManager::getWorldFromUniqueName( const String& _uName ) const
    {
        const u32 worldCount = getWorldCount();
        for(u32 iWorld = 0; iWorld < worldCount; ++iWorld)
        {
            World* pWorld = getWorldAt(iWorld);
            if(pWorld->getUniqueName() == _uName)
                return pWorld;
        }
        return NULL;
    }
    
    void WorldManager::onSceneAsyncLoadFinished(const Scene* _pScene)
    {
        ITF_ASSERT_MSG(_pScene, "No way ! The scene manager just told us that this scene finished to load !");

        World* pWorld = _pScene->getWorld();
        ITF_ASSERT_MSG(pWorld, "The scene must reference his world");

        if (pWorld->getIgnoreCells())
            _pScene->disconnectFromCells();

        pWorld->setSceneAsyncLoadRunning(bfalse);
        pWorld->setAlwaysActive(pWorld->isAlwaysActive());
    }

    void WorldManager::init()
    {
        SCENE_MANAGER->addListener(this);
        CAMERA->startFrame();
        initStaticWorlds();
    }

    void WorldManager::destroy()
    {
        // Destroy static worlds before the whole list
        if(m_bInitStaticWorldsCalledOnce)
            destroyStaticWorlds();

        while(getWorldCount() > 0)
        {
            ITF_VERIFY(deleteWorld(getWorldAt(0)));
        }

        SCENE_MANAGER->removeListener(this);
    }
    
    void WorldManager::initStaticWorlds()
    {
#if defined(_DEBUG)
        check();
#endif // _DEBUG

        if(m_bInitStaticWorldsCalledOnce)
        {
            ITF_ASSERT_MSG(0, "Only init once !");
            return;
        }

        m_rGlobalWorld = newStaticWorld("GlobalWorld", btrue, btrue);
        World* gWorld = static_cast<World*>(m_rGlobalWorld.getObject());
        gWorld->setAlwaysActive(btrue);
        m_rSleepWorld = newStaticWorld("SleepWorld", bfalse, btrue);
        gWorld = static_cast<World*>(m_rSleepWorld.getObject());
        gWorld->setAlwaysActive(bfalse);

        m_bInitStaticWorldsCalledOnce = btrue;

#if defined(_DEBUG)
        check();
#endif // _DEBUG
    }
    
    ObjectRef WorldManager::newStaticWorld(const String& _name, bbool _bAlwaysActive, bbool _ignoreCells)
    {
        World* pWorld = newAlloc(mId_Scene,World);
        Scene* pScene = newAlloc(mId_Scene,Scene);

        pScene->setPath(_name);
        pScene->setUniqueName(_name);

        pWorld->setIgnoreCells(_ignoreCells);
        pWorld->addScene(pScene);
        pWorld->setAlwaysActive(_bAlwaysActive);
        addWorldToList(pWorld);

        return pWorld->getRef();
    }

    bbool WorldManager::deleteStaticWorld(ObjectRef _wRef)
    {
        bbool bresult = bfalse;
        World* pWorld = static_cast<World*>(GETOBJECT(_wRef));
        if(pWorld)
        {
            pWorld->setActive(bfalse);

            Scene* pScene = pWorld->getRootScene();
            ITF_ASSERT(pScene);
            if(pScene)
                pScene->destroy();
            SF_DEL(pScene);

            ITF_VERIFY_MSG(removeWorldFromList(pWorld), "Can't remove static world from list");

            SF_DEL(pWorld);  
            bresult = btrue;
        }   

        return bresult;
    }

    void WorldManager::destroyStaticWorlds()
    {
#if defined(_DEBUG)
        check();
#endif // _DEBUG

        ITF_VERIFY(deleteStaticWorld(m_rGlobalWorld));
        m_rGlobalWorld = ITF_INVALID_OBJREF;

        ITF_VERIFY(deleteStaticWorld(m_rSleepWorld));
        m_rSleepWorld = ITF_INVALID_OBJREF;

#if defined(_DEBUG)
        check();
#endif // _DEBUG
    }

    void WorldManager::addWorldToList(World* _pWorld)
    { 
        ITF_ASSERT_MSG(m_worldList.find(_pWorld) == -1, "Don't add the world twice in the world list !");
        m_worldList.push_back(_pWorld);
    }

    bbool WorldManager::removeWorldFromList(World* _pWorld)
    {
        bbool result = bfalse;
        const i32 iIndex = m_worldList.find(_pWorld);
        if(iIndex >= 0)
        {
            m_worldList.eraseNoOrder(iIndex);
            result = btrue;
        }
        return result;
    }
    
    void WorldManager::switchToWorld(ObjectRef _wRef)
    {
        // If same world skip
        if(m_rCurrentWorld == _wRef)
        {
            ITF_ASSERT_MSG(0, "You are trying to switch to the current world, useless isn't it ? Maybe it's a logical problem.");
            return;
        }

        // Warn all listeners before switch
        World* pWorld = static_cast<World*>(GETOBJECT(_wRef));
        for (u32 i = 0; i < m_pWorldEventsListeners.size(); i++)
            m_pWorldEventsListeners[i]->onSwitchToWorld(pWorld, bfalse);

        // Disable and destroy previous world
        if(m_rCurrentWorld.isValid())
        {
            World* pWorld = static_cast<World*>(GETOBJECT(m_rCurrentWorld));
            ITF_ASSERT_MSG(pWorld, "The current world is a deleted world !");
            pWorld->setActive(bfalse);

            if(m_autoUnloadPreviousWorld)
            {
                ITF_VERIFY(deleteWorld(pWorld));
            }
        }

        m_rCurrentWorld = _wRef;

        // Activate new current world
        if(m_rCurrentWorld.isValid())
        {
            // Get the base scene and enable it
            World* pWorld = static_cast<World*>(GETOBJECT(_wRef));
            ITF_ASSERT_MSG(pWorld, "Can't switch to a deleted world");
            ITF_ASSERT_MSG(pWorld->getIgnoreCells() || !pWorld->isActive(), "The new world should be inactive");
            pWorld->setActive(btrue);
            pWorld->applyWorldFileSettings();
        }

        // Warn all listeners after switch
        for (u32 i = 0; i < m_pWorldEventsListeners.size(); i++)
            m_pWorldEventsListeners[i]->onSwitchToWorld(pWorld, btrue);
    }
    
    void WorldManager::reinit(bbool _bFullReinit)
    {
        // Handle active worlds
        for(u32 i = 0; i < getWorldCount(); ++i)
        {
            World* pWorld = getWorldAt(i);
            ITF_ASSERT_MSG(pWorld, "The manager contains a deleted world");

            if(pWorld->isActive())
                pWorld->reinit(_bFullReinit);
        }
    }

    void WorldManager::startFrame()
    {
        const SafeArray<View*>& views = View::getViews();        
        for (u32 i = 0; i < views.size(); i++)
        {
            if (views[i]->isActive() && views[i]->getCamera())
            {
                Camera* pCam = views[i]->getCamera();
                pCam->startFrame();
            }
        }
        
        // Handle all worlds
        const u32 uWorldCount = getWorldCount();
        for(u32 i = 0; i < uWorldCount; ++i)
        {
            World* pWorld = getWorldAt(i);
            ITF_ASSERT_MSG(pWorld, "The world manager contains a deleted world");

            pWorld->swapActiveObjectsList();
            pWorld->flushPending();
        }

        // Must be done in two separate steps

        // Handle active worlds
        bbool loadingScreenIsVisible = LOADINGSCREEN->isVisible();
        //bbool gameIsPaused = CHEATMANAGER->getPause() || GAMEMANAGER->isInPause();

        for(u32 i = 0; i < uWorldCount; ++i)
        {
            World* pWorld = getWorldAt(i);
            ITF_ASSERT_MSG(pWorld, "The world manager contains a deleted world");

            if  (
                    pWorld->isActive()      // world must be active
                    &&
                    !loadingScreenIsVisible
              /*      (
                        pWorld->getAllowUpdate()    // ok if world must be updated, but this will be false in case of pause
                        ||
                        (gameIsPaused && !loadingScreenIsVisible)
                    )*/
                )
                    pWorld->startFrame();


        }
    }

    void WorldManager::updateSceneGraph(f32 _elapsed)
    {
#ifndef ITF_FINAL
        PROFILERMANAGER->clear();
#endif

#ifdef ASSERT_ENABLED
        static u32 lastFrame = U32_INVALID;
        u32 currentFrame = CURRENTFRAME;
        ITF_ASSERT_MSG(lastFrame != currentFrame, "WorldManager::update is called twice in the main loop");
        lastFrame = currentFrame;
#endif // _DEBUG

#ifdef ITF_SUPPORT_EDITOR
        handleResourceErrors();
#endif //ITF_SUPPORT_EDITOR

        u32 listenersCount = m_pWorldEventsListeners.size();
        for (u32 i = 0; i < listenersCount; i++)
            m_pWorldEventsListeners[i]->onStartUpdateProcess(_elapsed);

        PHYSWORLD->update(_elapsed);

        // Handle active worlds
        for(u32 i = 0; i < getWorldCount(); ++i)
        {
            World* pWorld = getWorldAt(i);
            ITF_ASSERT_MSG(pWorld, "The manager contains a deleted world");

            if(pWorld->isActive() && pWorld->getAllowUpdate())
            {
                pWorld->update(_elapsed);
            }
        }

        for (u32 iGrp = 0; iGrp < ObjectGroup::GroupIndex_Max; iGrp++)
        {
#ifdef ITF_CTR
            m_updateGroups[iGrp].update(1.0f/60.0f);// temp for KOM CTR
#else
	        m_updateGroups[iGrp].update(_elapsed);
#endif
        }

        Pickable::registerAllModifiedInWorldCells();
        
        for (u32 iGrp = 0; iGrp < ObjectGroup::GroupIndex_Max; iGrp++)
        {
            m_updateGroups[iGrp].postUpdate();
        }

        m_is2DPass = btrue;
        for(u32 i = 0; i < getWorldCount(); ++i)
        {
            World* pWorld = getWorldAt(i);
            if(pWorld->isActive())
                pWorld->update2D(_elapsed);
        }

        for (u32 iGrp = 0; iGrp < ObjectGroup::GroupIndex_Max; iGrp++)
        {
#ifdef ITF_CTR
            m_update2DGroups[iGrp].update(1.0f/60.0f);// temp for KOM CTR
#else
	        m_update2DGroups[iGrp].update(_elapsed);
#endif
        }
        m_is2DPass = bfalse;

        Frise::UpdateAllAnimations(_elapsed);
        ITF_ParticleGenerator::FlushGenerators();

        ACTORSMANAGER->update(_elapsed);
        STIMSMANAGER->update(_elapsed);
#ifndef ITF_FINAL
        TESTMANAGER->update(_elapsed);
#endif // ITF_FINAL
        UI_MENUMANAGER->update();
		// JP : Commented as this is WIP
		//MENUMANAGER->update();

        for (u32 i = 0; i < listenersCount; i++)
            m_pWorldEventsListeners[i]->onUpdateProcess(_elapsed);

        for (u32 i = 0; i < listenersCount; i++)
            m_pWorldEventsListeners[i]->onEndUpdateProcess(_elapsed);
    }

    void WorldManager::draw()
    {
        // Handle active worlds
        for(u32 i = 0; i < getWorldCount(); ++i)
        {
            World* pWorld = getWorldAt(i);
            ITF_ASSERT_MSG(pWorld, "The manager contains a deleted world");

            if(pWorld->isActive() && pWorld->getAllowDraw())
                pWorld->fillDrawList();
        }

        // process light on particles gen.
        for (u32 j=0; j < m_particleGeneratorToLight.size(); j++)
        {
            for (u32 il = 0 ; il < m_activeLights.size() ; il++)
            {
                LightComponent *light = m_activeLights[il];
                ITF_ParticleGenerator* poolr = m_particleGeneratorToLight[j];
                light->processLight(NULL, poolr);
            }
        }
        m_particleGeneratorToLight.clear();


        for (u32 iGrp = 0; iGrp < ObjectGroup::GroupIndex_Max; iGrp++)
            m_drawGroups[iGrp].draw(m_activeLights);
        m_activeLights.clear();
        
        for(u32 i = 0; i < getWorldCount(); ++i)
        {
            World* pWorld = getWorldAt(i);
            ITF_ASSERT_MSG(pWorld, "The manager contains a deleted world");

            if(pWorld->isActive() && pWorld->getAllowUpdate())
                pWorld->postDraw();
        }

        for(u32 i = 0; i < getWorldCount(); ++i)
        {
            World* pWorld = getWorldAt(i);
            if(pWorld->isActive() && pWorld->getAllowDraw())
                pWorld->draw2D();
        }

    }
    

    
    void WorldManager::addEventsListener(Interface_WorldEventsListener* _listener)
    {
        csAutoLock cs(m_csWorldEvents);

        ITF_ASSERT(_listener);

        if (m_pWorldEventsListeners.find(_listener) == -1)
        {
            m_pWorldEventsListeners.push_back(_listener);
        }
        else
        {
            ITF_ASSERT_MSG(0, "Trying to register a listener already registered");
        }
    }

    void WorldManager::removeEventsListener(Interface_WorldEventsListener* _listener)
    {
        csAutoLock cs(m_csWorldEvents);

        ITF_ASSERT(_listener);

        const i32 index = m_pWorldEventsListeners.find(_listener);
        if (index != -1)
        {
            m_pWorldEventsListeners.eraseNoOrder((u32)index);
        }
        else
        {
            ITF_ASSERT_MSG(0, "Trying to unregister a unregistered listener");
        }
    }


    void WorldManager::pauseAllDraws()
    {
        for(u32 i = 0; i < getWorldCount(); ++i)
        {
            World* pWorld = getWorldAt(i);
            if(!pWorld)
                continue;

            pWorld->pushAllowDraw(bfalse);
        }
    }
    
    void WorldManager::pauseAllUpdates()
    {
        for(u32 i = 0; i < getWorldCount(); ++i)
        {
            World* pWorld = getWorldAt(i);
            if(!pWorld)
                continue;

            pWorld->pushAllowUpdate(bfalse);
        }
    }
    
    void WorldManager::unpauseAllDraws()
    {
        for(u32 i = 0; i < getWorldCount(); ++i)
        {
            World* pWorld = getWorldAt(i);
            if(!pWorld)
                continue;

            pWorld->popAllowDraw();
        }
    }
    
    void WorldManager::unpauseAllUpdates()
    {
        for(u32 i = 0; i < getWorldCount(); ++i)
        {
            World* pWorld = getWorldAt(i);
            if(!pWorld)
                continue;

            pWorld->popAllowUpdate();
        }
    }


    

#if defined(_DEBUG)
    
    void WorldManager::check()const
    {
        for(u32 i = 0; i < getWorldCount(); ++i)
        {
            const World* pWorld = getWorldAt(i);
            ITF_ASSERT_MSG(pWorld, "The WorldManager contains a deleted world...");
            if(!pWorld)
                continue;

            pWorld->check();
        }
    }

#endif // _DEBUG

    void WorldManager::flushPending()
    {
        for(u32 i = 0; i < getWorldCount(); ++i)
        {
            World* pWorld = getWorldAt(i);
            ITF_ASSERT_MSG(pWorld, "The WorldManager contains a deleted world...");
            if(!pWorld)
                continue;

            pWorld->flushPending();
        }
    }

void    WorldManager::checkPickableCanBeDeleted(const Pickable* _pick,u32 step)
{
    for (u32 iWorld = 0; iWorld < getWorldCount(); iWorld++)
    {
        World* curWorld = getWorldAt(iWorld);
        if (!curWorld)
            continue;

        for (u32 iScene = 0; iScene < curWorld->getSceneCount(); iScene++)
        {
            Scene* curScene = curWorld->getSceneAt(iScene);
            if (!curScene)
                continue;
            
            curScene->checkPickableCanBeDeleted(_pick,step);
        }
    }
}

#ifdef ITF_SUPPORT_EDITOR


    void    WorldManager::handleResourceErrors()
{
    ITF_VECTOR<ResourceManager::ResourceError> errList;
    RESOURCE_MANAGER->getErrors(errList);
    if (0 == errList.size())
        return;

    ITF_VECTOR<ResourceManager::ResourceError>::iterator ErrBeg = errList.begin();
    ITF_VECTOR<ResourceManager::ResourceError>::iterator ErrEnd = errList.end();
    ITF_VECTOR<ResourceManager::ResourceError>::iterator itErr = ErrBeg;
    PathString_t pathName;

    for (u32 iWorld = 0; iWorld < getWorldCount(); iWorld++)
    {
        World* curWorld = getWorldAt(iWorld);
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
                    for (itErr = ErrBeg; itErr != ErrEnd; ++itErr)
                    {
                        if(pGroup->usesResource((*itErr).m_id, btrue))
                        {
                            (*itErr).m_clientsFound++;
                            String Label = (*itErr).getErrorLabel();
                            Resource* res = ((*itErr).m_id).getResource();
                            if (res)
                            {
                                res->getPath().getString(pathName);
                                Label += FilePath::getFilename(pathName);
                            }
                            pFrieze->setResourceError(Label);
                            pFrieze->recomputeData();

                            pGroup->removeResource((*itErr).m_id, btrue);
                        }
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
                    for (itErr = ErrBeg; itErr != ErrEnd; ++itErr)
                    {
                        if (pGroup->usesResource((*itErr).m_id, btrue))
                        {
                            (*itErr).m_clientsFound++;
                            if (!pActor->hasDataError())
                            {
                                String Label = (*itErr).getErrorLabel();
                                Resource* res = ((*itErr).m_id).getResource();
                                if (res)
                                {
                                    res->getPath().getString(pathName);
                                    Label += FilePath::getFilename(pathName);
                                }
                                pActor->setResourceError(Label);
                            }
                        }
                    }
                }
            }
        }
    }

#ifdef ASSERT_ENABLED
    for (itErr = ErrBeg; itErr != ErrEnd; ++itErr)
    {
        if (0 == (*itErr).m_clientsFound)
        {
            Resource* res = ((*itErr).m_id).getResource();
            if (res) // twice the same code, the if() statement is there to avoid warnings with unused res variable
            {
                ITF_ASSERT_MSG(bfalse, "unhandled file error with resource: %s", res->getPath().getString8().cStr());
            }
            else
            {
                ITF_ASSERT_MSG(bfalse, "unhandled file error");
            }
            
        }
    }
#endif // ASSERT_ENABLED
}

#endif // ITF_SUPPORT_EDITOR

    void   WorldManager::addListener(Interface_WorldEventsListener* _listener)
    {
        ITF_ASSERT_MSG(WORLD_MANAGER, "The WorldManager must be created before trying to register in!");
        if(WORLD_MANAGER)
            WORLD_MANAGER->addEventsListener(_listener);
    }

    bbool   WorldManager::removeListener(Interface_WorldEventsListener* _listener)
    {
        if(!WORLD_MANAGER)
            return bfalse;

        WORLD_MANAGER->removeEventsListener(_listener);
        return btrue;
    }

    BaseObject* WorldManager::getObject( const Scene* _pContextScene, const ObjectId& _id )
    {
        if(_pContextScene)
            return _pContextScene->getObject(_id);

        BaseObject* result = NULL;
        const u32 worldCount = WORLD_MANAGER->getWorldCount();
        for(u32 i = 0; i < worldCount; ++i)
        {
            World* pWorld = WORLD_MANAGER->getWorldAt(i);
            ITF_ASSERT(pWorld);
            if(pWorld)
                result = pWorld->getObject(_id);

            if(result)
                break;
        }
        return result;
    }

    Actor* WorldManager::getActor( const Scene* _pContextScene, const ObjectId& _id )
    {
        BaseObject * obj = WorldManager::getObject(_pContextScene, _id);
        if (obj)
            return obj->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));
        return NULL;
    }

    void WorldManager::onStartDrawProcess()
    {
        for (u32 i = 0; i < m_pWorldEventsListeners.size(); i++)
            m_pWorldEventsListeners[i]->onStartDrawProcess();
    }
    
    void WorldManager::onEndDrawProcess()
    {
        for (u32 i = 0; i < m_pWorldEventsListeners.size(); i++)
            m_pWorldEventsListeners[i]->onEndDrawProcess();
    }

    WorldFile* WorldManager::loadWorldFile(const Path& _relativePath) const
    {
        if(String(_relativePath.getExtension()) == "isd")
        {
            WorldFile* pWorldFile = new WorldFile;
            if(pWorldFile->loadFromFile(_relativePath))
                return pWorldFile;

            SF_DEL(pWorldFile);
        }
        return NULL;
    }

    bbool WorldManager::isPrefetching() const
    {
        return View::getViews().size() > 0;
    }


    void WorldManager::getAll2DObjects(ObjectRefList& _list2D, bbool _activeScenesOnly)
    {
        const u32 worldCount = getWorldCount();
        for(u32 iWorld = 0; iWorld < worldCount; ++iWorld)
        {
            World* pWorld = getWorldAt(iWorld);

            const u32 sceneCount = pWorld->getSceneCount();
            for(u32 iScene = 0; iScene < sceneCount; ++iScene)
            {
                Scene* pScene = pWorld->getSceneAt(iScene);
                if (!_activeScenesOnly || pScene->isActive())
                    pScene->getAll2DObjects(_list2D);
            }
        }
    }

    /*    void WorldManager::onObjectBecomeInactive(Pickable* pObject)
    {
        m_updateGroups[pObject->getObjectGroupIndex()].removeElement(pObject);
        m_drawGroups[pObject->getObjectGroupIndex()].removeElement(pObject);
    }*/

} // namespace ITF
