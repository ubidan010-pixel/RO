#include "precompiled_inputadapter_PS3.h"

#ifndef _ITF_INPUTADAPTER_PS3_H_
#include "adapters/InputAdapter_PS3/InputAdapter_PS3.h"
#endif //_ITF_INPUTADAPTER_PS3_H_

#ifndef _ITF_VEC2D_H_
#include "core/math/vec2d.h"
#endif //_ITF_VEC2D_H_

#ifndef _ITF_MATHTOOLS_H_
#include "core/math/MathTools.h"
#endif //_ITF_MATHTOOLS_H_

#ifndef _ITF_SYSTEMADAPTER_
#include "core/AdaptersInterfaces/SystemAdapter.h"
#endif //_ITF_SYSTEMADAPTER_

#include <sysutil/sysutil_sysparam.h>
#include <algorithm>

static CellPadData  sPadData[CELL_UTIL_PAD_MAX];

/* Bit assignment of digital buttons. */
/* "CELL_PAD_BTN_OFFSET_DIGITAL1" assignment */
#define CELL_UTIL_BUTTON_SELECT   ( CELL_PAD_CTRL_SELECT << 0 )
#define CELL_UTIL_BUTTON_L3       ( CELL_PAD_CTRL_L3	 << 0 )
#define CELL_UTIL_BUTTON_R3       ( CELL_PAD_CTRL_R3	 << 0 )
#define CELL_UTIL_BUTTON_START    ( CELL_PAD_CTRL_START	 << 0 )
#define CELL_UTIL_BUTTON_UP       ( CELL_PAD_CTRL_UP	 << 0 )
#define CELL_UTIL_BUTTON_RIGHT    ( CELL_PAD_CTRL_RIGHT  << 0 )
#define CELL_UTIL_BUTTON_DOWN     ( CELL_PAD_CTRL_DOWN	 << 0 )
#define CELL_UTIL_BUTTON_LEFT     ( CELL_PAD_CTRL_LEFT	 << 0 )

/* "CELL_PAD_BTN_OFFSET_DIGITAL2" assignment */
#define CELL_UTIL_BUTTON_L2       ( CELL_PAD_CTRL_L2		<< 8 )
#define CELL_UTIL_BUTTON_R2       ( CELL_PAD_CTRL_R2		<< 8 )
#define CELL_UTIL_BUTTON_L1       ( CELL_PAD_CTRL_L1		<< 8 )
#define CELL_UTIL_BUTTON_R1       ( CELL_PAD_CTRL_R1		<< 8 )
#define CELL_UTIL_BUTTON_TRIANGLE ( CELL_PAD_CTRL_TRIANGLE	<< 8 )
#define CELL_UTIL_BUTTON_CIRCLE   ( CELL_PAD_CTRL_CIRCLE	<< 8 )
#define CELL_UTIL_BUTTON_CROSS    ( CELL_PAD_CTRL_CROSS		<< 8 )
#define CELL_UTIL_BUTTON_SQUARE   ( CELL_PAD_CTRL_SQUARE	<< 8 )

#define CELL_UTIL_ANALOG_RIGHT    ( CELL_PAD_BTN_OFFSET_ANALOG_RIGHT_X	<< 16 )
#define CELL_UTIL_ANALOG_LEFT     ( CELL_PAD_BTN_OFFSET_ANALOG_LEFT_X	<< 16 )

namespace ITF
{

//    const f32 INPUT_DEADZONE_LEFT = XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE/32767.0f;

