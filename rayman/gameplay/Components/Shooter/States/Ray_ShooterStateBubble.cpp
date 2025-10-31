#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_SHOOTERCONTROLLERCOMPONENT_H_
#include "rayman/gameplay/Components/Shooter/Ray_ShooterControllerComponent.h"
#endif //_ITF_RAY_SHOOTERCONTROLLERCOMPONENT_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_AIMANAGER_H_
#include "gameplay/managers/AIManager.h"
#endif //_ITF_AIMANAGER_H_

#ifndef _ITF_PHYSPHANTOM
#include "engine/physics/PhysPhantom.h"
#endif //_ITF_PHYSPHANTOM

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_


namespace ITF
{
    IMPLEMENT_OBJECT_RTTI_SUBCLASS(Ray_ShooterControllerComponent,StateBubble)

	void Ray_ShooterControllerComponent::StateBubble::onEnter()
	{
		Super::onEnter();

        if ( getStateMachine()->isChangingImplement() )
        {
            m_moveVector = Vec2d::Zero;
            
            getParent()->m_characterPhys->setProcessContacts(bfalse);
            m_previousWindMultiplier = getParent()->m_characterPhys->getWindMultiplier();
            getParent()->m_characterPhys->setWindMultiplier( 0.0f );

            // Set  the radius of the phantom
            m_shape.setRadius( getParent()->getTemplate()->getBubbleStatePhantomRadius() );
            getParent()->setCurrentPhantomShape( &m_shape );
        }

        if ( IS_CURSTATE( STATE_BUBBLE ) )
        {
            m_previousFaction = getParent()->getFaction();
            getParent()->setFaction( RAY_FACTION_GHOST );
        }    
      
        if ( IS_CURSTATE( STATE_BUBBLE_EXIT ) )
        {
            const f32 leftDist = 6.0f;
            const f32 upDist = 5.0f;

            m_bezierPos0 = getParent()->GetActor()->get2DPos();
            Vec2d pos3Dir = ( Vec2d::Up * upDist );
            m_bezierPos3 = m_bezierPos0 + pos3Dir;

            Vec2d moveDir;
            // check pos exit screen ?
            ICameraController* cameraController = CAMERACONTROLLERMANAGER->getMainCameraController();
            if ( cameraController != NULL )
            {
                Vec2d camPos = cameraController->getCameraPosition().truncateTo2D();
                f32 camHeight = tanf( cameraController->getCameraFocale() * 0.5f ) * cameraController->getDepth();
                f32 camWidth = camHeight * CAMERACONTROLLERMANAGER->m_screenRatio;
                Vec2d vec = ( m_bezierPos3 - camPos );
                f32 distToCam = ( vec ).dot( Vec2d::Up );

                if ( distToCam > camHeight )
                {
                    m_bezierPos3 = m_bezierPos0 - pos3Dir;
                }

                Vec2d middleStartPos = m_bezierPos0 + ( m_bezierPos3 - m_bezierPos0 ) * 0.5f;
                Vec2d middle = middleStartPos + ( Vec2d::Right * leftDist );
                m_bezierPos1 = m_bezierPos0 + ( middle - m_bezierPos0 ) * 0.5f;
                                               
                vec = ( m_bezierPos1 - camPos );
                distToCam = vec.dot( Vec2d::Right );

                if ( distToCam > camWidth )
                {
                    middle -= ( Vec2d::Right * ( camWidth - f32_Abs( ( m_bezierPos1 - camPos ).dot( Vec2d::Right ) ) ) );
                }
                
                m_bezierPos1 = m_bezierPos0 + ( middle - m_bezierPos0 ) * 0.5f;
                m_bezierPos2 = middle + ( m_bezierPos3 - middle ) * 0.5f;
            }

            m_bezierTimer = 0.0f;

            // play exit FX 
            if ( getParent()->m_fxController != NULL )
            {
                u32 fxHandle = getParent()->m_fxController->playFX( ITF_GET_STRINGID_CRC(BubbleExitFX,2763088395) );
                if ( fxHandle != U32_INVALID )
                {
                    getParent()->m_fxController->setFXPos( fxHandle, getParent()->m_actor->getPos() );
                }
            }
        }
        // Init drop lum
        //
        else if ( IS_CURSTATE( STATE_BUBBLE ) )
        {
            //getParent()->startDeathDropLum();
            m_angle = 0.0f;
        }
	}

	//------------------------------------------------------------------------------------------------------------------
	void Ray_ShooterControllerComponent::StateBubble::onExit()
	{
		Super::onExit();

        if ( IS_CURSTATE( STATE_BUBBLE ) )
        {
            getParent()->setFaction( m_previousFaction );
        }
      
        if ( getStateMachine()->isChangingImplement() )
        {
            getParent()->m_characterPhys->setProcessContacts(btrue);
            getParent()->m_characterPhys->setWindMultiplier( m_previousWindMultiplier );

            if ( getStateMachine()->getNextStateID() != STATE_DEATH_BUBBLE )
            {
                getParent()->GetActor()->setAngle( 0.0f );
            }

            // restore init shape
            getParent()->setCurrentPhantomShape( getParent()->getTemplate()->getPhantomShape() );
        }
	}

    //----------------------------------------------------------------------------------------------------------------------
    u32 Ray_ShooterControllerComponent::StateBubble::onValidateStateChange( u32 _prevStateID, u32 _wantedStateID )
    {
        if ( getStateMachine()->getCurImplement() != this )
            return _wantedStateID;

        return _wantedStateID;
    }

