#ifndef _ITF_INPUTADAPTER_H_
#define _ITF_INPUTADAPTER_H_
#include <iostream>

#ifndef _ITF_TEMPLATESINGLETON_H_
#include "core/templateSingleton.h"
#endif //_ITF_TEMPLATESINGLETON_H_

#ifndef _ITF_CORETYPES_H_
#include "core/types.h"
#endif //_ITF_CORETYPES_H_

#ifndef _ITF_SYSTEMADAPTER_
#include "core/AdaptersInterfaces/SystemAdapter.h"
#endif //_ITF_SYSTEMADAPTER_

#include <algorithm>
#include <limits>

namespace ITF
{
    enum KeyCode
    {
        KEY_SPACE = 32,
        KEY_SPECIAL = 256,
        KEY_ESC,
        KEY_F1,
        KEY_F2,
        KEY_F3,
        KEY_F4,
        KEY_F5,
        KEY_F6,
        KEY_F7,
        KEY_F8,
        KEY_F9,
        KEY_F10,
        KEY_F11,
        KEY_F12,
        KEY_F13,
        KEY_F14,
        KEY_F15,
        KEY_F16,
        KEY_F17,
        KEY_F18,
        KEY_F19,
        KEY_F20,
        KEY_F21,
        KEY_F22,
        KEY_F23,
        KEY_F24,
        KEY_F25,
        KEY_UP,
        KEY_DOWN,
        KEY_LEFT,
        KEY_RIGHT,
        KEY_LSHIFT,
        KEY_RSHIFT,
        KEY_LCTRL,
        KEY_RCTRL,
        KEY_LALT,
        KEY_RALT,
        KEY_TAB,
        KEY_ENTER,
        KEY_BACKSPACE,
        KEY_INSERT,
        KEY_DEL,
        KEY_PAGEUP,
        KEY_PAGEDOWN,
        KEY_HOME,
        KEY_END,
        KEY_KP_0,
        KEY_KP_1,
        KEY_KP_2,
        KEY_KP_3,
        KEY_KP_4,
        KEY_KP_5,
        KEY_KP_6,
        KEY_KP_7,
        KEY_KP_8,
        KEY_KP_9,
        KEY_KP_DIVIDE,
        KEY_KP_MULTIPLY,
        KEY_KP_SUBTRACT,
        KEY_KP_ADD,
        KEY_KP_DECIMAL,
        KEY_KP_EQUAL,
        KEY_KP_ENTER,
        KEY_PRINT,

        KEY_COUNT,
        ENUM_FORCE_SIZE_32(KeyCode)
    };

    class Interface_InputListener;

    // Input device type for tracking last used input
    enum InputDeviceType : u32
    {
        InputDevice_None = 0,
        InputDevice_Keyboard,
        InputDevice_Gamepad,
    };

    // *** BUTTONS ***
    // XBOX SPECIFIC + COMMON
    enum JoyButton_Common : u32
    {
        m_joyButton_A = 0,
        m_joyButton_B,
        m_joyButton_X,
        m_joyButton_Y,
        m_joyButton_LB,
        m_joyButton_RB,
        m_joyButton_Back,
        m_joyButton_Start,
        m_joyButton_DPadL,
        m_joyButton_DPadR,
        m_joyButton_DPadU,
        m_joyButton_DPadD,
        m_joyStick_Left,
        m_joyStick_Right,
        m_joyButton_ThumbLeft,
        m_joyButton_ThumbRight,
        m_joyButton_TriggerLeft,
        m_joyButton_TriggerRight,

        m_joyButton_CommonCount,
    };

    // Sony specific
    enum JoyButton_Sony : u32
    {
        m_joyButton_Cross = m_joyButton_A,
        m_joyButton_Circle = m_joyButton_B,
        m_joyButton_Triangle = m_joyButton_Y,
        m_joyButton_Square = m_joyButton_X,
        m_joyButton_R1 = m_joyButton_RB,
        m_joyButton_L1 = m_joyButton_LB,
        m_joyButton_R2 = m_joyButton_TriggerRight,
        m_joyButton_L2 = m_joyButton_TriggerLeft,
    };

    // PS3 SPECIFIC
    enum JoyButton_PS3
    {
        m_joyButton_Select = m_joyButton_Back,
    };


