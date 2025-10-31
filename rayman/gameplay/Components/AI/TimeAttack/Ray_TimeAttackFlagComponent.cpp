#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_TIMEATTACKFLAGCOMPONENT_H_
#include "rayman/gameplay/Components/ai/TimeAttack/Ray_TimeAttackFlagComponent.h"
#endif //_ITF_RAY_TIMEATTACKFLAGCOMPONENT_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef ITF_RAYONLINETRACKING_MANAGER_H
	#include "rayman/gameplay/Ray_OnlineTrackingManager.h"
#endif

#ifdef ITF_SUPPORT_CHEAT
#ifndef _ITF_RAY_CHEATMANAGER_H_
#include "rayman/gameplay/Managers/Ray_CheatManager.h"
#endif //_ITF_RAY_CHEATMANAGER_H_
#endif // ITF_SUPPORT_CHEAT

#ifndef ITF_RAYONLINETRACKING_MANAGER_H
	#include "rayman/gameplay/Ray_OnlineTrackingManager.h"
#endif


namespace ITF
{

IMPLEMENT_OBJECT_RTTI(Ray_TimeAttackFlagComponent)
BEGIN_SERIALIZATION_CHILD(Ray_TimeAttackFlagComponent)

END_SERIALIZATION()

Ray_TimeAttackFlagComponent::Ray_TimeAttackFlagComponent()
: m_timeCounter(0.f)
{
}

Ray_TimeAttackFlagComponent::~Ray_TimeAttackFlagComponent()
{
}

void Ray_TimeAttackFlagComponent::onActorClearComponents()
{
    Super::onActorClearComponents();

    if ( RAY_GAMEMANAGER->getTimeAttackActorFlag() == m_actor->getRef() )
    {
        RAY_GAMEMANAGER->setTimeAttackActorFlag(ObjectRef::InvalidRef);
    }
}

void Ray_TimeAttackFlagComponent::onActorLoaded(Pickable::HotReloadType _hotReload )
{
    Super::onActorLoaded(_hotReload);

    RAY_GAMEMANAGER->setTimeAttackActorFlag(m_actor->getRef());

    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTrigger,1343042510),this);
}

void Ray_TimeAttackFlagComponent::Update( f32 _dt )
{
    Super::Update(_dt);

    if ( m_timeCounter )
    {
        m_timeCounter -= _dt;

        if ( m_timeCounter <= 0.f )
        {
            m_timeCounter = 0.f;

            EventSequenceActorPrepare stopEvent(btrue);

            for (u32 i = 0; i < GAMEMANAGER->getMaxPlayerCount(); i++)
            {
                Actor* player = GAMEMANAGER->getActivePlayer(i);
                if ( player )
                {
                    player->onEvent(&stopEvent);
                }
            }

            RAY_GAMEMANAGER->setTriggerTimeAttackSequence();
        }
    }
}

void Ray_TimeAttackFlagComponent::onEvent( Event* _event )
{

	LOG("Ray_TimeAttackFlagComponent::onEvent");

    if ( _event->DynamicCast<EventTrigger>(ITF_GET_STRINGID_CRC(EventTrigger,1343042510)) != NULL )
    {


		LOG("Ray_TimeAttackFlagComponent::onEvent inside if ");
		
        if ( !m_timeCounter )
        {
            m_timeCounter = getTemplate()->getWaitTime();
            RAY_GAMEMANAGER->setTimeAttackFinished();

        }

#ifdef ITF_SUPPORT_ONLINETRACKING
				
			RAY_GAMEMANAGER->getOnlineTrackingManager()->m_timeAttackTime = RAY_GAMEMANAGER->getTimeAttackTimer();
			RAY_GAMEMANAGER->getOnlineTrackingManager()->SendLevelStop(Ray_OnlineTrackingManager::CP_EndOfTimeAttack);
#endif

    }
}

///////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_OBJECT_RTTI(Ray_TimeAttackFlagComponent_Template)
BEGIN_SERIALIZATION(Ray_TimeAttackFlagComponent_Template)

    SERIALIZE_MEMBER("waitTime",m_waitTime);

END_SERIALIZATION()

Ray_TimeAttackFlagComponent_Template::Ray_TimeAttackFlagComponent_Template()
: m_waitTime(2.f)
{
}

Ray_TimeAttackFlagComponent_Template::~Ray_TimeAttackFlagComponent_Template()
{
}

}
