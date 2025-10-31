#include "precompiled_inputadapter_x360.h"

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_INPUTADAPTER_X360_H_
#include "adapters/InputAdapter_x360/InputAdapter_x360.h"
#endif //_ITF_INPUTADAPTER_X360_H_

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif //_ITF_ERRORHANDLER_H_

#ifndef _ITF_SYSTEMADAPTER_
#include "core/AdaptersInterfaces/SystemAdapter.h"
#endif //_ITF_SYSTEMADAPTER_


#ifndef ITF_FINAL
#include <xbdm.h>
#endif //ITF_FINAL

namespace ITF
{
    CONTROL_DESCRIPTION g_ControlDescGamepad[] = 
    {
        XINPUTID_DPAD_UP,			L"DPAD_UP",		XINPUT2_TYPE_BOOL,m_joyButton_DPadU,
        XINPUTID_DPAD_DOWN,			L"DPAD_DOWN",	XINPUT2_TYPE_BOOL,m_joyButton_DPadD,
        XINPUTID_DPAD_LEFT,			L"DPAD_LEFT",	XINPUT2_TYPE_BOOL,m_joyButton_DPadL,
        XINPUTID_DPAD_RIGHT,		L"DPAD_RIGHT",	XINPUT2_TYPE_BOOL,m_joyButton_DPadR,
        XINPUTID_START,				L"START",		XINPUT2_TYPE_BOOL,m_joyButton_Start,
        XINPUTID_BACK,				L"BACK",		XINPUT2_TYPE_BOOL,m_joyButton_Back,
        XINPUTID_LEFT_THUMB,		L"L_THUMB",		XINPUT2_TYPE_BOOL,m_joyButton_ThumbLeft,
        XINPUTID_RIGHT_THUMB,		L"R_THUMB",		XINPUT2_TYPE_BOOL,m_joyButton_ThumbRight,
        XINPUTID_LEFT_SHOULDER,		L"L_SHOULDER",	XINPUT2_TYPE_BOOL,m_joyButton_LB,
        XINPUTID_RIGHT_SHOULDER,	L"R_SHOULDER",	XINPUT2_TYPE_BOOL,m_joyButton_RB,
        XINPUTID_A,					L"A",			XINPUT2_TYPE_BOOL,m_joyButton_A,
        XINPUTID_B,					L"B",			XINPUT2_TYPE_BOOL,m_joyButton_B,
        XINPUTID_X,					L"X",			XINPUT2_TYPE_BOOL,m_joyButton_X,
        XINPUTID_Y,					L"Y",			XINPUT2_TYPE_BOOL,m_joyButton_Y,
        XINPUTID_LEFT_TRIGGER,		L"L_TRIGGER",	XINPUT2_TYPE_FLOAT,m_joyTrigger_Left,
        XINPUTID_RIGHT_TRIGGER,		L"R_TRIGGER",	XINPUT2_TYPE_FLOAT,m_joyTrigger_Right,
        XINPUTID_BUTTONS,			L"BUTTONS",		XINPUT2_TYPE_DWORD,U32_INVALID,
        XINPUTID_LEFT_THUMBSTICK,	L"L_STICK",		XINPUT2_TYPE_POINT,m_joyStick_Left,
        XINPUTID_RIGHT_THUMBSTICK,	L"R_STICK",     XINPUT2_TYPE_POINT,m_joyStick_Right,
    };

    const f32 INPUT_DEADZONE_LEFT = XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE/32767.0f;
    const DWORD g_dwNumDescGamepad = ARRAYSIZE( g_ControlDescGamepad );

    Joy::Joy()
    {
        m_id = -1;
        ZeroMemory(&m_axes, sizeof(float) * MAX_AXES);
        ZeroMemory(&m_buttons,sizeof(InputAdapter::PressStatus) * JOY_MAX_BUT);
        m_eInputDeviceState = EINPUTDEVICE_NOTCONNECTED;

        ITF_MemSet(&m_rumbleState,0,sizeof(RumbleState));
        ITF_MemSet( m_bControlDataBlock, 0x00, sizeof(m_bControlDataBlock) );

        m_DeviceID = XINPUTID_GAMEPAD;
 
    }

