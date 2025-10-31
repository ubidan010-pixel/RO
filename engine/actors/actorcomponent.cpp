#include "precompiled_engine.h"

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _ITF_EVENTS_H_
#include "engine/events/Events.h"
#endif //_ITF_EVENTS_H_

#ifndef ITF_TEMPLATEDATABASE_H
#include "engine/TemplateManager/TemplateDatabase.h"
#endif // ITF_TEMPLATEDATABASE_H

namespace ITF
{
IMPLEMENT_OBJECT_RTTI(ActorComponent)

BEGIN_SERIALIZATION_ABSTRACT(ActorComponent)
END_SERIALIZATION()

ActorComponent::ActorComponent()
: m_actor(NULL)
, m_template(NULL)
{
    m_pause = bfalse;
}

ActorComponent::~ActorComponent()
{
}

World* ActorComponent::getWorld() const
{
    ITF_ASSERT(m_actor);
    return m_actor->getWorld();
}

bbool ActorComponent::isActive() const
{
    return m_actor->isActive() && !isPaused();
}

void ActorComponent::pause()
{
    if ( !m_pause )
    {
        m_pause = btrue;

        if ( m_actor->isActive() )
        {
            onBecomeInactive();
        }
    }
}

void ActorComponent::unpause()
{
    if (m_pause)
    {
        m_pause = bfalse;
        
        if ( m_actor->isActive() )
        {
            onBecomeActive();
        }
    }
}

void ActorComponent::setTemplate( const TemplateActorComponent* _template )
{
    ITF_ASSERT(_template->getComponentClassCRC()==GetObjectClassCRC());
    m_template = _template;
}

}
