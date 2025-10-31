#ifndef _ITF_WAR_PLAYERCONTROLLERCOMPONENT_H_
#define _ITF_WAR_PLAYERCONTROLLERCOMPONENT_H_

#ifndef _ITF_PLAYERCONTROLLERCOMPONENT_H_
#include "gameplay/Components/Player/PlayerControllerComponent.h"
#endif //_ITF_PLAYERCONTROLLERCOMPONENT_H_

#ifndef _ITF_INPUTADAPTER_H_
#include "engine/AdaptersInterfaces/InputAdapter.h"
#endif //_ITF_INPUTADAPTER_H_

#ifndef _ITF_ANIMATEDCOMPONENT_H_
#include "engine/actors/components/animatedcomponent.h"
#endif //_ITF_ANIMATEDCOMPONENT_H_

#ifndef _ITF_PHYSFORCEMODIFIER_H_
#include "engine/physics/PhysForceModifier.h"
#endif //_ITF_PHYSFORCEMODIFIER_H_

#ifndef _ITF_GAMEPLAYEVENTS_H_
#include "gameplay/GameplayEvents.h"
#endif //_ITF_GAMEPLAYEVENTS_H_

#ifndef _ITF_WAR_GAMEPLAYEVENTS_H_
#include "war/gameplay/War_GameplayEvents.h"
#endif //_ITF_WAR_GAMEPLAYEVENTS_H_

#ifndef _ITF_WAR_GAMEPLAYTYPES_H_
#include "war/gameplay/War_GameplayTypes.h"
#endif //_ITF_WAR_GAMEPLAYTYPES_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_


namespace ITF
{
    class FXEmitterComponent;   // used by feedback system
    class AnimatedComponent;


class War_PlayerControllerComponent : public PlayerControllerComponent
{
    DECLARE_OBJECT_CHILD_RTTI(War_PlayerControllerComponent, PlayerControllerComponent,2852347349)

private:

    /*struct MovementParams
    {
        f32     m_walkForce;                        // The force we apply when walking
        f32     m_jumpForceMinXSpeed;               // The minimum speed to apply the maximum jump force
        f32     m_jumpForceMaxXSpeed;               // The maximum speed to apply the minimum jump force
        f32     m_jumpForceMinXSpeedForce;          // The minimum force to apply at m_jumpForceMinXSpeed
        f32     m_jumpForceMaxXSpeedForce;          // The maximum force to apply at m_jumpForceMaxXSpeed
    };*/

    struct HangingInfo
    {
        Vec2d m_hangPos;
        PolyLine * m_polyline;
        u32 m_edge;
        bbool m_right;
        bbool m_climb;
        bbool m_solid;
    };

    // Move Directions
    enum EMoveDir
    {
        MOVEDIR_NONE,
        MOVEDIR_UP,
        MOVEDIR_DOWN,
        MOVEDIR_LEFT,
        MOVEDIR_RIGHT,
        ENUM_FORCE_SIZE_32(EMoveDir)
    };

    enum EActionFlag
    {
        ACTIONFLAG_CROUCH       = 0x01,                             // If the character is crouched or not
        ACTIONFLAG_ROLLING      = 0x02,                             // If the character is rolling on the ground
        ACTIONFLAG_SPRINTING    = 0x04,                             // If the character is sprinting
        ACTIONFLAG_DOUBLEJUMP   = 0x08,                             // If the character is double jumping
        ACTIONFLAG_LOWROOF      = 0x10,                             // If the roof is low,
        ENUM_FORCE_SIZE_32(EActionFlag)
    };

#include "War_PlayerControllerStates.h"

public:
    DECLARE_SERIALIZE()

    enum EStance
    {
        STANCE_STAND,
        STANCE_HANG,
        STANCE_CLIMB,
        ENUM_FORCE_SIZE_32(EStance)
    };

    enum OrientationPose
    {
        ORIENTATION_VERTICAL,
        ORIENTATION_HORIZONTAL,
        ENUM_FORCE_SIZE_32(OrientationPose)
    };

    War_PlayerControllerComponent();
    virtual ~War_PlayerControllerComponent();

    virtual bbool               needsUpdate() const { return btrue; }
    virtual bbool               needsDraw() const { return bfalse; }
    virtual bbool               needsDraw2D() const { return bfalse; }

    virtual void                onActorLoaded();
    virtual void                Update( f32 _deltaTime );
    virtual void                updateAnimInput();
    virtual void                onForceMove( );
    virtual void                drawEdit( class ActorDrawEditInterface* drawInterface, u32 _flags  ) const;


