#include "precompiled_gameplay.h"

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

#ifndef _ITF_UIMENUMANAGER_H_
#include "engine/actors/managers/UIMenuManager.h"
#endif //_ITF_UIMENUMANAGER_H_

#ifndef _ITF_UIMENU_H_
#include "gameplay/components/UI/UIMenu.h"
#endif //_ITF_UIMENU_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

namespace ITF
{
    ///////////////////////////////////////////////////////////////////////////////////////////
    IMPLEMENT_OBJECT_RTTI(UIGameOptionComponent)
    BEGIN_SERIALIZATION_CHILD(UIGameOptionComponent)
        BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
            SERIALIZE_MEMBER("labelPath", m_labelPath);
            SERIALIZE_MEMBER("backgroundPath", m_backgroundPath);
        END_CONDITION_BLOCK()
    END_SERIALIZATION()

    namespace
    {
        struct BackgroundPulseState
        {
            BackgroundPulseState()
            : m_originalScale(Vec2d::One)
            , m_originalScaleInitialized(bfalse)
            , m_timer(0.0f)
            , m_isPulsing(bfalse)
            {
            }

            Vec2d   m_originalScale;
            bbool   m_originalScaleInitialized;
            f32     m_timer;
            bbool   m_isPulsing;
        };

        ITF_MAP<ObjectRef, BackgroundPulseState> s_backgroundPulseStates;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    UIGameOptionComponent::UIGameOptionComponent()
    : Super()
    , m_labelActor(NULL)
    , m_backgroundActor(NULL)
    , m_backgroundOriginalScale(Vec2d::One)
    , m_backgroundTimer(0.0f)
    , m_backgroundScaleInitialized(bfalse)
    , m_labelColorsApplied(bfalse)
    , m_selectionInitialized(bfalse)
    , m_wasSelected(bfalse)
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
        m_backgroundActor = NULL;
        m_backgroundOriginalScale = Vec2d::One;
        m_backgroundTimer = 0.0f;
        m_backgroundScaleInitialized = bfalse;
        m_labelColorsApplied = bfalse;
        m_selectionInitialized = bfalse;
        m_wasSelected = bfalse;
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
    void UIGameOptionComponent::applyLabelColor(bbool isSelected)
    {
        if (!m_labelActor || !m_labelColorsApplied)
            return;

        UIComponent* labelComponent = m_labelActor->GetComponent<UIComponent>();
        if (!labelComponent || !labelComponent->m_hasColorOverride)
            return;

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
    Actor* UIGameOptionComponent::resolveActorFromPath(const String8& path) const
    {
        if (path.isEmpty())
        {
            return NULL;
        }

        ObjectPath objectPath;
        ITF_STDSTRING pathStr = path.cStr();
        objectPath.fromString(pathStr);

        if (!objectPath.isValid())
        {
            return NULL;
        }

        Pickable* pickable = NULL;
        if (objectPath.getIsAbsolute())
        {
            pickable = SceneObjectPathUtils::getObjectFromAbsolutePath(objectPath);
        }
        else
        {
            pickable = SceneObjectPathUtils::getObjectFromRelativePath(m_actor, objectPath);
        }

        if (!pickable)
        {
            return NULL;
        }

        return pickable->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor, 2546623115));
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    bbool UIGameOptionComponent::shouldKeepBackgroundPulseOnDeselection() const
    {
        if (!m_backgroundActor || m_backgroundPath.isEmpty())
        {
            return bfalse;
        }

        if (!UI_MENUMANAGER)
        {
            return bfalse;
        }

        UIMenu* menu = UI_MENUMANAGER->getMenu(getMenuID());
        if (!menu)
        {
            return bfalse;
        }

        const ObjectRefList uiComponentsList = menu->getUIComponentsList();
        for (u32 i = 0; i < uiComponentsList.size(); ++i)
        {
            UIComponent* uiComponent = UIMenuManager::getUIComponent(uiComponentsList[i]);
            if (!uiComponent || uiComponent == this || !uiComponent->getIsSelected())
            {
                continue;
            }

            if (!uiComponent->IsClassCRC(UIGameOptionComponent::GetClassCRCStatic()))
            {
                continue;
            }

            UIGameOptionComponent* selectedOption = static_cast<UIGameOptionComponent*>(uiComponent);
            if (!selectedOption->m_backgroundActor && !selectedOption->m_backgroundPath.isEmpty())
            {
                selectedOption->resolveBackgroundActor();
            }

            if (selectedOption->m_backgroundActor == m_backgroundActor)
            {
                return btrue;
            }
        }

        return bfalse;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIGameOptionComponent::resolveLabelActor()
    {
        m_labelActor = resolveActorFromPath(m_labelPath);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIGameOptionComponent::resolveBackgroundActor()
    {
        m_backgroundActor = resolveActorFromPath(m_backgroundPath);
        m_backgroundScaleInitialized = bfalse;

        if (m_backgroundActor)
        {
            const ObjectRef backgroundRef = m_backgroundActor->getRef();
            BackgroundPulseState& pulseState = s_backgroundPulseStates[backgroundRef];

            if (!pulseState.m_originalScaleInitialized)
            {
                pulseState.m_originalScale = m_backgroundActor->getScale();
                pulseState.m_originalScaleInitialized = btrue;
            }

            m_backgroundOriginalScale = pulseState.m_originalScale;
            m_backgroundTimer = pulseState.m_timer;
            m_backgroundScaleInitialized = btrue;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIGameOptionComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);
        resolveLabelActor();
        resolveBackgroundActor();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIGameOptionComponent::Update(f32 _deltaTime)
    {
        Super::Update(_deltaTime);

        const bbool isSelected = getIsSelected();
        const bbool labelColorsAppliedPreviously = m_labelColorsApplied;

        if (!m_labelColorsApplied)
            applyLabelColors();

        if (!labelColorsAppliedPreviously && m_labelColorsApplied)
        {
            applyLabelColor(isSelected);
        }

        if (!m_selectionInitialized || isSelected != m_wasSelected)
        {
            applyLabelColor(isSelected);
            handleSelectionChanged(isSelected);
            m_wasSelected = isSelected;
            m_selectionInitialized = btrue;
        }

        if (isSelected && m_backgroundActor && m_backgroundScaleInitialized && m_backgroundActor->getUserFriendly() == m_backgroundPath)
        {
            const UIGameOptionComponent_Template* optionTemplate = getTemplate();
            if (optionTemplate)
            {
                const ObjectRef backgroundRef = m_backgroundActor->getRef();
                BackgroundPulseState& pulseState = s_backgroundPulseStates[backgroundRef];

                if (!pulseState.m_originalScaleInitialized)
                {
                    pulseState.m_originalScale = m_backgroundOriginalScale;
                    pulseState.m_originalScaleInitialized = btrue;
                }

                const f32 curScale = 1.0f + optionTemplate->getIdleSelectedScale() *
                    f32_Sin(MTH_2PI * optionTemplate->getIdleSelectedPulseFrequency() * pulseState.m_timer);

                m_backgroundActor->setScale(pulseState.m_originalScale * curScale);

                pulseState.m_timer += _deltaTime;
                pulseState.m_isPulsing = btrue;

                m_backgroundOriginalScale = pulseState.m_originalScale;
                m_backgroundTimer = pulseState.m_timer;
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIGameOptionComponent::handleSelectionChanged(bbool isSelected)
    {
        updateBackgroundSelection(isSelected);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIGameOptionComponent::updateBackgroundSelection(bbool isSelected)
    {
        if (!m_backgroundActor && !m_backgroundPath.isEmpty())
        {
            resolveBackgroundActor();
        }

        if (!m_backgroundActor || m_backgroundPath.isEmpty())
        {
            return;
        }

        if (m_backgroundActor->getUserFriendly() != m_backgroundPath)
        {
            return;
        }

        const ObjectRef backgroundRef = m_backgroundActor->getRef();
        BackgroundPulseState& pulseState = s_backgroundPulseStates[backgroundRef];

        if (!pulseState.m_originalScaleInitialized)
        {
            pulseState.m_originalScale = m_backgroundActor->getScale();
            pulseState.m_originalScaleInitialized = btrue;
        }

        m_backgroundOriginalScale = pulseState.m_originalScale;
        m_backgroundTimer = pulseState.m_timer;
        m_backgroundScaleInitialized = btrue;

        if (isSelected)
        {
            if (!pulseState.m_isPulsing)
            {
                pulseState.m_timer = 0.0f;
            }
            m_backgroundTimer = pulseState.m_timer;
        }
        else if (shouldKeepBackgroundPulseOnDeselection())
        {
            return;
        }
        else
        {
            pulseState.m_isPulsing = bfalse;
            m_backgroundTimer = pulseState.m_timer;
        }

        m_backgroundActor->setScale(m_backgroundOriginalScale);
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

    ///////////////////////////////////////////////////////////////////////////////////////////
    IMPLEMENT_OBJECT_RTTI(UIGameOptionComponent_Template)
    BEGIN_SERIALIZATION_CHILD(UIGameOptionComponent_Template)
        SERIALIZE_MEMBER("idleSelectedScale", m_idleSelectedScale);
        SERIALIZE_MEMBER("idleSelectedPulseFrequency", m_idleSelectedPulseFrequency);
    END_SERIALIZATION()

    UIGameOptionComponent_Template::UIGameOptionComponent_Template()
    : Super()
    , m_idleSelectedScale(0.02f)
    , m_idleSelectedPulseFrequency(0.5f)
    {
    }

    UIGameOptionComponent_Template::~UIGameOptionComponent_Template()
    {
    }
}
