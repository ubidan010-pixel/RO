#include "precompiled_gameplay_war.h"

#ifndef _ITF_WAR_PLAYERCONTROLLERCOMPONENT_H_
#include "war/gameplay/Components/Player/War_PlayerControllerComponent.h"
#endif //_ITF_WAR_PLAYERCONTROLLERCOMPONENT_H_

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

#ifndef _ITF_WAR_ANIMATIONMARKERS_H_
#include "war/gameplay/War_AnimationMarkers.h"
#endif //_ITF_WAR_ANIMATIONMARKERS_H_

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

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(War_PlayerControllerComponent::StateHitReceive)

////////////////////
// Receive Hit State
////////////////////
War_PlayerControllerComponent::StateHitReceive::StateHitReceive()
: m_prevSpeedDir(Vec2d::Zero)
, m_prevGravityMultiplier(1.f)
, m_airMoveInitialSpeed(0.f)
, m_receivedHitDirX(0.f)
, m_animCursor(0.f)
, m_hitOnAir(bfalse)
, m_hitAirMove(bfalse)
, m_justEntered(bfalse)
{
}

void War_PlayerControllerComponent::StateHitReceive::onEnter()
{
    Super::onEnter();

    setupHit();

    m_prevGravityMultiplier = m_characterPhys->getGravityMultiplier();
    m_characterPhys->setGravityMultiplier(m_parent->m_receiveHitGravityMultiplier);
    m_justEntered = btrue;
}

void War_PlayerControllerComponent::StateHitReceive::onExit()
{
    m_characterPhys->setGravityMultiplier(m_prevGravityMultiplier);
}

void War_PlayerControllerComponent::StateHitReceive::setupHit()
{
    m_prevSpeedDir = m_characterPhys->getSpeed();
    m_prevSpeedDir.normalize();

    //Neil
    //m_fxController->playHit(m_parent->m_receivedHitInstigator,m_parent->m_receivedHitLevel);

    // remove HP (unless hit by another player, unless friendly fire is authorized)
    Actor* instigatorActor = AIUtils::getActor(m_parent->m_receivedHitInstigator);
    if ( !instigatorActor
        || !AIUtils::isEnemy(m_actor, instigatorActor)
        /*|| GAMEMANAGER->getAllowFriendlyFire()*/ )
    {
#pragma message("FIX THIS")
        //GAMEMANAGER->getPlayer(m_parent->m_playerIndex)->addHitPoints(-1);
    }

    switch(m_parent->m_receivedHitType)
    {
        case WAR_RECEIVEDHITTYPE_NORMALHIT:
            setupNormalHit();
            break;
        // HACK
        //case RECEIVEDHITTYPE_NORMALPUNCH:
        //    setupNormalHit();
        //    break;
        //case RECEIVEDHITTYPE_LOWEJECTING:
        //    setupEjectHit();
        //    break;
    }

    m_hitOnAir = m_characterPhys->getStickedEdge() == NULL;
    m_hitAirMove = bfalse;

    Vec2d myDir;

    myDir = AIUtils::getLookDir(m_actor,m_animComponent->getFlipped());

    if ( myDir.dot(m_parent->m_receivedHitDir) > 0.f )
    {
        m_receivedHitDirX = -1.f;
    }
    else
    {
        m_receivedHitDirX = 1.f;
    }
}

void War_PlayerControllerComponent::StateHitReceive::restart()
{
    setupHit();
    m_animComponent->resetCurTime();
}

void War_PlayerControllerComponent::StateHitReceive::setupNormalHit()
{
    BaseObject* obj = GETOBJECT(m_parent->m_receivedHitInstigator);

    if ( obj )
    {
        Actor* actor = obj->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));

        if ( actor )
        {
            // We have to reorient the character towards the sender of the hit
            Vec2d dir = actor->get2DPos() - m_parent->GetActor()->get2DPos();

            dir.normalize();

            f32 dotProd = Vec2d::Right.Rotate(m_parent->GetActor()->getAngle()).dot(dir);

            if ( ( dotProd < 0.f && m_parent->m_lookRight ) ||
                 ( dotProd > 0.f && !m_parent->m_lookRight ) )
            {
                m_parent->setLookRight(!m_parent->m_lookRight);
                m_parent->resetDesiredLookDir();
            }

            // Cancel our speed
            m_characterPhys->setSpeed(Vec2d::Zero);

            // Add the force based on the hit level
            m_characterPhys->addForce((m_parent->m_receivedHitDir*
                static_cast<f32>(m_parent->m_receivedHitLevel+1)*
                m_parent->m_receiveHitForce));

            m_prevSpeedDir = m_parent->m_receivedHitDir;
        }
    }
}

