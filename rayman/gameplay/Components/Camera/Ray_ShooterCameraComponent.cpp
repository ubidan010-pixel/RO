#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_SHOOTERCAMERACOMPONENT_H_
#include "rayman/gameplay/Components/Camera/Ray_ShooterCameraComponent.h"
#endif //_ITF_RAY_SHOOTERCAMERACOMPONENT_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_RAY_SHOOTERCAMERAMODIFIERCOMPONENT_H_
#include "rayman/gameplay/Components/Camera/Ray_ShooterCameraModifierComponent.h"
#endif //_ITF_RAY_SHOOTERCAMERAMODIFIERCOMPONENT_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

//#define DBG_SHOOTERCAMERA   1


namespace ITF
{
    //-----------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(Ray_ShooterCameraComponent)
    BEGIN_SERIALIZATION_CHILD(Ray_ShooterCameraComponent)
        SERIALIZE_FUNCTION(presaveCheckpoint, ESerialize_Checkpoint_Save);

        BEGIN_CONDITION_BLOCK(ESerializeGroup_Checkpoint)
            SERIALIZE_MEMBER("Pos",m_posAtCheckpoint);
        END_CONDITION_BLOCK()

        SERIALIZE_MEMBER("useInitModifier",     m_useInitModifier );
        if ( m_useInitModifier )
        {
            SERIALIZE_OBJECT( "initModifier",   m_initModifier );
        }        
    END_SERIALIZATION()

    //-----------------------------------------------------
    BEGIN_SERIALIZATION( ShooterCameraModifier )
        SERIALIZE_MEMBER("moveSpeed",   m_moveSpeed );
        SERIALIZE_MEMBER("zOffset",     m_zOffset );
    END_SERIALIZATION()

    //-----------------------------------------------------
    BEGIN_SERIALIZATION( ShooterCameraModifier_Transition )
        
        SERIALIZE_ENUM_BEGIN( "type", m_type );
            SERIALIZE_ENUM_VAR( Linear );
            SERIALIZE_ENUM_VAR( AccelDecel );
            SERIALIZE_ENUM_VAR( X2 );
            SERIALIZE_ENUM_VAR( InvX2 );
            SERIALIZE_ENUM_VAR( X3 );
            SERIALIZE_ENUM_VAR( InvX3 );
        SERIALIZE_ENUM_END();
        
        SERIALIZE_MEMBER("duration",                m_duration );
        SERIALIZE_MEMBER("additionalSpeed",         m_additionalSpeed );
        SERIALIZE_MEMBER("notifyPlayerDelay",       m_notifyPlayerDelay );
    END_SERIALIZATION()

    Ray_ShooterCameraComponent* Ray_ShooterCameraComponent::m_instance = NULL;


    //-----------------------------------------------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------------------------------------------
    void ShooterCameraModifier_Transition::computeBlendCursorFromType(      f32 & _blendCursor,
                                                                            const TransitionType & _type, 
                                                                            const f32 & _timerCursor, 
                                                                            const f32 & _dt )
    {
        f32 cursor = _timerCursor;
        if ( cursor >= 0.9999f )
        {
            cursor = 1.f;
        }
        // Update transition types
        //--------------------------
        switch ( _type )
        {
            case Linear :
            {
                _blendCursor = cursor;
                break;
            }
            case AccelDecel :
            {
                f32 factor = cursor * cursor * 2.f;
                if ( cursor > 0.5f )
                {
                    factor = 1.0f - ( ( 1.f - cursor ) * ( 1.f - cursor ) * 2.f );
                }
                _blendCursor = factor;
                break;
            }
            case X2 :
            {
                _blendCursor = cursor * cursor;
                break;
            }
            case InvX2 :
            {
                _blendCursor = 1.0f - ( ( 1.f - cursor ) * ( 1.f - cursor ) );
                break;
            }
            case X3 :
            {
                _blendCursor = cursor * cursor * cursor;
                break;
            }
            case InvX3 :
            {
                _blendCursor = 1.0f - ( ( 1.f - cursor ) * ( 1.f - cursor ) * ( 1.f - cursor ) );
                break;
            }
            /*case InvExpo :
            {
                _blendCursor = Interpolate( _blendCursor, 1.f, _dt * _factor );
                if ( _blendCursor >= 0.9999f )
                {
                    _blendCursor = 1.f;
                }
                break;
            }*/
        }
    }

