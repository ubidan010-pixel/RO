#include "precompiled_engine.h"

#ifdef ITF_SUPPORT_DEBUGFEATURE

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

namespace ITF
{
    namespace DebugDraw
    {
        void line3D( const Vec3d& _from, const Vec3d& _to, const Color& _color /*= Color::white() */, f32 _width /*= 1.f*/, f32 _duration /*= 0.f*/ )
        {
            GFX_ADAPTER->drawDBG3DLine(_from, _to, _color, _duration, _width);
        }

        void line2D( const Vec2d& _from, const Vec2d& _to, f32 _z, const Color& _color /*= Color::white() */, f32 _width /*= 1.f*/, f32 _duration /*= 0.f*/ )
        {
            GFX_ADAPTER->drawDBG3DLine(_from.to3d(_z), _to.to3d(_z), _color, _duration, _width);
        }



        // Draw a line with an arrow in the middle, dimensions in pixels (screen-space).
        void arrow3D(
            const Vec3d& _from, const Vec3d& _to,
            const Color& _color /*= Color::white()*/,
            f32 _lineWidth /*= 5.f*/,
            f32 _arrowLength /*= 40.f*/,
            f32 _arrowWidth /*= 30.f*/)
        {
            const u32 color = _color.getAsU32();


            Vec3d fromScreen, toScreen;
            GFX_ADAPTER->compute3DTo2D(_from, fromScreen);
            GFX_ADAPTER->compute3DTo2D(_to, toScreen);

            Vec2d from2d = fromScreen.truncateTo2D();
            Vec2d to2d = toScreen.truncateTo2D();


            // draw the line
            GFX_ADAPTER->draw2dLine(from2d, to2d, _lineWidth, color);


            // draw the arrow triangle
            {
                Vec2d middle = (from2d + to2d) * 0.5f;
                Vec2d dir = (to2d - from2d).normalize();
                Vec2d halfLength = dir * _arrowLength * 0.5f;
                Vec2d halfWidth = dir.getPerpendicular() * _arrowWidth * 0.5f;

                Vec2d p1 = middle + halfLength;
                Vec2d p2 = middle - halfLength + halfWidth;
                Vec2d p3 = middle - halfLength - halfWidth;
                GFX_ADAPTER->draw2dTriangle(p1, p2, p3, color);
            }
        }

        void arrow2D(
            const Vec2d& _from, const Vec2d& _to, f32 _z,
            const Color& _color /*= Color::white() */,
            f32 _lineWidth /*= 5.f*/,
            f32 _arrowLength /*= 40.f*/,
            f32 _arrowWidth /*= 30.f*/)
        {
            arrow3D(_from.to3d(_z), _to.to3d(_z), _color, _lineWidth, _arrowLength, _arrowWidth);
        }



        void circle( const Vec3d& _pos, f32 _radius,
                     const Color& _color /*= Color::white()*/, f32 _duration /*= 0.f*/,
                     const String& _text /*= String::emptyString*/, f32 _textAngle /*= MTH_PIBY4*/ )
        {
            GFX_ADAPTER->drawDBGCircle(_pos, _radius, _color, _duration);

            if (_text != String::emptyString)
            {
                Vec3d pos = _pos + (Vec2d::Right.Rotate(_textAngle) * _radius).to3d();
                Vec3d pos2d;
                GFX_ADAPTER->compute3DTo2D(pos, pos2d);
                GFX_ADAPTER->drawDBGText(_text, pos2d.m_x, pos2d.m_y, _color.m_r, _color.m_g, _color.m_b);
            }
        }

        void circle( const Vec2d& _pos, f32 _z, f32 _radius,
                     const Color& _color /*= Color::white()*/, f32 _duration /*= 0.f*/,
                     const String& _text /*= String::emptyString*/, f32 _textAngle /*= MTH_PIBY4*/ )
        {
            circle(Vec3d(_pos.m_x, _pos.m_y, _z), _radius, _color, _duration, _text, _textAngle);
        }



