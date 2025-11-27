#include "precompiled_engine.h"

#ifndef _ITF_INPUTDEVICE_H_
#include "engine/zinput/ZInputDevice.h"
#endif //_ITF_INPUTDEVICE_H_

#ifndef _ITF_INPUTUTILS_H_
#include "engine/zinput/ZInputUtils.h"
#endif //_ITF_INPUTUTILS_H_

namespace ITF
{

    void IInputDevice::ImplProcessActions( ActionMap& actionMap )
    {
        if(!IsDeviceValid())  return;

        // ProcessActions
        for ( u32 i = 0; i < actionMap.size(); i++ )
        {
            ZAction& action = actionMap[i];

            for ( size_t j = 0; j < action.m_inputs.size(); j++ )
            {
                ZInput& input = action.m_inputs[j];
                ITF_ASSERT(!input.m_match[m_id]);
                if ( !input.m_match[m_id] )
                {
                    //Need a fix
                    //if (input.m_translatedControl == U32_INVALID)
                        input.m_translatedControl = TranslateControl(input.m_control,m_controlMap);
                    
                    u32 control = input.m_translatedControl;
                    if ( control != U32_INVALID )
                    {
						SInputInfo &inputInfo = m_deviceInfo.m_inputInfo[control];
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

            if ( info.m_type == SInputInfo::INPUTTYPE_AXIS )
            {
                InputUtils::ProcessAxis(info,newInfo);
            }
            else if ( info.m_type == SInputInfo::INPUTTYPE_BUTTON )
            {
                InputUtils::ProcessButton(info,newInfo);
            }
            else if ( info.m_type == SInputInfo::INPUTTYPE_ACCELEROMETER )
            {
                InputUtils::ProcessAccelerometer(info, newInfo);
            }
        }
    }

    void IInputDevice::ProcessActions( ActionMap * actionMap )
    {
        ImplProcessActions((*actionMap));
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
        if (logicalControl < m_controlRemap.size())
        {
            m_controlRemap[logicalControl] = physicalControl;
        }
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

} //namespace ITF
