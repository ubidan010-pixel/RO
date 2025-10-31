#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_BLACKSWARMSPAWNERCOMPONENT_H_
#include "Rayman/gameplay/Components/Misc/BlackSwarm/Ray_BlackSwarmSpawnerComponent.h"
#endif //_ITF_RAY_BLACKSWARMSPAWNERCOMPONENT_H_

#ifndef _ITF_RAY_BLACKSWARMZONEMANAGER_H_
#include "Rayman/gameplay/Managers/Ray_BlackSwarmZoneManager.h"
#endif //_ITF_RAY_BLACKSWARMZONEMANAGER_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

namespace ITF
{
	IMPLEMENT_OBJECT_RTTI(Ray_BlackSwarmSpawnerComponent)

	BEGIN_SERIALIZATION_CHILD(Ray_BlackSwarmSpawnerComponent)
	END_SERIALIZATION()

	BEGIN_VALIDATE_COMPONENT(Ray_BlackSwarmSpawnerComponent)
	END_VALIDATE_COMPONENT()

	//------------------------------------------------------------------
	Ray_BlackSwarmSpawnerComponent::Ray_BlackSwarmSpawnerComponent()
	:	m_registered(bfalse),
		m_activationDistance(0.0f)
	{
	}

	//------------------------------------------------------------------
	Ray_BlackSwarmSpawnerComponent::~Ray_BlackSwarmSpawnerComponent()
	{
		//unregister swarm generator to swarm zones manager
		if(m_registered)
		{
			Ray_BlackSwarmZoneManager * const manager = Ray_BlackSwarmZoneManager::getSingleton();
            if( manager != NULL )
            {
                manager->unregisterSwarmSpawner( m_actor->getRef() );
		    }
            m_registered = bfalse;
        }
	}

	//------------------------------------------------------------------
	void Ray_BlackSwarmSpawnerComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
	{
		Super::onActorLoaded(_hotReload);

		m_activationDistance = getTemplate()->getActivationDistance();

		//register swarm generator to swarm zones manager
        Ray_BlackSwarmZoneManager * const manager = Ray_BlackSwarmZoneManager::getSingleton();
        ITF_ASSERT_MSG( manager != NULL, "Swarm zones manager is not yet created." );
        if( manager != NULL )
        {
            manager->registerSwarmSpawner( m_actor->getRef() );
            m_registered = btrue;
        }
	}

#ifdef ITF_SUPPORT_EDITOR
	//------------------------------------------------------------------
	void Ray_BlackSwarmSpawnerComponent::drawEdit(ActorDrawEditInterface * _interface, u32 _flags) const
	{
		Super::drawEdit(_interface, _flags);

		DebugDraw::circle(m_actor->get2DPos(), m_actor->getDepth(), 1.0f, Color::red());
		DebugDraw::circle(m_actor->get2DPos(), m_actor->getDepth(), m_activationDistance, Color::green());
	}
#endif // ITF_SUPPORT_EDITOR

	//*****************************************************************************
    // Template
    //*****************************************************************************

	IMPLEMENT_OBJECT_RTTI(Ray_BlackSwarmSpawnerComponent_Template)

	BEGIN_SERIALIZATION(Ray_BlackSwarmSpawnerComponent_Template)
		SERIALIZE_MEMBER("activationDistance", m_activationDistance);
    END_SERIALIZATION()

	//------------------------------------------------------------------
	Ray_BlackSwarmSpawnerComponent_Template::Ray_BlackSwarmSpawnerComponent_Template()
	:	m_activationDistance(10.0f)
	{
	}

	//------------------------------------------------------------------
	Ray_BlackSwarmSpawnerComponent_Template::~Ray_BlackSwarmSpawnerComponent_Template()
	{
	}
}