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

#ifndef _ITF_RAY_OPTIONMENUHELPER_H_
#include "rayman/gameplay/Managers/GameOptions/Ray_OptionMenuHelper.h"
#endif //_ITF_RAY_OPTIONMENUHELPER_H_

#include <algorithm>

namespace ITF
{
    ///////////////////////////////////////////////////////////////////////////////////////////
    IMPLEMENT_OBJECT_RTTI(UIFloatOptionComponent)
    BEGIN_SERIALIZATION_CHILD(UIFloatOptionComponent)
        BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
            SERIALIZE_MEMBER("sliderBackgroundPath", m_sliderBackgroundPath);
            SERIALIZE_MEMBER("sliderBackgroundStartPath", m_sliderBackgroundStartPath);
            SERIALIZE_MEMBER("sliderBackgroundEndPath", m_sliderBackgroundEndPath);
            SERIALIZE_MEMBER("sliderCursorPath", m_sliderCursorPath);
            SERIALIZE_MEMBER("sliderBackgroundSelectedPath", m_sliderBackgroundSelectedPath);
            SERIALIZE_MEMBER("sliderCursorSelectedPath", m_sliderCursorSelectedPath);
            SERIALIZE_MEMBER("sliderBackgroundDeactivatedPath", m_sliderBackgroundDeactivatedPath);
            SERIALIZE_MEMBER("sliderCursorDeactivatedPath", m_sliderCursorDeactivatedPath);
        END_CONDITION_BLOCK()
    END_SERIALIZATION()

    ///////////////////////////////////////////////////////////////////////////////////////////
    UIFloatOptionComponent::UIFloatOptionComponent()
    : Super()
    , m_sliderBackgroundActor(NULL)
    , m_sliderBackgroundStartActor(NULL)
    , m_sliderBackgroundEndActor(NULL)
    , m_sliderCursorActor(NULL)
    , m_sliderBackgroundSelectedActor(NULL)
    , m_sliderCursorSelectedActor(NULL)
    , m_sliderBackgroundDeactivatedActor(NULL)
    , m_sliderCursorDeactivatedActor(NULL)
    , m_cursorGraphComponent(NULL)
    , m_isSliding(bfalse)
    , m_originalCursorScale(1.0f, 1.0f)
    , m_originalCursorSelectedScale(1.0f, 1.0f)
    , m_value(0.5f)
    , m_exitEditAfterRelease(bfalse)
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
        m_sliderBackgroundActor = NULL;
        m_sliderBackgroundStartActor = NULL;
        m_sliderBackgroundEndActor = NULL;
        m_sliderCursorActor = NULL;
        m_sliderBackgroundSelectedActor = NULL;
        m_sliderCursorSelectedActor = NULL;
        m_sliderBackgroundDeactivatedActor = NULL;
        m_sliderCursorDeactivatedActor = NULL;
        m_cursorGraphComponent = NULL;
        m_isSliding = bfalse;
        m_exitEditAfterRelease = bfalse;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIFloatOptionComponent::resolveSliderActors()
    {
        m_sliderBackgroundActor = NULL;
        m_sliderBackgroundStartActor = NULL;
        m_sliderBackgroundEndActor = NULL;
        m_sliderCursorActor = NULL;
        m_sliderBackgroundSelectedActor = NULL;
        m_sliderCursorSelectedActor = NULL;
        m_sliderBackgroundDeactivatedActor = NULL;
        m_sliderCursorDeactivatedActor = NULL;

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
        resolveActor(m_sliderBackgroundPath, m_sliderBackgroundActor);
        resolveActor(m_sliderBackgroundSelectedPath, m_sliderBackgroundSelectedActor);
        resolveActor(m_sliderCursorSelectedPath, m_sliderCursorSelectedActor);
        resolveActor(m_sliderBackgroundDeactivatedPath, m_sliderBackgroundDeactivatedActor);
        resolveActor(m_sliderCursorDeactivatedPath, m_sliderCursorDeactivatedActor);

        if (m_sliderCursorActor)
        {
            m_cursorGraphComponent = m_sliderCursorActor->GetComponent<TextureGraphicComponent2D>();
            if (m_cursorGraphComponent)
            {
                m_originalCursorScale = m_sliderCursorActor->getScale();
            }
        }

        if (m_sliderCursorSelectedActor)
        {
            m_originalCursorSelectedScale = m_sliderCursorSelectedActor->getScale();
        }
    }

