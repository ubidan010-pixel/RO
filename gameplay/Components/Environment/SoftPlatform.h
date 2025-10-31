#ifndef _ITF_SOFTPLATFORM_H_
#define _ITF_SOFTPLATFORM_H_

namespace ITF
{

class SoftPlatform : public IEventListener
{
public:

    struct BodyInitData
    {
        Vec2d               m_pos;
        bbool               m_static;
        f32                 m_gravityMultiplier;
        f32                 m_windMultiplier;
    };

    struct ConstraintInitData
    {
        u32                 m_bodyAIndex;
        u32                 m_bodyBIndex;
        Angle               m_minAngle;
        Angle               m_maxAngle;
        f32                 m_minLength;
        f32                 m_maxLength;
        f32                 m_stiff;
        f32                 m_damp;
        bbool               m_limitAngle;
        bbool               m_relaxLength;
    };

    struct SoftPlatformInitData
    {
        f32                             m_depth;
        f32                             m_movingPolyForce;
        f32                             m_weightMultiplier;
        f32                             m_landSpeedMultiplier;
        f32                             m_hitForceMultiplier;
        f32                             m_impulseMultiplier;
        StringID                        m_gameMaterial;
        SafeArray <BodyInitData>        m_bodies;
        SafeArray <ConstraintInitData>  m_constraints;
        bbool                           m_phantom;
    };

    struct BodyData
    {
        u32                 m_constraintIndex;
        class PhysBody*     m_body;
        class PhysShape*    m_shape;
    };

    struct ConstraintData
    {
        u32                     m_bodyAIndex;
        u32                     m_bodyBIndex;
        class PhysConstraint*   m_constraint;
    };

    typedef SafeArray <BodyData> BodiesArray;
    typedef SafeArray <ConstraintData> ConstraintArray;

    SoftPlatform();
    virtual ~SoftPlatform();

    virtual void                setOwner( Actor* _owner );

    virtual void                init( const SoftPlatformInitData& _initData );
    virtual void                update( f32 _dt );
    virtual void                onEvent( Event * _event );

    void                        clear();
    void                        registerPhysics( bbool _val );
    void                        setDepth( f32 _z, bbool _updateBodies );

    BodiesArray&                getBodyList() { return m_bodies; }
    const BodiesArray&          getBodyList() const { return m_bodies; }

    ConstraintArray&            getConstraintsList() { return m_constraints; }
    const ConstraintArray&      getConstraintsList() const { return m_constraints; }

    void                        applyForce( u32 _bodyIndex, const Vec2d& _force, const Vec2d& _pos );
    void                        applyImpulse( u32 _bodyIndex, const Vec2d& _impulse, f32 _t );

#ifdef ITF_SUPPORT_EDITOR
    void                        drawEdit( ActorDrawEditInterface* drawInterface, u32 _flags ) const;
#endif // ITF_SUPPORT_EDITOR

protected:

    void                        updatePolyline( f32 _dt );
    virtual void                addForces( f32 _dt );

    virtual void                processNewUser( class EventStickOnPolyline* _stickEvent );
    virtual void                processUserEdgeChange( class EventStickOnPolylineChangeEdge* _event );
    virtual void                processHangingNew( class EventHanging* _event );
    virtual void                processHangingUpdate( class EventHangUpdate* _event );
    virtual void                processHitStim( HitStim* _hit );
    virtual void                processRopeSwingImpulse( EventRopeSwingImpulse* _impulse );
    virtual void                processBounceEvent( EventBouncedOnPolyline* _bounce );

    struct User
    {
        ObjectRef               m_actor;
        u32                     m_bodyIndex;
        f32                     m_weight;
    };

    struct Hanging
    {
        ObjectRef               m_actor;
        u32                     m_bodyIndex;
        f32                     m_weight;
        f32                     m_t;
    };

    typedef ITF_LIST <User> UsersList;
    typedef ITF_LIST <Hanging> HangingList;

    void                        addUserForces();
    void                        addHangingForces( f32 _dt );
    void                        addMovingPolylineForces( f32 _dt );

    UsersList                   m_users;
    HangingList                 m_hanging;

    Actor*                      m_owner;
    BodiesArray                 m_bodies;
    ConstraintArray             m_constraints;
    f32                         m_depth;
    Vec2d                       m_gravity;
    class PhysPhantom*          m_phantom;
    class PolyLine*             m_polyline;
    f32                         m_movingPolyForce;
    f32                         m_weightMultiplier;
    f32                         m_landSpeedMultiplier;
    f32                         m_hitForceMultiplier;
    f32                         m_impulseMultiplier;
    bbool                       m_registered;
};

}

#endif // _ITF_SOFTPLATFORM_H_