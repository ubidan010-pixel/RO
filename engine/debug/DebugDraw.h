#ifndef _ITF_DEBUGDRAW_H_
#define _ITF_DEBUGDRAW_H_

#ifdef ITF_SUPPORT_DEBUGFEATURE

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h" // because of the usage of Color() class
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_PHYSSHAPES_H_
#include "engine/physics/PhysShapes.h"
#endif //_ITF_PHYSSHAPES_H_

#ifndef _ITF_BEZIERCURVE_H_
#include "engine/gameplay/BezierCurve.h"
#endif // _ITF_BEZIERCURVE_H_

namespace ITF
{
    namespace DebugDraw
    {
        // draw a line with a screen-space size (width is centered)
        void        line3D( const Vec3d& _from, const Vec3d& _to, const Color& _color = Color::white(), f32 _width = 1.f, f32 _duration = 0.f );
        void        line2D( const Vec2d& _from, const Vec2d& _to, f32 _z, const Color& _color = Color::white(), f32 _width = 1.f, f32 _duration = 0.f );

        void        dottedLine( const Vec3d& _from, const Vec3d& _to, const f32 _segmentCount = 10, const Color& _color = Color::white(), f32 _width = 1.f, f32 _duration = 0.f );
        void        dottedLine( const Vec2d& _from, const Vec2d& _to, f32 _z, const f32 _segmentCount = 10, const Color& _color = Color::white(), f32 _width = 1.f, f32 _duration = 0.f );

        // draw an arrow with a screen-space size
        void        arrow3D( const Vec3d& _from, const Vec3d& _to, const Color& _color = Color::white(), f32 _lineWidth = 5.f, f32 _arrowLength = 40.f, f32 _arrowWidth = 30.f );
        void        arrow2D( const Vec2d& _from, const Vec2d& _to, f32 _z, const Color& _color = Color::white(), f32 _lineWidth = 5.f, f32 _arrowLength = 40.f, f32 _arrowWidth = 30.f );

        void        circle( const Vec2d& _pos, f32 _z, f32 _radius,
                            const Color& _color = Color::white(), f32 _duration = 0.f,
                            const String& _text = String::emptyString, f32 _textAngle = MTH_PIBY4 );
        void        circle( const Vec3d& _pos, f32 _radius,
                            const Color& _color = Color::white(), f32 _duration = 0.f,
                            const String& _text = String::emptyString, f32 _textAngle = MTH_PIBY4 );

        void        AABB( const Vec2d& _pos, f32 _z, f32 _angle, const ITF::AABB& _aabb,
                          const Color& _color = Color::white(), f32 _duration = 0, f32 _lineWidth = 1.f,
                          const String& _text = String::emptyString );
        void        AABB( const Vec3d& _pos, f32 _angle, const ITF::AABB& _aabb,
                          const Color& _color = Color::white(), f32 _duration = 0, f32 _lineWidth = 1.f,
                          const String& _text = String::emptyString );
        void        AABB( const Transform2d& _transform, f32 _z, const ITF::AABB& _aabb,
                          const Color& _color = Color::white(), f32 _duration = 0, f32 _lineWidth = 1.f,
                          const String& _text = String::emptyString );
        void        AABB( const Transform3d& _transform, const ITF::AABB& _aabb,
                          const Color& _color = Color::white(), f32 _duration = 0, f32 _lineWidth = 1.f,
                          const String& _text = String::emptyString );

        void        edge( const PolyLineEdge* _edge, f32 _z,
                          const Color& _color = Color::white(), f32 _duration = 0, const String& _text = String::emptyString );
        void        edge( const Vec2d& _p1, const Vec2d& _p2, const Vec2d& _normal, f32 _z,
                          const Color& _color = Color::white(), f32 _duration = 0, const String& _text = String::emptyString );

        void        pointList( const Vec2d& _pos, f32 _z, f32 _angle, const SafeArray<Vec2d>& _points,
                               const Color& _color /*= Color::white()*/, f32 _duration /*= 0.f*/, const String* _text /*= NULL */ );
        void        pointList( const Vec3d& _pos, f32 _angle, const SafeArray<Vec2d>& _points,
                               const Color& _color /*= Color::white()*/, f32 _duration /*= 0.f*/, const String* _text /*= NULL */ );

        void        polyline( const PolyLine& _poly, const Vec2d& _offset = Vec2d::Zero, const Color& _color = Color::white(), f32 _duration = 0 );

