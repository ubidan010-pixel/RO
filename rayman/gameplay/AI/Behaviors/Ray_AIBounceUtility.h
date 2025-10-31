#ifndef _ITF_RAY_AIBOUNCEUTILITY_H_
#define _ITF_RAY_AIBOUNCEUTILITY_H_

namespace ITF
{
    class AIComponent;
    class Actor;
    class PhysComponent;
    class AnimLightComponent;

    class Ray_AIBounceUtility
    {
    public:
        Ray_AIBounceUtility() 
            : m_bounceUtility_groundBounceSpeedFactor(0,0)
            , m_bounceForceAmplitude(-1)
            , m_maxBounceOnlyForce_Angle(MTH_PI)
            , m_smallAngleForBounceOnlyForce(0)
            , m_hasStateForWallBounce(bfalse)
			, m_bounceOnWall_onlyKeepNormalPartOfSpeed(bfalse)
            , m_groundToVerticalBlendFactor(0)
            , m_bouncingOnWall_isFromCharacter(bfalse)
        {bounceUtility_reset();}

        ///////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////
        ////High level functions

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Set if has state for wall bounce
        void bounceUtility_setHasStateForWallBounce(bbool _hasState) {m_hasStateForWallBounce = _hasState;}

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Set blend-to-vertical factor
        void bounceUtility_setGroundToVerticalBlendFactor(f32 _groundToVerticalBlend) {m_groundToVerticalBlendFactor = _groundToVerticalBlend;}

		///////////////////////////////////////////////////////////////////////////////////////////
		/// Set if only keep normal part of speed when bouncing on wall
		void bounceUtility_onlyKeepNormalPartOfSpeedWhenBouncingOnWall(bbool _onlyKeepNormalPart) { m_bounceOnWall_onlyKeepNormalPartOfSpeed = _onlyKeepNormalPart;}

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Set bounce force amplitude, if you use bounce force (only the orthogonal part)
        void bounceUtility_setBounceForceAmplitude(f32 _bounceForceAmplitude) {m_bounceForceAmplitude = _bounceForceAmplitude;}

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Set maximum angle of bounce only force from vertical. Default is PI
        void bounceUtility_setBounceForceMaxAngleFromVertical(f32 _angle) {m_maxBounceOnlyForce_Angle = _angle;}

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Set small angle from vertical. If ground normal is smaller, it will be considered as 0. Default is 0.
        void bounceUtility_setSmallAngleForBounceOnlyForce(f32 _angle) {m_smallAngleForBounceOnlyForce = _angle;}

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// reset bounce system
        void bounceUtility_reset();

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// set speed factor which is applied when bouncing.
        void bounceUtility_setGroundBounceSpeedFactor(const Vec2d &_factor) {m_bounceUtility_groundBounceSpeedFactor = _factor;}

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// bounceUtility_registerEvents
        /// @param _actor the actor
        /// @param _aiComponent the aiComponent
        void bounceUtility_registerEvents(Actor *_actor, AIComponent *_aiComponent);

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Call this when you enter jump/fall state
        /// @param _jumpingAnim : animation for jumping
        /// @param _physComponent
        /// @param _animComponent
        virtual void bounceUtility_EnterJumpOrFall(const StringID &_jumpingAnim, StickToPolylinePhysComponent *_physComponent, AnimLightComponent *_animComponent);

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// call that to handle your jump/fall state
        /// @param _physComponent
        /// @param _jumpAnim : ID of anim for jumping
        /// @param _fallAnim : ID of anim for falling
        /// @param _speedThatTriggersFloating : when below this Y speed,you start "floating" at apex
        /// @param _lastSpeed : speed at end of next update
        void bounceUtility_UpdateJumpOrFall(StickToPolylinePhysComponent *_physComponent,
            const StringID &_jumpAnim,
            const StringID &_fallAnim,
            f32 _speedThatTriggersFloating,
            const Vec2d &_lastSpeed);

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// clear current bounce. Call this at end of your update(), when everything was processed
        void bounceUtility_clearCurrentBounceCollision() {bounceUtility_reset();}

        ///////////////////////////////////////////////////////////////////////////////////////////
        ///  register events.
        void bounceUtility_registerEvents();




