#ifndef _ITF_RAY_SHOOTERACTORPARAMETERCOMPONENT_H_
#define _ITF_RAY_SHOOTERACTORPARAMETERCOMPONENT_H_

#ifndef _ITF_RAY_AICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_AIComponent.h"
#endif //_ITF_RAY_AICOMPONENT_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif

namespace ITF
{
    //---------------------------------------------------------------------------------------------------
    // Is the vacuum abilitie data template
    //
    class Ray_VacuumData_Template
    {
        DECLARE_SERIALIZE();
    public :
        Ray_VacuumData_Template();
        ~Ray_VacuumData_Template(){}
        ITF_INLINE  bbool getCanBeVacuumed()			const { return m_canBeVacuumed; }
        ITF_INLINE  f32 getVacuumMinDuration()			const { return m_vacuumMinDuration; }
        ITF_INLINE  f32 getVacuumMaxDuration()			const { return m_vacuumMaxDuration; }

    private:
        bbool                       m_canBeVacuumed;
        f32                         m_vacuumMinDuration;
        f32                         m_vacuumMaxDuration;
    };

    //---------------------------------------------------------------------------------------------------
    // stack info data class
    class Ray_ShooterActorParameter_StackData
    {
        DECLARE_SERIALIZE();
    public :
        Ray_ShooterActorParameter_StackData() : m_inStackSize( U32_INVALID ), m_numProjectiles( 1 ), m_max( 0 ), m_notStackable( bfalse ), m_notStackableKeepAlive( bfalse ) {}
        ITF_INLINE bbool needProjectileSpawnee()	const {	return ( !m_projectilePath.isEmpty() ); }
        ITF_INLINE bbool isValid()                  const { return ( m_inStackSize != U32_INVALID );  }

        StringID    m_type;                 // Type of the object
        Path        m_projectilePath;       // generate a spawned object ?
        u32         m_numProjectiles;       // number of spawned object generated
        u32         m_max;                  // number of object with same type stacked
        u32         m_inStackSize;          // size of the stacked object
        bbool       m_notStackable;         // unstackable obj ?
        bbool       m_notStackableKeepAlive;// unstackable but keep alive the actor 
    };

    //---------------------------------------------------------------------------------------------------
    class Ray_ShooterActorParameterComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI( Ray_ShooterActorParameterComponent, ActorComponent ,4085444954)

    public:
        DECLARE_SERIALIZE()

        enum vacuumFxType
        {
            Vacuuming,
            VacuumingStop,
            Detached,
            Ejected,
            ENUM_FORCE_SIZE_32( vacuumFxType )
        };

		Ray_ShooterActorParameterComponent();
		virtual ~Ray_ShooterActorParameterComponent();

        virtual void    onActorLoaded(Pickable::HotReloadType /*_hotReload*/);

        virtual bbool	needsUpdate() const   { return btrue; }
        virtual bbool	needsDraw() const     { return bfalse; }
        virtual bbool	needsDraw2D() const   { return bfalse; }
        virtual void    onBecomeInactive();

		virtual void	Update( f32 _dt );
		virtual void	onEvent( Event * _event);
		virtual void	onUpdatePos();
		//virtual void  onResourceReady();
        ITF_INLINE const Ray_ShooterActorParameter_StackData &  getStackData() const;
        ITF_INLINE Actor *                                      getGeneratedProjectile( class Scene * _scene, const Vec3d & _pos, const f32 & _angle )        { return ( m_projectileGenerator.isInitialized() ? m_projectileGenerator.getSpawnee( _scene, _pos, _angle ) : NULL ); }
        void                                                    get2DPosWithoutVacuumInfluence( Vec2d & _resPos ) const;

    protected:
        void            reset();
        void			playVacuumFx( vacuumFxType _type );
        void			updateVacuumAnim( f32 _dt );
        void            updateVacuumGauge( f32 _dt );
        void            giveDeathReward();

        bbool           canChangeToBehavior( AIBehavior * _bvh );
        bbool           canBeVacuumed();
        void            onVacuumed();

        void            ProcessEventIsInfluencedByVacuum( Ray_EventIsInfluencedByVacuum * _evt );
        void            ProcessEventCanBeVacuum( Ray_EventCanBeVacuum * _evt );
        void            ProcessEventShooterEjectActor( Ray_EventShooterEjectActor * _evt );
        void            ProcessVacuumStim( VacuumStim * _evt );
        void            ProcessEventShooterSwallow( Ray_EventShooterSwallow * _evt );

    private:

        ITF_INLINE const class Ray_ShooterActorParameterComponent_Template*  getTemplate() const;
		
		Ray_AIComponent*	            m_rayAiComponent;
		PhysComponent *		            m_physComponent;
        class FXControllerComponent *   m_fxControllerComponent;
        class TweenComponent *          m_tweenComponent;

		AIBehavior *		m_playerStartVacuumBehavior;
        AIBehavior *		m_playerVacuumBehavior;
        AIBehavior *		m_playerVacuumEffectiveBehavior;
        AIBehavior *		m_playerEjectBehavior;

