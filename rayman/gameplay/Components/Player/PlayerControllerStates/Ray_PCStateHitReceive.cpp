#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_
#include "rayman/gameplay/Components/Player/Ray_PlayerControllerComponent.h"
#endif //_ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_

#ifndef _ITF_ID_SERVER_H_
#include "core/IDserver.h"
#endif //_ITF_ID_SERVER_H_

#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_

#ifndef _ITF_PHYSBODY_H_
#include "engine/physics/PhysBody.h"
#endif //_ITF_PHYSBODY_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_ANIMATIONTRACK_H_
#include "engine/animation/AnimationTrack.h"
#endif //_ITF_ANIMATIONTRACK_H_

#ifndef _ITF_RAY_ANIMATIONMARKERS_H_
#include "rayman/gameplay/Ray_AnimationMarkers.h"
#endif //_ITF_RAY_ANIMATIONMARKERS_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_RAY_POWERUPNAMES_H_
#include "rayman/gameplay/Ray_PowerUpNames.h"
#endif //_ITF_RAY_POWERUPNAMES_H_

#ifndef ITF_RAYONLINETRACKING_MANAGER_H
	#include "rayman/gameplay/Ray_OnlineTrackingManager.h"
#endif

#ifndef _ITF_ONLINETRACKINGADAPTER_H
#include "engine/AdaptersInterfaces/OnlineTrackingAdapter.h"
#endif //_ITF_ONLINETRACKINGADAPTER_WIN32_H

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_VERSIONING_H_
#include "core/versioning.h"
#endif //_ITF_VERSIONING_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_CONFIG_H_
#include "core/Config.h"
#endif //_ITF_CONFIG_H_


