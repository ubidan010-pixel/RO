#ifndef _ITF_GROUNDAICONTROLLERCOMPONENT_H_
#define _ITF_GROUNDAICONTROLLERCOMPONENT_H_

#ifndef _ITF_BASEAICONTROLLERCOMPONENT_H_
#include "gameplay/Components/AI/BaseAIControllerComponent.h"
#endif //_ITF_BASEAICONTROLLERCOMPONENT_H_

#ifndef _ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_
#include "gameplay/Components/common/StickToPolylinePhysComponent.h"
#endif //_ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_

namespace ITF
{

class GroundAIControllerComponent : public BaseAIControllerComponent
{
    DECLARE_OBJECT_CHILD_RTTI(GroundAIControllerComponent,BaseAIControllerComponent,2338098180);
    DECLARE_SERIALIZE()
    DECLARE_VALIDATE_COMPONENT()

public:

    GroundAIControllerComponent();
    virtual ~GroundAIControllerComponent();

    virtual bbool       needsUpdate() const { return btrue; }
    virtual bbool       needsDraw() const { return bfalse; }
    virtual bbool       needsDraw2D() const { return bfalse; }

    virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
    virtual void        Update( f32 _dt );
    virtual void        onEvent(Event * _event);

    void                setJump( bbool _forced = bfalse )   { m_jump = btrue; m_forceJump = _forced; }
    void                jumpToTarget( const Vec3d& _pos );
    void                resetJumpToTarget();
    void                setSwimingPolylineData( ObjectRef _ref, u32 _surfaceEdge = U32_INVALID, f32 _surfaceEdgeT = 0.f, bbool _isOnSurface = bfalse );

    bbool               isSwiming() const                   { return m_currentNavMode == NAVMODE_SWIM; }
    bbool               isJumpControlled() const            { return m_currentNavMode == NAVMODE_CONTROLLEDJUMP; }

    void                setRunOnWalls( bbool _val ) { m_wallRun = _val; }

    void                setDisablePhysics( bbool _val ) { m_disablePhysics = _val; }

    void                setSprint( bbool _val )             { m_sprint = _val; }
    bbool               isSprinting() const                 { return m_sprint; }

protected:

    const class GroundAIControllerComponent_Template* getTemplate() const;
    class StickToPolylinePhysComponent* getPhys() const { return static_cast<class StickToPolylinePhysComponent*>(m_phys); }

    enum ENavMode
    {
        NAVMODE_NONE,
        NAVMODE_NORMAL,
        NAVMODE_SWIM,
        NAVMODE_CONTROLLEDJUMP,
    };

    // On ground
    void                updateMoveGround( f32 _dt );
    f32                 calculateEfficiency() const;
    virtual void        updateMoveTargetMultiplier( f32 _deltaTime, bbool _move );
    virtual f32         getWalkMaxSpeed() const;

    // In the air
    void                updateMoveAir( f32 _dt );

    // Controlled Jump
    void                updateControlledJump( f32 _dt );

    // In the water
    void                updateMoveSwim( f32 _dt );
    void                swimingUpdateAirFriction( PolyLine* _poly );
    void                swimingUpdatePolyData( PolyLine* _poly );
    void                swimUpdateDive( f32 _dt, PolyLine* _poly, Vec2d& _moveDir, f32& _moveSize, Vec2d& _swimForce, Vec2d& _floatForce );
    void                swimApplyForces( f32 _dt, const Vec2d& _swimForce, const Vec2d& _floatForce );
    void                swimUpdateRotation( f32 _dt );
    void                swimGetFloatForce( const PolyLineEdge& _surfaceEdge, Vec2d& _floatForce );
    void                swimGetSwimForce( f32 _dt, const Vec2d& _moveDir, f32 _moveSize, Vec2d& _swimForce );
    void                setSwimTargetRotation( f32 _target );
    void                resetSwimTargetRotation();
    
    void                performJump();

    void                clearFrame();
    void                setNavMode();

    void                checkAvoidance();
    void                setAvoidance( bbool _val );

    // common
    AnimLightComponent*         m_animComponent;
    class PhantomComponent*     m_phantomComponent;
    f32                         m_moveTargetBlendTimer;
    f32                         m_currentMoveTargetMultiplier;
    bbool                       m_mustAvoid;
    f32                         m_avoidanceCheckTimer;
    f32                         m_avoidanceSpeedFactor;
    ENavMode                    m_currentNavMode;
    ENavMode                    m_nextNavMode;

