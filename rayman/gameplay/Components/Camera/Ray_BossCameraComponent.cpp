#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_BOSSCAMERACOMPONENT_H_
#include "rayman/gameplay/Components/Camera/Ray_BossCameraComponent.h"
#endif //_ITF_RAY_BOSSCAMERACOMPONENT_H_

#ifndef _ITF_RAY_BOSSPLANTNODECOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_BossPlantNodeComponent.h"
#endif //_ITF_RAY_BOSSPLANTNODECOMPONENT_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/Managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_CAMERACONTROLLERMANAGER_H_
#include "engine/actors/components/Camera/CameraControllerManager.h"
#endif // _ITF_CAMERACONTROLLERMANAGER_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_BossCameraComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_BossCameraComponent)
    END_SERIALIZATION()

    Ray_BossCameraComponent::Ray_BossCameraComponent()
        : Super()
        , m_zOffset(10)
        , m_zOffsetCurveDuration(0)
        , m_zOffsetCurveT(0)
        , m_offsetCurveDuration(0)
        , m_offsetCurveT(0)
        , m_animComponent(NULL)
    {
    }

    Ray_BossCameraComponent::~Ray_BossCameraComponent()
    {
    }

    void Ray_BossCameraComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventNodeReached,2473939155), this);

        m_attachBone.setFromStringID(getTemplate()->getAttachBone());

        m_animComponent = m_actor->GetComponent<AnimLightComponent>();

        m_zOffsetCurveT = m_zOffsetCurveDuration;
        m_offsetCurveT = m_offsetCurveDuration;
    }

    void Ray_BossCameraComponent::onResourceReady()
    {
        Super::onResourceReady();

        if ( m_animComponent )
        {
            m_attachBone.init(m_animComponent);
        }
    }

    void Ray_BossCameraComponent::onCheckpointLoaded()
    {
        Super::onCheckpointLoaded();

        m_zOffsetCurveT = m_zOffsetCurveDuration;
        m_offsetCurveT = m_offsetCurveDuration;
        m_zOffset = 10;
        m_offset = Vec2d::Zero;
    }


    void Ray_BossCameraComponent::Update( f32 _dt )
    {
        Super::Update(_dt);

        //DEBUG_DRAW_CURVE_OFS(m_zOffsetCurve, m_actor->getRef(), -2, 0, 0);
        //DEBUG_DRAW_CURVE_OFS(m_offsetCurve, m_actor->getRef(), 2, 0, 0);

        if (m_zOffsetCurveT < m_zOffsetCurveDuration)
        {
            m_zOffsetCurveT += _dt;
            m_zOffset = m_zOffsetCurve.getValue(m_zOffsetCurveT / m_zOffsetCurveDuration);
        }

        if (m_offsetCurveT < m_offsetCurveDuration)
        {
            m_offsetCurveT += _dt;
            f32 value = m_offsetCurve.getValue(m_offsetCurveT / m_offsetCurveDuration);
            m_offset = m_offsetCurvePrev + m_offsetCurveDir * value;
        }
    }

    Vec3d Ray_BossCameraComponent::getCameraPosition() const 
    {
        Vec3d pos;

        if (m_animComponent && m_attachBone.isValid())
        {
            m_animComponent->getBonePos(m_attachBone.getIndex(), pos);
        }
        else
        {
            pos = m_actor->getPos();
        }

        pos.m_x += m_offset.m_x;
        pos.m_y += m_offset.m_y;
        pos.m_z += m_zOffset;

        return pos;
    }

    void Ray_BossCameraComponent::onEvent( Event* _event )
    {
        Super::onEvent(_event);

        if ( Ray_EventNodeReached* nodeReached = _event->DynamicCast<Ray_EventNodeReached>(ITF_GET_STRINGID_CRC(Ray_EventNodeReached,2473939155)) )
        {
            processNodeReached(nodeReached);
        }
    }

    void Ray_BossCameraComponent::processNodeReached( Ray_EventNodeReached* _nodeReached )
    {
        if (_nodeReached->getCameraOn())
        {
            setAsMainCamera(btrue);
        }
        else if (_nodeReached->getCameraOff())
        {
            setAsMainCamera(bfalse);
        }

        if (_nodeReached->getCameraZOffset() != 0)
        {
            m_zOffsetCurve.m_yofs = m_zOffset;
            m_zOffsetCurve.m_yScale = _nodeReached->getCameraZOffset() - m_zOffset;
            m_zOffsetCurveDuration = _nodeReached->getCameraZOffsetDuration();
            m_zOffsetCurveT = 0;

            m_zOffsetCurve.m_yMin = -100000;
            m_zOffsetCurve.m_yMax = 100000;

            if (m_zOffsetCurveDuration == 0)
            {
                m_zOffset = _nodeReached->getCameraZOffset();
            }
        }

        if (_nodeReached->getCameraOffset() != Vec2d::Zero)
        {
            m_offsetCurvePrev = m_offset;
            m_offsetCurveDir = _nodeReached->getCameraOffset() - m_offset;
            f32 delta = m_offsetCurveDir.norm();
            m_offsetCurveDir *= 1 / delta;

            m_offsetCurve.m_yScale = delta;
            m_offsetCurveDuration = _nodeReached->getCameraOffsetDuration();
            m_offsetCurveT = 0;

            m_offsetCurve.m_yMin = -100000;
            m_offsetCurve.m_yMax = 100000;

            if (m_offsetCurveDuration == 0)
            {
                m_offset = _nodeReached->getCameraOffset();
            }
        }
    }

    void Ray_BossCameraComponent::onCameraEvent( Event* _event )
    {
        // relay the event to the actor; if we have a Ray_CameraLimiterComponent attached, it'll handle the event
        m_actor->onEvent(_event);
    }

    //-------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(Ray_BossCameraComponent_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_BossCameraComponent_Template)
        SERIALIZE_MEMBER("attachBone", m_attachBone);
    END_SERIALIZATION()

    Ray_BossCameraComponent_Template::Ray_BossCameraComponent_Template()
    {
    }

}// namespace ITF