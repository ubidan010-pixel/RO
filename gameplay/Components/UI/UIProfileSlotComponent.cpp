#include "precompiled_gameplay.h"

#ifndef _ITF_UIPROFILESLOTCOMPONENT_H_
#include "gameplay/components/UI/UIProfileSlotComponent.h"
#endif //_ITF_UIPROFILESLOTCOMPONENT_H_

#ifndef _ITF_EVENTMANAGER_H_
#include "engine/events/EventManager.h"
#endif //_ITF_EVENTMANAGER_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(UIProfileSlotComponent)
    BEGIN_SERIALIZATION_CHILD(UIProfileSlotComponent)
        BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
            SERIALIZE_MEMBER("playerIndex", m_playerIndex);
            SERIALIZE_MEMBER("resetButtonPath", m_resetButtonPath);
            SERIALIZE_MEMBER("controlsPath", m_controlsPath);
            SERIALIZE_MEMBER("playerTitlePath", m_playerTitlePath);
            SERIALIZE_MEMBER("playerTitlePathConnected", m_playerTitlePathConnected);
            SERIALIZE_MEMBER("playerTitlePathDisconnected", m_playerTitlePathDisconnected);
            SERIALIZE_MEMBER("actionsBgPath", m_actionsBgPath);
            SERIALIZE_MEMBER("actionsBgPathConnected", m_actionsBgPathConnected);
            SERIALIZE_MEMBER("actionsBgPathDisconnected", m_actionsBgPathDisconnected);
            SERIALIZE_MEMBER("actionUpPath", m_actionUpPath);
            SERIALIZE_MEMBER("actionDownPath", m_actionDownPath);
            SERIALIZE_MEMBER("actionLeftPath", m_actionLeftPath);
            SERIALIZE_MEMBER("actionRightPath", m_actionRightPath);
            SERIALIZE_MEMBER("actionRunPath", m_actionRunPath);
            SERIALIZE_MEMBER("actionJumpPath", m_actionJumpPath);
            SERIALIZE_MEMBER("actionHitPath", m_actionHitPath);
            SERIALIZE_MEMBER("actionBackPath", m_actionBackPath);
            SERIALIZE_MEMBER("connectControllerTextPath", m_connectControllerTextPath);
        END_CONDITION_BLOCK()
    END_SERIALIZATION()

    UIProfileSlotComponent::UIProfileSlotComponent()
    : m_playerIndex(0)
    , m_isControllerConnected(bfalse)
    , m_controllerType(InputAdapter::Pad_Invalid)
    , m_eventListenerRegistered(bfalse)
    {
    }

    UIProfileSlotComponent::~UIProfileSlotComponent()
    {
        unregisterEventListeners();
        UIProfileSlotComponent::clear();
    }

    void UIProfileSlotComponent::clear()
    {
        m_playerIndex = 0;
        m_resetButtonPath.clear();
        m_controlsPath.clear();
        m_playerTitlePath.clear();
        m_playerTitlePathConnected.clear();
        m_playerTitlePathDisconnected.clear();
        m_actionsBgPath.clear();
        m_actionsBgPathConnected.clear();
        m_actionsBgPathDisconnected.clear();
        m_actionUpPath.clear();
        m_actionDownPath.clear();
        m_actionLeftPath.clear();
        m_actionRightPath.clear();
        m_actionRunPath.clear();
        m_actionJumpPath.clear();
        m_actionHitPath.clear();
        m_actionBackPath.clear();
        m_connectControllerTextPath.clear();
        m_isControllerConnected = bfalse;
        m_controllerType = InputAdapter::Pad_Invalid;
    }

    void UIProfileSlotComponent::registerEventListeners()
    {
        if (m_eventListenerRegistered)
            return;

        if (EVENTMANAGER)
        {
            EVENTMANAGER->registerEvent(ITF_GET_STRINGID_CRC(EventControllerStateChanged, 3543189344), this);
            m_eventListenerRegistered = btrue;
        }
    }

    void UIProfileSlotComponent::unregisterEventListeners()
    {
        if (!m_eventListenerRegistered)
            return;

        if (EVENTMANAGER)
        {
            EVENTMANAGER->unregisterEvent(ITF_GET_STRINGID_CRC(EventControllerStateChanged, 3543189344), this);
            m_eventListenerRegistered = bfalse;
        }
    }

    void UIProfileSlotComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);
        if (INPUT_ADAPTER && m_playerIndex < JOY_MAX_COUNT)
        {
            m_isControllerConnected = INPUT_ADAPTER->isPadConnected(m_playerIndex);
            m_controllerType = INPUT_ADAPTER->getPadType(m_playerIndex);
        }
        LOG("[UIProfileSlotComponent] Initialized for player %u - Controller connected: %s, Type: %d",
            m_playerIndex,
            m_isControllerConnected ? "true" : "false",
            static_cast<i32>(m_controllerType));
        registerEventListeners();
        updateAllVisibility();
    }

    void UIProfileSlotComponent::Update(f32 _deltaTime)
    {
        Super::Update(_deltaTime);

        if (!INPUT_ADAPTER || m_playerIndex >= JOY_MAX_COUNT)
            return;

        const bbool isConnectedNow = INPUT_ADAPTER->isPadConnected(m_playerIndex);
        const InputAdapter::PadType padTypeNow = INPUT_ADAPTER->getPadType(m_playerIndex);

        if (isConnectedNow != m_isControllerConnected || padTypeNow != m_controllerType)
        {
            m_isControllerConnected = isConnectedNow;
            m_controllerType = padTypeNow;
            updateAllVisibility();
        }
    }

    void UIProfileSlotComponent::onActorClearComponents()
    {
        unregisterEventListeners();
        Super::onActorClearComponents();
    }

    void UIProfileSlotComponent::onBecomeActive()
    {
        Super::onBecomeActive();
        registerEventListeners();
        updateAllVisibility();
    }

    void UIProfileSlotComponent::onBecomeInactive()
    {
        unregisterEventListeners();
        Super::onBecomeInactive();
    }

    void UIProfileSlotComponent::onPressed()
    {
        Super::onPressed();
    }

    void UIProfileSlotComponent::onReleased()
    {
        Super::onReleased();
    }

    void UIProfileSlotComponent::onRollover()
    {
        Super::onRollover();
    }

    void UIProfileSlotComponent::onRollout()
    {
        Super::onRollout();
    }

    void UIProfileSlotComponent::onAction(const StringID & action)
    {
        Super::onAction(action);
    }

    bbool UIProfileSlotComponent::isControllerConnected() const
    {
        if (INPUT_ADAPTER && m_playerIndex < JOY_MAX_COUNT)
        {
            return INPUT_ADAPTER->isPadConnected(m_playerIndex);
        }
        return m_isControllerConnected;
    }

    InputAdapter::PadType UIProfileSlotComponent::getControllerType() const
    {
        if (INPUT_ADAPTER && m_playerIndex < JOY_MAX_COUNT)
        {
            return INPUT_ADAPTER->getPadType(m_playerIndex);
        }
        return m_controllerType;
    }

    void UIProfileSlotComponent::onEvent(Event* _event)
    {
        if (EventControllerStateChanged* controllerEvent =
            _event->DynamicCast<EventControllerStateChanged>(ITF_GET_STRINGID_CRC(EventControllerStateChanged, 3543189344)))
        {
            onControllerStateChanged(
                controllerEvent->getPadIndex(),
                controllerEvent->isConnected(),
                controllerEvent->getPadType()
            );
        }
    }

    void UIProfileSlotComponent::onControllerStateChanged(u32 padIndex, bbool connected, InputAdapter::PadType padType)
    {
        if (padIndex != m_playerIndex)
            return;
        bbool wasConnected = m_isControllerConnected;
        m_isControllerConnected = connected;
        m_controllerType = padType;
        LOG("[UIProfileSlotComponent] Player %u controller state changed: %s -> %s (Type: %d)",
            m_playerIndex,
            wasConnected ? "connected" : "disconnected",
            connected ? "connected" : "disconnected",
            static_cast<i32>(padType));
        if (wasConnected != connected)
        {
            updateAllVisibility();
            LOG("[UIProfileSlotComponent] Total connected controllers: %u", getConnectedControllersCount());
        }
    }

    void UIProfileSlotComponent::setActorVisibility(const String8& actorPath, bbool visible)
    {
        if (actorPath.isEmpty() || !m_actor)
            return;

        Scene* scene = m_actor->getScene();
        if (!scene)
            return;

        Actor* targetActor = scene->getActorFromUserFriendly(actorPath);
        if (!targetActor)
        {
            LOG("[UIProfileSlotComponent] Player %u: Actor '%s' not found", m_playerIndex, actorPath.cStr());
            return;
        }

        if (visible)
            targetActor->enable();
        else
            targetActor->disable();
    }

    void UIProfileSlotComponent::updateAllVisibility()
    {
        bbool visible = isControllerConnected();
        LOG("[UIProfileSlotComponent] Player %u: Updating visibility (controller %s)",
            m_playerIndex, visible ? "connected" : "disconnected");
        setActorVisibility(m_resetButtonPath, visible);
        setActorVisibility(m_controlsPath, visible);

        if (!m_playerTitlePathConnected.isEmpty() && !m_playerTitlePathDisconnected.isEmpty())
        {
            setActorVisibility(m_playerTitlePathConnected, visible);
            setActorVisibility(m_playerTitlePathDisconnected, !visible);
        }

        if (!m_actionsBgPathConnected.isEmpty() && !m_actionsBgPathDisconnected.isEmpty())
        {
            setActorVisibility(m_actionsBgPathConnected, visible);
            setActorVisibility(m_actionsBgPathDisconnected, !visible);
        }

        setActorVisibility(m_actionUpPath, visible);
        setActorVisibility(m_actionDownPath, visible);
        setActorVisibility(m_actionLeftPath, visible);
        setActorVisibility(m_actionRightPath, visible);
        setActorVisibility(m_actionRunPath, visible);
        setActorVisibility(m_actionJumpPath, visible);
        setActorVisibility(m_actionHitPath, visible);

        setActorVisibility(m_connectControllerTextPath, !visible);
    }

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
