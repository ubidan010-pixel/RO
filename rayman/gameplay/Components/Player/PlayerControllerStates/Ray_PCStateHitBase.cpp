#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_
#include "rayman/gameplay/Components/Player/Ray_PlayerControllerComponent.h"
#endif //_ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_

#ifndef _ITF_ANIMATIONTRACK_H_
#include "engine/animation/AnimationTrack.h"
#endif //_ITF_ANIMATIONTRACK_H_

#ifndef _ITF_RAY_ANIMATIONMARKERS_H_
#include "rayman/gameplay/Ray_AnimationMarkers.h"
#endif //_ITF_RAY_ANIMATIONMARKERS_H_

#ifndef _ITF_STIMSMANAGER_H_
#include "gameplay/managers/stimsmanager.h"
#endif //_ITF_STIMSMANAGER_H_

#ifndef _ITF_ID_SERVER_H_
#include "core/IDserver.h"
#endif //_ITF_ID_SERVER_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_

#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI_SUBCLASS(Ray_PlayerControllerComponent,StateHitBase)

////////////////////
// Hit Base State
////////////////////
void Ray_PlayerControllerComponent::StateHitBase::update(f32 _dt)
{
    switch(m_parent->m_stance)
    {
    case STANCE_HANG:
        updateHang(_dt);
        break;
    case STANCE_CLIMB:
        updateClimb(_dt);
        break;
    case STANCE_SWIM:
        updateSwim(_dt);
        break;
    case STANCE_STAND:
    default:
        updateStand(_dt);
        break;
    }

}

void Ray_PlayerControllerComponent::StateHitBase::updateLookDir()
{
    if ( !canUpdateLookDir() )
    {
        return;
    }

    m_parent->updateLookRightFromMoveDirection(m_parent->getMoveData().getMoveDirection());
}

void Ray_PlayerControllerComponent::StateHitBase::updateStand(f32 _dt)
{
    updateLookDir();

    if ( !m_characterPhys->getStickedEdge() )
    {
        if ( m_parent->m_currentPunchHitType == PUNCHTYPE_CRUSH )
        {
            m_parent->UpdatePhysicCrushHit(_dt);
        }
        else
        {
            if (m_parent->m_airHitInProgress)
            {
                m_parent->UpdatePhysicAir(btrue);
            }
        }
    }
}

void Ray_PlayerControllerComponent::StateHitBase::updateHang(f32 _dt)
{
    Vec2d pos;
    f32 angle;

    if ( m_parent->getWorldHangPos(pos,angle) )
    {
        m_animComponent->lockMagicBox(pos.to3d(m_actor->getPos().m_z));
        m_actor->setAngle(angle);
    }
}

void Ray_PlayerControllerComponent::StateHitBase::updateClimb(f32 _dt)
{
    updateLookDir();

    m_parent->UpdatePhysicClimbingEdge(_dt);
}

void Ray_PlayerControllerComponent::StateHitBase::updateSwim(f32 _dt)
{
    m_parent->UpdatePhysicSwim(_dt);
}

}
