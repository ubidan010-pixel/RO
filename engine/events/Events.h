#ifndef _ITF_EVENTS_H_
#define _ITF_EVENTS_H_

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

#ifndef _ITF_VEC3D_H_
#include "core/math/vec3d.h"
#endif //_ITF_VEC3D_H_

#ifndef _ITF_OBJECTFACTORY_H_
#include "core/serializer/ObjectFactory.h"
#endif //_ITF_OBJECTFACTORY_H_

#ifndef _ITF_SEQUENCEPLAYERCOMPONENT_H_
#include "engine/sequenceplayer/SequencePlayerComponent.h"
#endif //_ITF_SEQUENCEPLAYERCOMPONENT_H_

#ifndef _ITF_SOUND_ADAPTER_
#include "engine/AdaptersInterfaces/SoundAdapter.h"
#endif //_ITF_SOUND_ADAPTER_
#include "core/types.h"

#ifndef _ITF_SOUNDMANAGER_H_
#include "engine/sound/SoundManager.h"
#endif //_ITF_SOUNDMANAGER_H_

#ifndef _ITF_CONFIG_H_
#include "core/Config.h"
#endif //_ITF_CONFIG_H_

namespace ITF
{
    class Event : public IRTTIObject
    {
        DECLARE_OBJECT_CHILD_RTTI(Event, IRTTIObject,772421353);
        DECLARE_SERIALIZE_VIRTUAL()

    public:
        Event() {}
        Event(ObjectRef _sender) : m_sender(_sender) {}
        virtual ~Event() {};

        ObjectRef   getSender() const { return m_sender; }
        void        setSender(ObjectRef _val) { m_sender = _val; }

        virtual ObjectRef getActivator() const { return ITF_INVALID_OBJREF; }
        virtual void setActivator( ObjectRef _activator ) {}
        virtual void setFromRetriggerOnCheckpoint( const bbool _value ) {}

    protected:
        ObjectRef m_sender;
    };

