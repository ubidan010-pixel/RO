#include "precompiled_engine.h"

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_SUBSCENECOMPONENT_H_
#include "engine/actors/components/SubSceneComponent.h"
#endif //_ITF_SUBSCENECOMPONENT_H_

#ifndef _ITF_SUBSCENEACTOR_H_
#include "engine/actors/SubSceneActor.h"
#endif //_ITF_SUBSCENEACTOR_H_

#ifndef SERIALIZEROBJECTBINARY_H
#include "core/serializer/ZSerializerObjectBinary.h"
#endif // SERIALIZEROBJECTBINARY_H

#ifndef _ITF_BASEOBJECTFACTORY_H_
#include "engine/factory/baseObjectFactory.h"
#endif //_ITF_BASEOBJECTFACTORY_H_

#ifndef ITF_FINAL
#ifndef _ITF_CONFIG_H_
#include "core/Config.h"
#endif //_ITF_CONFIG_H_
#endif // ITF_FINAL

#ifndef _ITF_CHEATMANAGER_H_
#include "engine/gameplay/CheatManager.h"
#endif //_ITF_CHEATMANAGER_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_VERSIONING_H_
#include "core/versioning.h"
#endif //_ITF_VERSIONING_H_

#ifndef _ITF_LIGHTCOMPONENT_H_
#include "gameplay/components/misc/LightComponent.h"
#endif //_ITF_LIGHTCOMPONENT_H_

#ifndef _ITF_CELL_H_
#include "engine/scene/cell.h"
#endif //_ITF_CELL_H_

#ifndef _ITF_REPORTISSUEUTILS_H_
#include "engine/debug/reportIssueUtils.h"
#endif //_ITF_REPORTISSUEUTILS_H_

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_ANIMATIONRES_H_
#include "engine/animation/AnimationRes.h"
#endif //_ITF_ANIMATIONRES_H_

#ifndef _ITF_GAMEINTERFACE_H_
#include "engine/gameplay/GameInterface.h"
#endif //_ITF_GAMEINTERFACE_H_

#ifndef _ITF_GAMEMATERIAL_H_
#include "engine/gameplay/GameMaterial.h"
#endif //_ITF_GAMEMATERIAL_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/FilePath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_PROFILERMACRO_H_
#include "engine/profiler/profilerMacro.h"
#endif //_ITF_PROFILERMACRO_H_

#ifndef _ITF_SCENE_MANAGER_H_
#include "engine/scene/sceneManager.h"
#endif //_ITF_SCENE_MANAGER_H_

#ifndef _ITF_LOADING_SEQUENCE_H_
#include "engine/boot/LoadingSequence.h"
#endif //_ITF_LOADING_SEQUENCE_H_

#ifndef _ITF_PREFETCH_FCT_H_
#include "engine/boot/PrefetchFct.h"
#endif //_ITF_PREFETCH_FCT_H_

namespace ITF
{

    u32 Scene::m_staticSerializationFlags = 0;

    IMPLEMENT_OBJECT_RTTI(Scene)

    BEGIN_SERIALIZATION_CHILD(Scene)

        SERIALIZE_FUNCTION(clear, ESerialize_Data_Load);
        SERIALIZE_FUNCTION(preSerializeSave, ESerialize_Data_Save);
        SERIALIZE_FUNCTION(fixSerializableContent, ESerialize_Data_Save);

        u32 currentEngineVersion = Versioning::getEngineVersion();

        BEGIN_CONDITION_BLOCK(ESerialize_Data_Save)
            SERIALIZE_MEMBER("ENGINE_VERSION", currentEngineVersion);
        END_CONDITION_BLOCK()
        BEGIN_CONDITION_BLOCK(ESerialize_Data_Load)
            SERIALIZE_MEMBER("ENGINE_VERSION", m_fileEngineVersion);
        END_CONDITION_BLOCK()

#if !defined(ITF_CONSOLE)

        BEGIN_CONDITION_BLOCK(ESerialize_DataRaw)
            SERIALIZE_CONTAINER("TABS", m_associatedTabs);
            SERIALIZE_CONTAINER("GRIDUNIT", m_gridUnit);
            SERIALIZE_CONTAINER_OBJECT("PLATFORM_FILTER", m_filterByPlatform);
        END_CONDITION_BLOCK()

#endif //!ITF_CONSOLE

        BEGIN_CONDITION_BLOCK(ESerialize_DataBin)
            SERIALIZE_MEMBER("DEPENDENCIES", m_dependencies);
            SERIALIZE_MEMBER("MUSIC_THEME_PATH", m_musicThemePath);
        END_CONDITION_BLOCK()

        BEGIN_CONDITION_BLOCK(ESerializeGroup_Data)
            SERIALIZE_CONTAINER_WITH_FACTORY("FRISE", m_FriseList, &BASEOBJECT_FACTORY->m_objectFactory);
            SERIALIZE_CONTAINER_WITH_FACTORY("ACTORS", m_ActorsList, &BASEOBJECT_FACTORY->m_objectFactory);
            SERIALIZE_CONTAINER_OBJECT("friezeConnections", m_friezeConnections);
            SERIALIZE_MEMBER("MUSIC_THEME", m_musicTheme);
        END_CONDITION_BLOCK()

        SERIALIZE_FUNCTION(postSerializeSave, ESerialize_Data_Save);
        SERIALIZE_FUNCTION(fixSerializableContent, ESerialize_Data_Load);
        SERIALIZE_FUNCTION(finishSerializationLoad, ESerialize_Data_Load);
    
    END_SERIALIZATION()

    String Scene::s_CurrentBinaryTarget = "UNSET";

    bool PrefetchScene( const Path& path, PrefetchFactory::Params& params )
    {
        PathString_t sceneName;

        path.getString(sceneName);

        return SCENE_MANAGER->prefetchScene( sceneName ) != NULL;
    }

    Scene::Scene()
        : m_bActive(bfalse)
        , m_bAllowUnload(btrue)
        , m_serializationFlags(0)
        , m_worldID(ITF_INVALID_OBJREF)
        , m_forceLoadResources(bfalse)
        , m_isAlwaysActive(bfalse)
        , m_isAsyncLoadingPending(bfalse)
        , m_loaded(bfalse)
        , m_SubSceneActorHolder(NULL)
        , m_fileEngineVersion(0)
        , m_musicTheme(StringID::Invalid)
#if !defined(ITF_CONSOLE)
        , m_gridUnit(0.5f)
#endif // !ITF_CONSOLE
    {
        setObjectType(BaseObject::eScene);
    }

    Scene::~Scene()
    {
        destroy();
    }

    void Scene::preSerializeSave()
    {
        removeUnserializableFromLists();
    }

    void Scene::postSerializeSave()
    {
        restoreUnserializableFromLists();
    }

    f32 Scene::getReferenceDepth() const
    {
        if(SubSceneActor* pSSA = getSubSceneActorHolder())
            return pSSA->getDepth();

        return 0.0f;
    }

    void Scene::finishSerializationLoad()
    {
        if ( hasStaticSerializationFlag( flag_dependency ) )
            return;

        ITF_ASSERT(m_PickableList.size() == 0);
        m_PickableList.clear();

        // All frises must be in pickable list
        for (i32 it = 0; it < i32(m_FriseList.size()); ++it)
        {
            Frise* pFrise = m_FriseList[it]->DynamicCast<Frise>(ITF_GET_STRINGID_CRC(Frise,2579114664));
            if(!pFrise)
            {
                ITF_ASSERT_MSG(0, "Invalid frise in m_FriseList");
                m_FriseList.eraseNoOrder(it--);
                continue;
            }
            pFrise->preloadFileDependencies();

            m_PickableList.push_back(pFrise);
        }

        // All actors must be in pickable list
        for (i32 it = 0; it < i32(m_ActorsList.size()); ++it)
        {
            Actor* pActor = m_ActorsList[it]->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));
            if(!pActor)
            {
                ITF_ASSERT_MSG(0, "Invalid actor in m_ActorsList");
                m_ActorsList.eraseNoOrder(it--);
                continue;
            }

            m_PickableList.push_back(pActor);

            SubSceneActor* act = pActor->DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817));
            if (act)
            {
                ITF_ASSERT(m_SubSceneActors.find(act->getRef()) == -1);
                m_SubSceneActors.push_back(act->getRef());
            }
        }
        
        // Generic process of newly added pickables
        for (u32 it = 0; it < m_PickableList.size(); ++it)
        {
            Pickable* pObj = m_PickableList[it];

            // Finish scene assignation
            pObj->onAddedToScene(this);
            registerObjectWithID(pObj);
        }

#if defined(ASSERT_ENABLED)
        const u32 currentVersion = Versioning::getEngineVersion();
        if(currentVersion > 0) //if not in beta version
        {
            ITF_ASSERT_MSG(currentVersion >= m_fileEngineVersion, "Warning, loading map from more recent engine.");
        }
