#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_
#include "rayman/gameplay/Components/Player/Ray_PlayerControllerComponent.h"
#endif //_ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

namespace ITF
{
IMPLEMENT_OBJECT_RTTI_SUBCLASS(Ray_PlayerControllerComponent,Ray_State)

void Ray_PlayerControllerComponent::Ray_State::init(
    PlayerControllerComponent* _parent,
    AnimatedComponent* _animComponent,
    StickToPolylinePhysComponent* _characterPhys, 
    FXControllerComponent* _fxController )
{
    Super::init(_parent, _animComponent, _characterPhys, _fxController);
    m_parent = static_cast<Ray_PlayerControllerComponent*>(_parent);
}

bool Ray_PlayerControllerComponent::Ray_State::checkBounce()
{
    if ( m_parent->m_bounceCurrentDangerLevel >= s_dangerLevelDeath )
    {
        m_parent->triggerDeath(DEATHTYPE_REGULARDEATH);
        return true;
    }
    else if ( m_parent->m_bounceCurrentDangerLevel != 0 )
    {
        m_parent->changeState(&m_parent->m_stateReceiveHit);
        return true;
    }
    else if ( !m_parent->m_bounceTimer && m_parent->m_bounceCurrentDir != Vec2d::Zero )
    {
        if ( m_parent->m_currentState == &m_parent->m_stateJumping )
        {
            m_parent->m_stateJumping.restartJump();
        }
        else
        {
            m_parent->changeState(&m_parent->m_stateJumping);
        }
        
        return true;
    }

    return false;
}

#ifndef ITF_FINAL
void Ray_PlayerControllerComponent::Ray_State::drawDebug()
{
    if ( m_parent->m_drawAnimInputs )
    {
#define MAX_BUFF 256
#define DRAW_INPUT_INT(name,value) \
    SPRINTF_S(buf,MAX_BUFF,"%s: %d",name,value); \
    GFX_ADAPTER->drawDBGText(buf,fX,fY); \
    fY += fYOffset;
#define DRAW_INPUT_FLOAT(name,value) \
    SPRINTF_S(buf,MAX_BUFF,"%s: %f",name,value); \
    GFX_ADAPTER->drawDBGText(buf,fX,fY); \
    fY += fYOffset;

        char buf[MAX_BUFF];
        f32 fX = 0.f;
        f32 fY = 35.f;
        f32 fYOffset = 15.f;

        f32 totalSpeed = 0.f;
        f32 speedX = 0.f;
        f32 speedY = 0.f;
        u32 inAir = 0;

        // It sucks to duplicate this, but it's just for debug
        const Vec2d& speed = m_characterPhys->getSpeed();
        const PolyLineEdge* edge = m_characterPhys->getStickedEdge();
        Vec2d dir = Vec2d::Right.Rotate(m_actor->getAngle());

        if ( m_actor->isFlipped() )
        {
            dir = -dir;
        }

        totalSpeed = speed.norm();

        speedX = speed.dot(dir);
        speedY = speed.dot(-m_parent->m_currentGravityDir);
        inAir = edge == NULL;

        DRAW_INPUT_FLOAT("Speed",totalSpeed);
        DRAW_INPUT_FLOAT("SpeedX",speedX);
        DRAW_INPUT_FLOAT("SpeedY",speedY);
        DRAW_INPUT_FLOAT("MoveX",m_parent->m_moveInput.m_x);
        DRAW_INPUT_FLOAT("MoveY",m_parent->m_moveInput.m_y);
        DRAW_INPUT_INT("Stance",m_parent->m_stance);
        DRAW_INPUT_INT("PrevStance",m_parent->m_prevStance);
        DRAW_INPUT_INT("InAir",inAir);
        DRAW_INPUT_INT("PunchLevel",m_parent->m_currentPunchHitLevel);
        DRAW_INPUT_INT("ReceiveHitLevel",m_parent->m_receivedHitLevel);
        DRAW_INPUT_INT("OrientationPose",m_parent->m_orientationPose);
        DRAW_INPUT_FLOAT("Angle",MTH_RADTODEG*m_actor->getAngle());
        DRAW_INPUT_INT("UTurn",m_parent->m_performingUTurn);
        DRAW_INPUT_INT("PerformingSlide",m_parent->isPerformingSlide());
        DRAW_INPUT_FLOAT("AchievedSpeedFactor",m_parent->m_achievedSpeedFactor);

#undef MAX_BUFF
#undef DRAW_INPUT_INT
#undef DRAW_INPUT_FLOAT
    }
}
#endif // ITF_FINAL

}