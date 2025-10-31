#ifndef _ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_
#define _ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_

#ifndef _ITF_RAY_BASICPLAYERCONTROLLERCOMPONENT_H_
#include "rayman/gameplay/Components/Player/Ray_BasicPlayerControllerComponent.h"
#endif //_ITF_RAY_BASICPLAYERCONTROLLERCOMPONENT_H_

#ifndef _ITF_INPUTADAPTER_H_
#include "engine/AdaptersInterfaces/InputAdapter.h"
#endif //_ITF_INPUTADAPTER_H_

#ifndef _ITF_PHYSFORCEMODIFIER_H_
#include "engine/physics/PhysForceModifier.h"
#endif //_ITF_PHYSFORCEMODIFIER_H_

#ifndef _ITF_GAMEPLAYEVENTS_H_
#include "gameplay/GameplayEvents.h"
#endif //_ITF_GAMEPLAYEVENTS_H_

#ifndef _ITF_RAY_GAMEPLAYEVENTS_H_
#include "rayman/gameplay/Ray_GameplayEvents.h"
#endif //_ITF_RAY_GAMEPLAYEVENTS_H_

#ifndef _ITF_RAY_GAMEPLAYTYPES_H_
#include "rayman/gameplay/Ray_GameplayTypes.h"
#endif //_ITF_RAY_GAMEPLAYTYPES_H_

#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

#ifndef _ITF_RAY_GAMEMATERIAL_H_
#include "rayman/gameplay/Ray_GameMaterial.h"
#endif //_ITF_RAY_GAMEMATERIAL_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif

#ifndef _ITF_RAY_PLAYERCONTROLLERPOWERUPDISPLAYS_H_
#include "Ray_PlayerControllerPowerUpDisplays.h"
#endif //_ITF_RAY_PLAYERCONTROLLERPOWERUPDISPLAYS_H_

#ifndef _ITF_PERLIN_NOISE_H_
#include "Core/math/PerlinNoise.h"
#endif // _ITF_PERLIN_NOISE_H_

#ifndef _ITF_INPUTLISTENER_H_
#include "engine/zinput/ZInputListener.h"
#endif //_ITF_INPUTLISTENER_H_

namespace ITF
{

class Ray_PlayerControllerComponent : public Ray_BasicPlayerControllerComponent, IInputListener
{
    friend class Ray_PlayerControllerComponent_Template;

    DECLARE_OBJECT_CHILD_RTTI(Ray_PlayerControllerComponent, Ray_BasicPlayerControllerComponent,2850198285)
    DECLARE_VALIDATE_COMPONENT()

private:

    struct HangingInfo
    {
        HangingInfo() 
            : m_hangPos(Vec2d::Zero)
            , m_polyline(ITF_INVALID_OBJREF)
            , m_edge(U32_INVALID)
            , m_actor(ITF_INVALID_OBJREF)
            , m_lookRight(btrue)
            , m_climb(bfalse)
            , m_coopActionMode(ACTIONCOOPMODE_NONE)
        {}
        Vec2d           m_hangPos;
        ObjectRef       m_polyline;
        u32             m_edge;
        ObjectRef       m_actor;
        bbool           m_lookRight;
        bbool           m_climb;
        EActionCoopMode m_coopActionMode;
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
        ACTIONFLAG_CROUCH       = 0x001,                             // If the character is crouched or not
        ACTIONFLAG_ROLLING      = 0x002,                             // If the character is rolling on the ground
        ACTIONFLAG_SPRINTING    = 0x004,                             // If the character is sprinting
        ACTIONFLAG_LOWROOF      = 0x010,                             // If the roof is low
        ACTIONFLAG_SUPPORTING   = 0x020,                             // If the character is supporting another object
        ACTIONFLAG_SUPPORTED    = 0x040,                             // If the character is being supported
        ACTIONFLAG_LAYING_DOWN  = 0x080,                             // If the character is laying down another character
        ACTIONFLAG_SLIDE        = 0x100,                             // If the character is performing a slide
        ACTIONFLAG_PATHBLOCKED  = 0x400,                             // If the path in front is blocked,
        ENUM_FORCE_SIZE_32(EActionFlag)
    };

    enum ECornerPose
    {
        CORNERPOSE_NONE,
        CORNERPOSE_BACK,
        CORNERPOSE_FRONT,
        ENUM_FORCE_SIZE_32(ECornerPose)
    };

    enum EFrictionMode
    {
        FRICTIONMODE_NORMAL,
        FRICTIONMODE_ROLLING,
        FRICTIONMODE_SLIDE,
        FRICTIONMODE_RECEIVEHIT,
        FRICTIONMODE_TORNADO,
        FRICTIONMODE_REDUCEDSIZEHIT,
        FRICTIONMODE_CRUSH,
        FRICTIONMODE_ICECOMPENSATE,
        FRICTIONMODE_WALLSLIDE,
        ENUM_FORCE_SIZE_32(EFrictionMode)
    };

    enum EGravityMode
    {
        GRAVITYMODE_NORMAL,
        GRAVITYMODE_AIRHIT,
        GRAVITYMODE_RECEIVEHIT,
        GRAVITYMODE_WALLSLIDE,
        GRAVITYMODE_SWIM,
        GRAVITYMODE_SWIMDOLPHIN,
        GRAVITYMODE_DEADSOUL,
        GRAVITYMODE_WALLRUNJUMP,
        GRAVITYMODE_REDUCEDSIZEHIT,
        ENUM_FORCE_SIZE_32(EGravityMode)
    };

    // Move data
    struct MoveData
    {
        MoveData()
            : m_move(Vec2d::Zero)
            , m_moveNormalized(Vec2d::Zero)
            , m_moveReal(Vec2d::Zero)
            , m_moveRealNormalized(Vec2d::Zero)
            , m_prevMove(Vec2d::Zero)
            , m_prevMoveNormalized(Vec2d::Zero)
            , m_moveDirection(MOVEDIR_NONE)
            , m_moveSameDir(bfalse)
        {
        }

    public:

        void                    update( const Vec2d& _move );
        void                    rotate( f32 _angle );
        void                    flip();

        void                    resetSameDir() { m_moveSameDir = bfalse; }
        void                    resetMove() { m_moveDirection = MOVEDIR_NONE; }
        
        EMoveDir                getMoveDirectionFromAngle( f32 _angle ) const;

        const Vec2d&            getMove() const { return m_move; }
        const Vec2d&            getMoveNormalized() const { return m_moveNormalized; }
        const Vec2d&            getMoveReal() const { return m_moveReal; }
        const Vec2d&            getMoveRealNormalized() const { return m_moveRealNormalized; }
        const Vec2d&            getPrevMove() const { return m_prevMove; }
        const Vec2d&            getPrevMoveNormalized() const { return m_prevMoveNormalized; }
        EMoveDir                getMoveDirection() const { return m_moveDirection; }
        bbool                   getMoveSameDir() const { return m_moveSameDir; }
        
    private:

        Vec2d                   m_move;
        Vec2d                   m_moveNormalized;
        Vec2d                   m_moveReal;
        Vec2d                   m_moveRealNormalized;
        Vec2d                   m_prevMove;
        Vec2d                   m_prevMoveNormalized;
        EMoveDir                m_moveDirection;
        bbool                   m_moveSameDir;
    };

#include "Ray_PlayerControllerStates.h"


public:
    DECLARE_SERIALIZE()

    Ray_PlayerControllerComponent();
    virtual ~Ray_PlayerControllerComponent();

    virtual bbool               needsUpdate() const { return btrue; }
    virtual bbool               needsDraw() const { return bfalse; }
    virtual bbool               needsDraw2D() const { return bfalse; }

    virtual void                onBecomeActive();
    virtual void                onBecomeInactive();
    virtual void                onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
    virtual void                onResourceReady();
    virtual void                onActorClearComponents();
    virtual void                onCheckpointLoaded();
    virtual void                Update( f32 _deltaTime );

    void                        processQueryAIData( Ray_EventQueryAIData * aiQuery );
    void                        updateSoftCollisions( f32 _deltaTime );
    virtual void                updateAnimInput();
    virtual void                updateSoundInput();
    virtual void                updateFxInput();
    virtual void                updateWiiInput();

    virtual void                onForceMove( );

    void                        flipMoveData();
    MoveData&                   getMoveData() { return m_move; }
    MoveData&                   getMoveDataAnalog() { return m_moveAnalog; }
    MoveData&                   getMoveDataGravity() { return m_moveGravity; }
    MoveData&                   getMoveDataGravityAnalog() { return m_moveGravityAnalog; }
    const MoveData&             getMoveData() const { return m_move; }
    const MoveData&             getMoveDataAnalog() const { return m_moveAnalog; }
    const MoveData&             getMoveDataGravity() const { return m_moveGravity; }
    const MoveData&             getMoveDataGravityAnalog() const { return m_moveGravityAnalog; }

    EMoveDir                    getLookDirection() const;

    f32                         getCurrentEfficiency()  const { return m_currentEfficiency; }
    bbool                       isHanging()             const { return m_hangingPolyline != ITF_INVALID_OBJREF || m_hangingActor != ITF_INVALID_OBJREF; }
    ObjectRef                   getHangingPolyline()    const { return m_hangingPolyline; }
    u32                         getHangingEdge()        const { return m_hangingEdge; }
    void                        getHangEdgePolyline( class PolyLine** _polyLine, const class PolyLineEdge** _edge ) const;

    bbool                       isClimbing()            const { return m_climbingPolyline != ITF_INVALID_OBJREF; }
    ObjectRef                   getClimbingPolyline()   const { return m_climbingPolyline; }
    u32                         getClimbingEdge()       const { return m_climbingEdge; }
    void                        prepareClimbingJump();

    void                        changeState( PlayerControllerState* _newState);

    EStance                     getStance() const { return m_stance; }

    void                        updateAccrobaticStage( f32 _dt );
    void                        addAccrobaticStage();
    void                        resetAccrobaticStage() { m_currentAccrobaticStage = 0; }
    u32                         getAccrobaticStage() const {  return m_currentAccrobaticStage; }

    // add function for camera
    bbool                       isHelicoptering() const { return m_stance == STANCE_HELICO; }
    bbool                       isFighting() { return m_actionPose == ACTIONPOSE_FIGHT; }
    bbool                       isWallSliding() { return m_stance == STANCE_WALLSLIDE; }
    bbool                       isJumping() { return m_currentState == &m_stateJumping; }
    Vec2d                       getMoveDirNormalized() { return m_move.getMoveNormalized(); }
    bbool                       isPerformingUTurn() const { return m_performingUTurn != UTURNTYPE_NONE; }
    bbool                       isJumpOnAirTimerAllowed() const { return m_jumpOnAirTimer > 0.f; }
    bbool                       isSwimming() const {return m_stance == STANCE_SWIM; }
    ObjectRef                   getSwingOnActor() const {return m_swingOnActor;}    
    f32                         getSwingOnActorLength() const { return m_swingLength; }
    ObjectRef                   getClimbOnPolyline() const { return m_climbingPolyline;}
    bbool                       isClimbOnPolylineVertical() const { return m_climbingVertical;}
    Vec2d                       getCurrentWindForce() const;
    EPunchType                  getCurrentPunchHitType() const {return m_currentPunchHitType;}

    bbool                       isPowerUpEnabled(const StringID& _id) const;

    virtual bbool               isDead() const;
    virtual bbool               isDetectable( const ActorRef _detector ) const;
    bbool                       isInDeadSoul() const { return m_currentState == &m_stateDeadSoul; }

    // InputListener
    virtual void                Receive( u32 _playerIndex, f32 axis, const StringID& action );

#ifdef ITF_SUPPORT_EDITOR
    virtual void                drawEdit( class ActorDrawEditInterface* drawInterface, u32 _flags  ) const;
    virtual void                onEditorMove(bbool _modifyInitialPos = btrue);
#endif // ITF_SUPPORT_EDITOR

private:

    const class Ray_PlayerControllerComponent_Template* getTemplate() const;

    virtual bbool               internalOnEvent( class Event* _event );

    void                        updateTimers( f32 _dt );
    void                        updateStickOnWalls( f32 _dt );
    void                        updateSprint();
    void                        updateStates( f32 _deltaTime );
    void                        updateMoveCursor( f32 _deltaTime );
    void                        updateMoveToPoint();
    void                        updateMoveToPointRight();
    void                        updateMoveToPointLeft();
    void                        updateMoveToPointUp();
    void                        updateBaseInfo( f32 _dt );
    void                        processPendingEvents();
    void                        processDeadPendingEvents();
    void                        resetPendingEvents();
    void                        resetBaseInfo();
    void                        setMoveToPoint( const Vec2d& _pos, f32 _moveToThreshold = 0.5f );
    void                        resetMoveToPoint();
    void                        finishMoveToPoint( const Vec2d& _feetPos );
    void                        disablePlayer( bbool _val );

    void                        updateFx(f32 _deltaTime);

#ifdef ITF_SUPPORT_CHEAT
    void                        updateCheats();
#endif

    void                        UpdateMoveInput( f32 _deltaTime);
    void                        updateCrouch( f32 _deltaTime );

    void                        UpdatePhysState();
    void                        UpdateMoveDir();
    void                        UpdateDesiredLookDir();
    void                        setDesiredLookDir();
    void                        UpdatePhysicWalk( f32 _deltaTime );
    void                        UpdatePhysicAir( bbool _allowMagnetAttraction = bfalse );
    void                        UpdatePhysicHairlicopter( f32 _dt );
    void                        UpdatePhysicCrushHit( f32 _dt );
    void                        preparePhysicAirHit();
    void                        cancelPhysicAirHit();
    void                        resetPhysicAirHit();
#ifdef ITF_SUPPORT_CHEAT
    void                        UpdatePhysicFlyMode( f32 _deltaTime );
#endif
    void                        UpdatePhysicClimbingEdge( f32 _dt );
    void                        updateClimbEdgeOrientation( f32 _dt );
    void                        updateClimbEdgeMove( f32 _dt, const PolyLine* _polyLine, const PolyLineEdge* _edge, Vec2d& _moveInput );
    void                        updateClimbEdgeApplyMove( f32 _dt, const PolyLineEdge* _edge, f32& _move, f32& _moveSign, EMoveDir& _climbDir );
    void                        updateClimbEdgeAdjustClimbT( const PolyLine* _polyLine, const PolyLineEdge* _edge, f32& _move );
    void                        updateClimbEdgeSetAnimData( f32 _dt, const class PolyLineEdge* _edge, f32 _move, f32 _moveSign, Vec2d& _moveInput );
    void                        updateClimbEdgeUpdateSprinting( EMoveDir _climbDir );
    void                        updateClimbEdgeGetMoveData( const PolyLineEdge* _edge, f32& _moveSign, EMoveDir& _climbDir, f32& _move ) const;
    void                        updateClimbEdgeGetMoveSize( f32 _deltaTime, f32 _moveDir, EMoveDir _climbDir, f32& _moveSize ) const;
    void                        updateClimbEdgeUpdateInertia( f32 _deltaTime, EMoveDir _climbDir, f32 _move );
    f32                         updateClimbEdgeGetClimbSignFromClimbDir( EMoveDir _moveDir, bbool _vertical, const class PolyLineEdge* _edge ) const;
    EMoveDir                    updateClimbEdgeGetClimbDirFromMoveSign( f32 _moveSign, bbool _vertical, const class PolyLineEdge* _edge ) const;
    void                        updateClimbEdgeAllowHangCoop( PolyLine* _polyLine, u32 _edgeIndex );
    void                        UpdatePhysicSwim( f32 _dt );
    void                        UpdateMoveTargetMultiplier( f32 _deltaTime );
    void                        UpdateSupport(f32 _dt);
    void                        UpdateSupported(f32 _dt);
    void                        updateSquadData(f32 _deltaTime);
    void                        updateIdleTimer(f32 _dt, bbool _analog );
    void                        updateGravityMode( f32 _dt );
    f32                         getMoveTargetBlendTime() const;
    void                        updateSprintRelease( f32 _dt );
    void                        updateIceCompensate();
    void                        helicoProcessWind();

    void                        updateClimbSwingImpulse( const PolyLine* _polyLine, Vec2d& _moveInput );

    bbool                       useCameraLimits() const;
    bbool                       isCameraLimited() const;
    void                        updateCameraLimiter( f32 _dt );

    bbool                       isTryingMove() const;
    void                        checkSquash();

    void                        PerformJump();
    void                        addJumpAirImpulse();
    void                        performJumpProcessImpulse( const Vec2d& _gravityDir,
                                                           const Vec2d& _gravityNormal,
                                                           const Vec2d& _setSpeed,
                                                           Vec2d& _addForce ) const;
    void                        performJumpProcessStances( const Vec2d& _originalSpeed,
                                                           const Vec2d& _gravityNormal,
                                                           Vec2d& _setSpeed,
                                                           Vec2d& _addForce,
                                                           f32& _airControlDelay,
                                                           bbool& _airControlAllowHelico,
                                                           bbool& _wallSlideAirControl,
                                                           bbool& _airControlDisableBlend,
                                                           f32& _airControlDisableAmount ) const;
    void                        performJumpAddMovingPlatform( const Vec2d& _gravityDir,
                                                              const Vec2d& _gravityNormal,
                                                              Vec2d& _setSpeed,
                                                              Vec2d& _addForce  ) const;

    bbool                       tryWallJump( bbool _fromGround );
    bbool                       isEdgeGoodForWallJump( PolyLine* _polyLine, u32 _edgeIndex, const PolyLineEdge* _edge, bbool& _normalJump ) const;
    void                        calculateEfficiency();
    void                        calculateAchievedSpeedFactor( const Vec2d& _currentTarget, f32 _deltaTime );
    void                        updateAchievedSpeedFactor( f32 _deltaTime );
    void                        resetAchievedSpeedFactor();
    f32                         getWalkMaxSpeed() const;
    void                        sendWindStim( const Vec2d& _dir, const PhysForceModifier_Instance& _forceModifier );

    void                        processCrush( EventCrushed* _crushed );
    void                        processPhysicsUpdated();

