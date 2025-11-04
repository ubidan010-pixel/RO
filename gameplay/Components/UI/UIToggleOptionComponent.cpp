#include "precompiled_gameplay.h"

#ifndef _ITF_UITOGGLEOPTIONCOMPONENT_H_
#include "gameplay/components/UI/UIToggleOptionComponent.h"
#endif //_ITF_UITOGGLEOPTIONCOMPONENT_H_

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
    IMPLEMENT_OBJECT_RTTI(UIToggleOptionComponent)
    BEGIN_SERIALIZATION_CHILD(UIToggleOptionComponent)
        BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
            SERIALIZE_MEMBER("labelPath", m_labelPath);
        END_CONDITION_BLOCK()
    END_SERIALIZATION()

    ///////////////////////////////////////////////////////////////////////////////////////////
    UIToggleOptionComponent::UIToggleOptionComponent()
    : Super()
    , m_labelActor(NULL)
    , m_labelColorsApplied(bfalse)
    {
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    UIToggleOptionComponent::~UIToggleOptionComponent()
    {
        clear();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIToggleOptionComponent::clear()
    {
        m_labelActor = NULL;
        m_labelColorsApplied = bfalse;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIToggleOptionComponent::applyLabelColors()
    {
        if (m_labelColorsApplied || !m_labelActor || m_labelPath.isEmpty())
            return;

        if (m_labelActor->getUserFriendly() != m_labelPath)
            return;

        UIComponent* labelComponent = m_labelActor->GetComponent<UIComponent>();
        if (!labelComponent)
            return;

        const UIComponent_Template* optionTemplate = static_cast<const UIComponent_Template*>(m_template);
        if (!optionTemplate)
            return;

        const Color& optionTextColor = optionTemplate->getTextColor();
        if (optionTextColor.getAsU32() == 0x00000000)
            return;

        labelComponent->m_hasColorOverride = btrue;
        labelComponent->m_overrideTextColor = optionTextColor;
        labelComponent->m_overrideTextColorHighlighted = optionTemplate->getTextColorHighlighted();
        labelComponent->m_overrideTextColorInactive = optionTemplate->getTextColorInactive();

        m_labelColorsApplied = btrue;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIToggleOptionComponent::resolveLabelActor()
    {
        m_labelActor = NULL;

        if (m_labelPath.isEmpty())
            return;

        ObjectPath labelPath;
        ITF_STDSTRING pathStr = m_labelPath.cStr();
        labelPath.fromString(pathStr);

        if (!labelPath.isValid())
            return;

        Pickable* pickable = NULL;
        if (labelPath.getIsAbsolute())
        {
            pickable = SceneObjectPathUtils::getObjectFromAbsolutePath(labelPath);
        }
        else
        {
            pickable = SceneObjectPathUtils::getObjectFromRelativePath(m_actor, labelPath);
        }

        if (pickable)
        {
            m_labelActor = pickable->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor, 2546623115));
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIToggleOptionComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);
        resolveLabelActor();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIToggleOptionComponent::Update(f32 _deltaTime)
    {
        Super::Update(_deltaTime);
        
        if (!m_labelColorsApplied)
            applyLabelColors();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIToggleOptionComponent::onActorClearComponents()
    {
        Super::onActorClearComponents();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIToggleOptionComponent::onBecomeActive()
    {
        Super::onBecomeActive();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIToggleOptionComponent::onBecomeInactive()
    {
        Super::onBecomeInactive();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIToggleOptionComponent::onPressed()
    {
        Super::onPressed();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIToggleOptionComponent::onReleased()
    {
        Super::onReleased();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIToggleOptionComponent::onRollover()
    {
        Super::onRollover();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIToggleOptionComponent::onRollout()
    {
        Super::onRollout();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIToggleOptionComponent::onAction(const StringID & action)
    {
        Super::onAction(action);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    IMPLEMENT_OBJECT_RTTI(UIToggleOptionComponent_Template)
    BEGIN_SERIALIZATION_CHILD(UIToggleOptionComponent_Template)
    END_SERIALIZATION()

    UIToggleOptionComponent_Template::UIToggleOptionComponent_Template()
    : Super()
    {
    }

    UIToggleOptionComponent_Template::~UIToggleOptionComponent_Template()
    {
    }
}


