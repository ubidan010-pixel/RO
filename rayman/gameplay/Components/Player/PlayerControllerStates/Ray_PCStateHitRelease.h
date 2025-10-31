#ifndef _ITF_RAY_PCSTATEHITRELEASE_H_
#define _ITF_RAY_PCSTATEHITRELEASE_H_

#ifndef _ITF_RAY_PCSTATEHITBASE_H_
#include "rayman/gameplay/Components/Player/PlayerControllerStates/Ray_PCStateHitBase.h"
#endif //_ITF_RAY_PCSTATEHITBASE_H_

class StateHitRelease : public StateHitBase
{
    DECLARE_OBJECT_CHILD_RTTI(StateHitRelease,StateHitBase,1800515954)
public:
    StateHitRelease();
    ~StateHitRelease() {}

    virtual void                onEnter();
    virtual void                onExit();
    virtual bbool               checkStateChange( f32 _dt );
    virtual void                update(f32 _dt);
    virtual void                onEvent(Event * _event);
    virtual void                updateAnimInputs();

    void                        setForceReducedHitUp( bbool _val ) { m_forceReducedHitUp = _val; }
    void                        setCrushAttackLimitDistance( f32 _val ) { m_crushDistanceLimit = _val; }

#ifdef ITF_SUPPORT_DEBUGFEATURE
    virtual void                drawDebug();
#endif

    virtual StringID getAnimAction() const
    {
        return ITF_GET_STRINGID_CRC(RELEASEHIT,3042988373);
    }

protected:

    struct HitQueueInfo
    {
        ObjectRef                   m_receiver;
        ObjectRef                   m_closestCollideable;
        ObjectRef                   m_closestPoly;
        u32                         m_closestEdgeIndex;
        Vec2d                       m_pos;
        Vec2d                       m_dir;
        f32                         m_dist;
        f32                         m_animDist;
        f32                         m_timer;
        bbool                       m_fail;
        bbool                       m_blocks;
        PhysContactsContainer       m_contacts;
        StringID                    m_contactMaterial;
    };

    struct MemorizedHitInfo
    {
        ObjectRef                   m_receiver;
        ObjectRef                   m_poly;
        ObjectRef                   m_collideable;
        u32                         m_edgeIndex;
        f32                         m_timer;
    };

    typedef FixedArray <HitQueueInfo,50> HitQueue;
    typedef FixedArray <MemorizedHitInfo,60> MemorizedQueue;

    virtual bbool                   canUpdateLookDir() const { return bfalse; }

    bbool                           checkChangeStateWantsMove() const;
    void                            changeToNextState();

    f32                             getRehitDelay() const;
    void                            updateCrouchStand();
    void                            updateMemorizedHits( f32 _dt );
    void                            updatePermanentHit();

    void                            setupCrushHit();
    void                            updateCrushHit( f32 _dt );

    void                            setupReducedSizeHit();

    void                            setupTornadoAttack();

    void                            updateDashHit( f32 _dt );
    bbool                           checkShouldExitDashHit( f32 _minSpeed ) const;

    void                            fillHitQueue( bbool _updateAnimValues, const Vec2d& _hitDir );
    void                            updateAnimValuesFromQueue();
    void                            calculateHitShapeFromEvent( AnimPolylineEvent* _animPolyEvent );
    void                            calculateHitShapeFromPolyLine( const Vec2d* _points, u32 _numPoints );
    void                            calculateHitQueue( const Vec2d& _hitDir, HitQueue& _queue );
    void                            releaseHitsFromQueue( PunchStim* _stim );
    void                            processPerformHit();
    void                            processPerformCrush();
    void                            processQueryHitType(Ray_EventQueryHitType* _hitTypeQuery);

    PunchStim*                      setupPunchStim() const;
    void                            sendClimbImpulse( const Vec2d& _dir ) const;
    void                            setReceiveHitTypeOnStim( PunchStim* _stim ) const;
    virtual Vec2d                   getBaseHitDir() const;

    void                            hitQueueApplyFilter( const PhysContactsContainer& _allContacts,
                                                         const Vec2d& _hitDir,
                                                         PhysContactsContainer& _filteredContacts, u32& _collisionIndex );

    void                            checkSwitchHit();

    void                            processCrushHitGround( ObjectRef _poly, u32 _edgeIndex );

    void                            shakeCamera(u32 _level);

    HitQueue                        m_hitQueue;
    MemorizedQueue                  m_memorizedHits;
    PhysShapePolygon::PointsList    m_shapePoints;
    PhysShapePolygon                m_shape;
    f32                             m_switchHitTimer;       // We allow a small time to transform the hit into a uturn hit
    f32                             m_crushTravelTime;      // The time we've been performing the crush attack
    f32                             m_crushPrevSpeedY;      // The crush attack previous speed
    Vec2d                           m_crushPrevPos;         // Previous position on crush attack
    Vec2d                           m_crushStartPos;        // The starting position of the crush attack
    f32                             m_crushStuckCounter;    // How long we've been stuck
    f32                             m_crushDistanceLimit;   // Limit distance to travel with the crush attack
    u32                             m_crushWasBlockedCounter;// Consecutive times the crush was blocked
    bbool                           m_crushWasBlocked;      // If the crush attack was blocked
    EMoveDir                        m_reducedHitDir;        // Direction of the reduced size hit
    f32                             m_dashCounter;          // Counter to finish the reduced size hit
    f32                             m_dashTotal;            // Total time of the dash
    EMoveDir                        m_dashAttackMoveDir;    // The direction we input when we did the dash attack
    Vec2d                           m_dashSpeedLastFrame;   // Last frame speed while dashing
    bbool                           m_dashHadHitLastFrame;  // If we hit something on the last frame
    bbool                           m_preparingHit;         // If we prepare the hit over time
    bbool                           m_performingHit;        // If we perform the hit over time
    bbool                           m_skipFirstReleaseHit;  // If this is the first time we are in perform hit
    bbool                           m_justEntered;          // If we just entered the state
    bbool                           m_canMove;              // This flag is true if the anim marker that tell us that we can move again is reached
    bbool                           m_exitSoon;             // True if we want to exit as soon as possible
    bbool                           m_permanentHit;         // If we hit during the whole animation
    bbool                           m_shakeCamReleased;     // If the shake cam was already released
    bbool                           m_hitsReleased;         // If the hits were release already
    bbool                           m_hitSomethingThisRound;// If we hit something this round
    bbool                           m_hitSomethingThisFrame;// If we hit something this frame
    bbool                           m_crushReleased;        // If the crush hit was released
    bbool                           m_crushWasOnGround;     // If the crush attack already hit the ground
    bbool                           m_resetFrictionMultiplier; // If we need to reset the friction multiplier
    bbool                           m_inverseHitDir;        // If we have to inverse the hit direction
    bbool                           m_forceReducedHitUp;    // If we force a reduced hit up

#ifdef ITF_SUPPORT_DEBUGFEATURE
    void                            addDbgShape();
    FixedArray <PhysShapePolygon,5> m_dbgDraws;
#else
    void                            addDbgShape()           {}
#endif //ITF_SUPPORT_DEBUGFEATURE
};

#endif //_ITF_RAY_PCSTATEHITRELEASE_H_