    const Vec2d&                getCurrentMove() const { return m_currentMove; };

    f32                         getCurrentEfficiency()  const { return m_currentEfficiency; }
    bbool                       isHanging()             const { return m_hangingPolyline != ITF_INVALID_OBJREF; }
    ObjectRef                   getHangingPolyline()    const { return m_hangingPolyline; }
    u32                         getHangingEdge()        const { return m_hangingEdge; }
    f32                         getHangExtraRadius()    const { return m_hangRadiusExtra; }

    bbool                       isClimbing()            const { return m_climbingPolyline != ITF_INVALID_OBJREF; }
    ObjectRef                   getClimbingPolyline()   const { return m_climbingPolyline; }
    u32                         getClimbingEdge()       const { return m_climbingEdge; }


    bbool                       getIsLookingRight() const { return m_lookRight; }
    void                        onEvent(Event * _event);

    void                        changeState( War_State* _newState);
    EStance                     getStance() const { return m_stance; }

    u32                         getAccrobaticStage() const {  return m_currentAccrobaticStage; }

private:

    void                        clear();

    void                        updateStates( f32 _deltaTime );

    void                        updateFx(f32 _deltaTime);
    void                        updateCheats( const InputAdapter::PressStatus* _buttons,
                                              const f32 * _axes,
                                              class StickToPolylinePhysComponent* _characterPhys,
                                              class AnimatedComponent* _anim );
    void                        UpdateMoveInput( f32 _deltaTime, const InputAdapter::PressStatus* _buttons,
                                                 class StickToPolylinePhysComponent* _characterPhys,
                                                 f32 * _axes );
    void                        updateSprint( class StickToPolylinePhysComponent* _phys );

    void                        UpdatePhysState( class StickToPolylinePhysComponent* _phys );
    void                        UpdateMoveDir( class StickToPolylinePhysComponent* _phys, f32* _axes );
    void                        UpdateDesiredLookDir();
    void                        setDesiredLookDir();
    void                        UpdatePhysicWalk( class StickToPolylinePhysComponent* _phys, f32 _deltaTime );
    void                        UpdatePhysicHanging( class StickToPolylinePhysComponent* _phys );
    void                        UpdatePhysicAir( class StickToPolylinePhysComponent* _phys );
    void                        UpdatePhysicAirHit( class StickToPolylinePhysComponent* _phys );
    void                        UpdatePhysicEarthquake( f32 _dt, class StickToPolylinePhysComponent* _phys );
    void                        UpdatePhysicFlyMode( f32 _deltaTime );
    void                        UpdatePhysicClimbingEdge( f32 _dt, class AnimatedComponent* _animComponent, class StickToPolylinePhysComponent* _phys );
    void                        UpdateMoveTargetMultiplier( f32 _deltaTime );
    void                        checkSquash();

    void                        PerformJump( class StickToPolylinePhysComponent* _phys );
    void                        calculateEfficiency( class StickToPolylinePhysComponent* _phys );
    void                        calculateWalkTargetMoveEfficiency( class StickToPolylinePhysComponent* _phys,
                                                                   const Vec2d& _currentTarget,
                                                                   f32 _deltaTime );
    void                        rotateAim(f32 angle);

    void                        processPolyline(Event* _event);
    void                        processCrush( EventCrushed* _crushed );

    void                        detachFromEverything( class StickToPolylinePhysComponent* _phys,
                                                      class AnimatedComponent* _anim );

    // Joining/leaving game, checkpoint, teleport...
    void                        processPlayerActivationChanged( class EventPlayerActivationChanged* _eventActivation );
    void                        processCheckpointReached( class EventCheckpointReached* _eventCheckpoint );
    void                        processRevertToLastCheckpoint( class EventRevertToLastCheckpoint* _eventRevert );
    void                        processTeleport( class EventTeleport* _eventTeleport );

    // Grab things in the air (can be hanging or climbing)
    void                        setHangState( struct HangingInfo& _info );
    bbool                       tryHanging( class StickToPolylinePhysComponent* _phys, HangingInfo & _info ) const;
    bbool                       processContactHang( StickToPolylinePhysComponent* _phys,
                                                    const struct SCollidableContact& _contact,
                                                    const Vec2d& _headPos, HangingInfo& _info ) const;
    void                        calculateHangPosCheck( const PolyLine* _polyLine,
                                                       const PolyLineEdge& _edge,
                                                       u32 _edgeIndex,
                                                       bbool _isSolid,
                                                       bbool _rightSide,
                                                       Vec2d& _pos, f32& _angle ) const;

