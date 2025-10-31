
#ifndef _ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_
#define _ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_

#ifndef _ITF_PHYSCOMPONENT_H_
#include "engine/actors/components/physcomponent.h"
#endif //_ITF_PHYSCOMPONENT_H_

#ifndef _ITF_PHYSTYPES_H_
#include "engine/physics/PhysTypes.h"
#endif //_ITF_PHYSTYPES_H_

#ifndef _ITF_PHYSSHAPES_H_
#include "engine/physics/PhysShapes.h"
#endif //_ITF_PHYSSHAPES_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

namespace ITF
{

class StickToPolylinePhysComponent : public PhysComponent
{
    DECLARE_OBJECT_CHILD_RTTI(StickToPolylinePhysComponent,PhysComponent,3089159513);

public:
    DECLARE_SERIALIZE()

    enum EdgeOrientationType
    {
        EDGEORIENTATIONTYPE_GROUND,
        EDGEORIENTATIONTYPE_WALL,
        EDGEORIENTATIONTYPE_ROOF,
        ENUM_FORCE_SIZE_32(0)
    };

    StickToPolylinePhysComponent();
    virtual ~StickToPolylinePhysComponent();

    virtual bbool               needsUpdate() const { return btrue; }
    virtual bbool               needsDraw() const { return bfalse; }
    virtual void                Update( f32 _deltaTime );
    virtual void                onForceMove( );
#ifdef ITF_SUPPORT_EDITOR
    virtual void                onEditorMove(bbool _modifyInitialPos = btrue);
#endif // ITF_SUPPORT_EDITOR
    virtual void                onEvent(Event * _event);
    virtual void                onUpdatePos();
    virtual void                onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
    virtual void                onCheckpointLoaded();

    // Accessors
    Vec2d                       calculateGroundDir() const;
    const Vec2d&                getCurrentGroundDir() const { return m_currentGroundDir; }
    const PhysContactsContainer& getContacts() const { return m_contacts; }
    f32                         getFrictionCoef() const { return m_currentFrictionCoef; }
    class PolyLine*             getStickedPolyline() const;
    const class PolyLineEdge*   getStickedEdge() const;
    ObjectRef                   getStickedPolylineRef() const { return m_stickedLineRef; }
    u32                         getStickedEdgeIndex() const { return m_stickedEdge; }

    void                        setFrictionMultiplier( f32 _mult ) { m_frictionMultiplier = _mult; }
    f32                         getFrictionMultiplier() const { return m_frictionMultiplier; }

    void                        setAngularSpeedMultiplier( f32 _mult ) { m_angularSpeedMultiplier = _mult; }

    ITF_INLINE f32              getAirFriction() const;
    f32                         getTotalAirFriction() const { return getAirFriction() * getAirFrictionMultiplier(); }
    void                        setAirFrictionMultiplier( f32 _mult ) { m_airFrictionMultiplier = _mult; }
    f32                         getAirFrictionMultiplier() const { return m_airFrictionMultiplier; }

    void                        setSpeedMultiplier( const Vec2d& _mult ) { m_speedMultiplier = _mult;}
    const Vec2d&                getSpeedMultiplier() const { return m_speedMultiplier; }

    ITF_INLINE f32              getUnstickMinFrictionMultiplier() const { return m_unstickMinFrictionMultiplier; }
    ITF_INLINE void             setUnstickMinFrictionMultiplier(f32 val) { m_unstickMinFrictionMultiplier = val; }

    f32                         getAngleWRTGravity();
    Vec2d                       getSpeedWRTGravity();

    void                        setDisabled( bbool _val, bbool _unstick, bbool _clearInfo );
    void                        setDisabled( bbool _val) { setDisabled(_val, btrue, btrue); }
    void                        setProcessContacts(bbool _val) { m_processContacts = _val;}

    const Vec2d&                getWorldSpeed() const { return m_worldSpeed; }
    f32                         getWorldSpeedAverageX() const { return m_averageXSpeed; }
    f32                         getWorldSpeedAverageY() const { return m_averageYSpeed; }
    Vec2d                       getWorldSpeedAverage() const { return Vec2d(m_averageXSpeed,m_averageYSpeed); }
    f32                         getTorqueAverage() const { return m_averageTorque; }
    bbool                       getIsRotatingWithTorque() const { return m_rotateWithTorque; }
    void                        resetTorque();
    void                        resetWorldSpeed();

