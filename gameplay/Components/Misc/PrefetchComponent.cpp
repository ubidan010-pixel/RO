#include "precompiled_gameplay.h"

#ifndef _ITF_PREFETCH_COMPONENT_H_
#include "gameplay/components/misc/PrefetchComponent.h"
#endif //_ITF_PREFETCH_COMPONENT_H_

#ifndef _ITF_EVENTS_H_
#include "engine/events/Events.h"
#endif //_ITF_EVENTS_H_

#ifndef _ITF_WORLD_MANAGER_H_
#include "engine/scene/worldManager.h"
#endif //_ITF_WORLD_MANAGER_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(PrefetchComponent)

BEGIN_SERIALIZATION_CHILD(PrefetchComponent)
END_SERIALIZATION()
    
BEGIN_VALIDATE_COMPONENT(PrefetchComponent)
    LinkComponent* linkComponent = m_actor->GetComponent<LinkComponent>();
    VALIDATE_COMPONENT_PARAM("LinkComponent",linkComponent, "Prefetch component needs link component");
END_VALIDATE_COMPONENT()

PrefetchComponent::PrefetchComponent()
: Super()
{
}


PrefetchComponent::~PrefetchComponent()
{
}

void PrefetchComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTrigger,1343042510),this);
}

void PrefetchComponent::onEvent( Event * _event )
{
    Super::onEvent(_event);

    if (EventTrigger * evt = _event->DynamicCast<EventTrigger>(ITF_GET_STRINGID_CRC(EventTrigger,1343042510)))
    {
        if (evt->getActivated())
            start();
        else
            stop();
    }
}

