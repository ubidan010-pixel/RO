#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_CHANGEPAGECOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_ChangePageComponent.h"
#endif //_ITF_RAY_CHANGEPAGECOMPONENT_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_LINKMANAGER_H_
#include "gameplay/Managers/LinkManager.h"
#endif //_ITF_LINKMANAGER_H_

#ifndef _ITF_SHAPEDETECTORCOMPONENT_H_
#include "gameplay/Components/Trigger/ShapeDetectorComponent.h"
#endif //_ITF_SHAPEDETECTORCOMPONENT_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_MUSICMANAGER_H_
#include "engine/gameplay/musicmanager.h"
#endif //_ITF_MUSICMANAGER_H_

#ifndef _ITF_METRONOMEMANAGER_H_
#include "engine/sound/MetronomeManager.h"
#endif //_ITF_METRONOMEMANAGER_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_ChangePageComponent)
    BEGIN_SERIALIZATION_CHILD(Ray_ChangePageComponent)
	
        BEGIN_CONDITION_BLOCK( ESerializeGroup_Checkpoint )
            SERIALIZE_MEMBER( "finalDefaultPos", m_finalDefaultPos );
        END_CONDITION_BLOCK()

    END_SERIALIZATION()

	BEGIN_VALIDATE_COMPONENT(Ray_ChangePageComponent)
	VALIDATE_COMPONENT_PARAM("",m_shapeDetectorComponent, "ChangePageComponent requires a ShapeDetectorComponent");
	END_VALIDATE_COMPONENT()

    

	//--------------------------------------------------------------------------------------------------------
    Ray_ChangePageComponent::Ray_ChangePageComponent()
    : Super()
	, m_shapeDetectorComponent(NULL)
    , m_linkComponent(NULL)
    , m_animatedComponent(NULL)
	, m_computedExitType( ExitType_Left )
	, m_computedExitPointValid( bfalse )
	, m_computedFinalPointValid( bfalse )
    , m_computedFinalPointInWater( bfalse )
    , m_warpInitialized(bfalse)
    , m_openingTime(0.f)
    , m_otherDoorTime(0.f)
    , m_enableDelayCounter(0.f)
    , m_openingFrameCheck(U32_INVALID)
    , m_opening(bfalse)
    , m_pageDoorsOpened(bfalse)
    , m_fxControllerComponent( NULL )
    , m_remainingTimeToRestartElectoonSound(MTH_HUGE)
    , m_currentElectoonSoundHandle(U32_INVALID)
    , m_isShut(bfalse)
	{
    }

	Ray_ChangePageComponent::~Ray_ChangePageComponent()
	{
	}

    //--------------------------------------------------------------------------------------------------------
    bbool Ray_ChangePageComponent::isCageMapDoor() const
    {
        return getTemplate()->isCageMapDoor();
    }
	//--------------------------------------------------------------------------------------------------------
	void Ray_ChangePageComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
	{
		Super::onActorLoaded(_hotReload);

        m_finalDefaultPos = m_actor->get2DPos();
		m_shapeDetectorComponent = m_actor->GetComponent<ShapeDetectorComponent>();
        m_linkComponent = m_actor->GetComponent<LinkComponent>();
        m_animatedComponent = m_actor->GetComponent<AnimatedComponent>();
        m_fxControllerComponent = m_actor->GetComponent<FXControllerComponent>();
        		
		ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventChangePage,1004490573), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventChangePageQueryPoints,1998255083), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTrigger,1343042510),this);
        m_remainingTimeToRestartElectoonSound = getTemplate()->getElectoonHelpPeriod();

        if ( getTemplate()->getOpeningRadius() && getTemplate()->getOpeningTime() && m_animatedComponent )
        {
            m_animatedComponent->setUpdateAnimInput(this);
        }

        m_enableDelayCounter = getTemplate()->getEnableDelay();
	}

    //--------------------------------------------------------------------------------------------------------
    void Ray_ChangePageComponent::onBecomeActive()
    {
        Super::onBecomeActive();

        if ( m_enableDelayCounter )
        {
            m_shapeDetectorComponent->setDisabled(btrue);
        }
    }

	//--------------------------------------------------------------------------------------------------------
	void Ray_ChangePageComponent::onEvent( Event * _event)
	{
		// Event from trigger
		if ( Ray_EventChangePage * eventChangePage = _event->DynamicCast<Ray_EventChangePage>(ITF_GET_STRINGID_CRC(Ray_EventChangePage,1004490573)) )
		{
            processChangePageEvent( eventChangePage );
		}
        else if ( Ray_EventChangePageQueryPoints * eventChangePageQueryPoints = _event->DynamicCast<Ray_EventChangePageQueryPoints>(ITF_GET_STRINGID_CRC(Ray_EventChangePageQueryPoints,1998255083)) )
        {
            processChangePageQueryPointsEvent( eventChangePageQueryPoints );
        }
        else if ( EventTrigger * trigger = _event->DynamicCast<EventTrigger>(ITF_GET_STRINGID_CRC(EventTrigger,1343042510)) )
        {
            processEventTrigger( trigger );
        }

        // !! No call to parent is wanted !!
        //Super::onEvent( _event );
	}

    //--------------------------------------------------------------------------------------------------------
    Vec2d Ray_ChangePageComponent::getPlayerSpawnPos( u32 _playerIndex ) const 
    { 
        Actor * player = GAMEMANAGER->getActivePlayer( _playerIndex );
        if ( player != NULL )
        {
            return m_finalDefaultPos - ( ( Vec2d::Right.Rotate(player->getAngle()-MTH_PIBY2) ) * player->getRadius() );
        }
        return m_finalDefaultPos; 
    }

	//--------------------------------------------------------------------------------------------------------
	Ray_ChangePageComponent::ExitType Ray_ChangePageComponent::ComputePoints(	Vec2d const & _enterDir, 
																				Vec2d & _res_enterPoint, 
																				Vec2d & _res_exitPoint, 
																				Vec2d & _res_finalPoint,
																				bbool & _res_exitPointValid, 
																				bbool & _res_finalPointValid,
                                                                                bbool & _res_finalPointInWater,
                                                                                bbool _enterPointOnly /*= bfalse*/ ) const
	{
		// Check exit Type
		//------------------
		ExitType exitType;
        Vec2d actorDir = Vec2d::Right.Rotate( m_actor->getAngle() );
		f32 dotSign = getSign( _enterDir.dot( actorDir ) );
		
		f32 horizDot = actorDir.dot( Vec2d::Right ) * dotSign;
		f32 vertiDot = actorDir.dot( Vec2d::Up ) * dotSign;

		if ( horizDot >= 0.72f )
		{
			exitType = ExitType_Right;
		}
		else if ( horizDot <= -0.72f )
		{
			exitType = ExitType_Left;
		}
		else if ( vertiDot > 0.0f )
		{
			exitType = ExitType_Up;
		}
		else
		{
			exitType = ExitType_Down;
		}

		// Compute enter point
		//--------------------
		PhysShapeBox* triggerShape = static_cast<PhysShapeBox*>( m_shapeDetectorComponent->getCurrentShape() );
		ITF_ASSERT( triggerShape != NULL );
        if ( triggerShape == NULL )
        {
            _res_enterPoint = _res_exitPoint = _res_finalPoint = m_actor->get2DPos();
            _res_finalPointValid = bfalse;
            _res_exitPointValid = bfalse;
            return exitType;
        }

		// Enter point is the trigger shape enter point
		Vec2d enterPoint = getEnterPoint( -dotSign, triggerShape, actorDir );

		// Compute exit points
		//----------------------
		const f32 horizDistTest = triggerShape->getExtent().m_y;
		const f32 vertDistTest = triggerShape->getExtent().m_x + 5.0f;

		Vec2d exitPoint = getExitPoint( -dotSign, triggerShape->getExtent().m_x, actorDir );
		Vec2d finalPoint = exitPoint;
		bbool computedExitPointValid = !_enterPointOnly;
		bbool computedFinalPointValid = !_enterPointOnly;

		switch ( exitType )
		{
			case ExitType_Left :
			case ExitType_Right :
			{
                if ( !_enterPointOnly )
                {
				    finalPoint = getFinalPoint( -dotSign, triggerShape, actorDir );

				    if ( !isPointInWater( finalPoint ) )
				    {
					    // Exit point on ground
                        f32 lineCheckLength = (horizDistTest*2.f) + 3.f;
					    computedExitPointValid = getPosOnGround( exitPoint , exitPoint + Vec2d( 0.f, horizDistTest ), lineCheckLength );
					    // Final point (is on zone shape)
					    computedFinalPointValid = getPosOnGround( finalPoint , finalPoint + Vec2d( 0.f, horizDistTest ), lineCheckLength );
				    }				
                }
				break;
			}

			case ExitType_Up :
			{
				Vec2d upColl;
				f32 collDist = vertDistTest;
				
				// Get the box orientation by default
				f32 sideSign = getSign( ( exitPoint - m_actor->get2DPos() ).dot( Vec2d::Right ) );
				// User defined ?
				if ( getTemplate()->isExitUpSideDefined() )
				{
					sideSign = getTemplate()->isExitUpLeftSide() ? -1.f : 1.f;
				}	
				// recompute enter/exit point to be vertical
				enterPoint = m_actor->get2DPos() + Vec2d( 0.f, ( enterPoint - m_actor->get2DPos() ).dot( Vec2d::Up ) );

                if ( !_enterPointOnly )
                {
    			    exitPoint = m_actor->get2DPos() + Vec2d( 0.f, ( exitPoint - m_actor->get2DPos() ).dot( Vec2d::Up ) );

				    // vertical coll check
				    if ( getPosOnGround( upColl, exitPoint, -vertDistTest ) )
				    {
					    upColl += Vec2d( 0.0, -0.1f );
					    collDist = (exitPoint - upColl).norm();
				    }
				    else
				    {
					    upColl += Vec2d( 0.0f, vertDistTest );
				    }
#ifndef ITF_FINAL
				    //DebugDraw::circle( upColl, 0.0, 0.3f, Color::red(), 0.0f );
#endif // ITF_FINAL
    											
				    upColl += Vec2d( sideSign * ( triggerShape->getExtent().m_y + getTemplate()->getExitUpSideOffset() ), 0.0f );

#ifndef ITF_FINAL
				    //DebugDraw::circle( upColl, 0.0, 0.3f, Color::red(), 0.0f );				
#endif // ITF_FINAL
				    computedFinalPointValid = getPosOnGround( finalPoint, upColl, vertDistTest + collDist );
				    if ( !computedFinalPointValid )
				    {
					    finalPoint -= Vec2d( 0.0f, collDist );
				    }
				    else
				    {
					    // Check water
					    if ( isPointInWater( exitPoint ) && isPointInWater( finalPoint ) )
					    {
						    finalPoint = exitPoint + Vec2d( 0.0f, getTemplate()->getExitUpSideOffset() );
					    }
				    }
                }
				break;
			}
			case ExitType_Down :
			{
				// Recompute the enter/exit point
				enterPoint = m_actor->get2DPos() + Vec2d( 0.f, -( enterPoint - m_actor->get2DPos() ).dot( -Vec2d::Up ) );

                if ( !_enterPointOnly )
                {
				    exitPoint = m_actor->get2DPos() + Vec2d( 0.f, -( exitPoint - m_actor->get2DPos() ).dot( -Vec2d::Up ) );
				    finalPoint = exitPoint;
				    computedFinalPointValid = btrue;

				    // User defined, check collisions
				    if ( getTemplate()->isExitDownSideDefined() )
				    {
					    f32 sideSign = getTemplate()->isExitDownLeftSide() ? -1.f : 1.f;
					    finalPoint = exitPoint + Vec2d( sideSign * ( triggerShape->getExtent().m_y + getTemplate()->getExitDownSideOffset() ), 0.0f );
					    computedFinalPointValid = getPosOnGround( finalPoint, finalPoint, vertDistTest * 2.0f );
				    }
				    // Water check
				    //
				    if ( isPointInWater( exitPoint ) && isPointInWater( finalPoint ) )
				    {
					    finalPoint = exitPoint + Vec2d( 0.0f, -getTemplate()->getExitDownSideOffset() );
				    }
                }
				break;
			}
		}

        _res_enterPoint = enterPoint; 
        if ( !_enterPointOnly )
        {
		    _res_exitPoint = exitPoint;
		    _res_finalPoint = finalPoint;
		    _res_exitPointValid = computedExitPointValid;
		    _res_finalPointValid = computedFinalPointValid;
            _res_finalPointInWater = ( isPointInWater( _res_exitPoint ) && isPointInWater( _res_finalPoint ) );
        }
		return exitType;
	}

    //--------------------------------------------------------------------------------------------------------
    bbool Ray_ChangePageComponent::isRefDoor( ObjectRef _ref ) const
    {
        if ( m_linkComponent != NULL )
        {
            const LinkComponent::ChildrenList& children = m_linkComponent->getChildren();
            u32 numChildren = children.size();

            for ( u32 i = 0; i < numChildren; i++ )
            {
                const ChildEntry& child = children[i];

                if ( child.hasTag(ITF_GET_STRINGID_CRC(DoorLink,2094447933)) )
                {
                    const ObjectPath& path = child.getPath();
                    Pickable* target;

                    if ( path.getIsAbsolute() )
                    {
                        target = SceneObjectPathUtils::getObjectFromAbsolutePath(path);
                    }
                    else
                    {
                        target = SceneObjectPathUtils::getObjectFromRelativePath(m_actor,path);
                    }

                    if ( target && target->getRef() == _ref )
                    {
                        return btrue;
                    }
                }
            }
        }

        return bfalse;
    }

	//--------------------------------------------------------------------------------------------------------
	bbool Ray_ChangePageComponent::getPosOnGround( Vec2d & _res_foundPoint, Vec2d const & _startPoint, f32 const & _deltaDist ) const
	{
		_res_foundPoint = _startPoint;
		Vec2d groundTestVec = Vec2d( 0.0f, -_deltaDist );
		PhysRayCastContactsContainer groundContact;
		f32 nearestVal = 99999.9f; 
		i32 nearestIdx = -1;

		if ( !PHYSWORLD->rayCastEnvironment(	_startPoint, 
											_startPoint + groundTestVec, 
											ECOLLISIONFILTER_ENVIRONMENT,  
											m_actor->getDepth(), groundContact ) )
		{
			return bfalse;
		}

        Vec2d normal;

		for ( u32 i = 0; i < groundContact.size(); ++i )
		{
			if ( groundContact[i].m_t0 < nearestVal )
			{
				PolyLine* poly = AIUtils::getPolyLine(groundContact[i].m_collidableUserData);

                if ( poly )
                {
                    Actor* ownerActor = poly->getOwnerActor();
                    if ( ownerActor && isRefDoor(ownerActor->getRef()) )
                    {
                        continue;
                    }

					normal = poly->getEdgeAt(groundContact[i].m_edgeIndex0).m_normalizedVector.getPerpendicular();

                    if ( normal.dot( groundTestVec ) < 0.0f )
                    {
                        nearestVal = groundContact[i].m_t0;
                        nearestIdx = i;
                    }
				}
			}
		}
		// Has a valid point use it
		if ( nearestIdx >= 0 )
		{
			_res_foundPoint += ( groundTestVec * groundContact[nearestIdx].m_t0 );
			return btrue;
		}
		return bfalse;
	}

	//--------------------------------------------------------------------------------------------------------
	bbool Ray_ChangePageComponent::isPointInWater( const Vec2d& _point ) const
	{
		return ( AIUtils::getPolylineRegion( m_actor->getDepth(),s_RegionName_Swim, _point ) != NULL ) ? btrue : bfalse;
	}

    //--------------------------------------------------------------------------------------------------------
    Ray_ChangePageComponent * Ray_ChangePageComponent::getDestinationPageComponent() const
    {
        Ray_ChangePageComponent * destPageComp = NULL;

        if ( m_linkComponent != NULL )
        {
            if ( ObjectRefList* children = LINKMANAGER->getChildren( m_actor->getRef() ) )
            {
                for (u32 i = 0; i < children->size(); i++)
                {
                    BaseObject * baseObj = ((*children)[i]).getObject();
                    
                    if ( baseObj != NULL )
                    {
                        if ( Actor* child = baseObj->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115)) )
                        {
                            destPageComp = child->GetComponent<Ray_ChangePageComponent>();

                            if ( destPageComp != NULL )
                            {
                                break;
                            }
                        }
                    }                    
                }
            }
        }
        return destPageComp;
    }

    //--------------------------------------------------------------------------------------------------------
    Ray_ChangePageComponent * Ray_ChangePageComponent::getStartPageComponent() const
    {
        Ray_ChangePageComponent * startPageComp = NULL;

        if ( m_linkComponent != NULL )
        {
            if ( ObjectRefList* parents = LINKMANAGER->getParents( m_actor->getRef() ) )
            {
                for (u32 i = 0; i < parents->size(); i++)
                {
                    BaseObject * baseObj = ((*parents)[i]).getObject();

                     if ( baseObj != NULL )
                     {
                         if ( Actor* parent = baseObj->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115)) )
                         {
                             startPageComp = parent->GetComponent<Ray_ChangePageComponent>();

                             if ( startPageComp != NULL )
                             {
                                 break;
                             }                            
                         }
                     }
                }
            }
        }
        return startPageComp;
    }

    //--------------------------------------------------------------------------------------------------------
    void Ray_ChangePageComponent::setShut(bbool _isShut) 
    {
        m_isShut = _isShut;
    }

	//--------------------------------------------------------------------------------------------------------
	void Ray_ChangePageComponent::processChangePageEvent( Ray_EventChangePage * _evt )
	{
        if (m_isShut || (RAY_GAMEMANAGER->isTimeAttack() && isCageMapDoor()))
            return;

        //********************
        // Page seq finished
        //********************
        if ( _evt->isFinished() )
        {
            if ( getTemplate()->isWarpZoneExit() && ( getDestinationPageComponent() == NULL ) )
            {
                EventShow show( 0.0f, getTemplate()->getWarpZoneExitArrivalFadeOutDuration() );
                m_actor->onEvent(&show);
            }

            closePageDoors();
            return;
        }

        Vec2d refPos = _evt->getRefPos().Rotate( m_actor->getAngle() );			// "ref pos" is the local position of activator in the trigger, put it in global
        Actor* activator = AIUtils::getActor(_evt->getActivator());

        if ( !activator )
        {
            return;
        }
        
        Ray_ChangePageComponent * destPageComp = getDestinationPageComponent();
        ActorRef destPageRef;
        f32 waitPlayerDuration  = getTemplate()->getWaitPlayersDuration();
        bbool fadeInOut         = getTemplate()->getUseFadeOutIn();
        
        // is a warp zone
        //-----------------
        if ( isWarpZone() )
        {
            if ( ( getStartPageComponent() == NULL ) && ( getDestinationPageComponent() == NULL ) )
            {
                ITF_WARNING( m_actor, !isWarpZone(), "Warp Page must be linked to an other page actor !" );
            }
        }

        //----------------
        // WARP exit CASE
        //----------------
        if ( getTemplate()->isWarpZoneExit() )
        {
            // warp zone exit enter
            if ( destPageComp != NULL )
            {
                m_computedExitType = ExitType_None;
                m_computedEnterPoint = m_computedExitPoint = m_computedFinalPoint = m_actor->get2DPos(); 
                m_computedExitPointValid = m_computedFinalPointValid = m_computedFinalPointInWater = btrue;
                destPageRef = destPageComp->GetActor()->getRef();
                fadeInOut = btrue;

                // ask actor show
                EventShow show( 1.0f, 0.0f );
                destPageComp->GetActor()->onEvent(&show);
            }
            // warp zone exit arrival (do nothing, can return)
            else
            {
                return;
            }
        }
        else
        {
            // check ONE way
            //------------------
            if ( getTemplate()->isOnWay() )
            {
                Vec2d checkDir = Vec2d::Right;

                if ( m_actor->isFlipped() )
                {
                    checkDir *= -1.f;
                }

                if ( checkDir.dot( _evt->getRefPos() ) > 0.0 )
                {
                    return;
                }
            }

            // IS ALONE
            //----------
            if ( destPageComp == NULL )
            {
                m_computedExitType = ComputePoints( -refPos, m_computedEnterPoint, m_computedExitPoint, m_computedFinalPoint, m_computedExitPointValid, m_computedFinalPointValid, m_computedFinalPointInWater );
            }
            // Has a destination page actor
            //-------------------------------
            else
            {
                // Enter point computing only, exit point will be requested on dest page by the gamemanager
                m_computedExitType = ComputePoints( -refPos, m_computedEnterPoint, m_computedExitPoint, m_computedFinalPoint, m_computedExitPointValid, m_computedFinalPointValid, m_computedFinalPointInWater, btrue );
                destPageRef = destPageComp->GetActor()->getRef();
                m_computedExitPoint = m_actor->get2DPos();

                // destination page is a warp, then save the enter point
                if ( destPageComp->isWarpZone() )
                {
                    waitPlayerDuration = destPageComp->getWaitPlayersDuration();
                    fadeInOut = btrue;
                }
                else if ( isWarpZone() )
                {
                    fadeInOut = btrue;
                }
            }
        }

        ActorRef checkpointRef = getActivateCheckpoint();
        bbool isCageMap = getTemplate()->isCageMapDoor();
        bbool verticalEject = ( m_computedExitType == ExitType_Up ) && !m_computedFinalPointInWater;
        Ray_EventTriggerChangePage triggerChangePageEvent(m_actor->getRef(),m_computedEnterPoint.to3d(),m_computedExitPoint.to3d(),
            m_computedFinalPoint.to3d(),verticalEject,destPageRef,checkpointRef,waitPlayerDuration,fadeInOut, isCageMap);
        activator->onEvent(&triggerChangePageEvent);

        if ( m_fxControllerComponent != NULL )
        {
            u32 fxHandle = m_fxControllerComponent->playFX( ITF_GET_STRINGID_CRC(Enter,2000132787) );
            m_fxControllerComponent->setFXPosFromHandle( fxHandle, m_actor->getPos() );
        }
	}

    //--------------------------------------------------------------------------------------------------------
    void Ray_ChangePageComponent::updateElectoonSound(f32 _dt)
    {
        if (!getTemplate()->isCageMapDoor())
        {
            return;
        }

        PackedObjectPath packedPath(m_actor);

        m_remainingTimeToRestartElectoonSound -= _dt;
        if (m_remainingTimeToRestartElectoonSound<0)
        {
            //We may need to reboot sound
            if (m_currentElectoonSoundHandle!=U32_INVALID)
            {
                m_fxControllerComponent->stopFXFromHandle(m_currentElectoonSoundHandle);
                m_currentElectoonSoundHandle = U32_INVALID;
            }
        }

        bbool mustPlaySound = RAY_GAMEMANAGER->didPassDoorToCageMap(packedPath) == bfalse   ;
        bbool isPlayingSound = m_currentElectoonSoundHandle!=U32_INVALID;

        if (mustPlaySound != isPlayingSound)
        {
            ITF_WARNING_CATEGORY(LD,m_actor,m_fxControllerComponent,"Trying to play FX Electoons_Help but there is no FXControllerComponent on the actor. Sound won't be played");

            if ( m_fxControllerComponent )
            {
                if (mustPlaySound)
                {
                    m_currentElectoonSoundHandle = m_fxControllerComponent->playFX(ITF_GET_STRINGID_CRC(Electoons_Help,1565634733));
                    m_remainingTimeToRestartElectoonSound = getTemplate()->getElectoonHelpPeriod();
                }
                else
                {
                    m_fxControllerComponent->stopFXFromHandle(m_currentElectoonSoundHandle);
                    m_currentElectoonSoundHandle = U32_INVALID;
                }
            }
        }

    }

    //--------------------------------------------------------------------------------------------------------
    ActorRef Ray_ChangePageComponent::getActivateCheckpoint() const
    {
        if ( m_linkComponent != NULL )
        {
            const LinkComponent::ChildrenList& children = m_linkComponent->getChildren();
            u32 numChildren = children.size();

            for ( u32 i = 0; i < numChildren; i++ )
            {
                const ChildEntry& child = children[i];

                if ( child.hasTag(ITF_GET_STRINGID_CRC(activateCheckpoint,1465678949)) )
                {
                    const ObjectPath& path = child.getPath();
                    Pickable* target;

                    if ( path.getIsAbsolute() )
                    {
                        target = SceneObjectPathUtils::getObjectFromAbsolutePath(path);
                    }
                    else
                    {
                        target = SceneObjectPathUtils::getObjectFromRelativePath(m_actor,path);
                    }

                    if ( target )
                    {
                        return target->getRef();
                    }

                    break;
                }
            }
        }

		return ObjectRef::InvalidRef;
    }

    //--------------------------------------------------------------------------------------------------------
    void Ray_ChangePageComponent::processChangePageQueryPointsEvent( Ray_EventChangePageQueryPoints * _evt )
    {
        Vec2d tmp;
        Vec2d enterDir = Vec2d::Right.Rotate( m_actor->getAngle());
        
        if ( !m_actor->isFlipped() )
        {
            enterDir *= -1.f;
        }

        m_computedExitType = ComputePoints( enterDir, tmp, m_computedExitPoint, m_computedFinalPoint, m_computedExitPointValid, m_computedFinalPointValid, m_computedFinalPointInWater );
        _evt->setExitPoint(m_computedExitPoint.to3d());
        _evt->setFinalPoint(m_computedFinalPoint.to3d());
        _evt->setFinalPointNeedEject( ( m_computedExitType == ExitType_Up ) && !m_computedFinalPointInWater );
    }

    //--------------------------------------------------------------------------------------------------------
    void Ray_ChangePageComponent::processEventTrigger( EventTrigger * _evt )
    {
        // not trigger by me
        //
        if ( _evt->getSender() != m_actor->getRef() )
        {
            if ( _evt->getActivated() )
            {
                m_actor->enable();
                m_warpInitialized = btrue;
            }
            else if ( !_evt->getActivated() )
            {
                m_actor->disable();
            }
        }
    }

    //--------------------------------------------------------------------------------------------------------
    Vec2d Ray_ChangePageComponent::getEnterPoint( f32 _sign, const PhysShapeBox * _triggerShape, const Vec2d& _actorDir ) const
    {
		if ( _triggerShape == NULL )
		{
			return Vec2d::Zero;
		}
		f32 extend = _triggerShape->getExtent().m_x;
	/*	if ( _isLeftRight )*/
		{
			if (  ( _sign * _actorDir).dot( Vec2d::Right ) > 0.0f )
			{
				extend += ( getTemplate()->getExitRightSideOffset() * getActorScale().m_x );
			}
			else
			{
				extend += ( getTemplate()->getExitLeftSideOffset() * getActorScale().m_x );
			}
		}		

        return ( _sign * _actorDir * extend ) + m_actor->get2DPos();
    }

    //--------------------------------------------------------------------------------------------------------
    Vec2d Ray_ChangePageComponent::getExitPoint( f32 _sign, f32 _extent, const Vec2d& _actorDir ) const
    {
        return ( -_sign * _actorDir * _extent ) + m_actor->get2DPos();
    }

    //--------------------------------------------------------------------------------------------------------
    Vec2d Ray_ChangePageComponent::getFinalPoint( f32 _sign, const PhysShapeBox * _triggerShape, const Vec2d& _actorDir ) const
    {
		if ( _triggerShape == NULL )
		{
			return Vec2d::Zero;
		}
		f32 extend = 0.0f;
		if (  ( -_sign * _actorDir).dot( Vec2d::Right ) > 0.0f )
		{
			extend = ( getTemplate()->getExitRightSideOffset() * getActorScale().m_x ) + _triggerShape->getExtent().m_x;
		}
		else
		{
			extend = ( getTemplate()->getExitLeftSideOffset() * getActorScale().m_x ) + _triggerShape->getExtent().m_x;
		}		

        return getExitPoint( _sign, extend, _actorDir );
    }


    //--------------------------------------------------------------------------------------------------------
    Vec2d Ray_ChangePageComponent::getActorScale() const
    {
        if ( getTemplate()->isWarpZoneExit() )
        {
            return ( m_actor->getScale() * getTemplate()->getWarpZoneExitScaleFactor() );
        }
        return m_actor->getScale();
    }

	////--------------------------------------------------------------------------------------------------------
	void  Ray_ChangePageComponent::Update( f32 _dt )
	{
        if ( m_enableDelayCounter )
        {
            m_enableDelayCounter -= _dt;

            if ( m_enableDelayCounter <= 0.f )
            {
                m_enableDelayCounter = 0.f;
                m_shapeDetectorComponent->setDisabled(bfalse);
            }
        }

        if ( getTemplate()->isWarpZoneExit() && !m_warpInitialized )
        {
            PhysShapeBox* triggerShape = static_cast<PhysShapeBox*>( m_shapeDetectorComponent->getCurrentShape() );
            if ( triggerShape != NULL )
            {
                m_warpInitialized = btrue;
                m_actor->disable();
            }
        }

        updatePageDoors(_dt);
        updateOpening(_dt);
        updateElectoonSound(_dt);
	}

    //--------------------------------------------------------------------------------------------------------
    void Ray_ChangePageComponent::updatePageDoors( f32 _dt )
    {
        if ( m_pageDoorsOpened )
        {
            return;
        }

        // If they are not opened, check if one of them is opened
        if ( m_linkComponent != NULL )
        {
            FixedArray <Actor*,4> pageDoors;
            const LinkComponent::ChildrenList& children = m_linkComponent->getChildren();
            u32 numChildren = children.size();
            bbool openDoors = bfalse;

            for ( u32 i = 0; i < numChildren; i++ )
            {
                const ChildEntry& child = children[i];

                if ( child.hasTag(ITF_GET_STRINGID_CRC(DoorLink,2094447933)) )
                {
                    const ObjectPath& path = child.getPath();
                    Pickable* target;

                    if ( path.getIsAbsolute() )
                    {
                        target = SceneObjectPathUtils::getObjectFromAbsolutePath(path);
                    }
                    else
                    {
                        target = SceneObjectPathUtils::getObjectFromRelativePath(m_actor,path);
                    }

                    if ( target )
                    {
                        Actor* pageDoor = target->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));

                        if ( pageDoor )
                        {
                            if ( !pageDoors.full() )
                            {
                                pageDoors.push_back(pageDoor);
                            }

                            if ( !openDoors )
                            {
                                Ray_EventBreakableQuery query;
                                
                                pageDoor->onEvent(&query);

                                if ( query.getIsBroken() )
                                {
                                    openDoors = btrue;
                                }
                            }
                        }
                    }
                }
            }

            if ( openDoors )
            {
                // one of the doors is open, so open all of them
                u32 numDoors = pageDoors.size();
                Ray_EventBreakableBreak breakEvent(m_actor->getRef(),btrue);

                for ( u32 i = 0; i < numDoors; i++ )
                {
                    pageDoors[i]->onEvent(&breakEvent);
                }

                m_pageDoorsOpened = btrue;
            }
        }
    }

    //--------------------------------------------------------------------------------------------------------
    void Ray_ChangePageComponent::closePageDoors()
    {
        if ( !m_pageDoorsOpened )
        {
            return;
        }

        // If they are not opened, check if one of them is opened
        if ( m_linkComponent != NULL )
        {
            const LinkComponent::ChildrenList& children = m_linkComponent->getChildren();
            u32 numChildren = children.size();
            Ray_EventBreakableBreak breakEvent(m_actor->getRef(),bfalse);

            for ( u32 i = 0; i < numChildren; i++ )
            {
                const ChildEntry& child = children[i];

                if ( child.hasTag(ITF_GET_STRINGID_CRC(DoorLink,2094447933)) )
                {
                    const ObjectPath& path = child.getPath();
                    Pickable* target;

                    if ( path.getIsAbsolute() )
                    {
                        target = SceneObjectPathUtils::getObjectFromAbsolutePath(path);
                    }
                    else
                    {
                        target = SceneObjectPathUtils::getObjectFromRelativePath(m_actor,path);
                    }

                    if ( target )
                    {
                        Actor* pageDoor = target->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));

                        if ( pageDoor )
                        {
                            pageDoor->onEvent(&breakEvent);
                        }
                    }
                }
            }
        }

        m_pageDoorsOpened = bfalse;
    }

    //--------------------------------------------------------------------------------------------------------
    void Ray_ChangePageComponent::updateOpening( f32 _dt )
    {
        if (m_isShut || (RAY_GAMEMANAGER->isTimeAttack() && isCageMapDoor()))
        {
            m_opening = bfalse;
            m_openingTime = 0;
        }
        else if ( m_openingFrameCheck == CURRENTFRAME &&
             m_opening )
        {
            // fine, the other door is opening, so we just take its time
            m_openingTime = m_otherDoorTime;
        }
        else
        {
            const u32 uPlayerCount = GAMEMANAGER->getMaxPlayerCount();
            const DepthRange depthRange(m_actor->getDepth());
            f32 maxDistSq = getTemplate()->getOpeningRadius();
            
            m_opening = bfalse;

            maxDistSq *= maxDistSq;

            for (u32 i = 0; i < uPlayerCount; ++i)
            {
                Actor* pActor = GAMEMANAGER->getActivePlayer(i);

                if ( !pActor || depthRange != DepthRange(pActor->getDepth()) )
                {
                    continue;
                }

                f32 dist = ( pActor->get2DPos() - m_actor->get2DPos() ).sqrnorm();

                if ( dist > maxDistSq )
                {
                    continue;
                }

                EventQueryIsDead query;
                pActor->onEvent(&query);
                if (query.getIsDead())
                {
                    continue;
                }

                m_opening = btrue;
                break;
            }

            if ( m_openingFrameCheck == CURRENTFRAME )
            {
                if ( m_opening )
                {
                    // The other door is not opening, but we are, so we set the other to open
                    m_openingTime = Min(m_openingTime+_dt,getTemplate()->getOpeningTime());

                    Ray_ChangePageComponent * destPage = getDestinationPageComponent();

                    if ( destPage )
                    {
                        destPage->setOpeningFromOtherDoor(m_opening,m_openingTime);
                    }
                }
                else
                {
                    // None of us are opening, and since the other door updated first, I take his time
                    m_openingTime = m_otherDoorTime;
                }
            }
            else
            {
                // We are the first door in updating, so we update us and set the time on the other door
                m_openingFrameCheck = CURRENTFRAME;

                if ( m_opening )
                {
                    m_openingTime = Min(m_openingTime+_dt,getTemplate()->getOpeningTime());
                }
                else
                {
                    m_openingTime = Max(m_openingTime-_dt,0.f);
                }

                Ray_ChangePageComponent * destPage = getDestinationPageComponent();

                if ( destPage )
                {
                    destPage->setOpeningFromOtherDoor(m_opening,m_openingTime);
                }
            }
        }
    }

    //--------------------------------------------------------------------------------------------------------
    void Ray_ChangePageComponent::setOpeningFromOtherDoor( bbool _openState, f32 _openingTime )
    {
        if ( m_openingFrameCheck == CURRENTFRAME )
        {
            // The other door is changing our state after we had updated, this must mean we have to open
            m_opening = _openState;
            m_openingTime = _openingTime;
        }
        else
        {
            // We haven't updated yet, so if the other door says we open, we open with that data. If not, we will see in our update
            m_openingFrameCheck = CURRENTFRAME;
            m_opening = _openState;
            m_otherDoorTime = _openingTime;
        }
    }

    //--------------------------------------------------------------------------------------------------------
    void Ray_ChangePageComponent::updateAnimInput()
    {
        f32 t = m_openingTime/getTemplate()->getOpeningTime();

        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(Opening,133555280),t);
    }

