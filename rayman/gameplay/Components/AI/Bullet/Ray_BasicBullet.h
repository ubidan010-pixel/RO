#ifndef _ITF_RAY_BASICBULLET_H_
#define _ITF_RAY_BASICBULLET_H_

#ifndef _ITF_EVENTLISTENER_H_
#include "engine/events/IEventListener.h"
#endif //_ITF_EVENTLISTENER_H_

#ifndef _ITF_RAY_GAMEMATERIAL_H_
#include "rayman/gameplay/Ray_GameMaterial.h"
#endif //_ITF_RAY_GAMEMATERIAL_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_


namespace ITF
{     
	class Ray_BasicBullet : public IRTTIObject, public IEventListener
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_BasicBullet,IRTTIObject,21844882);

    public:
        DECLARE_SERIALIZE();

        Ray_BasicBullet();
        virtual ~Ray_BasicBullet();

		void					init( Actor * _actor, const class Ray_BasicBullet_Template * _bulletTemplate );
        virtual void			update( f32 _dt );
        virtual void			onEvent( Event* _event );
		virtual void			onBecomeActive();
		virtual void			onBecomeInactive();
		
		ITF_INLINE void			setMoveDir( const Vec2d & _dir )		{ m_moveDir = _dir; }
		ITF_INLINE void			setMoveSpeed( const f32 & _speed )		{ m_moveSpeed = _speed; }
		ITF_INLINE Vec2d		getMoveDir() const						{ return m_moveDir; }
		ITF_INLINE f32			getMoveSpeed() const 					{ return m_moveSpeed; }
        ITF_INLINE void			setOwner( const ActorRef & _owner )		{ m_owner = _owner; }
        void			        setFaction( Faction _faction );
        ITF_INLINE const Faction getFaction()	    const;
        ITF_INLINE const Vec2d & getDefaultMoveSpeed() const;
        ITF_INLINE const bbool  getDestroyOnExitScreen() const;
        
		ITF_INLINE bbool		isObsolete()		const	{ return m_isObsolete; }
        ITF_INLINE bbool		hasHit()		    const	{ return ( m_isObsoleteFromHit || isEnvTouched() ); }
        ITF_INLINE bbool		hasExitScreen()		const	{ return m_isObsoleteFromExitScreen; }
		ITF_INLINE bbool		isEnvTouched()		const	{ return ( m_envTouchedContacts.size() > 0 ); }
		ITF_INLINE const PhysContactsContainer &	getEnvTouchedContacts()		const { return m_envTouchedContacts; }
        void                    setPosOutOfEnvCollisions();
        ITF_INLINE bbool        isVisible() const { return CAMERA->isRectVisible( m_actor->getAABB(), m_actor->getDepth() ); }
        
	protected:
        enum FXInputType
        {
            FXInput_Bounce,
            FXInput_Impact,
            FXInput_Paf,
            FXInput_Death,
        };

        const class PhysShape *	getShape();
        class PunchStim *       setupStim();
		void					sendStim();
        bbool					checkCharactersContacts();
		void					checkEnvContacts();
        void                    filterContacts();
		void					onHitSuccessful(EventHitSuccessful* _hitSuccessful);
        void					onNotifyStim( EventStimNotify* _notifyStim );
		bbool	                bounce();
        void	                playFXInput( FXInputType _type );
        bbool                   computeEnvTouchedData( const SCollidableContact & _contact );

		Vec2d                   m_moveDir;
		f32					    m_moveSpeed;
        Vec2d                   m_prevMoveDir;
        
        Vec2d                   m_prevPos;
        f32                     m_timer;
        bbool                   m_sendStims;
        u32                     m_currentNumHits;
        ActorRef                m_lastHitRef;
        u32                     m_beforeStimNumHits;
		bbool				    m_isObsolete;
        bbool				    m_isObsoleteFromHit;
        bbool				    m_isObsoleteFromExitScreen;
		PhysShapeCircle		    m_physComponentShape;
        ActorRef                m_lastBounceObj;  
        bbool                   m_hitSuccessfullEvtBounce;
        bbool                   m_hitSuccessfullEvtImpact;
        ActorRef                m_hitSuccessfullTouchedActor;
        u32                     m_bounceCount;
		
		PhysContactsContainer	        m_envTouchedContacts;
        const Ray_GameMaterial_Template * m_envTouchedContactsGmat;
        Vec2d                           m_envTouchedImpactPos;
        Vec2d                           m_envTouchedImpactNormal;
        ObjectRef                       m_envTouchedActor;
        SafeArray<ObjectRef, 2>         m_localCharContactProcessed;

	private :
		ITF_INLINE const class Ray_BasicBullet_Template * getTemplate() const;

        void                    updateAutoSeek( f32 _dt );

		const class Ray_BasicBullet_Template *	m_template;
		class Actor *			                m_actor;
		class AIComponent *		                m_aiComponent;
        class FXControllerComponent *           m_fxControllerComponent;
		Faction					                m_prevFaction;
        Faction                                 m_specificFaction;
		bbool					                m_active;
        ActorRef                                m_owner;
		bbool                                   m_autoSeekStarted;
        f32                                     m_autoSeekTimer;
        ActorRef                                m_autoSeekTarget;
    };

    //-------------------------------------------------------------------------------------
	class Ray_BasicBullet_Template : public IRTTIObject
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_BasicBullet_Template,IRTTIObject,3221367250);
        DECLARE_SERIALIZE();
    public:

        Ray_BasicBullet_Template();
        ~Ray_BasicBullet_Template();

        ITF_INLINE const PhysShape* getStimShape()					const { return m_stimShape; }
        ITF_INLINE Faction 			getFaction()					const { return m_faction; }
        ITF_INLINE f32				getLifetime()					const { return m_lifetime; }
        ITF_INLINE EReceivedHitType getHitType()					const { return m_hitType; }
        ITF_INLINE u32              getHitLevel()					const { return m_hitLevel; }
        ITF_INLINE bbool			getDestroyOnExitScreen()		const { return m_destroyOnExitScreen; }
        ITF_INLINE u32				getNumHits()					const { return m_numHits; }
        ITF_INLINE bbool            getSkipSuccessiveHits()         const { return m_skipSuccessiveHits; }
		ITF_INLINE bbool			getDestroyOnEnvironment()		const { return m_destroyOnEnvironment; }
		ITF_INLINE bbool			getCheckEnvironment()			const { return m_checkEnvironment; }
        ITF_INLINE bbool			getCheckEnvironmentHit()		const { return m_checkEnvironmentHit; }
		ITF_INLINE const Vec2d &	getDefaultSpeed()				const { return m_defaultSpeed; }
		ITF_INLINE f32				getRotationSpeed()				const { return m_rotationSpeed; }
        ITF_INLINE bbool			getAllowBounce()			    const { return m_allowBounce; }
        ITF_INLINE u32			    getBounceMaxNb()			    const { return m_bounceMaxNb; }
        ITF_INLINE bbool			isCameraRelative()			    const { return m_cameraRelative; }
        ITF_INLINE bbool            getAutoSeek()                   const { return m_autoSeek; }
        ITF_INLINE f32              getAutoSeekStartDelay()         const { return m_autoSeekStartDelay; }
        ITF_INLINE f32              getAutoSeekRefreshDelay()       const { return m_autoSeekRefreshDelay; }
        ITF_INLINE f32              getAutoSeekRange()              const { return m_autoSeekRange; }
        ITF_INLINE Angle            getAutoSeekMaxTurnAngle()       const { return m_autoSeekMaxTurnAngle; }
        
        ITF_INLINE const StringID & getFxNameImpact()               const { return m_fxNameImpact; }
        ITF_INLINE const StringID & getFxNameBounce()               const { return m_fxNameBounce; }
        ITF_INLINE const StringID & getFxNamePaf()                  const { return m_fxNamePaf; }
        ITF_INLINE const StringID & getFxNameDeath()                const { return m_fxNameDeath; }
        ITF_INLINE const bbool      getFxKillAllBefore()            const { return m_fxKillAllBefore; }

    private:

        PhysShape*          m_stimShape;
        
		Faction             m_faction;
        f32                 m_lifetime;
        EReceivedHitType	m_hitType;
        u32                 m_hitLevel;
		u32                 m_numHits;
        bbool               m_skipSuccessiveHits;

		bbool               m_destroyOnExitScreen;
		bbool				m_destroyOnEnvironment;
        bbool				m_checkEnvironment;
        bbool				m_checkEnvironmentHit;
        bbool				m_allowBounce;
        u32                 m_bounceMaxNb;

		Vec2d				m_defaultSpeed;
		f32					m_rotationSpeed;
        bbool               m_cameraRelative;
		
        bbool               m_autoSeek;
        f32                 m_autoSeekStartDelay;
        f32                 m_autoSeekRefreshDelay;
        f32                 m_autoSeekRange;
        Angle               m_autoSeekMaxTurnAngle;

        StringID            m_fxNameImpact;
        StringID            m_fxNameBounce;
        StringID            m_fxNamePaf;
        StringID            m_fxNameDeath;
        bbool               m_fxKillAllBefore;
    };

    ITF_INLINE const class Ray_BasicBullet_Template * Ray_BasicBullet::getTemplate() const
    {
        return static_cast<const class Ray_BasicBullet_Template *>(m_template);
    }

    ITF_INLINE const Faction Ray_BasicBullet::getFaction() const
    {
        if ( getTemplate() != NULL )
        {
            return ( m_specificFaction != FACTION_UNKNOWN ) ? m_specificFaction : getTemplate()->getFaction();
        }
        return FACTION_UNKNOWN;
    }
    
    ITF_INLINE const Vec2d & Ray_BasicBullet::getDefaultMoveSpeed() const
    {
        if ( getTemplate() != NULL )
        {
            return getTemplate()->getDefaultSpeed();
        }
        return Vec2d::Zero;
    }
    
    ITF_INLINE const bbool Ray_BasicBullet::getDestroyOnExitScreen() const
    {
        if ( getTemplate() != NULL )
        {
            return getTemplate()->getDestroyOnExitScreen();
        }
        return bfalse;
    }

};

#endif //_ITF_RAY_BASICBULLET_H_