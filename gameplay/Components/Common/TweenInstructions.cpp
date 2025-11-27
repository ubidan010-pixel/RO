#include "precompiled_gameplay.h"

#ifndef _ITF_TWEENINSTRUCTIONS_H_
#include "gameplay/Components/Common/TweenInstructions.h"
#endif //_ITF_TWEENINSTRUCTIONS_H_

#ifndef _ITF_TWEENCOMPONENT_H_
#include "gameplay/Components/Common/TweenComponent.h"
#endif //_ITF_TWEENCOMPONENT_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_EVENTMANAGER_H_
#include "engine/events/EventManager.h"
#endif //_ITF_EVENTMANAGER_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

namespace ITF
{
    ///////////////////////////////////////////////////////////////////////

    IMPLEMENT_OBJECT_RTTI(TweenInstruction_Template)

    BEGIN_SERIALIZATION_ABSTRACT(TweenInstruction_Template)
        SERIALIZE_MEMBER("duration", m_duration);
        SERIALIZE_MEMBER("name", m_name);
    END_SERIALIZATION()

    MY_BEGIN_VALIDATE(TweenInstruction_Template)
        MY_VALIDATE_PARAM("duration", m_duration >= 0.f, "must be positive");
    MY_END_VALIDATE()


    IMPLEMENT_OBJECT_RTTI(TweenInstruction)

    MY_BEGIN_VALIDATE(TweenInstruction)                
#ifndef ITF_FINAL //Do not compile in final target without the test
        ITF_ASSERT_CRASH(m_template, "instruction hasn't been initialized");        
#endif //ITF_FINAL
        MY_VALIDATE_OBJECT("", m_template);
    MY_END_VALIDATE()

    BEGIN_SERIALIZATION_ABSTRACT(TweenInstruction)
        SERIALIZE_MEMBER("name", m_name);
    END_SERIALIZATION()

    void TweenInstruction::init( TweenComponent* _owner, const TweenInstruction_Template* _template )
    {
        m_tweenComponent = _owner;
        m_actor = _owner->GetActor();
        m_template = _template;
        m_name = _template->getName();
        onActorLoaded(Pickable::HotReloadType_None);
    }


    ///////////////////////////////////////////////////////////////////////

    IMPLEMENT_OBJECT_RTTI(TweenTranslation)
    IMPLEMENT_OBJECT_RTTI(TweenTranslation_Template)

    BEGIN_SERIALIZATION_ABSTRACT_CHILD(TweenTranslation_Template)
        SERIALIZE_MEMBER("startDuration", m_timeSmoother.ts);
        SERIALIZE_MEMBER("endDuration", m_timeSmoother.te);
        SERIALIZE_MEMBER("startSpeed", m_timeSmoother.vs);
        SERIALIZE_MEMBER("endSpeed", m_timeSmoother.ve);
        SERIALIZE_MEMBER("rotateTrajectory", m_rotateTrajectory);
        SERIALIZE_MEMBER("rotateActor", m_autoRotate);
        SERIALIZE_MEMBER("angle", m_angle);
        SERIALIZE_MEMBER("angleOffset", m_angleOffset);
        SERIALIZE_MEMBER("scaleMultiplier", m_scaleMultiplier);
    END_SERIALIZATION()

    MY_BEGIN_VALIDATE_CHILD(TweenTranslation_Template)
        MY_VALIDATE_PARAM("startDuration+endDuration", m_timeSmoother.ts + m_timeSmoother.te <= m_duration, "sum must be lower than total duration");
    MY_END_VALIDATE()


    void TweenTranslation_Template::onTemplateLoaded()
    {
        m_timeSmoother.init(m_duration, m_dist);
    }

    void TweenTranslation_Template::execute( const f32 _t, TweenCoordinates& _coordinates ) const
    {
        f32 st = m_timeSmoother.getSmoothedT(_t);
        execute(st, _coordinates,
            _coordinates.getTransform().transformPos( getPosInternal(st) ),
            getAngleInternal(st));
    }

    void TweenTranslation_Template::executeEnd( TweenCoordinates& _coordinates ) const
    {
        executeEnd(_coordinates,
            _coordinates.getTransform().transformPos( m_movement ),
            m_trajectoryRotation, m_actorRotation,
            _coordinates.getTransform().transformAngle(m_endTangentAngle) );
    }

    void TweenTranslation_Template::execute( const f32 _t, TweenCoordinates& _coordinates,
        const Vec3d& _posInternal, const f32 _angleInternal ) const
    {
        _coordinates.m_pos = _posInternal;
        _coordinates.m_scaleMultiplier += m_scaleMultiplier * _t;


        if (m_autoRotate)
        {
            _coordinates.m_actorAngle += _angleInternal;
        }
        else
        {
            _coordinates.m_actorAngle += m_angle.ToRadians() * _t;
        }
        _coordinates.m_actorAngle += m_angleOffset.ToRadians();
    }

