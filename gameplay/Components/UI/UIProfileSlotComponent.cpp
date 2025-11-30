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

#ifndef _ITF_TEXTUREGRAPHICCOMPONENT2D_H_
#include "engine/actors/components/texturegraphiccomponent2D.h"
#endif //_ITF_TEXTUREGRAPHICCOMPONENT2D_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

namespace ITF
{
    ///////////////////////////////////////////////////////////////////////////////////////////
    IMPLEMENT_OBJECT_RTTI(UIProfileSlotComponent)
    BEGIN_SERIALIZATION_CHILD(UIProfileSlotComponent)
        BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
            SERIALIZE_MEMBER("playerIndex", m_playerIndex);
            SERIALIZE_MEMBER("resetButtonPath", m_resetButtonPath);
            SERIALIZE_MEMBER("controlsPath", m_controlsPath);
            SERIALIZE_MEMBER("presetPath", m_presetPath);
            SERIALIZE_MEMBER("playerTitlePath", m_playerTitlePath);
            SERIALIZE_MEMBER("actionsBgPath", m_actionsBgPath);
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

    ///////////////////////////////////////////////////////////////////////////////////////////
    UIProfileSlotComponent::UIProfileSlotComponent()
    : Super()
    , m_playerIndex(0)
    , m_resetButtonPath()
    , m_controlsPath()
    , m_presetPath()
    , m_playerTitlePath()
    , m_actionsBgPath()
    , m_actionUpPath()
    , m_actionDownPath()
    , m_actionLeftPath()
    , m_actionRightPath()
    , m_actionRunPath()
    , m_actionJumpPath()
    , m_actionHitPath()
    , m_actionBackPath()
    , m_connectControllerTextPath()
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
        m_controlsPath.clear();
        m_presetPath.clear();
        m_playerTitlePath.clear();
        m_actionsBgPath.clear();
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

        // Update all UI elements visibility based on initial state
        updateAllVisibility();
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
        updateAllVisibility();
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
            updateAllVisibility();

            // Log current total connected controllers
            LOG("[UIProfileSlotComponent] Total connected controllers: %u", getConnectedControllersCount());
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
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

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIProfileSlotComponent::setActorBackgroundTexture(const String8& actorPath, const Path& texturePath)
    {
        if (actorPath.isEmpty() || texturePath.isEmpty() || !m_actor)
            return;

        Scene* scene = m_actor->getScene();
        if (!scene)
            return;

        Actor* targetActor = scene->getActorFromUserFriendly(actorPath);
        if (!targetActor)
        {
            LOG("[UIProfileSlotComponent] Player %u: Actor '%s' not found for texture change", m_playerIndex, actorPath.cStr());
            return;
        }

        // Get TextureGraphicComponent2D from the actor
        TextureGraphicComponent2D* texComp = targetActor->GetComponent<TextureGraphicComponent2D>();
        if (!texComp)
        {
            LOG("[UIProfileSlotComponent] Player %u: Actor '%s' has no TextureGraphicComponent2D", m_playerIndex, actorPath.cStr());
            return;
        }

        // Create new resource ID for the texture and change it
        ResourceID newTextureID = targetActor->addResource(Resource::ResourceType_Texture, texturePath);
        if (newTextureID.isValidResourceId())
        {
            texComp->changeTexture(newTextureID);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIProfileSlotComponent::updateAllVisibility()
    {
        bbool visible = isControllerConnected();
        LOG("[UIProfileSlotComponent] Player %u: Updating visibility (controller %s)",
            m_playerIndex, visible ? "connected" : "disconnected");

        // Header elements
        setActorVisibility(m_resetButtonPath, visible);
        setActorVisibility(m_controlsPath, visible);
        setActorVisibility(m_presetPath, visible);

        // Player title - change texture instead of hiding
        const Path& titleTexConnected = getTemplate()->getPlayerTitleTextureConnected();
        const Path& titleTexDisconnected = getTemplate()->getPlayerTitleTextureDisconnected();
        if (!titleTexConnected.isEmpty() && !titleTexDisconnected.isEmpty())
        {
            const Path& titleTexturePath = visible ? titleTexConnected : titleTexDisconnected;
            setActorBackgroundTexture(m_playerTitlePath, titleTexturePath);
        }

        // Actions background - change texture instead of hiding
        const Path& texConnected = getTemplate()->getActionsBgTextureConnected();
        const Path& texDisconnected = getTemplate()->getActionsBgTextureDisconnected();
        if (!texConnected.isEmpty() && !texDisconnected.isEmpty())
        {
            const Path& texturePath = visible ? texConnected : texDisconnected;
            setActorBackgroundTexture(m_actionsBgPath, texturePath);
        }

        // Action icons - show/hide based on connection
        setActorVisibility(m_actionUpPath, visible);
        setActorVisibility(m_actionDownPath, visible);
        setActorVisibility(m_actionLeftPath, visible);
        setActorVisibility(m_actionRightPath, visible);
        setActorVisibility(m_actionRunPath, visible);
        setActorVisibility(m_actionJumpPath, visible);
        setActorVisibility(m_actionHitPath, visible);
        setActorVisibility(m_actionBackPath, visible);

        // Connect controller text - INVERTED: show when disconnected, hide when connected
        setActorVisibility(m_connectControllerTextPath, !visible);
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
        SERIALIZE_MEMBER("actionsBgTextureConnected", m_actionsBgTextureConnected);
        SERIALIZE_MEMBER("actionsBgTextureDisconnected", m_actionsBgTextureDisconnected);
        SERIALIZE_MEMBER("playerTitleTextureConnected", m_playerTitleTextureConnected);
        SERIALIZE_MEMBER("playerTitleTextureDisconnected", m_playerTitleTextureDisconnected);
    END_SERIALIZATION()

    UIProfileSlotComponent_Template::UIProfileSlotComponent_Template()
    : Super()
    , m_actionsBgTextureConnected()
    , m_actionsBgTextureDisconnected()
    , m_playerTitleTextureConnected()
    , m_playerTitleTextureDisconnected()
    {
    }

    UIProfileSlotComponent_Template::~UIProfileSlotComponent_Template()
    {
    }
}
