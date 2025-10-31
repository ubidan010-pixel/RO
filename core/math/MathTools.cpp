#include "precompiled_core.h"

#ifndef _ITF_VEC2D_H_
#include "core/math/Vec2d.h"
#endif //_ITF_VEC2D_H_

#ifndef _ITF_MATHTOOLS_H_
#include "core/math/MathTools.h"
#endif //_ITF_MATHTOOLS_H_

#ifndef _ITF_ANGLE_H_
#include "core/math/Angle.h"
#endif // _ITF_ANGLE_H_

#ifndef _ITF_MATRIX44_H_
#include "core/math/matrix44/matrix44.h"
#endif //_ITF_MATRIX44_H_

#include <float.h>

namespace ITF
{
#ifdef _x86
const Matrix44 ALIGN16(MatrixIdentity(1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f ,0.f, 0.f, 1.f));
#else
const Matrix44 MatrixIdentity(1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f ,0.f, 0.f, 1.f);
#endif
const GMatrix44 GMatrixIdentity(1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, 0.f ,0.f, 0.f, 1.f);
	
const f64 EPSILON14    = 0.0000001;
const f64 EPSILON3     = 0.0001;

const Angle Angle::Zero(bfalse,0.f);
const Angle Angle::Angle360(bfalse,MTH_2PI);
const Angle Angle::Angle180(bfalse,MTH_PI);


#ifdef F32_WII

f32 f32_Cos(register f32 _float)
{
    return cosf_a(modulo2Pi(_float));
}

f32 f32_Sin(register f32 _float)
{
    return sinf_a(modulo2Pi(_float));
}

void CosSin_0_2pi_a(register f32 _float,register  f32 *_pf_Cos,register  f32 *_pf_Sin)
{
    register f32 angle = modulo2Pi(_float);

    register __vec2x32float__   aa;
    register __vec2x32float__   result;
    asm
    {
        ps_merge00 aa, angle, angle
    }
    result = sincosf_a(aa);
    asm
    {
 		psq_st     result, 0(_pf_Sin), 1, 0
        ps_merge10 aa, result, result
 		psq_st     aa, 0(_pf_Cos), 1, 0
    }
}

void CosSin_0_2pi_f(register f32 _float,register  f32 *_pf_Cos,register  f32 *_pf_Sin)
{
    register f32 angle = modulo2Pi(_float);

    register __vec2x32float__   aa;
    register __vec2x32float__   result;
    asm
    {
        ps_merge00 aa, angle, angle
    }
    result = sincosf_f(aa);
    asm
    {
 		psq_st     result, 0(_pf_Sin), 1, 0
        ps_merge10 aa, result, result
 		psq_st     aa, 0(_pf_Cos), 1, 0
    }
}

f32 testAngle(Vec2d const& _v)
{
    return _v.getAngleOpt();
}

using ps::Float2;

namespace constants
{
    #define PS_2PI     6.283185307f
    #define PS_PI      3.141592654f
    #define PS_PIDIV2  1.570796327f
    #define PS_PIDIV3  1.047197551f
    #define PS_PIDIV4  0.785398163f
    #define PS_INV_PI  0.318309886f
    #define PS_INV_2PI 0.159154943f