#endif // ASSERT_ENABLED
    }

    void Scene::destroy()
    {
        SCENE_MANAGER->onScenePreDestroy(this);

        //unload all resources...
		BaseObject *buffer[128];
		SafeArray<BaseObject*> resolvedRefs(128,MemoryId::mId_Temporary,buffer);
        ID_SERVER->getObjectListNoNullV2(getActiveObjects(), &resolvedRefs);
        const u32 uCount = resolvedRefs.size();
        for (u32 index = 0;index< uCount;++index)
        {
            Pickable* pObj = static_cast<Pickable*>(resolvedRefs[index]);
            pObj->unloadResources();
        }

        clear();
        SCENE_MANAGER->onSceneDestroyed(this);

        PathString_t pathName;

        m_szPath.getString(pathName);

        FILEMANAGER->unregisterBundle( FilePath::getFilenameWithoutExtension( pathName ) );
    }

    void Scene::getActiveObjectsRecursive(BaseObjectList& _listToFill) const
    {
        // Add local objects
        {
            const BaseObjectList& objList = getActiveObjects();
            _listToFill.mergeArray(objList);
        }

        {
			BaseObject *buffer[128];
			SafeArray<BaseObject*> resolvedRefs(128,MemoryId::mId_Temporary,buffer);
            ID_SERVER->getObjectListNoNull(m_SubSceneActors, resolvedRefs);
            const u32 count = resolvedRefs.size();
            for (u32 i = 0; i < count; i++)
            {
                Actor* pActor = static_cast<Actor*>(resolvedRefs[i]);
                SubSceneComponent* pSSC = pActor->GetComponent<SubSceneComponent>();
                if(const Scene* pScene = pSSC->getSubScene())
                    pScene->getActiveObjectsRecursive(_listToFill);
            }
        }
    }

    bbool Scene::isPhysicalReady() const
    {
        if (isAsyncLoadingPending())
            return bfalse;

        if(isEmpty())
            return btrue;

        bbool ret = btrue;
        if (getIsAlwaysActive() || (getWorld() && getWorld()->getIgnoreCells()))
        {
            for (u32 it = 0; it < m_PickableList.size(); ++it)
            {
                Pickable* pObj = m_PickableList[it];
                pObj->updatePhysicalReady();
                pObj->loadResources();
                if (!pObj->isPhysicalReady())
                {
                    #ifndef ITF_FINAL
                        if (reportIssueUtils::investigateMapLoadingStuck(pObj))
                            continue;
                    #endif
                    ret  = bfalse;
                }
                if (pObj->getCellEnableData().m_isSubSceneActor)
                {
                    SubSceneActor* pSSA = static_cast<SubSceneActor*>(pObj);
                    if(Scene* pSubScene = pSSA->getSubScene())
                        if (!pSubScene->isPhysicalReady())
                            ret  = bfalse;
                }
            }
        }

        return ret;
    }

    void Scene::activeAll(bbool _clearActiveListFirst)
    {
        if (_clearActiveListFirst)
        {
            clearActiveObjectsList();
        }
        u32 uSize = m_PickableList.size();
        for(u32 i = 0; i < uSize; ++i)
        {
            Pickable* pObj = m_PickableList[i];
            ITF_ASSERT(pObj);
            pObj->loadResources();
            addActiveObject(pObj->getRef());
        }
    }

    //--------------------------------------------------------------------------------------------------------

    void Scene::forceLoadResourcesAndDisableUnload()
    {
        u32 uSize = m_PickableList.size();
        for(u32 i = 0; i < uSize; ++i)
        {
            Pickable* pObj = m_PickableList[i];
            ITF_ASSERT_MSG(pObj, "The pickable list contains a NULL pointer");
            pObj->loadResources();
        }
    }

    //--------------------------------------------------------------------------------------------------------

    bbool Scene::isAlwaysActiveActor(Actor* _pActor)
    {
        return m_AlwaysActiveActors.find(_pActor) >= 0;
    }


    void   Scene::setAlwaysActiveActor(Actor* _pActor, bbool _alwaysActive)
    {
        ITF_ASSERT(_pActor);
        ITF_ASSERT(!_pActor->is2DActor());
        ITF_ASSERT(_pActor->getScene() == this);
  
        if (_alwaysActive)
        {
            ITF_ASSERT_MSG(m_AlwaysActiveActors.find(_pActor) < 0, "only set once please");
            ITF_ASSERT_MSG(m_ActorsList.find(_pActor) >= 0, "The actor must be registered");
            m_AlwaysActiveActors.push_back(_pActor);

            _pActor->loadResources();
            _pActor->unregisterFromWorldCells();
            _pActor->setAllowedCell(bfalse);
        }
        else
        {
            i32 index = m_AlwaysActiveActors.find(_pActor);
            ITF_ASSERT_MSG(index >= 0, "already not always active");
            ITF_ASSERT_MSG(m_ActorsList.find(_pActor) >= 0, "The actor must be registered");
            m_AlwaysActiveActors.eraseNoOrder(index);
            //_pActor->onBecomeInactive();
            _pActor->resetCellUpdateInfo();
            _pActor->setAllowedCell(btrue);
            _pActor->registerInWorldCells();
        }
    }
    
    //--------------------------------------------------------------------------------------------------------

    /*
    void  Scene::unregisterObjectFromActiveList(ObjectRef _ref)
    {
        ObjectRefList& objectToUpdate =   m_activeObjects;
        for (u32 it = 0; it < objectToUpdate.size(); it++)
        {
            if (objectToUpdate[it] == _ref)
            {
                objectToUpdate.eraseNoOrder(it);

                Pickable*  pObj =static_cast<Pickable*> (GETOBJECT(_ref));
                ITF_ASSERT(pObj);
                if ( pObj )
                {
                    pObj->unloadResources();
                }
                break;
            }
        }
    }
    */


