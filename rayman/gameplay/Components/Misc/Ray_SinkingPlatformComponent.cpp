#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAYSINKINGPLATFORMCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_SinkingPlatformComponent.h"
#endif //_ITF_RAYSINKINGPLATFORMCOMPONENT_H_

#ifndef _ITF_POLYLINECOMPONENT_H_
#include "gameplay/Components/Environment/PolylineComponent.h"
#endif //_ITF_POLYLINECOMPONENT_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_TWEENCOMPONENT_H_
#include "gameplay/Components/Common/TweenComponent.h"
#endif //_ITF_TWEENCOMPONENT_H_

#ifndef _ITF_OBJECTCONTROLLERCOMPONENT_H_
#include "gameplay/Components/Misc/ObjectControllerComponent.h"
#endif //_ITF_OBJECTCONTROLLERCOMPONENT_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_
namespace ITF
{

    IMPLEMENT_OBJECT_RTTI(Ray_SinkingPlatformComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_SinkingPlatformComponent)
    END_SERIALIZATION()


    BEGIN_VALIDATE_COMPONENT(Ray_SinkingPlatformComponent)
    VALIDATE_COMPONENT_PARAM("Ray_SinkingPlatformComponent", !(m_polylineComponent&&m_objectControllerComponent), "cannot have a  polyline and an object controller");
    END_VALIDATE_COMPONENT()

    Ray_SinkingPlatformComponent::Ray_SinkingPlatformComponent()
    : m_polylineComponent(NULL)
    , m_animComponent(NULL)
    , m_tweenComponent(NULL)
    , m_objectControllerComponent(NULL)
    , m_currentWeight(0.0f)
    , m_currentSpeed(0.0f)
    , m_waterInfluenceBoneIndex(U32_INVALID)
    , m_previousWeight(0.0f)
    , m_sinkFx(U32_INVALID)
    , m_floatFx(U32_INVALID)
    {
    }

    void Ray_SinkingPlatformComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        m_users.clear();

        m_polylineComponent = m_actor->GetComponent<PolylineComponent>();
        m_animComponent= m_actor->GetComponent<AnimLightComponent>();
        m_tweenComponent = m_actor->GetComponent<TweenComponent>();
        m_objectControllerComponent = m_actor->GetComponent<ObjectControllerComponent>();
        m_fxControllerComponent = m_actor->GetComponent<FXControllerComponent>();

