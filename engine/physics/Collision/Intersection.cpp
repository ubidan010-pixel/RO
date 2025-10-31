#include "precompiled_engine.h"

#ifndef _ITF_INTERSECTION_H_
#include "engine/physics/Collision/Intersection.h"
#endif //_ITF_INTERSECTION_H_

#ifndef _ITF_TRANSFORM_H_
#include "core/math/transform.h"
#endif //_ITF_TRANSFORM_H_

namespace ITF
{

namespace IntersectionMath
{
    bbool isPointInTriangle( const Vec2d& _t0, const Vec2d& _t1, const Vec2d& _t2, const Vec2d& _p )
    {
        if ( (_p - _t0).cross(_t1 - _t0) < 0.f )
        {
            return bfalse;
        }

        if ( (_p - _t1).cross(_t2 - _t1) < 0.f )
        {
            return bfalse;
        }

        if ( (_p - _t2).cross(_t0 - _t2) < 0.f )
        {
            return bfalse;
        }

        return btrue;
    }

    bbool intersectTriangleWithCircle( const Vec2d& _t0, const Vec2d& _t1, const Vec2d& _t2, const Vec2d& _c, f32 _radius )
    {
        if ( isPointInTriangle(_t0,_t1,_t2,_c) )
        {
            return btrue;
        }

        f32 radiusSq = _radius*_radius;
        Vec2d t1_t0 = _t1 - _t0;
        f32 t = (_c - _t0).dot(t1_t0) / t1_t0.dot(t1_t0);

        t = Clamp(t,0.f,1.f);

        Vec2d pointInSeg = _t0 + (t1_t0*t);
        f32 dist = ( _c - pointInSeg ).sqrnorm();

        if ( dist <= radiusSq )
        {
            return btrue;
        }

        Vec2d t2_t1 = _t2 - _t1;

        t = (_c - _t1).dot(t2_t1) / t2_t1.dot(t2_t1);
        t = Clamp(t,0.f,1.f);

        pointInSeg = _t1 + (t2_t1*t);
        dist = ( _c - pointInSeg ).sqrnorm();

        if ( dist <= radiusSq )
        {
            return btrue;
        }

        Vec2d t0_t2 = _t0 - _t2;

        t = (_c - _t2).dot(t0_t2) / t0_t2.dot(t0_t2);
        t = Clamp(t,0.f,1.f);

        pointInSeg = _t2 + (t0_t2*t);
        dist = ( _c - pointInSeg ).sqrnorm();

        if ( dist <= radiusSq )
        {
            return btrue;
        }

        return bfalse;
    }

    // _t0, _t1, _t2: triangle points
    // _p0, _p1: capsule start and end
    // _radius: radius of the capsule
    // _contact: resulting contact
    bbool intersectCapsuleWithTriangle( const Vec2d& _t0, const Vec2d& _t1, const Vec2d& _t2, const Vec2d& _p0, const Vec2d& _p1, f32 _radius, f32& _t )
    {
        // First test one circle
        if ( intersectTriangleWithCircle(_t0,_t1,_t2,_p0,_radius) )
        {
            _t = 0.f;
            return btrue;
        }

        // Then test the other
        if ( intersectTriangleWithCircle(_t0,_t1,_t2,_p1,_radius) )
        {
            _t = 1.f;
            return btrue;
        }

        // And finally test the rectangle with the triangle
        // for this, put the triangle in local coordinates to the rectangle
        Vec2d rectVec = _p1 - _p0;
        Vec2d rectDir = rectVec; rectDir.normalize();
        f32 rectSize = rectVec.norm();

        transform2d xf;

        xf.m_pos = _p0;
        xf.m_rot.m_col1 = rectDir;
        xf.m_rot.m_col2 = xf.m_rot.m_col1.getPerpendicular();

        Vec2d t0 = mulTransform2dT(xf,_t0);
        Vec2d t1 = mulTransform2dT(xf,_t1);
        Vec2d t2 = mulTransform2dT(xf,_t2);

        if ( intersectTriangleWithRectangle(t0,t1,t2,rectSize,_radius) )
        {
            _t = 0.5f;
            return btrue;
        }

        return bfalse;
    }