    // Polyline Events
    void                        processEventPolylineUpdate( class EventStickOnPolylineUpdate* _event );
    void                        processBlockedByPolylineEvent( class EventBlockedByPolyline* _event );
    void                        checkBounceEdge();
    void                        resetBounce();

    void                        processBounceStim( class BounceStim* _stim );
    void                        processBounce( u32 _dangerLevel, Vec2d _bounceDirection, BounceType _bounceType, Vec2d _speedAtBounce, StringID _bounceGameMaterial );
    void                        getCurrentBounceData( f32& _height, f32& _airControlDelay) const;
    void                        performBounce();
    void                        performBounceJump();
    void                        performBounceWindTunnel();
    void                        setBounceEdgeCheck( const Vec2d& _dir, u32 _danger, ObjectRef _poly, u32 _edgeIndex, const Vec2d& _speedAtCollision, const StringID& _gameMat );
    void                        resetBounceEdgeCheck();

    void                        detachFromEverything();
    void                        processDetachEvent( EventDetach* _detachEvent );


    // Grab things in the air (can be hanging or climbing)
    void                        setHangState( struct HangingInfo& _info );
    bbool                       tryHanging( HangingInfo & _info ) const;
    void                        calculateHangPosCheck( PolyLine* _polyLine,
                                                       const PolyLineEdge& _edge,
                                                       u32 _edgeIndex,
                                                       bbool _rightSide,
                                                       Vec2d& _pos, f32& _angle ) const;

    // Hanging
    bbool                       processContactHangSide( const struct SCollidableContact& _contact,
                                                        PolyLine* _polyLine, const PolyLineEdge& _edge,
                                                        u32 _edgeIndex, const Vec2d& _headPos,
                                                        HangingInfo& _info ) const;
    bbool                       hangFromPoint( HangingInfo & _info);
    void                        clearHanging();
    bbool                       isValidPointForHanging( PolyLine* _poly,
                                                        const PolyLineEdge& _edge,
                                                        u32 _edgeIndex, bbool _right ) const;
    void                        getHangCoopCollisionData( Actor* supporter, Vec2d& _collisionPos, f32& _angle, Vec2d& _snapPos, bbool& _lookRight ) const;
    bbool                       isCurrentHangColliding() const;
    bbool                       isCurrentHangValid() const;
    bbool                       isHangTopColliding() const;
    bbool                       isHangingPosColliding( const Vec2d& _collisionCheckPos, f32 _collisionCheckAngle,
                                                       bbool _actor ) const;
    bbool                       getWorldHangPos( Vec2d& _ret, f32& _angle ) const;
    bbool                       isEdgeDirValidToHang( PolyLine* _poly,
                                                      u32 _edgeIndex,
                                                      const PolyLineEdge& _edge,
                                                      bbool _right ) const;
    bbool                       tryHangFromPolyline( class PolyLine* _poly, const class PolyLineEdge* _edge,
                                                     u32 _edgeIndex, bbool _rightSide,
                                                     HangingInfo& _info );

    // Climbing (ropes, edges, etc)
    bbool                       processContactClimbActor( Actor* _victim, const Vec2d& _headPos, HangingInfo& _info ) const;
    bbool                       processContactClimbPolyline( const struct SCollidableContact& _contact,
                                                     PolyLine* _polyLine,
                                                     const PolyLineEdge& _edge,
                                                     const Vec2d& _headPos,
                                                     HangingInfo& _info ) const;
    bbool                       checkClimbCollision( ObjectRef _climbPoly, u32 _edgeIndex, const Vec2d& _climbPos, f32 _climbAngle ) const;
    bbool                       climbFromPoint( HangingInfo & _info);
    void                        clearClimbing();
    void                        getClimbEdgePolyline( class PolyLine** _polyLine, const class PolyLineEdge** _edge ) const;
    bbool                       getWorldClimbPos( Vec2d& _ret, f32& _angle ) const;

    void                        receivePunch    (const class PunchStim* _punch );
    void                        updateWeapon();

    bbool                       startNormalPunch();

    // Swiming
    bbool                       trySwim( bbool _checkSpeed );
    void                        setSwimingPolyline( const class PolyLine* _polyLine );
    void                        swimingUpdatePolylines( const class PolyLine* _polyLine, f32 _dt );
    void                        swimingUpdateAirFriction();
    void                        setSwimTargetRotation( f32 _target );
    void                        resetSwimTargetRotation( bbool& _performTwist );
    bbool                       trySwimSprintJump();
    void                        setupSwimDolphinJump();
    void                        resetSwimUpsideDown();
    bbool                       shouldExitSwiming() const;
    void                        swimUpdateDive( f32 _dt, const PolyLineEdge& _surfaceEdge, bbool& _performTwist );
    void                        swimUpdateFloat( f32 _dt, const class PolyLineEdge& _surfaceEdge, bbool& _performTwist );
    void                        swimProcessPush( Vec2d& _moveDir, Vec2d& _moveAxis );
    void                        UpdatePhysicSwimRotation( f32 _dt, bbool _performTwist );
    void                        UpdatePhysicSwimingCurve( f32 _dt, f32 targetDif );
    void                        swimSetSwimForce( f32 _dt, Vec2d& _moveDir, f32 _moveSize, bbool _float );
    void                        swimSetFloatForce( const PolyLineEdge& _surfaceEdge );
    void                        swimApplyForces( f32 _dt, bbool _mustFloat );
    bbool                       handleSwimInsideOutside( f32 _dt, const PolyLineEdge* _closestEdge );
    bbool                       swimCanMove() const;
    void                        checkAchievementPiranha(const PolyLine * const _previousWaterPolyline, const PolyLine * const _nextWaterPolyline);


    // feedback handling section
    void                        handleChangeStateFeedback() const;
    void                        handleLandingFeedback( class FXEmitterComponent* _fx, const PolyLineEdge* _edge, Vec3d& _col) const;
    void                        handleJumpingFeedback( class FXEmitterComponent* _fx, const PolyLineEdge* _edge, Vec3d& _col) const;
    void                        handleAnimTrackEvent(AnimGameplayEvent* _event);

    // Set the stance
    void                        setStance( EStance _stance );
    void                        resetStance( EStance _stance ) ;
    void                        changeStanceHelico();
    void                        changeStanceStand();
    void                        changeStanceSwim();
    void                        changeStanceWallSlide();
    void                        changeStanceHang();
    void                        changeStanceClimb();

    // Action Pose
    void                        setActionPose( EActionPose _pose );
    void                        updateActionPose( f32 _dt );

    // Mode Zone
    void                        updateMoodZone();

    // Currents
    void                        updateRegionCurrent( f32 _dt );

    // U-Turn
    void                        startUTurn( EUturnType _type );
    void                        resetUTurn();    

    // Sliding
    void                        setSlide( bbool _val );
    bbool                       isPerformingSlide() const { return isActionFlagSet(ACTIONFLAG_SLIDE); }
    f32                         getMinSlideSpeed( f32 _friction ) const;

    // Crouch
    void                        setCrouch( bbool _val );
    bbool                       isCrouch() const { return isActionFlagSet(ACTIONFLAG_CROUCH); }

    // Rolling
    void                        setRolling( bbool _val );
    bbool                       isRolling() const { return isActionFlagSet(ACTIONFLAG_ROLLING); }

    // Sprinting
    void                        setSprinting( bbool _val );
    bbool                       isSprinting() const { return isActionFlagSet(ACTIONFLAG_SPRINTING); }
    bbool                       trySprint() const;

    // Low Roof
    void                        setLowRoof( bbool _val );
    bbool                       isLowRoof() const { return isActionFlagSet(ACTIONFLAG_LOWROOF); }

    // Supporting   
    void                        setSupporting( bbool _val );
    bbool                       isSupporting() const { return isActionFlagSet(ACTIONFLAG_SUPPORTING); }
    bbool                       isSupportHolding() const { return isSupporting() && m_objectsOnPedestal.size() != 0; }
    bbool                       isOnPedestal() const { return m_onPedestal != ITF_INVALID_OBJREF; }
    Vec2d                       getPedestalPos() const;
    void                        startOnPedestal( ObjectRef _pedestalActor, const Vec2d& _pedestalPos );
    void                        stopOnPedestal();
    void                        checkSetPedestal( bbool& exitCornerPose, bbool& allowSupport, bbool& allowCrouch );

    // Supported   
    void                        setSupported( bbool _val );
    bbool                       isSupported() const { return isActionFlagSet(ACTIONFLAG_SUPPORTED); }

    // Laying down   
    void                        setLayingDown( bbool _val );
    bbool                       isLayingDown() const { return isActionFlagSet(ACTIONFLAG_LAYING_DOWN); }

    // Path blocked
    void                        setPathBlocked( bbool _val );
    bbool                       isPathBlocked() const { return isActionFlagSet(ACTIONFLAG_PATHBLOCKED); }

    // Look right
    void                        setLookRight( bbool _lookRight );

    void                        actorLock();
    void                        clearActorLock();
    void                        updateActorLock();
    bbool                       isActorLocked() const { return m_actorLocked;}

    // Input
    void                        setMoveInput( const Vec2d& _moveInput ) { m_moveInput = _moveInput; }
    void                        resetDesiredLookDir();

    //
    virtual void                processTeleport( EventTeleport* _eventTeleport );
    virtual void                processPlayerIndexChange( class EventPlayerIndexChange* _playerIndex );

    // 
    void                        processInteractionQuery(EventInteractionQuery * _event);
    void                        processSnapDataQuery(EventQuerySnapData* _event);
    void                        processSwingLatchEvent( class Ray_EventSwingLatch* _event );
    void                        processHangOnCharacter( class Ray_EventHangOnCharacter* _event );
    void                        processQueryHangInfo( class Ray_EventQueryHangInfo* _event );

    // Join game, checkpoint handling, dead sould / revive
    void                        processPlayerActivationChanged( EventPlayerActivationChanged* _eventActivation );
    void                        changePlayerMode( Player *player,  GameMode _mode);
    void                        processPlayerModeChanged( EventPlayerModeChanged* _eventGameMode );
    void                        processPlayerModeSet( EventPlayerModeSet* _eventGameModeSet );
    void                        processPlayerHpChanged( EventPlayerHpChanged* _eventHP );
    void                        processRevertToLastCheckpoint( EventRevertToLastCheckpoint* _eventRevert );
	void                        processChangePage( Ray_EventChangePage* _eventChangePage );
    void                        processTriggerVictory( Ray_EventTriggerVictory* _victory );
    void                        processSequenceActorPrepare( class EventSequenceActorPrepare* _sequenceEvent );
    void                        processSequenceActorActivate( class EventSequenceActorActivate* _sequenceEvent );
    void                        processSequenceActorReady( class EventSequenceActorReady* _sequenceEvent );
    void                        processReset();
    void                        processScaleChanged();
    void                        processBounceToLayer( class Ray_EventBounceToLayer* _bounce );
    void                        processJumpToPos( class Ray_EventJumpToPos* _jump );
    void                        processStargateUse( class Ray_EventStargateUse* _event );
    void                        processCheckPedestalSupport( class Ray_EventCheckPedestalSupport * _pedestalSupportCheck );
    void                        processQueryPedestalInfo( class Ray_EventQueryPedestalInfo * _pedestalSupportCheck );
    void                        processReleasePedestal( class Ray_EventReleasePedestal * _releasePedestal );
    void                        processScaleFunnelTrigger( class Ray_EventScaleFunnelTrigger * _scaleFunnelTrigger );
    void                        processAINetworkWaypointEvent( class Ray_EventAINetworkWaypoint * _aiNetworkWaypoint );

    void                        processMagnetTrigger( class Ray_EventMagnetTrigger* _magnetTrigger );
    Vec2d                       getMagnetForce( bbool _allowMagnetAttraction );

    // Returns true if the user is not expecting any action
    bbool                       playerIsIdle() const;

    void                        teleportToCurrentSpawnPoint( bbool& _joinAlive );

    bbool                       isJumpQueued() const { return m_jumpQueueTimer!=0.f; }
    bbool                       isHitQueued() const { return m_hitQueueTimer!=0.f && m_hitDisabledTimer == 0.f; }

    bbool                       isOnGeyser() const;

    bbool                       tryWallSlide( f32 _dt );
    bbool                       isPolylineEdgeGoodForWallSlide( PolyLine* _poly, const PolyLineEdge* _edge, u32 _edgeIndex, bbool _fromHang ) const;
    bbool                       findWallSlidingPolyline( ObjectRef _currentPoly, u32 _currentEdge, PolyLine** _poly, u32* _edge, bbool _fromHang, bbool _speedTest ) const;
    void                        setPrevWallSlide( ObjectRef _poly, u32 _edge );
    void                        resetPrevWallSlide();

    void                        setCurrentPunch( EPunchType _type, u32 _level );
    void                        updateCurrentPunchOrientation();

    void                        updateLookRightFromMoveDirection( EMoveDir _moveDir );

    bbool                       checkFallOnCharacter();

    void                        checkLowRoof();

    void                        setActionFlag( EActionFlag _flag ) { m_actionFlags |= _flag; }
    void                        removeActionFlag( EActionFlag _flag ) { m_actionFlags &= (~_flag); }
    void                        toggleActionFlag( EActionFlag _flag );
    bbool                       isActionFlagSet( EActionFlag _flag ) const { return (m_actionFlags&_flag)==static_cast<u32>(_flag); }

    bbool                       setAirPunch();
    bbool                       setIdlePunch();
    bbool                       setWaterPunch();
    void                        setUTurnKick();


    Vec3d                       getHitFxPos( const Vec3d& _default );
    Vec3d                       getBonePos( u32 _index, const Vec3d& _default ) const;

    void                        receiveDamage( i32 _points );
    void                        processInmunity( f32 _dt );

    void                        setPhantomShape();
    void                        resetStandPhantomShapeScaled();

    void                        setJumpData( f32 _jumpMultiplier = 1.f, f32 _jumpMultiplierNoImpulse = 1.f, f32 _jumpImpulseX = 0.f )
    {
         m_currentJumpMultiplier = _jumpMultiplier;
         m_currentJumpMultiplierNoImpulse = _jumpMultiplierNoImpulse;
         m_currentJumpImpulseX = _jumpImpulseX;
    }

    void                        setSurfingTimer();

    bbool                       tryStepMiniJump() const;

    // tip-toes corner pose
    ECornerPose                 calculateCornerPose( ) const;
    void                        exitCornerPose( ECornerPose _pose, f32 _dt );

    void                        setFrictionMode( EFrictionMode _frictionMode, f32 _value );
    void                        resetFrictionMode( EFrictionMode _frictionMode );

    void                        setGravityMode( EGravityMode _frictionMode, f32 _value );
    void                        resetGravityMode( EGravityMode _frictionMode );

    bbool                       isInDeadRegion() const;
    bbool                       isInNoFriendlyFireRegion() const;

    void                        removeAllHealth();
    void                        triggerDeath( EDeathType _type );
    void                        resetDeath();

    void                        setLastHitNature( EHitNature _hitNature );

    bbool                       shouldTriggerHelicopterAction() const;
    bbool                       shouldTriggerHelicoFromHold() const;
    bbool                       isPressingHelicopterAction() const;
    bbool                       isHoldingHelicopterAction() const;

    f32                         calculateHeight() const;

    void                        checkSetAirBrake();

    // SOUND FX
    void                        startSlideFx();
    void                        stopSlideFx();
    bbool                       isSlideFx();
    void                        startWallSlideFx(const class PolyLine* _poly, u32 _edge);
    void                        stopWallSlideFx();
    void                        startJumpFx();
    void                        startBounceOnLineFx(const GameMaterial_Template * _mat);
    void                        stopJumpFx();
    void                        startDiveFx();
    void                        startDiveOutFx();
    void                        startHelicoFx();
    void                        stopHelicoFx();
    void                        startDeadSoulFx();
    void                        stopDeadSoulFx();
    void                        startLandFx(const GameMaterial_Template * _mat);
    void                        startWallLandFx(const GameMaterial_Template * _mat);
    void                        updateLianaSlideFx();
    void                        stopLianaSlideFx();
    void                        updateSprintFx();
    void                        startSprintFx();
    void                        stopSprintFx();
    void                        startSprintReducedFx();
    void                        stopSprintReducedFx();
    void                        startWallRunFx();
    void                        stopWallRunFx();
    void                        startWallRunReducedFx();
    void                        stopWallRunReducedFx();
    u32                         m_slideFxHandle;
    u32                         m_wallSlideFxHandle;
    u32                         m_deadSoulFxHandle;
    u32                         m_jumpFxHandle;
    u32                         m_lianaSlideFx;
    u32                         m_sprintFxHandle;
    u32                         m_sprintReducedFxHandle;
    u32                         m_wallRunFxHandle;
    u32                         m_wallRunReducedFxHandle;


    void                        processInstantKill();
    void                        clearSwing();
    bbool                       hasOtherLivePlayer();
    bbool                       hasOtherLivePlayerOnscreen();

    f32                         getSoftCollisionRadius() const;

public:
    bbool                       isImmune() const;
    void                        disableSoftCollision();
    void                        restoreSoftCollision();
    bbool                       isSoftCollisionEnabled(){ return !m_softCollisionDisabled;}
    
    void                        setMaxSpeedLimit( bbool _val );

    void                        processPowerUp(const Ray_EventPowerUp* _powerUp);

    bbool                       isSizeReduced() const;
    ECharacterSize              getCharacterSize() const;

    void                        playSnapFX();

    void                        updateCheatAllPlayersTogether();

    void                        cleanListener();
    void                        addListener();
    void                        removeListener();

    void                        computeCurrentRegions();
    bbool                       isPosInWater( const Vec3d& _pos ) const;

    void                        setIdleState();

    static const f32            SURVIVE_PIRANHAS_MIN_TIME_INSIDE;
    static const f32            SURVIVE_PIRANHAS_MIN_TIME_OUTSIDE;
    static const f32            SURVIVE_PIRANHAS_COUNTDOWN_AFTER_HIT;

