#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_EYEANIMATIONCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_EyeAnimationComponent.h"
#endif //_ITF_RAY_EYEANIMATIONCOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_EyeAnimationComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_EyeAnimationComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(Ray_EyeAnimationComponent)
        VALIDATE_COMPONENT_PARAM("", m_animatedComponent, "Ray_EyeAnimationComponent requires an AnimatedComponent");
        VALIDATE_COMPONENT_PARAM("", m_eyeInputIndexX != U32_INVALID, "unknown input '%s'", getTemplate()->getEyeInputX().getDebugString());
        VALIDATE_COMPONENT_PARAM("", m_eyeInputIndexY != U32_INVALID, "unknown input '%s'", getTemplate()->getEyeInputY().getDebugString());
    END_VALIDATE_COMPONENT()


    Ray_EyeAnimationComponent::Ray_EyeAnimationComponent()
        : m_animatedComponent(NULL)
        , m_eyeInputIndexX(U32_INVALID)
        , m_eyeInputIndexY(U32_INVALID)
    {
    }

    Ray_EyeAnimationComponent::~Ray_EyeAnimationComponent()
    {
    }

    void Ray_EyeAnimationComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        m_animatedComponent = m_actor->GetComponent<AnimatedComponent>();

        m_currentEyeCursor.m_x = 0.5f;
        m_currentEyeCursor.m_y = 0.5f;
    }

    void Ray_EyeAnimationComponent::onResourceReady()
    {
        Super::onResourceReady();

        if (m_eyeInputIndexX == U32_INVALID)
        {
            m_eyeInputIndexX = m_animatedComponent->findInputIndex(getTemplate()->getEyeInputX());
            m_eyeInputIndexY = m_animatedComponent->findInputIndex(getTemplate()->getEyeInputY());
        }
    }

    void Ray_EyeAnimationComponent::Update( f32 _dt )
    {
        Super::Update(_dt);

        Actor* target = AIUtils::getClosestLivePlayer(m_actor);
        if (!target)
        {
            return;
        }


        Vec2d targetPos = target->get2DPos() + getTemplate()->getTargetOffset();


        Vec2d unrotatedEyeRestPos = getTemplate()->getEyeOffset() * m_actor->getScale();
        Vec2d rotatedEyeRestPos = unrotatedEyeRestPos.Rotate(m_actor->getAngle()) + m_actor->get2DPos();
//#ifdef DEVELOPER_BEN
//        f32 z = m_actor->getDepth();
//        DebugDraw::circle(rotatedEyeRestPos.to3d(z), 0.1f, Color::red(), 0.f, "eye");
//        DebugDraw::line2D(rotatedEyeRestPos, targetPos, z);
//#endif
        Vec2d eyeToTargetWorldSpace = targetPos - rotatedEyeRestPos;
        Vec2d eyeToTargetModelSpace = eyeToTargetWorldSpace.Rotate(-m_actor->getAngle());

        Vec2d eyeToTargetAnimSpace = eyeToTargetModelSpace * getTemplate()->getDistanceFactor();
        eyeToTargetAnimSpace.m_y = -eyeToTargetAnimSpace.m_y;
        //normalize on ellipsis
        Vec2d eyeRadius = (getTemplate()->getEyeInputMax() - getTemplate()->getEyeInputMin()) * 0.5f;   // radius of ellipsis on which the eye goes (in animation space)
        eyeToTargetAnimSpace /= eyeRadius;
        f32 targetDist = eyeToTargetAnimSpace.norm();
        if (targetDist > 1.f)
        {
            eyeToTargetAnimSpace *= 1.f / targetDist;
        }
        eyeToTargetAnimSpace *= eyeRadius;

        eyeToTargetAnimSpace += (getTemplate()->getEyeInputMax() + getTemplate()->getEyeInputMin()) * 0.5f;

        m_currentEyeCursor += (eyeToTargetAnimSpace - m_currentEyeCursor) * getTemplate()->getEyeInputSmoothFactor();
        m_animatedComponent->setInputFromIndex(m_eyeInputIndexX, m_currentEyeCursor.m_x);
        m_animatedComponent->setInputFromIndex(m_eyeInputIndexY, m_currentEyeCursor.m_y);
    }

    //-------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(Ray_EyeAnimationComponent_Template)
    BEGIN_SERIALIZATION(Ray_EyeAnimationComponent_Template)
        SERIALIZE_MEMBER("eyeCursorSmoothFactor", m_eyeInputSmoothFactor);
        SERIALIZE_MEMBER("eyeInputX", m_eyeInputX);
        SERIALIZE_MEMBER("eyeInputY", m_eyeInputY);
        SERIALIZE_MEMBER("eyeMin", m_eyeInputMin);
        SERIALIZE_MEMBER("eyeMax", m_eyeInputMax);
        SERIALIZE_MEMBER("targetOffset", m_targetOffset);
        SERIALIZE_MEMBER("eyeOffset", m_eyeOffset);
        SERIALIZE_MEMBER("distanceFactor", m_distanceFactor);
    END_SERIALIZATION()

    Ray_EyeAnimationComponent_Template::Ray_EyeAnimationComponent_Template()
    : m_eyeInputSmoothFactor(0.5f)
    , m_eyeInputX("Eye_LeftToRight")
    , m_eyeInputY("Eye_UpToDown")
    , m_eyeInputMin(0.f, 0.f)
    , m_eyeInputMax(1.f, 1.f)
    , m_targetOffset(0.f, 0.5f)
    , m_eyeOffset(0.f, 0.f)
    , m_distanceFactor(0.1f)
    {
    }
};