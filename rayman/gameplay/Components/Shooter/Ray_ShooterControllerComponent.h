#ifndef _ITF_RAY_SHOOTERCONTROLLERCOMPONENT_H_
#define _ITF_RAY_SHOOTERCONTROLLERCOMPONENT_H_

#ifndef _ITF_RAY_BASICPLAYERCONTROLLERCOMPONENT_H_
#include "rayman/gameplay/Components/Player/Ray_BasicPlayerControllerComponent.h"
#endif //_ITF_RAY_BASICPLAYERCONTROLLERCOMPONENT_H_

#ifndef _ITF_PHYSFORCEMODIFIER_H_
#include "engine/physics/PhysForceModifier.h"
#endif //_ITF_PHYSFORCEMODIFIER_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_
#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif

#ifndef _ITF_BASICSTATEMACHINE_H_
#include "rayman\gameplay\Components\Shooter\BasicStateMachine.h"
#endif //_ITF_BASICSTATEMACHINE_H_

#ifndef _ITF_RAY_SHOOTERCAMERACOMPONENT_H_
#include "rayman/gameplay/Components/Camera/Ray_ShooterCameraComponent.h"
#endif //_ITF_RAY_SHOOTERCAMERACOMPONENT_H_

#ifndef _ITF_RAY_HEARTSHIELDCOMPONENT_H_
#include "rayman/gameplay/Components/Player/PowerUpDisplays/Ray_HeartShield.h"
#endif //_ITF_RAY_HEARTSHIELDCOMPONENT_H_

#ifndef _ITF_RAY_SWARMREPELLERPOWERUP_H_
#include "rayman/gameplay/Components/Player/PowerUpDisplays/Ray_SwarmRepellerPowerUp.h"
#endif //_ITF_RAY_SWARMREPELLERPOWERUP_H_

#ifndef _ITF_POOLACTORCOMPONENT_H_
#include "gameplay/Components/Common/PoolActorComponent.h"
#endif //_ITF_POOLACTORCOMPONENT_H_

#ifndef _ITF_RAY_SHOOTERVACUUMSKILL_H_
#include "rayman/gameplay/Components/Shooter/Ray_ShooterVacuumSkill.h"
#endif //_ITF_RAY_SHOOTERVACUUMSKILL_H_

#ifndef _ITF_INPUTLISTENER_H_
#include "engine/zinput/ZInputListener.h"
#endif //_ITF_INPUTLISTENER_H_

#ifndef _ITF_BEZIERPATCH_H_
#include "engine/display/BezierPatch.h"
#endif //_ITF_BEZIERPATCH_H_

#ifndef ITF_FINAL
    //#define DEBUG_SHOOTER
#endif //ITF_FINAL

namespace ITF
{
	//*****************************
	//	   SHOOTER ANIM MRK
	//*****************************
    #define MRK_Shoot ITF_GET_STRINGID_CRC(MRK_Shoot,2154153497)
	
	//=======================================================================================================================
	//												SHOOTER CONTROLLER
	//=======================================================================================================================

	class Ray_ShooterControllerComponent : public Ray_BasicPlayerControllerComponent, public IInputListener 
    {		
        DECLARE_OBJECT_CHILD_RTTI(Ray_ShooterControllerComponent, Ray_BasicPlayerControllerComponent,3971257716)
        DECLARE_VALIDATE_COMPONENT()

#include "Ray_ShooterControllerStates.h"
    public:
        DECLARE_SERIALIZE();

		//*********************************
		//		SHOOTER CONSTANTS
		//*********************************
		/*static const u32	VaccumButton	=	m_joyButton_A;
		static const u32	ShootButton		=	m_joyButton_X;*/
		
		//*****************************
		//		SHOOTER STATES
		//*****************************
		enum SHOOTER_PLAYER_STATES
		{
			STATE_WAIT_PLAYER,
            
			STATE_NORMAL,					// <= Normal FIRST of the group
            STATE_NORMAL_RIDE_ON,
			STATE_NORMAL_CHOOSE,
			STATE_NORMAL_CHOOSE_ATTACK,
			STATE_NORMAL_CHOOSE_VACCUM,
			STATE_NORMAL_CANTVACCUM,		// <= Normal LAST of the group
			
