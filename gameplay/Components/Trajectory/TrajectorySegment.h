#ifndef _ITF_TRAJECTORYSEGMENT_H_
#define _ITF_TRAJECTORYSEGMENT_H_

#ifndef _ITF_TRAJECTORYDATA_H_
#include "gameplay/Components/Trajectory/TrajectoryData.h"
#endif //_ITF_TRAJECTORYDATA_H_

#ifndef _ITF_MATHTOOLS_H_
#include "core/math/MathTools.h"
#endif //_ITF_MATHTOOLS_H_

namespace ITF {

class TrajectoryData;
class TrajectoryLinkInfo;

//------------------------------------------------------------------------------
/// Computes & draw actual trajectory between two nodes. Meant to be created
/// each frame. NB[LaurentCou]: if this ever becomes bottleneck there's probably
/// room for optimization.
class TrajectorySegment
{
public:
    TrajectorySegment(const TrajectoryData& _data);
    
    void getPosAt(f32 _length, Vec3d& _pos) const;
    void getDirAt(f32 _length, Vec3d& _dir) const;
    
    ITF_INLINE bbool beginsCurved() const { return m_beginsCurved; }
    ITF_INLINE bbool endsCurved() const { return m_endsCurved; }

    ITF_INLINE f32 getTotalLength() const { return m_totalLength; }

#ifdef ITF_SUPPORT_DEBUGFEATURE
    void draw(StringID _type, f32 _lineWidth, f32 _corridorWidth);
#endif // ITF_SUPPORT_DEBUGFEATURE

private:
    TrajectorySegment();
    
    // link type
    StringID m_type;

    // control points
    Vec3d m_beginPos;
    f32 m_beginScale;
    Vec3d m_endPos;
    f32 m_endScale;
    
    // direction, length
    Vec3d m_dir;

    // begin/end curves
    bbool m_beginsCurved;
    Bezier3<Vec3d> m_beginCurve;
    bbool m_endsCurved;
    Bezier3<Vec3d> m_endCurve;
    
    // jump curve
    Bezier4<Vec3d> m_jumpCurve;    
    
    // lengths
    f32 m_beginLength;
    f32 m_straightLength;
    f32 m_endLength;
    f32 m_totalLength;

    // upside down angles
    f32 m_beginAngle;
    f32 m_endAngle;
};

} // namespace ITF

#endif // _ITF_TRAJECTORYSEGMENT_H_