    Joy::~Joy()
    {
    }

    void    Joy::mapButtons(const CONTROL_DESCRIPTION& controlDesc,bbool boolValue,f32 floatValue,XINPUT2_POINT* pointValue)
    {
        if (controlDesc.dwDataType == XINPUT2_TYPE_FLOAT)
        {
            switch (controlDesc.mappingValue)
            {
                case m_joyTrigger_Left:
                case m_joyTrigger_Right:
                    m_axes[controlDesc.mappingValue] =  floatValue;
                    break;
                default:
                    ITF_ASSERT(0);
                    break;
            }
        }
        else
        {
            switch (controlDesc.mappingValue)
            {
                case m_joyButton_DPadU:
                case m_joyButton_DPadD:
                case m_joyButton_DPadL:
                case m_joyButton_DPadR:
                case m_joyButton_A:
                case m_joyButton_B:
                case m_joyButton_X:
                case m_joyButton_Y:
                case m_joyButton_LB:
                case m_joyButton_RB:
                case m_joyButton_Back:
                case m_joyButton_Start:
                case m_joyButton_ThumbLeft:
                case m_joyButton_ThumbRight:
                    m_buttons[controlDesc.mappingValue] =  boolValue ? InputAdapter::Pressed :  InputAdapter::Released;
                break;

                case m_joyStick_Left:
                case m_joyStick_Right:
                    {
                        u32 index = (controlDesc.mappingValue == m_joyStick_Left) ? m_joyStickLeft_X : m_joyStickRight_X;
                        if( (*pointValue).x < INPUT_DEADZONE_LEFT    &&
                            (*pointValue).x > -INPUT_DEADZONE_LEFT   &&
                            (*pointValue).y < INPUT_DEADZONE_LEFT    &&
                            (*pointValue).y > -INPUT_DEADZONE_LEFT)
                        {
                            m_axes[index] = 0;
                            m_axes[index+1] = 0;
                        }
                        else
                        {

                            m_axes[index] = (*pointValue).x;
                            m_axes[index+1] = (*pointValue).y;
                        }
                    }
                    break;
                default:
                    break;
            }

        }
    }

    // The speed given is between 0-1
    // For 360 pads, the left motor handles low-frequency vibration and the right motor handles high-frequency vibration
    void Joy::padVibration   (f32 _leftMotorSpeed, f32 _rightMotorSpeed, f64 _time)
    {
        XINPUT_STATE state ;
        DWORD dwResult = XInputGetState(getId(), &state);
        if( dwResult == ERROR_SUCCESS )
        {
            bbool StartMotor = (_leftMotorSpeed + _leftMotorSpeed) > 0.0f;

            if(m_rumbleState.motorOn != StartMotor)
            {            
                m_rumbleState.motorOn = StartMotor;

                // MotorSpeed: 0-65535
                XINPUT_VIBRATION vibration;
                vibration.wRightMotorSpeed  = (WORD)(_leftMotorSpeed * 65535.0f);
                vibration.wLeftMotorSpeed   = (WORD)(_rightMotorSpeed * 65535.0f);

                XInputSetState(getId(), &vibration);
            }

            if(StartMotor)
            {
                m_rumbleState.duringTime    = _time;
                m_rumbleState.startTime     = SYSTEM_ADAPTER->getTime();  
            }            
        }
    }