    // Hanging
    bbool                       processContactHangSide( StickToPolylinePhysComponent* _phys,
                                                        const struct SCollidableContact& _contact,
                                                        PolyLine* _polyLine, const PolyLineEdge& _edge,
                                                        u32 _edgeIndex, const Vec2d& _headPos,
                                                        HangingInfo& _info ) const;
    void                        hangFromPoint( HangingInfo & _info);
    void                        clearHanging( class StickToPolylinePhysComponent* _phys, class AnimatedComponent* _anim );
    bbool                       isValidPointForHanging( const Vec2d& _hangPoint, const Vec2d& _collisionCheckPos,
                                                        f32 _collisionCheckAngle, const Vec2d& _headPos,
                                                        const PolyLine* _poly, u32 _edgeIndex, bbool _right, bbool _isSolid ) const;
    bbool                       isCurrentHangColliding() const;
    bbool                       isHangingPosColliding( const Vec2d& _collisionCheckPos, f32 _collisionCheckAngle,
                                                       bbool _isSolid ) const;
    bbool                       getWorldHangPosFromLocal( f32 _edgeBlend, Vec2d& _ret, f32& _angle ) const;
    bbool                       isEdgeDirValidToHang( const PolyLineEdge& _edge, bbool _isSolid ) const;
    bbool                       isCurrentHangDirValid() const;

    // Climbing (ropes, edges, etc)
    bbool                       processContactClimb( class StickToPolylinePhysComponent* _phys,
                                                     const struct SCollidableContact& _contact,
                                                     PolyLine* _polyLine,
                                                     const PolyLineEdge& _edge,
                                                     const Vec2d& _headPos,
                                                     HangingInfo& _info ) const;
    bbool                       checkClimbCollision( class StickToPolylinePhysComponent* _phys, ObjectRef _skipPolyline  ) const;
    void                        climbFromPoint( HangingInfo & _info);
    void                        clearClimbing( class StickToPolylinePhysComponent* _phys, class AnimatedComponent* _anim );
    void                        getClimbEdgePolyline( class PolyLine** _polyLine, const class PolyLineEdge** _edge ) const;
    bbool                       isVerticalClimb( const class PolyLineEdge* _edge ) const;
    bbool                       tryHangFromPolyline( class PolyLine* _polyLine, const class PolyLineEdge* _edge, u32 _edgeIndex );

    // PLEASE SORT ME!
    void                        receiveHit    (const War_HitStim* _hit);
    PhysShapePolygon*           getCurrentPunchShape();
    void                        handleAnimTrackEvent(AnimGameplayEvent* _event);

    // Set the stance
    void                        setStance( EStance _stance );
    void                        resetStance( EStance _stance ) ;

    // U-Turn
    void                        startUTurn();
    void                        resetUTurn();
    bbool                       isPerformingUTurn() const { return m_performingUTurn; }

    // Crouch
    void                        setCrouching( bbool _val );
    bbool                       isCrouching() const { return isActionFlagSet(ACTIONFLAG_CROUCH); }

    // Rolling
    void                        setRolling( bbool _val );
    bbool                       isRolling() const { return isActionFlagSet(ACTIONFLAG_ROLLING); }

    // Sprinting
    void                        setSprinting( bbool _val );
    bbool                       isSprinting() const { return isActionFlagSet(ACTIONFLAG_SPRINTING); }

    // Double jump - feature disabled for now
    bbool                       canDoubleJump() const { return bfalse; }    // return m_powerUpManager.hasPowerUp(Ray_PowerUp_DoubleJump);
    void                        setDoubleJumping( bbool _val );
    bbool                       isDoubleJumping() const { return isActionFlagSet(ACTIONFLAG_DOUBLEJUMP); }

    // Low Roof
    void                        setLowRoof( bbool _val );
    bbool                       isLowRoof() const { return isActionFlagSet(ACTIONFLAG_LOWROOF); }


    void                        setMoveInput( const Vec2d& _moveInput );
    void                        setLockMoveInput( bbool _lock ) { m_lockMoveInput = _lock; }
    bbool                       isLockMoveInput() const { return m_lockMoveInput; }
    void                        setLookRight( bbool _lookRight ) { if ( !m_lockLookRight ) { m_lookRight = _lookRight; } }
    void                        resetDesiredLookDir();
    void                        setLockLookRight( bbool _lock ) { m_lockLookRight = _lock; }
    bbool                       isLockLookRight() const { return m_lockLookRight; }

    // Returns true if the user is not expecting any action
    bbool                       playerIsIdle( const InputAdapter::PressStatus* _buttons, f32 * _axes ) const;