namespace ITF
{

IMPLEMENT_OBJECT_RTTI_SUBCLASS(Ray_PlayerControllerComponent,StateHitReceive)

////////////////////
// Receive Hit State
////////////////////
Ray_PlayerControllerComponent::StateHitReceive::StateHitReceive()
: m_blockedPolyline(ITF_INVALID_OBJREF)
, m_blockedEdgeIndex(U32_INVALID)
, m_currentSpeed(Vec2d::Zero)
, m_justEntered(bfalse)
, m_canMove(bfalse)
, m_triggerDeath(bfalse)
, m_wasBlocked(bfalse)
, m_appexStart(bfalse)
, m_appexStop(bfalse)
, m_appex(bfalse)
{
}

void Ray_PlayerControllerComponent::StateHitReceive::onEnter()
{
    Super::onEnter();

    m_parent->cancelPhysicAirHit();
    m_parent->stopOnPedestal();

    if ( m_parent->m_stance != STANCE_SWIM )
    {
        m_parent->setStance(STANCE_STAND);
    }

    m_justEntered = btrue;
    m_triggerDeath = bfalse;
    m_wasBlocked = bfalse;
    m_appex = bfalse;

    setupHit(bfalse);

    // reset super punches (RO-11294)
    // + only if taking damage (RO-14106)
    if (m_parent->m_receivedHitDamage > 0)
    {
        RAY_GAMEMANAGER->getPowerUpManager().resetVolatilePerPlayerData(btrue);
    }
}

void Ray_PlayerControllerComponent::StateHitReceive::onExit()
{
    m_parent->resetGravityMode(GRAVITYMODE_RECEIVEHIT);
    m_parent->resetFrictionMode(FRICTIONMODE_RECEIVEHIT);
}

void Ray_PlayerControllerComponent::StateHitReceive::setupHit( bbool _restart )
{
    m_parent->receiveDamage(m_parent->m_receivedHitDamage);
    m_canMove = bfalse;
    m_appexStart = bfalse;
    m_appexStop = bfalse;

    switch(m_parent->m_receivedHitType)
    {
        case RECEIVEDHITTYPE_UPPUNCH:
        case RECEIVEDHITTYPE_EJECTY:
            setupPunchUp(_restart);
            break;
        case RECEIVEDHITTYPE_HURTBOUNCE:
            setupHurtBounce(_restart);
            break;
        case RECEIVEDHITTYPE_EARTHQUAKE:
            setupEarthquake(_restart);
            break;
        default:
            setupPunchFront(_restart);
            break;
    }

    m_parent->resetBounce();

#ifdef ITF_SUPPORT_ONLINETRACKING


	if (RAY_GAMEMANAGER->getOnlineTrackingManager())
		{

		u32 num_players = GAMEMANAGER->getNumActivePlayers();

		if (num_players==2)
		{
			RAY_GAMEMANAGER->getOnlineTrackingManager()->m_Paf2Players++;
		}
		else if (num_players==3)
		{
			RAY_GAMEMANAGER->getOnlineTrackingManager()->m_Paf3Players++;
		}
		else if (num_players==4)
		{
			RAY_GAMEMANAGER->getOnlineTrackingManager()->m_Paf4Players++;
		}


		RAY_GAMEMANAGER->getOnlineTrackingManager()->SavePersistant();

	}

	if (CONFIG->m_enableonlineTracking==btrue)
	{

		if (ONLINETRACKING_ADAPTER && !m_parent->isDead() )
		{

			ITF::String8 last_paf,last_offscreen;

			u32 currentEngineVersion = Versioning::getEngineVersion();

			String str,checkpointstr;
            PathString_t levelName;

			if (CURRENTWORLD!=NULL)
			{
					if (CURRENTWORLD->getRootScene()!=NULL)
					{
						CURRENTWORLD->getRootScene()->getPath().getString(levelName);
					}
			}

			Actor* checkpoint = GAMEMANAGER->getCurrentCheckpoint();

			ITF_STDSTRING checkPointStr = "";

			if ( checkpoint )
			{
					ObjectPath path;
					SceneObjectPathUtils::getAbsolutePathFromObject(checkpoint,path);
					path.toString(checkPointStr);
			}

			u32 num_players = GAMEMANAGER->getNumActivePlayers();

			if (RAY_GAMEMANAGER->getOnlineTrackingManager()->m_lastPaf == 0)
			{
				last_paf = "NO";
			}
			else
			{
				last_paf = "YES";
			}

			if (RAY_GAMEMANAGER->getOnlineTrackingManager()->m_lastOffscreen == 0)
			{
				last_offscreen = "NO";
			}
			else
			{
				last_offscreen = "YES";
			}

			str.setTextFormat("VERSION=%d&NUMPLAYERS=%d&PLAYERID=%d&LEVELNAME=%s&CHECKPOINT=%s&DEATH=NO&OFFSCREEN=%s&PAF=%s&X=%.1f&Y=%.1f&Z=%.1f",
				currentEngineVersion,
				num_players,
				m_parent->m_playerIndex+1,
                StringToUTF8(levelName).get(),
				checkPointStr.c_str(),
				last_offscreen.cStr(),
				last_paf.cStr(),
				m_parent->m_actor->getPos().getX(),
				m_parent->m_actor->getPos().getY(),
				m_parent->m_actor->getPos().getZ() );

			StringConverter c(str);

            // old api.
            //ONLINETRACKING_ADAPTER->sendTag("PLAYER_HIT",c.getChar());

            auto trk = ONLINETRACKING_ADAPTER;
            if (trk)
            {
                trk->setAttributeString("checkPoint", checkPointStr.c_str());
                trk->setAttributeString("levelName", StringToUTF8(levelName).get());
                trk->setAttributeInt("numPlayers", num_players);
                trk->setAttributeString("offscreen", last_offscreen.cStr());
                trk->setAttributeString("paf", last_paf.cStr());
                trk->setAttributeInt("playerId", m_parent->m_playerIndex + 1);
                trk->setAttributeFloat("positionX", m_parent->m_actor->getPos().getX());
                trk->setAttributeFloat("positionY", m_parent->m_actor->getPos().getY());
                trk->setAttributeFloat("positionZ", m_parent->m_actor->getPos().getZ());

                trk->sendSignal("PlayerDeath");
            }

			RAY_GAMEMANAGER->getOnlineTrackingManager()->onHitExit(); // reset paf/offscreen//
		}
	}
#endif // ITF_SUPPORT_ONLINETRACKING

}

void Ray_PlayerControllerComponent::StateHitReceive::setupPunchFront( bbool _restart )
{
    f32 force = 0;
    f32 friction = 1.f;

    switch(m_parent->m_receivedHitLevel)
    {
    case 0:
        force = m_parent->getTemplate()->getReceivePunchFrontWeakForce();
        friction = m_parent->getTemplate()->getReceivePunchFrontWeakFrictionMultiplier();
        break;
    case 1:
        force = m_parent->getTemplate()->getReceivePunchFrontStrongForce();
        friction = m_parent->getTemplate()->getReceivePunchFrontStrongFrictionMultiplier();
        break;
    default:
        force = m_parent->getTemplate()->getReceivePunchFrontMegaForce();
        friction = m_parent->getTemplate()->getReceivePunchFrontMegaFrictionMultiplier();
        break;
    }

    Vec2d hitDir;
    
    if ( m_parent->m_stance == STANCE_SWIM )
    {
        hitDir = m_parent->m_receivedHitDir;
    }
    else
    {
        Vec2d lookDir = AIUtils::getLookDir(m_actor,m_actor->isFlipped());

        if ( m_parent->m_receivedHitDir.dot(lookDir) > 0.f )
        {
            lookDir *= -1.f;
            m_parent->setLookRight(m_actor->isFlipped());
            m_parent->resetDesiredLookDir();
        }

        hitDir = -lookDir;
    }

    // Cancel our speed
    m_characterPhys->setSpeed(Vec2d::Zero);

    // Add the force based on the hit level
    m_characterPhys->addForce(hitDir*force);
    m_parent->setFrictionMode(FRICTIONMODE_RECEIVEHIT,friction);

    if ( _restart )
    {
        m_animComponent->resetTree();
    }
}

void Ray_PlayerControllerComponent::StateHitReceive::setupPunchUp( bbool _restart )
{
    if ( !_restart || m_characterPhys->getSpeed().dot(m_parent->m_currentGravityDir) > 1.f ||
         m_parent->m_receivedHitLevel > 0 )
    {
        f32 force = 0;
        f32 gravity = 1.f;

        switch(m_parent->m_receivedHitLevel)
        {
        case 0:
            force = m_parent->getTemplate()->getReceivePunchUpWeakForce();
            gravity = m_parent->getTemplate()->getReceivePunchUpWeakGravityMultiplier();
            break;
        case 1:
            force = m_parent->getTemplate()->getReceivePunchUpStrongForce();
            gravity = m_parent->getTemplate()->getReceivePunchUpStrongGravityMultiplier();
            break;
        default:
            force = m_parent->getTemplate()->getReceivePunchUpMegaForce();
            gravity = m_parent->getTemplate()->getReceivePunchUpMegaGravityMultiplier();
            break;
        }

        m_characterPhys->setSpeed(Vec2d::Zero);
        m_characterPhys->addForce(-m_parent->m_currentGravityDir*force);
        m_parent->setGravityMode(GRAVITYMODE_RECEIVEHIT,gravity);
        m_appex = bfalse;

        CAMERACONTROLLERMANAGER->setSubjectJustReceivedPunchUp(m_actor->getRef());
    }
    else
    {
        if ( m_appex )
        {
            m_animComponent->resetCurTime();
        }
    }
}

void Ray_PlayerControllerComponent::StateHitReceive::setupHurtBounce( bbool _restart )
{
    f32 friction = 1.f;
    f32 forceH = 0.f;
    f32 forceV = 0.f;
    f32 swimMult = 1.f;

    switch(m_parent->m_receivedHitLevel)
    {
    case 0:
        friction = m_parent->getTemplate()->getReceiveBounceWeakFrictionMult();
        forceH = m_parent->getTemplate()->getReceiveBounceHWeakForce();
        forceV = m_parent->getTemplate()->getReceiveBounceVWeakForce();
        swimMult = m_parent->getTemplate()->getReceiveBounceWeakSwimingMult();
        break;
    case 1:
        friction = m_parent->getTemplate()->getReceiveBounceStrongFrictionMult();
        forceH = m_parent->getTemplate()->getReceiveBounceHStrongForce();
        forceV = m_parent->getTemplate()->getReceiveBounceVStrongForce();
        swimMult = m_parent->getTemplate()->getReceiveBounceStrongSwimingMult();
        break;
    default:
        friction = m_parent->getTemplate()->getReceiveBounceMegaFrictionMult();
        forceH = m_parent->getTemplate()->getReceiveBounceHMegaForce();
        forceV = m_parent->getTemplate()->getReceiveBounceVMegaForce();
        swimMult = m_parent->getTemplate()->getReceiveBounceMegaSwimingMult();
        break;
    }

    m_characterPhys->setSpeed(Vec2d::Zero);

    if ( m_parent->isSwimming() )
    {
        m_characterPhys->addForce(m_parent->m_bounceCurrentDir*forceH*swimMult*m_parent->m_bounceMultiplier);
    }
    else
    {
        if ( m_parent->m_bounceCurrentDir != Vec2d::Zero )
        {
            StickToPolylinePhysComponent::EdgeOrientationType orientationType = m_characterPhys->getEdgeOrientationType(-m_parent->m_bounceCurrentDir.getPerpendicular());

            switch(orientationType)
            {
                case StickToPolylinePhysComponent::EDGEORIENTATIONTYPE_GROUND:
                    {
                        m_characterPhys->addForce(-m_parent->m_currentGravityDir*forceV);
                    }
                    break;
                case StickToPolylinePhysComponent::EDGEORIENTATIONTYPE_WALL:
                    {
                        Vec2d forceDir;
                        const PolyLineEdge* edge = m_characterPhys->getStickedEdge();

                        if ( edge )
                        {
                            forceDir = edge->m_normalizedVector;
                        }
                        else
                        {
                            forceDir = Vec2d::Right;
                        }

                        f32 forceSign = m_parent->m_bounceCurrentDir.dot(forceDir) < 0.f ? -1.f : 1.f;

                        m_characterPhys->addForce(forceDir*forceSign*forceH);
                        m_parent->setFrictionMode(FRICTIONMODE_RECEIVEHIT,friction);
                    }
                    break;
                case StickToPolylinePhysComponent::EDGEORIENTATIONTYPE_ROOF:
                    break;
            }
        }
    }

    if ( _restart )
    {
        m_animComponent->resetTree();
    }
}

void Ray_PlayerControllerComponent::StateHitReceive::setupEarthquake( bbool _restart )
{
    m_characterPhys->setSpeed(Vec2d::Zero);

    if ( !m_characterPhys->getStickedEdge() )
    {
        f32 force = 0;

        switch(m_parent->m_receivedHitLevel)
        {
        case 0:
            force = m_parent->getTemplate()->getReceiveEarthquakeWeakForce();
            break;
        case 1:
            force = m_parent->getTemplate()->getReceiveEarthquakeStrongForce();
            break;
        default:
            force = m_parent->getTemplate()->getReceiveEarthquakeMegaForce();
            break;
        }

        m_characterPhys->addForce(m_parent->m_currentGravityDir*force);
    }

    if ( _restart )
    {
        m_animComponent->resetTree();
    }
}

bbool Ray_PlayerControllerComponent::StateHitReceive::checkStateChange( f32 _dt )
{
    if ( m_triggerDeath || m_parent->isInDeadRegion() )
    {
        m_triggerDeath = bfalse;
        exitState();
        return btrue;
    }

    bbool dead = m_parent->isDead();

    if ( !dead && checkBounce())
    {
        return btrue;
    }

    switch(m_parent->m_receivedHitType)
    {
    case RECEIVEDHITTYPE_EJECTY:
    case RECEIVEDHITTYPE_UPPUNCH:
        {
            if ( checkFinishedPunchUp() )
            {
                return btrue;
            }
        }
        break;
    case RECEIVEDHITTYPE_HURTBOUNCE:
        {
            if ( checkFinishedHurtBounce() )
            {
                return btrue;
            }
        }
        break;
    case RECEIVEDHITTYPE_EARTHQUAKE:
        {
            if ( checkFinishedEarthquake() )
            {
                return btrue;
            }
        }
        break;
    default:
        {
            if ( checkFinishedPunchFront() )
            {
                return btrue;
            }
        }
        break;
    }

    if ( m_canMove && !dead )
    {
        if ( exitFromAction() )
        {
            return btrue;
        }
    }

    return bfalse;
}

bbool Ray_PlayerControllerComponent::StateHitReceive::exitFromAction()
{
    if ( m_parent->isJumpQueued() && !m_justEntered )
    {
        if ( m_parent->tryWallJump(bfalse) )
        {
            return btrue;
        }
    }

    HangingInfo info;

    if ( m_parent->tryHanging(info) )
    {
        m_parent->setHangState(info);
        return btrue;
    }

    if  ( m_parent->isHitQueued() )
    {
        if ( m_characterPhys->getStickedEdge() )
        {
            if ( m_parent->setIdlePunch() )
            {
                return btrue;
            }
        }
        else
        {
            if ( m_parent->isSwimming() )
            {
                if ( m_parent->setWaterPunch() )
                {
                    return btrue;
                }
            }
            else if ( m_parent->setAirPunch() )
            {
                return btrue;
            }
        }
    }

    if ( m_parent->shouldTriggerHelicoFromHold() )
    {
        m_parent->changeState(&m_parent->m_stateHelicopter);
        return btrue;
    }

    return bfalse;
}

void Ray_PlayerControllerComponent::StateHitReceive::exitState()
{
    if ( m_parent->isDead() )
    {
        m_parent->triggerDeath(DEATHTYPE_REGULARDEATH);
        return;
    }
    else if ( exitFromAction() )
    {
        return;
    }
    else if ( m_parent->m_stance == STANCE_SWIM )
    {
        BaseObject* obj = GETOBJECT(m_parent->m_swimingPolyline);

        if ( !obj )
        {
            return;
        }

        PolyLine* poly = obj->DynamicCast<PolyLine>(ITF_GET_STRINGID_CRC(PolyLine,1932163747));

        if ( poly && poly->isPointInside(m_actor->get2DPos()) )
        {
            m_parent->changeState(&m_parent->m_stateSwimIdle);
        }
        else
        {
            m_parent->changeState(&m_parent->m_stateFalling);
        }
    }
    else
    {
        if ( m_parent->trySwim(bfalse) )
        {
            if ( m_parent->m_currentState != &m_parent->m_stateSwimIdle )
            {
                m_parent->changeState(&m_parent->m_stateSwimIdle);
            }
        }
        else if ( m_characterPhys->getStickedEdge() )
        {
            m_parent->changeState(&m_parent->m_stateIdle);
        }
        else
        {
            m_parent->changeState(&m_parent->m_stateFalling);
        }
    }
}

bbool Ray_PlayerControllerComponent::StateHitReceive::checkFinishedPunchFront()
{
    if ( !m_justEntered )
    {
        if ( checkCrashedOnWall() )
        {
            return btrue;
        }

        if ( isCurrentAnimFinished() )
        {
            exitState();
            return btrue;
        }

        if ( !m_parent->isDead() && m_canMove && !m_parent->playerIsIdle() )
        {
            exitState();
            return btrue;
        }
    }

    return bfalse;
}

bbool Ray_PlayerControllerComponent::StateHitReceive::checkFinishedPunchUp()
{
    if ( checkCrashedOnWall() )
    {
        return btrue;
    }

    if ( !m_justEntered )
    {
        if ( m_parent->isDead() )
        {
            exitState();
            return btrue;
        }
        else if ( m_characterPhys->getStickedEdge() && isCurrentAnimFinished() )
        {
            exitState();
            return btrue;
        }
        else if ( !m_parent->isDead() && m_canMove && !m_parent->playerIsIdle() )
        {
            exitState();
            return btrue;
        }
    }

    return bfalse;
}

bbool Ray_PlayerControllerComponent::StateHitReceive::checkFinishedHurtBounce()
{
    if ( isCurrentAnimFinished() )
    {
        exitState();
        return btrue;
    }

    return bfalse;
}

bbool Ray_PlayerControllerComponent::StateHitReceive::checkFinishedEarthquake()
{
    if ( isCurrentAnimFinished() )
    {
        exitState();
        return btrue;
    }

    return bfalse;
}

void Ray_PlayerControllerComponent::StateHitReceive::update( f32 _dt )
{
    Super::update(_dt);

    switch(m_parent->m_receivedHitType)
    {
        case RECEIVEDHITTYPE_EJECTY:
        case RECEIVEDHITTYPE_UPPUNCH:
            updatePunchUp(_dt);
            break;
        case RECEIVEDHITTYPE_HURTBOUNCE:
            updateHurtBounce(_dt);
            break;
        case RECEIVEDHITTYPE_EARTHQUAKE:
            updateEarthquake(_dt);
            break;
        default:
            updatePunchFront(_dt);
            break;
    }

    updateSwimStance(_dt);
    
    m_justEntered = bfalse;
    m_currentSpeed = m_characterPhys->getSpeed();
}

void Ray_PlayerControllerComponent::StateHitReceive::updatePunchFront( f32 _dt )
{
    if ( !m_characterPhys->getStickedEdge() )
    {
        m_parent->UpdatePhysicAir(btrue);
    }
}

void Ray_PlayerControllerComponent::StateHitReceive::updatePunchUp( f32 _dt )
{
    if ( m_appexStart )
    {
        f32 dotProd = m_parent->m_currentGravityDir.getPerpendicular().dot(m_characterPhys->getSpeed());

        if ( dotProd < 0.f )
        {
            m_characterPhys->setSpeed(m_parent->m_currentGravityDir.getPerpendicular()*dotProd);
        }

        m_parent->setGravityMode(GRAVITYMODE_RECEIVEHIT,0.f);
        m_appexStart = bfalse;
        m_appex = btrue;
    }
    else if ( m_appexStop )
    {
        m_parent->resetGravityMode(GRAVITYMODE_RECEIVEHIT);
        m_appexStop = bfalse;
        m_appex = bfalse;
    }
}

void Ray_PlayerControllerComponent::StateHitReceive::updateHurtBounce( f32 _dt )
{
    if ( m_characterPhys->getStickedEdgeIndex() == U32_INVALID && !m_parent->isSwimming() )
    {
        m_parent->UpdatePhysicAir();
    }
}

void Ray_PlayerControllerComponent::StateHitReceive::updateEarthquake( f32 _dt )
{

}

void Ray_PlayerControllerComponent::StateHitReceive::updateSwimStance( f32 _dt )
{
    bbool wasSwiming = m_parent->isSwimming();
    bbool insideWater;

    if ( m_parent->trySwim(bfalse) )
    {
        m_parent->setStance(STANCE_SWIM);
        insideWater = btrue;
    }
    else
    {
        insideWater = bfalse;
    }

    if ( !insideWater && wasSwiming )
    {
        m_parent->setStance(STANCE_STAND);
        m_parent->setGravityMode(GRAVITYMODE_RECEIVEHIT,m_parent->getTemplate()->getReceiveHitSwimGravityMultiplier());
    }
}

bbool Ray_PlayerControllerComponent::StateHitReceive::checkCrashedOnWall()
{
    if ( m_wasBlocked )
    {
        m_wasBlocked = bfalse;

        PolyLine* poly = NULL;
        const PolyLineEdge* edge = NULL;

        AIUtils::getPolyLine(m_blockedPolyline,m_blockedEdgeIndex,poly,edge);

        if ( poly && edge )
        {
            setCrashAnim(poly,edge,m_blockedEdgeIndex);
            return btrue;
        }
    }

    return bfalse;
}

void Ray_PlayerControllerComponent::StateHitReceive::setCrashAnim( const PolyLine* _poly, const class PolyLineEdge* _edge, u32 _edgeIndex )
{
    m_parent->m_stateReceiveHitCrash.setCrashData(_poly->getRef(),_edgeIndex);
    m_parent->changeState(&m_parent->m_stateReceiveHitCrash);
}

void Ray_PlayerControllerComponent::StateHitReceive::onEvent(Event * _event)
{
    Super::onEvent(_event);

    if ( AnimGameplayEvent* animEvent = _event->DynamicCast<AnimGameplayEvent>(ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301)) )
    {
        if ( animEvent->getName() == MRK_Allow_Move )
        {
            m_canMove = btrue;
        }
        else if ( animEvent->getName() == MRK_Death )
        {
            m_triggerDeath = m_parent->isDead();
        }
        else if ( animEvent->getName() == MRK_UpHitApex_Start )
        {
            m_appexStart = btrue;
        }
        else if ( animEvent->getName() == MRK_UpHitApex_Stop )
        {
            m_appexStop = btrue;
        }
    }
    else if ( EventBlockedByPolyline* blockedEvent = _event->DynamicCast<EventBlockedByPolyline>(ITF_GET_STRINGID_CRC(EventBlockedByPolyline,2950236768)) )
    {
        if ( !m_wasBlocked )
        {
            PolyLine* poly = NULL;
            const PolyLineEdge* edge = NULL;

            AIUtils::getPolyLine(blockedEvent->getPolyline(),blockedEvent->getEdge(),poly,edge);

            if ( edge )
            {
                f32 dotProd = edge->m_normalizedVector.getPerpendicular().dot(blockedEvent->getSpeed());

                // Make sure our speed was going quite opposite to the edge we are blocked by
                if ( dotProd < -1.f )
                {
                    m_wasBlocked = btrue;
                    m_blockedPolyline = blockedEvent->getPolyline();
                    m_blockedEdgeIndex = blockedEvent->getEdge();
                }
            }
        }
    }
}

void Ray_PlayerControllerComponent::StateHitReceive::restart()
{
    if ( m_parent->isDead() )
    {
        m_parent->triggerDeath(DEATHTYPE_REGULARDEATH);
    }
    else
    {
        m_triggerDeath = bfalse;
        setupHit(btrue);
    }
}

void Ray_PlayerControllerComponent::StateHitReceive::updateAnimInputs()
{
    Super::updateAnimInputs();

    if ( ( m_parent->m_receivedHitType == RECEIVEDHITTYPE_EJECTY ||
           m_parent->m_receivedHitType == RECEIVEDHITTYPE_UPPUNCH ) &&
           m_characterPhys->getStickedEdge() )
    {
        // trick the speed so that we keep the same animation this frame
        m_animComponent->setInputFromIndex(m_parent->m_animInputIndex[ANIMINPUT_SPEEDY],m_currentSpeed.m_y);
    }
}

}
