#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_SHOOTERGARDIANAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Shooter/Ray_ShooterGardianAIComponent.h"
#endif //_ITF_RAY_SHOOTERGARDIANAICOMPONENT_H_

#ifndef _ITF_AIRECEIVEHITBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIReceiveHitBehavior.h"
#endif //_ITF_AIRECEIVEHITBEHAVIOR_H_

#ifndef _ITF_RAY_GAMEMATERIAL_H_
#include "rayman/gameplay/Ray_GameMaterial.h"
#endif //_ITF_RAY_GAMEMATERIAL_H_

#ifndef _ITF_RAY_AISHOOTERVACUUMBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/Shooter/Ray_AIShooterVacuumBehavior.h"
#endif //_ITF_RAY_AISHOOTERVACUUMBEHAVIOR_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_ShooterGardianAIComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_ShooterGardianAIComponent)
    END_SERIALIZATION()

    const f32 k_sameHitSenderBeforeRehitDuration = 0.35f;

    //--------------------------------------------------------------------------------------------------------------------------------
    Ray_ShooterGardianAIComponent::Ray_ShooterGardianAIComponent()
    :Super()
    , m_hitAddAnimCursor( 99.9f )
    , m_lastHitTimer( 0.0f )
    , m_vacuumBhv( NULL )
    , m_isInvulnerable( bfalse )
    , m_defaultMaxHealthMulti( -1 )
    , m_deathRewardTimer( 0.0f )
    , m_deathNextSpawnTime( 0.0f )
    , m_deathLastSpawnDelay( 0.0f )
    {
    }

    //--------------------------------------------------------------------------------------------------------------------------------
    Ray_ShooterGardianAIComponent::~Ray_ShooterGardianAIComponent()
    {
        EVENTMANAGER->unregisterEvent(ITF_GET_STRINGID_CRC(EventPlayerActivationChanged,1593682208), this);
    }

    //--------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterGardianAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor, ITF_GET_STRINGID_CRC(Ray_EventShooterInvulnerableActor,1840838639), this);
        
        m_vacuumBhv = m_behavior->DynamicCast<Ray_AIShooterVacuumBehavior>(ITF_GET_STRINGID_CRC(Ray_AIShooterVacuumBehavior,1724246635));
        if ( m_vacuumBhv != NULL )
        {
            ACTOR_REGISTER_EVENT_COMPONENT(m_actor, ITF_GET_STRINGID_CRC(Ray_EventShooterVacuumBhvChangeState,2152722158), this );
        }     

        EVENTMANAGER_REGISTER_EVENT_LISTENER(ITF_GET_STRINGID_CRC(EventPlayerActivationChanged,1593682208),this);
        m_defaultMaxHealthMulti = getTemplate()->getHealth_default();
        updateMultiPlayerHealth();

        if ( !getTemplate()->getDeathRewardSpawnPath().isEmpty() )
        {
            SPAWNER->declareNeedsSpawnee( m_actor, &m_deathRewardGenerator, getTemplate()->getDeathRewardSpawnPath() );
        }
        m_deathRewardTimer = 0.0f;
        m_deathNextSpawnTime = 0.0f;
        m_deathLastSpawnDelay = 0.0f;
    }

    //--------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterGardianAIComponent::updateMultiPlayerHealth()
    {
        u32 numPlayer = GAMEMANAGER->getNumActivePlayers();

        i32 defaultMaxHealthMulti = getTemplate()->getHealth_default() + i32( f32( getTemplate()->getHealth_default() ) * getTemplate()->getMultiPlayerLifePointFactor() * f32_Max( 0.f, f32( numPlayer ) - 1.0f ) );
        setHealth( i32 ( f32( defaultMaxHealthMulti ) * f32( getHealth() ) / f32( m_defaultMaxHealthMulti ) ) );

        m_defaultMaxHealthMulti = defaultMaxHealthMulti;
    }

    //--------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterGardianAIComponent::Update( f32 _deltaTime )
    {
        Super::Update(_deltaTime);

        m_hitAddAnimCursor = f32_Min( m_hitAddAnimCursor + _deltaTime, 99.9f );
        m_lastHitTimer = f32_Max( m_lastHitTimer - _deltaTime, 0.f );

        if ( ( getHealth() == 0 ) && ( m_deathRewardTimer < getTemplate()->getDeathRewardSpawnDuration() ) && m_deathRewardGenerator.isInitialized() )
        {
            m_deathRewardTimer += _deltaTime;

            if ( m_deathRewardTimer >= m_deathNextSpawnTime )
            {
                Vec2d offsetVec = Vec2d::Right.Rotate( Seeder::getSharedSeeder().GetFloat( getTemplate()->getDeathRewardSpawnAngle().m_x, getTemplate()->getDeathRewardSpawnAngle().m_y ) );
                offsetVec *= Seeder::getSharedSeeder().GetFloat( getTemplate()->getDeathRewardSpawnDist().m_x, getTemplate()->getDeathRewardSpawnDist().m_y );
                Actor * reward = m_deathRewardGenerator.getSpawnee( CURRENTWORLD->getRootScene(), m_actor->getPos() + offsetVec.to3d(m_actor->getDepth() ) );

                if ( reward != NULL )
                {
                    Ray_EventChildLaunch launch( m_actor->getPos() );
                    launch.setSender(m_actor->getRef());
                    reward->onEvent(&launch);
                }

                if ( getTemplate()->getDeathRewardNumber() > 0 )
                {
                    f32 delay = getTemplate()->getDeathRewardSpawnDuration() / f32( getTemplate()->getDeathRewardNumber() );
                    f32 newDelay = Seeder::getSharedSeeder().GetFloat( delay * 0.2f, delay );
                    m_deathNextSpawnTime += ( delay - m_deathLastSpawnDelay ) + newDelay;
                    m_deathLastSpawnDelay = newDelay;
                }
            }
        }
    }

    //--------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterGardianAIComponent::onBehaviorFinished()
    {
        Super::onBehaviorFinished();
    }    

    //--------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterGardianAIComponent::onEvent( Event * _event)
    {
        Super::onEvent(_event);

        if (Ray_EventShooterVacuumBhvChangeState * chgEvt = _event->DynamicCast<Ray_EventShooterVacuumBhvChangeState>(ITF_GET_STRINGID_CRC(Ray_EventShooterVacuumBhvChangeState,2152722158)))
        {
            ITF_ASSERT( m_vacuumBhv != NULL );
            if ( m_vacuumBhv != NULL )
            {
                m_vacuumBhv->processEvtChangeState( chgEvt );
            }
        }
        else if (Ray_EventShooterInvulnerableActor * invulnerableEvt = _event->DynamicCast<Ray_EventShooterInvulnerableActor>(ITF_GET_STRINGID_CRC(Ray_EventShooterInvulnerableActor,1840838639)))
        {
            m_isInvulnerable = invulnerableEvt->IsInvulnerable();
        }
        else if ( EventPlayerActivationChanged * _playerActivation = _event->DynamicCast<EventPlayerActivationChanged>(ITF_GET_STRINGID_CRC(EventPlayerActivationChanged,1593682208)) )
        {
            updateMultiPlayerHealth();
        }
    }

    //--------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterGardianAIComponent::processHit( HitStim* _hit )
    {
        if ( ( _hit->getLevel() >= getTemplate()->getMinHitStunLevel() ) && ( m_receiveHitBehavior != m_currentBehavior ) )
        {
            Super::processHit( _hit );        
            m_lastHitTimer = k_sameHitSenderBeforeRehitDuration;

            // if invulnerable, send succefull hit anyway
            if ( m_isInvulnerable )
            {
                Actor* sender = AIUtils::getActor( _hit->getSender() );
                if ( sender != NULL )
                {
                    EventHitSuccessful hitSuccessful;
                    hitSuccessful.setSender( m_actor->getRef() );
                    sender->onEvent(&hitSuccessful);
                }
            }
        }
        else if ( ( getLastAttacker() != _hit->getSender() ) || ( m_lastHitTimer == 0.f ) )
        {
            // check contact materials only if not stun 
            //
            if ( m_receiveHitBehavior != m_currentBehavior )
            {
                const PhysContactsContainer& contacts = _hit->getContacts();
                for ( u32 i = 0; i < contacts.size(); ++i )
                {
                    const SCollidableContact& contact = contacts[i];
                    PolyLine* poly(NULL);
                    const PolyLineEdge* edge(NULL);

                    if ( contact.m_edgeIndex != U32_INVALID )
                    {
                        AIUtils::getPolyLine(contact.m_collidableUserData, contact.m_edgeIndex, poly, edge);
                        if ( poly && edge )
                        {
                            if ( poly->m_isCollisionActive )
                            {
                                const Ray_GameMaterial_Template* mat = static_cast<const Ray_GameMaterial_Template*>(World::getGameMaterial(edge->getGameMaterial()));
                                // specific case, use dangerous damage to know that the hit can't be computed
                                if ( mat && ( ( mat->getDangerousLevel() >= 2 ) || mat->getShooterBounce() ) )
                                {
                                    return;
                                }
                                break;
                            }
                        }
                    }
                }
            }
            
            m_hitAddAnimCursor = 0.f;
            m_lastHitTimer = k_sameHitSenderBeforeRehitDuration;
            
            receiveHit( _hit );
            if ( getHealth() <= 0 )
            {
                startDeath();
            }                   

            Actor* sender = AIUtils::getActor( _hit->getSender() );
            if ( sender != NULL )
            {
                EventHitSuccessful hitSuccessful;
                hitSuccessful.setSender( m_actor->getRef() );
                sender->onEvent(&hitSuccessful);

                if ( m_fxController != NULL )
                {
                    u32 fxHandle = m_fxController->playFeedback( _hit->getSender(), _hit->getActionFromHit(), m_actor->getRef() );
                    m_fxController->setFXPos( fxHandle, _hit->getFxPos() );
                }
            }
        }
    }

    //--------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterGardianAIComponent::startDeath()
    {
        m_actor->clearBinds( btrue );
        m_actor->getBinding()->unbindFromParent();
        Actor * parent = AIUtils::getActor( m_actor->getBinding()->getParent() );
        if ( parent != NULL )
        {
            if ( ObjBinding * binding = ObjBinding::getBindingFromObj( parent->getRef() ) )
            {
                binding->forceRemovePending();
            }
        }
        Super::startDeath();
    }

    //--------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterGardianAIComponent::receiveHit( HitStim* _hit, bbool _damageDependsOnLevel /*= btrue*/, u32 _forcedDamage /*= 0*/ )
    {
        Super::receiveHit( _hit, !m_isInvulnerable );
    }

    //--------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterGardianAIComponent::updateAnimInput()
    {
        Super::updateAnimInput();

        if (m_animatedComponent)
        {
            m_animatedComponent->setInput( ITF_GET_STRINGID_CRC(HitAddAnimCursor,2339373408), m_hitAddAnimCursor );
        }
    }
  