    void TweenTranslation_Template::executeEnd( TweenCoordinates& _coordinates,
        const Vec3d& _movement, const f32 _trajectoryRotation,
        const f32 _actorRotation, const f32 _tangentAngle ) const
    {
        _coordinates.m_pos = _movement;
        _coordinates.m_scaleMultiplier += m_scaleMultiplier;


        if (m_autoRotate)
        {
            _coordinates.m_actorAngle += _actorRotation;
        }
        else
        {
            _coordinates.m_actorAngle += m_angle.ToRadians();
        }
        _coordinates.m_actorAngle += m_angleOffset.ToRadians();



        // specific to end

        _coordinates.m_tangentAngle = _tangentAngle;


        if (m_rotateTrajectory)
        {
            _coordinates.m_trajectoryAngle += _trajectoryRotation;
        }
        else
        {
            _coordinates.m_trajectoryAngle = 0.f;
        }
    }

#ifdef ITF_SUPPORT_DEBUGFEATURE
    void TweenTranslation_Template::drawDebug( const Transform3d& _worldTransform, const Color& _color ) const
    {
        // draw exit tangent

        //const Vec3d p0 = _worldTransform.transformPos(m_movement);
        //const Vec3d p1 = p0 + Vec2d::Right.Rotate( _worldTransform.transformAngle(m_endTangentAngle) ).to3d();
        //DebugDraw::line3D(p0, p1, _color, 5);
    }
#endif // ITF_SUPPORT_DEBUGFEATURE

    ///////////////////////////////////////////////////////////////////////

    IMPLEMENT_OBJECT_RTTI(TweenTranslationLine)
    IMPLEMENT_OBJECT_RTTI(TweenTranslationLine_Template)

    BEGIN_SERIALIZATION_CHILD(TweenTranslationLine_Template)
        SERIALIZE_MEMBER("movement", m_movement);
    END_SERIALIZATION()

    void TweenTranslationLine_Template::onTemplateLoaded()
    {
        m_dist = m_movement.norm();
        m_trajectoryRotation = m_dist ? Vec3d_getAngle(m_movement) : 0.f;
        m_actorRotation = m_trajectoryRotation;
        m_endTangentAngle = m_trajectoryRotation;

        if (m_dist < MTH_EPSILON)
        {
            // no linear movement -> use angular distance (start/endSpeed should be given angularly as well, linear speed is zero anyway)
            m_dist = f32_Abs(getAngle());
        }

        Super::onTemplateLoaded() ;
    }

#ifdef ITF_SUPPORT_DEBUGFEATURE
    void TweenTranslationLine_Template::drawDebug( const Transform3d& _worldTransform, const Color& _color ) const
    {
        DebugDraw::line3D(_worldTransform.getPos(), _worldTransform.transformPos(m_movement), _color);

        Super::drawDebug(_worldTransform, _color);
    }
#endif // ITF_SUPPORT_DEBUGFEATURE

    ///////////////////////////////////////////////////////////////////////

    IMPLEMENT_OBJECT_RTTI(TweenTranslationSine)
    IMPLEMENT_OBJECT_RTTI(TweenTranslationSine_Template)

    BEGIN_SERIALIZATION_CHILD(TweenTranslationSine_Template)
        SERIALIZE_MEMBER("movement", m_movement);
        SERIALIZE_MEMBER("amplitude", m_amplitude);
        SERIALIZE_MEMBER("cycleCount", m_cycleCount);
        SERIALIZE_MEMBER("cycleOffset", m_cycleOffset);
    END_SERIALIZATION()

    MY_BEGIN_VALIDATE_CHILD(TweenTranslationSine_Template)
        MY_VALIDATE_PARAM("cycleCount", m_cycleCount > 0.f, "must be strictly positive");
    MY_END_VALIDATE()

    void TweenTranslationSine_Template::onTemplateLoaded()
    {
        m_dist = m_movement.norm();
        m_trajectoryRotation = m_dist ? Vec3d_getAngle(m_movement) : 0.f;
        m_startPos = getSinePos(0.f);       // m_startPos must not be rotated
        m_startAngle = getSineAngle(0.f);
        m_movement = getPosInternal(1.f);
        m_actorRotation = getAngleInternal(1.f);
        m_endTangentAngle = getSineAngle(1.f);

        Super::onTemplateLoaded();
    }

    Vec2d TweenTranslationSine_Template::getSinePos( f32 _t ) const
    {
        f32 y = (_t * m_cycleCount + m_cycleOffset) * MTH_2PI;
        return Vec2d(_t * m_dist, f32_Sin(y) * m_amplitude);
    }

    f32 TweenTranslationSine_Template::getSineAngle( f32 _t ) const
    {
        f32 y = (_t * m_cycleCount + m_cycleOffset) * MTH_2PI;
        return atanf( f32_Cos(y) * m_amplitude );
    }

    Vec3d TweenTranslationSine_Template::getPosInternal( f32 _t ) const
    {
        Vec2d pos = getSinePos(_t);
        pos -= m_startPos;
        if (m_trajectoryRotation)
        {
            pos = pos.Rotate(m_trajectoryRotation);
        }

        f32 z = m_movement.m_z * _t;
        return pos.to3d(z);
    }

