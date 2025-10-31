#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_
#include "rayman/gameplay/Components/Player/Ray_PlayerControllerComponent.h"
#endif //_ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_RAY_POWERUPNAMES_H_
#include "rayman/gameplay/Ray_PowerUpNames.h"
#endif //_ITF_RAY_POWERUPNAMES_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

namespace ITF
{
///////////////////////////////////////////////////////////////////////

IMPLEMENT_OBJECT_RTTI_SUBCLASS(Ray_PlayerControllerComponent,StateStargate)

Ray_PlayerControllerComponent::StateStargate::StateStargate()
: m_currentStage(STARGATESTAGE_SUCKIN)
, m_nextStage(STARGATESTAGE_SUCKIN)
, m_currentTimer(0.f)
, m_totalTime(0.f)
, m_suckedInPoint(Vec3d::Zero)
, m_stargateStart(Vec3d::Zero)
, m_speed(Vec3d::Zero)
, m_rotateStart(0.f)
, m_rotateTarget(0.f)
, m_rotateCounter(0.f)
, m_rotateTime(0.f)
, m_decelerateTime(0.f)
, m_decelerateCounter(0.f)
, m_justEntered(bfalse)
, m_lastTrip(bfalse)
{
    m_noiseGenX.SetFrequency(0.2f);
    m_noiseGenX.SetPersistence(0.5f);
    m_noiseGenX.SetNbOctave(1);

    m_noiseGenY.SetFrequency(0.2f);
    m_noiseGenY.SetPersistence(0.5f);
    m_noiseGenY.SetNbOctave(1);
}

Ray_PlayerControllerComponent::StateStargate::~StateStargate()
{
    stopTrail();
}

void Ray_PlayerControllerComponent::StateStargate::onActorLoaded(Pickable::HotReloadType _hotReloadType )
{
    Super::onActorLoaded(_hotReloadType);

    if (!m_parent->getTemplate()->getStargateTravelTrailPath().isEmpty())
    {
        SPAWNER->declareNeedsSpawnee(m_actor, &m_trailGenerator, m_parent->getTemplate()->getStargateTravelTrailPath());
    }
}

void Ray_PlayerControllerComponent::StateStargate::onEnter()
{
    Super::onEnter();

    m_currentStage = STARGATESTAGE_SUCKIN;
    m_nextStage = STARGATESTAGE_SUCKIN;
    m_parent->detachFromEverything();
    m_parent->resetMoveToPoint();
    m_characterPhys->setDisabled(btrue);
    m_currentTimer = 0.f;
    m_totalTime = 0.f;
	m_rotateCounter = 0.f;
	m_rotateStart = 0.f;
	m_rotateTarget = 0.f;
	m_rotateTime = 0.f;
    m_decelerateTime = 0.f;
    m_decelerateCounter = 0.f;
    m_lastTrip = bfalse;
    m_justEntered = btrue;
	m_speed = m_characterPhys->getSpeed().to3d();

    m_spline.clear();
    m_noiseGenX.SetSeed(Seeder::getSharedSeeder().GetU32());
    m_noiseGenY.SetSeed(Seeder::getSharedSeeder().GetU32());
}

void Ray_PlayerControllerComponent::StateStargate::onExit()
{
    Super::onExit();

    if ( m_currentStage != STARGATESTAGE_WAIT )
    {
        Ray_EventStargateStageFinished finishedStage(m_actor->getRef(),m_currentStage,btrue);

        RAY_GAMEMANAGER->onEvent(&finishedStage);
    }

    m_characterPhys->setDisabled(bfalse);
    stopTrail();
}

bbool Ray_PlayerControllerComponent::StateStargate::checkStateChange( f32 _dt )
{
    if ( m_currentStage == STARGATESTAGE_END )
    {
        m_parent->changeState(&m_parent->m_stateFalling);
        return btrue;
    }

    return bfalse;
}

void Ray_PlayerControllerComponent::StateStargate::update(f32 _dt)
{
    Super::update(_dt);

    if ( m_justEntered || m_currentStage != m_nextStage )
    {
        setNextStage();
    }

    switch(m_currentStage)
    {
        case STARGATESTAGE_SUCKIN:
            updateSuckedIn(_dt);
            break;
        case STARGATESTAGE_WAIT:
            updateWait(_dt);
            break;
        case STARGATESTAGE_TRAVEL:
            updateTravel(_dt);
            break;
    }

    updateTrail(_dt);

    m_justEntered = bfalse;
}

void Ray_PlayerControllerComponent::StateStargate::setNextStage()
{
    if ( m_currentStage != m_nextStage || m_justEntered )
    {
        m_currentStage = m_nextStage;
        m_currentTimer = 0.f;

        if ( m_currentStage == STARGATESTAGE_TRAVEL )
        {
            startTrail();

			Vec2d firstP = m_spline.GetNthPoint(0).truncateTo2D();
			Vec2d secondP = m_spline.GetNthPoint(1).truncateTo2D();
			f32 timeSecondP = m_spline.getTimeAt(1);

			Vec2d dir = secondP - firstP;
			dir.normalize();
			f32 dotP = Vec2d::Right.dot(dir);
			m_rotateStart = 0.f;
			m_rotateTarget = -f32_ACos(dotP);

            if ( m_actor->isFlipped() )
            {
                m_rotateTarget *= -1.f;
            }

			m_rotateCounter = m_rotateTime = timeSecondP;
        }
		else if ( m_currentStage == STARGATESTAGE_SUCKIN )
		{
			f32 noiseAmplitude = m_parent->getTemplate()->getStargateNoiseAmplitude();
			f32 xNoise = m_noiseGenX.Get(0.f) * noiseAmplitude;
			f32 yNoise = m_noiseGenY.Get(0.f) * noiseAmplitude;

			m_waitPos = m_suckedInPoint.truncateTo2D();
			m_suckedInPoint.m_x += xNoise;
			m_suckedInPoint.m_y += yNoise;

            Vec2d waitPosDir = ( m_waitPos - m_stargateStart.truncateTo2D() );
            f32 dotP = Vec2d::Right.dot(waitPosDir);

            if ( dotP > -MTH_EPSILON )
            {
                m_actor->setFlipped(bfalse);
            }
            else
            {
                m_actor->setFlipped(btrue);
            }
		}
    }
}

void Ray_PlayerControllerComponent::StateStargate::updateSuckedIn( f32 _dt )
{
    Vec3d dirToPos = m_suckedInPoint - m_actor->getPos();
    Vec3d f = ( dirToPos * m_parent->getTemplate()->getStargateSuckinForceK() ) - ( m_speed * m_parent->getTemplate()->getStargateSuckinForceD() );

    m_speed += f*_dt;

    m_characterPhys->setSpeed(m_speed.truncateTo2D());

    Vec3d newPos = m_actor->getPos() + (m_speed*_dt);

    m_actor->setPos(newPos);

    bbool inPlace = dirToPos.IsEqual(Vec3d::Zero,0.1f);

    if ( !inPlace )
    {
        Vec2d newDirToPos = m_suckedInPoint.truncateTo2D() - newPos.truncateTo2D();

        newDirToPos.normalize();
        dirToPos.normalize();

        if ( newDirToPos.dot(dirToPos.truncateTo2D()) <= 0.f )
        {
            inPlace = btrue;
        }
    }

    if ( inPlace )
    {
        Ray_EventStargateStageFinished finishedStage(m_actor->getRef(),STARGATESTAGE_SUCKIN);

        RAY_GAMEMANAGER->onEvent(&finishedStage);

        m_nextStage = m_currentStage = STARGATESTAGE_WAIT;
        m_currentTimer = 0.f;
    }
}

void Ray_PlayerControllerComponent::StateStargate::updateWait( f32 _dt )
{
    f32 noiseAmplitude = m_parent->getTemplate()->getStargateNoiseAmplitude();
    f32 curTime = m_currentTimer;
    f32 xNoise = m_noiseGenX.Get(curTime) * noiseAmplitude;
    f32 yNoise = m_noiseGenY.Get(curTime) * noiseAmplitude;

    // Safeguard against Perlin noise going crazy
    // PerlinNoise::Get() seems to return a value between -0.5 and 0.5, will assert otherwise
    if ( abs(xNoise) >= noiseAmplitude || abs(yNoise) >= noiseAmplitude )
    {
        // Bring the noise back towards zero
        xNoise -= xNoise * _dt;
        yNoise -= yNoise * _dt;
    }

    Vec2d newPos = m_waitPos + Vec2d(xNoise,yNoise);

    m_actor->setPos(newPos.to3d(m_actor->getPos().m_z));

    m_currentTimer += _dt * m_parent->getTemplate()->getStargateNoiseTimeMultiplier();
}

void Ray_PlayerControllerComponent::StateStargate::updateTravel( f32 _dt )
{
    f32 deltaTime = _dt;
    f32 totalTime = m_spline.getTimeAt(m_spline.GetNumPoints()-1);

    if ( m_lastTrip )
    {
        if ( !m_decelerateCounter )
        {
            f32 decelerateTime = Max(0.f,totalTime - m_parent->getTemplate()->getStargateDecelerateTime());

            if ( m_currentTimer <= decelerateTime && (m_currentTimer+_dt) >= decelerateTime )
            {
                m_decelerateTime = m_decelerateCounter = Min(totalTime,m_parent->getTemplate()->getStargateDecelerateTime());
            }
        }
    }

    if ( m_decelerateCounter )
    {
        f32 t = m_decelerateCounter/m_decelerateTime;
        
        t *= t;

        deltaTime *= Interpolate(0.2f,1.f,t);

        m_decelerateCounter = Max(m_decelerateCounter-deltaTime,0.f);
    }

	f32 prevTimer = m_currentTimer;
	f32 rotateTimer = totalTime - m_rotateTime;
    m_currentTimer = Min(m_currentTimer+deltaTime,totalTime);

    Vec3d newPos;

    m_spline.GetInterpolatedAtTime(m_currentTimer,newPos);

    m_actor->setPos(newPos);

	if ( m_rotateCounter )
	{
		m_rotateCounter = Max(m_rotateCounter-deltaTime,0.f);

		f32 t = 1.f - (m_rotateCounter/m_rotateTime);
		f32 newAngle = Interpolate(m_rotateStart,m_rotateTarget,t);

		m_actor->setAngle(newAngle);
	}
	if ( prevTimer < rotateTimer && m_currentTimer >= rotateTimer )
	{
		m_rotateTarget = 0.f;
		m_rotateStart = m_actor->getAngle();
		m_rotateCounter = m_rotateTime;
	}

    if ( m_currentTimer >= totalTime )
    {
        Ray_EventStargateStageFinished finishedStage(m_actor->getRef(),STARGATESTAGE_TRAVEL);

        RAY_GAMEMANAGER->onEvent(&finishedStage);

        m_nextStage = m_currentStage = STARGATESTAGE_WAIT;
        m_currentTimer = 0.f;
        m_waitPos = m_actor->get2DPos();
    }
}

void Ray_PlayerControllerComponent::StateStargate::startTrail()
{
    Actor* trail = m_trailGenerator.getSpawnee(m_actor->getScene(), m_actor->getPos());

    if (trail)
    {
        // if world doesn't support dynamic loading, do it manually
        if (trail->getScene()->getWorld()->getIgnoreCells())
        {
            trail->loadResources();
        }
        trail->enable();
        m_trailRef = trail->getRef();
        m_actor->getBinding()->bindChild(m_trailRef);
    }
}

void Ray_PlayerControllerComponent::StateStargate::stopTrail()
{
    Actor* trail = m_trailRef.getActor();

    if (trail)
    {
        m_actor->getBinding()->unbindChild(m_trailRef);
        m_trailRef = ITF_INVALID_OBJREF;
        trail->requestDestruction();
    }
}

void Ray_PlayerControllerComponent::StateStargate::updateTrail( f32 _dt )
{
    Actor* trail = m_trailRef.getActor();

    if (trail)
    {
        trail->setPos(m_actor->getPos());
    }
}

void Ray_PlayerControllerComponent::StateStargate::onEvent( Event* _event )
{
    Super::onEvent(_event);

    if (Ray_EventStargateUse* stargateUseEvent = _event->DynamicCast<Ray_EventStargateUse>(ITF_GET_STRINGID_CRC(Ray_EventStargateUse,3921841867)))
    {
        processStargateUse(stargateUseEvent);
    }
}

void Ray_PlayerControllerComponent::StateStargate::processStargateUse( class Ray_EventStargateUse* _stargateUseEvent )
{
    if ( _stargateUseEvent->getStage() == STARGATESTAGE_SUCKIN )
    {
        m_suckedInPoint = _stargateUseEvent->getSuckPoint();
        m_stargateStart = _stargateUseEvent->getStargateStart();
    }
    else if ( _stargateUseEvent->getStage() == STARGATESTAGE_TRAVEL )
    {
        m_spline = _stargateUseEvent->getSpline();
        m_lastTrip = _stargateUseEvent->getIsLastTrip();
    }

    m_nextStage = _stargateUseEvent->getStage();
}

void Ray_PlayerControllerComponent::StateStargate::updateAnimInputs()
{
    Super::updateAnimInputs();

    m_animComponent->setInputFromIndex(m_parent->m_animInputIndex[ANIMINPUT_STARGATESTAGE],static_cast<u32>(m_currentStage));
}

}