    bbool                       isJumpQueued() const { return m_queueJump || m_jumpQueueTimer; }
    bbool                       isHitQueued() const { return m_queueHit; }

    void                        measurePlayWorldSpeed( f32 _dt );

    void                        setCurrentPunch( u32 _type, u32 _level );

    // This transforms an angle into a move direction
    EMoveDir                    getMoveDirectionFromAngle( f32 _angle ) const;
    void                        updateLookRightFromMoveDirection( EMoveDir _moveDir );

    bbool                       checkFallOnCharacter( class StickToPolylinePhysComponent* _phys,
                                                      Actor** _actor ) const;

    void                        checkLowRoof( class StickToPolylinePhysComponent* _phys );

    void                        setActionFlag( EActionFlag _flag ) { m_actionFlags |= _flag; }
    void                        removeActionFlag( EActionFlag _flag ) { m_actionFlags &= (~_flag); }
    void                        toggleActionFlag( EActionFlag _flag ) { if (isActionFlagSet(_flag)) removeActionFlag(_flag);  else setActionFlag(_flag); }
    bbool                       isActionFlagSet( EActionFlag _flag ) const { return (m_actionFlags&_flag)==static_cast<u32>(_flag); }

    void                        setAirPunch();
    void                        setIdlePunch( class StickToPolylinePhysComponent* _phys );
    void                        resetAllowEarthquakePunch() { m_allowEarthquakePunch = bfalse; }
    void                        updateAllowEarthquakePunch();

    Vec3d                       getHitFxPos( class AnimLightComponent* _anim, const Vec3d& _default );
    Vec3d                       getBonePos( class AnimLightComponent* _anim, u32 _index, const Vec3d& _default ) const;

    void                        postLoadProperties();

	StateIdle                   m_stateIdle;
    StateHitPrepare             m_statePrepareHit;
    StateHitRelease             m_stateReleaseHit;
    StateJump                   m_stateJumping;
    StateAim                    m_stateAim;
    StateFalling                m_stateFalling;
    StateHanging                m_stateHanging;
    StateHangToIdle             m_stateHangToIdle;
    StateHangToJump             m_stateHangToJump;
    StateHitReceive             m_stateReceiveHit;
    StateHitReceiveLand         m_stateReceiveHitLand;
    StateHitReceiveCrash        m_stateReceiveHitCrash;
    StateHurtBounce             m_stateHurtBounce;
    StateClimbing               m_stateClimbing;
    StateClimbingToIdle         m_stateClimbingToIdle;
    StateIdleToClimb            m_stateIdleToClimb;
    StateCheatFlyMode           m_stateCheatFlyMode;
    StatePunchComboAccrobatic   m_statePunchAccrobatic;
    StatePunchComboBurst        m_statePunchBurst;
    StateCrushedByCharacter     m_stateCrushedByCharacter;
    StateHangToStanding         m_stateHangToStand;

    static const f32            s_efficiencyCalculationDelay;

    // Hanging state data
    ObjectRef                   m_hangingPolyline;                  // The polyline we are hanging from
    ObjectRef                   m_previousHangingPolyline;          // The previous polyline we were hanging from
    u32                         m_hangingEdge;                      // The edge from the polyline where we are hanging
    bbool                       m_hangingSolid;                     // The hanging polyline is solid

    // Climbing state data
    ObjectRef                   m_climbingPolyline;                 // The polyline we are hanging from
    ObjectRef                   m_previousClimbingPolyline;         // The previous polyline we were climbing
    u32                         m_climbingEdge;                     // The edge from the polyline where we are hanging
    f32                         m_climbingT;                        // Position inside the edge we are climbing (from 0 to 1)
    bbool                       m_climbingVerticalAnim;             // If the climbing stance is for vertical or horizontal polylines
    
    // Phantom data for receiving stims from the world
    PhysShape*                  m_phantomShapeCrouch;               // The shape used by the phantom when we are crouched