			STATE_VACCUM_START,				// <= Vaccum FIRST of the group
			STATE_VACCUM_LOOP, 
            STATE_VACCUM_LOOP_CATCH,
			STATE_VACCUM_CATCH, 
			STATE_VACCUM_FINISHED,
            STATE_VACCUM_FINISHED_CATCH,
			STATE_VACCUM_FINISHED_FROM_START,
			STATE_VACCUM_FINISHED_NORMAL,
			STATE_VACCUM_FINISHED_FULL,
			STATE_VACCUM_QUICK,				// <= Vaccum LAST of the group

			STATE_SPIT_START,				// <= Spit FIRST of the group
			STATE_SPIT_END,
			STATE_SPIT_TO_STAND,			
            STATE_SPIT_BASESHOOT,           
            STATE_SPIT_BASESHOOT_RESTART,   // <= Spit LAST of the group

			STATE_ATTACK_START,
            STATE_ATTACK_COMBO,

			STATE_HIT,
            STATE_DEATH,                    // <= Death FIRST of the group
            STATE_DEATH_BUBBLE,             // <= Death LAST of the group

            STATE_BUBBLE,                   // <= Bubble FIRST of the group
            STATE_BUBBLE_EXIT,              // <= Bubble LAST of the group

            STATE_SEQUENCE_LAND_TRAVEL,     // <= Sequence FIRST of the group
            STATE_SEQUENCE_LAND_EXIT,       // <= Sequence LAST of the group
			
			STATE_COUNT,
			ENUM_FORCE_SIZE_32(SHOOTER_PLAYER_STATES)
		};

		// GROUPS
		//--------
		#define STATE_GROUP_NORMAL		        STATE_NORMAL,		        STATE_NORMAL_CANTVACCUM
		#define STATE_GROUP_VACCUM		        STATE_VACCUM_START,         STATE_VACCUM_QUICK
        #define STATE_GROUP_VACCUM_FINISHED		STATE_VACCUM_FINISHED,      STATE_VACCUM_FINISHED_FULL
		#define STATE_GROUP_SPIT		        STATE_SPIT_START,	        STATE_SPIT_BASESHOOT_RESTART
        #define STATE_GROUP_DEATH		        STATE_DEATH,	            STATE_DEATH_BUBBLE
        #define STATE_GROUP_BUBBLE		        STATE_BUBBLE,	            STATE_BUBBLE_EXIT
        #define STATE_GROUP_SEQUENCE_LAND       STATE_SEQUENCE_LAND_TRAVEL,	STATE_SEQUENCE_LAND_EXIT


        // capacity flags 
		enum Shooter_Player_CapacityFlags
		{
			canMove		= 0x00000001,
            cannotMove	= 0x00000002,
            comboNext	= 0x00000004,
            ENUM_FORCE_SIZE_32(Shooter_Player_CapacityFlags)
		};

        // Player modes are global (can touch every states)
        enum Shooter_Player_Modes
        {
            normal		        = 0,
            cameraTransition,
            ENUM_FORCE_SIZE_32(Shooter_Player_Modes)
        };

		//*********************************
		//		SHOOTER DECLARATION
		//*********************************
        Ray_ShooterControllerComponent();
        virtual ~Ray_ShooterControllerComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        Update(f32 _deltaTime);
        virtual void        updateAnimInput();
        virtual void        updateSoundInput();
        virtual void        onEvent( Event* _event );
        virtual void        onBecomeActive();
        virtual void        onBecomeInactive();
        virtual void        onActorClearComponents();
        virtual void        onResourceReady();
        //virtual void        Draw();

#ifdef ITF_SUPPORT_EDITOR
        virtual void        drawEdit( class ActorDrawEditInterface* _drawInterface, u32 _flags ) const;
        virtual void        onEditorMove(bbool _modifyInitialPos = btrue);
#endif // ITF_SUPPORT_EDITOR

        ITF_INLINE bbool	canCharge() const { return btrue; }

		// Capacity flags
		ITF_INLINE bbool	hasCapacityFlag( Shooter_Player_CapacityFlags const & _flag ) const		{ return ( _flag & m_capacityFlags ); }
		ITF_INLINE void		setCapacityFlag( Shooter_Player_CapacityFlags const & _flag )			{ m_capacityFlags |= _flag; }
		ITF_INLINE void		removeCapacityFlag( Shooter_Player_CapacityFlags const & _flag )		{ m_capacityFlags &= ~_flag; }
		ITF_INLINE void		resetCapacityFlag()														{ m_capacityFlags = 0; }

