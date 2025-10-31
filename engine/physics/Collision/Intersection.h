
#ifndef _ITF_INTERSECTION_H_
#define _ITF_INTERSECTION_H_

namespace ITF
{

namespace IntersectionMath
{
    bbool intersectTriangleWithCircle( const Vec2d& _t0, const Vec2d& _t1, const Vec2d& _t2, const Vec2d& _c, f32 _radius );
    bbool intersectCapsuleWithTriangle( const Vec2d& _t0, const Vec2d& _t1, const Vec2d& _t2, const Vec2d& _p0, const Vec2d& _p1, f32 _radius, f32& _t );

    // important: triangle coordinates in local space
    bbool intersectTriangleWithRectangle( const Vec2d& _t0, const Vec2d& _t1, const Vec2d& _t2, f32 _width, f32 _halfHeight );
    
    // important: line points in local space
    bbool intersectLineWithRectangle( const Vec2d& _p0, const Vec2d& _p1, f32 _l, f32 _r, f32 _t, f32 _b );

    bbool intersectMovingCircleWithSegment( const Vec2d& _p0, const Vec2d& _p1, const Vec2d& _pDir, f32 _pSize,
                                            const Vec2d& _circle0, const Vec2d& _circle1, const Vec2d& _circleDir, f32 _circleMoveSize, f32 _circleRadius,
                                            f32& _t, Vec2d& _circlePos, Vec2d& _pPos, Vec2d& _normal );

    bbool intersectCircleWithSegment( const Vec2d& _circle, f32 _radius,
                                      const Vec2d& _p0, const Vec2d& _pDir, f32 _pSize,
                                      Vec2d& _pPos, Vec2d& _normal );

}

}

#endif  // _ITF_INTERSECTION_H_