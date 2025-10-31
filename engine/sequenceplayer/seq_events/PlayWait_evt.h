
#ifndef _ITF_PLAYWAIT_EVT_H_
#define _ITF_PLAYWAIT_EVT_H_


#ifndef _ITF_SEQUENCEPLAYERCOMPONENT_H_
#include "engine/sequenceplayer/SequencePlayerComponent.h"
#endif //_ITF_SEQUENCEPLAYERCOMPONENT_H_


namespace ITF
{

class Blob;
class PlayWait_evtTemplate;

class PlayWait_evt : public SequenceEventWithActor
{
    DECLARE_OBJECT_CHILD_RTTI(PlayWait_evt,SequenceEventWithActor,138106054);

public:
    PlayWait_evt                    (                       );
    virtual ~PlayWait_evt           (                       ) {};
    
    static SequenceEvent *  create(SequencePlayerComponent* _owner, SequenceEvent_Template *_template);

    void                    init                    (   const Vec3d& _pos   );
    void                    forceCurrentFrame       (   i32 _frame          );
    
    bbool                   isBindedActorOk         (                       )          { return true; }
    virtual void            onBecomeActive          (                       );

    PlayWait_evtTemplate*    getTemplate() { return (PlayWait_evtTemplate *)m_template; }

private:
};


class PlayWait_evtTemplate : public SequenceEventWithActor_Template
{
    DECLARE_SERIALIZE()
    DECLARE_OBJECT_CHILD_RTTI(PlayWait_evtTemplate,SequenceEventWithActor_Template,2722851120);

public:
    PlayWait_evtTemplate                    (                       );
    virtual ~PlayWait_evtTemplate           (                       ) {};
    EvtType getType                         (                       )  const  { return event_PlayWait;}

#ifdef ITF_SUPPORT_BLOBEVENT
    static  SequenceEvent_Template* createFromBlob  (Blob* _blob, SequencePlayerComponent_Template* _owner );

    virtual     void                serializeToBlob (Blob* _blob);
    virtual     void                fillFromBlob    (Blob* _blob);
#endif //ITF_SUPPORT_BLOBEVENT

    u32                             getContinueWith (    ) const    { return m_continueWith; }
    void                            setContinueWith (u32 val)       { m_continueWith = val;  }

private:
    u32     m_continueWith;
};

} // namespace ITF

#endif // _ITF_PLAYWAIT_EVT_H_