    f32                         m_height;                           // The height of the character
    f32                         m_cheatFlyModeSpeed;                // The speed for the fly mode cheat
    f32                         m_walkForce;                        // The force we apply when walking
    f32                         m_airForce;                         // The force we apply when in the air
    f32                         m_airTopXSpeed;                     // When the character reaches this speed he won't be able to add more force
    f32                         m_climbLandSpeedImpulseMultiplier;  // A multiplier to the impulse sent from the world speed of the character when climbing
    f32                         m_climbLandSpeedImpulseTime;        // Time during which we apply an impulse after landing
    f32                         m_climbLandSpeedImpulseDecreaseTime;// Time during the land impulse will decrease to zero
    f32                         m_climbLandSpeedImpulseTimer;       // The timer to apply the speed impulse from landing
    Vec2d                       m_climbLandSpeedImpulseValue;       // The speed at the time of the landing
    f32                         m_climbVerticalSpeedUp;             // The climb speed for the vertical edges, climbing up
    f32                         m_climbVerticalDescendAccel;        // The acceleration when descending from a rope
    f32                         m_climbVerticalDescendMaxSpeed;     // The maximum speed when descending from a rope
    f32                         m_climbVerticalUpAnimRate;          // The rate at which the climb up animation will play
    f32                         m_climbVerticalJumpImpulse;         // The jump impulse applied before jumping when pushing the stick
    f32                         m_climbHorizontalSpeed;             // The speed to move on horizontal ropes
    f32                         m_climbHorizontalMoveAnimRate;      // The rate at which the climb horizontal animation will play
    f32                         m_climbEdgeInertiaBrake;            // The deceleration of the inertia
    f32                         m_climbEdgeDistanceRange;           // The range to accept polylines to grab for climbing
    f32                         m_climbEdgeActivationTime;          // During this time we won't be trying to climb on polylines
    f32                         m_climbEdgeActivationCounter;       // If the counter is != 0.f we don't try to climb on the previous polyline
    f32                         m_climbEdgeSwingImpulse;            // The swing strength when climbing ropes
    f32                         m_climbEdgePunchImpulse;            // When we punch while climbing a rope we apply an impulse in the opposite direction
    f32                         m_climbEdgeMoveImpulse;             // Impulse applied on the rope when the character grabs it on the move cycle
    Angle                       m_climbEdgeAngularSpeed;            // Angular speed to match the target angle
    f32                         m_climbEdgeJumpMultiplier;          // Multiplier to the jump force when we are climbing a polyline (ropes, etc)
    f32                         m_climbTimerThreshold;              // Time before allowing to climb from hanging
    f32                         m_moveTargetMultiplierMin;          // The minimum multiplier we apply to reach the target speed
    f32                         m_moveTargetMultiplierMax;          // The maximum multiplier we apply to reach the target speed
    f32                         m_moveTargetMultiplierBlendTime;    // The time it takes to go from the minimum multiplier to the max
    f32                         m_jumpQueueTime;                    // Time for queueing jump action while we are in the air
    f32                         m_jumpOnAirTime;                    // Time for allowing jumping while in the air when we just dropped from an edge
    f32                         m_moveTargetBlendTimer;             // The timer to go from one speed multiplier to the other
    f32                         m_currentMoveTargetMultiplier;      // The current move target multiplier
    f32                         m_jumpQueueTimer;                   // If we come in contact with an edge while this timer is on then we jump
    f32                         m_jumpOnAirTimer;                   // This timer allows us to jump while on the air when we just dropped from an edge
    f32                         m_jumpForceMinXSpeed;               // The minimum horizontal speed to apply the minimum jump force
    f32                         m_jumpForceMaxXSpeed;               // The maximum horizontal speed to apply the maximum jump force
    f32                         m_jumpForceMinXSpeedForce;          // The minimum force to apply at m_jumpForceMinXSpeed
    f32                         m_jumpForceMaxXSpeedForce;          // The maximum force to apply at m_jumpForceMaxXSpeed
    f32                         m_jumpExtraForceMinYSpeed;          // The minimum vertical speed to apply the minimum jump extra force
    f32                         m_jumpExtraForceMaxYSpeed;          // The maximum vertical speed to apply the maximum jump extra force
    f32                         m_jumpExtraForceMinYSpeedForce;     // The minimum extra force to apply at m_jumpForceMinYSpeed
    f32                         m_jumpExtraForceMaxYSpeedForce;     // The maximum extra force to apply at m_jumpForceMaxYSpeed
    f32                         m_jumpForceFromHangMultiplier;      // Jump force multiplier when jumping from hang position
    f32                         m_jumpForceMultiplier;              // current value of multiplier
    f32                         m_jumpEdgeSpeedMultiplier;          // Multiplier to the edge speed if it moves when jumping
    f32                         m_airSuspensionMinYSpeed;           // Minimum Y speed to take into account when calculating the suspension multiplier
    f32                         m_airSuspensionMaxYSpeed;           // Maximum Y speed to take into account when calculating the suspension multiplier
    f32                         m_airSuspensionMinMultiplier;       // Suspension multiplier at minimum Y speed
    f32                         m_airSuspensionMaxMultiplier;       // Suspension multiplier at maximum Y speed
    f32                         m_airSuspensionTime;                // Time interval to allow air suspension
    f32                         m_airSuspensionCounter;             // Timer to allow air suspension
    f32                         m_efficiencyMinSpeed;               // Min speed to calculate efficiency
    f32                         m_efficiencyMaxSpeed;               // Max speed to calculate efficiency
    f32                         m_efficiencyMinSpeedValue;          // Efficiency at min speed
    f32                         m_efficiencyMaxSpeedValue;          // Efficiency at max speed
    f32                         m_currentEfficiency;                // Current efficiency
    f32                         m_efficiencyCalculationTimer;       // A timer to calculate the right efficiency when moving
    f32                         m_targetMoveEfficiency;             // How close am I to the speed I want to achieve
    f32                         m_frictionModulator;                // Multiplier to friction coefficient
    f32                         m_hangRadiusExtra;                  // Extra radius to detect the ledges for hanging
    Vec2d                       m_hangRectOffset;                   // Offset of the rect used to validate hanging points
    Vec2d                       m_hangRectSize;                     // Size of the rect used to validate hanging points
    Vec2d                       m_hangCollisionExtentSolid;         // Extents of the box used to check if we collide with something when hanging (solid ground)
    Vec2d                       m_hangCollisionExtentNonSolid;      // Extents of the box used to check if we collide with something when hanging (non solid ground)
    Vec2d                       m_hangingCollisionCheckOffsetSolid; // Player hanging offset for the collision test
    Vec2d                       m_hangingCollisionCheckOffsetNonSolid; // Player hanging offset for the collision test
    PhysShapeBox                m_hangingCollisionShapeSolid;       // Shape used to check collision when hanging on solid ground
    PhysShapeBox                m_hangingCollisionShapeNonSolid;    // Shape used to check collision when hanging on non solid ground
    f32                         m_hangMaxSpeed;                     // Max speed in the opposite direction of the gravity to process hanging corners
    f32                         m_hangEdgeActivationCounter;        // If the counter is != 0.f we don't try to hang on the previous polyline
    f32                         m_hangEdgeActivationTime;           // During this time we won't be trying to hang on polylines
    f32                         m_airHitForceX;                     // airHit Force
    f32                         m_airHitForceY;                     // airHit Force
    f32                         m_airHitAntiGravfactor;             // airHit anti gravity factor Min
    f32                         m_airHitForceApplicationTime;       // airHit mode time to full force application
    f32                         m_airHitUnstickMinFrictionMultiplier; //airHit mode, multiplier used to modify physics unstick value
    f32					        m_airHitMinDragSpeedX;              // Speed at which drag force is applied
    f32					        m_airHitDragForceX;                 // Drag force
    f32                         m_minBounceSpeed;                   // Min speed for bouncing
    f32                         m_maxBounceSpeed;                   // Min speed for bouncing
    f32                         m_minBounceAngle;                   // Min bounce Angle
    f32                         m_maxBounceAngle;                   // Max bounce Angle
    f32                         m_bounceSpeedMultiplier;            // bounce speed multiplier
    f32                         m_bounceJumpMultiplier;             // bounce speed multiplier
    f32                         m_bounceDelay;                      // delay to make the bounce
    f32                         m_bounceTimer;                      // delay counter to make the bounce
    f32                         m_freeFallCurveRange;               // Range of speed to set the cursor on the fall anim
    f32                         m_receiveHitForce;                  // Hit force applied when receiving a punch
    f32                         m_receiveLowHitEjectForce;          // Ejecting Y force on the character when receiving a low hit
    f32                         m_receiveLowHitEjectAnimSpeed;      // The speed of the animation to set the cursor proceduraly
    f32                         m_receiveHitGravityMultiplier;      // Gravity multiplier when receiving a hit
    f32                         m_receiveHitFrictionMultiplier;     // Friction multiplier when receiving a hit
    f32                         m_slideFrictionMultiplier;          // Friction for the character when sliding
    f32                         m_slideMinFastSpeed;                // The minimum speed to be considered fast slide
    u32                         m_slideHitLevel;                    // The level of the hit when sliding
    f32                         m_minSpeedForSlide;                 // Minimum speed to start sliding
    f32                         m_slidePrevFrictionMultiplier;      // The friction multiplier before sliding
    u32                         m_numAccrobaticPunchCombo;          // The number of available punch accrobatic combo
    f32                         m_burstPunchInputFrequency;         // Input Frequency for burst punch combo
    f32                         m_accrobaticPunchInputFrequency;    // Input Frequency for accrobatic punch combo
    u32                         m_currentAccrobaticStage;           // The current accrobatic stage
    f32                         m_minSpeedForAccrobaticCombo;       // The minimum speed to be in accrobatic stage
    ObjectRef                   m_receivedHitInstigator;            // The instigator of the received hit
    u32                         m_receivedHitLevel;                 // The level of the hit received
    Vec2d                       m_receivedHitDir;                   // The direction of the hit received
    War_EReceivedHitType        m_receivedHitType;                  // The received hit type
    Vec2d                       m_currentTargetForce;               // The current force applied to the physics
    Vec2d                       m_currentGravity;                   // The current gravity in our position
    Vec2d                       m_currentGravityDir;                // The current gravity direction in our position
    Vec2d                       m_gravityAtPreviousMove;            // Remember the gravity when we kept the stick pressed in the same direction
    Vec2d                       m_punchShapeOffset;                 // Offset for the punch shape when sending the stim
    Vec2d                       m_playWorldSpeed;                   // The world speed when the character is free to move
    Vec2d                       m_prevPos;