    // important: triangle coordinates in local space
    // http://wonderfl.net/c/fAsW/
    bbool intersectTriangleWithRectangle( const Vec2d& _t0, const Vec2d& _t1, const Vec2d& _t2, f32 _width, f32 _halfHeight )
    {
        f32 l = 0.f;
        f32 r = _width;
        f32 t = _halfHeight; 
        f32 b = -_halfHeight;

        //if all points of the triangle are on one side of the rect 
        if( (l>_t0.m_x && l>_t1.m_x && l>_t2.m_x) || (r<_t0.m_x && r<_t1.m_x && r<_t2.m_x) || (t<_t0.m_y && t<_t1.m_y && t<_t2.m_y) || (b>_t0.m_y && b>_t1.m_y && b>_t2.m_y) )
        {
            return false;
        }

        //if there is a point of the triangle inside the rect
        if( (l<_t0.m_x && _t0.m_x<r && t>_t0.m_y && _t0.m_y<b) || (l<_t1.m_x && _t1.m_x<r && t>_t1.m_y && _t1.m_y>b) || (l<_t2.m_x && _t2.m_x<r && t>_t2.m_y && _t2.m_y>b) )
        {
            return true;
        }

        return intersectLineWithRectangle(_t0,_t1,l,r,t,b) ||
               intersectLineWithRectangle(_t1,_t2,l,r,t,b) ||
               intersectLineWithRectangle(_t2,_t0,l,r,t,b);
    }

    // important: line points in local space
    // http://wonderfl.net/c/fAsW/
    bbool intersectLineWithRectangle( const Vec2d& _p0, const Vec2d& _p1, f32 _l, f32 _r, f32 _t, f32 _b )
    {
        // Calculate m and c for the equation for the line (y = mx+c)
        f32 m = (_p1.m_y-_p0.m_y) / (_p1.m_x-_p0.m_x); 
        f32 c = _p0.m_y -(m*_p0.m_x);
        f32 top_intersection, bottom_intersection;
        f32 toptrianglepoint, bottomtrianglepoint;

        // if the line is going up from right to left then the top intersect point is on the left
        if(m>0)
        {
            top_intersection = (m*_l  + c); 
            bottom_intersection = (m*_r  + c);
        } 
        // otherwise it's on the right
        else 
        {
            top_intersection = (m*_r  + c); 
            bottom_intersection = (m*_l  + c); 
        }

        // work out the top and bottom extents for the triangle
        if(_p0.m_y<_p1.m_y)
        {
            toptrianglepoint = _p0.m_y;
            bottomtrianglepoint = _p1.m_y;
        }
        else
        {
            toptrianglepoint = _p1.m_y; 
            bottomtrianglepoint = _p0.m_y; 
        }

        f32 topoverlap,botoverlap;

        // and calculate the overlap between those two bounds
        topoverlap = top_intersection>toptrianglepoint ? top_intersection : toptrianglepoint; 
        botoverlap = bottom_intersection<bottomtrianglepoint ? bottom_intersection : bottomtrianglepoint; 

        // (topoverlap<botoverlap) : 
        // if the intersection isn't the right way up then we have no overlap

        // (!((botoverlap<t) || (topoverlap>b)) :
        // If the bottom overlap is higher than the top of the rectangle or the top overlap is 
        // lower than the bottom of the rectangle we don't have intersection. So return the negative 
        // of that. Much faster than checking each of the points is within the bounds of the rectangle. 
        return (topoverlap<botoverlap) && (!((botoverlap<_t) || (topoverlap>_b)));
    }

    void intersectEndPointsMovingCircleEdge( const Vec2d& _circle0, const Vec2d& _circleDir, f32 _circleMoveSize, f32 _radiusSq, const Vec2d& intersection,
                                             f32& _t, Vec2d& _circlePos, Vec2d& _pPos, Vec2d& _normal )
    {
        Vec2d vecToPos = intersection - _circle0;
        f32 dist = _circleDir.dot(vecToPos);
        Vec2d p1 = _circle0 + (_circleDir * dist);
        dist = (intersection - p1).norm();

        float back = f32_Sqrt(Max(0.f,_radiusSq - (dist * dist)));

        _circlePos = p1 - (_circleDir * back);
        _pPos = intersection;
        _t = (_circlePos - _circle0).norm() / _circleMoveSize;
        _normal = _pPos - _circlePos;
        _normal.normalize();
    }