	StateIdle                   m_stateIdle;
    StateHitPrepare             m_statePrepareHit;
    StateHitRelease             m_stateReleaseHit;
    StateJump                   m_stateJumping;
    StateHelicopter             m_stateHelicopter;
    StateFalling                m_stateFalling;
    StateHanging                m_stateHanging;
    StateIdleToHang             m_stateIdleToHang;
    StateHangToIdle             m_stateHangToIdle;
    StateHangToJump             m_stateHangToJump;
    StateCoopHangClimb          m_stateCoopHangClimb;
    StateHitReceive             m_stateReceiveHit;
    StateHitReceiveCrash        m_stateReceiveHitCrash;
    StateClimbing               m_stateClimbing;
#ifdef ITF_SUPPORT_CHEAT
    StateFlyMode                m_stateFlyMode;
#endif
    StatePunchComboMitraille    m_statePunchMitraille;
    StateWallSlide              m_stateWallSlide;
    StateCrushedByCharacter     m_stateCrushedByCharacter;
    StateDeadSoul               m_stateDeadSoul;
    StateRevive                 m_stateRevive;
    StateHangToStanding         m_stateHangToStand;
    StateLock                   m_stateLock;
    StateSnapDance              m_stateSnapDance;
    StatePlayAnim               m_statePlayAnim;
    StateSwimIdle               m_stateSwimIdle;
    StateSwing                  m_stateSwing;
    StateStartLevel             m_stateStartLevel;
    StateExitLevel              m_stateExitLevel;
    StateDeath                  m_stateDeath;
    StateBounceToLayer          m_stateBounceToLayer;
    StateJumpToPos              m_stateJumpToPos;
    StateCaught                 m_stateCaught;
    StateCaughtInPipe           m_stateCaughtInPipe;
    StateStargate               m_stateStargate;
    StateOnPedestal             m_stateOnPedestal;

    static const f32            s_achievedSpeedCalculationDelay;

    // Hanging state data
    ObjectRef                   m_hangingPolyline;                  // The polyline we are hanging from
    ObjectRef                   m_hangingActor;                     // The actor we are hanging from
    ObjectRef                   m_previousHangingPolyline;          // The previous polyline we were hanging from
    ObjectRef                   m_previousHangingActor;             // The previous actor we were hanging from
    u32                         m_hangingEdge;                      // The edge from the polyline where we are hanging
    HangingInfo                 m_hangInfo;                         // The info of the current hanging polyline

    // Climbing state data
    ObjectRef                   m_climbingPolyline;                 // The polyline we are hanging from
    ObjectRef                   m_previousClimbingPolyline;         // The previous polyline we were climbing
    u32                         m_climbingEdge;                     // The edge from the polyline where we are hanging
    f32                         m_climbingT;                        // Position inside the edge we are climbing (from 0 to 1)
    bbool                       m_climbingVertical;                 // If the climbing stance is for vertical or horizontal polylines
    bbool                       m_climbingSlide;                    // If we slide straight away to the bottom on this rope
    bbool                       m_climbingForceSidewaysJump;        // If when jumping we force the sideways jump
    PhysShapeBox                m_climbingCollisionCheckShape;      // Shape used to check collision when climbing
    u32                         m_climbingConstrainedEdge;
    f32                         m_climbingConstrainedT;
    f32                         m_climbingIntertia;
    f32                         m_climbingPrevMoveSign;
    bbool                       m_climbingOnBottomCorner;           // If Rayman is standing at the end of the rope
    bbool                       m_climbingAllowDropVertical;        // We allow dropping when a player reached the end of the rope (for vertical)
                                                                    // We always allow dropping on horizontal
    bbool                       m_climbingAllowExitHorizontal;      // Special flag to say if we allow unhanging from horizontal ropes. It's special
                                                                    // because it's handled from the idleToClimb state
    bbool                       m_climbingJustStarted;
    bbool                       m_climbingAllowHangCoop;            // If he can support someone for coop hanging


    // Jump multipliers
    f32                         m_currentJumpMultiplier;            // The current Jump multiplier
    f32                         m_currentJumpMultiplierNoImpulse;   // The current Jump multiplier with no impulse
    f32                         m_currentJumpImpulseX;              // The current Jump impulse if we press left or right
    f32                         m_currentJumpForceDisableAirControl;// Force air control disable
    Vec2d                       m_currentJumpSpeedX;                // The current Jump xSpeed we must keep
    bbool                       m_currentJumpUseWallDir;            // If the current jump should work as a wall jump
    bbool                       m_currentJumpForceSideways;         // If we want to force the current jump as a sideways jump
    bbool                       m_currentJumpWallRun;               // If the jump we are performing is a wallrun jump
    bbool                       m_currentJumpQueueCrushAttack;      // If we must queue a crush attack with the jump
    bbool                       m_currentJumpKeepSpeedX;            // If we must keep the x speed for this jump

    // Fall
    bbool                       m_fallFromRoof;

    // Swiming state data
    ObjectRef                   m_swimingPolyline;                  // The polyline where we are swiming
    f32                         m_swimingWaterResistance;           // Density of the water while swiming
    bbool                       m_swimingApplyEnterWaterResistance; // If we need to apply the water resistance multiplier when we enter the water
    u32                         m_swimingClosestEdge;               // The closest edge on the swiming polyline
    f32                         m_swimingClosestEdgeT;              // The closest edge point on the swiming polyline
    u32                         m_swimingSurfaceEdge;               // The surface edge on the swiming polyline
    f32                         m_swimingSurfaceT;                  // The surface point on the swiming polyline
    f32                         m_swimingTime;                      // Time the character has been in the water
    f32                         m_swimingTargetRotation;            // The rotation we try to achieve
    f32                         m_swimingCurrentAngularSpeed;       // Current Angular speed
    f32                         m_swimingCurveCursor;               // Current value of the swiming curve cursor
    f32                         m_swimingCurveTarget;               // The target of the swiming curve cursor
    f32                         m_swimingCurveSpeed;                // The current speed of the swiming curve cursor
    f32                         m_swimingStandPose;                 // We stay in the stand pose if m_swimingStandPose > 1
    f32                         m_swimingSprintJumpGravityCounter;  // Counter for the gravity multiplier
    f32                         m_swimingDashCounter;               // Swiming dash counter, we sprint while this counter is > 0
    Vec2d                       m_swimingFloatForce;                // The current floating force
    bbool                       m_swimingIsInside;                  // If we are inside the swiming polyline
    bbool                       m_swimingIsOnSurface;               // If we are on the surface of the swiming polyline
    bbool                       m_swimingIsOnJumpRange;             // If we are inside the range of being able to jump
    bbool                       m_swimingEnterDown;                 // If we entered the water head first
    bbool                       m_swimingTwist;                     // If we are turning the character down to up
    bbool                       m_survivePiranhaPondInside;         // If the current swimming polyline is containing piranhas and player has not been hit by them yet
    bbool                       m_survivePiranhaPondOutside;        // If player just went out a piranhas pond without taking damages
    f32                         m_survivePiranhaPondTimer;          // Time the player has spent inside the piranhas polyline
    f32                         m_survivePiranhaPondLastHitTimer;   // Countdown afterhit inside a piranhas pond
    
    // Phantom data for receiving stims from the world
    PhysShape*                  m_phantomShapeScaled;               // The regular standing shape but scaled

    bbool                       m_airApplyBrake;                    // True if we should brake while in the air
    f32                         m_airControlDisableDelay;           // Time while we disable the air control
    f32                         m_airControlDisableTotal;           // The total time of the air control disabled
    bbool                       m_airControlDisableAllowHelico;     // If we allow the helico while the air control is disabled
    bbool                       m_airControlDisableBlend;           // If we blend the disabled air control
    f32                         m_airControlDisableAmount;          // How much is the air control disabled (0 = no control, 1 = full control)
    bbool                       m_airControlJustFinished;           // If the air control finished this frame
    f32                         m_climbActivationCounter;           // If the counter is != 0.f we don't try to climb on the previous polyline
    f32                         m_moveTargetBlendTimer;             // The timer to go from one speed multiplier to the other
    f32                         m_currentMoveTargetMultiplier;      // The current move target multiplier
    f32                         m_jumpQueueTimer;                   // If we come in contact with an edge while this timer is on then we jump
    f32                         m_hitQueueTimer;                    // Timer to perform a hit during a timeframe
    f32                         m_hitDisabledTimer;                 // Timer to disable the hit for some occasions
    f32                         m_jumpOnAirTimer;                   // This timer allows us to jump while on the air when we just dropped from an edge
    f32                         m_surfingTimer;                     // Surfing timer to disable entering surfing and being able to jump
    Vec2d                       m_jumpOnAirEdge;                    // The last edge recorded for the jump on air
    f32                         m_currentEfficiency;                // Current efficiency
    f32                         m_achievedSpeedCalculationTimer;    // A timer to calculate the right efficiency when moving
    f32                         m_achievedSpeedFactor;              // How close am I to the speed I want to achieve
    f32                         m_targetAchievedSpeedFactor;        // The target achieved speed factor
    f32                         m_achievedSpeedFactorV;             // Velocity to move towards the target m_targetAchievedSpeedFactor
    ECornerPose                 m_onCorner;                         // The character is on the corner of a polyline
    PhysShapeBox                m_hangingCollisionShapeSolid;       // Shape used to check collision when hanging on solid ground
    PhysShapeBox                m_hangingCollisionShapeCoop;        // Shape used to check collision when hanging from a character
    f32                         m_hangActivationCounter;        // If the counter is != 0.f we don't try to hang on the previous polyline
    f32                         m_helicopterSuspensionTimer;        // Timer for suspension
    f32                         m_helicopterCooldown;
    f32                         m_airHitForceApplicationTimer;      // airHit mode timer to full force application
    bbool                       m_helicopterCooldownJustFinished;   // If the helicopter cool down just expired
    bbool                       m_airHitInProgress;
    bbool                       m_airHitPrepared;
    f32                         m_airHitForceMultiplier;

    ActorRef                    m_bounceSender;                     // The actor that sent us the bounce
    Vec2d                       m_bounceCurrentDir;                 // The direction we use to bounce
    Vec2d                       m_bounceSpeedAtTime;                // The speed at the moment of the bounce
    Vec2d                       m_bounceCurrentOrigin;              // The origin of the bounce
    BounceType                  m_bounceCurrentType;                // Type of bounce (enemy or bumper)
    StringID                    m_bounceCurrentGameMaterial;        // GameMaterial of bounce edge
    BounceReactType             m_bounceCurrentReactType;           // The level of the jump
    Vec2d                       m_bounceRepositionTotal;            // Movement to apply to position the character on the proper bounce path
    Vec2d                       m_bounceRepositionFinalSpeed;       // The final speed after the reposition
    f32                         m_bounceRepositionTotalTime;        // The total time to reposition
    f32                         m_bounceRepositionCounter;          // The current time for positioning the character on the proper bounce path
    f32                         m_bounceMultiplier;                 // A multiplier to the bouncing force
    u32                         m_bounceCurrentDangerLevel;         // The damage level of the current bounce
    f32                         m_bounceJumpAllowTimer;             // A timer to allow jumping while bouncing
    f32                         m_bounceTimer;                      // delay counter to make the bounce
    Vec2d                       m_bounceCheckEdgeDir;               // Check if we bounce on this edge
    u32                         m_bounceCheckEdgeDangerLevel;       // Danger level of the edge where we have to check if we bounce
    ObjectRef                   m_bounceCheckEdgePolyRef;           // Polyline reference of the edge where we check if we bounce
    u32                         m_bounceCheckEdgeIndex;             // Index of the bouncing edge
    StringID                    m_bounceCheckEdgeGameMaterial;      // The game material of the edge where we check if we bounce
    Vec2d                       m_bounceCheckEdgeSpeed;             // The speed at the time we hit the edge
    ActorList                   m_bouncePropagation;                // List of actors to propagate the bounce
    ObjectRef                   m_bouncePrevFramePoly;              // The polyline where we bounced on the last frame
    f32                         m_windDisableCounter;               // Timer to leave the wind disabled
    u32                         m_currentAccrobaticStage;           // The current accrobatic stage
    bbool                       m_nextJumpIsAccrobatic;
    f32                         m_accrobaticStageKeepCounter;       // Timer to keep the accrobatic stage
    ObjectRef                   m_receivedHitInstigator;            // The instigator of the received hit
    u32                         m_receivedHitLevel;                 // The level of the hit received
    u32                         m_receivedHitDamage;                // The damage inflicted by the hit received
    Vec2d                       m_receivedHitDir;                   // The direction of the hit received
    EReceivedHitType            m_receivedHitType;                  // The received hit type
    Vec3d                       m_receivedHitFXPos;                 // The received hit FX position
    Vec2d                       m_receivedHitPosition;              // Average position of received hit contacts
    Vec2d                       m_currentTargetForce;               // The current force applied to the physics
    Vec2d                       m_currentGravity;                   // The current gravity in our position
    Vec2d                       m_currentGravityDir;                // The current gravity direction in our position
    Vec2d                       m_prevPos;
    bbool                       m_prevWasSprint;                    // For the rewards, if we were sprinting
    bbool                       m_checkSprintAward;                 // If we should check for the sprint award
    SafeArray <ObjectRef>       m_kungFuComboTarget;                // The target for the kungFu Combo

    ObjectRef                   m_currentMagnet;                    // The magnet we're close to, if any (use to recover from the crab pit)
    f32                         m_magnetForce;                      // The force applied when attracted by a magnet

    Vec2d                       m_wallSlideDir;                     // The current direction of the wall for wall slide
    f32                         m_wallSlideStickDelayCounter;       // A counter to prevent from wallsliding
    ObjectRef                   m_wallSlidePrevPoly;                // The polyline of the previous wallslide
    u32                         m_wallSlidePrevEdge;                // The polyline edge of the previous wallslide
    bbool                       m_wallSlideAirControl;              // Use the wallslide air control or not (less force)
    Vec2d                       m_wallSlideTestSpeed;               // If we are blocked, use this speed to test if we can wallslide

    f32                         m_wallJumpDisableTimer;             // Wall jump disabled timer

    // Crouch
    f32                         m_disableCrouchTimer;               // A timer to disable crouching
    f32                         m_unCrouchCounter;                  // Time since we released the stick to uncrouch

    // Sprint
    f32                         m_sprintActivationDelay;
    f32                         m_sprintMaxSpeedBlendTimer;         // The timer to go from regular walk speed to spring speed
    f32                         m_releaseSprintForce;               // The force to take into account when we release the sprint
    f32                         m_releaseSprintForceSign;           // The sign of the force (1 same direction as the edge)
    bbool                       m_releaseSprintActive;              // If the release sprint intertia is active
    bbool                       m_releaseSprintWasSprinting;        // If we were sprinting the frame before
    bbool                       m_wasOnGroundPrevFrame;             // If we were on the ground the previous frame

    // Swing
    ObjectRef                   m_swingOnActor;                     // The actor we are using to swing
    f32                         m_swingLength;                      // The radius of the swinging rod
    f32                         m_swingMaxAngle;                    // The maximum angle to swing
    f32                         m_swingGravMultiplier;              // The multiplier to gravity for the pendulum motion
    f32                         m_swingJumpSign;                    // Sign on X for the jump while swinging
#ifndef ITF_FINAL
    bbool                       m_swingDrawDebug;                   // Debug info from swinging

    // We leave these as bool so that they show nicely on the property editor
    bbool                       m_displayHangDebug;                 // If we should display the hang debug information
    bbool                       m_drawAnimInputs;                   // Display the values of the anim inputs
#endif // ITF_FINAL

    // Damage
    f32                         m_inmunityCounter;
    f32                         m_inmunityCounterReceiveHit;

    Vec2d                       m_previousSpeed;
    f32                         m_lastJumpTimer;

    EStance                     m_stance;                           // In which stance is the player (standing, hanging, climbing, etc)
    EStance                     m_prevStance;                       // What was the previous stance of the player
    EOrientationPose            m_orientationPose;                  // If the current action is done horizontally or vertically
    EActionPose                 m_actionPose;                       // Stand, Fight...
    EMoodZone                   m_moodZone;                         // The mood inside the current region
    f32                         m_poseTimeCounter;                  // Counter to switch poses
    
    // Hitting
    EPunchType                  m_currentPunchHitType;              // The type of the punch we are performing
    u32                         m_currentPunchHitLevel;             // The level of the punch we are performing
    EPunchReleaseResult         m_currentPunchResult;               // The current punch result
    f32                         m_currentPunchDistance;             // How far the punch has to go in the animation
    f32                         m_crushHitSuspensionCounter;        // Time counter to be in suspension while performing the earthquake
    f32                         m_crushHitYMultiplier;              // The Y speed multiplier when performing the earthquake
    f32                         m_crushRetriggerCounter;            // A counter to know when we can trigger again the crush attack
    u32                         m_hitFxBoneIndex;                   // Index of the bone where we place hit fx
    u32                         m_burstPunchToDeliver;              // How many punch burst we have left to deliver
    f32                         m_dashAttackCooldown;               // Timer to be able to hit again while reduced
    f32                         m_timeSinceDashAttack;              // The time since the last dash attack
    bbool                       m_reducedHitUpDisabledUntilLand;    // The reduced hit up is disabled until we land on a surface
    
    // Punching
    PhysShapePolygon            m_swingHitShape;                    // Shape for hitting while swinging
    f32                         m_currentPunchOrientation;          // The orientation of the current punch

    // U-Turn handling
    EUturnType                  m_performingUTurn;                  // If the character is performing a UTurn (from standing or moving)
    bbool                       m_uTurnFlipped;                     // If we already flipped the character on the uturn
    bbool                       m_uTurnFinished;                    // Flag that tells us if the u turn is finished
    bbool                       m_flipActor;                        // If we need to flip the actor
    f32                         m_uTurnComboCounter;                // Counter to allow uturn combos
    f32                         m_uTurnCounter;                     // Current duration of the uturn

    EFrictionMode               m_frictionMode;
    EGravityMode                m_gravityMode;
    f32                         m_gravityModeValue;
    f32                         m_gravityModeDuration;
    f32                         m_gravityModeCounter;

