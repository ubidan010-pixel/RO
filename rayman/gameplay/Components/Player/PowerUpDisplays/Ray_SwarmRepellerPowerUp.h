#ifndef _ITF_RAY_SWARMREPELLERPOWERUP_H_
#define _ITF_RAY_SWARMREPELLERPOWERUP_H_

#ifndef _ITF_RAY_POWERUPDISPLAY_H_
#include "rayman/gameplay/Components/Player/PowerUpDisplays/Ray_PowerUpDisplay.h"
#endif //_ITF_RAY_POWERUPDISPLAY_H_

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif //_ITF_SPAWNER_H_

namespace ITF 
{
	//forward declaration
	class Ray_BlackSwarmRepellerAIComponent;

	/*****************************************************
	*	Ray_SwarmRepellerPowerUp
	*
	*		Spawns and manage position of the swarm repeller power up
	*****************************************************/
	class Ray_SwarmRepellerPowerUp : public Ray_PowerUpDisplay
	{
		DECLARE_OBJECT_CHILD_RTTI(Ray_SwarmRepellerPowerUp, Ray_PowerUpDisplay,27597761)
		DECLARE_VALIDATE_OBJECT()

	public:
		Ray_SwarmRepellerPowerUp();
		virtual ~Ray_SwarmRepellerPowerUp();

        virtual void onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void onBecomeInactive();
		virtual void update(f32 _dt);
        virtual void onEvent(ITF::Event * _event);

	private:
		ITF_INLINE const class Ray_SwarmRepellerPowerUp_Template * getTemplate() const;
	    
		void createRepeller();
		void destroyRepeller();

		void updateRepellerPosition(const f32 _dt);
		void checkDisableCondition();
        void checkRefill();

		bbool m_initialized;
		Vec3d m_previousTargetPos;
		Vec3d m_smoothedTargetPosSpeed;
		SpawneeGenerator m_repellerGenerator;
		ActorRef m_repellerRef;
		Ray_BlackSwarmRepellerAIComponent * m_repellerComponent;
	};

	//------------------------------------------------------------------------------
	class Ray_SwarmRepellerPowerUp_Template : public Ray_PowerUpDisplay_Template
	{
		DECLARE_OBJECT_CHILD_RTTI(Ray_SwarmRepellerPowerUp_Template,Ray_PowerUpDisplay_Template,3113042002);
		DECLARE_SERIALIZE();

	public:
		Ray_SwarmRepellerPowerUp_Template();
		virtual ~Ray_SwarmRepellerPowerUp_Template();
	    
		ITF_INLINE const Path& getRepellerActor() const;
		ITF_INLINE const Vec2d& getPlayerFollowOffset() const;
		ITF_INLINE f32 getSpeedBlend() const;
		ITF_INLINE f32 getSpeedMin() const;
		ITF_INLINE f32 getSpeedMax() const;
		ITF_INLINE f32 getBlendAtSpeedMin() const;
		ITF_INLINE f32 getBlendAtSpeedMax() const;

	private:
		Path m_repellerActor;
		Vec2d m_playerFollowOffset;
		f32 m_speedBlend;
		f32 m_speedMin;
		f32 m_speedMax;
		f32 m_blendAtSpeedMin;
		f32 m_blendAtSpeedMax;
	};

	//------------------------------------------------------------------------------
	ITF_INLINE const class Ray_SwarmRepellerPowerUp_Template* Ray_SwarmRepellerPowerUp::getTemplate() const
	{
		return static_cast<const Ray_SwarmRepellerPowerUp_Template *>(m_template);
	}

	ITF_INLINE const Path& Ray_SwarmRepellerPowerUp_Template::getRepellerActor() const
	{ 
		return m_repellerActor; 
	}

	ITF_INLINE const Vec2d& Ray_SwarmRepellerPowerUp_Template::getPlayerFollowOffset() const
	{ 
		return m_playerFollowOffset; 
	}

	ITF_INLINE f32 Ray_SwarmRepellerPowerUp_Template::getSpeedBlend() const
	{ 
		return m_speedBlend; 
	}

	ITF_INLINE f32 Ray_SwarmRepellerPowerUp_Template::getSpeedMin() const 
	{ 
		return m_speedMin; 
	}

	ITF_INLINE f32 Ray_SwarmRepellerPowerUp_Template::getSpeedMax() const 
	{ 
		return m_speedMax; 
	}

	ITF_INLINE f32 Ray_SwarmRepellerPowerUp_Template::getBlendAtSpeedMin() const 
	{ 
		return m_blendAtSpeedMin; 
	}

	ITF_INLINE f32 Ray_SwarmRepellerPowerUp_Template::getBlendAtSpeedMax() const
	{ 
		return m_blendAtSpeedMax; 
	}

} // namespace ITF

#endif //_ITF_RAY_SWARMREPELLERPOWERUP_H_
