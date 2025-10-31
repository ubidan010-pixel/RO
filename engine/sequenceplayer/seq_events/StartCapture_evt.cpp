#include "precompiled_engine.h"

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_STARTCAPTURE_EVT_H_
#include "engine/sequenceplayer/seq_events/StartCapture_evt.h"
#endif //_ITF_STARTCAPTURE_EVT_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(StartCapture_evt)


StartCapture_evt::StartCapture_evt()
{
}

SequenceEvent * StartCapture_evt::create(SequencePlayerComponent* _owner, SequenceEvent_Template *_template)
{
    StartCapture_evt *pNew  = new StartCapture_evt();
    pNew->m_ownerSequence   = _owner;
    pNew->m_template        = _template;
    return pNew;
}

void StartCapture_evt::init(const Vec3d& _pos)
{
}

void StartCapture_evt::forceCurrentFrame(   i32 _frame  )
{
    StartCapture_evtTemplate* evtTemplate = getTemplate();
    ITF_UNUSED(evtTemplate);
    ITF_ASSERT(_frame >= (evtTemplate->getStartFrame() + evtTemplate->getOffset()) && _frame <= (evtTemplate->getStopFrame() + evtTemplate->getOffset()));
    GFX_ADAPTER->startScreenCapture();
}

void    StartCapture_evt::onBecomeInactive()
{
    GFX_ADAPTER->stopScreenCapture();
}

/////////////////////////////////////////////////////
/// TEMPLATE
/////////////////////////////////////////////////////
IMPLEMENT_OBJECT_RTTI(StartCapture_evtTemplate)

BEGIN_SERIALIZATION_CHILD(StartCapture_evtTemplate)
END_SERIALIZATION()

StartCapture_evtTemplate::StartCapture_evtTemplate()
{
}

#ifdef ITF_SUPPORT_BLOBEVENT

SequenceEvent_Template*   StartCapture_evtTemplate::createFromBlob( Blob* _blob, SequencePlayerComponent_Template* _owner )
{
    StartCapture_evtTemplate* pNew = new StartCapture_evtTemplate();
    pNew->setOwnerSequence(_owner);
    pNew->fillFromBlob(_blob);

    return pNew;
}


void StartCapture_evtTemplate::serializeToBlob (Blob* _blob)
{

}

void StartCapture_evtTemplate::fillFromBlob(Blob* _blob)
{
    fillCommonFromBlob(_blob);
}
#endif //ITF_SUPPORT_BLOBEVENT

} // namespace ITF



