#include "precompiled_gameplay.h"

#ifndef _ITF_CIRCLEINTERPOLATORCOMPONENT_H_
#include "gameplay/Components/FX/CircleInterpolatorComponent.h"
#endif //_ITF_CIRCLEINTERPOLATORCOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(CircleInterpolatorComponent_Template)

    BEGIN_SERIALIZATION_CHILD(CircleInterpolatorComponent_Template)
        SERIALIZE_MEMBER("innerRadius", m_innerRadius);
        SERIALIZE_MEMBER("outerRadius", m_outerRadius);
    END_SERIALIZATION()

    CircleInterpolatorComponent_Template::CircleInterpolatorComponent_Template()
        : m_innerRadius(1.0f)
        , m_outerRadius(2.0f)
    {
    }

    CircleInterpolatorComponent_Template::~CircleInterpolatorComponent_Template()
    {
    }

    ///////////////////////////////////////////////////////////////////////////////////////////

    IMPLEMENT_OBJECT_RTTI(CircleInterpolatorComponent)

    BEGIN_SERIALIZATION_CHILD(CircleInterpolatorComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(CircleInterpolatorComponent)
    END_VALIDATE_COMPONENT()


    CircleInterpolatorComponent::CircleInterpolatorComponent()
        : Super()
    {
    }

    CircleInterpolatorComponent::~CircleInterpolatorComponent()
    {
    }

    f32 CircleInterpolatorComponent::getInputValue( f32 _dt )
    {
        // grow AABB
        const f32 radius = Max(getTemplate()->getInnerRadius(),getTemplate()->getOuterRadius());
        AABB aabb(Vec2d(-radius,-radius));
        aabb.grow(Vec2d(radius,radius));
        m_actor->growRelativeAABB(aabb);


        Vec2d input = getInputPos();
        const f32 distToCenter = (input - m_actor->get2DPos()).norm();
        f32 output = 1.0f - Clamp((distToCenter - getTemplate()->getInnerRadius())/(getTemplate()->getOuterRadius() - getTemplate()->getInnerRadius()),0.0f,1.0f);
        return output;
    }

#ifdef ITF_SUPPORT_EDITOR
    void CircleInterpolatorComponent::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        Super::drawEdit(_drawInterface,_flags);

        if (_flags != ActorComponent::DrawEditFlag_All)
            return;


        DebugDraw::circle(getInputPos(), m_actor->getDepth(), 0.2f);

        DebugDraw::circle(m_actor->getPos(), getTemplate()->getInnerRadius(), Color::red());
        DebugDraw::circle(m_actor->getPos(), getTemplate()->getOuterRadius(), Color::blue());

        DebugDraw::text("VALUE = %f", m_inputValue);
    }
#endif // ITF_SUPPORT_EDITOR

};