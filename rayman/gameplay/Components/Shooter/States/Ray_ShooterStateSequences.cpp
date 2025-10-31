#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_SHOOTERCONTROLLERCOMPONENT_H_
#include "rayman/gameplay/Components/Shooter/Ray_ShooterControllerComponent.h"
#endif //_ITF_RAY_SHOOTERCONTROLLERCOMPONENT_H_

#ifndef _ITF_RAY_SHOOTERCHECKPOINTCOMPONENT_H_
#include "rayman/gameplay/Components/Shooter/Misc/Ray_ShooterCheckPointComponent.h"
#endif //_ITF_RAY_SHOOTERCHECKPOINTCOMPONENT_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI_SUBCLASS(Ray_ShooterControllerComponent,StateSequences)

    Ray_ShooterControllerComponent::StateSequences::StateSequences()
    : Super()
    , m_travelPos0( Vec2d::Zero )
    , m_travelPos1( Vec2d::Zero )
    , m_travelPos2( Vec2d::Zero )
    , m_travelPos3( Vec2d::Zero )
    , m_travelTimer( 0.0f )
    , m_travelDuration( 0.0f )
    , m_travelInitialized( bfalse )
    {
    }

    //------------------------------------------------------------------------------------------------------------------
	void Ray_ShooterControllerComponent::StateSequences::onEnter()
	{
		Super::onEnter();

        if ( getStateMachine()->isChangingImplement() )
        {
            getParent()->m_characterPhys->setProcessContacts(bfalse);
        }

        //****************
        // TRAVEL START
        //****************
        if ( IS_CURSTATE( STATE_SEQUENCE_LAND_TRAVEL ) )
        {
            m_travelInitialized = bfalse;
            RAY_GAMEMANAGER->getShooterGameModeController()->setPlayerLanding( getParent()->getPlayerIndex(), btrue );
        }
         //****************
        // TRAVEL START
        //****************
        else if ( IS_CURSTATE( STATE_SEQUENCE_LAND_EXIT ) )
        {
            RAY_GAMEMANAGER->changePlayerGameMode( getParent()->getPlayerIndex(), RAY_GAMEMODE_PLATFORMER );
        }
	}

	//------------------------------------------------------------------------------------------------------------------
	void Ray_ShooterControllerComponent::StateSequences::onExit()
	{
		Super::onExit();

        if ( getStateMachine()->isChangingImplement() )
        {
            getParent()->m_characterPhys->setProcessContacts(btrue);
            getParent()->GetActor()->setAngle( 0.0f );
            RAY_GAMEMANAGER->getShooterGameModeController()->setPlayerLanding( getParent()->getPlayerIndex(), bfalse );
        }
	}

    //------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterControllerComponent::StateSequences::computeTravel( const Vec2d & _destPos )
    {
        m_travelInitialized = btrue;
        m_travelPos0 = getParent()->GetActor()->get2DPos();     // start pos
        m_travelPos3 = _destPos;

        // get the destination position
        //
      /*  Actor * finalCheckPoint = AIUtils::getActor( RAY_GAMEMANAGER->getShooterGameModeController()->getActiveCheckPoint() );
        ITF_ASSERT( finalCheckPoint != NULL );
        if ( finalCheckPoint != NULL )
        {
            Ray_ShooterCheckPointComponent * checkpointComp = finalCheckPoint->GetComponent<Ray_ShooterCheckPointComponent>();
            ITF_ASSERT( checkpointComp != NULL );
            if ( checkpointComp != NULL )
            {
                m_travelPos3 = checkpointComp->getPlayerSpawnPos( getParent()->getPlayerIndex() );
            }
        }*/

        // Compute other points
        //
        Vec2d destVecX = m_travelPos3 - m_travelPos0;
        Vec2d destVecY = destVecX;
        destVecX.m_y = 0.0f;
        destVecY.m_x = 0.0f;
        m_travelPos1 = m_travelPos0 + ( destVecX * 0.5f ) + ( destVecY * 0.5f );
        m_travelPos2 = m_travelPos0 + ( destVecX * 0.5f ) + destVecY;

        m_travelTimer = 0.0f;
        f32 distFactor = f32_Min( ( m_travelPos3 - m_travelPos0 ).sqrnorm(), 25.0f ) / 25.0f;
        m_travelDuration = ( 1.0f * distFactor ) + Seeder::getSharedSeeder().GetFloat( 0.0f, 0.5f );

    }

	//------------------------------------------------------------------------------------------------------------------
	void Ray_ShooterControllerComponent::StateSequences::changeStateCheck( f32 _deltaTime )
	{
		Super::changeStateCheck( _deltaTime );

        if ( IS_CURSTATE( STATE_SEQUENCE_LAND_EXIT ) )
        {
            getStateMachine()->setCurStateFinished();
        }
	}

	//------------------------------------------------------------------------------------------------------------------
	void Ray_ShooterControllerComponent::StateSequences::update( f32 _dt )
	{
		Super::update( _dt );

        ITF_ASSERT( m_travelInitialized );      // travel must have been init before        

        getParent()->m_stateWantedMove = btrue;
        getParent()->m_stateWantedMoveVector = Vec2d::Zero;
        getParent()->m_stateWantedLocalSpeedMax = getParent()->getTemplate()->getMaxLocalSpeed();
        
        if ( IS_CURSTATE( STATE_SEQUENCE_LAND_TRAVEL ) && m_travelInitialized )
        {
            m_travelTimer += _dt;

            f32 cursor = Min( m_travelTimer / m_travelDuration, 1.0f );
            f32 factor = cursor;// ( cursor * cursor );
            factor = 1.0f - ( ( 1.0f - cursor ) * ( 1.0f - cursor ) );

            Vec2d moveDir = getBezierTangent( m_travelPos0, m_travelPos1, m_travelPos2, m_travelPos3, factor );
            moveDir = moveDir.normalize() * ( 1.0f - cursor );
            getParent()->m_stateWantedMoveInput = moveDir;
            getParent()->m_listenerMove = moveDir;

            // call the bubble state update to manage angle
            if ( getParent()->m_isDead )
            {
                StateBubble * bubbleState = static_cast<StateBubble*>( getStateMachine()->getState( STATE_BUBBLE )->getImplement() );
                if ( bubbleState != NULL )
                {
                    bubbleState->updateBubble( _dt );                
                }
            }

            Vec2d pos = getBezierPosition(m_travelPos0, m_travelPos1, m_travelPos2, m_travelPos3, factor );
            getParent()->GetActor()->set2DPos( pos );
            

#ifndef ITF_FINAL
            //DebugDraw::bezier( m_travelPos0, m_travelPos1, m_travelPos2, m_travelPos3, 0.f );
#endif // ITF_FINAL

            if ( factor == 1.0f )
            {
                getStateMachine()->setCurStateFinished();
            }
        }
	}

    //------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterControllerComponent::StateSequences::chooseAnimation()
    {
        if ( getParent()->m_isDead )
        {
            getStateMachine()->getAnimCurState()->setWantedAnimation( getStateMachine()->getAnimState( STATE_BUBBLE )->getDefaultAnimation() );
        }
    }

    //------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterControllerComponent::StateSequences::onEvent( Event * _event )
    {
        Super::onEvent( _event );

        if (Ray_EventShooterLandingRequested* landingRequested = _event->DynamicCast<Ray_EventShooterLandingRequested>(ITF_GET_STRINGID_CRC(Ray_EventShooterLandingRequested,492783699)))
        {
            computeTravel( landingRequested->getLandingPos() );
        }
    }

}