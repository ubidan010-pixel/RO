#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AISHOOTERFLYINGBOMBBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/Shooter/Ray_AIShooterFlyingBombBehavior.h"
#endif //_ITF_RAY_AISHOOTERFLYINGBOMBBEHAVIOR_H_

#ifndef _ITF_VIRTUALLINKCOMPONENT_H_
#include "gameplay/components/misc/VirtualLinkComponent.h"
#endif //_ITF_VIRTUALLINKCOMPONENT_H_

#ifndef _ITF_RAY_AICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_AIComponent.h"
#endif //_ITF_RAY_AICOMPONENT_H_

#ifndef _ITF_AIBEZIERACTION_H_
#include "gameplay/ai/Actions/AIBezierAction.h"
#endif //_ITF_AIBEZIERACTION_H_

#ifndef _ITF_DETECTORCOMPONENT_H_
#include "gameplay/Components/Trigger/DetectorComponent.h"
#endif //_ITF_DETECTORCOMPONENT_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#ifdef SHOOTER_PROFILING
#include "rayman/gameplay/Managers/Ray_ShooterGameModeController.h"
#include "rayman/gameplay/Ray_GameManager.h"
#endif //SHOOTER_PROFILING

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_AIShooterFlyingBombBehavior)
		BEGIN_VALIDATE_BEHAVIOR(Ray_AIShooterFlyingBombBehavior)
    END_VALIDATE_BEHAVIOR()

    #define s_AlarmCursor ITF_GET_STRINGID_CRC(AlarmCursor,2014025844)
    #define s_WarnCursor ITF_GET_STRINGID_CRC(WarnCursor,2610543747)

    Ray_AIShooterFlyingBombBehavior::Ray_AIShooterFlyingBombBehavior()
        : Super()
		, m_moveAction( NULL )
        , m_openingAction( NULL )
        , m_alarmAction( NULL )
        , m_explodeAction( NULL )
        , m_alarmTimer( 0.0f )
        , m_alarmBlinkTimer( 0.0f )
        , m_alarmBlinkAnimDuration( 0.0f )
        , m_warnAnimDuration( 0.0f )
        , m_hitTimer( 0.0f )
        , m_multiHitTimer( 0.0f )
    {
    }

    Ray_AIShooterFlyingBombBehavior::~Ray_AIShooterFlyingBombBehavior()
    {
    }

	//---------------------------------------------------------------------------------------------------------------
	void Ray_AIShooterFlyingBombBehavior::onActorLoaded(Pickable::HotReloadType _hotReload)
	{
		// Create actions
		if ( getTemplate()->getMoveActionTemplate() )
		{
			m_moveAction = createAiActionAndCast<AIBezierAction>( getTemplate()->getMoveActionTemplate() );
        }
        if ( getTemplate()->getOpeningActionTemplate() )
        {
            m_openingAction = createAiAction( getTemplate()->getOpeningActionTemplate() );
        }
        if ( getTemplate()->getWaitBeforeExplodeActionTemplate() )
        {
            m_alarmAction = createAiAction( getTemplate()->getWaitBeforeExplodeActionTemplate() );
        }
        if ( getTemplate()->getExplodeActionTemplate() )
        {
            m_explodeAction = createAiAction( getTemplate()->getExplodeActionTemplate() );
        }

        // after action add
		Super::onActorLoaded(_hotReload);

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor, ITF_GET_STRINGID_CRC(EventTrigger,1343042510), m_aiComponent );
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor, ITF_GET_STRINGID_CRC(HitStim,2121727796), m_aiComponent );

        m_detectorComponent =  m_actor->GetComponent<DetectorComponent>();

#ifdef SHOOTER_PROFILING
        if ( !m_actor->isSerializable() )
        {
            SP_ADD( "FlyingBomb", m_actor->getRef() )
        }
