#include "precompiled_engine.h"

#ifndef _ITF_KEEPALIVECOMPONENT_H_
#include "engine/actors/components/KeepAlivecomponent.h"
#endif //_ITF_KEEPALIVECOMPONENT_H_

#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_EVENTS_H_
#include "engine/events/Events.h"
#endif //_ITF_EVENTS_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(KeepAliveComponent)
BEGIN_SERIALIZATION_CHILD(KeepAliveComponent)
END_SERIALIZATION()

KeepAliveComponent::KeepAliveComponent()
: Super()
, m_updateDone(bfalse)
, m_tryDying(bfalse)
, m_disableOnly(bfalse)
{ 
}

KeepAliveComponent::~KeepAliveComponent()
{
     clear();
}

void KeepAliveComponent::clear()
{
    if (m_tryDying)
        m_actor->requestDestruction();
}

void KeepAliveComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    m_tryDying      = bfalse;
    m_updateDone    = bfalse;

    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventDie,1997554881),this);
}

void KeepAliveComponent::Update(f32 _deltaTime)
{
    Super::Update(_deltaTime);

    if (!m_tryDying)
        return;

    if (!m_updateDone)
    {
        // send another time a die event to be sure ...
        EventDie eventDie(m_disableOnly);
        m_actor->onEvent(&eventDie);
    }

    const Actor::ComponentsVector& actorComponents = m_actor->GetAllComponents();
    bbool keepAlive = bfalse;

    u32 count = actorComponents.size();
    for (u32 i=0; i<count; i++)
    {
        ActorComponent * component = actorComponents[i];
        if (component->keepAlive())
        {
            keepAlive = btrue;
            break;
        }
    }

    if (!keepAlive)
    {
        // seb : it could happen
        //ITF_WARNING_CATEGORY(GPP,m_actor, m_updateDone, "Using KeepAliveComponent on actor with no component alive -> actor is born dead !");

        Scene* scene = m_actor->getScene();
        ITF_ASSERT(scene);
        if (scene)
        {
            if (m_disableOnly)
            {
                Scene * pScene = m_actor->getScene();
                if (pScene && pScene->isAlwaysActiveActor(m_actor))
                {
                    pScene->setAlwaysActiveActor(m_actor, bfalse);
                    m_actor->setAllowedCell(bfalse);
                }
                m_actor->disable();
            } else
            {
                m_actor->requestDestruction();
            }
        }
        m_tryDying = bfalse;
    }

    m_updateDone = btrue;
} 

void KeepAliveComponent::onEvent(Event *_event)
{
    Super::onEvent(_event);

    if (_event->IsClassCRC(EventDie::GetClassCRCStatic()))
    {
        m_tryDying = btrue;
        m_disableOnly = ((EventDie *)_event)->isDisableOnly();
    }
}

void KeepAliveComponent::onBecomeActive()
{
    Super::onBecomeActive();
    if (getTemplate()->getStartDying())
    {
        EventDie eventDie;
        m_actor->onEvent(&eventDie);
    }
}

//-------------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(KeepAliveComponent_Template)
BEGIN_SERIALIZATION(KeepAliveComponent_Template)

    SERIALIZE_MEMBER("startDying", m_startDying);

END_SERIALIZATION()

KeepAliveComponent_Template::KeepAliveComponent_Template()
: m_startDying(bfalse)
{
}

}