    f32                         getRelativeEdgeDistance() const { return m_relDist; }

    // We use this when we go from hang to climb to make sure at the end we stick on the polyline we climbed
    void                        forceStickOnEdge( class PolyLine* _polyLine,
                                                  const class PolyLineEdge* _edge,
                                                  u32 _edgeIndex );
    void                        forceFindStickEdge();

    // Set if we want the physics to update our rotation or not
    void                        setUpdateRotation( bbool _val ) { m_updateRotation = _val; }

    // Releases us from an edge
    void                        unstick( bbool _addToPlatformMemory = bfalse );

    //speed loss mutliplier
    ITF_INLINE f32              getSpeedLossMultiplier() const { return m_speedLossMultiplier; }
    ITF_INLINE void             setSpeedLossMultiplier(f32 _val) { m_speedLossMultiplier = _val; }

    Vec2d                       getStickedPosition();
    const Vec2d&                getPreviousPos() const { return m_prevPosition; }

    void                        setStickOnWalls( bbool _val, bbool _checkConvex = btrue, bbool _shiftGravity = btrue ) { m_stickOnWalls = _val; m_stickOnWallsCheckConvex = _checkConvex; m_stickOnWallsShiftGravity = _shiftGravity; }
    void                        setStickOnWallsInputDir( const Vec2d& _dir ) { m_stickOnWallsInputDir = _dir; }
    bbool                       getStickOnWalls() const { return m_stickOnWalls; }
    EdgeOrientationType         getEdgeOrientationType( const Vec2d& _dir ) const;

    void                        setStickingDisabled( bbool _val ) { m_disableSticking = _val; }

    void                        setSecondaryCollisionCheck( const Vec2d& _pos, f32 _radius );

    // Max speed clamping
    void                        enableMaxSpeedClamping( f32 _maxSpeedTotal, f32 _maxSpeedX, f32 _maxSpeedY );
    void                        disableMaxSpeed();

    void                        setDisableWindForces( bbool _val ) { m_disableWindForces = _val; }

    f32                         getWallAngle() const;
    f32                         getRoofAngle() const;

    // Use it at your own risk
    void                        advanceDistance( f32 _dist );

    static const f32            s_wallRunAngleThreshold;
    static const f32            s_physRoofAngle;
    static const f32            s_physWallAngle;

private:


    ITF_INLINE const class StickToPolylinePhysComponent_Template*  getTemplate() const;

    struct TravelEdgeData
    {
        TravelEdgeData() : m_polyRef(ITF_INVALID_OBJREF), m_edgeIndex(U32_INVALID) {}
        TravelEdgeData( ObjectRef _ref, u32 _edge ) : m_polyRef(_ref), m_edgeIndex(_edge) {}

        ObjectRef   m_polyRef;
        u32         m_edgeIndex;
    };

    struct EdgeMemoryData
    {
        EdgeMemoryData() : m_polyRef(ITF_INVALID_OBJREF), m_edgeIndex(U32_INVALID), m_time(0.f) {}
        EdgeMemoryData( ObjectRef _ref, u32 _edge, f32 _time ) : m_polyRef(_ref), m_edgeIndex(_edge), m_time(_time) {}

        ObjectRef   m_polyRef;
        u32         m_edgeIndex;
        f32         m_time;
    };

    struct DynamicContactData
    {
        PolyLine*           m_polyline;             // The polyline that collides
        const PolyLineEdge* m_edgeStart;            // The edge at the start of the frame
        const PolyLineEdge* m_edgeEnd;              // The edge at the end of the frame
        Vec2d               m_edgePos0Start;        // The first point of the edge at the start of the frame
        Vec2d               m_edgePos1Start;        // The second point of the edge at the start of the frame
        Vec2d               m_edgePos0End;          // The first point of the edge at the end of the frame
        Vec2d               m_edgePos1End;          // The second point of the edge at the end of the frame
    };

