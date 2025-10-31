#include "precompiled_GFXAdapter_WII.h"

#ifndef ITF_FINAL

#ifndef _ITF_WII_DRAWPRIM_H_
#include "adapters/GFXAdapter_WII/GFXAdapter_WII.h"
#endif //_ITF_WII_DRAWPRIM_H_

namespace ITF
{

//----------------------------------------------------------------------------//

void GFXAdapter_WII::reshape()
{
	ITF_ASSERT_MSG(0, "GFXAdapter_WII::reshape not implemented");
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::CalcCoord(const Vec2d& _in2d, float _zPlane, Vec2d& _out3d)
{
	ITF_ASSERT_MSG(0, "GFXAdapter_WII::CalcCoord not implemented");
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::unprojectPoint(const Vec2d& _in, float _viewDistance, const GMatrix44* _MatrixModel, const GMatrix44* _MatrixProj, const int* _View, Vec2d& _out)
{
	ITF_ASSERT_MSG(0, "GFXAdapter_WII::unprojectPoint not implemented");
}

//----------------------------------------------------------------------------//

void GFXAdapter_WII::setLookAtMatrix(GMatrix44* _MatrixOut, double _midx, double _midy, float _viewDistance, float _aspect)
{
	ITF_ASSERT_MSG(0, "GFXAdapter_WII::setLookAtMatrix not implemented");
}

///----------------------------------------------------------------------------//

void GFXAdapter_WII::project(const Vec3d& _in3d, Vec2d & _out2d)
{
    ITF_ASSERT_MSG(0, "GFXAdapter_WII::project not implemented");
}

} // namespace ITF

#endif // ITF_FINAL
