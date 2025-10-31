#include "precompiled_engine.h"

#ifndef _ITF_PLAYSKIP_EVT_H_
#include "engine/sequenceplayer/seq_events/PlaySkip_evt.h"
#endif //_ITF_PLAYSKIP_EVT_H_

#ifndef _ITF_LABEL_EVT_H_
#include "engine/sequenceplayer/seq_events/Label_evt.h"
#endif //_ITF_LABEL_EVT_H_

#ifndef _ITF_LUAHANDLER_H_
#include "core/script/LUAHandler.h"
#endif //_ITF_LUAHANDLER_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_

#ifndef _ITF_ANIMLIGHTCOMPONENT_H_
#include "engine/actors/components/AnimLightComponent.h"
#endif //_ITF_ANIMLIGHTCOMPONENT_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(PlaySkip_evt)

PlaySkip_evt::PlaySkip_evt()
{
}

void    PlaySkip_evt::init(const Vec3d& _pos)
{
}

SequenceEvent * PlaySkip_evt::create(SequencePlayerComponent* _owner, SequenceEvent_Template *_template)
{
    PlaySkip_evt *pNew  = new PlaySkip_evt();
    pNew->m_ownerSequence   = _owner;
    pNew->m_template        = _template;
    return pNew;
}

void    PlaySkip_evt::forceCurrentFrame(   i32 _frame  )
{
}


void PlaySkip_evt::onBecomeActive()
{
    Super::onBecomeActive();
    m_ownerSequence->setSkipSequenceEvent(this);
}

void    PlaySkip_evt::onBecomeInactive()
{
    Super::onBecomeInactive();
	m_ownerSequence->removeSkipSequenceEvent();
}

void PlaySkip_evt::prefetchResources(i32 _pos)
{
    m_ownerSequence->prefetchResources(
        getTemplate()->getStopFrame() +
        getTemplate()->getOffset()    -
        m_ownerSequence->getCurrentFrame());
}


/////////////////////////////////////////////////////
/// TEMPLATE
/////////////////////////////////////////////////////

IMPLEMENT_OBJECT_RTTI(PlaySkip_evtTemplate)

BEGIN_SERIALIZATION_CHILD(PlaySkip_evtTemplate)
    SERIALIZE_ENUM_BEGIN("skipContextIcon",m_skipContextIcon);
        SERIALIZE_ENUM_VAR(ContextIcon_Invalid);
        SERIALIZE_ENUM_VAR(ContextIcon_Select);
        SERIALIZE_ENUM_VAR(ContextIcon_Continue);
        SERIALIZE_ENUM_VAR(ContextIcon_Enter);
        SERIALIZE_ENUM_VAR(ContextIcon_Skip);
        SERIALIZE_ENUM_VAR(ContextIcon_Back);
        SERIALIZE_ENUM_VAR(ContextIcon_Retry);
        SERIALIZE_ENUM_VAR(ContextIcon_Delete);
    SERIALIZE_ENUM_END();
END_SERIALIZATION()

PlaySkip_evtTemplate::PlaySkip_evtTemplate()
{
    m_skipContextIcon = ContextIcon_Skip;
}

#ifdef ITF_SUPPORT_BLOBEVENT
SequenceEvent_Template*   PlaySkip_evtTemplate::createFromBlob( Blob* _blob, SequencePlayerComponent_Template* _owner )
{
    PlaySkip_evtTemplate* pNew = new PlaySkip_evtTemplate();
    pNew->setOwnerSequence(_owner);
    pNew->fillFromBlob(_blob);

    return pNew;
}

void PlaySkip_evtTemplate::serializeToBlob (Blob* _blob)
{
    serializeCommonToBlob(_blob);

    _blob->pushInt32((i32)m_skipContextIcon);
}

void PlaySkip_evtTemplate::fillFromBlob(Blob* _blob)
{
    fillCommonFromBlob(_blob);

    m_skipContextIcon = (EContextIcon)_blob->extractInt32();
}

#endif //ITF_SUPPORT_BLOBEVENT

} // namespace ITF