    // PS5 SPECIFIC
    enum JoyButton_PS5 : u32
    {
        m_joyButton_TouchPad = m_joyButton_Back,
        m_joyButton_Options = m_joyButton_Start
    };

    // WII SPECIFIC
    enum JoyButton_WII
    {
        m_joyButton_Home = m_joyButton_CommonCount,
        m_joyButton_1,
        m_joyButton_2,
        m_joyButton_C,
        m_joyButton_Z,
        m_joyButton_ZR,
        m_joyButton_ZL,
        m_joyActionShaked
    };

    // Switch
    enum JoyButton_Switch : u32
    {
        m_joyButton_Plus = m_joyButton_Start,
        m_joyButton_Minus = m_joyButton_Back,
        m_joyButton_R = m_joyButton_RB,
        m_joyButton_L = m_joyButton_LB,
        m_joyButton_LeftSL = m_joyButton_CommonCount,
        m_joyButton_LeftSR,
        m_joyButton_RightSL,
        m_joyButton_RightSR,

        m_joyButton_NxCount
    };

    // *** JOYSTICK ***
    // COMMON
    enum JoyAxis_t
    {
        m_joyStickLeft_X=0,
        m_joyStickLeft_Y,
        m_joyTrigger_Left,
        m_joyStickRight_X,
        m_joyStickRight_Y,
        m_joyTrigger_Right,

        JOY_MAX_AXES
    };

    // WII SPECIFIC
    const u32 m_joyStick_X = m_joyStickLeft_X;
    const u32 m_joyStick_Y = m_joyStickLeft_Y;


#define  JOY_MAX_BUT        32
#define  JOY_MAX_COUNT      4 //When getting pad statuses, you can iterate from 0 to JOY_MAX_COUNT-1

#define JOY_BUT_A           m_joyButton_A
#define JOY_BUT_B           m_joyButton_B
#define JOY_BUT_X           m_joyButton_X
#define JOY_BUT_Y           m_joyButton_Y
#define JOY_BUT_LB          m_joyButton_LB
#define JOY_BUT_RB          m_joyButton_RB
#define JOY_BUT_BACK        m_joyButton_Back
#define JOY_BUT_START       m_joyButton_Start
#define JOY_BUT_LS          m_joyButton_ThumbLeft
#define JOY_BUT_RS          m_joyButton_ThumbRight
#define JOY_BUT_DPAD_UP     m_joyButton_DPadU
#define JOY_BUT_DPAD_DOWN   m_joyButton_DPadD
#define JOY_BUT_DPAD_LEFT   m_joyButton_DPadL
#define JOY_BUT_DPAD_RIGHT  m_joyButton_DPadR

#define JOY_AXIS_LX         0
#define JOY_AXIS_LY         1
#define JOY_AXIS_RX         2
#define JOY_AXIS_RY         3
#define JOY_AXIS_LT         4
#define JOY_AXIS_RT         5
#define JOY_MAX_AXES        6

    class InputAdapter : public TemplateSingleton<InputAdapter>
    {
    public:
        static const f32 fDoublePressMaxDuration;
        ///////////////////////////////////////////////////////////////////////////////////////////
        ///Button classes. Each button can belong to several classes, we can combine them with OR binary operator
        typedef u8 ButtonClassMask;

        static const ButtonClassMask BUTTONCLASS_STANDARD = 1; //on XBOX : A,B,X,Y,START,SELECT
        static const ButtonClassMask BUTTONCLASS_ANALOGSTICK = 2; //on XBOX : Left/right analog stick buttons
        static const ButtonClassMask BUTTONCLASS_DIGITALPAD = 4; //on XBOX : digital left,right,up,down buttons
        static const ButtonClassMask BUTTONCLASS_SHOULDER_UPPER = 8; //on XBOX : LB/RB
        static const ButtonClassMask BUTTONCLASS_TRIGGER = 16; //on XBOX : LT/RT
        static const ButtonClassMask BUTTONCLASS_UNKNOWN = 128;

        static const ButtonClassMask BUTTONCLASS_PUSHBUTTONS = BUTTONCLASS_STANDARD | BUTTONCLASS_ANALOGSTICK |
            BUTTONCLASS_SHOULDER_UPPER;

