#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_
#include "rayman/gameplay/Components/Player/Ray_PlayerControllerComponent.h"
#endif //_ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_

#ifndef _ITF_ID_SERVER_H_
#include "core/IDserver.h"
#endif //_ITF_ID_SERVER_H_

#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI_SUBCLASS(Ray_PlayerControllerComponent,StateHitReceiveCrash)

////////////////////
// Receive Hit Crash State
////////////////////
Ray_PlayerControllerComponent::StateHitReceiveCrash::StateHitReceiveCrash()
: m_crashPoly(ITF_INVALID_OBJREF)
, m_prevPos(Vec2d::Zero)
, m_crashDist(0.f)
, m_crashTargetAngle(0.f)
, m_crashEdge(U32_INVALID)
, m_justEntered(bfalse)
, m_triggerDeath(bfalse)
{

}

void Ray_PlayerControllerComponent::StateHitReceiveCrash::onEnter()
{
    Super::onEnter();

    m_justEntered = btrue;
    m_triggerDeath = bfalse;
    m_prevPos = m_actor->get2DPos();
    m_characterPhys->setDisabled(btrue);

    PolyLine* poly;
    const PolyLineEdge* edge;

    AIUtils::getPolyLine(m_crashPoly,m_crashEdge,poly,edge);

    if ( poly && edge )
    {
        Vec2d dirToActor = m_actor->get2DPos() - edge->getPos();

        m_crashDist = edge->m_normalizedVector.dot(dirToActor);

        Vec2d normal = edge->m_normalizedVector.getPerpendicular();
        f32 dotProd = normal.dot(-Vec2d::Up);

        if ( dotProd >= 0.95f )
        {
            // crash up
            m_parent->m_orientationPose = ORIENTATION_VERTICAL;
            m_crashTargetAngle = getVec2Angle(-edge->m_normalizedVector);
        }
        else
        {
            // crash front

            f32 angle = getVec2Angle(normal);
            f32 difA = getShortestAngleDelta(m_actor->getAngle(),angle);
            f32 difB = getShortestAngleDelta(m_actor->getAngle(),angle+MTH_PI);

            if ( f32_Abs(difA) < f32_Abs(difB) )
            {
                m_crashTargetAngle = angle;
            }
            else
            {
                m_crashTargetAngle = angle + MTH_PI;
            }

            Vec2d dir = Vec2d::Right.Rotate(m_crashTargetAngle);

            bbool lookRight = dir.dot(edge->m_normalizedVector.getPerpendicular()) < 0.f;

            m_parent->setLookRight(lookRight);

            m_parent->m_orientationPose = ORIENTATION_HORIZONTAL;
        }
    }
}

void Ray_PlayerControllerComponent::StateHitReceiveCrash::onExit()
{
    m_characterPhys->setDisabled(bfalse);

    if ( isSticked() )
    {
        m_animComponent->resetLockMagicBox();
    }
}

bbool Ray_PlayerControllerComponent::StateHitReceiveCrash::checkStateChange( f32 _dt )
{
    if ( m_triggerDeath )
    {
        m_triggerDeath = bfalse;

        if ( m_parent->isDead() )
        {
            m_parent->triggerDeath(DEATHTYPE_REGULARDEATH);
            return btrue;
        }
    }

    if ( isCurrentAnimFinished() )
    {
        if ( m_parent->isDead() )
        {
            m_parent->triggerDeath(DEATHTYPE_REGULARDEATH);
        }
        else if ( m_parent->m_stance == STANCE_SWIM )
        {
            bbool insideWater = btrue;
            BaseObject* obj = GETOBJECT(m_parent->m_swimingPolyline);

            if ( !obj )
            {
                insideWater = bfalse;
            }
            else
            {
                PolyLine* poly = obj->DynamicCast<PolyLine>(ITF_GET_STRINGID_CRC(PolyLine,1932163747));

                if ( !poly )
                {
                    insideWater = bfalse;
                }
                else
                {
                    if ( !poly->isPointInside(m_actor->get2DPos()) )
                    {
                        insideWater = bfalse;
                    }
                }
            }

            if ( insideWater )
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
            m_parent->changeState(&m_parent->m_stateFalling);
        }

        return btrue;
    }

    return bfalse;
}

void Ray_PlayerControllerComponent::StateHitReceiveCrash::update(f32 _dt)
{
    if ( !m_justEntered )
    {
        if ( m_crashPoly != ITF_INVALID_OBJREF )
        {
            PolyLine* poly;
            const PolyLineEdge* edge;
            
            AIUtils::getPolyLine(m_crashPoly,m_crashEdge,poly,edge);

            if ( poly && edge )
            {
                Vec2d pos = edge->getPos() + ( edge->m_normalizedVector * m_crashDist );

                updateStickEdge(pos);

                if ( isSticked() )
                {
                    m_animComponent->lockMagicBox(pos.to3d(m_actor->getPos().m_z));
                }
                else
                {
                    m_animComponent->resetLockMagicBox();
                }
            }
            else
            {
                unstick();
                m_animComponent->resetLockMagicBox();
            }
        }
    }
    else
    {
        m_justEntered = bfalse;
    }

    m_actor->setAngle(m_crashTargetAngle);
    m_prevPos = m_actor->get2DPos();
}

void Ray_PlayerControllerComponent::StateHitReceiveCrash::updateStickEdge( const Vec2d& _newPos )
{
    f32 radius = m_characterPhys->getRadius();
    PhysShapeCircle circle(radius);
    PhysContactsContainer contacts;

    PHYSWORLD->checkEncroachment(m_prevPos,_newPos,m_actor->getAngle(),
                                &circle,ECOLLISIONFILTER_ENVIRONMENT,m_actor->getDepth(),contacts);

    u32 numContacts = contacts.size();

    for ( u32 index = 0; index < numContacts; index++ )
    {
        const SCollidableContact& contact = contacts[index];

        if ( contact.m_collidableUserData == m_crashPoly )
        {
            continue;
        }

        unstick();
        break;
    }
}

void Ray_PlayerControllerComponent::StateHitReceiveCrash::unstick()
{
    m_crashPoly = ITF_INVALID_OBJREF;
    m_crashEdge = U32_INVALID;
}

void Ray_PlayerControllerComponent::StateHitReceiveCrash::setCrashData( ObjectRef _crashPoly, u32 _crashEdge )
{
    m_crashPoly = _crashPoly;
    m_crashEdge = _crashEdge;
}

void Ray_PlayerControllerComponent::StateHitReceiveCrash::onEvent(Event * _event)
{
    Super::onEvent(_event);

    if ( AnimGameplayEvent* animEvent = _event->DynamicCast<AnimGameplayEvent>(ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301)) )
    {
        if ( animEvent->getName() == MRK_Death )
        {
            m_triggerDeath = m_parent->isDead();
        }
    }
}

}