    CONTROL_DESCRIPTION g_ControlDescGamepad[] = 
    {

        {CELL_UTIL_BUTTON_UP,				"DPAD_UP",0,m_joyButton_DPadU},
        {CELL_UTIL_BUTTON_DOWN,				"DPAD_DOWN",0,m_joyButton_DPadD},
        {CELL_UTIL_BUTTON_LEFT,				"DPAD_LEFT",0,m_joyButton_DPadL},
        {CELL_UTIL_BUTTON_RIGHT,			"DPAD_RIGHT",0,m_joyButton_DPadR},
        
        {CELL_UTIL_BUTTON_START,			"START",0,m_joyButton_Start},
        {CELL_UTIL_BUTTON_SELECT,			"BACK",0,m_joyButton_Select},
        
        {CELL_UTIL_BUTTON_L1,				"L_SHOULDER",0,m_joyButton_L1},
        {CELL_UTIL_BUTTON_R1,				"R_SHOULDER",0,m_joyButton_R1},

        {CELL_UTIL_BUTTON_L2,			    "L_SHOULDER2",0,m_joyButton_L2},
        {CELL_UTIL_BUTTON_R2,		        "R_SHOULDER2",0,m_joyButton_R2},

        {CELL_UTIL_BUTTON_L3,			    "L_THUMB",0,m_joyButton_ThumbLeft},
        {CELL_UTIL_BUTTON_R3,		        "R_THUMB",0,m_joyButton_ThumbRight},

        {CELL_UTIL_BUTTON_CROSS,			"DPAD_CROSS",0,m_joyButton_Cross},
        {CELL_UTIL_BUTTON_CIRCLE,			"DPAD_CIRCLE",0,m_joyButton_Circle},
        {CELL_UTIL_BUTTON_SQUARE,		    "DPAD_SQUARE",0,m_joyButton_Square},
        {CELL_UTIL_BUTTON_TRIANGLE,			"DPAD_TRIANGLE",0,m_joyButton_Triangle},

        {CELL_UTIL_BUTTON_L2,		        "L_TRIGGER",	1,m_joyTrigger_Left},
        {CELL_UTIL_BUTTON_R2,		        "R_TRIGGER",	1,m_joyTrigger_Right},
        {CELL_UTIL_BUTTON_LEFT,    			"BUTTONS",		2,-1},
        {CELL_UTIL_ANALOG_LEFT,	            "L_STICK",	       2,m_joyStick_Left},
        {CELL_UTIL_ANALOG_RIGHT,            "R_STICK",     2,m_joyStick_Right}
    };


    const u32 g_dwNumDescGamepad = sizeof( g_ControlDescGamepad )/sizeof(g_ControlDescGamepad[0]);

    u32 getITFButtonFromPS3Button(u32 PS3Button)
    {
        for(u32 i = 0; i < g_dwNumDescGamepad; i++)
        {
            if(g_ControlDescGamepad[i].ID == PS3Button)
                return g_ControlDescGamepad[i].mappingValue;
        }
        ITF_ASSERT_MSG(0, "PS3 button not found");
        return U32_INVALID;
    }


    Joy::Joy()
    {
        m_id = -1;
        ITF_MemSet(&m_axes, 0,sizeof(float) * MAX_AXES);
        ITF_MemSet(&m_buttons,0,sizeof(InputAdapter::PressStatus) * JOY_MAX_BUT);
        ITF_MemSet(&m_rumbleState,0,sizeof(RumbleState));
  //      m_eInputDeviceState = EINPUTDEVICE_NOTCONNECTED;

//        ITF_MemSet( m_bControlDataBlock, 0x00, sizeof(m_bControlDataBlock) );
    }

    Joy::~Joy()
    {
    }

