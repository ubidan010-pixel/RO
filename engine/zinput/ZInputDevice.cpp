#include "precompiled_engine.h"

#ifndef _ITF_INPUTDEVICE_H_
#include "engine/zinput/ZInputDevice.h"
#endif //_ITF_INPUTDEVICE_H_

#ifndef _ITF_INPUTUTILS_H_
#include "engine/zinput/ZInputUtils.h"
#endif //_ITF_INPUTUTILS_H_

#ifndef _ITF_INPUTADAPTER_H_
#include "engine/AdaptersInterfaces/InputAdapter.h"
#endif //_ITF_INPUTADAPTER_H_

#ifndef _ITF_ZPAD_BASE_H_
#include "engine/zinput/ZPad_Base.h"
#endif //_ITF_ZPAD_BASE_H_

#ifndef _ITF_SYSTEMADAPTER_
#include "core/AdaptersInterfaces/SystemAdapter.h"
#endif //_ITF_SYSTEMADAPTER_

namespace ITF
{
    namespace
    {
        struct ButtonToControlMapping
        {
            u32 buttonIndex;
            u32 controlIndex;
        };

        struct AxisToControlMapping
        {
            u32 axisIndex;
            f32 threshold;
            bbool positive;
            u32 controlIndex;
        };

        static const ButtonToControlMapping kButtonMappings[] = {
            { m_joyButton_DPadU,      ZPad_Base::DPAD_UP },
            { m_joyButton_DPadD,      ZPad_Base::DPAD_DOWN },
            { m_joyButton_DPadL,      ZPad_Base::DPAD_LEFT },
            { m_joyButton_DPadR,      ZPad_Base::DPAD_RIGHT },
            { m_joyButton_A,          ZPad_Base::BUTTON_FACE_SOUTH },
            { m_joyButton_B,          ZPad_Base::BUTTON_FACE_EAST },
            { m_joyButton_X,          ZPad_Base::BUTTON_FACE_WEST },
            { m_joyButton_Y,          ZPad_Base::BUTTON_FACE_NORTH },
            { m_joyButton_LB,         ZPad_Base::BUTTON_L_SHOULDER },
            { m_joyButton_RB,         ZPad_Base::BUTTON_R_SHOULDER },
            { m_joyButton_ThumbLeft,  ZPad_Base::BUTTON_L_THUMB },
            { m_joyButton_ThumbRight, ZPad_Base::BUTTON_R_THUMB },
            { m_joyButton_Start,      ZPad_Base::BUTTON_START },
            { m_joyButton_Back,       ZPad_Base::BUTTON_SELECT },
        };

        static const AxisToControlMapping kAxisMappings[] = {
            { m_joyTrigger_Left,   0.5f, btrue,  ZPad_Base::TRIGGER_LEFT },
            { m_joyTrigger_Right,  0.5f, btrue,  ZPad_Base::TRIGGER_RIGHT },
            { m_joyStickLeft_Y,    0.5f, btrue,  ZPad_Base::STICK_L_UP },
            { m_joyStickLeft_Y,    0.5f, bfalse, ZPad_Base::STICK_L_DOWN },
            { m_joyStickLeft_X,    0.5f, bfalse, ZPad_Base::STICK_L_LEFT },
            { m_joyStickLeft_X,    0.5f, btrue,  ZPad_Base::STICK_L_RIGHT },
            { m_joyStickRight_Y,   0.5f, btrue,  ZPad_Base::STICK_R_UP },
            { m_joyStickRight_Y,   0.5f, bfalse, ZPad_Base::STICK_R_DOWN },
            { m_joyStickRight_X,   0.5f, bfalse, ZPad_Base::STICK_R_LEFT },
            { m_joyStickRight_X,   0.5f, btrue,  ZPad_Base::STICK_R_RIGHT },
        };

        ITF_INLINE bbool IsButtonActive(InputAdapter::PressStatus status)
        {
            return status == InputAdapter::Pressed || status == InputAdapter::JustPressed;
        }

        ITF_INLINE bbool IsAxisActive(f32 value, f32 threshold, bbool positive)
        {
            return positive ? (value > threshold) : (value < -threshold);
        }
    }

    void IInputDevice::ImplProcessActions( ActionMap& actionMap )
    {
        ImplProcessActionsWithCategory(actionMap, InputCategory_Gameplay);
    }

    namespace
    {
        ITF_INLINE bbool ShouldUseRawInput(const StringID& actionName)
        {
            if (actionName == ITF_GET_STRINGID_CRC(WM_ENTER, 2267322818) ||
                actionName == ITF_GET_STRINGID_CRC(WM_BACK, 4244387740) ||
                actionName == ITF_GET_STRINGID_CRC(WM_HOME, 1806382459) ||
                actionName == ITF_GET_STRINGID_CRC(MENU_VALID, 1084313942) ||
                actionName == ITF_GET_STRINGID_CRC(MENU_BACK, 2477582220))
            {
                return btrue;
            }

            const char* name = actionName.getDebugString();
            if (!name) return bfalse;
            if (name[0] == 'W' && name[1] == 'M' && name[2] == '_')
                return btrue;
            if (name[0] == 'M' && name[1] == 'E' && name[2] == 'N' && name[3] == 'U' && name[4] == '_')
                return btrue;
            return bfalse;
        }

