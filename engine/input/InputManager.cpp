#include "precompiled_engine.h"

#ifndef _ITF_INPUT_MANAGER_H_
#include "engine/input/InputManager.h"
#endif //_ITF_INPUT_MANAGER_H_

#ifndef _ITF_KEYBOARD_INPUT_SOURCE_H_
#include "engine/input/KeyboardInputSource.h"
#endif //_ITF_KEYBOARD_INPUT_SOURCE_H_

#ifndef _ITF_CONTROLLER_INPUT_SOURCE_H_
#include "engine/input/ControllerInputSource.h"
#endif //_ITF_CONTROLLER_INPUT_SOURCE_H_

#ifndef _ITF_INPUT_MAPPING_DEFAULTS_H_
#include "engine/input/InputMappingDefaults.h"
#endif //_ITF_INPUT_MAPPING_DEFAULTS_H_

namespace ITF
{
    InputManager::InputManager()
        : m_initialized(false)
    {
    }

    InputManager::~InputManager()
    {
        Shutdown();
    }

    void InputManager::Initialize()
    {
        if (m_initialized)
        {
            LOG("[InputManager] Initialize: Already initialized");
            return;
        }

        LOG("[InputManager] Initialize: Starting initialization");

        if (!GetInputSource(KEYBOARD_DEVICE_ID))
        {
            LOG("[InputManager] Initialize: Creating KeyboardInputSource");
            auto keyboardSource = std::make_unique<KeyboardInputSource>();
            RegisterInputSource(std::move(keyboardSource));
            LOG("[InputManager] Initialize: KeyboardInputSource registered");
        }
        else
        {
            LOG("[InputManager] Initialize: KeyboardInputSource already exists");
        }

        LOG("[InputManager] Initialize: Initializing default mappings");
        InitializeDefaultMappings();
        LOG("[InputManager] Initialize: Default mappings initialized");

        m_initialized = true;
        LOG("[InputManager] Initialize: Initialization complete");
    }

    void InputManager::Update()
    {
        if (!m_initialized)
            return;

        UpdateInputSources();
        UpdateVirtualState();
    }

    void InputManager::Shutdown()
    {
        if (!m_initialized)
            return;

        m_inputMapping.Clear();
        for (u32 i = 0; i < JOY_MAX_COUNT; ++i)
        {
            m_remapOverrides[i].Clear();
        }
        m_actionMapping.Clear();

        m_inputSources.clear();
        m_inputSourcePtrs.clear();

        m_initialized = false;
    }

    void InputManager::RegisterInputSource(std::unique_ptr<IInputSource> source)
    {
        if (!source)
            return;

        IInputSource* ptr = source.get();
        m_inputSources.push_back(std::move(source));
        m_inputSourcePtrs.push_back(ptr);
    }

    IInputSource* InputManager::GetInputSource(u32 deviceId) const
    {
        for (IInputSource* source : m_inputSourcePtrs)
        {
            if (source && source->GetDeviceId() == deviceId)
                return source;
        }
        return nullptr;
    }

    void InputManager::SetRemap(u32 player, const PhysicalInput& from, const VirtualInput& to)
    {
        if (player >= JOY_MAX_COUNT)
            return;

        m_remapOverrides[player].AddMapping(from, to);
    }

    void InputManager::ClearRemap(u32 player, const VirtualInput& virtualInput)
    {
        if (player >= JOY_MAX_COUNT)
            return;

        std::vector<PhysicalInput> physicalInputs;
        m_remapOverrides[player].GetPhysicalInputs(virtualInput, physicalInputs);

        for (const auto& physical : physicalInputs)
        {
            m_remapOverrides[player].RemoveMapping(physical);
        }
    }

    void InputManager::ResetRemap(u32 player)
    {
        if (player >= JOY_MAX_COUNT)
            return;

        m_remapOverrides[player].Clear();
    }

    void InputManager::ResetAllRemaps()
    {
        for (u32 i = 0; i < JOY_MAX_COUNT; ++i)
        {
            m_remapOverrides[i].Clear();
        }
    }