void War_PlayerControllerComponent::StateHitReceive::setupEjectHit()
{
    m_characterPhys->setSpeed(Vec2d::Zero);
    m_characterPhys->addForce((-m_parent->m_currentGravityDir)*m_parent->m_receiveLowHitEjectForce);

}

bbool War_PlayerControllerComponent::StateHitReceive::checkStateChange( f32 _dt, const InputAdapter::PressStatus* _buttons, f32* _axes )
{
    if (checkBounce())
    {
        return btrue;
    }

    switch(m_parent->m_receivedHitType)
    {
    // HACK
    //case RECEIVEDHITTYPE_LOWEJECTING:
    //    {
    //        if ( checkFinishedLowHitEject() )
    //        {
    //            return btrue;
    //        }
    //    }
    //    break;
    case WAR_RECEIVEDHITTYPE_NORMALHIT:
    default:
        {
            if ( checkFinishedNormalPunch() )
            {
                return btrue;
            }
        }
        break;
    }

    return bfalse;
}

bbool War_PlayerControllerComponent::StateHitReceive::checkFinishedNormalPunch()
{
    if ( !m_hitAirMove && isCurrentAnimFinished() )
    {
        if ( m_parent->m_receivedHitLevel > 0 && m_hitOnAir )
        {
            m_hitAirMove = btrue;
            m_airMoveInitialSpeed = m_characterPhys->getSpeed().sqrnorm();
        }
        else
        {
            m_parent->changeState(&m_parent->m_stateIdle);
            return btrue;
        }
    }

    if ( m_hitOnAir )
    {
        if ( m_characterPhys->getStickedEdge() )
        {
            setLandAnim(m_characterPhys->getStickedEdge());
            return btrue;
        }

        if ( checkExitOnAir() )
        {
            return bfalse;
        }
    }

    return bfalse;
}

void War_PlayerControllerComponent::StateHitReceive::update( f32 _dt,const InputAdapter::PressStatus* _buttons, f32 * _axes )
{
    Super::update(_dt,_buttons,_axes);

    switch(m_parent->m_receivedHitType)
    {
        // HACK
        //case RECEIVEDHITTYPE_LOWEJECTING:
        //    updateLowHitEject();
        //    break;
        case WAR_RECEIVEDHITTYPE_NORMALHIT:
        default:
            updateNormalPunch();
            break;
    }
    
    m_justEntered = bfalse;
}

void War_PlayerControllerComponent::StateHitReceive::updateNormalPunch()
{
    if ( !m_hitAirMove &&
         m_parent->m_receivedHitType == WAR_RECEIVEDHITTYPE_NORMALHIT &&
         isCurrentAnimFinished() &&
         m_parent->m_receivedHitLevel > 0 &&
         m_hitOnAir )
    {
        m_hitAirMove = btrue;
        m_airMoveInitialSpeed = m_characterPhys->getSpeed().sqrnorm();
    }

    if ( m_hitAirMove )
    {
        f32 currentSpeedSq = Max(m_airMoveInitialSpeed,m_characterPhys->getSpeed().sqrnorm());
        f32 t = currentSpeedSq/m_airMoveInitialSpeed;
        m_animCursor = 1.f-t;
    }
}