    void Joy::UpdateInputState( const CONTROL_DESCRIPTION controlDesc[],const InputMappingTranslator& _mappingTranslator)
    {
        XINPUT2CONTEXT context;
        BOOL changed;

        ITF_MemSet( &context, 0, sizeof(context) );
        InputAdapter::PressStatus temp_buttons[JOY_MAX_BUT];

        ITF_Memcpy(temp_buttons,m_buttons,sizeof(temp_buttons));
 

        if( XInput2Sample( XInput2InputDevice(m_id), &context, &changed ) )
        {
            for( DWORD dwControl = 0; dwControl < g_dwNumDescGamepad; dwControl++ )
            {
                XINPUTID ID = _mappingTranslator.getTranslatedID(m_DeviceID,controlDesc[dwControl].ID);
            
                switch( controlDesc[dwControl].dwDataType )
                {
                case XINPUT2_TYPE_BOOL:
                    {
                        bbool bRead;
                        if( XInput2GetBool(context, ID, &bRead) ) 
                        {
                            mapButtons(controlDesc[dwControl],bRead,0.0f);

                        }

                    }
                    break;

                case XINPUT2_TYPE_DWORD:
                    {
                        DWORD dwRead;
                        if( XInput2GetDWord(context,ID, &dwRead) ) 
                        {
                        }

                    }
                    break;

                case XINPUT2_TYPE_FLOAT:
                    {
                        FLOAT fRead;
                        if( XInput2GetFloat(context, ID, &fRead) ) 
                        {
                            mapButtons(controlDesc[dwControl],0,fRead);
                        }

                    }
                    break;

                case XINPUT2_TYPE_POINT:
                    {
                        XINPUT2_POINT pointRead;
                        if( XInput2GetPoint(context, ID, &pointRead) ) 
                        {
                            pointRead = pointRead;

                            mapButtons(controlDesc[dwControl],bfalse,0.0f,&pointRead);
                        }

                    }
                    break;

                }
            }

        }

        f32 triggerThreshold = 0.5f;
        XInput2GetFloat(context, XINPUTID_TRIGGER_THRESHOLD, &triggerThreshold);

        m_buttons[m_joyButton_TriggerLeft] = (m_axes[m_joyTrigger_Left]>=triggerThreshold) ? InputAdapter::Pressed :  InputAdapter::Released;;
        m_buttons[m_joyButton_TriggerRight] = (m_axes[m_joyTrigger_Right]>=triggerThreshold) ? InputAdapter::Pressed :  InputAdapter::Released;;

        for (u32 i = 0; i < JOY_MAX_BUT; ++i)
        {
            if (temp_buttons[i] != InputAdapter::JustPressed && temp_buttons[i] != InputAdapter::Pressed && m_buttons[i] == InputAdapter::Pressed)
                m_buttons[i] = InputAdapter::JustPressed;
            else if (temp_buttons[i] != InputAdapter::JustReleased && temp_buttons[i] != InputAdapter::Released && m_buttons[i] == InputAdapter::Released)
                m_buttons[i] = InputAdapter::JustReleased;
        }


        // rumble
        if(m_rumbleState.motorOn)
        {
            if(m_rumbleState.startTime + m_rumbleState.duringTime < SYSTEM_ADAPTER->getTime())                
            {
                padVibration(0.0f, 0.0f, 0.0);
            }
        }
    }

    float Joy::getAxe      (u32 _axe)  const
    {
        ITF_ASSERT(_axe < MAX_AXES);
        return m_axes[_axe];
    }

    InputAdapter::PressStatus Joy::getButton   (u32 _button)   const
    {
        ITF_ASSERT(_button < JOY_MAX_BUT);
        return m_buttons[_button];
    }

    //----------------------------------------------------------------------------//

    /// CALLBACKS.
    void CALLBACK MousePosCB( i32 xPos, i32 yPos, void* pUserContext )
    {/*
        if (INPUT_ADAPTER)
            INPUT_ADAPTER->onMousePos(xPos, yPos);*/
    }

    void CALLBACK MouseWheelCB( i32 nMouseWheelDelta, void* pUserContext )
    {/*
        if (INPUT_ADAPTER)
            INPUT_ADAPTER->onMouseWheel(nMouseWheelDelta);*/
    }


