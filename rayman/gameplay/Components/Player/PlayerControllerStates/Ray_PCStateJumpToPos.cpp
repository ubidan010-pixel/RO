#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_
#include "rayman/gameplay/Components/Player/Ray_PlayerControllerComponent.h"
#endif //_ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI_SUBCLASS(Ray_PlayerControllerComponent,StateJumpToPos)

////////////////////
// Bounce To Layer State
////////////////////
Ray_PlayerControllerComponent::StateJumpToPos::StateJumpToPos()
: m_targetPos(Vec2d::Zero)
, m_refObj(ITF_INVALID_OBJREF)
, m_refPoly(ITF_INVALID_OBJREF)
, m_edgeIndex(U32_INVALID)
, m_edgeT(0.f)
{
}

void Ray_PlayerControllerComponent::StateJumpToPos::onEnter()
{
    Super::onEnter();

    m_characterPhys->setDisabled(btrue);
    m_posThisFrame = getTargetPos();
}

void Ray_PlayerControllerComponent::StateJumpToPos::onExit()
{
    Super::onExit();

    resetTarget();
    m_characterPhys->setDisabled(bfalse);
}

bbool Ray_PlayerControllerComponent::StateJumpToPos::checkStateChange( f32 _dt )
{
    m_posThisFrame = getTargetPos();

    if ( m_posThisFrame == Vec2d::Zero )
    {
        exitState();
        return btrue;
    }

    Vec2d vecToPos = m_posThisFrame - m_actor->get2DPos();
    f32 distToPosSq = vecToPos.sqrnorm();
    f32 radius = m_characterPhys->getRadius();

    if ( distToPosSq < (radius*radius) )
    {
        Vec2d groundPos = m_posThisFrame + (m_parent->m_currentGravityDir*radius);
        
        vecToPos = groundPos - m_actor->get2DPos();

        bbool isDown = m_parent->m_currentGravityDir.dot(vecToPos) >= (radius*0.8);

        if ( isDown )
        {
            m_characterPhys->forceFindStickEdge();

            if ( m_characterPhys->getStickedEdgeIndex() != U32_INVALID )
            {
                exitState();
                return btrue;
            }
        }
    }

    if ( checkBounce() )
    {
        return btrue;
    }

    if  ( m_parent->isHitQueued() )
    {
        if ( m_parent->setAirPunch() )
        {
            return btrue;
        }
    }

    return bfalse;
}

void Ray_PlayerControllerComponent::StateJumpToPos::exitState()
{
    if ( m_characterPhys->getStickedEdgeIndex() == U32_INVALID )
    {
        m_characterPhys->forceFindStickEdge();
    }

    if ( m_characterPhys->getStickedEdgeIndex() != U32_INVALID )
    {
        m_parent->changeState(&m_parent->m_stateIdle);
    }
    else
    {
        m_parent->changeState(&m_parent->m_stateFalling);
    }
}

void Ray_PlayerControllerComponent::StateJumpToPos::update(f32 _dt)
{
    Super::update(_dt);

    Vec2d pos = m_actor->get2DPos();

    updateX(_dt,pos.m_x);
    updateY(_dt,pos.m_x,pos.m_y);

    EventTeleport evtTeleport( pos.to3d(m_actor->getPos().m_z), 0.0f, bfalse );

    m_actor->onEvent( &evtTeleport );
}

void Ray_PlayerControllerComponent::StateJumpToPos::updateX( f32 _dt, f32& _x )
{
    static const f32 s_maxSpeedX = 5.f;
    static const f32 s_speedAccel = 30.f;
    static const f32 s_speedBrake = 100.f;
    static const f32 s_closeRange = 0.05f;

    f32 xDist = m_posThisFrame.m_x - _x;
    f32 xSpeed = m_characterPhys->getSpeed().m_x;
    f32 speedSign = getSign(xSpeed);

    if ( ( xDist > 0.f && xSpeed >= 0.f ) ||
         ( xDist < 0.f && xSpeed <= 0.f ) )
    {
        if ( fabs(xDist) < s_closeRange )
        {
            if ( fabs(xSpeed) > 1.f )
            {
                xSpeed = Max(xSpeed+(s_speedBrake*_dt*-speedSign),1.f*speedSign);
            }
            else
            {
                xSpeed = Min(xSpeed+(s_speedAccel*_dt*speedSign),1.f*speedSign);
            }
        }
        else
        {
            if ( fabs(xSpeed) > s_maxSpeedX )
            {
                xSpeed = Max(xSpeed+(s_speedBrake*_dt*-speedSign),s_maxSpeedX*speedSign);
            }
            else
            {
                if ( xDist < 0.f )
                {
                    xSpeed = Max(xSpeed-(s_speedAccel*_dt),-s_maxSpeedX);
                }
                else
                {
                    xSpeed = Min(xSpeed+(s_speedAccel*_dt),s_maxSpeedX);
                }
            }
        }
    }
    else
    {
        xSpeed += s_speedBrake*_dt*-speedSign;
    }

    _x += xSpeed * _dt;

    m_characterPhys->setSpeed(Vec2d(xSpeed,m_characterPhys->getSpeed().m_y));
}

