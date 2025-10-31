#include "precompiled_engine.h"

#ifndef _ITF_EXECUTELUA_EVT_H_
#include "engine/sequenceplayer/seq_events/ExecuteLUA_evt.h"
#endif //_ITF_EXECUTELUA_EVT_H_

namespace ITF
{

ExecuteLUA_evt::ExecuteLUA_evt()
{
}

SequenceEvent * ExecuteLUA_evt::create(SequencePlayerComponent* _owner, SequenceEvent_Template *_template)
{
    ExecuteLUA_evt *pNew    = new ExecuteLUA_evt();
    pNew->m_ownerSequence   = _owner;
    pNew->m_template        = _template;
    return pNew;
}

void ExecuteLUA_evt::init(const Vec3d& _pos)
{
}

void ExecuteLUA_evt::forceCurrentFrame(   i32 _frame  )
{
    ExecuteLUA_evtTemplate *evtTemplate = getTemplate();
    ITF_UNUSED(evtTemplate);
    ITF_ASSERT(_frame >= (evtTemplate->getStartFrame() + evtTemplate->getOffset()) && _frame <= (evtTemplate->getStopFrame() + evtTemplate->getOffset()));
}


/////////////////////////////////////////////////////
/// TEMPLATE
/////////////////////////////////////////////////////
IMPLEMENT_OBJECT_RTTI(ExecuteLUA_evtTemplate)

BEGIN_SERIALIZATION_CHILD(ExecuteLUA_evtTemplate)
END_SERIALIZATION()

ExecuteLUA_evtTemplate::ExecuteLUA_evtTemplate()
{
}

#ifdef ITF_SUPPORT_BLOBEVENT
SequenceEvent_Template*   ExecuteLUA_evtTemplate::createFromBlob( Blob* _blob, SequencePlayerComponent_Template* _owner )
{
    ExecuteLUA_evtTemplate* pNew = new ExecuteLUA_evtTemplate();
    pNew->setOwnerSequence(_owner);
    pNew->fillFromBlob(_blob);

    return pNew;
}

void ExecuteLUA_evtTemplate::serializeToBlob (Blob* _blob)
{
}

void ExecuteLUA_evtTemplate::fillFromBlob(Blob* _blob)
{
    fillCommonFromBlob(_blob);
}

#endif //ITF_SUPPORT_BLOBEVENT

} // namespace ITF


