
#ifndef _ITF_PLAYSOUND_EVT_H_
#define _ITF_PLAYSOUND_EVT_H_


#ifndef _ITF_SEQUENCEPLAYERCOMPONENT_H_
#include "engine/sequenceplayer/SequencePlayerComponent.h"
#endif //_ITF_SEQUENCEPLAYERCOMPONENT_H_

#ifndef _ITF_SOUNDDESCRIPTOR_H_
#include "engine/sound/SoundDescriptor.h"
#endif //_ITF_SOUNDDESCRIPTOR_H_

#include "engine/AdaptersInterfaces/AudioMiddlewareAdapter_Types.h"


namespace ITF
{

class Blob;
class PlaySound_evtTemplate;

class PlaySound_evt : public SequenceEventWithActor
{
public:

    PlaySound_evt              (   );
    virtual ~PlaySound_evt     (   ) {};

    static SequenceEvent * create(SequencePlayerComponent* _owner, SequenceEvent_Template *_template);

    void                    init                    (  const Vec3d& _pos );
    void                    forceCurrentFrame       (   i32 _frame       );
    void                    onBecomeActive          (                    );
    void                    onBecomeInactive        (                    );
    void                    forceStop               (                    );

    PlaySound_evtTemplate *getTemplate() { return (PlaySound_evtTemplate *)m_template; }

private:
    ITF::SoundHandle                     m_soundHandle;
};


class PlaySound_evtTemplate : public SequenceEventWithActor_Template
{
public:
    DECLARE_SERIALIZE()
    DECLARE_OBJECT_CHILD_RTTI(PlaySound_evtTemplate,SequenceEventWithActor_Template,2329430233);

    PlaySound_evtTemplate              (           );
    virtual ~PlaySound_evtTemplate     (           ) { };
    EvtType getType                    (           ) const  { return event_PlaySound ;}

#ifdef ITF_SUPPORT_BLOBEVENT
    static  SequenceEvent_Template* createFromBlob  (Blob* _blob, SequencePlayerComponent_Template* _owner );
    virtual     void                serializeToBlob (Blob* _blob);
    virtual     void                fillFromBlob    (Blob* _blob);
#endif // ITF_SUPPORT_BLOBEVENT

    const Path &                    getSoundPath() { return m_sound; }
    const SoundParams &             getParams() { return m_soundParams; }
    const f32                       getVolume() { return m_volume; }
    const String8 &                 getCategory() { return m_category; }
    const bbool                     getIsStream() { return m_isStream; }
    const StringID&           getWwiseEventGuid() const { return m_wwiseEventGuid; }


private:
    Path                 m_sound;
    f32                  m_volume; // in DB
    String8              m_category;
    bbool                m_isStream;

    SoundParams          m_soundParams;

    StringID      m_wwiseEventGuid;

};
} // namespace ITF

#endif // _ITF_PLAYSOUND_EVT_H_