		bbool				m_vacuuming;
        bbool				m_wasVacuuming;
        bbool				m_hasBeenVaccumed;
        ActorRef		    m_hasBeenVaccumedOwner;
		f32					m_vacuumSize;
		ActorRef 			m_vacuumer;
		f32					m_vacuumSpeedFactor;

		bbool				m_actorPosUpdated;
		Vec2d				m_vacuumAnimVector;
		Vec2d				m_vacuumAnimDir;
		f32					m_vacuumAnimTimer;
		f32					m_vacuumAnimFactor;
		f32					m_vacuumBlendTimer;

        f32                 m_vacuumingGaugeTimer;
        
        u32                 m_vacuumingFxHandle;
        Ray_EventSpawnReward* m_vacuumedReward;
        SpawneeGenerator	m_deathRewardGenerator;
        bbool               m_deathRewardGiven;
        PunchStim *         m_onVacuumedStim;
        
        PunchStim *         m_onEnemyReVacuumedStim;
        bbool               m_enemyReVacuumed;
        Ray_ShooterActorParameter_StackData * m_enemyReVacuumedStackData;
        
        SpawneeGenerator	m_projectileGenerator;      // stacked obj generated projectile

    };

    //---------------------------------------------------------------------------------------------------

    class Ray_ShooterActorParameterComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI( Ray_ShooterActorParameterComponent_Template, TemplateActorComponent ,2034283474);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_ShooterActorParameterComponent);

    public:

        Ray_ShooterActorParameterComponent_Template();
        virtual ~Ray_ShooterActorParameterComponent_Template();

		ITF_INLINE const Ray_VacuumData_Template *			getVaccumDataTemplate() const       { return &m_vacuumData; }
		ITF_INLINE const TemplateAIBehavior *				getPlayerEjectBehavior() const      { return m_playerEjectBehavior; }
		ITF_INLINE const TemplateAIBehavior *				getVacuumBehavior() const           { return m_playerVacuumBehavior; }
        ITF_INLINE const TemplateAIBehavior *				getStartVacuumBehavior() const      { return m_playerStartVacuumBehavior; }
        ITF_INLINE const TemplateAIBehavior *				getVacuumEffectiveBehavior() const  { return m_playerVacuumEffectiveBehavior; }
        const StringID &		                            getEnemyReVacuumedSwallowedBhvName() const { return m_enemyReVacuumedSwallowedBhvName; }
        
        const StringID &		                            getVacuumFXByType( Ray_ShooterActorParameterComponent::vacuumFxType _type ) const;
        ITF_INLINE const Ray_EventSpawnReward * 			getVacuumedReward() const           { return m_vacuumedReward; }
        ITF_INLINE const Path & 			                getDeathRewardSpawnPath() const     { return m_deathRewardSpawnPath; }
        ITF_INLINE const u32 			                    getDeathRewardNumber() const        { return m_deathRewardNumber; }

        ITF_INLINE const Ray_ShooterActorParameter_StackData & getStackData() const             { return m_stackData; }
        ITF_INLINE const Ray_ShooterActorParameter_StackData * getStackDataPtr() const          { return &m_stackData; }
        ITF_INLINE const PunchStim *		                getOnVacuumedStim() const           { return m_onVacuumedStim; }
        ITF_INLINE const PunchStim *		                getOnEnemyReVacuumedStim() const    { return m_onEnemyReVacuumedStim; }

    private:
		Ray_VacuumData_Template			m_vacuumData;
		class TemplateAIBehavior *      m_playerEjectBehavior;
		class TemplateAIBehavior *      m_playerVacuumBehavior;
        class TemplateAIBehavior *      m_playerStartVacuumBehavior;
        class TemplateAIBehavior *      m_playerVacuumEffectiveBehavior;
        StringID                        m_enemyReVacuumedSwallowedBhvName;
        Ray_EventSpawnReward*           m_vacuumedReward;
        Path                            m_deathRewardSpawnPath;
        u32                             m_deathRewardNumber;
        PunchStim *                     m_onVacuumedStim;
        PunchStim *                     m_onEnemyReVacuumedStim;
        
        struct VacuumFxNames
        {
            DECLARE_SERIALIZE();
            StringID    m_fxNameVacuuming;
            StringID    m_fxNameVacuumingStop;
            StringID    m_fxNameDetached;
            StringID    m_fxNameEjected;
        };
       
        VacuumFxNames                   m_vacuumFxNamesContainer;

        Ray_ShooterActorParameter_StackData m_stackData;
    };


    const Ray_ShooterActorParameterComponent_Template*  Ray_ShooterActorParameterComponent::getTemplate() const {return static_cast<const Ray_ShooterActorParameterComponent_Template*>(m_template);}
    const Ray_ShooterActorParameter_StackData &         Ray_ShooterActorParameterComponent::getStackData()          const { return ( ( m_enemyReVacuumedStackData != NULL ) ? (*m_enemyReVacuumedStackData) : getTemplate()->getStackData() ); }
}

#endif // _ITF_RAY_SHOOTERACTORPARAMETERCOMPONENT_H_