    bbool intersectMovingCircleWithSegment( const Vec2d& _p0, const Vec2d& _p1, const Vec2d& _pDir, f32 _pSize,
                                            const Vec2d& _circle0, const Vec2d& _circle1, const Vec2d& _circleDir, f32 _circleMoveSize, f32 _circleRadius,
                                            f32& _t, Vec2d& _circlePos, Vec2d& _pPos, Vec2d& _normal )
    {
        // First a simple test, see if we are already inside the edge
        Vec2d vecToPos = _circle0 - _p0;
        f32 distVecToPos = _pDir.dot(vecToPos);
        Vec2d contactOnEdge = _p0 + ( _pDir * Clamp(distVecToPos,0.f,_pSize) );
        Vec2d p1C = contactOnEdge - _circle0;
        f32 dist = p1C.sqrnorm();
        f32 radiusSq = _circleRadius*_circleRadius;

        if ( dist <= radiusSq )
        {
            _t = 0.f;
            _circlePos = _circle0;
            _pPos = contactOnEdge;
            _normal = p1C;
            _normal.normalize();

            return btrue;
        }

        Vec2d intersection;
        Vec2d aC;
        Vec2d p2;
        f32 p2Dist;

        // Check if we cross the edge in our path
        if ( Line_Line(_circle0,_circleDir,_p0,_pDir,intersection) )
        {
            aC = _circle0 - intersection;
            contactOnEdge = _p0 + ( _pDir * distVecToPos );
            p1C = _circle0 - contactOnEdge;
            f32 p1Csize = p1C.norm();

            if ( f32_Abs(p1Csize) <= MTH_EPSILON )
            {
                p2 = intersection - (_circleDir*_circleRadius);
            }
            else
            {
                p2 = intersection - ( _circleDir * (aC.norm() / p1C.norm()) * _circleRadius );
            }

            p2Dist = (p2 - _circle0).dot(_circleDir);

            if (p2Dist >= -MTH_EPSILON && p2Dist <= _circleMoveSize)
            {
                Vec2d v = p2 - _p0;
                f32 d = _pDir.dot(v);

                if (d >= 0 && d <= _pSize)
                {
                    _circlePos = p2;
                    _pPos = _p0 + ( _pDir * d );
                    _t = (_circlePos - _circle0).norm() / _circleMoveSize;
                    _normal = _pPos - _circlePos;
                    _normal.normalize();
                    return btrue;
                }
            }
        }

        Vec2d lineClose, lineFar;

        // Fine who is closer, the start of the edge or the end
        if ( vecToPos.sqrnorm() < ( _p1 - _circle0 ).sqrnorm() )
        {
            lineClose = _p0;
            lineFar = _p1;
        }
        else
        {
            lineClose = _p1;
            lineFar = _p0;
        }

        // Test if any point of the edge is inside the circle trajectory
        vecToPos = lineClose - _circle0;
        dist = _circleDir.dot(vecToPos);
        dist = Clamp(dist,0.f,_circleMoveSize);
        Vec2d p = _circle0 + (_circleDir * dist);
        dist = (p - lineClose).sqrnorm();

        if (dist <= radiusSq)
        {
            intersectEndPointsMovingCircleEdge(_circle0,_circleDir,_circleMoveSize,radiusSq,lineClose,_t,_circlePos,_pPos,_normal);
            return btrue;
        }

        // Test the second point for the same
        vecToPos = lineFar - _circle0;
        dist = _circleDir.dot(vecToPos);
        dist = Clamp(dist,0.f,_circleMoveSize);
        p = _circle0 + (_circleDir * dist);
        dist = (p - lineFar).sqrnorm();

        if (dist <= radiusSq)
        {
            intersectEndPointsMovingCircleEdge(_circle0,_circleDir,_circleMoveSize,radiusSq,lineFar,_t,_circlePos,_pPos,_normal);
            return btrue;
        }

        return bfalse;
    }

    bbool intersectCircleWithSegment( const Vec2d& _circle, f32 _radius,
                                      const Vec2d& _p0, const Vec2d& _pDir, f32 _pSize,
                                      Vec2d& _pPos, Vec2d& _normal )
    {
        Vec2d vecToPos = _circle - _p0;
        f32 dist = _pDir.dot(vecToPos);
        dist = Clamp(dist,0.f,_pSize);
        Vec2d contactOnEdge = _p0 + ( _pDir * dist );
        dist = ( _circle - contactOnEdge ).sqrnorm();

        if ( dist <= (_radius*_radius) )
        {
            _pPos = contactOnEdge;
            _normal = _pPos - _circle;
            _normal.normalize();
            return btrue;
        }
        else
        {
            return bfalse;
        }
    }
}

}

