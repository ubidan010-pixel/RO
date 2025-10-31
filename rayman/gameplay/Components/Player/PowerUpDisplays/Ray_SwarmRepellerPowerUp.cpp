#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_SWARMREPELLERPOWERUP_H_
#include "rayman/gameplay/Components/Player/PowerUpDisplays/Ray_SwarmRepellerPowerUp.h"
#endif //_ITF_RAY_SWARMREPELLERPOWERUP_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_RAY_BLACKSWARMREPELLERCOMPONENT_H_
#include "Rayman/gameplay/Components/Misc/BlackSwarm/Ray_BlackSwarmRepellerComponent.h"
#endif //_ITF_RAY_BLACKSWARMREPELLERCOMPONENT_H_

namespace ITF
{

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_SwarmRepellerPowerUp)
BEGIN_VALIDATE_OBJECT(Ray_SwarmRepellerPowerUp)
    VALIDATE_PARAM("repellerActor", !getTemplate()->getRepellerActor().isEmpty(), "Path mandatory");
END_VALIDATE_OBJECT()

//------------------------------------------------------------------------------
Ray_SwarmRepellerPowerUp::Ray_SwarmRepellerPowerUp()
:	Super(),
    m_initialized(bfalse),
    m_previousTargetPos(Vec3d::Zero),
    m_smoothedTargetPosSpeed(Vec3d::Zero),
	m_repellerGenerator(),
    m_repellerRef(ActorRef::InvalidRef),
	m_repellerComponent(NULL)
{
}

//------------------------------------------------------------------------------
Ray_SwarmRepellerPowerUp::~Ray_SwarmRepellerPowerUp()
{
	//destroy repeller
	destroyRepeller();
}

//------------------------------------------------------------------------------
void Ray_SwarmRepellerPowerUp::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);
    
    // declare actor spawnees
    if (!getTemplate()->getRepellerActor().isEmpty())
    {
		SPAWNER->declareNeedsSpawnee(m_actor, &m_repellerGenerator, getTemplate()->getRepellerActor());
    }
}

//------------------------------------------------------------------------------
void Ray_SwarmRepellerPowerUp::onBecomeInactive()
{
    Super::onBecomeInactive();

    destroyRepeller();
}

//------------------------------------------------------------------------------
void Ray_SwarmRepellerPowerUp::update(f32 _dt)
{
    Super::update(_dt);

	if( m_initialized )
    {
		if( isEnabled() )
		{
			//repeller follows player actor
			updateRepellerPosition(_dt);
            //check refill (player has picked up a new repeller power up)
            checkRefill();
			//check repeller job done
			checkDisableCondition();
		}
		else
		{
            //power up has been disabled so destroy repeller
			destroyRepeller();
			m_initialized = bfalse;
		}
    }
	else
	{
		if( isEnabled() )
		{
            //power up has been enabled so create repeller
			createRepeller();
			m_initialized = btrue;
		}
	}
}

//------------------------------------------------------------------------------
void Ray_SwarmRepellerPowerUp::updateRepellerPosition(const f32 _dt)
{
	Actor* repeller = m_repellerRef.getActor();
    if( repeller != NULL )
    {
        // current position
        Vec3d newPos = repeller->getPos();
    
        // get target position
        Vec2d offset = getTemplate()->getPlayerFollowOffset();
        if (AIUtils::getFlipped(m_actor))
        {
            offset.m_x = -offset.m_x;
        }
        offset *= m_actor->getScale();
        Vec3d targetPos = m_actor->getPos() + offset.to3d();
        
        // smooth position depending on target speed
        AIUtils::advancedMichelSmooth(
            newPos,
            m_smoothedTargetPosSpeed,
            m_previousTargetPos,
            targetPos,
            getTemplate()->getSpeedBlend(),
            getTemplate()->getSpeedMin(),getTemplate()->getSpeedMax(),
            getTemplate()->getBlendAtSpeedMin(),
            getTemplate()->getBlendAtSpeedMax(),
            _dt
            );

		//make the power visual always be behing rayman
		newPos.m_z = m_actor->getDepth() - 0.001f;

        // set new pos
        repeller->setPos(newPos);
    }
}