    MoveData                    m_move;
    MoveData                    m_moveAnalog;
    MoveData                    m_moveGravity;
    MoveData                    m_moveGravityAnalog;
    EMoveDir                    m_desiredLookDir;                   // The desired look direction
    Vec2d                       m_moveInput;                        // The move input we will feed to the animation

    u32                         m_actionFlags;                      // The current actions of the player
    f32                         m_idleTimer;                        // Time the player hasn't entered any input

    u32                         m_helicoFxHandle;

    ObjectRef                   m_lockPrevBinding;
    ObjectRef                   m_lockSnapActor;
    bbool                       m_actorLocked;
    bbool                       m_lockPrevPhysics;
    bbool                       m_lockPhantomRemoved;
    bbool                       m_pageAirTravelling;
    ActorRef                    m_pageCurActor;
    bbool                       m_pageActive;

    //Pedestal and hang polyline
    ObjectRefList               m_objectsOnPedestal;
    mutable u32                 m_pedestalBoneIndex;
    ObjectRef                   m_onPedestal;
    Vec2d                       m_onPedestalPos;
    Vec2d                       m_onPedestalStartPos;
    Vec2d                       m_onPedestalLastPos;
    f32                         m_onPedestalInterpolateCounter;
    f32                         m_onPedestalDisableCounter;
    ObjectRef                   m_onPedestalDisableRef;
    bbool                       m_onPedestalValid;
    bbool                       m_onPedestalAllowFall;
    static const f32            s_onPedestalInterpolateDuration;
   
    EActionCoopMode             m_coopActionMode;
    ObjectRef                   m_coopActionVictim;
    ObjectRef                   m_coopActionMain;

    class Ray_SquadMemberData*  m_squadData;

    bbool                       m_playerDisabled;
    bbool                       m_inputDisabled;
    bbool                       m_moveToPoint;
    bbool                       m_moveToPointFinished;
    Vec2d                       m_moveToPointTarget;
	f32							m_moveToPointFinishedToler;
    bbool                       m_moveToPointFirstTime;
    bbool                       m_bounceToLayer;
    SpawneeGenerator            m_bounceToLayerSpawnGenerator;
    SpawneeGenerator            m_tinySpawnGenerator;
    ActorRef                    m_tinyFXRef;
    bbool                       m_tinyTrailDisabled;
    Color                       m_tinyTrailCurrentColor;
    Color                       m_tinyTrailTargetColor;

    EDeathType                  m_currentDeathType;
    EHitNature                  m_lastHitNature;

    f32                         m_moveCursor;

    class CameraSubjectComponent* m_cameraSubjectComponent;
    f32                         m_cameraLimiterTimer;
    f32                         m_offscreenTimer;
    bbool                       m_lastOnscreenPlayerKillsEveryone;  // when the last onscreen player dies, everyone dies (force-kill offscreen players)

    f32                         m_walkOnWallsDisableTimer;
    bbool                       m_walkOnWallsDisableUntilLand;
    bbool                       m_walkOnWallKeepDirFromSpeed;
    
    struct PolylineRegion
    {
        StringID        m_regionId;
        ObjectRef       m_polyRef;
    };

    typedef SafeArray <PolylineRegion> RegionList;

    RegionList                  m_currentRegions;
    PhysActorForceArray         m_currentWindForces;
    ObjectRef                   m_prevCurrentRegion;
    Vec2d                       m_prevCurrentMove;

    SpawneeGenerator            m_generator;
    bbool                       m_squashed;
    bbool                       m_handsCaught;
    bbool                       m_handsReleased;
    ObjectRef                   m_handsRef;
    bbool                       m_caughtInPipe;
    ObjectRef                   m_pipeRef;
    bbool                       m_instantKill;
    bbool                       m_justActivated;
    u32                         m_justActivatedFrame;


    // Base info, to be used when jumping and applying the speed of the platform where we are
    ObjectRef                   m_baseInfoPrevPoly;
    u32                         m_baseInfoPrevEdgeIndex;
    f32                         m_baseInfoPrevRelDist;
    ObjectRef                   m_baseInfoPrevActor;
    f32                         m_baseInfoPrevMultiplierX;
    f32                         m_baseInfoPrevMultiplierY;
    Vec2d                       m_baseInfoSpeed;
    Vec2d                       m_baseInfoPrevPos;

    // Queued events
    Ray_EventTriggerEnterDoor   m_enterDoorEvent;
    bbool                       m_enterDoorReceived;
    Ray_EventTriggerChangePage  m_changePageEvent;
    bbool                       m_changePageReceived;
    Ray_EventTriggerStargate    m_stargateEvent;
    bbool                       m_stargateReceived;
    Ray_EventScaleDoorTrigger   m_scaleDoorTriggerEvent;
    bbool                       m_scaleDoorReceived;
    bbool                       m_scaleTunnelReceived;
    f32                         m_scaleTunnelScale;
    f32                         m_scaleTunnelT;
    Ray_EventJumpToPos          m_jumpToPos;
    bbool                       m_jumpToPosReceived;
    bbool                       m_snapDanceReceived;
    Ray_EventReturnTeeth        m_returnTeeth;
    bbool                       m_throwTeethReceived;
    Ray_EventPlayAnimState      m_playAnimEvent;
    bbool                       m_playAnimReceived;
    PunchStim                   m_punchEvent;
    bbool                       m_punchEventReceived;

    // Soft collisions
    f32                         m_softCollisionRadiusMultiplier;
    f32                         m_softCollisionForceMultiplier;
    f32                         m_softCollisionRestoreForceTimer;
    bbool                       m_softCollisionDisabled;
    
    // power-up displays
    SafeArray<Ray_PowerUpDisplay*> m_powerUpDisplays;
    Ray_HeartShield             m_heartShield;
    Ray_SuperPunchGauge         m_superPunchBasicGauge;
    Ray_SuperPunchGauge         m_superPunchSeekingGauge;
	Ray_SwarmRepellerPowerUp	m_swarmRepellerPowerUp;

    // input helper
    f64			m_InputLastTimePressed;
    bbool		m_InputIsDoublePressed;

	// bubble mode timer

	f32							m_stateDeadSoulTimer;
    bbool                       m_forceFindGround;

    // Input listener
	bbool                       m_listenerAdded;
    Vec2d                       m_listenerMove;
    bbool                       m_listenerAttack;
    bbool                       m_listenerAttackHold;
    bbool                       m_listenerJump;
    bbool                       m_listenerJumpHold;
    bbool                       m_listenerHelico;
    bbool                       m_listenerHelicoHold;
    bbool                       m_listenerSprint;    
    bbool                       m_listenerLeave;
#ifdef ITF_SUPPORT_CHEAT
    bbool                       m_listenerFlyCheat;
#endif
    bbool                       m_listenerSprintTrigger;    // start rayman to sprint until a direction is released

    struct SprintTrigger
    {
        SprintTrigger() : m_actif(bfalse), m_timer(0.0)  {}
        bbool                   m_actif;
        f64                     m_timer;       // timer starting once a direction is released
    };
    SprintTrigger               m_sprintTrigger;

    // Toggle/Hold for Run accessibility option
    bbool                       m_runToggleActive;          // Toggle state for run
    bbool                       m_prevRunButtonPressed;     // Edge detection for toggle

    // ai network
    bbool                       m_aiNetworkRegistered;
    ActorRef                    m_aiNetworkCurrentWaypoint;

    u32                         m_bubblizerIdentifier;
    
    bbool                       m_checkRewardBounce;

    enum AnimInputEnum
    {
        ANIMINPUT_SPEED,
        ANIMINPUT_SPEEDX,
        ANIMINPUT_SPEEDY,
        ANIMINPUT_AVGSPEEDX,
        ANIMINPUT_AVGSPEEDY,
        ANIMINPUT_MOVE,
        ANIMINPUT_MOVEX,
        ANIMINPUT_MOVEY,
        ANIMINPUT_STANCE,
        ANIMINPUT_PREVSTANCE,
        ANIMINPUT_INAIR,
        ANIMINPUT_PUNCHLEVEL,
        ANIMINPUT_RECEIVEHITLEVEL,
        ANIMINPUT_ORIENTATIONPOSE,
        ANIMINPUT_ACTIONPOSE,
        ANIMINPUT_MOODZONE,
        ANIMINPUT_ANGLE,
        ANIMINPUT_UTURN,
        ANIMINPUT_ACHIEVEDSPEEDFACTOR,
        ANIMINPUT_PUNCHHITTYPE,
        ANIMINPUT_RECEIVEDHITTYPE,
        ANIMINPUT_SUPPORTING,
        ANIMINPUT_COOPMODE,
        ANIMINPUT_COOPMODEPARTNERSTANCE,
        ANIMINPUT_COOPMODEPARTNERORIENTATIONPOSE,
        ANIMINPUT_ISSPRINTING,
        ANIMINPUT_IDLETIME,
        ANIMINPUT_GROUNDANGLE,
        ANIMINPUT_ISPATHBLOCKED,
        ANIMINPUT_DEATHTYPE,
        ANIMINPUT_PUNCHORIENTATION,
        ANIMINPUT_MOVECURSOR,
        ANIMINPUT_SLIDEACTION,
        ANIMINPUT_ISCROUCHED,
        ANIMINPUT_FALLFROMROOF,
        ANIMINPUT_ONCORNER,
        ANIMINPUT_AIRPOSE,
        ANIMINPUT_FALLANGLECURSOR,
        ANIMINPUT_HANGTOJUMPCURSOR,
        ANIMINPUT_PREPAREHITCURSOR,
        ANIMINPUT_ACTIONFROMUTURN,
        ANIMINPUT_CRUSHTRAVELTIME,
        ANIMINPUT_CRUSHRELEASED,
        ANIMINPUT_PUNCHRESULT,
        ANIMINPUT_PUNCHDISTANCE,
        ANIMINPUT_ROLLING,
        ANIMINPUT_INTERACTING,
        ANIMINPUT_LAYINGDOWN,
        ANIMINPUT_ISLOWROOF,
        ANIMINPUT_ACCROBATICSTAGE,
        ANIMINPUT_JUMPFROMWALLRUN,
        ANIMINPUT_PUNCHBURSTSTAGE,
        ANIMINPUT_REVIVECURSOR,
        ANIMINPUT_STARGATESTAGE,
        ANIMINPUT_DIVEMODE,
        ANIMINPUT_SWIMENTERHEAD,
        ANIMINPUT_SWIMCURVECURSOR,
        ANIMINPUT_SWIMTWIST,
        ANIMINPUT_SWINGQUADRANT,
        ANIMINPUTMAX,
    };

    FixedArray <u32,ANIMINPUTMAX> m_animInputIndex;
};

ITF_INLINE void Ray_PlayerControllerComponent::toggleActionFlag( EActionFlag _flag )
{
    if ( isActionFlagSet(_flag) )
    {
        removeActionFlag(_flag);
    }
    else
    {
        setActionFlag(_flag);
    }
}

//---------------------------------------------------------------------------------------------------

class Ray_PlayerControllerComponent_Template : public Ray_BasicPlayerControllerComponent_Template
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_PlayerControllerComponent_Template,Ray_BasicPlayerControllerComponent_Template,2964336576);
    DECLARE_SERIALIZE();
    DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_PlayerControllerComponent);