    f64 DINPUT_lastLeftMouseClick = 0.;

    void CALLBACK MouseButtonCB( u32 _Button, u32 _action, void* pUserContext )
    {/*
        if (INPUT_ADAPTER)
        {
            InputAdapter::MouseButton but = (InputAdapter::MouseButton)_Button;
            InputAdapter::PressStatus status = (InputAdapter::PressStatus)_action;
            if (but == InputAdapter::MB_Left && status == InputAdapter::Pressed)
            {
                f64 thisTime = ELLASPEDTIME;
                f64 deltaTime = thisTime - DINPUT_lastLeftMouseClick;
                if (deltaTime >= 0. && deltaTime < 0.4)
                {
                    status = InputAdapter::Double_Press;
                }
                DINPUT_lastLeftMouseClick = thisTime;
            }
            INPUT_ADAPTER->onMouseButton(but, status);
        }*/
    }

    int DINPUT_Key_ToITF(int _key)
    {
        if (_key < 27) return _key;

        switch(_key)
        {
            case VK_CLEAR: return KEY_DEL; break;
            case VK_RETURN: return KEY_ENTER; break;
            case VK_SHIFT: 
                return KEY_SPECIAL; break;
            case VK_CONTROL: 
                return KEY_SPECIAL; break;

            case VK_LSHIFT: return KEY_LSHIFT; break;
            case VK_RSHIFT: return KEY_RSHIFT; break;
            case VK_LCONTROL: return KEY_LCTRL; break;
            case VK_RCONTROL: return KEY_RCTRL; break;
            case VK_LMENU: return KEY_LALT; break;
            case VK_RMENU: return KEY_RALT; break;

            case VK_ESCAPE: return KEY_ESC; break;

            case VK_SPACE: return KEY_SPACE; break;
            case VK_PRIOR: return KEY_PAGEDOWN; break;
            case VK_NEXT: return KEY_PAGEUP; break;
            case VK_END: return KEY_END; break;
            case VK_HOME: return KEY_HOME; break;
            case VK_LEFT: return KEY_LEFT; break;
            case VK_UP: return KEY_UP; break;
            case VK_RIGHT: return KEY_RIGHT; break;
            case VK_DOWN: return KEY_DOWN; break;
/* 
            case VK_SELECT: return KEY_SPECIAL; break;
            case VK_PRINT: return KEY_SPECIAL; break;
            case VK_EXECUTE: return KEY_SPECIAL; break;
            case VK_SNAPSHOT: return KEY_SPECIAL; break;
            case VK_INSERT: return KEY_SPECIAL; break;
            case VK_DELETE: return KEY_SPECIAL; break;
            case VK_HELP: return KEY_SPECIAL; break;

            case VK_LWIN: return KEY_SPECIAL; break;
            case VK_RWIN: return KEY_SPECIAL; break;
            case VK_APPS: return KEY_SPECIAL; break;

            case VK_SLEEP: return KEY_SPECIAL; break;
*/
            case VK_NUMPAD0: return KEY_KP_0; break;
            case VK_NUMPAD1: return KEY_KP_1; break;
            case VK_NUMPAD2: return KEY_KP_2; break;
            case VK_NUMPAD3: return KEY_KP_3; break;
            case VK_NUMPAD4: return KEY_KP_4; break;
            case VK_NUMPAD5: return KEY_KP_5; break;
            case VK_NUMPAD6: return KEY_KP_6; break;
            case VK_NUMPAD7: return KEY_KP_7; break;
            case VK_NUMPAD8: return KEY_KP_8; break;
            case VK_NUMPAD9: return KEY_KP_9; break;
            case VK_MULTIPLY: return KEY_KP_MULTIPLY; break;
            case VK_ADD: return KEY_KP_ADD; break;

            case VK_SEPARATOR: return KEY_KP_EQUAL; break;

            case VK_SUBTRACT: return KEY_KP_SUBTRACT; break;
            case VK_DECIMAL: return KEY_KP_DECIMAL; break;
            case VK_DIVIDE: return KEY_KP_DIVIDE; break;
            
            case VK_F1: return KEY_F1; break;
            case VK_F2: return KEY_F2; break;
            case VK_F3: return KEY_F3; break;
            case VK_F4: return KEY_F4; break;
            case VK_F5: return KEY_F5; break;
            case VK_F6: return KEY_F6; break;
            case VK_F7: return KEY_F7; break;
            case VK_F8: return KEY_F8; break;
            case VK_F9: return KEY_F9; break;
            case VK_F10: return KEY_F10; break;
            case VK_F11: return KEY_F11; break;
            case VK_F12: return KEY_F12; break;
            case VK_F13: return KEY_F13; break;
            case VK_F14: return KEY_F14; break;
            case VK_F15: return KEY_F15; break;
            case VK_F16: return KEY_F16; break;
            case VK_F17: return KEY_F17; break;
            case VK_F18: return KEY_F18; break;
            case VK_F19: return KEY_F19; break;
            case VK_F20: return KEY_F20; break;
            case VK_F21: return KEY_F21; break;
            case VK_F22: return KEY_F22; break;
            case VK_F23: return KEY_F23; break;
            case VK_F24: return KEY_F24; break;
        }

        return _key;
    }

#define BEGIN_MAPPING(_type)       deviceType = _type;
#define MAPPING(_key,_value)       m_mappingTranslator.add(deviceType,_key,_value);
#define END_MAPPING()

