#include "precompiled_gameplay.h"

#ifndef _ITF_UIPROFILESLOTCOMPONENT_H_
#include "gameplay/components/UI/UIProfileSlotComponent.h"
#endif //_ITF_UIPROFILESLOTCOMPONENT_H_

#ifndef _ITF_EVENTS_H_
#include "engine/events/Events.h"
#endif //_ITF_EVENTS_H_

#ifndef _ITF_EVENTMANAGER_H_
#include "engine/events/EventManager.h"
#endif //_ITF_EVENTMANAGER_H_

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

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
    , m_isControllerConnected(bfalse)
    , m_controllerType(InputAdapter::Pad_Invalid)
    , m_eventListenerRegistered(bfalse)
    {
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    UIProfileSlotComponent::~UIProfileSlotComponent()
    {
        unregisterEventListeners();
        clear();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIProfileSlotComponent::clear()
    {
        m_playerIndex = 0;
        m_resetButtonPath = StringID();
        m_isControllerConnected = bfalse;
        m_controllerType = InputAdapter::Pad_Invalid;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIProfileSlotComponent::registerEventListeners()
    {
        if (m_eventListenerRegistered)
            return;

        if (EVENTMANAGER)
        {
            EVENTMANAGER->registerEvent(ITF_GET_STRINGID_CRC(EventControllerStateChanged, 1847293651), this);
            m_eventListenerRegistered = btrue;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIProfileSlotComponent::unregisterEventListeners()
    {
        if (!m_eventListenerRegistered)
            return;

        if (EVENTMANAGER)
        {
            EVENTMANAGER->unregisterEvent(ITF_GET_STRINGID_CRC(EventControllerStateChanged, 1847293651), this);
            m_eventListenerRegistered = bfalse;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIProfileSlotComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        // Check initial controller state
        if (INPUT_ADAPTER && m_playerIndex < JOY_MAX_COUNT)
        {
            m_isControllerConnected = INPUT_ADAPTER->isPadConnected(m_playerIndex);
            m_controllerType = INPUT_ADAPTER->getPadType(m_playerIndex);
        }

        // Register for controller events
        registerEventListeners();

        // Update reset button visibility based on initial state
        updateResetButtonVisibility();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIProfileSlotComponent::Update(f32 _deltaTime)
    {
        Super::Update(_deltaTime);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIProfileSlotComponent::onActorClearComponents()
    {
        unregisterEventListeners();
        Super::onActorClearComponents();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIProfileSlotComponent::onBecomeActive()
    {
        Super::onBecomeActive();
        registerEventListeners();
        updateResetButtonVisibility();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIProfileSlotComponent::onBecomeInactive()
    {
        unregisterEventListeners();
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
    bbool UIProfileSlotComponent::isControllerConnected() const
    {
        // Always check current state from InputAdapter for accuracy
        if (INPUT_ADAPTER && m_playerIndex < JOY_MAX_COUNT)
        {
            return INPUT_ADAPTER->isPadConnected(m_playerIndex);
        }
        return m_isControllerConnected;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    InputAdapter::PadType UIProfileSlotComponent::getControllerType() const
    {
        if (INPUT_ADAPTER && m_playerIndex < JOY_MAX_COUNT)
        {
            return INPUT_ADAPTER->getPadType(m_playerIndex);
        }
        return m_controllerType;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIProfileSlotComponent::onEvent(Event* _event)
    {
        if (EventControllerStateChanged* controllerEvent = 
            _event->DynamicCast<EventControllerStateChanged>(ITF_GET_STRINGID_CRC(EventControllerStateChanged, 1847293651)))
        {
            onControllerStateChanged(
                controllerEvent->getPadIndex(),
                controllerEvent->isConnected(),
                controllerEvent->getPadType()
            );
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIProfileSlotComponent::onControllerStateChanged(u32 padIndex, bbool connected, InputAdapter::PadType padType)
    {
        // Only process events for our player index
        if (padIndex != m_playerIndex)
            return;

        bbool wasConnected = m_isControllerConnected;
        m_isControllerConnected = connected;
        m_controllerType = padType;

        // State changed - update UI accordingly
        if (wasConnected != connected)
        {
            updateResetButtonVisibility();
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIProfileSlotComponent::updateResetButtonVisibility()
    {
        // TODO: Implement show/hide logic for reset button based on controller connection
        // This can be expanded later to find the reset button actor by userfriendly name
        // and enable/disable it based on m_isControllerConnected
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
