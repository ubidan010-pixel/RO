#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_PLATFORMERCAMERACOMPONENT_H_
#include "rayman/gameplay/Components/Camera/Ray_PlatformerCameraComponent.h"
#endif //_ITF_RAY_PLATFORMERCAMERACOMPONENT_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/Managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_
#include "rayman/gameplay/Components/Player/Ray_PlayerControllerComponent.h"
#endif //_ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_CAMERASUBJECTCOMPONENT_H_
#include "gameplay/Components/Misc/CameraSubjectComponent.h"
#endif //_ITF_CAMERASUBJECTCOMPONENT_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_PlatformerCameraComponent)
    BEGIN_SERIALIZATION_CHILD(Ray_PlatformerCameraComponent)
    END_SERIALIZATION()

    IMPLEMENT_OBJECT_RTTI(Ray_PlatformerCameraComponent_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_PlatformerCameraComponent_Template)
        SERIALIZE_MEMBER("useEjectMargin", m_useEjectMargin);
        SERIALIZE_MEMBER("ejectMargin", m_ejectMargin);
        SERIALIZE_MEMBER("ejectMarginDetachesPlayer", m_ejectMarginDetachesPlayer);
        SERIALIZE_MEMBER("ejectForce", m_ejectForce);
        SERIALIZE_MEMBER("useDeathMargin", m_useDeathMargin);
        SERIALIZE_MEMBER("deathMargin", m_deathMargin);
        SERIALIZE_MEMBER("timeOut", m_timeOut);
        SERIALIZE_MEMBER("lastOnscreenPlayerKillsEveryone", m_lastOnscreenPlayerKillsEveryone);
        SERIALIZE_MEMBER("pursuitUseDeathMargin", m_pursuitUseDeathMargin);
        SERIALIZE_MEMBER("pursuitDeathMargin", m_pursuitDeathMargin);
        SERIALIZE_MEMBER("pursuitTimeOut", m_pursuitTimeOut);
    END_SERIALIZATION()

    Ray_PlatformerCameraComponent_Template::Ray_PlatformerCameraComponent_Template()
        : m_useEjectMargin(btrue)
        , m_ejectMargin(-1.f)
        , m_ejectMarginDetachesPlayer(btrue)
        , m_ejectForce(100)
        , m_useDeathMargin(btrue)
        , m_deathMargin(1.f)
        , m_timeOut(2.f)
        , m_lastOnscreenPlayerKillsEveryone(btrue)
        , m_pursuitUseDeathMargin(btrue)
        , m_pursuitDeathMargin(1.f)
        , m_pursuitTimeOut(2.f)
    {
    }
    Ray_PlatformerCameraComponent_Template::~Ray_PlatformerCameraComponent_Template()
    {
    }


    Ray_PlatformerCameraComponent::Ray_PlatformerCameraComponent()
        : Super()
    {
     
    }

    Ray_PlatformerCameraComponent::~Ray_PlatformerCameraComponent()
    {
        if ( EVENTMANAGER )
        {
            EVENTMANAGER->unregisterEvent(ITF_GET_STRINGID_CRC(EventPlayerModeChanged,1336647275), this);
        }
    }

    void Ray_PlatformerCameraComponent::onEvent( Event* _event )
    {
        Super::onEvent(_event);

        if ( EventPlayerModeChanged* eventGameMode = _event->DynamicCast<EventPlayerModeChanged>(ITF_GET_STRINGID_CRC(EventPlayerModeChanged,1336647275)) )
        {
            if ( GAMEMANAGER->areAllActivePlayersInGameMode(RAY_GAMEMODE_PLATFORMER) )
            {
                CAMERACONTROLLERMANAGER->setMainCameraController(this, getTemplate()->getRampUpDestinationCoeff(), getTemplate()->getRampUpCoeff());
            }
        }
    }
   
    void Ray_PlatformerCameraComponent::onCameraEvent( Event* _event )
    {
        if ( Ray_EventQueryCameraLimiter* query = _event->DynamicCast<Ray_EventQueryCameraLimiter>(ITF_GET_STRINGID_CRC(Ray_EventQueryCameraLimiter,358012532)) )
        {
            processQueryLimiter(query);
        }
    }

    void Ray_PlatformerCameraComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        EVENTMANAGER_REGISTER_EVENT_LISTENER(ITF_GET_STRINGID_CRC(EventPlayerModeChanged,1336647275),this);
    }

    //////////////////////////////////////////////////////////////////////////
    // test camera
    bbool Ray_PlatformerCameraComponent::isPlayingPageSequence( ObjectRef _objRef )
    {
        if( !RAY_GAMEMANAGER->isPlayingPageSequence() )
            return bfalse;       

        return RAY_GAMEMANAGER->isPlayerInSequence(_objRef);
    }

    bbool Ray_PlatformerCameraComponent::isPlayingShooter( ObjectRef _objRef )
    {
        if ( const Player * player = GAMEMANAGER->getPlayerFromActor( _objRef, bfalse ) )
        {
            if ( player->getCurrentMode() == RAY_GAMEMODE_SHOOTER )
            {
                return btrue;
            }
        }

        return bfalse;
    }

    void Ray_PlatformerCameraComponent::setSubjectStance ( SubjectStance* _subject )
    {        
        const Actor* actor = (Actor*)GETOBJECT( _subject->m_objectRef );

        Ray_PlayerControllerComponent* controlComp = actor->GetComponent<Ray_PlayerControllerComponent>();
        AnimLightComponent* anim = actor->GetComponent<AnimLightComponent>();

        if (controlComp)
        {
            StickToPolylinePhysComponent* physComp = actor->GetComponent<StickToPolylinePhysComponent>();

            // swing on actor
            const ObjectRef swingOnActor = controlComp->getSwingOnActor();
            if ( swingOnActor.isValid() )
            {
                Actor* swingActor = AIUtils::getActor(swingOnActor);

                if ( swingActor )
                {                    
                    _subject->m_isLockedPosition = btrue;
                    _subject->m_lockedPosition = swingActor->get2DPos() -Vec2d(0.f, controlComp->getSwingOnActorLength());
                }
            }

            // climb on polyline
            const ObjectRef climbOnPolyline = controlComp->getClimbOnPolyline();
            if ( climbOnPolyline.isValid() && controlComp->isClimbOnPolylineVertical() )
            {
                const PolyLine* poly = (PolyLine*)GETOBJECT(climbOnPolyline);
                
                if ( poly && poly->getPosCount()>0 )
                {                   
                    _subject->m_isLockedPosition = btrue;                        
                    _subject->m_lockedPosition = poly->getPosAt(0) -Vec2d(0.f, poly->getLength());
                }
            }

            // stick
            if( physComp->getStickedPolyline() || controlComp->isHanging() || controlComp->isClimbing() || controlComp->isWallSliding() || _subject->m_isLockedPosition )
            {
                _subject->m_stick = btrue;
            }   
            else
            {
                _subject->m_windForce = controlComp->getCurrentWindForce();
            }

            // page system
            if ( isPlayingPageSequence( _subject->m_objectRef ) )
            {
                _subject->m_isLockedPosition = btrue;
                _subject->m_lockedPosition = RAY_GAMEMANAGER->getChangePageSequenceFinalPoint().truncateTo2D();

                const CameraSubjectComponent* subjectComp = actor->GetComponent<CameraSubjectComponent>();
                if ( subjectComp )
                    _subject->m_lockedPosition.m_y += ( subjectComp->getBoundingBoxMax().m_y -subjectComp->getBoundingBoxMin().m_y ) *actor->getScale().m_y *0.5f;
            }

            // hang
            _subject->m_hang = controlComp->isHanging();

            //  u-turn
            _subject->m_performUTurn = controlComp->isPerformingUTurn();            

            //  orientation
            if ( anim )
            {
                _subject->m_lookDir = AIUtils::getLookDir( actor, actor->isFlipped());
            }

            // joy
            _subject->m_joystickMoveNormalized = controlComp->getMoveDirNormalized();

            // wall slide
            _subject->m_wallSlide = controlComp->isWallSliding();

            // jump on air timer
            _subject->m_jumpOnAirTimerAllowed = controlComp->isJumpOnAirTimerAllowed();

            //_subject->m_fight = controlComp->isFighting();
            // _subject->m_isHelicoptering = controlComp->isHelicoptering();
            //_subject->m_swim = controlComp->isSwimming();
        }
    }

    void Ray_PlatformerCameraComponent::processQueryLimiter( Ray_EventQueryCameraLimiter* _query )
    {
        CameraModifierComponent* cameraModifier = getCameraModifier();
        if (!cameraModifier)
        {
            // no modifier -> no limits
            return;
        }


        Ray_CameraLimiterData& limiterData = _query->getLimiterData();
        const bbool inPursuit = CAMERACONTROLLERMANAGER->hasLeadCameraSubject();

        // set some defaults that the modifier may not want to override
        limiterData.setTimeOut( inPursuit ? getTemplate()->getPursuitTimeOut() : getTemplate()->getTimeOut() );
        limiterData.setConstraint(cameraModifier);

        // relay the query to the modifier, see if it wants to handle in our stead
        cameraModifier->GetActor()->onEvent(_query);
        if (_query->getQueryHandled())
        {
            return;
        }


        // the modifier didn't handle the margin, let's set our own

        Vec2d direction = cameraModifier->getUpdateData().getDirection();
        direction.normalize();

        limiterData.setUseEjectMargin( getTemplate()->getUseEjectMargin() );
        if (getTemplate()->getUseEjectMargin())
        {
            const f32 margin = getTemplate()->getEjectMargin();
            limiterData.setEjectMargin( Margin(margin, margin, margin, margin) );
            limiterData.setEjectMarginActive( computeLimits(direction, bfalse) );
            limiterData.setEjectMarginDetachesPlayer(getTemplate()->getEjectMarginDetachesPlayer());
            limiterData.setEjectForce( getTemplate()->getEjectForce() );
        }


        bbool useDeathMargin = getTemplate()->getUseDeathMargin();
        f32 deathMargin = getTemplate()->getDeathMargin();
        if (inPursuit)
        {
            useDeathMargin = getTemplate()->getPursuitUseDeathMargin();
            deathMargin = getTemplate()->getPursuitDeathMargin();
        }
        limiterData.setUseDeathMargin(useDeathMargin);
        if (useDeathMargin)
        {
            limiterData.setDeathMargin(computeLimits(deathMargin, direction, btrue));
        }


        limiterData.setLastOnscreenPlayerKillsEveryone(getTemplate()->getLastOnscreenPlayerKillsEveryone());


        _query->setQueryHandled(btrue);
    }

    // enable/disable the limits depending on the modifier direction
    Margin Ray_PlatformerCameraComponent::computeLimits( const f32 _margin, const Vec2d& _direction, const bbool _useVerticalLimit )
    {
        #define DISABLED 100.f

        Margin clipped(DISABLED, DISABLED, DISABLED, DISABLED);

        if (_direction.m_x > 0.5f)
        {
            clipped.m_left = _margin;
        }
        else if (_direction.m_x < -0.5f)
        {
            clipped.m_right = _margin;
        }

        if (_useVerticalLimit)
        {
            if (_direction.m_y > 0.5f)
            {
                clipped.m_bottom = _margin;
            }
            else if (_direction.m_y < -0.5f)
            {
                clipped.m_top = _margin;
            }
        }

        return clipped;
    }

    // enable/disable the limits depending on the modifier direction
    MarginBool Ray_PlatformerCameraComponent::computeLimits( const Vec2d& _direction, const bbool _useVerticalLimit )
    {
        MarginBool active;

        if (_direction.m_x > 0.5f)
        {
            active.m_left = btrue;
        }
        else if (_direction.m_x < -0.5f)
        {
            active.m_right = btrue;
        }

        if (_useVerticalLimit)
        {
            if (_direction.m_y > 0.5f)
            {
                active.m_bottom = btrue;
            }
            else if (_direction.m_y < -0.5f)
            {
                active.m_top = btrue;
            }
        }

        return active;
    }