//------------------------------------------------------------------------------
void Ray_SwarmRepellerPowerUp::checkDisableCondition()
{
    //check repeller job finished condition
	ITF_ASSERT_MSG( m_repellerComponent != NULL, "BlackSwarmRepeller power up : cannot get spawned repeller component." );
	if( m_repellerComponent != NULL )
	{
		if( m_repellerComponent->isFinished() )
		{
			setEnabled(bfalse);
		}
	}
    //check player left condition
    const Player * const player = GAMEMANAGER->getPlayerFromActor(m_actor->getRef());
    if( player )
    {
        if( player->isDead() )
        {
            setEnabled(bfalse);
        }
    }
    else
    {
        setEnabled(bfalse);
    }
}

//------------------------------------------------------------------------------
void Ray_SwarmRepellerPowerUp::checkRefill()
{
    //ammo > 1 means that player has refill power up ammo (pick a new swarm repeller power up)
    if( getAmmo() > 1 )
    {
        //relaunch repeller
        ITF_ASSERT_MSG( m_repellerComponent != NULL, "BlackSwarmRepeller power up : cannot get spawned repeller component." );
	    if( m_repellerComponent != NULL )
	    {
            m_repellerComponent->forceRepellerState( Ray_BlackSwarmRepellerAIComponent::RepellerState_OFF_TO_ON );
	    }

        //use 1 ammo
        useAmmo();
    }
}

//------------------------------------------------------------------------------
void Ray_SwarmRepellerPowerUp::createRepeller()
{
    // guard
    if (!CURRENTWORLD)
    {
        return;
    }

    Actor* repeller = m_repellerGenerator.getSpawnee(CURRENTWORLD->getRootScene(), m_actor->getPos());
    ITF_WARNING(m_actor, repeller, "Can't spawn repeller used by power up");
    if(repeller != NULL)
    {
        repeller->enable();
        m_repellerRef = repeller->getRef();
        m_actor->getBinding()->bindChild(m_repellerRef);
        
        // store target pos
        m_previousTargetPos = m_actor->getPos();

		//get black swarm repeller component
		m_repellerComponent = repeller->GetComponent<Ray_BlackSwarmRepellerAIComponent>();
		ITF_ASSERT_MSG( m_repellerComponent != NULL, "BlackSwarmRepeller power up : cannot get spawned repeller component." );
    }

    //use 1 ammo
    useAmmo();
}

//------------------------------------------------------------------------------
void Ray_SwarmRepellerPowerUp::destroyRepeller()
{
	if( m_initialized )
	{
		//unbind repeller
		m_actor->getBinding()->unbindChild(m_repellerRef);
		//get repeller from ref
		Actor * repeller = m_repellerRef.getActor();
		if( repeller != NULL )
		{
			repeller->requestDestruction();
		}
		//clear ref
		m_repellerRef = ActorRef::InvalidRef;

		//clear repeller component ptr
		m_repellerComponent = NULL;
	}
}

//------------------------------------------------------------------------------
void Ray_SwarmRepellerPowerUp::onEvent(ITF::Event * _event)
{
    Super::onEvent(_event);

    // process events
    if (EventReset* reset = _event->DynamicCast<EventReset>(ITF_GET_STRINGID_CRC(EventReset,2551104629)))
    {
        if( m_initialized )
        {
            destroyRepeller();
            m_initialized = bfalse;
        }
    }
}




//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_SwarmRepellerPowerUp_Template)
BEGIN_SERIALIZATION_CHILD(Ray_SwarmRepellerPowerUp_Template)
    SERIALIZE_MEMBER("repellerActor", m_repellerActor);
    SERIALIZE_MEMBER("playerFollowOffset", m_playerFollowOffset);
    SERIALIZE_MEMBER("speedBlend", m_speedBlend);
    SERIALIZE_MEMBER("speedMin", m_speedMin);
    SERIALIZE_MEMBER("speedMax", m_speedMax);
    SERIALIZE_MEMBER("blendAtSpeedMin", m_blendAtSpeedMin);
    SERIALIZE_MEMBER("blendAtSpeedMax", m_blendAtSpeedMax);
END_SERIALIZATION()

//------------------------------------------------------------------------------
Ray_SwarmRepellerPowerUp_Template::Ray_SwarmRepellerPowerUp_Template()
    : Super()
	, m_repellerActor(Path::EmptyPath)
    , m_playerFollowOffset(Vec2d::Zero)
    , m_speedBlend(1.0f)
    , m_speedMin(0.0f)
    , m_speedMax(1.0f)
    , m_blendAtSpeedMin(0.0f)
    , m_blendAtSpeedMax(1.0f)
{
}

//------------------------------------------------------------------------------
Ray_SwarmRepellerPowerUp_Template::~Ray_SwarmRepellerPowerUp_Template()
{
}

} // namespace ITF
