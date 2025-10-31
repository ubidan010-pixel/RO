
#ifndef _ITF_PLAYSKIP_EVT_H_
#define _ITF_PLAYSKIP_EVT_H_


#ifndef _ITF_SEQUENCEPLAYERCOMPONENT_H_
#include "engine/sequenceplayer/SequencePlayerComponent.h"
#endif //_ITF_SEQUENCEPLAYERCOMPONENT_H_

#ifndef _ITF_GAMEPLAYTYPES_H_
#include "gameplay/GameplayTypes.h"
#endif //_ITF_GAMEPLAYTYPES_H_

namespace ITF
{

class Blob;
class PlaySkip_evtTemplate;

class PlaySkip_evt : public SequenceEvent
{
    DECLARE_OBJECT_CHILD_RTTI(PlaySkip_evt,SequenceEvent,2680464182);

public:

    PlaySkip_evt                    (                       );
    virtual ~PlaySkip_evt           (                       ) {};

    static SequenceEvent *      create(SequencePlayerComponent* _owner, SequenceEvent_Template *_template);

    void                        init                    (  const Vec3d& _pos     );
    void                        forceCurrentFrame       (   i32 _frame           );

	virtual void                onBecomeActive			(           );
	virtual void                onBecomeInactive		(           );
    virtual void                prefetchResources       (  i32 _pos );

    PlaySkip_evtTemplate*       getTemplate() { return (PlaySkip_evtTemplate *)m_template; }
};

class PlaySkip_evtTemplate : public SequenceEvent_Template
{
    DECLARE_SERIALIZE()
    DECLARE_OBJECT_CHILD_RTTI(PlaySkip_evtTemplate,SequenceEvent_Template,173814531);

public:

    PlaySkip_evtTemplate            (                       );
    virtual ~PlaySkip_evtTemplate   (                       ) {};
    EvtType getType                 (                       )  const  { return event_PlaySkip;}
    EContextIcon getSkipContextIcon (                       )  const  { return m_skipContextIcon;}

#ifdef ITF_SUPPORT_BLOBEVENT
    static  SequenceEvent_Template* createFromBlob  (Blob* _blob, SequencePlayerComponent_Template* _owner );

    virtual     void                serializeToBlob (Blob* _blob);
    virtual     void                fillFromBlob    (Blob* _blob);
#endif //ITF_SUPPORT_BLOBEVENT

private:
    EContextIcon m_skipContextIcon;
};

} // namespace ITF

#endif // _ITF_PLAYSKIP_EVT_H_