#ifdef ITF_SUPPORT_DEBUGFEATURE
    void Ray_PlatformerCameraComponent::drawDebug( const CameraControllerData& _data )
    {
        Super::drawDebug(_data);

        if (_data.m_weight <= MTH_EPSILON)
        {
            return;
        }


        CameraModifierComponent* cameraModifier = getCameraModifier();
        if (!cameraModifier)
        {
            // no modifier -> no limits
            return;
        }

        //DebugDraw::line3D(cameraModifier->GetActor()->getPos(), cameraModifier->GetActor()->getPos() + direction.to3d());
        //DebugDraw::AABB(Vec3d::Zero, 0, CAMERACONTROLLERMANAGER->getAABB());

        Ray_EventQueryCameraLimiter query;
        processQueryLimiter( &query );
        f32 z = cameraModifier->GetActor()->getDepth();

        if (getTemplate()->getUseEjectMargin())
        {
            AABB aabb;
            if ( query.getLimiterData().getEjectAABB(z, aabb) )
            {
                DebugDraw::AABB(Vec3d::Zero, 0, aabb, Color::green(), 0, 5, "cam soft col");
            }
        }

        bbool useDeathMargin = getTemplate()->getUseDeathMargin();
        if (CAMERACONTROLLERMANAGER->hasLeadCameraSubject())
        {
            useDeathMargin = getTemplate()->getPursuitUseDeathMargin();
        }
        if (useDeathMargin)
        {
            AABB aabb;
            if ( query.getLimiterData().getDeathAABB(z, aabb) )
            {
                DebugDraw::AABB(Vec3d::Zero, 0, aabb, Color::red(), 0, 5, "cam kill col");
            }
        }
    }
#endif // ITF_SUPPORT_DEBUGFEATURE

}// namespace ITF