    // swim
    ObjectRef                   m_swimPolyline;
    f32                         m_waterResistance;
    u32                         m_swimingSurfaceEdge;
    f32                         m_swimingSurfaceT;
    Vec2d                       m_swimingInertia;
    f32                         m_swimingTargetRotation;
    f32                         m_swimingCurrentAngularSpeed;
    bbool                       m_swimingIsOnSurface;
    bbool                       m_swimingIsOnJumpRange;

    // jump
    Vec3d                       m_controlledJumpStart;
    Vec3d                       m_controlledJumpEnd;
    Vec2d                       m_controlledJumpPrevPos;
    f32                         m_controlledJumpTimer;
    f32                         m_controlledJumpTimeTotal;

    f32                         m_sprintMaxSpeedBlendTimer;         // The timer to go from regular walk speed to spring speed
    

    bbool                       m_jump;
    bbool                       m_forceJump;
    bbool                       m_disablePhysics;
    bbool                       m_wallRun;
    bbool                       m_sprint;
};

class GroundAIControllerComponent_Template : public BaseAIControllerComponent_Template
{
    DECLARE_OBJECT_CHILD_RTTI(GroundAIControllerComponent_Template,BaseAIControllerComponent_Template,1160833416);
    DECLARE_SERIALIZE();
    DECLARE_ACTORCOMPONENT_TEMPLATE(GroundAIControllerComponent);

public:

    GroundAIControllerComponent_Template();
    ~GroundAIControllerComponent_Template();

    ITF_INLINE f32 getWalkForce() const { return m_walkForce; }
    ITF_INLINE f32 getWalkEfficiencyMaxSpeed() const { return m_walkEfficiencyMaxSpeed; }
    ITF_INLINE Angle getWalkMinGroundAngleEfficiencyAngle() const { return m_walkMinGroundAngleEfficiencyAngle; }
    ITF_INLINE Angle getWalkMaxGroundAngleEfficiencyAngle() const { return m_walkMaxGroundAngleEfficiencyAngle; }
    ITF_INLINE f32 getWalkMinGroundAngleEfficiencyMult() const { return m_walkMinGroundAngleEfficiencyMult; }
    ITF_INLINE f32 getWalkMaxGroundAngleEfficiencyMult() const { return m_walkMaxGroundAngleEfficiencyMult; }
    ITF_INLINE f32 getWalkMoveTargetMultiplierMin() const { return m_walkMoveTargetMultiplierMin; }
    ITF_INLINE f32 getWalkMoveTargetMultiplierMax() const { return m_walkMoveTargetMultiplierMax; }
    ITF_INLINE f32 getWalkMoveTargetMultiplierBlendTime() const { return m_walkMoveTargetMultiplierMax; }

    ITF_INLINE f32 getAirForce() const { return m_airForce; }
    ITF_INLINE f32 getAirTopSpeed() const { return m_airTopSpeed; }
    ITF_INLINE f32 getAirFriction() const { return m_airFriction; }
    ITF_INLINE f32 getAirSuspensionMinYSpeed() const { return m_airSuspensionMinYSpeed; }
    ITF_INLINE f32 getAirSuspensionMaxYSpeed() const { return m_airSuspensionMaxYSpeed; }
    ITF_INLINE f32 getAirSuspensionMinMultiplier() const { return m_airSuspensionMinMultiplier; }
    ITF_INLINE f32 getAirSuspensionMaxMultiplier() const { return m_airSuspensionMaxMultiplier; }

    ITF_INLINE f32 getSwimingWaterResistMinSpeed() const { return m_swimingWaterResistMinSpeed; }
    ITF_INLINE f32 getSwimingWaterResistMaxSpeed() const { return m_swimingWaterResistMaxSpeed; }
    ITF_INLINE f32 getSwimingWaterResistMaxMult() const { return m_swimingWaterResistMaxMult; }
    ITF_INLINE f32 getSwimingJumpRange() const { return m_swimingJumpRange; }
    ITF_INLINE f32 getSwimingInertiaSpeed() const { return m_swimingInertiaSpeed; }
    ITF_INLINE f32 getSwimingTopSpeed() const { return m_swimingTopSpeed; }
    ITF_INLINE f32 getSwimingTurnDistMult() const { return m_swimingTurnDistMult; }
    ITF_INLINE f32 getSwimingTurnSpeedMult() const { return m_swimingTurnSpeedMult; }

    ITF_INLINE f32 getJumpForce() const { return m_jumpForce; }