    f32 TweenTranslationSine_Template::getAngleInternal( f32 _t ) const
    {
        f32 a = getSineAngle(_t);
        a -= m_startAngle;
        return canonizeAngle(a);
    }

#ifdef ITF_SUPPORT_DEBUGFEATURE
    void TweenTranslationSine_Template::drawDebug( const Transform3d& _worldTransform, const Color& _color ) const
    {
        ITF_ASSERT_CRASH(m_cycleCount > MTH_EPSILON, "cycleCount must be strictly positive!");

        u32 stepCount = (u32)roundFloat(16.f * m_cycleCount);
        f32 stepDt = 1.f / (f32)stepCount;

        DBG3DLine* lineBuffer = GFX_ADAPTER->getDBG3DLineBuffer(stepCount + 1);

        Vec3d prevPos = _worldTransform.getPos();
        for (u32 j = 0; j < stepCount + 1; j++)
        {
            Vec3d pos = _worldTransform.transformPos(getPosInternal(stepDt * j));

            lineBuffer->p1 = prevPos;
            lineBuffer->p2 = pos;
            lineBuffer->r = _color.m_r;
            lineBuffer->g = _color.m_g;
            lineBuffer->b = _color.m_b;
            lineBuffer->duration = 0.f;
            lineBuffer->size = 1.f;
            lineBuffer++;

            prevPos = pos;
        }

        Super::drawDebug(_worldTransform, _color);
    }
#endif // ITF_FINAL

    ///////////////////////////////////////////////////////////////////////

    IMPLEMENT_OBJECT_RTTI(TweenTranslationCircle)
    IMPLEMENT_OBJECT_RTTI(TweenTranslationCircle_Template)

    BEGIN_SERIALIZATION_CHILD(TweenTranslationCircle_Template)
        SERIALIZE_MEMBER("pivot", m_pivot);
        SERIALIZE_MEMBER("cycleCount", m_cycleCount);
    END_SERIALIZATION()

    MY_BEGIN_VALIDATE_CHILD(TweenTranslationCircle_Template)
    MY_END_VALIDATE()

    void TweenTranslationCircle_Template::onTemplateLoaded()
    {
        m_trajectoryRotation = m_cycleCount * MTH_2PI;
        m_radius = m_pivot.truncateTo2D().norm();

        if (m_radius > MTH_EPSILON)
        {
            // pivot -> linear movement
            m_pivotAngle = (m_pivot.truncateTo2D() / m_radius).getAngle();
            m_movement = getPosInternal(1.f);

            // linear distance (start/endSpeed should be given linearly as well)
            m_dist = f32_Abs(m_cycleCount * MTH_2PI * m_radius); // meters
        }
        else
        {
            // no pivot -> no linear movement

            // angular distance (start/endSpeed should be given angularly as well, linear speed is zero anyway)
            const f32 angDist = m_cycleCount ? m_cycleCount * MTH_2PI : getAngle();
            m_dist = f32_Abs(angDist); // radians
        }

        m_actorRotation = m_trajectoryRotation;
        m_endTangentAngle = m_actorRotation - MTH_PIBY2;

        Super::onTemplateLoaded();
    }

    Vec3d TweenTranslationCircle_Template::getPosInternal( f32 _t ) const
    {
        f32 a = (_t * m_cycleCount) * MTH_2PI;
        f32 cos, sin;
        f32_CosSin(a, &cos, &sin);

        Vec2d pos = Vec2d(cos, sin) * m_radius;
        pos = m_pivot.truncateTo2D() - pos.Rotate(m_pivotAngle);

        f32 y = 1 - cos;
        f32 z = m_pivot.m_z * y;

        return pos.to3d(z);
    }

    f32 TweenTranslationCircle_Template::getAngleInternal( f32 _t ) const
    {
        f32 a = (_t * m_cycleCount) * MTH_2PI;
        return canonizeAngle(a);
    }

#ifdef ITF_SUPPORT_DEBUGFEATURE
    void TweenTranslationCircle_Template::drawDebug( const Transform3d& _worldTransform, const Color& _color ) const
    {
        //DebugDraw::circle(_refPos + m_pivot, _z, m_radius, _color);

        //u32 segmentCount = 16;
        //for (u32 i = 0; i <= segmentCount; i++) 
        //{
        //    f32 fi = i * MTH_2PI / (float)_segsCount;
        //    v[i].setData(Vec3d((f32)cos(fi) * r + _x, (f32)sin(fi) * r + _y, _z), color);
        //}

        u32 stepCount = (u32)roundFloat(32.f * f32_Abs(m_cycleCount));
        f32 stepDt = 1.f / (f32)stepCount;

        DBG3DLine* lineBuffer = GFX_ADAPTER->getDBG3DLineBuffer(stepCount + 1);

        Vec3d prevPos = _worldTransform.getPos();
        for (u32 j = 0; j < stepCount + 1; j++)
        {
            Vec3d pos = _worldTransform.transformPos(getPosInternal(stepDt * j));

            lineBuffer->p1 = prevPos;
            lineBuffer->p2 = pos;
            lineBuffer->r = _color.m_r;
            lineBuffer->g = _color.m_g;
            lineBuffer->b = _color.m_b;
            lineBuffer->duration = 0.f;
            lineBuffer->size = 1.f;
            lineBuffer++;

            prevPos = pos;
        }

        GFX_ADAPTER->drawDBGCircle(_worldTransform.transformPos(m_pivot), 0.1f, _color, 0.f, 4);

        Super::drawDebug(_worldTransform, _color);
    }
#endif // ITF_SUPPORT_DEBUGFEATURE