        void AABB( const Transform3d& _transform, const ITF::AABB& _aabb,
                   const Color& _color /*= Color::white()*/, f32 _duration /*= 0*/, f32 _lineWidth /*= 1.f*/,
                   const String& _text /*= String::emptyString*/ )
        {
            const Vec3d p0 = _transform.transformPos( _aabb.getMin() );
            const Vec3d p1 = _transform.transformPos( Vec2d(_aabb.getMin().m_x, _aabb.getMax().m_y) );
            const Vec3d p2 = _transform.transformPos( _aabb.getMax() );
            const Vec3d p3 = _transform.transformPos( Vec2d(_aabb.getMax().m_x, _aabb.getMin().m_y) );

            GFX_ADAPTER->drawDBG3DLine(p0, p1, _color.m_r, _color.m_g, _color.m_b, _duration, _lineWidth);
            GFX_ADAPTER->drawDBG3DLine(p1, p2, _color.m_r, _color.m_g, _color.m_b, _duration, _lineWidth);
            GFX_ADAPTER->drawDBG3DLine(p2, p3, _color.m_r, _color.m_g, _color.m_b, _duration, _lineWidth);
            GFX_ADAPTER->drawDBG3DLine(p3, p0, _color.m_r, _color.m_g, _color.m_b, _duration, _lineWidth);

            if (_text != String::emptyString)
            {
                Vec3d pos2d;
                GFX_ADAPTER->compute3DTo2D(p2, pos2d);
                GFX_ADAPTER->drawDBGText(_text, pos2d.m_x, pos2d.m_y, _color.m_r, _color.m_g, _color.m_b);
            }
        }

        void AABB( const Transform2d& _transform, f32 _z, const ITF::AABB& _aabb,
                   const Color& _color /*= Color::white()*/, f32 _duration /*= 0*/, f32 _lineWidth /*= 1.f*/,
                   const String& _text /*= String::emptyString*/ )
        {
            DebugDraw::AABB(_transform.to3d(_z), _aabb, _color, _duration, _lineWidth, _text);
        }

        void AABB( const Vec2d& _pos, f32 _z, f32 _angle, const ITF::AABB& _aabb,
                   const Color& _color /*= Color::white()*/, f32 _duration /*= 0*/, f32 _lineWidth /*= 1.f*/,
                   const String& _text /*= String::emptyString*/ )
        {
            DebugDraw::AABB(Transform3d(_pos.to3d(_z), _angle), _aabb, _color, _duration, _lineWidth, _text);
        }

        void AABB( const Vec3d& _pos, f32 _angle, const ITF::AABB& _aabb,
                   const Color& _color /*= Color::white()*/, f32 _duration /*= 0*/, f32 _lineWidth /*= 1.f*/,
                   const String& _text /*= String::emptyString*/ )
        {
            DebugDraw::AABB(Transform3d(_pos, _angle), _aabb, _color, _duration, _lineWidth, _text);
        }



        void edge( const PolyLineEdge* _edge, f32 _z,
                   const Color& _color /*= Color::white()*/, f32 _duration /*= 0 */,
                   const String& _text /*= String::emptyString*/ )
        {
            edge(
                _edge->getPos(),
                _edge->getPos() + _edge->m_vector,
                _edge->m_normalizedVector.getPerpendicular(),
                _z, _color, _duration, _text);
        }

        void edge( const Vec2d& _p1, const Vec2d& _p2, const Vec2d& _normal, f32 _z,
                   const Color& _color /*= Color::white()*/, f32 _duration /*= 0 */,
                   const String& _text /*= String::emptyString*/ )
        {
            // edge (size 3)
            GFX_ADAPTER->drawDBGLine(_p1, _p2, _color, _duration, 3.0f, _z);

            // normal (size 1)
            Vec2d midPoint = (_p1 + _p2) * 0.5f;
            GFX_ADAPTER->drawDBGLine(midPoint, midPoint + _normal * 0.1f, _color, _duration, 1.0f, _z);

            if (_text != String::emptyString)
            {
                Vec3d screenPos;
                GFX_ADAPTER->compute3DTo2D(midPoint.to3d(), screenPos);
                screenPos.m_x += _normal.m_x * 25;
                screenPos.m_y -= _normal.m_y * 25;
                GFX_ADAPTER->drawDBGText(_text, _color, screenPos.m_x, screenPos.m_y);
            }
        }



