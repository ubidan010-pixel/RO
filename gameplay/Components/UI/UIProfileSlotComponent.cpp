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
        m_resetButtonPath.clear();
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

        // Log initialization info
        LOG("[UIProfileSlotComponent] Initialized for player %u - Controller connected: %s, Type: %d",
            m_playerIndex,
            m_isControllerConnected ? "true" : "false",
            static_cast<i32>(m_controllerType));

        // Log all connected controllers on first slot initialization
        if (m_playerIndex == 0)
        {
            logAllControllersState();
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

        // Log the state change
        LOG("[UIProfileSlotComponent] Player %u controller state changed: %s -> %s (Type: %d)",
            m_playerIndex,
            wasConnected ? "connected" : "disconnected",
            connected ? "connected" : "disconnected",
            static_cast<i32>(padType));

        // State changed - update UI accordingly
        if (wasConnected != connected)
        {
            updateResetButtonVisibility();
            
            // Log current total connected controllers
            LOG("[UIProfileSlotComponent] Total connected controllers: %u", getConnectedControllersCount());
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIProfileSlotComponent::updateResetButtonVisibility()
    {
        // Skip if no reset button path is set
        if (m_resetButtonPath.isEmpty())
        {
            LOG("[UIProfileSlotComponent] Player %u: No reset button path set", m_playerIndex);
            return;
        }

        // Get the scene from the actor
        if (!m_actor)
        {
            LOG("[UIProfileSlotComponent] Player %u: No actor found", m_playerIndex);
            return;
        }

        Scene* scene = m_actor->getScene();
        if (!scene)
        {
            LOG("[UIProfileSlotComponent] Player %u: No scene found", m_playerIndex);
            return;
        }

        // Find the reset button actor by its userfriendly name
        Actor* resetButtonActor = scene->getActorFromUserFriendly(m_resetButtonPath);
        if (!resetButtonActor)
        {
            LOG("[UIProfileSlotComponent] Player %u: Reset button actor '%s' not found in scene",
                m_playerIndex, m_resetButtonPath.cStr());
            return;
        }

        // Show reset button if controller is connected, hide if not
        bbool shouldShow = isControllerConnected();
        
        if (shouldShow)
        {
            resetButtonActor->enable();
            LOG("[UIProfileSlotComponent] Player %u: Showing reset button '%s' (controller connected)",
                m_playerIndex, m_resetButtonPath.cStr());
        }
        else
        {
            resetButtonActor->disable();
            LOG("[UIProfileSlotComponent] Player %u: Hiding reset button '%s' (controller disconnected)",
                m_playerIndex, m_resetButtonPath.cStr());
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    u32 UIProfileSlotComponent::getConnectedControllersCount()
    {
        u32 count = 0;
        if (INPUT_ADAPTER)
        {
            for (u32 i = 0; i < JOY_MAX_COUNT; ++i)
            {
                if (INPUT_ADAPTER->isPadConnected(i))
                {
                    ++count;
                }
            }
        }
        return count;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIProfileSlotComponent::logAllControllersState()
    {
        LOG("[UIProfileSlotComponent] === Controllers State Summary ===");
        LOG("[UIProfileSlotComponent] Total connected: %u / %u", getConnectedControllersCount(), JOY_MAX_COUNT);
        
        if (INPUT_ADAPTER)
        {
            for (u32 i = 0; i < JOY_MAX_COUNT; ++i)
            {
                bbool connected = INPUT_ADAPTER->isPadConnected(i);
                InputAdapter::PadType padType = INPUT_ADAPTER->getPadType(i);
                LOG("[UIProfileSlotComponent]   Player %u: %s (Type: %d)",
                    i,
                    connected ? "CONNECTED" : "disconnected",
                    static_cast<i32>(padType));
            }
        }
        LOG("[UIProfileSlotComponent] =================================");
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