    ///////////////////////////////////////////////////////////////////////

    Bezier4<Vec3d> getBallisticBezier( const Vec3d& _start, const Vec3d& _end )
    {
        static f32 s_leftCursor = 0.5f;
        static f32 s_rightCursor = 0.2f;

        const f32 dist = (_end - _start).norm();
        const f32 jumpHeight = 0.5f * dist;
        const f32 distX = _end.m_x - _start.m_x;

        Bezier4<Vec3d> bezier;
        bezier.m_p0 = _start;
        bezier.m_p1 = _start + Vec3d(distX * s_leftCursor, jumpHeight, 0);
        bezier.m_p2 = _end   + Vec3d(-distX * s_rightCursor, jumpHeight, 0);
        bezier.m_p3 = _end;

        return bezier;
    }

    Bezier4<Vec3d> getBallisticBezier( const Vec3d& _start, const Vec3d& _end, const Vec3d& _startTangent )
    {
        ITF_ASSERT(_startTangent != Vec3d::Zero);

        static f32 s_leftCursor = 0.5f;
        static f32 s_rightCursor = 0.2f;

        const f32 dist = (_end - _start).norm();
        const f32 jumpHeight = 0.5f * dist;
        const f32 distX = _end.m_x - _start.m_x;

        Bezier4<Vec3d> bezier;
        bezier.m_p0 = _start;
        bezier.m_p1 = _start + _startTangent;
        bezier.m_p2 = _end   + Vec3d(-distX * s_rightCursor, jumpHeight, 0);
        bezier.m_p3 = _end;

        return bezier;
    }

    f32 getBezierAngle( const Bezier4<Vec3d>& _bezier, const f32 _t )
    {
        Vec3d tangent = _bezier.getTangent(_t);
        f32 a = Vec3d_getAngle(tangent);
        return a;
    }

    ///////////////////////////////////////////////////////////////////////

    IMPLEMENT_OBJECT_RTTI(TweenBallistic)
    IMPLEMENT_OBJECT_RTTI(TweenBallistic_Template)

    BEGIN_SERIALIZATION_CHILD(TweenBallistic_Template)
        SERIALIZE_MEMBER("movement", m_movement);
        SERIALIZE_TANGENTMODE("tangentMode", m_tangentMode);
        SERIALIZE_MEMBER("startTangent", m_startTangent);
        SERIALIZE_MEMBER("disableCollisions", m_disableCollisions);
        SERIALIZE_MEMBER("speed", m_speed);
    END_SERIALIZATION()

    MY_BEGIN_VALIDATE_CHILD(TweenBallistic_Template)
        //MY_VALIDATE_PARAM("movement", m_movement != Vec3d::Zero, "must not be zero");
    MY_END_VALIDATE()

    BEGIN_SERIALIZATION_INSTANCE(TweenBallistic)
        SERIALIZE_MEMBER("movement", m_movement);
        SERIALIZE_MEMBER("startTangent", m_startTangent);
    END_SERIALIZATION()

    void TweenBallistic::onActorLoaded( Pickable::HotReloadType _hotReload )
    {
        Super::onActorLoaded(_hotReload);

        m_linkComponent = m_actor->GetComponent<LinkComponent>();


        if (m_movement == UNINIT_VEC3D)
        {
            m_movement = getTemplate()->getMovement();
        }
        else if (m_movement.m_z == UNINIT_F32)
        {
            m_movement.m_z = 0; // handles Vec2d -> Vec3d switch
        }

        if (m_startTangent == UNINIT_VEC3D)
        {
            m_startTangent = getTemplate()->getStartTangent();
        }
        else if (m_startTangent.m_z == UNINIT_F32)
        {
            m_startTangent.m_z = 0; // handles Vec2d -> Vec3d switch
        }


        m_duration = getTemplate()->getDuration();
    }

    void TweenBallistic::onBecomeActive( const TweenCoordinates& _coordinates )
    {
        Super::onBecomeActive(_coordinates);

        if (getTemplate()->getDisableCollisions())
        {
            disableCollisions(btrue);
        }

        initBezier(_coordinates);
    }

    void TweenBallistic::onBecomeInactive()
    {
        Super::onBecomeInactive();

        if (getTemplate()->getDisableCollisions())
        {
            disableCollisions(bfalse);
        }
    }

    void TweenBallistic::disableCollisions( const bbool _disable )
    {
        EventDisableCollision evt;
        evt.setDisable(_disable);
        m_actor->onEvent(&evt);

        if (m_linkComponent)
        {
            m_linkComponent->sendEventToChildren(&evt);
        }

        AIUtils::sendEventToBoundChildren(m_actor, &evt);
    }


