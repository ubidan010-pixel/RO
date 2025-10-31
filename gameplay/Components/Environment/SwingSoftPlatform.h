#ifndef _ITF_SWINGSOFTPLATFORM_H_
#define _ITF_SWINGSOFTPLATFORM_H_

#ifndef _ITF_SOFTPLATFORM_H_
#include "gameplay/components/Environment/SoftPlatform.h"
#endif //_ITF_SOFTPLATFORM_H_

namespace ITF
{

class SwingSoftPlatform : public SoftPlatform
{
    typedef SoftPlatform Super;

public:

    SwingSoftPlatform();
    ~SwingSoftPlatform();

    virtual void                setOwner( Actor* _owner );
    virtual void                init( const SoftPlatformInitData& _initData );
    virtual void                update( f32 _dt );
    virtual void                onEvent( Event * _event );

    void                        setNonStiffSwingSpeedFriction( f32 _val ) { m_nonStiffSwingSpeedFriction = _val; }
    void                        setNonStiffGravityShift( f32 _val ) { m_nonStiffGravityShift = _val; }
    void                        setStiffSwingSpeedFriction( f32 _val ) { m_stiffSwingSpeedFriction = _val ; }
    void                        setStiffGravityMultiplier( f32 _val ) { m_stiffGravityMultiplier = _val ; }
    void                        setStiffImpulseMultiplier( f32 _val ) { m_stiffImpulseMultiplier = _val ; }
    void                        setLandDragDuration( f32 _val ) { m_landDragDuration = _val ; }
    void                        setLandDragMultiplier( f32 _val ) { m_landDragMultiplier = _val ; }
    void                        setMaxSwingAngle( const Angle& _val ) { m_maxSwingAngle = _val ; }
    void                        setCanUseSmall( bbool _val ) { m_canUseSmall = _val; }
    void                        setCanUseNormal( bbool _val ) { m_canUseNormal = _val; }
    void                        setRepositionWithBones( bbool _val ) { m_repositionWithBones = _val; }

private:

    struct SwingBodyData
    {
        f32                     m_gravityMultiplier;
        f32                     m_windMultiplier;
    };

    typedef SafeArray <SwingBodyData> SwingBodyDataList;

    struct OrderedHangingInfo
    {
        ObjectRef               m_actor;
        u32                     m_bodyIndex;
        f32                     m_t;
    };

    typedef SafeArray <OrderedHangingInfo> OrderedHangingList;

    void                        updateShiftGravity( f32 _dt );
    void                        updateDragForce( f32 _dt );

    virtual void                addForces( f32 _dt );
    
    virtual void                processHangingNew( class EventHanging* _event );
    virtual void                processRopeSwingImpulse( EventRopeSwingImpulse* _event );
    void                        processGetConstrainedPosition( class EventGetPolylineConstrainedPosition* _event );

    void                        orderHangers();
    void                        setStiffBehavior( bbool _val );

    const ConstraintData*       getBodyConstraint( u32 _bodyIndex ) const;

    SwingBodyDataList           m_swingBodyData;

    u32                         m_numSwingForces;
    Vec2d                       m_swingingForces;
    u32                         m_swingingForcesLastBodyIndex;

    u32                         m_firstBodyIndex;
    u32                         m_lastBodyIndex;
    OrderedHangingList          m_hangersOrdered;
    bbool                       m_stiffBehavior;

    f32                         m_dragCounter;
    f32                         m_dragForce;

    f32                         m_nonStiffSwingSpeedFriction;
    f32                         m_nonStiffGravityShift;
    f32                         m_stiffSwingSpeedFriction;
    f32                         m_stiffGravityMultiplier;
    f32                         m_stiffImpulseMultiplier;
    f32                         m_landDragDuration;
    f32                         m_landDragMultiplier;
    Angle                       m_maxSwingAngle;
    bbool                       m_canUseNormal;
    bbool                       m_canUseSmall;
    bbool                       m_repositionWithBones;
    bbool                       m_allReduced;
};

}

#endif // _ITF_SWINGSOFTPLATFORM_H_