#include "precompiled_core.h"

#ifndef _ITF_VEC3D_H_
#include "core/math/vec3d.h"
#endif //_ITF_VEC3D_H_

#ifndef _ITF_VEC2D_H_
#include "core/math/vec2d.h"
#endif //_ITF_VEC2D_H_

namespace ITF
{

    const Vec3d Vec3d::XAxis    (1.0f, 0.0f, 0.0f);
    const Vec3d Vec3d::YAxis    (0.0f, 1.0f, 0.0f);
    const Vec3d Vec3d::ZAxis    (0.0f, 0.0f, 1.0f);
    const Vec3d Vec3d::Zero     (0.0f, 0.0f, 0.0f);
    const Vec3d Vec3d::One      (1.0f, 1.0f, 1.0f);
    const Vec3d Vec3d::Up       (0.0f, 1.0f, 0.0f);
    const Vec3d Vec3d::Right    (1.0f, 0.0f, 0.0f);
    const Vec3d Vec3d::Front    (0.0f, 0.0f, 1.0f);
    const Vec3d Vec3d::Infinity (F32_INFINITY, F32_INFINITY, F32_INFINITY);
} // namespace ITF

