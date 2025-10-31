#include "precompiled_gameplay.h"

// GLOBAL ENGINE CONFIG FILE. Must be the first included file
#ifndef _ENGINECOMMON_H_
#include "core/enginecommon.h"
#endif // _ENGINECOMMON_H_

#ifndef _ITF_AIBEZIERACTION_H_
#include "gameplay/ai/Actions/AIBezierAction.h"
#endif //_ITF_AIBEZIERACTION_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifdef ITF_SUPPORT_EDITOR
#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_
#endif

#ifndef _ITF_GAMEPLAYEVENTS_H_
#include "gameplay/GameplayEvents.h"
#endif //_ITF_GAMEPLAYEVENTS_H_

#ifndef _ITF_PHYSCOMPONENT_H_
#include "engine/actors/components/physcomponent.h"
#endif //_ITF_PHYSCOMPONENT_H_

namespace ITF
{

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(AIBezierAction)
BEGIN_VALIDATE_ACTION(AIBezierAction)
    VALIDATE_ACTION_PARAM("duration", ( getTemplate()->getSpeed()>0.0f ) || ( getTemplate()->getFixedDuration() > 0.0f ), "Positive speed or duration required");
    VALIDATE_ACTION_PARAM("updatePhysSpeedAtEnd", !(( getTemplate()->getFixedDuration() > 0.0f ) && (getTemplate()->getUpdatePhysSpeedAtEnd())), "Can't update physics speed with fixed duration");
END_VALIDATE_ACTION()

//------------------------------------------------------------------------------
AIBezierAction::AIBezierAction()
    : Super()
    , m_timer(0.0f)
    , m_t(0.0f)
    , m_p0(Vec3d::Zero)
    , m_p1(Vec3d::Zero)
    , m_p2(Vec3d::Zero)
    , m_p3(Vec3d::Zero)
    , m_physWasDisabled(bfalse)
    , m_physComponent(NULL)
{
}

//------------------------------------------------------------------------------
AIBezierAction::~AIBezierAction()
{
}

//------------------------------------------------------------------------------
void AIBezierAction::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    // not mandatory
    m_physComponent = m_actor->GetComponent<PhysComponent>();
        
    // listen to bezier setup event
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventSetupBezier,713993105),m_aiComponent);
}

//------------------------------------------------------------------------------
void AIBezierAction::onActivate()
{
    Super::onActivate();

    // make sure the physics component is disabled if present
    if (m_physComponent)
    {
        m_physWasDisabled = m_physComponent->getDisabled();
        m_physComponent->setDisabled(true);
    }
    
    // init timer
    m_timer = 0.0f;
    m_t = 0.0f;
}

//------------------------------------------------------------------------------
void AIBezierAction::onDeactivate()
{
    Super::onDeactivate();

    // re-enable physics if disabled previously
    if (m_physComponent)
    {
        m_physComponent->setDisabled(m_physWasDisabled);
    }
}

//------------------------------------------------------------------------------
void AIBezierAction::update(f32 _dt)
{
    Super::update(_dt);
    
    // update timer
    m_timer += _dt;
    
    // compute new position
    if ( getTemplate()->getFixedDuration() > 0.f )
    {
        f32 cursor = m_timer / getTemplate()->getFixedDuration();
        cursor = Clamp( cursor, 0.0f, 1.0f );
        f32 invCursor = 1.f - cursor;
        f32 factor = 0.f;
        switch ( getTemplate()->getFixedDurationInterpolType() )
        {
            case linear :   factor = cursor; break;
            case X2 :       factor = cursor*cursor; break;
            case X3 :       factor = cursor*cursor*cursor; break;
            case X4 :       factor = cursor*cursor*cursor*cursor; break;
            case X5 :       factor = cursor*cursor*cursor*cursor*cursor; break;
            case invX2 :    factor = 1.f - ( invCursor*invCursor ); break;
            case invX3 :    factor = 1.f - ( invCursor*invCursor*invCursor ); break;
            case invX4 :    factor = 1.f - ( invCursor*invCursor*invCursor*invCursor ); break;
            case invX5 :    factor = 1.f - ( invCursor*invCursor*invCursor*invCursor*invCursor ); break;
        }
        
        m_t = Clamp( factor, 0.0f, 1.0f );

        //m_t = Clamp( m_timer / getTemplate()->getFixedDuration(), 0.0f, 1.0f );
    }
    else
    {
        f32 distance = (m_p3 - m_p0).norm();
        m_t = Clamp(getTemplate()->getSpeed() * m_timer / distance, 0.0f, 1.0f);
    }

    // set new pos
    Vec3d pos = getBezierPosition(m_p0, m_p1, m_p2, m_p3, m_t);
    m_actor->setBoundWorldPos(pos);

    if (getTemplate()->getChangeAngle())
    {
        // orient along curve
        Vec3d tangent = getBezierTangent(m_p0, m_p1, m_p2, m_p3, m_t);
        tangent.normalize();
        
        bbool flipped = bfalse;
        f32 newAngle = 0.0f;
        AIUtils::getAngleAndFlipped(tangent.truncateTo2D(), newAngle, flipped);

        m_actor->setFlipped(flipped);
        m_actor->setBoundWorldAngle(newAngle);
    }
    else
    {
        // no rotation
        m_actor->setBoundWorldAngle(0);
    }

    // check when done
    if (m_t >= 1.0f)
    {
        // notify physics component of new speed (2D)
        if (m_physComponent && getTemplate()->getUpdatePhysSpeedAtEnd())
        {
            Vec2d speed2d = getEndTangent().truncateTo2D() * getTemplate()->getSpeed();
            m_physComponent->setSpeed(speed2d);
        }
        
        m_aiBehavior->onActionFinished();
    }
}

