#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_SHOWERTREECOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_ShowerTreeComponent.h"
#endif //_ITF_RAY_SHOWERTREECOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

namespace ITF
{
    class AngleRangeResult
    {
    public:
        AngleRangeResult()
            : angleInRange(0.f)
            , angleInRangeClamped(0.f)
            , cursor(0.f)
        {
        }

        f32 angleInRange;
        f32 angleInRangeClamped;
        f32 cursor;
    };

    // TODO: handle actor flip
    class AngleRange
    {
    public:
        AngleRange()
            : min(0.f)
            , max(0.f)
            , range(0.f)
            , clampLimit(0.f)
        {
        }

        f32 min;
        f32 max;
        f32 range;
        f32 clampLimit;

        void init(f32 _min, f32 _max)
        {
            min = _min;
            max = _max;

            range = getShortestAngleDelta(min, max);

            clampLimit = range * 0.5f - MTH_PI;
            NormalizeAngle2PI(clampLimit);
        }

        Transform2d getTransform( const Actor* _actor, const Vec2d& posOffset, const f32 angleOffset ) const
        {
            Transform2d actorTransform(_actor->get2DPos(), _actor->getAngle());
            Transform2d refTransform = actorTransform.transform(Transform2d(posOffset, angleOffset + min));
            return refTransform;
        }

        AngleRangeResult getAngleToTarget( const Transform2d& refTransform, const Vec2d& _target ) const
        {
            AngleRangeResult result;

            Vec2d vecToTarget = _target - refTransform.getPos();
            f32 angleToTarget = vecToTarget.normalize().getAngle();

            f32 angleInRange = refTransform.inverseTransformAngle(angleToTarget);
            NormalizeAngle2PI(angleInRange);

            f32 angleInRangeClamped = angleInRange;
            if (angleInRange > range)
            {
                angleInRangeClamped = angleInRange > clampLimit ? 0.f : range;
            }

            if (min > max)
            {
                angleInRange -= MTH_2PI;

                angleInRangeClamped = angleInRange;
                if (angleInRange < range)
                {
                    f32 clampLimitInv = clampLimit - MTH_2PI;
                    angleInRangeClamped = angleInRange < clampLimitInv ? 0.f : range;
                }
            }


            result.angleInRange = angleInRange;
            result.angleInRangeClamped = angleInRangeClamped;
            result.cursor = angleInRangeClamped / range;

            return result;
        }

#ifdef ITF_SUPPORT_DEBUGFEATURE
        void drawDebug( const Transform2d& refTransform, const f32 z, const f32 _radius ) const
        {
            const Vec2d Right(_radius, 0);

            const Vec2d pMin = refTransform.transformPos(Right);
            DebugDraw::line2D(refTransform.getPos(), pMin, z, Color::green());

            const Vec2d pMax = refTransform.transformPos(Right.Rotate(range));
            DebugDraw::line2D(refTransform.getPos(), pMax, z, Color::red());

            const Vec2d pLimit = refTransform.transformPos(Right.Rotate(clampLimit));
            DebugDraw::line2D(refTransform.getPos(), pLimit, z, Color::orange());

            const f32 count = 10;
            const f32 invCount = f32_Inv(count);
            Vec2d p0 = pMin;
            for (u32 i = 1; i < count + 1; i++)
            {
                const f32 angle = i * invCount * range;
                const Vec2d p1 = refTransform.transformPos(Right.Rotate(angle));
                DebugDraw::line2D(p0, p1, z, Color::yellow());
                p0 = p1;
            }
        }

