#ifndef _ITF_RAY_SCREENSIDETRAJECTORYFOLLOWERCOMPONENT_H_
#define _ITF_RAY_SCREENSIDETRAJECTORYFOLLOWERCOMPONENT_H_

#ifndef _ITF_TRAJECTORYFOLLOWERCOMPONENT_H_
#include "gameplay/Components/Trajectory/TrajectoryFollowerComponent.h"
#endif //_ITF_TRAJECTORYFOLLOWERCOMPONENT_H_


namespace ITF {

class AnimatedComponent;
class EventTrigger;
class LinkComponent;
class TrajectoryNodeComponent;

//------------------------------------------------------------------------------
class Ray_ScreenSideTrajectoryFollowerComponent : public TrajectoryFollowerComponent
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_ScreenSideTrajectoryFollowerComponent,TrajectoryFollowerComponent,2509899776)
    DECLARE_VALIDATE_COMPONENT()

public:
    DECLARE_SERIALIZE()

    Ray_ScreenSideTrajectoryFollowerComponent();
    virtual ~Ray_ScreenSideTrajectoryFollowerComponent();

    virtual void onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
    virtual void Update( f32 _deltaTime );
   
private:
    ITF_INLINE const class Ray_ScreenSideTrajectoryFollowerComponent_Template*  getTemplate() const;
};

//------------------------------------------------------------------------------
class Ray_ScreenSideTrajectoryFollowerComponent_Template : public TrajectoryFollowerComponent_Template
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_ScreenSideTrajectoryFollowerComponent_Template,TrajectoryFollowerComponent_Template,1140262903);
    DECLARE_SERIALIZE();
    DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_ScreenSideTrajectoryFollowerComponent);

public:
    Ray_ScreenSideTrajectoryFollowerComponent_Template();
    ~Ray_ScreenSideTrajectoryFollowerComponent_Template() {}

    ITF_INLINE f32 getDistanceFromSide() const { return m_distanceFromSide; }

private:
    f32 m_distanceFromSide;
};

//------------------------------------------------------------------------------
ITF_INLINE
const Ray_ScreenSideTrajectoryFollowerComponent_Template*
Ray_ScreenSideTrajectoryFollowerComponent::getTemplate() const
{
    return static_cast<const Ray_ScreenSideTrajectoryFollowerComponent_Template*>(m_template);
}

} // namespace ITF

#endif //_ITF_RAY_SCREENSIDETRAJECTORYFOLLOWERCOMPONENT_H_
