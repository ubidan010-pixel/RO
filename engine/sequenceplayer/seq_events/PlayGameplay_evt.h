
#ifndef _ITF_PLAYGAMEPLAYEVENT_EVT_H_
#define _ITF_PLAYGAMEPLAYEVENT_EVT_H_


#ifndef _ITF_SEQUENCEPLAYERCOMPONENT_H_
#include "engine/sequenceplayer/SequencePlayerComponent.h"
#endif //_ITF_SEQUENCEPLAYERCOMPONENT_H_

#ifndef _ITF_SEQUENCEPLAYERCOMPONENT_H_
#include "engine/sequenceplayer/SequencePlayerComponent.h"
#endif //_ITF_SEQUENCEPLAYERCOMPONENT_H_


namespace ITF
{

class Blob;
class PlayGameplay_evtTemplate;

class PlayGameplay_evt : public SequenceEventWithActor
{
    DECLARE_OBJECT_CHILD_RTTI(PlayGameplay_evt,SequenceEventWithActor,3560947707);

public:

    PlayGameplay_evt                    ();
    virtual ~PlayGameplay_evt           () {}

    static SequenceEvent *      create(SequencePlayerComponent* _owner, SequenceEvent_Template *_template);

    void                        init                    (  const Vec3d& _pos );
    void                        forceCurrentFrame       (   i32 _frame       );

    virtual     void            onBecomeActive(    );
    virtual     void            onBecomeInactive(    );

    PlayGameplay_evtTemplate*   getTemplate() { return (PlayGameplay_evtTemplate *)m_template; }

private:  
    bbool       m_hasExecuted;
};

class PlayGameplay_evtTemplate : public SequenceEventWithActor_Template
{
    DECLARE_SERIALIZE()
    DECLARE_OBJECT_CHILD_RTTI(PlayGameplay_evtTemplate,SequenceEventWithActor_Template,1477606536);

public:

    PlayGameplay_evtTemplate            ();
    virtual ~PlayGameplay_evtTemplate   ();
    EvtType getType                     (            )  const  { return event_GameplayEvent;}

#ifdef ITF_SUPPORT_BLOBEVENT
    static  SequenceEvent_Template* createFromBlob  (Blob* _blob, SequencePlayerComponent_Template* _owner );

    virtual     void                serializeToBlob (Blob* _blob);
    void                            fillFromBlob(Blob* _blob);
#endif //ITF_SUPPORT_BLOBEVENT
    
    const String8 &                 getOnEnterEventName () { return m_onEnterEventName; }
    Event*                          getOnEnterEvent     () { return m_onEnterEvent; }

    const String8 &                 getOnExitEventName  () { return m_onExitEventName; }
    Event*                          getOnExitEvent      () { return m_onExitEvent; }

    const bbool                     isExecuteOnce       () { return m_executeOnce; }
    const bbool                     isBroadcastEvent    () { return m_broadcastEvent; }


private:  
    String8     m_onEnterEventName;
    Event *     m_onEnterEvent;

    String8     m_onExitEventName;
    Event *     m_onExitEvent;

    bbool       m_executeOnce;
    bbool       m_broadcastEvent;
};


} // namespace ITF

#endif // _ITF_PLAYGAMEPLAYEVENT_EVT_H_