    Bezier4<Vec3d> TweenBallistic::getBezier( const TweenCoordinates& _coordinates ) const
    {
        // pre-apply set transform on bezier; once the set is started, we'll always work in init space directly and ignore set transform

        Vec3d startPos = _coordinates.m_pos;
        Vec3d endPos = m_movement;
        f32 tangentAngle = _coordinates.m_tangentAngle;

        if (!isSerializable())
        {
            Transform3d transform = _coordinates.getTransform();
            endPos = transform.transformPos(getTemplate()->getMovement());
            tangentAngle = transform.transformAngle(tangentAngle);
        }

        switch (getTemplate()->getTangentMode())
        {
        case TweenBallistic_Template::TangentMode_Auto:
            {
                Vec3d startTangent = Vec2d::Right.Rotate(tangentAngle).to3d();
                return getBallisticBezier(startPos, endPos, startTangent);
            }
        case TweenBallistic_Template::TangentMode_Custom:
            {
                ITF_ASSERT(isSerializable());
                return getBallisticBezier(startPos, endPos, m_startTangent);
            }
        default: // TangentMode_Jump
            {
                return getBallisticBezier(startPos, endPos);
            }
        }
    }

    void TweenBallistic::initBezier( const TweenCoordinates& _coordinates )
    {
        m_bezier = getBezier(_coordinates);

        Vec3d bezierMovement = (m_bezier.m_p3 - m_bezier.m_p0);
        const f32 dist = bezierMovement.norm();
        m_trajectoryRotation = dist ? Vec3d_getAngle(bezierMovement) : 0.f;

        m_startAngle = getBezierAngle(m_bezier, 0.f);
        m_endTangentAngle = getBezierAngle(m_bezier, 1.f);
        m_actorRotation = getAngleInternal(1.f);    // substracts startAngle


        if (getTemplate()->getSpeed())
        {
            m_duration = dist / getTemplate()->getSpeed();
        }

        m_timeSmoother = getTemplate()->getTimeSmoother();
        m_timeSmoother.init(m_duration, dist);
    }

    Vec3d TweenBallistic::getPosInternal( f32 _t ) const
    {
        return m_bezier.getPos(_t);
    }

    f32 TweenBallistic::getAngleInternal( f32 _t ) const
    {
        f32 a = getBezierAngle(m_bezier, _t) - m_startAngle;
        return canonizeAngle(a);
    }


    void TweenBallistic::execute( const f32 _t, TweenCoordinates& _coordinates ) const
    {
        const f32 st = m_timeSmoother.getSmoothedT(_t);
        getTemplate()->execute(_t, _coordinates,
            getPosInternal(st), getAngleInternal(st));
    }

    void TweenBallistic::executeEnd( TweenCoordinates& _coordinates ) const
    {
        Vec3d endPos = m_movement;
        if (!isSerializable())
        {
            Transform3d transform = _coordinates.getTransform();
            endPos = transform.transformPos(getTemplate()->getMovement());
        }

        getTemplate()->executeEnd(_coordinates, endPos,
            m_trajectoryRotation, m_actorRotation, m_endTangentAngle);
    }

#ifdef ITF_SUPPORT_DEBUGFEATURE
    void TweenBallistic::drawDebug( const Transform3d& _initTransform, const TweenCoordinates& _coordinates, const Color& _color ) const
    {
        const Bezier4<Vec3d> bezierInit = getBezier(_coordinates);

        Bezier4<Vec3d> bezierWorld;
        bezierWorld.m_p0 = _initTransform.transformPos(bezierInit.m_p0);
        bezierWorld.m_p1 = _initTransform.transformPos(bezierInit.m_p1);
        bezierWorld.m_p2 = _initTransform.transformPos(bezierInit.m_p2);
        bezierWorld.m_p3 = _initTransform.transformPos(bezierInit.m_p3);

        DebugDraw::bezier(bezierWorld, _color, 1.f, 0.f);

        //Super::drawDebug(_initTransform, _color);
        getTemplate()->drawDebug(_initTransform, _color);
    }
#endif // ITF_SUPPORT_DEBUGFEATURE

    ///////////////////////////////////////////////////////////////////////

    IMPLEMENT_OBJECT_RTTI(TweenInstructionWait)
    IMPLEMENT_OBJECT_RTTI(TweenInstructionWait_Template)

    BEGIN_SERIALIZATION_CHILD(TweenInstructionWait_Template)
    END_SERIALIZATION()



    ///////////////////////////////////////////////////////////////////////

    IMPLEMENT_OBJECT_RTTI(TweenInstructionEvent)
    IMPLEMENT_OBJECT_RTTI(TweenInstructionEvent_Template)

    BEGIN_SERIALIZATION_CHILD(TweenInstructionEvent_Template)
        SERIALIZE_OBJECT_WITH_FACTORY("event", m_event, ACTORSMANAGER->getEventFactory());
        SERIALIZE_MEMBER("triggerSelf", m_triggerSelf);
        SERIALIZE_MEMBER("triggerChildren", m_triggerChildren);
        SERIALIZE_MEMBER("triggerBoundChildren", m_triggerBoundChildren);
        SERIALIZE_MEMBER("triggerGameManager", m_triggerGameManager);
        SERIALIZE_MEMBER("triggerBroadcast", m_triggerBroadcast);
    END_SERIALIZATION()

    TweenInstructionEvent_Template::~TweenInstructionEvent_Template()
    {
        SF_DEL(m_event);
    }

    TweenInstructionEvent::~TweenInstructionEvent()
    {
        SF_DEL(m_event);
    }

