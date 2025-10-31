#ifndef __ITF_RAY_PICKEDLUMTRAJECTORYFOLLOWER_H__
#define __ITF_RAY_PICKEDLUMTRAJECTORYFOLLOWER_H__

namespace ITF
{
    class Ray_PickedLumTrajectoryFollower
    {
    public:
        struct CircularMotionDesc
        {
            CircularMotionDesc() 
                : m_circularMotionDuration(0.f), m_rotationCenter(0,0,0), m_circularMotionSpeed(1), m_radius(1), m_maxRank(1), m_rank(1) {}
            Vec3d m_rotationCenter;
            f32 m_circularMotionDuration;
            f32 m_circularMotionSpeed;
            f32 m_radius;
            u32 m_maxRank, m_rank;
        };

        //////////////////////////////////////////////////////////////////////////
        Ray_PickedLumTrajectoryFollower() 
            : m_initialTakenPos(0,0,0)
            , m_playerNextPos(0,0,0)
            , m_flightDirection2D(0,0)
            , m_flightStart2D(0,0,0)
            , m_flightGoal2D(0,0)
            , m_timeSinceTaken(0)
            , m_playerPos_WhenPicked(0,0,0) 
            , m_percentTimeStartFading(0.9f)
            , m_scoreValue(0)
            , m_playerIndex(0)
            , m_alphaThreshold(0.f)
            , m_clientProcessedReachedAlphaThreshold(bfalse)
            , m_state(State_FlyingToScore)
            , m_timeInCurrentState(0)
            , m_flightDelay(0)
            , m_usePlayerSpeed(bfalse)
            , m_scaleWhenReachedScore(1.f)
            , m_percentTimeStartScaling(0.5f)
            , m_hasAlreadySentReachScoreLum(bfalse)
        {}

        void initializeParameters(const Vec2d &_destinationScorePos, const ActorRef &_actorWhoPicked, f32 _takenTrajectoryFactorX, f32 _takenTrajectoryFactorY, bbool _usePlayerSpeed, const Vec3d &_playerPosWhenPicked, f32 _percentTimeStartFading, const AABB &_particleAABB, u32 _scoreValue, u32 _playerIndex, f32 _alphaThreshold, CircularMotionDesc *_circularMotion, const Vec3d &_currentPos, f32 _scaleWhenReachedScore, f32 _percentTimeStartScaling);
        void computeFlightTrajectory(bbool _initialize, const Vec3d &_currentPos);
        f32 computePosOnTrajectory(Vec3d &_projPos, Vec3d &_projSpeed, bbool &_isWaitingBeforeStart, f32 _delay, f32 &_alpha, bbool &_reachedAlphaThreshold, f32 &_scale);
        void incrementTime(f32 _dt) {m_timeSinceTaken += _dt;m_timeInCurrentState += _dt;}
        f32 getTimeSinceTaken() const {return m_timeSinceTaken;}
        const AABB &getRelativeParticleAABB() const {return m_relativeParticleAABB;}
        u32 getScoreValue() const {return m_scoreValue;}
        u32 getPlayerIndex() const {return m_playerIndex;}
        void setClientProcessedReachedAlphaThreshold() {m_clientProcessedReachedAlphaThreshold = btrue;}

        void setCircularMotionDescRank(u32 _i) {m_circularMotionDesc.m_rank = _i;}
        void setCircularMotionDescDuration(f32 _duration) {m_circularMotionDesc.m_circularMotionDuration = _duration;}
        f32 getCircularMotionRadius() const {return m_circularMotionDesc.m_radius;}
        void setCircularMotionRadius(f32 _radius) {m_circularMotionDesc.m_radius = _radius;}
        void setFlightDelay(f32 _delay) {m_flightDelay = _delay;}
        bbool getHasAlreadySentReachScoreLum() const {return m_hasAlreadySentReachScoreLum;}
        void setHasAlreadySentReachScoreLum(bbool _sent) {m_hasAlreadySentReachScoreLum = _sent;}

    private:
        enum State_t {State_FlyingToScore, State_CircularMovement};

        ITF_INLINE f32 computeInterpolator(f32 _lambda) {return _lambda*_lambda;}
        void setState(State_t _state);

        //given at either initializeParameters  or computeFlightTrajectory (or other property setting)
        Vec2d       m_destinationScorePos; //duplicated
        ActorRef    m_actorWhoPicked; //duplicated
        Vec3d       m_takenTrajectoryFactorXY;
        bbool       m_usePlayerSpeed; //duplicated
        Vec3d       m_playerPos_WhenPicked;
        f32         m_percentTimeStartFading;
        u32         m_scoreValue,m_playerIndex;
        f32         m_alphaThreshold;
        CircularMotionDesc m_circularMotionDesc;
        State_t     m_state;
        f32         m_timeInCurrentState;
        f32         m_flightDelay;
        f32         m_scaleWhenReachedScore;
        f32         m_percentTimeStartScaling;

        //Computed by computeFlightTrajectory
        Vec3d       m_initialTakenPos, m_playerNextPos;
        Vec2d       m_flightDirection2D;
        Vec3d       m_flightStart2D;
        Vec2d       m_flightGoal2D;
        f32         m_timeSinceTaken;

        AABB        m_relativeParticleAABB;

        //computed by computePosOnTrajectory
        u32         m_clientProcessedReachedAlphaThreshold;

        //
        bbool       m_hasAlreadySentReachScoreLum;

    };

}


#endif