    //-----------------------------------------------------------------------------------------------------------------------
    void ShooterCameraModifier_Transition::start( bbool _fromCurrentTime /*= bfalse*/  )
    {
        if ( _fromCurrentTime )
        {
            m_duration = m_timer;
        }
        m_timer = m_duration;
        m_blendCursor = 0.0f;
        m_curAdditionalSpeed = Vec2d::Zero;
        m_isStarted = btrue;
    }
    //-----------------------------------------------------------------------------------------------------------------------
    void ShooterCameraModifier_Transition::update( f32 _dt )
    {
        f32 curveCursor = 1.f;
        m_timer -= _dt;
        if ( m_duration > 0.f )
        {
            curveCursor = 1.f - ( m_timer / m_duration );
        }
        if ( m_timer <= 0.f )
        {
            curveCursor = 1.f;
            m_timer = 0.f;
        }
        computeBlendCursorFromType( m_blendCursor, m_type, curveCursor, _dt );

        // compute additionnal speed
        switch ( m_type )
        {
            case Linear :
            case InvX2 :
            case InvX3 :
            {
                m_curAdditionalSpeed = m_additionalSpeed * ( 1.f - m_blendCursor );
                break;
            }
            case AccelDecel :
            {
                f32 speedFactor = m_blendCursor;
                if ( m_blendCursor > 0.5f ) 
                {
                    speedFactor = 1.f - m_blendCursor;
                }
                speedFactor = speedFactor / 0.5f;
                m_curAdditionalSpeed = m_additionalSpeed * speedFactor;
                break;
            }
            case X2 :
            case X3 :
            {
                m_curAdditionalSpeed = m_additionalSpeed * m_blendCursor;
                break;
            }
            default :
            {
                // additional speed not managed !
                ITF_ASSERT( 0 );
                m_curAdditionalSpeed = Vec2d::Zero;
            }
        }
    }

    //-----------------------------------------------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------------------------------------------
    Ray_ShooterCameraComponent::Ray_ShooterCameraComponent()
        : Super()
        , m_activated( bfalse )
        , m_curZoffset( k_ShooterCameraDefaultZOffset )
        , m_curDepth( 0.f )
        , m_useInitModifier( bfalse )
        , m_modifierBlendCursor( 1.f )
        , m_transitionActive( bfalse )
        , m_transitionPlayerNotificationTimer( 0.f )
        , m_curMoveSpeed(Vec2d::Zero)
        , m_transitionAdditionalSpeed(Vec2d::Zero)
    {
        m_instance = this;

        EVENTMANAGER_REGISTER_EVENT_LISTENER(ITF_GET_STRINGID_CRC(EventRevertToLastCheckpoint,3654122082),this);
        EVENTMANAGER_REGISTER_EVENT_LISTENER(ITF_GET_STRINGID_CRC(EventPlayerModeChanged,1336647275),this);
    }

    //-----------------------------------------------------------------------------------------------------------------------
    Ray_ShooterCameraComponent::~Ray_ShooterCameraComponent()
    {
        m_instance = NULL;
        EVENTMANAGER->unregisterEvent(ITF_GET_STRINGID_CRC(EventRevertToLastCheckpoint,3654122082), this);
        EVENTMANAGER->unregisterEvent(ITF_GET_STRINGID_CRC(EventPlayerModeChanged,1336647275), this);
    }

    //-----------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterCameraComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);
        m_activated = bfalse;