    typedef FixedArray <DynamicContactData,32> DynContactDataList;

    // Computes the total forces (current forces, gravity, friction...)
    void                calculateSpeedFromForces( f32 _deltaTime );
    void                applyFrictionRegion( bbool& _sticked );
    void                applyGroundFrictionRegion( f32& _friction );

    // Sets our position and angle properly on the edge, based on our relative distance and angle
    void                getPosFromRelDist( PolyLine* _poly, const PolyLineEdge* _edge, u32 _edgeIndex, f32 relDist, f32 _t, Vec2d& _pos ) const;
    // Calculates our relative distance from the current position
    void                getRelDistFromPos( const Vec2d& _pos, f32 _t, f32& _relDist, f32& _relAngle ) const;
    // Adjusts our current relative distance and angle if we are out of the margins
    bbool               adjustCurrentRelDist( f32 _t, Vec2d& _speed );
    // Recalculates our relative position based on the world position, also updates the world position if we are sticked, unsticks if we go out of the edge, etc...
    void                recalculateRelPos( f32 _t, Vec2d& _pos, Vec2d& _speed );

    // Stick to an edge
    void                stick( class PolyLine* _polyLine, u32 _edgeIndex, Vec2d& _pos, f32 &_angle, f32 _t = 1.f );
    // Sets the current sticked edge
    void                setStickedEdge( class PolyLine* _polyLine, u32 _edgeIndex );
    // Projects our speed on the current edge
    Vec2d               projectSpeedOnDir( const Vec2d& _newDir, const Vec2d& _currentSpeed ) const;
    // Sets our position and angle from our relative position and angle from the edge, without transitions, within the limits
    void                SetEdgePosition( PolyLine* _polyLine, const class PolyLineEdge& _edge, u32 _edgeIndex, f32 _relDist, Vec2d& _pos, f32 _t = 1.f ) const;
    // Sets the final position and orientation on the actor and the physic body
    void                applyPosAndAngle( const Vec2d& _pos, f32 _angle );

    // This checks if based on our current speed we should detach from the current edge
    bbool               checkIfDetachFromCurrentEdgeWithSpeed( f32 _moveSide, f32 _t );

    // Calculates the angular speed based on the linear speed
    f32                 calculateAngularSpeedFromLinearSpeed( bbool _torqueRotation ) const;
    void                setCurrentGroundAngularSpeedFromRotation( f32 _currentRotation, f32 _targetRotation );

    // Transfers the speed from one direction into another
    void                transferSpeedToNewEdge( const Vec2d& _prevDir, const Vec2d& _newDir, bbool _back,
                                                Vec2d& _speed ) const;

    // Transfers the speed we carried from the air into the edge we landed on
    void                transferSpeedFromLanding( const Vec2d& _newDir, Vec2d& _speed,
                                                  const class PolyLine* _poly,
                                                  const class PolyLineEdge* _edge,
                                                  u32 _edgeIndex,
                                                  f32 _relDist ) const;

    // Readjusts the speed when we are blocked by an obstacle
    void                transferSpeedFromBlockingObstacle( const SCollidableContact& _contact,
                                                           f32 _t,
                                                           Vec2d& _newSpeed ) const;

    // Check contacts to see if we stick, or if we are blocked by something
    void                processContacts( const Vec2d& _prevPos, Vec2d& _pos, f32 _angle, f32 _extraRadius, bbool& _blocked, Vec2d& _speed, f32 _dt );
    void                processContactsUpdateStickPlatforms();
    void                processContactsStaticEnv( const Vec2d& _prevPos, Vec2d& _pos, f32 _angle, f32 _extraRadius, bbool _reuseContacts, bbool& _blocked, Vec2d& _speed );
    void                processContactsDynamicEnv( const Vec2d& _prevPos, Vec2d& _pos, f32 _angle, f32 _extraRadius, bbool& _blocked, Vec2d& _speed, f32 _dt );
    void                processContactsDynamicEnv_findDynContacts( const PhysContactsContainer& _allContacts, PhysContactsContainer& _dynContacts ) const;
    f32                 processContactsDynamicEnv_findTIncrement( const PhysContactsContainer& _dynContacts, f32 _dt, f32 _myMove ) const;
    void                processContactsDynamicEnv_resolveContacts( const PhysContactsContainer& _dynContacts, const Vec2d& _prevPos, Vec2d& _pos, f32 _angle, f32 _extraRadius, Vec2d& _speed, bbool& _blocked, f32 _dt );
    bbool               processContactsDynamicEnv_resolveBlockingContact( const SCollidableContact& _contact, const Vec2d& _prevPos, Vec2d& _newPos, Vec2d& _newSpeed, f32 _angle, f32 _t, const class PhysShapeCircle& _circle );

