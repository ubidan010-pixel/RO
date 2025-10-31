#include "precompiled_gameplay_rayman.h"

#ifndef __ITF_RAY_PICKEDLUMTRAJECTORYFOLLOWER_H__
#include "rayman/gameplay/ai/Behaviors/Lums/Ray_PickedLumTrajectoryFollower.h"
#endif //__ITF_RAY_PICKEDLUMTRAJECTORYFOLLOWER_H__

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_


#define LUMS2_FLIGHT_TIME               0.75f


namespace ITF
{
    ///////////////////////////////////////////////////////////////////////////////////////////
    f32 Ray_PickedLumTrajectoryFollower::computePosOnTrajectory(Vec3d &_projPos, Vec3d &_projSpeed, bbool &_isWaitingBeforeStart, f32 _delay, f32 &_alpha, bbool &_reachedAlphaThreshold, f32 &_scale)
    {
        _alpha = 1;
        _isWaitingBeforeStart = bfalse;

        _scale = 1;

        switch(m_state)
        {
        case State_CircularMovement:
            {
                f32 ejectionTime = m_timeInCurrentState;
                bbool finished = ejectionTime>=m_circularMotionDesc.m_circularMotionDuration;
                _reachedAlphaThreshold = bfalse;
                if (finished)
                {
                    ejectionTime = m_circularMotionDesc.m_circularMotionDuration;
                }


                Vec2d cosSin;
                f32 angleOffset = -(m_circularMotionDesc.m_rank*MTH_2PI)/m_circularMotionDesc.m_maxRank;
                Vec2d::CosSinOpt(&cosSin, angleOffset+ejectionTime*m_circularMotionDesc.m_circularMotionSpeed);
                cosSin *= m_circularMotionDesc.m_radius;
                
                Vec3d new3DPos = m_circularMotionDesc.m_rotationCenter + cosSin.to3d(0);
                GFX_ADAPTER->compute3DTo2D(new3DPos, _projPos);
                _projSpeed.clear();
                if (finished)
                {
                    setState(State_FlyingToScore);
                    computeFlightTrajectory(btrue, new3DPos);                    
                }
                return 0;

            } break;
        case State_FlyingToScore:
            {
                f32 parametricTime = m_timeInCurrentState - _delay - m_flightDelay;

                f32 lambda = parametricTime/LUMS2_FLIGHT_TIME;

                lambda = f32_Clamp(lambda, 0,1);

                f32 nextLambda = lambda + 0.1f;

                Vec2d line1Pos, line1PosStart, line2Pos, line2PosStart;
                Vec2d line1NextPos, line2NextPos;
                Vec2d pos2d, nextpos2d;

                m_flightStart2D.truncateTo2D(line1PosStart);

                //We line1 (colinear to player speed) with line2 (start to goal)
                line1Pos = line1PosStart+lambda*m_flightDirection2D*2;                
                line1NextPos = line1PosStart+nextLambda*m_flightDirection2D*2;                
                line2PosStart = line1PosStart+m_flightDirection2D;
                line2Pos = line2PosStart + (m_flightGoal2D-line2PosStart)*lambda;
                line2NextPos = line2PosStart + (m_flightGoal2D-line2PosStart)*nextLambda;

                f32 interp = computeInterpolator(lambda);
                f32 nextInterp = computeInterpolator(lambda);

                pos2d = line1Pos*(1-interp)+line2Pos*interp;
                nextpos2d = line1NextPos*(1-nextInterp)+line2NextPos*nextInterp;

                _projPos.setFromVec2d(pos2d, &m_flightStart2D.m_z);
                _projSpeed.m_x = nextpos2d.m_x-pos2d.m_x;
                _projSpeed.m_y = nextpos2d.m_y-pos2d.m_y;
                _projSpeed.m_z = 0;

                if (lambda>m_percentTimeStartFading)
                {
                    _alpha = (1.f-lambda)/(1-m_percentTimeStartFading);
                }

                bbool isBelowAlphaThreshold = _alpha<m_alphaThreshold;
                _reachedAlphaThreshold = isBelowAlphaThreshold && !m_clientProcessedReachedAlphaThreshold;

                f32 scaleLambda = lambda -m_percentTimeStartScaling;
                if (m_percentTimeStartFading<1)
                    scaleLambda = scaleLambda/(1-m_percentTimeStartScaling);
                else
                    scaleLambda = 0;
                _scale = f32_Lerp(1,m_scaleWhenReachedScore, f32_Clamp(scaleLambda,0,1));
                return lambda;
            } break;
        }

        ITF_ASSERT_CRASH(bfalse, "No state");
        return 0;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_PickedLumTrajectoryFollower::initializeParameters(const Vec2d &_destinationScorePos, const ActorRef &_actorWhoPicked, f32 _takenTrajectoryFactorX, f32 _takenTrajectoryFactorY, bbool _usePlayerSpeed, const Vec3d &_playerPosWhenPicked, f32 _percentTimeStartFading, const AABB &_particleAABB, u32 _scoreValue, u32 _playerIndex, f32 _alphaThreshold, CircularMotionDesc *_circularMotion, const Vec3d &_currentPos, f32 _scaleWhenReachedScore, f32 _percentTimeStartScaling)
    {
        m_destinationScorePos = _destinationScorePos;
        m_actorWhoPicked = _actorWhoPicked;
        m_takenTrajectoryFactorXY.set(_takenTrajectoryFactorX, _takenTrajectoryFactorY,0);
        m_usePlayerSpeed = _usePlayerSpeed;
        m_timeSinceTaken = 0;
        m_playerPos_WhenPicked = _playerPosWhenPicked;
        m_percentTimeStartFading = _percentTimeStartFading;
        m_relativeParticleAABB = _particleAABB;
        m_playerIndex = _playerIndex;
        m_scoreValue = _scoreValue;
        m_alphaThreshold = _alphaThreshold;
        m_clientProcessedReachedAlphaThreshold = bfalse;
        m_initialTakenPos = _currentPos;
        m_scaleWhenReachedScore = _scaleWhenReachedScore;
        m_percentTimeStartFading = _percentTimeStartScaling;
        m_hasAlreadySentReachScoreLum = bfalse;

        setCircularMotionDescRank(0);
        setCircularMotionDescDuration(0);
        setFlightDelay(0);

        if (_circularMotion)
        {
            m_circularMotionDesc = *_circularMotion;
            setState(State_CircularMovement);
        }
        else
        {
            setState(State_FlyingToScore);
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Ray_PickedLumTrajectoryFollower::setState(State_t _state)
    {
        m_state = _state;
        m_timeInCurrentState = 0;
    }


    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_PickedLumTrajectoryFollower::computeFlightTrajectory(bbool _initialize, const Vec3d &_currentPos)
    {
        Vec3d lumPos2D;
        Vec3d playerNextPos;
        Vec3d playerNextPos2d;
        Vec2d scorePos(0,0);

        if (_initialize)
        {
            m_initialTakenPos = _currentPos;
            m_timeSinceTaken = 0;
            m_timeInCurrentState = 0;
            m_clientProcessedReachedAlphaThreshold = bfalse;
        }
        GFX_ADAPTER->compute3DTo2D(m_initialTakenPos, lumPos2D);
        Actor *player = m_actorWhoPicked.getActor();
        if (_initialize)
        {
            if (player)        
                m_playerNextPos = player->getPos();
            else
                m_playerNextPos = m_playerPos_WhenPicked;
        }

        scorePos = m_destinationScorePos;
        if (player)
        {
            StickToPolylinePhysComponent *physComponent = player->GetComponent<StickToPolylinePhysComponent>();
            if (physComponent)
            {
                Vec2d speed = physComponent->getSpeed();
                if (_initialize)
                {
                    Vec3d anticipation(speed.m_x,speed.m_y,0.f);
                    anticipation *= m_takenTrajectoryFactorXY;
                    m_playerNextPos = player->getPos()+anticipation;
                }
                //AIUtils::drawDBGCircle(playerNextPos, 0.1f, Color::white(), "N",1);
                //AIUtils::drawDBGCircle(player->getPos(), 0.1f, Color::red(), "R",1);
                ITF_ASSERT(GAMEMANAGER->getPlayerFromActor(player->getRef()));
            }
        }

        GFX_ADAPTER->compute3DTo2D(m_playerNextPos, playerNextPos2d);

        //compute flight direction, the direction which the lum goes before actually going to score. Now, it goes in player's speed direction
        if (m_usePlayerSpeed)
            m_flightDirection2D = playerNextPos2d.truncateTo2D()-lumPos2D.truncateTo2D();
        else
        {
            m_flightDirection2D = scorePos-lumPos2D.truncateTo2D();
            m_flightDirection2D *= 0.2f;

        }
        f32 sqDist = m_flightDirection2D.sqrnorm();
        if (sqDist<1.f)
        {
            f32 screenPercent = 0.02f;
            m_flightDirection2D = Vec2d(GFX_ADAPTER->getScreenWidth()*screenPercent, GFX_ADAPTER->getScreenHeight()*screenPercent);
        }
        //initialize start/end pos
        m_flightStart2D = lumPos2D;
        m_flightGoal2D = scorePos;

    }

}
