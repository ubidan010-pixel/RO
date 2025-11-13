#ifndef _ITF_INPUTADAPTER_DINPUT_H_
#define _ITF_INPUTADAPTER_DINPUT_H_
#ifndef _ITF_INPUTADAPTER_H_
#include "engine/AdaptersInterfaces/InputAdapter.h"
#endif //_ITF_INPUTADAPTER_H_

#ifndef _ITF_VEC3D_H_
#include "core/math/vec3d.h"
#endif //_ITF_VEC3D_H_

#define DIRECTINPUT_VERSION 0x0800

#ifdef ITF_WINDOWS
// STB: commented as no more useful to emulate wiimote
// #define USE_WIIMOTE_LIB

#include <dinput.h>
#include <XInput.h>
#endif // ITF_WINDOWS

#ifdef USE_WIIMOTE_LIB
#include "WiiYourself_1.15/wiimote.h"
#endif

namespace ITF
{
    struct XINPUT_DEVICE_NODE
    {
        DWORD dwVidPid;
        XINPUT_DEVICE_NODE* pNext;
    };

    class Joy
    {
    public:
        static const u32 MAX_PAD = 4;
        static const f32 INPUT_DEADZONE;

        Joy();
        ~Joy();

        ITF_INLINE i32 getId() { return m_id; }
        ITF_INLINE void setId(i32 _id) { m_id = _id; }
        ITF_INLINE LPDIRECTINPUTDEVICE8 getDevice() { return m_joyDevice; }
        ITF_INLINE LPDIRECTINPUTDEVICE8& getRefDevice()
        {
            return (LPDIRECTINPUTDEVICE8&)m_joyDevice;
        }

        HRESULT UpdateInputState();
        HRESULT UpdateInputState360(u32 _numPad);
        float getAxe(u32 _axe) const;
        InputAdapter::PressStatus getButton(u32 _button) const;
        void padVibration360(u32 _numPad, f32 _leftMotorSpeed, f32 _rightMotorSpeed, f64 _time = 0.0);
        bool isConnected(u32 _numPad) const;

    protected:
        LPDIRECTINPUTDEVICE8 m_joyDevice;
        i32 m_id;
        float m_axes[JOY_MAX_AXES];
        InputAdapter::PressStatus m_buttons[JOY_MAX_BUT];

        struct RumbleState
        {
            f64 duringTime;
            f64 startTime;
            f64 lastEndTime;
            bbool motorOn;
        };

        struct CONTROLER_360_STATE
        {
            XINPUT_STATE state;
            XINPUT_VIBRATION vibration;
            RumbleState rumble;
            bool connected;
        } m_360_Controllers[MAX_PAD];
    };

#ifdef USE_WIIMOTE_LIB
    class WiimotePC
    {
    public:
        wiimote remote;

        static const u32 MAX_AXES = 6;
        static const u32 MAX_PAD = 4;
        static const f32 INPUT_DEADZONE;

        ITF_INLINE i32 getId() { return m_id; }
        ITF_INLINE void setId(i32 _id) { m_id = _id; }

        void init(i32 id);
        void update();
        void updateMapping();

        bool isActive() const { return m_active; }
        bool isConnected() const { return remote.IsConnected(); }
        bool isNunchuckPlugged() const { return remote.ExtensionType == wiimote_state::NUNCHUK; }
        bool isClassicPlugged() const { return remote.ExtensionType == wiimote_state::CLASSIC; }

        float getAxe(u32 _axe) const;
        InputAdapter::PressStatus getButton(u32 _button) const;
        bool isPressed(u32 _button) const;
        bool isJustPressed(u32 _button) const;

    protected:
        wiimote_state::extension_type m_previousExtension;
        i32 m_id;
        float m_axes[JOY_MAX_AXES];
        InputAdapter::PressStatus m_buttons[JOY_MAX_BUT];
        i32 m_buttonMapping[JOY_MAX_BUT];
        f64 m_timeLastCheck;
        bool m_active;

        f64 m_lastTimePressed;
        u32 m_lastButtonPressed;
        bool m_isDoublePressed;
    };

    class KPadInput
    {
    public:
        HRESULT InitKPadInput();
        void UpdateInputStateWiiMote();

        WiimotePC m_pads[WiimotePC::MAX_PAD];
        bool isActive() const { return m_active; }