    /// Constructor.
    InputAdapter_x360::InputAdapter_x360()
    {
        m_InputX360.InitDirectInput();
    
        getGamePadCount();

        ITF_MemSet(&m_keyboardStatePrevious,0,sizeof(m_keyboardStatePrevious));

        XINPUTID deviceType;

        //build mapping here
        //seems only GUITAR is required because they match the abyx button
        //-> seems not ahah :)
        BEGIN_MAPPING(XINPUTID_GUITAR)
            MAPPING(XINPUTID_A,XINPUTID_NECK_GREEN)
            MAPPING(XINPUTID_B,XINPUTID_NECK_RED)
            MAPPING(XINPUTID_Y,XINPUTID_NECK_YELLOW)
            MAPPING(XINPUTID_X,XINPUTID_NECK_BLUE)
        END_MAPPING()

        BEGIN_MAPPING(XINPUTID_DANCEPAD)
            MAPPING(XINPUTID_A,XINPUTID_NORTHEAST)
            MAPPING(XINPUTID_B,XINPUTID_NORTHWEST)
            MAPPING(XINPUTID_Y,XINPUTID_SOUTHWEST)
            MAPPING(XINPUTID_X,XINPUTID_SOUTHEAST)
        END_MAPPING()

        BEGIN_MAPPING(XINPUTID_FLIGHT_STICK)
            MAPPING(XINPUTID_LEFT_TRIGGER, XINPUTID_FLIGHT_STICK)
            MAPPING(XINPUTID_DPAD_UP,XINPUTID_HAT_UP)
            MAPPING(XINPUTID_DPAD_DOWN,XINPUTID_HAT_DOWN)
            MAPPING(XINPUTID_DPAD_LEFT,XINPUTID_HAT_LEFT)
            MAPPING(XINPUTID_DPAD_RIGHT,XINPUTID_HAT_RIGHT)
        END_MAPPING()
    }

    /// Destructor.
    InputAdapter_x360::~InputAdapter_x360()
    {    
    }

    void InputAdapter_x360::getGamePadPos( u32 _environment,u32 _pad, float *_pos, u32 _numAxes ) const
    {
        const Joy* pjoy = &m_InputX360.m_pad[_pad];
 
        if ((m_environmentInput & _environment) != 0) //This environment is enable ,fill the buffer
        {
            for (u32 i = 0 ; i < _numAxes ; i++)
                _pos[i] = pjoy->getAxe(i);
        }
        else
        {
            for (u32 i = 0 ; i < _numAxes ; i++)
                _pos[i] = 0.0f;
        }
    }