#ifdef ITF_SUPPORT_EDITOR
	//--------------------------------------------------------------------------------------------------------
	void Ray_ChangePageComponent::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
	{
		Super::drawEdit(_drawInterface, _flags);

		// compute points
		//
        Vec2d actorDir = Vec2d::Right.Rotate( m_actor->getAngle() ) * ( m_actor->isFlipped() ? -1.f : 1.f );
		Vec2d enterPoint = m_computedEnterPoint; 
		Vec2d exitPoint = m_computedExitPoint;
		Vec2d finalPoint = m_computedFinalPoint;
		bbool exitPointValid = m_computedExitPointValid;
		bbool finalPointValid = m_computedFinalPointValid;
        bbool finalPointinWater = m_computedFinalPointInWater;
		ExitType exitType = m_computedExitType;
        Vec2d enterDir = getTemplate()->isOnWay() ? actorDir : exitPoint - enterPoint;
		Vec3d res2d;
		Color pointFdkColor = Color::blue();

		// Get the closest  player
		//
		Actor * closestPlayer = AIUtils::getClosestLivePlayer( m_actor );
		if ( ( closestPlayer != NULL ) && !getTemplate()->isOnWay() )
		{
		    enterDir = m_actor->get2DPos() - closestPlayer->get2DPos();
        }
        
        // destination page ?
        Ray_ChangePageComponent * destPage = getDestinationPageComponent();

        // is warp exit ??
        if ( getTemplate()->isWarpZoneExit() )
        {
            enterPoint = m_actor->get2DPos();
            enterPoint.m_y -= 0.25f;
            GFX_ADAPTER->compute3DTo2D( enterPoint.to3d(m_actor->getPos().m_z), res2d);
            if ( destPage != NULL )
            {
                GFX_ADAPTER->drawDBGText( "WARP Exit - Enter", res2d.m_x, res2d.m_y );
                return;
            }
            else
            {
                GFX_ADAPTER->drawDBGText( "WARP Exit - Arrival", res2d.m_x, res2d.m_y );
            }            
        }

        Vec2d destPageDir = Vec2d::Right;

        if ( ( destPage != NULL ) && destPage->isActive() )
        {
            destPageDir = Vec2d::Right.Rotate( destPage->GetActor()->getAngle() );
            Vec2d tmp;
            ComputePoints( enterDir, enterPoint, exitPoint, finalPoint, exitPointValid, finalPointValid, finalPointinWater, btrue );
            exitType = destPage->ComputePoints( destPageDir, tmp, exitPoint, finalPoint, exitPointValid, finalPointValid, finalPointinWater );
        }
        else
        {
            exitType = ComputePoints( enterDir, enterPoint, exitPoint, finalPoint, exitPointValid, finalPointValid, finalPointinWater );
        }        
		
		f32 dotSign = getSign( enterDir.dot( actorDir ) );
		PhysShapeBox* curShape = static_cast<PhysShapeBox*>( m_shapeDetectorComponent->getCurrentShape() );
		Vec2d finalPos = getFinalPoint( -dotSign, curShape, actorDir );
		Vec2d enterPos = getEnterPoint( -dotSign, curShape, actorDir );
		
		// Display left/right zone
		//
		if ( ( exitType != ExitType_Up ) && ( exitType != ExitType_Down ) )
		{
			//DebugDraw::shape( m_actor->getPos(), m_actor->getAngle(), &m_exitShape, Color::green() );
			if ( curShape != NULL )
			{	
				Vec2d toFinalPosVec = finalPos - m_actor->get2DPos();
				toFinalPosVec.normalize();
				DebugDraw::line2D( finalPos + ( toFinalPosVec.getPerpendicular() * curShape->getExtent().m_y ), finalPos - ( toFinalPosVec.getPerpendicular() * curShape->getExtent().m_y ), m_actor->getDepth(), Color::green(), 2.0f );
                if ( !getTemplate()->isWarpZoneExit() )
                {
				    DebugDraw::line2D( enterPos + ( toFinalPosVec.getPerpendicular() * curShape->getExtent().m_y ), enterPos - ( toFinalPosVec.getPerpendicular() * curShape->getExtent().m_y ), m_actor->getDepth(), Color::green(), 2.0f );
                }
			}
		}

		// display enter point
		//
        if ( !getTemplate()->isWarpZoneExit() )
        {
		    GFX_ADAPTER->compute3DTo2D( enterPoint.to3d(m_actor->getPos().m_z), res2d);
		    GFX_ADAPTER->drawDBGText( "Enter Point", res2d.m_x, res2d.m_y );
		    DebugDraw::circle( enterPoint, m_actor->getDepth(), 0.2f, pointFdkColor, 0.0f );
        }

		// Display big arrows
		//
		Vec2d endPos = m_actor->get2DPos() + actorDir;
        Color actorDirColor  = ( exitPointValid && finalPointValid ) ? Color::blue() : Color::red();
        Color actorInvDirColor  = ( exitPointValid && finalPointValid ) ? Color::green() : Color::red();

        if ( getStartPageComponent() == NULL )
        {
            actorDirColor = actorInvDirColor;
        }
        GFX_ADAPTER->drawArrow(m_actor->getPos(), endPos.to3d(m_actor->getPos().m_z), actorDirColor, 0.8f);
        if ( !getTemplate()->isOnWay() )        
        {
            GFX_ADAPTER->drawArrow(m_actor->getPos(), (m_actor->get2DPos() - actorDir).to3d(m_actor->getPos().m_z), actorInvDirColor, 0.8f);
        }

        if ( ( destPage != NULL ) && destPage->isActive() )
        {
            actorDirColor  = ( exitPointValid && finalPointValid ) ? Color::blue() : Color::red();
            GFX_ADAPTER->drawArrow( destPage->GetActor()->getPos(), ( destPage->GetActor()->get2DPos() + destPageDir ).to3d(destPage->GetActor()->getPos().m_z), actorDirColor, 0.8f);
        }

		// Draw exit / final points
		if ( exitPointValid )
		{	
			if ( ( exitType != ExitType_Up ) && ( exitType != ExitType_Down ) )
			{
				DebugDraw::arrow2D( exitPoint + Vec2d( 0.0, 1.0f ), exitPoint, m_actor->getDepth(), pointFdkColor, 5.0, 30.0, 40.0 );
			}
			else
			{
				DebugDraw::circle( exitPoint, m_actor->getDepth(), 0.2f, pointFdkColor, 0.0f );
			}
			GFX_ADAPTER->compute3DTo2D(exitPoint.to3d(m_actor->getPos().m_z), res2d);
			GFX_ADAPTER->drawDBGText( "Exit Point", res2d.m_x, res2d.m_y );
		}
		else
		{
			DebugDraw::circle( exitPoint, m_actor->getDepth(), 0.8f, Color::red(), 0.0f );
			DebugDraw::line2D( exitPoint - Vec2d( 0.5f, 0.5f ), exitPoint + Vec2d( 0.5f, 0.5f ), m_actor->getDepth(), Color::red(), 2.0f );
			DebugDraw::line2D( exitPoint - Vec2d( 0.5f, -0.5f ), exitPoint + Vec2d( 0.5f, -0.5f ), m_actor->getDepth(), Color::red(), 2.0f );

			GFX_ADAPTER->compute3DTo2D(exitPoint.to3d(m_actor->getPos().m_z), res2d);
			GFX_ADAPTER->drawDBGText( "INVALID Exit Point", res2d.m_x, res2d.m_y );
		}

		if ( finalPointValid )
		{
			DebugDraw::arrow2D( finalPoint + Vec2d( 0.0, 1.0f ), finalPoint, m_actor->getDepth(), pointFdkColor, 5.0, 30.0, 40.0 );

			GFX_ADAPTER->compute3DTo2D( finalPoint.to3d(m_actor->getPos().m_z), res2d );
			GFX_ADAPTER->drawDBGText("Final Point", res2d.m_x, res2d.m_y );
		}
		else
		{
			DebugDraw::circle( finalPoint, m_actor->getDepth(), 0.8f, Color::red(), 0.0f );
			DebugDraw::line2D( finalPoint - Vec2d( 0.5f, 0.5f ), finalPoint + Vec2d( 0.5f, 0.5f ), m_actor->getDepth(), Color::red(), 2.0f );
			DebugDraw::line2D( finalPoint - Vec2d( 0.5f, -0.5f ), finalPoint + Vec2d( 0.5f, -0.5f ), m_actor->getDepth(), Color::red(), 2.0f );

			GFX_ADAPTER->compute3DTo2D( finalPoint.to3d(m_actor->getPos().m_z), res2d );
			GFX_ADAPTER->drawDBGText("INVALID Final Point", res2d.m_x, res2d.m_y );
		}

        DebugDraw::circle( m_finalDefaultPos, m_actor->getDepth(), 0.5f, Color::red(), 0.0f );
	}
