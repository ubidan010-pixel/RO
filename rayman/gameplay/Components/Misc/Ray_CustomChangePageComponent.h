#ifndef _ITF_RAY_CUSTOMCHANGEPAGECOMPONENT_H_
#define _ITF_RAY_CUSTOMCHANGEPAGECOMPONENT_H_

#ifndef _ITF_RAY_CHANGEPAGECOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_ChangePageComponent.h"
#endif //_ITF_RAY_CHANGEPAGECOMPONENT_H_

namespace ITF
{
    class Ray_CustomChangePageComponent : public Ray_ChangePageComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_CustomChangePageComponent, Ray_ChangePageComponent, 3338454047);
        DECLARE_VALIDATE_COMPONENT()

    public:
        DECLARE_SERIALIZE()

        Ray_CustomChangePageComponent();
        virtual ~Ray_CustomChangePageComponent();

        virtual void onActorLoaded(Pickable::HotReloadType _hotReload);

        virtual ExitType ComputePoints(Vec2d const & _enterDir, 
                                      Vec2d & _res_enterPoint, 
                                      Vec2d & _res_exitPoint, 
                                      Vec2d & _res_finalPoint, 
                                      bbool & _res_exitPointValid, 
                                      bbool & _res_finalPointValid,
                                      bbool & _res_finalPointInWater,
                                      bbool _enterPointOnly = bfalse) const;

        ITF_INLINE const class Ray_CustomChangePageComponent_Template* getCustomTemplate() const;
    };

    //---------------------------------------------------------------------------------------------------

    class Ray_CustomChangePageComponent_Template : public Ray_ChangePageComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_CustomChangePageComponent_Template, Ray_ChangePageComponent_Template, 2001252747);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_CustomChangePageComponent);

    public:
        Ray_CustomChangePageComponent_Template();
        ~Ray_CustomChangePageComponent_Template() {}

        ITF_INLINE const Vec3d& getCustomExitPoint() const { return m_customExitPoint; }
        ITF_INLINE const Vec3d& getCustomFinalPoint() const { return m_customFinalPoint; }
        ITF_INLINE bbool getUseCustomPoints() const { return m_useCustomPoints; }

    private:
        Vec3d m_customExitPoint;
        Vec3d m_customFinalPoint;
        bbool m_useCustomPoints;
    };

    const Ray_CustomChangePageComponent_Template* Ray_CustomChangePageComponent::getCustomTemplate() const 
    {
        return static_cast<const Ray_CustomChangePageComponent_Template*>(m_template);
    }

} // namespace ITF

#endif // _ITF_RAY_CUSTOMCHANGEPAGECOMPONENT_H_
