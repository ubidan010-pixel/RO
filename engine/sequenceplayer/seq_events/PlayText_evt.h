
#ifndef _ITF_PLAYTEXT_EVT_H_
#define _ITF_PLAYTEXT_EVT_H_


#ifndef _ITF_SEQUENCEPLAYERCOMPONENT_H_
#include "engine/sequenceplayer/SequencePlayerComponent.h"
#endif //_ITF_SEQUENCEPLAYERCOMPONENT_H_

#ifndef _ITF_SIMPLETEXTCOMPONENT_H_
#include "gameplay/components/UI/SimpleTextComponent.h"
#endif //_ITF_SIMPLETEXTCOMPONENT_H_

namespace ITF
{

class Blob;
class PlayText_evtTemplate;


class PlayText_evt : public SequenceEventWithActor
{
    DECLARE_OBJECT_CHILD_RTTI(PlayText_evt,SequenceEventWithActor,3534884615);

public:

    PlayText_evt              (   );
    virtual ~PlayText_evt     (   ) {};

    static SequenceEvent * create(SequencePlayerComponent* _owner, SequenceEvent_Template *_template);

    void                    init                    (  const Vec3d& _pos );
    void                    forceCurrentFrame       (   i32 _frame       );
    void                    onBecomeActive          (                    );
    void                    onBecomeInactive        (                    );

    PlayText_evtTemplate *getTemplate() { return (PlayText_evtTemplate *)m_template; }

private:
    u32                     m_index;
};


class PlayText_evtTemplate : public SequenceEventWithActor_Template
{
public:
    DECLARE_SERIALIZE()
    DECLARE_OBJECT_CHILD_RTTI(PlayText_evtTemplate,SequenceEventWithActor_Template,2006548793);

    PlayText_evtTemplate              (           );
    virtual ~PlayText_evtTemplate     (           ) { };
    EvtType getType                    (           ) const  { return event_PlayText ;}

#ifdef ITF_SUPPORT_BLOBEVENT
    static  SequenceEvent_Template* createFromBlob  (Blob* _blob, SequencePlayerComponent_Template* _owner );
    virtual     void                serializeToBlob (Blob* _blob);
    virtual     void                fillFromBlob    (Blob* _blob);
#endif // ITF_SUPPORT_BLOBEVENT

    const Path &                    getTextPath() { return m_text; }
    const SimpleTextComponent &     getParams() { return m_textParams; }

private:
    Path                    m_text;
    
    SimpleTextComponent     m_textParams;
};
} // namespace ITF

#endif // _ITF_PLAYTEXT_EVT_H_