        ///////////////////////////////////////////////////////////////////////////////////////////
        enum MouseButton
        {
            MB_Left,
            MB_Right,
            MB_Middle,
            ENUM_FORCE_SIZE_32(3)
        };

        ///////////////////////////////////////////////////////////////////////////////////////////
        enum PressStatus
        {
            Released = 0,
            Pressed = 1,
            Double_Press = 2,
            JustPressed = 3,
            JustReleased = 4,
            ENUM_FORCE_SIZE_32(4)
        };

        ///////////////////////////////////////////////////////////////////////////////////////////
        enum EnvironmentInput
        {
            EnvironmentEngine = 1,
            EnvironmentLua = 2,
            EnvironmentAll = EnvironmentEngine | EnvironmentLua,
            ENUM_FORCE_SIZE_32(5)
        };

        ///////////////////////////////////////////////////////////////////////////////////////////
        enum PadType
        {
            Pad_Invalid = -1,
            Pad_Other = 0,
            Pad_X360,
            Pad_PS3,
            Pad_WiiSideWay,
            Pad_WiiNunchuk,
            Pad_WiiClassic,
            Pad_Vita,
            Pad_CTR,
            Pad_PS5,
            Pad_NX_Joycon,
            Pad_NX_Joycon_Dual,
            Pad_NX_Pro,
			Pad_GenericXBox,
            Pad_Keyboard,
            PadType_Count,
        };

        // ButtonMode is used to determinate which buttons we want when we call getGamePadButtons
        enum ButtonMode
        {
            MixedMode = 0,
            // A Mix between extension and wii remote, EX: if the "b" of the classic is pressed, b = pressed
            ExtensionOnly
            // Only the main extension (like classic), Ex: if the "b" of wii remote is pressed, b = release
        };

        enum PlayerState
        {
            eNotConnected,
            eBubble,
            ePlaying
        };

    private:
    protected:
        ButtonClassMask m_buttonClasses[JOY_MAX_BUT];
        u32 m_environmentInput;
        ButtonMode m_buttonMode;
        bbool m_inMenu;
        bbool m_focused;

        PlayerState m_connectedPlayers[JOY_MAX_COUNT];
        float m_axes[JOY_MAX_COUNT][JOY_MAX_AXES];
        PressStatus m_buttons[JOY_MAX_COUNT][JOY_MAX_BUT];
    private:
        bbool m_PadConnected[JOY_MAX_COUNT]{};
        PadType m_PadType[JOY_MAX_COUNT]{};
        InputDeviceType m_lastUsedInputDevice[JOY_MAX_COUNT]{};
        bbool m_keyboardShareEnabled;

        bbool m_useShakeAttack;
        f32 m_threshold;
        f32 m_delay;
        f32 m_swimMaxSpeed;
        f32 m_swimSmooth;
        bbool m_runUseB;
        bbool m_runUseShake;
        f32 m_runTimerStop;
    public:
        /**
        Registers a new listener for input events. you must create a class that inherits from Interface_InputListener
        Doublons are not checked and will lead to a crash.
        @param      _listener   the listener to register
        @param      _priority   listener update order (ascending)
        @exception              asserts if  _listener is null.
        */
        virtual void addListener(Interface_InputListener* _listener, u32 _priority);

        /**
        Unregisters an existing listener for input events.
        @param      _listener   the listener to register
        @param      _priority   listener update order (ascending)
        @exception              asserts if  _listener is null.
        */
        virtual void removeListener(Interface_InputListener* _listener);

        /**
        Note: No need to register listeners to call this method.
        @param      _key    the key to be tested (see KEY_ static i32 collection in InputAdapter.h)
        @return             btrue if the key is currently pressed
        @exception          asserts if  _key is out of the supported range
        */
        virtual bbool isKeyPressed(i32 _key) const;

        virtual bbool isMousePressed(MouseButton _but) const;

        /**
        Note: No need to register listeners to call this method.
        @param      _x        mouse X destination variable
        @param      _y        mouse Y destination variable
        */
        virtual void getMousePos(i32& _x, i32& _y) const;

        /**
        @return                the number of connected game pads
        */
        virtual u32 getGamePadCount() { return 0; }


        virtual void flushKeys();

