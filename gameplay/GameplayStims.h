#ifndef _ITF_GAMEPLAYSTIMS_H_
#define _ITF_GAMEPLAYSTIMS_H_

#ifndef _ITF_EVENTS_H_
#include "engine/events/Events.h"
#endif //_ITF_EVENTS_H_

#ifndef _ITF_PHYSTYPES_H_
#include "engine/physics/PhysTypes.h"
#endif //_ITF_PHYSTYPES_H_

#ifndef _ITF_GAMEPLAYTYPES_H_
#include "gameplay/GameplayTypes.h"
#endif //_ITF_GAMEPLAYTYPES_H_

namespace ITF
{

    class EventStim : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventStim,Event,316939190);
        DECLARE_SERIALIZE()

    public:

        EventStim()
            : Super()
            , m_phantomRef(ITF_INVALID_OBJREF)
            , m_shape(NULL)
            , m_pos(Vec2d::Zero)
            , m_z(0.0f)
            , m_prevPos(Vec2d::Zero)
            , m_angle(0.f)
            , m_stopProcessing(bfalse)
        {}
        virtual ~EventStim() {}

        const Vec2d&                            getPos() const { return m_pos; }
        void                                    setPos( const Vec2d& _pos ) { m_pos = _pos; }

        const Vec2d&                            getPrevPos() const { return m_prevPos; }
        void                                    setPrevPos( const Vec2d& _prevPos ) { m_prevPos = _prevPos; }

        f32                                     getAngle() const { return m_angle; }
        void                                    setAngle( f32 _angle ) { m_angle = _angle; }

        f32                                     getDepth() const { return m_z; }
        void                                    setDepth( f32 _z ) { m_z = _z; }

        const class PhysShape*                  getShape() const { return m_shape; }
        void                                    setShape( const class PhysShape* _shape ) { m_shape = _shape; }

        ObjectRef                               getPhantomRef() const { return m_phantomRef; }
        void                                    setPhantomRef( ObjectRef _ref ) { m_phantomRef = _ref; }

        const PhysContactsContainer&            getContacts() const { return m_contacts; }
        void                                    resetContacts() { m_contacts.clear(); }
        void                                    addContact( const SCollidableContact& _contact ) { m_contacts.push_back(_contact); }

        ITF_INLINE bbool                        getStopProcessing() const { return m_stopProcessing; }
        ITF_INLINE void                         setStopProcessing( bbool _value ) { m_stopProcessing = _value; }

        virtual void                            reset();

    private:

        ObjectRef                               m_phantomRef;       // The phantom that caught this stim
        const class PhysShape*                  m_shape;            // The shape of the stim. Do not keep this as it is reused by the stims manager
        Vec2d                                   m_pos;              // The position where we put the shape
        Vec2d                                   m_prevPos;          // The previous position for sweep collision detection
        f32                                     m_angle;            // The angle we set for the shape
        f32                                     m_z;                // The depth for collision check
        PhysContactsContainer                   m_contacts;         // The contact information from the collision
        bbool                                   m_stopProcessing;   // If true, the stim will not be sent to any more receivers
    };

    class HitStim : public EventStim
    {
        DECLARE_OBJECT_CHILD_RTTI(HitStim,EventStim,2121727796);
        DECLARE_SERIALIZE()

    public:

        HitStim()
            : Super()
            , m_level(0)
            , m_dir(Vec2d::Zero)
            , m_fxPos(Vec3d::Zero)
            , m_faction(U32_INVALID)
        {}
        virtual ~HitStim() {}
        virtual void reset();

        u32                         getLevel() const { return m_level; }
        void                        setLevel( u32 _level ) { m_level = _level; }

        // When the stim is radial it will return a direction relative to the position of the actor and the stim
        virtual Vec2d               getDirection() const { return m_dir; }
        void                        setDirection( const Vec2d& _dir ) { m_dir = _dir; }

        const Vec3d&                getFxPos() const { return m_fxPos; }
        void                        setFxPos( const Vec3d& _pos ) { m_fxPos = _pos; }

        ITF_INLINE ActorRef         getOriginalSender() const { return m_originalSender; }
        ITF_INLINE void             setOriginalSender(ActorRef _ref) { m_originalSender = _ref; }
        ITF_INLINE Faction          getFaction() const { return m_faction; }
        ITF_INLINE void             setFaction(ITF::Faction val) { m_faction = val; }

        virtual StringID            getActionFromHit() const { return StringID::Invalid; }

    protected:

        u32                         m_level;
        Vec2d                       m_dir;
        Vec3d                       m_fxPos;
        Faction                     m_faction;
        ActorRef                    m_originalSender;

    };

    class WindStim : public EventStim
    {
        DECLARE_OBJECT_CHILD_RTTI(WindStim,EventStim,2026477007);
        DECLARE_SERIALIZE()

        WindStim()
            : Super()
            , m_dir(Vec2d::Zero)
        {}
        virtual ~WindStim() {}

        const Vec2d&                getDir() const { return m_dir; }
        void                        setDir( const Vec2d& _dir ) { m_dir = _dir; }

    public:

        Vec2d                       m_dir;
    };

    class SolidStim : public EventStim
    {
        DECLARE_OBJECT_CHILD_RTTI(SolidStim,EventStim,3144684763);
        DECLARE_SERIALIZE()

        SolidStim()
            : Super()
            , m_dir(Vec2d::Zero)
        {}
        virtual ~SolidStim() {}

        const Vec2d&                getDirection() const { return m_dir; }
        void                        setDirection( const Vec2d& _dir ) { m_dir = _dir; }

    public:

        Vec2d                       m_dir;
    };

    class EventStimNotify : public Event
    {
        DECLARE_OBJECT_CHILD_RTTI(EventStimNotify,Event,1337973749);
        DECLARE_SERIALIZE()

    public:
        EventStimNotify()
            : Super()
        {}
        virtual ~EventStimNotify() {}

        void                        setContact(SCollidableContact _contact) { m_contact = _contact; }
        const SCollidableContact&   getContact() {return m_contact; }

    private:
        SCollidableContact   m_contact;
    };


}

#endif // _ITF_GAMEPLAYSTIMS_H_