        void pointList( const Vec2d& _pos, f32 _z, f32 _angle, const SafeArray<Vec2d>& _points,
                        const Color& _color /*= Color::white()*/, f32 _duration /*= 0.f*/,
                        const String& _text /*= String::emptyString*/ )
        {
            if ( _points.size() > 1 )
            {
                Vec2d firstP = _points[0].Rotate(_angle) + _pos;
                Vec2d prevP = firstP;

                for ( u32 i = 1; i < _points.size(); i++ )
                {
                    Vec2d p = _points[i].Rotate(_angle) + _pos;
                    Vec2d normal = (p - prevP).getPerpendicular().normalize();

                    edge(prevP, p, normal, _z, _color, _duration);
                    //GFX_ADAPTER->drawDBGLine(prevP, p, _color.m_r, _color.m_g, _color.m_b, _duration, 1.0f, _z);

                    prevP = p;
                }

                Vec2d normal = (firstP - prevP).getPerpendicular().normalize();
                edge(prevP, firstP, normal, _z, _color, _duration, _text);
                //GFX_ADAPTER->drawDBGLine(prevP, firstP, _color.m_r, _color.m_g, _color.m_b, _duration, 1.0f, _z);
            }
        }

        void pointList( const Vec3d& _pos, f32 _angle, const SafeArray<Vec2d>& _points,
                        const Color& _color /*= Color::white()*/, f32 _duration /*= 0.f*/,
                        const String& _text /*= String::emptyString*/ )
        {
            pointList(_pos.truncateTo2D(), _pos.m_z, _angle, _points, _color, _duration, _text);
        }



        void shape( const Vec3d& _pos, f32 _angle, const PhysShape* _shape,
                    const Color& _color /*= Color::white()*/, f32 _duration /*= 0.f*/,
                    const String& _text /*= String::emptyString*/, f32 _textAngle /*= MTH_PIBY4*/ )
        {
            if ( const PhysShapePolygon* polygonShape = _shape->DynamicCast<PhysShapePolygon>(ITF_GET_STRINGID_CRC(PhysShapePolygon,3261252112)) )
            {
                pointList(_pos, _angle, polygonShape->getVertex(), _color, _duration, _text);
            }
            else if ( const PhysShapeCircle* circleShape = _shape->DynamicCast<PhysShapeCircle>(ITF_GET_STRINGID_CRC(PhysShapeCircle,3922519168)) )
            {
                circle(_pos, circleShape->getRadius(), _color, _duration, _text, _textAngle);
            }
            else if ( const PhysShapePolyline* polylineShape = _shape->DynamicCast<PhysShapePolyline>(ITF_GET_STRINGID_CRC(PhysShapePolyline,4121355535)) )
            {
                polyline(*polylineShape->getPolyline(), Vec2d::Zero, _color, _duration);
            }
        }

        void shape( const Vec2d& _pos, f32 _z, f32 _angle, const PhysShape* _shape,
                    const Color& _color /*= Color::white()*/, f32 _duration /*= 0.f*/,
                    const String& _text /*= String::emptyString*/, f32 _textAngle /*= MTH_PIBY4*/ )
        {
            shape(Vec3d(_pos.m_x, _pos.m_y, _z), _angle, _shape, _color, _duration, _text, _textAngle);
        }



        void polyline( const PolyLine& _poly, const Vec2d& _offset /*= Vec2d::Zero*/, const Color& _color /*= Color::white()*/, f32 _duration /*= 0*/ )
        {
            const f32 z = _poly.getDepth();

            const u32 edgeCount = _poly.getPosCount();
            for (u32 i = 0; i < edgeCount; i++)
            {
                const PolyLineEdge& edge = _poly.getEdgeAt(i);
                //DebugDraw::edge(&edge, z, _color, _duration);
                DebugDraw::edge(
                    _offset + edge.getPos(),
                    _offset + edge.getPos() + edge.m_vector,
                    edge.m_normalizedVector.getPerpendicular(),
                    z, _color, _duration);
            }
        }



#define CREATE_BUFFER()\
        const int bufferSize = 1024;\
        char buffer[bufferSize];\
        va_list args;\
        va_start(args, _format);\
        _vsnprintf_s(buffer, bufferSize-1, _format, args);\
        va_end(args);\
        buffer[bufferSize-1] = 0;\

        void text( const char* _format, ... )
        {
            CREATE_BUFFER();

            GFX_ADAPTER->drawDBGText(buffer);
        }