//--------------------------------------------------------------------------------------------------------
    void Scene::fillUpdateList(ObjectGroup* _listToFill)
    {
        ITF_ASSERT(isActive());

        if (CHEATMANAGER->getPause())
            return;

        STATS_PICKABLE_INC(getPickableList().size())
        STATS_ACTOR_INC(getActors().size())
        STATS_FRIZE_INC(getFrises().size())

        const BaseObjectList& list = getActiveObjects();
        ID_SERVER->getObjectListNoNullV2(list);
        const u32 uActiveObjectCount = list.m_solvedPointers.size();

        // update all active objects
        for (u32 index = 0; index < uActiveObjectCount; ++index)
        {
            Pickable* pObj = (Pickable*)list.m_solvedPointers[index];

            if (pObj->isEnabled())   // can happen to get there for cinematics actors
                pObj->addObjectInGroupForUpdate(_listToFill);
        }
    }

    void Scene::fill2DUpdateList(ObjectGroup* _listToFill)
    {
        ITF_ASSERT(isActive());
        m_2DActorsToDraw.clear();
        if (!isActive())
        {
            return;
        }
        
        const bbool bPaused = CHEATMANAGER->getPause() || (!getWorld()->getAllowUpdate());

        u32 count = m_2DActorsList.size();

        const AABB & screenAABB = GFX_ADAPTER->getScreenAABB();
        u32 frame = CURRENTFRAME;

        for (u32 i = 0; i < count; i++)
        {
            Pickable* pObj = static_cast<Pickable*>(m_2DActorsList[i]);
            if(!pObj->isEnabled() || pObj->isTaggedForDeletion())
                continue;

            if (!pObj->isActive() && !bPaused)
                pObj->onBecomeActive();

            if (!pObj->isActive())  // onBecomeActive() can reject querry because resources are not ready
                continue;

            if(!bPaused && pObj->isPhysicalReady())
            {
                pObj->setActiveForFrame(frame);
                pObj->addObjectInGroupForUpdate(_listToFill);
            }
            else
            {
                if (pObj->getObjectType() == BaseObject::eActor)
                    ((Actor*)pObj)->updatePhysicalReady();
            }

            if (pObj->getAABB().checkOverlap(screenAABB))
                m_2DActorsToDraw.push_back(pObj);
        }
    }

    LightInfo* Scene::getLightAt(u32 _i)
    {
        if (_i >= m_activeLights.size()) return NULL;
        ObjectRef& ref = m_activeLights[_i];
        
        const Pickable* pObj = (Pickable*)ref.getObject();

        if (pObj)
        {
            const Actor* pAct = static_cast<const Actor*>(pObj);
            LightComponent *light = pAct->GetComponent<LightComponent>();
            return &light->m_lightInfo;
        }
        
        return NULL;
    }


    void Scene::fillDrawList()
    {
        ObjectRefList& lights = getActiveLightList();
        for (u32 il = 0; il < lights.size();  il++)
        {
            Actor* pActor = (Actor*)(lights[il].getObject());
            if (pActor)
            {
                LightComponent *light = pActor->GetComponent<LightComponent>();
                if (light)
                    WORLD_MANAGER->addActiveLight(light);
            }
        }

        const BaseObjectList& list = getActiveObjects();
        ID_SERVER->getObjectListNoNullV2(list);
        SafeArray<BaseObject*>& ObjToDraw = list.m_solvedPointers;

        const u32 uSize     = ObjToDraw.size();

        for (u32 index = 0;index<uSize;++index)
        {
            Pickable* pObj = (Pickable*)ObjToDraw[index];
            ITF_ASSERT_MSG(pObj->getSceneRef() == getRef(), "invalid scene object");

            bbool bAllowDraw = btrue;
            if (pObj->getObjectType() == BaseObject::eActor)
            {
                Actor * actor = (Actor * )pObj;
                if (actor->is2DActor())
                    continue;


                bAllowDraw = actor->isEnabled();
            }

            if (bAllowDraw && pObj->isVisible(CAMERA))
            {     
                pObj->setLastVisibleFrame(CURRENTFRAME);
                pObj->addObjectInGroupForDraw(); //pObj->draw();
            }
        }
     }

    void Scene::reinit( bbool _bFullReinit )
    {
        const u32 uSize = m_FriseList.size();
        for(u32 i = 0; i < uSize; ++i)
        {
            Frise* pObj = static_cast<Frise*>(m_FriseList[i]);
            ITF_ASSERT(pObj);
            pObj->recomputeData();
        }

		BaseObject *buffer[128];
		SafeArray<BaseObject*> resolvedRefs(128,MemoryId::mId_Temporary,buffer);
        ID_SERVER->getObjectListNoNull(m_SubSceneActors, resolvedRefs);
        const u32 count = resolvedRefs.size();
        for (u32 i = 0; i < count; i++)
        {
            SubSceneActor* pSSA = static_cast<SubSceneActor*>(resolvedRefs[i]);
            if(Scene* pScene = pSSA->getSubScene())
                pScene->reinit(_bFullReinit);
        }
    }

    void Scene::registerFrise(Pickable* _pObj)
    {
        ITF_ASSERT(_pObj != 0);
        
        m_Pending_RegisterFrise.push_back(_pObj);
    }

    void Scene::registerPickable( Pickable* _pObj )
    {
        ITF_ASSERT_CRASH(_pObj, "Bad parameter");
        ITF_ASSERT_MSG(Synchronize::getCurrentThreadId() == ThreadSettings::m_settings[eThreadId_mainThread].m_threadID, "m_Pending_RegisterActor is not thread safe");

        switch(_pObj->getObjectType())
        {
            case BaseObject::eActor:
                registerActor(_pObj);
                break;

            case BaseObject::eFrise:
                registerFrise(_pObj);
                break;

            default:
                ITF_ASSERT_CRASH(0, "Unknow type, what are trying to do ?");
                break;
        }
    }
    
    void Scene::unregisterPickable(Pickable* _pObj, bbool _delete/* = btrue*/)
    {
        ITF_ASSERT_CRASH(_pObj, "Bad parameter");

        switch(_pObj->getObjectType())
        {
            case BaseObject::eActor:
                unregisterActor(_pObj, _delete);
                break;

            case BaseObject::eFrise:
                unregisterFrise(_pObj, _delete);
                break;

            default:
                ITF_ASSERT_CRASH(0, "Unknow type, what are trying to do ?");
                break;
        }
    }

    void Scene::registerActor(Pickable* _pObj)
    {
        ITF_ASSERT_MSG(Synchronize::getCurrentThreadId() == ThreadSettings::m_settings[eThreadId_mainThread].m_threadID, "m_Pending_RegisterActor is not thread safe");
        ITF_ASSERT(_pObj != 0);
        m_Pending_RegisterActor.push_back(_pObj);
    }

    void Scene::unregisterFrise(Pickable* _pObj, bbool _delete)
    {
#if !defined(ITF_CONSOLE_FINAL)
        ITF_ASSERT_CRASH(_pObj && _pObj->getObjectType() == BaseObject::eFrise, "Invalid parameter");

        for(u32 i = 0; i < m_Pending_UnregisterFrise.size(); ++i)
        {
            ITF_ASSERT_CRASH(m_Pending_UnregisterFrise[i].first != _pObj, "Can't call unregister twice for the same object");
        }
#endif // ITF_CONSOLE_FINAL
        if (_delete)
            _pObj->setTaggedForDeletion();

        m_Pending_UnregisterFrise.push_back(std::pair<Pickable*, bbool>(_pObj, _delete));
    }


    void Scene::unregisterActor(Pickable* _pObj, bbool _delete)
    {
#if !defined(ITF_CONSOLE_FINAL)
        ITF_ASSERT_CRASH(_pObj && _pObj->getObjectType() == BaseObject::eActor, "Invalid parameter");

        for(u32 i = 0; i < m_Pending_UnregisterActor.size(); ++i)
        {
            ITF_ASSERT_CRASH(m_Pending_UnregisterActor[i].first != _pObj, "Can't call unregister twice for the same object");
        }
#endif // ITF_CONSOLE_FINAL
        if (_delete)
        {
            _pObj->setTaggedForDeletion();
        }

        m_Pending_UnregisterActor.push_back(std::pair<Pickable*, bbool>(_pObj, _delete));
    }

    void Scene::unregisterActorAvailableForSpawn(Actor* _pObj)
    {
        _pObj->deletionRequest();
        m_Pending_UnregisterActorAvailableForSpawn.push_back(std::pair<Pickable*, bbool>(_pObj, btrue));
    }

    void Scene::flushPending(bbool _recursive /*= bfalse*/)
    {
        processUnregisterPending();
        processRegisterPending();

        if(_recursive)
        {
			BaseObject *buffer[128];
			SafeArray<BaseObject*> resolvedRefs(128,MemoryId::mId_Temporary,buffer);
            ID_SERVER->getObjectListNoNull(m_SubSceneActors, resolvedRefs);
            const u32 uSSACount = resolvedRefs.size();
            for(u32 i = 0; i < uSSACount; ++i)
            {
                const SubSceneActor* pSSA = static_cast<SubSceneActor*>(resolvedRefs[i]);
                Scene* pScene = pSSA->getSubScene();
                ITF_ASSERT_MSG(pScene, "The subscene actor has no scene bound");
                if(!pScene)
                    continue;

                pScene->flushPending(btrue);
            }
        }
    }

    void Scene::add2DActor(Actor* _pAct)
    {
        ITF_ASSERT_MSG(m_2DActorsList.find(_pAct) < 0, "actor already inside scene");
        _pAct->setAllowedCell(bfalse);
        _pAct->loadResources();
        m_2DActorsList.push_back(_pAct);
    }

    void Scene::remove2DActor(Actor* _pAct)
    {
        i32 index = m_2DActorsList.find(_pAct);
        if (index >= 0)
        {
            _pAct->unloadResources();
            m_2DActorsList.eraseNoOrder(index);
        }
        
        index = m_2DActorsToDraw.find(_pAct);
        if (index >= 0)
            m_2DActorsToDraw.eraseKeepOrder(index);
    }

    void Scene::processRegisterPending()
    {
        const u32 uPFCount = m_Pending_RegisterFrise.size();
        const u32 uPACount = m_Pending_RegisterActor.size();

        Pickable* newPickablesBuff[512];
        PickableList newPickables(sizeof(newPickablesBuff)/sizeof(Pickable*), MemoryId::mId_Temporary, newPickablesBuff);
       
        // Flush frise
        for(u32 i = 0; i < uPFCount; ++i)
        {
            Pickable* pPickable = m_Pending_RegisterFrise[i];
            Frise* pFrise       = static_cast<Frise*>(pPickable);

            const i32 index = m_FriseList.find(pFrise);
            ITF_ASSERT_CRASH(index < 0, "The object is already in the list!");
            if(index < 0)
            {
                m_FriseList.push_back(pFrise);
                newPickables.push_back(pPickable);
            }
        }
        m_Pending_RegisterFrise.clear();


        // Flush actors
        for(u32 i = 0; i < uPACount; ++i)
        {
            Pickable* pPickable = m_Pending_RegisterActor[i];
            Actor* pActor       = static_cast<Actor*>(pPickable);

            const i32 index = m_ActorsList.find(pActor);
            ITF_ASSERT_CRASH(index < 0, "The object is already in the list!");
            if(index < 0)
            {
#ifdef CHECK_WORLDMANAGER_LIST
                if (WORLD_MANAGER)
                    WORLD_MANAGER->checkPickableCanBeDeleted(pActor,1);
#endif //CHECK_WORLDMANAGER_LIST

                m_ActorsList.push_back(pActor);
                newPickables.push_back(pPickable);

                if (pActor->is2DActor())
                    add2DActor(pActor);
            }

            if(SubSceneActor* act = pActor->DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817)))
            {
                ITF_ASSERT_CRASH(m_SubSceneActors.find(act->getRef()) == -1, "The object is already in the list!");
                m_SubSceneActors.push_back(act->getRef());
            }
        }
        m_Pending_RegisterActor.clear();

        const u32 uNewPickableCount = newPickables.size();
        for(u32 i = 0; i < uNewPickableCount; ++i)
        {
            Pickable* pPickable = newPickables[i];
                
            const i32 index = m_PickableList.find(pPickable);
            ITF_ASSERT_CRASH(index < 0, "The object is already in the list!");
            if(index < 0)
                m_PickableList.push_back(pPickable);

            pPickable->onAddedToScene(this);
            ITF_ASSERT_CRASH(pPickable->getSceneRef() == getRef(), "onAddedToScene is not doing its job");

            registerObjectWithID(pPickable);

            SCENE_MANAGER->broadcastObjectAdded(this, pPickable);
        }
    }

    void Scene::processUnregisterPending()
    {
        // Flush transfer list
        for(u32 i = 0; i < m_Pending_Tranfert.size(); ++i)
        {
            const std::pair<Pickable*, ObjectRef>& entry = m_Pending_Tranfert[i];
            Pickable* pPickable     = entry.first;
            Scene* destinationScene = static_cast<Scene*>(entry.second.getObject());

            unregisterPickable(pPickable, destinationScene == NULL);

            if(destinationScene)
                destinationScene->registerPickable(pPickable);

            ITF_ASSERT_MSG(destinationScene, "Destination scene doesn't exist!");
        }
        m_Pending_Tranfert.clear();

        Pickable*    removedListBuff[256];
        Pickable*    deletedListBuff[256];
        PickableList removedList(sizeof(removedListBuff)/sizeof(Pickable*),MemoryId::mId_Temporary,removedListBuff);
        PickableList deletedList(sizeof(deletedListBuff)/sizeof(Pickable*),MemoryId::mId_Temporary,deletedListBuff);

        // Because the below SF_DEL(....); performed when flushing deleteList can re-fill m_Pending_Unregister**
        while(m_Pending_UnregisterFrise.size() 
            || m_Pending_UnregisterActor.size() )
        {
            removedList.clear();
            deletedList.clear();

            // Flush frise list
            for(u32 i = 0; i < m_Pending_UnregisterFrise.size(); ++i)
            {
                Pickable* pPickable = m_Pending_UnregisterFrise[i].first;
                Frise* pFrise       = static_cast<Frise*>(pPickable);

                i32 index = m_FriseList.find(pFrise);
                ITF_ASSERT_CRASH( index >= 0, "Trying to remove an object pointer not in the container!");
                if(index >= 0)
                {
                    m_FriseList.eraseNoOrder(index);

                    ITF_ASSERT_CRASH( hasStaticSerializationFlag( flag_dependency ) || pFrise->getSceneRef() == getRef(), "can't unregister from anonymous scene");

                    if (m_Pending_UnregisterFrise[i].second) //delete requested
                    {
                        pPickable->deletionRequest();
                        deletedList.push_back(pPickable);
                    }
                    removedList.push_back(pPickable);
                }
            }
            m_Pending_UnregisterFrise.clear();

            // Flush actor list
            for(u32 i = 0; i < m_Pending_UnregisterActor.size(); ++i)
            {
                Pickable* pPickable = m_Pending_UnregisterActor[i].first;
                Actor* pActor       = static_cast<Actor*>(pPickable);

                // dpunset: I comment this assert because it's possible that an actor is unregistered twice. Some
                // actors need to spawn other actors, and when they are deleted they have to delete them too. Right
                // now the scene is automatically deleting every one, so it's possible that the deletion is
                // requested from the parent actor and the scene.
                // It's safe anyway, because if it's not on the containers we don't do anything with it.
                //ITF_ASSERT_MSG(index >= 0, "Trying to remove an object pointer not in the container!");

                i32 index = m_ActorsList.find(pActor);
                if(index >= 0)
                {
                    if(SubSceneActor* act = pActor->DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817)))
                    {
                        const i32 actIndex = m_SubSceneActors.find(act->getRef());
                        ITF_ASSERT_CRASH( hasStaticSerializationFlag( flag_dependency ) || actIndex >= 0, "all subscene actors should be in the list");
                        if (actIndex >= 0)
                            m_SubSceneActors.eraseNoOrder(actIndex);
                    }

                    m_ActorsList.eraseKeepOrder(index);

                    index = m_AlwaysActiveActors.find(pActor);
                    if (index >= 0)
                        m_AlwaysActiveActors.eraseNoOrder(index);

                    if (pActor->is2DActor())
                    {
                        remove2DActor(pActor);
                    }

                    ITF_ASSERT_CRASH( hasStaticSerializationFlag( flag_dependency ) || pActor->getSceneRef() == getRef(), "can't unregister from anonymous scene");

                    if (m_Pending_UnregisterActor[i].second) //delete requested
                    {
                        pActor->deletionRequest();

                        removedList.push_back(pPickable);
                        deletedList.push_back(pPickable);
                    }
                    else
                        removedList.push_back(pPickable);
                }
            }
            m_Pending_UnregisterActor.clear();

            // Generic process on removed objects
            for(u32 i = 0; i < removedList.size(); ++i)
            {
                Pickable* pObj = removedList[i];

                const i32 index = m_PickableList.find(pObj);
                ITF_ASSERT_CRASH( hasStaticSerializationFlag( flag_dependency ) || index >= 0, "Trying to remove an object pointer not in the container!");
                if(index >= 0)
                    m_PickableList.eraseKeepOrder(index);

                // If the object is still in the active object list
                // For exemple: It's happening in editor, when G is used 
                removeActiveObject(pObj->getRef());

                SCENE_MANAGER->broadcastObjectRemoved(this, pObj, deletedList.find(pObj) != -1);

                // Make sure the object belongs to a scene
                pObj->onRemovedFromScene(this,pObj->isDeletionRequested());    
                ITF_ASSERT_CRASH( hasStaticSerializationFlag( flag_dependency ) || NULL == pObj->getScene(), "onRemovedFromScene did not perform its duty");
                unregisterObjectWithID(pObj->getObjectId());
            }

            // Proceed to deletion after flushing all pending lists
            for(u32 i = 0; i < deletedList.size(); ++i)
            {
                Pickable* pObj = deletedList[i];

                ITF_ASSERT_MSG(removedList.find(pObj) != -1, "The pickable must be in the removed list");
                ITF_ASSERT(pObj->isDeletionRequested());
                SF_DEL(pObj);
            }
        }
    }

    void Scene::processUnregisterActorAvailableForSpawnPending()
    {
        PendingRequestList Pending_UnregisterActorAvailableForSpawn = m_Pending_UnregisterActorAvailableForSpawn;
        m_Pending_UnregisterActorAvailableForSpawn.clear();

        for (u32 i=0; i<Pending_UnregisterActorAvailableForSpawn.size(); i++)
        {
            std::pair<Pickable*, bbool> &item = Pending_UnregisterActorAvailableForSpawn[i];
            Actor *pObj = (Actor*)item.first;
            ITF_ASSERT(item.second);
            if (item.second)
            {
                ITF_ASSERT(pObj->isDeletionRequested());

                pObj->onRemovedFromScene(this,pObj->isDeletionRequested());
                unregisterObjectWithID(pObj->getObjectId());
                SF_DEL(pObj);
            }
        }
    }

    void Scene::switchToActive()
    {
        setActive(btrue);
    }

    void Scene::setActive(bbool _bActive)
    {
        if ( m_bActive != _bActive )
        {
            // We need to make sure everything is on its proper list before
            flushPending();

            if ( _bActive )
            {
                const u32 uSize = m_PickableList.size();
                for ( u32 i = 0; i < uSize; ++i )
                {
                    Pickable* obj = m_PickableList[i];
                    ITF_ASSERT_MSG(obj, "The pickable list must not contain a deleted pointer !");

                    if ( obj )
                    {
                        /*
                        if(!getIsAlwaysActive())
                        {
                            ITF_ASSERT_MSG(!obj->isActive(), "Warning! the obj is active before scene activation...");
                        }
                        */
                        obj->onSceneActive();
                    }
                }
            }
            else
            {
                u32 uSize = m_PickableList.size();
                for ( u32 i = 0; i < uSize; ++i )
                {
                    Pickable* obj = m_PickableList[i];
                    ITF_ASSERT_MSG(obj, "The pickable list must not contain a deleted pointer !");

                    if ( obj )
                    {
                        obj->onSceneInactive();
                    }
                }

                // Deactivate all the previously active objects and flush the lists
                const BaseObjectList& list = getActiveObjects();
                ID_SERVER->getObjectListNoNullV2(list);
                SafeArray<BaseObject*>& resolvedRefs = list.m_solvedPointers;

                uSize = resolvedRefs.size();
                for ( u32 index = 0; index < uSize; index++ )
                {
                    Pickable* pObj = static_cast<Pickable*>(resolvedRefs[index]);
                    pObj->onBecomeInactive();
                }

                clearActiveObjectsList();
            }
            
            m_bActive = _bActive;
            SCENE_MANAGER->broadcastPostSceneActivation(this, _bActive);
        }
    }

    void Scene::clear()
    {
#ifdef _DEBUG
        checkConsistency();
#endif // _DEBUG

        m_Pending_Tranfert.clear();

        flushPending();

        // scene unregistering is performed from most complex objects to less complex objects:
        // - actors can be frieze clients so they are deleted first
        for(u32 i = 0; i < m_ActorsList.size(); ++i)
            unregisterActor(m_ActorsList[i]);

        processUnregisterPending();
        
        for(u32 i = 0; i < m_FriseList.size(); ++i)
            unregisterFrise(m_FriseList[i]);

        processUnregisterPending();


        processUnregisterActorAvailableForSpawnPending();


        ITF_ASSERT(m_PickableList.size() == 0);

        ITF_ASSERT_MSG(m_objectIdMap.size() == 0, "Some objects were not unregistered properly");

        clearObjectIDMap();
        m_filterByPlatform.clear();

        m_loaded = bfalse;
        m_templateClientHandler.freeUsedTemplates();
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    void  Scene::onSceneCollect(ITF_VECTOR<Pickable*>& _vActor,ITF_VECTOR<Pickable*>& _vFrieze)
    {
        // init actors first to solve groups
        PickableList&  actors = m_ActorsList;
        u32 actorsCount = actors.size();    // PRE-FECTHING COUNT BECAUSE onLaoded() CAN SPAWN ACTORS FOR WHICH onLoaded MUSTN'T BE CALLED

        for (u32 itActor = 0; itActor < actorsCount; itActor++)
        {
            Actor * pActor = static_cast<Actor*>(actors[itActor]);
           _vActor.push_back(pActor);
           pActor->onSceneCollect( _vActor, _vFrieze);

        }

        PickableList& friezeList = m_FriseList;
        u32 friezecount = friezeList.size();
        for (u32 i=0; i< friezecount; i++)
        {
            Frise* pFrz = static_cast<Frise*>(friezeList[i]);
            _vFrieze.push_back(pFrz);
        }
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    void Scene::onSceneLoaded()
    {
        ITF_ASSERT_CRASH(Synchronize::getCurrentThreadId() == ThreadSettings::m_settings[eThreadId_mainThread].m_threadID, "Wrong thread...");

#ifdef ASSERT_ENABLED
        World* pWorld = getWorld();
        ITF_ASSERT_CRASH(pWorld, "At this point the scene must belongs to a world, maybe Word::addScene(this) is missing before calling Scene::onSceneLoaded()");
#endif // ASSERT_ENABLED

        ITF_ASSERT(!m_loaded);
        if ( m_loaded )
            return;
        m_loaded = btrue;

        // init actors first to solve groups
        PickableList&  actors = m_ActorsList;
        u32 actorsCount = actors.size();    // PRE-FECTHING COUNT BECAUSE onLaoded() CAN SPAWN ACTORS FOR WHICH onLoaded MUSTN'T BE CALLED

        for (u32 itActor = 0; itActor < actorsCount; itActor++)
        {
            Actor * pActor = static_cast<Actor*>(actors[itActor]);
            pActor->onLoaded();
        }
        

        PickableList& friezeList = m_FriseList;
        u32 friezecount = friezeList.size();
        for (u32 i=0; i< friezecount; i++)
        {
            Frise* pFrz = static_cast<Frise*>(friezeList[i]);
            pFrz->onLoaded();
            pFrz->postCheckPoint(); // omit to call this one to get placement bugs with subgroups & tweenings
        }
        

        flushPending();

#ifdef _DEBUG
        checkConsistency();
#endif // _DEBUG

        LoadingSequence::releaseResources( m_prefetched );
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifdef _DEBUG
    void Scene::checkConsistency() const
    {
        return;
#if 0
        static int iCounter = 0;
        iCounter++;
        if(iCounter < 180)
        {
            return;
        }
        else
        {
            iCounter = 0;
        }

        // Check if lists contains only registered objref
        // And if objref are valid
        {
            const ObjectRefList* registeredLists[] =
            {
                &m_FriseList,
                &m_ActorsList
            };
            const u32 uCount = sizeof(registeredLists) / sizeof(registeredLists[0]);
            u32 objRefCount = 0;

            for(u32 i = 0; i < uCount; ++i)
            {
                const ObjectRefList& list = *(registeredLists[i]);
                const u32 listSize = list.size();

                objRefCount += listSize;

                for(u32 j = 0; j < listSize; j++)
                {
                    const ObjectRef objRef = list[j];
                    const BaseObject* pObj = GETOBJECT(objRef);

                    ITF_ASSERT(pObj);
                    ITF_ASSERT(isPendingRegisterOrRegistered(objRef));
                }
            }

            ITF_ASSERT(m_PickableList.size() == objRefCount);
        }

        // Check if pending unregistration are using registered objref
        {
            const u32 pendingUnregCount = m_Pending_Unregister.size();
            ITF_ASSERT(pendingUnregCount <= m_PickableList.size());
            for(u32 i = 0; i < pendingUnregCount; ++i)
            {
                ITF_ASSERT(isPendingRegisterOrRegistered(m_Pending_Unregister[i].m_Ref));
            }
        }
#endif // 0
    }
#endif // _DEBUG


    void Scene::offsetPosition(const Vec3d& _deltaPivot, bbool _applyToInitialPosition, bbool _updateFriezes)
    {
        // Process scene's elements to place them relatively to the new origin
        for(u32 i = 0; i < m_PickableList.size(); ++i)
        {
            Pickable* pPickable = m_PickableList[i];
            ITF_ASSERT(pPickable);
            if(!pPickable)
                continue;

            if (!_updateFriezes && pPickable->getObjectType() == BaseObject::eFrise)
                continue;

            pPickable->offsetPosition(_deltaPivot, _applyToInitialPosition);
        }
    }
    

    void Scene::rotateContent(const Vec3d& _rotationOrigin, f32 _currentAngleRad, f32 _deltaRad)
    {
        const PickableList& list = getPickableList();
        const u32 listSize = list.size();
        for(u32 i = 0; i < listSize; ++i)
            list[i]->rotate(_deltaRad, bfalse, &_rotationOrigin);
    }

    void Scene::storeCurrentToInitial()
    {
        for(u32 i = 0; i < getPickableList().size(); ++i)
        {
            Pickable* pPickable = getPickableList()[i];

            if(!pPickable)
                continue;

            pPickable->storeCurrentToInitial();
        }
    }

    void Scene::replaceReinitDataByCurrent()
    {
        for(u32 i = 0; i < getActors().size(); ++i)
        {
            Actor* pActor = static_cast<Actor*>(getActors()[i]);

            if(!pActor)
                continue;

            pActor->replaceReinitDataByCurrent();
        }
    }

    void Scene::onLoadReposition()
    {
		BaseObject *buffer[128];
		SafeArray<BaseObject*> resolvedRefs(128,MemoryId::mId_Temporary,buffer);
        ID_SERVER->getObjectListNoNull(m_SubSceneActors, resolvedRefs);
        for(u32 i = 0; i < resolvedRefs.size(); ++i)
        {
            SubSceneActor* pSSA = static_cast<SubSceneActor*>(resolvedRefs[i]);
            pSSA->onLoadReposition();
        }
    }

    bbool Scene::isEmpty() const
    {
        return m_PickableList.size() == 0;
    }

    void Scene::setWorldID( ObjectRef _ID )
    {
        m_worldID = _ID;

        /*
        SafeArray<BaseObject*> resolvedRefs;
        ID_SERVER->getObjectListNoNull(m_SubSceneActors, resolvedRefs);
        for(u32 i = 0; i < resolvedRefs.size(); ++i)
        {
            SubSceneActor* pSSA = static_cast<SubSceneActor*>(resolvedRefs[i]);

            Scene* pScene = pSSA->getSubScene();
            if(pScene)
                pScene->setWorldID(_ID);
        }
        */
    }
    
    World* Scene::getWorld()const
    {
        return static_cast<World*>(m_worldID.getObject());
    }

    void Scene::loadSubScenes()
    {
        // Call for subscene loading
        for(u32 i = 0; i < m_SubSceneActors.size(); ++i)
        {
            SubSceneActor* pSSA = static_cast<SubSceneActor*>(m_SubSceneActors[i].getObject());

            if(!pSSA->getEmbedScene())
                pSSA->loadExternalScene();
            else
            {
                Scene* pSubScene = pSSA->getSubScene();

                if ( pSubScene != NULL )
                    pSubScene->loadSubScenes();
            }
        }
    }

    void Scene::disconnectFromCells() const 
    {
        u32 uSize;

        uSize = m_FriseList.size();
        for(u32 i = 0; i < uSize; ++i)
        {
            Frise* pObj = static_cast<Frise*>(m_FriseList[i]);
            ITF_ASSERT(pObj);
            pObj->setAllowedCell(bfalse);
        }

        uSize = m_ActorsList.size();
        for(u32 i = 0; i < uSize; ++i)
        {
            Actor*  pObj = static_cast<Actor*>(m_ActorsList[i]);
            ITF_ASSERT_MSG(pObj, "The actor list contains a NULL pointer");
            pObj->setAllowedCell(bfalse);
        }
    }

    void Scene::setIsAlwaysActive( bbool _val)
    {
        if (_val)
        {
            setActive(btrue);
        }

        m_isAlwaysActive = _val;
    }


    Actor* Scene::getActorFromUserFriendly(const String8& _friendly)const 
    {
        for (u32 i = 0; i < m_ActorsList.size(); ++i)
        {
            Actor* pActor = static_cast<Actor*>(m_ActorsList[i]);
            ITF_ASSERT_MSG(pActor, "A deleted actor is still in the list");
            if (pActor && !pActor->getUserFriendly().isEmpty() && pActor->getUserFriendly().equals(_friendly, true))
                return pActor;
        }
        return NULL;
    }

    Pickable* Scene::getPickableFromUserFriendly(const String8& _friendly)const 
    {
        for (u32 i = 0; i < m_PickableList.size(); ++i)
        {
            Pickable* pickable = m_PickableList[i];
            ITF_ASSERT_MSG(pickable, "A deleted pickable is still in the list");
            if (pickable && !pickable->getUserFriendly().isEmpty() && pickable->getUserFriendly().equals(_friendly, true))
                return pickable;
        }
        return NULL;
    }

    Scene* Scene::clone()
    {
        const char* label = "Clone scene";
        ArchiveMemory sceneContent;
        Scene* pScene = this;
        {
            CSerializerObjectBinary serializeBin(&sceneContent);
            serializeBin.SerializeObject(label, pScene, &BASEOBJECT_FACTORY->m_objectFactory, ESerialize_Data_Save);
        }
        sceneContent.rewindForReading();
        pScene = NULL;
        {
            CSerializerObjectBinary serializeBin(&sceneContent);
            serializeBin.SerializeObject(label, pScene, &BASEOBJECT_FACTORY->m_objectFactory, ESerialize_Data_Load);
            if(pScene)
                pScene->setPath(getPath());
        }

        return pScene;
    }

    void Scene::removeUnserializableFromLists()
    {
        // Remove non serialisable objects from objList before serialisation

        ITF_ASSERT_MSG(m_FrisesNotSerialized.size() == 0, "The list must be empty!");
        ITF_ASSERT_MSG(m_ActorsNotSerialized.size() == 0, "The list must be empty!");

        i32 iCount = 0;
        
        // Proceed FriseList
        iCount = i32(m_FriseList.size());
        for(i32 i = 0; i < iCount; ++i)
        {
            Frise* pFrise = m_FriseList[i]->DynamicCast<Frise>(ITF_GET_STRINGID_CRC(Frise,2579114664));
            ITF_ASSERT_MSG(pFrise, "There is a null pointer in the container or the pointer is in the wrong container");
            if(pFrise && (!pFrise->isSerializable() || isFilteringObject(pFrise->getUserFriendly(), getBinaryTarget())))
            {
                m_FrisesNotSerialized.push_back(pFrise);
                m_FriseList.eraseNoOrder(i);

                --iCount;
                --i;
            }
        }

        bbool isCooking = ((m_serializationFlags & Scene::flag_isCooking) == Scene::flag_isCooking);

        // Proceed ActorList
        iCount = i32(m_ActorsList.size());
        for(i32 i = 0; i < iCount; ++i)
        {
            Actor* pActor = m_ActorsList[i]->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));

            bbool removeActor = bfalse;
            //check components 
            if (isCooking)
            {
                //We remove any actor that have a todo  Component
                if (pActor)
                {
                    const Actor::ComponentsVector& components = pActor->GetAllComponents();
                    for ( u32 i = 0; i < components.size(); ++i )
                    {
                        const ActorComponent* pComponent = components[i];
                        //if we start to remove more than 2 components,start an array
                        //TODO Component ||  Ray_SceneSettingsComponent

                        if (pComponent->DynamicCast<ActorComponent>(ITF_GET_STRINGID_CRC(TODOComponent,269527281))  ||
                            pComponent->DynamicCast<ActorComponent>(ITF_GET_STRINGID_CRC(Ray_SceneSettingsComponent,129898962)))
                        {
                            removeActor = btrue;
                            break;
                        }
                    }
                }
            }

            ITF_ASSERT_MSG(pActor, "There is a null pointer in the container or the pointer is in the wrong container");
            if(removeActor || (pActor && (!pActor->isSerializable() || (pActor->getLua().isEmpty() || isFilteringObject(pActor->getUserFriendly(), getBinaryTarget())))))
            {
                m_ActorsNotSerialized.push_back(pActor);
                m_ActorsList.eraseNoOrder(i);

                --iCount;
                --i;
            }
        }

        // Remove unused friendlies from target filter (prevent removed pickable to stay forever)
        for(u32 i = 0; i < m_PickableList.size(); ++i)   
        {
            ITF_VECTOR<TargetFilterList>::iterator itTarget = m_filterByPlatform.begin();
            while(itTarget != m_filterByPlatform.end())
            {
                ITF_VECTOR<String8>::iterator itObj = itTarget->m_objList.begin();
                while(itObj != itTarget->m_objList.end())
                {
                    if(getPickableFromUserFriendly(*itObj) == NULL)
                        itObj = itTarget->m_objList.erase(itObj);
                    else
                        ++itObj;
                }

                if(itTarget->m_objList.size() == 0)
                    itTarget = m_filterByPlatform.erase(itTarget);
                else
                    ++itTarget;
            }
        }
    }

    void Scene::restoreUnserializableFromLists()
    {
        // Add non serialisable objects to objList after serialisation
        {
            const u32 uCount = m_FrisesNotSerialized.size();
            for(u32 i = 0; i < uCount; ++i)
            {
                ITF_ASSERT_MSG(m_FriseList.find(m_FrisesNotSerialized[i]) < 0, "Already in the list!");
                m_FriseList.push_back(m_FrisesNotSerialized[i]);
            }
            m_FrisesNotSerialized.clear();
        }

        {
            const u32 uCount = m_ActorsNotSerialized.size();
            for(u32 i = 0; i < uCount; ++i)
            {
                ITF_ASSERT_MSG(m_ActorsList.find(m_ActorsNotSerialized[i]) < 0, "Already in the list!");
                m_ActorsList.push_back(m_ActorsNotSerialized[i]);
            }
            m_ActorsNotSerialized.clear();
        }
    }

    void Scene::fixSerializableContent()
    {
#if !defined(ITF_FINAL)
        // Remove NULL pointers and invalid friezes
        for (u32 it = 0; it < m_FriseList.size(); )
        {
            Frise* pFrise = m_FriseList[it]->DynamicCast<Frise>(ITF_GET_STRINGID_CRC(Frise,2579114664));
            bbool isValid = btrue;

            if( !pFrise )
                isValid = bfalse;
            else
            {
                u32 posCount = pFrise->getPosCount();

                if ( posCount < 2 || ( pFrise->isLooping() && posCount < 3 ) )
                    isValid = bfalse;
            }
                                
            if ( !isValid )
            {
                ITF_ASSERT_MSG(0, "Invalid frise in m_FriseList");
                m_FriseList.eraseNoOrder(it);
                continue;
            }

            ++it;
        }

        // Remove NULL pointer and actors with invalid path
        for (u32 it = 0; it < m_ActorsList.size(); )
        {
            Actor* pActor = m_ActorsList[it]->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));
            if(!pActor || (pActor->getLua().getString().isEmpty() ))
            {
                ITF_ASSERT_MSG(0, "Invalid actor in m_ActorsList");
                m_ActorsList.eraseNoOrder(it);
                continue;
            }
            ++it;
        }
