#ifndef _ITF_INPUT_MANAGER_H_
#define _ITF_INPUT_MANAGER_H_

#ifndef _ITF_TEMPLATESINGLETON_H_
#include "core/templateSingleton.h"
#endif //_ITF_TEMPLATESINGLETON_H_

#ifndef _ITF_INPUT_TYPES_H_
#include "engine/input/InputTypes.h"
#endif //_ITF_INPUT_TYPES_H_

#ifndef _ITF_INPUTADAPTER_H_
#include "engine/AdaptersInterfaces/InputAdapter.h"
#endif //_ITF_INPUTADAPTER_H_

#ifndef _ITF_IINPUT_SOURCE_H_
#include "engine/input/IInputSource.h"
#endif //_ITF_IINPUT_SOURCE_H_

#ifndef _ITF_INPUT_MAPPING_H_
#include "engine/input/InputMapping.h"
#endif //_ITF_INPUT_MAPPING_H_

#ifndef _ITF_ACTION_MAPPING_H_
#include "engine/input/ActionMapping.h"
#endif //_ITF_ACTION_MAPPING_H_

#ifndef _ITF_VIRTUAL_INPUT_STATE_H_
#include "engine/input/VirtualInputState.h"
#endif //_ITF_VIRTUAL_INPUT_STATE_H_

#ifndef _ITF_CORETYPES_H_
#include "core/types.h"
#endif //_ITF_CORETYPES_H_

#include <vector>
#include <memory>

namespace ITF
{
    class InputManager : public TemplateSingleton<InputManager>
    {
    public:
        InputManager();
        ~InputManager();

        void Initialize();
        void Update();
        void Shutdown();

        void RegisterInputSource(std::unique_ptr<IInputSource> source);
        IInputSource* GetInputSource(u32 deviceId) const;
        const std::vector<IInputSource*>& GetAllInputSources() const { return m_inputSourcePtrs; }

        void SetRemap(u32 player, const PhysicalInput& from, const VirtualInput& to);
        void ClearRemap(u32 player, const VirtualInput& virtualInput);
        void ResetRemap(u32 player);
        void ResetAllRemaps();
        bool GetRemappedInput(u32 player, const PhysicalInput& physical, VirtualInput& virtualInput) const;

        void getGamePadButtons(u32 player, InputAdapter::PressStatus* buttons, u32 numButtons) const;
        void getGamePadPos(u32 player, float* axes, u32 numAxes) const;

        InputMapping& GetInputMapping() { return m_inputMapping; }
        const InputMapping& GetInputMapping() const { return m_inputMapping; }
        ActionMapping& GetActionMapping() { return m_actionMapping; }
        const ActionMapping& GetActionMapping() const { return m_actionMapping; }
        const VirtualInputState& GetVirtualInputState() const { return m_virtualState; }
        bool IsInitialized() const { return m_initialized; }

    private:
        std::vector<std::unique_ptr<IInputSource>> m_inputSources;
        std::vector<IInputSource*> m_inputSourcePtrs;

        InputMapping m_inputMapping;
        InputMapping m_remapOverrides[JOY_MAX_COUNT];
        ActionMapping m_actionMapping;

        VirtualInputState m_virtualState;

        bool m_initialized;
        void InitializeDefaultMappings();
        void UpdateInputSources();
        void UpdateVirtualState();
    };

#define INPUT_MANAGER InputManager::getptr()

} // namespace ITF

#endif //_ITF_INPUT_MANAGER_H_