    u32 InputAdapter_x360::getConnectedPadIndex (u32 _padIndex) const
    {
        u32 padIndex = 0;/*
        u32 activeCount = 0;
        for (; padIndex < maxPad; padIndex++)
        {
            if (isPadConnected(padIndex))
            {
                if (activeCount == _padIndex)
                    break;
                activeCount++;
            }
        }*/
        return padIndex;
    }

    void InputAdapter_x360::getGamePadButtons( u32 _environment ,u32 _pad, PressStatus* _buttons, u32 _numButtons ) const
    {
        ITF_ASSERT(_numButtons <= JOY_MAX_BUT); // just increase JOY_MAX_BUT if you assert here

        const Joy* pJoy = &m_InputX360.m_pad[_pad];

        if ((m_environmentInput & _environment) != 0) //This environment is enable ,fill the buffer
        {
            for (u32 i = 0; i < _numButtons; i++)
                _buttons[i] = pJoy->getButton(i);
        }
        else
        {
            for (u32 i = 0; i < _numButtons; i++)
                _buttons[i] = Released;
        }
    }

    u32 InputAdapter_x360::getGamePadCount()
    {
        u32 count = 0;

        for (u32 i = 0; i < m_InputX360.m_numberpad; i++)
        {
            Joy* joy = &m_InputX360.m_pad[i];
            if (joy->getDeviceState() == EINPUTDEVICE_CONNECTEDCOMPATIBLE)
                count++;
        }

        return count;
    }

    void InputAdapter_x360::startRumble( u32 _numPad, f64 _time, f32 _leftMotorSpeed, f32 _rightMotorSpeed )
    {
        ITF_ASSERT(_numPad < InputX360::MAX_PAD);

        Joy* pJoy = &m_InputX360.m_pad[_numPad];
        pJoy->padVibration(_leftMotorSpeed, _rightMotorSpeed, _time);    
    }

    void InputAdapter_x360::stopRumble(u32 _numPad)
    {
        ITF_ASSERT(_numPad < InputX360::MAX_PAD);

        Joy* pJoy = &m_InputX360.m_pad[_numPad];
        pJoy->padVibration(0.0f, 0.0f, 0.0);
    }

    void InputAdapter_x360::updateMouse()
    {/*
        UCHAR Buttons = 0xFF;
        SHORT x = 0xFFFF;
        SHORT y = 0xFFFF;
        SHORT Wheel = 0xFFFF;

        DWORD res = DmGetMouseChanges(&Buttons,&x,&y,&Wheel);

        static int m_lastMouseX = 1280/2;
        static int m_lastMouseY = 720/2;

        if (res == ERROR_SUCCESS)
        {
            res = ERROR_SUCCESS;
            char t[255];
            m_lastMouseX += x;
            m_lastMouseY += y;


            if (m_lastMouseX<0)
                m_lastMouseX=0;
            if (m_lastMouseY<0)
                m_lastMouseY = 0;

            if (m_lastMouseX>1279)
                m_lastMouseX = 1279;

                if (m_lastMouseY>719)
                    m_lastMouseY = 719;

            sprintf(t,"posD %d %d // %d %d\n",x,y,m_lastMouseX,m_lastMouseY);

            onMousePos(m_lastMouseX,m_lastMouseY);
            OutputDebugStringA(t);

        }*/
    }

    //----------------------------------------------------------------------------//


    void InputAdapter_x360::onKeyCB( u32 nChar, bbool bKeyDown)
    {
        LOG_GUILL_THE_WHITE("[onKeyCB] nChar: %d\t state: %d\t", nChar, bKeyDown);

        InputAdapter::PressStatus status = InputAdapter::Pressed;
        if (!bKeyDown) 
            status = InputAdapter::Released;

        onKey(DINPUT_Key_ToITF((i32)nChar), status);
    }