        void drawDebugResult( const Transform2d& refTransform, const f32 z, const f32 _radius, const AngleRangeResult& result ) const
        {
            const Vec2d Right(_radius, 0);

            DebugDraw::line2D(refTransform.getPos(), refTransform.transformPos(Right.Rotate(result.angleInRange)), z, Color::white());
            DebugDraw::line2D(refTransform.getPos(), refTransform.transformPos(Right.Rotate(result.angleInRangeClamped)), z, Color::blue(), 3);

            DebugDraw::text("range: %.0f", range * MTH_RADTODEG);
            DebugDraw::text("angle: %.0f", result.angleInRange * MTH_RADTODEG);
            DebugDraw::text("clamp limit: %.0f", clampLimit * MTH_RADTODEG);
            DebugDraw::text("angle clamped: %.0f", result.angleInRangeClamped * MTH_RADTODEG);
            DebugDraw::text("cursor: %.0f%%", result.cursor * 100);
        }
#endif // ITF_SUPPORT_DEBUGFEATURE
    };



    IMPLEMENT_OBJECT_RTTI(Ray_ShowerTreeComponent_Template)

    BEGIN_SERIALIZATION(Ray_ShowerTreeComponent_Template)
        SERIALIZE_MEMBER("input", m_input);
        SERIALIZE_MEMBER("minAngle", m_minAngle);
        SERIALIZE_MEMBER("maxAngle", m_maxAngle);
        SERIALIZE_MEMBER("angleOffset", m_angleOffset);
        SERIALIZE_MEMBER("posOffset", m_posOffset);
    END_SERIALIZATION()

    //-------------------------------------------------------------------------------------

    IMPLEMENT_OBJECT_RTTI(Ray_ShowerTreeComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_ShowerTreeComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(Ray_ShowerTreeComponent)
    END_VALIDATE_COMPONENT()

    Ray_ShowerTreeComponent::Ray_ShowerTreeComponent()
        : m_animComponent(NULL)
        , m_animatedComponent(NULL)
    {
    }

    Ray_ShowerTreeComponent::~Ray_ShowerTreeComponent()
    {
    }


    void Ray_ShowerTreeComponent::onActorLoaded( Pickable::HotReloadType _hotReload )
    {
        Super::onActorLoaded(_hotReload);

        m_animComponent = m_actor->GetComponent<AnimLightComponent>();
        m_animatedComponent = m_actor->GetComponent<AnimatedComponent>();
    }

    void Ray_ShowerTreeComponent::Update( f32 _dt )
    {
        Super::Update(_dt);

        Actor* player = AIUtils::getLeadLivePlayer(m_actor);
        if (!player)
        {
            return;
        }

        AngleRange angleRange;
        angleRange.init(getTemplate()->getMinAngle(), getTemplate()->getMaxAngle());

        Transform2d refTransform = angleRange.getTransform(m_actor, getTemplate()->getPosOffset() * m_actor->getScale(), getTemplate()->getAngleOffset());
        AngleRangeResult result = angleRange.getAngleToTarget(refTransform, player->get2DPos());

        f32 cursor = Min(result.cursor, 0.99f);    // setProceduralCursor(1) --> 0...

        if (m_animatedComponent)
        {
            m_animatedComponent->setInput(getTemplate()->getInput(), cursor);
        }
        else if (m_animComponent)
        {
            m_animComponent->setProceduralCursor(cursor);
        }
    }

#ifdef ITF_SUPPORT_EDITOR
    void Ray_ShowerTreeComponent::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        Super::drawEdit(_drawInterface, _flags);

        if (_flags != ActorComponent::DrawEditFlag_All)
            return;

        const f32 z = m_actor->getDepth();
        const f32 debugRadius = 2;

        AngleRange angleRange;
        angleRange.init(getTemplate()->getMinAngle(), getTemplate()->getMaxAngle());
        Transform2d refTransform = angleRange.getTransform(m_actor, getTemplate()->getPosOffset() * m_actor->getScale(), getTemplate()->getAngleOffset());

        angleRange.drawDebug(refTransform, z, debugRadius);

        if ( Actor* player = AIUtils::getLeadLivePlayer(m_actor) )
        {
            AngleRangeResult result = angleRange.getAngleToTarget(refTransform, player->get2DPos());
            angleRange.drawDebugResult(refTransform, z, debugRadius, result);
        }
    }
#endif // ITF_SUPPORT_EDITOR
};