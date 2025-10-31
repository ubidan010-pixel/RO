#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_SCREENSIDETRAJECTORYFOLLOWERCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_ScreenSideTrajectoryFollowerComponent.h"
#endif //_ITF_RAY_SCREENSIDETRAJECTORYFOLLOWERCOMPONENT_H_

namespace ITF
{

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_ScreenSideTrajectoryFollowerComponent)
BEGIN_SERIALIZATION_CHILD(Ray_ScreenSideTrajectoryFollowerComponent)
END_SERIALIZATION()
BEGIN_VALIDATE_COMPONENT(Ray_ScreenSideTrajectoryFollowerComponent)
END_VALIDATE_COMPONENT()

//------------------------------------------------------------------------------
Ray_ScreenSideTrajectoryFollowerComponent::Ray_ScreenSideTrajectoryFollowerComponent()
    : Super()
{
}

//------------------------------------------------------------------------------
Ray_ScreenSideTrajectoryFollowerComponent::~Ray_ScreenSideTrajectoryFollowerComponent()
{
}

//------------------------------------------------------------------------------
void Ray_ScreenSideTrajectoryFollowerComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);
}



//------------------------------------------------------------------------------
void Ray_ScreenSideTrajectoryFollowerComponent::Update( f32 _deltaTime )
{
    Super::Update(_deltaTime);

    Vec3d projPos;
    Vec3d sidePos;
    Vec3d desiredPos;

    GFX_ADAPTER->compute3DTo2D(m_actor->getPos(), projPos);
    projPos.m_x = 0;
    GFX_ADAPTER->compute2DTo3D(projPos, sidePos);

    desiredPos = sidePos + Vec3d(getTemplate()->getDistanceFromSide(), 0,0);

    f32 deltaPos = desiredPos.m_x - m_actor->get2DPos().m_x;

    setRuntimeSpeed((deltaPos<0)?0:getTemplate()->getSpeed(), bfalse);
    /*
#ifdef DEVELOPER_SMATHIEU
    LOG("pos=%f,%f,%f", m_actor->getPos().m_x, m_actor->getPos().m_y, m_actor->getPos().m_z);
#endif
    */
}


//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_ScreenSideTrajectoryFollowerComponent_Template)
BEGIN_SERIALIZATION_CHILD(Ray_ScreenSideTrajectoryFollowerComponent_Template)
    SERIALIZE_MEMBER("distanceFromSide", m_distanceFromSide);
END_SERIALIZATION()

//------------------------------------------------------------------------------
Ray_ScreenSideTrajectoryFollowerComponent_Template::Ray_ScreenSideTrajectoryFollowerComponent_Template()
    :m_distanceFromSide(1)
{
}

} // namespace ITF
