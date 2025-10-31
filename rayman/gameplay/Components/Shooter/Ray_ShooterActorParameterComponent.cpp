#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_SHOOTERACTORPARAMETERCOMPONENT_H_
#include "rayman\gameplay\Components\Shooter\Ray_ShooterActorParameterComponent.h"
#endif //_ITF_RAY_SHOOTERACTORPARAMETERCOMPONENT_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_TWEENCOMPONENT_H_
#include "gameplay/Components/Common/TweenComponent.h"
#endif //_ITF_TWEENCOMPONENT_H_


namespace ITF
{
    //----------------------------------------------------------------------------------------------------------------------------------
    BEGIN_SERIALIZATION(Ray_VacuumData_Template)
        SERIALIZE_MEMBER("canBeVacuumed",				m_canBeVacuumed );
        SERIALIZE_MEMBER("vacuumMinDuration",			m_vacuumMinDuration);
        SERIALIZE_MEMBER("vacuumMaxDuration",			m_vacuumMaxDuration);
    END_SERIALIZATION()

    Ray_VacuumData_Template::Ray_VacuumData_Template()
    : m_canBeVacuumed(btrue)
    , m_vacuumMinDuration( 0.5f )
    , m_vacuumMaxDuration( 0.8f )
    {
    }
    //----------------------------------------------------------------------------------------------------------------------------------
    BEGIN_SERIALIZATION( Ray_ShooterActorParameter_StackData )
        SERIALIZE_MEMBER("projectilePath",	        m_projectilePath );
        SERIALIZE_MEMBER("numProjectiles",	        m_numProjectiles );
        SERIALIZE_MEMBER("max",				        m_max );
        SERIALIZE_MEMBER("inStackSize",		        m_inStackSize );
        SERIALIZE_MEMBER("notStackable",	        m_notStackable );
        SERIALIZE_MEMBER("notStackableKeepAlive",	m_notStackableKeepAlive );
        SERIALIZE_MEMBER("type",	                m_type );
    END_SERIALIZATION()

    //----------------------------------------------------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(Ray_ShooterActorParameterComponent)
    BEGIN_SERIALIZATION_CHILD(Ray_ShooterActorParameterComponent)
    //    //SERIALIZE_FUNCTION(checkLoadedData, ESerialize_Template_Load | ESerialize_PropertyEdit_Load);
    END_SERIALIZATION()

	//----------------------------------------------------------------------------------------------------------------------------------
    Ray_ShooterActorParameterComponent::Ray_ShooterActorParameterComponent()
    : Super()
	, m_playerEjectBehavior( NULL )
	, m_playerVacuumBehavior( NULL )
    , m_playerStartVacuumBehavior( NULL )
    , m_playerVacuumEffectiveBehavior( NULL )
	, m_rayAiComponent( NULL )
	, m_physComponent( NULL )
    , m_fxControllerComponent( NULL )
    , m_tweenComponent( NULL )
	, m_vacuuming( bfalse )
    , m_hasBeenVaccumed( bfalse )
    , m_wasVacuuming( bfalse )
	, m_vacuumSize( 0.f )
	, m_vacuumer( NULL )
	, m_vacuumSpeedFactor( 1.f )
	, m_actorPosUpdated( bfalse )
	, m_vacuumAnimTimer( 0.f )
	, m_vacuumAnimVector( Vec2d::Zero )
	, m_vacuumAnimDir( Vec2d::Zero )
	, m_vacuumAnimFactor( 0.f )
	, m_vacuumBlendTimer( 0.f )
    , m_vacuumingFxHandle( U32_INVALID )
    , m_vacuumedReward( NULL )
    , m_vacuumingGaugeTimer( 0.f )
    , m_deathRewardGiven( bfalse )
    , m_onVacuumedStim( NULL )
    , m_onEnemyReVacuumedStim( NULL )
    , m_enemyReVacuumed( bfalse )
    , m_enemyReVacuumedStackData( NULL )
    {		
    }

	Ray_ShooterActorParameterComponent::~Ray_ShooterActorParameterComponent()
	{
		// => behaviors ares registered in AI and destroyed by the AI
        SF_DEL( m_vacuumedReward );
        SF_DEL( m_onVacuumedStim );
        SF_DEL( m_onEnemyReVacuumedStim );
        SF_DEL( m_enemyReVacuumedStackData );
        
#ifdef ITF_SUPPORT_EDITOR
        EVENTMANAGER->unregisterEvent( ITF_GET_STRINGID_CRC(Ray_EventQueryLumCount,3794901482), this );
#endif //ITF_SUPPORT_EDITOR
	}

	//----------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterActorParameterComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
		Super::onActorLoaded(_hotReload);

		m_rayAiComponent = m_actor->GetComponent<Ray_AIComponent>();
		m_physComponent = m_actor->GetComponent<PhysComponent>();
        m_fxControllerComponent = m_actor->GetComponent<FXControllerComponent>();
        m_tweenComponent = m_actor->GetComponent<TweenComponent>();        

