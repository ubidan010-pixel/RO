#include "precompiled_core.h"

#ifndef _ITF_BV_AABB_H_
#include "core/boundingvolume/AABB.h"
#endif //_ITF_BV_AABB_H_

namespace ITF
{
    BEGIN_SERIALIZATION(AABB)
        SERIALIZE_MEMBER("MIN",m_Min);
        SERIALIZE_MEMBER("MAX",m_Max);
    END_SERIALIZATION()

    BEGIN_SERIALIZATION(Margin)
        SERIALIZE_MEMBER("left", m_left);
        SERIALIZE_MEMBER("right", m_right);
        SERIALIZE_MEMBER("top", m_top);
        SERIALIZE_MEMBER("bottom", m_bottom);
    END_SERIALIZATION()


    void AABB::RotateAround(f32 _angle, const Vec2d &center)
    {
        Vec2d vCS;
        Vec2d::CosSin(&vCS, _angle);

        Vec2d   corner1 = Vec2d::mergeXY(&m_Min, &m_Max);
        Vec2d   corner2 = Vec2d::mergeXY(&m_Max, &m_Min);
        Vec2d   corner3, corner4;

        corner1 = corner1.RotateAroundCS(center, vCS);
        corner2 = corner2.RotateAroundCS(center, vCS);
        corner3 = m_Min.RotateAroundCS(center, vCS);
        corner4 = m_Max.RotateAroundCS(center, vCS);

        m_Min = m_Max = corner1;
        grow(corner2);
        grow(corner3);
        grow(corner4);
    }

    void AABB::Rotate(f32 _angle)
    {
        Vec2d vCS;
        Vec2d::CosSin(&vCS, _angle);

        Vec2d   corner1 = Vec2d::mergeXY(&m_Min, &m_Max);
        Vec2d   corner2 = Vec2d::mergeXY(&m_Max, &m_Min);
        Vec2d   corner3, corner4;

        corner1 = corner1.RotateCS(vCS);
        corner2 = corner2.RotateCS(vCS);
        corner3 = m_Min.RotateCS(vCS);
        corner4 = m_Max.RotateCS(vCS);

        m_Min = m_Max = corner1;
        grow(corner2);
        grow(corner3);
        grow(corner4);
    }

} // namespace ITF