        m_curDepth = m_actor->getDepth();
        reset();
    }

    //-----------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterCameraComponent::reset()
    {
        if ( !m_useInitModifier )
        {
            m_toModifier.m_moveSpeed = getTemplate()->getDefaultSpeed();
            m_toModifier.m_zOffset = getTemplate()->getDefaultZOffset();
        }
        else
        {
            m_toModifier = m_initModifier;
        }
        m_toModifier.m_depth = m_curDepth;

        m_modifierBlendCursor = 1.f;
        m_transitionActive = bfalse;
        m_transitionAdditionalSpeed = Vec2d::Zero;
    }

    //-----------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterCameraComponent::presaveCheckpoint()
    {
        m_posAtCheckpoint = m_actor->getPos();
    }

    //-----------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterCameraComponent::onCheckpointLoaded()
    {
        // Use only shooter check points
        //
        Actor* checkpoint = AIUtils::getActor( RAY_GAMEMANAGER->getShooterGameModeController()->getActiveCheckPoint() );
        
        if ( checkpoint != NULL )
        {
            shooterTeleportToCheckPoint( checkpoint, RAY_GAMEMANAGER->getShooterGameModeController()->activeCheckPointIsCameraActivator() );
        }
    }
    //-----------------------------------------------------------------------------------------------------------------------
    // teleport keeping camera move direction
	//
	void Ray_ShooterCameraComponent::dbgTeleport( const Vec3d& _pos )
    {
        Vec2d camMoveDir = getSpeed().normalize();
        Vec2d newPos = _pos.truncateTo2D();
        f32 camHeight = tanf( getCameraFocale() * 0.5f ) * m_curZoffset;
        f32 camWidth = camHeight * CAMERACONTROLLERMANAGER->m_screenRatio;

     
        // keep the camera move direction
        newPos = m_actor->get2DPos();
        if ( camMoveDir.IsEqual( Vec2d::Zero ) )
        {
            camMoveDir = Vec2d::Right;
        }
        Vec2d oldToNewPos = _pos.truncateTo2D() - m_actor->get2DPos();
        newPos += ( camMoveDir * ( oldToNewPos.dot( camMoveDir ) ) );

        if ( ( newPos - _pos.truncateTo2D() ).norm() > camWidth )
        {
            newPos = _pos.truncateTo2D();
        }        
        
        Vec3d finalPos = newPos.to3d((_pos.m_z) );
        m_actor->setPos( finalPos );
        m_curDepth = _pos.m_z;
    }
    //-----------------------------------------------------------------------------------------------------------------------
    // teleport keeping camera move direction
    //
  /*  void Ray_ShooterCameraComponent::shooterTeleport( const Vec3d& _pos )
    {
        ICameraController* cameraController  = CAMERACONTROLLERMANAGER->getMainCameraController();
        if ( cameraController->getActorRef() == m_actor->getRef() )
        {
            dbgTeleport(_pos);
            m_curDepth = _pos.m_z;
            reset();
        }
    }*/

    //-----------------------------------------------------------------------------------------------------------------------
    // teleport the camera on the given check point checking checkpoint camera links
    //
    void Ray_ShooterCameraComponent::shooterTeleportToCheckPoint( Actor * _checkPoint, bbool _isCameraActivator )
    {
        ITF_ASSERT( _checkPoint != NULL );
        if ( _checkPoint == NULL )
            return;

        // reset the camera first 
        //-------------------------
        reset();

        const ShooterCameraModifier * usedModifier = NULL;

        // check first if the check point has linked camera modifier
        //
        LinkComponent * linkComponent = _checkPoint->GetComponent<LinkComponent>();
        if ( linkComponent != NULL )
        {
            const LinkComponent::ChildrenList & children = linkComponent->getChildren();
            u32 i;
            for ( i = 0; i < children.size(); ++i )
            {
                if ( children[i].hasTag( ITF_GET_STRINGID_CRC( Cam, 2982239217) ) )
                {
                    break;
                }
            }
            // has found a modifier to use 
            if (  i < children.size() )
            {
                BaseObject * obj = linkComponent->getChildObject( children[i].getPath() );
                if ( Actor * modifierActor = obj->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115)) )
                {
                    Ray_ShooterCameraModifierComponent * modifierComp = modifierActor->GetComponent<Ray_ShooterCameraModifierComponent>();
                    ITF_WARNING( _checkPoint, ( modifierComp != NULL ), "Camera CheckPoint has link with camera init tag linked to a non cam modifier obj !" );

                    if ( modifierComp != NULL )
                    {
                        usedModifier = &(modifierComp->getModifier());
                    }
                }
            }
        }

        // camera activator; the cam pos is offseted
        //---------------------------------------------
        if ( _isCameraActivator )
        {
            f32 curZOffset = usedModifier != NULL ? usedModifier->m_zOffset : m_toModifier.m_zOffset;

            //Vec2d camPos = m_actor->get2DPos() + ( Vec2d::Right * ( checkpoint->get2DPos() - m_actor->get2DPos() ).dot( Vec2d::Right ) );
            Vec3d camPos = _checkPoint->getPos();

            f32 camHeight = tanf( getCameraFocale() * 0.5f ) * curZOffset;
            f32 camWidth = camHeight * CAMERACONTROLLERMANAGER->m_screenRatio;

            m_actor->setPos( camPos + ( Vec3d( 1.f, 0.f, 0.f ) * camWidth ) );
        }
        else 
        {
            m_actor->setPos( m_actor->getWorldInitialPos() );
            m_actor->setDepth( _checkPoint->getDepth() );
        }
                
        // Save "to" modifiers
        //---------------------
        if ( usedModifier != NULL )
        {
            m_toModifier = *usedModifier;
        }
        m_curDepth = m_actor->getDepth();
        m_toModifier.m_depth = m_curDepth;
        CAMERACONTROLLERMANAGER->m_controllerJustTeleported = btrue;
    }

    //-----------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterCameraComponent::onEvent( Event* _event )
    {
        Super::onEvent(_event);

        if ( EventPlayerModeChanged* eventGameMode = _event->DynamicCast<EventPlayerModeChanged>(ITF_GET_STRINGID_CRC(EventPlayerModeChanged,1336647275)) )
        {
            if ( GAMEMANAGER->areAllActivePlayersInGameMode(RAY_GAMEMODE_SHOOTER) )
            {
                CAMERACONTROLLERMANAGER->setMainCameraController(this, getTemplate()->getRampUpDestinationCoeff(), getTemplate()->getRampUpCoeff());
                m_activated = btrue;
            } 
            else if (m_activated)
            {
                CAMERACONTROLLERMANAGER->removeMainCameraController(this, getTemplate()->getRampUpDestinationCoeff(), getTemplate()->getRampUpCoeff());
                m_activated = bfalse;
            }
        }
        /*else if ( EventRevertToLastCheckpoint* eventRevert = _event->DynamicCast<EventRevertToLastCheckpoint>(ITF_GET_STRINGID_CRC(EventRevertToLastCheckpoint,3654122082)) )
        {
            m_movement->disable();
        }*/
    }

    //-----------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterCameraComponent::Update(f32 _dt)
    {
        if ( !m_activated )
            return;

        updateCameraTransition(_dt);
        updateCameraFromModifier(_dt);

#ifdef DBG_SHOOTERCAMERA
        for ( u32 i = 0; i < m_debugTransitionPoints.size(); ++i )
        {
            DebugDraw::circle( m_debugTransitionPoints[i], 0.02f, Color::green(), 0.0f );
        }
#endif //DBG_SHOOTERCAMERA
    }

    //-----------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterCameraComponent::updateCameraTransition(f32 _dt)
    {
        // Notify event after camera transition
        //---------------------------------------
        if ( m_transitionPlayerNotificationTimer > 0.f )
        {
            m_transitionPlayerNotificationTimer -= _dt;
            if ( m_transitionPlayerNotificationTimer < 0.f )
            {
                m_transitionPlayerNotificationTimer = 0.0f;
                sendTransitionNotifyEvent();
            }
        }

        if ( !m_transitionActive )
            return;

        // Notify event before camera transition
        //-----------------------------------------
        if ( m_transitionPlayerNotificationTimer < 0.f )
        {
            m_transitionPlayerNotificationTimer += _dt;
            if ( m_transitionPlayerNotificationTimer < 0.f )
                return;
            else
            {
                m_transitionPlayerNotificationTimer = 0.0f;
                m_curTransition.start();
            }
        }

        m_curTransition.update(_dt);
        m_modifierBlendCursor = m_curTransition.getBlendCursor();
        m_transitionAdditionalSpeed = m_curTransition.getCurAdditionalSpeed();

        // Finished ?
        if ( m_curTransition.isFinished() )
        {
            m_modifierBlendCursor = 1.f;
            m_transitionActive = bfalse;
            m_transitionAdditionalSpeed = Vec2d::Zero;
        }
    }

    //-----------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterCameraComponent::updateCameraFromModifier(f32 _dt)
    {
         if ( !m_activated )
             return;
         
        m_curMoveSpeed  = Interpolate( m_fromModifier.m_moveSpeed, m_toModifier.m_moveSpeed, m_modifierBlendCursor ) + m_transitionAdditionalSpeed;
        m_curZoffset    = Interpolate( m_fromModifier.m_zOffset, m_toModifier.m_zOffset, m_modifierBlendCursor );
        m_curDepth      = Interpolate( m_fromModifier.m_depth, m_toModifier.m_depth, m_modifierBlendCursor );

        m_actor->set2DPos( m_actor->get2DPos() + ( m_curMoveSpeed * _dt ) );
        m_actor->setDepth( m_curDepth );

#ifdef DBG_SHOOTERCAMERA
        if ( m_transitionActive )
        {
            m_debugTransitionPoints.push_back( m_actor->getPos() );
        }
#endif //DBG_SHOOTERCAMERA
    }

    //-----------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterCameraComponent::registerModifier(  const ShooterCameraModifier & _newModifier, 
                                                        const ShooterCameraModifier_Transition & _transition )
    {
        // Save "from" modifier
        computeModifierFromCamera( m_fromModifier );
        // save "To" modifier
        m_toModifier = _newModifier;
        // save the transition
        m_curTransition = _transition;

        startTransition();
    }

    //-----------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterCameraComponent::computeModifierFromCamera( ShooterCameraModifier & _modifier )
    {
        _modifier.m_moveSpeed  = m_curMoveSpeed;
        _modifier.m_zOffset    = m_curZoffset;
        _modifier.m_depth      = m_curDepth;

        m_transitionAdditionalSpeed = Vec2d::Zero;
    }

    //-----------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterCameraComponent::startTransition()
    {
        m_transitionActive      = btrue;
        m_modifierBlendCursor   = 0.f;
        m_transitionPlayerNotificationTimer = m_curTransition.m_notifyPlayerDelay;
        
        if ( m_transitionPlayerNotificationTimer > 0.f )
        {
            m_curTransition.start();
        }
        else if ( m_transitionPlayerNotificationTimer < 0.f )
        {
            sendTransitionNotifyEvent();
        }
        else
        {
            sendTransitionNotifyEvent();
            m_curTransition.start();
        }

#ifdef DBG_SHOOTERCAMERA
        m_debugTransitionPoints.clear();
#endif //DBG_SHOOTERCAMERA
    }

    //-----------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterCameraComponent::sendTransitionNotifyEvent()
    {
        Ray_EventShooterCamera_TransitionNotify evt;
        evt.setDestDepth( m_toModifier.m_depth );
        evt.setTransitionType( m_curTransition.m_type );
        evt.setTransitionDuration( m_curTransition.m_duration );
        EVENTMANAGER->broadcastEvent( &evt );
    }

    //-----------------------------------------------------------------------------------------------------------------------
    bbool Ray_ShooterCameraComponent::isInTransition( f32 & _res_DestDepth )
    {
        _res_DestDepth = m_toModifier.m_depth;
        return m_transitionActive;
    }

    //-----------------------------------------------------------------------------------------------------------------------
    bbool Ray_ShooterCameraComponent::getCurTransitionInfo( ShooterCameraModifier_Transition * _transition, f32 & _res_DestDepth )
    {
        if ( m_transitionActive )
        {
            _res_DestDepth = m_toModifier.m_depth;
            ITF_MEMCOPY( _transition, &m_curTransition, sizeof( ShooterCameraModifier_Transition ) );
            return btrue;
        }
        return bfalse;
    }

    //-----------------------------------------------------------------------------------------------------------------------
    Vec2d Ray_ShooterCameraComponent::getSpeed() const 
    {
        if ( m_activated )
        {
            return m_curMoveSpeed;//.truncateTo2D();
        }
        return Vec2d::Zero;
    }

    //-----------------------------------------------------------------------------------------------------------------------
    Vec3d Ray_ShooterCameraComponent::getCameraPosition() const 
    {
        Vec3d pos;
        if (m_actor)
        {
            pos = m_actor->getPos();
            pos.m_z += m_curZoffset;
        }
        return pos;
    }

