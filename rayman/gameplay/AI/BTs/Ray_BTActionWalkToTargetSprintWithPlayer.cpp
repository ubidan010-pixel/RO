
#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_BTACTIONWALKTOTARGETSPRINTWITHPLAYER_H_
#include "rayman/gameplay/ai/BTs/Ray_BTActionWalkToTargetSprintWithPlayer.h"
#endif // _ITF_RAY_BTACTIONWALKTOTARGETSPRINTWITHPLAYER_H_

#ifndef _ITF_RAY_GROUNDAICONTROLLERCOMPONENT_H_
#include "rayman/gameplay/Components/ai/Ray_GroundAIControllerComponent.h"
#endif //_ITF_RAY_GROUNDAICONTROLLERCOMPONENT_H_

#ifndef _ITF_BEHAVIORTREE_H_
#include "gameplay/ai/BTs/BehaviorTree.h"
#endif // _ITF_BEHAVIORTREE_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(Ray_BTActionWalkToTargetSprintWithPlayer);

Ray_BTActionWalkToTargetSprintWithPlayer::Ray_BTActionWalkToTargetSprintWithPlayer()
: m_checkTimer(0.f)
, m_sprinting(bfalse)
{
}

Ray_BTActionWalkToTargetSprintWithPlayer::~Ray_BTActionWalkToTargetSprintWithPlayer()
{
}

bbool Ray_BTActionWalkToTargetSprintWithPlayer::validate( Actor* _actor ) const
{
    bbool retVal = Super::validate(_actor);

    if ( !retVal )
    {
        return bfalse;
    }

    if ( m_aiController && m_aiController->DynamicCast<Ray_GroundAIControllerComponent>(ITF_GET_STRINGID_CRC(Ray_GroundAIControllerComponent,1159656342)) == NULL )
    {
        ITF_WARNING_PARAM(_actor,"Ray_groundAIControllerComponent",m_aiController!=NULL,"%s: Ray_groundAIControllerComponent is mandatory",StringID(GetObjectClassCRC()).getDebugString());
        return bfalse;
    }

    return btrue;
}

void Ray_BTActionWalkToTargetSprintWithPlayer::onActivate()
{
    Super::onActivate();

    checkSprinting();
}

void Ray_BTActionWalkToTargetSprintWithPlayer::onDeactivate()
{
    Super::onDeactivate();

    getAIController()->setSprint(bfalse);
}

bbool Ray_BTActionWalkToTargetSprintWithPlayer::update( f32 _dt )
{
    bbool retVal = Super::update(_dt);

    m_checkTimer -= _dt;

    if ( m_checkTimer <= 0.f )
    {
        checkSprinting();
    }

    getAIController()->setSprint(m_sprinting);

    return retVal;
}

void Ray_BTActionWalkToTargetSprintWithPlayer::checkSprinting()
{
    m_sprinting = bfalse;
    m_checkTimer = getTemplate()->getCheckIntervals();

    const DepthRange depthRange(m_tree->getActor()->getDepth());
    const u32 uPlayerCount = GAMEMANAGER->getMaxPlayerCount();
    f32 radiusCheckSq = getTemplate()->getRange()*getTemplate()->getRange();
    f32 minDistSq = 0.f;
    Actor* closest = NULL;

    for (u32 i = 0; i < uPlayerCount; ++i)
    {
        Actor* pActor = GAMEMANAGER->getActivePlayer(i);

        if ( !pActor || depthRange != DepthRange(pActor->getDepth()) )
        {
            continue;
        }

        f32 distSq = ( pActor->get2DPos() - m_tree->getActor()->get2DPos() ).sqrnorm();

        if ( distSq < radiusCheckSq )
        {
            if ( closest == ITF_INVALID_OBJREF ||
                distSq < minDistSq )
            {
                closest = pActor;
                minDistSq = distSq;
            }
        }
    }

    if ( closest )
    {
        EventQueryIsSprinting isSprinting;

        closest->onEvent(&isSprinting);

        m_sprinting = isSprinting.getIsSprinting();
    }
}

Ray_GroundAIControllerComponent* Ray_BTActionWalkToTargetSprintWithPlayer::getAIController() const
{
    return static_cast<Ray_GroundAIControllerComponent*>(m_aiController);
}

IMPLEMENT_OBJECT_RTTI(Ray_BTActionWalkToTargetSprintWithPlayer_Template);

BEGIN_SERIALIZATION_CHILD(Ray_BTActionWalkToTargetSprintWithPlayer_Template)

END_SERIALIZATION()

Ray_BTActionWalkToTargetSprintWithPlayer_Template::Ray_BTActionWalkToTargetSprintWithPlayer_Template()
: m_checkIntervals(1.f)
, m_range(10.f)
{
}

}