#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RayVita_AIRelicBehavior_H_
#include "rayman/gameplay/GamePlayVita/ai/Behaviors/Relic/RayVita_AIRelicBehavior.h"
#endif //_ITF_RayVita_AIRelicBehavior_H_

#ifndef _ITF_PHANTOMCOMPONENT_H_
#include "gameplay/Components/Misc/PhantomComponent.h"
#endif //_ITF_PHANTOMCOMPONENT_H_

#ifndef _ITF_RAY_AICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_AIComponent.h"
#endif //_ITF_RAY_AICOMPONENT_H_

#ifndef _ITF_PHYSPHANTOM
#include "engine/physics/PhysPhantom.h"
#endif //_ITF_PHYSPHANTOM

#ifndef _ITF_POLYLINECOMPONENT_H_
#include "gameplay/Components/Environment/PolylineComponent.h"
#endif //_ITF_POLYLINECOMPONENT_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_


#ifndef _ITF_TRIGGERCOMPONENT_H_
#include "gameplay/Components/Trigger/TriggerComponent.h"
#endif //_ITF_TRIGGERCOMPONENT_H_

#ifndef _ITF_RAY_PLAYERHUDSCORECOMPONENT_H_
#include "rayman/gameplay/Components/Player/Ray_PlayerHudScoreComponent.h"
#endif //_ITF_RAY_PLAYERHUDSCORECOMPONENT_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#define RELIC_STAND_ANIM            "Stand"
#define RELIC_SIGNE_ANIM            "Signe"
#define RELIC_TOUCH_ANIM            "Touch"
#define RELIC_MOVE_ANIM				"Move"
#define RELIC_ICON_ANIM				"Icon"


namespace ITF
{

    IMPLEMENT_OBJECT_RTTI(RayVita_AIRelicBehavior_Template)
    BEGIN_SERIALIZATION_CHILD(RayVita_AIRelicBehavior_Template)
		SERIALIZE_MEMBER("frescoId", m_frescoId);
        SERIALIZE_MEMBER("relicIndex", m_relicIndex);
		SERIALIZE_MEMBER("idleSound", m_idleSound);

		SERIALIZE_MEMBER("takenTrajectoryFactorX", m_takenTrajectoryFactorX);
		SERIALIZE_MEMBER("takenTrajectoryFactorY", m_takenTrajectoryFactorY);
		SERIALIZE_MEMBER("percentTimeStartFading", m_percentTimeStartFading);
		SERIALIZE_MEMBER("alphaWhenReachedScore", m_alphaWhenReachedScore);
		SERIALIZE_MEMBER("endDuration", m_endDuration);
		SERIALIZE_MEMBER("iconDuration", m_iconDuration);
		SERIALIZE_MEMBER("pickingAnimDuration", m_pickingAnimDuration);
		SERIALIZE_OBJECT("particleLumAABB", m_particleLumAABB);
		SERIALIZE_MEMBER("scaleWhenReachedScore", m_scaleWhenReachedScore);
		SERIALIZE_MEMBER("percentTimeStartScaling", m_percentTimeStartScaling);
		SERIALIZE_MEMBER("signeDuration", m_signeDuration);
		SERIALIZE_MEMBER("waitingBeforeSigneDuration", m_waitingBeforeSigneDuration);
		SERIALIZE_MEMBER("waitingBeforePlayIdleSoundDuration", m_waitingBeforePlayIdleSoundDuration);
    END_SERIALIZATION()

    RayVita_AIRelicBehavior_Template::RayVita_AIRelicBehavior_Template()
	: m_frescoId(0)
    , m_relicIndex(-1)
	, m_idleSound(StringID::Invalid)

	, m_takenTrajectoryFactorX(0.5f)
	, m_takenTrajectoryFactorY(0.2f)
	, m_percentTimeStartFading(0.8f)
	, m_alphaWhenReachedScore(0.2f)
	, m_particleLumAABB(Vec2d(-0.25f,0), Vec2d(0.25f,1-0.655f))
	, m_scaleWhenReachedScore(0.5f)
	, m_percentTimeStartScaling(0.5f)
	, m_signeDuration(1.0f)
	, m_waitingBeforeSigneDuration(1.0f)
	, m_waitingBeforePlayIdleSoundDuration(30.0f)
   {
    }

    IMPLEMENT_OBJECT_RTTI(RayVita_AIRelicBehavior)
    BEGIN_VALIDATE_BEHAVIOR(RayVita_AIRelicBehavior)
        VALIDATE_BEHAVIOR_PARAM("frescoId", m_frescoId>=0, "invalid fresco Id");
		VALIDATE_BEHAVIOR_PARAM("relicIndex", m_relicIndex>=0, "invalid relic index");
        VALIDATE_BEHAVIOR_PARAM("", m_triggerComponent!=NULL, "There must be a TriggerComponent for detecting picking");
    END_VALIDATE_BEHAVIOR()


