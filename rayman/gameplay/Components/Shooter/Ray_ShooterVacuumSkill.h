#ifndef _ITF_RAY_SHOOTERVACUUMSKILL_H_
#define _ITF_RAY_SHOOTERVACUUMSKILL_H_

#ifndef _ITF_PHYSFORCEMODIFIER_H_
#include "engine/physics/PhysForceModifier.h"
#endif //_ITF_PHYSFORCEMODIFIER_H_

namespace ITF
{

	//===============================================================================================
	//										VACUUM SKILL
	//===============================================================================================
	/*class BaseSkill
	{
		BaseSkill()
	protected:
		m_actor

	};*/

	class VacuumSkill// : public BaseSkill
	{
	public :

		VacuumSkill();
		virtual ~VacuumSkill();
		
		void				initialize( Actor * _parentActor, AnimatedComponent* _animComponent, const class VacuumSkill_Template* _template );
        void                reset( bbool _destroyObj = bfalse );
        
		bbool               hasAmmo() const { /*return ( m_ammo || ( m_specificProjectiles.size() > 0  ) );*/ return ( m_stackedObjects.size() > 0 ); }
		bbool				isFull( bbool _realStackSizeCheck = bfalse );
		f32					getFillFactor() const;
		u32					getFillValue() const			{ return m_stackFillValue; }
		u32					isFillValueHasChanged() const	{ return m_fillValueHasChanged; }		// Valid one frame (updated in aspiration update) 
		u32					getBiggestObjSize() const;
		u32					getTotalVaccumedSizeFromStart() const { return m_totalVaccumedSizeFromStart; }

		void				processVacuumed( Ray_EventShooterVacuumed * onVacuumed );
        void                addVacuum();
        void                removeVacuum();
		void				aspirate();
		void				aspirationUpdate( f32 _dt, Vec3d const &_startPos );
        void				aspirationFinished();                               // Swallowed all vaccumed actors

		void				updateSpecificProjectiles( Vec3d const & _pos );	// Used to update specific projectiles (vacuumed actors...)

		bbool				hasVacuumedActors() { return ( ( m_actorsBeingVacuumed.size() > 0 ) ? btrue : bfalse ); }
        bbool				isVacuumedActor( ActorRef const & _actor );
		
		Actor*				Shoot( Vec3d const &_startPos, Vec2d const &_dir );
		ITF_INLINE void		setInstantVacuum( bbool _active )		            { m_instantVacuumActive = _active; }
        ITF_INLINE bbool	isActive()	    const	                            { return m_vacuumActive; }
        ITF_INLINE void	    setPowerFactor( f32 _factor )                       { m_powerFactor = _factor; }
        ITF_INLINE f32	    getVacuumWidth()const                               { return m_vacuumWidth; }
        Vec2d	            getVacuumDirectionAndPos( Vec2d & _res_pos )const;        
        

#ifdef ITF_SUPPORT_DEBUGFEATURE
		void				debugDraw() const ;
#endif // ITF_SUPPORT_DEBUGFEATURE

	protected :

		struct VacuumedActor
		{
			ActorRef m_actor;
			f32 m_timer;
            u32 m_stackSize;
            bbool m_hasBeenSwallowed;
		};
		struct StackedObject
		{
			StackedObject() : m_isGenerator( bfalse ), m_arrayIndex( U32_INVALID ), m_stackSize(1)/*, m_shootDataIndex( U32_INVALID )*/ {}
			bbool	    m_isGenerator;
			u32		    m_arrayIndex;
			u32		    m_stackSize;
            StringID    m_type;
		};

        void                sendVacuumStim();
        void                updateVacuumShape();

		void                processSwallowed( VacuumedActor & _vacuumedActor, bbool _finished = bfalse );

        const class Ray_ShooterActorParameter_StackData *   getActorStackData( Actor * _actor );
        class Ray_ShooterActorParameterComponent *          getActorShooterActorParamComponent( Actor * _actor );

		void				stackPop( StackedObject & _popedObj );
		void				stackObject( StackedObject & _stackedObj );
		void				stackFreeSpace( const u32 & _sizeToReserve );
		void				stackRemoveAt( const u32 & _idx, bbool _destroyObj = bfalse );
		i32					stackFindObjIdx( const u32 & _arrayIdx, bbool _isSpecProjectile );	// Find the given array idx (generator or spe projectile)
        i32					stackFindObjTypeIndex( const StringID & _stackData_Type );
		u32					addSpecificProjectile( Actor * _actor );
		void				removeSpecificProjectileAt( const u32 & _idx );

        bbool               addVacuumedActor( Actor * _actor );
        void                removeVacuumedActor( u32 _idx );
        	

		// Internal skill vars
		Actor *						m_actor;
		AnimatedComponent *			m_animComponent;
		
		// Parameters
		PhysForceModifier_Instance	m_vacuumForce;
        f32                         m_vacuumWidth;

		// internal vars
		SafeArray<ActorRef>	        m_specificProjectiles;
        f32                         m_powerFactor;
		
		bbool						m_vacuumActive;
		bbool						m_instantVacuumActive;
		u32							m_totalVaccumedSizeFromStart;
								
		SafeArray<VacuumedActor>	m_actorsBeingVacuumed;
		SafeArray<StackedObject>	m_stackedObjects;
		u32							m_stackFillValue;
		bbool						m_fillValueHasChanged;
        u32                         m_actorsBeingVacuumedFillValue;

        const class VacuumSkill_Template*   m_template;
	};
	
	
    //-------------------------------------------------------------------------------------
    class VacuumSkill_Template// : public BaseSkill
    {
        DECLARE_SERIALIZE();

    public :

        VacuumSkill_Template();
        ~VacuumSkill_Template() {}

        const PhysForceModifier		        getVacuumForce()	const		{ return m_vacuumForce; }
		const PhysForceModifier	*	        getVacuumForcePtr() const		{ return &m_vacuumForce; }
        f32						            getStickDuration()	const		{ return m_stickDuration; }
		u32						            getStackSize()		const		{ return m_stackSize; }
		u32						            getStackFullSize()	const		{ return m_stackFullSize; }
		bbool								isStackInLifoMode() const		{ return m_stackLifoMode; } 
		f32									getVaccumSpeedFactor() const	{ return m_vaccumSpeedFactor; } 
		
		f32									getStateStartDuration()	const { return m_stateStartDuration; }
        f32									getStateStopDuration()	const { return m_stateStopDuration; }
		f32									getStateLoopDuration()	const { return m_stateLoopDuration; }
		f32									getStateBeforeRestartDuration()	const	{ return m_stateBeforeRestartDuration; }

    private:

        // Parameters
        PhysForceModifier		m_vacuumForce;
        f32						m_stickDuration;			// Duration of the vacuumed actor translation
		u32						m_stackSize;				// Is max allocated space for the stack
		u32						m_stackFullSize;			// Value which define the stack size when full (can be different from stack size)
		f32						m_vaccumSpeedFactor;		// aspiration speed factor 
		bbool					m_stackLifoMode;			// by default is false else free space from stack front

		//Temp vars
		f32						m_stateStartDuration;
        f32						m_stateStopDuration;
		f32						m_stateLoopDuration;
		f32						m_stateBeforeRestartDuration;
    };
};

#endif //_ITF_RAY_SHOOTERVACUUMSKILL_H_