    // Returns true if a contact contains a valid polyline where we can stick
    bbool               isContactValidToStick( const struct SCollidableContact& _contact, f32 _t, const Vec2d& _pos, f32 _angle, bbool _dynCollisionTest, const Vec2d& _speed ) const;

    // Returns true if a contact is considered an blocking obstacle
    bbool               isContactBlockingObstacle( const struct SCollidableContact& _contact, f32 _angle, const Vec2d& _moveDir, f32 _t = 1.f ) const;
    bbool               isContactBlockingObstacle( const Vec2d& _edgePos0, const Vec2d& _edgeDir, f32 _edgeLength, const class GameMaterial_Template* _gameMaterial,
                                                   PolyLine* _poly, const PolyLineEdge* _edge, f32 _t,
                                                   const SCollidableContact& _contact, const Vec2d& _moveDir ) const;

    // Process hitting an obstacle
    void                processContactObstacle( const struct SCollidableContact& _contact, f32 _t,
                                                Vec2d& _pos, Vec2d& _speed, bbool& _blocked );

    // This will travel _dist along the polyline and return the edge and relative distance from it
    void                travelToEdge( class PolyLine* _polyLine, u32 _edgeIndex, f32 _dist, f32 _t,
                                      class PolyLine*& _resPoly, u32& _resEdge, f32& _resRelDist ) const;
    bbool               findIntersectionBetweenEdges( const class PolyLineEdge* _currentEdge, const AIUtils::AdjacentEdgeData& _adjEdgeData, bbool _checkBack, Vec2d& _intersection ) const;

    // Calculates the current friction coefficient with the forces from this frame
    void                calculateFrictionCoefficient();

    // Counts the first travelled edges of the frame
    void                prepareTravelledEdgesThisFrame();

    // Move the character in time
    void                move( f32 _dt, Vec2d& _pos, f32 _angle );
    void                moveLoop( f32 _dt, Vec2d& _speed, bbool& _blocked, const Vec2d& _prevPos, Vec2d& _pos, f32 _angle, f32& _distanceAdvanced );
    void                moveFree( f32 _dt, Vec2d& _speed, Vec2d& _pos );
    void                moveSticked( f32 _dt, Vec2d& _speed, Vec2d& _pos );
    void                applyMaterialPreMove();
    void                applyMaterialPostMove( f32 _dt, bbool& _blocked, Vec2d& _pos, f32 _angle, f32& _distanceAdvanced );

    void                applyForces( f32 _dt );
    void                setWindForces();

    // Rotates the character in time
    void                rotate( f32 _deltaTime, const Vec2d& _pos, f32& _angle );

    void                updateCurrentGravity( const Vec2d& _pos );

    ITF_INLINE f32      getUnstickMinFriction() const;
    ITF_INLINE f32      getUnstickMinForceExternal() const;

    // Speed Average
    void                measureSpeedAverage( f32 _deltaTime );
    void                measureValueAverage( f32 _deltaTime, f32 _currentSpeed, bbool resetOnChange, f32& _averageSpeed, f32* _measures );
    void                measureTorqueAverage( f32 _deltaTime );

    // Corner landing case
    bbool               isCornerCase( PolyLine* _poly, u32 _edgeIndex,
                                      f32 _relDist, const Vec2d& _speed, f32 _t,
                                      u32& _case ) const;


    // Event processing
    void                processQueryPhysics( class EventQueryPhysicsData* _query ) const;

    // Checks if the initial forces will make us unstick from the polyline
    void                checkUnstickFromForces();

    f32                 calculateCurrentGroundAngle( f32 _actorAngle ) const;

