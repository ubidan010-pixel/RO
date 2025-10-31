#include "precompiled_engine.h"

#ifndef _ITF_PHYSFORCEMODIFIER_H_
#include "engine/physics/PhysForceModifier.h"
#endif //_ITF_PHYSFORCEMODIFIER_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

namespace ITF
{

BEGIN_SERIALIZATION(PhysForceModifier)

    SERIALIZE_MEMBER("force",m_force);
    SERIALIZE_MEMBER("offset",m_offset);
    SERIALIZE_MEMBER("rotation",m_rotation);
    SERIALIZE_MEMBER("centerForce",m_centerForce);
    SERIALIZE_MEMBER("centerForceMaxSpeed",m_centerForceMaxSpeed);
    SERIALIZE_MEMBER("centerForceSpeed2Force",m_centerForceSpeed2Force);
    SERIALIZE_MEMBER("gradientPercentage",m_gradientPercent);
    SERIALIZE_MEMBER("speedMultiplierX",m_speedMultiplierX);
    SERIALIZE_MEMBER("speedMultiplierY",m_speedMultiplierY);
    SERIALIZE_MEMBER("point",m_point);
    SERIALIZE_MEMBER("inverted",m_inverted);

    SERIALIZE_ENUM_BEGIN("Type",m_type);
        SERIALIZE_ENUM_VAR(TYPE_CIRCLE);
        SERIALIZE_ENUM_VAR(TYPE_BOX);
        SERIALIZE_ENUM_VAR(TYPE_POLYLINE);
    SERIALIZE_ENUM_END();

    switch(m_type)
    {
        case TYPE_CIRCLE:
            SERIALIZE_OBJECT("Circle",m_circle);
            break;
        case TYPE_BOX:
            SERIALIZE_OBJECT("Box",m_box);
            break;
        case TYPE_POLYLINE:
            SERIALIZE_OBJECT("PolyLine",m_polyline);
            break;
    }

END_SERIALIZATION()

BEGIN_SERIALIZATION_SUBCLASS(PhysForceModifier,BoxData)

    SERIALIZE_MEMBER("width",m_width);
    SERIALIZE_MEMBER("height",m_height);

END_SERIALIZATION()

BEGIN_SERIALIZATION_SUBCLASS(PhysForceModifier,CircleData)

    SERIALIZE_MEMBER("radius",m_radius);
    SERIALIZE_MEMBER("angleStart",m_start);
    SERIALIZE_MEMBER("angleEnd",m_end);

END_SERIALIZATION()

BEGIN_SERIALIZATION_SUBCLASS(PhysForceModifier,PolylineData)

