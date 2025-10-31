#include "precompiled_gameplay.h"

#ifndef _ITF_BOXINTERPOLATORCOMPONENT_H_
#include "gameplay/Components/FX/BoxInterpolatorComponent.h"
#endif //_ITF_BOXINTERPOLATORCOMPONENT_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(BoxInterpolatorComponent_Template)

    BEGIN_SERIALIZATION_CHILD(BoxInterpolatorComponent_Template)
        SERIALIZE_OBJECT("innerAABB", m_innerAABB);
        SERIALIZE_OBJECT("outerAABB", m_outerAABB);
        SERIALIZE_MEMBER("AABBRotation", m_AABBRotation);
    END_SERIALIZATION()

    BoxInterpolatorComponent_Template::BoxInterpolatorComponent_Template()
        : m_AABBRotation(btrue,0.0f)
    {
    }

    BoxInterpolatorComponent_Template::~BoxInterpolatorComponent_Template()
    {
    }

    ///////////////////////////////////////////////////////////////////////////////////////////

    IMPLEMENT_OBJECT_RTTI(BoxInterpolatorComponent)

    BEGIN_SERIALIZATION_CHILD(BoxInterpolatorComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(BoxInterpolatorComponent)
    END_VALIDATE_COMPONENT()


    BoxInterpolatorComponent::BoxInterpolatorComponent()
        : Super()
        , m_output(0.0f)
    {
    }

    BoxInterpolatorComponent::~BoxInterpolatorComponent()
    {
    }

    bbool BoxInterpolatorComponent::canUpdate()
    {
        const Vec2d input = getInputPos();
        Vec2d newInput = input - m_actor->get2DPos();
        newInput = newInput.Rotate(-getTemplate()->getAABBRotation().Radians()-m_actor->getAngle());

        AABB inner = getTemplate()->getInnerAABB();
        inner.Scale(m_actor->getScale());
        AABB outer = getTemplate()->getOuterAABB();
        outer.Scale(m_actor->getScale());

        if ( !outer.contains(newInput) )
        {
            // Need a last update to set final value
            if ( m_output > 0.0f )
                return btrue;
            else
                return bfalse;
        }
        else if ( inner.contains(newInput) )
        {
            // Need a last update to set final value
            if ( m_output < 1.0f )
                return btrue;
            else
                return bfalse;
        }

        return btrue;
    }

    f32 BoxInterpolatorComponent::getInputValue( f32 _dt )
    {
        const Vec2d input = getInputPos();
        Vec2d newInput = input - m_actor->get2DPos();
        newInput = newInput.Rotate(-getTemplate()->getAABBRotation().Radians()-m_actor->getAngle());

        AABB inner = getTemplate()->getInnerAABB();
        inner.Scale(m_actor->getScale());
        AABB outer = getTemplate()->getOuterAABB();
        outer.Scale(m_actor->getScale());

        const Vec2d& innerMin = inner.getMin();
        const Vec2d& innerMax = inner.getMax();
        const Vec2d& outerMin = outer.getMin();
        const Vec2d& outerMax = outer.getMax();

        const f32 leftSpace = outerMin.m_x - innerMin.m_x;
        const f32 rightSpace = outerMax.m_x - innerMax.m_x;
        const f32 upperSpace = outerMax.m_y - innerMax.m_y;
        const f32 downSpace = outerMin.m_y - innerMin.m_y;

        const f32 leftRatio     = leftSpace != 0.0f  ? 1.0f - Clamp((newInput.m_x - innerMin.m_x)/leftSpace,0.0f,1.0f)   : 1.0f;
        const f32 rightRatio    = rightSpace != 0.0f ? 1.0f - Clamp((newInput.m_x - innerMax.m_x)/rightSpace,0.0f,1.0f)  : 1.0f;
        const f32 downRatio     = downSpace != 0.0f  ? 1.0f - Clamp((newInput.m_y - innerMin.m_y)/downSpace,0.0f,1.0f)   : 1.0f;
        const f32 upRatio       = upperSpace != 0.0f ? 1.0f - Clamp((newInput.m_y - innerMax.m_y)/upperSpace,0.0f,1.0f)  : 1.0f;

        m_output = Min(Min(leftRatio,rightRatio),Min(upRatio,downRatio));


        // grow AABB
        return m_output;
    }

    void BoxInterpolatorComponent::Update( f32 _dt )
    {
        AABB inner = getTemplate()->getInnerAABB();
        inner.Scale(m_actor->getScale());
        AABB outer = getTemplate()->getOuterAABB();
        outer.Scale(m_actor->getScale());
        inner.RotateAround(getTemplate()->getAABBRotation().Radians() + m_actor->getAngle(),Vec2d::Zero);
        outer.RotateAround(getTemplate()->getAABBRotation().Radians() + m_actor->getAngle(),Vec2d::Zero);
        m_actor->growRelativeAABB(inner);
        m_actor->growRelativeAABB(outer);

        Super::Update(_dt);
    }

#ifdef ITF_SUPPORT_EDITOR
    void BoxInterpolatorComponent::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        Super::drawEdit(_drawInterface,_flags);

        if (_flags != ActorComponent::DrawEditFlag_All)
            return;


        f32 z = m_actor->getDepth();

        DebugDraw::circle(getInputPos(), z, 0.2f);

        AABB inner = getTemplate()->getInnerAABB();
        inner.Scale(m_actor->getScale());
        DebugDraw::AABB(m_actor->get2DPos(), z, getTemplate()->getAABBRotation().Radians() + m_actor->getAngle(), inner, Color::yellow());

        AABB outer = getTemplate()->getOuterAABB();
        outer.Scale(m_actor->getScale());
        DebugDraw::AABB(m_actor->get2DPos(), z, getTemplate()->getAABBRotation().Radians() + m_actor->getAngle(), outer, Color::blue());

        DebugDraw::text("VALUE = %f", m_inputValue);
    }


#endif // ITF_SUPPORT_EDITOR

};