        ///////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////
        //// LOW LEVEL FUNCTIONS

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Call this when your object is supposed to be able to bounce (with inverted X speed) on a wall. It doesn't need to actually bounce.
        /// @param _physComponent
        /// @return btrue if just bounced
        bbool bounceUtility_updateBounceOnWall(StickToPolylinePhysComponent *_physComponent);

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Call this to bounce if on ground edge (either sticked or blocked)
        /// @param jumpingAnim : animation for jumping
        /// @param _physComponent
        /// @param _animComponent
        void bounceUtility_bounceIfOnGroundEdge(const StringID &jumpingAnim, StickToPolylinePhysComponent *_physComponent, AnimLightComponent *_animComponent);

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// check if we are on ground edge
        /// @param _physComponent
        /// @param[out] _outEdge the output normalized edge, if returns btrue
        bbool bounceUtility_isOnGroundEdge(StickToPolylinePhysComponent *_physComponent, Vec2d &_outEdge, bbool &_isFromActor);

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// get orthogonal bounce force from edge direction
        /// @param _normalizedEdgeDir : NORMALIZED edge direction
        /// @param _factor : multiplier
        Vec2d bounceUtility_getBounceOnlyForce(const Vec2d& _normalizedEdgeDir, f32 _factor ) const;

        ///////////////////////////////////////////////////////////////////////////////////////////
        Vec2d bounceUtility_mayBlendEdgeDirWhenGround(StickToPolylinePhysComponent *_physComponent, const Vec2d &_normalizedEdgeDir);

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// bounce on edge. This changes current animation and adds calculated force
        /// @param animationID : the animation to set
        /// @param physComponent
        /// @param animComponent
        /// @param normalizedEdgeDir : the edge direction (normalized)
        void bounceUtility_bounceOnEdge(const StringID &animationID,
            StickToPolylinePhysComponent *physComponent,
            AnimLightComponent *animComponent,
            const Vec2d &normalizedEdgeDir);

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// call this from your onEvent methode
        /// @param _event
        /// @param _physComponent
        void bounceUtility_processEvent(Event *_event, StickToPolylinePhysComponent *_physComponent);     

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Compute force to reach a given position. Call this continuously until you reach it.
        /// @param _mass : Mass of solid
        /// @param _speed : current speed
        /// @param _currentPos : current position
        /// @param _goal : desired goal
        /// @param _forceDuration : computed force will be roughly constant for this duration
        /// @param _maxForce : force limitation
        Vec2d bounceUtility_getForceToReach(f32 _mass, const Vec2d &_speed, const Vec2d &_currentPos, const Vec2d &_goal, const f32 _forceDuration, f32 _maxForce);


        ///////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////////////
        //// FUNCTIONS YOU NEED TO OVERRIDE

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// override this function to get bounce force, taking everything (ennemy pos...) into account.
        /// Call bounceOnlyForce in the override.
        /// @param _normalizedEdgeDir : NORMALIZED edge direction
        virtual Vec2d bounceUtility_getBounceForce(Vec2d _normalizedEdgeDir) = 0;

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// this pure virtual function is called by bounceUtility_UpdateJumpOrFall when on ground
        /// @param _normalizedEdgeDir : NORMALIZED edge direction
        virtual void bounceUtility_UpdateJumpOrFall_onIsOnGround(const Vec2d &_normalizedEdgeDir, bbool isFromActor) = 0;
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// this pure virtual function is called by bounceUtility_UpdateJumpOrFall when at top of bounce
        virtual void bounceUtility_UpdateJumpOrFall_onNearTopOfJump() = 0;

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// callback fired when we bounce on wall
        virtual void bounceUtility_UpdateBounceOnWall_onBouncingOnWall(const Vec2d &_wallNormal, const Vec2d &_newSpeed) {}

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Override this function to set your own current animation
        /// @param _newAnim
        virtual void setCurrentAnim(const StringID &_newAnim) = 0;

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// get the current animation. Override this function.
        virtual const StringID &getCurrentAnim() const = 0;

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Overload this function to have your own bounce system. This one uses forces
        /// @param _physComponent
        /// @param _normalizedEdgeDir
        virtual void bounceUtility_bounceOnEdge_applyBouncePhysics(StickToPolylinePhysComponent *_physComponent, const Vec2d &_normalizedEdgeDir);

    protected:
        bbool m_bouncingOnWall;
        Vec2d m_bouncingOnWall_NextSpeed;
        Vec2d m_bouncingOnWall_wallNormal;
        bbool m_bouncingOnWall_isFromCharacter;
        Vec2d m_lastBlockingEdge;
        Vec2d m_bounceUtility_groundBounceSpeedFactor;
        f32 m_bounceForceAmplitude;
        f32 m_maxBounceOnlyForce_Angle;
        f32 m_smallAngleForBounceOnlyForce;
        bbool m_hasStateForWallBounce;
		bbool m_bounceOnWall_onlyKeepNormalPartOfSpeed;
        f32 m_groundToVerticalBlendFactor;
    };
}


#endif
