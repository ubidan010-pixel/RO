#include "precompiled_gameplay.h"

#ifndef _ITF_UIFLOATOPTIONCOMPONENT_H_
#include "gameplay/components/UI/UIFloatOptionComponent.h"
#endif //_ITF_UIFLOATOPTIONCOMPONENT_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_OBJECTPATH_H_
#include "core/ObjectPath.h"
#endif //_ITF_OBJECTPATH_H_

#ifndef _ITF_UICOMPONENT_H_
#include "gameplay/components/UI/UIComponent.h"
#endif //_ITF_UICOMPONENT_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_INPUT_ADAPTER_H_
#include "engine/AdaptersInterfaces/InputAdapter.h"
#endif //_ITF_INPUT_ADAPTER_H_

#ifndef _ITF_UIMENU_H_
#include "gameplay/components/UI/UIMenu.h"
#endif //_ITF_UIMENU_H_

namespace ITF
{
    ///////////////////////////////////////////////////////////////////////////////////////////
    IMPLEMENT_OBJECT_RTTI(UIFloatOptionComponent)
    BEGIN_SERIALIZATION_CHILD(UIFloatOptionComponent)
        BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
            SERIALIZE_MEMBER("sliderBackgroundStartPath", m_sliderBackgroundStartPath);
            SERIALIZE_MEMBER("sliderBackgroundEndPath", m_sliderBackgroundEndPath);
            SERIALIZE_MEMBER("sliderCursorPath", m_sliderCursorPath);
            SERIALIZE_MEMBER("sliderBackgroundSelectedPath", m_sliderBackgroundSelectedPath);
            SERIALIZE_MEMBER("sliderCursorSelectedPath", m_sliderCursorSelectedPath);
        END_CONDITION_BLOCK()
    END_SERIALIZATION()