	//------------------------------------------------------------------------------------------------------------------
	void Ray_ShooterControllerComponent::StateBubble::changeStateCheck( f32 _deltaTime )
	{
		Super::changeStateCheck( _deltaTime );

        if ( IS_CURSTATE( STATE_BUBBLE ) )
        {
            if ( RAY_GAMEMANAGER->getShooterGameModeController()->isAllPlayerDead() || 
                ( ( getParent()->getPlayerIndex() != GAMEMANAGER->getMainIndexPlayer() ) && ( getParent()->getLeaveListener() ) && !getStateMachine()->isCurStateHasJustChanged() )
                )
            {
                CHANGE_STATE( STATE_DEATH_BUBBLE );
				////
            }
        }
	}

	//------------------------------------------------------------------------------------------------------------------
	void Ray_ShooterControllerComponent::StateBubble::update( f32 _dt )
	{
		Super::update( _dt );

        if ( IS_CURSTATE( STATE_BUBBLE_EXIT ) )
        {
            updateBubbleExit(_dt);
        }
        else
        {
            //getParent()->updateDeathDropLum( _dt );
            updateBubble(_dt);
        }

        getParent()->m_stateWantedMove = btrue;
        getParent()->m_stateWantedMoveVector = m_moveVector;
        getParent()->m_stateWantedLocalSpeedMax = getParent()->getTemplate()->getBubbleStateLocalSpeedMax();
	}

    //------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterControllerComponent::StateBubble::updateBubble( f32 _dt )
    {        
        Vec2d moveInput = getParent()->getMoveListener();
        f32 inputForce = moveInput.norm();
        f32 targetSpeed = Interpolate( 0.0f, getParent()->getTemplate()->getBubbleStateLocalSpeedMax(), inputForce );

        Vec2d moveInputNormalized = moveInput;
        moveInputNormalized.normalize();
        m_moveVector = Interpolate( m_moveVector, targetSpeed * moveInputNormalized, Min( _dt * getParent()->getTemplate()->getBubbleStateBlendFactor(), 1.0f ) );

        Vec2d borderNormal = getParent()->m_borderScreenNomal;

        if ( !borderNormal.IsEqual( Vec2d::Zero ) && ( borderNormal.dot(m_moveVector) < 0.f ) )
        {
            //m_moveVector = -m_moveVector;
            Vec2d moveDir = m_moveVector;
            f32 speed = moveDir.norm();
            moveDir = ( speed != 0.0f ) ? ( moveDir / speed ) : Vec2d::Zero;

            f32 angle = moveDir.getShortestAngle( borderNormal );
            m_moveVector = borderNormal.Rotate( ( MTH_PIBY2 + ( MTH_PIBY2 - angle ) ) * getSign( borderNormal.getPerpendicular().dot( moveDir ) ) );
            m_moveVector *= speed;
        }

        // update angle
        //
        f32 wantedAngle = Interpolate( MTH_PIBY4, -MTH_PIBY4, ( moveInputNormalized.dot( Vec2d::Right ) * 0.5f ) + 0.5f );
        m_angle = Interpolate( m_angle, wantedAngle, Min( _dt * getParent()->getTemplate()->getBubbleStateBlendFactor(), 1.0f ) );
        getParent()->GetActor()->setAngle( canonizeAngle( m_angle ) );
    }

    //------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterControllerComponent::StateBubble::updateBubbleExit( f32 _dt )
    {
        m_bezierTimer += _dt;

        f32 cursor = Min( m_bezierTimer / 0.5f, 1.0f );
        //f32 cursor = Min( m_bezierTimer / 1.0f, 1.0f );
        f32 factor = ( cursor * cursor );
        factor = Clamp( factor, 0.0f, 1.0f );

        m_bezierPos3 = getParent()->getUndeadActorPos();
        // No valid position
        //
        if ( m_bezierPos3.IsEqual( Vec2d::Zero ) )
        {
            CHANGE_STATE( STATE_DEATH_BUBBLE );
            return;
        }

        Vec2d pos = getBezierPosition(m_bezierPos0, m_bezierPos1, m_bezierPos2, m_bezierPos3, factor );
        getParent()->GetActor()->set2DPos( pos );

#ifndef ITF_FINAL
        //DebugDraw::bezier( m_bezierPos0, m_bezierPos1, m_bezierPos2, m_bezierPos3, 0.f );
#endif // ITF_FINAL

        if ( factor == 1.0f )
        {
            getParent()->revive();
            getParent()->setImmuneTime( getParent()->getTemplate()->getReviveImmunityDuration(), btrue );
            CHANGE_STATE( STATE_NORMAL );
        }
    }

    //------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterControllerComponent::StateBubble::changeStateCheckAnimFinished()
    {
       
    }

    //------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterControllerComponent::StateBubble::onEvent( Event * _evt )
    {
        if ( PunchStim* hit = _evt->DynamicCast<PunchStim>(ITF_GET_STRINGID_CRC(PunchStim,200533519)) )
        {
            if ( IS_CURSTATE( STATE_BUBBLE ) )
            {
                if ( ( hit->getFaction() == RAY_FACTION_FRIENDLY ) || ( hit->getFaction() == RAY_FACTION_PLAYER ) )
                {
                    CHANGE_STATE( STATE_BUBBLE_EXIT );
                }
            }
        }
    }
}
