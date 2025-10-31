#ifndef _ITF_RAY_AIGROUNDROAMBEHAVIOR_H_
#define _ITF_RAY_AIGROUNDROAMBEHAVIOR_H_

#ifndef _ITF_RAY_AIGROUNDBASEMOVEMENTBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ground/Ray_AIGroundBaseMovementBehavior.h"
#endif //_ITF_RAY_AIGROUNDBASEMOVEMENTBEHAVIOR_H_

namespace ITF
{
    class AIPlayAnimAction;
    class AIFallAction;
    class AIJumpAction;

    class Ray_AIGroundRoamBehavior : public Ray_AIGroundBaseMovementBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIGroundRoamBehavior, Ray_AIGroundBaseMovementBehavior,2675782030);
        DECLARE_VALIDATE_BEHAVIOR()

    public:

        Ray_AIGroundRoamBehavior();
        virtual ~Ray_AIGroundRoamBehavior();

        virtual void                        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void                        onActivate();
        virtual void                        update( f32 _delta  );
        virtual void                        onEvent( Event* _event );
        virtual void                        onActionFinished();
        virtual void                        updateAnimInputs();

        void                                setTryReachPosition( bbool _val ) { m_tryReachPosition = _val; }

    protected:
        ITF_INLINE const class Ray_AIGroundRoamBehavior_Template * getTemplate() const;


        AIAction*                           m_fall;
        AIJumpAction*                       m_jumpUp;
        AIJumpAction*                       m_jumpDown;
        AIAction*                           m_roamPause;        // silly pause animation after walking for too long (keep same dir afterwards)
        AIAction*                           m_slopePause;       // silly pause animation when the slope is too high (u-turn afterwards)
        AIAction*                           m_obstaclePause;    // silly pause animation when reaching an obstacle (u-turn afterwards)
        AIAction*                           m_wallPause;        // silly pause animation when hitting a wall (u-turn afterwards)
        AIAction*                           m_defaultPause;     // default silly pause animation (optional, immediate u-turn if null)

        virtual void                        updateIdle( f32 _dt );
        virtual bbool                       updateMove( f32 _dt );

        virtual void                        startIdle();
        void                                startMove( bbool _walkLeft );
        void                                startFall();
        void                                startJumpUp( const PolyLineEdge* _wallEdge, f32 _wallHeight );
        void                                startJumpDown( const PolyLineEdge* _wallEdge, f32 _wallHeight );
        void                                startHitWall( const PolyLineEdge* _wallEdge, f32 _wallHeight );
        virtual void                        startAnimPause( AIAction* _pause, bbool _uturnAfterwards );

        void                                onActionFinishedMove();
        virtual void                        onActionFinishedUturn();
        void                                onActionFinishedJumpUp();
        void                                onActionFinishedJumpDown();
        void                                onActionFinishedFall();
        void                                onActionFinishedPause();
        void                                onActionFinishedHitWall();
        void                                restoreAfterHitWall();

        void                                processEventBlockedByPolyline( EventBlockedByPolyline* _blockedEvent );
        bbool                               checkBlockingObstacle();
        bbool                               checkBlockingPolyline( PolyLine* _poly, const PolyLineEdge* _edge, u32 _edgeIndex, const Vec2d& _contactPoint );
        bbool                               checkStuck( f32 _dt );
        bbool                               checkSlope( PolyLine* poly );
        bbool                               checkWall( PolyLine* poly );

        bbool                               checkHole( PolyLine* poly );
        bbool                               checkWaypoint();
        bbool                               isValidSlope( f32 _slopeAngle ) const;
#ifdef ITF_SUPPORT_DEBUGFEATURE
        void                                debugContacts();
