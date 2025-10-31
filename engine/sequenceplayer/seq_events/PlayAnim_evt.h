
#ifndef _ITF_PLAYANIM_EVT_H_
#define _ITF_PLAYANIM_EVT_H_


#ifndef _ITF_SEQUENCEPLAYERCOMPONENT_H_
#include "engine/sequenceplayer/SequencePlayerComponent.h"
#endif //_ITF_SEQUENCEPLAYERCOMPONENT_H_

#ifndef _ITF_BOOLEVENTLIST_H_
#include "engine/sequenceplayer/seq_utilities/BoolEventList.h"
#endif //_ITF_BOOLEVENTLIST_H_

#ifndef _ITF_SPLINE_H_
#include "core/math/spline.h"
#endif //_ITF_SPLINE_H_

namespace ITF
{

class Blob;
class AnimLightComponent;

class PlayAnim_evt : public SequenceEventWithActor
{
    DECLARE_OBJECT_CHILD_RTTI(PlayAnim_evt,SequenceEventWithActor,1287061660);

public:

    PlayAnim_evt                    ();
    virtual ~PlayAnim_evt           ();

    static SequenceEvent *  create(SequencePlayerComponent* _owner, SequenceEvent_Template *_template);

    void                    init                    (  const Vec3d& _pos );
    void                    forceCurrentFrame       (   i32 _frame           );

    virtual void            onBecomeActive();
    virtual void            onBecomeInactive();
    virtual bbool           linkBindedActor          ( bbool _forceLink = bfalse);

    void                    resetSubAnimsToPlay();
    static void             commitSubAnimsToPlay(ActorRef _actorRef, u32 _state);

    i32                     getAnimationIndex();
    i32                     setAnimation(i32 _frame);

    f32                     getStopFrameAnim();
    bbool                   getFlip(i32 _frame, Actor * _actor);
    bbool                   getUsePatches(i32 _frame, AnimLightComponent* animComp);


    u32                     getAnimationDuration(i32 _index);

    const class PlayAnim_evtTemplate *  getTemplate() const { return (PlayAnim_evtTemplate *)m_template; }

private:
    void                    setForceAnim(bbool _force);   

    bbool                   m_forceAnimDone;
    u32                     m_animationDuration;
};

class PlayAnim_evtTemplate : public SequenceEventWithActor_Template
{
    DECLARE_SERIALIZE()
    DECLARE_OBJECT_CHILD_RTTI(PlayAnim_evtTemplate,SequenceEventWithActor_Template,143171982);

public:

    enum typeAnim
    {
        type_not_defined = 0,
        type_anim,
        type_action,
        type_sub_anim,
        ENUM_FORCE_SIZE_32(0)
    };

    PlayAnim_evtTemplate            (           );
    virtual ~PlayAnim_evtTemplate   (           ) {}
    EvtType getType                 (           )  const  { return event_PlayAnim;}

#ifdef ITF_SUPPORT_BLOBEVENT
    static  SequenceEvent_Template* createFromBlob  (Blob* _blob, SequencePlayerComponent_Template* _owner );

    virtual     void                serializeToBlob (Blob* _blob);
    virtual     void                fillFromBlob    (Blob* _blob);
#endif //ITF_SUPPORT_BLOBEVENT

    ITF_INLINE void             setAnim               ( const String8&  _anim   ) { m_anim = _anim; }
    ITF_INLINE String8          getAnim               (                         ) const { return m_anim; }
               String           getAnimName           (                         ) const;

    ITF_INLINE void             setAnimType           ( typeAnim  _typeAnim     ) { m_typeAnim = _typeAnim; }
    ITF_INLINE typeAnim         getAnimType           (                         ) const { return m_typeAnim; }

    ITF_INLINE void             setLoop               ( bbool     _loop         ) { m_loop = _loop; }
    ITF_INLINE bbool            getLoop               (                         ) const { return m_loop; }

    ITF_INLINE void             setPlayRate           ( f32     _playRate     ) { m_playRate = _playRate; }
    ITF_INLINE f32              getPlayRate           (                         ) const { return m_playRate; }

    ITF_INLINE void             setBeginMarker        ( i32       _beginMarker  ) { m_beginMarker = _beginMarker; }
    ITF_INLINE i32              getBeginMarker        (                         ) const { return m_beginMarker; }

    ITF_INLINE void             setEndMarker          ( i32       _endMarker    ) { m_endMarker = _endMarker; }
    ITF_INLINE i32              getEndMarker          (                         ) const { return m_endMarker; }

    ITF_INLINE bbool            getCycle              (                         ) const { return m_cycle; }
     
    ITF_INLINE const BoolEventList& getFlipBoolList       (                         ) const { return m_flip; }
    ITF_INLINE const Spline &       getWeightSpline       (                         ) const { return m_weight; }
    ITF_INLINE const BoolEventList& getUsePatchesBoolList (                         ) const { return m_usePatches; }
    ITF_INLINE const Spline &       getColorSpline        (                         ) const { return m_color; }
    ITF_INLINE const Spline &       getAlphaSpline        (                         ) const { return m_alpha; }

private:
    String8                 m_anim;
    typeAnim                m_typeAnim;
    bbool                   m_loop;
    bbool                   m_cycle;
    f32                     m_playRate;
    i32                     m_beginMarker;
    i32                     m_endMarker;

    BoolEventList           m_flip;
    Spline                  m_weight;
    BoolEventList           m_usePatches;
    Spline                  m_color;
    Spline                  m_alpha;
};


} // namespace ITF

#endif // _ITF_PLAYANIM_EVT_H_