    private:
        bool m_active;
    };
#endif // USE_WIIMOTE_LIB
    class DirectInput
    {
    public:
        DirectInput();
        ~DirectInput();

        LPDIRECTINPUT8 m_directInput;

        HRESULT SetupForIsXInputDevice();
        void FreeDirectInput();
        bool IsXInputDevice(const GUID* pGuidProductFromDirectInput);
        void CleanupForIsXInputDevice();
        bool mb_FilterOutXinputDevices;
        XINPUT_DEVICE_NODE* mp_XInputDeviceList;

        static BOOL CALLBACK EnumJoysticksCallback(
            const DIDEVICEINSTANCE* pdidInstance,
            LPVOID pvRef);

        static BOOL CALLBACK EnumObjectsCallback(
            const DIDEVICEOBJECTINSTANCE* pdidoi,
            VOID* pContext);

        HRESULT InitDirectInput(HWND hDlg);

        void RefreshControllerList(InputAdapter* adapter = nullptr);
        bool DetectControllerChanges(InputAdapter* adapter = nullptr);
        bool IsPS5Controller(const DIDEVICEINSTANCE* pdidInstance);
        bool IsPS4Controller(const DIDEVICEINSTANCE* pdidInstance);
        void ClassifyController(u32 padIndex, const DIDEVICEINSTANCE* pdidInstance);
        void HandleControllerDisconnection(u32 padIndex, InputAdapter* adapter = nullptr);
        void HandleControllerConnection(u32 padIndex, InputAdapter* adapter = nullptr);

#ifdef DX_KEYBOARD
        bool InitKeyboard();
        bool KeyPressed(int);
        LPDIRECTINPUTDEVICE8 m_pDIKeyboardDevice;
        static const int m_keyCount = 256;
        char m_KeyBuffer[m_keyCount];
        char m_prevKeyBuffer[m_keyCount];
#endif // DX_KEYBOARD

        GUID m_Instance;
        u32 m_numberpad;
        u32 m_lastScanTime;
        bool m_allowHotSwap;

        struct CONTROLLER_TYPE
        {
            enum Type
            {
                XINPUT = 0,
                DINPUT,
                PS5_CONTROLLER,
                PS4_CONTROLLER,
                GENERIC_CONTROLLER,
                ENUM_FORCE_SIZE_32(0)
            };

            Joy m_joy;
            Type m_typePad;
            bool m_wasConnected;
            u32 m_lastActiveTime;
            GUID m_deviceGUID;
        } m_pad[Joy::MAX_PAD];
    };

    class InputAdapter_DINPUT : public InputAdapter
    {
    public:
        InputAdapter_DINPUT();
        ~InputAdapter_DINPUT();

        HRESULT getDXPadButton(u32 _pad, PressStatus* _buttons, u32 _numButtons);
        u32 getGamePadCount() override;
        const char* GetControllerTypeName(u32 padIndex) const override;
        void padVibration(u32 _numPad, f32 _rightMotorSpeed, f32 _leftMotorSpeed) override;

        void startRumble(u32 _numPad, f64 _time, f32 _leftMotorSpeed, f32 _rightMotorSpeed) override;
        void stopRumble(u32 _numPad) override;
        void updateAllInputState() override;
        void setFocus() override;
        void unsetFocus() override;
        void HandleControllerStateChange();
        ControllerType GetControllerType(InputValue& value) override;
    private:
        void UpdatePads() override;
        PressStatus GetButtonStatus(InputValue) override;
        float GetAxe(InputValue) override;
        bbool IsButtonPressed(InputValue) override;
        u32 getConnectedPadIndex(u32 _padIndex) const;
        void getPadAcc(u32 _numPad, Vec3d* _acc);

    public:
        static bbool IsDirectInput(DirectInput::CONTROLLER_TYPE::Type);
    private:

        DirectInput DXinput;
        static CRITICAL_SECTION s_controllerMutex;
        static bool s_mutexInitialized;


#ifdef USE_WIIMOTE_LIB
        KPadInput KPinput;
#endif // USE_WIIMOTE_LIB
    };

#define INPUT_ADAPTER_DINPUT (static_cast<InputAdapter_DINPUT*>(INPUT_ADAPTER))
} // namespace ITF

#endif //_ITF_INPUTADAPTER_DINPUT_H_
