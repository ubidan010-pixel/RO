
#ifndef _ITF_PLAYSPAWN_EVT_H_
#define _ITF_PLAYSPAWN_EVT_H_


#ifndef _ITF_SEQUENCEPLAYERCOMPONENT_H_
#include "engine/sequenceplayer/SequencePlayerComponent.h"
#endif //_ITF_SEQUENCEPLAYERCOMPONENT_H_


namespace ITF
{

class Blob;
class PlaySpawn_evtTemplate;

class PlaySpawn_evt : public SequenceEventWithActor
{
    DECLARE_OBJECT_CHILD_RTTI(PlaySpawn_evt,SequenceEventWithActor,1632466582);

public:

    PlaySpawn_evt               (               );
    virtual ~PlaySpawn_evt      (               );

    static SequenceEvent *      create(SequencePlayerComponent* _owner, SequenceEvent_Template *_template);

    void                        init                    (  const Vec3d& _pos );
    void                        forceCurrentFrame       (   i32 _time        );
    
    void                        spawnActor();
    void                        loadActorResources();
    void                        unloadActorResources();


    ITF_INLINE bbool            isBindedActorOk          (  ) { return true; }
    virtual void                onBecomeActive();
    virtual void                onBecomeInactive();

    PlaySpawn_evtTemplate*      getTemplate() { return (PlaySpawn_evtTemplate *)m_template; }

private:

    bbool                       m_actorSpawnDone;
};


class PlaySpawn_evtTemplate : public SequenceEventWithActor_Template
{
    DECLARE_SERIALIZE()
    DECLARE_OBJECT_CHILD_RTTI(PlaySpawn_evtTemplate,SequenceEventWithActor_Template,2135584656);

public:

    PlaySpawn_evtTemplate                               (                    );
    virtual ~PlaySpawn_evtTemplate                      (                    ) {}
    EvtType                         getType             (                    )  const  { return event_PlaySpawn;}

#ifdef ITF_SUPPORT_BLOBEVENT
    static  SequenceEvent_Template* createFromBlob  (Blob* _blob, SequencePlayerComponent_Template* _owner );
    
    virtual void                    serializeToBlob (Blob* _blob);
    void                            fillFromBlob(Blob* _blob);
#endif //ITF_SUPPORT_BLOBEVENT

    ITF_INLINE void                 setFileName              ( String8& _fileName )         { m_fileName = _fileName;}
    ITF_INLINE const String8 &      getFileName              (                    )  const  { return m_fileName;}

    ITF_INLINE void                 setStay                  ( bbool _stay        )         { m_stay = _stay;}
    ITF_INLINE bbool                isStay                   (                    )         { return m_stay;}

    ITF_INLINE void                 setVisible               ( bbool _stay        )         { m_stay = _stay;}
    ITF_INLINE bbool                isVisible                (                    )         { return m_stay;}

	ITF_INLINE const Vec2d &		getSpawnOffset	         (                    ) const	{ return m_spawnOffset;}
	ITF_INLINE void					setSpawnOffset			 ( Vec2d& _offset     )			{ m_spawnOffset.m_x = _offset.m_x; m_spawnOffset.m_y = _offset.m_y; }

	ITF_INLINE bbool				getFlipped				 (                    )			{ return m_flipped;}
	ITF_INLINE void					setFlipped				 ( bbool _flipped     )			{ m_flipped = _flipped;}


private:

    bbool                       m_visible;
    bbool                       m_stay;
    String8                     m_fileName;
	Vec2d						m_spawnOffset;
	bbool						m_flipped;
};

} // namespace ITF

#endif // _ITF_PLAYSPAWN_EVT_H_