        virtual bbool       isDead() const { return m_isDead; }
        virtual bbool       isDetectable( const ActorRef _detector ) const;

    protected:
        virtual void        registerDatas();

        void                Receive( u32 _deviceID /* player */, f32 _axis, const StringID& _action );

    private:

        enum WaterSNDType
        {
            None = 0,
            DiveIn,
            DiveOut,
            Surf,
            SurfUnder,
            ENUM_FORCE_SIZE_32( WaterSNDType )
        };

        const class Ray_ShooterControllerComponent_Template* getTemplate() const;

        void				onAnimEvent( AnimGameplayEvent * _animEvent );
		void				onAnimPolylineEvent( AnimPolylineEvent * _animPolylineEvent );
        void				onCameraEvent( Event * _cameEvent );

        void                processPlayerActivationChanged( EventPlayerActivationChanged* _eventActivation );
        void                processGameModeChanged( EventPlayerModeChanged* _eventGameMode );
        void                processPlayerHpChanged( EventPlayerHpChanged* _eventHP );
        void                processCheckpointReached( EventCheckpointReached* _eventCheckpoint );
        void                processRevertToLastCheckpoint( EventRevertToLastCheckpoint* _eventRevert );
        void                processTeleport( EventTeleport* _eventTeleport );
        void                processStopPlayer( bbool _stop );
        void                processPowerUp(const Ray_EventPowerUp* _powerUp);
        void                processPlayerIndexChange( class EventPlayerIndexChange* _playerIndex );
        void                processLandingRequested( class Ray_EventShooterLandingRequested* _landingRequested );
        void                processBounceToLayer( class Ray_EventBounceToLayer * _bounceToLayer );

        void                initialize();
        void                updateModes( f32 _deltaTime );
        void                updateStates( f32 _deltaTime );
        void                updateMove( f32 _deltaTime );
        void                updateClamp( f32 _deltaTime );
        void                updateAnimBlend( f32 _deltaTime );

        void                processCollisions( f32 _deltaTime );
        bbool               checkOutOfScreen( f32 _deltaTime );
        /*bbool               checkCrushedByPolyline();
        bbool               checkHurtByPolyline();*/
        void                checkContacts();
        void                processHitStim( PunchStim* _hit );
        void                processSolidStim( SolidStim* _solid );
        void                processVacuumStim( VacuumStim* _solid );
        void                processBounceStim( BounceStim * _bounceStim );
        void                disablePlayer( bbool _disable );

        bbool               isImmune();
        void                setImmuneTime( f32 _duration, bbool _processHitAction = bfalse );
        void                receiveDamage( i32 _numHP );
        void                die();
		void                revive();
		Vec2d				getUndeadActorPos();
        void                bounceBackFromHit( const Vec2d& _hitDirection, bbool _friendlyHits = bfalse );
        void                blinkWhenHurt( f32 _deltaTime );
        ITF_INLINE bbool    isPlayerDisable() const             { return m_playerDisabled; }
        bbool               isPlayerInShooterMode() const;
        
        bbool               canChangeMode( Shooter_Player_Modes _dest_mode, f32 _dest_Depth );
        void                checkWaterRegion();
        void                setCurrentWaterPolyline( const class PolyLine * _polyline );
        void                setCurrentFluidContactPolyline( const class PolyLine * _polyline );
        void                updateFluidStickToEdge();
                
        void                shoot();
		bbool				hasAmmo();
		void				performHit();

        void                getSwallowPos(Vec3d & _pos);
        void                spit();
		ITF_INLINE VacuumSkill	*			getVacuumSkill()	{ return &m_vacuumSkill; }
		ITF_INLINE PlayerStateMachine *		getStateMachine()	{ return &m_stateMachine; }
        ITF_INLINE const PlayerStateMachine * getStateMachine() const	{ return &m_stateMachine; }

        ITF_INLINE const Vec2d& getMoveListener()           const { return m_listenerMove; }
        ITF_INLINE const bbool  getVacuumListener()         const { return m_listenerVacuum; }
        ITF_INLINE const bbool  getVacuumHoldListener()     const { return m_listenerVacuumHold; }
        ITF_INLINE const bbool  getVacuumReleasedListener() const { return m_listenerVacuumReleased; }

