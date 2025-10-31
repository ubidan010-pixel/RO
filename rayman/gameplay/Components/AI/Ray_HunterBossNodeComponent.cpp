#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_HUNTERBOSSNODECOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_HunterBossNodeComponent.h"
#endif //_ITF_RAY_HUNTERBOSSNODECOMPONENT_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_WORLD_MANAGER_H_
#include "engine/scene/worldManager.h"
#endif //_ITF_WORLD_MANAGER_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_HunterBossNodeComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_HunterBossNodeComponent)

        BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
            SERIALIZE_OBJECT("data", m_nodeData);
        END_CONDITION_BLOCK()

    END_SERIALIZATION()


    BEGIN_VALIDATE_COMPONENT(Ray_HunterBossNodeComponent)
    END_VALIDATE_COMPONENT()


    Ray_HunterBossNodeComponent::Ray_HunterBossNodeComponent()
        : Super()
   {
    }

    Ray_HunterBossNodeComponent::~Ray_HunterBossNodeComponent()
    {
    }



    void Ray_HunterBossNodeComponent::onActorLoaded( Pickable::HotReloadType _hotReload )
    {
        Super::onActorLoaded(_hotReload);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventQueryHunterNode,210353395),this);

    }
    void Ray_HunterBossNodeComponent::onEvent(Event * _event )
    {
        Super::onEvent(_event);
        if (Ray_EventQueryHunterNode * query = _event->DynamicCast<Ray_EventQueryHunterNode>(ITF_GET_STRINGID_CRC(Ray_EventQueryHunterNode,210353395)))
        {
            *query = m_nodeData;
        }

    }

#ifdef ITF_SUPPORT_EDITOR
    void Ray_HunterBossNodeComponent::drawEdit( class ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        Super::drawEdit( _drawInterface,_flags );
        if (_flags != ActorComponent::DrawEditFlag_All)
            return;

        DebugDraw::circle(m_actor->getPos(),m_nodeData.m_finishRadius);
    }
#endif // ITF_SUPPORT_EDITOR

    //----------------------------------------------------------------------------------------------------------------------------------------

    IMPLEMENT_OBJECT_RTTI(Ray_HunterBossNodeComponent_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_HunterBossNodeComponent_Template)
    END_SERIALIZATION()



};