        void        shape( const Vec2d& _pos, f32 _z, f32 _angle, const PhysShape* _shape,
                           const Color& _color = Color::white(), f32 _duration = 0.f,
                           const String& _text = String::emptyString, f32 _textAngle = MTH_PIBY4 );
        void        shape( const Vec3d& _pos, f32 _angle, const PhysShape* _shape,
                           const Color& _color = Color::white(), f32 _duration = 0.f,
                           const String& _text = String::emptyString, f32 _textAngle = MTH_PIBY4 );

        // draw text on the debug stack (bottom of the screen)
        void        text( const char* _format, ... );
        // draw text on the debug stack (bottom of the screen) with given color
        void        text( const Color& _color,
                          const char* _format, ... );
        // draw text at world coordinates
        void        text( const Vec3d& _pos,
                          const Color& _color,
                          const char* _format, ... );
        // draw text at world coordinates with screen offset
        void        text( const Vec3d& _worldPos, const Vec2d& _pixelOffset,
                          const Color& _textColor, const Color& _bgColor,
                          const char* _format, ... );
        // draw text at world coordinates
        void        text( const Vec2d& _pos, f32 _z,
                          const Color& _color,
                          const char* _format, ... );
        // draw text at screen coordinates
        void        text( f32 _x, f32 _y,
                          const Color& _color,
                          const char* _format, ... );
        ADD_COLOR(defaultBackgroundColor,     0.5f, 1.0f, 1.0f, 1.0f);

        // draw bezier 4 controls points
        void        bezier( const Vec3d& _p0, const Vec3d& _p1, const Vec3d& _p2, const Vec3d& _p3,
                            const Color& _color = Color::white(), const f32 _width = 1.0f, const f32 _controlPointsRadius = 0.f );
        void        bezier( const Vec2d& _p0, const Vec2d& _p1, const Vec2d& _p2, const Vec2d& _p3, const f32 _z,
                            const Color& _color = Color::white(), const f32 _width = 1.0f, const f32 _controlPointsRadius = 0.f );
        ITF_INLINE void bezier( const Bezier4<Vec3d>& _bezier, const Color& _color = Color::white(), const f32 _width = 1.0f, const f32 _controlPointsRadius = 0.f ) { bezier(_bezier.m_p0, _bezier.m_p1, _bezier.m_p2, _bezier.m_p3, _color, _width, _controlPointsRadius); }
        ITF_INLINE void bezier( const Bezier4<Vec2d>& _bezier, const f32 _z, const Color& _color = Color::white(), const f32 _width = 1.0f, const f32 _controlPointsRadius = 0.f ) { bezier(_bezier.m_p0, _bezier.m_p1, _bezier.m_p2, _bezier.m_p3, _z, _color, _width, _controlPointsRadius); }

        // draw bezier 3 controls points
        void        bezier( const Vec2d& _p0, const Vec2d& _p1, const Vec2d& _p2, const f32 _z,
                            const Color& _color = Color::white(), const f32 _width = 1.0f, const f32 _controlPointsRadius = 0.f );
        void        bezier( const Vec3d& _p0, const Vec3d& _p1, const Vec3d& _p2,
                            const Color& _color = Color::white(), const f32 _width = 1.0f, const f32 _controlPointsRadius = 0.f );
        ITF_INLINE void bezier( const Bezier3<Vec2d>& _bezier, const f32 _z, const Color& _color = Color::white(), const f32 _width = 1.0f, const f32 _controlPointsRadius = 0.f ) { bezier(_bezier.m_p0, _bezier.m_p1, _bezier.m_p2, _z, _color, _width, _controlPointsRadius); }
        ITF_INLINE void bezier( const Bezier3<Vec3d>& _bezier, const Color& _color = Color::white(), const f32 _width = 1.0f, const f32 _controlPointsRadius = 0.f ) { bezier(_bezier.m_p0, _bezier.m_p1, _bezier.m_p2, _color, _width, _controlPointsRadius); }

        void        bezierCurve( const BezierCurve& _curve,
                                 const Color& _color = Color::white(), const f32 _width = 1.0f, const f32 _controlPointsRadius = 0.f );
        void        bezierCurve( const Transform3d& _transform, const BezierCurve& _curve,
                                 const Color& _color = Color::white(), const f32 _width = 1.0f, const f32 _controlPointsRadius = 0.f );
        void        bezierCurve( const Vec3d& _pos, const f32 _angle, const BezierCurve& _curve,
                                 const Color& _color = Color::white(), const f32 _width = 1.0f, const f32 _controlPointsRadius = 0.f );

    };

};

#endif // ITF_SUPPORT_DEBUGFEATURE

#endif // _ITF_DEBUGDRAW_H_