        ITF_INLINE const bbool  getShootListener()          const { return m_listenerShoot; }
        ITF_INLINE const bbool  getShootHoldListener()      const { return m_listenerShootHold; }
        ITF_INLINE const bbool  getLeaveListener()          const { return m_listenerLeave; }

        void                cleanListener();
        void                addListener();
        void                removeListener();

        bbool               m_playerDisabled;

		// buttons
        f32					m_vaccumButtonPressedDuration;
        f32					m_shootButtonPressedDuration;
		
		bbool				m_isDead;
		f32					m_isDeadWaitRespawnTimer;
        bbool               m_justActivatedDead;
        bbool               m_gameModeJustChanged;
        // Vacuumed data
        bbool               m_hasBeenVacuumed;
        f32                 m_isVacuumedTimer;
        bbool               m_isUnderVacuumInfluence;
        f32                 m_isUnderVacuumInfluenceForce;
        ActorRef            m_isUnderVacuumInfluenceOwner;    
        
        bbool               m_isInWater;
        bbool               m_downInWater;
        ObjectRef           m_waterPolyline;
        WaterSNDType        m_waterSNDType;
        u32                 m_waterSndSurfHandle;
        u32                 m_waterSndGlobalHandle;
        SafeArray<u32,4>    m_fluidContactEdgeList;
        ObjectRef           m_fluidContactPolyline;
                
        // immunity after getting hurt
        f32                 m_hurtTimer;
        bbool               m_hurtImmunityProcessHitAction;
        u32                 m_gunBoneIndex;
        u32                 m_swallowBoneIndex;
        u32                 m_lastHitFrame;
        
        ActorRef            m_invincibleAgainstActor;
        f32                 m_invincibleTimer;
        f32                 m_outofScreenTimer;

        AIUtils::HitShape   m_shape;

		// vacuum
		VacuumSkill			m_vacuumSkill;
		f32					m_vacuumRestartTimer;

        Vec2d               m_moveInput;

        bbool               m_stateWantedMove;
        Vec2d				m_stateWantedMoveVector;            // computed by states
        f32				    m_stateWantedLocalSpeedMax;
        Vec2d				m_stateWantedMoveInput;

        Vec2d				m_moveVector;
        f32                 m_moveVaccumInertiaTimer;
        Vec2d               m_borderScreenNomal;
        f32				    m_moveSpeed;
        Vec2d               m_lastValidInputDir;

		u32					m_capacityFlags;					// bit field

		PlayerStateMachine	m_stateMachine;

		u32					m_temp_AnimFinishedCount;

        // anim related
        f32                 m_animMoveBlendCursor;
        f32                 m_animMoveXCursor;
        f32                 m_animMoveYCursor; 

        // mode
        Shooter_Player_Modes    m_curMode;
        
        // camera transition mode params
        ShooterCameraModifier_Transition    m_camTransMode_Transition;
        f32                                 m_camTransMode_ChangeDepthDest;
        f32                                 m_camTransMode_ChangeDepthStartValue;

        // basic Bullets
        ActorPool                   m_baseBulletPool;
        SpawneeGenerator            m_deathFXGenerator;
        
        // power up
        Ray_HeartShield             m_heartShield;
        Ray_SwarmRepellerPowerUp	m_swarmRepellerPowerUp;

        // input listener 
        bbool                       m_listenerAdded;

        Vec2d                       m_listenerMove; 
        bbool                       m_listenerLeave;

        // TWO BUTTONS Check mode
        bbool                       m_listenerShoot;
        bbool                       m_listenerShootHold;
        bbool                       m_listenerVacuum;
        bbool                       m_listenerVacuumHold;              
        bbool                       m_listenerVacuumReleased;
        bbool                       m_listenerTest;

