#ifndef _ITF_RAY_BLACKSWARMREPELLERCOMPONENT_H_
#define _ITF_RAY_BLACKSWARMREPELLERCOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/ActorComponent.h"
#endif //_ITF_ACTORCOMPONENT_H_


namespace ITF
{
	//forward declaration
	class AnimLightComponent;
    class FXControllerComponent;

	class Ray_BlackSwarmRepellerAIComponent : public ActorComponent
	{
		DECLARE_OBJECT_CHILD_RTTI(Ray_BlackSwarmRepellerAIComponent, ActorComponent,533755526)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

	public:

        //Repeller state
		enum RepellerState
		{
			RepellerState_UNDEFINED = -1,
			RepellerState_OFF,
			RepellerState_OFF_TO_ON,
			RepellerState_ON,
			RepellerState_ON_TO_OFF,
            RepellerState_COUNT,
			ENUM_FORCE_SIZE_32(0)
		};

		Ray_BlackSwarmRepellerAIComponent();
        virtual ~Ray_BlackSwarmRepellerAIComponent();

        virtual bbool       needsUpdate()   const { return btrue; }
        virtual bbool       needsDraw()     const { return btrue; }
        virtual bbool       needsDraw2D()   const { return bfalse; }

        virtual void        onActorLoaded(Pickable::HotReloadType _hotReload);
        virtual void        onBecomeActive();
        virtual void        onBecomeInactive();
        virtual void        Update( f32 _dt );
		virtual void		Draw();
		virtual void		onEvent(Event * _event);

		ITF_INLINE f32 getRepelVisualRadius() const;
		ITF_INLINE f32 getRepelLogicalRadius() const;
		ITF_INLINE const Vec2d & getRepelCenter() const;
        ITF_INLINE bbool isFinished() const;
        ITF_INLINE bbool isTrustable() const;

        //force repeller to switch to the wanted state
        void forceRepellerState(const RepellerState _state);

	private:
		ITF_INLINE const class Ray_BlackSwarmRepellerAIComponent_Template * getTemplate() const;

		//compute repel center from actor's position, repel offset, actor's rotation and actor's scale
		void computeRepelCenter();
		//update AABB according to radius pulsation
		void updateAABB();

		//pulse mesh creation (VB allocation, ...)
		void createPulseMesh();
		//compute pulse mesh vertex buffer
		void computePulseVertexBuffer();
		//pulse mesh destruction
		void destroyPulseMesh();

		//change repeller state
		void ChangeRepellerState(const RepellerState _newState);
		//udpate repeller state
		void UpdateRepellerState(const f32 _dt);
        //compute start and end bar ratio for each repeller state
        void computeRepellerStateData();
        //get current state ratio
        f32 getCurrentRepellerStateTimeRatio() const;
        //get ratio from current cycle state
        f32 getCurrentCycleRatio() const;
        //compute cycle timer using a wanted state and a radius
        f32 computeCycleTimerFromRadius(const RepellerState _state, const f32 _radius) const;

        //process animation event
        void processAnimEvent(const AnimGameplayEvent * const _event);


		//Radius pulse state
		enum RadiusPulseState
		{
			RadiusPulseState_UNDEFINED = -1,
			RadiusPulseState_NONE,
			RadiusPulseState_INCREASING,
			RadiusPulseState_DECREASING,
			ENUM_FORCE_SIZE_32(1)
		};
		//change radius pulse state
		void ChangeRadiusPulseState(const RadiusPulseState _newState);
		//update radius pulse
		void UpdateRadiusPulse(const f32 _dt);


        //Sound management
        void playLoopSound();
        void playInteractiveSound();
        void playPeriodicSound();
        void stopFx(u32 & _handle) const;

        //Fx management
        void playFx(const StringID & _name);

        //min between two interactive sounds stop/start
        static const f32 MIN_TIME_BETWEEN_INTERACTIVE_SOUNDS;		
		
        //max radius
        f32 m_maxRadius;
        f32 m_minRadius;
		//current radius
		f32	m_radius;
		//state of repeller
		RepellerState m_repellerState;
        struct RepellerStateData
        {
            RepellerStateData()
            :   m_startRatio(0.0f),
                m_endRatio(0.0f),
                m_infinite(bfalse)
            {
            }

