#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AIREDWIZARDDANCEBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/RedWizard/Ray_AIRedWizardDanceBehavior.h"
#endif //_ITF_RAY_AIREDWIZARDDANCEBEHAVIOR_H_

#ifndef _ITF_RAY_AIGROUNDCOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_GroundAIComponent.h"
#endif //_ITF_RAY_AIGROUNDCOMPONENT_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_METRONOMEMANAGER_H_
#include "engine/sound/MetronomeManager.h"
#endif //_ITF_METRONOMEMANAGER_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_AIRedWizardDanceBehavior_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_AIRedWizardDanceBehavior_Template)
        SERIALIZE_MEMBER("syncRatio", m_syncRatio);
        SERIALIZE_MEMBER("syncOffset", m_syncOffset);
    END_SERIALIZATION()

    Ray_AIRedWizardDanceBehavior_Template::Ray_AIRedWizardDanceBehavior_Template()
    : m_syncRatio(1.f)
    , m_syncOffset(0.f)
    {

    }

    IMPLEMENT_OBJECT_RTTI(Ray_AIRedWizardDanceBehavior)

    Ray_AIRedWizardDanceBehavior::Ray_AIRedWizardDanceBehavior()
        : Super()
        , m_waitingForSync(btrue)
        , m_target(ITF_INVALID_OBJREF)
        , m_targetFlipped(bfalse)
    {
    }

    Ray_AIRedWizardDanceBehavior::~Ray_AIRedWizardDanceBehavior()
    {
    }


    void Ray_AIRedWizardDanceBehavior::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventDance,83576490), m_aiComponent);
    }

    ITF::bbool Ray_AIRedWizardDanceBehavior::isActivatedByEvent( Event* _event )
    {
        return _event->IsClassCRC(ITF_GET_STRINGID_CRC(Ray_EventDance,83576490));
    }

    void Ray_AIRedWizardDanceBehavior::onActivate()
    {
        Super::onActivate();

        m_waitingForSync = btrue;
    }

    void Ray_AIRedWizardDanceBehavior::onDeactivate()
    {
        Super::onDeactivate();

        m_target = ITF_INVALID_OBJREF;
        m_targetBone = StringID::InvalidId;
    }

    void Ray_AIRedWizardDanceBehavior::onEvent( Event* _event )
    {
        Super::onEvent(_event);

        if ( Ray_EventDance* dance = _event->DynamicCast<Ray_EventDance>(ITF_GET_STRINGID_CRC(Ray_EventDance,83576490)) )
        {
            m_target = _event->getSender();
            m_targetBone = StringID::InvalidId;
        }
    }

    void Ray_AIRedWizardDanceBehavior::update( f32 _dt )
    {
        Actor* target = AIUtils::getActor(m_target);
        ITF_ASSERT(target);
        if (!target)
        {
            m_aiComponent->onBehaviorFinished();
            return;
        }


        Vec3d targetPos = target->getPos();

        if ( m_targetBone.isValid() )
        {
            if ( AnimLightComponent* targetAnim = target->GetComponent<AnimLightComponent>() )
            {
                u32 targetBone = targetAnim->getBoneIndex(m_targetBone);
                targetAnim->getBonePos(targetBone, targetPos);
            }
        }


        f32 delta = targetPos.m_x - m_actor->getPos().m_x;
        f32 targetDistance = f32_Abs(delta);

        if ( targetDistance > 0.1f )
        {
            m_move->setWalkLeft(delta < 0);
            setAction(m_move);
        }
        else
        {
            m_actor->setFlipped(m_targetFlipped);
            setAction(m_idle);

            if (m_waitingForSync)
            {
                f32 metronome = METRONOME_MANAGER->getBarPercentage(getTemplate()->getSyncOffset(), getTemplate()->getSyncRatio(),btrue);
                if (metronome < _dt * 2)
                {
                    m_waitingForSync = bfalse;
                }
            }

            m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(WaitingToDance,3871966685), m_waitingForSync);
        }


        Super::update(_dt);
    }

}