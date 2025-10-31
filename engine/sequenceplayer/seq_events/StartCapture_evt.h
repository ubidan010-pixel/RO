
#ifndef _ITF_STARTCAPTURE_EVT_H_
#define _ITF_STARTCAPTURE_EVT_H_


#ifndef _ITF_SEQUENCEPLAYERCOMPONENT_H_
#include "engine/sequenceplayer/SequencePlayerComponent.h"
#endif //_ITF_SEQUENCEPLAYERCOMPONENT_H_


namespace ITF
{

class Blob;
class StartCapture_evtTemplate;

class StartCapture_evt : public SequenceEvent
{
    DECLARE_OBJECT_CHILD_RTTI(StartCapture_evt,SequenceEvent,2721863645);

public:

    StartCapture_evt                (                       );
    virtual ~StartCapture_evt       (                       )   { };

    static SequenceEvent *  create(SequencePlayerComponent* _owner, SequenceEvent_Template *_template);

    void                    init                    (  const Vec3d& _pos    );
    void                    forceCurrentFrame       (   i32 _frame          );

    virtual void            onBecomeInactive  ( );    

    StartCapture_evtTemplate*getTemplate() { return (StartCapture_evtTemplate *)m_template; }

private:
};

class StartCapture_evtTemplate : public SequenceEvent_Template
{
    DECLARE_SERIALIZE()
    DECLARE_OBJECT_CHILD_RTTI(StartCapture_evtTemplate,SequenceEvent_Template,2073084928);

public:

    StartCapture_evtTemplate                (                       );
    virtual ~StartCapture_evtTemplate       (                       )  { };
            EvtType getType                 (                       )  const  { return event_StartCapture;}

#ifdef ITF_SUPPORT_BLOBEVENT

    static  SequenceEvent_Template* createFromBlob  (Blob* _blob, SequencePlayerComponent_Template* _owner );
    
    virtual     void                serializeToBlob (Blob* _blob);
    virtual     void                fillFromBlob    (Blob* _blob);
#endif //ITF_SUPPORT_BLOBEVENT

private:
};

} // namespace ITF

#endif // _ITF_STARTCAPTURE_EVT_H_