    void TweenInstructionEvent::onActorLoaded( Pickable::HotReloadType _hotReload )
    {
        Super::onActorLoaded(_hotReload);
        
        if ( getTemplate()->getEvent() )
        {
            m_event = getTemplate()->getEvent()->CreateNewObject()->DynamicCast<Event>(ITF_GET_STRINGID_CRC(Event,772421353));
            BinaryClone(const_cast<Event*>(getTemplate()->getEvent()),m_event);
        }
    }

    void TweenInstructionEvent::onBecomeActive( const TweenCoordinates& _coordinates )
    {
        Super::onBecomeActive(_coordinates);

        if (!m_event)
        {
            return;
        }


        m_event->setSender(m_actor->getRef());

        if (getTemplate()->getTriggerSelf())
        {
            m_actor->onEvent(m_event);
        }

        if (getTemplate()->getTriggerChildren())
        {
            m_tweenComponent->sendEventToChildren(m_event);
        }

        if (getTemplate()->getTriggerBoundChildren())
        {
            AIUtils::sendEventToBoundChildren(m_actor, m_event);
        }

        if (getTemplate()->getTriggerGameManager())
        {
            GAMEMANAGER->onEvent(m_event);
        }

        if (getTemplate()->getTriggerBroadcast())
        {
            EVENTMANAGER->broadcastEvent(m_event);
        }
    }


    ///////////////////////////////////////////////////////////////////////

    IMPLEMENT_OBJECT_RTTI(TweenInstructionFX)
    IMPLEMENT_OBJECT_RTTI(TweenInstructionFX_Template)

    BEGIN_SERIALIZATION_CHILD(TweenInstructionFX_Template)
        SERIALIZE_MEMBER("fx", m_fxControlID);
        SERIALIZE_MEMBER("stop", m_stop);
    END_SERIALIZATION()

    BEGIN_SERIALIZATION_INSTANCE(TweenInstructionFX)
        SERIALIZE_MEMBER("target", m_target);
    END_SERIALIZATION()

    void TweenInstructionFX::onActorLoaded( Pickable::HotReloadType _hotReload )
    {
        Super::onActorLoaded(_hotReload);

        m_fxController = m_actor->GetComponent<FXControllerComponent>();
        //ITF_WARNING(m_actor, m_fxController, "TweenInstructionFX requires an FXControllerComponent");
    }

    void TweenInstructionFX::onBecomeActive( const TweenCoordinates& _coordinates )
    {
        Super::onBecomeActive(_coordinates);

        if (!m_fxController)
        {
            return;
        }

        if (!getTemplate()->getStop())
        {
            if (m_fxPlayed)
            {
                const FXControl* fx = m_fxController->getFXControl(getTemplate()->getFXControlID());
                if (fx && fx->m_fxPlayOnce)
                {
                    return;
                }
            }

            u32 fxHandle = m_fxController->playFX(getTemplate()->getFXControlID());
            m_fxPlayed = btrue;

            if ( Pickable* target = getTarget() )
            {
                m_fxController->setFXPosFromHandle(fxHandle, target->getPos());
            }
        }
        else
        {
            m_fxController->stopFXFromHandle(getTemplate()->getFXControlID());
            m_fxPlayed = bfalse;
        }
    }

    void TweenInstructionFX::onSetFinished()
    {
        if (m_fxPlayed)
        {
            m_fxPlayed = bfalse;

            const FXControl* fx = m_fxController->getFXControl(getTemplate()->getFXControlID());
            if (fx && fx->m_fxStopOnEndAnim)
            {
                m_fxController->stopFXFromHandle(getTemplate()->getFXControlID());
            }
        }
    }

    Pickable* TweenInstructionFX::getTarget() const
    {
        if (m_target.isValid())
        {
            ITF_ASSERT(!m_target.getIsAbsolute());
            return SceneObjectPathUtils::getObjectFromRelativePath(m_actor, m_target);
        }

        return NULL;
    }

#ifdef ITF_SUPPORT_DEBUGFEATURE
    void TweenInstructionFX::drawDebug( const Transform3d& _initTransform, const TweenCoordinates& _coordinates, const Color& _color ) const
    {
        if (getTemplate()->getStop())
        {
            return;
        }

        Vec3d startPos = _initTransform.transformPos(_coordinates.m_pos);

        Vec3d targetPos;
        if ( Pickable* target = getTarget() )
        {
            targetPos = target->getPos();
            DebugDraw::line3D(startPos, targetPos, Color::white());
        }
        else
        {
            targetPos = startPos;
        }

        DebugDraw::text(targetPos, Vec2d::Zero, Color::white(), DebugDraw::defaultBackgroundColor(),
            getTemplate()->getFXControlID().getDebugString());
    }
#endif // ITF_SUPPORT_DEBUGFEATURE

    ///////////////////////////////////////////////////////////////////////

    IMPLEMENT_OBJECT_RTTI(TweenInstructionInput)
    IMPLEMENT_OBJECT_RTTI(TweenInstructionInput_Template)

    BEGIN_SERIALIZATION_CHILD(TweenInstructionInput_Template)
        SERIALIZE_MEMBER("inputName", m_inputName);
        SERIALIZE_MEMBER("floatValue", m_inputValue_f32);
        SERIALIZE_MEMBER("uintValue", m_inputValue_u32);
    END_SERIALIZATION()