    // We store where we are on the edge as a factor of the angle and distance, so that we can restore it at the begining of the frame in case the polyline moved
    void                updateEdgeFactors( const AIUtils::AdjacentEdgeData& _adjacentEdgeData,
                                           PolyLine* _currentPolyline, const PolyLineEdge* _currentEdge, u32 _currentEdgeIndex, f32 _relDist, f32 _t,
                                           f32& _insideFactor, f32& _betweenFactor ) const;
    f32                 getRelDistFromEdgesFactor() const;

    bbool               isEdgeInMemory( ObjectRef _ref, u32 _edgeIndex ) const;
    void                updateEdgeMemory( f32 _dt );

    void                clampWithMaxSpeed( Vec2d& _speed ) const;

    void                notifyBlocked( ObjectRef _polyRef, PolyLine* _poly, u32 _edgeIndex, const Vec2d& _speed, const Vec2d& _contactP );

    ////////////////////////////////////////////////////////////////////

    PhysContactsContainer           m_contacts;
    FixedArray <TravelEdgeData,10>  m_traveledEdgesThisFrame;   // We keep a history of the edges where we've been this frame
                                                                // When we move we don't want to consider those edges as obstacles
    SafeArray <EdgeMemoryData>      m_memoryStickedEdges;       // We keep a history of the edges where we sticked recently, we won't stick on them
    SafeArray <EdgeMemoryData>      m_memoryNoStickPlatforms;   // We keep a list of the platforms we don't want to land on

    ObjectRef               m_stickedLineRef;           // Sticked line
    u32                     m_stickedEdge;              // Sticked edge
    Vec2d                   m_currentGroundDir;         // Current ground dir. When changing from one edge to another we may be interpolating it. It's used to apply the forces
    Vec2d                   m_extraCollisionCheckPos;   // Position of the secondary circle for collision checks
    f32                     m_extraCollisionCheckRadius;// Radius of the secondary circle for collision checks

    // To see if we should unstick from our current edge we must check the angle between our
    // speed and the edge, taking into account also the size of the speed vector
    f32                     m_unstickMinFrictionMultiplier; // Minimum friction coefficient to be able to stick multiplier

    // Our angular speed will depend on our linear speed.
    // At MinLinear our angular speed will be MinAngular
    // At MaxLinear our angular speed will be MaxAngular
    f32                     m_angularSpeedMultiplier;   // Multiplier to the angular speed
    Angle                   m_groundAngularSpeedMultiplierMaxAngle;
    f32                     m_groundCurrentAngularSpeed;// The current ground angular speed

    // Speed loss when landing on an edge

    f32                     m_speedLossMultiplier;       // General speed loss multiplier
    Vec2d                   m_speedMultiplier;           // Speed Multiplier

    Vec2d                   m_windForcesThisFrame;
    Vec2d                   m_windSpeedMultiplier;

    f32                     m_relDist;                  // The relative distance on the current polyline
    f32                     m_insideEdgeFactor;         // If we are inside one edge, the percentage of how much inside we are
    f32                     m_betweenEdgesFactor;       // If we are between two edges, the percentage of how much inside
    f32                     m_relAngle;                 // The relative angle on the current polyline
    f32                     m_targetAngle;              // We always try to interpolate towards the target angle
    f32                     m_frictionMultiplier;       // Friction multiplier
    f32                     m_airFrictionMultiplier;    // Friction multiplier while in the air
    f32                     m_frictionProjection;       // The force we apply on the body projected on the normal of the edge
    f32                     m_currentFrictionCoef;      // Our current friction coefficient

    static const i32        m_solverMaxLoops = 1;

    // The average of the speed over some frames
    static const u32        s_speed_average_num_frames = 15;
    f32                     m_averageXSpeedMeasures[s_speed_average_num_frames];
    f32                     m_averageYSpeedMeasures[s_speed_average_num_frames];
    f32                     m_averageXSpeed;
    f32                     m_averageYSpeed;
    Vec2d                   m_prevPosition;
    Vec2d                   m_worldSpeed;