            RepellerStateData(const f32 _startRatio, const f32 _endRatio, const bbool _infinite)
            :   m_startRatio(_startRatio),
                m_endRatio(_endRatio),
                m_infinite(_infinite)
            {
            }

            f32 m_startRatio;
            f32 m_endRatio;
            bbool m_infinite;
        };
        FixedArray<RepellerStateData, RepellerState_COUNT> m_repellerStateData;
        //duration of repeller cycle
        f32 m_cycleDuration;
		//timer of repeller state
		f32 m_cycleTimer;
        //instance sync offset
        f32 m_syncOffset_instance;
		//state of radius pulsation
		RadiusPulseState	m_radiusPulseState;
		//timer of radius pulsation
		f32 m_radiusPulseTimer;
		//current value of the pulse
		f32 m_radiusPulse;
        //current value of alpha (alpha pulse)
        f32 m_alphaPulse;
		//is repeller registered to black swarm manager
		bbool m_registered;
		//repel center
		Vec2d m_repelCenter;
		//final radius used to store final radius (repeller state radius + pulse radius) to simplify getRepelRadius inlined function
		f32 m_finalRadius;
        //time since last punch
        f32 m_timeSinceLastInteractionSound;

		//component ptr
		AnimLightComponent * m_animLightComponent;
        FXControllerComponent * m_fxControllerComponent;

        //sound handles
        u32 m_soundLoopHandle;

        //particle fx handle
        u32 m_fxHandle;

		//pulse texture resource id
		ResourceID m_pulseTextureId;

		//pulse mesh
		ITF_Mesh m_mesh;
		VertexPCT m_cacheVB[6];

		//cycle counter
		i32 m_cycle;
	};

	//*****************************************************************************
    // Template
    //*****************************************************************************

	class Ray_BlackSwarmRepellerAIComponent_Template : public TemplateActorComponent
	{
		DECLARE_OBJECT_CHILD_RTTI(Ray_BlackSwarmRepellerAIComponent_Template, TemplateActorComponent,2331876365);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_BlackSwarmRepellerAIComponent);

	public:
        Ray_BlackSwarmRepellerAIComponent_Template();
        ~Ray_BlackSwarmRepellerAIComponent_Template();

		ITF_INLINE bbool isInteractive() const;
		ITF_INLINE f32 getSafeZoneToleranceDistance() const;
		ITF_INLINE f32 getSafeZoneRadiusMin() const;
		ITF_INLINE f32 getSafeZoneRadiusMax() const;
        ITF_INLINE bbool useSynchro() const;
        ITF_INLINE f32 getSyncOffset() const;
        ITF_INLINE Ray_BlackSwarmRepellerAIComponent::RepellerState getDefaultRepellerState() const;
        ITF_INLINE f32 getCycleDuration() const;
        ITF_INLINE i32 getMaxAllowedCycles() const;
        ITF_INLINE f32 getStateRatioOFF() const;
        ITF_INLINE f32 getStateRatioOFFtoON() const;
        ITF_INLINE f32 getStateRatioON() const;
        ITF_INLINE f32 getStateRatioONtoOFF() const;
		ITF_INLINE f32 getSafeZonePulseRadius() const;
		ITF_INLINE f32 getSafeZonePulseTime() const;
        ITF_INLINE f32 getMinAlphaValue() const;
        ITF_INLINE f32 getMaxAlphaValue() const;
		ITF_INLINE const Vec2d & getSafeZoneCenterOffset() const;
		ITF_INLINE const Path & getPulseTexturePath() const;
        ITF_INLINE const StringID & getParticleFxName() const;
        ITF_INLINE f32 getParticleFxSizeRatio() const;
        ITF_INLINE GFX_BLENDMODE getTextureBlendMode() const;
        ITF_INLINE const StringID & getInteractiveSoundName() const;
        ITF_INLINE const StringID & getPeriodicSoundName() const;