    #define relicOutStopMarker ITF_GET_STRINGID_CRC(MRK_Out_GameplayStop,3127983330)

    ///////////////////////////////////////////////////////////////////////////////////////////
    RayVita_AIRelicBehavior::RayVita_AIRelicBehavior()
        : Super()
        , m_state(State_Stands)
        , m_triggerComponent(NULL)
        , m_frescoId(0)
		, m_relicIndex(-1)
		, m_destinationScorePos(0,0)
		, m_timeInCurrentState(0.f)
		, m_usePickingAnim(btrue)
		, m_waitingBeforSignTime(0.f)
		, m_idleSoundTime(0.f)
		, m_fxControllerComponent(NULL)
		, m_linkComponent(NULL)
    {        
    }

    RayVita_AIRelicBehavior::~RayVita_AIRelicBehavior()
    {
        if ( EVENTMANAGER )
        {
            EVENTMANAGER->unregisterEvent(ITF_GET_STRINGID_CRC(EventRevertToLastCheckpoint,3654122082), m_aiComponent);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void RayVita_AIRelicBehavior::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

		m_frescoId = getTemplate()->getFrescoId();
		m_relicIndex = getTemplate()->getRelicIndex();
        m_triggerComponent = m_actor->GetComponent<TriggerComponent>();
		m_fxControllerComponent = m_actor->GetComponent<FXControllerComponent>();
		m_linkComponent = m_actor->GetComponent<LinkComponent>();

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301), m_aiComponent);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventRewardPickedUp,3660427225), m_aiComponent);
        EVENTMANAGER_REGISTER_EVENT_LISTENER(ITF_GET_STRINGID_CRC(EventRevertToLastCheckpoint,3654122082),m_aiComponent);
        //
        resetState();
    }

    void RayVita_AIRelicBehavior::onResourceReady()
    {
        Super::onResourceReady();                        
    }

	void RayVita_AIRelicBehavior::onActorBecomeInactive()
	{
		Super::onActorBecomeInactive();

		if(Ray_PlayerHudScoreComponent::getIsRelicCaptionEnabled())
		{
			Ray_PlayerHudScoreComponent::setIsRelicCaptionEnabled(bfalse);
			m_actor->disable();
			m_animComponent->enableDraw(bfalse);
			//if(m_linkComponent)
			//{
			//	EventTrigger trigger;
			//	trigger.setSender(m_actor->getRef());
			//	trigger.setActivated(bfalse);
			//	m_linkComponent->sendEventToChildren(&trigger);
			//}
		}
	}

    void RayVita_AIRelicBehavior::resetState()
    {
        //if (RAY_GAMEMANAGER->getIsRelicTaken(m_relicIndex))
		if (RAY_GAMEMANAGER->getIsRelicTaken(RAY_GAMEMANAGER->getCurrentLevelName(),m_relicIndex))
		{
			setState(State_Taken);
		}
        else
        {
            setState(State_Stands);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void RayVita_AIRelicBehavior::update( f32 _deltaTime )
    {
        Super::update(_deltaTime);

		m_timeInCurrentState += _deltaTime;
		m_waitingBeforSignTime += _deltaTime;
		m_idleSoundTime += _deltaTime;
		m_pickingTrajectoryFollower.incrementTime(_deltaTime);
		
#ifdef ITF_VITA //CASA::VITA::TARIK::FRESCO GAMEPLAY
		if (m_state != State_Taken &&  INPUT_ADAPTER->IsTPJustPressed() )
		{
			AABB aabb = m_actor->getAABB();

			if ( INPUT_ADAPTER->IsUnderTPPos(aabb,m_actor->getPos().getZ(),20.f) )
			{
				setState(State_BeingTaken);
			}				
		}
#endif

		if (m_state < State_BeingTaken && isNearMainPlayer())
		{
			setState(State_BeingTaken);
		}

        //Update the states. 
        switch (m_state)
        {
            ///////////////////////////////////////////////////////////////////////////////////////////
            case State_Stands:
            {
				if (m_waitingBeforSignTime>=getTemplate()->getWaitingBeforeSigneDuration())
				{
					m_waitingBeforSignTime = 0.f;
					setState(State_Signe);
				}

				if (m_idleSoundTime>=getTemplate()->getWaitingBeforePlayIdleSoundDuration())
				{
					m_idleSoundTime = 0.f;
					playSound(getTemplate()->getIdleSoundID());
				}

			} break;
            ///////////////////////////////////////////////////////////////////////////////////////////
            case State_Signe:
            {
				if (m_timeInCurrentState>=getTemplate()->getSigneDuration())
				{
					setState(State_Stands);
				}
            } break;
            ///////////////////////////////////////////////////////////////////////////////////////////
            case State_BeingTaken:
            {
				if (m_pickingTrajectoryFollower.getTimeSinceTaken()>=getTemplate()->getPickingAnimDuration() || !m_usePickingAnim)
				{
					setState(State_FlyingToInterface);
				}

            } break;            
            ///////////////////////////////////////////////////////////////////////////////////////////
            case State_FlyingToInterface:
            {
				f32 lambda = moveOnTrajectory();
				if (lambda>=1)
				{
					setState(State_PreTaken);
				}
               
            } break;	

			case State_PreTaken:
				{
					moveOnTrajectory();
					if (m_timeInCurrentState>=getTemplate()->getEndDuration())
					{
						setState(State_EndTaken);
					}
				} break;
			///////////////////////////////////////////////////////////////////////////////////////////
            case State_EndTaken:
            {
				if (m_timeInCurrentState>=getTemplate()->getIconDuration())
				{
					setState(State_Taken);
				}            
			} break;
			///////////////////////////////////////////////////////////////////////////////////////////
            case State_Taken:
            {
            } break;        
		}
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void RayVita_AIRelicBehavior::onEvent( Event* _event )
    {
        Super::onEvent(_event);

        if (AnimGameplayEvent *animEvent = _event->DynamicCast<AnimGameplayEvent>(ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301)))
        {
            if ((animEvent->getName()==relicOutStopMarker))
            {
                setState(State_Stands);
            }
        }
//#ifndef ITF_VITA //CASA::VITA::TARIK::FRESCO GAMEPLAY
        else if (Ray_EventRewardPickedUp *pickedupEvt = _event->DynamicCast<Ray_EventRewardPickedUp>(ITF_GET_STRINGID_CRC(Ray_EventRewardPickedUp,3660427225)))
        {
            setState(State_BeingTaken);
        }
//#endif
        //else if (EventRevertToLastCheckpoint *checkpointEvent = _event->DynamicCast<EventRevertToLastCheckpoint>(ITF_GET_STRINGID_CRC(EventRevertToLastCheckpoint,3654122082)))
        //{
        //    resetState();
        //}

    }



    ///////////////////////////////////////////////////////////////////////////////////////////
    ///Set state, and initialize the state we enter
    void RayVita_AIRelicBehavior::setState(State_t _state)
    {
        if (_state!=m_state)
        {
			m_timeInCurrentState = 0;
            m_state = _state;
        }

        if (_state==State_Stands)
        {
            m_triggerComponent->unpause();
        }
        else
        {
            m_triggerComponent->pause();
        }

        switch(_state)
        {
            case State_Stands:
            {
                m_animComponent->setAnim(RELIC_STAND_ANIM);
            } 
			break;

            case State_Signe:
            {
                m_animComponent->setAnim(RELIC_SIGNE_ANIM);
            } break;

			case State_BeingTaken:
            {
				Color curFogColor;
				curFogColor = m_animComponent->getColorFog();
				curFogColor.m_a = 0.f;
				m_animComponent->setColorFog( curFogColor );
				m_animComponent->setAnim(RELIC_TOUCH_ANIM);
				m_pickingTrajectoryFollower.computeFlightTrajectory(btrue, m_actor->getPos());
				const u32 playerIndex = GAMEMANAGER->getMainIndexPlayer();
				fetchTrajectoryParameters(GAMEMANAGER->getActivePlayer(playerIndex));
				//if(m_linkComponent)
				//{
				//	EventTrigger trigger;
				//	trigger.setSender(m_actor->getRef());
				//	trigger.setActivated(bfalse);
				//	m_linkComponent->sendEventToChildren(&trigger);
				//}
            } 
			break;

			case State_FlyingToInterface:
			{
				m_animComponent->setAnim(RELIC_MOVE_ANIM);
			} 
			break;

            case State_PreTaken: 
            {
				m_animComponent->setAnim(RELIC_ICON_ANIM);
            } 
			break;

			case State_EndTaken:
            {
				RAY_GAMEMANAGER->setIsRelicTaken(RAY_GAMEMANAGER->getCurrentLevelName(),m_relicIndex, btrue);
				RelicState relicState = RAY_GAMEMANAGER->getPersistentGameData()->UpdateInGameRelicsInfo(m_frescoId);
				REWARD_MANAGER->Action_New(Ray_RewardManager::Action_PickedRelic, 0);
				if(relicState==RELIC_STATE_NEW)
					Ray_PlayerHudScoreComponent::setIsRelicCaptionEnabled(btrue,btrue,m_frescoId);
				else
					Ray_PlayerHudScoreComponent::setIsRelicCaptionEnabled(btrue,bfalse,m_frescoId);
				m_animComponent->enableDraw(bfalse);
            } 
			break;

			case State_Taken:
			{					
				Ray_PlayerHudScoreComponent::setIsRelicCaptionEnabled(bfalse);
				m_actor->disable();
				m_animComponent->enableDraw(bfalse);
				//if(m_linkComponent)
				//{
				//	EventTrigger trigger;
				//	trigger.setSender(m_actor->getRef());
				//	trigger.setActivated(bfalse);
				//	m_linkComponent->sendEventToChildren(&trigger);
				//}
			} 
			break;        
		}
    }

	///////////////////////////////////////////////////////////////////////////////////////////
	bbool RayVita_AIRelicBehavior::fetchTrajectoryParameters(Actor *_player)
	{
		if (!_player)
			return bfalse;

		m_destinationScorePos = Vec2d::Zero;
		Vec3d playerPos_WhenPicked = m_actor->getPos();
		m_destinationScorePos = Ray_PlayerHudScoreComponent::getRelicPos();

		AABB scaledParticleLumAABB = getTemplate()->getParticleLumAABB();
		f32 scale = m_actor->getScale().m_x;
		scaledParticleLumAABB.setMin(scaledParticleLumAABB.getMin()*scale);
		scaledParticleLumAABB.setMax(scaledParticleLumAABB.getMax()*scale);

		//m_destinationScorePos+=Vec2d(0.f,150.0f);

		m_pickingTrajectoryFollower.initializeParameters(m_destinationScorePos, 
			_player->getRef(), 
			getTemplate()->getTakenTrajectoryFactorX(), getTemplate()->getTakenTrajectoryFactorY(),
			false,
			playerPos_WhenPicked,
			getTemplate()->getPercentTimeStartFading(),
			scaledParticleLumAABB,
			0,1,//m_addedLumValue, m_addedLumPlayerIndex,
			getTemplate()->getAlphaWhenReachedScore(),
			NULL,//circularMotionDesc,
			m_actor->getPos(),
			getTemplate()->getScaleWhenReachedScore(),
			getTemplate()->getPercentTimeStartScaling());

		return btrue;

	}

    ///////////////////////////////////////////////////////////////////////////////////////////
    f32 RayVita_AIRelicBehavior::moveOnTrajectory()
    {
        Vec3d projPos, projSpeed;
        Vec3d newPos;
        bbool isWaitingBeforeStart=bfalse;
        m_pickingTrajectoryFollower.computeFlightTrajectory(bfalse, m_actor->getPos());
        f32 alpha;
        bbool justReachedAlphaThreshold;
        f32 dummyScale=1;
        f32 lambda = m_pickingTrajectoryFollower.computePosOnTrajectory(projPos, projSpeed,isWaitingBeforeStart,0, alpha, justReachedAlphaThreshold,dummyScale);
        f32 completion = lambda;

        m_actor->setFlipped(bfalse);//projSpeed.m_x>=0);
        GFX_ADAPTER->compute2DTo3D(projPos, newPos);
        m_actor->forceMove(newPos);         

        return completion;
    }

	void RayVita_AIRelicBehavior::playSound(const StringID& _sound)
	{
		if ( m_fxControllerComponent != NULL)
		{
			m_fxControllerComponent->playFX(_sound);
		}
	}


	bbool RayVita_AIRelicBehavior::isNearMainPlayer() const
	{
		const u32 playerIndex = GAMEMANAGER->getMainIndexPlayer();
		Actor* playerActor = GAMEMANAGER->getActivePlayer(playerIndex);
		if (!playerActor) { return bfalse; }
		AABB _AABB = playerActor->getAABB();;
		f32 _z = playerActor->getPos().getZ();

		Vec3d AABB2D_Min, AABB2D_Max;
		Vec3d AABB3D_Min, AABB3D_Max;
		Vec3d pos3D, pos2D;
		AABB3D_Min.set(_AABB.getMin().m_x, _AABB.getMin().m_y, _z);
		AABB3D_Max.set(_AABB.getMax().m_x, _AABB.getMax().m_y, _z);
		pos3D.set(m_actor->getPos().getX(), m_actor->getPos().getY(), m_actor->getPos().getZ());
		GFX_ADAPTER->compute3DTo2D(AABB3D_Min, AABB2D_Min);
		GFX_ADAPTER->compute3DTo2D(AABB3D_Max, AABB2D_Max);
		GFX_ADAPTER->compute3DTo2D(pos3D, pos2D);
		_AABB.setMin(Vec2d(AABB2D_Min.m_x, AABB2D_Max.m_y));//_AABB.setMin(AABB2D_Min);
		_AABB.setMax(Vec2d(AABB2D_Max.m_x, AABB2D_Min.m_y));//_AABB.setMax(AABB2D_Max);

		if (_AABB.contains(pos2D, .0))
		{
			return btrue;
		}

		return bfalse;
	}
}
