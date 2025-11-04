#include "precompiled_gameplay.h"

#include "rayman/gameplay/Managers/GameOptions/Ray_GameOptionNames.h"

#ifndef _ITF_UIGAMEOPTIONCOMPONENT_H_
#include "gameplay/components/UI/UIGameOptionComponent.h"
#endif //_ITF_UIGAMEOPTIONCOMPONENT_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_OBJECTPATH_H_
#include "core/ObjectPath.h"
#endif //_ITF_OBJECTPATH_H_

#ifndef _ITF_UICOMPONENT_H_
#include "gameplay/components/UI/UIComponent.h"
#endif //_ITF_UICOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _ITF_UIMENU_H_
#include "gameplay/components/UI/UIMenu.h"
#endif //_ITF_UIMENU_H_

#ifndef _ITF_UIMENUMANAGER_H_
#include "engine/actors/managers/UIMenuManager.h"
#endif //_ITF_UIMENUMANAGER_H_

namespace ITF
{
    ///////////////////////////////////////////////////////////////////////////////////////////
    IMPLEMENT_OBJECT_RTTI(UIGameOptionComponent)
    BEGIN_SERIALIZATION_CHILD(UIGameOptionComponent)
        BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
            SERIALIZE_MEMBER("labelPath", m_labelPath);
            SERIALIZE_MEMBER("subMenuTextId", m_subMenuTextIdPath);
        END_CONDITION_BLOCK()
    END_SERIALIZATION()

    ///////////////////////////////////////////////////////////////////////////////////////////
    UIGameOptionComponent::UIGameOptionComponent()
    : Super()
    , m_labelActor(NULL)
    , m_labelColorsApplied(bfalse)
    , m_subMenuActor(NULL)
    , m_subMenuComponent(NULL)
    {
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    UIGameOptionComponent::~UIGameOptionComponent()
    {
        clear();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIGameOptionComponent::clear()
    {
        m_labelActor = NULL;
        m_labelColorsApplied = bfalse;
        m_subMenuActor = NULL;
        m_subMenuComponent = NULL;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIGameOptionComponent::applyLabelColors()
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
    void UIGameOptionComponent::updateLabelColor()
    {
        if (!m_labelActor || !m_labelColorsApplied)
            return;

        UIComponent* labelComponent = m_labelActor->GetComponent<UIComponent>();
        if (!labelComponent || !labelComponent->m_hasColorOverride)
            return;

        const bbool isSelected = getIsSelected();
        if (isSelected)
        {
            labelComponent->m_overrideTextColor = labelComponent->m_overrideTextColorHighlighted;
        }
        else
        {
            const UIComponent_Template* optionTemplate = static_cast<const UIComponent_Template*>(m_template);
            if (optionTemplate)
            {
                labelComponent->m_overrideTextColor = optionTemplate->getTextColor();
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIGameOptionComponent::resolveLabelActor()
    {
        m_labelActor = NULL;

        if (m_labelPath.isEmpty())
        {
            return;
        }

        ObjectPath labelPath;
        ITF_STDSTRING pathStr = m_labelPath.cStr();
        labelPath.fromString(pathStr);

        if (!labelPath.isValid())
        {
            return;
        }

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
    void UIGameOptionComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);
        resolveLabelActor();
        resolveSubMenuActor();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIGameOptionComponent::Update(f32 _deltaTime)
    {
        Super::Update(_deltaTime);

        if (!m_labelColorsApplied)
            applyLabelColors();

        updateLabelColor();
        updateSubMenuTextColor();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIGameOptionComponent::onActorClearComponents()
    {
        Super::onActorClearComponents();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIGameOptionComponent::onBecomeActive()
    {
        Super::onBecomeActive();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIGameOptionComponent::onBecomeInactive()
    {
        Super::onBecomeInactive();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIGameOptionComponent::onPressed()
    {
        Super::onPressed();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIGameOptionComponent::onReleased()
    {
        Super::onReleased();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIGameOptionComponent::onRollover()
    {
        Super::onRollover();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIGameOptionComponent::onRollout()
    {
        Super::onRollout();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIGameOptionComponent::onAction(const StringID & action)
    {
        Super::onAction(action);
    }

    void UIGameOptionComponent::resolveSubMenuActor()
    {
        m_subMenuActor = NULL;
        m_subMenuComponent = NULL;

        if (m_subMenuTextIdPath.isEmpty())
            return;

        ObjectPath subMenuPath;
        ITF_STDSTRING pathStr = m_subMenuTextIdPath.cStr();
        subMenuPath.fromString(pathStr);

        if (!subMenuPath.isValid())
            return;

        Pickable* pickable = NULL;
        if (subMenuPath.getIsAbsolute())
        {
            pickable = SceneObjectPathUtils::getObjectFromAbsolutePath(subMenuPath);
        }
        else
        {
            pickable = SceneObjectPathUtils::getObjectFromRelativePath(m_actor, subMenuPath);
        }

        if (pickable)
        {
            m_subMenuActor = pickable->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor, 2546623115));
            if (m_subMenuActor)
            {
                m_subMenuComponent = m_subMenuActor->GetComponent<UIComponent>();
            }
        }
    }

    void UIGameOptionComponent::updateSubMenuTextColor()
    {
        if (!m_subMenuComponent || !UI_MENUMANAGER)
            return;

        UIMenu* menu = UI_MENUMANAGER->getMenu(OPTION_MENU_NAME);
        if (!menu)
            return;

        const bbool isSelected = (menu->getUIComponentSelected() == this);
        static const Color whiteColor = Color::white();
        static const Color yellowColor = Color(0xffffc47c);

        m_subMenuComponent->m_hasColorOverride = btrue;
        m_subMenuComponent->m_overrideTextColor = isSelected ? whiteColor : yellowColor;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    IMPLEMENT_OBJECT_RTTI(UIGameOptionComponent_Template)
    BEGIN_SERIALIZATION_CHILD(UIGameOptionComponent_Template)
    END_SERIALIZATION()

    UIGameOptionComponent_Template::UIGameOptionComponent_Template()
    : Super()
    {
    }

    UIGameOptionComponent_Template::~UIGameOptionComponent_Template()
    {
    }
}