        /**
        Queries the current position of one or more axes of a GamePad. The positional values are
        returned in an array (_pos), where the first element represents the first axis of the GamePad (normally the X
        axis). Each position is in the range -1.0 to 1.0. Where applicable, the positive direction of an axis is
        right, forward or up, and the negative direction is left, back or down.
        If _numAxes exceeds the number of axes supported by the GamePad, or if the GamePad is not available, the
        unused elements in the pos array will be set to 0.0 (zero).
        example:
        float position[ 2 ];
        getGamePadPos( 0, position, 2 ); // x axis value = position[0], y axis value = position[1]

        @param        _pad        the index of the GamePad
        @param        _pos        the array where values should be stored
        @param        _numAxes    how many axes do we need to get

        */
        virtual void getGamePadPos(u32 _environment, u32 _pad, float* _pos, u32 _numAxes) const;
        /**
        Queries the current state of one or more buttons of a GamePad. The button states are
        returned in an array, where the first element represents the first button of the GamePad.
        If _numButtons exceeds the number of buttons supported by the GamePad, or if the GamePad is not
        available, the unused elements in the buttons array will be set to "Released" (see enum PressStatus).

        @param        _pad        The index of the GamePad.
        @param        _buttons    An array that will hold the button states for all requested buttons.
        @param        _numButtons    Specifies how many buttons should be returned.
        */
        virtual void getGamePadButtons(u32 _environment, u32 _pad, PressStatus* _buttons, u32 _numButtons) const;
        /**
        Queries button classes (bit mask). The button classes are
        returned in an array, where the first element represents the first button of the GamePad.
        If _numButtons exceeds the number of buttons supported by the GamePad, or if the GamePad is not
        available, the unused elements in the buttons array will be set to "BUTTONCLASS_UNKNOWN" (see enum PressStatus).

        @param        _pad              The index of the GamePad.
        @param        _buttonClasses    Bit combination of BUTTONCLASS* values.
        @param        _numButtons       Specifies how many buttons should be returned.
        */
        virtual void getGamePadButtonClasses(u32 _pad, ButtonClassMask* _buttonClasses, u32 _numButtons) const;

        /**
        @param        _numPad           The index of the GamePad.
        @param        _leftMotorSpeed   Speed between 0-1.
        @param        _rightMotorSpeed  Speed between 0-1.
        */
        virtual void padVibration(u32 _numPad, f32 _leftMotorSpeed, f32 _rightMotorSpeed);

        /**
        Set/Get if the pad is connected
        @param        _numPad           The index of the GamePad.
        */
        ITF_INLINE bbool isPadConnected(u32 _numPad) const
        {
            ITF_ASSERT(_numPad < JOY_MAX_COUNT);
            return (_numPad < JOY_MAX_COUNT) ? m_PadConnected[_numPad] : bfalse;
        }

        ITF_INLINE void setPadConnected(u32 _numPad, bbool _connected)
        {
            ITF_ASSERT(_numPad < JOY_MAX_COUNT);
            if (_numPad < JOY_MAX_COUNT)
                m_PadConnected[_numPad] = _connected;
        }

        /**
        Set the button mode, if mixed, all the buttons are listened, if extensionOnly ..
        @param      _buttonMode         The button mode (see Enum)
        */
        ITF_INLINE void setButtonModeStatus(ButtonMode _buttonMode) { m_buttonMode = _buttonMode; }

        /**
        Set/Get the status of a PAD. If the pad is disable, it can be actived
        @param      _numPad         The index of the GamePad
        */
        virtual void setIsPadEnable(u32 _numPad, bbool _enable)
        {
        }

        virtual bbool getIsPadEnable(u32 _numPad) const { return btrue; }

        /**
        Set/Get the type of pad corresponding to the target and the pad extension (for WII)
        @param        _numPad           The index of the GamePad.
        */
        ITF_INLINE PadType getPadType(u32 _numPad) const
        {
            ITF_ASSERT(_numPad < JOY_MAX_COUNT);
            return (_numPad < JOY_MAX_COUNT) ? m_PadType[_numPad] : Pad_Other;
        }

        ITF_INLINE void setPadType(u32 _numPad, PadType _type)
        {
            ITF_ASSERT(_numPad < JOY_MAX_COUNT);
            if (_numPad < JOY_MAX_COUNT)
                m_PadType[_numPad] = _type;
        }