    // Crouch
    f32                         m_disableCrouchTimer;               // A timer to disable crouching
    f32                         m_disableCrouchTime;                // How long we disable the crouch (comes from lua)
    f32                         m_rollingJumpMultiplier;            // A multiplier to the jump when we roll
    f32                         m_rollingPrevFrictionMultiplier;    // The friction multiplier before rolling
    f32                         m_rollingFrictionMultiplier;        // The friction multiplier while rolling
    f32                         m_crouchJumpMultiplier;             // A multiplier to the jump when we are crouched

    // Fall on victim
    f32                         m_fallOnVictimMinSpeed;             // Minimum speed to be falling on someone
    f32                         m_fallOnVictimJumpMultiplier;       // Fall on victim jump multiplier

    // Sprint
    f32                         m_sprintForceMultiplier;            // A multiplier to the work force when sprinting
    f32                         m_sprintEfficiencyMultiplier;       // A multiplier to the efficiency max speed when sprinting
    Angle                       m_sprintStickRangeMinAngleMinSpeed; // Stick range at min speed when sprinting
    Angle                       m_sprintStickRangeMaxAngleMinSpeed; // Stick range at min speed when sprinting
    Angle                       m_sprintStickRangeMinAngleMaxSpeed; // Stick range at max speed when sprinting
    Angle                       m_sprintStickRangeMaxAngleMaxSpeed; // Stick range at max speed when sprinting
    f32                         m_sprintStickRangeMinSpeed;         // Speed to use min stick ranges when sprinting
    f32                         m_sprintStickRangeMaxSpeed;         // Speed to use max stick ranges when sprinting
    u32                         m_sprintStickRangeIndex;