		if ( m_rayAiComponent != NULL )
		{
			//m_rayAiComponent->setVacuumDataTemplate( getTemplate()->getVaccumDataTemplate() );

			// Create behaviors
			if ( getTemplate()->getPlayerEjectBehavior() )
			{
				m_playerEjectBehavior = m_rayAiComponent->createAndAddAiBehaviorExtern( getTemplate()->getPlayerEjectBehavior() );
			}
			if ( getTemplate()->getVacuumBehavior()  )
			{
				m_playerVacuumBehavior = m_rayAiComponent->createAndAddAiBehaviorExtern( getTemplate()->getVacuumBehavior() );
			}
            if ( getTemplate()->getVacuumEffectiveBehavior() )
            {
                m_playerVacuumEffectiveBehavior = m_rayAiComponent->createAndAddAiBehaviorExtern( getTemplate()->getVacuumEffectiveBehavior() );
            }
            if ( getTemplate()->getStartVacuumBehavior() )
            {
                m_playerStartVacuumBehavior = m_rayAiComponent->createAndAddAiBehaviorExtern( getTemplate()->getStartVacuumBehavior() );
            }
		}

		ACTOR_REGISTER_EVENT_COMPONENT(m_actor, ITF_GET_STRINGID_CRC(Ray_EventShooterEjectActor,937196133), this );
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor, ITF_GET_STRINGID_CRC(Ray_EventShooterSwallow,48834168), this );
		ACTOR_REGISTER_EVENT_COMPONENT(m_actor, ITF_GET_STRINGID_CRC(VacuumStim,593568508), this );
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor, ITF_GET_STRINGID_CRC(Ray_EventCanBeVacuum,774659105), this );
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor, ITF_GET_STRINGID_CRC(Ray_EventIsInfluencedByVacuum,1958637106), this );
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor, ITF_GET_STRINGID_CRC(Ray_EventQueryHasBeenVacuumed,2297394390), this );
#ifdef ITF_SUPPORT_EDITOR
        EVENTMANAGER_REGISTER_EVENT_LISTENER(ITF_GET_STRINGID_CRC(Ray_EventQueryLumCount,3794901482),this);
