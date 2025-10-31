#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_SHOOTERCONTROLLERCOMPONENT_H_
#include "rayman/gameplay/Components/Shooter/Ray_ShooterControllerComponent.h"
#endif //_ITF_RAY_SHOOTERCONTROLLERCOMPONENT_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#ifndef _ITF_SOUNDCOMPONENT_H_
#include "engine/actors/components/SoundComponent.h"
#endif //_ITF_SOUNDCOMPONENT_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

//#ifndef _ITF_PHYSSHAPES_H_
//#include "engine/physics/PhysShapes.h"
//#endif //_ITF_PHYSSHAPES_H_

#ifndef _ITF_PHYSPHANTOM
#include "engine/physics/PhysPhantom.h"
#endif //_ITF_PHYSPHANTOM

//#ifndef _ITF_PHYSBODY_H_
//#include "engine/physics/PhysBody.h"
//#endif //_ITF_PHYSBODY_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

#ifndef _ITF_RAY_GAMEMATERIAL_H_
#include "rayman/gameplay/Ray_GameMaterial.h"
#endif //_ITF_RAY_GAMEMATERIAL_H_

#ifndef _ITF_RAY_SHOOTERCAMERACOMPONENT_H_
#include "rayman/gameplay/Components/Camera/Ray_ShooterCameraComponent.h"
#endif //_ITF_RAY_SHOOTERCAMERACOMPONENT_H_

#ifndef _ITF_ANIMATION_H_
#include "engine/animation/Animation.h"
#endif //_ITF_ANIMATION_H_

#ifndef _ITF_CHEATMANAGER_H_
#include "engine/gameplay/CheatManager.h"
#endif //_ITF_CHEATMANAGER_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_STIMSMANAGER_H_
#include "gameplay/managers/stimsmanager.h"
#endif //_ITF_STIMSMANAGER_H_

#ifndef _ITF_RAY_SHOOTERACTORPARAMETERCOMPONENT_H_
#include "rayman\gameplay\Components\Shooter\Ray_ShooterActorParameterComponent.h"
#endif //_ITF_RAY_SHOOTERACTORPARAMETERCOMPONENT_H_

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_PHYSBODY_H_
#include "engine/physics/PhysBody.h"
#endif //_ITF_PHYSBODY_H_

#ifndef ITF_RAYONLINETRACKING_MANAGER_H
	#include "rayman/gameplay/Ray_OnlineTrackingManager.h"
