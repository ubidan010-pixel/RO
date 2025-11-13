#ifndef _ITF_VIRTUAL_INPUT_STATE_H_
#define _ITF_VIRTUAL_INPUT_STATE_H_

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

#ifndef _ITF_CORETYPES_H_
#include "core/types.h"
#endif //_ITF_CORETYPES_H_

#include <vector>

namespace ITF
{
    class VirtualInputState
    {
    public:
        VirtualInputState();
        ~VirtualInputState() = default;
        void Reset();
        void UpdateFromPhysicalInputs(const std::vector<IInputSource*>& sources, const InputMapping& mapping);
        InputAdapter::PressStatus GetButton(u32 player, VirtualButton button) const;
        f32 GetAxis(u32 player, VirtualAxis axis) const;
        void GetButtons(u32 player, InputAdapter::PressStatus* buttons, u32 numButtons) const;
        void GetAxes(u32 player, f32* axes, u32 numAxes) const;

    private:
        InputAdapter::PressStatus m_virtualButtons[JOY_MAX_COUNT][VIRTUAL_BUTTON_COUNT];
        f32 m_virtualAxes[JOY_MAX_COUNT][VIRTUAL_AXIS_COUNT];
        void UpdateButtonFromPhysicalInput(const PhysicalInput& physical, const VirtualInput& virtualInput, IInputSource* source);
        void UpdateAxisFromPhysicalInput(const PhysicalInput& physical, const VirtualInput& virtualInput, IInputSource* source);
        InputAdapter::PressStatus CombinePressStatus(InputAdapter::PressStatus current, InputAdapter::PressStatus newStatus) const;
    };

} // namespace ITF

#endif //_ITF_VIRTUAL_INPUT_STATE_H_