    void TweenInstructionInput::onBecomeActive( const TweenCoordinates& _coordinates )
    {
        Super::onBecomeActive(_coordinates);

        if (getTemplate()->getInputName().isValid())
        {
            if (getTemplate()->getInputValue_f32() != UNINIT_F32)
            {
                EventSetFloatInput inputEvent;
                inputEvent.setInputName(getTemplate()->getInputName());
                inputEvent.setInputValue(getTemplate()->getInputValue_f32());
                m_actor->onEvent(&inputEvent);
            }

            if (getTemplate()->getInputValue_u32() != U32_INVALID)
            {
                EventSetUintInput inputEvent;
                inputEvent.setInputName(getTemplate()->getInputName());
                inputEvent.setInputValue(getTemplate()->getInputValue_u32());
                m_actor->onEvent(&inputEvent);
            }
        }
    }



    ///////////////////////////////////////////////////////////////////////

    IMPLEMENT_OBJECT_RTTI(TweenInstructionAnim)
    IMPLEMENT_OBJECT_RTTI(TweenInstructionAnim_Template)

    BEGIN_SERIALIZATION_CHILD(TweenInstructionAnim_Template)
        SERIALIZE_MEMBER("anim", m_anim);
    END_SERIALIZATION()

    void TweenInstructionAnim::onActorLoaded( Pickable::HotReloadType _hotReload )
    {
        Super::onActorLoaded(_hotReload);

        m_animComponent = m_actor->GetComponent<AnimLightComponent>();
        ITF_WARNING(m_actor, m_animComponent, "TweenInstructionAnim requires an AnimLightComponent/AnimatedComponent");
    }

    void TweenInstructionAnim::onBecomeActive( const TweenCoordinates& _coordinates )
    {
        Super::onBecomeActive(_coordinates);

        if (m_animComponent)
        {
            m_animComponent->setAnim(getTemplate()->getAnim());
        }
    }



    ///////////////////////////////////////////////////////////////////////

    IMPLEMENT_OBJECT_RTTI(TweenInstructionFlip)
    IMPLEMENT_OBJECT_RTTI(TweenInstructionFlip_Template)

    BEGIN_SERIALIZATION_CHILD(TweenInstructionFlip_Template)
    END_SERIALIZATION()

    void TweenInstructionFlip::onBecomeActive( const TweenCoordinates& _coordinates )
    {
        Super::onBecomeActive(_coordinates);

        m_actor->setFlipped(!m_actor->isFlipped());
    }



    ///////////////////////////////////////////////////////////////////////

    IMPLEMENT_OBJECT_RTTI(TweenInstructionPlaySet)
    IMPLEMENT_OBJECT_RTTI(TweenInstructionPlaySet_Template)

    BEGIN_SERIALIZATION_CHILD(TweenInstructionPlaySet_Template)
        SERIALIZE_MEMBER("set", m_instructionSetName);
    END_SERIALIZATION()

    MY_BEGIN_VALIDATE_CHILD(TweenInstructionPlaySet_Template)
        MY_VALIDATE_PARAM("set", m_instructionSetName.isValid(), "please provide a valid name");
    MY_END_VALIDATE()

    void TweenInstructionPlaySet::onBecomeActive( const TweenCoordinates& _coordinates )
    {
        Super::onBecomeActive(_coordinates);

        m_tweenComponent->playSet(getTemplate()->getInstructionSetName());
    }



    ///////////////////////////////////////////////////////////////////////

    IMPLEMENT_OBJECT_RTTI(TweenTeleport)
    IMPLEMENT_OBJECT_RTTI(TweenTeleport_Template)

    BEGIN_SERIALIZATION_CHILD(TweenTeleport_Template)
        SERIALIZE_MEMBER("position", m_position);
        SERIALIZE_MEMBER("resetAngle", m_resetAngle);
    END_SERIALIZATION()

    MY_BEGIN_VALIDATE_CHILD(TweenTeleport_Template)
    MY_END_VALIDATE()

    void TweenTeleport_Template::execute( const f32 _t, TweenCoordinates& _coordinates ) const
    {
        _coordinates.m_pos = m_position;
    }

    void TweenTeleport_Template::executeEnd( TweenCoordinates& _coordinates ) const
    {
        _coordinates.m_pos = m_position;
        _coordinates.m_trajectoryAngle = 0.f;
        _coordinates.m_tangentAngle = 0.f;
        if (m_resetAngle)
        {
            _coordinates.m_actorAngle = 0.f;
        }
    }

#ifdef ITF_SUPPORT_DEBUGFEATURE
    void TweenTeleport_Template::drawDebug( const Transform3d& _initTransform, const TweenCoordinates& _coordinates, const Color& _color ) const
    {
        const Vec3d startPos = _initTransform.transformPos(_coordinates.m_pos);
        const Vec3d endPos = _initTransform.transformPos(m_position);
        DebugDraw::dottedLine(startPos, endPos, 10, _color);
        DebugDraw::text(endPos, Vec2d::Zero, Color::white(), DebugDraw::defaultBackgroundColor(), "TP");
    }
#endif // ITF_SUPPORT_DEBUGFEATURE