    // The average of the torque over some frames
    static const u32        s_torque_average_num_frames = 15;
    f32                     m_averageTorqueMeasures[s_torque_average_num_frames];
    f32                     m_averageTorque;
    f32                     m_prevAngle;
    f32                     m_prevGroundAngle;
    f32                     m_torqueRotationSign;

    bbool                   m_forceRotateWithGravity;
    bbool                   m_updateRotation;
    bbool                   m_debugLog;
    bbool                   m_processContacts;
    bbool                   m_rotateWithTorque;
    bbool                   m_forceEdgeAngleNext;
    bbool                   m_disableSticking;
    bbool                   m_hasExternalForces;
    bbool                   m_disableWindForces;

    // Ground / Wall / Roof
    bbool                   m_stickOnWalls;
    bbool                   m_stickOnWallsCheckConvex;
    bbool                   m_stickOnWallsShiftGravity;
    Vec2d                   m_stickOnWallsInputDir;

    // Max speed handling
    bbool                   m_maxSpeedEnabled;
    f32                     m_maxSpeedX;
    f32                     m_maxSpeedY;
    f32                     m_maxSpeedTotal;

    f32                     m_cornerCaseDisableDelay;

    ///////////////////////////////////////////////////////////
};

//---------------------------------------------------------------------------------------------------

class StickToPolylinePhysComponent_Template : public PhysComponent_Template
{
    DECLARE_OBJECT_CHILD_RTTI(StickToPolylinePhysComponent_Template,PhysComponent_Template,3898305065);
    DECLARE_SERIALIZE();
    DECLARE_ACTORCOMPONENT_TEMPLATE(StickToPolylinePhysComponent);

public:

    StickToPolylinePhysComponent_Template();
    ~StickToPolylinePhysComponent_Template() {}

    f32                     getFriction() const { return m_friction; }
    f32                     getAirFriction() const { return m_airFriction; }
    const Angle&            getUnstickMinAngle() const { return m_unstickMinAngle; }
    const Angle&            getUnstickMaxAngle() const { return m_unstickMaxAngle; }
    f32                     getUnstickMinAngleSpeed() const { return m_unstickMinAngleSpeed; }
    f32                     getUnstickMaxAngleSpeed() const { return m_unstickMaxAngleSpeed; }
    f32                     getUnstickMinFriction() const { return m_unstickMinFriction; }
    f32                     getUnstickMinForceExternal() const { return m_unstickMinForceExternal; }
    f32                     getAngularSpeedMinLinear() const { return m_angularSpeedMinLinear; }
    f32                     getAngularSpeedMaxLinear() const { return m_angularSpeedMaxLinear; }
    const Angle&            getAngularSpeedMinAngular() const { return m_angularSpeedMinAngular; }
    const Angle&            getAngularSpeedMaxAngular() const { return m_angularSpeedMaxAngular; }
    f32                     getAngularSpeedAirMultiplier() const { return m_angularSpeedAirMultiplier; }
    const Angle&            getGroundAngularSpeedMultiplierMinAngle() const { return m_groundAngularSpeedMultiplierMinAngle; }
    const Angle&            getGroundAngularSpeedMultiplierMaxAngle() const { return m_groundAngularSpeedMultiplierMaxAngle; }
    f32                     getGroundAngularSpeedMultiplierMinMultiplier() const { return m_groundAngularSpeedMultiplierMinMult; }
    f32                     getGroundAngularSpeedMultiplierMaxMultiplier() const { return m_groundAngularSpeedMultiplierMaxMult; }
    const Angle&            getTransferSpeedLossMinAngle() const { return m_transferSpeedLossMinAngle; }
    const Angle&            getTransferSpeedLossMaxAngle() const { return m_transferSpeedLossMaxAngle; }
    f32                     getTransferSpeedLossMinLoss() const { return m_transferSpeedLossMinLoss; }
    f32                     getTransferSpeedLossMaxLoss() const { return m_transferSpeedLossMaxLoss; }
    const Angle&            getSpeedLossLandEdgeMinAngle() const { return m_speedLossLandEdgeMinAngle; }
    const Angle&            getSpeedLossLandEdgeMaxAngle() const { return m_speedLossLandEdgeMaxAngle; }
    f32                     getSpeedLossLandMin() const { return m_speedLossLandMin; }
    f32                     getSpeedLossLandMax() const { return m_speedLossLandMax; }
    f32                     getTorqueRotationMultiplier() const { return m_torqueRotationMultiplier; }
    f32                     getMinSpeedStickToWall() const { return m_minSpeedStickToWall; }
    bbool                   getTreatCornerCase() const { return m_treatCornerCase; }
    f32                     getCornerCaseDisableDelay() const { return m_cornerCaseDisableDelay; }
    bbool                   getTreatMovingPolylines() const { return m_treatMovingPolylines; }

private:


