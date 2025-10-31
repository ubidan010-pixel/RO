#include "precompiled_gameplay.h"

#ifndef _ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_
#include "gameplay/Components/common/StickToPolylinePhysComponent.h"
#endif //_ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_

#ifndef _ITF_AISTICKBONEACTION_H_
#include "gameplay/ai/Actions/AIStickBoneAction.h"
#endif //_ITF_AISTICKBONEACTION_H_

#ifndef _ITF_ANIMATEDCOMPONENT_H_
#include "engine/actors/components/animatedcomponent.h"
#endif //_ITF_ANIMATEDCOMPONENT_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

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
    IMPLEMENT_OBJECT_RTTI(AIStickBoneAction_Template)

    BEGIN_SERIALIZATION_CHILD(AIStickBoneAction_Template)
        SERIALIZE_MEMBER("aiStickBoneName",m_stickBoneName);
        SERIALIZE_MEMBER("aiStickBoneEnd",m_stickBoneEnd);
        SERIALIZE_MEMBER("aiStickBoneTime",m_stickTime);
    END_SERIALIZATION()

    AIStickBoneAction_Template::AIStickBoneAction_Template()
    : m_stickTime(0.1f)
    , m_stickBoneEnd(bfalse)
    {

    }

    IMPLEMENT_OBJECT_RTTI(AIStickBoneAction)


    AIStickBoneAction::AIStickBoneAction()
    : m_stickBoneIndex(U32_INVALID)
    , m_physComponent(NULL)
    , m_stickCurrentTime(0.0f)
    {
    }

    AIStickBoneAction::~AIStickBoneAction()
    {
    }

    void AIStickBoneAction::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        m_physComponent = m_actor->GetComponent<PhysComponent>();
    }

    void AIStickBoneAction::onActivate()
    {
        Super::onActivate();

        m_stickCurrentTime = 0.0f;
        m_stickBoneIndex = U32_INVALID;

        Actor * objActor = AIUtils::getActor(m_stickActor);

        if ( objActor )
        {
            AnimLightComponent* animLight = objActor->GetComponent<AnimLightComponent>();

            if ( animLight )
            {
                m_stickBoneIndex = (u32)animLight->getBoneIndex(getTemplate()->getStickBoneName());
            }
        }
    }

    void AIStickBoneAction::update( f32 _delta )
    {
        if (m_stickBoneIndex == U32_INVALID)
            return;

        Actor * objActor = AIUtils::getActor(m_stickActor);

        if ( objActor )
        {
            AnimLightComponent* animLight = objActor->GetComponent<AnimLightComponent>();

            if ( animLight )
            {
                Vec3d pos;

                animLight->getBonePos(m_stickBoneIndex, pos, getTemplate()->getStickBoneEnd());

                if (m_stickCurrentTime < getTemplate()->getStickTime())
                {
                    pos = (m_actor->getPos() * (getTemplate()->getStickTime() - m_stickCurrentTime) + pos * m_stickCurrentTime)*(1.f/getTemplate()->getStickTime());
                    m_stickCurrentTime += _delta;
                }
                Vec3d direction = pos - m_actor->getPos();

                //m_actor->setScale(m_actor->getScale() * (1.0f - (m_stickCurrentTime/m_stickTime)));
                //GFX_ADAPTER->drawArrow(pos, m_actor->getPos(), 1.f, 0.f, 0.f, 1.0f);

                m_actor->setFlipped(direction.dot(Vec3d::Right) < 0.0f);
                m_actor->updateComponentPosAngle(pos,m_actor->getAngle(), m_aiComponent);

                if (m_physComponent)
                    m_physComponent->onForceMove();
            }
        }
    }  

    bbool AIStickBoneAction::isSticked()
    {
        return m_stickCurrentTime >= getTemplate()->getStickTime(); 
    }
}