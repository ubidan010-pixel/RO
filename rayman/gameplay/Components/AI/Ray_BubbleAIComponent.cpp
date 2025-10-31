#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_BUBBLEAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_BubbleAiComponent.h"
#endif //_ITF_RAY_BUBBLEAICOMPONENT_H_

#ifndef _ITF_PHANTOMCOMPONENT_H_
#include "gameplay/Components/Misc/PhantomComponent.h"
#endif //_ITF_PHANTOMCOMPONENT_H_

#ifndef _ITF_AIMANAGER_H_
#include "gameplay/managers/AIManager.h"
#endif //_ITF_AIMANAGER_H_

#ifndef _ITF_RAY_AIUTILS_H_
#include "Rayman/gameplay/AI/Utils/Ray_AIUtils.h"
#endif //_ITF_RAY_AIUTILS_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_RAY_AIBUBBLEDEATHBEHAVIOR_H_
#include "rayman/gameplay/AI/Behaviors/Ray_AIBubbleDeathBehavior.h"
#endif //_ITF_RAY_AIBUBBLEDEATHBEHAVIOR_H_

#ifndef _ITF_RAY_ALTRANQUILOAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_alTranquiloAiComponent.h"
#endif //_ITF_RAY_ALTRANQUILOAICOMPONENT_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_BubbleAiComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_BubbleAiComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(Ray_BubbleAiComponent)
    END_VALIDATE_COMPONENT()


    Ray_BubbleAiComponent::Ray_BubbleAiComponent()
        : Super()
        , m_scaleInput(0.0f)
        , m_fastScale(bfalse)
        , m_animatedComponent(NULL)
        , m_phantomInitialScale(1.0f)
        , m_bubbleBehavior(NULL)
    {
    }

    Ray_BubbleAiComponent::~Ray_BubbleAiComponent()
    {
    }

    void Ray_BubbleAiComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        m_animatedComponent = m_actor->GetComponent<AnimatedComponent>();
        m_phantomComponent = m_actor->GetComponent<PhantomComponent>();
        m_physComponent = m_actor->GetComponent<PhysComponent>();

        m_bubbleBehavior = createAiBehaviorAndCast<Ray_AIBubbleDeathBehavior>(getTemplate()->getBubbleBehavior());

        Super::onActorLoaded(_hotReload);

        m_phantomInitialScale = m_phantomComponent->getFactoryShapeScaled()->DynamicCast<PhysShapeCircle>(ITF_GET_STRINGID_CRC(PhysShapeCircle,3922519168))->getRadius();

        setBehavior(m_bubbleBehavior);
    }

    void Ray_BubbleAiComponent::Update( f32 _dt )
    {
        Super::Update(_dt);

        if ( Actor* owner = m_owner.getActor() )
        {
            Vec3d pos;
            u32 boneIndex = m_animatedComponent->getBoneIndex(getTemplate()->getSnapBone());
            m_animatedComponent->getBonePos(boneIndex,pos);

            Vec3d posOwner = owner->getPos();
            if ( AnimLightComponent* ownerAnim = owner->GetComponent<AnimLightComponent>() )
            {
                u32 boneIndexOwner = ownerAnim->getBoneIndex(getTemplate()->getOwnerSnapBone());
                ownerAnim->getBonePos(boneIndexOwner, posOwner);
            }

            m_actor->setPos(posOwner + (m_actor->getPos()-pos));


            const f32 targetScale = m_fastScale ? getTemplate()->getMaxScale() : getTemplate()->getMinScale();
            const f32 scaleSpeed = m_fastScale ? getTemplate()->getFastScaleSpeed() : getTemplate()->getScaleSpeed();

            m_scaleInput = Min(m_scaleInput + _dt * scaleSpeed, targetScale);
            if (m_scaleInput == targetScale)
            {
                release();
            }

            PhysShapeCircle* circle = m_phantomComponent->getShape()->DynamicCast<PhysShapeCircle>(ITF_GET_STRINGID_CRC(PhysShapeCircle,3922519168));
            circle->setRadius(m_phantomInitialScale * m_scaleInput);

            m_physComponent->setRadiusMultiplier(m_scaleInput);
        }
    }

    void Ray_BubbleAiComponent::updateAnimInput()
    {
        Super::updateAnimInput();

        static StringID s_scale("scale");
        m_animatedComponent->setInput(s_scale, m_scaleInput);
    }

    void Ray_BubbleAiComponent::release()
    {
        if ( Actor* owner = m_owner.getActor() )
        {
            if ( Ray_alTranquiloAiComponent* ownerAI = owner->GetComponent<Ray_alTranquiloAiComponent>() )
            {
                ownerAI->onBubbleReleased(m_actor->getRef());
            }
        }

        m_owner.invalidate();
        m_bubbleBehavior->release();
    }

    f32 Ray_BubbleAiComponent::getSoftCollisionRadius() const
    {
        return m_physComponent->getRadius();
    }

    void Ray_BubbleAiComponent::setLifetime( const f32 _value )
    {
        m_bubbleBehavior->setLifetime(_value);
    }

    void Ray_BubbleAiComponent::setFloatForceX( const f32 _value )
    {
        m_bubbleBehavior->setFloatForceX(_value);
    }

    //-------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(Ray_BubbleAiComponent_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_BubbleAiComponent_Template)
        SERIALIZE_MEMBER("scaleSpeed",m_scaleSpeed);
        SERIALIZE_MEMBER("fastScaleSpeed",m_fastScaleSpeed);
        SERIALIZE_MEMBER("minScale",m_minScale);
        SERIALIZE_MEMBER("maxScale",m_maxScale);
        SERIALIZE_MEMBER("snapBone",m_snapBone);
        SERIALIZE_MEMBER("ownerSnapBone",m_ownerSnapBone);
        SERIALIZE_OBJECT_WITH_FACTORY("bubbleBehavior", m_bubbleBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
    END_SERIALIZATION()

    Ray_BubbleAiComponent_Template::Ray_BubbleAiComponent_Template()
        : m_scaleSpeed(1.0f)
        , m_fastScaleSpeed(5.0f)
        , m_minScale(1.0f)
        , m_maxScale(2.0f)
        , m_bubbleBehavior(NULL)
    {
    }

    Ray_BubbleAiComponent_Template::~Ray_BubbleAiComponent_Template()
    {
        SF_DEL(m_bubbleBehavior);
    }

};