        void text( const Color& _color, const char* _format, ... )
        {
            CREATE_BUFFER();

            GFX_ADAPTER->drawDBGText(buffer, ITF_POS2D_AUTO, ITF_POS2D_AUTO, _color.m_r, _color.m_g, _color.m_b);
        }

        void text( f32 _x, f32 _y, const Color& _color, const char* _format, ... )
        {
            CREATE_BUFFER();

            GFX_ADAPTER->drawDBGText(buffer, _x, _y, _color.m_r, _color.m_g, _color.m_b);
        }

        void text( const Vec3d& _pos, const Color& _color, const char* _format, ... )
        {
            CREATE_BUFFER();

            Vec3d pos2d;
            GFX_ADAPTER->compute3DTo2D(_pos, pos2d);
            GFX_ADAPTER->drawDBGText(buffer, pos2d.m_x, pos2d.m_y, _color.m_r, _color.m_g, _color.m_b);
        }

        void text( const Vec2d& _pos, f32 _z, const Color& _color, const char* _format, ... )
        {
            CREATE_BUFFER();

            Vec3d pos2d;
            GFX_ADAPTER->compute3DTo2D(_pos.to3d(_z), pos2d);
            GFX_ADAPTER->drawDBGText(buffer, pos2d.m_x, pos2d.m_y, _color.m_r, _color.m_g, _color.m_b);
        }

        void text( const Vec3d& _worldPos, const Vec2d& _pixelOffset, const Color& _textColor, const Color& _bgColor, const char* _format, ... )
        {
            CREATE_BUFFER();

            Vec3d pos2d;
            GFX_ADAPTER->compute3DTo2D(_worldPos, pos2d);
            Vec2d pos = pos2d.truncateTo2D() + _pixelOffset;
            GFX_ADAPTER->drawDBGText(buffer, pos.m_x, pos.m_y, _textColor.m_r, _textColor.m_g, _textColor.m_b);

            const f32 textSize = f32(GFX_ADAPTER->getDebugFontSize());
            const u32 uColor = _bgColor.getAsU32();
            GFX_ADAPTER->draw2dBox(Vec2d(pos.m_x - 5, pos.m_y), strlen(buffer) * textSize / 2.5f, textSize, uColor, uColor, uColor, uColor);
        }

        void bezier( const Vec3d& _p0, const Vec3d& _p1, const Vec3d& _p2, const Vec3d& _p3,
                     const Color& _color /*= Color::white()*/, const f32 _width /*= 1.0f*/, const f32 _controlPointsRadius /*= 0.f*/ )
        {
            Vec3d prevPos = _p0;

            for (f32 t = 0.0f; t < 1.0f; t += 0.01f)
            {
                Vec3d curPos = getBezierPosition(_p0, _p1, _p2, _p3, t);
                line3D(prevPos, curPos, _color, _width);
                prevPos = curPos;
            }

            if (_controlPointsRadius > 0.f)
            {
                DebugDraw::circle(_p0, _controlPointsRadius, _color);
                DebugDraw::circle(_p1, _controlPointsRadius, _color);
                DebugDraw::circle(_p2, _controlPointsRadius, _color);
                DebugDraw::circle(_p3, _controlPointsRadius, _color);
            }
        }

        void bezier( const Vec2d& _p0, const Vec2d& _p1, const Vec2d& _p2, const Vec2d& _p3, const f32 _z,
                     const Color& _color /*= Color::white()*/, const f32 _width /*= 1.0f*/, const f32 _controlPointsRadius /*= 0.f*/ )
        {
            bezier( _p0.to3d(_z), _p1.to3d(_z), _p2.to3d(_z), _p3.to3d(_z),
                    _color, _width, _controlPointsRadius );
        }


        void bezier( const Vec3d& _p0, const Vec3d& _p1, const Vec3d& _p2,
                     const Color& _color /*= Color::white()*/, const f32 _width /*= 1.0f*/, const f32 _controlPointsRadius /*= 0.f*/ )
        {
            Vec3d prevPos = _p0;

            for (f32 t = 0.0f; t < 1.0f; t += 0.01f)
            {
                Vec3d curPos = getBezierPosition(_p0, _p1, _p2, t);
                line3D(prevPos, curPos, _color, _width);
                prevPos = curPos;
            }

            if (_controlPointsRadius > 0.f)
            {
                DebugDraw::circle(_p0, _controlPointsRadius, _color);
                DebugDraw::circle(_p1, _controlPointsRadius, _color);
                DebugDraw::circle(_p2, _controlPointsRadius, _color);
            }
        }