        // Swap SOUTH and EAST buttons when platform requires it (NX/Ounce/Japan PS5)
        ITF_INLINE u32 ApplyButtonSwapIfNeeded(u32 control)
        {
            if (SYSTEM_ADAPTER && SYSTEM_ADAPTER->isBackAndSelectButtonsInverted())
            {
                if (control == ZPad_Base::BUTTON_FACE_SOUTH)
                    return ZPad_Base::BUTTON_FACE_EAST;
                if (control == ZPad_Base::BUTTON_FACE_EAST)
                    return ZPad_Base::BUTTON_FACE_SOUTH;
            }
            return control;
        }
    }

    void IInputDevice::ImplProcessActionsWithCategory( ActionMap& actionMap, EInputCategory category )
    {
        if(!IsDeviceValid())  return;

        // ProcessActions
        for ( u32 i = 0; i < actionMap.size(); i++ )
        {
            ZAction& action = actionMap[i];
            bbool useRawInput = (category == InputCategory_Menu) || ShouldUseRawInput(action.m_action);
            SDeviceInfo& deviceInfoToUse = useRawInput ? m_deviceInfoRaw : m_deviceInfo;

            for ( size_t j = 0; j < action.m_inputs.size(); j++ )
            {
                ZInput& input = action.m_inputs[j];
                if ( !input.m_match[m_id] )
                {
                    //Need a fix
                    //if (input.m_translatedControl == U32_INVALID)
                    input.m_translatedControl = TranslateControl(input.m_control,m_controlMap);

                    u32 control = input.m_translatedControl;
                    // Apply button swap for platforms that need SOUTH/EAST swapped (NX/Ounce/Japan PS5)
                    control = ApplyButtonSwapIfNeeded(control);
                    
                    if ( control != U32_INVALID && control < deviceInfoToUse.m_inputInfo.size() )
                    {
						SInputInfo &inputInfo = deviceInfoToUse.m_inputInfo[control];
                        InputUtils::UpdateActionInput(input,inputInfo, m_id);

                        if ( input.m_match[m_id] )
                        {
							SInputInfo::EInputType inputType = inputInfo.m_type;
                            if ( inputType == SInputInfo::INPUTTYPE_AXIS )
                            {
                                action.m_axis[m_id] = inputInfo.m_axisInfo.m_axis;
                            }
                            else if ( inputType == SInputInfo::INPUTTYPE_BUTTON )
                            {
                                if ( inputInfo.m_buttonInfo.m_status != SInputInfo::BUTTONSTATUS_NONE )
                                {
                                    action.m_axis[m_id] = 1.f;
                                }
                            }
                            else if ( inputType == SInputInfo::INPUTTYPE_ACCELEROMETER )
                            {
                                action.m_axis[m_id] = 1.f;
                            }

                            // Apply options
                            if(action.m_axis[m_id])
                            {
                                if ( action.m_inverted )
                                    action.m_axis[m_id] *= -1.f;
                                if ( action.m_scale )
                                    action.m_axis[m_id] *= action.m_scale;
                            }

                            action.m_matchs[m_id]++;
                        }
                    }
                }
            }
        }
    }