    class EventTrigger : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventTrigger,Event,1343042510);
        DECLARE_SERIALIZE()

    public :
        EventTrigger()
            : m_toggle(bfalse)
            , m_activated(bfalse)
            , m_activator(ITF_INVALID_OBJREF)
            , m_refPos(Vec2d::Zero)
            , m_fromRetriggerOnCheckpoint(bfalse)
        {}

        ITF_INLINE bbool getToggle() const { return m_toggle; }
        ITF_INLINE void setToggle(bbool _val) { m_toggle = _val; }

        ITF_INLINE bbool getActivated() const { return m_activated; }
        ITF_INLINE void setActivated(bbool _val) { m_activated = _val; }

        virtual ObjectRef getActivator() const { return m_activator; }
        virtual void setActivator(ObjectRef _val) { m_activator = _val; }

        ITF_INLINE const Vec2d& getRefPos() const { return m_refPos; }
        ITF_INLINE void setRefPos( const Vec2d& _pos ) { m_refPos = _pos; }

        ITF_INLINE bbool getFromRetriggerOnCheckpoint() const { return m_fromRetriggerOnCheckpoint; }
        virtual void setFromRetriggerOnCheckpoint( const bbool _value ) { m_fromRetriggerOnCheckpoint = _value; }

    private:
        bbool           m_toggle;
        bbool           m_activated;
        ObjectRef       m_activator;
        Vec2d           m_refPos;
        bbool           m_fromRetriggerOnCheckpoint;
    };
    
    class EventTeleport : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventTeleport,Event,1075308886);
        DECLARE_SERIALIZE()
    
    public:

        EventTeleport()
            : m_applyPosAndAngle(btrue)
            , m_pos(Vec3d::Zero)
            , m_angle(0.f)
            , m_reset(bfalse)
            , m_flip(bfalse)
            , m_overrideFlip(bfalse)
        {}
        EventTeleport( const Vec3d& _pos, f32 _angle, bbool _reset )
            : m_applyPosAndAngle(btrue)
            , m_pos(_pos)
            , m_angle(_angle)
            , m_reset(_reset)
            , m_overrideFlip(bfalse)
            , m_flip(bfalse)
        {}

        ITF_INLINE bbool        getApplyPosAndAngle() const { return m_applyPosAndAngle; }
        const Vec3d&            getPos() const { return m_pos; }
        f32                     getAngle() const { return m_angle; }
        bbool                   getReset() const { return m_reset; }
        bbool                   getOverrideFlip() const { return m_overrideFlip; }
        bbool                   getFlip() const { return m_flip; }

        void                    setFlip( bbool _val ) { m_overrideFlip = btrue; m_flip = _val; }

    private:

        bbool m_applyPosAndAngle;   // true by default; if false, then this is just a callback
        Vec3d m_pos;
        f32 m_angle;
        bbool m_reset;
        bbool m_flip;
        bbool m_overrideFlip;
    };

    class EventTeleportToActor : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventTeleportToActor, Event,557779126);

    public:
        EventTeleportToActor() : Super(), m_actorRef() {}
        EventTeleportToActor(const ActorRef& _actorRef) : Super(), m_actorRef(_actorRef) {}

        ITF_INLINE const ActorRef& getActorRef() const { return m_actorRef; }

    private:
        ActorRef m_actorRef;
    };

    class EventReset : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventReset,Event,2551104629);
        DECLARE_SERIALIZE()
    };


    class EventActivateCheckpoint : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventActivateCheckpoint,Event,1094365377);
        DECLARE_SERIALIZE()

    public:

        EventActivateCheckpoint() : m_activate(btrue) {}
        EventActivateCheckpoint( bbool _activate ) : m_activate(_activate) {}
        ITF_INLINE bbool getActivate() const { return m_activate; }
        ITF_INLINE void setActivate(bbool _val) { m_activate = _val; }

    private:
        bbool               m_activate;

    };

    class EventSequenceControl : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventSequenceControl,Event,1151213732);
        DECLARE_SERIALIZE()

    public:

        EventSequenceControl() : m_state(SequencePlayerComponent::State_Stopped) , m_forceLabel(bfalse) {}
        EventSequenceControl( SequencePlayerComponent::State _state ) : m_state(_state), m_forceLabel(bfalse) {}
        
        ITF_INLINE SequencePlayerComponent::State getState() const { return m_state; }
        ITF_INLINE void setState(SequencePlayerComponent::State _val) { m_state = _val; }

        ITF_INLINE const String8 & getLabel() const { return m_label; }
        ITF_INLINE void setLabel(const String8 & _val) { m_label = _val; }

        ITF_INLINE bbool isForceLabel() const { return m_forceLabel; }
        ITF_INLINE void  setForceLabel(bbool _val) { m_forceLabel = _val; }


    private:
        SequencePlayerComponent::State  m_state;
        String8                         m_label;
        bbool                           m_forceLabel;
    };


    class EventSequenceActorActivate : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventSequenceActorActivate,Event,1119571699);
        DECLARE_SERIALIZE()

    public:

        EventSequenceActorActivate() : m_activate(btrue) {}
        EventSequenceActorActivate( bbool _activate ) : m_activate(_activate) {}

        ITF_INLINE bbool        getActivate() const { return m_activate; }
        ITF_INLINE void         setActivate(bbool _activate) { m_activate = _activate; }

    private:
        bbool       m_activate;
    };

    class EventSequenceActorSaveZ : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventSequenceActorSaveZ,Event,2573269496);
        DECLARE_SERIALIZE()

    public:

        EventSequenceActorSaveZ() {}

    private:
    };

    class EventSequenceSetPlayerPos : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventSequenceSetPlayerPos,Event,3409872873);
        DECLARE_SERIALIZE()

    public:
        EventSequenceSetPlayerPos()
        : m_playerId(0)
        , m_playerMode(GAMEMODE_UNKNOWN)
        , m_useBaseAdjust(bfalse) {}

        EventSequenceSetPlayerPos(const String8 & _actor, u32 _playerId, GameMode _playerMode)
        : m_actor(_actor)
        , m_playerId(_playerId)
        , m_playerMode(_playerMode)
        , m_useBaseAdjust(bfalse) {}


        ITF_INLINE const String8 &  getActor() const { return m_actor; }
        ITF_INLINE void             setActor(const String8 & _actor) { m_actor = _actor; }

        ITF_INLINE u32              getPlayerId() const { return m_playerId; }
        ITF_INLINE void             setPlayerId(u32 _playerId) { m_playerId = _playerId; }

        ITF_INLINE GameMode         getPlayerMode() const { return m_playerMode; }
        ITF_INLINE void             setPlayerMode(GameMode _playerMode) { m_playerMode = _playerMode; }

        ITF_INLINE bbool            getUseBaseAdjust() const { return m_useBaseAdjust; }
        ITF_INLINE void             setUseBaseAdjust(bbool _useBaseAdjust) { m_useBaseAdjust = _useBaseAdjust; }


    private:
        String8     m_actor;
        u32         m_playerId;
        GameMode    m_playerMode;
        bbool       m_useBaseAdjust;
    };

    class EventSequenceSpeedChange : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventSequenceSpeedChange,Event,3037544556);
        DECLARE_SERIALIZE()

    public:

        EventSequenceSpeedChange() : m_speed(1.f) 
        {}

        EventSequenceSpeedChange(f32 _speed) : m_speed(_speed)
        {}

        ITF_INLINE f32  getSpeed() const { return m_speed; }
        ITF_INLINE void setSpeed(f32 _speed) { m_speed = _speed; }

    private:
        f32     m_speed;
    };

    class EventSequenceActivatePlayers : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventSequenceActivatePlayers,Event,3249842910);
        DECLARE_SERIALIZE()

    public:
        bbool               getActivate() const { return m_activate; }
        void                setActivate(bbool _activate) { m_activate = _activate; }

        EventSequenceActivatePlayers() : m_activate(btrue) {}

    private:
        bbool       m_activate;
    };


    class EventSequenceActorPrepare : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventSequenceActorPrepare,Event,3178520967);
        DECLARE_SERIALIZE()

    public:

        EventSequenceActorPrepare()
            : m_lockInput(btrue)
            , m_friction(1.f)
            , m_revive(bfalse)
        {}

        EventSequenceActorPrepare(bbool _lockInput)
            : m_lockInput(_lockInput)
            , m_friction(1.f)
            , m_revive(bfalse)
        {}

        ITF_INLINE bbool getLockInput() const { return m_lockInput; }
        ITF_INLINE void  setLockInput( const bbool _value ) { m_lockInput = _value; }

        ITF_INLINE f32  getFriction() const { return m_friction; }
        ITF_INLINE void setFriction( const f32 _value ) { m_friction = _value; }

        ITF_INLINE bbool getRevive() const { return m_revive; }
        ITF_INLINE void  setRevive( const bbool _value ) { m_revive = _value; }

    private:
        bbool m_lockInput;
        f32   m_friction;
        bbool m_revive;
    };


    class EventSequenceActorReady : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventSequenceActorReady,Event,1601154857);
        DECLARE_SERIALIZE()

    public:

        EventSequenceActorReady() : m_isReady(bfalse) {}
        EventSequenceActorReady( bbool _ready ) : m_isReady(_ready) {}

        ITF_INLINE bbool        isReady() const { return m_isReady; }
        ITF_INLINE void         setReady(bbool _isReady) { m_isReady = _isReady; }

    private:
        bbool       m_isReady;
    };

    class EventWaitForActor : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventWaitForActor,Event,866051242);
        DECLARE_SERIALIZE()
    };


    class EventEndWait : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventEndWait,Event,4251945611);
        DECLARE_SERIALIZE()
    };

	class EventSkipSequence : public Event
	{
		DECLARE_OBJECT_CHILD_RTTI(EventSkipSequence,Event,4247999814);
		DECLARE_SERIALIZE()
	};

    class EventVideoCapture : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventVideoCapture,Event,1922224351);
        DECLARE_SERIALIZE()

    public:
        EventVideoCapture()
            : m_start(btrue)  { }
        EventVideoCapture(bbool _start)
            : m_start(_start) { }


        ITF_INLINE bbool isStart (            ) const { return m_start; }
        ITF_INLINE void  setStart(bbool _start)       { m_start = _start; }

    private:
        bbool m_start;
    };


    class DisplayText : public Event
    {
        DECLARE_SERIALIZE()
        DECLARE_OBJECT_CHILD_RTTI(DisplayText,Event,3327400120);

    public:

        DisplayText() {}
        virtual ~DisplayText() {}
        ITF_INLINE const String8 & getTextFriendly() const { return m_textFriendly; }
    private:
        String8 m_textFriendly;
    };

    class EventPause : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventPause,Event,829919267);
        DECLARE_SERIALIZE()

    public:

        EventPause() : m_pause(btrue) {}
        EventPause( bbool _pause ) : m_pause(_pause) {}
        ITF_INLINE bbool getPause() const { return m_pause; }
        ITF_INLINE void setPause(bbool _val) { m_pause = _val; }

    private:
        bbool               m_pause;

    };


    class EventDie : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventDie,Event,1997554881);
        DECLARE_SERIALIZE()

    public:

        EventDie() : m_disableOnly(bfalse) {}
        EventDie( bbool _disableOnly ) : m_disableOnly(_disableOnly) {}
        ITF_INLINE bbool isDisableOnly() const { return m_disableOnly; }

    private:
        bbool               m_disableOnly;
    };


    class EventAddForce : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventAddForce,Event,1731368564);
        DECLARE_SERIALIZE()

    public:
        enum ForceType
        {
            ForceType_UNDEFINED = -1,
            ForceType_DEFAULT = 0,
            ForceType_FAN,            
            ENUM_FORCE_SIZE_32(0)
        };

        EventAddForce() : m_force(Vec2d::Zero), m_forceType(ForceType_DEFAULT) {}
        EventAddForce( const Vec2d & _force ) : m_force(_force), m_forceType(ForceType_DEFAULT) {}
        EventAddForce( const Vec2d & _force, const ForceType _type ) : m_force(_force), m_forceType(_type) {}
        ITF_INLINE const Vec2d & getForce() const { return m_force; }
        ITF_INLINE void setForce(const Vec2d & _force) { m_force = _force; }
        ITF_INLINE ForceType getForceType() const { return m_forceType; }

    private:
        Vec2d m_force;
        ForceType m_forceType;
    };

    class EventSetSpeed : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventSetSpeed,Event,2924911315);
        DECLARE_SERIALIZE()

    public:

        EventSetSpeed() : m_speed(Vec2d::Zero) {}
        EventSetSpeed( const Vec2d & _speed ) : m_speed(_speed) {}
        ITF_INLINE const Vec2d & getSpeed() const { return m_speed; }
        ITF_INLINE void setSpeed(const Vec2d & _speed) { m_speed = _speed; }

    private:
        Vec2d m_speed;
    };

    class EventUnstick: public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventUnstick,Event,54024986);
        DECLARE_SERIALIZE()

    public:

        EventUnstick(){}
    };

    class EventChildActorDestroyed : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventChildActorDestroyed,Event,1818946457);
        DECLARE_SERIALIZE()

    public:

        EventChildActorDestroyed() : m_actor(ITF_INVALID_OBJREF) {}
        EventChildActorDestroyed(ObjectRef _actor) : m_actor(_actor){}
        ObjectRef getActor() const {  return m_actor; }
    private:
        ObjectRef m_actor;

    };

    class EventAnimChanged : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventAnimChanged,Event,153009624);
        DECLARE_SERIALIZE()

    public:

        EventAnimChanged(){}
        EventAnimChanged(StringID _animFriendly) : m_animFriendly(_animFriendly){}
        ITF_INLINE const StringID & getAnimFriendly() const { return m_animFriendly; }
    private:
        StringID m_animFriendly;
    };

    class EventSetBusVolume : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventSetBusVolume,Event,1648440666);
        DECLARE_SERIALIZE()
    public:
        EventSetBusVolume() : m_time(0.0f) {}
        EventSetBusVolume(StringID _bus, Volume _volume, f32 _time) : m_bus(_bus), m_volume(_volume), m_time(_time){}
        ITF_INLINE StringID getBus() const { return m_bus; }
        ITF_INLINE void setBus(StringID _val) { m_bus = _val; }
        ITF_INLINE Volume getVolume() const { return m_volume; }
        ITF_INLINE void setVolume(Volume _val) { m_volume = _val; }
        ITF_INLINE f32 getTime() const { return m_time; }
        ITF_INLINE void setTime(f32 _val) { m_time = _val; }
    private:
        StringID m_bus;
        Volume m_volume;
        f32 m_time;
    };

    class EventSetBusReverb : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventSetBusReverb,Event,98707735);
        DECLARE_SERIALIZE()
    public:
        EventSetBusReverb() : m_changeActivation(bfalse), m_activate(bfalse), m_changePreset(bfalse), m_preset(ReverbPreset_NONE) {}
        ITF_INLINE StringID getBus() const { return m_bus; }
        ITF_INLINE void setBus(StringID _val) { m_bus = _val; }
        ITF_INLINE bbool getActivate() const { return m_activate; }
        ITF_INLINE void setActivate(bbool _val) { m_activate = _val; }
        ITF_INLINE bbool getChangeActivation() const { return m_changeActivation; }
        ITF_INLINE void setChangeActivation(bbool _val) { m_changeActivation = _val; }
        ITF_INLINE bbool getChangePreset() const { return m_changePreset; }
        ITF_INLINE void setChangePreset(bbool _val) { m_changePreset = _val; }
        ITF_INLINE ReverbPreset getPreset() const { return m_preset; }
        ITF_INLINE void setPreset(ReverbPreset _val) { m_preset = _val; }
    private:
        StringID m_bus;
        bbool m_changeActivation;
        bbool m_activate;
        bbool m_changePreset;
        ReverbPreset m_preset;
    };
    class EventSetBusFilter: public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventSetBusFilter,Event,326081700);
        DECLARE_SERIALIZE()
    public:
        EventSetBusFilter() 
            : m_changeFrequency(bfalse)
        , m_frequency(1.0f)
        , m_changeType(bfalse)
        , m_type (FilterType_None)
        , m_changeQ(bfalse)
        , m_Q(1.0f)
        {}
        ITF_INLINE StringID getBus() const { return m_bus; }
        ITF_INLINE void setBus(StringID _val) { m_bus = _val; }
        ITF_INLINE bbool getChangeFrequency() const { return m_changeFrequency; }
        ITF_INLINE void setChangeFrequency(bbool _val) { m_changeFrequency = _val; }
        ITF_INLINE f32 getFrequency() const { return m_frequency; }
        ITF_INLINE void setFrequency(f32 _val) { m_frequency = _val; }
        ITF_INLINE bbool getChangeType() const { return m_changeType; }
        ITF_INLINE void setChangeType(bbool _val) { m_changeType = _val; }
        ITF_INLINE FilterType getType() const { return m_type; }
        ITF_INLINE void setType(FilterType _val) { m_type = _val; }
        ITF_INLINE bbool getChangeQ() const { return m_changeQ; }
        ITF_INLINE void setChangeQ(bbool _val) { m_changeQ = _val; }
        ITF_INLINE f32 getQ() const { return m_Q; }
        ITF_INLINE void setQ(f32 _val) { m_Q = _val; }
    private:
        StringID m_bus;
        bbool m_changeFrequency;
        f32 m_frequency;
        bbool m_changeType;
        FilterType m_type;
        bbool m_changeQ;
        f32 m_Q;
    };

    // meant to be broadcasted; the camera listens and registers/unregisters the sender's children as subjects
    class EventRegisterCameraSubject : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventRegisterCameraSubject,Event,1683052496);
        DECLARE_SERIALIZE()

    public:
        enum Action
        {
            ACTION_NONE,
            ACTION_REGISTER,
            ACTION_UNREGISTER,
            ENUM_FORCE_SIZE_32(Action)
        };

        #define SERIALIZE_ACTION(name,val) \
        SERIALIZE_ENUM_BEGIN(name,val); \
            SERIALIZE_ENUM_VAR(ACTION_NONE); \
            SERIALIZE_ENUM_VAR(ACTION_REGISTER); \
            SERIALIZE_ENUM_VAR(ACTION_UNREGISTER); \
        SERIALIZE_ENUM_END();

        EventRegisterCameraSubject()
            : m_actionActivator(ACTION_NONE)
            , m_actionChildren(ACTION_NONE)
            , m_delay(1.f)
        {}

        ITF_INLINE EventRegisterCameraSubject::Action getActionActivator() const { return m_actionActivator; }
        ITF_INLINE void setActionActivator( const EventRegisterCameraSubject::Action _value ) { m_actionActivator = _value; }

        ITF_INLINE EventRegisterCameraSubject::Action getActionChildren() const { return m_actionChildren; }
        ITF_INLINE void setActionChildren( const EventRegisterCameraSubject::Action _value ) { m_actionChildren = _value; }

        ITF_INLINE f32 getDelay() const { return m_delay; }
        ITF_INLINE void setDelay( f32 _value ) { m_delay = _value; }

        virtual ObjectRef getActivator() const { return m_activator; }
        virtual void setActivator(ObjectRef _val) { m_activator = _val; }

    private:

        Action m_actionActivator;
        Action m_actionChildren;
        ObjectRef m_activator;
        f32 m_delay;
    };



    // meant to be broadcasted; the camera listens and shakes
    class EventCameraShake : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventCameraShake,Event,458805059);
        DECLARE_SERIALIZE()

    public:
        EventCameraShake()
            : m_name()
        {}

        ITF_INLINE const StringID& getName() const { return m_name; }

    private:
        StringID    m_name;
    };


    // meant to be broadcasted; the camera listens and ignores further shakes
    class EventCameraIgnoreShake : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventCameraIgnoreShake,Event,1831643083);
        DECLARE_SERIALIZE()

    public:
        EventCameraIgnoreShake()
            : m_ignoreShake(btrue)
        {}

        ITF_INLINE ITF::bbool getIgnoreShake() const { return m_ignoreShake; }
        ITF_INLINE void setIgnoreShake( const ITF::bbool _value ) { m_ignoreShake = _value; }

    private:
        bbool   m_ignoreShake;
    };



    // meant to be sent to a player actor; will be transmitted to the pad rumble
    // manager with the right pad index
    class EventPadRumbleStart : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventPadRumbleStart,Event,2869984648);
        DECLARE_SERIALIZE()

    public:
        EventPadRumbleStart() : Super(), m_name() {}
        EventPadRumbleStart(const StringID& _name) : m_name(_name) {}

        ITF_INLINE const StringID& getName() const { return m_name; }

    private:
        StringID    m_name;
    };


    // meant to be sent to a player actor; will be transmitted to the pad rumble
    // manager with the right pad index
    class EventPadRumbleStop : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventPadRumbleStop,Event,2252742105);
        DECLARE_SERIALIZE()
    };



    // meant to be broadcasted; the metronome listens and changes BPM
    class EventMetronomeSetBPM : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventMetronomeSetBPM, Event,1145786736);
        DECLARE_SERIALIZE()

    public:
        EventMetronomeSetBPM()
            : m_bpm(120)
        {}

        ITF_INLINE u32 getBPM() const { return m_bpm; }
        ITF_INLINE void setBPM( const u32 _value ) { m_bpm = _value; }

    private:
        u32   m_bpm;
    };



    class EventSetFloatInput : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventSetFloatInput, Event,808064645);
        DECLARE_SERIALIZE()

    public:
        EventSetFloatInput() : m_inputValue(0.f) {}

        ITF_INLINE StringID getInputName() const { return m_inputName; }
        ITF_INLINE void setInputName(StringID _val) { m_inputName = _val; }
        ITF_INLINE f32 getInputValue() const { return m_inputValue; }
        ITF_INLINE void setInputValue(f32 _val) { m_inputValue = _val; }
    private:
        StringID    m_inputName;
        f32         m_inputValue;
    };
    class EventSetUintInput : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventSetUintInput, Event,1614618903);
        DECLARE_SERIALIZE()

    public:

        EventSetUintInput() : m_inputValue(0) {}

        ITF_INLINE StringID getInputName() const { return m_inputName; }
        ITF_INLINE void setInputName(StringID _val) { m_inputName = _val; }
        ITF_INLINE u32 getInputValue() const { return m_inputValue; }
        ITF_INLINE void setInputValue(u32 _val) { m_inputValue = _val; }
    private:
        StringID    m_inputName;
        u32         m_inputValue;
    };

    class EventScaleChanged : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventScaleChanged, Event,3630354374);
        DECLARE_SERIALIZE()

    public:
        EventScaleChanged() {}

    private:

    };

    class EventBossBubonHit : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventBossBubonHit, Event,1018185172);
        DECLARE_SERIALIZE()

    public:
        EventBossBubonHit() {}

    private:

    };


    class EventGeneric : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventGeneric,Event,4063838687);
        DECLARE_SERIALIZE()

    public :
        EventGeneric() {}

        ITF_INLINE StringID getId() const { return m_id; }
        ITF_INLINE void setId( StringID _value ) { m_id = _value; }

    private:

        StringID        m_id;
    };


    class EventOnLink : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventOnLink,Event,1558255516);
        DECLARE_SERIALIZE()

    public :
        EventOnLink()      : m_linked(bfalse)               {}
        ITF_INLINE ObjectRef    getChild() const            { return m_child; }
        ITF_INLINE void         setChild(ObjectRef _val)    { m_child = _val; }
        ITF_INLINE bbool        getLinked() const           { return m_linked; }
        ITF_INLINE void         setLinked(bbool _val)       { m_linked = _val; }

    private:

        ObjectRef   m_child;
        bbool       m_linked;
    };

    class EventLanguageChanged : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventLanguageChanged, Event,1029567175);
        DECLARE_SERIALIZE()

    public:
        EventLanguageChanged()
            : m_oldLanguage(ITF_LANGUAGE_UNKNOWN)
            , m_newLanguage(ITF_LANGUAGE_UNKNOWN)
        {}

        EventLanguageChanged(ITF_LANGUAGE _oldLanguage, ITF_LANGUAGE _newLanguage)
            : m_oldLanguage(_oldLanguage)
            , m_newLanguage(_newLanguage)
        {}

        ITF_INLINE ITF_LANGUAGE getOldLanguage() const { return m_oldLanguage; }
        ITF_INLINE ITF_LANGUAGE getNewLanguage() const { return m_newLanguage; }

    private:
        ITF_LANGUAGE m_oldLanguage;
        ITF_LANGUAGE m_newLanguage;
    };

    class EventSingletonConfigChanged : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventSingletonConfigChanged,Event,32858252);

    public :
        EventSingletonConfigChanged(const String& _absoluteFilename) : m_absoluteFilename(_absoluteFilename) {}
        ITF_INLINE const String& getAbsoluteFilename() const { return m_absoluteFilename; }

    private:
        EventSingletonConfigChanged() {}
        String m_absoluteFilename;
    };

    class EventUndelaySpawn : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventUndelaySpawn,Event,2219542589);

    public :
        EventUndelaySpawn() {}
        ~EventUndelaySpawn() {}
    };

    class EventFluidCollisionPolylineChanged : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventFluidCollisionPolylineChanged,Event,3871666563);

    public:
        EventFluidCollisionPolylineChanged() {}
        ~EventFluidCollisionPolylineChanged() {}
    };

    class EventOnLinkedToWaveGenerator : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventOnLinkedToWaveGenerator,Event,3839654009);

    public:
        EventOnLinkedToWaveGenerator() : m_isLinked(btrue) {}
        EventOnLinkedToWaveGenerator(ActorRef _waveGenerator, bbool _isLinked) {m_isLinked = _isLinked; m_waveGenerator = _waveGenerator;}

        bbool getIsLinked() const {return m_isLinked;}
        ActorRef getWaveGenerator() const {return m_waveGenerator;}

    private:
        bbool m_isLinked;
        ActorRef m_waveGenerator;
    };

    class EventPlayMusic : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventPlayMusic,Event,2305092816);
        DECLARE_SERIALIZE()

    public :
        EventPlayMusic()
            : m_metronomeType(StringID::Invalid)
            , m_nodeName(StringID::Invalid)
            , m_fadeTime(0.2f)
            , m_playOnNext(U32_INVALID)
            , m_stopAndPlay(btrue)
        {
            m_volume.setratio(1.0f);
        }
        ~EventPlayMusic() {}

        ITF_INLINE u32          getMetronomeType() const { return m_metronomeType; }
        ITF_INLINE StringID     getNodeName() const { return m_nodeName; }
        ITF_INLINE f32          getFadeTime() const { return m_fadeTime; }
        ITF_INLINE Volume       getVolume() const { return m_volume; }
        ITF_INLINE const u32    getPlayOnNext() const { return m_playOnNext; }
        ITF_INLINE bbool        getStopAndPlay() const { return m_stopAndPlay; }

        ITF_INLINE void         setMetronomeType(u32 _metronomeType) { m_metronomeType = _metronomeType; }
        ITF_INLINE void         setNodeName(StringID& _nodeName) { m_nodeName = _nodeName; }
        ITF_INLINE void         setFadeTime(f32 _fadeTime) { m_fadeTime = _fadeTime; }
        ITF_INLINE void         setVolume(Volume _volume) { m_volume = _volume; }
        ITF_INLINE void         setPlayOnNext(u32 _playOnNext) { m_playOnNext = _playOnNext; }
        ITF_INLINE void        setStopAndPlay(const bbool _stopAndPlay) { m_stopAndPlay = _stopAndPlay; }

    private:
        u32         m_metronomeType;
        StringID    m_nodeName;
        f32         m_fadeTime;
        Volume      m_volume;
        u32         m_playOnNext;       // define at which time this sound should be synchronized (1 bar = 24)
        bbool       m_stopAndPlay;
    };

    class EventStopMusic : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventStopMusic,Event,2288454954);
        DECLARE_SERIALIZE()

    public :
        EventStopMusic()
            : m_metronomeType(U32_INVALID)
            , m_fadeTime(0.0f)
        {}
        ~EventStopMusic() {}

        ITF_INLINE u32          getMetronomeType() const { return m_metronomeType; }
        ITF_INLINE f32          getFadeTime() const { return m_fadeTime; }

        ITF_INLINE void         setMetronomeType(u32 _metronome) { m_metronomeType = _metronome; }
        ITF_INLINE void         setFadeTime(f32 _time) { m_fadeTime = _time; }

    private:
        u32         m_metronomeType;
        f32         m_fadeTime;
    };

    class EventResetAfterFxAlpha : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventResetAfterFxAlpha,Event,1923263133);
        DECLARE_SERIALIZE()
    };

    class EventShowScoreboard : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventShowScoreboard, Event,4168491256);
        DECLARE_SERIALIZE()

    public :
        EventShowScoreboard()
            : m_display(bfalse)
        {}
        ~EventShowScoreboard() {}

        ITF_INLINE bbool     getDisplay() const { return m_display; }

    private:
        bbool       m_display;
    };

    class EventGoToMainMenu : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventGoToMainMenu, Event,1087093414);
        DECLARE_SERIALIZE()
    public:
        EventGoToMainMenu() {}
    };

    class EventBusMix : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventBusMix, Event,2930162672);
        DECLARE_SERIALIZE()
    public:
        EventBusMix() : m_activate(btrue) {}
        ITF_INLINE bbool getActivate() const { return m_activate; }
        ITF_INLINE void setActivate(ITF::bbool val) { m_activate = val; }
        ITF_INLINE  BusMix & getBusMix()  { return m_busMix; }
    private:
        bbool   m_activate;
        BusMix m_busMix;
    };

    class EventPlayMovie : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventPlayMovie, Event,797811199);
        DECLARE_SERIALIZE()
    public:
        EventPlayMovie(): m_play(btrue) {}
        ITF_INLINE bbool getPlay() const { return m_play; }
        ITF_INLINE void setPlay(bbool val) { m_play = val; }
    private:
        bbool  m_play; 
    };

    class EventActorActiveChanged : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventActorActiveChanged, Event, 323621555);

    public:
        EventActorActiveChanged()
            : m_actorRef(ITF_INVALID_OBJREF), m_active(bfalse) {
        }

        EventActorActiveChanged(Actor* a, bbool active)
            : m_actorRef(a ? a->getRef() : ITF_INVALID_OBJREF), m_active(active) {
        }

        ITF_INLINE Actor* getActor()  const { return m_actorRef.getActor(); }
        ITF_INLINE bbool  isActive()  const { return m_active; }

    private:
        ActorRef m_actorRef;
        bbool    m_active;
    };

    class EventControllerStateChanged : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventControllerStateChanged, Event, 1847293651);

    public:
        EventControllerStateChanged()
            : m_padIndex(0)
            , m_connected(bfalse)
            , m_padType(InputAdapter::Pad_Invalid)
        {}

        EventControllerStateChanged(u32 _padIndex, bbool _connected, InputAdapter::PadType _padType = InputAdapter::Pad_Invalid)
            : m_padIndex(_padIndex)
            , m_connected(_connected)
            , m_padType(_padType)
        {}

        ITF_INLINE u32 getPadIndex() const { return m_padIndex; }
        ITF_INLINE bbool isConnected() const { return m_connected; }
        ITF_INLINE InputAdapter::PadType getPadType() const { return m_padType; }

    private:
        u32 m_padIndex;
        bbool m_connected;
        InputAdapter::PadType m_padType;
    };

} // namespace ITF

#endif //_ITF_EVENTS_H_
