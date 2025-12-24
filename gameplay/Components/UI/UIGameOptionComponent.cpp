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
            SERIALIZE_MEMBER("selectedBackgroundPath", m_selectedBackgroundPath);
        END_CONDITION_BLOCK()
    END_SERIALIZATION()

    namespace
    {
        static const f32 s_gameOptionEditScale = 1.1f;
        static const f32 s_backgroundSelectedScale = 1.1f;

        struct BackgroundScaleState
        {
            BackgroundScaleState()
            : m_originalScale(Vec2d::One)
            , m_originalScaleInitialized(bfalse)
            , m_selectedCount(0)
            {
            }

            Vec2d   m_originalScale;
            bbool   m_originalScaleInitialized;
            u32     m_selectedCount;
        };

        ITF_MAP<ObjectRef, BackgroundScaleState> s_backgroundScaleStates;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    UIGameOptionComponent::UIGameOptionComponent()
    : Super()
    , m_labelActor(NULL)
    , m_backgroundActor(NULL)
    , m_backgroundOriginalScale(Vec2d::One)
    , m_backgroundScaleInitialized(bfalse)
    , m_selectedBackgroundActor(NULL)
    , m_selectedBackgroundVisible(bfalse)
    , m_labelColorsApplied(bfalse)
    , m_selectionInitialized(bfalse)
    , m_wasSelected(bfalse)
    , m_isInEditingMode(bfalse)
    , m_backgroundHighlighted(bfalse)
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
        if (m_backgroundHighlighted)
        {
            updateBackgroundSelection(bfalse);
        }

        if (m_selectedBackgroundActor && m_selectedBackgroundVisible)
        {
            m_selectedBackgroundActor->disable();
        }

        m_labelActor = NULL;
        m_backgroundActor = NULL;
        m_backgroundOriginalScale = Vec2d::One;
        m_backgroundScaleInitialized = bfalse;
        m_selectedBackgroundActor = NULL;
        m_selectedBackgroundVisible = bfalse;
        m_labelColorsApplied = bfalse;
        m_selectionInitialized = bfalse;
        m_wasSelected = bfalse;
        m_isInEditingMode = bfalse;
        m_backgroundHighlighted = bfalse;
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
        if (!m_labelActor)
            return;

        UIComponent* labelComponent = m_labelActor->GetComponent<UIComponent>();
        if (!labelComponent)
            return;

        setGameOptionEditScale(labelComponent, isSelected && m_isInEditingMode);

        if (!m_labelColorsApplied || !labelComponent->m_hasColorOverride)
            return;

        if (!isSelected)
        {
            const UIComponent_Template* optionTemplate = static_cast<const UIComponent_Template*>(m_template);
            if (optionTemplate)
            {
                labelComponent->m_overrideTextColor = optionTemplate->getTextColor();
            }
            return;
        }
        labelComponent->m_overrideTextColor = labelComponent->m_overrideTextColorHighlighted;
        // labelComponent->m_overrideTextColor = m_isInEditingMode
        //     ? labelComponent->m_overrideTextColorInactive
        //     : labelComponent->m_overrideTextColorHighlighted;
    }

    void UIGameOptionComponent::setGameOptionEditScale(UIComponent* component, bbool editing) const
    {
        if (!component)
            return;

        component->m_gameOptionEditScale = editing ? s_gameOptionEditScale : 1.0f;
    }

    void UIGameOptionComponent::setEditingMode(bbool editing)
    {
        if (m_isInEditingMode == editing)
            return;

        m_isInEditingMode = editing;
        applyLabelColor(getIsSelected());
        updateSelectedBackgroundVisibility(getIsSelected());
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
            BackgroundScaleState& scaleState = s_backgroundScaleStates[backgroundRef];

            if (!scaleState.m_originalScaleInitialized)
            {
                scaleState.m_originalScale = m_backgroundActor->getScale();
                scaleState.m_originalScaleInitialized = btrue;
            }

            m_backgroundOriginalScale = scaleState.m_originalScale;
            m_backgroundScaleInitialized = btrue;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIGameOptionComponent::resolveSelectedBackgroundActor()
    {
        m_selectedBackgroundActor = resolveActorFromPath(m_selectedBackgroundPath);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIGameOptionComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);
        resolveLabelActor();
        resolveBackgroundActor();
        resolveSelectedBackgroundActor();
        updateSelectedBackgroundVisibility(getIsSelected());
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
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIGameOptionComponent::handleSelectionChanged(bbool isSelected)
    {
        updateBackgroundSelection(isSelected);
        updateSelectedBackgroundVisibility(isSelected);
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
        BackgroundScaleState& scaleState = s_backgroundScaleStates[backgroundRef];

        if (!scaleState.m_originalScaleInitialized)
        {
            scaleState.m_originalScale = m_backgroundActor->getScale();
            scaleState.m_originalScaleInitialized = btrue;
        }

        m_backgroundOriginalScale = scaleState.m_originalScale;
        m_backgroundScaleInitialized = btrue;

        if (isSelected)
        {
            if (!m_backgroundHighlighted)
            {
                ++scaleState.m_selectedCount;
                m_backgroundHighlighted = btrue;
            }
        }
        else
        {
            if (m_backgroundHighlighted)
            {
                if (scaleState.m_selectedCount > 0)
                {
                    --scaleState.m_selectedCount;
                }
                m_backgroundHighlighted = bfalse;
            }
        }

        if (scaleState.m_selectedCount > 0)
        {
            m_backgroundActor->setScale(m_backgroundOriginalScale * s_backgroundSelectedScale);
        }
        else
        {
            m_backgroundActor->setScale(m_backgroundOriginalScale);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIGameOptionComponent::updateSelectedBackgroundVisibility(bbool isSelected)
    {
        const bbool shouldBeVisible = isSelected && m_isInEditingMode;

        if (!m_selectedBackgroundActor && !m_selectedBackgroundPath.isEmpty())
        {
            resolveSelectedBackgroundActor();
        }

        if (!m_selectedBackgroundActor || m_selectedBackgroundPath.isEmpty())
        {
            return;
        }

        if (m_selectedBackgroundActor->getUserFriendly() != m_selectedBackgroundPath)
        {
            return;
        }

        if (shouldBeVisible)
        {
            m_selectedBackgroundActor->enable();
            m_selectedBackgroundVisible = btrue;
        }
        else
        {
            m_selectedBackgroundActor->disable();
            m_selectedBackgroundVisible = bfalse;
        }
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