//------------------------------------------------------------------------------
void AIBezierAction::onEvent(Event * _event)
{
    Super::onEvent(_event);
    
    // process events
    if (EventSetupBezier* setupBezierEvt = _event->DynamicCast<EventSetupBezier>(ITF_GET_STRINGID_CRC(EventSetupBezier,713993105)))
    {
        setupBezier(
            setupBezierEvt->getP0(),
            setupBezierEvt->getP1(),
            setupBezierEvt->getP2(),
            setupBezierEvt->getP3()
            );
    }
}

//------------------------------------------------------------------------------
void AIBezierAction::updateAnimInputs()
{
    // cursor for logical animations
    m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(BezierCursor,1278433106), m_t);
}

//------------------------------------------------------------------------------
void AIBezierAction::setupBezier(
    const Vec3d& _p0,
    const Vec3d& _p1,
    const Vec3d& _p2,
    const Vec3d& _p3
    )
{
    m_p0 = _p0;
    m_p1 = _p1;
    m_p2 = _p2;
    m_p3 = _p3;
    
    // set init pos
    m_actor->setBoundWorldPos(m_p0);
}

#ifdef ITF_SUPPORT_EDITOR
//------------------------------------------------------------------------------
void AIBezierAction::drawEdit(ActorDrawEditInterface* _drawInterface, u32 _flags) const
{
    Super::drawEdit(_drawInterface, _flags);

    // debug draw
    static const f32 stepCount = 10;
    static const f32 timeStep = 1.0f / stepCount;
    static const f32 lineWidth = 4.0f;
    Vec3d prevPos = m_p0;
    for (f32 i=1; i<=stepCount; ++i)
    {
        Vec3d pos = getBezierPosition(m_p0, m_p1, m_p2, m_p3, i*timeStep);
        DebugDraw::line3D(prevPos, pos, Color::magenta(), lineWidth);
        prevPos = pos;
    }
}
#endif // ITF_SUPPORT_EDITOR

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(AIBezierAction_Template)
BEGIN_SERIALIZATION_CHILD(AIBezierAction_Template)
    SERIALIZE_MEMBER("changeAngle", m_changeAngle);
    SERIALIZE_MEMBER("updatePhysSpeedAtEnd", m_updatePhysSpeedAtEnd);
    SERIALIZE_MEMBER("speed", m_speed);
    SERIALIZE_MEMBER("fixedDuration", m_fixedDuration );
    SERIALIZE_MEMBER("fixedDurationInterpolType", m_fixedDurationInterpolType );
END_SERIALIZATION()

//------------------------------------------------------------------------------
AIBezierAction_Template::AIBezierAction_Template()
    : Super()
    , m_changeAngle(bfalse)
    , m_updatePhysSpeedAtEnd(bfalse)
    , m_speed(1.0f)
    , m_fixedDuration( 0.0f )
    , m_fixedDurationInterpolType( 0 )
{
    m_animDefaultAction = ITF_GET_STRINGID_CRC(BEZIER,4061805659);
}

//------------------------------------------------------------------------------
AIBezierAction_Template::~AIBezierAction_Template()
{
}

} // namespace ITF