bbool War_PlayerControllerComponent::StateHitReceive::checkExitOnAir()
{
    // When we are in the air we check if we crash with a wall or if we will land on something
    const PhysContactsContainer& contacts = m_characterPhys->getContacts();
    Vec2d gravity = PHYSWORLD->getGravity(m_actor->get2DPos(),m_actor->getDepth());
    const PolyLineEdge* edge = NULL;
    const PolyLine* poly = NULL;
    u32 edgeIndex = U32_INVALID;

    gravity.normalize();

    bbool ejectedUp = m_prevSpeedDir != Vec2d::Zero && gravity.dot(m_prevSpeedDir) < 0.25f;

    for ( u32 index = 0; index < contacts.size(); index++ )
    {
        const SCollidableContact& contact = contacts[index];

        if ( contact.m_edgeIndex != U32_INVALID )
        {
            // Maybe we are crashing into a wall
            BaseObject* obj = GETOBJECT(contact.m_collidableUserData);

            if ( obj )
            {
                poly = obj->DynamicCast<PolyLine>(ITF_GET_STRINGID_CRC(PolyLine,1932163747));

                if ( poly )
                {
                    edge = &poly->getEdgeAt(contact.m_edgeIndex);
                    edgeIndex = contact.m_edgeIndex;
                    break;
                }
            }
        }
    }

    // If we hit an edge play the anim of crashing on a wall
    if ( edge )
    {
        if ( ejectedUp )
        {
            setCrashAnim(poly,edge,edgeIndex);
        }
        else
        {
            setLandAnim(edge);
        }

        return btrue;
    }
    else
    {
        if ( m_characterPhys->getSpeed() != Vec2d::Zero )
        {
            m_prevSpeedDir = m_characterPhys->getSpeed();
            m_prevSpeedDir.normalize();
        }
    }

    return bfalse;
}

void War_PlayerControllerComponent::StateHitReceive::setLandAnim( const PolyLineEdge* _edge )
{
    if ( fabs(m_prevSpeedDir.dot(_edge->m_normalizedVector)) > 0.4f )
    {
        // Land horizontal
        m_parent->m_orientationPose = ORIENTATION_HORIZONTAL;
    }
    else
    {
        // Land vertical
        m_parent->m_orientationPose = ORIENTATION_VERTICAL;
    }

    m_parent->changeState(&m_parent->m_stateReceiveHitLand);
}

void War_PlayerControllerComponent::StateHitReceive::setCrashAnim( const PolyLine* _poly, const class PolyLineEdge* _edge, u32 _edgeIndex )
{
    Vec2d vertAxis = -Vec2d::Up.Rotate(m_actor->getAngle());
    Vec2d feetPos = m_actor->get2DPos() + ( vertAxis * m_actor->getRadius() );
    Vec2d dirToFeet = feetPos - _edge->getPos();

    Vec2d normal = _edge->m_normalizedVector.getPerpendicular();
    f32 angle = getVec2Angle(normal);
    f32 difA = getShortestAngleDelta(m_actor->getAngle(),angle);
    f32 difB = getShortestAngleDelta(m_actor->getAngle(),angle+MTH_PI);
    f32 crashDist = _edge->m_normalizedVector.dot(dirToFeet);
    f32 targetAngle = 0.f;

    if ( fabs(difA) < fabs(difB) )
    {
        targetAngle = angle;
    }
    else
    {
        targetAngle = angle + MTH_PI;
    }

    m_parent->m_stateReceiveHitCrash.setCrashData(_poly->getRef(),_edgeIndex,crashDist,targetAngle);

    m_parent->changeState(&m_parent->m_stateReceiveHitCrash);
}

bbool War_PlayerControllerComponent::StateHitReceive::canRestart( War_EReceivedHitType _receiveHitType ) const
{
    // HACK
    //return _receiveHitType != RECEIVEDHITTYPE_LOWEJECTING
    //    || m_parent->m_receivedHitType != RECEIVEDHITTYPE_LOWEJECTING;

    return btrue;
}

void War_PlayerControllerComponent::StateHitReceive::updateAnimInputs()
{
    Super::updateAnimInputs();

    static const StringID s_HitOnAir = "HitOnAir";
    static const StringID s_HitAirMove = "HitAirMove";
    static const StringID s_ReceiveHitDirX = "ReceivedHitDirX";

    m_animComponent->setInput(s_HitOnAir,static_cast<u32>(m_hitOnAir));
    m_animComponent->setInput(s_HitAirMove,static_cast<u32>(m_hitAirMove));
    m_animComponent->setInput(s_ReceiveHitDirX,m_receivedHitDirX);
    m_animComponent->setProceduralCursor(m_animCursor);
}

}
