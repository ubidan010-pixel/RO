
#ifndef _ITF_LABEL_EVT_H_
#define _ITF_LABEL_EVT_H_


#ifndef _ITF_SEQUENCEPLAYERCOMPONENT_H_
#include "engine/sequenceplayer/SequencePlayerComponent.h"
#endif //_ITF_SEQUENCEPLAYERCOMPONENT_H_


namespace ITF
{

class Blob;
class Label_evtTemplate;

class Label_evt : public SequenceEvent
{
    DECLARE_OBJECT_CHILD_RTTI(Label_evt,SequenceEvent,4006362151);

public:

    Label_evt                    (                       );
    virtual ~Label_evt           (                       ) {};

    static SequenceEvent *      create(SequencePlayerComponent* _owner, SequenceEvent_Template *_template);

    virtual void                init(const Vec3d& _pos);
    virtual void                forceCurrentFrame(i32 _frame);

	virtual void                onBecomeActive();
	virtual void                onBecomeInactive();
    virtual void                prefetchResources(i32 _pos);

    Label_evtTemplate*       getTemplate() { return (Label_evtTemplate *)m_template; }
};

class Label_evtTemplate : public SequenceEvent_Template
{
    DECLARE_SERIALIZE()
    DECLARE_OBJECT_CHILD_RTTI(Label_evtTemplate,SequenceEvent_Template,3005583724);

public:

    Label_evtTemplate            (                       );
    virtual ~Label_evtTemplate   (                       ) {};
    EvtType getType                 (                       )  const  { return event_Label;}

#ifdef ITF_SUPPORT_BLOBEVENT
    static  SequenceEvent_Template* createFromBlob  (Blob* _blob, SequencePlayerComponent_Template* _owner );

    virtual     void                serializeToBlob (Blob* _blob);
    virtual     void                fillFromBlob    (Blob* _blob);
#endif// ITF_SUPPORT_BLOBEVENT

    const String8 &                  getLabel()  { return m_label; }

private:
    String8                 m_label;
};

} // namespace ITF

#endif // _ITF_LABEL_EVT_H_

