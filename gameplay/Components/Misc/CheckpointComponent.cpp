#include "precompiled_gameplay.h"

#ifndef _ITF_CHECKPOINTCOMPONENT_H_
#include "gameplay/components/misc/CheckpointComponent.h"
#endif //_ITF_CHECKPOINTCOMPONENT_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_TRIGGERCOMPONENT_H_
#include "gameplay/Components/Trigger/TriggerComponent.h"
#endif //_ITF_TRIGGERCOMPONENT_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

namespace ITF
{
IMPLEMENT_OBJECT_RTTI(CheckpointComponent)

BEGIN_SERIALIZATION_CHILD(CheckpointComponent)

    BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
        SERIALIZE_MEMBER("INDEX", m_index);
        SERIALIZE_MEMBER("joinAlive", m_joinAlive);
    END_CONDITION_BLOCK()

    BEGIN_CONDITION_BLOCK(ESerializeGroup_Checkpoint)
        SERIALIZE_MEMBER("active", m_active);
    END_CONDITION_BLOCK()

END_SERIALIZATION()

CheckpointComponent::CheckpointComponent()
    : Super()
    , m_index(U32_INVALID)
    , m_registered(bfalse)
    , m_active(btrue)
    , m_joinAlive(bfalse)
{
}

CheckpointComponent::~CheckpointComponent()
{
    clear();
}

void CheckpointComponent::clear()
{
    unregisterFromManager();
}

void CheckpointComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventActivateCheckpoint,1094365377),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTrigger,1343042510),this);
    
    registerToManager();
}

void CheckpointComponent::onEvent( Event * _event)
{
    Super::onEvent(_event);

    if ( EventActivateCheckpoint* onActivate = _event->DynamicCast<EventActivateCheckpoint>(ITF_GET_STRINGID_CRC(EventActivateCheckpoint,1094365377)) )
    {
        m_active = onActivate->getActivate();

        if ( m_active )
        {
            TriggerComponent* triggerComp = m_actor->GetComponent<TriggerComponent>();

            if ( triggerComp )
            {
                triggerComp->resetTriggerDone();
            }
        }
    }
    else if (EventTrigger* onTrigger = _event->DynamicCast<EventTrigger>(ITF_GET_STRINGID_CRC(EventTrigger,1343042510)))
    {
        if ( m_active && onTrigger->getActivated() )
        {
            ObjectRef activator = onTrigger->getActivator();
            if (GAMEMANAGER->isPlayerActor(activator))
            {
                checkpointReached(activator);
            }
        }
    }
}

void CheckpointComponent::onSceneActive()
{
    if ( !m_registered ) // is registered right after post loading
        registerToManager();
}

void CheckpointComponent::onSceneInactive()
{
    ITF_ASSERT(m_registered);
    unregisterFromManager();
}

void CheckpointComponent::checkpointReached( ObjectRef _player )
{
    GAMEMANAGER->onReachedCheckpoint(GetActor()->getRef());
}

void CheckpointComponent::registerToManager()
{
    if ( !m_registered )
    {
        if(World* pWorld = GetActor()->getWorld())
        {
            GAMEMANAGER->registerCheckpoint(pWorld->getRef(), GetActor()->getRef());
        }
        m_registered = btrue;
    }
}

void CheckpointComponent::unregisterFromManager()
{
    if ( m_registered )
    {
        if(World* pWorld = GetActor()->getWorld())
        {
            GAMEMANAGER->unregisterCheckpoint(pWorld->getRef(), GetActor()->getRef());
        }
        m_registered = bfalse;
    }
}

Vec2d CheckpointComponent::getPlayerSpawnPos( u32 _playerIndex ) const
{
    LinkComponent* linkComponent = m_actor->GetComponent<LinkComponent>();

    if ( linkComponent )
    {
        Player* player = GAMEMANAGER->getPlayer(_playerIndex);

        if ( player && player->getID().isValid() )
        {
            const LinkComponent::ChildrenList& children = linkComponent->getChildren();
            u32 numChildren = children.size();

            for ( u32 i = 0; i < numChildren; i++ )
            {
                const ChildEntry& child = children[i];

                if ( child.hasTag(player->getID()) )
                {
                    const ObjectPath& path = child.getPath();
                    Pickable* target;
                    
                    if ( path.getIsAbsolute() )
                    {
                        target = SceneObjectPathUtils::getObjectFromAbsolutePath(path);
                    }
                    else
                    {
                        target = SceneObjectPathUtils::getObjectFromRelativePath(m_actor,path);
                    }

                    if ( target )
                    {
                        return target->get2DPos();
                    }

                    break;
                }
            }
        }
    }

    return m_actor->get2DPos();
}

bbool CheckpointComponent::canSpawnPlayer() const
{
    if ( getTemplate() )
        return getTemplate()->getSpawnPlayer();
    return true;
}

#ifdef ITF_SUPPORT_EDITOR
void CheckpointComponent::drawEdit( class ActorDrawEditInterface* /*drawInterface*/, u32 _flags ) const
{
    Vec3d textPos;
    GFX_ADAPTER->compute3DTo2D(m_actor->getPos(), textPos);

    String index;
    index.setTextFormat("%u", m_index);
    GFX_ADAPTER->drawDBGText(index, Color::white(), textPos.m_x, textPos.m_y);
}
#endif // ITF_SUPPORT_EDITOR

//-------------------------------------------------------------------------------------

IMPLEMENT_OBJECT_RTTI(CheckpointComponent_Template)
BEGIN_SERIALIZATION(CheckpointComponent_Template)
    SERIALIZE_MEMBER("spawnPlayer",m_spawnPlayer);
END_SERIALIZATION()

}