    f32                     m_friction;                 // Our friction
    f32                     m_airFriction;              // Friction while in the air

    // To see if we should unstick from our current edge we must check the angle between our
    // speed and the edge, taking into account also the size of the speed vector
    Angle                   m_unstickMinAngle;          // Minimum angle between edge and speed to unstick
    Angle                   m_unstickMaxAngle;          // Maximum angle between edge and speed to unstick
    f32                     m_unstickMinAngleSpeed;     // Speed to unstick at minimum angle
    f32                     m_unstickMaxAngleSpeed;     // Speed to unstick at minimum angle
    f32                     m_unstickMinFriction;       // Minimum friction coefficient to be able to stick
    f32                     m_unstickMinForceExternal;  // Minimum force to unstick from the current edge

    // Our angular speed will depend on our linear speed.
    // At MinLinear our angular speed will be MinAngular
    // At MaxLinear our angular speed will be MaxAngular
    f32                     m_angularSpeedMinLinear;    // Minimum speed taken into account for angular speed calculation
    f32                     m_angularSpeedMaxLinear;    // Maximum speed taken into account for angular speed calculation
    Angle                   m_angularSpeedMinAngular;   // Angular speed at min linear speed
    Angle                   m_angularSpeedMaxAngular;   // Angular speed at max linear speed
    f32                     m_angularSpeedAirMultiplier;// Multiplier to angular speed when in air
    Angle                   m_groundAngularSpeedMultiplierMinAngle;
    Angle                   m_groundAngularSpeedMultiplierMaxAngle;
    f32                     m_groundAngularSpeedMultiplierMinMult;
    f32                     m_groundAngularSpeedMultiplierMaxMult;

    // Speed loss coefficient when moving from one edge (or no edge) to another
    Angle                   m_transferSpeedLossMinAngle; // Angle for min speed loss coefficient
    Angle                   m_transferSpeedLossMaxAngle; // Angle for max speed loss coefficient
    f32                     m_transferSpeedLossMinLoss;  // Loss at min angle
    f32                     m_transferSpeedLossMaxLoss;  // Loss at max angle

    // Speed loss when landing on an edge
    Angle                   m_speedLossLandEdgeMinAngle; // Min angle with gravity to lose speed when landing
    Angle                   m_speedLossLandEdgeMaxAngle; // Min angle with gravity to lose speed when landing
    f32                     m_speedLossLandMin;          // Minimum loss of speed when landing
    f32                     m_speedLossLandMax;          // Maximum loss of speed when landing

    f32                     m_torqueRotationMultiplier;  // A multiplier to the angular speed when we rotate with the torque

    // Ground / Wall / Roof
    f32                     m_minSpeedStickToWall;

    bbool                   m_treatCornerCase;
    f32                     m_cornerCaseDisableDelay;
    bbool                   m_treatMovingPolylines;
};



const StickToPolylinePhysComponent_Template*  StickToPolylinePhysComponent::getTemplate() const {return static_cast<const StickToPolylinePhysComponent_Template*>(m_template);}

f32              StickToPolylinePhysComponent::getAirFriction() const { return getTemplate()->getAirFriction(); }
f32              StickToPolylinePhysComponent::getUnstickMinFriction() const { return getTemplate()->getUnstickMinFriction() * getUnstickMinFrictionMultiplier(); }
f32              StickToPolylinePhysComponent::getUnstickMinForceExternal() const { return getTemplate()->getUnstickMinForceExternal() * getUnstickMinFrictionMultiplier(); }

}

#endif // _ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_

