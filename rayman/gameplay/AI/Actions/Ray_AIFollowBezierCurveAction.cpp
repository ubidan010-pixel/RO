#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AIFOLLOWBEZIERCURVEACTION_H_
#include "rayman/gameplay/ai/Actions/Ray_AIFollowBezierCurveAction.h"
#endif //_ITF_RAY_AIFOLLOWBEZIERCURVEACTION_H_

#ifndef _ITF_BEZIERCURVE_H_
#include "engine/gameplay/BezierCurve.h"
#endif // _ITF_BEZIERCURVE_H_

#ifndef _ITF_BEZIERCURVECOMPONENT_H_
#include "gameplay/Components/Misc/BezierCurveComponent.h"
#endif //_ITF_BEZIERCURVECOMPONENT_H_

#ifndef _ITF_PHYSCOMPONENT_H_
#include "engine/actors/components/physcomponent.h"
#endif //_ITF_PHYSCOMPONENT_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

namespace ITF
{

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_AIFollowBezierCurveAction)
BEGIN_VALIDATE_ACTION(Ray_AIFollowBezierCurveAction)
    VALIDATE_ACTION_PARAM("", m_physComponent, "PhysComponent mandatory");
    VALIDATE_ACTION_PARAM("", m_curveComponent, "BezierCurveComponent mandatory");
END_VALIDATE_ACTION()

//------------------------------------------------------------------------------
Ray_AIFollowBezierCurveAction::Ray_AIFollowBezierCurveAction()
    : Super()
    , m_speed(0.0f)
    , m_distance(0.0f)
    , m_physComponent(NULL)
    , m_curveComponent(NULL)
{
}

//------------------------------------------------------------------------------
Ray_AIFollowBezierCurveAction::~Ray_AIFollowBezierCurveAction()
{
}

//------------------------------------------------------------------------------
void Ray_AIFollowBezierCurveAction::onActorLoaded(Pickable::HotReloadType _hotReload)
{   
    Super::onActorLoaded(_hotReload);

    // mandatory, validated
    m_physComponent = m_actor->GetComponent<PhysComponent>();
    m_curveComponent = m_actor->GetComponent<BezierCurveComponent>();

    // not binding-compatible
    ITF_WARNING(
        m_actor, !m_actor->getParentBind(),
        "This actor shouldn't be bound because its Ray_AIFollowBezierCurveAction is "
        "moving it in world position");
}

//------------------------------------------------------------------------------
void Ray_AIFollowBezierCurveAction::onActivate()
{
    Super::onActivate();

    // remember initial speed
    m_speed = m_physComponent->getSpeed().norm();
    m_distance = 0.0f;

    // disable physics
    m_physComponent->setDisabled(true);
}

//------------------------------------------------------------------------------
void Ray_AIFollowBezierCurveAction::onDeactivate()
{
    Super::onDeactivate();

    // re-enable physics
    m_physComponent->setDisabled(bfalse);
}

//------------------------------------------------------------------------------
void Ray_AIFollowBezierCurveAction::update(f32 _dt)
{
    // blend to template speed
    m_speed = AIUtils::michelSmooth(m_speed, getTemplate()->getSpeed(), 0.1f);

    // update distance on curve
    m_distance += m_speed * _dt;
    
    // get position and tangent on curve
    BezierCurve& curve = m_curveComponent->getBezierCurve();
    m_distance = Min(m_distance, curve.getTotalLength());
    Vec3d curvePos = Vec3d::Zero;
    Vec3d curveTangent = Vec3d::Zero;
    curve.getPosTanAtDistance(m_distance, curvePos, curveTangent);

    // move on curve
    if (m_distance < curve.getTotalLength())
    {
        Vec3d newPos = m_curveComponent->actorTransform(curvePos);
        m_actor->setPos(newPos);
    }
    else
    {
        // inform physics of final speed
        Vec3d tangent = m_curveComponent->actorTransform(curveTangent, bfalse);
        tangent.normalize();
        Vec2d finalSpeed = m_speed * tangent.truncateTo2D();
        m_physComponent->setSpeed(finalSpeed);
        m_physComponent->resetForces();

        // done!
        m_aiBehavior->onActionFinished();
    }
}

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_AIFollowBezierCurveAction_Template)
BEGIN_SERIALIZATION_CHILD(Ray_AIFollowBezierCurveAction_Template)
    SERIALIZE_MEMBER("speed", m_speed);
END_SERIALIZATION()

//------------------------------------------------------------------------------
Ray_AIFollowBezierCurveAction_Template::Ray_AIFollowBezierCurveAction_Template()
    : Super()
    , m_speed(1.0f)
{
}

} // namespace ITF