    void TweenInstructionFactory_Template::init()
    {
        RegisterObject<TweenTranslationLine_Template>(ITF_GET_STRINGID_CRC(TweenTranslationLine_Template,3862585322));
        RegisterObject<TweenTranslationSine_Template>(ITF_GET_STRINGID_CRC(TweenTranslationSine_Template,2798634649));
        RegisterObject<TweenTranslationCircle_Template>(ITF_GET_STRINGID_CRC(TweenTranslationCircle_Template,410144229));
        RegisterObject<TweenBallistic_Template>(ITF_GET_STRINGID_CRC(TweenBallistic_Template,1898920804));
        RegisterObject<TweenInstructionWait_Template>(ITF_GET_STRINGID_CRC(TweenInstructionWait_Template,71414445));
        RegisterObject<TweenInstructionEvent_Template>(ITF_GET_STRINGID_CRC(TweenInstructionEvent_Template,372486786));
        RegisterObject<TweenInstructionFX_Template>(ITF_GET_STRINGID_CRC(TweenInstructionFX_Template,1721356376));
        RegisterObject<TweenInstructionInput_Template>(ITF_GET_STRINGID_CRC(TweenInstructionInput_Template,2854985971));
        RegisterObject<TweenInstructionAnim_Template>(ITF_GET_STRINGID_CRC(TweenInstructionAnim_Template,2809340194));
        RegisterObject<TweenInstructionFlip_Template>(ITF_GET_STRINGID_CRC(TweenInstructionFlip_Template,1619460815));
        RegisterObject<TweenInstructionPlaySet_Template>(ITF_GET_STRINGID_CRC(TweenInstructionPlaySet_Template,39621327));
        RegisterObject<TweenTeleport_Template>(ITF_GET_STRINGID_CRC(TweenTeleport_Template,1685727569));
    }

    void TweenInstructionFactory::init()
    {
        //RegisterObject<TweenBallistic>(ITF_GET_STRINGID_CRC(TweenBallistic,1641159219));
        //RegisterObject<TweenInstructionFX>(ITF_GET_STRINGID_CRC(TweenInstructionFX,3479894667));

        RegisterObject<TweenTranslationLine>(ITF_GET_STRINGID_CRC(TweenTranslationLine,327013100));
        RegisterObject<TweenTranslationSine>(ITF_GET_STRINGID_CRC(TweenTranslationSine,1348589277));
        RegisterObject<TweenTranslationCircle>(ITF_GET_STRINGID_CRC(TweenTranslationCircle,2879495710));
        RegisterObject<TweenBallistic>(ITF_GET_STRINGID_CRC(TweenBallistic,1641159219));
        RegisterObject<TweenInstructionWait>(ITF_GET_STRINGID_CRC(TweenInstructionWait,2978193431));
        RegisterObject<TweenInstructionEvent>(ITF_GET_STRINGID_CRC(TweenInstructionEvent,2636892017));
        RegisterObject<TweenInstructionFX>(ITF_GET_STRINGID_CRC(TweenInstructionFX,3479894667));
        RegisterObject<TweenInstructionInput>(ITF_GET_STRINGID_CRC(TweenInstructionInput,1422489750));
        RegisterObject<TweenInstructionAnim>(ITF_GET_STRINGID_CRC(TweenInstructionAnim,1652757797));
        RegisterObject<TweenInstructionFlip>(ITF_GET_STRINGID_CRC(TweenInstructionFlip,2996166264));
        RegisterObject<TweenInstructionPlaySet>(ITF_GET_STRINGID_CRC(TweenInstructionPlaySet,915643568));
        RegisterObject<TweenTeleport>(ITF_GET_STRINGID_CRC(TweenTeleport,240238977));
    }

    void TimeSmoother::init( const f32 _duration, const f32 _dist )
    {
        m_duration = _duration;
        m_dist = _dist;

        ITF_ASSERT(m_dist >= 0);
        f32 tm = m_duration - ts - te;
        vm = (m_dist - 0.5f * vs * ts - 0.5f * ve * te) / (tm + 0.5f * ts + 0.5f * te);

        ds = 0.5f * ts * (vs + vm);
        f32 de = 0.5f * te * (ve + vm);
        dm = m_dist - ds - de;

        as = (vm - vs) / ts;
        ae = (ve - vm) / te;

        //f32 ds2 = vs * ts + 0.5f * as * ts * ts;
        //f32 de2 = vm * te + 0.5f * ae * te * te;
        //f32 dm2 = vm * tm;
    }

    f32 TimeSmoother::getSmoothedT( f32 _t ) const
    {
        if (m_dist <= MTH_EPSILON)
        {
            return _t;
        }

        f32 t = _t * m_duration;
        f32 d = 0.f;

        if (t < ts)
        {
            // start
            d = vs * t + 0.5f * as * t * t;
        }
        else if (t <= (m_duration - te)) 
        {
            // middle
            t -= ts;
            d = ds + vm * t;
        }
        else
        {
            // end
            t -= (m_duration - te);
            d = ds + dm + vm * t + 0.5f * ae * t * t;
        }

        return d / m_dist;
    }

};
