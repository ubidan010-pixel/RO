#ifndef _ITF_GAMEPLAYEVENTS_H_
#define _ITF_GAMEPLAYEVENTS_H_

#ifndef _ITF_EVENTS_H_
#include "engine/events/Events.h"
#endif //_ITF_EVENTS_H_

#ifndef _ITF_GAMEPLAYTYPES_H_
#include "gameplay/GameplayTypes.h"
#endif //_ITF_GAMEPLAYTYPES_H_

#ifndef _ITF_PHYSTYPES_H_
#include "engine/physics/PhysTypes.h"
#endif //_ITF_PHYSTYPES_H_

namespace ITF
{
    class EventHanging : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventHanging,Event,2826410601);
        DECLARE_SERIALIZE()

    public:

        EventHanging()
            : m_actor(ITF_INVALID_OBJREF)
            , m_polyline(ITF_INVALID_OBJREF)
            , m_edgeIndex(U32_INVALID)
            , m_weight(0.f)
            , m_speed(Vec2d::Zero)
            , m_t(0.f)
            , m_characterSize(CHARACTERSIZE_NORMAL)
            , m_isHanging(bfalse)
        {}
        EventHanging( const ObjectRef& _actor, const ObjectRef& _polyLine, u32 _edgeIndex, f32 _weight,
                      const Vec2d& _speed, f32 _t, bbool _hanging, ECharacterSize _size )
            : m_actor(_actor)
            , m_polyline(_polyLine)
            , m_edgeIndex(_edgeIndex)
            , m_weight(_weight)
            , m_speed(_speed)
            , m_t(_t)
            , m_characterSize(_size)
            , m_isHanging(_hanging)
        {}
        virtual ~EventHanging() {}

        const ObjectRef&    getActor() const { return m_actor; }
        const ObjectRef&    getPolyline() const { return m_polyline; }
        u32                 getEdge() const { return m_edgeIndex; }
        f32                 getWeight() const { return m_weight; }
        const Vec2d&        getSpeed() const { return m_speed; }
        bbool               isHanging() const { return m_isHanging; }
        f32                 getT() const { return m_t; }
        ECharacterSize      getCharacterSize() const { return m_characterSize; }

    private:

        ObjectRef       m_actor;
        ObjectRef       m_polyline;
        u32             m_edgeIndex;
        f32             m_weight;
        Vec2d           m_speed;
        f32             m_t;
        ECharacterSize  m_characterSize;
        bbool           m_isHanging;
    };

    class EventHangUpdate : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventHangUpdate,Event,3049416002);
        DECLARE_SERIALIZE()

    public:

        EventHangUpdate()
            : m_actor(ITF_INVALID_OBJREF)
            , m_polyline(ITF_INVALID_OBJREF)
            , m_edgeIndex(U32_INVALID)
            , m_weight(0.f)
            , m_speed(Vec2d::Zero)
            , m_t(0.f)
        {}
        EventHangUpdate( const ObjectRef& _actor, const ObjectRef& _polyLine, u32 _edgeIndex, f32 _weight,
                         const Vec2d& _speed, f32 _t )
            : m_actor(_actor)
            , m_polyline(_polyLine)
            , m_edgeIndex(_edgeIndex)
            , m_weight(_weight)
            , m_speed(_speed)
            , m_t(_t)
        {}
        virtual ~EventHangUpdate() {}

        const ObjectRef&    getActor() const { return m_actor; }
        const ObjectRef&    getPolyline() const { return m_polyline; }
        u32                 getEdge() const { return m_edgeIndex; }
        f32                 getWeight() const { return m_weight; }
        const Vec2d&        getSpeed() const { return m_speed; }
        f32                 getT() const { return m_t; }

    private:

        ObjectRef       m_actor;
        ObjectRef       m_polyline;
        u32             m_edgeIndex;
        f32             m_weight;
        Vec2d           m_speed;
        f32             m_t;
    };

    class EventStickOnPolyline : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventStickOnPolyline,Event,471231076);
        DECLARE_SERIALIZE()

    public:

        EventStickOnPolyline()
            : m_polyline(ITF_INVALID_OBJREF)
            , m_actor(ITF_INVALID_OBJREF)
            , m_edgeIndex(U32_INVALID)
            , m_weight(0.f)
            , m_pos(Vec2d::Zero)
            , m_speed(Vec2d::Zero)
            , m_sticked(bfalse)
        {}
        EventStickOnPolyline( const ObjectRef& _polyLine, const ObjectRef& _actor, u32 _edgeIndex, f32 _weight,
                              const Vec2d& _pos, const Vec2d& _speed, bbool _sticked )
            : m_polyline(_polyLine)
            , m_actor(_actor)
            , m_edgeIndex(_edgeIndex)
            , m_weight(_weight)
            , m_pos(_pos)
            , m_speed(_speed)
            , m_sticked(_sticked)
        {}
        virtual ~EventStickOnPolyline() {}

        const ObjectRef&    getPolyline() const { return m_polyline; }
        const ObjectRef&    getActor() const { return m_actor; }
        u32                 getEdge() const { return m_edgeIndex; }
        f32                 getWeight() const { return m_weight; }
        const Vec2d&        getPos() const { return m_pos; }
        const Vec2d&        getSpeed() const { return m_speed; }
        bbool               isSticked() const { return m_sticked; }

    private:

        ObjectRef       m_polyline;
        ObjectRef       m_actor;
        u32             m_edgeIndex;
        f32             m_weight;
        Vec2d           m_pos;
        Vec2d           m_speed;
        bbool           m_sticked;
    };



    class EventStickOnPolylineChangeEdge : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventStickOnPolylineChangeEdge,Event,4222832239);
        DECLARE_SERIALIZE()

    public:

        EventStickOnPolylineChangeEdge()
            : m_polyline(ITF_INVALID_OBJREF)
            , m_actor(ITF_INVALID_OBJREF)
            , m_edgeIndex(U32_INVALID)
            , m_weight(0.f)
            , m_pos(Vec2d::Zero)
            , m_speed(Vec2d::Zero)
        {}
        EventStickOnPolylineChangeEdge( const ObjectRef& _polyLine, const ObjectRef& _actor, u32 _edgeIndex, f32 _weight,
                                        const Vec2d& _pos, const Vec2d& _speed )
            : m_polyline(_polyLine)
            , m_actor(_actor)
            , m_edgeIndex(_edgeIndex)
            , m_weight(_weight)
            , m_pos(_pos)
            , m_speed(_speed)
        {}
        virtual ~EventStickOnPolylineChangeEdge() {}

        const ObjectRef&    getPolyline() const { return m_polyline; }
        const ObjectRef&    getActor() const { return m_actor; }
        u32                 getEdge() const { return m_edgeIndex; }
        f32                 getWeight() const { return m_weight; }
        const Vec2d&        getPos() const { return m_pos; }
        const Vec2d&        getSpeed() const { return m_speed; }

    private:

        ObjectRef       m_polyline;
        ObjectRef       m_actor;
        u32             m_edgeIndex;
        f32             m_weight;
        Vec2d           m_pos;
        Vec2d           m_speed;
    };



    class EventStickOnPolylineUpdate : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventStickOnPolylineUpdate,Event,2702146949);
        DECLARE_SERIALIZE()

    public:

        EventStickOnPolylineUpdate()
            : m_polyline(ITF_INVALID_OBJREF)
            , m_edgeIndex(U32_INVALID)
            , m_speed(Vec2d::Zero)
        {}
        EventStickOnPolylineUpdate( const ObjectRef& _polyLine, u32 _edgeIndex, const Vec2d& _speed )
            : m_polyline(_polyLine)
            , m_edgeIndex(_edgeIndex)
            , m_speed(_speed)
        {}
        virtual ~EventStickOnPolylineUpdate() {}

        const ObjectRef&    getPolyline() const { return m_polyline; }
        u32                 getEdge() const { return m_edgeIndex; }
        const Vec2d&        getSpeed() const { return m_speed; }

    private:

        ObjectRef       m_polyline;
        u32             m_edgeIndex;
        Vec2d           m_speed;
    };



    class EventBlockedByPolyline : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventBlockedByPolyline,Event,2950236768);
        DECLARE_SERIALIZE()

    public:

        EventBlockedByPolyline()
            : m_polyline(ITF_INVALID_OBJREF)
            , m_edgeIndex(U32_INVALID)
            , m_speed(Vec2d::Zero)
        {}
        EventBlockedByPolyline( const ObjectRef& _polyLine, u32 _edgeIndex, const Vec2d& _speed , const Vec2d& _contact )
            : m_polyline(_polyLine)
            , m_edgeIndex(_edgeIndex)
            , m_speed(_speed)
            , m_contact(_contact)
        {}
        virtual ~EventBlockedByPolyline() {}

        const ObjectRef&    getPolyline() const { return m_polyline; }
        u32                 getEdge() const { return m_edgeIndex; }
        const Vec2d&        getSpeed() const { return m_speed; }
        const Vec2d&        getContact() const { return m_contact; }

    private:

        ObjectRef       m_polyline;
        u32             m_edgeIndex;
        Vec2d           m_speed;
        Vec2d           m_contact;
    };

    class EventPolylineBlockingContact : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventPolylineBlockingContact,Event,1992742979);
        DECLARE_SERIALIZE()

    public:

        EventPolylineBlockingContact()
            : m_polyline(ITF_INVALID_OBJREF)
            , m_edgeIndex(U32_INVALID)
            , m_speed(Vec2d::Zero)
        {}
        EventPolylineBlockingContact( const ObjectRef& _polyLine, u32 _edgeIndex, const Vec2d& _speed , const Vec2d& _contact )
            : m_polyline(_polyLine)
            , m_edgeIndex(_edgeIndex)
            , m_speed(_speed)
            , m_contact(_contact)
        {}
        virtual ~EventPolylineBlockingContact() {}

        const ObjectRef&    getPolyline() const { return m_polyline; }
        u32                 getEdge() const { return m_edgeIndex; }
        const Vec2d&        getSpeed() const { return m_speed; }
        const Vec2d&        getContact() const { return m_contact; }

    private:

        ObjectRef       m_polyline;
        u32             m_edgeIndex;
        Vec2d           m_speed;
        Vec2d           m_contact;
    };



    // sent to the owner of a polyline when walljumping off of it (we may or may not be sticked to that polyline)
    class EventWallJump : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventWallJump,Event,2476933602);
        DECLARE_SERIALIZE()

    public:

        EventWallJump() {}
        virtual ~EventWallJump() {}
    };



    class EventTrail : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventTrail,Event,2898640431);
        DECLARE_SERIALIZE()

    public:

        EventTrail()
            : m_launch(btrue)
            , m_forceNoFade(bfalse)
            , m_useSetColor(bfalse)
            , m_setColor(Color::white())
        {}
        EventTrail( bbool _launch, bbool _forceNoFade = bfalse )
            : m_launch(_launch)
            , m_forceNoFade(_forceNoFade)
            , m_useSetColor(bfalse)
            , m_setColor(Color::white())
        {}

        virtual ~EventTrail() {}

        const bbool     getLaunch() const { return m_launch; }
        const bbool     getForceNoFade() const { return m_forceNoFade; }

        void            setColor( const Color& _color ) { m_useSetColor = btrue; m_setColor = _color; }
        bbool           getUseSetColor() const { return m_useSetColor; }
        const Color&    getSetColor() const { return m_setColor; }

    private:

        Color       m_setColor;
        bbool       m_launch;
        bbool       m_forceNoFade;
        bbool       m_useSetColor;
    };



    class EventActivate : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventActivate,Event,2639596100);
        DECLARE_SERIALIZE()

    public:

        EventActivate()
            : m_activated(bfalse)
        {}
        virtual ~EventActivate() {}

        ITF_INLINE bbool    getActivated() const { return m_activated; }
        ITF_INLINE void     setActivated( bbool _value ) { m_activated = _value; }

    private:

        bbool               m_activated;
    };



    class EventShow : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventShow,Event,111532112);
        DECLARE_SERIALIZE()
        typedef void (*OnEnd)();
    public:

        EventShow()
            : m_alpha(1.f)
            , m_transitionTime(0.f)
            , m_overrideColor(bfalse)
            , m_newColor(Color::white())
            , m_pauseOnEnd(bfalse)
            , m_destroyOnEnd(bfalse)
            ,m_onEndCallback(NULL)
        {}
        EventShow( f32 _alpha )
            : m_alpha(_alpha)
            , m_transitionTime(0.f)
            , m_overrideColor(bfalse)
            , m_newColor(Color::white())
            , m_pauseOnEnd(bfalse)
            , m_destroyOnEnd(bfalse)
            ,m_onEndCallback(NULL)
        {}
        EventShow( f32 _alpha, f32 _transitionTime )
            : m_alpha(_alpha)
            , m_transitionTime(_transitionTime)
            , m_overrideColor(bfalse)
            , m_newColor(Color::white())
            , m_pauseOnEnd(bfalse)
            , m_destroyOnEnd(bfalse)
            ,m_onEndCallback(NULL)
        {}
        EventShow( f32 _alpha, f32 _transitionTime,OnEnd _callback )
           : m_alpha(_alpha)
           , m_transitionTime(_transitionTime)
           , m_overrideColor(bfalse)
           , m_newColor(Color::white())
           , m_pauseOnEnd(bfalse)
           , m_destroyOnEnd(bfalse)
           ,m_onEndCallback(_callback)
        {}
        virtual ~EventShow() {}

        const f32       getAlpha() const { return m_alpha; }
        const f32       getTransitionTime() const { return m_transitionTime; }
        const bbool     getIsOverrideColor() const { return m_overrideColor; }
        const Color&    getOverrideColor() const { return m_newColor; }
        const OnEnd&    getOnEndCallback() const { return m_onEndCallback; }

        void            setOverrideColor( const Color& _color ) { m_overrideColor = btrue; m_newColor = _color; }
        ITF_INLINE bbool getDestroyOnEnd() const { return m_destroyOnEnd; }
        ITF_INLINE void setDestroyOnEnd(bbool _val) { m_destroyOnEnd = _val; }
        ITF_INLINE bbool getPauseOnEnd() const { return m_pauseOnEnd; }
        ITF_INLINE void setPauseOnEnd(bbool _val) { m_pauseOnEnd = _val; }

    private:

        f32         m_alpha;
        f32         m_transitionTime;
        bbool       m_overrideColor;
        Color       m_newColor;
        bbool       m_destroyOnEnd;
        bbool       m_pauseOnEnd;
        OnEnd       m_onEndCallback;
    };

    class EventRopeSwingImpulse : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventRopeSwingImpulse,Event,807862862);
        DECLARE_SERIALIZE()

    public:

        EventRopeSwingImpulse()
            : m_polyline(ITF_INVALID_OBJREF)
            , m_edge(U32_INVALID)
            , m_t(0.f)
            , m_weight(0.f)
            , m_impulse(Vec2d::Zero)
            , m_characterSize(CHARACTERSIZE_NORMAL)
        {}
        EventRopeSwingImpulse( const ObjectRef& _sender, const ObjectRef& _ref, u32 _edgeIndex, f32 _t, f32 _weight, const Vec2d& _impulse, ECharacterSize _characterSize )
            : Super(_sender)
            , m_polyline(_ref)
            , m_edge(_edgeIndex)
            , m_t(_t)
            , m_weight(_weight)
            , m_impulse(_impulse)
            , m_characterSize(_characterSize)
        {}
        virtual ~EventRopeSwingImpulse() {}

        const ObjectRef&    getPolyline() const { return m_polyline; }
        u32                 getEdgeIndex() const { return m_edge; }
        f32                 getT() const { return m_t; }
        f32                 getWeight() const { return m_weight; }
        Vec2d               getImpulse() const { return m_impulse; }
        ECharacterSize      getCharacterSize() const { return m_characterSize; }

    private:

        ObjectRef           m_polyline;
        u32                 m_edge;
        f32                 m_t;
        f32                 m_weight;
        Vec2d               m_impulse;
        ECharacterSize      m_characterSize;
    };

    class EventSwiming : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventSwiming,Event,2469288979);
        DECLARE_SERIALIZE()

    public:

        EventSwiming()
            : m_actor(ITF_INVALID_OBJREF)
            , m_swiming(0)
            , m_radius(0)
            , m_waterMultiplier(1.f)
            , m_weight(1.f)
            , m_queryPosition(bfalse)
        {}
        EventSwiming(ObjectRef _actor, bbool _swiming, Vec2d _speed, f32 _radius, f32 _waterMultiplier, f32 _weight, bbool _queryPosition )
            : m_actor(_actor)
            , m_swiming(_swiming)
            , m_speed(_speed)
            , m_radius(_radius)
            , m_waterMultiplier(_waterMultiplier)
            , m_weight(_weight)
            , m_queryPosition(_queryPosition)
        {}
        virtual ~EventSwiming() {}

        ObjectRef                   getActor() const { return m_actor; }
        void                        setActor( ObjectRef _actor ) { m_actor = _actor; }

        bbool                       isSwiming() const { return m_swiming; }
        void                        setSwiming( bbool _swiming ) { m_swiming = _swiming; }

        Vec2d                       getSpeed() const { return m_speed; }
        void                        setSpeed( Vec2d _speed ) { m_speed = _speed; }

        f32                         getRadius() const { return m_radius; }
        void                        setRadius( f32 _radius ) { m_radius = _radius; }

        f32                         getWaterMultiplier() const { return m_waterMultiplier; }
        void                        setWaterMultiplier( f32 _mult ) { m_waterMultiplier = _mult; }

        f32                         getWeight() const { return m_weight; }
        void                        setWeight( f32 _weight ) { m_weight = _weight; }

        bbool                       getQueryPosition() const { return m_queryPosition; }
        void                        setQueryPosition( bbool _queryPosition ) { m_queryPosition = _queryPosition; }

    private:
        ObjectRef                           m_actor;
        bbool                               m_swiming;
        Vec2d                               m_speed;
        f32                                 m_radius;
        f32                                 m_waterMultiplier;
        f32                                 m_weight;
        bbool                               m_queryPosition;
    };


    class EventSwimingPlouf : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventSwimingPlouf,Event,3882552276);
        DECLARE_SERIALIZE()

    public:

        EventSwimingPlouf()
            : m_alteration(0)
            , m_radius(1.f)
            , m_isRandom(bfalse)
        {}
        EventSwimingPlouf(Vec2d _pos, f32 _alteration, f32 _radius, bbool _isRandom)
            : m_pos(_pos)
            , m_alteration(_alteration)
            , m_radius(1.f)
            , m_isRandom(_isRandom)
        {}
        virtual ~EventSwimingPlouf() {}

        Vec2d                       getPos() const { return m_pos; }
        void                        setPos( const Vec2d & _pos ) { m_pos = _pos; }

        f32                         getAlteration() const { return m_alteration; }
        void                        setAlteration( f32 _alteration ) { m_alteration = _alteration; }

        f32                         getRadius() const { return m_radius; }
        void                        setRadius( f32 _radius ) { m_radius = _radius; }

        bbool                       isRandom() const { return m_isRandom; }
        void                        setRandom( bbool _random ) { m_isRandom = _random; }

    private:
        Vec2d                               m_pos;
        f32                                 m_alteration;
        f32                                 m_radius;
        bbool                               m_isRandom;
    };

    class EventCollide : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventCollide,Event,3122529699);
        DECLARE_SERIALIZE()

    public:

        EventCollide()
            : m_pos(Vec2d::Zero)
            , m_angle(0.f)
            , m_shape(NULL)
            , m_collided(bfalse)
        {}
        virtual ~EventCollide() {}

        Vec2d                       getPos() const { return m_pos; }
        void                        setPos( const Vec2d & _pos ) { m_pos = _pos; }

        f32                         getAngle() const { return m_angle; }
        void                        setAngle( f32 _angle ) { m_angle = _angle; }

        const class PhysShape*      getShape() const { return m_shape; }
        void                        setShape( const class PhysShape* _shape ) { m_shape = _shape; }

        bbool                       isCollide() { return m_collided; }
        void                        setCollide(bbool _collided) { m_collided = _collided; }

    private:

        Vec2d                               m_pos;
        f32                                 m_angle;
        const class PhysShape*              m_shape;            // The shape of the stim. This shape must be kept in memory by the actor
        bbool                               m_collided;         // The position where we put the shape
    };

    class EventQueryPhysShape : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventQueryPhysShape,Event,2080285933);
        DECLARE_SERIALIZE()

    public:

        EventQueryPhysShape()
            : m_physShape(NULL)
            , m_angle(0)
            , m_position(0,0)
        {}
        virtual ~EventQueryPhysShape() {}

        const PhysShape *getPhysShape() const {return m_physShape;}
        void setPhysShape( const PhysShape *_shape) {m_physShape = _shape;}

        const Vec2d &getPosition() const {return m_position;}
        void setPosition(const Vec2d &_pos) {m_position = _pos;}

        f32 getAngle() const {return m_angle;}
        void setAngle(f32 _angle) {m_angle = _angle;}
    private:
        const PhysShape *m_physShape;
        Vec2d m_position;
        f32 m_angle;
    };




    enum CharacterInteractionType
    {
        CharacterInteractionType_None       = 0,
        CharacterInteractionType_Crush      = 0x01,
        CharacterInteractionType_Support    = 0x02,
        CharacterInteractionType_Grab       = 0x04,
        CharacterInteractionType_Other      = 0x08,
        ENUM_FORCE_SIZE_32(CharacterInteractionType)
    };

    class EventInteractionQuery : public Event
    {
	    DECLARE_OBJECT_CHILD_RTTI(EventInteractionQuery,Event,209600608);
        DECLARE_SERIALIZE()

    public:

	    EventInteractionQuery()
            : m_interactionType(CharacterInteractionType_None)
            , m_contact(NULL)
        {}
        virtual ~EventInteractionQuery() {}

	    CharacterInteractionType               getInteraction() const { return m_interactionType; }
	    void                setInteraction(CharacterInteractionType value) { m_interactionType = value; }
        ITF_INLINE const SCollidableContact * getContact() const { return m_contact; }
        ITF_INLINE void setContact(const SCollidableContact * _val) { m_contact = _val; }

    private:

	    CharacterInteractionType               m_interactionType;
        const SCollidableContact *             m_contact;
    };



    class EventCrushed : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventCrushed,Event,3831882623);
        DECLARE_SERIALIZE()

    public:

        EventCrushed() : m_characterSize(CHARACTERSIZE_NORMAL), m_bounce(btrue) { }
        EventCrushed( ObjectRef _instigator ) : Event(_instigator), m_characterSize(CHARACTERSIZE_NORMAL), m_bounce(btrue) {}
        virtual ~EventCrushed() {}

        ITF_INLINE const Vec2d& getDirection() const { return m_direction; }
        ITF_INLINE void setDirection( const Vec2d& _value ) { m_direction = _value; }

        ITF_INLINE const Vec3d& getFxPos() const { return m_fxPos; }
        ITF_INLINE void setFxPos( const Vec3d& _value ) { m_fxPos = _value; }

        ITF_INLINE bbool getBounce() const { return m_bounce; }
        ITF_INLINE void setBounce( bbool _value ) { m_bounce = _value; }

        ITF_INLINE ECharacterSize getCharacterSize() const { return m_characterSize; }
        ITF_INLINE void setCharacterSize( ECharacterSize _value ) { m_characterSize = _value; }

    private:

        ECharacterSize          m_characterSize;
        Vec2d                   m_direction;
        Vec3d                   m_fxPos;
        bbool                   m_bounce;
    };



    class EventQueryPhysicsData : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventQueryPhysicsData,Event,2528864682);
        DECLARE_SERIALIZE()

    public:

        EventQueryPhysicsData()
            : m_radius(0.f)
            , m_speed(Vec2d::Zero)
            , m_worldSpeed(Vec2d::Zero)
            , m_weight(0.f)
            , m_polyline(ITF_INVALID_OBJREF)
            , m_edge(U32_INVALID)
        {}
        virtual ~EventQueryPhysicsData() {}

        f32                 getRadius() const { return m_radius; }
        const Vec2d&        getSpeed() const { return m_speed; }
        const Vec2d&        getWorldSpeed() const { return m_worldSpeed; }
        f32                 getWeight() const { return m_weight; }
        const ObjectRef&    getStickyPolyline() const {return m_polyline;}
        u32                 getStickyEdgePolyline() const {return m_edge;}
        const Vec2d &       getCurrentGroundDir() const { return m_currentGroundDir; }

        void                setRadius( f32 _radius ) { m_radius = _radius; }
        void                setSpeed( const Vec2d& _speed ) { m_speed = _speed; }
        void                setWorldSpeed( const Vec2d& _speed ) { m_worldSpeed = _speed; }
        void                setWeight( f32 _weight ) { m_weight = _weight; }
        void	            setStickedPolyline(const ObjectRef& _polyline){  m_polyline = _polyline;}
        void	            setStickedEdgePolyline(u32 _edge){  m_edge = _edge;}
        void                setCurrentGroundDir(const Vec2d & _dir) { m_currentGroundDir = _dir; }

    private:

        f32                 m_radius;
        Vec2d               m_speed;
        Vec2d               m_worldSpeed;
        f32                 m_weight;

        ObjectRef           m_polyline;
        u32                 m_edge;
        Vec2d               m_currentGroundDir;
    };



    class EventQuerySnapData : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventQuerySnapData,Event,747808288);
        DECLARE_SERIALIZE()

    public:

        EventQuerySnapData() : m_pos(Vec3d::Zero), m_angle(0.f), m_lookRight(bfalse) {}
        virtual ~EventQuerySnapData() {}

        f32                 getAngle() const { return m_angle; }
        const Vec3d&        getPos() const { return m_pos; }
        bbool               getLookRight() const { return m_lookRight; }

        void                setAngle( f32 _angle ) { m_angle = _angle; }
        void                setPos( const Vec3d& _pos ) { m_pos = _pos; }
        void                setLookRight( bbool _lookRight ) { m_lookRight = _lookRight; }

    private:

        f32                 m_angle;
        Vec3d               m_pos;
        bbool               m_lookRight;
    };



    class EventDetach : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventDetach, Event,889341216);
        DECLARE_SERIALIZE()

    public:
        EventDetach() : m_polyline(ITF_INVALID_OBJREF), m_edge(U32_INVALID) {}
        EventDetach( ObjectRef _polyline, u32 _edge ) : m_polyline(_polyline), m_edge(_edge) {}
        virtual ~EventDetach() {}

        ObjectRef           getPolyLine() const { return m_polyline; }
        u32                 getEdge() const { return m_edge; }

    private:

        ObjectRef           m_polyline;
        u32                 m_edge;
    };



    class EventQueryFaction : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventQueryFaction, Event,3678638658);
        DECLARE_SERIALIZE()

    public:

        EventQueryFaction()
            : m_faction(FACTION_UNKNOWN)
        {}
        virtual ~EventQueryFaction() {}

        Faction     getFaction() { return m_faction; }
        void        setFaction(Faction _faction) { m_faction = _faction; }

    private:

        Faction     m_faction;
    };



    // sent to know if an actor is dead (could be a player, an AI, a spawner, etc)
    class EventQueryIsDead : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventQueryIsDead, Event,2482198443);

    public:

        EventQueryIsDead()
            : m_isDead(bfalse)
        {}
        virtual ~EventQueryIsDead() {}

        ITF_INLINE bbool getIsDead() const { return m_isDead; }
        ITF_INLINE void setIsDead( bbool _value ) { m_isDead = _value; }

    private:

        bbool       m_isDead;
    };


    // sent to know if an actor is disabled
    class EventQueryIsDisabled : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventQueryIsDisabled, Event,1476509545);

    public:

        EventQueryIsDisabled()
            : m_isDisabled(bfalse)
        {}
        virtual ~EventQueryIsDisabled() {}

        ITF_INLINE bbool getIsDisabled() const { return m_isDisabled; }
        ITF_INLINE void setIsDisabled( bbool _value ) { m_isDisabled = _value; }

    private:

        bbool       m_isDisabled;
    };


    // sent to know if an actor is caught (By waterhands by example)
    class EventQueryIsCaught : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventQueryIsCaught, Event,1045544516);

    public:

        EventQueryIsCaught()
        : m_isCaught(bfalse)
        {}
        virtual ~EventQueryIsCaught() {}

        ITF_INLINE bbool getIsCaught() const { return m_isCaught; }
        ITF_INLINE void setIsCaught( bbool _value ) { m_isCaught = _value; }

    private:

        bbool       m_isCaught;
    };


    // sent back to an attacker after a stim he sent touched something
    class EventHitSuccessful : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventHitSuccessful, Event,818883013);
        DECLARE_SERIALIZE()

    public:

        EventHitSuccessful():m_isDead(bfalse), m_hasBeenDamaged(btrue) {}
        virtual ~EventHitSuccessful() {}
        ITF_INLINE  void setIsDead(bbool _isDead)                   { m_isDead = _isDead;}
        ITF_INLINE  void setHasBeenDamaged(bbool _damaged)          { m_hasBeenDamaged = _damaged;}
        ITF_INLINE  bbool getIsDead()                       const   { return m_isDead; }
        ITF_INLINE  bbool getHasBeenDamaged()               const   { return m_hasBeenDamaged; }
    private:
        bbool       m_isDead;
        bbool       m_hasBeenDamaged;
    };

    class EventForceOnActor : public Event
    {
    public:
        DECLARE_OBJECT_CHILD_RTTI(EventForceOnActor, Event,267634505);
        DECLARE_SERIALIZE()

        EventForceOnActor();
        EventForceOnActor( ObjectRef _actor, const Vec2d _pos, const Vec2d& _force );
        ~EventForceOnActor() {}

        ObjectRef               getActor() const { return m_actor; }
        const Vec2d&            getPos() const { return m_pos; }
        const Vec2d&            getForce() const { return m_force; }

    private:

        ObjectRef               m_actor;                // The actor who is receiving the force
        Vec2d                   m_pos;                  // The position where the force is applied
        Vec2d                   m_force;                // The force applied
    };

    class EventForceTransfer : public Event
    {
    public:
        DECLARE_OBJECT_CHILD_RTTI(EventForceTransfer, Event,1043808691);
        DECLARE_SERIALIZE()

        EventForceTransfer();
        EventForceTransfer( const StringID& _polyID, const StringID& _polyPointID,
                            const Vec2d& _pos, const Vec2d& _force );
        ~EventForceTransfer() {}

        const StringID&         getPolyID() const { return m_polyID; }
        const StringID&         getPolyPointID() const { return m_polyPointID; }
        const Vec2d&            getPos() const { return m_pos; }
        const Vec2d&            getForce() const { return m_force; }

    private:

        StringID                m_polyID;               // The polyline connection between the two actors
        StringID                m_polyPointID;          // The polyline point connection between the two actors
        Vec2d                   m_pos;                  // The position where the force is applied
        Vec2d                   m_force;                // The force applied
    };

    class EventAnimUpdated : public Event
    {
    public:
        DECLARE_OBJECT_CHILD_RTTI(EventAnimUpdated, Event,2246315305);
        DECLARE_SERIALIZE()

        EventAnimUpdated() : m_dt(0.f) {}
        EventAnimUpdated( f32 _dt ) : m_dt(_dt) {}
        ~EventAnimUpdated() {}

        f32                     getDt() const { return m_dt; }

    private:

        f32                     m_dt;
    };

    class EventPhysicsUpdated : public Event
    {
    public:
        DECLARE_OBJECT_CHILD_RTTI(EventPhysicsUpdated, Event,2632338894);
        DECLARE_SERIALIZE()

        EventPhysicsUpdated() {}
        ~EventPhysicsUpdated() {}

    private:

    };

    class EventPlayerActivationChanged : public Event
    {
    public:
        DECLARE_OBJECT_CHILD_RTTI(EventPlayerActivationChanged, Event,1593682208);

        EventPlayerActivationChanged(Player* _player, bbool _startDead, bbool _startLevel)
            : m_player(_player)
            , m_startDead(_startDead)
            , m_startLevel(_startLevel)
        {}
        EventPlayerActivationChanged()
            : m_player(NULL)
            , m_startDead(btrue)
            , m_startLevel(bfalse)
        {}
        ~EventPlayerActivationChanged() {}

        Player*     getPlayer() const { return m_player; }
        bbool       getStartDead() const { return m_startDead; }
        bbool       getStartLevel() const { return m_startLevel; }

    private:
        Player*     m_player;
        bbool       m_startDead;
        bbool       m_startLevel;
    };

    class EventPlayerIndexChange : public Event
    {
    public:
        DECLARE_OBJECT_CHILD_RTTI(EventPlayerIndexChange, Event,2964720152);

        EventPlayerIndexChange() : m_newIndex(U32_INVALID) {}
        EventPlayerIndexChange( u32 _newIndex ) : m_newIndex(_newIndex) {}
        ~EventPlayerIndexChange() {}

        u32         getNewIndex() const { return m_newIndex; }

    private:

        u32         m_newIndex;
    };

    // sent when the current game mode changes, eg between platformer and shooter
    class EventPlayerModeChanged : public EventTrigger
    {
        DECLARE_OBJECT_CHILD_RTTI(EventPlayerModeChanged, EventTrigger,1336647275);
        DECLARE_SERIALIZE()

    public:

        EventPlayerModeChanged()
            : m_mode(GAMEMODE_UNKNOWN)
        {}
        ~EventPlayerModeChanged() {}

        GameMode            getMode() const { return m_mode; }
        void                setMode(u32 _value) { m_mode = _value; }

    private:

        GameMode            m_mode;
    };


    // sent when the current game mode changes, eg between platformer and shooter
    class EventPlayerModeSet : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventPlayerModeSet, Event,722903700);
        DECLARE_SERIALIZE()

    public:

        EventPlayerModeSet()
            : m_mode(GAMEMODE_UNKNOWN)
        {}
        ~EventPlayerModeSet() {}

        GameMode            getMode() const { return m_mode; }
        void                setMode(u32 _value) { m_mode = _value; }

    private:

        GameMode            m_mode;
    };



    class EventMapLoaded : public Event
    {
    public:
        DECLARE_OBJECT_CHILD_RTTI(EventMapLoaded, Event,2286353176);

        EventMapLoaded() {}
        ~EventMapLoaded() {}
    };



    class EventCheckpointReached : public Event
    {
    public:
        DECLARE_OBJECT_CHILD_RTTI(EventCheckpointReached, Event,2346783043);

        EventCheckpointReached() {}
        ~EventCheckpointReached() {}
    };

    class EventPreCheckpointSave : public Event
    {
    public:
        DECLARE_OBJECT_CHILD_RTTI(EventPreCheckpointSave,Event,3321202665);
        EventPreCheckpointSave() {}
        ~EventPreCheckpointSave() {}
    };


    class EventRevertToLastCheckpoint : public Event
    {
    public:
        DECLARE_OBJECT_CHILD_RTTI(EventRevertToLastCheckpoint, Event,3654122082);

        EventRevertToLastCheckpoint() {}
        ~EventRevertToLastCheckpoint() {}
    };



    class EventPlayerHpChanged : public Event
    {
    public:
        DECLARE_OBJECT_CHILD_RTTI(EventPlayerHpChanged, Event,1555193400);

        EventPlayerHpChanged()
            : m_player(NULL)
            , m_previousHP(U32_INVALID)
            , m_justDied(bfalse)
            , m_justRevived(bfalse)
        {}
        ~EventPlayerHpChanged() {}

        Player*     getPlayer() const { return m_player; }
        void        setPlayer(Player* _value) { m_player = _value; }

        u32         getPreviousHP() const { return m_previousHP; }
        void        setPreviousHP(u32 _value) { m_previousHP = _value; }

        bbool       getJustDied() const { return m_justDied; }
        void        setJustDied(bbool _value) { m_justDied = _value; }

        bbool       getJustRevived() const { return m_justRevived; }
        void        setJustRevived(bbool _value) { m_justRevived = _value; }

    private:
        Player*     m_player;
        u32         m_previousHP;
        bbool       m_justDied;
        bbool       m_justRevived;
    };

    class EventPushForce : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventPushForce, Event,405779985);
        DECLARE_SERIALIZE()

    public:

        EventPushForce() : m_force(Vec2d::Zero) {}
        EventPushForce( const Vec2d& _force ) : m_force(_force) {}

        const Vec2d& getForce() const { return m_force; }

    private:

        Vec2d       m_force;

    };


    // used to setup an actor that belongs to a player (eg for bullets)
    class EventSetPlayer : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventSetPlayer, Event,4056921472);

    public:

        EventSetPlayer()
            : m_player(NULL)
        {}
        EventSetPlayer(Player* _player)
            : m_player(_player)
        {}

        ITF_INLINE Player*  getPlayer() const { return m_player; }

    private:

        Player*     m_player;
    };

    class EventLockPlayers : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventLockPlayers, Event,359572052);
        DECLARE_SERIALIZE()

    public:

        EventLockPlayers()
            : m_lock(btrue)
            , m_stopPlayers(bfalse)
            , m_stopInputs(btrue)
            , m_forceRevive(bfalse)
        {}
        EventLockPlayers(bbool _lock)
            : m_lock(_lock)
            , m_stopPlayers(bfalse)
            , m_stopInputs(btrue)
            , m_forceRevive(bfalse)
        {}
        ~EventLockPlayers() {}

        ITF_INLINE bbool getLock() const { return m_lock; }
        ITF_INLINE void  setLock(bbool _lock) { m_lock = _lock; }

        ITF_INLINE bbool getStopPlayers() const { return m_stopPlayers; }
        ITF_INLINE void  setStopPlayers(bbool _stopPlayers) { m_stopPlayers = _stopPlayers; }

        ITF_INLINE bbool getStopInputs() const { return m_stopInputs; }
        ITF_INLINE void  setStopInputs(bbool _stopInputs) { m_stopInputs = _stopInputs; }

        ITF_INLINE bbool getForceRevive() const { return m_forceRevive; }
        ITF_INLINE void  setForceRevive(bbool _revive) { m_forceRevive = _revive; }

    private:
        bbool       m_lock;
        bbool       m_stopPlayers;
        bbool       m_stopInputs;
        bbool       m_forceRevive;
    };



    class EventZoneTrigger : public EventTrigger
    {
        DECLARE_OBJECT_CHILD_RTTI(EventZoneTrigger, EventTrigger,173270600);
        DECLARE_SERIALIZE()

    public:

        EventZoneTrigger()
            : m_radius(1.f)
        {}
        ~EventZoneTrigger() {}

        ITF_INLINE StringID getZoneID() const { return m_zoneID; }
        ITF_INLINE f32      getRadius() const { return m_radius; }

    private:

        StringID    m_zoneID;
        f32         m_radius;
    };

    class EventQueryCanStickOrCollide : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventQueryCanStickOrCollide, Event,4105599404);
    public:
        EventQueryCanStickOrCollide() : m_canStick(btrue), m_canCollide(btrue) {}
        bbool canStick() const  { return m_canStick; }
        void setCanStick(bbool _canStick) { m_canStick = _canStick; }
        bbool canCollide() const  { return m_canCollide; }
        void setCanCollide(bbool _canCollide) { m_canCollide = _canCollide; }
    private:
        bbool m_canStick;
        bbool m_canCollide;

    };

    class EventSetDirection : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventSetDirection,Event,187560938);
        DECLARE_SERIALIZE()

    public:
        EventSetDirection() : m_direction(Vec2d::Zero)
        {
        }
        EventSetDirection(const Vec2d & _direction)
        : Super(), m_direction(_direction)
        {}
        ITF_INLINE Vec2d getDirection() const { return m_direction; }
        ITF_INLINE void setDirection(const Vec2d& _val) { m_direction = _val; }
    private:
        Vec2d m_direction;
    };

    class EventSetTarget : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventSetTarget,Event,4067373371);
        DECLARE_SERIALIZE()

    public:
        EventSetTarget() : m_target(ITF_INVALID_OBJREF)
        {
        }
        EventSetTarget(ObjectRef _target, Vec2d _offset = Vec2d::Zero)
            : Super(), m_target(_target), m_offset(_offset)
        {}
        ITF_INLINE ObjectRef getTarget() const { return m_target; }
        ITF_INLINE void setTarget(ObjectRef _val) { m_target = _val; }
        ITF_INLINE Vec2d getOffset() const { return m_offset; }
        ITF_INLINE void setOffset(Vec2d _val) { m_offset = _val; }
    private:

        ObjectRef m_target;
        Vec2d m_offset;
    };

    class EventSetTargetPos : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventSetTargetPos,Event,2424671732);

    public:
        EventSetTargetPos() : Super(), m_targetPos(Vec2d::Zero) {}
        EventSetTargetPos(Vec2d _targetPos) : Super(), m_targetPos(_targetPos) {}

        ITF_INLINE const Vec2d& getTargetPos() const { return m_targetPos; }

    private:
        Vec2d m_targetPos;
    };


    // sent when an AI dies; its components can decide to pause etc...
    class EventDead : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventDead,Event,634720568);
        DECLARE_SERIALIZE()

    public:
        EventDead() {}
    };


    class EventStopPlayer : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventStopPlayer,Event,3369505273);
        DECLARE_SERIALIZE()

    public:
        EventStopPlayer() : m_stop(bfalse) {}
        EventStopPlayer( bbool _stop ) : m_stop(_stop) {}

        bbool                   isStop() const { return m_stop; }

    private:

        bbool                   m_stop;
    };

    class EventBouncedOnPolyline : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventBouncedOnPolyline,Event,1421560006);
        DECLARE_SERIALIZE()

    public:
        EventBouncedOnPolyline();
        EventBouncedOnPolyline( ObjectRef _poly, u32 _edgeIndex, const Vec2d& _dir );

        const ObjectRef&            getPoly() const { return m_polyRef; }
        u32                         getEdgeIndex() const { return m_edgeIndex; }
        const Vec2d&                getDir() const { return m_dir; }

    private:

        ObjectRef                   m_polyRef;
        u32                         m_edgeIndex;
        Vec2d                       m_dir;
    };

    class EventQueryBlocksHits : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventQueryBlocksHits, Event,133722380);
        DECLARE_SERIALIZE()

    public:
        EventQueryBlocksHits()
            : m_canBlockHits(bfalse)
            , m_contacts(NULL)
            , m_currentPunchHitLevel(0)
        {}

        ITF_INLINE bbool getCanBlockHits() const { return m_canBlockHits; }
        ITF_INLINE void setCanBlockHits(bbool _val) { m_canBlockHits = _val; }

        ITF_INLINE u32 getCurrentPunchHitLevel() const { return m_currentPunchHitLevel; }
        ITF_INLINE void setCurrentPunchHitLevel(u32 _val) { m_currentPunchHitLevel = _val; }

        ITF_INLINE const PhysContactsContainer* getContacts() const { return m_contacts; }
        ITF_INLINE void setContacts( const PhysContactsContainer* _value ) { m_contacts = _value; }

    private:
        bbool m_canBlockHits;
        u32   m_currentPunchHitLevel;
        const PhysContactsContainer* m_contacts;
    };

    class EventQueryCanReceiveHit : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventQueryCanReceiveHit, Event,1176328316);
        DECLARE_SERIALIZE()

    public:
        EventQueryCanReceiveHit()
            : m_canReceiveHit(btrue)
            , m_hitStim(NULL)
        {}

        ITF_INLINE bbool        getCanReceiveHit() const { return m_canReceiveHit; }
        ITF_INLINE void         setCanReceiveHit(bbool _canReceiveHit) { m_canReceiveHit = _canReceiveHit; }
        ITF_INLINE class HitStim *    getStim () const { return m_hitStim; }
        ITF_INLINE void         setStim(HitStim * _stim) { m_hitStim = _stim; }
    private:
        bbool m_canReceiveHit;
        class HitStim * m_hitStim;
    };

	class EventChangeBehaviourQuery : public Event
	{
		DECLARE_OBJECT_CHILD_RTTI(EventChangeBehaviourQuery, Event,1120766952);
		DECLARE_SERIALIZE()
	public:
		EventChangeBehaviourQuery()
		: m_wantedBehaviour(NULL)
        , m_forceRestart( bfalse )
        , m_changeDone( bfalse )
		{ m_wantedBehaviourName.invalidate();}
		ITF_INLINE void					setWantedBehaviour( class AIBehavior * _bhv )	{ m_wantedBehaviour = _bhv; }
        ITF_INLINE void					setWantedBehaviourName( const StringID & _bhvName )	{ m_wantedBehaviourName = _bhvName; }
		ITF_INLINE class AIBehavior *   getWantedBehaviour() const						{ return m_wantedBehaviour; }
        ITF_INLINE const StringID &     getWantedBehaviourName() const				    { return m_wantedBehaviourName; }
        ITF_INLINE void					setForceRestart( bbool _forceRestart )	        { m_forceRestart = _forceRestart; }
        ITF_INLINE bbool                getForceRestart() const						    { return m_forceRestart; }
        ITF_INLINE void					setChangeDone( bbool _done )	                { m_changeDone = _done; }
        ITF_INLINE bbool                isChangeDone() const						    { return m_changeDone; }
	private:
		class AIBehavior *  m_wantedBehaviour;
        StringID            m_wantedBehaviourName;
        bbool               m_forceRestart;
        bbool               m_changeDone;
	};

    class EventChangeExternBhvValidationQuery : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventChangeExternBhvValidationQuery, Event,1163849318);
        DECLARE_SERIALIZE()
    public:
        EventChangeExternBhvValidationQuery(): m_changeValidated(btrue), m_bhvFinished( bfalse ), m_bhvStopped( bfalse )                            { m_wantedBehaviourName.invalidate();}
        EventChangeExternBhvValidationQuery( const StringID & _bhvName ) : m_changeValidated(btrue),m_bhvFinished(bfalse), m_bhvStopped( bfalse )   { m_wantedBehaviourName = _bhvName; }
        ITF_INLINE void					setWantedBehaviourName( const StringID & _bhvName )	    { m_wantedBehaviourName = _bhvName; }
        ITF_INLINE const StringID &     getWantedBehaviourName() const				            { return m_wantedBehaviourName; }
        ITF_INLINE const bbool &        isChangeValidated() const				                { return m_changeValidated; }
        ITF_INLINE void                 setChangeValidated( bbool _validated )                  { m_changeValidated = _validated; }
        ITF_INLINE const bbool &        isBhvFinished() const				                    { return m_bhvFinished; }
        ITF_INLINE void                 setBhvFinished( bbool _finished )                       { m_bhvFinished = _finished; }
        ITF_INLINE const bbool &        isBhvStopped() const				                    { return m_bhvStopped; }
        ITF_INLINE void                 setBhvStopped( bbool _stopped )                         { m_bhvStopped = _stopped; }
    private:
        StringID            m_wantedBehaviourName;
        bbool               m_changeValidated;
        bbool               m_bhvFinished;
        bbool               m_bhvStopped;
    };

    class EventSetupBezier : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventSetupBezier,Event,713993105);

    public:
        EventSetupBezier()
            : Super()
            , m_p0(Vec3d::Zero)
            , m_p1(Vec3d::Zero)
            , m_p2(Vec3d::Zero)
            , m_p3(Vec3d::Zero)
            {}
        EventSetupBezier(Vec3d _p0, Vec3d _p1, Vec3d _p2, Vec3d _p3)
            : Super()
            , m_p0(_p0)
            , m_p1(_p1)
            , m_p2(_p2)
            , m_p3(_p3)
            {}

        ITF_INLINE const Vec3d& getP0() const { return m_p0; }
        ITF_INLINE const Vec3d& getP1() const { return m_p1; }
        ITF_INLINE const Vec3d& getP2() const { return m_p2; }
        ITF_INLINE const Vec3d& getP3() const { return m_p3; }

    private:
        Vec3d m_p0;
        Vec3d m_p1;
        Vec3d m_p2;
        Vec3d m_p3;
    };

    class EventVirtualLinkBroadcast : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventVirtualLinkBroadcast,Event,3923486039);
        DECLARE_SERIALIZE()
    public:
        EventVirtualLinkBroadcast() : Super(), m_broadcastEvent(NULL) , m_childQuery( bfalse ) {}
        EventVirtualLinkBroadcast( ActorRef _emitter, const StringID & _channelID, Event * _broadcastEvent, bbool _childQuery )
            : Super(),
            m_emitter( _emitter ),
            m_channelID( _channelID ),
            m_broadcastEvent( _broadcastEvent ),
            m_childQuery( _childQuery )
        {}

        ITF_INLINE ActorRef         getEmitter() const                          { return m_emitter; }
        ITF_INLINE void             setEmitter( const ActorRef & _emitter )     { m_emitter = _emitter; }
        ITF_INLINE const StringID & getChannelID() const                        { return m_channelID; }
        ITF_INLINE void             setChannelID( const StringID & _channelID ) { m_channelID = _channelID; }
        ITF_INLINE Event *          getBroadcastEvent() const                   { return m_broadcastEvent; }
        ITF_INLINE void             setBroadcastEvent( Event * _event )         { m_broadcastEvent = _event; }
        ITF_INLINE bbool            isChildQuery() const                        { return m_childQuery; }
        ITF_INLINE void             setChildQuery( bbool _childQuery )          { m_childQuery = _childQuery; }
    private:
        ActorRef    m_emitter;
        StringID    m_channelID;
        Event *     m_broadcastEvent;
        bbool       m_childQuery;
    };

    class EventVirtualLinkChildQuery : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventVirtualLinkChildQuery,Event,940758855);
        DECLARE_SERIALIZE()
    public:
        EventVirtualLinkChildQuery() : Super() {}
        EventVirtualLinkChildQuery( ActorRef _child ) : Super(), m_child(_child){}
        ITF_INLINE ActorRef     getChild() const                        { return m_child; }
        ITF_INLINE void         setChild( const ActorRef & _child )     { m_child = _child; }
    private:
        ActorRef    m_child;
    };


    class EventDisableCollision : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventDisableCollision,Event,3761196874);
        DECLARE_SERIALIZE()

    public:
        EventDisableCollision()
            : m_disable(btrue)
        {}
        EventDisableCollision(bbool _disable)
            : m_disable(_disable)
        {}

        ITF_INLINE bbool getDisable() const { return m_disable; }
        ITF_INLINE void setDisable( bbool _value ) { m_disable = _value; }

    private:
        bbool m_disable;
    };

    class EventQueryCanZoomOnTarget : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventQueryCanZoomOnTarget,Event,4237975921);
    public:
        EventQueryCanZoomOnTarget() : m_canZoomOnTarget(btrue) //We can, except if the target explicitly denies
        {}

        bbool getCanZoomOnTarget() const {return m_canZoomOnTarget;}
        void setCanZoomOnTarget(bbool _canZoom) {m_canZoomOnTarget = _canZoom;}

    private:
        bbool m_canZoomOnTarget;
    };

    class EventForceStickOnEdge : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventForceStickOnEdge,Event,3946845675);
        DECLARE_SERIALIZE()
    public:
        EventForceStickOnEdge() : m_edgeIndex(U32_INVALID)
        {}

        ITF_INLINE ITF::Vec2d getPos() const { return m_pos; }
        ITF_INLINE void setPos(ITF::Vec2d val) { m_pos = val; }
        ITF_INLINE ITF::ObjectRef getPolyline() const { return m_polyline; }
        ITF_INLINE void setPolyline(ITF::ObjectRef val) { m_polyline = val; }
        ITF_INLINE u32 getEdgeIndex() const { return m_edgeIndex; }
        ITF_INLINE void setEdgeIndex(u32 val) { m_edgeIndex = val; }

    private:
        Vec2d m_pos;
        ObjectRef m_polyline;
        u32 m_edgeIndex;
    };

    class EventQueryWaterInfluence : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventQueryWaterInfluence,Event,1378586140);
        DECLARE_SERIALIZE()
    public:
        EventQueryWaterInfluence()
        {}

        ITF_INLINE ITF::Vec2d & getPos() { return m_pos; }
        ITF_INLINE void setPos(ITF::Vec2d val) { m_pos = val; }

    private:
        Vec2d m_pos;
    };

    class EventTrajectorySpawn : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventTrajectorySpawn,Event,3846946197);
        DECLARE_SERIALIZE()

    public:
        ITF_INLINE u32 getSpawneeIndex() const { return m_spawneeIndex; }

    private:
        u32 m_spawneeIndex;
    };

    class EventTriggerBehaviour : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventTriggerBehaviour,Event,2225168250);
        DECLARE_SERIALIZE()
    };

    class EventOrangeContact : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventOrangeContact,Event,4040257774);
        DECLARE_SERIALIZE()

    public:
        EventOrangeContact()
        : m_speed(Vec2d::Zero)
        {}

        ITF_INLINE const Vec2d & getSpeed() const { return m_speed; }
        ITF_INLINE void setSpeed(const Vec2d & _val) { m_speed = _val; }

    private:
        Vec2d m_speed;
    };

	class EventPoolActorRegistration : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventPoolActorRegistration,Event,3036909603);
        DECLARE_SERIALIZE()
    public:
        EventPoolActorRegistration() : m_register( bfalse ), m_poolOwner( ITF_INVALID_OBJREF ), m_poolID(U32_INVALID)   {}
        EventPoolActorRegistration( bbool _register, ActorRef _owner, u32 _poolId ) : m_register( _register ), m_poolOwner( _owner ),m_poolID( _poolId ) {}

        ITF_INLINE const ActorRef &     getPoolOwner() const    { return m_poolOwner; }
        ITF_INLINE const u32 &          getPoolID() const       { return m_poolID; }
        ITF_INLINE const bbool &        isRegistered() const    { return m_register; }
    private :
        ActorRef    m_poolOwner;
        u32         m_poolID;
        bbool       m_register;
    };

    class EventPoolActorRecycle : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventPoolActorRecycle,Event,1859888857);
        DECLARE_SERIALIZE()
    public:
        EventPoolActorRecycle() : m_registeredActor( ITF_INVALID_OBJREF ), m_poolID(U32_INVALID)   {}
        EventPoolActorRecycle( ActorRef _actor, u32 _poolId ) : m_registeredActor( _actor ),m_poolID( _poolId ) {}

        ITF_INLINE const ActorRef &     getRegisteredActor() const      { return m_registeredActor; }
        ITF_INLINE const u32 &          getPoolID() const               { return m_poolID; }
    private :
        ActorRef    m_registeredActor;
        u32         m_poolID;
    };

    class EventTimedSpawnerDataSet : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventTimedSpawnerDataSet, Event,436096372);
        DECLARE_SERIALIZE()
    public:
        EventTimedSpawnerDataSet() : m_spawnDelay(0), m_spawnRate(1), m_burstCount(-1), m_burstElementsCount(-1), m_burstDelay( 0.0f ){}
        ITF_INLINE void setData( f32 _spawnDelay, f32 _spawnRate, i32 _burstElementsCount, i32 _burstCount, f32 _burstDelay )
        {
            m_spawnDelay = _spawnDelay;
            m_spawnRate = _spawnRate;
            m_burstElementsCount = _burstElementsCount;
            m_burstCount = _burstCount;
            m_burstDelay = _burstDelay;
        }
        ITF_INLINE void getData( f32 & _spawnDelay, f32 & _spawnRate, i32 &_burstElementsCount, i32 &_burstCount, f32 &_burstDelay ) const
        {
            _spawnDelay = m_spawnDelay;
            _spawnRate = m_spawnRate;
            _burstElementsCount = m_burstElementsCount;
            _burstCount = m_burstCount;
            _burstDelay = m_burstDelay;
        }
    private:
        f32	m_spawnDelay;           // first spawn after 'delay' seconds
        f32 m_spawnRate;            // then spawn every 'rate' seconds
        i32 m_burstElementsCount;   // number of spawned object (-1 is infinite)
        i32 m_burstCount;           // number of burst to do (-1 is infinite)
        f32 m_burstDelay;           // delay between burst
    };

    class EventQueryIsSprinting : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventQueryIsSprinting, Event,2326390223);
        DECLARE_SERIALIZE()

    public:
        EventQueryIsSprinting() : m_isSprinting(bfalse) {}
        ~EventQueryIsSprinting() {}

        void                setIsSprinting( bbool _val ) { m_isSprinting = _val; }
        bbool               getIsSprinting() const { return m_isSprinting; }

    private:

        bbool               m_isSprinting;
    };

    class EventSetOriginalSender : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventSetOriginalSender, Event,1174287572);

    public:
        ITF_INLINE const ActorRef& get() const { return m_originalSender; }
        ITF_INLINE void set(const ActorRef& _val) { m_originalSender = _val; }

    private:
        ActorRef m_originalSender;
    };

    class EventGetPolylineConstrainedPosition : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventGetPolylineConstrainedPosition, Event,3189215538);
        DECLARE_SERIALIZE()

    public:
        EventGetPolylineConstrainedPosition() : m_poly(ITF_INVALID_OBJREF), m_edgeIndex(U32_INVALID), m_t(0.f) {}
        EventGetPolylineConstrainedPosition( ObjectRef _sender, ObjectRef _poly ) : Super(_sender), m_poly(_poly), m_edgeIndex(U32_INVALID), m_t(0.f) {}
        ~EventGetPolylineConstrainedPosition() {}

        ObjectRef                   getPolyline() const { return m_poly; }

        u32                         getEdgeIndex() const { return m_edgeIndex; }
        f32                         getT() const { return m_t; }

        void                        setEdgeIndex( u32 _edgeIndex ) { m_edgeIndex = _edgeIndex; }
        void                        setT( f32 _t ) { m_t = _t; }

    private:

        ObjectRef                   m_poly;
        u32                         m_edgeIndex;
        f32                         m_t;
    };

    class EventDisplayText : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventDisplayText, Event,3103222744);
        DECLARE_SERIALIZE()

    public:
        EventDisplayText() : m_lineId(U32_INVALID) {}
        ~EventDisplayText() {}

        ITF_INLINE u32 getLineId() const { return m_lineId; }
        ITF_INLINE void setLineId(const u32 _lineId) { m_lineId = _lineId; }

    private:
        u32 m_lineId;
    };

} // namespace ITF

#endif // _ITF_GAMEPLAYEVENTS_H_
