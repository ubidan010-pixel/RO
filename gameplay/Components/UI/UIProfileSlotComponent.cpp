#include "precompiled_gameplay.h"

#ifndef _ITF_UIPROFILESLOTCOMPONENT_H_
#include "gameplay/components/UI/UIProfileSlotComponent.h"
#endif //_ITF_UIPROFILESLOTCOMPONENT_H_

namespace ITF
{
    ///////////////////////////////////////////////////////////////////////////////////////////
    IMPLEMENT_OBJECT_RTTI(UIProfileSlotComponent)
    BEGIN_SERIALIZATION_CHILD(UIProfileSlotComponent)
        BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
            SERIALIZE_MEMBER("playerIndex", m_playerIndex);
            SERIALIZE_MEMBER("resetButtonPath", m_resetButtonPath);
        END_CONDITION_BLOCK()
    END_SERIALIZATION()

    ///////////////////////////////////////////////////////////////////////////////////////////
    UIProfileSlotComponent::UIProfileSlotComponent()
    : Super()
    , m_playerIndex(0)
    , m_resetButtonPath()
    {
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    UIProfileSlotComponent::~UIProfileSlotComponent()
    {
        clear();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIProfileSlotComponent::clear()
    {
        m_playerIndex = 0;
        m_resetButtonPath = StringID();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIProfileSlotComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIProfileSlotComponent::Update(f32 _deltaTime)
    {
        Super::Update(_deltaTime);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIProfileSlotComponent::onActorClearComponents()
    {
        Super::onActorClearComponents();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIProfileSlotComponent::onBecomeActive()
    {
        Super::onBecomeActive();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIProfileSlotComponent::onBecomeInactive()
    {
        Super::onBecomeInactive();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIProfileSlotComponent::onPressed()
    {
        Super::onPressed();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIProfileSlotComponent::onReleased()
    {
        Super::onReleased();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIProfileSlotComponent::onRollover()
    {
        Super::onRollover();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIProfileSlotComponent::onRollout()
    {
        Super::onRollout();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIProfileSlotComponent::onAction(const StringID & action)
    {
        Super::onAction(action);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    IMPLEMENT_OBJECT_RTTI(UIProfileSlotComponent_Template)
    BEGIN_SERIALIZATION_CHILD(UIProfileSlotComponent_Template)
    END_SERIALIZATION()

    UIProfileSlotComponent_Template::UIProfileSlotComponent_Template()
    : Super()
    {
    }

    UIProfileSlotComponent_Template::~UIProfileSlotComponent_Template()
    {
    }
}