    void  InputAdapter_x360::updateKeyboard()
    {

#ifndef ITF_FINAL
        if (XInputGetKeyboardState(0 ,&m_keyboardState) == ERROR_SUCCESS)
        {
            bbool bLCtrl    = (XINPUT_MODIFIER_LCONTROL == (XINPUT_MODIFIER_LCONTROL & m_keyboardState.bModifiers));
            bbool bRCtrl    = (XINPUT_MODIFIER_RCONTROL == (XINPUT_MODIFIER_RCONTROL & m_keyboardState.bModifiers));
            bbool bLAlt     = (XINPUT_MODIFIER_LALT     == (XINPUT_MODIFIER_LALT & m_keyboardState.bModifiers));
            bbool bRAlt     = (XINPUT_MODIFIER_RALT     == (XINPUT_MODIFIER_RALT & m_keyboardState.bModifiers));
            bbool bLShift   = (XINPUT_MODIFIER_LSHIFT   == (XINPUT_MODIFIER_LSHIFT & m_keyboardState.bModifiers));
            bbool bRShift   = (XINPUT_MODIFIER_RSHIFT   == (XINPUT_MODIFIER_RSHIFT & m_keyboardState.bModifiers));

            LOG_GUILL_THE_WHITE("[XINPUT_MODIFIER] LCONTROL: %d\t RCONTROL: %d\t LALT: %d\t RALT: \t LShift: %d\t RShift: %d", bLCtrl, bRCtrl, bLAlt, bRAlt, bLShift, bRShift);
            // Bad resultat:
            // -------------
            // RShift = nothing
            // RAlt   = LShift
            // LShift = RShift

            if (bLCtrl != m_keyboardModifierStatePrevious.bLCtrl)
                onKeyCB(VK_LCONTROL, bLCtrl);

            if (bRCtrl != m_keyboardModifierStatePrevious.bRCtrl)
                onKeyCB(VK_RCONTROL, bLCtrl);

            if (bLAlt != m_keyboardModifierStatePrevious.bLAlt)
                onKeyCB(VK_LMENU, bLAlt);

            if (bRAlt != m_keyboardModifierStatePrevious.bRAlt)
                onKeyCB(VK_RMENU, bRAlt);

            if (bLShift != m_keyboardModifierStatePrevious.bLShift)
                onKeyCB(VK_LSHIFT, bLShift);

            if (bRShift != m_keyboardModifierStatePrevious.bRShift)
                onKeyCB(VK_RSHIFT, bRShift);
            
            m_keyboardModifierStatePrevious.bLCtrl  = bLCtrl;
            m_keyboardModifierStatePrevious.bRCtrl  = bRCtrl;
            m_keyboardModifierStatePrevious.bLAlt   = bLAlt;
            m_keyboardModifierStatePrevious.bRAlt   = bRAlt;
            m_keyboardModifierStatePrevious.bLShift = bLShift;
            m_keyboardModifierStatePrevious.bRShift = bRShift;


            u32 keyPressed;
            bbool keyAlwaysPressed = bfalse;
            // pressed ?
            for (u32 indexCurrent = 0; indexCurrent < XINPUT_MAX_KEYBOARD_STATE; indexCurrent++)
            {
                keyPressed = m_keyboardState.VKCodes[indexCurrent];
                if(keyPressed == 0) //no key
                    continue;

                keyAlwaysPressed = bfalse;
                for (u32 indexPrevious = 0; indexPrevious < XINPUT_MAX_KEYBOARD_STATE; indexPrevious++)
                {
                    if(m_keyboardStatePrevious.VKCodes[indexPrevious] == keyPressed)
                    {
                        keyAlwaysPressed = btrue;
                        break;
                    }
                }
                // just pressed
                if(!keyAlwaysPressed)
                {
                    LOG_GUILL_THE_WHITE("[XINPUT_MODIFIER] Key pressed: %d", keyPressed);
                    onKeyCB(keyPressed, btrue);
                }
            }

            // release ?
            for (u32 indexPrevious = 0; indexPrevious < XINPUT_MAX_KEYBOARD_STATE; indexPrevious++)
            {
                keyPressed = m_keyboardStatePrevious.VKCodes[indexPrevious];
                if(keyPressed == 0) //no key
                    continue;

                keyAlwaysPressed = bfalse;
                for (u32 indexCurrent = 0; indexCurrent < XINPUT_MAX_KEYBOARD_STATE; indexCurrent++)
                {
                    if(m_keyboardState.VKCodes[indexCurrent] == keyPressed)
                    {
                        keyAlwaysPressed = btrue;
                        break;
                    }
                }
                // just released
                if(!keyAlwaysPressed)
                {
                    LOG_GUILL_THE_WHITE("[XINPUT_MODIFIER] Key released: %d", keyPressed);
                    onKeyCB(keyPressed, bfalse);
                }
            }
            
            // save preview states
            m_keyboardStatePrevious = m_keyboardState;
        }


#endif //!ITF_FINAL

    }

