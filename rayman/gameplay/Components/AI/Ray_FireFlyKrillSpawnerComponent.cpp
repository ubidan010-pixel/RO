#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_FIREFLYKRILLSPAWNERCOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_FireFlyKrillSpawnerComponent.h"
#endif //_ITF_RAY_FIREFLYKRILLSPAWNERCOMPONENT_H_

#ifndef _ITF_RAY_FIREFLYKRILLAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_FireFlyKrillAIComponent.h"
#endif //_ITF_RAY_FIREFLYKRILLAICOMPONENT_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_FireFlyKrillSpawnerComponent)
    
    BEGIN_SERIALIZATION_CHILD(Ray_FireFlyKrillSpawnerComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(Ray_FireFlyKrillSpawnerComponent)
    END_VALIDATE_COMPONENT()

    

    //------------------------------------------------------------------
    //static members used to manage krill <-> spawner detection
    //------------------------------------------------------------------
    ActorRefList Ray_FireFlyKrillSpawnerComponent::ms_spawners = ActorRefList();

    //------------------------------------------------------------------
    void Ray_FireFlyKrillSpawnerComponent::registerSpawner(const ActorRef &_spawner)
    {
        ITF_ASSERT(ms_spawners.find(_spawner) == U32_INVALID);
        ms_spawners.push_back(_spawner);
    }

    //------------------------------------------------------------------
    void Ray_FireFlyKrillSpawnerComponent::unregisterSpawner(const ActorRef &_spawner)
    {
        const u32 index = ms_spawners.find(_spawner);
        ITF_ASSERT(index != U32_INVALID);
        if(index != U32_INVALID)
        {
            ms_spawners.eraseNoOrder(index);
        }
    }

    //------------------------------------------------------------------
    bbool Ray_FireFlyKrillSpawnerComponent::getSpawnerInRange(const Vec2d & _position, const f32 _radius, Vec2d & _spawnerPosition)
    {
        const f32 sqrRadius = _radius * _radius;
        for(u32 index = 0; index < ms_spawners.size(); ++index)
        {
            const Actor * const spawnerActor = ms_spawners[index].getActor();
            ITF_ASSERT(spawnerActor);
            if( spawnerActor )
            {
                const Vec2d & spawnerPosition = spawnerActor->get2DPos();
                const f32 sqrDistance =(spawnerPosition - _position).sqrnorm();
                if( sqrDistance < sqrRadius )
                {
                    _spawnerPosition = spawnerPosition;
                    return btrue;
                }
            }
        }
        return bfalse;
    }

    //------------------------------------------------------------------
    Ray_FireFlyKrillSpawnerComponent::Ray_FireFlyKrillSpawnerComponent()
    : m_krillSpawner()
    , m_dockedKrill(ActorRef::InvalidRef)
    , m_instanceRegistered(bfalse)
    {
    }

    //------------------------------------------------------------------
    void Ray_FireFlyKrillSpawnerComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        //init spawner
        if( !getTemplate()->getKrillActorPath().isEmpty() )
        {
            SPAWNER->declareNeedsSpawnee(m_actor, &m_krillSpawner , getTemplate()->getKrillActorPath());
        }
    }

    //------------------------------------------------------------------
    void Ray_FireFlyKrillSpawnerComponent::onActorClearComponents()
    {
        Super::onActorClearComponents();
        unregisterInstance();
    }

    //------------------------------------------------------------------
    void Ray_FireFlyKrillSpawnerComponent::onBecomeActive()
    {
        Super::onBecomeInactive();
        registerInstance();
    }

    //------------------------------------------------------------------
    void Ray_FireFlyKrillSpawnerComponent::onBecomeInactive()
    {
        Super::onBecomeInactive();
        unregisterInstance();
    }

    //------------------------------------------------------------------
    void Ray_FireFlyKrillSpawnerComponent::Update(f32 _dt)
    {
        Super::Update(_dt);

        if( !m_dockedKrill.isValid() )
        {
            spawnKrill();
        }
        updatePlayerDetection();
    }

    //------------------------------------------------------------------
    void Ray_FireFlyKrillSpawnerComponent::registerInstance()
    {
        if( !m_instanceRegistered )
        {
            Ray_FireFlyKrillSpawnerComponent::registerSpawner(m_actor->getRef());
            m_instanceRegistered = btrue;
        }
    }

    //------------------------------------------------------------------
    void Ray_FireFlyKrillSpawnerComponent::unregisterInstance()
    {
        if( m_instanceRegistered )
        {
            Ray_FireFlyKrillSpawnerComponent::unregisterSpawner(m_actor->getRef());
            m_instanceRegistered = bfalse;
        }
    }

    //------------------------------------------------------------------
    void Ray_FireFlyKrillSpawnerComponent::spawnKrill()
    {
        const Actor * const krill = m_krillSpawner.getSpawnee(m_actor->getScene(), m_actor->getPos() );
        if( krill )
        {
            m_dockedKrill = krill->getRef();
        }
    }

    //------------------------------------------------------------------
    void Ray_FireFlyKrillSpawnerComponent::updatePlayerDetection()
    {
        if( !RAY_GAMEMANAGER->isPlayingPageSequence() )
        {
            const f32 radius = getTemplate()->getPlayerDetectionRadius();
            const f32 sqrRadius = radius * radius;
            const Vec2d & position = m_actor->get2DPos();

            ActorList players;
            AIUtils::getLivePlayers(players);
            for(u32 index = 0; index < players.size(); ++index)
            {
                const Actor * const player = players[index];
                ITF_ASSERT(player);

                const ActorRef playerRef = player->getRef();
                if( !Ray_FireFlyKrillAIComponent::isPlayerMatched( playerRef ) )
                {
                    if( !RAY_GAMEMANAGER->isPlayerInSequence( playerRef ) )
                    {
                        if( m_dockedKrill.isValid() )
                        {
                            const Vec2d & playerPos = player->get2DPos();
                            const f32 sqrDistance = (position - playerPos).sqrnorm();
                            if( sqrDistance < sqrRadius )
                            {
                                const Actor * const krill = m_dockedKrill.getActor();
                                if( krill )
                                {
                                    Ray_FireFlyKrillAIComponent * const krillComponent = krill->GetComponent<Ray_FireFlyKrillAIComponent>();
                                    ITF_ASSERT(krillComponent);
                                    if(krillComponent)
                                    {
                                        krillComponent->setTarget(playerRef);
                                    }
                                    m_dockedKrill.invalidate();
                                }
                                else
                                {
                                    m_dockedKrill.invalidate();
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    //*****************************************************************************
    // Template
    //*****************************************************************************

	IMPLEMENT_OBJECT_RTTI(Ray_FireFlyKrillSpawnerComponent_Template)

	BEGIN_SERIALIZATION(Ray_FireFlyKrillSpawnerComponent_Template)
        SERIALIZE_MEMBER("krillActorPath", m_krillActorPath);
        SERIALIZE_MEMBER("detectionRadius", m_playerDetectionRadius);
    END_SERIALIZATION()

	//------------------------------------------------------------------
	Ray_FireFlyKrillSpawnerComponent_Template::Ray_FireFlyKrillSpawnerComponent_Template()
    : m_krillActorPath(Path::EmptyPath)
    , m_playerDetectionRadius(10.0f)
	{
	}

	//------------------------------------------------------------------
	Ray_FireFlyKrillSpawnerComponent_Template::~Ray_FireFlyKrillSpawnerComponent_Template()
	{
	}
}