    void UIFloatOptionComponent::updateDeactivatedVisuals()
    {
        const bbool isActive = getActive();

        if (!isActive)
        {
            if (m_sliderBackgroundActor)
                m_sliderBackgroundActor->disable();
            if (m_sliderBackgroundSelectedActor)
                m_sliderBackgroundSelectedActor->disable();

            if (m_sliderCursorActor)
                m_sliderCursorActor->disable();
            if (m_sliderCursorSelectedActor)
                m_sliderCursorSelectedActor->disable();

            if (m_sliderBackgroundDeactivatedActor)
                m_sliderBackgroundDeactivatedActor->enable();
            if (m_sliderCursorDeactivatedActor)
                m_sliderCursorDeactivatedActor->enable();

            m_isSliding = bfalse;
            return;
        }

        if (m_sliderBackgroundDeactivatedActor)
            m_sliderBackgroundDeactivatedActor->disable();
        if (m_sliderCursorDeactivatedActor)
            m_sliderCursorDeactivatedActor->disable();

        if (m_sliderBackgroundActor)
            m_sliderBackgroundActor->enable();

        if (getIsSelected())
            switchToSelectedActors();
        else
            switchToNormalActors();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIFloatOptionComponent::updateSliderVisuals()
    {
        if (!m_sliderBackgroundStartActor || !m_sliderBackgroundEndActor)
            return;

        UIComponent* startComponent = m_sliderBackgroundStartActor->GetComponent<UIComponent>();
        UIComponent* endComponent = m_sliderBackgroundEndActor->GetComponent<UIComponent>();
        if (!startComponent || !endComponent)
            return;

        f32 startPos = startComponent->getRelativePosX();
        f32 endPos = endComponent->getRelativePosX();
        f32 width = endPos - startPos;

        if (width <= 0.0f)
            return;

        f32 cursorRelativePos = startPos + width * m_value;

        auto setCursorX = [cursorRelativePos](Actor* cursorActor) -> void
        {
            if (!cursorActor)
                return;

            UIComponent* cursorComponent = cursorActor->GetComponent<UIComponent>();
            if (!cursorComponent)
                return;

            Vec2d cursorRelativePosVec(cursorRelativePos, cursorComponent->getRelativePosY());
            cursorComponent->setRelativePos(cursorRelativePosVec);
        };

        setCursorX(m_sliderCursorActor);
        setCursorX(m_sliderCursorSelectedActor);
        setCursorX(m_sliderCursorDeactivatedActor);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIFloatOptionComponent::updateSliderFromMouse()
    {
        if (!m_isSliding || !m_sliderBackgroundStartActor || !m_sliderBackgroundEndActor)
            return;

        if (!INPUT_ADAPTER->isMousePressed(InputAdapter::MB_Left))
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
        if (m_sliderBackgroundDeactivatedActor)
            m_sliderBackgroundDeactivatedActor->disable();

        if (m_sliderCursorDeactivatedActor)
            m_sliderCursorDeactivatedActor->disable();

        if (m_sliderBackgroundActor)
            m_sliderBackgroundActor->enable();

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
        if (m_sliderBackgroundDeactivatedActor)
            m_sliderBackgroundDeactivatedActor->disable();

        if (m_sliderCursorDeactivatedActor)
            m_sliderCursorDeactivatedActor->disable();

        if (m_sliderBackgroundActor)
            m_sliderBackgroundActor->disable();

        if (m_sliderCursorActor)
            m_sliderCursorActor->disable();

        if (m_sliderBackgroundSelectedActor)
            m_sliderBackgroundSelectedActor->enable();

        if (m_sliderCursorSelectedActor)
            m_sliderCursorSelectedActor->enable();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIFloatOptionComponent::handleSelectionChanged(bbool isSelected)
    {
        Super::handleSelectionChanged(isSelected);

        if (!getActive())
        {
            updateDeactivatedVisuals();
            return;
        }

        if (isSelected)
        {
            switchToSelectedActors();

            if (m_sliderCursorSelectedActor)
            {
                class TextureGraphicComponent2D* selectedCursorGraph = m_sliderCursorSelectedActor->GetComponent<TextureGraphicComponent2D>();
                if (selectedCursorGraph)
                {
                    m_sliderCursorSelectedActor->setScale(m_originalCursorSelectedScale * getTemplate()->getScaleOnSelected());
                    selectedCursorGraph->setDrawColor(getTemplate()->getColorOnSelected().getAsU32());
                }
            }
        }
        else
        {
            switchToNormalActors();

            if (m_sliderCursorActor && m_cursorGraphComponent)
            {
                m_sliderCursorActor->setScale(m_originalCursorScale);
                m_cursorGraphComponent->setDrawColor(COLOR_WHITE);
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIFloatOptionComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);
        applyLabelColors();
        resolveSliderActors();
        
        if (m_sliderBackgroundStartActor)
            m_sliderBackgroundStartActor->disableDraw(btrue);
        
        if (m_sliderBackgroundEndActor)
            m_sliderBackgroundEndActor->disableDraw(btrue);
        
        switchToNormalActors();
        updateDeactivatedVisuals();
        updateSliderVisuals();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIFloatOptionComponent::Update(f32 _deltaTime)
    {
        Super::Update(_deltaTime);

        if (!getActive())
            return;

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
        updateDeactivatedVisuals();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIFloatOptionComponent::onBecomeInactive()
    {
        Super::onBecomeInactive();
        updateDeactivatedVisuals();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIFloatOptionComponent::onPressed()
    {
        Super::onPressed();
        m_isSliding = btrue;
        m_exitEditAfterRelease = INPUT_ADAPTER->isMousePressed(InputAdapter::MB_Left);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIFloatOptionComponent::onReleased()
    {
        Super::onReleased();
        m_isSliding = bfalse;
        if (m_exitEditAfterRelease)
        {
            Ray_OptionMenuHelper::requestExitEditMode(this);
        }
        m_exitEditAfterRelease = bfalse;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIFloatOptionComponent::onRollover()
    {
        Super::onRollover();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIFloatOptionComponent::onRollout()
    {
        Super::onRollout();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIFloatOptionComponent::onAction(const StringID & action)
    {
        Super::onAction(action);

        if (Ray_OptionMenuHelper::handleExternalEditingInput(this, action))
            return;

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


