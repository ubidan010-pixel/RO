#include "precompiled_engine.h"

#ifndef _ITF_SUBSCENECOMPONENT_H_
#include "engine/actors/components/SubSceneComponent.h"
#endif //_ITF_SUBSCENECOMPONENT_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_XMLALL_H_
#include "engine/XML/XML_All.h"
#endif //_ITF_XMLALL_H_

#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_SCENE_MANAGER_H_
#include "engine/scene/sceneManager.h"
#endif //_ITF_SCENE_MANAGER_H_

#ifndef _ITF_SYSTEMADAPTER_
#include "core/AdaptersInterfaces/SystemAdapter.h"
#endif //_ITF_SYSTEMADAPTER_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_SUBSCENEACTOR_H_
#include "engine/actors/SubSceneActor.h"
#endif //_ITF_SUBSCENEACTOR_H_

#ifndef _ITF_SCENE_MANAGER_H_
#include "engine/scene/sceneManager.h"
#endif //_ITF_SCENE_MANAGER_H_

#ifndef _ITF_BASEOBJECTFACTORY_H_
#include "engine/factory/baseObjectFactory.h"
#endif //_ITF_BASEOBJECTFACTORY_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_SCENE_MANAGER_H_
#include "engine/scene/sceneManager.h"
#endif //_ITF_SCENE_MANAGER_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_

namespace ITF
{

    IMPLEMENT_OBJECT_RTTI(SubSceneComponent)
    BEGIN_SERIALIZATION_CHILD(SubSceneComponent)
    END_SERIALIZATION()

    SubSceneComponent::SubSceneComponent()
    : Super()
    , m_pScene(NULL)
    {
        m_relativeAABB = AABB(Vec2d::Zero);
    }

    SubSceneComponent::~SubSceneComponent()
    {
        clear();
    }

    void SubSceneComponent::clear()
    {
        if(m_pScene)
        {
            ITF_VERIFY(SCENE_MANAGER->unloadScene(m_pScene));
        }
        m_pScene = NULL;
        m_relativeAABB = AABB(Vec2d::Zero);
    }

    void SubSceneComponent::Update( f32 _deltaTime )
    {
        m_actor->changeRelativeAABB(m_relativeAABB);
    }


    bbool SubSceneComponent::isPhysicalReady() const
    {
        if(m_SceneRelativePath.isEmpty())
            return btrue;

        if(m_pScene)
            return m_pScene->isPhysicalReady();

        return bfalse;
    }
    
    void SubSceneComponent::setExternalSubScene( const String& _relativePath, SubSceneActor* _pSSA_Holder )
    {
        ITF_ASSERT(!m_pScene);
        if(m_pScene)
            return;

        if(_relativePath.isEmpty())
            return;

        // Check if the new sub scene is already in the current scene hierarchy
        {
            Scene* curScene = _pSSA_Holder->getScene();
            while(curScene)
            {
                if(curScene->getPath() == _relativePath)
                {
                    ObjectPath actorPath;
                    SceneObjectPathUtils::getAbsolutePathFromObject(_pSSA_Holder, actorPath);

                    ITF_STDSTRING text;
                    actorPath.toString(text);
                    String8 msg("Recursive scene instance inclusion detected, loading is interrupted.\n\nObject path: ");
                    msg += text.c_str();

                    ITF_ERROR(msg.cStr());

                    _pSSA_Holder->setDataError("Recursive scene inclusion");

                    return;
                }

                if(const SubSceneActor* _pSSA_Holder = curScene->getSubSceneActorHolder())
                    curScene = _pSSA_Holder->getScene();
                else
                    curScene = NULL;
            }
        }
        
        LoadInfo info;
        info.m_mapPath = _relativePath;
        info.m_bAsyncLoad = bfalse;
        if(Scene* pNewScene = SCENE_MANAGER->loadSceneSynchronous(info, btrue, bfalse, bfalse))
        {
            LOG_OLIV("[SubSceneComponent] Loaded subscene : %s at : %s", StringToUTF8(pNewScene->getUniqueName()).get(), StringToUTF8(_relativePath).get());

            m_pScene = pNewScene;
            m_pScene->setSubSceneActorHolder(_pSSA_Holder);
            m_SceneRelativePath = _relativePath;

            rebuildAABB();
        }
    }

    void SubSceneComponent::setLoadedSubScene(Scene* _pScene)
    {
        ITF_ASSERT(!m_pScene);
        m_pScene = _pScene;
        m_SceneRelativePath = m_pScene->getPath();
        rebuildAABB();
    }

    void SubSceneComponent::onSceneActive()
    {
        if(m_pScene)
            m_pScene->setActive(btrue);
    }

    void SubSceneComponent::onSceneInactive()
    {
        if(m_pScene)
            m_pScene->setActive(bfalse);
    }

    void SubSceneComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

#if defined(ASSERT_ENABLED)
        SubSceneActor* pSSA = GetActor()->DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817));
        ITF_ASSERT_MSG(pSSA, "Someone is using the subscene component on something different than a subscene actor");
#endif // ASSERT_ENABLED
         m_relativeAABB = AABB(Vec2d::Zero);
    }

    void SubSceneComponent::rebuildAABB()
    {
        if(m_pScene && GetActor())
        {
            AABB locaAABB(GetActor()->getPos());
            const u32 uPickableCount = m_pScene->getPickableList().size();
            for(u32 i = 0; i < uPickableCount; ++i)
            {
                const Pickable* pPickable = m_pScene->getPickableList()[i];
                ITF_ASSERT(pPickable);
                if(!pPickable) continue;
                
                locaAABB.grow(pPickable->getAABB());
            }

            locaAABB.Translate(-m_actor->get2DPos());
            GetActor()->changeRelativeAABB(locaAABB);
            m_relativeAABB = locaAABB;
        }
    }

    void SubSceneComponent::growInternalRelativeAABBFromAbsolute(const AABB &_absoluteAabb, f32 _z)
    {
        AABB relativeAABB(_absoluteAabb);

        const f32 deltaX = CAMERA->getDeltaX();
        const f32 deltaY = CAMERA->getDeltaY();
        relativeAABB.projectTo2D(deltaX, deltaY, _z, m_actor->getDepth());

        relativeAABB.Translate(-m_actor->get2DPos());
        GetActor()->growRelativeAABB(relativeAABB);
        m_relativeAABB.grow(relativeAABB);
    }

    void SubSceneComponent::storeCurrentToInitial()
    {
        if(m_pScene)
            m_pScene->storeCurrentToInitial();
    }

#ifdef ITF_SUPPORT_EDITOR
    void SubSceneComponent::onEditorCreated( Actor* _original/* = NULL*/ )
    {
        Super::onEditorCreated(_original);

        rebuildAABB();
    }

    void SubSceneComponent::onEditorMove(bbool _modifyInitialPos)
    {
        Super::onEditorMove(_modifyInitialPos);

        if(m_pScene)
        {
            const PickableList& pickables = m_pScene->getPickableList();
            for(u32 i = 0; i < pickables.size(); ++i)
                pickables[i]->onEditorMove(_modifyInitialPos);
        }
        rebuildAABB();
    }
#endif // ITF_SUPPORT_EDITOR

    IMPLEMENT_OBJECT_RTTI(SubSceneComponent_Template)
    BEGIN_SERIALIZATION(SubSceneComponent_Template)
    END_SERIALIZATION()

} // namespace ITF
