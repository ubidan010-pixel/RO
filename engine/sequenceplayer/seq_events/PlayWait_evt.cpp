#include "precompiled_engine.h"

#ifndef _ITF_PLAYWAIT_EVT_H_
#include "engine/sequenceplayer/seq_events/PlayWait_evt.h"
#endif //_ITF_PLAYWAIT_EVT_H_

#ifndef _ITF_LUAHANDLER_H_
#include "core/script/LUAHandler.h"
#endif //_ITF_LUAHANDLER_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_

#ifndef _ITF_ANIMATEDCOMPONENT_H_
#include "engine/actors/components/animatedcomponent.h"
#endif //_ITF_ANIMATEDCOMPONENT_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_ANIMATION_H_
#include "engine/animation/Animation.h"
#endif //_ITF_ANIMATION_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_EVENTMANAGER_H_
#include "engine/events/EventManager.h"
#endif //_ITF_EVENTMANAGER_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(PlayWait_evt)

PlayWait_evt::PlayWait_evt()
{
}

SequenceEvent * PlayWait_evt::create(SequencePlayerComponent* _owner, SequenceEvent_Template *_template)
{
    PlayWait_evt *pNew  = new PlayWait_evt();
    pNew->m_ownerSequence   = _owner;
    pNew->m_template        = _template;
    return pNew;
}

void    PlayWait_evt::init(const Vec3d& _pos)
{
}


void    PlayWait_evt::forceCurrentFrame(   i32 _frame  )
{
}

void    PlayWait_evt::onBecomeActive()
{
    Super::onBecomeActive();
    if(m_ownerSequence->getState() != SequencePlayerComponent::State_Playing)
        return;

    Actor* actor = getBindedActor();
    if(actor)
    {
        EventWaitForActor evt;
        evt.setSender(m_ownerSequence->GetActor()->getRef());
        actor->onEvent(&evt);
    }

    m_ownerSequence->setCurrentFrame(getTemplate()->getStartFrame(), true);
}


/////////////////////////////////////////////////////
/// TEMPLATE
/////////////////////////////////////////////////////

IMPLEMENT_OBJECT_RTTI(PlayWait_evtTemplate)
BEGIN_SERIALIZATION_CHILD(PlayWait_evtTemplate)
    SERIALIZE_MEMBER("ContinueWith",m_continueWith);
END_SERIALIZATION()



PlayWait_evtTemplate::PlayWait_evtTemplate()
: m_continueWith(0)
{

}

#ifdef ITF_SUPPORT_BLOBEVENT
SequenceEvent_Template* PlayWait_evtTemplate::createFromBlob( Blob* _blob, SequencePlayerComponent_Template* _owner )
{
    PlayWait_evtTemplate* pNew = new PlayWait_evtTemplate();
    pNew->setOwnerSequence(_owner);
    pNew->fillFromBlob(_blob);

    return pNew;
}

void PlayWait_evtTemplate::serializeToBlob (Blob* _blob)
{
    serializeCommonToBlob(_blob);
    serializeFriendlyToBlob(_blob);

    _blob->pushInt32(m_continueWith); 
}

void PlayWait_evtTemplate::fillFromBlob(Blob* _blob)
{
    fillCommonFromBlob(_blob);
    fillFriendlyFromBlob(_blob);

    i32 continueWith = _blob->extractInt32();
    setContinueWith(continueWith);
}

#endif //ITF_SUPPORT_BLOBEVENT

} // namespace ITF