        // debug input for menus / context icons etc
        ITF_INLINE PadType getDebugInputPadType(u32 _numPad) const
        {
#ifndef ITF_FINAL
            if (SYSTEM_ADAPTER->hasDebugInput())
            {
                String8 debugInput = SYSTEM_ADAPTER->getDebugInput();
                static const String8 s_WII = "WII";
                static const String8 s_PS3 = "PS3";
                static const String8 s_PS5 = "PS5";
                static const String8 s_VITA = "VITA";
                static const String8 s_CTR = "CTR";
                static const String8 s_NX_Joycon = "NX_Joycon";
                static const String8 s_NX_Joycon_Dual = "NX_Joycon_Dual";
                static const String8 s_NX_Joycon_Pro = "NX_Joycon_Pro";
                if (debugInput == s_WII)
                {
                    // simulate some different Wii pads
                    if (_numPad == 0) return Pad_WiiClassic;
                    if (_numPad == 1) return Pad_WiiNunchuk;
                    else return Pad_WiiSideWay;
                }
                else if (debugInput == s_PS3)
                {
                    return Pad_PS3;
                }
                else if (debugInput == s_PS5)
                {
                    return Pad_PS5;
                }
                else if (debugInput == s_VITA)
                {
                    return Pad_Vita;
                }
                else if (debugInput == s_CTR)
                {
                    return Pad_CTR;
                }
                else if (debugInput == s_NX_Joycon)
                {
                    return Pad_NX_Joycon;
                }
                else if (debugInput == s_NX_Joycon_Dual)
                {
                    return Pad_NX_Joycon_Dual;
                }
                else if (debugInput == s_NX_Joycon_Pro)
                {
                    return Pad_NX_Pro;
                }
            }
#endif
            return getPadType(_numPad);
        }

        /**
        Start to rumble with a pad
        @param        _numPad           The index of the GamePad.
        @param        _time             Time in ms to rumble.
        */
        virtual void startRumble(u32 _numPad, f64 _time, f32 _leftMotorSpeed, f32 _rightMotorSpeed)
        {
        }

        virtual void stopRumble(u32 _numPad)
        {
        }

        // pauseAllRumble(btrue): stop all rumble, and don't start any other until it pauseAllRumble(bfalse) is called.
        virtual void pauseAllRumble(bbool _pause)
        {
        }

        InputAdapter();
        virtual ~InputAdapter(); // always declare virtual destructor for adapters

        virtual void onMouseButton(MouseButton _but, PressStatus _status);
        virtual void onKey(i32 _key, PressStatus _status);
        virtual void onMouseWheel(i32 _wheelValue);
        virtual void onMousePos(i32 _x, i32 _y);

        virtual void dispatchEventsToListeners();
        void disableEnvironment(u32 _flag) { m_environmentInput = m_environmentInput & (~_flag); }
        void enableEnvironment(u32 _flag) { m_environmentInput |= _flag; }

        virtual void setFocus() { m_focused = true; }
        virtual void unsetFocus() { m_focused = false; }

        virtual void platformUpdateKeyboardState();
        virtual void UpdatePads() { ITF_ASSERT_MSG(0, "Not implemented"); }
        virtual void updateAllInputState();
        void ResetInputState();
        virtual const char* GetControllerTypeName(u32 padIndex) const
        {
            ITF_ASSERT_MSG(0, "Not implemented");
            return nullptr;
        }
        void setUSEShakeAttack(bbool useAttack) { m_useShakeAttack = useAttack; }
        void setThreshold(f32 threshold) { m_threshold = threshold; }
        void setDelay(f32 delay) { m_delay = delay; }
        void setSwimMaxSpeed(f32 maxspeed) { m_swimMaxSpeed = maxspeed; }
        void setSwimSmooth(f32 smooth) { m_swimSmooth = smooth; }
        void setRunUseB(bbool useb) { m_runUseB = useb; }
        void setRunUseShake(bbool useshake) { m_runUseShake = useshake; }
        void setRunTimerStop(f32 timer) { m_runTimerStop = timer; }

        f32 getThreshold() const { return m_threshold; }
        bbool getUSEShakeAttack() const { return m_useShakeAttack; }
        f32 getDelay() const { return m_delay; }
        f32 getSwimSmooth() const { return m_swimSmooth; }
        f32 getSwimMaxSpeed() const { return m_swimMaxSpeed; }
        bbool getRunUseB() const { return m_runUseB; }
        bbool getRunUseShake() const { return m_runUseShake; }
        f32 getRunTimerStop() const { return m_runTimerStop; }

