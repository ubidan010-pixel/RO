

#include "precompiled_gameplay.h"

#ifndef _ITF_WINDCOMPONENT_H_
#include "gameplay/components/misc/WindComponent.h"
#endif //_ITF_WINDCOMPONENT_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(WindComponent)

BEGIN_SERIALIZATION_CHILD(WindComponent)

END_SERIALIZATION()

WindComponent::WindComponent()
: m_registered(bfalse)
{
}

WindComponent::~WindComponent()
{
    clear();
}

void WindComponent::clear()
{
    if ( m_registered )
    {
        u32 numMods = m_modifiers.size();

        for ( u32 i = 0; i < numMods; i++ )
        {
            PHYSWORLD->removeWindForce(m_actor->getRef(),&m_modifiers[i]);
        }
        
        m_registered = bfalse;
    }

    m_modifiers.clear();
}

void WindComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    m_modifiers.resize(getTemplate()->getForceModifiers().size());

    for ( u32 i = 0; i < getTemplate()->getForceModifiers().size(); i++ )
    {
        m_modifiers[i].setScale(m_actor->getScale());
        m_modifiers[i].setTemplate(&getTemplate()->getForceModifiers()[i]);
    }

    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventScaleChanged,3630354374),this);
}

void WindComponent::onBecomeActive()
{
    Super::onBecomeActive();

    if ( m_modifiers.size() && !m_registered )
    {
        u32 numMods = m_modifiers.size();

        for ( u32 i = 0; i < numMods; i++ )
        {
            PHYSWORLD->addWindForce(m_actor->getRef(),&m_modifiers[i],m_actor->getDepth());
        }

        m_registered = btrue;
    }
}

void WindComponent::onBecomeInactive()
{
    Super::onBecomeInactive();

    if ( m_registered )
    {
        u32 numMods = m_modifiers.size();

        for ( u32 i = 0; i < numMods; i++ )
        {
            PHYSWORLD->removeWindForce(m_actor->getRef(),&m_modifiers[i]);
        }

        m_registered = bfalse;
    }
}

void WindComponent::onEvent( Event* _event )
{
    if(EventScaleChanged * teleportEvent = _event->DynamicCast<EventScaleChanged>(ITF_GET_STRINGID_CRC(EventScaleChanged,3630354374)))
    {
        u32 numMods = m_modifiers.size();

        for ( u32 i = 0; i < numMods; i++ )
        {
            m_modifiers[i].setScale(m_actor->getScale());
        }
    }
}

#ifdef ITF_SUPPORT_EDITOR
void WindComponent::drawEdit( class ActorDrawEditInterface* drawInterface, u32 _flags ) const
{
    if (_flags != ActorComponent::DrawEditFlag_All)
        return;

    Vec2d pos = m_actor->get2DPos();
    f32 angle = m_actor->getAngle();

    for ( u32 i = 0; i < m_modifiers.size(); i++ )
    {
        m_modifiers[i].drawDebug(pos, angle, m_actor->getDepth());
    }
}
#endif // ITF_SUPPORT_EDITOR

//-------------------------------------------------------------------------------------

IMPLEMENT_OBJECT_RTTI(WindComponent_Template)
BEGIN_SERIALIZATION(WindComponent_Template)

    SERIALIZE_CONTAINER_OBJECT("windAreas",m_modifiers);

END_SERIALIZATION()

}