    ITF_INLINE f32 getJumpToActorYFuncPoint0Dist() const { return m_jumpToActorYFuncPoint0Dist; }
    ITF_INLINE f32 getJumpToActorYFuncPoint1Dist() const { return m_jumpToActorYFuncPoint1Dist; }
    ITF_INLINE f32 getJumpToActorXZFuncPoint0T() const { return m_jumpToActorXZFuncPoint0T; }
    ITF_INLINE f32 getJumpToActorXZFuncPoint1T() const { return m_jumpToActorXZFuncPoint1T; }
    ITF_INLINE f32 getJumpToActorSpeed() const { return m_jumpToActorSpeed; }
    ITF_INLINE f32 getJumpToActorMinTime() const { return m_jumpToActorMinTime; }

    ITF_INLINE f32 getAvoidanceRadius() const { return m_avoidanceRadius; }
    ITF_INLINE f32 getAvoidanceMinVariation() const { return m_avoidanceMinVariation; }
    ITF_INLINE f32 getAvoidanceMaxVariation() const { return m_avoidanceMaxVariation; }

    ITF_INLINE f32 getSprintForceMultiplier() const { return m_sprintForceMultiplier; }
    ITF_INLINE f32 getSprintEfficiencyMultiplier() const { return m_sprintEfficiencyMultiplier; }
    ITF_INLINE f32 getSprintMaxSpeedBlendDuration() const { return m_sprintMaxSpeedBlendDuration; }

private:

    f32                                     m_walkForce;
    f32                                     m_walkEfficiencyMaxSpeed;
    f32                                     m_walkEfficiencyMaxSpeedRandomFactor;
    Angle                                   m_walkMinGroundAngleEfficiencyAngle;
    Angle                                   m_walkMaxGroundAngleEfficiencyAngle;
    f32                                     m_walkMinGroundAngleEfficiencyMult;
    f32                                     m_walkMaxGroundAngleEfficiencyMult;
    f32                                     m_walkMoveTargetMultiplierMin;
    f32                                     m_walkMoveTargetMultiplierMax;
    f32                                     m_walkMoveTargetMultiplierBlendTime;

    f32                                     m_airForce;
    f32                                     m_airTopSpeed;
    f32                                     m_airFriction;
    f32                                     m_airSuspensionMinYSpeed;           // Minimum Y speed to take into account when calculating the suspension multiplier
    f32                                     m_airSuspensionMaxYSpeed;           // Maximum Y speed to take into account when calculating the suspension multiplier
    f32                                     m_airSuspensionMinMultiplier;       // Suspension multiplier at minimum Y speed
    f32                                     m_airSuspensionMaxMultiplier;       // Suspension multiplier at maximum Y speed

    f32                                     m_swimingWaterResistMinSpeed;       // Minimum speed to apply the minimum friction
    f32                                     m_swimingWaterResistMaxSpeed;       // Maximum speed to apply the maximum friction
    f32                                     m_swimingWaterResistMaxMult;        // Maximum multiplier to water resistance
    f32                                     m_swimingJumpRange;                 // The distance from the surface to be able to jump
    f32                                     m_swimingInertiaSpeed;              // The inertia speed
    f32                                     m_swimingTopSpeed;                  // Max speed when swiming
    f32                                     m_swimingTurnDistMult;              // Angular Speed calculation: distance multiplier
    f32                                     m_swimingTurnSpeedMult;             // Angular Speed calculation: speed multiplier

    f32                                     m_jumpForce;

    f32                                     m_jumpToActorYFuncPoint0Dist;       // Distance of the first bezier point to calculate Y
    f32                                     m_jumpToActorYFuncPoint1Dist;       // Distance of the second bezier point to calculate Y
    f32                                     m_jumpToActorXZFuncPoint0T;         // Percentage of the first bezier point to calculate X and Z
    f32                                     m_jumpToActorXZFuncPoint1T;         // Percentage of the first bezier point to calculate X and Z
    f32                                     m_jumpToActorSpeed;                 // Speed to calculate the time it will take to do the jump
    f32                                     m_jumpToActorMinTime;               // Minimum time to perform the jump

    f32                                     m_avoidanceRadius;
    f32                                     m_avoidanceMinVariation;
    f32                                     m_avoidanceMaxVariation;

    f32                                     m_sprintForceMultiplier;            // A multiplier to the work force when sprinting
    f32                                     m_sprintEfficiencyMultiplier;       // A multiplier to the efficiency max speed when sprinting
    f32                                     m_sprintMaxSpeedBlendDuration;      // The duration of the blend to go from regular walk speed to spring speed
};

ITF_INLINE const class GroundAIControllerComponent_Template* GroundAIControllerComponent::getTemplate() const
{
    return static_cast<const class GroundAIControllerComponent_Template*>(m_template);
}

}

#endif // _ITF_GROUNDAICONTROLLERCOMPONENT_H_