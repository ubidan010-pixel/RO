#include "precompiled_gameplay.h"

#ifndef _ITF_PLAYANIMBASEDONBONEANGLECOMPONENT_H_
#include "gameplay/Components/Misc/PlayAnimBasedOnBoneAngleComponent.h"
#endif //_ITF_PLAYANIMBASEDONBONEANGLECOMPONENT_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(PlayAnimBasedOnBoneAngleComponent)

    BEGIN_SERIALIZATION_CHILD(PlayAnimBasedOnBoneAngleComponent)
    END_SERIALIZATION()

    PlayAnimBasedOnBoneAngleComponent::PlayAnimBasedOnBoneAngleComponent()
        : Super()
    {
    }

    PlayAnimBasedOnBoneAngleComponent::~PlayAnimBasedOnBoneAngleComponent()
    {
    }

    void PlayAnimBasedOnBoneAngleComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventAnimUpdated,2246315305),this);

        m_animComponent = m_actor->GetComponent<AnimLightComponent>();
        ITF_ASSERT(m_animComponent);
    }

    void PlayAnimBasedOnBoneAngleComponent::onEvent( Event* _event )
    {
        Super::onEvent(_event);

        if ( EventAnimUpdated* animUpdated = _event->DynamicCast<EventAnimUpdated>(ITF_GET_STRINGID_CRC(EventAnimUpdated,2246315305)) )
        {
            processAnimUpdated(animUpdated);
        }
    }

    void PlayAnimBasedOnBoneAngleComponent::processAnimUpdated( EventAnimUpdated* _animUpdated )
    {
        if ( !m_animComponent || !m_animComponent->isLoaded() )
        {
            return;
        }

        u32 boneIndex = m_animComponent->getBoneIndex(getTemplate()->getBoneName());
        ITF_ASSERT_MSG(boneIndex != U32_INVALID, "invalid bone!");
        if ( boneIndex == U32_INVALID )
        {
            return;
        }

        f32 min = getTemplate()->getMinAngle().ToRadians();
        f32 max = getTemplate()->getMaxAngle().ToRadians();
        f32 boneAngle = m_animComponent->getBone(boneIndex)->m_AngleLocal;

        f32 delta = getShortestAngleDelta(min, max);
        f32 relativeAngle = getShortestAngleDelta(min, boneAngle);
        if (delta > 0)
        {
            relativeAngle = Clamp(relativeAngle, 0.f, delta);
        }
        else
        {
            relativeAngle = Clamp(relativeAngle, delta, 0.f);
        }

        f32 cursor = relativeAngle / delta;
        m_animComponent->setProceduralCursor(cursor);

        //String msg;msg.setTextFormat("angle: %.f - %.f%%", boneAngle * MTH_RADTODEG, cursor * 100);
        //GFX_ADAPTER->drawDBGText(msg);
    }

    //-------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(PlayAnimBasedOnBoneAngleComponent_Template)
    BEGIN_SERIALIZATION(PlayAnimBasedOnBoneAngleComponent_Template)
        SERIALIZE_MEMBER("bone", m_boneName);
        SERIALIZE_MEMBER("minAngle", m_minAngle);
        SERIALIZE_MEMBER("maxAngle", m_maxAngle);
    END_SERIALIZATION()

    PlayAnimBasedOnBoneAngleComponent_Template::PlayAnimBasedOnBoneAngleComponent_Template()
    : m_minAngle(bfalse, 0)
    , m_maxAngle(bfalse, MTH_PIBY2)
    {
    }
}