#ifdef ITF_SUPPORT_EDITOR
    //--------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterGardianAIComponent::onEditorMove(bbool _modifyInitialPos)
    {
        Super::onEditorMove(_modifyInitialPos);
    }
#endif // ITF_SUPPORT_EDITOR

    //--------------------------------------------------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(Ray_ShooterGardianAIComponent_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_ShooterGardianAIComponent_Template)
        SERIALIZE_MEMBER( "minHitStunLevel",            m_minHitStunLevel );
        SERIALIZE_MEMBER( "multiPlayerLifePointFactor", m_multiPlayerLifePointFactor );
        SERIALIZE_MEMBER( "deathRewardSpawnPath",       m_deathRewardSpawnPath );
        SERIALIZE_MEMBER( "deathRewardNumber",          m_deathRewardNumber );
        SERIALIZE_MEMBER( "deathRewardSpawnDuration",   m_deathRewardSpawnDuration );
        SERIALIZE_MEMBER( "deathRewardSpawnDist",       m_deathRewardSpawnDist );
        SERIALIZE_MEMBER( "deathRewardSpawnAngle",      m_deathRewardSpawnAngle );
    END_SERIALIZATION()

    Ray_ShooterGardianAIComponent_Template::Ray_ShooterGardianAIComponent_Template()
    : Super()
    , m_minHitStunLevel( 2 )
    , m_multiPlayerLifePointFactor( 1.0f )
    , m_deathRewardNumber( 1 )
    , m_deathRewardSpawnDuration( 2.0f )
    , m_deathRewardSpawnDist( Vec2d( 0.0f, 1.0f ) )
    , m_deathRewardSpawnAngle( Vec2d( 0.0f, MTH_PI ) )
    {
    }

    Ray_ShooterGardianAIComponent_Template::~Ray_ShooterGardianAIComponent_Template()
    {
    }
}