        /**
        Gets the validation/cancellation button
        */
        virtual u32 getValidationButton() const { return m_joyButton_A; }
        virtual u32 getCancellationButton() const { return m_joyButton_B; }

        //CASA::CTR
        virtual bbool IsTPPressed() const { return bfalse; }
        virtual bbool IsTPJustPressed() const { return bfalse; }
        virtual bbool IsTPReleased() const { return btrue; }
        virtual bbool IsTPJustReleased() const { return bfalse; }

        virtual u16 GetTPPosX() { return 0; }
        virtual u16 GetTPPosY() { return 0; }
        //CASA>

        virtual void OnControllerConnected(u32 _padIndex,i32 _deviceID= -1,i32 _deviceOutputID =-1,bool isSony = false);
        virtual void OnControllerDisconnected(u32 _padIndex);
        void SetKeyboardControllerSharing(bbool enabled) { m_keyboardShareEnabled = enabled; }
        bbool IsKeyboardControllerSharingEnabled() const { return m_keyboardShareEnabled; }

        // Track last used input device per player for proper icon display
        ITF_INLINE InputDeviceType getLastUsedInputDevice(u32 _player) const
        {
            return (_player < JOY_MAX_COUNT) ? m_lastUsedInputDevice[_player] : InputDevice_None;
        }

        ITF_INLINE void setLastUsedInputDevice(u32 _player, InputDeviceType _deviceType)
        {
            if (_player < JOY_MAX_COUNT)
            {
                m_lastUsedInputDevice[_player] = _deviceType;
            }
        }

        // Get the appropriate PadType based on last used input (keyboard or actual gamepad type)
        PadType getLastUsedPadType(u32 _player) const;

    protected:
        virtual PressStatus getKeyboardStatus(i32 key) const;
        virtual u32 getKeyboardPressTime(i32 key) const;
    };

#define INPUT_ADAPTER InputAdapter::getptr()


    /*
        TO AVOID MULTIPLE INHERITENCE ISSUES, PLEASE CONSIDERHAVING ONLY PURE VIRTUAL FUNCTIONS
        IN LISTENER AND NO MEMBER
    */

    class Interface_InputListener
    {
    public:
        /**
        Called for all listeners when a mouse button event occurs
        @param  _but    the button for which the event occurs
        @param  _status the new status for _but button
        @return         btrue to keep on notifying other listeners. bfalse to break the notification loop (for this event only)
        */
        virtual bbool onMouseButton(InputAdapter::MouseButton _but, InputAdapter::PressStatus _status) = 0;
        //  { ITF_UNUSED(_but); ITF_UNUSED(_status); return btrue; }

        /**
        Called for all listeners when a mouse move event occurs
        @param  _x        mouse x position
        @param  _y        mouse y position
        @return         btrue to keep on notifying other listeners. bfalse to break the notification loop (for this event only)
        */
        virtual bbool onMousePos(i32 _x, i32 _y) = 0; //   { ITF_UNUSED(_x); ITF_UNUSED(_y); return btrue; }

        /**
        Called for all listeners when a mouse wheel event occurs
        @param  _wheelValue the mouse wheel's current value
        @param  _wheelDelta the delta since last wheel event
        @return             btrue to keep on notifying other listeners. bfalse to break the notification loop (for this event only)
        */
        virtual bbool onMouseWheel(i32 _wheelValue, i32 _wheelDelta) = 0;
        //   { ITF_UNUSED(_wheelValue); ITF_UNUSED(_wheelDelta); return btrue; }

        /**
        Called for all listeners when a mouse wheel event occurs
        @param  _key        the key  for which an event occured
        @param  _status     the key's new status
        @return             btrue to keep on notifying other listeners. bfalse to break the notification loop (for this event only)
        */
        virtual bbool onKey(i32 _key, InputAdapter::PressStatus _status) = 0;
        //   { ITF_UNUSED(_key); ITF_UNUSED(_status); return btrue; }
    };
} // namespace ITF

#endif //_ITF_INPUTADAPTER_H_