    void IInputDevice::ImplUpdate()
    {
        if(!IsDeviceValid())  return;

        u32 maxControls = m_deviceInfo.m_inputInfo.size();
        static SDeviceInfo newDeviceInfo;
        newDeviceInfo.m_inputInfo.resize( Max(maxControls, (u32)newDeviceInfo.m_inputInfo.size()) );

        ITF_MemSet(&newDeviceInfo.m_inputInfo[0],0, sizeof(SInputInfo)*maxControls);
        UpdateDeviceInfo(newDeviceInfo);

        for ( u32 i = 0; i < maxControls; i++ )
        {
            SInputInfo& info = m_deviceInfo.m_inputInfo[i];
            const SInputInfo& newInfo = newDeviceInfo.m_inputInfo[i];

            info.m_dirty = false;
            info.m_type = newInfo.m_type;

            if ( newInfo.m_type == SInputInfo::INPUTTYPE_AXIS )
            {
                InputUtils::ProcessAxis(info,newInfo);
            }
            else if ( newInfo.m_type == SInputInfo::INPUTTYPE_BUTTON )
            {
                InputUtils::ProcessButton(info,newInfo);
            }
            else if ( newInfo.m_type == SInputInfo::INPUTTYPE_ACCELEROMETER )
            {
                InputUtils::ProcessAccelerometer(info, newInfo);
            }
        }
        u32 maxControlsRaw = m_deviceInfoRaw.m_inputInfo.size();
        if (maxControlsRaw > 0)
        {
            static SDeviceInfo newDeviceInfoRaw;
            newDeviceInfoRaw.m_inputInfo.resize( Max(maxControlsRaw, (u32)newDeviceInfoRaw.m_inputInfo.size()) );

            ITF_MemSet(&newDeviceInfoRaw.m_inputInfo[0], 0, sizeof(SInputInfo)*maxControlsRaw);
            UpdateDeviceInfoRaw(newDeviceInfoRaw);

            for ( u32 i = 0; i < maxControlsRaw; i++ )
            {
                SInputInfo& info = m_deviceInfoRaw.m_inputInfo[i];
                const SInputInfo& newInfo = newDeviceInfoRaw.m_inputInfo[i];

                info.m_dirty = false;
                info.m_type = newInfo.m_type;

                if ( newInfo.m_type == SInputInfo::INPUTTYPE_AXIS )
                {
                    InputUtils::ProcessAxis(info,newInfo);
                }
                else if ( newInfo.m_type == SInputInfo::INPUTTYPE_BUTTON )
                {
                    InputUtils::ProcessButton(info,newInfo);
                }
                else if ( newInfo.m_type == SInputInfo::INPUTTYPE_ACCELEROMETER )
                {
                    InputUtils::ProcessAccelerometer(info, newInfo);
                }
            }
        }
    }

    void IInputDevice::ProcessActions( ActionMap * actionMap )
    {
        ImplProcessActions((*actionMap));
    }

    void IInputDevice::ProcessActionsWithCategory( ActionMap * actionMap, EInputCategory category )
    {
        ImplProcessActionsWithCategory((*actionMap), category);
    }

    void IInputDevice::Update()
    {
        if(!m_initialized)
        {
            InitDeviceInfo();
            m_initialized = btrue;
        }

        ImplUpdate();
    }

    u32 IInputDevice::TranslateControl( const StringID& control, const ControlTranslateMap& map )
    {
        ControlTranslateMap::const_iterator it = map.find(control);

        if ( it != map.end() )
            return it->second;
        else
            return U32_INVALID;
    }

    void IInputDevice::SetRemap(u32 logicalControl, u32 physicalControl)
    {
        if (logicalControl >= m_controlRemap.size()) return;
        u32 oldPhysical = m_controlRemap[logicalControl];
        for (u32 i = 0; i < m_controlRemap.size(); ++i)
        {
            if (i != logicalControl && m_controlRemap[i] == physicalControl)
            {
                m_controlRemap[i] = oldPhysical;
                break;
            }
        }
        m_controlRemap[logicalControl] = physicalControl;
    }

    void IInputDevice::ResetRemapping()
    {
        for (u32 i = 0; i < m_controlRemap.size(); ++i)
        {
            m_controlRemap[i] = i;
        }
    }

    void IInputDevice::ApplyRemapping(SDeviceInfo& deviceInfo)
    {
        if (m_controlRemap.empty()) return;
        ITF_VECTOR<SInputInfo> sourceInput = deviceInfo.m_inputInfo;

        u32 count = Min((u32)m_controlRemap.size(), (u32)deviceInfo.m_inputInfo.size());
        for (u32 i = 0; i < count; ++i)
        {
            u32 sourceIndex = m_controlRemap[i];
            if (sourceIndex < sourceInput.size() && sourceIndex != i)
            {
                deviceInfo.m_inputInfo[i] = sourceInput[sourceIndex];
            }
        }
    }

    u32 IInputDevice::GetRemap(u32 logicalControl) const
    {
        if (logicalControl < m_controlRemap.size())
        {
            return m_controlRemap[logicalControl];
        }
        return logicalControl;
    }

    u32 IInputDevice::GetFirstActiveControl() const
    {
        if (!IsDeviceValid()) return U32_INVALID;

        InputAdapter::PressStatus buttons[JOY_MAX_BUT];
        INPUT_ADAPTER->getGamePadButtons(InputAdapter::EnvironmentAll, m_id, buttons, JOY_MAX_BUT);

        f32 axes[JOY_MAX_AXES];
        INPUT_ADAPTER->getGamePadPos(InputAdapter::EnvironmentAll, m_id, axes, JOY_MAX_AXES);
        for (const auto& mapping : kButtonMappings)
        {
            if (IsButtonActive(buttons[mapping.buttonIndex]))
            {
                u32 control = ApplyButtonSwapIfNeeded(mapping.controlIndex);
                return control;
            }
        }
        for (const auto& mapping : kAxisMappings)
        {
            if (IsAxisActive(axes[mapping.axisIndex], mapping.threshold, mapping.positive))
            {
                return mapping.controlIndex;
            }
        }

        return U32_INVALID;
    }

} //namespace ITF
