
#ifndef _ITF_EXECUTELUA_EVT_H_
#define _ITF_EXECUTELUA_EVT_H_


#ifndef _ITF_SEQUENCEPLAYERCOMPONENT_H_
#include "engine/sequenceplayer/SequencePlayerComponent.h"
#endif //_ITF_SEQUENCEPLAYERCOMPONENT_H_


namespace ITF
{

class Blob;
class ExecuteLUA_evtTemplate;

class ExecuteLUA_evt : public SequenceEvent
{
public:

    ExecuteLUA_evt              (   );
    virtual ~ExecuteLUA_evt     (   ) {};

    static SequenceEvent * create(SequencePlayerComponent* _owner, SequenceEvent_Template *_template);

    void                    init                    (  const Vec3d& _pos );
    void                    forceCurrentFrame       (   i32 _frame       );
    
    ExecuteLUA_evtTemplate *getTemplate() { return (ExecuteLUA_evtTemplate *)m_template; }

private:
};


class ExecuteLUA_evtTemplate : public SequenceEvent_Template
{
public:
    DECLARE_SERIALIZE()
    DECLARE_OBJECT_CHILD_RTTI(ExecuteLUA_evtTemplate,SequenceEvent_Template,3753561476);

    ExecuteLUA_evtTemplate              (           );
    virtual ~ExecuteLUA_evtTemplate     (           ) {};
    EvtType getType                     (           ) const  { return event_ExecuteLUA ;}

#ifdef ITF_SUPPORT_BLOBEVENT
    static  SequenceEvent_Template* createFromBlob  (Blob* _blob, SequencePlayerComponent_Template* _owner );
    
    virtual     void                serializeToBlob (Blob* _blob);
    virtual     void                fillFromBlob    (Blob* _blob);
#endif //ITF_SUPPORT_BLOBEVENT

private:
};
} // namespace ITF

#endif // _ITF_EXECUTELUA_EVT_H_