    //----------------------------------------------------------------------------//

    void InputAdapter_x360::updateAllInputState()
    {
        u32 i;
        m_InputX360.EnumJoysticks();

        for (i = 0; i < m_InputX360.m_numberpad; i++)
        {
            Joy* joy = &m_InputX360.m_pad[i];
            if (joy->getDeviceState() == EINPUTDEVICE_CONNECTEDCOMPATIBLE)
            {
               ITF_ASSERT(i<maxPad);
               setPadConnected(i, btrue);
               joy->UpdateInputState(g_ControlDescGamepad,m_mappingTranslator);
            }
            else
                setPadConnected(i, bfalse);
        }
        for (;i<maxPad; i++)
            setPadConnected(i, bfalse);

        updateKeyboard();
        updateMouse();
    }

    //----------------------------------------------------------------------------//

    HRESULT InputX360::InitDirectInput()
    {
        EnumJoysticks();
        return S_OK;
    }


    XINPUTID InputMappingTranslator::getTranslatedID(XINPUTID _device,XINPUTID _key) const
    {
        mapDeviceMapping::const_iterator iterDevice = m_deviceMapping.find(_device);
        if (iterDevice != m_deviceMapping.end()) //we found the device
        {
            const mapKeyTranslator& keyTranslator = (iterDevice->second);
            mapKeyTranslator::const_iterator iterKey = keyTranslator.find(_key);
            if (iterKey!=keyTranslator.end()) //we find the key ,we return the mapping
            {
                return iterKey->second;
            }
        }

        //we found nothing ,return the default behavior
        return _key;
    }

    InputX360::InputX360()
    {        
        m_numberpad = 0;
    }

    InputX360::~InputX360()
    {
       
    }

#define INPUT_PROPERTY(_type) XInput2HasProperty( context, _type)

    void InputX360::EnumJoysticks()
    {
        m_numberpad = 0;
        XINPUT2CONTEXT context;
        BOOL changed;

        ITF_MemSet( &context, 0, sizeof(context) );

        for (u32 index = 0;index<MAX_PAD;++index)
        {
            Joy* pJoy = &m_pad[index];
            if( XInput2Sample( XInput2InputDevice(index), &context, &changed ) )
            {
                if( EINPUTDEVICE_NOTCONNECTED == pJoy->getDeviceState() )
                {
                    XINPUTID deviceID;
                    if (XInput2GetDeviceId(context,&deviceID))
                    {
                        pJoy->setDeviceID(deviceID);
                    }
                    
                    pJoy->setDeviceState(EINPUTDEVICE_CONNECTEDCOMPATIBLE);
                }
            }
            else
            {
                pJoy->setDeviceState(EINPUTDEVICE_NOTCONNECTED);
            }

            pJoy->setId(index);

            m_numberpad++;
        }
    }

} // namespace ITF

