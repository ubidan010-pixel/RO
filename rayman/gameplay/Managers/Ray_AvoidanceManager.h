#ifndef _ITF_RAY_AVOIDANCEMANAGER_H_
#define _ITF_RAY_AVOIDANCEMANAGER_H_

namespace ITF
{
    #define RAY_AVOIDANCEMANAGER Ray_AvoidanceManager::get()

    // AvoidanceData
    class Ray_SquadMemberData
    {
        friend class Ray_AvoidanceManager;

    public:
        Ray_SquadMemberData()
            : m_actor(ITF_INVALID_OBJREF)
            , m_radius(1)
            , m_minDelta(0.2f)
            , m_enabled(btrue)
            , m_isBlocked(bfalse)
        {}

        ITF_INLINE f32      getRadius() const { return m_radius; }
        ITF_INLINE void     setRadius(f32 _value) { m_radius = _value; }

        ITF_INLINE f32      getMinDelta() const { return m_minDelta; }
        ITF_INLINE void     setMinDelta(f32 _value) { m_minDelta = _value; }

        ITF_INLINE bbool    getEnabled() const { return m_enabled; }
        ITF_INLINE void     setEnabled(bbool _value) { m_enabled = _value; }

        ITF_INLINE Vec2d    getPosition() const { return m_position; }
        ITF_INLINE void     setPosition(Vec2d _value) { m_position = _value; }

        ITF_INLINE Vec2d    getDestination() const { return m_destination; }
        ITF_INLINE void     setDestination(Vec2d _value) { m_destination = _value; }

        ITF_INLINE bbool    getIsBlocked() const { return m_isBlocked; }
        ITF_INLINE void     setIsBlocked(bbool _value) { m_isBlocked = _value; }

        // get members on the left/right if any, and return _actor's index
        void                getNeighbors( Ray_SquadMemberData*& _left, Ray_SquadMemberData*& _right );

        struct AvoidanceData
        {
            f32 penetrationLeft;
            f32 penetrationRight;
            f32 destinationLeft;
            f32 destinationRight;
            bbool blocked;
        };

        // get where we should be, depending on where we want to go
        void                avoidNeighbors( bbool _targetIsLeft, AvoidanceData & _data);

    private:
        ObjectRef           m_actor;
        f32                 m_radius;
        f32                 m_minDelta;
        bbool               m_enabled;
        Vec2d               m_position;
        Vec2d               m_destination;
        bbool               m_isBlocked;        // can this actor be pushed or not?

        void                getPenetration( f32 _dist, f32 _avoidanceRadius, bbool _isInTargetDir, bbool _oppositeIsBlocking, f32 _oppositeAvailableDist, f32& _penetration, f32& _destination, bbool& _blocked );

        Vec3d               get3DPos();     // for debug
    };

    class Ray_AvoidanceManager
    {
        friend class Ray_SquadMemberData;

        static Ray_AvoidanceManager*    s_instance;
    public:

        static void                 create()    { s_instance = newAlloc(mId_Gameplay, Ray_AvoidanceManager()); }
        static void                 destroy()   { SF_DEL(s_instance); }
        static Ray_AvoidanceManager*    get()       { return s_instance; }  //ITF_ASSERT_MSG(s_instance, "Ray_AvoidanceManager not ready, be sure to call Ray_AvoidanceManager::create() !");


        Ray_AvoidanceManager();
        ~Ray_AvoidanceManager();

        void                    init();
        void                    update( f32 _dt );

#ifdef ITF_SUPPORT_DEBUGFEATURE
        ITF_INLINE bbool        getDrawDebug() const { return m_drawDebug; }
        ITF_INLINE void         setDrawDebug( bbool _value ) { m_drawDebug = _value; }
#endif // ITF_SUPPORT_DEBUGFEATURE

        Ray_SquadMemberData*    addMember( ObjectRef _actor );
        void                    removeMember( ObjectRef _actor );

    private:

#ifdef ITF_SUPPORT_DEBUGFEATURE
        void                    drawDebug();
#endif // ITF_SUPPORT_DEBUGFEATURE

        typedef ITF_VECTOR<Ray_SquadMemberData*> MemberList;
        MemberList m_members;
        MemberList m_enabledMembers;

#ifdef ITF_SUPPORT_DEBUGFEATURE
        bbool m_drawDebug;
#endif // ITF_SUPPORT_DEBUGFEATURE
    };
}

#endif //_ITF_RAY_AVOIDANCEMANAGER_H_