    SERIALIZE_MEMBER("animId",m_animId);

END_SERIALIZATION()

PhysForceModifier::PhysForceModifier()
: m_type(TYPE_BOX)
, m_force(Vec2d::Zero)
, m_gradientPercent(0.f)
, m_point(bfalse)
, m_inverted(bfalse)
, m_offset(Vec2d::Zero)
, m_rotation(bfalse,0.f)
, m_centerForce(0.f)
, m_centerForceMaxSpeed(0.f)
, m_centerForceSpeed2Force(0.f)
, m_speedMultiplierX(1.f)
, m_speedMultiplierY(1.f)
{
}

bbool PhysForceModifier_Instance::checkForce( const Vec2d& _pos, f32 _angle, const Vec2d& _checkPos, const Vec2d& _speed, Vec2d& _force, Vec2d& _speedMultiplier ) const
{
    Vec2d pos = _pos + m_template->getOffset().Rotate(_angle);
    f32 angle = _angle + m_template->getRotation().ToRadians();

    switch(m_template->getType())
    {
    case PhysForceModifier::TYPE_BOX:
        return checkForceBox(pos,angle,_checkPos,_speed,_force,_speedMultiplier);
    case PhysForceModifier::TYPE_CIRCLE:
        return checkForceCircle(pos,angle,_checkPos,_force,_speedMultiplier);
    case PhysForceModifier::TYPE_POLYLINE:
        return checkForcePolyline(pos,angle,_checkPos,_speed,_force,_speedMultiplier);

    default:
        ITF_ASSERT(0);
        return bfalse;
    }
}

bbool PhysForceModifier_Instance::checkForceCircle( const Vec2d& _pos, f32 _angle, const Vec2d& _checkPos, Vec2d& _force, Vec2d& _speedMultiplier ) const
{
    f32 dist = ( _checkPos - _pos ).norm();
    f32 radius = m_template->getCircle().m_radius * m_scale.m_x;

    if ( radius < dist )
    {
        return bfalse;
    }

    Vec2d forceDir = _pos - _checkPos;

    forceDir.normalize();

    if ( m_template->getCircle().m_start != m_template->getCircle().m_end )
    {
        Angle angle(bfalse,getVec2Angle(-forceDir));
        Angle worldAngle(bfalse,_angle);
        Angle clamped = ClampAngle(angle,m_template->getCircle().m_start+worldAngle,m_template->getCircle().m_end+worldAngle);

        if ( angle != clamped )
        {
            return bfalse;
        }
    }

    if ( m_template->isPoint())
    {
        f32 strength = m_template->getForce().norm();

        if ( m_gradientPercent )
        {
            f32 gradientSize = radius*m_gradientPercent;
            f32 gradientStart = radius - gradientSize;

            if ( dist >= gradientStart )
            {
                f32 t = ( dist - gradientStart ) / gradientSize;
                strength *= ( 1.f - t );
            }
        }

        _force = forceDir*strength;
    }
    else
    {
        _force = m_template->getForce().Rotate(_angle);

        if ( m_gradientPercent )
        {
            Vec2d forceDir = _force;

            forceDir.normalize();

            Vec2d gravStart = _pos - (forceDir*radius);
            Vec2d vecToPos = _checkPos - gravStart;
            f32 dist = forceDir.dot(vecToPos);
            f32 gravSize = radius*2.f;
            f32 gradientSize = gravSize*m_gradientPercent;

            ITF_ASSERT(dist>=0.f&&dist<gravSize);

            if ( dist < gradientSize )
            {
                f32 t = dist / gradientSize;
                _force *= t;
            }
        }
    }

    if ( m_template->isInverted() )
    {
        _force *= -1.f;
    }

    _speedMultiplier = Vec2d(m_template->getSpeedMultiplierX(),m_template->getSpeedMultiplierY());

    return btrue;
}

bbool PhysForceModifier_Instance::checkForceBox( const Vec2d& _pos, f32 _angle, const Vec2d& _checkPos, const Vec2d& _speed, Vec2d& _force, Vec2d& _speedMultiplier ) const
{
    f32 width = m_template->getBox().m_width*m_scale.m_x;
    f32 height = m_template->getBox().m_height*m_scale.m_y;
    f32 halfWidth = width*0.5f;
    f32 halfHeight = height*0.5f;
    f32 distSq = ( _checkPos - _pos ).sqrnorm();

    if ( distSq > ( (halfWidth*halfWidth) + (halfHeight*halfHeight) ) )
    {
        return bfalse;
    }

    Vec2d localPos = ( _checkPos - _pos ).Rotate(-_angle);
    Vec2d localSpeed = _speed.Rotate(-_angle);

    if ( localPos.m_x < -halfWidth || localPos.m_x > halfWidth )
    {
        return bfalse;
    }

    if ( localPos.m_y < -halfHeight || localPos.m_y > halfHeight )
    {
        return bfalse;
    }

    if ( m_template->isPoint() )
    {
        Vec2d forceDir = _pos - _checkPos;

        forceDir.normalize();

        f32 strength = m_template->getForce().norm();

        if ( m_gradientPercent )
        {
            f32 gradHalfWidth = halfWidth - (halfWidth*m_gradientPercent);
            f32 gradHalfHeight = halfHeight - (halfHeight*m_gradientPercent);

            if ( localPos.m_x < -gradHalfWidth || localPos.m_x > gradHalfWidth ||
                 localPos.m_y < -gradHalfHeight || localPos.m_y > gradHalfHeight )
            {
                Vec2d intersectionA,intersectionB;
                Vec2d pA(-gradHalfWidth,gradHalfHeight);
                Vec2d pB(gradHalfWidth,gradHalfHeight);
                Vec2d pC(gradHalfWidth,-gradHalfHeight);
                Vec2d pD(-gradHalfWidth,-gradHalfHeight);

                if ( Segment_Segment(Vec2d::Zero,localPos,pA,pB,intersectionA) )
                {
                    if ( intersectionLineLine(Vec2d::Zero,localPos,Vec2d(-halfWidth,halfHeight),Vec2d(halfWidth,halfHeight),intersectionB) )
                    {
                        f32 size = ( intersectionB - intersectionA ).norm();
                        f32 dist = ( localPos - intersectionB ).norm();

                        strength *= dist/size;
                    }
                }
                else if ( Segment_Segment(Vec2d::Zero,localPos,pB,pC,intersectionA) )
                {
                    if ( intersectionLineLine(Vec2d::Zero,localPos,Vec2d(halfWidth,halfHeight),Vec2d(halfWidth,-halfHeight),intersectionB) )
                    {
                        f32 size = ( intersectionB - intersectionA ).norm();
                        f32 dist = ( localPos - intersectionB ).norm();

                        strength *= dist/size;
                    }
                }
                else if ( Segment_Segment(Vec2d::Zero,localPos,pC,pD,intersectionA) )
                {
                    if ( intersectionLineLine(Vec2d::Zero,localPos,Vec2d(halfWidth,-halfHeight),Vec2d(-halfWidth,-halfHeight),intersectionB) )
                    {
                        f32 size = ( intersectionB - intersectionA ).norm();
                        f32 dist = ( localPos - intersectionB ).norm();

                        strength *= dist/size;
                    }
                }
                else if ( Segment_Segment(Vec2d::Zero,localPos,pD,pA,intersectionA) )
                {
                    if ( intersectionLineLine(Vec2d::Zero,localPos,Vec2d(-halfWidth,-halfHeight),Vec2d(-halfWidth,halfHeight),intersectionB) )
                    {
                        f32 size = ( intersectionB - intersectionA ).norm();
                        f32 dist = ( localPos - intersectionB ).norm();

                        strength *= dist/size;
                    }
                }
                else
                {
                    // It should have collided because we are supposed to be inside of the gradient
                    ITF_ASSERT(0);
                }
            }
        }

        _force = forceDir*strength;
    }
    else
    {
        _force = m_template->getForce().Rotate(_angle);

        if ( m_gradientPercent )
        {
            f32 range = width * ( 1.f - m_gradientPercent );
            f32 start = width*m_gradientPercent;
            f32 posX = localPos.m_x + halfWidth;

            if ( posX >= start && posX <= width )
            {
                posX -= start;
                f32 t = posX / range;

                _force *= (1.f-t);
            }
        }
    }

    if ( m_template->isInverted() )
    {
        _force *= -1.f;
    }

    if ( m_template->getCenterForce() && localPos.m_y )
    {
        Vec2d forceDir = _force;
        f32 scale = f32_Abs(localPos.m_y) / halfHeight;
        
        scale = Clamp(scale,0.f,1.f);
        forceDir.normalize();

        Vec2d centerForceDir = forceDir.getPerpendicular();
        
        if ( localPos.m_y > 0.f )
        {
            centerForceDir *= -1.f;
        }

        f32 speedProj = localSpeed.dot(centerForceDir);
        f32 speedScale;

        if ( speedProj <= 0.f )
        {
            speedScale = 1.f;
        }
        else if ( speedProj >= m_template->getCenterForceMaxSpeed() )
        {
            speedScale = 0.f;
        }
        else
        {
            speedScale = speedProj / m_template->getCenterForceMaxSpeed();
        }

        _force += centerForceDir * (m_template->getCenterForce() * scale * speedScale);
    }
    
    _speedMultiplier = Vec2d(m_template->getSpeedMultiplierX(),m_template->getSpeedMultiplierY());

    return btrue;
}

bbool PhysForceModifier_Instance::checkForcePolyline( const Vec2d& _pos, f32 _angle, const Vec2d& _checkPos, const Vec2d& _speed, Vec2d& _force, Vec2d& _speedMultiplier ) const
{
    if ( !m_polylineData.m_polygon.isValid() )
    {
        return bfalse;
    }

    Vec2d localPos = ( _checkPos - _pos ).Rotate(-_angle);

    if ( !m_polylineData.m_aabb.contains(localPos) )
    {
        return bfalse;
    }

    if ( !m_polylineData.m_polygon.isPointInside(localPos) )
    {
        return bfalse;
    }

    Vec2d localSpeed = _speed.Rotate(-_angle);
    Vec2d forceDir;
    f32 strength = m_template->getForce().norm();

    if ( m_template->isPoint() )
    {
        forceDir = m_polylineData.m_center - _pos;
    }
    else
    {
        forceDir = m_polylineData.m_dir.Rotate(_angle);
    }

    if ( m_template->isInverted() )
    {
        forceDir *= -1.f;
    }

    forceDir.normalize();

    _force = forceDir*strength;

    f32 distX = m_polylineData.m_dir.dot(localPos);
    f32 distY = m_polylineData.m_dir.getPerpendicular().dot(localPos);
    f32 scale;

    distX = Clamp(distX,m_polylineData.m_start,m_polylineData.m_end);

    if ( m_gradientPercent > 0.f && m_gradientPercent < 1.f && m_polylineData.m_end > m_polylineData.m_start )
    {
        f32 range = m_polylineData.m_end - m_polylineData.m_start;
        f32 t = ( distX - m_polylineData.m_start ) / range;

        if ( t < m_gradientPercent )
        {
            scale = 1.f;
        }
        else
        {
            t = ( t - m_gradientPercent ) / ( 1.f - m_gradientPercent );
            t = Clamp(t,0.f,1.f);
            scale = Interpolate(1.f,0.f,t);
        }
    }
    else
    {
        scale = 1.f;
    }

    //DebugDraw::text(_checkPos, 0, Color::red(), "t %.2f", scale);

    _force *= scale;

    if ( m_template->getCenterForce() && distY )
    {
        Vec2d centerForceDir = m_polylineData.m_dir.getPerpendicular();

        if ( distY > 0.f )
        {
            centerForceDir *= -1.f;
        }

        f32 speedProj = localSpeed.dot(centerForceDir);
        f32 speedScale;

        if ( speedProj <= 0.f )
        {
            speedScale = 1.f;
        }
        else if ( speedProj >= m_template->getCenterForce() )
        {
            speedScale = 0.f;
        }
        else
        {
            speedScale = speedProj / m_template->getCenterForceMaxSpeed();
        }

        _force += centerForceDir * (m_template->getCenterForce() * f32_Abs(distY) * speedScale);
    }

    _speedMultiplier = Vec2d(m_template->getSpeedMultiplierX(),m_template->getSpeedMultiplierY());

    return btrue;
}

// _force2Speed: multiplier to transform the force into speed
// _speedLimit: scale the total force based on this limiter
// _maxOppScale: scale factor when the speed goes opposite of the force
// _currentSpeed: current speed of the character
// _force: wind force that will be modified as a result
void PhysForceModifier::applySpeedLimitation( f32 _force2Speed, f32 _speedLimit, f32 _maxOppScale, const Vec2d& _currentSpeed, Vec2d& _force )
{
    Vec2d forceSpeed = _force * _force2Speed;
    Vec2d forceDir = forceSpeed;
    f32 forceSpeedVal = forceSpeed.norm();

    forceDir.normalize();

    f32 speedProj = forceDir.dot(_currentSpeed);

    if ( speedProj >= _speedLimit )
    {
        _force = Vec2d::Zero;
        return;
    }

    f32 dif = _speedLimit - speedProj;
    f32 t = dif / _speedLimit;

    if ( speedProj < 0.f )
    {
        t = Clamp(t,0.f,_maxOppScale);
        t += 1.f;
    }
    else
    {
        t = Clamp(t,0.f,1.f);
    }

    _force = forceDir * ( (forceSpeedVal*t) / _force2Speed );

    return;
}

#ifdef ITF_SUPPORT_DEBUGFEATURE
void PhysForceModifier_Instance::drawDebug( const Vec2d& _pos, f32 _angle, f32 z ) const
{
    const Vec2d posWithOffSet = (_pos + m_template->getOffset().Rotate(_angle));
    const Vec3d pos = posWithOffSet.to3d(z);

    Angle worldAngle = Angle(bfalse,_angle)+m_template->getRotation();
    switch(m_template->getType())
    {
    case PhysForceModifier::TYPE_CIRCLE:
        {
            f32 radius = m_template->getCircle().m_radius * m_scale.m_x;

            GFX_ADAPTER->drawDBGCircle(pos, radius, 0.f, 1.f, 0.f);

            if ( m_template->getCircle().m_start != m_template->getCircle().m_end )
            {
                Vec2d dir = (m_template->getCircle().m_start+worldAngle).ToVector();
                Vec3d offset = (dir*radius).to3d();

                GFX_ADAPTER->drawDBG3DLine(pos, pos+offset, 0.f, 1.f, 0.f);

                dir = (m_template->getCircle().m_end+worldAngle).ToVector();

                GFX_ADAPTER->drawDBG3DLine(pos, pos+offset, 1.f, 0.f, 0.f);
            }

            if ( !m_template->isPoint() )
            {
                Vec2d grav = m_template->getForce();

                grav.normalize();

                grav = grav.Rotate(worldAngle.ToRadians());

                Vec3d offset = (grav*radius*0.5f).to3d();

                GFX_ADAPTER->drawDBG3DLine(pos, pos+offset, 0.f, 0.f, 1.f);

                if ( m_gradientPercent )
                {
                    Vec2d gravDir = grav;
                    f32 gravAngle = getVec2Angle(gravDir);

                    f32 diameter = radius*2.f;
                    f32 gradientSize = diameter*m_gradientPercent;

                    f32 forceEnd = radius;
                    f32 forceStart = forceEnd - ( diameter - gradientSize );

                    f32 angle = f32_ACos(forceStart/radius);

                    Vec2d posA = (Vec2d::Right*radius).Rotate(angle);
                    Vec2d posB = posA - ( Vec2d::Up * posA.m_y * 2.f );

                    posA = posA.Rotate(gravAngle) + _pos;
                    posB = posB.Rotate(gravAngle) + _pos;

                    GFX_ADAPTER->drawDBG3DLine(posA.to3d(z), posB.to3d(z), 1.f, 1.f, 0.f);
                }
            }
            else
            {
                if ( m_gradientPercent )
                {
                    f32 gradientSize = radius*m_gradientPercent;
                    f32 gradientStart = radius - gradientSize;

                    GFX_ADAPTER->drawDBGCircle(pos.m_x, pos.m_y, gradientStart, 1.f, 1.f, 0.f);
                }
            }
        }

        break;
    case PhysForceModifier::TYPE_BOX:
        {
            f32 width = m_template->getBox().m_width * m_scale.m_x;
            f32 height = m_template->getBox().m_height * m_scale.m_y;
            Vec2d xDir = worldAngle.ToVector();
            Vec2d yDir = xDir.getPerpendicular();
            Vec2d pA = posWithOffSet - (xDir*width*0.5f) + (yDir*height*0.5f);
            Vec2d pB = pA + (xDir*width);
            Vec2d pC = pB - (yDir*height);
            Vec2d pD = pC - (xDir*width);

            GFX_ADAPTER->drawDBG3DLine(pA.to3d(z), pB.to3d(z), 0.f, 1.f, 0.f);
            GFX_ADAPTER->drawDBG3DLine(pB.to3d(z), pC.to3d(z), 0.f, 1.f, 0.f);
            GFX_ADAPTER->drawDBG3DLine(pC.to3d(z), pD.to3d(z), 0.f, 1.f, 0.f);
            GFX_ADAPTER->drawDBG3DLine(pD.to3d(z), pA.to3d(z), 0.f, 1.f, 0.f);

            if ( !m_template->isPoint() )
            {
                Vec2d grav = m_template->getForce();

                grav.normalize();

                grav = grav.Rotate(worldAngle.ToRadians());

                Vec3d offset = (grav*height*0.5f).to3d();

                GFX_ADAPTER->drawDBG3DLine(pos, pos+offset, 0.f, 0.f, 1.f);

                if ( m_gradientPercent )
                {
                    Vec2d pA((width*m_gradientPercent)-(width*0.5f),height*0.5f);
                    Vec2d pB(pA.m_x,-height*0.5f);

                    pA = pA.Rotate(worldAngle.ToRadians()) + posWithOffSet;
                    pB = pB.Rotate(worldAngle.ToRadians()) + posWithOffSet;

                    GFX_ADAPTER->drawDBG3DLine(pA.to3d(z), pB.to3d(z), 1.f, 1.f, 0.f);
                }
            }
            else
            {
                if ( m_gradientPercent )
                {
                    f32 gradientWidth = width - (width*m_gradientPercent);
                    f32 gradientHeight = height - (height*m_gradientPercent);
                    Vec2d pA = posWithOffSet - (xDir*gradientWidth*0.5f) + (yDir*gradientHeight*0.5f);
                    Vec2d pB = pA + (xDir*gradientWidth);
                    Vec2d pC = pB - (yDir*gradientHeight);
                    Vec2d pD = pC - (xDir*gradientWidth);

                    GFX_ADAPTER->drawDBG3DLine(pA.to3d(z), pB.to3d(z), 1.f, 1.f, 0.f);
                    GFX_ADAPTER->drawDBG3DLine(pB.to3d(z), pC.to3d(z), 1.f, 1.f, 0.f);
                    GFX_ADAPTER->drawDBG3DLine(pC.to3d(z), pD.to3d(z), 1.f, 1.f, 0.f);
                    GFX_ADAPTER->drawDBG3DLine(pD.to3d(z), pA.to3d(z), 1.f, 1.f, 0.f);
                }
            }
        }
        break;
    case PhysForceModifier::TYPE_POLYLINE:
        if ( m_polylineData.m_polygon.isValid() )
        {
            f32 angle = worldAngle.ToRadians();
            DebugDraw::AABB(pos, angle, m_polylineData.m_aabb);
            DebugDraw::shape(pos, angle, &m_polylineData.m_polygon);

            const Vec3d dir = m_polylineData.m_dir.Rotate(angle).to3d();
            Vec3d startPos = pos + dir * m_polylineData.m_start;
            GFX_ADAPTER->drawArrow(startPos, startPos + dir, 1.f, 0.f, 0.f, 1.0f);

            if ( m_gradientPercent > 0.f && m_gradientPercent < 1.f && m_polylineData.m_end > m_polylineData.m_start )
            {
                Vec3d endPos = pos + dir * m_polylineData.m_end;

                f32 gradientLength = m_polylineData.m_start + (m_polylineData.m_end - m_polylineData.m_start) * m_gradientPercent;
                Vec3d gradientPos = pos + dir * gradientLength;

                f32 debugWidth = 1.f;
                bbool aligned = bfalse;
                if (fabs(m_polylineData.m_dir.m_x) < MTH_EPSILON)
                {
                    // aligned with Y
                    debugWidth = m_polylineData.m_aabb.getWidth();
                    aligned = btrue;
                }
                else if (fabs(m_polylineData.m_dir.m_y) < MTH_EPSILON)
                {
                    // aligned with X
                    debugWidth = m_polylineData.m_aabb.getHeight();
                    aligned = btrue;
                }

                Vec3d perp = dir.getPerpendicular() * debugWidth * 0.5f;

                if (aligned)
                {
                    DebugDraw::text(startPos + perp, Color::yellow(), "1");
                    DebugDraw::text(gradientPos + perp, Color::yellow(), "1");
                    DebugDraw::text(endPos + perp, Color::yellow(), "0");
                    DebugDraw::line3D(gradientPos - perp, gradientPos + perp, Color::yellow(), 5.f);
                }
                else
                {
                    DebugDraw::text(startPos, Color::yellow(), "1");
                    DebugDraw::line3D(startPos - perp, startPos + perp, Color::yellow(), 2.f);
                    DebugDraw::text(gradientPos, Color::yellow(), "1");
                    DebugDraw::line3D(gradientPos - perp, gradientPos + perp, Color::yellow(), 5.f);
                    DebugDraw::text(endPos, Color::yellow(), "0");
                    DebugDraw::line3D(endPos - perp, endPos + perp, Color::yellow(), 2.f);
                }
            }
        }
        break;
    }
}
#endif // ITF_SUPPORT_DEBUGFEATURE

}