#endif //SHOOTER_PROFILING

	}

    //---------------------------------------------------------------------------------------------------------------
    void Ray_AIShooterFlyingBombBehavior::onResourceReady()
    {
        Super::onResourceReady();

        ITF_ASSERT( m_animComponent != NULL );
        if ( m_animComponent != NULL )
        {
            m_animComponent->setBankState( getTemplate()->getAnimationBankState() );
        }
    }

	//---------------------------------------------------------------------------------------------------------------
	void Ray_AIShooterFlyingBombBehavior::onActivate()
	{
       /* m_lastHitActorsList.clear();
        m_lastHitActorsFrameList.clear();*/
        m_multiHitTimer = 0.0f;

        // drop directly in the map ?
        if ( !getTemplate()->isTriggered() )
        {
		    if ( m_openingAction != NULL )
		    {
			    startOpeningAction();
		    }
            else
            {
                startAlarmAction();
            }
        }
	}

    //---------------------------------------------------------------------------------------------------------------
    void Ray_AIShooterFlyingBombBehavior::onDeactivate()
    {
        if ( ( m_currentAction != NULL ) && ( m_currentAction == m_moveAction  ) )
        {
            m_actor->setScale( m_destinationScale );
        }

        Super::onDeactivate();

        if ( m_animatedComponent != NULL )
        {
            m_animatedComponent->setInput( s_AlarmCursor, 0.0f );
            m_animatedComponent->setInput( s_WarnCursor, 0.0f );
        }
    }

    //---------------------------------------------------------------------------------------------------------------
    void Ray_AIShooterFlyingBombBehavior::onActorBecomeInactive()
    {
        Super::onActorBecomeInactive();

        Ray_AIComponent * rayai = m_aiComponent->DynamicCast<Ray_AIComponent>(ITF_GET_STRINGID_CRC(Ray_AIComponent,2143961386));

        // spawned, destroy it
        if ( !m_actor->isSerializable() && m_actor->isSpawned() && ( ( rayai == NULL ) || !rayai->hasBeenVacuumed() ) )
        {
            m_actor->requestDestruction();
        }
    }
   
	//---------------------------------------------------------------------------------------------------------------
	void Ray_AIShooterFlyingBombBehavior::onEvent( Event * _event)
	{
		Super::onEvent( _event );

        if ( EventTrigger* trigger = _event->DynamicCast<EventTrigger>(ITF_GET_STRINGID_CRC(EventTrigger,1343042510)) )
        {
            if ( trigger->getActivated() )
            {
                BaseObject * baseobj = trigger->getSender().getObject();
                if ( baseobj != NULL )
                {
                    if ( Actor * sender = baseobj->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115)) )
                    {
                        Ray_EventDynamicFogParamQuery forParamQuery;
                        forParamQuery.setSender( m_actor->getRef() );
                        sender->onEvent( &forParamQuery );
                        u32 fogColor;
                        f32 fogMaxDepth;
                        
                        if ( forParamQuery.getFogParams( fogColor, fogMaxDepth ) )
                        {
                            Ray_EventDynamicFogParamSet forParamSet( fogColor, fogMaxDepth );
                            forParamSet.setSender( m_actor->getRef() );  
                            m_actor->onEvent( &forParamSet );
                        }

                        m_destinationActor = sender->getRef();
                        VirtualLinkComponent * vlinkCp = sender->GetComponent<VirtualLinkComponent>();
                        if ( vlinkCp != NULL )
                        {
                            m_destinationActor = vlinkCp->getLastBroadcastEmitter();
                        }
                        startMoveAction();
                    }
                }
            }
        }
        else if ( HitStim* hitevt = _event->DynamicCast<HitStim>(ITF_GET_STRINGID_CRC(HitStim,2121727796)) )
        {
            processHit( hitevt );
        }
	}

    //---------------------------------------------------------------------------------------------------------------
    void Ray_AIShooterFlyingBombBehavior::processHit( HitStim * _stim )
    {
        Actor* sender = AIUtils::getActor( _stim->getSender() );
        if ( sender == NULL )
        {
            return;
        }

        if ( !AIUtils::isEnemyHit( _stim,m_aiComponent->getFaction() ) )
        {
            return;
        }
        if ( m_currentAction == m_explodeAction )
        {
            return;
        }
        if ( !m_actor->isEnabled() || !m_actor->isActive() )
        {
            return;
        }
        /*if ( !processHitSender( _stim->getSender() ) )
        {
            return;
        }*/
        if ( m_multiHitTimer == 0.0f )
        {
            m_aiComponent->receiveHit( _stim );
            m_multiHitTimer = 0.2f;
            if ( m_aiComponent->getHealth() <= 0 )
            {
                startExplodeAction();
            }
        }
        m_hitTimer = m_warnAnimDuration;

        if (m_fxController)
        {
            u32 fxHandle = m_fxController->playFeedback( _stim->getSender(), _stim->getActionFromHit(), m_actor->getRef() );
            m_fxController->setFXPosFromHandle(fxHandle, _stim->getFxPos());
        }

        // notify the sender that he touched us
        if ( sender != NULL )
        {
            EventHitSuccessful hitSuccessful;
            hitSuccessful.setSender(m_actor->getRef());
            hitSuccessful.setHasBeenDamaged( btrue );
            sender->onEvent(&hitSuccessful);
        }
    }

    //---------------------------------------------------------------------------------------------------------------
    /*bbool Ray_AIShooterFlyingBombBehavior::processHitSender( ObjectRef _sender )
    {
        bbool process = btrue;
        bbool addSender = btrue;

        // check object list
        //
        for ( u32 i = 0; i < m_lastHitActorsFrameList.size(); ++i )
        {
            ObjectRef & actor = m_lastHitActorsList[i];
            u32 & frame = m_lastHitActorsFrameList[i];

            if ( frame < ( CURRENTFRAME - 1 ) )
            {
                if ( actor != _sender )
                {
                    m_lastHitActorsList.eraseNoOrder(i);
                    m_lastHitActorsFrameList.eraseNoOrder(i);
                    i--;
                    continue;
                }
                else
                {
                    frame = CURRENTFRAME;
                    addSender = bfalse;
                }
            }
            else if ( actor == _sender )
            {
                addSender = bfalse;
                process = bfalse;
            }
        }

        if ( addSender )
        {
            m_lastHitActorsList.push_back( _sender );
            m_lastHitActorsFrameList.push_back( CURRENTFRAME );
        }
        return process;
    }*/

	//---------------------------------------------------------------------------------------------------------------
    void Ray_AIShooterFlyingBombBehavior::update( f32 _delta )
    {
        Super::update(_delta);

        // moving 
        //--------
        if ( m_currentAction == m_moveAction )
        {
            f32 curScale = Interpolate( getTemplate()->getMoveActionInitScale(), m_destinationScale.m_x, m_moveAction->getCurrentCursor() );
            m_actor->setScale( Vec2d( curScale, curScale ) );
        }
        // Alarm
        //--------
        if ( m_currentAction == m_alarmAction )
        {
            if ( getTemplate()->getAlarmDuration() > 0.0f )
            {
                m_alarmTimer += _delta;

                f32 speedFactor = Min( m_alarmTimer / getTemplate()->getAlarmDuration(), 1.0f );
                if ( m_alarmTimer < getTemplate()->getAlarmDuration() )
                {
                    speedFactor = 1.f + ( speedFactor * 10.f );
                    m_alarmBlinkTimer += speedFactor*_delta;
                }
                else
                {
                    m_alarmBlinkTimer = 0.0f;
                    startExplodeAction();
                }
            }           
        }

        // detect something ? explode
        if (    ( m_currentAction == m_alarmAction ) ||
                ( m_currentAction == m_openingAction ) )
        {
            ActorRefList& actorsInside = m_detectorComponent->getActorsInside();

            if ( ( m_detectorComponent != NULL ) && ( actorsInside.size() > 0 ) )
            {
                Actor* actor;
                for ( u32 i = 0; i < actorsInside.size(); ++i )
                {
                    actor = AIUtils::getActor( actorsInside[i] );

                    if ( ( actor != NULL ) && AIUtils::isEnemy( m_aiComponent->getFaction(), AIUtils::getFaction(actor )) )
                    {
                        startExplodeAction();
                        break;                        
                    }
                }
            }
        }

        // hit timers
        //
        m_hitTimer = Max( 0.0f, m_hitTimer -_delta );
        m_multiHitTimer = Max( 0.0f, m_multiHitTimer -_delta );
    }

    //---------------------------------------------------------------------------------------------------------------
    void Ray_AIShooterFlyingBombBehavior::startMoveAction()
    {
        if ( m_moveAction == NULL )
            return;
        
        Vec3d destPos = m_actor->getPos();
        Actor * destActor = m_destinationActor.getActor();
        if ( destActor != NULL )
        {
            destPos = destActor->getPos();
        }

        Vec3d destVec = destPos - m_actor->getPos();
        Vec3d verticalVec( 0.0f, 0.5f, 0.0f );
        verticalVec.m_x = Seeder::getSharedSeeder().GetFloat(getTemplate()->getMoveActionVecOffsetMin().m_x, getTemplate()->getMoveActionVecOffsetMax().m_x );
        verticalVec.m_y = Seeder::getSharedSeeder().GetFloat(getTemplate()->getMoveActionVecOffsetMin().m_y, getTemplate()->getMoveActionVecOffsetMax().m_y );

        m_moveAction->setupBezier(  m_actor->getPos(), 
                                    m_actor->getPos() + ( destVec * 0.33f ) + verticalVec,
                                    m_actor->getPos() + ( destVec * 0.66f ) + verticalVec,
                                    destPos );

        // change scale
        //
        m_destinationScale = m_actor->getScale();
        f32 scale = getTemplate()->getMoveActionInitScale();
        m_actor->setScale( Vec2d( scale, scale ) );

        setAction( m_moveAction );
    }

    //---------------------------------------------------------------------------------------------------------------
    void Ray_AIShooterFlyingBombBehavior::startOpeningAction()
    {
        if ( m_openingAction == NULL )
            return;

       setAction( m_openingAction );
    }

    //---------------------------------------------------------------------------------------------------------------
    void Ray_AIShooterFlyingBombBehavior::startAlarmAction()
    {
        if ( m_alarmAction == NULL )
            return;

        m_alarmTimer = 0.f;
        m_alarmBlinkTimer = 0.f;
        
        setAction( m_alarmAction );
    }
    //---------------------------------------------------------------------------------------------------------------
    void Ray_AIShooterFlyingBombBehavior::startExplodeAction()
    {
        if ( m_explodeAction == NULL )
            return;
        
        setAction( m_explodeAction );
    }
    
	//---------------------------------------------------------------------------------------------------------------
    void Ray_AIShooterFlyingBombBehavior::onActionFinished()
    {
        Super::onActionFinished();

        if ( m_currentAction == m_moveAction )
        {
            m_actor->setScale( m_destinationScale );

            if ( getTemplate()->getExplodeOnMoveActionsEnd() )
            {
                startExplodeAction();
            }
            else
            {
                startOpeningAction();
            }
        }
        else if ( m_currentAction == m_openingAction )
        {
            startAlarmAction();
        }
        else if ( m_currentAction == m_explodeAction )
        {            
        }
	}

    //---------------------------------------------------------------------------------------------------------------
    void Ray_AIShooterFlyingBombBehavior::updateAnimInputs()
    {
        Super::updateAnimInputs();

        if ( m_alarmBlinkAnimDuration == 0.f )
        {
            SubAnim *subAnim = m_animatedComponent->getSubAnim( getTemplate()->getAlarmAnimName() ); 
            if ( subAnim != NULL )
            {
                m_alarmBlinkAnimDuration = subAnim->getDuration();            
            }
        }
        if ( m_warnAnimDuration == 0.f )
        {
            SubAnim *subAnim = m_animatedComponent->getSubAnim( getTemplate()->getWarnAnimName() ); 
            if ( subAnim != NULL )
            {
                m_warnAnimDuration = subAnim->getDuration();            
            }
        }
  
        f32 alarmCursor = 0.f;

        if ( m_currentAction == m_alarmAction )
        {
            if ( m_alarmBlinkAnimDuration > 0.f )
            {
                alarmCursor = f32_Modulo( m_alarmBlinkTimer, m_alarmBlinkAnimDuration ) / m_alarmBlinkAnimDuration;
            }
        }

        // hit feedback
        //
        f32 warnCursor = 0.f;

        if ( m_warnAnimDuration > 0.f )
        {
            warnCursor = 1.0f - ( f32_Modulo( m_hitTimer, m_warnAnimDuration ) / m_warnAnimDuration );
        }

        m_animatedComponent->setInput( s_AlarmCursor, alarmCursor );
        m_animatedComponent->setInput( s_WarnCursor, warnCursor );

    }

	//---------------------------------------------------------------------------------------------------------------
	IMPLEMENT_OBJECT_RTTI(Ray_AIShooterFlyingBombBehavior_Template)
	BEGIN_SERIALIZATION_CHILD(Ray_AIShooterFlyingBombBehavior_Template)
		SERIALIZE_OBJECT_WITH_FACTORY   ("moveAction",	                m_moveAction,               ACTORSMANAGER->getAIActionsFactory() );
        SERIALIZE_OBJECT_WITH_FACTORY   ("openingAction",	            m_openingAction,            ACTORSMANAGER->getAIActionsFactory() );
        SERIALIZE_OBJECT_WITH_FACTORY   ("alarmAction",	                m_alarmAction,              ACTORSMANAGER->getAIActionsFactory() );
        SERIALIZE_OBJECT_WITH_FACTORY   ("explodeAction",	            m_explodeAction,            ACTORSMANAGER->getAIActionsFactory() );
        SERIALIZE_MEMBER                ("alarmAnimName",               m_alarmAnimName );
        SERIALIZE_MEMBER                ("warnAnimName",                m_warnAnimName );
        SERIALIZE_MEMBER                ("alarmDuration",               m_alarmDuration );
        SERIALIZE_MEMBER                ("isTriggered",                 m_isTriggered );
        SERIALIZE_MEMBER                ("moveActionInitScale",         m_moveActionInitScale );
        SERIALIZE_MEMBER                ("animationBankState",          m_animationBankState );
        SERIALIZE_MEMBER                ("explodeOnMoveActionsEnd",     m_explodeOnMoveActionsEnd );
        SERIALIZE_MEMBER                ("moveActionVecOffsetMin",      m_moveActionVecOffsetMin );
        SERIALIZE_MEMBER                ("moveActionVecOffsetMax",      m_moveActionVecOffsetMax );
	END_SERIALIZATION()

	Ray_AIShooterFlyingBombBehavior_Template::Ray_AIShooterFlyingBombBehavior_Template()
	: Super()
    , m_moveAction( NULL )
    , m_openingAction( NULL )
    , m_alarmAction( NULL )
    , m_explodeAction( NULL )
    , m_alarmDuration( 0.0f )
    , m_isTriggered( bfalse )
    , m_moveActionInitScale( 1.f )
    , m_animationBankState( 0 )
    , m_explodeOnMoveActionsEnd( bfalse )
    , m_moveActionVecOffsetMin( 0.0f, 0.5f )
    , m_moveActionVecOffsetMax( 0.0f, 0.5f )
	{
	}

	Ray_AIShooterFlyingBombBehavior_Template::~Ray_AIShooterFlyingBombBehavior_Template()
	{
        SF_DEL( m_moveAction );
        SF_DEL( m_openingAction );
        SF_DEL( m_alarmAction );
        SF_DEL( m_explodeAction );
	}

}