        if (getTemplate()->getWaterInfluenceBone().isValid())
        {
            ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventQueryWaterInfluence,1378586140), this);
        }

        if (m_tweenComponent)
        {
            m_tweenComponent->setApplyCoordinates(bfalse);
        }

        if (m_objectControllerComponent)
        {
            ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventHanging,2826410601),this);
            ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventStickOnPolyline,471231076),this);
        }

        m_actor->setObjectGroupIndex(ObjectGroup::GroupIndex_Collision);
    }

    Ray_SinkingPlatformComponent::~Ray_SinkingPlatformComponent()
    {
    }


	//----------------------------------------------------------------------------------------------------------------------------
    void Ray_SinkingPlatformComponent::Update( f32 _deltaTime )
    {
        Super::Update(_deltaTime);
        f32 weight = 0.0f;
        if (m_polylineComponent)
            weight = m_polylineComponent->getWeight();
        else
        {
            for (u32 i = 0; i < m_users.size(); ++i)
                weight += m_users[i].m_weight;
        }

        Vec3d initPos;
        if (m_tweenComponent)
            initPos = m_tweenComponent->getTargetPos();
        else
            initPos = m_actor->getBoundLocalInitialPos();

        Vec3d localPos = m_actor->getLocalPos();

        if (weight < getTemplate()->getWeightStep())
        {
            if (m_previousWeight  >= getTemplate()->getWeightStep() && m_fxControllerComponent)
            {
                m_floatFx = m_fxControllerComponent->playFX(getTemplate()->getFloatFx());
                m_fxControllerComponent->stopFXFromHandle(m_sinkFx);
                m_sinkFx = U32_INVALID;
            }
            const f32 initY = 0.0f;
            const f32 y = localPos.m_y - initPos.m_y;
            const f32 newY = y + (initY - y) * getTemplate()->getRestorationFactor();


            Vec3d pos = localPos - initPos;
            pos.m_y = newY;
            m_actor->setLocalPos(initPos + pos);
            if (f32_Abs(newY) < MTH_BIG_EPSILON)
            {
                if (m_fxControllerComponent)
                {
                    m_fxControllerComponent->stopFXFromHandle(m_floatFx);
                    m_floatFx = U32_INVALID;
                }
            }
    
        }
        else
        {
            if (m_previousWeight  < getTemplate()->getWeightStep() && m_fxControllerComponent)
            {
                m_sinkFx = m_fxControllerComponent->playFX(getTemplate()->getSinkFx());
                m_fxControllerComponent->stopFXFromHandle(m_floatFx);
                m_floatFx = U32_INVALID;
            }

            const f32 initY = 0.0f;
            const f32 y = localPos.m_y - initPos.m_y;

            const f32 limit = initY - getTemplate()->getMaxDistance();

            const f32 mult = 1.0f + fmodf(weight, getTemplate()->getStepMultiplier());
            const f32 newY = y + (limit - y) * getTemplate()->getFactor() * mult;

            Vec3d pos = localPos - initPos;
            pos.m_y = newY;
            m_actor->setLocalPos(pos + initPos);
            if (f32_Abs(newY - limit) < MTH_BIG_EPSILON)
            {
                if (m_fxControllerComponent)
                {
                    m_fxControllerComponent->stopFXFromHandle(m_sinkFx);
                    m_sinkFx = U32_INVALID;
                }
            }

        }
        m_previousWeight = weight;
   }

    void Ray_SinkingPlatformComponent::onResourceReady()
    {
        Super::onResourceReady();
        if (m_animComponent)
            m_waterInfluenceBoneIndex = m_animComponent->getBoneIndex(getTemplate()->getWaterInfluenceBone());

    }

    void Ray_SinkingPlatformComponent::onEvent( Event * _event )
    {
        Super::onEvent(_event);
        if (m_waterInfluenceBoneIndex != U32_INVALID)
        {
            if (EventQueryWaterInfluence * onQuery = _event->DynamicCast<EventQueryWaterInfluence>(ITF_GET_STRINGID_CRC(EventQueryWaterInfluence,1378586140)))
            {
                m_animComponent->getBonePos(m_waterInfluenceBoneIndex, onQuery->getPos());
            }
        }
        if (m_objectControllerComponent)
        {
            if ( EventStickOnPolyline* stickEvent = _event->DynamicCast<EventStickOnPolyline>(ITF_GET_STRINGID_CRC(EventStickOnPolyline,471231076)) )
            {
                if (stickEvent->isSticked())
                {
                    User user;
                    user.m_user = stickEvent->getActor();
                    user.m_weight = stickEvent->getWeight();
                    m_users.push_back(user);
                }
                else
                {
                    for (u32 i = 0; i < m_users.size(); ++i)
                    {
                        if (m_users[i].m_user == stickEvent->getActor())
                        {
                            m_users.eraseNoOrder(i);
                            break;
                        }
                    }
                }
            }
            else if ( EventHanging* hanging = _event->DynamicCast<EventHanging>(ITF_GET_STRINGID_CRC(EventHanging,2826410601)) )
            {
                if (hanging->isHanging())
                {
                    User user;
                    user.m_user = hanging->getActor();
                    user.m_weight = hanging->getWeight();
                    m_users.push_back(user);
                }
                else
                {
                    for (u32 i = 0; i < m_users.size(); ++i)
                    {
                        if (m_users[i].m_user == hanging->getActor())
                        {
                            m_users.eraseNoOrder(i);
                            break;
                        }
                    }
                }
            }
        }
    }


    //-------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(Ray_SinkingPlatformComponent_Template)
    BEGIN_SERIALIZATION(Ray_SinkingPlatformComponent_Template)
    SERIALIZE_MEMBER("weightStep",m_weightStep);
    SERIALIZE_MEMBER("factor",m_factor);
    SERIALIZE_MEMBER("stepMultiplier",m_stepMultiplier);
    SERIALIZE_MEMBER("maxDistance",m_maxDistance);
    SERIALIZE_MEMBER("restorationFactor",m_restorationFactor);
    SERIALIZE_MEMBER("waterInfluenceBone",m_waterInfluenceBone);
    SERIALIZE_MEMBER("sinkFx",m_sinkFx);
    SERIALIZE_MEMBER("floatFx",m_floatFx);
    END_SERIALIZATION()

    Ray_SinkingPlatformComponent_Template::Ray_SinkingPlatformComponent_Template()
    : m_weightStep( 0.94f)
    , m_factor (0.01f)
    , m_stepMultiplier(0.25f)
    , m_maxDistance(3.0f)
    , m_restorationFactor(0.02f)
    {
    }

    Ray_SinkingPlatformComponent_Template::~Ray_SinkingPlatformComponent_Template()
    {
    }

}