    ///////////////////////////////////////////////////////////////////////////////////////////
    UIFloatOptionComponent::UIFloatOptionComponent()
    : Super()
    , m_sliderBackgroundStartActor(NULL)
    , m_sliderBackgroundEndActor(NULL)
    , m_sliderCursorActor(NULL)
    , m_sliderBackgroundSelectedActor(NULL)
    , m_sliderCursorSelectedActor(NULL)
    , m_cursorGraphComponent(NULL)
    , m_isSliding(bfalse)
    , m_value(0.5f)
    {
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    UIFloatOptionComponent::~UIFloatOptionComponent()
    {
        clear();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIFloatOptionComponent::clear()
    {
        m_sliderBackgroundStartActor = NULL;
        m_sliderBackgroundEndActor = NULL;
        m_sliderCursorActor = NULL;
        m_sliderBackgroundSelectedActor = NULL;
        m_sliderCursorSelectedActor = NULL;
        m_cursorGraphComponent = NULL;
        m_isSliding = bfalse;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIFloatOptionComponent::resolveSliderActors()
    {
        m_sliderBackgroundStartActor = NULL;
        m_sliderBackgroundEndActor = NULL;
        m_sliderCursorActor = NULL;
        m_sliderBackgroundSelectedActor = NULL;
        m_sliderCursorSelectedActor = NULL;

        auto resolveActor = [this](const String8& path, Actor*& outActor) -> void
        {
            if (path.isEmpty())
                return;

            ObjectPath objectPath;
            ITF_STDSTRING pathStr = path.cStr();
            objectPath.fromString(pathStr);

            if (!objectPath.isValid())
                return;

            Pickable* pickable = NULL;
            if (objectPath.getIsAbsolute())
            {
                pickable = SceneObjectPathUtils::getObjectFromAbsolutePath(objectPath);
            }
            else
            {
                pickable = SceneObjectPathUtils::getObjectFromRelativePath(m_actor, objectPath);
            }

            if (pickable)
            {
                outActor = pickable->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor, 2546623115));
            }
        };

        resolveActor(m_sliderBackgroundStartPath, m_sliderBackgroundStartActor);
        resolveActor(m_sliderBackgroundEndPath, m_sliderBackgroundEndActor);
        resolveActor(m_sliderCursorPath, m_sliderCursorActor);
        resolveActor(m_sliderBackgroundSelectedPath, m_sliderBackgroundSelectedActor);
        resolveActor(m_sliderCursorSelectedPath, m_sliderCursorSelectedActor);

        if (m_sliderCursorActor)
        {
            m_cursorGraphComponent = m_sliderCursorActor->GetComponent<TextureGraphicComponent2D>();
            if (m_cursorGraphComponent)
            {
                m_originalCursorScale = m_sliderCursorActor->getScale();
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIFloatOptionComponent::updateSliderVisuals()
    {
        if (!m_sliderBackgroundStartActor || !m_sliderBackgroundEndActor || !m_sliderCursorActor)
            return;

        UIComponent* startComponent = m_sliderBackgroundStartActor->GetComponent<UIComponent>();
        UIComponent* endComponent = m_sliderBackgroundEndActor->GetComponent<UIComponent>();
        UIComponent* cursorComponent = m_sliderCursorActor->GetComponent<UIComponent>();

        if (!startComponent || !endComponent || !cursorComponent)
            return;

        f32 startPos = startComponent->getRelativePosX();
        f32 endPos = endComponent->getRelativePosX();
        f32 width = endPos - startPos;

        if (width <= 0.0f)
            return;

        f32 cursorRelativePos = startPos + width * m_value;
        f32 cursorRelativePosY = cursorComponent->getRelativePosY();

        Vec2d cursorRelativePosVec(cursorRelativePos, cursorRelativePosY);
        cursorComponent->setRelativePos(cursorRelativePosVec);
        m_relativePosX = cursorRelativePos;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIFloatOptionComponent::updateSliderFromMouse()
    {
        if (!m_isSliding || !m_sliderBackgroundStartActor || !m_sliderBackgroundEndActor)
            return;

        UIComponent* startComponent = m_sliderBackgroundStartActor->GetComponent<UIComponent>();
        UIComponent* endComponent = m_sliderBackgroundEndActor->GetComponent<UIComponent>();

        if (!startComponent || !endComponent)
            return;

        i32 x, y;
        INPUT_ADAPTER->getMousePos(x, y);
        f32 screenWidth = (f32)GFX_ADAPTER->getScreenWidth();

        if (screenWidth <= 0.0f)
            return;

        f32 relativePos = x / screenWidth;
        f32 startPos = startComponent->getRelativePosX();
        f32 endPos = endComponent->getRelativePosX();

        if (relativePos >= startPos && relativePos <= endPos)
        {
            f32 width = endPos - startPos;
            if (width > 0.0f)
            {
                f32 newValue = (relativePos - startPos) / width;
                setValue(newValue, bfalse);
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIFloatOptionComponent::switchToNormalActors()
    {
        if (m_sliderBackgroundSelectedActor)
            m_sliderBackgroundSelectedActor->disable();

        if (m_sliderCursorSelectedActor)
            m_sliderCursorSelectedActor->disable();

        if (m_sliderCursorActor)
            m_sliderCursorActor->enable();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIFloatOptionComponent::switchToSelectedActors()
    {
        if (m_sliderCursorActor)
            m_sliderCursorActor->disable();

        if (m_sliderBackgroundSelectedActor)
            m_sliderBackgroundSelectedActor->enable();

        if (m_sliderCursorSelectedActor)
            m_sliderCursorSelectedActor->enable();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIFloatOptionComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);
        resolveSliderActors();
        switchToNormalActors();
        updateSliderVisuals();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIFloatOptionComponent::Update(f32 _deltaTime)
    {
        Super::Update(_deltaTime);
        updateSliderFromMouse();
        updateSliderVisuals();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIFloatOptionComponent::onActorClearComponents()
    {
        Super::onActorClearComponents();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIFloatOptionComponent::onBecomeActive()
    {
        Super::onBecomeActive();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIFloatOptionComponent::onBecomeInactive()
    {
        Super::onBecomeInactive();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIFloatOptionComponent::onPressed()
    {
        Super::onPressed();
        m_isSliding = btrue;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIFloatOptionComponent::onReleased()
    {
        Super::onReleased();
        m_isSliding = bfalse;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIFloatOptionComponent::onRollover()
    {
        Super::onRollover();
        switchToSelectedActors();

        if (m_sliderCursorActor && m_cursorGraphComponent)
        {
            m_sliderCursorActor->setScale(m_originalCursorScale * getTemplate()->getScaleOnSelected());
            m_cursorGraphComponent->setDrawColor(getTemplate()->getColorOnSelected().getAsU32());
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIFloatOptionComponent::onRollout()
    {
        Super::onRollout();
        switchToNormalActors();

        if (m_sliderCursorActor && m_cursorGraphComponent)
        {
            m_sliderCursorActor->setScale(m_originalCursorScale);
            m_cursorGraphComponent->setDrawColor(COLOR_WHITE);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIFloatOptionComponent::onAction(const StringID & action)
    {
        Super::onAction(action);

        if (!m_isSliding)
            return;

        if (action == input_actionID_RightHold)
        {
            setValue(m_value + getTemplate()->getSpeedCursor());
        }
        else if (action == input_actionID_LeftHold)
        {
            setValue(m_value - getTemplate()->getSpeedCursor());
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIFloatOptionComponent::setValue(f32 val, bbool updatePosition)
    {
        val = Clamp(val, 0.0f, 1.0f);
        if (val == m_value)
            return;

        m_value = val;

        if (updatePosition)
        {
            updateSliderVisuals();
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    IMPLEMENT_OBJECT_RTTI(UIFloatOptionComponent_Template)
    BEGIN_SERIALIZATION_CHILD(UIFloatOptionComponent_Template)
        SERIALIZE_MEMBER("speedCursor", m_speedCursor);
        SERIALIZE_MEMBER("scaleOnSelected", m_scaleOnSelected);
        SERIALIZE_MEMBER("colorOnSelected", m_colorOnSelected);
    END_SERIALIZATION()

    UIFloatOptionComponent_Template::UIFloatOptionComponent_Template()
    : Super()
    , m_speedCursor(0.01f)
    , m_scaleOnSelected(1.0f)
    , m_colorOnSelected(COLOR_WHITE)
    {
    }

    UIFloatOptionComponent_Template::~UIFloatOptionComponent_Template()
    {
    }
}