#endif

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_ShooterControllerComponent)

    BEGIN_VALIDATE_COMPONENT(Ray_ShooterControllerComponent)
        m_heartShield.validate(_isComponentValidated);
    END_VALIDATE_COMPONENT()

    BEGIN_SERIALIZATION_CHILD(Ray_ShooterControllerComponent)
    END_SERIALIZATION()

    Ray_ShooterControllerComponent::Ray_ShooterControllerComponent()
    : Super()
    , m_playerDisabled( bfalse )
    , m_justActivatedDead( bfalse )
    , m_isDead(bfalse)
    , m_gameModeJustChanged( bfalse )
    , m_isDeadWaitRespawnTimer(0.f)
    , m_hurtTimer(0.0f)
    , m_hurtImmunityProcessHitAction( bfalse )
    , m_outofScreenTimer(0.0f)
    , m_gunBoneIndex(U32_INVALID)
    , m_swallowBoneIndex(U32_INVALID)
	, m_moveInput(Vec2d::Zero)
	, m_moveVector( Vec2d::Zero )
    , m_moveVaccumInertiaTimer( 0.0f )
    , m_moveSpeed( 0.0f )
    , m_lastValidInputDir( Vec2d::Zero )
	, m_capacityFlags( 0 )
	, m_temp_AnimFinishedCount( 0 )
	, m_vacuumRestartTimer( 0.0f )
    , m_vaccumButtonPressedDuration( 0.0f )
    , m_shootButtonPressedDuration( 0.0f )
    , m_animMoveBlendCursor( 0.5f )
    , m_animMoveXCursor( 0.5f )
    , m_animMoveYCursor( 0.5f )
    , m_curMode( normal )
    , m_camTransMode_ChangeDepthDest( 0.f )
    , m_camTransMode_ChangeDepthStartValue( 0.f )
    , m_heartShield()
    , m_swarmRepellerPowerUp()
 	, m_stateWantedMove( bfalse )
    , m_stateWantedLocalSpeedMax( 0.f )
    , m_stateWantedMoveInput( Vec2d::Zero )
    , m_isVacuumedTimer( 0.f )
    , m_isUnderVacuumInfluence( bfalse )
    , m_isUnderVacuumInfluenceForce( 0.f )
    , m_hasBeenVacuumed( bfalse )
    , m_isInWater( bfalse )
    , m_downInWater( bfalse )
    , m_waterSNDType( None )
    , m_listenerMove(Vec2d::Zero) 
    , m_listenerLeave(bfalse)
    , m_listenerShoot(bfalse)
    , m_listenerShootHold( bfalse )
    , m_listenerVacuum(bfalse)
    , m_listenerVacuumHold(bfalse)
    , m_listenerVacuumReleased(bfalse)
    , m_listenerAdded(bfalse)
    , m_listenerTest( bfalse )
    , m_invincibleTimer( 0.0f )
    , m_waterSndSurfHandle( U32_INVALID )
    , m_waterSndGlobalHandle( U32_INVALID )
    , m_lastHitFrame( U32_INVALID )
    //, m_test( 0.0f)
    {
        // Init state machine
		//---------------------
		getStateMachine()->reserveSize( STATE_COUNT );
		//WithAnimStateImplement * idleState		= CREATE_STATE_IMPLEMENT( WithAnimStateImplement );
		StateNormal * normalState				= CREATE_STATE_IMPLEMENT( StateNormal );
		StateVacuum * vaccumState				= CREATE_STATE_IMPLEMENT( StateVacuum );
		StateSpit *	spitState					= CREATE_STATE_IMPLEMENT( StateSpit );
		StateAttack *	attackState				= CREATE_STATE_IMPLEMENT( StateAttack );
		StateHit *	hitState				    = CREATE_STATE_IMPLEMENT( StateHit );
        StateSequences * sequencesState			= CREATE_STATE_IMPLEMENT( StateSequences );
        StateBubble * bubbleState			    = CREATE_STATE_IMPLEMENT( StateBubble );
        
		
		
		// WAIT PLAYER
		//**************
		//ADD_ANIM_STATE		( STATE_WAIT_PLAYER,		idleState		, "IDLE"	);
		ADD_ANIM_STATE		( STATE_WAIT_PLAYER,		NULL			, "IDLE"	);
                		
		// NORMAL
		//********
		ADD_STATE			( STATE_NORMAL_CHOOSE,				normalState							);
		ADD_STATE			( STATE_NORMAL_CHOOSE_ATTACK,		normalState							);
		ADD_STATE			( STATE_NORMAL_CHOOSE_VACCUM,		normalState							);
		ADD_ANIM_STATE		( STATE_NORMAL,						normalState		, "NORMAL"			);
        ADD_ANIM_STATE		( STATE_NORMAL_RIDE_ON,		        normalState		, "RIDE_ON"	        );
		ADD_ANIM_STATE		( STATE_NORMAL_CANTVACCUM,			normalState		, "FULL_CANTVACUUM"	);

        getStateMachine()->getAnimState( STATE_NORMAL_CANTVACCUM )->setEndCheckByAnimEvent();
        getStateMachine()->getAnimState( STATE_NORMAL_RIDE_ON )->setEndCheckByAnimEvent();
        getStateMachine()->getAnimState( STATE_NORMAL_RIDE_ON )->setDefaultNextStateID( STATE_NORMAL );
        getStateMachine()->getAnimState( STATE_NORMAL )->setRestartAnimIfSame( bfalse );
        getStateMachine()->getAnimState( STATE_NORMAL )->setEndCheckByAnimEnd( bfalse );            // do not check animation end beacause move are procedural
        
        
		// VACCUM
		//********
		ADD_ANIM_STATE		( STATE_VACCUM_START,				vaccumState		, "VACUUM_START"	);
		ADD_ANIM_STATE		( STATE_VACCUM_LOOP,				vaccumState		, "VACUUM"			);
        ADD_ANIM_STATE		( STATE_VACCUM_LOOP_CATCH,			vaccumState		, "VACUUM_CATCH"    );
        ADD_STATE			( STATE_VACCUM_CATCH,			    vaccumState							);
		ADD_STATE			( STATE_VACCUM_FINISHED,			vaccumState							);
        ADD_ANIM_STATE		( STATE_VACCUM_FINISHED_CATCH,		vaccumState		, "VACUUM_CATCH"	);
        //ADD_STATE		    ( STATE_VACCUM_FINISHED_NORMAL,	    vaccumState							);
        ADD_ANIM_STATE		( STATE_VACCUM_FINISHED_FROM_START,	vaccumState		, "VACUUM_STOP"	);
		ADD_ANIM_STATE		( STATE_VACCUM_FINISHED_FULL,		vaccumState		, "CATCH"			);
		ADD_ANIM_STATE		( STATE_VACCUM_QUICK,				vaccumState		, "VACUUM"			);

		getStateMachine()->getAnimState( STATE_VACCUM_START )->setEndCheckByAnimEvent();
        getStateMachine()->getAnimState( STATE_VACCUM_LOOP_CATCH )->setEndCheckByAnimEvent();
        getStateMachine()->getAnimState( STATE_VACCUM_LOOP_CATCH )->setDefaultNextStateID( STATE_VACCUM_LOOP );

		getStateMachine()->getAnimState( STATE_VACCUM_FINISHED_FROM_START )->setEndCheckByAnimEvent();
        getStateMachine()->getAnimState( STATE_VACCUM_FINISHED_CATCH )->setEndCheckByAnimEvent();
        getStateMachine()->getAnimState( STATE_VACCUM_FINISHED_CATCH )->setDefaultNextStateID( STATE_VACCUM_FINISHED );
        
        //getStateMachine()->getAnimState( STATE_VACCUM_FINISHED_NORMAL )->setEndCheckByAnimEvent();
		getStateMachine()->getAnimState( STATE_VACCUM_FINISHED_FULL )->setEndCheckByAnimEvent();
				
		// SPIT
		//*******
        ADD_ANIM_STATE		( STATE_SPIT_START,			    spitState		, "SPIT_START"	);
		ADD_STATE			( STATE_SPIT_END,			    spitState						);
		ADD_ANIM_STATE		( STATE_SPIT_TO_STAND,		    spitState		, "SPIT_TO_STAND");
        ADD_ANIM_STATE		( STATE_SPIT_BASESHOOT,		    spitState		, "NORMAL"      );
        ADD_ANIM_STATE		( STATE_SPIT_BASESHOOT_RESTART,	spitState		, "NORMAL"      );      // ->swap to first one

		getStateMachine()->getAnimState( STATE_SPIT_TO_STAND )->setEndCheckByAnimEvent( btrue );
        getStateMachine()->getAnimState( STATE_SPIT_BASESHOOT )->setEndCheckByAnimEvent( bfalse );          // don't check anim's end because this anim is in add with other adds...
        getStateMachine()->getAnimState( STATE_SPIT_BASESHOOT )->setDefaultNextStateID( STATE_NORMAL );
        getStateMachine()->getAnimState( STATE_SPIT_BASESHOOT )->setRestartAnimIfSame( bfalse );
        getStateMachine()->getAnimState( STATE_SPIT_BASESHOOT_RESTART )->setRestartAnimIfSame( bfalse );
        getStateMachine()->getAnimState( STATE_SPIT_BASESHOOT_RESTART )->setEndCheckByAnimEvent( bfalse );

		// ATTACK
		//********
		ADD_ANIM_STATE		( STATE_ATTACK_START,		attackState		, "PUNCH"	);
        ADD_ANIM_STATE		( STATE_ATTACK_COMBO,		attackState		, "PUNCH_COMBO"	);
        getStateMachine()->getAnimState( STATE_ATTACK_START )->setDefaultNextStateID( STATE_NORMAL );
        getStateMachine()->getAnimState( STATE_ATTACK_COMBO )->setDefaultNextStateID( STATE_NORMAL );
        
		// HIT
		//******
		ADD_ANIM_STATE		( STATE_HIT,				hitState		, "HIT"	    );
        ADD_ANIM_STATE		( STATE_DEATH,				hitState		, "DEATH"	);
        ADD_ANIM_STATE		( STATE_DEATH_BUBBLE,		hitState		, "BUBBLE_DEATH" );
		getStateMachine()->getAnimState( STATE_HIT )->setDefaultNextStateID( STATE_NORMAL );
        getStateMachine()->getAnimState( STATE_HIT )->setEndCheckByAnimEvent();

        // HIT
		//******
		ADD_ANIM_STATE		( STATE_BUBBLE,				bubbleState		, "BUBBLE"	);
        ADD_ANIM_STATE		( STATE_BUBBLE_EXIT,		bubbleState		, "BUBBLE_EXIT"	);

        //************
        // SEQUENCES
        //************
        ADD_ANIM_STATE		( STATE_SEQUENCE_LAND_TRAVEL,       sequencesState, "NORMAL" );
        ADD_ANIM_STATE		( STATE_SEQUENCE_LAND_EXIT,         sequencesState, "NORMAL" );
        getStateMachine()->getAnimState( STATE_SEQUENCE_LAND_TRAVEL )->setDefaultNextStateID( STATE_SEQUENCE_LAND_EXIT );
        getStateMachine()->getAnimState( STATE_SEQUENCE_LAND_TRAVEL )->setEndCheckByAnimEnd( bfalse );
        getStateMachine()->getAnimState( STATE_SEQUENCE_LAND_EXIT )->setDefaultNextStateID( STATE_NORMAL );

        //m_vacuumPatchTexture.invalidateResourceId();
    }

    Ray_ShooterControllerComponent::~Ray_ShooterControllerComponent()
    {
        EVENTMANAGER->unregisterEvent(ITF_GET_STRINGID_CRC(EventPlayerActivationChanged,1593682208), this);
        EVENTMANAGER->unregisterEvent(ITF_GET_STRINGID_CRC(EventPlayerHpChanged,1555193400), this);
        EVENTMANAGER->unregisterEvent(ITF_GET_STRINGID_CRC(EventCheckpointReached,2346783043), this);
        EVENTMANAGER->unregisterEvent(ITF_GET_STRINGID_CRC(EventRevertToLastCheckpoint,3654122082), this);
        EVENTMANAGER->unregisterEvent(ITF_GET_STRINGID_CRC(Ray_EventShooterCamera_TransitionNotify,501414184), this);
    }


    //----------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterControllerComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        EVENTMANAGER_REGISTER_EVENT_LISTENER(ITF_GET_STRINGID_CRC(EventPlayerActivationChanged,1593682208),this);
        EVENTMANAGER_REGISTER_EVENT_LISTENER(ITF_GET_STRINGID_CRC(EventPlayerHpChanged,1555193400),this);
        EVENTMANAGER_REGISTER_EVENT_LISTENER(ITF_GET_STRINGID_CRC(EventCheckpointReached,2346783043),this);
        EVENTMANAGER_REGISTER_EVENT_LISTENER(ITF_GET_STRINGID_CRC(EventRevertToLastCheckpoint,3654122082),this);
        EVENTMANAGER_REGISTER_EVENT_LISTENER(ITF_GET_STRINGID_CRC(Ray_EventShooterCamera_TransitionNotify,501414184),this);

        Super::onActorLoaded(_hotReload);
        
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventQueryIsDead,2482198443), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTeleport,1075308886), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(PunchStim,200533519), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(SolidStim,3144684763), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(VacuumStim,593568508), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301),this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(AnimPolylineEvent,4083589773),this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventShooterVacuumed,1513162250),this);
		ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventPlayerModeChanged,1336647275), this);
		ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventAnimChanged,153009624), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventSequenceActorActivate,1119571699), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventPowerUp,875814764), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventPoolActorRecycle,1859888857), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventShooterLandingRequested,492783699), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventBounceToLayer,3529219527), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventShooterSwallow,48834168), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventShooterVacuumReleased,1672774138), this);
        
		m_vacuumSkill.initialize( m_actor, m_animComponent, &getTemplate()->getVacuumSkill() );

        // init heart shield
        m_heartShield.init(m_actor,m_playerIndex, &getTemplate()->getHeartShield());
        m_heartShield.onActorLoaded(_hotReload);

        m_swarmRepellerPowerUp.init(m_actor,m_playerIndex, &getTemplate()->getSwarmRepellerPowerUp());
        m_swarmRepellerPowerUp.onActorLoaded(_hotReload);

        // init base bullet pool
        m_baseBulletPool.initialize( m_actor, 10, getTemplate()->getBaseSpawnBulletPath(), btrue, btrue );

        // death FX generator
        if ( !getTemplate()->getDeathSpawnFXPath().isEmpty() )
        {
            SPAWNER->declareNeedsSpawnee( m_actor, &m_deathFXGenerator, getTemplate()->getDeathSpawnFXPath() );
        }        

        // init state machine
        m_stateMachine.initialize( this, m_actor->GetComponent<AnimatedComponent>() );
        CHANGE_STATE( STATE_WAIT_PLAYER );

        // Init vacuum patch
        /*if ( !getTemplate()->getVacuumPatchTextureFile().isEmpty() )
        {
            m_vacuumPatchTexture = m_actor->addResource(Resource::ResourceType_Texture, getTemplate()->getVacuumPatchTextureFile().getString());
        }*/
    }
    //-----------------------------------------------------------------------------------------------------------------
    void Ray_ShooterControllerComponent::onBecomeActive()
    {
        Super::onBecomeActive();

        if ( !m_listenerAdded )
        {
            addListener();
        }

        // restore power ups displays
        m_heartShield.onBecomeActive();
        m_swarmRepellerPowerUp.onBecomeActive();
    }
    //-----------------------------------------------------------------------------------------------------------------
    void Ray_ShooterControllerComponent::onBecomeInactive()
    {
        Super::onBecomeInactive();

        if ( m_listenerAdded )
        {
            removeListener();
        }

        // kill power ups displays
        m_heartShield.onBecomeInactive();
        m_swarmRepellerPowerUp.onBecomeInactive();
    }
    //-----------------------------------------------------------------------------------------------------------------
    void Ray_ShooterControllerComponent::onActorClearComponents()
    {
        Super::onActorClearComponents();

        if ( m_listenerAdded )
        {
            removeListener();
        }
    }
    //-----------------------------------------------------------------------------------------------------------------
    void Ray_ShooterControllerComponent::addListener()
    {
        GAMEMANAGER->addGamePlayInputListener(this);
        m_listenerAdded = btrue;
    }
    //-----------------------------------------------------------------------------------------------------------------
    void Ray_ShooterControllerComponent::removeListener()
    {
        GAMEMANAGER->removeInputListener(this);
        m_listenerAdded = bfalse;
    }
    //-----------------------------------------------------------------------------------------------------------------
    void Ray_ShooterControllerComponent::registerDatas()
    {
        // not register in ai manager if not in shooter mode
        if ( m_playerIndex != U32_INVALID && isPlayerInShooterMode() )
        {
            Super::registerDatas();
        }
    }

    //-----------------------------------------------------------------------------------------------------------------
    void Ray_ShooterControllerComponent::initialize()
    {
        m_vaccumButtonPressedDuration   = 0.0f;
        m_shootButtonPressedDuration    = 0.0f;

        m_isDead                        = bfalse;
        m_gameModeJustChanged           = bfalse;
        m_isDeadWaitRespawnTimer        = 0.0f;
        m_isVacuumedTimer               = 0.0f;
        m_isUnderVacuumInfluence        = bfalse;
        m_isUnderVacuumInfluenceForce   = 0.0f;
        m_hasBeenVacuumed               = bfalse;
        m_isUnderVacuumInfluenceOwner.invalidate();
        
        m_isInWater                     = bfalse;
        m_downInWater                   = bfalse;
        m_waterSNDType                  = None;
        m_fluidContactEdgeList.clear();
        
        m_hurtTimer                     = 0.0f;//getTemplate()->getHurtImmunityDuration(); // immune for a while
        m_hurtImmunityProcessHitAction  = bfalse;
        m_invincibleAgainstActor.invalidate();
        m_invincibleTimer               = 0.0f;
        m_outofScreenTimer              = 0.0f;
        m_lastHitFrame                  = U32_INVALID;

        m_vacuumRestartTimer            = 0.0f;

        m_moveInput                     = Vec2d::Zero;
        m_moveVector                    = Vec2d::Zero;
        m_stateWantedMoveVector         = Vec2d::Zero;
        m_lastValidInputDir             = Vec2d::Zero;
        m_stateWantedMove               = bfalse;
        m_stateWantedLocalSpeedMax      = 0.0f;
        m_stateWantedMoveInput          = Vec2d::Zero;
        m_moveVaccumInertiaTimer        = 0.0f;
        m_moveSpeed                     = 0.0f;

        m_capacityFlags                 = 0;
        m_temp_AnimFinishedCount        = 0;   

        m_animMoveBlendCursor           = 0.5f;
        m_animMoveXCursor               = 0.5f;  
        m_animMoveYCursor               = 0.5f; 

        m_curMode                       = normal;

        m_camTransMode_ChangeDepthDest  = 0.f;
        m_camTransMode_ChangeDepthStartValue = 0.0f;

        m_vacuumSkill.removeVacuum();
        m_vacuumSkill.reset();
        if ( m_characterPhys != NULL )
        {
            m_characterPhys->setStickingDisabled( btrue );
        }        

        getStateMachine()->changeStateRestartAnim( STATE_NORMAL );
        
        // reset heart shield
        m_heartShield.reset();
        //m_swarmRepellerPowerUp.reset();

        setCurrentWaterPolyline( NULL );
        setCurrentFluidContactPolyline( NULL );
    }

	//-----------------------------------------------------------------------------------------------------------------
    void Ray_ShooterControllerComponent::onEvent( Event* _event )
    {
        Super::onEvent(_event);

        if ( EventPlayerActivationChanged* eventActivation = _event->DynamicCast<EventPlayerActivationChanged>(ITF_GET_STRINGID_CRC(EventPlayerActivationChanged,1593682208)) )
        {
            processPlayerActivationChanged(eventActivation);
        }
        else if ( EventPlayerHpChanged* eventHP = _event->DynamicCast<EventPlayerHpChanged>(ITF_GET_STRINGID_CRC(EventPlayerHpChanged,1555193400)) )
        {
            processPlayerHpChanged(eventHP);
        }
        else if ( EventCheckpointReached* eventCheckpoint = _event->DynamicCast<EventCheckpointReached>(ITF_GET_STRINGID_CRC(EventCheckpointReached,2346783043)) )
        {
            processCheckpointReached(eventCheckpoint);
        }
        else if ( EventRevertToLastCheckpoint* eventRevert = _event->DynamicCast<EventRevertToLastCheckpoint>(ITF_GET_STRINGID_CRC(EventRevertToLastCheckpoint,3654122082)) )
        {
            processRevertToLastCheckpoint(eventRevert);
        }
        /*else if ( EventTeleport* teleport = _event->DynamicCast<EventTeleport>(ITF_GET_STRINGID_CRC(EventTeleport,1075308886)) )            //=> is done by the base class
        {
            processTeleport(teleport);
        }*/
        else if ( EventPlayerModeChanged* eventGameMode = _event->DynamicCast<EventPlayerModeChanged>(ITF_GET_STRINGID_CRC(EventPlayerModeChanged,1336647275)) )
        {
            processGameModeChanged(eventGameMode);
        }
        else if ( PunchStim* hit = _event->DynamicCast<PunchStim>(ITF_GET_STRINGID_CRC(PunchStim,200533519)) )
        {
            processHitStim(hit);
        }
        else if ( SolidStim* solid = _event->DynamicCast<SolidStim>(ITF_GET_STRINGID_CRC(SolidStim,3144684763)) )
        {
            processSolidStim(solid);
        }
        else if ( VacuumStim* vacuumed = _event->DynamicCast<VacuumStim>(ITF_GET_STRINGID_CRC(VacuumStim,593568508)) )
        {
            processVacuumStim(vacuumed);
        }
        else if ( BounceStim* bounce = _event->DynamicCast<BounceStim>(ITF_GET_STRINGID_CRC(BounceStim,819805069)) )
        {
            processBounceStim(bounce);        
        }        
        else if ( EventQueryIsDead* queryDead = _event->DynamicCast<EventQueryIsDead>(ITF_GET_STRINGID_CRC(EventQueryIsDead,2482198443)) )
        {
            queryDead->setIsDead( m_isDead );
        }
        else if (AnimPolylineEvent * polyEvent = _event->DynamicCast<AnimPolylineEvent>(ITF_GET_STRINGID_CRC(AnimPolylineEvent,4083589773)))
        {
			onAnimPolylineEvent( polyEvent );
        }
        else if (Ray_EventShooterVacuumed * onVacuumed = _event->DynamicCast<Ray_EventShooterVacuumed>(ITF_GET_STRINGID_CRC(Ray_EventShooterVacuumed,1513162250)))
        {
            if ( getStateMachine()->isCurStateInGroupRange( STATE_GROUP_VACCUM ) )
            {
                m_vacuumSkill.processVacuumed(onVacuumed);
            }
        }
		else if (AnimGameplayEvent * animEvent = _event->DynamicCast<AnimGameplayEvent>(ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301)))
		{
			onAnimEvent(animEvent);
		}
		else if (EventAnimChanged * animChanged = _event->DynamicCast<EventAnimChanged>(ITF_GET_STRINGID_CRC(EventAnimChanged,153009624)))
		{
			m_temp_AnimFinishedCount = 0;
		}
        else if (Ray_EventShooterCamera_TransitionNotify * changeDepth = _event->DynamicCast<Ray_EventShooterCamera_TransitionNotify>(ITF_GET_STRINGID_CRC(Ray_EventShooterCamera_TransitionNotify,501414184)))
        {
            if ( m_actor->isActive() )
            {
                onCameraEvent( _event );
            }
        }
        else if (EventSequenceActorActivate * stopevt = _event->DynamicCast<EventSequenceActorActivate>(ITF_GET_STRINGID_CRC(EventSequenceActorActivate,1119571699)))
        {
            processStopPlayer( stopevt->getActivate() );
        }
        else if (Ray_EventPowerUp* powerUp = _event->DynamicCast<Ray_EventPowerUp>(ITF_GET_STRINGID_CRC(Ray_EventPowerUp,875814764)))
        {
            processPowerUp(powerUp);
        }
        else if (EventPoolActorRecycle* recyclePoolActor = _event->DynamicCast<EventPoolActorRecycle>(ITF_GET_STRINGID_CRC(EventPoolActorRecycle,1859888857)))
        {
            m_baseBulletPool.onEvent( recyclePoolActor );
        }
        else if (Ray_EventShooterLandingRequested* landingRequested = _event->DynamicCast<Ray_EventShooterLandingRequested>(ITF_GET_STRINGID_CRC(Ray_EventShooterLandingRequested,492783699)))
        {
            processLandingRequested( landingRequested );
        }
        else if (Ray_EventBounceToLayer* bounceToLayer = _event->DynamicCast<Ray_EventBounceToLayer>(ITF_GET_STRINGID_CRC(Ray_EventBounceToLayer,3529219527)))
        {
            processBounceToLayer( bounceToLayer );
        }
        else if ( Ray_EventShooterSwallow* swallowed = _event->DynamicCast<Ray_EventShooterSwallow>(ITF_GET_STRINGID_CRC(Ray_EventShooterSwallow,48834168)) )
        {
            if ( !m_isDead )
            {
                die();
            }
        }
        else if ( Ray_EventShooterVacuumReleased* released = _event->DynamicCast<Ray_EventShooterVacuumReleased>(ITF_GET_STRINGID_CRC(Ray_EventShooterVacuumReleased,1672774138)) )
        {
            m_hasBeenVacuumed = bfalse;
            m_isUnderVacuumInfluence = bfalse;
            m_isVacuumedTimer = 0.0;
        }
		
        // Call state machine onevent
        getStateMachine()->onEvent( _event );
        
        // forward event to heart shield
        m_heartShield.onEvent(_event);
        m_swarmRepellerPowerUp.onEvent(_event);
    }

    bbool Ray_ShooterControllerComponent::isDetectable( const ActorRef _detector ) const
    {
        return !isPlayerDisable() && !m_isDead &&
            !getStateMachine()->isCurStateInGroupRange(STATE_GROUP_BUBBLE) &&
            !IS_CURSTATE( STATE_SEQUENCE_LAND_TRAVEL );
    }


	//-----------------------------------------------------------------------------------------------------------------
	// YELLOW event...
	void Ray_ShooterControllerComponent::onAnimEvent( AnimGameplayEvent * _animEvent )
	{
		ITF_ASSERT( _animEvent != NULL );
		if ( _animEvent == NULL )
		{
			return;
		}
		// Shoot event
		//--------------
		if ( _animEvent->getName() == MRK_Shoot )
		{
			if ( IS_CURSTATE( STATE_SPIT_START ) )
			{
				CHANGE_STATE( STATE_SPIT_END );
			}
            else
            {
                spit();
            }
		}
		else if ( _animEvent->getName() == MRK_Allow_Move )
		{
			setCapacityFlag( canMove );
            removeCapacityFlag( cannotMove );
		}
		else if ( _animEvent->getName() == MRK_Perform_Hit )
		{
			performHit();
		}
		else if ( _animEvent->getName() == MRK_Finish_Anim ) 
		{
			bbool apply = btrue;
			m_temp_AnimFinishedCount++;

			if ( IS_CURSTATE( STATE_VACCUM_FINISHED_FULL ) ) //|| IS_CURSTATE( STATE_NORMAL_CANTVACCUM ) )
			{
				if ( m_temp_AnimFinishedCount < 2 )
				{
					apply = bfalse;
				}
			}
			
			if ( apply )
			{
				getStateMachine()->setAnimFinishedFormEvent();
			}
		}
        else if ( _animEvent->getName() == MRK_ComboHit_Next )
        {
            setCapacityFlag( comboNext );
        }
	}
	//-----------------------------------------------------------------------------------------------------------------
	// BLEU event...
	void Ray_ShooterControllerComponent::onAnimPolylineEvent( AnimPolylineEvent * _animPolylineEvent )
	{
		ITF_ASSERT( _animPolylineEvent != NULL );
		if ( _animPolylineEvent == NULL )
		{
			return;
		}
		if (_animPolylineEvent->getName() == MRK_Perform_Hit_Start )
		{
			m_shape.setShape( _animPolylineEvent, m_animComponent, m_actor );
		}
	}

    //-----------------------------------------------------------------------------------------------------------------
    void Ray_ShooterControllerComponent::onCameraEvent( Event * _cameEvent )
    {
        if ( Ray_EventShooterCamera_TransitionNotify * camTransition = _cameEvent->DynamicCast<Ray_EventShooterCamera_TransitionNotify>(ITF_GET_STRINGID_CRC(Ray_EventShooterCamera_TransitionNotify,501414184)))
        {
            if ( canChangeMode( cameraTransition, camTransition->getDestDepth() ) )
            {
                // Check if the player is in the camera rec
                //
                if ( !m_actor->isVisible(CAMERA) )
                {
                    die();
                }
                else
                {
                    m_curMode = cameraTransition;
                    m_camTransMode_ChangeDepthStartValue    = m_actor->getDepth();
                    m_camTransMode_ChangeDepthDest          = camTransition->getDestDepth();
                    m_camTransMode_Transition.m_duration    = camTransition->getTransitionDuration();
                    m_camTransMode_Transition.m_type        = static_cast<ShooterCameraModifier_Transition::TransitionType>( camTransition->getTransitionType() );
                    m_camTransMode_Transition.start();
                    
                    if ( m_fxController != NULL )
                    {
                        u32 fxHandle = m_fxController->playFX( ITF_GET_STRINGID_CRC(ChangingZ,786434816) );
                        if ( fxHandle != U32_INVALID )
                        {
                            m_fxController->setFXPos( fxHandle, m_actor->getPos() );
                        }
                    }
                    //CHANGE_STATE( STATE_SEQUENCE_CHANGEDEPTH );
                }                
            }
        }
    }

	//-----------------------------------------------------------------------------------------------------------------
	void Ray_ShooterControllerComponent::performHit()
	{
		bbool flipped = m_actor->isFlipped();
		Vec2d dir = Vec2d::Right.Rotate(m_actor->getAngle());

		PunchStim* stim = STIMSMANAGER->requestStim<PunchStim>( m_shape.getShape() );

		if(stim)
		{
			stim->setDepth(m_actor->getDepth());
			stim->setSender(m_actor->getRef());
			stim->setLevel(0);
			stim->setReceivedHitType(RECEIVEDHITTYPE_FRONTPUNCH);
			stim->setAngle(m_actor->getAngle());
            stim->setFaction(getFaction());

			if ( flipped )
			{
				dir *= -1.f;
			}

			Vec2d shapePos = m_actor->get2DPos();
			stim->setPos(shapePos);
			stim->setPrevPos(shapePos);
			stim->setDirection(dir);
			stim->setFxPos(shapePos.to3d(m_actor->getPos().m_z));

			//AIUtils::drawDBGPhysShape(m_actor->get2DPos() + offset,m_actor->getDepth(),m_actor->getAngle(),m_shape,Color::white(),1);

			STIMSMANAGER->sendStim(stim);
		}
	}

    //-----------------------------------------------------------------------------------------------------------------
    void Ray_ShooterControllerComponent::setImmuneTime( f32 _duration, bbool _processHitAction /*=bfalse*/ ) 
    {
        m_hurtTimer = _duration;
        m_hurtImmunityProcessHitAction = _processHitAction;
    }
	
	//-----------------------------------------------------------------------------------------------------------------
	bbool Ray_ShooterControllerComponent::isImmune() 
	{ 
        if ( static_cast<Ray_Player*>( RAY_GAMEMANAGER->getPlayer(m_playerIndex) )->isDead() )
        {
            return btrue;
        }
		return ( m_hurtTimer > 0.0f ); 
	}

    //-----------------------------------------------------------------------------------------------------------------
    void Ray_ShooterControllerComponent::processPlayerActivationChanged( EventPlayerActivationChanged* _eventActivation )
    {
        Ray_Player* player = static_cast<Ray_Player*>( _eventActivation->getPlayer() );
        if ( player->getIndex() == m_playerIndex )
        {
            m_justActivatedDead = bfalse;
            if ( player->getActive() )
            {
                if ( isPlayerInShooterMode() && _eventActivation->getStartDead() && RAY_GAMEMANAGER->getShooterGameModeController()->isActive() )
                {
                    m_justActivatedDead = btrue;
                    revive();
                    if ( !m_gameModeJustChanged )
                    {
                        initialize();
                    }                    
                    die();
                }
            }
        }
    }

    //-----------------------------------------------------------------------------------------------------------------
    void Ray_ShooterControllerComponent::processStopPlayer( bbool _stop )
    {
        disablePlayer( _stop ) ;
    }
    //-----------------------------------------------------------------------------------------------------------------
    void Ray_ShooterControllerComponent::disablePlayer( bbool _disable )
    {
        m_playerDisabled = _disable;
        m_characterPhys->resetForces();
        m_characterPhys->setSpeed(Vec2d::Zero);
        m_characterPhys->setDisabled(_disable,bfalse,btrue);
    }

    //-----------------------------------------------------------------------------------------------------------------
    bbool Ray_ShooterControllerComponent::isPlayerInShooterMode() const
    { 
        Player * player = GAMEMANAGER->getPlayer( m_playerIndex );
        if ( player != NULL )
        {
            return ( ( player->getCurrentMode() == RAY_GAMEMODE_SHOOTER ) ? btrue : bfalse ); 
        }
        return bfalse;
    }

    //-----------------------------------------------------------------------------------------------------------------
    void Ray_ShooterControllerComponent::processCheckpointReached( EventCheckpointReached* _eventCheckpoint )
    {
       
    }

    //-----------------------------------------------------------------------------------------------------------------
    void Ray_ShooterControllerComponent::processRevertToLastCheckpoint( EventRevertToLastCheckpoint* _eventRevert )
    {
        StateHit * stateDeath = static_cast<StateHit*>( getStateMachine()->getState( STATE_DEATH_BUBBLE )->getImplement() );
        if ( stateDeath != NULL )
        {
            stateDeath->destroyFX();
        }

        // IS a spawned component... will be destroyed then disable myself
       // m_actor->disable();
        return;
    }
   
    //-----------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterControllerComponent::processTeleport( EventTeleport* _eventTeleport )
    {
        m_characterPhys->setDisabled(bfalse);
        m_characterPhys->resetForces();
        m_characterPhys->setSpeed(Vec2d::Zero);

        if ( _eventTeleport->getReset() )
        {
            if ( !m_justActivatedDead && !m_gameModeJustChanged )
            {
                if ( m_isDead && !isPlayerDisable() )
                {
                    revive();
                }
                initialize();

                if ( !isPlayerInShooterMode() )
                {
                    CHANGE_STATE( STATE_WAIT_PLAYER );
                }
            }
        }
    
        m_actor->setWorldInitialPos( _eventTeleport->getPos() ); 
        m_actor->setWorldInitialRot( _eventTeleport->getAngle() );
        m_actor->setPos( _eventTeleport->getPos() );

        m_justActivatedDead = bfalse;
        m_gameModeJustChanged = bfalse;

        Super::processTeleport( _eventTeleport );
    }

    //-----------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterControllerComponent::processGameModeChanged( EventPlayerModeChanged* _eventGameMode )
    {
        Ray_Player* player = static_cast<Ray_Player*>(GAMEMANAGER->getPlayerFromActor(_eventGameMode->getActivator()));

        if ( player )
        {
            switch ( _eventGameMode->getMode() )
            {
            case RAY_GAMEMODE_SHOOTER:
                {
                    initialize();

                    if ( player->isDead() )
                    {
                        die();
                    }                    
                    else
                    {
                        CHANGE_STATE( STATE_NORMAL_RIDE_ON );
                    }
                    m_heartShield.show( btrue );
                    //m_swarmRepellerPowerUp.show( btrue );

                    // check camera transition
                    //---------------------------
                    if ( SHOOTERCAMERA != NULL )
                    {
                        f32 destDepth;
                        if ( SHOOTERCAMERA->isInTransition( destDepth ) )
                        {
                            if ( canChangeMode( cameraTransition, destDepth ) )
                            {
                                m_curMode = cameraTransition;
                                SHOOTERCAMERA->getCurTransitionInfo( &m_camTransMode_Transition, destDepth );
                                
                                m_camTransMode_Transition.start( m_camTransMode_Transition.isStarted() );
                                
                                m_camTransMode_ChangeDepthStartValue    = m_actor->getDepth();
                                m_camTransMode_ChangeDepthDest          = destDepth;
                            }
                        }
                        //has no the same depth as the camera, update it
                        if ( ( m_curMode != cameraTransition  ) && ( f32_Abs( destDepth - m_actor->getDepth() ) > 0.1f ) ) 
                        {
                            m_actor->setDepth( destDepth );
                        }
                    }
                    // register in AI manager
                    registerDatas();

                    m_gameModeJustChanged = btrue;
                }
                break;

            case RAY_GAMEMODE_PLATFORMER:
                {
                    // stop
                    //changeState(&m_stateIdle);
                    initialize();
                    m_characterPhys->setSpeed(Vec2d::Zero);
                    m_heartShield.show( bfalse );
                    //m_swarmRepellerPowerUp.show( bfalse );
                    CHANGE_STATE( STATE_WAIT_PLAYER );

                    // remove data from AI manager
                    unregisterDatas();
                }
                break;
            }
        }
    }

    void Ray_ShooterControllerComponent::processPlayerHpChanged( EventPlayerHpChanged* _eventHP )
    {
        if ( _eventHP->getPlayer()->getIndex() == m_playerIndex )
        {
            //if ( _eventHP->getJustRevived() )
            {
                //changeState(&m_stateNormal);
            }
        }
    }

    void Ray_ShooterControllerComponent::processPowerUp(const Ray_EventPowerUp* _powerUp)
    {
        // enable/disable power ups through events
        // NB[LaurentCou]: same as Ray_PlayerControllerComponent::processPowerUp
        Ray_PowerUpManager& powerUpManager = RAY_GAMEMANAGER->getPowerUpManager();
        powerUpManager.processPowerUp(_powerUp, m_playerIndex);
    }

    void Ray_ShooterControllerComponent::processPlayerIndexChange( EventPlayerIndexChange* _playerIndex )
    {
        if ( _playerIndex->getNewIndex() != m_playerIndex )
        {
            Super::processPlayerIndexChange(_playerIndex);

            m_heartShield.setPlayerIndex(m_playerIndex);
            m_swarmRepellerPowerUp.setPlayerIndex(m_playerIndex);
        }
        else
        {
            Super::processPlayerIndexChange(_playerIndex);
        }
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //
    void Ray_ShooterControllerComponent::processLandingRequested( class Ray_EventShooterLandingRequested* _landingRequested )
    {
        if ( isPlayerInShooterMode() && !IS_CURSTATE( STATE_SEQUENCE_LAND_TRAVEL ) && !IS_CURSTATE( STATE_SEQUENCE_LAND_EXIT ) )
        {
            CHANGE_STATE( STATE_SEQUENCE_LAND_TRAVEL );
        }
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //
    void Ray_ShooterControllerComponent::processBounceToLayer( class Ray_EventBounceToLayer * _bounceToLayer )
    {
        // Receive Hit
        //
        if ( _bounceToLayer->getHurt() )
        {
            receiveDamage( 1 );
        }
        // the event is forwarded to the "hit state"
    }

    //-----------------------------------------------------------------------------------------------------------------------------
    //
    void Ray_ShooterControllerComponent::Update(f32 _deltaTime)
    {
        Super::Update(_deltaTime);

        if ( !isPlayerDisable() )
        {
            checkWaterRegion();
            
            updateModes( _deltaTime );

            updateStates( _deltaTime );
        }

        // player not in shooter mode
        if ( !isPlayerInShooterMode() )
        {
            m_characterPhys->resetForces();
            m_characterPhys->setAirFrictionMultiplier( 100.0f );
        }
        else if ( !isPlayerDisable() )
        {
            // collision are checked before all because phys comp is updated after the controller.
            if ( !getStateMachine()->isCurStateInGroupRange( STATE_GROUP_SEQUENCE_LAND ) )
            {
                processCollisions(_deltaTime);
            }

            updateMove( _deltaTime );

            updateClamp(_deltaTime);

            updateFluidStickToEdge();

            m_hurtTimer = Max( 0.0f, m_hurtTimer - _deltaTime );
            
            // update heart shield
            m_heartShield.update(_deltaTime);
            m_swarmRepellerPowerUp.update(_deltaTime);
        }

        //****************
        // Vaccum Skill
        //****************
        Vec3d swallowPos;
        getSwallowPos( swallowPos );
		m_vacuumSkill.updateSpecificProjectiles( swallowPos );

        // ANIMS
        //********
        updateAnimBlend( _deltaTime );
        updateAnimInput();

        // update power-ups lifetime
        RAY_GAMEMANAGER->getPowerUpManager().update(_deltaTime);

        m_isUnderVacuumInfluence = bfalse;
        m_gameModeJustChanged    = bfalse;

        cleanListener( ); 
    }

    //---------------------------------------------------------------------------------------------------------------------------------
    //
    void Ray_ShooterControllerComponent::updateModes( f32 _deltaTime )
    {
        switch( m_curMode )
        {
            case cameraTransition :
            {
                if ( !m_camTransMode_Transition.isFinished() )
                {
                    m_camTransMode_Transition.update(_deltaTime);
                    f32 depth = Interpolate( m_camTransMode_ChangeDepthStartValue, m_camTransMode_ChangeDepthDest, m_camTransMode_Transition.getBlendCursor() );
                    m_actor->setDepth( depth );
                    if ( m_camTransMode_Transition.isFinished() )
                    {
                        m_curMode = normal;
                    }                
                }
                break;
            }
        }
    }

    //---------------------------------------------------------------------------------------------------------------------------------
    //
    bbool Ray_ShooterControllerComponent::canChangeMode( Shooter_Player_Modes _dest_mode, f32 _dest_Depth )
    {
        switch( _dest_mode )
        {
            case cameraTransition :
            {
                return ( f32_Abs( _dest_Depth - m_actor->getDepth() ) > 0.3f ) ? btrue : bfalse;
            }
        }
        return btrue;
    }

    //---------------------------------------------------------------------------------------------------------------------------------
    //
    void Ray_ShooterControllerComponent::checkWaterRegion()
    {
        PolyLine* curPolyLine = AIUtils::getPolylineRegion( m_actor->getDepth(),s_RegionName_Swim, m_actor->get2DPos() );
        bbool inWater = ( curPolyLine != NULL ) ? btrue : bfalse;
        m_waterSNDType = None;

        if ( m_isInWater && !inWater )
        {
           setCurrentWaterPolyline( NULL );
           
           m_waterSNDType = DiveOut;
        } 
        else if ( inWater && !m_isInWater )
        {
           setCurrentWaterPolyline( curPolyLine );
           
           m_waterSNDType = DiveIn;
        } 

        m_isInWater = inWater;

        bbool checkResistance = bfalse;
        Vec2d resistanceNormal = Vec2d::Zero;
        f32 waterCheckRadius = getTemplate()->getWaterCheckRadius() * m_actor->getScale().m_x;

        // check down actor in water ?
        //*******************************
        if ( !m_isInWater )
        {
            Vec2d downPosCheck = m_actor->get2DPos() + ( -Vec2d::Up * waterCheckRadius );

            PolyLine* downWaterPolyLine = AIUtils::getPolylineRegion( m_actor->getDepth(),s_RegionName_Swim, downPosCheck );

            if ( downWaterPolyLine != NULL )
            {
                checkResistance = btrue;
                resistanceNormal = Vec2d::Up;
                
                // add the water polyline
                if ( !m_downInWater )
                {
                    setCurrentWaterPolyline( downWaterPolyLine );
                }
                m_downInWater = btrue;

                m_waterSNDType = ( m_waterSNDType == None ) ? Surf : m_waterSNDType;
            }
            else if( m_downInWater )
            {
                setCurrentWaterPolyline( NULL );
                m_downInWater = bfalse;
            }
        }
        else
        {
            checkResistance = btrue;
            resistanceNormal = -Vec2d::Up;
            m_downInWater = bfalse;
        }

        // check resistance ?
        //********************
        if ( checkResistance )
        {
            PolyLine* waterPolyLine = AIUtils::getPolyLine(m_waterPolyline);

            if ( waterPolyLine != NULL )
            {
                // check if the water check radius cross water's surface
                Vec2d waterSurface = Vec2d::Right;  
                Vec2d waterPos = waterPolyLine->getAABB().getMax();
                Vec2d intersection;
                
                if ( Line_Line( m_actor->get2DPos(), Vec2d::Up, waterPos, waterSurface, intersection ) )
                {
#ifndef ITF_FINAL
                    /*DebugDraw::circle(intersection, 0.0f, 0.1f, Color::green() );
                    DebugDraw::line2D( m_actor->get2DPos() + ( Vec2d::Up * waterCheckRadius ), m_actor->get2DPos() - ( Vec2d::Up * waterCheckRadius ), 0.0f, Color::black() );*/
#endif //ITF_FINAL

                    f32 distToColl = ( intersection - m_actor->get2DPos() ).norm();
                    if (  distToColl < waterCheckRadius )
                    {
                        m_characterPhys->addForce( resistanceNormal * ( 1.0f - ( distToColl / waterCheckRadius ) ) * getTemplate()->getWaterResistanceForce() );

                        m_waterSNDType = ( ( m_waterSNDType == None ) && !m_downInWater )? SurfUnder : m_waterSNDType;
                    }
                }
            }
        }

#ifndef ITF_FINAL
     /*   PolyLine* waterPolyLine = AIUtils::getPolyLine(m_waterPolyline);
        if ( ( waterPolyLine != NULL ) && ( waterPolyLine->m_ownerFrise != NULL ) )
        {
            GFX_ADAPTER->drawDBGAABB( waterPolyLine->getAABB(), Color::yellow() );
        }*/
#endif //ITF_FINAL

        //******************
        // Play water FXs
        //******************
        if ( m_fxController != NULL )
        {
            const StringID s_diveIn      = ITF_GET_STRINGID_CRC(DiveIn,3320800552);
            const StringID s_diveOut     = ITF_GET_STRINGID_CRC(DiveOut,3488328555); 
            const StringID s_surf        = ITF_GET_STRINGID_CRC(Surf,4252188527); 
            const StringID s_inWater     = ITF_GET_STRINGID_CRC(InWater,3332205583); 
            const StringID * fxName = NULL;

            switch ( m_waterSNDType )
            {
                case DiveIn     : fxName = &s_diveIn;  break;
                case DiveOut    : fxName = &s_diveOut;  break;
            }
            
            u32 fxHandle = U32_INVALID;
            if ( fxName != NULL )
            {
                fxHandle = m_fxController->playFX( *fxName );
                m_fxController->setFXPos( fxHandle, m_actor->getPos() );
            }
            
            // surf sound 
            //-------------
            if ( ( ( m_waterSNDType == Surf ) || ( m_waterSNDType == SurfUnder ) ) && ( m_waterSndSurfHandle == U32_INVALID ) )
            {
                m_waterSndSurfHandle = m_fxController->playFX( s_surf );
            }
            else if ( ( m_waterSNDType == None ) && ( m_waterSndSurfHandle != U32_INVALID ) ) 
            {
                m_fxController->stopFX( m_waterSndSurfHandle );
                m_waterSndSurfHandle = U32_INVALID;
            }

            // global sound
            //---------------
            if ( m_isInWater && ( m_waterSndGlobalHandle == U32_INVALID ) )
            {
                m_waterSndGlobalHandle = m_fxController->playFX( s_inWater );
            }
            else if ( !m_isInWater && ( m_waterSndGlobalHandle != U32_INVALID ) )
            {
                m_fxController->stopFX( m_waterSndGlobalHandle );
                m_waterSndGlobalHandle = U32_INVALID;
            }

            if ( m_waterSndGlobalHandle != U32_INVALID )
                m_fxController->setFXPos( m_waterSndGlobalHandle, m_actor->getPos() );
            if ( m_waterSndSurfHandle != U32_INVALID )
                m_fxController->setFXPos( m_waterSndSurfHandle, m_actor->getPos() );
        }
    }

    //---------------------------------------------------------------------------------------------------------------------------------
    //
    void Ray_ShooterControllerComponent::setCurrentWaterPolyline( const class PolyLine * _polyline )
    {
        // same poly
        if ( ( m_characterPhys == NULL ) || ( ( _polyline != NULL ) && ( _polyline->getRef() == m_waterPolyline ) ) )
        {
            return;
        }

        PolyLine* prevPolyLine = AIUtils::getPolyLine(m_waterPolyline);

        if (prevPolyLine && prevPolyLine->m_ownerFrise)
        {
            EventSwiming swiming(m_actor->getRef(), bfalse, m_characterPhys->getSpeed() + m_characterPhys->getForce()*LOGICDT, 0.f, m_characterPhys->getWaterMultiplier(), m_characterPhys->getWeight(),bfalse);
            prevPolyLine->m_ownerFrise->onEvent(&swiming);
        }
        m_waterPolyline.invalidate();
        
        if ( _polyline != NULL )
        {
            if ( _polyline->m_ownerFrise )
            {
                EventSwiming swiming(m_actor->getRef(), btrue, m_characterPhys->getSpeed() + m_characterPhys->getForce()*LOGICDT, m_actor->getRadius(), m_characterPhys->getWaterMultiplier(), m_characterPhys->getWeight(),bfalse);
                _polyline->m_ownerFrise->onEvent(&swiming);
            }
            m_waterPolyline = _polyline->getRef();
        }
    }

    //---------------------------------------------------------------------------------------------------------------------------------
    //
    void Ray_ShooterControllerComponent::setCurrentFluidContactPolyline( const class PolyLine * _polyline )
    {
        // same poly
        if ( ( m_characterPhys == NULL ) || ( ( _polyline != NULL ) && ( _polyline->getRef() == m_fluidContactPolyline ) ) )
        {
            return;
        }

        PolyLine* prevPolyLine = AIUtils::getPolyLine(m_fluidContactPolyline);
        const f32 fluidContactWeightFactor = 1.0f / m_actor->getScale().m_y;

        if (prevPolyLine && prevPolyLine->m_ownerFrise)
        {
            EventSwiming swiming(m_actor->getRef(), bfalse, m_characterPhys->getSpeed() + m_characterPhys->getForce()*LOGICDT, 0.f, m_characterPhys->getWaterMultiplier(), m_characterPhys->getWeight(),bfalse);
            prevPolyLine->m_ownerFrise->onEvent(&swiming);
        }
        m_fluidContactPolyline.invalidate();

        if ( _polyline != NULL )
        {
            if ( _polyline->m_ownerFrise )
            {
                EventSwiming swiming(m_actor->getRef(), btrue, m_characterPhys->getSpeed() + m_characterPhys->getForce()*LOGICDT, m_actor->getRadius(), m_characterPhys->getWaterMultiplier(), m_characterPhys->getWeight() * fluidContactWeightFactor,bfalse);
                _polyline->m_ownerFrise->onEvent(&swiming);
            }
            m_fluidContactPolyline = _polyline->getRef();
        }

        // clear fluid contacts
        m_fluidContactEdgeList.clear();
    }

    //---------------------------------------------------------------------------------------------------------------------------------
    //
    void Ray_ShooterControllerComponent::updateFluidStickToEdge()
    {
        PolyLine* fluidPolyLine = AIUtils::getPolyLine(m_fluidContactPolyline);

        if ( ( m_fluidContactEdgeList.size() > 0 ) && ( fluidPolyLine != NULL ) )
        {
            Vec2d moveDir = m_moveVector.normalize();

            for ( u32 i = 0; i < m_fluidContactEdgeList.size(); ++i )
            {
                u32 edgeIdx = m_fluidContactEdgeList[i];

                ITF_ASSERT( edgeIdx < fluidPolyLine->getPosCount() );
                if ( edgeIdx < fluidPolyLine->getPosCount() )
                {
                    const PolyLineEdge& edge = fluidPolyLine->getEdgeAt( edgeIdx );
                    f32 projDistToPos = edge.m_normalizedVector.dot( m_actor->get2DPos() - edge.getPos() );
                    Vec2d contactPos = edge.getPos() + ( edge.m_normalizedVector * projDistToPos );

                    /*DebugDraw::circle( contactPos, 0.0f, 0.05f, Color::green() );
                    DebugDraw::line2D( edge.getPos(), edge.getPos() + ( edge.m_normalizedVector * edge.getLength() ), 0.0f, Color::red() );*/

                    f32 distToContact = ( contactPos - m_actor->get2DPos() ).norm();

                    Vec2d edgeToPos = distToContact > 0.0f ? ( ( m_actor->get2DPos() - contactPos ) / distToContact ) : Vec2d::Zero;

                    if (    ( edgeToPos.dot( moveDir ) > 0.7f )                             // not the same dir as wanted move dir
                        || ( distToContact > ( m_characterPhys->getRadius() + 0.1f ) ) 
                        || ( !m_moveVector.IsEqual( Vec2d::Zero, 0.01f ) && ( ( projDistToPos < -0.05f ) || ( projDistToPos > ( edge.getLength() + 0.05f ) ) ) )   // far to the edge
                        )       
                    {
                        m_fluidContactEdgeList.eraseKeepOrder( i );
                        i--;
                    }                       
                    else
                    {
                        m_actor->set2DPos( contactPos + ( edgeToPos * ( m_characterPhys->getRadius() + 0.0f ) ) );
                    }                
                }
            }

            if ( m_fluidContactEdgeList.size() == 0 )
            {
                setCurrentFluidContactPolyline( NULL );
            }
        }
    }

    //---------------------------------------------------------------------------------------------------------------------------------
    //
    void Ray_ShooterControllerComponent::updateStates( f32 _deltaTime )
    {
        // 'back' to disable the player
        //
        if ( ( m_playerIndex != GAMEMANAGER->getMainIndexPlayer() ) && ( m_listenerLeave ) 
            && !IS_CURSTATE( STATE_BUBBLE ) && !getStateMachine()->isCurStateInGroupRange( STATE_GROUP_SEQUENCE_LAND ) 
            && isPlayerInShooterMode() )
        {
            Ray_Player* player = static_cast<Ray_Player*>(GAMEMANAGER->getPlayer(m_playerIndex));
            ITF_ASSERT( player != NULL );
            if ( player != NULL )
            {
                player->addHitPoints(-10);
                die();
            }
        }
		
        // check hold durations
        //-----------------------
        m_vaccumButtonPressedDuration   = !m_listenerVacuumHold ? 0.0f : ( m_vaccumButtonPressedDuration + _deltaTime );
        m_shootButtonPressedDuration    = !m_listenerShootHold ? 0.0f : ( m_shootButtonPressedDuration + _deltaTime );
                
        // vaccum restart
        if ( !getStateMachine()->isCurStateInGroupRange( STATE_GROUP_VACCUM ) )
        {
            m_vacuumRestartTimer = Max( m_vacuumRestartTimer - _deltaTime, 0.0f );
        }

        //invincibility
        if (m_invincibleTimer == 0.0f)
        {
            m_invincibleAgainstActor.invalidate();
        }

        m_invincibleTimer = Max(0.0f,m_invincibleTimer-_deltaTime);

         // State machine update
        //**********************
        //getStateMachine()->setAxesAndButtons( axes, _buttons );
		getStateMachine()->update( _deltaTime );

    }
    
    //---------------------------------------------------------------------------------------------------------------------------------
    //
    void Ray_ShooterControllerComponent::updateMove( f32 _deltaTime )
    {
        ITF_ASSERT( SHOOTERCAMERA != NULL );

        Vec2d camVelocity = SHOOTERCAMERA ? SHOOTERCAMERA->getSpeed() : Vec2d::Zero;
        Vec2d currentVelocity = m_characterPhys->getSpeed();
        Vec2d targetVelocity = camVelocity;

        // Input check
        m_moveInput = m_listenerMove;

        if ( hasCapacityFlag( cannotMove ) )
        {
            m_moveInput.m_x = 0.0f;
            m_moveInput.m_y = 0.0f;
        }
        f32 inputForce = m_moveInput.norm();
        f32 maxLocalSpeed = getTemplate()->getMaxLocalSpeed();

        if ( !m_stateWantedMove )
        {
            f32 targetSpeed = Interpolate( getTemplate()->getMinLocalSpeed(), maxLocalSpeed, inputForce);
            f32 blendFactor = m_isInWater ? getTemplate()->getWaterMoveSpeedBlendFactor() : getTemplate()->getDefaultMoveSpeedBlendFactor();

		    if ( getVacuumSkill()->isFull() )
		    {
			    blendFactor = f32_Min( blendFactor, getTemplate()->getVaccumMoveSpeedBlendFactor() );
		    }
            else
            {
                // vaccum inertia
                //
                if (    getStateMachine()->isCurStateInGroupRange( STATE_GROUP_VACCUM ) &&
                        !getStateMachine()->isCurStateInGroupRange( STATE_GROUP_VACCUM_FINISHED ) //&&
                        //!IS_CURSTATE( STATE_VACCUM_START )
                   )
                {
                    blendFactor = f32_Min( blendFactor, getTemplate()->getVaccumMoveSpeedBlendFactor() );
                    m_moveVaccumInertiaTimer += _deltaTime;
                    m_moveVaccumInertiaTimer = Min( m_moveVaccumInertiaTimer, getTemplate()->getVacuumSkill().getStateLoopDuration() );
                    f32 factor = 1.0f - Min( m_moveVaccumInertiaTimer / getTemplate()->getVacuumSkill().getStateLoopDuration(), 1.0f );
                    blendFactor *= factor;
                    targetSpeed *= factor;
                    if ( factor < 0.001 )
                    {
                        m_moveVector = Vec2d::Zero;
                    }
                }
                else
                {
                    m_moveVaccumInertiaTimer -= _deltaTime;
                    m_moveVaccumInertiaTimer = 0.0f;
                    m_moveVaccumInertiaTimer = Max( m_moveVaccumInertiaTimer, 0.0f );
                    f32 factor = 1.0f - ( m_moveVaccumInertiaTimer / getTemplate()->getVacuumSkill().getStateLoopDuration() );
                    blendFactor *= factor;
                    targetSpeed *= factor;
                }        
            }        
               
            Vec2d moveInputNormalized = m_moveInput;
            moveInputNormalized.normalize();
            if ( inputForce >= 0.02f )
            {
                m_lastValidInputDir = moveInputNormalized;
            }

            // Speed inertia only is no stick
            f32 speedBlendFactor = 9999.9f;
            if ( inputForce < 0.02f )
            {
                speedBlendFactor = m_isInWater ? getTemplate()->getWaterMoveSpeedInertialBlendFactor() : getTemplate()->getDefaultMoveSpeedInertialBlendFactor();
            }
            m_moveSpeed = Interpolate( m_moveSpeed, targetSpeed, Min( _deltaTime * speedBlendFactor, 1.0f ) );
            m_moveVector = Interpolate( m_moveVector, m_moveSpeed * m_lastValidInputDir, Min( _deltaTime * blendFactor, 1.0f ) );
        }
        else
        {
            m_moveVector    = m_stateWantedMoveVector;
            maxLocalSpeed   = m_stateWantedLocalSpeedMax; 
            m_moveInput     = m_stateWantedMoveInput;
        }

        targetVelocity = camVelocity + m_moveVector;

        Vec2d deltaVelocity = (targetVelocity - currentVelocity);
        
        // PHYS params
        //**************
        m_characterPhys->addForce( (deltaVelocity * maxLocalSpeed ) );
        m_characterPhys->setAirFrictionMultiplier( ( inputForce > 0.0f ) ?  1.0f : 0.0f );
        m_characterPhys->setUpdateRotation( bfalse );
        
        // reset values
        m_stateWantedMoveVector = Vec2d::Zero;
        m_stateWantedMove       = bfalse;


#ifdef DEBUG_SHOOTER
        /*String msg;
        msg.setTextFormat("input %.0f%%", inputForce * 100);
        GFX_ADAPTER->drawDBGText(msg);
        //msg.setTextFormat("force %.2f", localForce.norm());
        //GFX_ADAPTER->drawDBGText(msg);
        msg.setTextFormat("speed %.2f", m_characterPhys->getSpeed().norm());
        GFX_ADAPTER->drawDBGText(msg);

        GFX_ADAPTER->drawDBGCircle(m_actor->getPos(), 0.1f);
        GFX_ADAPTER->drawDBGLine(m_actor->getPos(), m_actor->getPos() + m_moveInput.to3d());
        */
#endif
    }

    //---------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterControllerComponent::updateClamp( f32 _deltaTime )
    {
        m_borderScreenNomal = Vec2d::Zero;

        // camera not yet ready 
        if ( CAMERACONTROLLERMANAGER->m_controllerJustTeleported )
        {
            return;
        }

        AABB screenAABB = CAMERA->getAABBatZ(m_actor->getDepth());
        //  ( 19.095f, 11.426f );
        Vec2d camAABBSize( screenAABB.getWidth(), screenAABB.getHeight() );
        Vec2d maxMargin( camAABBSize.m_x * getTemplate()->getEjectMarginX().m_y, camAABBSize.m_y * getTemplate()->getEjectMarginY().m_x );
        Vec2d minMargin( camAABBSize.m_x * getTemplate()->getEjectMarginX().m_x, camAABBSize.m_y * getTemplate()->getEjectMarginY().m_y );

        screenAABB.setMin( screenAABB.getMin() + minMargin );
        screenAABB.setMax( screenAABB.getMax() - maxMargin );
        const Vec2d& min = screenAABB.getMin();
        const Vec2d& max = screenAABB.getMax();

        Vec2d pos = m_actor->get2DPos();
        Vec2d penetration;
        if (pos.m_x < min.m_x)
            penetration.m_x = pos.m_x - min.m_x;
        else if (pos.m_x > max.m_x)
            penetration.m_x = pos.m_x - max.m_x;
        if (pos.m_y < min.m_y)
            penetration.m_y = pos.m_y - min.m_y;
        else if (pos.m_y > max.m_y)
            penetration.m_y = pos.m_y - max.m_y;

        // get the wind force
        PhysActorForceArray windForces;
        Vec2d windForce = Vec2d::Zero;

        PHYSWORLD->getWindForces(m_actor->get2DPos(), Vec2d::Zero, m_actor->getDepth(), windForces);

        for ( u32 i = 0; i < windForces.size(); i++ )
        {
            windForce += windForces[i].m_force;
        }

        // apply a force to move away from the screen border
        f32 penetrationCoeff = Min(penetration.norm(), 1.f);
        if (penetrationCoeff > MTH_EPSILON)
        {
            Vec2d normal = -penetration.normalize();
            if ( windForce.dot( normal ) >= 0.0f )
            {
                m_characterPhys->addForce( ( penetrationCoeff * normal * getTemplate()->getEjectFromBorderForce() ) );
            }            
            m_borderScreenNomal = normal;
        }

#ifdef DEBUG_SHOOTER
        GFX_ADAPTER->drawDBGAABB(screenAABB, Color::red());
        /*GFX_ADAPTER->drawDBGLine(m_actor->get2DPos(), m_actor->get2DPos() - penetration, Color::red());
        GFX_ADAPTER->drawDBGAABB(screenAABB);

        AABB deathAABB = CAMERA->getAABBatZ(m_actor->getDepth());
        deathAABB.setMin( deathAABB.getMin() + getTemplate()->getDeathMargin() );
        deathAABB.setMax( deathAABB.getMax() - getTemplate()->getDeathMargin() );
        GFX_ADAPTER->drawDBGAABB(deathAABB, Color::red());*/
#endif // DEBUG_SHOOTER
    }

    //---------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterControllerComponent::processCollisions( f32 _deltaTime )
    {
        if ( !isImmune() )
        {
            if ( checkOutOfScreen( _deltaTime ) )
            {
                die();
            }
        }

        if ( !m_isDead )
        {
            checkContacts();
        }        

        // check vacuum influence
        if ( m_isUnderVacuumInfluence && !m_hasBeenVacuumed )
        {
            m_isVacuumedTimer += _deltaTime;
            f32 deltaDuration = getTemplate()->getVacuumedResistanceDurationMax() - getTemplate()->getVacuumedResistanceDurationMin();
            // is vacuumed
            if ( m_isVacuumedTimer >= ( getTemplate()->getVacuumedResistanceDurationMin() + ( deltaDuration * m_isUnderVacuumInfluenceForce ) ) )
            {
                Actor * vacuumer = AIUtils::getActor( m_isUnderVacuumInfluenceOwner );
                if ( vacuumer != NULL )
                {
                    Ray_EventShooterVacuumed vacuumedEvent;
                    vacuumedEvent.setSender( m_actor->getRef() );
                    vacuumer->onEvent( &vacuumedEvent );
                    m_hasBeenVacuumed = vacuumedEvent.isVaccumEffective();
                }
            }
        }
        else 
        {
            m_isVacuumedTimer = f32_Max( 0.0f, m_isVacuumedTimer - _deltaTime );
        }
    }

    //---------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterControllerComponent::processSolidStim( SolidStim* _solid )
    {
        die();
    }

    //---------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterControllerComponent::processVacuumStim( VacuumStim* _vacuumStim )
    {
        if ( _vacuumStim->getSender() != m_actor->getRef() )
        {
            Actor * vacuumer = AIUtils::getActor( _vacuumStim->getSender() );
            if ( vacuumer != NULL )
            {
                bbool receiveDamage = bfalse;
                if ( AIUtils::isEnemy( getFaction(), AIUtils::getFaction( vacuumer ), &receiveDamage ) && receiveDamage )
                {
                    if ( _vacuumStim->getVacuumDist() > 0.0f )
                    {
                        f32 dist = (m_actor->get2DPos() - vacuumer->get2DPos()).norm();
                        m_isUnderVacuumInfluenceForce = Min( 1.0f, dist / _vacuumStim->getVacuumDist() );
                        m_isUnderVacuumInfluenceOwner = _vacuumStim->getSender();
                        m_isUnderVacuumInfluence = btrue;
                    }               
                }
            }
        }
    }

    //---------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterControllerComponent::processBounceStim( BounceStim * _bounceStim )
    {
        if ( _bounceStim == NULL )
        {
            return;
        }
        // !!! temp management !!! test !!!!
        f32 multiplier = _bounceStim->getMultiplier();
        m_characterPhys->addForce( _bounceStim->getBounceDir() * getTemplate()->getBounceStimDefaultForce() * multiplier );
    }

    //---------------------------------------------------------------------------------------------------------------------------
    bbool Ray_ShooterControllerComponent::checkOutOfScreen( f32 _deltaTime )
    {
        if ( CAMERA->getMode() == Camera::Camera_Editor )
            return bfalse;

        if ( CAMERACONTROLLERMANAGER->m_controllerJustTeleported )
        {
            return bfalse;
        }

        AABB deathAABB;

        if (CAMERACONTROLLERMANAGER->getAABB(m_actor->getDepth(),deathAABB) )
        {
            if ( !deathAABB.contains( m_actor->get2DPos() ) )
            {
                m_outofScreenTimer += _deltaTime;
                if ( m_outofScreenTimer > getTemplate()->getOutOfScreenDurationBeforeDeath() )
                {
                    return btrue;
                }
            }
            else
            {
                m_outofScreenTimer = 0.0f;
            }

            Vec2d camAABBSize( deathAABB.getWidth(), deathAABB.getHeight() );
            Vec2d margin( camAABBSize.m_x * getTemplate()->getDeathMargin().m_x, camAABBSize.m_y * getTemplate()->getDeathMargin().m_y );

            deathAABB.setMin( deathAABB.getMin() + margin );
            deathAABB.setMax( deathAABB.getMax() - margin );
            
            return !deathAABB.contains(m_actor->get2DPos());
        }
        else if ( !deathAABB.isZero() )
            return btrue;

        return bfalse;
    }

    //---------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterControllerComponent::checkContacts()
    {   
        const PhysContactsContainer& contacts = m_characterPhys->getContacts();

        Vec2d hitNormal = Vec2d::Right;
        bbool solidPenetrating = bfalse;
        const f32 penetratingRadiusSq = m_characterPhys->getRadius() * 0.9f * m_characterPhys->getRadius() * 0.9f;
        bbool immune = isImmune();

        for ( u32 i = 0; i < contacts.size(); ++i )
        {
            const SCollidableContact& contact = contacts[i];
            const BaseObject* obj = GETOBJECT(contact.m_collidableUserData);
            if ( obj != NULL )
            {
                const PolyLine* polyLine = obj->DynamicCast<PolyLine>(ITF_GET_STRINGID_CRC(PolyLine,1932163747));
                if (polyLine)
                {
                    // we touched a polyline, 
                    const PolyLineEdge& edge = polyLine->getEdgeAt(contact.m_edgeIndex);
                    const Ray_GameMaterial_Template* material = (const Ray_GameMaterial_Template*)(World::getGameMaterial(edge.getGameMaterial()));
                    
                    Vec2d edgeNormal = edge.m_normalizedVector.getPerpendicular();
                    bbool isSolid = ( material == NULL ) || material->isSolid();

                    // fluid frise ?
                    if ( ( polyLine->m_ownerFrise != NULL ) && ( polyLine->m_ownerFrise->getConfig() != NULL ) )
                    {
                        if ( polyLine->m_ownerFrise->getConfig()->m_methode == ( Frise::InFluid ) )
                        {
                            setCurrentFluidContactPolyline( polyLine );

                            if ( m_fluidContactEdgeList.find( contact.m_edgeIndex ) < 0 )
                            {
                                m_fluidContactEdgeList.push_back( contact.m_edgeIndex );
                            }
                        }
                    }

                    // Let's see if it's dangerous
                    if ( ( material != NULL ) && !immune )
                    {
                        u32 dangerLevel = material->getDangerousLevel();
                        if ( dangerLevel && ( ( polyLine->getOwnerActor() == NULL ) || !m_vacuumSkill.isVacuumedActor( polyLine->getOwnerActor()->getRef() ) ) )
                        {
                            receiveDamage( 1 );
                            bounceBackFromHit( edgeNormal );
                            break;
                        }
                        // !! temp !! test !!!
                       /* else if ( material->getDjembe() )
                        {
                            m_characterPhys->addForce( edgeNormal * getTemplate()->getDjembeBounceForce() );
                        }*/
                    }

                    // has a solid polyline in the phys radius
                    //
                    if ( isSolid && ( ( m_actor->get2DPos() - contact.m_contactPoint ).sqrnorm() <= penetratingRadiusSq ) )
                    {
                        solidPenetrating = btrue;
                    }
                }
            }
        }

        if ( solidPenetrating )
        {
            die();
        }
    }
    
    //---------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterControllerComponent::processHitStim( PunchStim* _hit )
    {
        if ( _hit->getOriginalSender() == m_actor->getRef() )
            return;

        if (_hit->getSender() == m_invincibleAgainstActor)
            return;

        // use earthquake type, to bypass the is vacuumed check
		if ( m_vacuumSkill.isVacuumedActor( _hit->getSender() ) && ( _hit->getReceivedHitType() != RECEIVEDHITTYPE_EARTHQUAKE ) )
			return;

        bbool shouldReceiveDamage;
        if ( !AIUtils::isEnemyHit(_hit, getFaction(), &shouldReceiveDamage) )
            return;

        bbool immune = isImmune();
        if ( immune && !m_hurtImmunityProcessHitAction )
            return;

        if ( shouldReceiveDamage && !immune )
        {
            receiveDamage( 1 /*_hit->getLevel()*/ );
        }

        // is immune and play bounce => check multi receive hit
        u32 lastFrame = m_lastHitFrame;
        m_lastHitFrame = CURRENTFRAME;
        if ( immune )
        {
            if ( lastFrame + 1 >= m_lastHitFrame )
            {
                return;
            }
        }        

        bbool friendlyHit = !shouldReceiveDamage;
        if ( friendlyHit )
            return;

        // send hit success
        //
        Actor * sender = AIUtils::getActor( _hit->getSender() );
        if ( sender != NULL )
        {
            EventHitSuccessful hitSuccessful;
            hitSuccessful.setSender( m_actor->getRef() );
            sender->onEvent(&hitSuccessful);
        }

        // freindly hit, no bounce back
        /*if ( friendlyHit )
            return;*/

        // process move dir
        //
        //Vec2d initNormal = _hit->getDirection();
        Vec2d initNormal = m_actor->get2DPos() - sender->get2DPos();
        initNormal.normalize();
        Vec2d normal = initNormal;

        // has contacts ?
        const PhysContactsContainer& contacts = _hit->getContacts();
        if ( contacts.size() > 0 )
        {
            const SCollidableContact& contact = contacts[0];
            PolyLine* poly(NULL);
            const PolyLineEdge* edge(NULL);
            //Vec2d normal = contact.m_normal;// m_contactPoint - _hit->getPos();
            initNormal = m_actor->get2DPos() - contact.m_contactPoint;
            initNormal.normalize();
            normal = initNormal;

            if ( contact.m_edgeIndex != U32_INVALID )
            {
                AIUtils::getPolyLine(contact.m_collidableUserData, contact.m_edgeIndex, poly, edge);
                if ( poly && edge )
                {
                    normal = edge->m_normalizedVector.getPerpendicular();
                    normal.normalize();
                }
            }
        }

       
        Vec2d vSpeed = _hit->getPrevPos().IsEqual( Vec2d::Zero ) ? Vec2d::Zero : ( _hit->getPos() - _hit->getPrevPos() );

        // move out of the hit
        if ( !vSpeed.IsEqual( Vec2d::Zero, 0.02f ) && ( vSpeed.dot( normal ) > 0.f ) )
        {
            vSpeed.normalize();

            if ( !friendlyHit )
            {
                normal = vSpeed.getPerpendicular();
                normal *= getSign( normal.dot( initNormal ) );
            }
        }
        else if ( _hit->getIsRadial() )
        {
            normal = initNormal;
        }
       /* else
        {
            normal = _hit->getDirection();
        }*/
              
        bounceBackFromHit( normal, friendlyHit );
       /* }
        else
        {
            bounceBackFromHit(_hit->getDirection(), friendlyHit );
        }*/
    }

    //---------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterControllerComponent::receiveDamage( i32 _numHP )
    {
        setImmuneTime( getTemplate()->getHurtImmunityDuration(), btrue );

        Ray_Player* player = static_cast<Ray_Player*>(GAMEMANAGER->getPlayer(m_playerIndex));
        ITF_ASSERT( player != NULL );
        if ( player != NULL )
        {
            player->addHitPoints(-_numHP);
            player->setWasHitInLevel( btrue );
            
            CHANGE_STATE( STATE_HIT );
        }
    }

    //---------------------------------------------------------------------------------------------------------------------------
    // Die, check if other players are dead to restart to previous checkpoint
    void Ray_ShooterControllerComponent::revive()
    {
        if ( m_isDead )
        {
           /* Vec2d pos = getUndeadActorPos();
            if ( !pos.IsEqual( Vec2d::Zero ) )
            {
                m_actor->set2DPos( pos );
            }*/

            Ray_Player* pCurRayPlayer = static_cast<Ray_Player*>( RAY_GAMEMANAGER->getPlayer(m_playerIndex) );
            if ( pCurRayPlayer->getActive() )
            {
                pCurRayPlayer->revive();
            }

            m_actor->disableDraw( bfalse );
            m_characterPhys->setDisabled(bfalse);
            m_characterPhys->setSpeed(Vec2d::Zero);
            m_isDead = bfalse;
            m_hasBeenVacuumed = bfalse;
        }
    }

    //---------------------------------------------------------------------------------------------------------------------------
    // Die, check if other players are dead to restart to previous checkpoint
    Vec2d Ray_ShooterControllerComponent::getUndeadActorPos()
    {
        Ray_Player* pCurRayPlayer;
        for ( u8 i = 0 ; i < RAY_GAMEMANAGER->getMaxPlayerCount(); ++i )
        {
            pCurRayPlayer = static_cast<Ray_Player*>( RAY_GAMEMANAGER->getPlayer(i) );
            if ( pCurRayPlayer && pCurRayPlayer->getActive() && !pCurRayPlayer->isDead() )
            {
                return pCurRayPlayer->getActor()->get2DPos();
            }
        }
        return Vec2d::Zero;
    }


    //---------------------------------------------------------------------------------------------------------------------------
    // Die, check if other players are dead to restart to previous checkpoint
    void Ray_ShooterControllerComponent::die()
    {

#ifdef ITF_SUPPORT_ONLINETRACKING

	RAY_GAMEMANAGER->getOnlineTrackingManager()->OnlineTrackingTrackDeath(m_playerIndex);

#endif 
        setImmuneTime( getTemplate()->getHurtImmunityDuration() );

        if ( !m_isDead )
        {
            Ray_Player* pCurRayPlayer = static_cast<Ray_Player*>( RAY_GAMEMANAGER->getPlayer(m_playerIndex) );
            if ( pCurRayPlayer->getActive() )
            {
                pCurRayPlayer->die();
                // cheet
                if ( !pCurRayPlayer->isDead() )
                {
                    if ( checkOutOfScreen( 0.0f ) )
                    {
                        RAY_GAMEMANAGER->resetToLastCheckpoint();
                    }
                    return;
                }
            }
           
            // Check other players life
            bbool restartGame = RAY_GAMEMANAGER->getShooterGameModeController()->isAllPlayerDead();
          
            // Restart
            if ( restartGame )
            {
                CHANGE_STATE( STATE_DEATH );
                // Disable draw actor
          /*    m_actor->disableDraw( btrue );
                m_characterPhys->setDisabled(btrue);
                m_characterPhys->setSpeed(Vec2d::Zero);

                m_actor->disable();
                RAY_GAMEMANAGER->resetToLastCheckpoint();// triggerRestoreDeath((RAY_GAMEMANAGER->getPlayer(m_playerIndex))->getCurrentActor(),RAY_GAMEMANAGER->getDeathFadeDuration());
                */
            }
            // Multiplayer, wait time before respawn
            else 
            {
                CHANGE_STATE( STATE_BUBBLE );
                m_isDeadWaitRespawnTimer = 0.0;
                // remove vacuumed actors
                m_vacuumSkill.reset( btrue );
            }
        }		

        m_isDead = btrue;
    }

    void Ray_ShooterControllerComponent::bounceBackFromHit( const Vec2d& _hitDirection, bbool _friendlyHits /*=bfalse*/ )
    {
        if ( !_friendlyHits )
        {
            m_characterPhys->addForce( getTemplate()->getEjectFromHitForce() * _hitDirection );
        }
        else
        {
            m_characterPhys->addForce( getTemplate()->getEjectFromFriendlyHitForce() * _hitDirection );
        }
        
        //GFX_ADAPTER->drawDBG3DLine(m_actor->getPos(), m_actor->getPos() + _hitDirection.to3d(), Color::red(), 1);
    }

    void Ray_ShooterControllerComponent::blinkWhenHurt( f32 _deltaTime )
    {
        // when we're hit, timer is reset and we're invincible (we can't be hit again)
        // when timer reaches the duration, we stop being invincible

        f32 alpha = 1;

        if ( isImmune() )
        {
            f32 ratio = m_hurtTimer / getTemplate()->getHurtImmunityDuration();
            ratio = f32_Abs(f32_Cos(ratio * getTemplate()->getHurtBlinkCount() * MTH_PI));
            alpha = InverseInterpolate(getTemplate()->getHurtBlinkMinAlpha(), getTemplate()->getHurtBlinkMaxAlpha(), ratio);
        }

        m_animComponent->setAlpha(alpha);
    }

    void Ray_ShooterControllerComponent::shoot(  )
    {

    }
	bbool Ray_ShooterControllerComponent::hasAmmo()
	{
        return ( ( m_baseBulletPool.isInitialized() ) || m_vacuumSkill.hasAmmo() );		
	}

    //------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterControllerComponent::updateAnimBlend( f32 _deltaTime )
    {
        // MOVE input
        //---------------
        const f32 blendSpeedFactor = 10.0f;
        Vec2d moveInput = m_moveInput;

        // cam transition, change the input value
        if ( m_curMode == cameraTransition )
        {
            f32 inputAngle = Clamp( f32_Abs( moveInput.getAngle() ), MTH_PIBY4, MTH_PI - MTH_PIBY4 );
            
            if ( m_camTransMode_ChangeDepthStartValue < m_camTransMode_ChangeDepthDest ) 
                inputAngle = -inputAngle;
            moveInput = Vec2d( 1.0f, 0.0f ).Rotate( inputAngle );
        }
        
        f32 moveNorm = moveInput.norm();
        if ( moveNorm != 0.0f )
        {
            moveInput = moveInput / moveNorm;
        }

        f32 inputAngle = ( moveInput != Vec2d::Zero ) ? moveInput.getAngle() : MTH_PIBY2;
        f32 xCursor = ( f32_Abs( inputAngle ) / MTH_PI );
        f32 yAngle = inputAngle - MTH_PIBY2;
        NormalizeAnglePI( yAngle );
        f32 yCursor = f32_Abs( yAngle ) / MTH_PI;
        if ( moveNorm < 0.001f )
        {
            xCursor = 0.5f;
            yCursor = 0.5f;
        }

        xCursor = Interpolate( 0.5f, xCursor, Min( moveNorm, 1.0f ) );
        yCursor = Interpolate( 0.5f, yCursor, Min( moveNorm, 1.0f ) );
        f32 blendCursor = f32_Abs( 1.0f - ( yCursor / 0.5f ) );
        
        m_animMoveXCursor = Interpolate( m_animMoveXCursor, xCursor, _deltaTime * blendSpeedFactor );
        m_animMoveYCursor = Interpolate( m_animMoveYCursor, yCursor, _deltaTime * blendSpeedFactor );
        m_animMoveBlendCursor = Interpolate( m_animMoveBlendCursor, blendCursor, _deltaTime * blendSpeedFactor );
    }

    //------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterControllerComponent::updateAnimInput()
    {
        #define s_Ammo          ITF_GET_STRINGID_CRC(Ammo,2718370018)
        #define s_FillValue     ITF_GET_STRINGID_CRC(FillValue,2697247514)
        #define s_blendCursor   ITF_GET_STRINGID_CRC(MoveBlendCursor,1863137430)
        #define s_RLCursor      ITF_GET_STRINGID_CRC(MoveRLCursor,895875713)
        #define s_UDCursor      ITF_GET_STRINGID_CRC(MoveUDCursor,900533874)
              
        m_animComponent->setInput(s_Ammo, static_cast<u32>( hasAmmo() ) );

        u32 fillValue = m_vacuumSkill.getFillValue();
		m_animComponent->setInput(s_FillValue, fillValue );

        // MOVE input
        //---------------
        m_animComponent->setInput( s_RLCursor, m_animMoveXCursor );
        m_animComponent->setInput( s_UDCursor, m_animMoveYCursor );
        m_animComponent->setInput( s_blendCursor, m_animMoveBlendCursor );
    }

	void Ray_ShooterControllerComponent::spit()
	{
		ITF_ASSERT(hasAmmo());
        
        Vec3d bonePos;
        m_animComponent->getBonePos(m_gunBoneIndex,bonePos);
        Vec2d shootDir = Vec2d::Right.Rotate( m_actor->getAngle() );
        
        // get the position of the start shoot in the coll
        //
        Vec3d posToBone = bonePos - m_actor->getPos();
        f32 distToPos = posToBone.norm();
        if ( distToPos != 0.0f ) 
        {
            posToBone /= distToPos;
        }
        Vec3d shootPos = bonePos;
        if ( distToPos >= m_characterPhys->getRadius() )
        {
            shootPos =  m_actor->getPos() + ( posToBone * m_characterPhys->getRadius() );
        }        
        shootPos.m_z -= 0.02f;      // spawn pos behind the player
        
        if ( !m_vacuumSkill.hasAmmo() && m_baseBulletPool.isInitialized() )
        {
#ifdef DEBUG_SHOOTER
            f64 curTime = SYSTEM_ADAPTER->getTime();
            m_dbg_fireRate = f32( curTime - m_dbg_fireRateLastFireTime );
            m_dbg_fireRateLastFireTime = curTime;
#endif //DEBUG_SHOOTER

            Actor * spawnedActor = m_baseBulletPool.getActor( shootPos, 0.0f );
            if ( spawnedActor != NULL )
            {
                EventSetDirection dirEvt;
                dirEvt.setSender( m_actor->getRef() );
                dirEvt.setDirection( shootDir );
                spawnedActor->onEvent(&dirEvt);
                spawnedActor->enable();
            }
            return; 
        }

		Actor * ejectedActor = m_vacuumSkill.Shoot( shootPos, shootDir );
		if ( ejectedActor != NULL )
		{
			ejectedActor->enable();

		    ejectedActor->setPos( shootPos );
			Ray_EventShooterEjectActor ejectEvent( shootDir, getTemplate()->getSpitObjSpeed() );
            ejectEvent.setSender( m_actor->getRef() );
			ejectedActor->onEvent(&ejectEvent);

			EventShow show(1.0f,0.1f);
			ejectedActor->onEvent(&show);
			m_invincibleAgainstActor = ejectedActor->getRef();
			m_invincibleTimer = getTemplate()->getInvincibilityTime();
		}
	}

    //------------------------------------------------------------------------------------------------------------------------
	void Ray_ShooterControllerComponent::getSwallowPos( Vec3d & _pos )
	{
        _pos = Vec3d::Zero;
        if ( m_swallowBoneIndex != U32_INVALID )
        {
		    m_animComponent->getBonePos(m_swallowBoneIndex,_pos);
        }
	}

    //------------------------------------------------------------------------------------------------------------------------
	void Ray_ShooterControllerComponent::onResourceReady()
	{
		Super::onResourceReady();

        if ( m_animComponent == NULL )
        {
            return;
        }

		m_gunBoneIndex = m_animComponent->getBoneIndex(getTemplate()->getGunBone());
		ITF_ASSERT_MSG(m_gunBoneIndex != U32_INVALID, "you need to specify the gunBone");
		m_swallowBoneIndex = m_animComponent->getBoneIndex(getTemplate()->getSwallowBone());
		ITF_ASSERT_MSG(m_swallowBoneIndex != U32_INVALID, "you need to specify the swallowBone");

        // set the bank state
        ITF_ASSERT( m_animComponent != NULL );
        if ( m_animComponent != NULL )
        {
            m_animComponent->setBankState( getTemplate()->getAnimBankState() );
        }
	}

    //------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterControllerComponent::updateSoundInput()
    {
        Super::updateSoundInput();

        if ( m_soundComponent == NULL )
            return;
        
        //Default values
        m_soundComponent->setInput( ITF_GET_STRINGID_CRC(Speed,4218325830),     m_moveVector.norm() );
        m_soundComponent->setInput( ITF_GET_STRINGID_CRC(SpeedX,3996234170),    m_moveVector.m_x );
        m_soundComponent->setInput( ITF_GET_STRINGID_CRC(SpeedY,461177981),     m_moveVector.m_y );
    }

    //------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterControllerComponent::cleanListener()
    {
        m_listenerMove = Vec2d::Zero;         
        m_listenerLeave = bfalse;

        m_listenerShoot = bfalse;
        m_listenerShootHold = bfalse;
        m_listenerVacuum = bfalse;
        m_listenerVacuumHold = bfalse;              
        m_listenerVacuumReleased = bfalse;
        m_listenerTest = bfalse;
    }
    //------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterControllerComponent::Receive( u32 _deviceID /* player */, f32 _axis, const StringID& _action )
    {
        if ( isPlayerDisable() || m_playerIndex != _deviceID )
        {
            return; 
        }
        const StringID s_Vacuum          = ITF_GET_STRINGID_CRC(VACUUM,3229720905);
        const StringID s_VacuumHold      = ITF_GET_STRINGID_CRC(VACUUM_HOLD,1441444811);
        const StringID s_VacuumReleased  = ITF_GET_STRINGID_CRC(VACUUM_RELEASED,2587803822);
        const StringID s_Shoot           = ITF_GET_STRINGID_CRC(SHOOT,3651126636);
        const StringID s_ShootHold       = ITF_GET_STRINGID_CRC(SHOOT_HOLD,1361638555);
        const StringID s_MoveX           = ITF_GET_STRINGID_CRC(MOVEX,965019289);
        const StringID s_MoveY           = ITF_GET_STRINGID_CRC(MOVEY,951160567);
        const StringID s_Leave           = ITF_GET_STRINGID_CRC(LEAVE,982417778);
        const StringID s_Test            = ITF_GET_STRINGID_CRC(TEST,3984939898);

        if ( _action == s_Shoot )
        {
            m_listenerShoot = _axis > 0.0f;
        }
        else if ( _action == s_Leave )
        {
            m_listenerLeave = _axis > 0.0f;
        }
        else if ( _action == s_MoveX )
        {
            m_listenerMove.m_x = _axis;
        }
        else if ( _action == s_MoveY )
        {
            m_listenerMove.m_y = _axis;
        }
        else if ( _action == s_ShootHold )
        {
            m_listenerShootHold = _axis > 0.0f;
        }
        // Vacuum -----
        else if ( _action == s_Vacuum )
        {
            m_listenerVacuum = _axis > 0.0f;
        }
        else if ( _action == s_VacuumHold )
        {
            m_listenerVacuumHold = _axis > 0.0f;
        }
        else if ( _action == s_VacuumReleased )
        {
            m_listenerVacuumReleased = _axis > 0.0f;
        }
        else if ( _action == s_Test )
        {
            m_listenerTest = btrue;
        }
    }

    //------------------------------------------------------------------------------------------------------------------------
    //void Ray_ShooterControllerComponent::Draw()
    //{
    //    Super::Draw();

    //    BezierPatchParams params;

    //    params.m_p0 = m_actor->get2DPos();
    //    params.m_p1 = m_actor->get2DPos() + Vec2d::Right * 2.0f; 
    //    params.m_p2 = params.m_p1 + Vec2d::Up;
    //    params.m_p3 = params.m_p2 + Vec2d::Right * 2.0f;

    //    //f32 countTileSeg = nodeSource.m_lengthSeg / getTemplate()->getTileLength();

    //    const Ray_ShooterControllerComponent_Template::VacuumPatchData & data = getTemplate()->getVacuumPatchData();
    //    m_test += 0.005f;
    //    if ( m_test >= 1.0f )
    //    {
    //        m_test = 0.0f;
    //    }

    //    params.m_startUV = data.m_startUV;
    //    params.m_startUV.m_x += m_test;

    //    params.m_endUV = data.m_endUV;
    //    params.m_endUV.m_x += m_test;

    //    // Get UV for Y
    //  /*  if (m_useAtlas)
    //    {
    //        Vec2d uv0, uv1;
    //        getUVDatabyIndex(m_curIndexAtlas, uv0, uv1);
    //        params.m_startUV.m_y = uv0.m_y;
    //        params.m_endUV.m_y = uv1.m_y;
    //    }*/

    //    params.m_startWidth = data.m_startWidth;
    //    params.m_endWidth = data.m_endWidth;

    //    params.m_z = m_actor->getDepth();
    //    params.m_texture = (Texture*)m_vacuumPatchTexture.getResource();
    //    params.m_uvMode = ( BEZIER_PATCH_UVMODE )data.m_uvMode;
    //    params.m_blendMode = ( GFX_BLENDMODE )data.m_blendMode;

    //    params.m_startColor = params.m_endColor = Color::white();
    //    params.m_fogColor = Color::white();
    //    params.m_fogColor.m_a = 0.0f;

    //    params.m_hdiv = data.m_hdiv;
    //    params.m_vdiv = data.m_vdiv;

    //    m_vacuumPatch.drawPatch( params );

    //}
   

#ifdef ITF_SUPPORT_EDITOR
	//------------------------------------------------------------------------------------------------------------------------
	void Ray_ShooterControllerComponent::drawEdit( class ActorDrawEditInterface* _drawInterface, u32 _flags ) const
	{
        Super::drawEdit( _drawInterface, _flags );
		m_vacuumSkill.debugDraw();

#ifdef DEBUG_SHOOTER
        String s_text;
        s_text.setTextFormat( "FireRate: %f", m_dbg_fireRate );
        Vec3d res2d;
        Vec3d displayPos = m_actor->getPos();
        //displayPos.m_x += 0.0f;
        displayPos.m_y += 1.5f;
        GFX_ADAPTER->compute3DTo2D( displayPos, res2d );
        GFX_ADAPTER->drawDBGText( s_text, res2d.m_x, res2d.m_y );
#endif //DEBUG_SHOOTER
	}

	//------------------------------------------------------------------------------------------------------------------------
	void Ray_ShooterControllerComponent::onEditorMove(bbool _modifyInitialPos)
	{
		Super::onEditorMove(_modifyInitialPos);

		// Move the camera too
		if ( SHOOTERCAMERA != NULL )
		{
			SHOOTERCAMERA->dbgTeleport( m_actor->getPos() );
		}
	}
#endif // ITF_SUPPORT_EDITOR

    //-------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(Ray_ShooterControllerComponent_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_ShooterControllerComponent_Template)

        SERIALIZE_MEMBER("animBankState", m_animBankState);
        SERIALIZE_MEMBER("minLocalSpeed", m_minLocalSpeed);
        SERIALIZE_MEMBER("maxLocalSpeed", m_maxLocalSpeed);
		SERIALIZE_MEMBER("vaccumMoveSpeedBlendFactor", m_vaccumMoveSpeedBlendFactor);
        SERIALIZE_MEMBER("defaultMoveSpeedBlendFactor", m_defaultMoveSpeedBlendFactor);
        SERIALIZE_MEMBER("defaultMoveSpeedInertialBlendFactor", m_defaultMoveSpeedInertialBlendFactor );
        SERIALIZE_MEMBER("waterMoveSpeedInertialBlendFactor", m_waterMoveSpeedInertialBlendFactor );
        SERIALIZE_MEMBER("waterMoveSpeedBlendFactor", m_waterMoveSpeedBlendFactor );
        SERIALIZE_MEMBER("waterCheckRadius", m_waterCheckRadius );
        SERIALIZE_MEMBER("waterResistanceForce", m_waterResistanceForce );
        SERIALIZE_MEMBER("ejectFromBorderForce", m_ejectFromBorderForce);
        SERIALIZE_MEMBER("ejectFromHitForce", m_ejectFromHitForce);
        SERIALIZE_MEMBER("ejectFromFriendlyHitForce", m_ejectFromFriendlyHitForce);
        SERIALIZE_MEMBER("ejectMarginX", m_ejectMarginX);
        SERIALIZE_MEMBER("ejectMarginY", m_ejectMarginY);
        SERIALIZE_MEMBER("deathMargin", m_deathMargin);
        SERIALIZE_MEMBER("outOfScreenDurationBeforeDeath", m_outOfScreenDurationBeforeDeath );
        SERIALIZE_MEMBER("hurtInvincibleDuration", m_hurtImmunityDuration);
        SERIALIZE_MEMBER("reviveImmunityDuration", m_reviveImmunityDuration);
        SERIALIZE_MEMBER("hurtBlinkCount", m_hurtBlinkCount);
        SERIALIZE_MEMBER("hurtBlinkMinAlpha", m_hurtBlinkMinAlpha);
        SERIALIZE_MEMBER("hurtBlinkMaxAlpha", m_hurtBlinkMaxAlpha);
        SERIALIZE_MEMBER("gunBone", m_gunBone);
        SERIALIZE_MEMBER("swallowBone", m_swallowBone);
        SERIALIZE_MEMBER("invincibilityTime", m_invincibilityTime);
        SERIALIZE_OBJECT("vacuumSkill",m_vacuumSkill);
		SERIALIZE_MEMBER("spitObjSpeed", m_spitObjSpeed );
        SERIALIZE_MEMBER("baseSpawnBulletPath", m_baseSpawnBulletPath );
        SERIALIZE_MEMBER("deathSpawnFXPath", m_deathSpawnFXPath );
                
        SERIALIZE_OBJECT("heartShield",m_heartShield);
        SERIALIZE_OBJECT("swarmRepeller",m_swarmRepellerPowerUp);

        SERIALIZE_MEMBER("bubbleStateBlendFactor",  m_bubbleStateBlendFactor );
        SERIALIZE_MEMBER("bubbleStateLocalSpeedMax", m_bubbleStateLocalSpeedMax );
        SERIALIZE_MEMBER("bubbleStatePhantomRadius", m_bubbleStatePhantomRadius );

        SERIALIZE_MEMBER("djembeBounceForce", m_djembeBounceForce );
        SERIALIZE_MEMBER("bounceStimDefaultForce", m_bounceStimDefaultForce );
        SERIALIZE_MEMBER("vacuumedResistanceDurationMin", m_vacuumedResistanceDurationMin );
        SERIALIZE_MEMBER("vacuumedResistanceDurationMax", m_vacuumedResistanceDurationMax );
        
        SERIALIZE_MEMBER("autoFireDelay", m_autoFireDelay );
        
		SERIALIZE_MEMBER("DBG_CanVaccumIfFull", m_DBG_CanVaccumIfFull );
		SERIALIZE_MEMBER("DBG_VaccumAllowPressStop", m_DBG_VaccumAllowPressStop );

        //SERIALIZE_MEMBER("vacuumPatchTextureFile", m_vacuumPatchTextureFile );
        //SERIALIZE_OBJECT("vacuumPatchData", m_vacuumPatchData );
		
    END_SERIALIZATION()

    //BEGIN_SERIALIZATION_SUBCLASS(Ray_ShooterControllerComponent_Template, VacuumPatchData )
    //    SERIALIZE_MEMBER("startUV", m_startUV);
    //    SERIALIZE_MEMBER("endUV", m_endUV);
    //    SERIALIZE_MEMBER("startWidth", m_startWidth);
    //    SERIALIZE_MEMBER("endWidth", m_endWidth);
    //    SERIALIZE_MEMBER("blendMode", m_blendMode);
    //    SERIALIZE_MEMBER("uvMode", m_uvMode);
    //    SERIALIZE_MEMBER("hdiv", m_hdiv);
    //    SERIALIZE_MEMBER("vdiv", m_vdiv);
    //END_SERIALIZATION()

    //    Ray_ShooterControllerComponent_Template::VacuumPatchData::VacuumPatchData()
    //    : m_startUV(0.0f, 0.0f)
    //    , m_endUV(1.0f, 1.0f)
    //    , m_startWidth(1.f)
    //    , m_endWidth(1.f)
    //    , m_blendMode(GFX_BLEND_ALPHA)
    //    , m_uvMode(UV_MODE_OPTIMUM)
    //    , m_hdiv(MESH_DEFAULT_HDIV)
    //    , m_vdiv(MESH_DEFAULT_VDIV)
    //{   
    //}

    Ray_ShooterControllerComponent_Template::Ray_ShooterControllerComponent_Template()
    //: m_playerIndex(U32_INVALID)
    : Super()
    , m_animBankState( 0 )
    , m_minLocalSpeed(0)
    , m_maxLocalSpeed(10)
	, m_vaccumMoveSpeedBlendFactor( 9999.9f )
    , m_defaultMoveSpeedBlendFactor( 9999.9f )
    , m_defaultMoveSpeedInertialBlendFactor( 9999.9f )
    , m_waterMoveSpeedInertialBlendFactor( 9999.9f )
    , m_waterMoveSpeedBlendFactor( 9999.9f )
    , m_ejectFromBorderForce(100)
    , m_ejectFromHitForce(100)
    , m_ejectFromFriendlyHitForce( 100.f )
    , m_hurtImmunityDuration(1)
    , m_reviveImmunityDuration( 0.5f )
    , m_hurtBlinkCount(3)
    , m_hurtBlinkMinAlpha(0)
    , m_hurtBlinkMaxAlpha(1)
    , m_ejectMarginX( 0.0785545f, 0.0785545f )      // % screen X : 0.104739, 0.0785545
    , m_ejectMarginY( 0.043759f, 0.043759f )        // % screen Y : 0.043759, 0.043759
    , m_deathMargin(Vec2d::Zero)
    , m_outOfScreenDurationBeforeDeath( 2.0f )
    , m_invincibilityTime(0.3f)
    , m_heartShield()
    , m_swarmRepellerPowerUp()
	, m_DBG_CanVaccumIfFull( bfalse )
	, m_DBG_VaccumAllowPressStop( bfalse )
    , m_bubbleStateBlendFactor( 99999.9f )
    , m_bubbleStateLocalSpeedMax( 10.f )
    , m_bubbleStatePhantomRadius( 0.5f )
    , m_djembeBounceForce( 100.f )
    , m_bounceStimDefaultForce( 100.f )
    , m_vacuumedResistanceDurationMin( 0.5f )
    , m_vacuumedResistanceDurationMax( 0.5f )
    , m_autoFireDelay( 0.5f )
    , m_waterCheckRadius( 0.5f )
    , m_waterResistanceForce( 70.0f )
    {
		m_spitObjSpeed.set( 10.f, 0.0 );
    }
};