#endif // ITF_SUPPORT_EDITOR

	//--------------------------------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------------------------------
	//--------------------------------------------------------------------------------------------------------
	Ray_ChangePageComponent_Template::Ray_ChangePageComponent_Template()
	//: m_exitZoneOffset( 1.0f )
    : m_waitPlayerDuration( 0.f )
    , m_useFadeOutIn( bfalse )
    , m_warpZone( bfalse )
    , m_warpZoneExit( bfalse )
    , m_warpZoneExitScaleFactor( 1.f, 1.f )
    , m_warpZoneExitArrivalFadeOutDuration( 4.0f )
    , m_openingRadius(0.f)
    , m_openingTime(0.f)
    , m_enableDelay(0.f)
    , m_exitUpSideOffset( 1.5f )
    , m_exitDownSideOffset( 1.5f )
    , m_exitUpLeftSide( -1 )
    , m_exitDownLeftSide( -1 )
    , m_exitLeftSideOffset( 6.0f )
    , m_exitRightSideOffset( 6.0f )
    , m_oneWay( bfalse )
    , m_isCageMapDoor( bfalse )
    , m_electoonHelpPeriod(2.f)
	{
	}	

    IMPLEMENT_OBJECT_RTTI(Ray_ChangePageComponent_Template)
	BEGIN_SERIALIZATION(Ray_ChangePageComponent_Template)
        SERIALIZE_MEMBER( "waitPlayersDuration",                m_waitPlayerDuration );
        SERIALIZE_MEMBER( "useFadeOutIn",                       m_useFadeOutIn );
        SERIALIZE_MEMBER( "warpZone",                           m_warpZone );
        SERIALIZE_MEMBER( "warpZoneExit",                       m_warpZoneExit );
        SERIALIZE_MEMBER( "warpZoneExitScaleFactor",            m_warpZoneExitScaleFactor );
        SERIALIZE_MEMBER( "warpZoneExitArrivalFadeOutDuration", m_warpZoneExitArrivalFadeOutDuration );
        SERIALIZE_MEMBER( "openingRadius",                      m_openingRadius );
        SERIALIZE_MEMBER( "openingTime",                        m_openingTime );
        SERIALIZE_MEMBER( "enableDelay",                        m_enableDelay );
        SERIALIZE_MEMBER( "ExitUpSideOffset",	                m_exitUpSideOffset);
        SERIALIZE_MEMBER( "ExitUpLeftSide",		                m_exitUpLeftSide);
        SERIALIZE_MEMBER( "ExitDownSideOffset",	                m_exitDownSideOffset);
        SERIALIZE_MEMBER( "ExitDownLeftSide",	                m_exitDownLeftSide);
        SERIALIZE_MEMBER( "ExitLeftSideOffset",	                m_exitLeftSideOffset);
        SERIALIZE_MEMBER( "ExitRightSideOffset",                m_exitRightSideOffset);
        SERIALIZE_MEMBER( "OneWay",                             m_oneWay);
        SERIALIZE_MEMBER( "isCageMapDoor",                      m_isCageMapDoor);
        SERIALIZE_MEMBER( "electoonHelpPeriod",                 m_electoonHelpPeriod);
    END_SERIALIZATION()


} // namespace ITF