public:

    Ray_PlayerControllerComponent_Template();
    ~Ray_PlayerControllerComponent_Template();

    f32                 getHeight() const { return m_height; }
    f32                 getFlyModeSpeed() const { return m_flyModeSpeed; }
    f32                 getWalkForce() const { return m_walkForce; }
    f32                 getWalkMiniJumpForce() const { return m_walkMiniJumpForce; }
    f32                 getWalkMiniJumpMaxStep() const { return m_walkMiniJumpMaxStep; }
    f32                 getWalkIceCompensateFrictionMultiplier() const { return m_iceCompensateFrictionMultiplier; }
    f32                 getAirForce() const { return m_airForce; }
    f32                 getAirForceWallJump() const { return m_airForceWallJump; }
    f32                 getAirControlDelayWallJump() const { return m_airControlDelayWallJump; }
    f32                 getAirControlDelayFall() const { return m_airControlDelayFall; }
    f32                 getAirWallJumpExtraRadius() const { return m_airWallJumpExtraRadius; }
    f32                 getAirWallJumpCornerCheck() const { return m_airWallJumpCornerCheck; }
    f32                 getAirTopXSpeed() const { return m_airTopXSpeed; }
    f32                 getAirNoMoveFrictionX() const { return m_airNoMoveFrictionX; }
    f32                 getWallRunJumpDuration() const { return m_wallRunJumpDuration; }
    f32                 getWallRunJumpAirControlDelay() const { return m_wallRunJumpAirControlDelay; }
    f32                 getWallRunJumpKeepSpeedFactor() const { return m_wallRunJumpKeepSpeedFactor; }
    f32                 getWallRunJumpForce() const { return m_wallRunJumpForce; }
    f32                 getWallRunMinSpeed() const { return m_wallRunMinSpeed; }
    f32                 getWallRunUpThreshold() const { return m_wallRunUpThreshold; }
    f32                 getSwimingWaterResistMinSpeed() const { return m_swimingWaterResistMinSpeed; }
    f32                 getSwimingWaterResistMaxSpeed() const { return m_swimingWaterResistMaxSpeed; }
    f32                 getSwimingWaterResistMaxMult() const { return m_swimingWaterResistMaxMult; }
    f32                 getSwimingWaterResistEnterDuration() const { return m_swimingWaterResistEnterDuration; }
    f32                 getSwimingWaterResistEnterForceSpeedMultiplier() const { return m_swimingWaterResistEnterForceSpeedMultiplier; }
    f32                 getSwimingWaterResistEnterDownForceSpeedMultiplier() const { return m_swimingWaterResistEnterDownForceSpeedMultiplier; }
    f32                 getSwimingTopSpeed() const { return m_swimingTopSpeed; }
    f32                 getSwimingForce() const { return m_swimingForce; }
    f32                 getSwimingMinExitSpeed() const { return m_swimingMinExitSpeed; }
    f32                 getSwimingSurfaceForceDistMultInside() const { return m_swimingSurfaceForceDistMultInside; }
    f32                 getSwimingSurfaceForceSpeedMultInside() const { return m_swimingSurfaceForceSpeedMultInside; }
    f32                 getSwimingSurfaceForceDistMultOutside() const { return m_swimingSurfaceForceDistMultOutside; }
    f32                 getSwimingSurfaceForceSpeedMultOutside() const { return m_swimingSurfaceForceSpeedMultOutside; }
    f32                 getSwimingSurfaceRange() const { return m_swimingSurfaceRange; }
    f32                 getSwimingJumpRange() const { return m_swimingJumpRange; }
    f32                 getSwimingTimeAllowAction() const { return m_swimingTimeAllowAction; }
    f32                 getSwimingRegainControl() const { return m_swimingRegainControl; }
    f32                 getSwimingTurnDistMult() const { return m_swimingTurnDistMult; }
    f32                 getSwimingTurnSpeedMult() const { return m_swimingTurnSpeedMult; }
    f32                 getSwimingSprintMaxSpeedMultiplier() const { return m_swimingSprintMaxSpeedMultiplier; }
    f32                 getSwimingCurveMaxOffset() const { return m_swimingCurveMaxOffset; }
    f32                 getSwimingCurveMaxDif() const { return m_swimingCurveMaxDif; }
    f32                 getSwimingCurveForceS() const { return m_swimingCurveForceS; }
    f32                 getSwimingCurveForceD() const { return m_swimingCurveForceD; }
    f32                 getSwimingTurnUpsideDownWait() const { return m_swimingTurnUpsideDownWait; }
    f32                 getSwimingSprintJumpMinSpeed() const { return m_swimingSprintJumpMinSpeed; }
    f32                 getSwimingSprintJumpGravityMult() const { return m_swimingSprintJumpGravityMult; }
    f32                 getSwimingSprintJumpGravityDuration() const { return m_swimingSprintJumpGravityDuration; }
    f32                 getSwimingSprintJumpAngularSpeedMult() const { return m_swimingSprintJumpAngularSpeedMult; }
    f32                 getSwimingMoveTargetMultiplierBlendTime() const { return m_swimingMoveTargetMultiplierBlendTime; }
    f32                 getSwimingDolphinYSpeed() const { return m_swimingDolphinYSpeed; }
    f32                 getSwimingDolphinWallJumpDisableTimer() const { return m_swimingDolphinWallJumpDisableTimer; }
    f32                 getSwimingPushThreshold() const { return m_swimingPushThreshold; }
    f32                 getSwimingDashDuration() const { return m_swimingDashDuration; }
    f32                 getMoveTargetMultiplierMin() const { return m_moveTargetMultiplierMin; }
    f32                 getMoveTargetMultiplierMax() const { return m_moveTargetMultiplierMax; }
    f32                 getMoveTargetMultiplierBlendTime() const { return m_moveTargetMultiplierBlendTime; }
    f32                 getJumpQueueTime() const { return m_jumpQueueTime; }
    f32                 getHitQueueTime() const { return m_hitQueueTime; }
    f32                 getHitDisableOnBounce() const { return m_hitDisableOnBounce; }
    f32                 getHitFilterNonFacingOpponentSpeedX() const { return m_hitFilterNonFacingOpponentSpeedX; }
    f32                 getHitQueueBlockingHitsMinDistance() const { return m_hitQueueBlockingHitsMinDistance; }
    f32                 getWindDisableOnBounce() const { return m_windDisableOnBounce; }
    f32                 getJumpOnAirTime() const { return m_jumpOnAirTime; }
    f32                 getJumpForceY() const { return m_jumpForceY; }
    f32                 getJumpForceYBounceMultiplier() const { return m_jumpForceYBounceMultiplier; }
    f32                 getJumpMultiplierClimbH() const { return m_jumpMultiplierClimbH; }
    f32                 getJumpMultiplierNoImpulseClimbH() const { return m_jumpMultiplierNoImpulseClimbH; }
    f32                 getJumpImpulseXClimbH() const { return m_jumpImpulseXClimbH; }
    f32                 getJumpMultiplierClimbV() const { return m_jumpMultiplierClimbV; }
    f32                 getJumpMultiplierNoImpulseClimbV() const { return m_jumpMultiplierNoImpulseClimbV; }
    f32                 getJumpImpulseXClimbV() const { return m_jumpImpulseXClimbV; }
    f32                 getJumpMultiplierHang() const { return m_jumpMultiplierHang; }
    f32                 getJumpMultiplierNoImpulseHang() const { return m_jumpMultiplierNoImpulseHang; }
    f32                 getJumpImpulseXHang() const { return m_jumpImpulseXHang; }
    f32                 getJumpMultiplierRolling() const { return m_jumpMultiplierRolling; }
    f32                 getJumpMultiplierNoImpulseRolling() const { return m_jumpMultiplierNoImpulseRolling; }
    f32                 getJumpImpulseXRolling() const { return m_jumpImpulseXRolling; }
    f32                 getJumpMultiplierCrouch() const { return m_jumpMultiplierCrouch; }
    f32                 getJumpMultiplierNoImpulseCrouch() const { return m_jumpMultiplierNoImpulseCrouch; }
    f32                 getJumpImpulseXCrouch() const { return m_jumpImpulseXCrouch; }
    f32                 getJumpMultiplierFallOnVictim() const { return m_jumpMultiplierFallOnVictim; }
    f32                 getJumpMultiplierNoImpulseFallOnVictim() const { return m_jumpMultiplierNoImpulseFallOnVictim; }
    f32                 getJumpImpulseXFallOnVictim() const { return m_jumpImpulseXFallOnVictim; }
    f32                 getJumpMultiplierPedestal() const { return m_jumpMultiplierPedestal; }
    f32                 getJumpMultiplierNoImpulsePedestal() const { return m_jumpMultiplierNoImpulsePedestal; }
    f32                 getJumpImpulseXPedestal() const { return m_jumpImpulseXPedestal; }
    f32                 getJumpMultiplierOnPedestal() const { return m_jumpMultiplierOnPedestal; }
    f32                 getJumpMultiplierNoImpulseOnPedestal() const { return m_jumpMultiplierNoImpulseOnPedestal; }
    f32                 getJumpImpulseXOnPedestal() const { return m_jumpImpulseXOnPedestal; }
    f32                 getAirSuspensionDelay() const { return m_airSuspensionDelay; }
    f32                 getAirSuspensionPushThreshold() const { return m_airSuspensionPushThreshold; }
    f32                 getAirSuspensionMinYSpeed() const { return m_airSuspensionMinYSpeed; }
    f32                 getAirSuspensionMaxYSpeed() const { return m_airSuspensionMaxYSpeed; }
    f32                 getAirSuspensionMinMultiplier() const { return m_airSuspensionMinMultiplier; }
    f32                 getAirSuspensionMaxMultiplier() const { return m_airSuspensionMaxMultiplier; }
    f32                 getEfficiencyMaxSpeed() const { return m_efficiencyMaxSpeed; }
    f32                 getAchievedSpeedFactorDefault() const { return m_achievedSpeedFactorDefault; }
    f32                 getAchievedSpeedFactorS() const { return m_achievedSpeedFactorS; }
    f32                 getAchievedSpeedFactorD() const { return m_achievedSpeedFactorD; }
    Angle               getMinGroundAngleEfficiencyAngle() const { return m_minGroundAngleEfficiencyAngle; }
    Angle               getMaxGroundAngleEfficiencyAngle() const { return m_maxGroundAngleEfficiencyAngle; }
    f32                 getMinGroundAngleEfficiencyMult() const { return m_minGroundAngleEfficiencyMult; }
    f32                 getMaxGroundAngleEfficiencyMult() const { return m_maxGroundAngleEfficiencyMult; }
    f32                 getCornerPoseExtraDist() const { return m_cornerPoseExtraDist; }
    Angle               getHangMaxGroundAngle() const { return m_hangMaxGroundAngle; }
    f32                 getHangRadiusCheck() const { return m_hangRadiusCheck; }
    f32                 getHangCornerMaxSpeed() const { return m_hangCornerMaxSpeed; }
    Vec2d               getHangingCollisionCheckOffsetSolid() const { return m_hangingCollisionCheckOffsetSolid; }
    Vec2d               getHangingCollisionCheckOffsetCoop() const { return m_hangingCollisionCheckOffsetCoop; }
    f32                 getHangCoopDistanceCheck() const { return m_hangCoopDistanceCheck; }
    Vec2d               getHangRectSize() const { return m_hangRectSize; }
    Vec2d               getHangCollisionExtentSolid() const { return m_hangCollisionExtentSolid; }
    Vec2d               getHangCollisionExtentCoop() const { return m_hangCollisionExtentCoop; }
    Vec2d               getHangRectOffset() const { return m_hangRectOffset; }
    f32                 getHangMaxSpeed() const { return m_hangMaxSpeed; }
    f32                 getHangEdgeActivationTime() const { return m_hangEdgeActivationTime; }
    f32                 getHelicopterForceX() const { return m_helicopterForceX; }
    f32                 getHelicopterUnstickMinFrictionMultiplier() const { return m_helicopterUnstickMinFrictionMultiplier; }
    f32                 getHelicopterMinDragSpeedX() const { return m_helicopterMinDragSpeedX; }
    f32                 getHelicopterAirFrictionMultiplier() const { return m_helicopterAirFrictionMultiplier; }
    f32                 getHelicopterDragForceX() const { return m_helicopterDragForceX; }
    const Angle&        getHelicopterWindForcesAngle() const { return m_helicopterWindForcesAngle; }
    f32                 getHelicopterWindForcesMultiplier() const { return m_helicopterWindForcesMultiplier; }
    const Angle&        getHelicopterWindSpeedAngle() const { return m_helicopterWindSpeedAngle; }
    Vec2d               getHelicopterWindSpeedMultiplier() const { return m_helicopterWindSpeedMultiplier; }
    f32                 getHelicopterReleaseDelay() const { return m_helicopterReleaseDelay; }
    f32                 getHelicopterMaxSpeedUp() const { return m_helicopterMaxSpeedUp; }
    f32                 getHelicopterMaxSpeedDown() const { return m_helicopterMaxSpeedDown; }
    f32                 getHelicopterGravityMultiplier() const { return m_helicopterGravityMultiplier; }
    f32                 getHelicopterCooldown() const { return m_helicopterCooldown; }
    f32                 getHelicopterSuspensionTime() const { return m_helicopterSuspensionTime; }
    f32                 getHelicopterSuspensionMultiplier() const { return m_helicopterSuspensionMultiplier; }
    f32                 getAirHitForceApplicationTime() const { return m_airHitForceApplicationTime; }
    f32                 getAirHitUnstickMinFrictionMultiplier() const { return m_airHitUnstickMinFrictionMultiplier; }
    f32                 getAirHitMinDragSpeedX() const { return m_airHitMinDragSpeedX; }
    f32                 getAirHitDragForceX() const { return m_airHitDragForceX; }
    f32                 getAirHitGravityMultiplier() const { return m_airHitGravityMultiplier; }
    const Vec2d&        getAirHitSpeedMultiplier() const { return m_airHitSpeedMultiplier; }
    const Vec2d&        getAirHitSpeedMultiplierReduced() const { return m_airHitSpeedMultiplierReduced; }
    f32                 getRehitTimer() const { return m_rehitTimer; }
    f32                 getFightPoseDuration() const { return m_fightPoseDuration; }
    Angle               getMinBounceAngle() const { return m_minBounceAngle; }
    f32                 getBounceJumpMargin() const { return m_bounceJumpMargin; }
    f32                 getBounceHeightPlatformLevel1() const { return m_bounceHeightPlatformLevel1; }
    f32                 getBounceHeightPlatformLevel2() const { return m_bounceHeightPlatformLevel2; }
    f32                 getBounceHeightPlatformLevel3() const { return m_bounceHeightPlatformLevel3; }
    f32                 getBounceHeightEnemyLevel1() const { return m_bounceHeightEnemyLevel1; }
    f32                 getBounceHeightEnemyLevel2() const { return m_bounceHeightEnemyLevel2; }
    f32                 getBounceHeightEnemyLevel3() const { return m_bounceHeightEnemyLevel3; }
    f32                 getBounceHeightTalkingHatLevel1() const { return m_bounceHeightTalkingHatLevel1; }
    f32                 getBounceHeightTalkingHatLevel2() const { return m_bounceHeightTalkingHatLevel2; }
    f32                 getBounceHeightTalkingHatLevel3() const { return m_bounceHeightTalkingHatLevel3; }
    f32                 getBounceHeightPolylineLevel1() const { return m_bounceHeightPolylineLevel1; }
    f32                 getBounceHeightPolylineLevel2() const { return m_bounceHeightPolylineLevel2; }
    f32                 getBounceHeightPolylineLevel3() const { return m_bounceHeightPolylineLevel3; }
    f32                 getBounceHeightWave() const { return m_bounceHeightWave; }
    f32                 getBounceHeightWaveLevel3() const { return m_bounceHeightWaveLevel3; }
    f32                 getBounceEnemyXSpeedMultiplier() const { return m_bounceEnemyXSpeedMultiplier; }
    f32                 getBouncePlatformAirControlDelayLevel1() const { return m_bouncePlatformAirControlDelayLevel1; }
    f32                 getBouncePlatformAirControlDelayLevel2() const { return m_bouncePlatformAirControlDelayLevel2; }
    f32                 getBouncePlatformAirControlDelayLevel3() const { return m_bouncePlatformAirControlDelayLevel3; }
    f32                 getBounceEnemyAirControlDelayLevel1() const { return m_bounceEnemyAirControlDelayLevel1; }
    f32                 getBounceEnemyAirControlDelayLevel2() const { return m_bounceEnemyAirControlDelayLevel2; }
    f32                 getBounceEnemyAirControlDelayLevel3() const { return m_bounceEnemyAirControlDelayLevel3; }
    f32                 getBounceTalkingHatAirControlDelayLevel1() const { return m_bounceTalkingHatAirControlDelayLevel1; }
    f32                 getBounceTalkingHatAirControlDelayLevel2() const { return m_bounceTalkingHatAirControlDelayLevel2; }
    f32                 getBounceTalkingHatAirControlDelayLevel3() const { return m_bounceTalkingHatAirControlDelayLevel3; }
    f32                 getBouncePolylineMaxXSpeed() const { return m_bouncePolylineMaxXSpeed; }
    f32                 getBounceDelay() const { return m_bounceDelay; }
    f32                 getBounceRepositionDuration() const { return m_bounceRepositionDuration; }
    f32                 getBounceToActorYFuncPoint0Dist() const { return m_bounceToActorYFuncPoint0Dist; }
    f32                 getBounceToActorYFuncPoint1Dist() const { return m_bounceToActorYFuncPoint1Dist; }
    f32                 getBounceToActorXZFuncPoint0T() const { return m_bounceToActorXZFuncPoint0T; }
    f32                 getBounceToActorXZFuncPoint1T() const { return m_bounceToActorXZFuncPoint1T; }
    f32                 getBounceToActorSpeed() const { return m_bounceToActorSpeed; }
    f32                 getBounceToActorMinTime() const { return m_bounceToActorMinTime; }
    f32                 getWindTunnelExitSpeedLevel0() const { return m_windTunnelExitSpeedLevel0; }
    f32                 getWindTunnelExitSpeedLevel1() const { return m_windTunnelExitSpeedLevel1; }
    f32                 getWindTunnelExitSpeedLevel2() const { return m_windTunnelExitSpeedLevel2; }
    Vec2d               getClimbingCollisionCheckExtent() const { return m_climbingCollisionCheckExtent; }
    f32                 getClimbVerticalSpeedUp() const { return m_climbVerticalSpeedUp; }
    f32                 getClimbVerticalDescendAccel() const { return m_climbVerticalDescendAccel; }
    f32                 getClimbVerticalAscendAccel() const { return m_climbVerticalAscendAccel; }
    f32                 getClimbVerticalDescendMaxSpeed() const { return m_climbVerticalDescendMaxSpeed; }
    f32                 getClimbVerticalDescendMaxSpeedSprint() const { return m_climbVerticalDescendMaxSpeedSprint; }
    f32                 getClimbHorizontalSpeed() const { return m_climbHorizontalSpeed; }
    f32                 getClimbHorizontalMoveAnimRate() const { return m_climbHorizontalMoveAnimRate; }
    f32                 getClimbSprintSpeedMultiplier() const { return m_climbSprintSpeedMultiplier; }
    f32                 getClimbHorizontalDescendMaxSpeed() const { return m_climbHorizontalDescendMaxSpeed; }
    f32                 getClimbHorizontalDescendMaxSpeedSprint() const { return m_climbHorizontalDescendMaxSpeedSprint; }
    f32                 getClimbSlideDisableAirControl() const { return m_climbSlideDisableAirControl; }
    f32                 getClimbEdgeInertiaBrake() const { return m_climbEdgeInertiaBrake; }
    f32                 getClimbEdgeDistanceRange() const { return m_climbEdgeDistanceRange; }
    f32                 getClimbEdgeActivationTime() const { return m_climbEdgeActivationTime; }
    f32                 getClimbEdgeSwingImpulse() const { return m_climbEdgeSwingImpulse; }
    f32                 getClimbEdgeSwingImpulseReduced() const { return m_climbEdgeSwingImpulseReduced; }
    f32                 getClimbEdgePunchImpulse() const { return m_climbEdgePunchImpulse; }
    f32                 getClimbEdgeMoveImpulse() const { return m_climbEdgeMoveImpulse; }
    Angle               getClimbEdgeAngularSpeed() const { return m_climbEdgeAngularSpeed; }
    f32                 getClimbEdgeCoopHangEdgeRange() const { return m_climbEdgeCoopHangEdgeRange; }
    Vec2d               getClimbMaxJumpSpeedTransfer() const { return m_climbMaxJumpSpeedTransfer; }
    Vec2d               getPunchShapeOffset() const { return m_punchShapeOffset; }
    f32                 getReceivePunchFrontWeakForce() const { return m_receivePunchFrontWeakForce; }
    f32                 getReceivePunchFrontStrongForce() const { return m_receivePunchFrontStrongForce; }
    f32                 getReceivePunchFrontMegaForce() const { return m_receivePunchFrontMegaForce; }
    f32                 getReceivePunchFrontWeakFrictionMultiplier() const { return m_receivePunchFrontWeakFrictionMult; }
    f32                 getReceivePunchFrontStrongFrictionMultiplier() const { return m_receivePunchFrontStrongFrictionMult; }
    f32                 getReceivePunchFrontMegaFrictionMultiplier() const { return m_receivePunchFrontMegaFrictionMult; }
    f32                 getReceivePunchUpWeakForce() const { return m_receivePunchUpWeakForce; }
    f32                 getReceivePunchUpStrongForce() const { return m_receivePunchUpStrongForce; }
    f32                 getReceivePunchUpMegaForce() const { return m_receivePunchUpMegaForce; }
    f32                 getReceivePunchUpWeakGravityMultiplier() const { return m_receivePunchUpWeakGravityMult; }
    f32                 getReceivePunchUpStrongGravityMultiplier() const { return m_receivePunchUpStrongGravityMult; }
    f32                 getReceivePunchUpMegaGravityMultiplier() const { return m_receivePunchUpMegaGravityMult; }
    f32                 getReceiveEarthquakeWeakForce() const { return m_receiveEarthquakeWeakForce; }
    f32                 getReceiveEarthquakeStrongForce() const { return m_receiveEarthquakeStrongForce; }
    f32                 getReceiveEarthquakeMegaForce() const { return m_receiveEarthquakeMegaForce; }
    f32                 getReceiveBounceVWeakForce() const { return m_receiveBounceVWeakForce; }
    f32                 getReceiveBounceVStrongForce() const { return m_receiveBounceVStrongForce; }
    f32                 getReceiveBounceVMegaForce() const { return m_receiveBounceVMegaForce; }
    f32                 getReceiveBounceHWeakForce() const { return m_receiveBounceHWeakForce; }
    f32                 getReceiveBounceHStrongForce() const { return m_receiveBounceHStrongForce; }
    f32                 getReceiveBounceHMegaForce() const { return m_receiveBounceHMegaForce; }
    f32                 getReceiveBounceWeakFrictionMult() const { return m_receiveBounceWeakFrictionMult; }
    f32                 getReceiveBounceStrongFrictionMult() const { return m_receiveBounceStrongFrictionMult; }
    f32                 getReceiveBounceMegaFrictionMult() const { return m_receiveBounceMegaFrictionMult; }
    f32                 getReceiveBounceWeakSwimingMult() const { return m_receiveBounceWeakSwimingMult; }
    f32                 getReceiveBounceStrongSwimingMult() const { return m_receiveBounceStrongSwimingMult; }
    f32                 getReceiveBounceMegaSwimingMult() const { return m_receiveBounceMegaSwimingMult; }
    f32                 getReceiveHitSwimGravityMultiplier() const { return m_receiveHitSwimGravityMultiplier; }
    f32                 getSlideFrictionMultiplier() const { return m_slideFrictionMultiplier; }
    f32                 getSlideMinFastSpeed() const { return m_slideMinFastSpeed; }
    u32                 getSlideHitLevel() const { return m_slideHitLevel; }
    f32                 getSlideMinSpeedToAllow() const { return m_slideMinSpeedToAllow; }
    f32                 getSlideMinSpeedMultMinFriction() const { return m_slideMinSpeedMultMinFriction; }
    f32                 getSlideMinSpeedMultMaxFriction() const { return m_slideMinSpeedMultMaxFriction; }
    f32                 getSlideFallDelay() const { return m_slideFallDelay; }
    f32                 getHoverMinYSpeed() const { return m_hoverMinYSpeed; }
    f32                 getSurfingJumpTime() const { return m_surfingJumpTime; }
    f32                 getMetrailleMaxHits() const { return m_metrailleMaxHits; }
    u32                 getMetrailleFinalHitLevel() const { return m_metrailleFinalHitLevel; }
    u32                 getMetrailleGroundVariation() const { return m_metrailleGroundVariation; }
    u32                 getMetrailleAirVariation() const { return m_metrailleAirVariation; }
    u32                 getMetrailleMaxBufferedHits() const { return m_metrailleMaxBufferedHits; }
    f32                 getAccrobaticStageKeepTime() const { return m_accrobaticStageKeepTime; }
    f32                 getSwingSpeedAdjustTime() const { return m_swingSpeedAdjustTime; }
    f32                 getSwingInterpolateTime() const { return m_swingInterpolateTime; }
    f32                 getSwingHitAreaHeight() const { return m_swingHitAreaHeight; }
    f32                 getSwingHitAreaOffset() const { return m_swingHitAreaOffset; }
    f32                 getSwingHitMinAngularSpeed() const { return m_swingHitMinAngularSpeed; }
    f32                 getSwingInitialBoost() const { return m_swingInitialBoost; }
    f32                 getSwingJumpXForceThreshold() const { return m_swingJumpXForceThreshold; }
    f32                 getSwingJumpXAngleThreshold() const { return m_swingJumpXAngleThreshold; }
    f32                 getSwingJumpForceX() const { return m_swingJumpForceX; }
    f32                 getSwingJumpDelay() const { return m_swingJumpDelay; }
    u32                 getUTurnHitLevel() const { return m_uTurnHitLevel; }
    u32                 getUTurnAirHitLevel() const { return m_uTurnAirHitLevel; }
    u32                 getCrouchHitLevel() const { return m_crouchHitLevel; }
    u32                 getTornadoHitLevel() const { return m_tornadoHitLevel; }
    u32                 getSpinHitLevel() const { return m_spinHitLevel; }
    u32                 getReducedSizeHitLevel() const { return m_reducedSizeHitLevel; }
    u32                 getUpperKickHitLevel() const { return m_upperKickHitLevel; }
    u32                 getCrushHitLevel() const { return m_crushHitLevel; }
    f32                 getCrushHitLevelIncrTimeFirst() const { return m_crushHitLevelIncrTimeFirst; }
    f32                 getCrushHitLevelIncrTimeSecond() const { return m_crushHitLevelIncrTimeSecond; }
    f32                 getCrushHitRetriggerDelay() const { return m_crushRetriggerDelay; }
    u32                 getSlideDownHitLevel() const { return m_slideDownHitLevel; }
    u32                 getSwingHitLevel() const { return m_swingHitLevel; }
    u32                 getHangCoopHitLevel() const { return m_hangCoopHitLevel; }
    u32                 getBurstHitLevel() const { return m_burstHitLevel; }
    u32                 getBurstAirHitLevel() const { return m_burstAirHitLevel; }
    u32                 getNormalPunchLevelForShakeCam() const { return m_normalPunchLevelForShakeCam; }
    f32                 getSwitchHitBufferTime() const { return m_switchHitBufferTime; }
    f32                 getTurnComboTime() const { return m_uTurnComboTime; }
    f32                 getCrushHitSuspensionTime() const { return m_crushHitSuspensionTime; }
    f32                 getCrushHitMinYSpeed() const { return m_crushHitMinYSpeed; }
    f32                 getCrushHitYMultiplier() const { return m_crushHitYMultiplier; }
    f32                 getCrushHitDownForce() const { return m_crushHitDownForce; }
    f32                 getCrushHitFriction() const { return m_crushHitFriction; }
    f32                 getSlideDownHitMinSpeed() const { return m_slideDownHitMinSpeed; }
    f32                 getSwimAttackNormalDuration() const { return m_swimAttackNormalDuration; }
    f32                 getSwimAttackNormalSpeed() const { return m_swimAttackNormalSpeed; }
    f32                 getSwimAttackSprintDuration() const { return m_swimAttackSprintDuration; }
    f32                 getSwimAttackSprintSpeed() const { return m_swimAttackSprintSpeed; }
    f32                 getSwimAttackCooldown() const { return m_swimAttackCooldown; }
    f32                 getSwimAttackFriction() const { return m_swimAttackFriction; }
    f32                 getSwimAttackMinSpeed() const { return m_swimAttackMinSpeed; }
    f32                 getSwimAttackMoveDamping() const { return m_swimAttackMoveDamping; }
    f32                 getSwimAttackSpamTime() const { return m_swimAttackSpamTime; }
    f32                 getSwimAttackSpamImpulseMultiplier() const { return m_swimAttackSpamImpulseMultiplier; }
    f32                 getSwimAttackSpamImpulseMultiplierSprint() const { return m_swimAttackSpamImpulseMultiplierSprint; }
    f32                 getTornadoDuration() const { return m_tornadoDuration; }
    f32                 getTornadoSpeed() const { return m_tornadoSpeed; }
    f32                 getTornadoCooldown() const { return m_tornadoCooldown; }
    f32                 getTornadoMinSpeed() const { return m_tornadoMinSpeed; }
    f32                 getTornadoFriction() const { return m_tornadoFriction; }
    f32                 getTornadoAllowMoveTime() const { return m_tornadoAllowMoveTime; }
    f32                 getReducedSizeHitDuration() const { return m_reducedSizeHitDuration; }
    f32                 getReducedSizeHitAirDuration() const { return m_reducedSizeHitAirDuration; }
    f32                 getReducedSizeHitUpDuration() const { return m_reducedSizeHitUpDuration; }
    f32                 getReducedSizeHitFriction() const { return m_reducedSizeHitFriction; }
    f32                 getReducedSizeHitGravity() const { return m_reducedSizeHitGravity; }
    f32                 getReducedSizeHitSideForceGround() const { return m_reducedSizeHitSideForceGround; }
    f32                 getReducedSizeHitSideForceAir() const { return m_reducedSizeHitSideForceAir; }
    f32                 getReducedSizeHitUpForce() const { return m_reducedSizeHitUpForce; }
    f32                 getReducedSizeHitMinSpeed() const { return m_reducedSizeHitMinSpeed; }
    f32                 getReducedSizeHitCrushAttackQueueLength() const { return m_reducedSizeHitCrushAttackQueueLength; }
    f32                 getReducedSizeHitCrushDistance() const { return m_reducedSizeHitCrushDistance; }
    f32                 getReducedSizeHitCooldown() const { return m_reducedSizeHitCooldown; }
    f32                 getReducedSizeHitUpCooldown() const { return m_reducedSizeHitUpCooldown; }
    f32                 getReducedSizeHitGroundFrictionMultiplierMin() const { return m_reducedSizeHitGroundFrictionMultiplierMin; }
    f32                 getReducedSizeHitGroundFrictionMultiplierMax() const { return m_reducedSizeHitGroundFrictionMultiplierMax; }
    const StringID&     getHitFxBoneName() const { return m_hitFxBoneName; }
    f32                 getPrepareHitLengthWeak() const { return m_prepareHitLengthWeak; }
    f32                 getPrepareHitLengthStrong() const { return m_prepareHitLengthStrong; }
    f32                 getPrepareHitLengthMega() const { return m_prepareHitLengthMega; }
    f32                 getRehitDelayPermanentHit() const { return m_rehitDelayPermanentHit; }
    f32                 getRehitDelayComboHit() const { return m_rehitDelayComboHit; }
    f32                 getWallSlideStartDelay() const { return m_wallSlideStartDelay; }
    f32                 getWallSlideStartDelaySpeedThreshold() const { return m_wallSlideStartDelaySpeedThreshold; }
    f32                 getWallSlideAngularSpeedMinLinear() const { return m_wallSlideAngularSpeedMinLinear; }
    f32                 getWallSlideAngularSpeedMaxLinear() const { return m_wallSlideAngularSpeedMaxLinear; }
    Angle               getWallSlideAngularSpeedMin() const { return m_wallSlideAngularSpeedMin; }
    Angle               getWallSlideAngularSpeedMax() const { return m_wallSlideAngularSpeedMax; }
    f32                 getWallSlideBrakeTime() const { return m_wallSlideBrakeTime; }
    f32                 getWallSlideStartSpeed() const { return m_wallSlideStartSpeed; }
    f32                 getWallSlideGravityMultiplier() const { return m_wallSlideGravityMultiplier; }
    f32                 getWallSlideJumpXImpulse() const { return m_wallSlideJumpXImpulse; }
    f32                 getWallSlideFrictionCoefCondition() const { return m_wallSlideFrictionCoefCondition; }
    f32                 getWallSlideMinEdgeMargin() const { return m_wallSlideMinEdgeMargin; }
    Angle               getWallPushWallAngle() const { return m_wallPushWallAngle; }
    f32                 getWallPushForce() const { return m_wallPushForce; }
    f32                 getWallPushDelay() const { return m_wallPushDelay; }
    f32                 getWallPushRepushDelay() const { return m_wallPushRepushDelay; }
    f32                 getWallSlideMaxSpeed() const { return m_wallSlideMaxSpeed; }
    f32                 getWallSlideUnstickDelay() const { return m_wallSlideUnstickDelay; }
    f32                 getWallSlideRestickDelay() const { return m_wallSlideRestickDelay; }
    f32                 getWallSlideExtraRadius() const { return m_wallSlideExtraRadius; }
    f32                 getWallSlideYSpeedThresholdToHang() const { return m_wallSlideYSpeedThresholdToHang; }
    f32                 getDisableCrouchTime() const { return m_disableCrouchTime; }
    f32                 getCrouchUnhangTime() const { return m_crouchUnhangTime; }
    f32                 getRollingFrictionMultiplier() const { return m_rollingFrictionMultiplier; }
    f32                 getRollingMinSpeed() const { return m_rollingMinSpeed; }
    f32                 getFallOnVictimMinSpeed() const { return m_fallOnVictimMinSpeed; }
    f32                 getSprintActivationLandingDelay() const { return m_sprintActivationLandingDelay; }
    f32                 getSprintForceMultiplier() const { return m_sprintForceMultiplier; }
    f32                 getSprintEfficiencyMultiplier() const { return m_sprintEfficiencyMultiplier; }
    f32                 getSprintMaxSpeedBlendDuration() const { return m_sprintMaxSpeedBlendDuration; }
    f32                 getSprintReleaseForceMultiplier() const { return m_sprintReleaseForceMultiplier; }
    f32                 getSprintReleaseForceMax() const { return m_sprintReleaseForceMax; }
    const Path&         getBounceToLayerFXFile() const { return m_bounceToLayerFXFile; }
    const Path&         getTinyFXFile() const { return m_tinyFXFile; }
    const Color&        getTinyTrailSprintColor() const { return m_tinyTrailSprintColor; }
    const Color&        getTinyTrailWallRunColor() const { return m_tinyTrailWallRunColor; }
    const Color&        getTinyTrailWallRunJumpColor() const { return m_tinyTrailWallRunJumpColor; }
    const Color&        getTinyTrailNormalColor() const { return m_tinyTrailNormalColor; }
    f32                 getPunchPushbackRadius() const { return m_punchPushbackRadius; }
    PhysShape*          getPhantomShapeCrouch() const { return m_phantomShapeCrouch; }
    PhysShape*          getPhantomShapeSwim() const { return m_phantomShapeSwim; }
    f32                 getInmunityTime() const { return m_inmunityTime; }
    f32                 getInmunityTimeReceiveHit() const { return m_inmunityTimeReceiveHit; }
    f32                 getInmunityTimeRevive() const { return m_inmunityTimeRevive; }
    f32                 getInmunityTimeReceiveHitRevive() const { return m_inmunityTimeReceiveHitRevive; }
    f32                 getSquashBounceMargin() const { return m_squashBounceMargin; }
    u32                 getSquashHurtLevel() const { return m_squashHurtLevel; }
    f32                 getSquashDeathPenetration() const { return m_squashDeathPenetration; }
    const StringID&     getCharacterSnapBone() const { return m_characterSnapBone; }
    const Ray_PlayerControllerComponent::StateDeadSoul_Template& getStateDeadSoul() const { return m_stateDeadSoul; }
    const Ray_PlayerControllerComponent::StateRevive_Template& getStateRevive() const { return m_stateRevive; }
    f32                 getAvoidanceRadius() const { return m_avoidanceRadius; }
    f32                 getDarktoonificationPosOffset() const { return m_darktoonificationPosOffset; }
    f32                 getPedestalWalkMultiplier() const { return m_pedestalWalkMultiplier; }
    f32                 getPedestalAreaRadius() const { return m_pedestalAreaRadius; }
    f32                 getPedestalDistanceCheck() const { return m_pedestalDistanceCheck; }
    f32                 getPedestalFeetDistanceCheck() const { return m_pedestalFeetDistanceCheck; }
    f32                 getPedestalDisableTimer() const { return m_pedestalDisableTimer; }
    const Angle&        getPedestalInputCone() const { return m_pedestalInputCone; }
    f32                 getOnPedestalLandBounceHeight() const { return m_onPedestalLandBounceHeight; }
    f32                 getDeathNormalWait() const { return m_deathNormalWait; }
    f32                 getMoveCursorMinEfficiency() const { return m_moveCursorMinEfficiency; }
    f32                 getMoveCursorMaxEfficiency() const { return m_moveCursorMaxEfficiency; }
    f32                 getMoveCursorMinAngle() const { return m_moveCursorMinAngle; }
    f32                 getMoveCursorMaxAngle() const { return m_moveCursorMaxAngle; }
    f32                 getMoveCursorMinMultiplier() const { return m_moveCursorMinMultiplier; }
    f32                 getMoveCursorMaxMultiplier() const { return m_moveCursorMaxMultiplier; }
    f32                 getMoveCursorEfficiencyAngle() const { return m_moveCursorEfficiencyAngle; }
    f32                 getWalkOnWallsDisableDelay() const { return m_walkOnWallsDisableDelay; }
    f32                 getWalkOnWallsReleaseStickDuration() const { return m_walkOnWallsReleaseStickDuration; }
    const Path&         getDeathFXActorSpawn() const { return m_deathFXActorSpawn; }
    ITF_INLINE const Margin& getOffscreenDeathMargin() const { return m_offscreenDeathMargin; }
    ITF_INLINE f32      getOffscreenDeathSmooth() const { return m_offscreenDeathSmooth; }
    ITF_INLINE const Margin& getOffscreenDeathFxMargin() const { return m_offscreenDeathFxMargin; }
    bbool               getApplyWindSpeedLimitation() const { return m_applyWindSpeedLimitation; }

    f32                 getClampingMaxSpeedTotal() const { return m_clampingMaxSpeedTotal; }
    f32                 getClampingMaxSpeedX() const { return m_clampingMaxSpeedX; }
    f32                 getClampingMaxSpeedY() const { return m_clampingMaxSpeedY; }
    f32                 getAirBrakeMaxSpeedX() const { return m_airBrakeMaxSpeedX; }

    f32                 getSoftCollisionRadiusMultiplier() const { return m_softCollisionRadiusMultiplier; }
    f32                 getSoftCollisionExitSpeed() const { return m_softCollisionExitSpeed; }
    f32                 getSoftCollisionExitForce() const { return m_softCollisionExitForce; }
    f32                 getSoftCollisionRestoreForceDelay() const { return m_softCollisionRestoreForceDelay; }
    f32                 getSoftCollisionRestoreForceDuration() const { return m_softCollisionRestoreForceDuration; }
    f32                 getCameraLimiterMaxCoeff() const  { return m_cameraLimiterMaxCoeff; }
	f32					getStargateSuckinForceK() const { return m_stargateSuckinK; }
	f32					getStargateSuckinForceD() const { return m_stargateSuckinD; }
    f32                 getStargateNoiseAmplitude() const { return m_stargateNoiseAmplitude; }
    f32                 getStargateNoiseTimeMultiplier() const { return m_stargateNoiseTimeMultiplier; }
	f32					getStargateRotationSpeedMultiplier() const { return m_stargateTravelRotationSpeedMultiplier; }
    f32					getStargateDecelerateTime() const { return m_stargateDecelerateTime; }
    f32                 getStargateDecelerateMultiplier() const { return m_stargateDecelerateMultiplier; }
    const Path&         getStargateTravelTrailPath() const { return m_stargateTravelTrailPath; }

    f32                 getDeadSoulInactiveOptOutTime() const { return m_deadSoulInactiveOptOutTime; }

    // power-up displays
    const Ray_HeartShield_Template& getHeartShield() const { return m_heartShield; }
    const Ray_SuperPunchGauge_Template& getSuperPunchBasicGauge() const { return m_superPunchBasicGauge; }
    const Ray_SuperPunchGauge_Template& getSuperPunchSeekingGauge() const { return m_superPunchSeekingGauge; }
	const Ray_SwarmRepellerPowerUp_Template &getSwarmRepellerPowerUp() const { return m_swarmRepellerPowerUp; }

