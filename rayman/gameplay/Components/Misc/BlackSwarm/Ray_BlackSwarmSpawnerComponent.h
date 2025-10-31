#ifndef _ITF_RAY_BLACKSWARMSPAWNERCOMPONENT_H_
#define _ITF_RAY_BLACKSWARMSPAWNERCOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/ActorComponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

namespace ITF
{
	//*****************************************************************************
    //	Swarm Spawner Component
	//		Defines an available spawn point to dead particles
    //*****************************************************************************

	class Ray_BlackSwarmSpawnerComponent : public ActorComponent
	{
		DECLARE_OBJECT_CHILD_RTTI(Ray_BlackSwarmSpawnerComponent, ActorComponent,3056762993)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

	public:
		Ray_BlackSwarmSpawnerComponent();
        virtual ~Ray_BlackSwarmSpawnerComponent();

		virtual bbool needsDraw() const { return bfalse; }
		virtual bbool needsDraw2D() const { return bfalse; }
		virtual bbool needsUpdate() const { return bfalse; }

#ifdef ITF_SUPPORT_EDITOR
		virtual void drawEdit(ActorDrawEditInterface* _drawInterface, u32 _flags) const;
#endif // ITF_SUPPORT_EDITOR

		virtual void onActorLoaded(Pickable::HotReloadType _hotReload);

		ITF_INLINE f32 getActivationDistance() const;

	private:
		//get actor template
		ITF_INLINE const class Ray_BlackSwarmSpawnerComponent_Template * getTemplate() const;


		//registered to swarm zone manager flag
		bbool m_registered;
		//activation distance
		f32 m_activationDistance;
	};

	//*****************************************************************************
    // Template
    //*****************************************************************************

	class Ray_BlackSwarmSpawnerComponent_Template : public TemplateActorComponent
	{
		DECLARE_OBJECT_CHILD_RTTI(Ray_BlackSwarmSpawnerComponent_Template, TemplateActorComponent,3193127414);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_BlackSwarmSpawnerComponent);

	public:
        Ray_BlackSwarmSpawnerComponent_Template();
        ~Ray_BlackSwarmSpawnerComponent_Template();

		ITF_INLINE f32 getActivationDistance() const;

	private:
		f32 m_activationDistance;		//distance from target the spawner turns on
	};

	ITF_INLINE const class Ray_BlackSwarmSpawnerComponent_Template * Ray_BlackSwarmSpawnerComponent::getTemplate() const
	{
		return static_cast<const class Ray_BlackSwarmSpawnerComponent_Template *>(m_template);
	}

	ITF_INLINE f32 Ray_BlackSwarmSpawnerComponent::getActivationDistance() const
	{
		return m_activationDistance;
	}

	ITF_INLINE f32 Ray_BlackSwarmSpawnerComponent_Template::getActivationDistance() const
	{
		return m_activationDistance;
	}
}

#endif //_ITF_RAY_BLACKSWARMSPAWNERCOMPONENT_H_