    // Double jump
    f32                         m_doubleJumpForceMultiplier;        // The force multiplier for the double jump

    // Chains
    f32                         m_efficiencyMaxSpeedWithChains;     // Max speed to calculate efficiency when in chains
    f32                         m_jumpMultiplierWithChains;         // A multiplier to the jump when we have chains
    ITF_INLINE f32              getEfficiencyMaxSpeed() { return m_hasChains ? m_efficiencyMaxSpeedWithChains : m_efficiencyMaxSpeed; }

    bbool                       m_lookRight;                        // If the character is looking right (default side)
    bbool                       m_moveSameDir;                      // It's true if we are supposed to keep moving the character in the same dir
    bbool                       m_performingSlide;                  // If the character is sliding
    bbool                       m_lockMoveInput;                    // Lock the moveinput so that we keep the same animation
    bbool                       m_lockLookRight;                    // Lock the lookRight setting
    bbool                       m_queueJump;                        // Queue a jump to process when we can
    bbool                       m_queueHit;                         // Queue a hit to process when we can

    // We leave these as bool so that they show nicely on the property editor
    bbool                       m_displayHangDebug;                 // If we should display the hang debug information
    bbool                       m_drawAnimInputs;                   // Display the values of the anim inputs


    i32                         m_punchLevel;
    u32                         m_punchType;

    Vec2d                       m_previousSpeed;
    Vec2d                       m_bounceSpeed;
    i32                         m_hurtBounceLevel;
    f32                         m_rehitTimer;
    f32                         m_lastJumpTimer;

    EStance                     m_stance;                           // In which stance is the player (standing, hanging, climbing, etc)
    EStance                     m_prevStance;                       // What was the previous stance of the player
    OrientationPose             m_orientationPose;                  // If the current action is done horizontally or vertically
    
