
#ifndef _ITF_PLAYLOOP_EVT_H_
#define _ITF_PLAYLOOP_EVT_H_


#ifndef _ITF_SEQUENCEPLAYERCOMPONENT_H_
#include "engine/sequenceplayer/SequencePlayerComponent.h"
#endif //_ITF_SEQUENCEPLAYERCOMPONENT_H_


namespace ITF
{

class Blob;
class PlayLoop_evtTemplate;

class PlayLoop_evt : public SequenceEvent
{
    DECLARE_OBJECT_CHILD_RTTI(PlayLoop_evt,SequenceEvent,2084041347);

public:

    PlayLoop_evt                    (                       );
    virtual ~PlayLoop_evt           (                       ) {};

    static SequenceEvent *      create(SequencePlayerComponent* _owner, SequenceEvent_Template *_template);

    void                        init                    (  const Vec3d& _pos     );
    void                        forceCurrentFrame       (   i32 _frame           );

    virtual void                onBecomeInactive(           );

    PlayLoop_evtTemplate*        getTemplate() { return (PlayLoop_evtTemplate *)m_template; }

};

class PlayLoop_evtTemplate : public SequenceEvent_Template
{
    DECLARE_SERIALIZE()
    DECLARE_OBJECT_CHILD_RTTI(PlayLoop_evtTemplate,SequenceEvent_Template,3712358567);

public:

    PlayLoop_evtTemplate            (                       );
    virtual ~PlayLoop_evtTemplate   (                       ) {};
    EvtType getType                 (                       )  const  { return event_PlayLoop;}

#ifdef ITF_SUPPORT_BLOBEVENT
    static  SequenceEvent_Template* createFromBlob  (Blob* _blob, SequencePlayerComponent_Template* _owner );

    virtual     void                serializeToBlob (Blob* _blob);
    virtual     void                fillFromBlob    (Blob* _blob);
#endif //ITF_SUPPORT_BLOBEVENT
};


} // namespace ITF

#endif // _ITF_PLAYLOOP_EVT_H_

