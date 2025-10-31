#include "precompiled_engine.h"

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

IMPLEMENT_OBJECT_RTTI(Label_evt)

Label_evt::Label_evt()
{
}

void    Label_evt::init(const Vec3d& _pos)
{
}

SequenceEvent * Label_evt::create(SequencePlayerComponent* _owner, SequenceEvent_Template *_template)
{
    Label_evt *pNew  = new Label_evt();
    pNew->m_ownerSequence   = _owner;
    pNew->m_template        = _template;
    return pNew;
}

void    Label_evt::forceCurrentFrame(   i32 _frame  )
{
}


void Label_evt::onBecomeActive()
{
}

void    Label_evt::onBecomeInactive()
{
}

void Label_evt::prefetchResources(i32 _pos)
{
}


/////////////////////////////////////////////////////
/// TEMPLATE
/////////////////////////////////////////////////////

IMPLEMENT_OBJECT_RTTI(Label_evtTemplate)

BEGIN_SERIALIZATION_CHILD(Label_evtTemplate)
SERIALIZE_MEMBER("Label",m_label);
END_SERIALIZATION()

Label_evtTemplate::Label_evtTemplate()
{
}


#ifdef ITF_SUPPORT_BLOBEVENT

SequenceEvent_Template*   Label_evtTemplate::createFromBlob( Blob* _blob, SequencePlayerComponent_Template* _owner )
{
    Label_evtTemplate* pNew = new Label_evtTemplate();
    pNew->setOwnerSequence(_owner);
    pNew->fillFromBlob(_blob);

    return pNew;
}

void Label_evtTemplate::serializeToBlob (Blob* _blob)
{
    serializeCommonToBlob(_blob);

    // label
    _blob->pushString8(m_label);
}

void Label_evtTemplate::fillFromBlob(Blob* _blob)
{
    fillCommonFromBlob(_blob);

    _blob->extractString8(m_label);
}

#endif //ITF_SUPPORT_BLOBEVENT

} // namespace ITF


