#include "precompiled_gameplay.h"

// GLOBAL ENGINE CONFIG FILE. Must be the first included file
#ifndef _ENGINECOMMON_H_
#include "core/enginecommon.h"
#endif // _ENGINECOMMON_H_

#ifndef _ITF_TRAJECTORYSEGMENT_H_
#include "gameplay/Components/Trajectory/TrajectorySegment.h"
#endif //_ITF_TRAJECTORYSEGMENT_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_TRAJECTORYNODECOMPONENT_H_
#include "gameplay/Components/Trajectory/TrajectoryNodeComponent.h"
#endif //_ITF_TRAJECTORYNODECOMPONENT_H_

namespace ITF {

//------------------------------------------------------------------------------
TrajectorySegment::TrajectorySegment(const TrajectoryData& _data)
    : m_type(_data.getCurrentLink().getType())
    , m_beginPos(Vec3d::Zero)
    , m_beginScale(0.0f)
    , m_endPos(Vec3d::Zero)
    , m_endScale(0.0f)
    , m_dir(Vec3d::Zero)
    , m_beginsCurved(bfalse)
    , m_beginCurve()
    , m_endsCurved(bfalse)
    , m_endCurve()
    , m_jumpCurve()
    , m_beginLength(0.0f)
    , m_straightLength(0.0f)
    , m_endLength(0.0f)
    , m_totalLength(0.0f)
    , m_beginAngle(0.0f)
    , m_endAngle(0.0f)
{
    // does the segment start curved?
    TrajectoryNodeComponent* currentNode = _data.getCurrentNode();
    ITF_ASSERT_CRASH(currentNode, "Current node is mandatory!");
    if (currentNode->useCurvedEnd() &&
        _data.getPreviousLink().useCurvedEnd() &&
        _data.getCurrentLink().useCurvedEnd())
    {
        m_beginsCurved = btrue;
    }

    // does the segment end curved?
    TrajectoryNodeComponent* nextNode = _data.getNextNode();
    ITF_ASSERT_CRASH(nextNode, "Next node is mandatory!");
    if (nextNode->useCurvedEnd() &&
        _data.getCurrentLink().useCurvedEnd() &&
        _data.getNextLink().useCurvedEnd())
    {
        m_endsCurved = btrue;
    }

    // begin scale
    Actor* previous = _data.getPreviousNodeRef().getActor();
    Actor* current = _data.getCurrentNodeRef().getActor();
    ITF_ASSERT_CRASH(current, "Current actor is mandatory!");
    m_beginPos = current->getPos();
    m_beginScale = 0.0f;
    if (m_beginsCurved)
    {
        m_beginScale = current->getScale().m_x;
        ITF_ASSERT(previous); // previous mandatory if begins curved
    }

    // end scale
    Actor* next = _data.getNextNodeRef().getActor();
    Actor* nextNext = _data.getNextNextNodeRef().getActor();
    ITF_ASSERT_CRASH(next, "Next actor is mandatory!");
    m_endPos = next->getPos();
    m_endScale = 0.0f;
    if (m_endsCurved)
    {
        m_endScale = next->getScale().m_x;
        ITF_ASSERT(nextNext); // next-next mandatory if ends curved
    }

    // compute segment direction
    m_dir = (m_endPos - m_beginPos).normalize();

    // compute control points
    if (m_type == TrajectoryLinkType_ToFly || m_type == TrajectoryLinkType_Jump)
    {
        // jump curve
        m_jumpCurve.m_p0 = m_beginPos;
        f32 jumpHeight = 0.5f * (m_endPos - m_beginPos).norm();
        f32 distX = m_endPos.m_x - m_beginPos.m_x;
        static f32 s_leftCursor = 0.5f;
        m_jumpCurve.m_p1 = m_beginPos + Vec3d(distX * s_leftCursor, jumpHeight, 0.0f);
        if (m_type == TrajectoryLinkType_ToFly)
        {
            // custom curve to go to fly nodes
            static f32 s_rightCursor = 0.4f;
            m_jumpCurve.m_p2 = m_endPos + Vec3d(-distX * s_rightCursor, 0.0f, 0.0f);
        }
        else
        {
            // "normal" jump
            static f32 s_rightCursor = 0.2f;
            m_jumpCurve.m_p2 = m_endPos + Vec3d(-distX * s_rightCursor, jumpHeight, 0.0f);
        }
        m_jumpCurve.m_p3 = m_endPos;
    }
    else
    {
        // begin curve
        if (m_beginsCurved)
        {
            const Vec3d& previousPos = previous->getPos();
            Vec3d previousDir = (m_beginPos - previousPos).normalize();
            Bezier3<Vec3d> fullBeginCurve(
                m_beginPos - previousDir * m_beginScale,
                m_beginPos,
                m_beginPos + m_dir * m_beginScale
                );
            Bezier3<Vec3d> dummy;
            fullBeginCurve.split(0.5f, dummy, m_beginCurve);
        }
        // end curve
        if (m_endsCurved)
        {
            const Vec3d& nextNextPos = nextNext->getPos();
            Vec3d nextDir = (nextNextPos - m_endPos).normalize();
            Bezier3<Vec3d> fullEndCurve(
                m_endPos - m_dir * m_endScale,
                m_endPos,
                m_endPos + nextDir * m_endScale
                );
            Bezier3<Vec3d> dummy;
            fullEndCurve.split(0.5f, m_endCurve, dummy);
        }
    }

    // compute lengths
    static const u32 numSamples = 10;
    if (m_beginsCurved && m_endsCurved)
    {
        m_beginLength = m_beginCurve.getLength(numSamples);
        m_straightLength = (m_endCurve.m_p0 - m_beginCurve.m_p2).norm();
        m_endLength = m_endCurve.getLength(numSamples);
    }
    else if (m_beginsCurved)
    {
        m_beginLength = m_beginCurve.getLength(numSamples);
        m_straightLength = (m_endPos - m_beginCurve.m_p2).norm();
        m_endLength = 0.0f;
    }
    else if (m_endsCurved)
    {
        m_beginLength = 0.0f;
        m_straightLength = (m_endCurve.m_p0 - m_beginPos).norm();
        m_endLength = m_endCurve.getLength(numSamples);
    }
    else
    {
        m_beginLength = 0.0f;
        m_straightLength = (m_endPos - m_beginPos).norm();
        m_endLength = 0.0f;
    }
    m_totalLength = m_beginLength + m_straightLength + m_endLength;

    // upside down angles
    Vec2d startDir = previous
        ? (m_beginPos.truncateTo2D() - previous->get2DPos()).normalize()
        : Vec2d::Right;
    startDir.normalize();
    m_beginAngle = startDir.getAngle() + MTH_PI;
    Vec2d endDir = m_endPos.truncateTo2D() - m_beginPos.truncateTo2D();
    endDir.normalize();
    m_endAngle = endDir.getAngle();
}

//------------------------------------------------------------------------------
void TrajectorySegment::getPosAt(f32 _length, Vec3d& _pos) const
{
    if (m_type == TrajectoryLinkType_ToFly || m_type == TrajectoryLinkType_Jump)
    {
        // jump
        f32 t = _length / m_totalLength;
        _pos = m_jumpCurve.getPos(t);
    }
    else
    {
        bbool inBegin = m_beginsCurved && (_length < m_beginLength);
        bbool inEnd = m_endsCurved && (_length > (m_totalLength-m_endLength));
        if (inBegin)
        {
            // begin curve
            f32 t = _length / m_beginLength;
            _pos = m_beginCurve.getPos(t);
        }
        else if (inEnd)
        {
            // end curve
            f32 t = (_length - m_beginLength - m_straightLength) / m_endLength;
            _pos = m_endCurve.getPos(t);
        }
        else
        {
            // straight (lerp)
            Vec3d beginPos = m_beginsCurved ? m_beginCurve.m_p2 : m_beginPos;
            Vec3d endPos = m_endsCurved ? m_endCurve.m_p0 : m_endPos;
            f32 t = (_length - m_beginLength) / m_straightLength;
            _pos = Interpolate(beginPos, endPos, t);
        }
    }
    ITF_ASSERT(_pos.isValid());
}

//------------------------------------------------------------------------------
void TrajectorySegment::getDirAt(f32 _length, Vec3d& _dir) const
{
    if (m_type == TrajectoryLinkType_Jump ||
        m_type == TrajectoryLinkType_ToFly ||
        m_type == TrajectoryLinkType_Fly ||
        m_type == TrajectoryLinkType_Land)
    {
        // right/left
        _dir = (m_endPos.m_x > m_beginPos.m_x) ? Vec3d::Right : -Vec3d::Right;
    }
    else if (m_type == TrajectoryLinkType_ToCeiling ||
            m_type == TrajectoryLinkType_ToFloor)
    {
        // upside-down
        f32 t = _length / m_totalLength;
        f32 angle = Interpolate(m_beginAngle, m_endAngle, t);
        _dir = Vec2d::Right.Rotate(angle).to3d();
    }
    else
    {
        bbool inBegin = m_beginsCurved && (_length < m_beginLength);
        bbool inEnd = m_endsCurved && (_length > (m_totalLength-m_endLength));
        if (inBegin)
        {
            // begin curve
            f32 t = _length / m_beginLength;
            _dir = m_beginCurve.getTangent(t);
        }
        else if (inEnd)
        {
            // end curve
            f32 t = (_length - m_beginLength - m_straightLength) / m_endLength;
            _dir = m_endCurve.getTangent(t);
        }
        else
        {
            // straight
            _dir = m_dir;
        }
    }
    ITF_ASSERT(_dir.isValid());
}

//------------------------------------------------------------------------------
#ifdef ITF_SUPPORT_DEBUGFEATURE
void TrajectorySegment::draw(StringID _type, f32 _lineWidth, f32 _corridorWidth)
{
    static const f32 stepCount = 10;
    static const f32 timeStep = 1.0f / stepCount;

    // jump
    if (_type == TrajectoryLinkType_Jump || _type == TrajectoryLinkType_ToFly)
    {
        DebugDraw::line3D(m_jumpCurve.m_p0, m_jumpCurve.m_p1, Color::magenta());
        DebugDraw::line3D(m_jumpCurve.m_p1, m_jumpCurve.m_p2, Color::magenta());
        DebugDraw::line3D(m_jumpCurve.m_p2, m_jumpCurve.m_p3, Color::magenta());

        Vec3d prevPos = m_jumpCurve.m_p0;
        for (f32 i=1; i<=stepCount; ++i)
        {
            Vec3d pos;
            getPosAt(i*timeStep*m_totalLength, pos);
            DebugDraw::line3D(prevPos, pos, Color::magenta(), _lineWidth);
            prevPos = pos;
        }
    }
    else
    {
        // begin curve
        if (m_beginsCurved)
        {
            // circle
            DebugDraw::circle(m_beginPos, m_beginScale, Color::yellow());

            Vec3d prevPos = m_beginCurve.m_p0;
            Vec3d prevNormal = (m_beginCurve.m_p1 - m_beginCurve.m_p0).normalize().getPerpendicular();
            for (f32 i=1; i<=stepCount; ++i)
            {
                Vec3d pos = m_beginCurve.getPos(i*timeStep);

                // line
                DebugDraw::line3D(prevPos, pos, Color::yellow(), _lineWidth);

                // corridor
                Vec3d normal = (i < stepCount) ?
                    (pos - prevPos).normalize().getPerpendicular() :
                    (m_beginCurve.m_p2 - m_beginCurve.m_p1).normalize().getPerpendicular();
                DebugDraw::line3D(
                    prevPos + prevNormal * _corridorWidth,
                    pos + normal * _corridorWidth,
                    Color::yellow());
                DebugDraw::line3D(
                    prevPos - prevNormal * _corridorWidth,
                    pos - normal * _corridorWidth,
                    Color::yellow());

                prevPos = pos;
                prevNormal = normal;
            }
        }
        
        // straight
        {
            Vec3d beginPos = m_beginsCurved ? m_beginCurve.m_p2 : m_beginPos;
            Vec3d endPos = m_endsCurved ? m_endCurve.m_p0 : m_endPos;

            // line
            DebugDraw::line3D(beginPos, endPos, Color::cyan(), _lineWidth);

            // arrow
            DebugDraw::arrow3D(beginPos, endPos, Color::cyan());

            // corridor
            Vec3d normal = (endPos - beginPos).normalize().getPerpendicular();
            DebugDraw::line3D(
                beginPos + normal * _corridorWidth,
                endPos + normal * _corridorWidth,
                Color::cyan());
            DebugDraw::line3D(
                beginPos - normal * _corridorWidth,
                endPos - normal * _corridorWidth,
                Color::cyan());    
        }
        
        // end curve
        if (m_endsCurved)
        {
            // circle
            DebugDraw::circle(m_endPos, m_endScale, Color::yellow());

            Vec3d prevPos = m_endCurve.m_p0;
            Vec3d prevNormal = (m_endCurve.m_p1 - m_endCurve.m_p0).normalize().getPerpendicular();
            for (f32 i=1; i<=stepCount; ++i)
            {
                Vec3d pos = m_endCurve.getPos(i*timeStep);

                // line
                DebugDraw::line3D(prevPos, pos, Color::yellow(), _lineWidth);

                // corridor
                Vec3d normal = (i < stepCount) ?
                    (pos - prevPos).normalize().getPerpendicular() :
                    (m_endCurve.m_p2 - m_endCurve.m_p1).normalize().getPerpendicular();
                DebugDraw::line3D(
                    prevPos + prevNormal * _corridorWidth,
                    pos + normal * _corridorWidth,
                    Color::yellow());
                DebugDraw::line3D(
                    prevPos - prevNormal * _corridorWidth,
                    pos - normal * _corridorWidth,
                    Color::yellow());

                prevPos = pos;
                prevNormal = normal;
            }
        }
    }
}
#endif // ITF_SUPPORT_DEBUGFEATURE

} // namespace ITF
