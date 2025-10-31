#include "precompiled_engine.h"

#ifndef _ITF_PLAYLOOP_EVT_H_
#include "engine/sequenceplayer/seq_events/PlayLoop_evt.h"
#endif //_ITF_PLAYLOOP_EVT_H_

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

#ifndef _ITF_ANIMLIGHTCOMPONENT_H_
#include "engine/actors/components/AnimLightComponent.h"
#endif //_ITF_ANIMLIGHTCOMPONENT_H_

#ifndef _ITF_ANIMATION_H_
#include "engine/animation/Animation.h"
#endif //_ITF_ANIMATION_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(PlayLoop_evt)

PlayLoop_evt::PlayLoop_evt()
{
}

void    PlayLoop_evt::init(const Vec3d& _pos)
{
}

SequenceEvent * PlayLoop_evt::create(SequencePlayerComponent* _owner, SequenceEvent_Template *_template)
{
    PlayLoop_evt *pNew  = new PlayLoop_evt();
    pNew->m_ownerSequence   = _owner;
    pNew->m_template        = _template;
    return pNew;
}

void    PlayLoop_evt::forceCurrentFrame(   i32 _frame  )
{
}

void    PlayLoop_evt::onBecomeInactive()
{
    Super::onBecomeInactive();
    if(m_ownerSequence->getState() != SequencePlayerComponent::State_Playing)
        return;
    
    PlayLoop_evtTemplate* evtTemplate = getTemplate();

    if(m_ownerSequence->getCurrentFrame() == evtTemplate->getStopFrame() + evtTemplate->getOffset())
    {
        if(evtTemplate->getStartFrame() + evtTemplate->getOffset() < 0 )
            m_ownerSequence->setCurrentFrame(0, bfalse, bfalse);
        else
            m_ownerSequence->setCurrentFrame(evtTemplate->getStartFrame() + evtTemplate->getOffset(), bfalse, bfalse);
    }
}

/////////////////////////////////////////////////////
/// TEMPLATE
/////////////////////////////////////////////////////

IMPLEMENT_OBJECT_RTTI(PlayLoop_evtTemplate)

BEGIN_SERIALIZATION_CHILD(PlayLoop_evtTemplate)
END_SERIALIZATION()

PlayLoop_evtTemplate::PlayLoop_evtTemplate()
{
}

#ifdef ITF_SUPPORT_BLOBEVENT

SequenceEvent_Template*   PlayLoop_evtTemplate::createFromBlob( Blob* _blob, SequencePlayerComponent_Template* _owner )
{
    PlayLoop_evtTemplate* pNew = new PlayLoop_evtTemplate();
    pNew->setOwnerSequence(_owner);

    pNew->fillFromBlob(_blob);

    return pNew;
}

void PlayLoop_evtTemplate::serializeToBlob (Blob* _blob)
{
    serializeCommonToBlob(_blob);
}

void PlayLoop_evtTemplate::fillFromBlob(Blob* _blob)
{
    fillCommonFromBlob(_blob);
}
#endif //ITF_SUPPORT_BLOBEVENT

} // namespace ITF


