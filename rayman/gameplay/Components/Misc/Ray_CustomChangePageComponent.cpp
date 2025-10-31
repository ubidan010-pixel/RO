#include "precompiled_gameplay_rayman.h"

#include "gameplay/Components/Trigger/ShapeDetectorComponent.h"

#ifndef _ITF_RAY_CUSTOMCHANGEPAGECOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_CustomChangePageComponent.h"
#endif //_ITF_RAY_CUSTOMCHANGEPAGECOMPONENT_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_CustomChangePageComponent)
    BEGIN_SERIALIZATION_CHILD(Ray_CustomChangePageComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(Ray_CustomChangePageComponent)
    END_VALIDATE_COMPONENT()

    //--------------------------------------------------------------------------------------------------------
    Ray_CustomChangePageComponent::Ray_CustomChangePageComponent()
        : Super()
    {
    }

    Ray_CustomChangePageComponent::~Ray_CustomChangePageComponent()
    {
    }

    //--------------------------------------------------------------------------------------------------------
    void Ray_CustomChangePageComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);
    }

    //--------------------------------------------------------------------------------------------------------
    Ray_CustomChangePageComponent::ExitType Ray_CustomChangePageComponent:: ComputePoints(
        Vec2d const & _enterDir,
        Vec2d & _res_enterPoint,
        Vec2d & _res_exitPoint,
        Vec2d & _res_finalPoint,
        bbool & _res_exitPointValid,
        bbool & _res_finalPointValid,
        bbool & _res_finalPointInWater,
        bbool _enterPointOnly /*= bfalse*/) const
    {
        if (getCustomTemplate()->getUseCustomPoints())
        {
            ShapeDetectorComponent* shapeDetector = m_actor->GetComponent<ShapeDetectorComponent>();
            PhysShapeBox* triggerShape = static_cast<PhysShapeBox*>(shapeDetector->getCurrentShape());
            if (triggerShape == NULL)
            {
                _res_enterPoint = _res_exitPoint = _res_finalPoint = m_actor->get2DPos();
                _res_finalPointValid = bfalse;
                _res_exitPointValid = bfalse;
                return ExitType_None;
            }

            Vec2d actorDir = Vec2d::Right.Rotate(m_actor->getAngle());
            f32 dotSign = getSign(_enterDir.dot(actorDir));
            _res_enterPoint = getEnterPoint(-dotSign, triggerShape, actorDir);

            Vec3d customExit = getCustomTemplate()->getCustomExitPoint();
            Vec3d customFinal = getCustomTemplate()->getCustomFinalPoint();
            Vec3d actorPos = m_actor->getPos();
            Vec3d absoluteExit = actorPos + customExit;
            Vec3d absoluteFinal = actorPos + customFinal;

            _res_exitPoint = absoluteExit.truncateTo2D();
            _res_finalPoint = absoluteFinal.truncateTo2D();
            _res_exitPointValid = btrue;
            _res_finalPointValid = btrue;
            _res_finalPointInWater = isPointInWater(_res_finalPoint);

            if (!_enterPointOnly)
            {
                f32 horizDot = actorDir.dot(Vec2d::Right) * dotSign;
                f32 vertiDot = actorDir.dot(Vec2d::Up) * dotSign;

                if (horizDot >= 0.72f)
                {
                    return ExitType_Right;
                }
                else if (horizDot <= -0.72f)
                {
                    return ExitType_Left;
                }
                else if (vertiDot > 0.0f)
                {
                    return ExitType_Up;
                }
                else
                {
                    return ExitType_Down;
                }
            }

            return ExitType_None;
        }
        else
        {
            return Super::ComputePoints(_enterDir, _res_enterPoint, _res_exitPoint, _res_finalPoint,
                                       _res_exitPointValid, _res_finalPointValid, _res_finalPointInWater, _enterPointOnly);
        }
    }


    Ray_CustomChangePageComponent_Template::Ray_CustomChangePageComponent_Template()
        : Super()
        , m_customExitPoint(Vec3d::Zero)
        , m_customFinalPoint(Vec3d::Zero)
        , m_useCustomPoints(bfalse)
    {
    }

    IMPLEMENT_OBJECT_RTTI(Ray_CustomChangePageComponent_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_CustomChangePageComponent_Template)
        SERIALIZE_MEMBER("customExitPoint", m_customExitPoint);
        SERIALIZE_MEMBER("customFinalPoint", m_customFinalPoint);
        SERIALIZE_MEMBER("useCustomPoints", m_useCustomPoints);
    END_SERIALIZATION()

} // namespace ITF