    float acos1[2] = { -0.0187293f, 0.0742610f };
    float acos2[2] = { -0.2121144f, 1.5707288f };
    float acos3[2] = { PS_PI, 1.0f };
}

f32 acosOpt(register float _x)
{
    Float2 cst1 = ps::Load(constants::acos1);
    Float2 cst2 = ps::Load(constants::acos2);
    Float2 cst3 = ps::Load(constants::acos3);

    Float2 x   = ps::Abs(_x);
    Float2 sqr = ps::SqrtEstimate(ps::SplatY(cst3) - x);
    Float2 tmp = ps::MaddX(x, cst1, ps::SplatY(cst1));
           tmp = ps::Madd(tmp, x, ps::SplatX(cst2));
           tmp = ps::Madd(tmp, x, ps::SplatY(cst2));
    Float2 res = ps::Mul(tmp, sqr);
    Float2 neg = ps::Nmsub(tmp, sqr, ps::SplatX(cst3));
           res = ps::Select(_x, res, neg);

    return ps::GetX(res);
}

#endif // F32_WII

// Computes closest points C1 and C2 of S1(s)=P1+s*(Q1-P1) and
// S2(t)=P2+t*(Q2-P2), returning s and t. Function result is squared
// distance between between S1(s) and S2(t)
f32 Segment_Segment_Dist (const Vec2d& segment0p0, const Vec2d& segment0p1, const Vec2d& segment1p0, const Vec2d& segment1p1,
                            Vec2d* closest0 , Vec2d* closest1 )
{
    ITF_UNUSED(EPSILON3);
    Vec2d d1 = segment0p1 - segment0p0; // Direction vector of segment S1
    Vec2d d2 = segment1p1 - segment1p0; // Direction vector of segment S2
    Vec2d r  = segment0p0 - segment1p0;

    f32 a = d1.sqrnorm();
    f32 e = d2.sqrnorm(); // Squared length of segment S2, always nonnegative
    f32 f = d2.dot(r);

      f64 s, t;
      Vec2d c1, c2;

    // Check if either or both segments degenerate into points
    if (a <= EPSILON14 && e <= EPSILON14) {
        // Both segments degenerate into points
        s = t = 0.0f;
        c1 = segment0p0;
        c2 = segment1p0;

        if ( closest0 )
        {
            *closest0 = c1;
        }

        if ( closest1 )
        {
            *closest1 = c2;
        }

        return (c1 - c2).sqrnorm();
    }
    if (a <= EPSILON14) {
        // First segment degenerates into a point
        s = 0.0f;
        t = f * f32_Inv(e); // s = 0 => t = (b*s + f) / e = f / e
        t = Clamp(t, 0.0, 1.0);
    } else {
        f32 c = d1.dot(r);
        if (e <= EPSILON14) {
            // Second segment degenerates into a point
            t = 0.0;
            s = Clamp(-c * f32_Inv(a), 0.f, 1.f); // t = 0 => s = (b*t - c) / a = -c / a
        } else {
            // The general nondegenerate case starts here
                  f32 b = d1.dot(d2);
            f64 denom = a*e-b*b; // Always nonnegative

            // If segments not parallel, compute closest point on L1 to L2, and
            // clamp to segment S1. Else pick arbitrary s (here 0)
            if (denom != 0.0) {
                s = Clamp(static_cast<f64>(b*f - c*e) / denom, 0.0, 1.0);
            } else s = 0.0;

            // Compute point on L2 closest to S1(s) using
            // t = Dot((P1+D1*s)-P2,D2) / Dot(D2,D2) = (b*s + f) / e
                  f64 tnom = b*s + f;
                  if (tnom < 0.0) {
                        t = 0.0;
                        s = Clamp(-c * f32_Inv(a), 0.f, 1.f);
                  } else if (tnom > e) {
                        t = 1.0;
                        s = Clamp((b - c) * f32_Inv(a), 0.f, 1.f);
                  } else {
                        t = tnom * f32_Inv(e);
                  }
            }
      }

    c1 = segment0p0 + d1 * (f32)s;
    c2 = segment1p0 + d2 * (f32)t;
//    return f32_Sqrt((c1 - c2).sqrnorm());

    if ( closest0 )
    {
        *closest0 = c1;
    }

    if ( closest1 )
    {
        *closest1 = c2;
    }

    return (c1 - c2).sqrnorm();
}


// convention:
// _s1p1: 1st point of line to be collided (projection support)
// _s1p2: 2nd point of line to be collided (projection support)
// _s2p1: origin point of moving sphere
// _s2p2: destination of moving sphere
int SegmentCollision(const Vec2d& _s1p1, const Vec2d& _s1p2, const Vec2d& _s2p1, const Vec2d& _s2p2, f32& _correction)
{
    Vec2d s1V = _s1p2 - _s1p1;
    Vec2d s1N = s1V;
    s1N.normalize();

    Vec2d s1Normal;
    Vec2d::SwapXmY(&s1Normal, &s1N); // -y, x

    Vec2d v1 = _s2p1 - _s1p1;
    f32 dot1 = Vec2d::Dot(&v1, &s1Normal);;
    Vec2d v2 = _s2p2 - _s1p1;
    f32 dot2 = Vec2d::Dot(&v2, &s1Normal);

    if ((dot1 >= 0.f) && (dot2 >= 0.f))
        return 0;

    if ((dot1 <= 0.f) && (dot2 <= 0.f))
        return 0;

    _correction = dot2;
    return 1;
}

/*
f64 Distance::Point_Segment(Point& p, Point& ps, Point& pe) 
{
      if (ps.x==pe.x && ps.y==pe.y) return Distance::Point_Point(ps,p);

      f64 sx=pe.x-ps.x;
      f64 sy=pe.y-ps.y;
 
      f64 ux=p.x-ps.x;
      f64 uy=p.y-ps.y;
 
      f64 dp=sx*ux+sy*uy;
      if (dp<0) return Distance::Point_Point(ps,p);
 
      f64 sn = sx*sx+sy*sy;
      if (dp>sn) return Distance::Point_Point(pe,p);
 
      f64 ratio = (f64)dp/sn;
      f64 projx = (f64)(ps.x + ratio*sx);
      f64 projy = (f64)(ps.y + ratio*sy);
 
      return Distance::Point_Point(Point(projx,projy),p);
}
*/
//////////////////////////////////////////////////////////////////////
//    AB = A + r(B-A)   r dans [0,1]
//    CD = C + s(D-C)   s dans [0,1]
//    Point* p = new Point(); // pour utiliser p
//////////////////////////////////////////////////////////////////////

bbool Segment_Segment(const Vec2d& _pa, const Vec2d& _pb, const Vec2d& _pc, const Vec2d& _pd, Vec2d& _collision, f32* _t )
{
      f32 xa = _pa.m_x;
      f32 ya = _pa.m_y;
      f32 xb = _pb.m_x;
      f32 yb = _pb.m_y;
      f32 xc = _pc.m_x;
      f32 yc = _pc.m_y;
      f32 xd = _pd.m_x;
      f32 yd = _pd.m_y;

      f32 den = ((xb-xa)*(yd-yc)-(yb-ya)*(xd-xc));

      if ( !den )
      {
          return bfalse;
      }

      f32 invden = f32_Inv(den);
      f32 r = ((ya-yc)*(xd-xc)-(xa-xc)*(yd-yc)) * invden;
      f32 s = ((ya-yc)*(xb-xa)-(xa-xc)*(yb-ya)) * invden;

      if ( 0.f<=r && r<=1.f && 0.f<=s && s<=1.f )
      {
          _collision.m_x = xa+r*(xb-xa);
          _collision.m_y = ya+r*(yb-ya);

          if ( _t )
          {
              *_t = r;
          }

          return btrue;
      }
    return bfalse;
}

bbool Segment_Segment(Vec2d& _pa, Vec2d& _pb, Vec2d& _pc, Vec2d& _pd, Vec2d& _collision, f32 _epsilon )
{
    f32 xa = _pa.m_x;
    f32 ya = _pa.m_y;
    f32 xb = _pb.m_x;
    f32 yb = _pb.m_y;
    f32 xc = _pc.m_x;
    f32 yc = _pc.m_y;
    f32 xd = _pd.m_x;
    f32 yd = _pd.m_y;

    f32 den = ((xb-xa)*(yd-yc)-(yb-ya)*(xd-xc));

    if ( !den )
    {
        return bfalse;
    }

    f32 invden = f32_Inv(den);
    f32 r = ((ya-yc)*(xd-xc)-(xa-xc)*(yd-yc)) * invden;
    f32 s = ((ya-yc)*(xb-xa)-(xa-xc)*(yb-ya)) * invden;

    if ( -_epsilon <= r && r <= 1.f+_epsilon && -_epsilon <= s && s <= 1.f+_epsilon )
    {
        _collision.m_x = xa+r*(xb-xa);
        _collision.m_y = ya+r*(yb-ya);
        return btrue;
    }

    return bfalse;
}

// _p is a position
// _dir is a direction
bbool Line_Line( const Vec2d& _pA, const Vec2d& _dirA, const Vec2d& _pB, const Vec2d& _dirB, Vec2d& _intersection )
{
    f32 xa = _pA.m_x;
    f32 ya = _pA.m_y;
    f32 xb = _pA.m_x + _dirA.m_x;
    f32 yb = _pA.m_y + _dirA.m_y;
    f32 xc = _pB.m_x;
    f32 yc = _pB.m_y;
    f32 xd = _pB.m_x + _dirB.m_x;
    f32 yd = _pB.m_y + _dirB.m_y;

    f32 den = ((xb-xa)*(yd-yc)-(yb-ya)*(xd-xc));

    if ( !den )
    {
        return bfalse;
    }

    f32 r = ((ya-yc)*(xd-xc)-(xa-xc)*(yd-yc)) * f32_Inv(den);

    _intersection.m_x = xa+r*(xb-xa);
    _intersection.m_y = ya+r*(yb-ya);

    return btrue;
}

// Barycentric coordinates method explained in http://www.blackpawn.com/texts/pointinpoly/default.html
bbool Point_In_Triangle( const Vec2d& _trA, const Vec2d& _trB, const Vec2d& _trC, const Vec2d& _p )
{
    Vec2d v0 = _trC - _trA;
    Vec2d v1 = _trB - _trA;
    Vec2d v2 = _p - _trA;

    // Compute dot products
    f32 dot00 = v0.dot(v0);
    f32 dot01 = v0.dot(v1);
    f32 dot02 = v0.dot(v2);
    f32 dot11 = v1.dot(v1);
    f32 dot12 = v1.dot(v2);

    // Compute barycentric coordinates
    f32 invDenom = f32_Inv((dot00 * dot11 - dot01 * dot01));
    f32 u = (dot11 * dot02 - dot01 * dot12) * invDenom;
    f32 v = (dot00 * dot12 - dot01 * dot02) * invDenom;

    // Check if point is in triangle
    return (u > 0.f) && (v > 0.f) && (u + v < 1.f);
}

int lineCircleIntersection(Vec2d _lineP1, Vec2d _lineP2, Vec2d _cirlceCenter, f32 _circleRadius,  Vec2d* _col,  Vec2d* _norm)
{
    Vec2d v1 = _lineP2 - _lineP1;
    f32 v1len = v1.norm();
    
    if (v1len < 0.00000001f)
        return 0;

    if (ITF_isnan(v1len))
        return 0;

    Vec2d v1n = v1 * f32_Inv(v1len);
    Vec2d v2 = _cirlceCenter - _lineP1;
    f32 dot = v1n.dot(v2);
    Vec2d proj1 = v1n * dot;
    Vec2d midpt = _lineP1 + proj1;

    f32 distToCenter = (midpt - _cirlceCenter).norm();
    if (distToCenter > _circleRadius) 
        return 0;
    if (distToCenter == _circleRadius)
    {
         _col[0] = midpt;
         _col[1] = midpt;
        _norm[0] = _col[0] - _cirlceCenter;
        _norm[0].normalize();
        _norm[1] = _norm[0];

        Vec2d p1toCol = _col[0] - _lineP1;
        f32 projCol = p1toCol.dot(v1n);
        if (projCol < 0.f)
            return 0;
        if (projCol > v1len)
            return 0;

        return 1;
    }
    f32 distToIntersection;
    if (distToCenter == 0.f)
    {
        distToIntersection = _circleRadius;
    }
    else
    {
        distToIntersection = (f32)f32_Sqrt(_circleRadius * _circleRadius - distToCenter * distToCenter);
    }
    f32 lineSegmentLength = f32_Inv(v1.norm());
    v1 *= lineSegmentLength;
    v1 *= distToIntersection;

    Vec2d c1 = midpt + v1;
    Vec2d c2 = midpt - v1;

    f32 c1dist = (c1-_lineP1).sqrnorm();
    f32 c2dist = (c2-_lineP1).sqrnorm();
    if (c1dist < c2dist)
    {
        _col[0] = c1;
        _col[1] = c2;
    }
    else
    {
        _col[0] = c2;
        _col[1] = c1;
    }

    Vec2d p1toCol = _col[0] - _lineP1;
    f32 projCol = p1toCol.dot(v1n);
    if (projCol < 0.f)
        return 0;
    if (projCol > v1len)
        return 0;

    _norm[0] = _col[0] - _cirlceCenter;
    _norm[0].normalize();
    _norm[1] = _col[1] - _cirlceCenter;
    _norm[1].normalize();
    return 2;

}

int IntersectLineCircle(
    const Vec2d& O,  // Line origin
    const Vec2d& D,  // Line direction
    const Vec2d& C,  // Circle center
    f32 radius,      // Circle radius
    f32* t,        // Parametric values at intersection points
    Vec2d* point,   // Intersection points
    Vec2d* normal) // Normals at intersection points
{
    Vec2d d = O - C;
    f32 a = D.dot(D); // sq line length
    f32 b = d.dot(D); // projection of circle center on the line
    f32 c = d.dot(d) - radius * radius; // sq dist from line origin to circle center

    f32 disc = b * b - a * c;
    if (disc < 0.0f) 
    {
        return 0;
    }

    f32 sqrtDisc = f32_Sqrt(disc);
    if (a < EPSILON14)
        return 0;

    f32 invA = f32_Inv(a);
    t[0] = (-b - sqrtDisc) * invA;
    t[1] = (-b + sqrtDisc) * invA;

    f32 invRadius = f32_Inv(radius);
    for (int i = 0; i < 2; ++i) {
        point[i] = O + D * t[i];
        normal[i] = (point[i] - C) * invRadius;
    }

    return 1;
}

int intersectionLineLine(const Vec2d& p1, const Vec2d& p2, const Vec2d& p3, const Vec2d& p4, Vec2d& _result)
{
    f32 div = (p4.m_y-p3.m_y)*(p2.m_x-p1.m_x)-(p4.m_x-p3.m_x)*(p2.m_y-p1.m_y);

    if (div==0.0f)
        return 0;

    f32 coeff = ((p4.m_x-p3.m_x)*(p1.m_y-p3.m_y)-(p4.m_y-p3.m_y)*(p1.m_x-p3.m_x)) * f32_Inv(div);

    _result = p1 +(p2-p1)*coeff;
    return 1;
}

bbool intersectionLineLineCoeff(const Vec2d& p1, const Vec2d& p2, const Vec2d& p3, const Vec2d& p4, f32& _coeff)
{
    f32 div = (p4.m_y-p3.m_y)*(p2.m_x-p1.m_x)-(p4.m_x-p3.m_x)*(p2.m_y-p1.m_y);

    if ( div == 0.0f )
        return bfalse;

    _coeff = ((p4.m_x-p3.m_x)*(p1.m_y-p3.m_y)-(p4.m_y-p3.m_y)*(p1.m_x-p3.m_x)) * f32_Inv(div);

    return btrue;
}


// calculates intersection and checks for parallel lines.   
// also checks that the intersection point is actually on   
// the line segment p1-p2   
int findIntersection(const Vec2d& p1, const Vec2d& p2, const Vec2d& p3, const Vec2d& p4, int _checkSeg, Vec2d& _result) 
{   
  f32 xD1,yD1,xD2,yD2,xD3,yD3;   
  f32 dot,deg,len1,len2;   
  f32 segmentLen1,segmentLen2;   
  f32 ua,ub,div;   
  
  // calculate differences   
  xD1=p2.m_x-p1.m_x;   
  xD2=p4.m_x-p3.m_x;   
  yD1=p2.m_y-p1.m_y;   
  yD2=p4.m_y-p3.m_y;   
  xD3=p1.m_x-p3.m_x;   
  yD3=p1.m_y-p3.m_y;     
  
  // calculate the lengths of the two lines   
  len1=f32_Sqrt(xD1*xD1+yD1*yD1);   
  len2=f32_Sqrt(xD2*xD2+yD2*yD2);   
  
  // calculate angle between the two lines.   
  dot=(xD1*xD2+yD1*yD2); // dot product   
  deg=dot * f32_Inv(len1*len2);   
  
  // if f32_Abs(angle)==1.f then the lines are parallell,   
  // so no intersection is possible   
  if(f32_Abs(deg)==1.f) return 0;   
  
  // find intersection Pt between two lines   
  Vec2d pt(0.f,0.f);
  div=f32_Inv(yD2*xD1-xD2*yD1);
  ua=(xD2*yD3-yD2*xD3) * div;
  ub=(xD1*yD3-yD1*xD3) * div;
  pt.m_x=p1.m_x+ua*xD1;   
  pt.m_y=p1.m_y+ub*yD1;   
  
  if (!_checkSeg)
  {
      _result = pt;
      return 1;
  }
  // calculate the combined length of the two segments   
  // between Pt-p1 and Pt-p2   
  xD1=pt.m_x-p1.m_x;   
  xD2=pt.m_x-p2.m_x;   
  yD1=pt.m_y-p1.m_y;   
  yD2=pt.m_y-p2.m_y;   
  segmentLen1=f32_Sqrt(xD1*xD1+yD1*yD1)+f32_Sqrt(xD2*xD2+yD2*yD2);   
  
  // calculate the combined length of the two segments   
  // between Pt-p3 and Pt-p4   
  xD1=pt.m_x-p3.m_x;   
  xD2=pt.m_x-p4.m_x;   
  yD1=pt.m_y-p3.m_y;   
  yD2=pt.m_y-p4.m_y;   
  segmentLen2=f32_Sqrt(xD1*xD1+yD1*yD1)+f32_Sqrt(xD2*xD2+yD2*yD2);   
  
  // if the lengths of both sets of segments are the same as   
  // the lenghts of the two lines the point is actually   
  // on the line segment.   
  
  // if the point isn't on the line, return null   
  if(f32_Abs(len1-segmentLen1)>0.01f || f32_Abs(len2-segmentLen2)>0.01f)   
    return 0;   
  
  // return the valid intersection
  _result = pt;
  return 1;   
}   


f32 PointSignedDistToSegment(const Vec2d& _point, const Vec2d& _segPt1, const Vec2d& _segPt2)
{
    // build data
    const Vec2d delta1P = _point - _segPt1;
    const Vec2d seg12   = _segPt2 - _segPt1;

    Vec2d segNormalized = seg12;
    segNormalized.normalize();

    const f32 signedProjOnSeg = segNormalized.dot(delta1P);
    if(signedProjOnSeg <= 0.0f)
        return delta1P.norm();

    if(signedProjOnSeg >= seg12.norm())
        return (_point - _segPt2).norm();

    const Vec2d perp = segNormalized.getPerpendicular();
    return -perp.dot(delta1P);
}

f32 getArcLength (f32 _radius, f32 _angle)
{
    f32 circum = 2.f * MTH_PI * _radius;
    return circum * _angle * f32_Inv(2.f * MTH_PI);
}

bool isPointInQuad(const Vec2d& _Point, Vec2d* _quad)
{
    // ------------------------------------
    u32        i;
    u32        j = 4 - 1;
    bool        oddNodes = false;
    // ------------------------------------

    for (i = 0; i < 4; i++)
    {
        if ((_quad[i].m_y < _Point.m_y && _quad[j].m_y >= _Point.m_y)
        ||  (_quad[j].m_y < _Point.m_y && _quad[i].m_y >= _Point.m_y))
        {
          if (_quad[i].m_x + (_Point.m_y - _quad[i].m_y)
            / (_quad[j].m_y - _quad[i].m_y)
            * (_quad[j].m_x - _quad[i].m_x) < _Point.m_x)
          {
            oddNodes =! oddNodes;
          }
        }
        j = i;
    }
    return oddNodes;
}

f32 canonizeAngle(f32 _angle)
{
    if( _angle > -MTH_PI && _angle < MTH_PI )
    {
        return _angle;
    }

    if( _angle < 0.0f )
    {
        _angle = MTH_2PI + f32_Modulo( _angle,MTH_2PI );
    }
    else
    {
        if( _angle >= MTH_2PI )
        {
            _angle = f32_Modulo( _angle,MTH_2PI );
        }
    }

    if( _angle > MTH_PI )
    {
        _angle = -( MTH_2PI - _angle );
    }

    return _angle;
}


f32 getShortestAngleDelta(f32 _from, f32 _to)
{
    _to = canonizeAngle(_to);
    _from = canonizeAngle(_from);
    return canonizeAngle(_to - _from);
}

bbool Line_Plane( const Vec3d & _startPos, const Vec3d & _dir, const Vec3d & _planPos, const Vec3d & _planNormale, f32 & _res_dist, Vec3d & _res_collidePos )
{
    _res_dist = _dir.dot( _planNormale );

    if (_res_dist == 0.0f )
        return bfalse;

    _res_dist = -(_startPos - _planPos).dot( _planNormale ) * f32_Inv(_res_dist);
    _res_collidePos = _startPos;
    _res_collidePos += _res_dist * _dir;

    return btrue;
}

} // namespace ITF

