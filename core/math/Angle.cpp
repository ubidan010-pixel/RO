
#include "precompiled_core.h"

#ifndef _ITF_ANGLE_H_
#include "core/math/Angle.h"
#endif // _ITF_ANGLE_H_

namespace ITF
{

Angle ClampAngle( const Angle& _angle, const Angle& _min, const Angle& _max )
{
    f32 difMinMax;
    f32 _minRad = _min.ToRadians();
    f32 _maxRad = _max.ToRadians();

    NormalizeAngle2PI(_minRad);
    NormalizeAngle2PI(_maxRad);

    if ( _minRad < _maxRad )
    {
        difMinMax = _maxRad - _minRad;
    }
    else
    {
        difMinMax = _maxRad + ( MTH_2PI - _minRad );
    }

    f32 minAngle = 0.f;
    f32 maxAngle = difMinMax;
    f32 currentAngle = _angle.ToRadians();

    currentAngle -= _minRad;

    NormalizeAngle2PI(currentAngle);
    
    if ( minAngle < maxAngle )
    {
        if ( currentAngle >= minAngle && currentAngle < maxAngle )
        {
            return _angle;
        }
    }
    else
    {
        if ( currentAngle <= minAngle && currentAngle >= maxAngle )
        {
            return _angle;
        }
    }

    f32 difMin = Angle::GetShortestDif(_min,_angle).ToRadians();
    f32 difMax = Angle::GetShortestDif(_max,_angle).ToRadians();

    if ( f32_Abs(difMin) < f32_Abs(difMax) )
    {
        return _min;
    }
    else
    {
        return _max;
    }
}

}