void Ray_PlayerControllerComponent::StateJumpToPos::updateY( f32 _dt, f32 _currentX, f32& _y )
{
    static const f32 s_maxSpeedY = 10.f;
    static const f32 s_speedAccel = 40.f;
    static const f32 s_speedBrake = 150.f;
    static const f32 s_closeRange = 0.05f;

    f32 yDist = m_posThisFrame.m_y - _y;
    f32 ySpeed = m_characterPhys->getSpeed().m_y;

    if ( yDist > 0.f )
    {
        // it's up
        if ( ySpeed < 0.f )
        {
            ySpeed += s_speedBrake*_dt;
        }
        else
        {
            if ( ySpeed > s_maxSpeedY )
            {
                ySpeed = Max(ySpeed-(s_speedBrake*_dt),s_maxSpeedY);
            }
            else
            {
                ySpeed = Min(ySpeed+(s_speedAccel*_dt),s_maxSpeedY);
            }
        }
    }
    else
    {
        // it's down
        if ( fabs(yDist) < s_closeRange )
        {
            ySpeed += s_speedBrake*_dt*-getSign(ySpeed);
        }
        else
        {
            ySpeed += m_parent->m_currentGravity.m_y*_dt;
        }
    }

    _y += ySpeed * _dt;

    m_characterPhys->setSpeed(Vec2d(m_characterPhys->getSpeed().m_x,ySpeed));
}

void Ray_PlayerControllerComponent::StateJumpToPos::resetTarget()
{
    m_targetPos = Vec2d::Zero;
    m_refObj = ITF_INVALID_OBJREF;
    m_refPoly = ITF_INVALID_OBJREF;
    m_edgeIndex = U32_INVALID;
    m_edgeT = 0.f;
}

Vec2d Ray_PlayerControllerComponent::StateJumpToPos::getTargetPos() const
{
    Vec2d ret = Vec2d::Zero;

    if ( m_targetPos != Vec2d::Zero )
    {
        ret = m_targetPos;
    }
    else if ( m_refObj != ITF_INVALID_OBJREF )
    {
        BaseObject* obj = GETOBJECT(m_refObj);

        if ( obj )
        {
            Pickable* pick = obj->DynamicCast<Pickable>(ITF_GET_STRINGID_CRC(Pickable,594660361));

            if ( pick )
            {
                ret = pick->get2DPos();
            }
        }
    }
    else if ( m_refPoly != ITF_INVALID_OBJREF )
    {
        if ( m_edgeIndex != U32_INVALID )
        {
            PolyLine* poly;
            const PolyLineEdge* edge;

            AIUtils::getPolyLine(m_refPoly,m_edgeIndex,poly,edge);

            if ( poly && edge )
            {
                ret = edge->getPos() + ( edge->m_vector * m_edgeT );
            }
        }
        else
        {
            PolyLine* poly = AIUtils::getPolyLine(m_refPoly);

            if ( poly )
            {
                const AABB& polyAABB = poly->getAABB();
                u32 numEdges = poly->getVectorsCount();
                Vec2d midPos = ( polyAABB.getMin() + polyAABB.getMax() ) * 0.5f;
                Vec2d midPosEdge = Vec2d::Zero;

                for ( u32 i = 0; i < numEdges; i++ )
                {
                    const PolyLineEdge& edge = poly->getEdgeAt(i);

                    if ( midPosEdge == Vec2d::Zero )
                    {
                        Vec2d vecToPos = midPos - edge.getPos();
                        f32 dist = edge.m_normalizedVector.dot(vecToPos);

                        if ( dist >= 0.f && dist <= edge.m_length )
                        {
                            midPosEdge = edge.getPos() + ( edge.m_normalizedVector * dist );
                        }
                    }

                    Vec2d vecToPos = m_actor->get2DPos() - edge.getPos();
                    f32 dist = edge.m_normalizedVector.dot(vecToPos);

                    if ( dist >= 0.f && dist <= edge.m_length )
                    {
                        ret.m_x = Clamp(m_actor->get2DPos().m_x,edge.getPos().m_x,edge.getPos().m_x+edge.m_vector.m_x);

                        f32 t = ( ret.m_x - edge.getPos().m_x ) / edge.m_length;

                        ret.m_y = edge.getPos().m_y + ( edge.m_vector.m_y * t );
                        break;
                    }
                }

                if ( ret == Vec2d::Zero && midPosEdge != Vec2d::Zero )
                {
                    ret = midPosEdge;
                }
            }
        }
    }

    if ( ret != Vec2d::Zero )
    {
        ret -= m_parent->m_currentGravityDir * (m_characterPhys->getRadius() * 0.9f);
    }

    return ret;
}

void Ray_PlayerControllerComponent::StateJumpToPos::setTargetPos( const Vec2d& _pos )
{
    resetTarget();

    m_targetPos = _pos;
}

void Ray_PlayerControllerComponent::StateJumpToPos::setTargetObj( ObjectRef _ref )
{
    resetTarget();

    m_refObj = _ref;
}

void Ray_PlayerControllerComponent::StateJumpToPos::setTargetPoly( ObjectRef _ref, u32 _edgeIndex, f32 _edgeT )
{
    resetTarget();

    m_refPoly = _ref;
    m_edgeIndex = _edgeIndex;
    m_edgeT = _edgeT;
}

}