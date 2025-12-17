#include "precompiled_engine.h"

#ifndef _ITF_INPUTUTILS_H_
#include "engine/zinput/ZInputUtils.h"
#endif //_ITF_INPUTUTILS_H_

namespace ITF
{
    struct STranslateQuery
    {
        SInputInfo::EInputQuery m_query;
        const char* m_string;
    };

    SInputInfo::EInputQuery InputUtils::TranslateQuery( const StringID & query )
    {
        if( query == ZINPUT_QUERY_AXIS )
            return SInputInfo::QUERY_AXIS; 
        if ( query == ZINPUT_QUERY_HOLD )
            return SInputInfo::QUERY_HOLD;
        if ( query == ZINPUT_QUERY_PRESS )
            return SInputInfo::QUERY_PRESS;
        if ( query == ZINPUT_QUERY_RELEASE )
            return SInputInfo::QUERY_RELEASE;
        if ( query == ZINPUT_QUERY_ACC )
            return SInputInfo::QUERY_ACC;

        //not found
        return SInputInfo::QUERY_MAX;
    }

    void InputUtils::ProcessAxis( SInputInfo& info, const SInputInfo& newInfo )
    {
        if ( !info.m_axisInfo.m_axis && newInfo.m_axisInfo.m_axis )
        {
            info.m_axisInfo.m_status = SInputInfo::BUTTONSTATUS_PRESS;
            info.m_pressTime = SYSTEM_ADAPTER->getTime();
            info.m_dirty = true;
        }
        else if ( info.m_axisInfo.m_axis && !newInfo.m_axisInfo.m_axis )
        {
            info.m_axisInfo.m_status = SInputInfo::BUTTONSTATUS_NONE;
            info.m_dirty = true;
        }
        else if ( info.m_axisInfo.m_axis && newInfo.m_axisInfo.m_axis )
        {
            info.m_axisInfo.m_status = SInputInfo::BUTTONSTATUS_HOLD;
            info.m_dirty = true;
        }

        if ( info.m_axisInfo.m_axis != newInfo.m_axisInfo.m_axis )
        {
            info.m_axisInfo.m_axis = newInfo.m_axisInfo.m_axis;
            info.m_dirty = true;
        }
    }

    void InputUtils::ProcessButton( SInputInfo& info, const SInputInfo& newInfo )
    {
        if ( info.m_buttonInfo.m_status == SInputInfo::BUTTONSTATUS_NONE && newInfo.m_buttonInfo.m_status == SInputInfo::BUTTONSTATUS_PRESS )
        {
            info.m_buttonInfo.m_status = SInputInfo::BUTTONSTATUS_PRESS;
            info.m_pressTime = SYSTEM_ADAPTER->getTime();
            info.m_dirty = true;
        }
        else if ( ( info.m_buttonInfo.m_status == SInputInfo::BUTTONSTATUS_PRESS ||
                    info.m_buttonInfo.m_status == SInputInfo::BUTTONSTATUS_HOLD ) && newInfo.m_buttonInfo.m_status == SInputInfo::BUTTONSTATUS_NONE )
        {
            info.m_buttonInfo.m_status = SInputInfo::BUTTONSTATUS_NONE;
            info.m_dirty = true;
        }
        else if ( info.m_buttonInfo.m_status == SInputInfo::BUTTONSTATUS_PRESS && newInfo.m_buttonInfo.m_status == SInputInfo::BUTTONSTATUS_PRESS )
        {
            info.m_buttonInfo.m_status = SInputInfo::BUTTONSTATUS_HOLD;
            info.m_dirty = true;
        }
    }

    void InputUtils::ProcessAccelerometer( SInputInfo& info, const SInputInfo& newInfo )
    {
        info.m_accelerometerInfo.m_acc = newInfo.m_accelerometerInfo.m_acc;
        info.m_accelerometerInfo.m_prevAcc = newInfo.m_accelerometerInfo.m_prevAcc;
    }

    void InputUtils::UpdateActionInput( ZInput& input, const struct SInputInfo& info, u32 deviceID )
    {
        // If the input is an axis, we do not want to set the flag as TRUE, 
        // when the value of joystick is too low
        if ( info.m_type == SInputInfo::INPUTTYPE_AXIS && input.m_axisRange != Vec2d::Zero )
        {
            if ( info.m_axisInfo.m_axis <= input.m_axisRange.m_x
                || info.m_axisInfo.m_axis >= input.m_axisRange.m_y) return; 
        }        

        //Need a fix
        //if(input.m_translatedQuery == SInputInfo::QUERY_UNSET)
            input.m_translatedQuery = TranslateQuery(input.m_query);

        if ( input.m_translatedQuery == SInputInfo::QUERY_AXIS )
        {
            if ( info.m_type == SInputInfo::INPUTTYPE_AXIS )
            {
                if ( !info.m_axisInfo.m_axis &&
                     !info.m_dirty )
                    return;

                input.m_match[deviceID] = true;
            }
            else
            {
                if ( info.m_buttonInfo.m_status == SInputInfo::BUTTONSTATUS_NONE &&
                     !info.m_dirty )
                    return;

                input.m_match[deviceID] = true;
            }
        }
        else if ( input.m_translatedQuery == SInputInfo::QUERY_ACC )
        {
            f32 acc     = info.m_accelerometerInfo.m_acc.y;
            f32 pacc    = info.m_accelerometerInfo.m_prevAcc.y;
            bbool isSameSign =  ( acc < 0 && input.m_threshold < 0 ) || ( acc > 0 && input.m_threshold > 0 );

            if ( isSameSign && f32_Abs(acc) > f32_Abs(input.m_threshold) ) 
            {
                // punch
                if ( acc - pacc  >  0  && 
                    input.m_timer[deviceID] + input.m_delay < SYSTEM_ADAPTER->getTime() ) 
                {
                    input.m_timer[deviceID] = SYSTEM_ADAPTER->getTime();
                    input.m_match[deviceID] = true;
                }
            }
        }
        else
        {
            SInputInfo::EButtonStatus status;

            if ( info.m_type == SInputInfo::INPUTTYPE_AXIS )
                status = info.m_axisInfo.m_status;
            else
                status = info.m_buttonInfo.m_status;

            switch(input.m_translatedQuery)
            {
                case SInputInfo::QUERY_PRESS:
                    input.m_match[deviceID] = status == SInputInfo::BUTTONSTATUS_PRESS;
                    break;
                case SInputInfo::QUERY_RELEASE:
                    input.m_match[deviceID] = ( status == SInputInfo::BUTTONSTATUS_NONE ) && info.m_dirty;
                    break;
                case SInputInfo::QUERY_HOLD:
                    input.m_match[deviceID] = status == SInputInfo::BUTTONSTATUS_HOLD;
                    break;
                default:
                    break;
            }
        }
    }
};