	private:
		//is repeller interactive (can be punched)
		bbool m_interactive;
		//min radius = default radius
		f32	m_safeZoneRadiusMin;
		//max radius = radius when GPEs is activated (punched)
		f32 m_safeZoneRadiusMax;
        //is repeller cycle synchro on metronome
        bbool m_useSynchro;
        //sync offset in bar
        f32 m_syncOffset;
        //cycle default state (repeller will start in this state)
        i32 m_defaultState;
		//cycle duration (cycle = OFF - OFF TO ON - ON - ON TO OFF) 2.0f = 1 bar at default bpm
        f32 m_cycleDuration;
        //max number of cycles the repeller is allowed to do (cycle = OFF - OFF TO ON - ON - ON TO OFF)
        i32 m_maxAllowedCycles;
        //states duration ratios
        f32 m_stateRatioOFF;
        f32 m_stateRatioOFFtoON;
        f32 m_stateRatioON;
        f32 m_stateRatioONtoOFF;
		//radius pulsation
		f32 m_safeZonePulseRadius;
		f32 m_safeZonePulseTime;
        //alpha pulsation
        f32 m_minAlphaValue;
        f32 m_maxAlphaValue;
		//safe zone center offset
		Vec2d m_safeZoneCenterOffset;
		//safe zone tolerance distance (difference between visual and safe area)
		f32 m_safeZoneToleranceDistance;
		//pulse texture path
		Path m_pulseTexturePath;
        //particle fx name
        StringID m_particleFxName;
        f32 m_particleFXSizeRatio;
        //texture blend mode
        u32 m_textureBlendMode;
        //fx names
        StringID m_periodicSoundName;
        StringID m_interactiveSoundName;
	};



	//*****************************************************************************
    // inlined functions definition
    //*****************************************************************************

	ITF_INLINE const class Ray_BlackSwarmRepellerAIComponent_Template * Ray_BlackSwarmRepellerAIComponent::getTemplate() const
    {
        return static_cast<const class Ray_BlackSwarmRepellerAIComponent_Template *>(m_template);
    }

	ITF_INLINE f32 Ray_BlackSwarmRepellerAIComponent::getRepelVisualRadius() const
    {
		return m_finalRadius;
    }

	ITF_INLINE f32 Ray_BlackSwarmRepellerAIComponent::getRepelLogicalRadius() const
    {
		if( m_finalRadius > 0.0f )
		{
			return m_finalRadius + getTemplate()->getSafeZoneToleranceDistance();
		}
		return 0.0f;
    }
		
	ITF_INLINE const Vec2d & Ray_BlackSwarmRepellerAIComponent::getRepelCenter() const
	{
		return m_repelCenter;
	}
    
    ITF_INLINE bbool Ray_BlackSwarmRepellerAIComponent::isFinished() const
    {
        const i32 maxCycleCount = getTemplate()->getMaxAllowedCycles();
        return (maxCycleCount >= 0) && (m_cycle > maxCycleCount);
    }

    ITF_INLINE bbool Ray_BlackSwarmRepellerAIComponent::isTrustable() const
    {
        return (m_repellerState == RepellerState_ON) && ((m_repellerStateData[RepellerState_ON].m_infinite) || (m_cycleDuration < 0.0f));
    }



	ITF_INLINE bbool Ray_BlackSwarmRepellerAIComponent_Template::isInteractive() const
	{
		return m_interactive;
	}

	ITF_INLINE f32 Ray_BlackSwarmRepellerAIComponent_Template::getSafeZoneToleranceDistance() const
	{
		return m_safeZoneToleranceDistance;
	}

	ITF_INLINE f32 Ray_BlackSwarmRepellerAIComponent_Template::getSafeZoneRadiusMin() const
	{
		return m_safeZoneRadiusMin;
	}

	ITF_INLINE f32 Ray_BlackSwarmRepellerAIComponent_Template::getSafeZoneRadiusMax() const
	{
		return m_safeZoneRadiusMax;
	}

    ITF_INLINE f32 Ray_BlackSwarmRepellerAIComponent_Template::getSyncOffset() const
    {
        return m_syncOffset;
    }

    ITF_INLINE Ray_BlackSwarmRepellerAIComponent::RepellerState Ray_BlackSwarmRepellerAIComponent_Template::getDefaultRepellerState() const
    {
        ITF_ASSERT( (m_defaultState >= Ray_BlackSwarmRepellerAIComponent::RepellerState_UNDEFINED) && (m_defaultState < Ray_BlackSwarmRepellerAIComponent::RepellerState_COUNT) );
        if( (m_defaultState >= Ray_BlackSwarmRepellerAIComponent::RepellerState_UNDEFINED) && (m_defaultState < Ray_BlackSwarmRepellerAIComponent::RepellerState_COUNT) )
        {
            return (Ray_BlackSwarmRepellerAIComponent::RepellerState)m_defaultState;
        }
        else 
        {
            return Ray_BlackSwarmRepellerAIComponent::RepellerState_UNDEFINED;
        }
    }

    ITF_INLINE f32 Ray_BlackSwarmRepellerAIComponent_Template::getCycleDuration() const
    {
        return m_cycleDuration;
    }

    ITF_INLINE i32 Ray_BlackSwarmRepellerAIComponent_Template::getMaxAllowedCycles() const
    {
        return m_maxAllowedCycles;
    }

    ITF_INLINE f32 Ray_BlackSwarmRepellerAIComponent_Template::getStateRatioOFF() const
    {
        return m_stateRatioOFF;
    }

    ITF_INLINE f32 Ray_BlackSwarmRepellerAIComponent_Template::getStateRatioOFFtoON() const
    {
        return m_stateRatioOFFtoON;
    }

    ITF_INLINE f32 Ray_BlackSwarmRepellerAIComponent_Template::getStateRatioON() const
    {
        return m_stateRatioON;
    }

    ITF_INLINE f32 Ray_BlackSwarmRepellerAIComponent_Template::getStateRatioONtoOFF() const
    {
        return m_stateRatioONtoOFF;
    }

    ITF_INLINE bbool Ray_BlackSwarmRepellerAIComponent_Template::useSynchro() const
    {
        return m_useSynchro;
    }

	ITF_INLINE f32 Ray_BlackSwarmRepellerAIComponent_Template::getSafeZonePulseRadius() const
	{
		return m_safeZonePulseRadius;
	}

	ITF_INLINE f32 Ray_BlackSwarmRepellerAIComponent_Template::getSafeZonePulseTime() const
	{
		return m_safeZonePulseTime;
	}

    ITF_INLINE f32 Ray_BlackSwarmRepellerAIComponent_Template::getMinAlphaValue() const
    {
        return m_minAlphaValue;
    }

    ITF_INLINE f32 Ray_BlackSwarmRepellerAIComponent_Template::getMaxAlphaValue() const
    {
        return m_maxAlphaValue;
    }

	ITF_INLINE const Vec2d & Ray_BlackSwarmRepellerAIComponent_Template::getSafeZoneCenterOffset() const
	{
		return m_safeZoneCenterOffset;
	}

	ITF_INLINE const Path & Ray_BlackSwarmRepellerAIComponent_Template::getPulseTexturePath() const
	{
		return m_pulseTexturePath;
	}

    ITF_INLINE const StringID & Ray_BlackSwarmRepellerAIComponent_Template::getParticleFxName() const
    {
        return m_particleFxName;
    }
        
    ITF_INLINE f32 Ray_BlackSwarmRepellerAIComponent_Template::getParticleFxSizeRatio() const
    {
        return m_particleFXSizeRatio;
    }

    ITF_INLINE GFX_BLENDMODE Ray_BlackSwarmRepellerAIComponent_Template::getTextureBlendMode() const
    {
        return (GFX_BLENDMODE)m_textureBlendMode;
    }

    ITF_INLINE const StringID & Ray_BlackSwarmRepellerAIComponent_Template::getInteractiveSoundName() const
    {
        return m_interactiveSoundName;
    }

    ITF_INLINE const StringID & Ray_BlackSwarmRepellerAIComponent_Template::getPeriodicSoundName() const
    {
        return m_periodicSoundName;
    }
}

#endif //_ITF_RAY_BLACKSWARMREPELLERCOMPONENT_H_