#ifdef ITF_SUPPORT_EDITOR
    //-----------------------------------------------------------------------------------------------------------------------
    void Ray_ShooterCameraComponent::drawEdit( class ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        Super::drawEdit( _drawInterface, _flags );

        // compute AABB
        f32 camHeight = tanf( getCameraFocale() * 0.5f ) * m_curZoffset;
        f32 camWidth = camHeight * CAMERACONTROLLERMANAGER->m_screenRatio;
        Vec3d borders( camWidth, camHeight, 0.0f );
        AABB tempAABB( m_actor->getPos() - borders, m_actor->getPos() + borders );

        GFX_ADAPTER->drawDBGAABB( tempAABB, 0, 0, 1, 0, 2, m_actor->getDepth() );

        Vec2d speed = getSpeed();
        String s_text;
        s_text.setTextFormat( "Speed : x = %f, y = %f", speed.m_x, speed.m_y );
        Vec3d res2d;
        Vec3d displayPos = m_actor->getPos();
        displayPos.m_y += 0.5f;
        GFX_ADAPTER->compute3DTo2D( displayPos, res2d );
        GFX_ADAPTER->drawDBGText( s_text, res2d.m_x, res2d.m_y );
        
        GFX_ADAPTER->drawDBGLine( m_actor->get2DPos(), m_actor->get2DPos() + speed.normalize() * 9999.0f , Color::blue(), 0.f, 2.f, m_actor->getDepth() );
    }
#endif // ITF_SUPPORT_EDITOR

    //-----------------------------------------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(Ray_ShooterCameraComponent_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_ShooterCameraComponent_Template)
        SERIALIZE_MEMBER("defaultSpeed",        m_defaultSpeed );   
        SERIALIZE_MEMBER("defaultZOffset",      m_defaultZOffset );   
    END_SERIALIZATION()

    Ray_ShooterCameraComponent_Template::Ray_ShooterCameraComponent_Template()
    : Super()
    , m_defaultSpeed( k_ShooterCameraDefaultSpeed )
    , m_defaultZOffset( k_ShooterCameraDefaultZOffset )
    {
    }

} // namespace ITF