void PrefetchComponent::Update( f32 _deltaTime )
{
    if ( m_prefetchs.size() == 0 )
    {
        return;
    }

    PrefetchList::iterator it = m_prefetchs.begin();

    while ( it != m_prefetchs.end() )
    {
        WorldPrefetch& prefetchWorld = *it;

        World* pWorld = static_cast<World*>(prefetchWorld.m_ref.getObject());
        ITF_ASSERT(pWorld);

        if ( !pWorld )
        {
            it = m_prefetchs.erase(it);
            continue;
        }
        
        if( !prefetchWorld.m_prefetchComplete && !pWorld->isSceneAsyncLoadRunning())
        {
            // Start to prefetch some ressources once
            if(!prefetchWorld.m_prefetchCalled)
            {
                LOG_OLIV("[PrefetchComponent] Async load over and pre prefetch ressources");
                pWorld->setPrefetchTargets(prefetchWorld.m_objects);
                pWorld->prefetchResource();
                prefetchWorld.m_prefetchCalled = btrue;
                LOG_OLIV("[PrefetchComponent] Post prefetch ressources");
            }

            // Prefetch was called, check for ready status
            if(pWorld->isPhysicalReady())
            {
                LOG_OLIV("[PrefetchComponent] Prefetch complete");
                prefetchWorld.m_prefetchComplete = btrue;
            }
        }
        
        if ( prefetchWorld.m_requestPrefetchStop && !pWorld->isSceneAsyncLoadRunning() )
        {
            LOG_OLIV("[PrefetchComponent] Pre stop");
            pWorld->stopPrefetch();
            ITF_VERIFY(WORLD_MANAGER->deleteWorld(pWorld));
            LOG_OLIV("[PrefetchComponent] Post stop");
            it = m_prefetchs.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

bbool PrefetchComponent::prefetchComplete() const
{
    for ( PrefetchList::const_iterator it = m_prefetchs.begin(); it != m_prefetchs.end(); ++it )
    {
        if ( !it->m_prefetchComplete )
        {
            return bfalse;
        }
    }

    return btrue;
}

void PrefetchComponent::start()
{
    LinkComponent* linkComponent = m_actor->GetComponent<LinkComponent>();

    ITF_ASSERT_MSG(linkComponent != NULL, "prefetch component needs link component");

    if ( !linkComponent )
    {
        return;
    }

    LOG_OLIV("[PrefetchComponent] Pre start");

    const LinkComponent::ChildrenList& childrenList = linkComponent->getChildren();
    u32 numChildren = childrenList.size();
    String8 mapName;

    for ( u32 i = 0; i < numChildren; i++ )
    {
        const ObjectPath& path = childrenList[i].getPath();
        bbool isAbsolute = path.getIsAbsolute();

        if ( isAbsolute )
        {
            SceneObjectPathUtils::getWorldFromAbsolutePath(path,mapName);
        }
        else
        {
            m_actor->getScene()->getWorld()->getRootScene()->getPath().getString(mapName);
        }

        bbool found = bfalse;
        WorldPrefetch* currentPrefetch = NULL;

        for ( u32 worldIndex = 0; worldIndex < m_prefetchs.size(); worldIndex++ )
        {
            WorldPrefetch& prefetch = m_prefetchs[worldIndex];

            if ( prefetch.m_worldName == mapName )
            {
                found = btrue;
                currentPrefetch = &prefetch;
                break;
            }
        }

        if ( !found )
        {
            m_prefetchs.push_back(WorldPrefetch());
            currentPrefetch = &m_prefetchs[m_prefetchs.size()-1];
            currentPrefetch->m_worldName = mapName;

            if ( isAbsolute )
            {
                LoadInfo loadInfo;
                loadInfo.m_mapPath          = mapName;
                loadInfo.m_bAsyncLoad       = btrue;
                loadInfo.m_disconnectFromCells = bfalse;

                currentPrefetch->m_ref      = WORLD_MANAGER->createAndLoadWorld(loadInfo);
                ITF_ASSERT_MSG(currentPrefetch->m_ref.isValid(), "WORLD_MANAGER->newWorld failed");
            }
        }

        currentPrefetch->m_requestPrefetchStop = bfalse;

        if ( isAbsolute )
        {
            if ( std::find(currentPrefetch->m_objects.begin(),currentPrefetch->m_objects.end(),path) == currentPrefetch->m_objects.end() )
            {
                currentPrefetch->m_objects.push_back(path);
            }
        }
        else
        {
            Pickable* target = SceneObjectPathUtils::getObjectFromRelativePath(m_actor,path);

            ITF_ASSERT_MSG(target != NULL, "Couldn't find target pickable for prefetch");

            if ( target )
            {
                ObjectPath absolutePath;

                SceneObjectPathUtils::getAbsolutePathFromObject(target,absolutePath);

                if ( absolutePath.isValid() )
                {
                    if ( std::find(currentPrefetch->m_objects.begin(),currentPrefetch->m_objects.end(),absolutePath) == currentPrefetch->m_objects.end() )
                    {
                        currentPrefetch->m_objects.push_back(absolutePath);
                    }
                }
            }
        }
    }

    LOG_OLIV("[PrefetchComponent] Post start");
}

void PrefetchComponent::stop()
{
    for ( u32 worldIndex = 0; worldIndex < m_prefetchs.size(); worldIndex++ )
    {
        WorldPrefetch& prefetch = m_prefetchs[worldIndex];

        prefetch.m_requestPrefetchStop = btrue;
    }

    LOG_OLIV("[PrefetchComponent] Stop requested");
}

bbool PrefetchComponent::getPrefetchTargetWorld( ObjectRef& _result ) const
{
    if(prefetchComplete())
    {
        if(m_prefetchs.size())
        {
            _result = m_prefetchs[0].m_ref;
            return btrue;
        }
    }
    
    return bfalse;
}

bbool PrefetchComponent::getPrefetchTargetPos( Vec3d& _result ) const
{
    if(prefetchComplete())
    {
        if(m_prefetchs.size())
        {
            const WorldPrefetch& infos = m_prefetchs[0];
            if(infos.m_objects.size())
            {
                const ObjectPath& objPath = infos.m_objects[0];
                if(objPath.getIsAbsolute())
                {
                    if(Pickable* target = SceneObjectPathUtils::getObjectFromAbsolutePath(objPath))
                    {
                        _result = target->getWorldInitialPos();
                        return btrue;
                    }
                }
                else
                {
                    ITF_ASSERT(0);
                }
            }
        }
    }
    
    return bfalse;
}

//-------------------------------------------------------------------------------------

IMPLEMENT_OBJECT_RTTI(PrefetchComponent_Template)
BEGIN_SERIALIZATION(PrefetchComponent_Template)
END_SERIALIZATION()

}