    // Hitting
    u32                         m_currentPunchHitType;              // The type of the punch we are performing
    u32                         m_currentPunchHitLevel;             // The level of the punch we are performing
    u32                         m_uTurnHitLevel;                    // The level of the uturn kick
    u32                         m_crouchHitLevel;                   // The level of the crouched kick
    u32                         m_tornadoHitLevel;                  // The level of the tornado kick
    u32                         m_accrobaticTornadoHitLevel;        // The level of the accrobatic tornado kick (in the air)
    u32                         m_jcvdHitLevel;                     // The level of the jean claude van damne kick
    u32                         m_earthQuakeHitLevel;               // The level of the earthquake hit
    u32                         m_uTurnJumpKickHitLevel;            // The level of the uturn jump kick
    u32                         m_slideDownHitLevel;                // The level of the hit when sliding down on a character
    f32                         m_uTurnHitAllowTime;                // We allow to transform the regular hit into a uturn hit during a small delay
    f32                         m_earthquakeSuspensionTime;         // Time to hang in the air while doing the earthquake punch
    f32                         m_earthquakeSuspensionCounter;      // Time counter to be in suspension while performing the earthquake
    f32                         m_earthquakeMinYSpeed;              // The minimum speed in Y to give an upward impulse
    f32                         m_earthquakeYMultiplier;            // The Y speed multiplier when performing the earthquake
    f32                         m_earthquakeDownForce;              // Force applied when we release the earthquake hit
    f32                         m_slideDownHitMinSpeed;             // Minimum speed to release slide down hits
    f32                         m_tornadoMinSpeed;                  // Minimum speed to perform the tornado
    f32                         m_tornadoFriction;                  // Friction multiplier when performing the tornado
    StringID                    m_hitFxBoneName;                    // Name of the bone where we place hit fx
    u32                         m_hitFxBoneIndex;                   // Index of the bone where we place hit fx
    bbool                       m_allowEarthquakePunch;             // If we allow doing an earthquake punch (only allowed if you don't press DOWN for a moment in the air)
    // Hitting shapes
    PhysShapePolygon            m_punchShapeLow;                    // Shape of the stim for the low punch
    PhysShapePolygon            m_punchShapeMed;                    // Shape of the stim for the medium punch
    PhysShapePolygon            m_punchShapeHigh;                   // Shape of the stim for the high punch
    PhysShapePolygon            m_crouchKickShape;                  // Shape for crouched kick
    PhysShapePolygon            m_jcvdKickShape;                    // Shape for Jean Claude Van Damne kick
    PhysShapePolygon            m_uturnKickShape;                   // Shape for uturn kick
    PhysShapePolygon            m_uturnJumpKickShape;               // Shape for uturn jump kick
    PhysShapePolygon            m_slideHitShape;                    // Shape for the slide hit
    PhysShapePolygon            m_earthquakeHitShape;               // Shape for the earthquake hit
    PhysShapePolygon            m_slideDownHitShape;                // Shape for the sliding down hit

    struct HitVictimData
    {
        ObjectRef m_victim;
        u32 m_edgeIndex;
    };

    typedef SafeArray <HitVictimData> HitVictimContainer;

    HitVictimContainer          m_slideHits;                        // Hits of the current slide

    // U-Turn handling
    bbool                       m_performingUTurn;                  // If the character is performing a UTurn (from standing or moving)
    bbool                       m_uTurnFinished;                    // Flag that tells us if the u turn is finished

    // World Values
    Vec2d                       m_currentMove;                      // Move value of the stick
    Vec2d                       m_currentMoveDir;                   // Move value dir of the stick
    Vec2d                       m_prevMove;                         // The previous move of the stick
    Vec2d                       m_prevMoveDir;                      // The previous direction of the stick
    EMoveDir                    m_moveDirection;                    // The direction we are pushing
    EMoveDir                    m_desiredLookDir;                   // The desired look direction

    // Gravity Values
    Vec2d                       m_currentMoveGravity;               // Move value of the stick
    Vec2d                       m_currentMoveDirGravity;            // Move value dir of the stick
    Vec2d                       m_prevMoveGravity;                  // The previous move of the stick
    Vec2d                       m_prevMoveDirGravity;               // The previous direction of the stick
    EMoveDir                    m_moveDirectionGravity;             // The direction we are pushing

    Vec2d                       m_moveInput;                        // The move input we will feed to the animation

    u32                         m_actionFlags;                      // The current actions of the player

    u32                         m_helicoFxHandle;


    bbool                       m_hasChains;
    bbool                       m_hasSoupiere;
    void                        processPickup(War_EventPickup* _evtPickup);
    void                        processTrigger(EventTrigger* _evtTrigger);
};
} // namespace ITF

#endif //_ITF_WAR_PLAYERCONTROLLERCOMPONENT_H_