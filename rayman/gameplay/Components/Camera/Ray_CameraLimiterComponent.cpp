#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_CAMERALIMITERCOMPONENT_H_
#include "rayman/gameplay/Components/Camera/Ray_CameraLimiterComponent.h"
#endif //_ITF_RAY_CAMERALIMITERCOMPONENT_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_CameraLimiterComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_CameraLimiterComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(Ray_CameraLimiterComponent)
    END_VALIDATE_COMPONENT()


    Ray_CameraLimiterComponent::Ray_CameraLimiterComponent()
        : Super()
        , m_ejectMargin(-1.f, -1.f, -1.f, -1.f)
        , m_deathMargin(1.f, 1.f, 1.f, 1.f)
    {
    }

    Ray_CameraLimiterComponent::~Ray_CameraLimiterComponent()
    {
    }

    void Ray_CameraLimiterComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventNodeReached,2473939155), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventQueryCameraLimiter,358012532), this);

        // copy defaults from templates; they may be overridden by nodes
        m_ejectMargin = getTemplate()->getEjectMargin();
        m_deathMargin = getTemplate()->getDeathMargin();
    }

    void Ray_CameraLimiterComponent::onEvent( Event* _event )
    {
        Super::onEvent(_event);

        if ( Ray_EventNodeReached* nodeReached = _event->DynamicCast<Ray_EventNodeReached>(ITF_GET_STRINGID_CRC(Ray_EventNodeReached,2473939155)) )
        {
            processNodeReached(nodeReached);
        }
        else if ( Ray_EventQueryCameraLimiter* query = _event->DynamicCast<Ray_EventQueryCameraLimiter>(ITF_GET_STRINGID_CRC(Ray_EventQueryCameraLimiter,358012532)) )
        {
            processQueryCameraLimiter(query);
        }
    }

    void Ray_CameraLimiterComponent::processNodeReached( Ray_EventNodeReached* _nodeReached )
    {
        if (!_nodeReached->getCameraEjectMargin().isZero())
        {
            m_ejectMargin = _nodeReached->getCameraEjectMargin();
        }

        if (!_nodeReached->getCameraDeathMargin().isZero())
        {
            m_deathMargin = _nodeReached->getCameraDeathMargin();
        }
    }

    void Ray_CameraLimiterComponent::processQueryCameraLimiter( Ray_EventQueryCameraLimiter* _query )
    {
        Ray_CameraLimiterData& limiterData = _query->getLimiterData();

        limiterData.setUseEjectMargin(getTemplate()->getUseEjectMargin());
        if (getTemplate()->getUseEjectMargin())
        {
            limiterData.setEjectMargin(m_ejectMargin);
            limiterData.setEjectMarginActive(MarginBool(btrue, btrue, btrue, btrue));
            limiterData.setEjectMarginDetachesPlayer(getTemplate()->getEjectMarginDetachesPlayer());
            limiterData.setEjectForce(getTemplate()->getEjectForce());
        }

        limiterData.setUseDeathMargin(getTemplate()->getUseDeathMargin());
        if (getTemplate()->getUseDeathMargin())
        {
            limiterData.setDeathMargin(m_deathMargin);
        }

        if (getTemplate()->getIgnoreConstraint())
        {
            limiterData.setConstraintActive(MarginBool());
        }

        limiterData.setTimeOut(getTemplate()->getTimeOut());

        limiterData.setLastOnscreenPlayerKillsEveryone(getTemplate()->getLastOnscreenPlayerKillsEveryone());

        _query->setQueryHandled(btrue);
    }

#ifndef ITF_FINAL
    /*void Ray_BossCameraComponent::drawDebug( const CameraControllerData& _data )
    {
        Super::drawDebug(_data);

        if (_data.m_weight <= MTH_EPSILON)
        {
            return;
        }

        Vec3d camPos = getCameraPosition();

        f32 screenY_by2 = f32_Abs( tan(getCameraFocale() * 0.5f) * m_zOffset );
        f32 screenX_by2 = screenY_by2 * CAMERA->m_screen_ratio;

        AABB screenAABB = AABB(
            Vec2d(camPos.m_x - screenX_by2, camPos.m_y - screenY_by2),
            Vec2d(camPos.m_x + screenX_by2, camPos.m_y + screenY_by2)
            );

        AABB ejectAABB = screenAABB;
        m_ejectMargin.addTo(ejectAABB);

        AABB deathAABB = screenAABB;
        m_deathMargin.addTo(deathAABB);


        Color screenColor = Color::black();
        screenColor.m_a = _data.m_weight;
        DebugDraw::AABB(Vec2d::Zero, m_actor->getDepth(), 0.f, screenAABB,
            screenColor, 0.f, 5.f);

        Color ejectColor = Color::orange();
        ejectColor.m_a = _data.m_weight;
        DebugDraw::AABB(Vec2d::Zero, m_actor->getDepth(), 0.f, ejectAABB,
            ejectColor, 0.f, 1.f, "eject");

        Color deathColor = Color::red();
        deathColor.m_a = _data.m_weight;
        DebugDraw::AABB(Vec2d::Zero, m_actor->getDepth(), 0.f, deathAABB,
            deathColor, 0.f, 1.f, "death");
    }*/
#endif // ITF_FINAL

    //-------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(Ray_CameraLimiterComponent_Template)
    BEGIN_SERIALIZATION(Ray_CameraLimiterComponent_Template)
        SERIALIZE_MEMBER("useEjectMargin", m_useEjectMargin);
        SERIALIZE_OBJECT("ejectMargin", m_ejectMargin);
        SERIALIZE_MEMBER("ejectMarginDetachesPlayer", m_ejectMarginDetachesPlayer);
        SERIALIZE_MEMBER("ejectForce", m_ejectForce);
        SERIALIZE_MEMBER("useDeathMargin", m_useDeathMargin);
        SERIALIZE_OBJECT("deathMargin", m_deathMargin);
        SERIALIZE_MEMBER("ignoreConstraint", m_ignoreConstraint);
        SERIALIZE_MEMBER("timeOut", m_timeOut);
        SERIALIZE_MEMBER("lastOnscreenPlayerKillsEveryone", m_lastOnscreenPlayerKillsEveryone);
    END_SERIALIZATION()

};