#endif // !ITF_FINAL
    }


    void Scene::forcedActorsSaveCheckpoint()
    {
        const PickableList&  actors = getActors(); 
        for (u32 itActor = 0; itActor < actors.size(); itActor++)
        {
            Actor * pActor = static_cast<Actor*>(actors[itActor]);
            pActor->saveCheckpointData();
        }
    }
    void Scene::destroySpawnees(bbool _recursive)
    {
        const PickableList&  actors = getActors();

        //remove spawned actors (created with Spawner class)
        for (u32 itActor = 0; itActor < actors.size(); itActor++)
        {
            Actor * pActor = static_cast<Actor*>(actors[itActor]);
            if (pActor->isSpawned() && !pActor->isDestructionRequested())
            {
                pActor->requestDestruction();
            }
        }

        //optionally, recurse
        if (_recursive)
        {
            const ObjectRefList &subscenes = getSubSceneActors();
            for (u32 i=0; i<subscenes.size(); i++)
            {
                ObjectRef objRef = subscenes[i];
                BaseObject *obj = objRef.getObject();
                if (obj)
                {
                    SubSceneActor *subsceneActor = obj->DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817));
                    if (subsceneActor)
                    {
                        Scene *scene = subsceneActor->getSubScene();
                        if (scene)
                        {
                            scene->destroySpawnees(_recursive);
                        }
                    }
                }
            }
        }
    }

    void Scene::deleteSpawnedActors()
    {
        const PickableList&  actors = getActors();

        //remove spawned actors (created with Spawner class)
        for (u32 itActor = 0; itActor < actors.size(); itActor++)
        {
            Actor * pActor = static_cast<Actor*>(actors[itActor]);
            pActor->deleteSpawnedActors();
        }
    }


    void    Scene::centerOnWorldOrigin()
    {
        // Recursively build the list of all actors
        SafeArray<ActorRef> AllActors;
        SafeArray<ObjectRef> AllFriezes;
        {
            const PickableList&  actors = getActors();
            for (u32 itActor = 0; itActor < actors.size(); itActor++)
            {
                Actor * pActor = static_cast<Actor*>(actors[itActor]);
             //   if(SubSceneActor* pSSA = pActor->DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817)))
                    AllActors.push_back(pActor->getRef());
          /*      if (pActor->isSerializable() && !pActor->isSpawned())
                {
                    pActor->registerContainedPickables(AllActors,AllFriezes);
                }*/
            }
            const PickableList& friezeList = getFrises();
            for (u32 i=0; i<friezeList.size(); i++)
            {
                const Frise* pFrz = (const Frise*)friezeList[i];
                AllFriezes.push_back(pFrz->getRef());
            } 
        }


        AABB aabb;
        bbool binit = bfalse;
        for (u32 itActor = 0; itActor < AllActors.size(); itActor++)
        {
            Actor * pActor = AllActors[itActor].getActor();
            if (pActor)
            {
                if (!binit)
                {
                    aabb = pActor->getAABB();
                    binit = true;
                }
                else
                    aabb.grow(pActor->getAABB());
            }
        }

        for (u32 it = 0; it < AllFriezes.size(); it++)
        {
            Frise * frz = (Frise *)AllFriezes[it].getObject();
            if (frz)
            {
                if (!binit)
                {
                    aabb = frz->getAABB();
                    binit = true;
                }
                else
                    aabb.grow(frz->getAABB());
            }
        }        

        Vec3d pos3d = -aabb.getCenter().to3d();

        for (u32 itActor = 0; itActor < AllActors.size(); itActor++)
        {
            Actor * pActor = AllActors[itActor].getActor();
            if (pActor)
            {
                pActor->setWorldInitialPos(pActor->getWorldInitialPos() + pos3d);
                pActor->setPos(pActor->getWorldInitialPos());
            }
        }

        for (u32 it = 0; it < AllFriezes.size(); it++)
        {
            Frise * frz = (Frise *)AllFriezes[it].getObject();
            if (frz)
            {
                frz->setWorldInitialPos(frz->getWorldInitialPos() + pos3d);
                frz->setPos(frz->getWorldInitialPos());
            }
        }
    }


    void Scene::forcedActorsLoadCheckpoint()
    {
        // Recursively delete all spawned actors
        deleteSpawnedActors();
        flushPending();
 
        // Recursively build the list of all actors
        ActorRef AllActorBuff[1024];
        ObjectRef AllFriezesBuff[1024];
        SafeArray<ActorRef>  AllActors (sizeof(AllActorBuff)/sizeof(ActorRef),    MemoryId::mId_Temporary, AllActorBuff);
        SafeArray<ObjectRef> AllFriezes(sizeof(AllFriezesBuff)/sizeof(ObjectRef), MemoryId::mId_Temporary, AllFriezesBuff);
        {
            const PickableList&  actors = getActors();
            for (u32 itActor = 0; itActor < actors.size(); itActor++)
            {
                Actor * pActor = static_cast<Actor*>(actors[itActor]);
                ITF_ASSERT(!pActor->isSpawned());
                if (pActor->isSerializable() && !pActor->isSpawned())
                {
                    pActor->registerContainedPickables(AllActors,AllFriezes);
                }
            }
            const PickableList& friezeList = getFrises();
            for (u32 i=0; i<friezeList.size(); i++)
            {
                const Frise* pFrz = (const Frise*)friezeList[i];
                AllFriezes.push_back(pFrz->getRef());
            } 
        }

        m_forcedActorLoadCP_Tmp.reserve(64);

        // Hot reload 1st step for all actors
        for (u32 itActor = 0; itActor < AllActors.size(); itActor++)
        {
            Actor * pActor = AllActors[itActor].getActor();
            if (pActor && pActor->isSerializable() && !pActor->isSpawned())
            {
                ActorHotReload data;
                pActor->fillHotReloadDataAndHotReload(data, bfalse, btrue); // don't skip instance data, use 2 step hot reload
                m_forcedActorLoadCP_Tmp.push_back(data);
            }
        }

        // Hot reload 2nd step for all actors
        for (u32 itActor = 0; itActor < m_forcedActorLoadCP_Tmp.size(); itActor++)
        {
            m_forcedActorLoadCP_Tmp[itActor].m_childrenBeforeClear.clear(); // because Actor::Clear() clears bindings. in this case, we need not to restore bindings because the children will do the job. Since all actors are reloaded here, let the children do the job
            if(Actor* pActor = m_forcedActorLoadCP_Tmp[itActor].m_actorRef.getActor())
                pActor->postHotReload(m_forcedActorLoadCP_Tmp[itActor], Pickable::HotReloadType_Checkpoint); 
        }

        // Post load checkpoint for all actors
        for (u32 itActor = 0; itActor < m_forcedActorLoadCP_Tmp.size(); itActor++)
        {
            if(Actor* pActor = m_forcedActorLoadCP_Tmp[itActor].m_actorRef.getActor())
                pActor->postLoadCheckpointData(); 
        }

        // Post load checkpoint for all frises
        for (u32 it = 0; it < AllFriezes.size(); it++)
        {
            Frise * frz = (Frise *)AllFriezes[it].getObject();
            if (frz)
                frz->postCheckPoint();
                //frz->onLoaded(btrue);
        }

        m_forcedActorLoadCP_Tmp.clear();
    }

    void Scene::registerObjectWithID( BaseObject* _object )
    {
        ITF_ASSERT_MSG(_object && _object->getObjectId().isValid(), "Trying to register an invalid object");

    #if !defined(ITF_FINAL)
        ITF_MAP<ObjectId, BaseObject*>::iterator it = m_objectIdMap.find(_object->getObjectId());
        if (it != m_objectIdMap.end())
        {
            ITF_ASSERT(it->second); // contains no object

            if(it->second)
            {
                Pickable* pick = _object->DynamicCast<Pickable>(ITF_GET_STRINGID_CRC(Pickable,594660361));
    #if defined(_DEBUG) && defined(ASSERT_ENABLED)
                String8 objId;
                _object->getObjectId(objId);
                String8 msg("Duplicate objectID: ");
                msg += objId;
                msg += "\n\t";

                if (pick)
                    msg += pick->getUserFriendly();
                else
                    msg += "base object with no friendly";

                msg +="\nconflicts with:\n\t";
                pick = (it->second)->DynamicCast<Pickable>(ITF_GET_STRINGID_CRC(Pickable,594660361));
                if (pick)
                    msg += pick->getUserFriendly();
                else
                    msg += "base object with no friendly";

                ITF_ASSERT_MSG(0, msg.cStr());
    #endif // _DEBUG

                if (pick)
                {
                    ITF_ERROR("ObjectID of '%s' is duplicated!!!\n\nA new ObjectID was assigned to fix the data.\n\nPlease save ASAP and contact an engineer.\n\nPrevious ObjectID : %x", pick->getUserFriendly().cStr(), _object->getObjectId().m_id);
                }
                // Try to fix data by generating a new object ID
                _object->regenerateObjectId();
            }
        }
    #endif // ! ITF_FINAL

        m_objectIdMap[_object->getObjectId()] = _object;
    }

    void Scene::unregisterObjectWithID( const ObjectId& _id )
    {
        ITF_MAP<ObjectId,BaseObject*>::iterator it = m_objectIdMap.find(_id);
        ITF_ASSERT_MSG(hasStaticSerializationFlag( flag_dependency ) || it!=m_objectIdMap.end(), "Trying to unregistering unknown objectID");

        if ( it != m_objectIdMap.end() )
            m_objectIdMap.erase(it);
    }

    BaseObject* Scene::getObject( const ObjectId& _id )const
    {
        ITF_MAP<ObjectId,BaseObject*>::const_iterator it = m_objectIdMap.find(_id);

        if ( it != m_objectIdMap.end() )
        {
            return it->second;
        }

        return NULL;
    }

    void    Scene::checkPickableIsNotInList(const Pickable* _pick, const PickableList& _list)
    {
        for (u32 it = 0; it < _list.size(); it++)
        {
            ITF_ASSERT(_pick != _list[it]);
        }
    }

    void    Scene::checkPickableIsNotInPendingList(const Pickable* _pick, const PendingRequestList& _list)
    {
        for (u32 it = 0; it < _list.size(); it++)
        {
            ITF_ASSERT(_pick != _list[it].first);
        }
    }

    void    Scene::checkPickableCanBeDeleted(const Pickable* _pick,u32 step)
    {
        ITF_ASSERT_MSG(Synchronize::getCurrentThreadId() == ThreadSettings::m_settings[eThreadId_mainThread].m_threadID, "m_Pending_RegisterActor is not thread safe");

        //step 0 test all

       switch (step)
       {

       case 0:
           checkPickableIsNotInList(_pick, m_Pending_RegisterFrise);
           checkPickableIsNotInList(_pick, m_Pending_RegisterActor);
           checkPickableIsNotInList(_pick, m_Pending_RegisterActorAlwaysActive);

           checkPickableIsNotInPendingList(_pick, m_Pending_UnregisterFrise);
           checkPickableIsNotInPendingList(_pick, m_Pending_UnregisterActor);
           checkPickableIsNotInPendingList(_pick, m_Pending_UnregisterActorAlwaysActive);
           checkPickableIsNotInPendingList(_pick, m_Pending_UnregisterActorAvailableForSpawn);
           
       case 1:

        checkPickableIsNotInList(_pick, getFrises());
        checkPickableIsNotInList(_pick, getActors());

        checkPickableIsNotInList(_pick, m_FrisesNotSerialized);
        checkPickableIsNotInList(_pick, m_ActorsNotSerialized);


       case 2:
        checkPickableIsNotInList(_pick, getPickableList());
        break;

       }
    }

    void Scene::clearObjectIDMap()
    {
    #if defined(_DEBUG) && !defined(ITF_CONSOLE)
        LOG("[Scene] Closing ObjectIdServer");
        ITF_MAP<ObjectId,BaseObject*>::iterator it = m_objectIdMap.begin();
        ITF_MAP<ObjectId,BaseObject*>::iterator end = m_objectIdMap.end();
        char buf[256];
        while(it != end)
        {
            ObjectId id = it->first;
            id.toString(buf);
            LOG("Still registered objectID: %s", buf);

            ITF_ASSERT_MSG(0, "ObjectIdServer is not empty");

            ++it;
        }
        LOG("[Scene] ObjectIdServer closed");
    #endif // _DEBUG

        m_objectIdMap.clear();
    }


    void Scene::filterObject( const String8& _friendly, const String& _platform )
    {
        ITF_VECTOR<TargetFilterList>::iterator targetIt = std::find(m_filterByPlatform.begin(), m_filterByPlatform.end(), _platform);

        if(targetIt != m_filterByPlatform.end())
        {
            TargetFilterList& targetList = *targetIt;

            ITF_VECTOR<String8>::iterator idIt = std::find(targetList.m_objList.begin(), targetList.m_objList.end(), _friendly);
            if(idIt == targetList.m_objList.end())
                targetList.m_objList.push_back(_friendly);
        }
        else
        {
            TargetFilterList targetList;
            targetList.m_platform = _platform;
            targetList.m_objList.push_back(_friendly);

            m_filterByPlatform.push_back(targetList);
        }
    }

    void Scene::unfilterObject( const String8& _friendly, const String& _platform )
    {
        ITF_VECTOR<TargetFilterList>::iterator targetIt = std::find(m_filterByPlatform.begin(), m_filterByPlatform.end(), _platform);

        if(targetIt != m_filterByPlatform.end())
        {
            TargetFilterList& targetList = *targetIt;

            ITF_VECTOR<String8>::iterator idIt = std::find(targetList.m_objList.begin(), targetList.m_objList.end(), _friendly);
            if(idIt != targetList.m_objList.end())
                targetList.m_objList.erase(idIt);

            if(targetList.m_objList.size() == 0)
                m_filterByPlatform.erase(targetIt);
        }
    }

    bbool Scene::isFilteringObject( const String8& _friendly, const String& _platform ) const
    {
        bbool result = bfalse;
        ITF_VECTOR<TargetFilterList>::const_iterator targetIt = std::find(m_filterByPlatform.begin(), m_filterByPlatform.end(), _platform);

        if(targetIt != m_filterByPlatform.end())
        {
            const TargetFilterList& targetList = *targetIt;

            result = (std::find(targetList.m_objList.begin(), targetList.m_objList.end(), _friendly) != targetList.m_objList.end());
        }

        return result;
    }
    
    u32 Scene::getFilteredPlatformsForObject( const String8& _friendly, ITF_VECTOR<String>& _results ) const
    {
        u32 foundCount = 0;
        ITF_VECTOR<TargetFilterList>::const_iterator targetIt = m_filterByPlatform.begin();

        while(targetIt != m_filterByPlatform.end())
        {
            if(isFilteringObject(_friendly, targetIt->m_platform))
            {
                ++foundCount;
                _results.push_back(targetIt->m_platform);
            }

            ++targetIt;
        }

        return foundCount;
    }

    bbool Scene::reorderSerializedLists()
    {
        bbool result = bfalse;

        // The goal of this code is to put parent actors before their children in the serialized list
        // So parents's onLoaded are called before at loading time
        for(u32 i = 0; i < m_ActorsList.size(); ++i)
        {
            Actor* pActor = static_cast<Actor*>(m_ActorsList[i]);

            // Search parent hierarchy in the same scene
            SafeArray<Actor*> parentHierarchy;

            Actor* currentParent = pActor;
            while(currentParent)
            {
                if(ActorBind* pParentBind = currentParent->getParentBind())
                {
                    Actor* pBindParent = static_cast<Actor*>(pParentBind->m_runtimeParent.getObject());
                    if(!pBindParent)
                    {
                        if(pParentBind->m_parent.getIsAbsolute())
                            pBindParent = static_cast<Actor*>(SceneObjectPathUtils::getObjectFromAbsolutePath(pParentBind->m_parent));
                        else
                            pBindParent = static_cast<Actor*>(SceneObjectPathUtils::getObjectFromRelativePath(pActor, pParentBind->m_parent));
                    }

                    // Find the common scene
                    if(pBindParent)
                    {
                        Actor* pCurrParent = pBindParent;
                        Scene* pCurrScene = pBindParent->getScene();
                        while(pCurrScene)
                        {
                            if(pCurrScene == this)
                            {
                                parentHierarchy.push_back(pCurrParent);
                                break;
                            }

                            SubSceneActor* pSSA = pCurrScene->getSubSceneActorHolder();
                            pCurrScene = pSSA ? pSSA->getScene() : NULL;
                            pCurrParent = pSSA;
                        }
                    }

                    currentParent = pBindParent;
                }
                else
                    currentParent = NULL;
            }

            i32 indexRef = i;
            u32 skipCount = 0;
            for(u32 iP = 0; iP < parentHierarchy.size(); ++iP)
            {
                Actor* pParent = parentHierarchy[iP];
                const i32 index = m_ActorsList.find(pParent);
                ITF_ASSERT(index != -1);

                if(index > indexRef)
                {
                    m_ActorsList.eraseKeepOrder(index);
                    m_ActorsList.insert(pParent, indexRef);
                    skipCount++;
                    result = btrue;
                }
                else
                {
                    indexRef = index;
                }
            }
            i += skipCount + 1;
        }

        // Reorder inside groups
        for(u32 i = 0; i < m_SubSceneActors.size(); ++i)
            if(SubSceneActor* pSSA = static_cast<SubSceneActor*>(m_SubSceneActors[i].getObject()))
                if(pSSA->getEmbedScene())
                    if(Scene* pSubScene = pSSA->getSubScene())
                        result |= pSubScene->reorderSerializedLists();

        return result;
    }

    void Scene::getAll2DObjects(ObjectRefList& _list2D)
    {
        const PickableList& lst2d = get2DActors();
        for (u32 i = 0; i < lst2d.size(); i++)
            _list2D.push_back(lst2d[i]->getRef());

        const PickableList& lstAct = getActors();
        for (u32 i = 0; i < lstAct.size(); i++)
        {
            Actor* pAct = (Actor*)lstAct[i];
            if(SubSceneActor* pSSA = ((Actor*)pAct)->DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817)))
            {
                if(Scene* pSubScene = pSSA->getSubScene())
                    pSubScene->getAll2DObjects(_list2D);
            }
        }
    } 

    void Scene::draw2D()
    {
        if (!isActive())
        {
            m_2DActorsToDraw.clear();
            return;
        }

        u32 count = m_2DActorsToDraw.size();
        for (u32 i = 0; i < count; i++)
        {
            Pickable* pObj = static_cast<Pickable*>(m_2DActorsToDraw[i]);
            pObj->draw2D();
        }
        m_2DActorsToDraw.clear();
    }

    void Scene::transferPickable( Pickable* _pObj, const Scene& _destination )
    {
#if !defined(ITF_CONSOLE_FINAL)        
        for(u32 i = 0; i < m_Pending_Tranfert.size(); ++i)
        {
            ITF_ASSERT_CRASH(m_Pending_Tranfert[i].first != _pObj, "Can't call transferPickable twice for the same object");
        }

        switch(_pObj->getObjectType())
        {
        case BaseObject::eActor:
            for(u32 i = 0; i < m_Pending_UnregisterActor.size(); ++i)
            {
                ITF_ASSERT_CRASH(m_Pending_UnregisterActor[i].first != _pObj, "Can't call transferPickable when the object is waiting for delete");
            }
        break;

        case BaseObject::eFrise:
            for(u32 i = 0; i < m_Pending_UnregisterFrise.size(); ++i)
            {
                ITF_ASSERT_CRASH(m_Pending_UnregisterFrise[i].first != _pObj, "Can't call transferPickable when the object is waiting for delete");
            }
        break;

        default:
        break;
        }
#endif // ITF_CONSOLE_FINAL

        m_Pending_Tranfert.push_back(std::pair<Pickable*, ObjectRef>(_pObj, _destination.getRef()));
    }

    bool Scene::findScene( StringID _sceneName, Scene *& _scene, Vec3d & pos )
    {
        if (m_szPath.getStringID() == _sceneName)
        {
            _scene      = this;
            pos         = Vec3d::Zero;
            return btrue;
        }
        
        for(u32 i = 0; i < getActors().size(); ++i)
        {
            Actor* pActor = static_cast<Actor*>(getActors()[i]);

            if(!pActor)
                continue;

            if(pActor->getCellEnableData().m_isSubSceneActor)
            {
                SubSceneActor* pSSA = static_cast<SubSceneActor*>(pActor);
                if(Scene* pSubScene = pSSA->getSubScene())
                {
                    if (pSubScene->findScene( _sceneName, _scene, pos ))
                    {
                        pos += pActor->getLocalPos();
                        return btrue;
                    }
                }
            }
        }
        return bfalse;
    }


} // namespace ITF