#endif // ITF_SUPPORT_DEBUGFEATURE

        f32                                 m_startTimer;
        bbool                               m_uturnAfterPause;

        // we came in contact with a blocking polyline
        const PolyLine*                     m_blockingPoly;
        const PolyLineEdge*                 m_blockingEdge;

        bbool                               m_pushing;

        f32                                 m_wallHeight;           // anim input for the height of the wall we just hit (different anims for low walls)
        Vec2d                               m_posBeforeHitWall;
        f32                                 m_angleBeforeHitWall;
        Vec2d                               m_targetDirection;
        bbool                               m_tryReachPosition;     // tells the behavior to always keep going forward even if there are holes, etc
    };

    class Ray_AIGroundRoamBehavior_Template : public Ray_AIGroundBaseMovementBehavior_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIGroundRoamBehavior_Template,Ray_AIGroundBaseMovementBehavior_Template,3869081513);
        DECLARE_SERIALIZE();
        DECLARE_AIBEHAVIOR_TEMPLATE(Ray_AIGroundRoamBehavior);

    public:

        Ray_AIGroundRoamBehavior_Template();
        ~Ray_AIGroundRoamBehavior_Template();

        ITF_INLINE const class AIAction_Template* getFall() const { return m_fall; }
        ITF_INLINE const class AIJumpAction_Template* getJumpUp() const { return m_jumpUp; }
        ITF_INLINE const class AIJumpAction_Template* getJumpDown() const { return m_jumpDown; }
        ITF_INLINE const class AIAction_Template* getRoamPause() const { return m_roamPause; }
        ITF_INLINE const class AIAction_Template* getSlopePause() const { return m_slopePause; }
        ITF_INLINE const class AIAction_Template* getObstaclePause() const { return m_obstaclePause; }
        ITF_INLINE const class AIAction_Template* getWallPause() const { return m_wallPause; }
        ITF_INLINE const class AIAction_Template* getDefaultPause() const { return m_defaultPause; }

        ITF_INLINE f32 getSlopeDetectionRange() const { return m_slopeDetectionRange; }
        ITF_INLINE Angle getMaxSlopeAngleUp() const { return m_maxSlopeAngleUp; }
        ITF_INLINE Angle getMaxSlopeAngleDown() const { return m_maxSlopeAngleDown; }
        ITF_INLINE StringID getWaypointID() const { return m_waypointID; }
        ITF_INLINE f32 getStartDelay() const { return m_startDelay; }
        ITF_INLINE bbool getCanPush() const { return m_canPush; }
        ITF_INLINE f32 getPushForce() const { return m_pushForce; }
        ITF_INLINE f32 getLowWallHeight() const { return m_lowWallHeight; }
#ifdef ITF_SUPPORT_DEBUGFEATURE
        ITF_INLINE bbool getDrawDebug() const { return m_drawDebug; }
#endif // ITF_SUPPORT_DEBUGFEATURE

    private:
        class AIAction_Template*                           m_fall;
        class AIJumpAction_Template*                       m_jumpUp;
        class AIJumpAction_Template*                       m_jumpDown;
        class AIAction_Template*                           m_roamPause;        // silly pause animation after walking for too long (keep same dir afterwards)
        class AIAction_Template*                           m_slopePause;       // silly pause animation when the slope is too high (u-turn afterwards)
        class AIAction_Template*                           m_obstaclePause;    // silly pause animation when reaching an obstacle (u-turn afterwards)
        class AIAction_Template*                           m_wallPause;        // silly pause animation when hitting a wall (u-turn afterwards)
        class AIAction_Template*                           m_defaultPause;     // default silly pause animation (optional, immediate u-turn if null)

        f32                                 m_slopeDetectionRange;
        Angle                               m_maxSlopeAngleUp;
        Angle                               m_maxSlopeAngleDown;
        StringID                            m_waypointID;
        f32                                 m_startDelay;
        bbool                               m_canPush;
        f32                                 m_pushForce;
        f32                                 m_lowWallHeight;        // if the wall is lower than this, we snap to its top
#ifdef ITF_SUPPORT_DEBUGFEATURE
        bbool                               m_drawDebug;
#endif // ITF_SUPPORT_DEBUGFEATURE
    };

    ITF_INLINE const class Ray_AIGroundRoamBehavior_Template * Ray_AIGroundRoamBehavior::getTemplate() const
    {
        return static_cast<const class Ray_AIGroundRoamBehavior_Template *>(m_template);
    }

}

#endif //_ITF_RAY_AIGROUNDROAMBEHAVIOR_H_