        // vacuum Patch 
        /*ResourceID                  m_vacuumPatchTexture;
        BezierPatch                 m_vacuumPatch;
        f32 m_test;*/

#ifdef DEBUG_SHOOTER
        f64                         m_dbg_fireRateLastFireTime;
        f32                         m_dbg_fireRate;
#endif //DEBUG_SHOOTER
    };

	
    //-------------------------------------------------------------------------------------
    class Ray_ShooterControllerComponent_Template : public Ray_BasicPlayerControllerComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_ShooterControllerComponent_Template,Ray_BasicPlayerControllerComponent_Template,687932757);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_ShooterControllerComponent);

    public:

       /* struct VacuumPatchData
        {
            VacuumPatchData();
            DECLARE_SERIALIZE();
            Vec2d   m_startUV;
            Vec2d   m_endUV;
            f32     m_startWidth;
            f32     m_endWidth;
            u32     m_uvMode;
            u32     m_blendMode;
            u32     m_hdiv;
            u32     m_vdiv;        
        };*/

        Ray_ShooterControllerComponent_Template();
        ~Ray_ShooterControllerComponent_Template() {}

        ITF_INLINE u32                  getAnimBankState() const { return m_animBankState; }
        ITF_INLINE f32                  getMinLocalSpeed() const { return m_minLocalSpeed; }
        ITF_INLINE f32                  getMaxLocalSpeed() const { return m_maxLocalSpeed; }
		ITF_INLINE f32                  getVaccumMoveSpeedBlendFactor() const { return m_vaccumMoveSpeedBlendFactor; }
        ITF_INLINE f32                  getDefaultMoveSpeedBlendFactor() const { return m_defaultMoveSpeedBlendFactor; }
        ITF_INLINE f32                  getDefaultMoveSpeedInertialBlendFactor() const { return m_defaultMoveSpeedInertialBlendFactor; }
        ITF_INLINE f32                  getWaterMoveSpeedBlendFactor() const { return m_waterMoveSpeedBlendFactor; }
        ITF_INLINE f32                  getWaterMoveSpeedInertialBlendFactor() const { return m_waterMoveSpeedInertialBlendFactor; }
        ITF_INLINE f32                  getWaterCheckRadius() const { return m_waterCheckRadius; }
        ITF_INLINE f32                  getWaterResistanceForce() const { return m_waterResistanceForce; }
        ITF_INLINE f32                  getEjectFromBorderForce() const { return m_ejectFromBorderForce; }
        ITF_INLINE f32                  getEjectFromHitForce() const { return m_ejectFromHitForce; }
        ITF_INLINE f32                  getEjectFromFriendlyHitForce() const { return m_ejectFromFriendlyHitForce; }
        ITF_INLINE const Vec2d&         getEjectMarginX() const { return m_ejectMarginX; }
        ITF_INLINE const Vec2d&         getEjectMarginY() const { return m_ejectMarginY; }
        ITF_INLINE const Vec2d&         getDeathMargin() const { return m_deathMargin; }
        ITF_INLINE f32                  getOutOfScreenDurationBeforeDeath() const { return m_outOfScreenDurationBeforeDeath; }
        ITF_INLINE f32                  getHurtImmunityDuration() const { return m_hurtImmunityDuration; }
        ITF_INLINE f32                  getReviveImmunityDuration() const { return m_reviveImmunityDuration; }
        ITF_INLINE u32                  getHurtBlinkCount() const { return m_hurtBlinkCount; }
        ITF_INLINE f32                  getHurtBlinkMinAlpha() const { return m_hurtBlinkMinAlpha; }
        ITF_INLINE f32                  getHurtBlinkMaxAlpha() const { return m_hurtBlinkMaxAlpha; }
        ITF_INLINE const StringID &     getGunBone() const { return m_gunBone; }
        ITF_INLINE const StringID &     getSwallowBone() const { return m_swallowBone; }
        ITF_INLINE f32                  getInvincibilityTime() const { return m_invincibilityTime; }
        ITF_INLINE const VacuumSkill_Template&	getVacuumSkill() const { return m_vacuumSkill; }
		ITF_INLINE const Vec2d&		    getSpitObjSpeed()	const	{ return m_spitObjSpeed; }
        
        ITF_INLINE const Path &			getBaseSpawnBulletPath()	const	{ return m_baseSpawnBulletPath; }
        ITF_INLINE const Path &			getDeathSpawnFXPath()	    const	{ return m_deathSpawnFXPath; }

        ITF_INLINE const Ray_HeartShield_Template&              getHeartShield() const { return m_heartShield; }
        ITF_INLINE const Ray_SwarmRepellerPowerUp_Template &    getSwarmRepellerPowerUp() const { return m_swarmRepellerPowerUp; }

  		ITF_INLINE f32                  getBubbleStateBlendFactor() const { return m_bubbleStateBlendFactor; }
        ITF_INLINE f32                  getBubbleStateLocalSpeedMax() const { return m_bubbleStateLocalSpeedMax; }
        ITF_INLINE f32                  getBubbleStatePhantomRadius()const { return m_bubbleStatePhantomRadius; }

        ITF_INLINE f32                  getDjembeBounceForce() const { return m_djembeBounceForce; }
        ITF_INLINE f32                  getBounceStimDefaultForce() const { return m_bounceStimDefaultForce; }
        
        ITF_INLINE f32                  getVacuumedResistanceDurationMin() const { return m_vacuumedResistanceDurationMin; }
        ITF_INLINE f32                  getVacuumedResistanceDurationMax() const { return m_vacuumedResistanceDurationMax; }
        ITF_INLINE f32                  getAutoFireDelay()const { return m_autoFireDelay; }

		// Debug data
		ITF_INLINE bbool				getDBG_CanVaccumIfFull()const	{ return m_DBG_CanVaccumIfFull; }
		ITF_INLINE bbool				getDBG_VaccumAllowPressStop()const	{ return m_DBG_VaccumAllowPressStop; }

        /*ITF_INLINE const Path &	        getVacuumPatchTextureFile()const	{ return m_vacuumPatchTextureFile; }

        ITF_INLINE const VacuumPatchData & getVacuumPatchData() const { return m_vacuumPatchData; }*/

    private:

        u32                             m_animBankState;                    // is the animation bank state of the actor
        f32                             m_minLocalSpeed;
        f32                             m_maxLocalSpeed;
		f32								m_vaccumMoveSpeedBlendFactor;		// Is the move speed blend factor during vaccum
        f32                             m_defaultMoveSpeedInertialBlendFactor;// is the move speed interia blend factor
        f32								m_defaultMoveSpeedBlendFactor;		// Is the move speed blend factor during by default
        f32                             m_waterMoveSpeedInertialBlendFactor;// is the move speed interia blend factor in water
        f32                             m_waterMoveSpeedBlendFactor;        // Is the move speed blend factor in water
        f32                             m_waterCheckRadius;
        f32                             m_waterResistanceForce;
        f32                             m_ejectFromBorderForce;             // move away from screen borders
        f32                             m_ejectFromHitForce;                // move away from hit (enemy bullet or dangerous material)
        f32                             m_ejectFromFriendlyHitForce;        // move away from hit (friendly bullet)
        Vec2d                           m_ejectMarginX;                     // push the player away if he's too close to the screen edges left/right (in screen %)
        Vec2d                           m_ejectMarginY;                     // push the player away if he's too close to the screen edges up/down (in screen %)
        Vec2d                           m_deathMargin;                      // kill the player if he goes past the screen edges (in screen %)
        f32                             m_outOfScreenDurationBeforeDeath;   // duration out of screen before death
        f32                             m_hurtImmunityDuration;             // how long are we invincible after getting hit?
        f32                             m_reviveImmunityDuration;           // how long are we invincible after revive ?
        u32                             m_hurtBlinkCount;
        f32                             m_hurtBlinkMinAlpha;
        f32                             m_hurtBlinkMaxAlpha;
        StringID                        m_gunBone;
        StringID                        m_swallowBone;
        f32                             m_invincibilityTime;
        VacuumSkill_Template	        m_vacuumSkill;
		Vec2d							m_spitObjSpeed;
        
        Path                            m_baseSpawnBulletPath;
        Path                            m_deathSpawnFXPath;

        // power ups
        Ray_HeartShield_Template        m_heartShield;
        Ray_SwarmRepellerPowerUp_Template m_swarmRepellerPowerUp;

  		f32                             m_bubbleStateBlendFactor;
        f32                             m_bubbleStateLocalSpeedMax;
        f32                             m_bubbleStatePhantomRadius;

        f32                             m_djembeBounceForce;
        f32                             m_bounceStimDefaultForce;
        f32                             m_vacuumedResistanceDurationMin;
        f32                             m_vacuumedResistanceDurationMax;

        f32                             m_autoFireDelay;

		// Debug data 
		bbool							m_DBG_CanVaccumIfFull;		
		bbool							m_DBG_VaccumAllowPressStop;		

   /*     Path                            m_vacuumPatchTextureFile;
        VacuumPatchData                 m_vacuumPatchData;*/
    };

    ITF_INLINE const Ray_ShooterControllerComponent_Template* Ray_ShooterControllerComponent::getTemplate() const
    {
        return static_cast<const Ray_ShooterControllerComponent_Template*>(m_template);
    }
};

#endif //_ITF_RAY_SHOOTERCONTROLLERCOMPONENT_H_
