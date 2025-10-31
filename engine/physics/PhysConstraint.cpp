#include "precompiled_engine.h"

#ifndef _ITF_PHYSCONSTRAINT_H_
#include "engine/physics/PhysConstraint.h"
#endif //_ITF_PHYSCONSTRAINT_H_

#ifndef _ITF_PHYSBODY_H_
#include "engine/physics/PhysBody.h"
#endif //_ITF_PHYSBODY_H_

#ifndef _ITF_ID_SERVER_H_
#include "core/IDserver.h"
#endif //_ITF_ID_SERVER_H_

namespace ITF
{

PhysConstraint::PhysConstraint()
: m_bodyA(NULL)
, m_bodyB(NULL)
, m_length(1.f)
, m_minAngleOffset(bfalse,0.f)
, m_maxAngleOffset(bfalse,0.f)
, m_minLengthOffset(0.f)
, m_maxLengthOffset(0.f)
, m_stiff(1.f)
, m_damp(0.f)
, m_limitAngle(bfalse)
, m_relaxLength(bfalse)
, m_nextConstraint(NULL)
, m_prevConstraint(NULL)
, m_poolId(U32_INVALID)
{
}

void PhysConstraint::init( const PhysConstraintInitCommonInfo& _initInfo )
{
    m_bodyA = _initInfo.m_bodyA;
    m_bodyB = _initInfo.m_bodyB;
    m_length = _initInfo.m_length;
    m_minAngleOffset = _initInfo.m_minAngle;
    m_maxAngleOffset = _initInfo.m_maxAngle;
    m_minLengthOffset = _initInfo.m_minLength;
    m_maxLengthOffset = _initInfo.m_maxLength;
    m_stiff = _initInfo.m_stiff;
    m_damp = _initInfo.m_damp;
    m_limitAngle = _initInfo.m_limitAngle;
    m_relaxLength = _initInfo.m_relaxLength;
}

PhysConstraint::~PhysConstraint()
{

}

// Apply forces from angle limits
void PhysConstraint::preUpdate( f32 _delta )
{
    PhysBody* bodyA = getBodyA();
    PhysBody* bodyB = getBodyB();

    if ( m_limitAngle )
    {
        const Vec2d& posA = bodyA->getPos();
        const Vec2d& posB = bodyB->getPos();
        f32 angleA = bodyA->getAngle();
        Vec2d delta = posB - posA;
        Angle angleDif = m_maxAngleOffset-m_minAngleOffset;
        Vec2d bodyDir = (Angle(bfalse,angleA)+m_minAngleOffset+(angleDif*0.5f)).ToVector();
        f32 deltaLength = delta.norm();
        Vec2d destPos = posA + (bodyDir*deltaLength);
        Vec2d dirToDest = destPos - posB;
        Vec2d force = ( dirToDest * m_stiff ) - ( bodyB->getSpeed() * m_damp );

        bodyB->addForce(force);
    }

    if ( m_relaxLength )
    {
        const Vec2d& posA = bodyA->getPos();
        const Vec2d& posB = bodyB->getPos();
        Vec2d delta = posB - posA;
        Vec2d dirToDest;
        f32 dist = delta.norm();
        f32 diff = (dist-m_length)/dist;

        if ( bodyB->isStatic() )
        {
            dirToDest = delta*diff;
        }
        else
        {
            dirToDest = delta*(0.5f*diff);
        }

        Vec2d force = ( dirToDest * m_stiff ) - ( bodyA->getSpeed() * m_damp );

        bodyA->addForce(force);

        if ( bodyA->isStatic() )
        {
            dirToDest = -(delta*diff);
        }
        else
        {
            dirToDest = -(delta*(0.5f*diff));
        }

        force = ( dirToDest * m_stiff ) - ( bodyB->getSpeed() * m_damp );

        bodyB->addForce(force);
    }
}

// Solve the constraint, the body will stay within the limits stablished
void PhysConstraint::solve()
{
    PhysBody* bodyA = getBodyA();
    PhysBody* bodyB = getBodyB();
    Vec2d posA = bodyA->getPos();
    Vec2d posB = bodyB->getPos();
    Vec2d delta = posB - posA;
    f32 deltalength = delta.norm();

    if ( deltalength == 0.f )
    {
        return;
    }

    Angle angleA(bfalse,bodyA->getAngle());

    // Normalize angle
    Vec2d dir = delta/deltalength;

    if ( m_limitAngle )
    {
        Angle dirAngle(bfalse,getVec2Angle(dir));
        Angle minAngle = angleA+m_minAngleOffset;
        Angle maxAngle = angleA+m_maxAngleOffset;
        
        dirAngle = ClampAngle(dirAngle,minAngle,maxAngle);

        dir = dirAngle.ToVector();
        delta = dir * deltalength;
        posB = posA + delta;
    }

    // Normalize position
    f32 diff;
    
    if ( m_relaxLength )
    {
        // In this case we allow the length to be within certain limits
        if ( deltalength < m_length + m_minLengthOffset )
        {
            // Move towards m_length + m_minLengthOffset
            diff = (deltalength-(m_length + m_minLengthOffset))/deltalength;
        }
        else if ( deltalength > m_length + m_maxLengthOffset )
        {
            // Move towards m_length + m_maxLengthOffset
            diff = (deltalength-(m_length + m_maxLengthOffset))/deltalength;
        }
        else
        {
            // If it's within limits we won't move the points
            diff = 0.f;
        }
    }
    else
    {
        diff = (deltalength-m_length)/deltalength;
    }

    if ( bodyB->isStatic() )
    {
        posA += delta*diff;
    }
    else
    {
        posA += delta*(0.5f*diff);
    }

    if ( bodyA->isStatic() )
    {
        posB -= delta*diff;
    }
    else
    {
        posB -= delta*(0.5f*diff);
    }

    if ( !bodyA->isStatic() )
    {
        bodyA->setPos(posA);
    }

    if ( !bodyB->isStatic() )
    {
        bodyB->setPos(posB);
    }
}

}