    bool InputManager::GetRemappedInput(u32 player, const PhysicalInput& physical, VirtualInput& virtualInput) const
    {
        if (player >= JOY_MAX_COUNT)
            return false;

        if (m_remapOverrides[player].GetVirtualInput(physical, virtualInput))
        {
            return true;
        }

        return m_inputMapping.GetVirtualInput(physical, virtualInput);
    }

    void InputManager::getGamePadButtons(u32 player, InputAdapter::PressStatus* buttons, u32 numButtons) const
    {
        if (!buttons || player >= JOY_MAX_COUNT)
            return;

        const u32 count = (numButtons < JOY_MAX_BUT) ? numButtons : JOY_MAX_BUT;
        
        static const VirtualButton joyToVirtualMapping[JOY_MAX_BUT] = {
            VIRTUAL_BUTTON_A,
            VIRTUAL_BUTTON_B,
            VIRTUAL_BUTTON_X,
            VIRTUAL_BUTTON_Y,
            VIRTUAL_BUTTON_LB,
            VIRTUAL_BUTTON_RB,
            VIRTUAL_BUTTON_BACK,
            VIRTUAL_BUTTON_START,
            VIRTUAL_BUTTON_DPAD_LEFT,
            VIRTUAL_BUTTON_DPAD_RIGHT,
            VIRTUAL_BUTTON_DPAD_UP,
            VIRTUAL_BUTTON_DPAD_DOWN,
            VIRTUAL_BUTTON_LTHUMB,
            VIRTUAL_BUTTON_RTHUMB,
            VIRTUAL_BUTTON_LTHUMB,
            VIRTUAL_BUTTON_RTHUMB,
            VIRTUAL_BUTTON_COUNT,
            VIRTUAL_BUTTON_COUNT,
        };

        for (u32 i = 0; i < count; ++i)
        {
            if (i < ITF_ARRAY_SIZE(joyToVirtualMapping))
            {
                VirtualButton virtualButton = joyToVirtualMapping[i];
                if (virtualButton < VIRTUAL_BUTTON_COUNT)
                {
                    buttons[i] = m_virtualState.GetButton(player, virtualButton);
                }
                else
                {
                    buttons[i] = InputAdapter::Released;
                }
            }
            else
            {
                buttons[i] = InputAdapter::Released;
            }
        }
    }

    void InputManager::getGamePadPos(u32 player, float* axes, u32 numAxes) const
    {
        if (!axes || player >= JOY_MAX_COUNT)
            return;
        
        const u32 count = (numAxes < JOY_MAX_AXES) ? numAxes : JOY_MAX_AXES;
        
        static const VirtualAxis joyToVirtualAxisMapping[JOY_MAX_AXES] = {
            VIRTUAL_AXIS_LEFT_STICK_X,
            VIRTUAL_AXIS_LEFT_STICK_Y,
            VIRTUAL_AXIS_LEFT_TRIGGER,
            VIRTUAL_AXIS_RIGHT_STICK_X,
            VIRTUAL_AXIS_RIGHT_STICK_Y,
            VIRTUAL_AXIS_RIGHT_TRIGGER,
        };

        for (u32 i = 0; i < count; ++i)
        {
            if (i < ITF_ARRAY_SIZE(joyToVirtualAxisMapping))
            {
                VirtualAxis virtualAxis = joyToVirtualAxisMapping[i];
                if (virtualAxis < VIRTUAL_AXIS_COUNT)
                {
                    axes[i] = m_virtualState.GetAxis(player, virtualAxis);
                }
                else
                {
                    axes[i] = 0.0f;
                }
            }
            else
            {
                axes[i] = 0.0f;
            }
        }
    }

    void InputManager::InitializeDefaultMappings()
    {
        InitializeDefaultKeyboardMappings(m_inputMapping);
        InitializeDefaultActionMappings(m_actionMapping);
    }

    void InputManager::UpdateInputSources()
    {
        for (IInputSource* source : m_inputSourcePtrs)
        {
            if (source && source->IsConnected())
            {
                source->Update();
            }
        }
    }

    void InputManager::UpdateVirtualState()
    {
        m_virtualState.Reset();
        m_virtualState.UpdateFromPhysicalInputs(m_inputSourcePtrs, m_inputMapping);
    }

} // namespace ITF