#endif //ITF_SUPPORT_EDITOR

                
        // !!!! temp !!!!!
        // reset parent bind activation
        if ( m_actor->getParentBind() != NULL )
        {
            m_actor->getParentBind()->setRuntimeDisabled(bfalse);
        }

        reset();

        // Init reward
        //
        if ( getTemplate()->getVacuumedReward() != NULL )
        {
            m_vacuumedReward = getTemplate()->getVacuumedReward()->CreateNewObject()->DynamicCast<Ray_EventSpawnReward>(ITF_GET_STRINGID_CRC(Ray_EventSpawnReward,145553727));
            if ( m_vacuumedReward != NULL )
            {
                BinaryClone(const_cast<Ray_EventSpawnReward*>(getTemplate()->getVacuumedReward()),m_vacuumedReward);
            }
        }

        // death reward spwaner
        //
        m_deathRewardGiven = bfalse;
        if ( !getTemplate()->getDeathRewardSpawnPath().isEmpty() )
        {
            SPAWNER->declareNeedsSpawnee( m_actor, &m_deathRewardGenerator, getTemplate()->getDeathRewardSpawnPath() );
        }        

        // init stacked object projectile generator
        //
        if ( getStackData().needProjectileSpawnee() )
        {
            SPAWNER->declareNeedsSpawnee( m_actor, &m_projectileGenerator, getStackData().m_projectilePath );
        }

        // create vacuumed stim event
        //
        if ( getTemplate()->getOnVacuumedStim() != NULL )
        {
            m_onVacuumedStim = getTemplate()->getOnVacuumedStim()->CreateNewObject()->DynamicCast<PunchStim>(ITF_GET_STRINGID_CRC(PunchStim,200533519));
            if ( m_onVacuumedStim != NULL )
            {
                BinaryClone(const_cast<PunchStim*>(getTemplate()->getOnVacuumedStim()),m_onVacuumedStim);
            }
        }
        // create re-vacuumed stim event
        //
        if ( getTemplate()->getOnEnemyReVacuumedStim() != NULL )
        {
            m_onEnemyReVacuumedStim = getTemplate()->getOnEnemyReVacuumedStim()->CreateNewObject()->DynamicCast<PunchStim>(ITF_GET_STRINGID_CRC(PunchStim,200533519));
            if ( m_onEnemyReVacuumedStim != NULL )
            {
                BinaryClone(const_cast<PunchStim*>(getTemplate()->getOnEnemyReVacuumedStim()), m_onEnemyReVacuumedStim );
            }
        }
    }

    //----------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterActorParameterComponent::onBecomeInactive()
    {
        Super::onBecomeInactive();

        if ( ( m_rayAiComponent != NULL ) && ( m_rayAiComponent->getHealth() == 0 ) && !m_deathRewardGiven )
        {
            giveDeathReward();        
        }
    }

    //----------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterActorParameterComponent::reset()
    {
        m_vacuuming = bfalse;
        m_hasBeenVaccumed = bfalse;
        m_wasVacuuming = bfalse;
        m_vacuumer.invalidate();
        m_hasBeenVaccumedOwner.invalidate();
        m_vacuumAnimFactor = 0.f;
        m_vacuumBlendTimer = 0.f;
        m_actorPosUpdated = bfalse;
        m_vacuumAnimTimer = 0.f;
        m_vacuumAnimVector.set( 0.f, 0.f );

        m_vacuumingGaugeTimer = 0.f;
        m_enemyReVacuumed = bfalse;
        
        SF_DEL( m_enemyReVacuumedStackData );

        if ( m_rayAiComponent != NULL )
        {
            m_rayAiComponent->setHasBeenVacuumed( bfalse );
        }
    }

	//----------------------------------------------------------------------------------------------------------------------------------
	void Ray_ShooterActorParameterComponent::onEvent( Event * _event )
	{   
        //********************
        // influenced ?
        //********************
        if ( Ray_EventIsInfluencedByVacuum * influenced = _event->DynamicCast<Ray_EventIsInfluencedByVacuum>(ITF_GET_STRINGID_CRC(Ray_EventIsInfluencedByVacuum,1958637106)) )
        {
            ProcessEventIsInfluencedByVacuum( influenced );
        }
        //********************
        // can be vacuumed ?
        //********************
        else if ( Ray_EventCanBeVacuum * canBeVacuum = _event->DynamicCast<Ray_EventCanBeVacuum>(ITF_GET_STRINGID_CRC(Ray_EventCanBeVacuum,774659105)) )
        {
            ProcessEventCanBeVacuum( canBeVacuum );
        }
        //*********
        // EJECTED
        //*********
		else if ( Ray_EventShooterEjectActor * ejectActorEvent = _event->DynamicCast<Ray_EventShooterEjectActor>(ITF_GET_STRINGID_CRC(Ray_EventShooterEjectActor,937196133)) )
		{
            ProcessEventShooterEjectActor( ejectActorEvent );            
		}
        //***********
        // VACUUMED
        //***********
		else if ( VacuumStim * vacuum = _event->DynamicCast<VacuumStim>(ITF_GET_STRINGID_CRC(VacuumStim,593568508)) )
		{
            ProcessVacuumStim( vacuum );
		}
        //**********************
        // EFFECTIVE VACUUM
        //**********************
        else if ( Ray_EventShooterSwallow * swallowed = _event->DynamicCast<Ray_EventShooterSwallow>(ITF_GET_STRINGID_CRC(Ray_EventShooterSwallow,48834168)) )
        {
            ProcessEventShooterSwallow( swallowed );
        }
        //*******************
        // HAS BEEN VACUUMED
        //*******************
        else if ( Ray_EventQueryHasBeenVacuumed * vacummed = _event->DynamicCast<Ray_EventQueryHasBeenVacuumed>(ITF_GET_STRINGID_CRC(Ray_EventQueryHasBeenVacuumed,2297394390)) )
        {
            vacummed->setHasBeenVacuumed( m_hasBeenVaccumed );
            vacummed->setVacuumedOwner( m_hasBeenVaccumedOwner );
        }
        //**************
        // LUMS QUERY
        //**************
        else if ( Ray_EventQueryLumCount *queryLumCount = _event->DynamicCast<Ray_EventQueryLumCount>(ITF_GET_STRINGID_CRC(Ray_EventQueryLumCount,3794901482)))
        {
            if ( m_actor->getWorld() != SPAWNER->getSpawnerWorld() )
            {
                // is really lums reward ?
                const Ray_EventSpawnRewardLum * rewardLums = IRTTIObject::SafeDynamicCast<const Ray_EventSpawnRewardLum>( getTemplate()->getVacuumedReward(), ITF_GET_STRINGID_CRC(Ray_EventSpawnRewardLum,812488722) );
                if ( rewardLums != NULL )
                {
                    queryLumCount->addNormalLums( rewardLums->getNumRewards() );
                }
            }
        }
	}

    //----------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterActorParameterComponent::ProcessEventIsInfluencedByVacuum( Ray_EventIsInfluencedByVacuum * _evt )
    {
        // not me
        if ( _evt->getSender() != m_actor->getRef() )
            return;

        // dead
        if ( ( m_rayAiComponent != NULL ) && ( m_rayAiComponent->getHealth() == 0 ) )
        {
            _evt->setInfluenced( bfalse );
            return;
        }
        // parent bind, ask parent
        if ( ( m_actor->getParentBind() != NULL ) && !m_actor->getParentBind()->isRuntimeDisabled() )
        {
            if ( Actor* parent = m_actor->getParent().getActor() )
            {
                parent->onEvent(_evt);
            }
        }

        // check if the bhv can be launched
        if (    ( m_rayAiComponent != NULL ) && _evt->getInfluenced() && 
            ( m_rayAiComponent->getCurrentBehavior() != m_playerStartVacuumBehavior ) &&
            ( m_rayAiComponent->getCurrentBehavior() != m_playerVacuumBehavior  ) 
            )
        {
            if ( m_playerStartVacuumBehavior != NULL )
            {
                _evt->setInfluenced( canChangeToBehavior( m_playerStartVacuumBehavior ) );
            }
            else if ( m_playerVacuumBehavior != NULL )
            {
                _evt->setInfluenced( canChangeToBehavior( m_playerVacuumBehavior ) );
            }
        }
    }

    //----------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterActorParameterComponent::ProcessEventCanBeVacuum( Ray_EventCanBeVacuum * _evt )
    {
        // not me
        if ( _evt->getSender() != m_actor->getRef() )
            return;

        // dead
        if ( m_rayAiComponent->getHealth() == 0 )
        {
            _evt->setCanBeVaccum( bfalse );
            return;
        }
        // parent bind, ask parent
        if ( ( m_actor->getParentBind() != NULL ) && !m_actor->getParentBind()->isRuntimeDisabled() )
        {
            if ( Actor* parent = m_actor->getParent().getActor() )
            {
                parent->onEvent(_evt);
            }
        }
    }

    //----------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterActorParameterComponent::ProcessEventShooterEjectActor( Ray_EventShooterEjectActor * _evt )
    {
        // Reset vacuum data
        //m_rayAiComponent->resetVaccumed();
        // re-enable the phys
        if ( m_physComponent != NULL )
        {
            m_physComponent->setDisabled( bfalse );
        }
        reset();

        if ( m_playerEjectBehavior != NULL )
        {	
            EventChangeBehaviourQuery changeToEjectBhv;
            changeToEjectBhv.setWantedBehaviour( m_playerEjectBehavior );
            m_actor->onEvent( &changeToEjectBhv );
            m_playerEjectBehavior->onEvent( _evt );
        }
        playVacuumFx( Ejected );
    }


    //----------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterActorParameterComponent::ProcessVacuumStim( VacuumStim * _evt )
    {
        if ( m_hasBeenVaccumed )
        {
            return;
        }
        Ray_EventIsInfluencedByVacuum influenced;
        influenced.setSender( m_actor->getRef() );
        m_actor->onEvent( &influenced );
        if ( !influenced.getInfluenced() )
        {
            return;
        }
        AIBehavior * bhvToStart = NULL;

        if ( m_playerStartVacuumBehavior && 
            ( m_rayAiComponent->getCurrentBehavior() != m_playerStartVacuumBehavior ) &&
            ( m_rayAiComponent->getCurrentBehavior() != m_playerVacuumBehavior )
            )
        {
            bhvToStart = m_playerStartVacuumBehavior;
        }
        else if (   m_playerVacuumBehavior && 
            ( m_rayAiComponent->getCurrentBehavior() != m_playerStartVacuumBehavior ) &&
            ( m_rayAiComponent->getCurrentBehavior() != m_playerVacuumBehavior )
            )
        {
            bhvToStart = m_playerVacuumBehavior;
        }

        if ( bhvToStart != NULL )
        {
            EventChangeBehaviourQuery newBhv;
            newBhv.setWantedBehaviour( bhvToStart );
            m_actor->onEvent( &newBhv );
        }
        m_vacuumer			= _evt->getSender();
        m_vacuumSize		= _evt->getVacuumDist();
        m_vacuumSpeedFactor = _evt->getVacuumSpeedFactor();
        m_vacuuming			= btrue;
    }

    //----------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterActorParameterComponent::ProcessEventShooterSwallow( Ray_EventShooterSwallow * _evt )
    {
        if ( !m_hasBeenVaccumed )
        {
            playVacuumFx( Detached );
        }
        m_hasBeenVaccumed = btrue;
        m_hasBeenVaccumedOwner = _evt->getSender();

        // send vacuum reward
        //---------------------
        if ( ( m_vacuumedReward != NULL ) && _evt->getSender().isValid() )
        {
            m_vacuumedReward->clear();
            m_vacuumedReward->setup(m_actor);
            m_vacuumedReward->setRewardReceiver( _evt->getSender() );
            m_vacuumedReward->setEjectionDir( -Vec2d::Right );
            RAY_GAMEMANAGER->onEvent(m_vacuumedReward);

            // concider death reward given
            m_deathRewardGiven = btrue;
        }
        
        // send vacuumed stim ?
        if ( m_onVacuumedStim != NULL )
        {
            Actor * owner = AIUtils::getActor( m_hasBeenVaccumedOwner );
            if ( owner != NULL )
            {
                m_onVacuumedStim->setReceivedHitType( RECEIVEDHITTYPE_EARTHQUAKE ); // use earthquake type, to bypass the is vacuumed check
                m_onVacuumedStim->setSender( m_actor->getRef() );
                m_onVacuumedStim->setDirection( -Vec2d::Right );
                m_onVacuumedStim->setPos( m_actor->get2DPos() );
                m_onVacuumedStim->setPrevPos( m_actor->get2DPos() );
                owner->onEvent( m_onVacuumedStim );
            }            
        }

        // re-vacuumed stim send
        if ( m_enemyReVacuumed )
        {
            if ( getTemplate()->getEnemyReVacuumedSwallowedBhvName().isValid() )
            {
                EventChangeBehaviourQuery newBhv;
                newBhv.setWantedBehaviourName( getTemplate()->getEnemyReVacuumedSwallowedBhvName() );
                m_actor->onEvent( &newBhv );
                // re-enable the actor to allow bhv playing
                m_actor->enable();
            }

            if ( m_onEnemyReVacuumedStim != NULL )
            {
                Actor * owner = AIUtils::getActor( m_hasBeenVaccumedOwner );
                if ( owner != NULL )
                {
                    m_onEnemyReVacuumedStim->setReceivedHitType( RECEIVEDHITTYPE_EARTHQUAKE ); // use earthquake type, to bypass the is vacuumed check
                    m_onEnemyReVacuumedStim->setSender( m_actor->getRef() );
                    m_onEnemyReVacuumedStim->setDirection( -Vec2d::Right );
                    m_onEnemyReVacuumedStim->setPos( m_actor->get2DPos() );
                    m_onEnemyReVacuumedStim->setPrevPos( m_actor->get2DPos() );
                    owner->onEvent( m_onEnemyReVacuumedStim );
                }
            }
            
            m_enemyReVacuumed = bfalse;
        }
    }

    //----------------------------------------------------------------------------------------------------------------------------------
    bbool Ray_ShooterActorParameterComponent::canChangeToBehavior( AIBehavior * _bvh )
    {
        if ( m_rayAiComponent == NULL )
            return bfalse;

        // current bhv is extern ?
        //
        if ( ( _bvh != NULL ) && m_rayAiComponent->getCurrentBehavior()->isExtern() )
        {
            EventChangeExternBhvValidationQuery query( _bvh->getTemplate()->getName() );
            m_actor->onEvent(&query);
            return query.isChangeValidated();
        }
        return btrue;
    }

    //----------------------------------------------------------------------------------------------------------------------------------
    bbool Ray_ShooterActorParameterComponent::canBeVacuumed() 
    {  
        if ( getTemplate()->getVaccumDataTemplate()->getCanBeVacuumed() && !m_hasBeenVaccumed )
        {
            Ray_EventCanBeVacuum canBeVacuum;
            canBeVacuum.setSender( m_actor->getRef() );
            m_actor->onEvent( &canBeVacuum );
            return canBeVacuum.getCanBeVaccum();
        }
        return bfalse;
    }

    //----------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterActorParameterComponent::onVacuumed()
    {
        Actor * vacuumer = m_vacuumer.getActor();
        if ( vacuumer != NULL )
        {
            Ray_EventShooterVacuumed event;
            event.setSender(m_actor->getRef());
            vacuumer->onEvent(&event);
            m_hasBeenVaccumed = event.isVaccumEffective();
            if ( m_hasBeenVaccumed )
            {
                if ( m_rayAiComponent != NULL )
                {
                    // is re-vacuumed during eject by a non player actor
                    //
                    if ( ( m_rayAiComponent->getCurrentBehavior() == m_playerEjectBehavior ) &&
                         ( AIUtils::getFaction( m_actor ) == RAY_FACTION_PLAYER ) &&
                         AIUtils::isEnemy( AIUtils::getFaction( m_actor ), AIUtils::getFaction( vacuumer ) )
                        )
                    {
                        // Change stack data to unstackable if re-vacuumed bhv exist
                        //
                        if ( getTemplate()->getEnemyReVacuumedSwallowedBhvName().isValid() )
                        {
                            if ( m_enemyReVacuumedStackData == NULL )
                            {
                                m_enemyReVacuumedStackData = newAlloc( mId_Gameplay, Ray_ShooterActorParameter_StackData() );
                                if ( m_enemyReVacuumedStackData != NULL )
                                {
                                    BinaryClone(const_cast<Ray_ShooterActorParameter_StackData*>(getTemplate()->getStackDataPtr()),m_enemyReVacuumedStackData );
                                }
                            }
                            if ( m_enemyReVacuumedStackData != NULL )
                            {
                                m_enemyReVacuumedStackData->m_notStackable = btrue;
                                m_enemyReVacuumedStackData->m_notStackableKeepAlive = btrue;
                            }
                        }

                        m_enemyReVacuumed = btrue;
                    }

                    if ( ( m_playerVacuumEffectiveBehavior != NULL ) && ( m_rayAiComponent->getCurrentBehavior() != m_playerVacuumEffectiveBehavior ) )
                    {
                        EventChangeBehaviourQuery newBhv;
                        newBhv.setWantedBehaviour( m_playerVacuumEffectiveBehavior );
                        m_actor->onEvent( &newBhv );
                    }

                    m_rayAiComponent->setHasBeenVacuumed( btrue );
                }

                playVacuumFx( Detached );

                m_hasBeenVaccumedOwner = m_vacuumer;                

                // unbind
                if ( m_actor->getParentBind() )
                {
                    m_actor->getParentBind()->setRuntimeDisabled( btrue );
                }                            
                else if ( m_actor->getBinding()->isBound() )
                {
                    m_actor->getBinding()->unbindFromParent();
                }

                // disable the phys
                if ( m_physComponent != NULL )
                {
                    m_physComponent->setDisabled( btrue );
                }

                // disable tween components
                if ( m_tweenComponent != NULL )
                {
                    m_tweenComponent->suspendTweening();
                }

                Ray_EventHasBeenVacuumed hasBeenVacuumed;
                m_actor->onEvent( &hasBeenVacuumed );
            }
        }
    }

	//----------------------------------------------------------------------------------------------------------------------------------
	void Ray_ShooterActorParameterComponent::onUpdatePos()
	{
		// Position has been updated, re-add vacuum vector
		//m_actor->set2DPos( m_actor->get2DPos() + m_vacuumAnimVector );
		//m_actorPosUpdated = btrue; //m_physComponent ? m_physComponent->getDisabled() :btrue;
	}

    //----------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterActorParameterComponent::get2DPosWithoutVacuumInfluence( Vec2d & _resPos ) const
    {
        _resPos = m_actor->get2DPos();

        if ( !m_actorPosUpdated )
        {
            if ( m_actor->getParentBind() && !m_actor->getParentBind()->isRuntimeDisabled() )
            {
                _resPos = ( m_actor->getBoundLocal2DPos() - m_vacuumAnimVector.Rotate( m_actor->getBoundLocalAngle() ) );
            }
            else
            {
                _resPos -= m_vacuumAnimVector; 
            }
        }
    }

	//----------------------------------------------------------------------------------------------------------------------------------
	void Ray_ShooterActorParameterComponent::updateVacuumAnim( f32 _dt )
	{
		const f32 vacuumBlendDuration = 0.2f;

        // has a no tween component activated ?
		if ( !m_actorPosUpdated && ( m_tweenComponent == NULL ) || !m_tweenComponent->isPlayingSet() )
		{
            if ( m_actor->getParentBind() && !m_actor->getParentBind()->isRuntimeDisabled() )
            {
                Vec2d pos = ( m_actor->getBoundLocal2DPos() - m_vacuumAnimVector.Rotate( m_actor->getBoundLocalAngle() ) );
                m_actor->setBoundLocal2DPos( pos );
            }
            else
            {
			    m_actor->set2DPos( m_actor->get2DPos() - m_vacuumAnimVector );
            }
		}
        m_actorPosUpdated = bfalse;
		
		// vacuuming
		//-----------
		Actor * vacuumer = m_vacuumer.getActor();
		if ( m_vacuuming && ( vacuumer != NULL ) )
		{
			if ( !( m_vacuumBlendTimer != 0.f ) )
			{
				m_vacuumAnimTimer = Seeder::getSharedSeeder().GetFloat( 0.f, MTH_PI );
			}			

			if ( m_vacuumSize > 0.f )
			{
				m_vacuumAnimDir = vacuumer->get2DPos() - m_actor->get2DPos();
				f32 vacuumerDist = m_vacuumAnimDir.norm();
				if ( vacuumerDist != 0.f )
				{
					m_vacuumAnimDir = m_vacuumAnimDir / vacuumerDist;
				}
				else
				{
					m_vacuumAnimDir = Vec2d::Zero;
				}
				m_vacuumAnimFactor = ( 1.0f - ( Min( vacuumerDist, m_vacuumSize ) / m_vacuumSize ) );
			}

			m_vacuumBlendTimer += _dt;
		}
		// no more vacuum
		else
		{
			m_vacuumBlendTimer -= _dt;
		}

		// Update blend timer
		m_vacuumBlendTimer = Max( Min( m_vacuumBlendTimer, vacuumBlendDuration ), 0.0f );

		// update vacuum anim data
		m_vacuumAnimVector = Vec2d::Zero;
		if ( m_vacuumBlendTimer > 0.f )
		{
			m_vacuumAnimTimer += _dt * ( 20.f  + ( m_vacuumAnimFactor * 10.f ) );
			m_vacuumAnimVector = m_vacuumAnimDir * abs ( f32_Sin( m_vacuumAnimTimer ) ) * ( 0.1f + ( m_vacuumAnimFactor * 0.4f ) );
			m_vacuumAnimVector *= ( m_vacuumBlendTimer / vacuumBlendDuration );
            
            if ( m_actor->getParentBind() && !m_actor->getParentBind()->isRuntimeDisabled() )
            {
                Vec2d pos = ( m_actor->getBoundLocal2DPos() + m_vacuumAnimVector.Rotate( m_actor->getBoundLocalAngle() ) );
                m_actor->setBoundLocal2DPos( pos );
            }
            else
            {
                m_actor->set2DPos( m_actor->get2DPos() + m_vacuumAnimVector );
                if ( m_physComponent != NULL )
                {
                    m_physComponent->onUpdatePos();
                }
            }
		}
	}

    //----------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterActorParameterComponent::updateVacuumGauge( f32 _dt )
    {
        //**********************
		// Vacuum gauge update 
		//**********************

		// increase gauge
		if ( m_vacuuming && !m_hasBeenVaccumed )
        {
			const f32 minVacuumDuration = getTemplate()->getVaccumDataTemplate()->getVacuumMinDuration();
			const f32 maxVacuumDuration = getTemplate()->getVaccumDataTemplate()->getVacuumMaxDuration();
			f32 moreDuration = 0.0f;

			Actor * actor = m_vacuumer.getActor();
			if ( actor && ( m_vacuumSize > 0.0 ) )
			{
				 f32 dist = (m_actor->get2DPos() - actor->get2DPos()).norm();
				 moreDuration = Max( 0.0f, maxVacuumDuration - minVacuumDuration ) * Min( 1.0f, dist / m_vacuumSize );
			}			
			m_vacuumingGaugeTimer += _dt * m_vacuumSpeedFactor;

			// is vaccumed... 
			if ( ( m_vacuumingGaugeTimer >= ( minVacuumDuration + moreDuration ) ) && canBeVacuumed() )
			{
				m_vacuumingGaugeTimer = minVacuumDuration + moreDuration;
				onVacuumed();
			}
		}
		// decrease gauge
		else if ( m_vacuumingGaugeTimer > 0.0f )
		{
			m_vacuumingGaugeTimer -= _dt;
			m_vacuumingGaugeTimer = Max( 0.0f, m_vacuumingGaugeTimer );
		}
    }
	//----------------------------------------------------------------------------------------------------------------------------------
	void Ray_ShooterActorParameterComponent::Update( f32 _dt )
	{
		Super::Update( _dt );

        // update gauge
        //---------------
        updateVacuumGauge( _dt );

		// Behavior update
		//--------------------
		if (    !m_vacuuming && m_rayAiComponent && 
                ( ( m_rayAiComponent->getCurrentBehavior() == m_playerVacuumBehavior ) ||
                  ( m_rayAiComponent->getCurrentBehavior() == m_playerStartVacuumBehavior )
                )
           )
		{
			m_rayAiComponent->onBehaviorExternFinished( btrue );
		}
        
		// Vacuum anim update
		//if ( !m_physComponent || m_physComponent->getDisabled() )
		{
			updateVacuumAnim( _dt );
		}

        // FX update
        if ( !m_vacuuming && m_wasVacuuming )
        {
            playVacuumFx( VacuumingStop );
        }
        else if ( m_vacuuming && !m_wasVacuuming )
        {
            playVacuumFx( Vacuuming );
        }
        m_wasVacuuming = m_vacuuming;
		m_vacuuming = bfalse;

        // death reward 
        //
        if ( !m_deathRewardGiven && ( m_rayAiComponent != NULL ) && ( m_rayAiComponent->getHealth() == 0 ) )
        {
            giveDeathReward();
        }
	}

    //----------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterActorParameterComponent::playVacuumFx( vacuumFxType _type )
    {
        if ( m_fxControllerComponent == NULL )
            return;

        const StringID & fxName = getTemplate()->getVacuumFXByType(_type);
        u32 fxHandle = U32_INVALID;
        if ( fxName.isValid() )
        {
            fxHandle = m_fxControllerComponent->playFX( fxName );
            m_fxControllerComponent->setFXPos( fxHandle, m_actor->getPos() );
        }
        
        if ( _type == Vacuuming )
        {
            m_vacuumingFxHandle = fxHandle;
        }
        else if ( ( _type == VacuumingStop ) && ( m_vacuumingFxHandle != U32_INVALID ) )
        {
            m_fxControllerComponent->stopFX( m_vacuumingFxHandle );
            m_vacuumingFxHandle = U32_INVALID;
        }                
    }

    //----------------------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterActorParameterComponent::giveDeathReward()
    {
        if ( !m_deathRewardGiven && m_deathRewardGenerator.isInitialized() && ( getTemplate()->getDeathRewardNumber() > 0 ) )
        {
            Vec2d initSpawnPos = m_actor->get2DPos();
            Vec2d spawnPos = initSpawnPos;

            for ( u32 i = 0; i < getTemplate()->getDeathRewardNumber(); ++i )
            {
                spawnPos = initSpawnPos;
                if ( i > 0 )
                {
                    Vec2d offsetVec = Vec2d::Right.Rotate( Seeder::getSharedSeeder().GetFloat( -MTH_PI, MTH_PI ) );
                    offsetVec *= Seeder::getSharedSeeder().GetFloat( 0.2f, 0.5f );
                    spawnPos += offsetVec;
                }
                m_deathRewardGenerator.getSpawnee( CURRENTWORLD->getRootScene(), spawnPos.to3d( m_actor->getPos().m_z ), 0.f );
            }    

            //Vec2d spawnPos = m_actor->get2DPos();
            //// create the reward
            //Actor * rewardActor = m_deathRewardGenerator.getSpawnee( m_actor->getScene(), spawnPos.to3d(( m_actor->getPos().m_z ) ), 0.f );
            //if ( rewardActor != NULL )
            //{
            //    Ray_EventSetBubblePrizeRewardNumber rewardNumChangeEvt( getTemplate()->getDeathRewardNumber() );
            //    rewardActor->onEvent( &rewardNumChangeEvt );
            //}

            m_deathRewardGiven = btrue;
        }
    }
	//----------------------------------------------------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------------------------------------------------
	IMPLEMENT_OBJECT_RTTI(Ray_ShooterActorParameterComponent_Template)
    BEGIN_SERIALIZATION(Ray_ShooterActorParameterComponent_Template)
		SERIALIZE_OBJECT("vacuumData",							        m_vacuumData);
		SERIALIZE_OBJECT_WITH_FACTORY("playerEjectBehavior",	        m_playerEjectBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("playerStartVaccumBehavior",	    m_playerStartVacuumBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
		SERIALIZE_OBJECT_WITH_FACTORY("playerVacuumBehavior",	        m_playerVacuumBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("playerVacuumEffectiveBehavior",	m_playerVacuumEffectiveBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
        SERIALIZE_MEMBER("enemyReVacuumedSwallowedBhvName",	            m_enemyReVacuumedSwallowedBhvName );

        SERIALIZE_OBJECT( "vacuumFxNamesContainer",                     m_vacuumFxNamesContainer );
        SERIALIZE_OBJECT_WITH_FACTORY("vacuumedReward",                 m_vacuumedReward, ACTORSMANAGER->getEventFactory() );
        SERIALIZE_MEMBER(    "deathRewardSpawnPath",                    m_deathRewardSpawnPath );
        SERIALIZE_MEMBER(    "deathRewardNumber",                       m_deathRewardNumber );

        SERIALIZE_OBJECT( "stackData",                                  m_stackData );
        SERIALIZE_OBJECT_WITH_FACTORY("onVacuumedStim",                 m_onVacuumedStim, ACTORSMANAGER->getEventFactory() );
        SERIALIZE_OBJECT_WITH_FACTORY("onEnemyReVacuumedStim",          m_onEnemyReVacuumedStim, ACTORSMANAGER->getEventFactory() );
    END_SERIALIZATION()

    BEGIN_SERIALIZATION_SUBCLASS( Ray_ShooterActorParameterComponent_Template, VacuumFxNames )
        SERIALIZE_MEMBER("fxNameVacuuming",				                m_fxNameVacuuming );
        SERIALIZE_MEMBER("fxNameVacuumingStop",				            m_fxNameVacuumingStop );
        SERIALIZE_MEMBER("fxNameDetached",				                m_fxNameDetached );
        SERIALIZE_MEMBER("fxNameEjected",				                m_fxNameEjected );
    END_SERIALIZATION()


	Ray_ShooterActorParameterComponent_Template::Ray_ShooterActorParameterComponent_Template()
	: Super()
	, m_playerEjectBehavior( NULL )
	, m_playerVacuumBehavior( NULL )
    , m_playerStartVacuumBehavior( NULL )
    , m_playerVacuumEffectiveBehavior( NULL )
    , m_vacuumedReward( NULL )
    , m_deathRewardNumber( 1 )
    , m_onVacuumedStim( NULL )
    , m_onEnemyReVacuumedStim( NULL )
	{
	}
	Ray_ShooterActorParameterComponent_Template::~Ray_ShooterActorParameterComponent_Template()
	{
		SF_DEL( m_playerEjectBehavior );
		SF_DEL( m_playerVacuumBehavior );
        SF_DEL( m_playerStartVacuumBehavior );
        SF_DEL( m_playerVacuumEffectiveBehavior );	
        SF_DEL( m_vacuumedReward );	
        SF_DEL( m_onVacuumedStim );
        SF_DEL( m_onEnemyReVacuumedStim );
    }

    //----------------------------------------------------------------------------------------------------------------------------------
    const StringID & Ray_ShooterActorParameterComponent_Template::getVacuumFXByType( Ray_ShooterActorParameterComponent::vacuumFxType _type ) const
    {
        switch( _type )
        {
            case Ray_ShooterActorParameterComponent::Vacuuming :        return m_vacuumFxNamesContainer.m_fxNameVacuuming;
            case Ray_ShooterActorParameterComponent::VacuumingStop :    return m_vacuumFxNamesContainer.m_fxNameVacuumingStop;
            case Ray_ShooterActorParameterComponent::Detached :         return m_vacuumFxNamesContainer.m_fxNameDetached;
            case Ray_ShooterActorParameterComponent::Ejected :          return m_vacuumFxNamesContainer.m_fxNameEjected;
        }
        return StringID::Invalid;
    }

} // namespace ITF