private:

    f32                         m_height;                           // The height of the character
    f32                         m_flyModeSpeed;                     // The speed for the fly mode cheat
    f32                         m_walkForce;                        // The force we apply when walking
    f32                         m_walkMiniJumpForce;                // Mini jump to go over small steps
    f32                         m_walkMiniJumpMaxStep;              // Maximum step to do a walk mini-jump
    f32                         m_iceCompensateFrictionMultiplier;  // Multiplier to character friction to compensate when turning on ice
    f32                         m_airForce;                         // The force we apply when in the air
    f32                         m_airForceWallJump;                 // The force we apply for x movement when jumping from a wall
    f32                         m_airControlDelayWallJump;          // Air control disable delay when doing a wall jump
    f32                         m_airControlDelayFall;              // Air control disable delay when falling
    f32                         m_airWallJumpExtraRadius;           // An extra radius when testing for wall jump
    f32                         m_airWallJumpCornerCheck;           // Distance on the wall to check if there is a corner
    f32                         m_airTopXSpeed;                     // When the character reaches this speed he won't be able to add more force
    f32                         m_airNoMoveFrictionX;               // A friction force on X when jumping and not pushing the stick
    f32                         m_wallRunJumpDuration;              // The duration of the wallrun jump
    f32                         m_wallRunJumpAirControlDelay;       // The air control delay when doing a wallrun jump
    f32                         m_wallRunJumpKeepSpeedFactor;       // How much speed we keep in the jump
    f32                         m_wallRunJumpForce;                 // Force of the wall run jump
    f32                         m_wallRunMinSpeed;                  // Minimum speed when wallrunning
    f32                         m_wallRunUpThreshold;               // Threshold to decide if we invert the speed to run up a wall
    f32                         m_swimingWaterResistMinSpeed;       // Minimum speed to apply the minimum friction
    f32                         m_swimingWaterResistMaxSpeed;       // Maximum speed to apply the maximum friction
    f32                         m_swimingWaterResistMaxMult;        // Maximum multiplier to water resistance
    f32                         m_swimingWaterResistEnterDuration;          // Multiplier to friction when we enter the water
    f32                         m_swimingWaterResistEnterForceSpeedMultiplier;// Force applied to brake the character as he enters the water
    f32                         m_swimingWaterResistEnterDownForceSpeedMultiplier;// Force applied to brake the character as he enters the water
    f32                         m_swimingTopSpeed;                  // Max speed when swiming
    f32                         m_swimingForce;                     // Force when swiming
    f32                         m_swimingMinExitSpeed;              // Minimum speed to exit water
    f32                         m_swimingSurfaceForceDistMultInside;// Multiplier to the force that pulls us to the surface
    f32                         m_swimingSurfaceForceSpeedMultInside;// Multiplier to the force that pulls us to the surface
    f32                         m_swimingSurfaceForceDistMultOutside;// Multiplier to the force that pulls us to the surface
    f32                         m_swimingSurfaceForceSpeedMultOutside;// Multiplier to the force that pulls us to the surface
    f32                         m_swimingSurfaceRange;              // The range to determine if we are on the surface
    f32                         m_swimingJumpRange;                 // The distance from the surface to be able to jump
    f32                         m_swimingTimeAllowAction;           // Time to allow player movements after entering the water
    f32                         m_swimingRegainControl;             // Time to regain control of the character after entering water
    f32                         m_swimingTurnDistMult;              // Angular Speed calculation: distance multiplier
    f32                         m_swimingTurnSpeedMult;             // Angular Speed calculation: speed multiplier
    f32                         m_swimingSprintMaxSpeedMultiplier;  // Multiplier to max speed for sprint power-up when swiming
    f32                         m_swimingCurveMaxOffset;            // Max offset of the swiming curve cursor
    f32                         m_swimingCurveMaxDif;               // Translate from angle to swiming curve offset
    f32                         m_swimingCurveForceS;               // The Stiff factor to update the current speed of the swiming curve cursor
    f32                         m_swimingCurveForceD;               // The Damp factor to update the current speed of the swiming curve cursor
    f32                         m_swimingTurnUpsideDownWait;        // Time we wait to turn upside-down
    f32                         m_swimingSprintJumpMinSpeed;        // Minimum speed for a sprint jump
    f32                         m_swimingSprintJumpGravityMult;     // Gravity multiplier for the swim sprint jump
    f32                         m_swimingSprintJumpGravityDuration; // Duration of the gravity multiplier
    f32                         m_swimingSprintJumpAngularSpeedMult;// A Multiplier to the angular speed while we sprint jump
    f32                         m_swimingMoveTargetMultiplierBlendTime; // Blend time for acceleration
    f32                         m_swimingDolphinYSpeed;             // Y Speed to set when doing the dolphin
    f32                         m_swimingDolphinWallJumpDisableTimer; // Wall Jump disable timer when jumping out of the water
    f32                         m_swimingPushThreshold;             // Threshold to push a wall instead of moving along
    f32                         m_swimingDashDuration;              // How long we dash when we press A
    f32                         m_moveTargetMultiplierMin;          // The minimum multiplier we apply to reach the target speed
    f32                         m_moveTargetMultiplierMax;          // The maximum multiplier we apply to reach the target speed
    f32                         m_moveTargetMultiplierBlendTime;    // The time it takes to go from the minimum multiplier to the max
    f32                         m_jumpQueueTime;                    // Time for queueing jump action while we are in the air
    f32                         m_hitQueueTime;                     // Time for queueing hit action
    f32                         m_hitDisableOnBounce;               // Time to disable the hitting when bouncing
    f32                         m_hitFilterNonFacingOpponentSpeedX; // Speed on X to know if we discard a hit on an opponent we are not facing
    f32                         m_hitQueueBlockingHitsMinDistance;  // If a hit is blocking, allow sending the next hits if they're closer than this distance
    f32                         m_windDisableOnBounce;              // The time to disable the wind while we are bouncing on an enemy
    f32                         m_jumpOnAirTime;                    // Time for allowing jumping while in the air when we just dropped from an edge
    f32                         m_jumpForceY;                       // The minimum force to apply at m_jumpForceMinXSpeed
    f32                         m_jumpForceYBounceMultiplier;       // A multiplier to the jump force when jumping from a bouncing material
    f32                         m_jumpMultiplierClimbH;             // Multiplier to jump when we are climbing horizontally with impulse
    f32                         m_jumpMultiplierNoImpulseClimbH;    // Multiplier to jump when we are climbing horizontally with no impulse
    f32                         m_jumpImpulseXClimbH;               // Impulse to jump when we are climbing horizontally and pressing the stick left or right
    f32                         m_jumpMultiplierClimbV;             // Multiplier to jump when we are climbing vertically with impulse
    f32                         m_jumpMultiplierNoImpulseClimbV;    // Multiplier to jump when we are climbing vertically with no impulse
    f32                         m_jumpImpulseXClimbV;               // Impulse to jump when we are climbing vertically and pressing the stick left or right
    f32                         m_jumpMultiplierHang;               // Multiplier to jump when we are climbing vertically with impulse
    f32                         m_jumpMultiplierNoImpulseHang;      // Multiplier to jump when we are climbing vertically with no impulse
    f32                         m_jumpImpulseXHang;                 // Impulse to jump when we are climbing vertically and pressing the stick left or right
    f32                         m_jumpMultiplierRolling;            // Multiplier to jump when we are rolling with impulse
    f32                         m_jumpMultiplierNoImpulseRolling;   // Multiplier to jump when we are rolling with no impulse
    f32                         m_jumpImpulseXRolling;              // Impulse to jump when we are rolling and pressing the stick left or right
    f32                         m_jumpMultiplierCrouch;             // Multiplier to jump when we are crouching with impulse
    f32                         m_jumpMultiplierNoImpulseCrouch;    // Multiplier to jump when we are crouching with no impulse
    f32                         m_jumpImpulseXCrouch;               // Impulse to jump when we are crouching and pressing the stick left or right
    f32                         m_jumpMultiplierFallOnVictim;       // Multiplier to jump when we fall on a victim with impulse
    f32                         m_jumpMultiplierNoImpulseFallOnVictim;// Multiplier to jump when we fall on a victim with no impulse
    f32                         m_jumpImpulseXFallOnVictim;         // Impulse to jump when we fall on a victim and pressing the stick left or right
    f32                         m_jumpMultiplierPedestal;           // Multiplier to jump when we are supporting someone with impulse
    f32                         m_jumpMultiplierNoImpulsePedestal;  // Multiplier to jump when we are supporting someone with no impulse
    f32                         m_jumpImpulseXPedestal;             // Impulse to jump when we are supporting someone and pressing the stick left or right
    f32                         m_jumpMultiplierOnPedestal;         // Multiplier to jump when we are on a pedestal
    f32                         m_jumpMultiplierNoImpulseOnPedestal;// Multiplier to jump when we are on a pedestal
    f32                         m_jumpImpulseXOnPedestal;           // Impulse to jump when we are are on a pedestal and pressing the stick left or right
    f32                         m_airSuspensionDelay;               // Delay to start applying suspension
    f32                         m_airSuspensionPushThreshold;       // If the player hold the jump button after this threshold and not tap it, we will add suspension until the end of the jump
    f32                         m_airSuspensionMinYSpeed;           // Minimum Y speed to take into account when calculating the suspension multiplier
    f32                         m_airSuspensionMaxYSpeed;           // Maximum Y speed to take into account when calculating the suspension multiplier
    f32                         m_airSuspensionMinMultiplier;       // Suspension multiplier at minimum Y speed
    f32                         m_airSuspensionMaxMultiplier;       // Suspension multiplier at maximum Y speed
    f32                         m_efficiencyMaxSpeed;               // Max speed to calculate efficiency
    f32                         m_achievedSpeedFactorDefault;       // Default value for the achievedSpeedFactor
    f32                         m_achievedSpeedFactorS;             // Multiplier to calculate the current m_achievedSpeedFactor
    f32                         m_achievedSpeedFactorD;             // Multiplier to calculate the current m_achievedSpeedFactor
    Angle                       m_minGroundAngleEfficiencyAngle;    // Minimum angle to modify the efficiency
    Angle                       m_maxGroundAngleEfficiencyAngle;    // Maximum angle to modify the efficiency
    f32                         m_minGroundAngleEfficiencyMult;     // Minimum angle to modify the efficiency
    f32                         m_maxGroundAngleEfficiencyMult;     // Maximum angle to modify the efficiency
    f32                         m_cornerPoseExtraDist;              // Extra distance to take into account the corner poses
    Angle                       m_hangMaxGroundAngle;               // The maximum angle of the ground to be able to hang
    f32                         m_hangRadiusCheck;                  // Radius to check for corners to hang
    f32                         m_hangCornerMaxSpeed;               // Maximum speed to allow going from idle_corner to hang
    Vec2d                       m_hangingCollisionCheckOffsetSolid; // Player hanging offset for the collision test
    Vec2d                       m_hangingCollisionCheckOffsetCoop;  // Player hanging offset for the collision test
    f32                         m_hangCoopDistanceCheck;            // Maximum distance from snap position to be able to hang coop
    Vec2d                       m_hangRectSize;                     // Size of the rect used to validate hanging points
    Vec2d                       m_hangCollisionExtentSolid;         // Extents of the box used to check if we collide with something when hanging (solid ground)
    Vec2d                       m_hangCollisionExtentCoop;          // Extents of the box used to check if we collide with something when hanging (coop)
    Vec2d                       m_hangRectOffset;                   // Offset of the rect used to validate hanging points
    f32                         m_hangMaxSpeed;                     // Max speed in the opposite direction of the gravity to process hanging corners
    f32                         m_hangEdgeActivationTime;           // During this time we won't be trying to hang on polylines
    f32                         m_helicopterForceX;                 // Helicopter Force
    f32                         m_helicopterUnstickMinFrictionMultiplier; //Helicopter mode, multiplier used to modify physics unstick value
    f32					        m_helicopterMinDragSpeedX;          // Speed at which drag force is applied
    f32                         m_helicopterAirFrictionMultiplier;  // air friction multiplier applied during helicopter
    f32                         m_helicopterReleaseDelay;           // Delay to stop the helico
    f32					        m_helicopterDragForceX;             // Drag force
    Angle                       m_helicopterWindForcesAngle;        // Angle of wind forces to be process by the helico
    f32                         m_helicopterWindForcesMultiplier;   // Multiplier to those wind forces while in the helico
    Angle                       m_helicopterWindSpeedAngle;         // Angle of wind forces to be process by the helico speed multiplier
    Vec2d                       m_helicopterWindSpeedMultiplier;    // Multiplier to those wind speed multiplier while in the helico
    f32                         m_helicopterMaxSpeedUp;             // Max speed to go up while doing the helicopter
    f32                         m_helicopterMaxSpeedDown;           // Max speed to go down while doing the helicopter
    f32                         m_helicopterSuspensionTime;         // Time we apply suspension doing the helicopter
    f32                         m_helicopterSuspensionMultiplier;   // How much the suspension is applied
    f32                         m_helicopterGravityMultiplier;      // Gravity multiplier when going down
    f32                         m_helicopterCooldown;               // A timer to be able to use the helicopter again
    f32                         m_airHitForceApplicationTime;       // airHit mode time to full force application
    f32                         m_airHitUnstickMinFrictionMultiplier; //airHit mode, multiplier used to modify physics unstick value
    f32					        m_airHitMinDragSpeedX;              // Speed at which drag force is applied
    f32					        m_airHitDragForceX;                 // Drag force
    f32                         m_airHitGravityMultiplier;
    Vec2d                       m_airHitSpeedMultiplier;
    Vec2d                       m_airHitSpeedMultiplierReduced;     // Multiplier to speed when we are reduced
    f32                         m_rehitTimer;
    f32                         m_fightPoseDuration;                // Duration of the fighting stance
    Angle                       m_minBounceAngle;                   // Min bounce Angle
    f32                         m_bounceJumpMargin;                 // A time margin to allow jumping after bouncing
    f32                         m_bounceHeightPlatformLevel1;       // Height for bounce from platform on level 1
    f32                         m_bounceHeightPlatformLevel2;       // Height for bounce from platform on level 2
    f32                         m_bounceHeightPlatformLevel3;       // Height for bounce from platform on level 3
    f32                         m_bounceHeightEnemyLevel1;          // Height for bounce from enemy on level 1
    f32                         m_bounceHeightEnemyLevel2;          // Height for bounce from enemyon level 2
    f32                         m_bounceHeightEnemyLevel3;          // Height for bounce from enemyon level 3
    f32                         m_bounceHeightTalkingHatLevel1;     // Height for bounce from talking hat on level 1
    f32                         m_bounceHeightTalkingHatLevel2;     // Height for bounce from talking hat level 2
    f32                         m_bounceHeightTalkingHatLevel3;     // Height for bounce from talking hat level 3
    f32                         m_bounceHeightPolylineLevel1;       // Height for bounce from polyline
    f32                         m_bounceHeightPolylineLevel2;       // Height for bounce from polyline
    f32                         m_bounceHeightPolylineLevel3;       // Height for bounce from polyline
    f32                         m_bounceHeightWave;                 // Height for bounce from wave
    f32                         m_bounceHeightWaveLevel3;           // Height for bounce from wave, level 3
    f32                         m_bounceEnemyXSpeedMultiplier;      // A multiplier to the xSpeed when bouncing on enemies
    f32                         m_bouncePlatformAirControlDelayLevel1;  // Air control delay when bouncing from a platform level 1
    f32                         m_bouncePlatformAirControlDelayLevel2;  // Air control delay when bouncing from a platform level 2
    f32                         m_bouncePlatformAirControlDelayLevel3;  // Air control delay when bouncing from a platform level 3
    f32                         m_bounceEnemyAirControlDelayLevel1; // Air control delay when bouncing from an enemy level 1
    f32                         m_bounceEnemyAirControlDelayLevel2; // Air control delay when bouncing from an enemy level 2
    f32                         m_bounceEnemyAirControlDelayLevel3; // Air control delay when bouncing from an enemy level 3
    f32                         m_bounceTalkingHatAirControlDelayLevel1; // Air control delay when bouncing from the talking hat level 1
    f32                         m_bounceTalkingHatAirControlDelayLevel2; // Air control delay when bouncing from the talking hat level 2
    f32                         m_bounceTalkingHatAirControlDelayLevel3; // Air control delay when bouncing from the talking hat level 3
    f32                         m_bouncePolylineMaxXSpeed;          // The maximum speed in the direction of the edge that we can have
    f32                         m_bounceDelay;                      // delay to make the bounce
    f32                         m_bounceRepositionDuration;         // The time it takes to position the character on the proper bounce path
    f32                         m_bounceToActorYFuncPoint0Dist;     // Distance of the first bezier point to calculate Y
    f32                         m_bounceToActorYFuncPoint1Dist;     // Distance of the second bezier point to calculate Y
    f32                         m_bounceToActorXZFuncPoint0T;       // Percentage of the first bezier point to calculate X and Z
    f32                         m_bounceToActorXZFuncPoint1T;       // Percentage of the first bezier point to calculate X and Z
    f32                         m_bounceToActorSpeed;               // Speed to calculate the time it will take to do the jump
    f32                         m_bounceToActorMinTime;             // Min time of the bouncing
    f32                         m_windTunnelExitSpeedLevel0;        // The exit speed from a wind tunnel with level 0
    f32                         m_windTunnelExitSpeedLevel1;        // The exit speed from a wind tunnel with level 1
    f32                         m_windTunnelExitSpeedLevel2;        // The exit speed from a wind tunnel with level 2
    Vec2d                       m_climbingCollisionCheckExtent;     // The extent of the box to check for collision while climbing
    f32                         m_climbVerticalSpeedUp;             // The climb speed for the vertical edges, climbing up
    f32                         m_climbVerticalAscendAccel;         // The acceleration when ascending from a rope
    f32                         m_climbVerticalDescendAccel;        // The acceleration when descending from a rope
    f32                         m_climbVerticalDescendMaxSpeed;     // The maximum speed when descending from a rope
    f32                         m_climbVerticalDescendMaxSpeedSprint;// The maximum speed when descending from a rope if sprinting
    f32                         m_climbHorizontalSpeed;             // The speed to move on horizontal ropes
    f32                         m_climbHorizontalMoveAnimRate;      // The rate at which the climb horizontal animation will play
    f32                         m_climbSprintSpeedMultiplier;       // Multiplier to speed when sprinting
    f32                         m_climbHorizontalDescendMaxSpeed;   // Max speed when sliding through a horizontal rope
    f32                         m_climbHorizontalDescendMaxSpeedSprint;// Max speed when sliding through a horizontal rope if sprinting
    f32                         m_climbSlideDisableAirControl;      // Disable the air control for a while when jumping from a sliding rope
    f32                         m_climbEdgeInertiaBrake;                // The deceleration of the inertia
    f32                         m_climbEdgeDistanceRange;           // The range to accept polylines to grab for climbing
    f32                         m_climbEdgeActivationTime;          // During this time we won't be trying to climb on polylines
    f32                         m_climbEdgeSwingImpulse;            // The swing strength when climbing ropes
    f32                         m_climbEdgeSwingImpulseReduced;     // The swing strength when climbing ropes when the player is reduced
    f32                         m_climbEdgePunchImpulse;            // When we punch while climbing a rope we apply an impulse in the opposite direction
    f32                         m_climbEdgeMoveImpulse;             // Impulse applied on the rope when the character grabs it on the move cycle
    Angle                       m_climbEdgeAngularSpeed;            // Angular speed to match the target angle
    f32                         m_climbEdgeCoopHangEdgeRange;       // Range allowed to be able to support someone
    Vec2d                       m_climbMaxJumpSpeedTransfer;        // The maximum speed we can transfer to Rayman when jumping from a climbing
    Vec2d                       m_punchShapeOffset;                 // Offset for the punch shape when sending the stim
    f32                         m_receivePunchFrontWeakForce;       // Hit force applied when receiving a front punch weak
    f32                         m_receivePunchFrontStrongForce;     // Hit force applied when receiving a front punch strong
    f32                         m_receivePunchFrontMegaForce;       // Hit force applied when receiving a front punch mega
    f32                         m_receivePunchFrontWeakFrictionMult;// Friction multiplier when receiving a punch front weak hit
    f32                         m_receivePunchFrontStrongFrictionMult;// Friction multiplier when receiving a punch front strong hit
    f32                         m_receivePunchFrontMegaFrictionMult;// Friction multiplier when receiving a punch front mega hit
    f32                         m_receivePunchUpWeakForce;          // Ejecting Y force on the character when receiving a punch up weak hit
    f32                         m_receivePunchUpStrongForce;        // Ejecting Y force on the character when receiving a punch up strong hit
    f32                         m_receivePunchUpMegaForce;          // Ejecting Y force on the character when receiving a punch up strong hit
    f32                         m_receivePunchUpWeakGravityMult;    // Gravity multiplier when receiving a punch up weak
    f32                         m_receivePunchUpStrongGravityMult;  // Gravity multiplier when receiving a punch up strong
    f32                         m_receivePunchUpMegaGravityMult;    // Gravity multiplier when receiving a punch up mega
    f32                         m_receiveEarthquakeWeakForce;       // Down force when performing the weak earthquake on someone in the air
    f32                         m_receiveEarthquakeStrongForce;     // Down force when performing the strong earthquake on someone in the air
    f32                         m_receiveEarthquakeMegaForce;       // Down force when performing the mega earthquake on someone in the air
    f32                         m_receiveBounceVWeakForce;          // Force of the weak hurt bounce
    f32                         m_receiveBounceVStrongForce;        // Force of the strong hurt bounce
    f32                         m_receiveBounceVMegaForce;          // Force of the mega hurt bounce
    f32                         m_receiveBounceHWeakForce;          // Force of the weak hurt bounce
    f32                         m_receiveBounceHStrongForce;        // Force of the strong hurt bounce
    f32                         m_receiveBounceHMegaForce;          // Force of the mega hurt bounce
    f32                         m_receiveBounceWeakFrictionMult;    // Friction multiplier when receiving a hurt bounce weak
    f32                         m_receiveBounceStrongFrictionMult;  // Friction multiplier when receiving a hurt bounce strong
    f32                         m_receiveBounceMegaFrictionMult;    // Friction multiplier when receiving a hurt bounce mega
    f32                         m_receiveBounceWeakSwimingMult;     // Multiplier to the bounce force when we are swiming
    f32                         m_receiveBounceStrongSwimingMult;   // Multiplier to the bounce force when we are swiming
    f32                         m_receiveBounceMegaSwimingMult;     // Multiplier to the bounce force when we are swiming
    f32                         m_receiveHitSwimGravityMultiplier;  // The gravity multiplier when receiving a hit while swiming
    f32                         m_slideFrictionMultiplier;          // Friction for the character when sliding
    f32                         m_slideMinFastSpeed;                // The minimum speed to be considered fast slide
    u32                         m_slideHitLevel;                    // The level of the hit when sliding
    f32                         m_slideMinSpeedToAllow;             // Minimum speed to start sliding
    f32                         m_slideMinSpeedMultMinFriction;     // The multiplier to the m_slideMinSpeed when friction is <=0
    f32                         m_slideMinSpeedMultMaxFriction;     // The multiplier to the m_slideMinSpeed when friction is >=1
    f32                         m_slideFallDelay;                   // The time we remain on slide pose while falling
    f32                         m_surfingJumpTime;                  // Trigger surfing pose time
    f32                         m_hoverMinYSpeed;                   // Hover min y speed
    f32                         m_metrailleMaxHits;                 // Time to release the final hit of the metraille
    u32                         m_metrailleFinalHitLevel;           // The level of the hit
    u32                         m_metrailleGroundVariation;         // Number of available animations for ground burst
    u32                         m_metrailleAirVariation;            // Number of available animations for air burst
    u32                         m_metrailleMaxBufferedHits;         // Max number of hits with the burst
    f32                         m_accrobaticStageKeepTime;          // Time to keep the accrobatic stage
    f32                         m_swingSpeedAdjustTime;             // The time it takes to adjust the speed of the swing
    f32                         m_swingInterpolateTime;             // The time we take to interpolate to the right position
    f32                         m_swingHitAreaHeight;               // Swing area height
    f32                         m_swingHitAreaOffset;               // Swing area offset
    f32                         m_swingHitMinAngularSpeed;          // Minimum angular speed to be able to hit
    f32                         m_swingInitialBoost;                // Boost to speed when attaching
    f32                         m_swingJumpXForceThreshold;         // Threshold to determine the x force of the jump
    f32                         m_swingJumpXAngleThreshold;         // Threshold to determine the x force of the jump
    f32                         m_swingJumpForceX;                  // The X force when jumping from swinging
    f32                         m_swingJumpDelay;                   // A delay while jumping is disabled
    u32                         m_uTurnHitLevel;                    // The level of the uturn kick
    u32                         m_uTurnAirHitLevel;                 // The level of the uturn kick in the air
    u32                         m_crouchHitLevel;                   // The level of the crouched kick
    u32                         m_tornadoHitLevel;                  // The level of the tornado kick
    u32                         m_spinHitLevel;                     // The level of the spin hit
    u32                         m_reducedSizeHitLevel;              // The level of the reduced size hit
    u32                         m_upperKickHitLevel;                // The level of the attack pressing up in the air
    u32                         m_crushHitLevel;                    // The level of the crush hit
    f32                         m_crushHitLevelIncrTimeFirst;       // The time to increase the level of the crush hit the first time
    f32                         m_crushHitLevelIncrTimeSecond;      // The time to increase the level of the crush hit the second time
    f32                         m_crushRetriggerDelay;              // A time delay to be able to do the crush attack again
    u32                         m_slideDownHitLevel;                // The level of the hit when sliding down on a character
    u32                         m_swingHitLevel;                    // The level of the swing hit
    u32                         m_hangCoopHitLevel;                 // The level of the hang coop hit
    u32                         m_burstHitLevel;                    // The level of the burst hit
    u32                         m_burstAirHitLevel;                 // The level of the burst hit in the air
    u32                         m_normalPunchLevelForShakeCam;      // The minimum level at which we will shake the camera when hittin a wall
    f32                         m_switchHitBufferTime;              // A time interval in which we can press the stick in another direction and execute a different hit
    f32                         m_uTurnComboTime;                   // Time to allow uturn combos
    f32                         m_crushHitSuspensionTime;           // Time to hang in the air while doing the earthquake punch
    f32                         m_crushHitMinYSpeed;                // The minimum speed in Y to give an upward impulse
    f32                         m_crushHitYMultiplier;              // The Y speed multiplier when performing the earthquake
    f32                         m_crushHitDownForce;                // Force applied when we release the earthquake hit
    f32                         m_crushHitFriction;                 // Friction when performing a crush hit
    f32                         m_slideDownHitMinSpeed;             // Minimum speed to release slide down hits
    f32                         m_swimAttackNormalDuration;         // Swim attack normal duration
    f32                         m_swimAttackNormalSpeed;            // Swim attack normal speed
    f32                         m_swimAttackSprintDuration;         // Swim attack sprinting duration
    f32                         m_swimAttackSprintSpeed;            // Swim attack sprinting speed
    f32                         m_swimAttackCooldown;               // Swim attack cooldown
    f32                         m_swimAttackFriction;               // Swim attack friction multiplier
    f32                         m_swimAttackMinSpeed;               // Swim attack minimum speed to abort
    f32                         m_swimAttackMoveDamping;            // Damping for move input when doing the tornado
    f32                         m_swimAttackSpamTime;               // Swim attack spam time (the time to consider it spam)
    f32                         m_swimAttackSpamImpulseMultiplier;  // Impulse multiplier when spamming the swim attack
    f32                         m_swimAttackSpamImpulseMultiplierSprint;  // Impulse multiplier when spamming the swim attack in sprint
    f32                         m_tornadoDuration;                  // Duration of the tornado
    f32                         m_tornadoSpeed;                     // Min Speed set when performing the tornado
    f32                         m_tornadoCooldown;                  // Cooldown for tornado
    f32                         m_tornadoMinSpeed;                  // Minimum speed to keep performing the tornado
    f32                         m_tornadoFriction;                  // Friction to set when performing the tornado
    f32                         m_tornadoAllowMoveTime;             // Time at which we allow to stop the tornado
    f32                         m_reducedSizeHitDuration;           // Reduced size hit duration
    f32                         m_reducedSizeHitAirDuration;        // Reduced size hit duration in the air
    f32                         m_reducedSizeHitUpDuration;         // Reduced size hit up duration
    f32                         m_reducedSizeHitFriction;           // Friction multiplier when we punch while having a reduced size
    f32                         m_reducedSizeHitGravity;            // Gravity multiplier when we punch while having a reduced size
    f32                         m_reducedSizeHitSideForceGround;    // Force left or right when performing the reduced size hit
    f32                         m_reducedSizeHitSideForceAir;       // Force left or right when performing the reduced size hit
    f32                         m_reducedSizeHitUpForce;            // Force up when performing the reduced size hit up
    f32                         m_reducedSizeHitMinSpeed;           // Minimum speed to keep doing the dash attack
    f32                         m_reducedSizeHitCrushAttackQueueLength; // For how long we jump before performing the crush attack
    f32                         m_reducedSizeHitCrushDistance;      // The distance of the crush attack when performed from the ground
    f32                         m_reducedSizeHitCooldown;           // Cooldown to be able to hit again while reduced
    f32                         m_reducedSizeHitUpCooldown;         // Cooldown to be able to hit up again while reduced
    f32                         m_reducedSizeHitGroundFrictionMultiplierMin;    // A multiplier to the friction based on the ground friction
    f32                         m_reducedSizeHitGroundFrictionMultiplierMax;    // A multiplier to the friction based on the ground friction
    StringID                    m_hitFxBoneName;                    // Name of the bone where we place hit fx
    f32                         m_prepareHitLengthWeak;             // The length of the prepare hit weak stage
    f32                         m_prepareHitLengthStrong;           // The length of the prepare hit strong stage
    f32                         m_prepareHitLengthMega;             // The length of the prepare hit mega stage
    f32                         m_rehitDelayPermanentHit;           // Delay to rehit a character when doing the spin attack, tornado, etc
    f32                         m_rehitDelayComboHit;               // Delay to rehit a character when doing combos
    f32                         m_wallSlideStartDelay;              // Delay to start wallsliding
    f32                         m_wallSlideStartDelaySpeedThreshold;// Threshold to override the delay start wallslide
    f32                         m_wallSlideAngularSpeedMinLinear;   // Minimum linear speed to take into account to calculate the angular speed
    f32                         m_wallSlideAngularSpeedMaxLinear;   // Maximum linear speed to take into account to calculate the angular speed
    Angle                       m_wallSlideAngularSpeedMin;         // Minimum angular speed for minimum linear speed
    Angle                       m_wallSlideAngularSpeedMax;         // Maximum angular speed for minimum linear speed
    f32                         m_wallSlideBrakeTime;               // Time to break on wall slide
    f32                         m_wallSlideStartSpeed;              // Speed when starting to wall slide
    f32                         m_wallSlideGravityMultiplier;       // Gravity multiplier when sliding down
    f32                         m_wallSlideJumpXImpulse;            // Impulse on X when jumping on a wall slide
    f32                         m_wallSlideFrictionCoefCondition;   // The friction coeficient that triggers the wall slide when we are idle
    f32                         m_wallSlideMinEdgeMargin;           // Minimum distance to the end of the edge to be able to slide
    Angle                       m_wallPushWallAngle;                // The maximum angle between the wall and the ground
    f32                         m_wallPushForce;                    // The force for pushing objects
    f32                         m_wallPushDelay;                    // The delay to start pushing objects
    f32                         m_wallPushRepushDelay;              // The delay to stay in push mode just after we send a force
    f32                         m_wallSlideMaxSpeed;                // Max speed for wall slide
    f32                         m_wallSlideUnstickDelay;            // A delay to unstick from wallslide when pushing in the opposite direction
    f32                         m_wallSlideRestickDelay;            // When we unstick from the wallslide by pushing we set a timer to be able to restick again
    f32                         m_wallSlideExtraRadius;             // An extra radius to check for wallslide walls
    f32                         m_wallSlideYSpeedThresholdToHang;   // A threshold to check if we go from wallslide to hang
    f32                         m_disableCrouchTime;                // How long we disable the crouch (comes from lua)
    f32                         m_crouchUnhangTime;                 // Time where we allow to unhang from polylines while uncrouching
    f32                         m_rollingFrictionMultiplier;        // The friction multiplier while rolling
    f32                         m_rollingMinSpeed;                  // Minimum speed to roll
    f32                         m_fallOnVictimMinSpeed;             // Minimum speed to be falling on someone
    f32                         m_sprintActivationLandingDelay;     // A delay to enable the sprint after landing
    f32                         m_sprintForceMultiplier;            // A multiplier to the work force when sprinting
    f32                         m_sprintEfficiencyMultiplier;       // A multiplier to the efficiency max speed when sprinting
    f32                         m_sprintMaxSpeedBlendDuration;      // The duration of the blend to go from regular walk speed to spring speed
    f32                         m_sprintReleaseForceMultiplier;     // The force multiplier to the previous movement when we release the sprint
    f32                         m_sprintReleaseForceMax;            // Clamp for the sprint release force
    Path                        m_bounceToLayerFXFile;              // Bounce to layer FX file
    Path                        m_tinyFXFile;
    Color                       m_tinyTrailSprintColor;             // The color of the trail when sprinting
    Color                       m_tinyTrailWallRunColor;            // The color of the trail when wallrunning
    Color                       m_tinyTrailWallRunJumpColor;        // The color of the trail when wallrun jumping
    Color                       m_tinyTrailNormalColor;             // The color of the trail when sprinting
    f32                         m_punchPushbackRadius;
    PhysShape*                  m_phantomShapeCrouch;               // The shape used by the phantom when we are crouched
    PhysShape*                  m_phantomShapeSwim;
    f32                         m_inmunityTime;
    f32                         m_inmunityTimeReceiveHit;
    f32                         m_inmunityTimeRevive;
    f32                         m_inmunityTimeReceiveHitRevive;
    f32                         m_squashBounceMargin;
    u32                         m_squashHurtLevel;
    f32                         m_squashDeathPenetration;
    StringID                    m_characterSnapBone;
    Ray_PlayerControllerComponent::StateDeadSoul_Template m_stateDeadSoul;
    Ray_PlayerControllerComponent::StateRevive_Template m_stateRevive;
    f32                         m_avoidanceRadius;                  // radius used for squad manager
    f32                         m_darktoonificationPosOffset;
    f32                         m_pedestalWalkMultiplier;
    f32                         m_pedestalAreaRadius;
    f32                         m_pedestalDistanceCheck;
    f32                         m_pedestalFeetDistanceCheck;
    f32                         m_pedestalDisableTimer;
    Angle                       m_pedestalInputCone;
    f32                         m_onPedestalLandBounceHeight;
    f32                         m_deathNormalWait;
    f32                         m_moveCursorMinEfficiency;
    f32                         m_moveCursorMaxEfficiency;
    f32                         m_moveCursorMinAngle;
    f32                         m_moveCursorMaxAngle;
    f32                         m_moveCursorMinMultiplier;
    f32                         m_moveCursorMaxMultiplier;
    f32                         m_moveCursorEfficiencyAngle;
    f32                         m_walkOnWallsDisableDelay;          // A delay to deactivate the walk on walls, to handle change of directions
    f32                         m_walkOnWallsReleaseStickDuration;  // Time to wait for unsticking if the joystick is release while wallrunning
    Path                        m_deathFXActorSpawn;
    Margin                      m_offscreenDeathMargin;         // if the player dies offscreen, bring him back to the closest edge of the screen + margin
    f32                         m_offscreenDeathSmooth;         // if the player dies offscreen, bring him back using this smooth factor
    Margin                      m_offscreenDeathFxMargin;       // if the player dies offscreen, play the death fx on the closest edge of the screen + margin
    bbool                       m_applyWindSpeedLimitation;

    f32                         m_clampingMaxSpeedTotal;
    f32                         m_clampingMaxSpeedX;
    f32                         m_clampingMaxSpeedY;
    f32                         m_airBrakeMaxSpeedX;

    f32                         m_softCollisionRadiusMultiplier;
    f32                         m_softCollisionExitSpeed;
    f32                         m_softCollisionExitForce;
    f32                         m_softCollisionRestoreForceDelay;
    f32                         m_softCollisionRestoreForceDuration;
    f32                         m_cameraLimiterMaxCoeff;
    
    // Stargate
	f32							m_stargateSuckinK;
	f32							m_stargateSuckinD;
    f32                         m_stargateNoiseAmplitude;
    f32                         m_stargateNoiseTimeMultiplier;
	f32							m_stargateTravelRotationSpeedMultiplier;
    f32                         m_stargateDecelerateTime;
    f32                         m_stargateDecelerateMultiplier;
    Path                        m_stargateTravelTrailPath;


    // Bubble dead soul
    f32                         m_deadSoulInactiveOptOutTime;

    // power-up displays
    Ray_HeartShield_Template    m_heartShield;
    Ray_SuperPunchGauge_Template m_superPunchBasicGauge;
    Ray_SuperPunchGauge_Template m_superPunchSeekingGauge;
	Ray_SwarmRepellerPowerUp_Template m_swarmRepellerPowerUp;
};

ITF_INLINE const Ray_PlayerControllerComponent_Template* Ray_PlayerControllerComponent::getTemplate() const
{
    return static_cast<const Ray_PlayerControllerComponent_Template*>(m_template);
}

} // namespace ITF

#endif //_ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_
