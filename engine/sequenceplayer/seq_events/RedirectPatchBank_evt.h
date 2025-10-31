
#ifndef _ITF_REDIRECTPATCHBANK_EVT_H_
#define _ITF_REDIRECTPATCHBANK_EVT_H_


#ifndef _ITF_SEQUENCEPLAYERCOMPONENT_H_
#include "engine/sequenceplayer/SequencePlayerComponent.h"
#endif //_ITF_SEQUENCEPLAYERCOMPONENT_H_

#ifndef _ITF_SUBANIMLIGHTSET_H_
#include    "engine/animation/SubAnimSet.h"
#endif //_ITF_SUBANIMLIGHTSET_H_


namespace ITF
{

class Blob;
class RedirectPatchBank_evtTemplate;


class RedirectPatchBank_evt : public SequenceEventWithActor
{
public:

    RedirectPatchBank_evt              (   );
    virtual ~RedirectPatchBank_evt     (   ) {};

    static SequenceEvent * create(SequencePlayerComponent* _owner, SequenceEvent_Template *_template);

    void                    init                    (  const Vec3d& _pos );
    void                    forceCurrentFrame       (   i32 _frame       );

    RedirectPatchBank_evtTemplate *getTemplate() { return (RedirectPatchBank_evtTemplate *)m_template; }

private:
    u32                     m_index;
};

struct  BankChange_TemplateList
{
    DECLARE_SERIALIZE()

    ITF_VECTOR<BankChange_Template> m_bankList;
};


class RedirectPatchBank_evtTemplate : public SequenceEventWithActor_Template
{
public:
    DECLARE_SERIALIZE()
    DECLARE_OBJECT_CHILD_RTTI(RedirectPatchBank_evtTemplate,SequenceEventWithActor_Template,2425993289);

    RedirectPatchBank_evtTemplate              (           );
    virtual ~RedirectPatchBank_evtTemplate     (           ) { };
    EvtType getType                    (           ) const  { return event_RedirectPatchBank ;}

#ifdef ITF_SUPPORT_BLOBEVENT
    static  SequenceEvent_Template* createFromBlob  (Blob* _blob, SequencePlayerComponent_Template* _owner );
    virtual     void                serializeToBlob (Blob* _blob);
    virtual     void                fillFromBlob    (Blob* _blob);
#endif // ITF_SUPPORT_BLOBEVENT

    const ITF_VECTOR<BankChange_Template> & getBankChangeList() { return m_bankChangeList.m_bankList; }

private:
    BankChange_TemplateList         m_bankChangeList;
};
} // namespace ITF

#endif // _ITF_REDIRECTPATCHBANK_EVT_H_

