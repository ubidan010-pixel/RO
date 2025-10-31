#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AIREDWIZARDPEDESTALBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/RedWizard/Ray_AIRedWizardPedestalBehavior.h"
#endif //_ITF_RAY_AIREDWIZARDPEDESTALBEHAVIOR_H_

#ifndef _ITF_RAY_AIGROUNDCOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_GroundAIComponent.h"
#endif //_ITF_RAY_AIGROUNDCOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_PHYSBODY_H_
#include "engine/physics/PhysBody.h"
#endif //_ITF_PHYSBODY_H_

#ifndef _ITF_GAMEMATERIAL_H_
#include "engine/gameplay/GameMaterial.h"
#endif //_ITF_GAMEMATERIAL_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_AIRedWizardPedestalBehavior_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_AIRedWizardPedestalBehavior_Template)
        SERIALIZE_OBJECT_WITH_FACTORY("idle", m_idle, ACTORSMANAGER->getAIActionsFactory());
        SERIALIZE_MEMBER("supportBone", m_supportBone);
        SERIALIZE_MEMBER("gameMaterial", m_gameMaterial);
    END_SERIALIZATION()

    Ray_AIRedWizardPedestalBehavior_Template::Ray_AIRedWizardPedestalBehavior_Template()
        : m_idle(NULL)
    {
    }

    Ray_AIRedWizardPedestalBehavior_Template::~Ray_AIRedWizardPedestalBehavior_Template()
    {
        SF_DEL(m_idle);
    }

    IMPLEMENT_OBJECT_RTTI(Ray_AIRedWizardPedestalBehavior)

    BEGIN_VALIDATE_BEHAVIOR(Ray_AIRedWizardPedestalBehavior)
        VALIDATE_BEHAVIOR_PARAM("supportBone", getTemplate()->getSupportBone().isValid(), "please set the support bone");
        VALIDATE_BEHAVIOR_PARAM("supportBone", m_supportBoneIndex != U32_INVALID, "invalid bone '%s'", getTemplate()->getSupportBone().getDebugString());
    END_VALIDATE_BEHAVIOR()

    Ray_AIRedWizardPedestalBehavior::Ray_AIRedWizardPedestalBehavior()
        : Super()
        , m_idle(NULL)
        , m_supportBoneIndex(U32_INVALID)
        , m_playerDetectRadius(0.f)
    {
    }

    Ray_AIRedWizardPedestalBehavior::~Ray_AIRedWizardPedestalBehavior()
    {
    }


    void Ray_AIRedWizardPedestalBehavior::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        m_idle = createAiAction(getTemplate()->getIdle());

        Super::onActorLoaded(_hotReload);

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventStickOnPolyline,471231076), m_aiComponent);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventHanging,2826410601), m_aiComponent);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventInteractionQuery,209600608), m_aiComponent);

        m_polyline.init(m_actor,m_aiComponent);
        if ( getTemplate()->getGameMaterial().getStringID() != GameMaterial_Template::nullMatName.getStringID() )
        {
            World::addGameMaterialFromFile(getTemplate()->getGameMaterial(), m_actor->getResourceGroup());
            m_polyline.setGameMaterial(getTemplate()->getGameMaterial());
        }
        m_polylinePoints.resize(2);
    }

    void Ray_AIRedWizardPedestalBehavior::onResourceReady()
    {
        Super::onResourceReady();

        if (m_animComponent)
        {
            m_supportBoneIndex = m_animComponent->getBoneIndex(getTemplate()->getSupportBone());
        }
    }


    bbool Ray_AIRedWizardPedestalBehavior::isActivatedByEvent( Event* _event )
    {
        Ray_EventAIOrder* order = _event->DynamicCast<Ray_EventAIOrder>(ITF_GET_STRINGID_CRC(Ray_EventAIOrder,1055366852));
        return order && order->getId() == ITF_GET_STRINGID_CRC(Pedestal,1212106600);
    }

    void Ray_AIRedWizardPedestalBehavior::setData( Event* _event )
    {
        if ( Ray_EventAIOrder* order = _event->DynamicCast<Ray_EventAIOrder>(ITF_GET_STRINGID_CRC(Ray_EventAIOrder,1055366852)) )
        {
            m_target = _event->getSender();
            m_playerDetectRadius = order->getPlayerDetectRadius();
        }
        else
        {
            ITF_ASSERT_MSG(0, "wrong order type!");
        }
    }

    void Ray_AIRedWizardPedestalBehavior::onActivate()
    {
        //Super::onActivate();

        m_polyline.activate();

        ITF_ASSERT(m_target.isValid());

        setAction(m_idle);
    }

    void Ray_AIRedWizardPedestalBehavior::onDeactivate()
    {
        Super::onDeactivate();

        m_polyline.deactivate();

        m_target.invalidate();
        m_playerDetectRadius = 0.f;
    }

    void Ray_AIRedWizardPedestalBehavior::onEvent( Event* _event )
    {
        Super::onEvent(_event);

        m_polyline.onEvent(_event);

        if (EventInteractionQuery* query = _event->DynamicCast<EventInteractionQuery>(ITF_GET_STRINGID_CRC(EventInteractionQuery,209600608)))
        {
            query->setInteraction(CharacterInteractionType_None);
        }
    }

    void Ray_AIRedWizardPedestalBehavior::update( f32 _dt )
    {
        Actor* target = AIUtils::getActor(m_target);
        ITF_ASSERT(target);
        if (!target)
        {
            m_aiComponent->onBehaviorFinished();
            return;
        }


        ActorList players;
        AIUtils::getPlayers(m_actor->getDepth(), players);
        if ( !AIUtils::hasActorInRange(players, target, bfalse, m_playerDetectRadius) )
        {
            // no more players in the target zone
            m_aiComponent->onBehaviorFinished();
            return;
        }

        if (m_currentAction == m_idle)
        {
            updatePolyline(_dt);
        }


        // update actions last
        Super::update(_dt);
    }

    void Ray_AIRedWizardPedestalBehavior::updateAnimInputs()
    {
        Super::updateAnimInputs();

        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(IsSupporting,987492597), m_polyline.getObjectsOnPolyline().size() > 0);
    }

    void Ray_AIRedWizardPedestalBehavior::updatePolyline( f32 _dt )
    {
        Vec3d left, right;
        m_animComponent->getBonePos(m_supportBoneIndex, left);
        m_animComponent->getBonePos(m_supportBoneIndex, right, btrue);

        if (m_actor->isFlipped())
        {
            m_polylinePoints[0] = right.truncateTo2D();
            m_polylinePoints[1] = left.truncateTo2D();
        }
        else
        {
            m_polylinePoints[0] = left.truncateTo2D();
            m_polylinePoints[1] = right.truncateTo2D();
        }

        m_polyline.setPoints(m_polylinePoints);
        m_polyline.update(_dt);
    }

#ifdef ITF_SUPPORT_EDITOR
    void Ray_AIRedWizardPedestalBehavior::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        Super::drawEdit(_drawInterface, _flags);

        if (_flags != ActorComponent::DrawEditFlag_All)
            return;

        m_polyline.drawEdit(_drawInterface, _flags);
    }
#endif // ITF_SUPPORT_EDITOR
}