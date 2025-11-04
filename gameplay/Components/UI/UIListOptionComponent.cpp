#include "precompiled_gameplay.h"

#ifndef _ITF_UILISTOPTIONCOMPONENT_H_
#include "gameplay/components/UI/UIListOptionComponent.h"
#endif //_ITF_UILISTOPTIONCOMPONENT_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_OBJECTPATH_H_
#include "core/ObjectPath.h"
#endif //_ITF_OBJECTPATH_H_

#ifndef _ITF_UICOMPONENT_H_
#include "gameplay/components/UI/UIComponent.h"
#endif //_ITF_UICOMPONENT_H_

namespace ITF
{
    ///////////////////////////////////////////////////////////////////////////////////////////
    IMPLEMENT_OBJECT_RTTI(UIListOptionComponent)
    BEGIN_SERIALIZATION_CHILD(UIListOptionComponent)
        BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
            SERIALIZE_MEMBER("valuePath", m_valuePath);
        END_CONDITION_BLOCK()
    END_SERIALIZATION()

    ///////////////////////////////////////////////////////////////////////////////////////////
    UIListOptionComponent::UIListOptionComponent()
    : Super()
    , m_valueActor(NULL)
    , m_valueColorsApplied(bfalse)
    {
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    UIListOptionComponent::~UIListOptionComponent()
    {
        clear();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIListOptionComponent::clear()
    {
        m_valueActor = NULL;
        m_valueColorsApplied = bfalse;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIListOptionComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);
        resolveValueActor();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIListOptionComponent::Update(f32 _deltaTime)
    {
        Super::Update(_deltaTime);
        
        if (!m_valueColorsApplied)
            applyValueColors();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIListOptionComponent::onActorClearComponents()
    {
        Super::onActorClearComponents();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIListOptionComponent::onBecomeActive()
    {
        Super::onBecomeActive();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIListOptionComponent::onBecomeInactive()
    {
        Super::onBecomeInactive();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIListOptionComponent::onPressed()
    {
        Super::onPressed();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIListOptionComponent::onReleased()
    {
        Super::onReleased();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIListOptionComponent::onRollover()
    {
        Super::onRollover();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIListOptionComponent::onRollout()
    {
        Super::onRollout();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIListOptionComponent::onAction(const StringID & action)
    {
        Super::onAction(action);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIListOptionComponent::applyValueColors()
    {
        if (m_valueColorsApplied || !m_valueActor || m_valuePath.isEmpty())
            return;

        if (m_valueActor->getUserFriendly() != m_valuePath)
            return;

        UIComponent* valueComponent = m_valueActor->GetComponent<UIComponent>();
        if (!valueComponent)
            return;

        const UIComponent_Template* optionTemplate = static_cast<const UIComponent_Template*>(m_template);
        if (!optionTemplate)
            return;

        const Color& optionTextColor = optionTemplate->getTextColor();
        if (optionTextColor.getAsU32() == 0x00000000)
            return;

        valueComponent->m_hasColorOverride = btrue;
        valueComponent->m_overrideTextColor = optionTextColor;
        valueComponent->m_overrideTextColorHighlighted = optionTemplate->getTextColorHighlighted();
        valueComponent->m_overrideTextColorInactive = optionTemplate->getTextColorInactive();

        m_valueColorsApplied = btrue;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIListOptionComponent::resolveValueActor()
    {
        m_valueActor = NULL;

        if (m_valuePath.isEmpty())
            return;

        ObjectPath valuePath;
        ITF_STDSTRING pathStr = m_valuePath.cStr();
        valuePath.fromString(pathStr);

        if (!valuePath.isValid())
            return;

        Pickable* pickable = NULL;
        if (valuePath.getIsAbsolute())
        {
            pickable = SceneObjectPathUtils::getObjectFromAbsolutePath(valuePath);
        }
        else
        {
            pickable = SceneObjectPathUtils::getObjectFromRelativePath(m_actor, valuePath);
        }

        if (pickable)
        {
            m_valueActor = pickable->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor, 2546623115));
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    IMPLEMENT_OBJECT_RTTI(UIListOptionComponent_Template)
    BEGIN_SERIALIZATION_CHILD(UIListOptionComponent_Template)
    END_SERIALIZATION()

    UIListOptionComponent_Template::UIListOptionComponent_Template()
    : Super()
    {
    }

    UIListOptionComponent_Template::~UIListOptionComponent_Template()
    {
    }
}