    void    Joy::mapButtons(const CONTROL_DESCRIPTION& controlDesc,bbool boolValue,f32 floatValue,const Vec2d* _pointValue)
    {

        if (controlDesc.dwDataType  == 1)
        {
            switch (controlDesc.mappingValue)
            {
            case m_joyTrigger_Left:
            case m_joyTrigger_Right:
                m_axes[controlDesc.mappingValue] = boolValue ? 1.0f :0.0f;
                break;
            default:
                ITF_ASSERT(0);
                break;
            }
        }
        else
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
            case m_joyButton_ThumbRight:
            case m_joyButton_ThumbLeft:
            case m_joyButton_Start:
            case m_joyButton_L2:
            case m_joyButton_R2:
                m_buttons[controlDesc.mappingValue] =  boolValue ? InputAdapter::Pressed :  InputAdapter::Released;
            break;

            case m_joyStick_Left:
                {
                    m_axes[0] = (*_pointValue).m_x;
                    m_axes[1] = -(*_pointValue).m_y;
                }
                break;
        case m_joyStick_Right:
                {
                    m_axes[3] = (*_pointValue).m_x;
                    m_axes[4] = -(*_pointValue).m_y;
                }
                break;
        }
    }


    float axis_convert(const unsigned char value) 
    {
        const float dead_zone = 0.16f;
        float newValue = value;
        newValue -= 127;
        newValue /= 128;

        float result = fabs( newValue );
        result = Clamp( result - dead_zone, 0.0f, 1.0f) / (1.0f - dead_zone);
        if( newValue < 0 )
            result *= -1;

        return result;
    }

    void Joy::UpdateInputState( const CONTROL_DESCRIPTION controlDesc[])
    {
        u32 pad = m_id;

        CellPadData paddata;
        int32_t ret = cellPadGetData(pad, &paddata);


        if(ret == CELL_PAD_OK && paddata.len != 0)
        {		
            InputAdapter::PressStatus temp_buttons[JOY_MAX_BUT];
            ITF_Memcpy(temp_buttons,m_buttons,sizeof(temp_buttons));

            sPadData[pad] = paddata;
            uint16_t digit1 = 0x00FF & paddata.button[CELL_PAD_BTN_OFFSET_DIGITAL1];
            uint16_t digit2 = 0x00FF & paddata.button[CELL_PAD_BTN_OFFSET_DIGITAL2];

            u32 cellPadUtilDigitalButtonData = (u32(digit2) << 8u) | u32(digit1);

            {
                for( u32 dwControl = 0; dwControl < g_dwNumDescGamepad; dwControl++ )
                {
                    bbool bRead = cellPadUtilDigitalButtonData & controlDesc[dwControl].ID;
                    Vec2d ret(Vec2d::Zero);
                    if (controlDesc[dwControl].dwDataType == 2)
                    {
                        uint32_t offset = (controlDesc[dwControl].ID>> 16) & 0x00FF;

                        ret.m_x = axis_convert(sPadData[pad].button[offset]);
                        ret.m_y = axis_convert(sPadData[pad].button[offset + 1]);
                    }

                    mapButtons(controlDesc[dwControl], bRead, 0.0f, &ret);
                }
            }

            for (u32 i = 0; i < JOY_MAX_BUT; ++i)
            {
                if (temp_buttons[i] != InputAdapter::JustPressed && temp_buttons[i] != InputAdapter::Pressed && m_buttons[i] == InputAdapter::Pressed)
                    m_buttons[i] = InputAdapter::JustPressed;
                else if (temp_buttons[i] != InputAdapter::JustReleased && temp_buttons[i] != InputAdapter::Released && m_buttons[i] == InputAdapter::Released)
                    m_buttons[i] = InputAdapter::JustReleased;
            }
        }
        else
        {
            for (u32 i = 0; i < JOY_MAX_BUT; ++i)
            {
                if (m_buttons[i] == InputAdapter::JustPressed)
                    m_buttons[i] = InputAdapter::Pressed;
                else if (m_buttons[i] == InputAdapter::JustReleased)
                    m_buttons[i] = InputAdapter::Released;
            }
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

    void Joy::UpdateNoInputState( const CONTROL_DESCRIPTION controlDesc[])
    {
        u32 pad = m_id;

        InputAdapter::PressStatus temp_buttons[JOY_MAX_BUT];
        ITF_Memcpy(temp_buttons,m_buttons,sizeof(temp_buttons));

        for( u32 dwControl = 0; dwControl < g_dwNumDescGamepad; dwControl++ )
        {
            Vec2d ret(Vec2d::Zero);
            mapButtons(controlDesc[dwControl], bfalse, 0.0f, &ret);
        }

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

    void Joy::padVibration( f32 _leftMotorSpeed, f32 _rightMotorSpeed, f64 _time )
    {        
        // Warning: on PS3 there is a 3sec timeout with rumble feature.
        // TODO: add an auto check if the time is superior than 3sec, 
        // call padViration many times.
        bbool StartMotor = (_leftMotorSpeed + _leftMotorSpeed) > 0.0f;
        //CellPadInfo2 info  ;
        //cellPadGetInfo2(&info);

        //bbool MotorEnable = bfalse;
        //if( info.device_capability[getId()] & CELL_PAD_CAPABILITY_ACTUATOR )
        //{
        //    MotorEnable = btrue;
        //}

        if(m_rumbleState.motorOn != StartMotor)
        {            
            m_rumbleState.motorOn = StartMotor;

            CellPadActParam param; 
            param.motor[0] = _leftMotorSpeed > 0.0f ? 1 : 0;  // 0 - 1
            param.motor[1] = (u8)(_rightMotorSpeed * 255.0f);    // 0 - 255
            ITF_MemSet(param.reserved, 0, sizeof(param.reserved));

            cellPadSetActDirect(getId(), &param);            
        }

        if(StartMotor)
        {
            m_rumbleState.duringTime    = _time;
            m_rumbleState.startTime     = SYSTEM_ADAPTER->getTime();  
        }  
    }

    //----------------------------------------------------------------------------//


    // Constructor.
    InputAdapter_PS3::InputAdapter_PS3()
    {
        int ret = cellPadInit(CELL_UTIL_PAD_MAX);

    #ifdef ITF_ALLOW_KEYBOARD
        initConvertKeyTable();

        u32 NbMaxKeyboardPort = 7;
        ITF_VERIFY(CELL_OK == cellKbInit(NbMaxKeyboardPort));
        m_currentKeyBoardPort = InvalidKeyboardPort;

        m_lastKeyPressed.reserve(MaxKeyboardKeyPressed); 
        m_keyPressed.reserve(MaxKeyboardKeyPressed);
    #endif
    }

    /// Destructor.
    InputAdapter_PS3::~InputAdapter_PS3()
    {    
#ifdef ITF_ALLOW_KEYBOARD
        cellKbEnd();
#endif
    }

    void InputAdapter_PS3::getGamePadPos( u32 _environment,u32 _pad, float *_pos, u32 _numAxes ) const
    {
        const Joy* pjoy = &m_Joy[_pad];

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

    u32 InputAdapter_PS3::getConnectedPadIndex (u32 _padIndex) const
    {
        return 0;
    }

    void InputAdapter_PS3::getGamePadButtons( u32 _environment ,u32 _pad, PressStatus* _buttons, u32 _numButtons ) const
    {
        ITF_ASSERT(_numButtons <= JOY_MAX_BUT); // just increase JOY_MAX_BUT if you assert here

        const Joy* pJoy = &m_Joy[_pad];

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

    u32 InputAdapter_PS3::getGamePadCount()
    {
        return CELL_UTIL_PAD_MAX + 1;
    }


    void InputAdapter_PS3::updateMouse()
    {
#ifdef ITF_ALLOW_MOUSE

#endif
    }

    bbool InputAdapter_PS3::isConnected(u8 pad) const
    {
        if(pad >= CELL_UTIL_PAD_MAX) return false;
        if(pad >= sPadInfo2.max_connect) return false;

        return (sPadInfo2.port_status[pad] & CELL_PAD_STATUS_CONNECTED);
    }


    void InputAdapter_PS3::updateAllInputState()
    {
        CellPadInfo2 pad_info2;
        int32_t cr = cellPadGetInfo2(&pad_info2);
        if(cr != CELL_PAD_OK)
            return;

        sPadInfo2 = pad_info2;
        bool intercepted = (sPadInfo2.system_info & CELL_PAD_INFO_INTERCEPTED);
        
        // update supported pads
        for(int pad = 0; pad < CELL_UTIL_PAD_MAX; ++pad)
        {
            Joy* joy = &m_Joy[pad];
            bool isSupportedPad = pad<JOY_MAX_COUNT;
            if(!isConnected(pad))
            {
                if (isSupportedPad)
                    setPadConnected(pad, bfalse);
                joy->UpdateNoInputState(g_ControlDescGamepad);
            }
            else 
            {
                if (isSupportedPad)
                    setPadConnected(pad, btrue);
                joy->setId(pad);
                if (intercepted)
                {
                    joy->UpdateNoInputState(g_ControlDescGamepad);
                }
                else
                {
                    joy->UpdateInputState(g_ControlDescGamepad);
                }
            }
        }

        updateMouse();
        updateKeyboard();
    }

    void InputAdapter_PS3::CleanKeyboardStates()
    {
        m_lastKeyPressed.resize(0);
        m_lastMetaKeyFlags = 0;
        if (m_currentKeyBoardPort != InvalidKeyboardPort)
        {
            ITF_VERIFY(CELL_OK == cellKbClearBuf(m_currentKeyBoardPort));
        }
    }

    void InputAdapter_PS3::KeyboardDisconnected()
    {
        m_currentKeyBoardPort = InvalidKeyboardPort;
        CleanKeyboardStates();
    }

    void InputAdapter_PS3::KeyboardConnected(u32 _port)
    {
        ITF_ASSERT(m_currentKeyBoardPort == InvalidKeyboardPort);
        m_currentKeyBoardPort = _port;
        ITF_VERIFY(CELL_OK == cellKbClearBuf(m_currentKeyBoardPort));
        ITF_VERIFY(CELL_OK == cellKbSetCodeType(m_currentKeyBoardPort, CELL_KB_CODETYPE_RAW));
        ITF_VERIFY(CELL_OK == cellKbSetReadMode(m_currentKeyBoardPort, CELL_KB_RMODE_PACKET));
    }

    void InputAdapter_PS3::updateKeyboard()
    {
#ifdef ITF_ALLOW_KEYBOARD

        m_keyPressed.resize(0);
        m_metaKeyFlags = 0;

        CellKbInfo cellKbInfo;
        i32 ret = cellKbGetInfo(&cellKbInfo);
        if (ret != CELL_KB_OK)
        {
            if (m_currentKeyBoardPort!=InvalidKeyboardPort)
                KeyboardDisconnected();
            CleanKeyboardStates();
            return;
        }

        // Check for interception by the system.
        if (cellKbInfo.info & CELL_KB_INFO_INTERCEPTED)
        {
            m_lastKeyPressed.resize(0);
            m_lastMetaKeyFlags = 0;
            return;
        }

        if (m_currentKeyBoardPort!=InvalidKeyboardPort
            && (m_currentKeyBoardPort>=cellKbInfo.max_connect
               || cellKbInfo.status[m_currentKeyBoardPort]==CELL_KB_STATUS_DISCONNECTED))
        {
            KeyboardDisconnected();
            return;
        }
        if (m_currentKeyBoardPort==InvalidKeyboardPort)
        {
            for(u32 i = 0; i<cellKbInfo.max_connect; ++i)
            {
                if (cellKbInfo.status[i]==CELL_KB_STATUS_CONNECTED)
                {
                    KeyboardConnected(i);
                }
            }
        }

        if (m_currentKeyBoardPort==InvalidKeyboardPort)
        {
            CleanKeyboardStates();
            return;
        }

        CellKbData  cellKbData;
        cellKbData.len = 0;
        cellKbData.mkey = 0;
        cellKbData.led = 0;
        memset(cellKbData.keycode, 0xFF, sizeof(cellKbData.keycode));

        ret = cellKbRead(m_currentKeyBoardPort, &cellKbData);
        if (ret != CELL_KB_OK)
        {
            KeyboardDisconnected();
            return;
        }

        m_keyPressed.assign(cellKbData.keycode, cellKbData.keycode+cellKbData.len);

        std::sort(m_keyPressed.begin(), m_keyPressed.end()); // sort them to be able to use sorted ranges operations
        m_metaKeyFlags = cellKbData.mkey;

        ProcessMetaKeyEvents();
        ProcessKeyEvents();

        m_lastMetaKeyFlags = m_metaKeyFlags;
        m_lastKeyPressed = m_keyPressed;

#endif
    }

    void InputAdapter_PS3::ProcessKeyEvents()
    {
        u32 diffResult[MaxKeyboardKeyPressed];
        // first, do m_keyPressed - m_lastKeyPressed and send these events as pressed keys
        u32 * endDiff = std::set_difference(m_keyPressed.begin(), m_keyPressed.end(),
                                            m_lastKeyPressed.begin(), m_lastKeyPressed.end(),
                                            diffResult);
        for(u32 * it = diffResult; it!=endDiff; ++it)
        {
            if (*it) // 0 is a special 'No Event' case
            {
                onKey(ConvertKeyCode(*it), InputAdapter::Pressed);
            }
        }
        // then, do m_lastKeyPressed-m_keyPressed and send these events as released keys
        endDiff = std::set_difference(m_lastKeyPressed.begin(), m_lastKeyPressed.end(),
                                            m_keyPressed.begin(), m_keyPressed.end(),
                                            diffResult);
        for(u32 * it = diffResult; it!=endDiff; ++it)
        {
            if (*it)
            {
                onKey(ConvertKeyCode(*it), InputAdapter::Released);
            }
        }
    }

    template <u32 Mask, u32 ItfKey>
    void InputAdapter_PS3::ProcessMetaKeyEvent()
    {
        bool pressed = (m_metaKeyFlags &  Mask) != 0;
        bool wasPressed = (m_lastMetaKeyFlags &  Mask) != 0;
        if (pressed != wasPressed)
        {
            onKey(ItfKey, pressed?InputAdapter::Pressed:InputAdapter::Released);
        }
    }

    void InputAdapter_PS3::ProcessMetaKeyEvents()
    {
        ProcessMetaKeyEvent<CELL_KB_MKEY_L_CTRL, KEY_LCTRL>();
        ProcessMetaKeyEvent<CELL_KB_MKEY_L_SHIFT, KEY_LSHIFT>();
        ProcessMetaKeyEvent<CELL_KB_MKEY_L_ALT, KEY_LALT>();
        // ProcessMetaKeyEvent<CELL_KB_MKEY_L_WIN, KEY_SPECIAL>();
        ProcessMetaKeyEvent<CELL_KB_MKEY_R_CTRL, KEY_RCTRL>();
        ProcessMetaKeyEvent<CELL_KB_MKEY_R_SHIFT, KEY_RSHIFT>();
        ProcessMetaKeyEvent<CELL_KB_MKEY_R_ALT, KEY_RALT>();
        // ProcessMetaKeyEvent<CELL_KB_MKEY_R_WIN, KEY_SPECIAL>();
    }

    void InputAdapter_PS3::initConvertKeyTable()
    {
        // init missing keys to 'special'
        for(u32 i = 0; i < MaxKeyboardKeyCode; ++i)
        {
            m_cellToItfConvertTable[i] = KEY_SPECIAL;
        }
        
        // first init logically following keys
        for(i32 i = CELL_KEYC_F1; i<=CELL_KEYC_F12; ++i)
        {
            m_cellToItfConvertTable[i] = KEY_F1 + (i - CELL_KEYC_F1);
        }

        for(i32 i = CELL_KEYC_KPAD_1; i<=CELL_KEYC_KPAD_9; ++i)
        {
            m_cellToItfConvertTable[i] = KEY_KP_1 + (i - CELL_KEYC_KPAD_1);
        }
        m_cellToItfConvertTable[CELL_KEYC_KPAD_0] = KEY_KP_0;

        // keys associated to ascii characters 
        // numbers
        for(i32 i = CELL_KEYC_1; i<=CELL_KEYC_9; ++i)
        {
            m_cellToItfConvertTable[i] = '1' + (i - CELL_KEYC_1);
        }
        m_cellToItfConvertTable[CELL_KEYC_0] = '0';
        
        // keys associated to ascii characters 
        // alpha
        for(i32 i = CELL_KEYC_A; i<=CELL_KEYC_Z; ++i)
        {
            m_cellToItfConvertTable[i] = 'A' + (i - CELL_KEYC_A);
        }

        // missing keys
        m_cellToItfConvertTable[CELL_KEYC_ESC] = KEY_ESC;
        m_cellToItfConvertTable[CELL_KEYC_UP_ARROW] = KEY_UP;
        m_cellToItfConvertTable[CELL_KEYC_LEFT_ARROW] = KEY_LEFT;
        m_cellToItfConvertTable[CELL_KEYC_RIGHT_ARROW] = KEY_RIGHT;
        m_cellToItfConvertTable[CELL_KEYC_DOWN_ARROW] = KEY_DOWN;

        // We use 0xE0 onward for the meta keys.
        static const i32 KEYC_L_CTRL   = 0xE0;
        static const i32 KEYC_L_SHIFT  = 0xE1;
        static const i32 KEYC_L_ALT    = 0xE2;
        static const i32 KEYC_L_WIN    = 0xE3;
        static const i32 KEYC_R_CTRL   = 0xE4;
        static const i32 KEYC_R_SHIFT  = 0xE5;
        static const i32 KEYC_R_ALT    = 0xE6;
        static const i32 KEYC_R_WIN    = 0xE7;

        m_cellToItfConvertTable[KEYC_L_SHIFT] = KEY_LSHIFT;
        m_cellToItfConvertTable[KEYC_R_SHIFT] = KEY_RSHIFT;
        m_cellToItfConvertTable[KEYC_L_CTRL] = KEY_LCTRL;
        m_cellToItfConvertTable[KEYC_R_CTRL] = KEY_RCTRL;
        m_cellToItfConvertTable[KEYC_L_ALT] = KEY_LALT;
        m_cellToItfConvertTable[KEYC_R_ALT] = KEY_RALT;
        m_cellToItfConvertTable[CELL_KEYC_SPACE] = KEY_SPACE;
        m_cellToItfConvertTable[CELL_KEYC_TAB] = KEY_TAB;
        m_cellToItfConvertTable[CELL_KEYC_ENTER] = KEY_ENTER;
        m_cellToItfConvertTable[CELL_KEYC_BS] = KEY_BACKSPACE;
        m_cellToItfConvertTable[CELL_KEYC_INSERT] = KEY_INSERT;
        m_cellToItfConvertTable[CELL_KEYC_DELETE] = KEY_DEL;
        m_cellToItfConvertTable[CELL_KEYC_PAGE_UP] = KEY_PAGEUP;
        m_cellToItfConvertTable[CELL_KEYC_PAGE_DOWN] = KEY_PAGEDOWN;
        m_cellToItfConvertTable[CELL_KEYC_HOME] = KEY_HOME;
        m_cellToItfConvertTable[CELL_KEYC_END] = KEY_END;
        m_cellToItfConvertTable[CELL_KEYC_SLASH] = KEY_KP_DIVIDE;
        m_cellToItfConvertTable[CELL_KEYC_KPAD_ASTERISK] = KEY_KP_MULTIPLY;
        m_cellToItfConvertTable[CELL_KEYC_KPAD_MINUS] = KEY_KP_SUBTRACT;
        m_cellToItfConvertTable[CELL_KEYC_KPAD_PLUS] = KEY_KP_ADD;
        m_cellToItfConvertTable[CELL_KEYC_KPAD_PERIOD] = KEY_KP_DECIMAL;
        m_cellToItfConvertTable[CELL_KEYC_KPAD_ENTER] = KEY_KP_ENTER;
        m_cellToItfConvertTable[CELL_KEYC_PRINTSCREEN] = KEY_PRINT;
    }

    i32 InputAdapter_PS3::ConvertKeyCode(u32 _cellKbKeyCode)
    {
        ITF_ASSERT(_cellKbKeyCode<MaxKeyboardKeyCode);
        if (_cellKbKeyCode<MaxKeyboardKeyCode)
        {
            ITF_ASSERT(m_cellToItfConvertTable[_cellKbKeyCode]!=KEY_SPECIAL);
            return m_cellToItfConvertTable[_cellKbKeyCode];
        }
        else
            return KEY_SPECIAL;
    }

    void InputAdapter_PS3::startRumble( u32 _numPad, f64 _time, f32 _leftMotorSpeed, f32 _rightMotorSpeed )
    {
        Joy* joy = &m_Joy[_numPad];
        joy->padVibration(_leftMotorSpeed, _rightMotorSpeed, _time);
    }

    void InputAdapter_PS3::stopRumble( u32 _numPad )
    {
        Joy* joy = &m_Joy[_numPad];
        joy->padVibration(0.0f,0.0f,0.0);
    }

    //----------------------------------------------------------------------------//

} // namespace ITF