        void bezier( const Vec2d& _p0, const Vec2d& _p1, const Vec2d& _p2, const f32 _z,
                     const Color& _color /*= Color::white()*/, const f32 _width /*= 1.0f*/, const f32 _controlPointsRadius /*= 0.f*/ )
        {
            bezier( _p0.to3d(_z), _p1.to3d(_z), _p2.to3d(_z),
                    _color, _width, _controlPointsRadius );
        }

        void bezierCurve( const BezierCurve& _curve, const Color& _color /*= Color::white()*/, const f32 _width /*= 1.0f*/, const f32 _controlPointsRadius /*= 0.f */ )
        {
            for (u32 i = 0; i < _curve.getNumEdges(); i++)
            {
                const BezierCurve::Edge* edge = _curve.getEdgeAt(i);
                DebugDraw::bezier(edge->m_bezier, _color, _width, _controlPointsRadius);
            }
        }

        void bezierCurve( const Transform3d& _transform, const BezierCurve& _curve, const Color& _color /*= Color::white()*/, const f32 _width /*= 1.0f*/, const f32 _controlPointsRadius /*= 0.f */ )
        {
            for (u32 i = 0; i < _curve.getNumEdges(); i++)
            {
                const BezierCurve::Edge* edge = _curve.getEdgeAt(i);
                Bezier4<Vec3d> transformed;
                transformed.m_p0 = _transform.transformPos(edge->m_bezier.m_p0);
                transformed.m_p1 = _transform.transformPos(edge->m_bezier.m_p1);
                transformed.m_p2 = _transform.transformPos(edge->m_bezier.m_p2);
                transformed.m_p3 = _transform.transformPos(edge->m_bezier.m_p3);
                DebugDraw::bezier(transformed, _color, _width, _controlPointsRadius);
            }
        }

        void bezierCurve( const Vec3d& _pos, const f32 _angle, const BezierCurve& _curve, const Color& _color /*= Color::white()*/, const f32 _width /*= 1.0f*/, const f32 _controlPointsRadius /*= 0.f */ )
        {
            for (u32 i = 0; i < _curve.getNumEdges(); i++)
            {
                const BezierCurve::Edge* edge = _curve.getEdgeAt(i);
                Bezier4<Vec3d> transformed;
                transformed.m_p0 = transformPos(_pos, _angle, edge->m_bezier.m_p0);
                transformed.m_p1 = transformPos(_pos, _angle, edge->m_bezier.m_p1);
                transformed.m_p2 = transformPos(_pos, _angle, edge->m_bezier.m_p2);
                transformed.m_p3 = transformPos(_pos, _angle, edge->m_bezier.m_p3);
                DebugDraw::bezier(transformed, _color, _width, _controlPointsRadius);
            }
        }


        void dottedLine( const Vec3d& _from, const Vec3d& _to, const f32 _segmentCount /*= 10*/, const Color& _color /*= Color::white()*/, f32 _width /*= 1.f*/, f32 _duration /*= 0.f */ )
        {
            const Vec3d dir = (_to - _from).normalize();
            const f32 segmentLength = (_to - _from).norm() / _segmentCount;

            for (f32 i = 0; i < _segmentCount; i++)
            {
                Vec3d p0 = _from + dir * segmentLength * i; 
                Vec3d p1 = p0 + dir * segmentLength * 0.5f;
                DebugDraw::line3D(p0, p1, _color);
            }
        }

        void dottedLine( const Vec2d& _from, const Vec2d& _to, f32 _z, const f32 _segmentCount /*= 10*/, const Color& _color /*= Color::white()*/, f32 _width /*= 1.f*/, f32 _duration /*= 0.f */ )
        {
            dottedLine(_from.to3d(_z), _to.to3d(_z), _segmentCount, _color, _width, _duration);
        }

    } // namespace DebugDraw

} // namespace ITF

#endif // ITF_SUPPORT_DEBUGFEATURE
