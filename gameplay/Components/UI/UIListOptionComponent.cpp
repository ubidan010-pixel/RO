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
            SERIALIZE_MEMBER("leftArrowPath", m_leftArrowPath);
            SERIALIZE_MEMBER("leftArrowHighlightPath", m_leftArrowHighlightPath);
            SERIALIZE_MEMBER("rightArrowPath", m_rightArrowPath);
            SERIALIZE_MEMBER("rightArrowHighlightPath", m_rightArrowHighlightPath);
        END_CONDITION_BLOCK()
    END_SERIALIZATION()

    ///////////////////////////////////////////////////////////////////////////////////////////
    UIListOptionComponent::UIListOptionComponent()
    : Super()
    , m_valueActor(NULL)
    , m_valueColorsApplied(bfalse)
    , m_leftArrowActor(NULL)
    , m_leftArrowHighlightActor(NULL)
    , m_rightArrowActor(NULL)
    , m_rightArrowHighlightActor(NULL)
    , m_isEditing(bfalse)
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
        m_leftArrowActor = NULL;
        m_leftArrowHighlightActor = NULL;
        m_rightArrowActor = NULL;
        m_rightArrowHighlightActor = NULL;
        m_isEditing = bfalse;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIListOptionComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);
        resolveValueActor();
        resolveLeftArrow();
        resolveRightArrow();
        hideAllArrows();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIListOptionComponent::Update(f32 _deltaTime)
    {
        Super::Update(_deltaTime);

        const bbool valueColorsAppliedPreviously = m_valueColorsApplied;
        if (!m_valueColorsApplied)
            applyValueColors();

        if (!valueColorsAppliedPreviously && m_valueColorsApplied)
        {
            applyValueColor(getIsSelected());
        }
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
        setEditingMode(!m_isEditing);
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
    void UIListOptionComponent::applyValueColor(bbool isSelected)
    {
        if (!m_valueActor || !m_valueColorsApplied)
            return;

        UIComponent* valueComponent = m_valueActor->GetComponent<UIComponent>();
        if (!valueComponent || !valueComponent->m_hasColorOverride)
            return;

        if (isSelected)
        {
            valueComponent->m_overrideTextColor = valueComponent->m_overrideTextColorHighlighted;
        }
        else
        {
            const UIComponent_Template* optionTemplate = static_cast<const UIComponent_Template*>(m_template);
            if (optionTemplate)
            {
                valueComponent->m_overrideTextColor = optionTemplate->getTextColor();
            }
        }
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
    void UIListOptionComponent::resolveLeftArrow()
    {
        m_leftArrowActor = NULL;
        m_leftArrowHighlightActor = NULL;

        if (!m_leftArrowPath.isEmpty())
        {
            ObjectPath leftArrowPath;
            ITF_STDSTRING pathStr = m_leftArrowPath.cStr();
            leftArrowPath.fromString(pathStr);

            if (leftArrowPath.isValid())
            {
                Pickable* pickable = NULL;
                if (leftArrowPath.getIsAbsolute())
                {
                    pickable = SceneObjectPathUtils::getObjectFromAbsolutePath(leftArrowPath);
                }
                else
                {
                    pickable = SceneObjectPathUtils::getObjectFromRelativePath(m_actor, leftArrowPath);
                }

                if (pickable)
                {
                    m_leftArrowActor = pickable->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor, 2546623115));
                }
            }
        }

        if (!m_leftArrowHighlightPath.isEmpty())
        {
            ObjectPath leftArrowHighlightPath;
            ITF_STDSTRING pathStr = m_leftArrowHighlightPath.cStr();
            leftArrowHighlightPath.fromString(pathStr);

            if (leftArrowHighlightPath.isValid())
            {
                Pickable* pickable = NULL;
                if (leftArrowHighlightPath.getIsAbsolute())
                {
                    pickable = SceneObjectPathUtils::getObjectFromAbsolutePath(leftArrowHighlightPath);
                }
                else
                {
                    pickable = SceneObjectPathUtils::getObjectFromRelativePath(m_actor, leftArrowHighlightPath);
                }

                if (pickable)
                {
                    m_leftArrowHighlightActor = pickable->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor, 2546623115));
                }
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIListOptionComponent::resolveRightArrow()
    {
        m_rightArrowActor = NULL;
        m_rightArrowHighlightActor = NULL;

        if (!m_rightArrowPath.isEmpty())
        {
            ObjectPath rightArrowPath;
            ITF_STDSTRING pathStr = m_rightArrowPath.cStr();
            rightArrowPath.fromString(pathStr);

            if (rightArrowPath.isValid())
            {
                Pickable* pickable = NULL;
                if (rightArrowPath.getIsAbsolute())
                {
                    pickable = SceneObjectPathUtils::getObjectFromAbsolutePath(rightArrowPath);
                }
                else
                {
                    pickable = SceneObjectPathUtils::getObjectFromRelativePath(m_actor, rightArrowPath);
                }

                if (pickable)
                {
                    m_rightArrowActor = pickable->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor, 2546623115));
                }
            }
        }

        if (!m_rightArrowHighlightPath.isEmpty())
        {
            ObjectPath rightArrowHighlightPath;
            ITF_STDSTRING pathStr = m_rightArrowHighlightPath.cStr();
            rightArrowHighlightPath.fromString(pathStr);

            if (rightArrowHighlightPath.isValid())
            {
                Pickable* pickable = NULL;
                if (rightArrowHighlightPath.getIsAbsolute())
                {
                    pickable = SceneObjectPathUtils::getObjectFromAbsolutePath(rightArrowHighlightPath);
                }
                else
                {
                    pickable = SceneObjectPathUtils::getObjectFromRelativePath(m_actor, rightArrowHighlightPath);
                }

                if (pickable)
                {
                    m_rightArrowHighlightActor = pickable->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor, 2546623115));
                }
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIListOptionComponent::hideAllArrows()
    {
        if (m_leftArrowActor)
            m_leftArrowActor->disable();

        if (m_leftArrowHighlightActor)
            m_leftArrowHighlightActor->disable();

        if (m_rightArrowActor)
            m_rightArrowActor->disable();

        if (m_rightArrowHighlightActor)
            m_rightArrowHighlightActor->disable();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIListOptionComponent::showArrows()
    {
        if (m_leftArrowActor)
            m_leftArrowActor->enable();

        if (m_leftArrowHighlightActor)
            m_leftArrowHighlightActor->disable();

        if (m_rightArrowActor)
            m_rightArrowActor->enable();

        if (m_rightArrowHighlightActor)
            m_rightArrowHighlightActor->disable();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIListOptionComponent::handleSelectionChanged(bbool isSelected)
    {
        Super::handleSelectionChanged(isSelected);

        applyValueColor(isSelected);

        if (!isSelected && m_isEditing)
            m_isEditing = bfalse;

        if (m_isEditing && isSelected)
            showArrows();
        else
            hideAllArrows();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIListOptionComponent::setEditingMode(bbool editing)
    {
        if (m_isEditing == editing)
            return;

        m_isEditing = editing;

        if (m_isEditing && getIsSelected())
            showArrows();
        else
            hideAllArrows();
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

