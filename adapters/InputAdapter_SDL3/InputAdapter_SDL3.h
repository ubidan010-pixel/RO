#ifndef _ITF_INPUTADAPTER_SDL3_H_
#define _ITF_INPUTADAPTER_SDL3_H_
#ifndef _ITF_INPUTADAPTER_H_
#include "engine/AdaptersInterfaces/InputAdapter.h"
#endif //_ITF_INPUTADAPTER_H_
#ifndef _ITF_SAFEARRAY_H_
#include "core/container/SafeArray.h"
#endif //_ITF_SAFEARRAY_H_
#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_gamepad.h>
#include <mmdeviceapi.h>

namespace ITF
{
    class SDLGamepad
    {
    public:
        SDLGamepad();
        ~SDLGamepad();

        ITF_INLINE u32 getId() { return m_id; }
        ITF_INLINE void setId(u32 _id) { m_id = _id; }
        ITF_INLINE SDL_JoystickID getJoystickId() const { return m_joystickId; }
        ITF_INLINE SDL_Gamepad* getGamepad() { return m_gamepad; }
        ITF_INLINE const SDL_Gamepad* getGamepad() const { return m_gamepad; }

        bool initialize(u32 playerIndex);
        void updateInputState();
        void cleanup();
        float getAxis(u32 _axis) const;
        InputAdapter::PressStatus getButton(u32 _button) const;
        bool isConnected() const;
        uint32_t m_deviceID;
        uint32_t m_deviceOutputID;
        InputAdapter::PadType m_padType;
        bbool isSonyController() { return m_padType == InputAdapter::Pad_PS4 || m_padType == InputAdapter::Pad_PS5; }

    private:
        void updateButtonState(u32 buttonIndex, bool pressed);
        void updateAxisState(u32 axisIndex, f32 value);

        SDL_Gamepad* m_gamepad;
        SDL_JoystickID m_joystickId;
        u32 m_id;
        bool m_connected;
        float m_axes[JOY_MAX_AXES];
        InputAdapter::PressStatus m_buttons[JOY_MAX_BUT];
    };

    class SDLInput
    {
    public:
        SDLInput();
        ~SDLInput();

        bool initialize(InputAdapter*);
        void cleanup();
        void updateInputState();
        void refreshGamepadList();

        SDLGamepad m_gamepads[JOY_MAX_COUNT];
        u32 m_gamepadCount;
        bool m_initialized;
        int m_scePadHandles[JOY_MAX_COUNT];
        SDL_Window* m_hiddenWindow;
    private:
        void handleGamepadConnected(SDL_JoystickID instanceId);
        void handleGamepadDisconnected(SDL_JoystickID instanceId);
        void setGamepadConnected(u32 index, bool connected, InputAdapter::PadType padType);
        void notifyDeviceConnectEvent(u32 padIndex, InputAdapter::PadType padType, bbool isConnected);
        bbool checkExistDeviceId(u32 deviceId);
        void updateSonyControllerDeviceIds(u32 padIndex, SDLGamepad& gamepad);
        void initScePad(int* outScePadHandles);
        void getScePadDeviceId(int padHandle, uint32_t& outResolvedId);
        bool getMMDeviceFromPadHandle(wchar_t const* containerInfo, IMMDevice*& outMmDevice);
        InputAdapter* m_adapter;
    };

    class InputAdapter_SDL3 : public InputAdapter
    {
    public:
        InputAdapter_SDL3();
        ~InputAdapter_SDL3() override;
        u32 getGamePadCount() override;
        const char* GetControllerTypeName(u32 padIndex) const override;
        void UpdatePads() override;
        void addListener(Interface_InputListener* _listener, u32 _priority) override;
        void removeListener(Interface_InputListener* _listener) override;
        bbool isKeyPressed(i32 _key) const override;
        bbool isMousePressed(MouseButton _but) const override;
        void getMousePos(i32& _x, i32& _y) const override;
        void flushKeys() override;
        void onMouseButton(MouseButton _but, PressStatus _status) override;
        void onMouseWheel(i32 _wheelValue) override;
        void onMousePos(i32 _x, i32 _y) override;
        void onKey(i32 _key, PressStatus _status) override;
        void dispatchEventsToListeners() override;
        void platformUpdateKeyboardState() override;
        PressStatus getKeyboardStatus(i32 key) const override;
        u32 getKeyboardPressTime(i32 key) const override;
        i32 GetFirstPressedRawKey() const override;

    protected:
        void OnPCControlModeChanged(PCControlMode previous, PCControlMode current) override;

    private:
        enum EditorEventType : i32
        {
            IA_EventKey = 0,
            IA_EventMouseButton,
            IA_EventMousePos,
            IA_EventMouseWheel,
        };

        struct ListenerEntry
        {
            Interface_InputListener* m_listener;
            u32 m_priority;
        };

        struct EventKeyPress
        {
            i32 m_key;
            PressStatus m_status;
        };

        struct EventMouseButton
        {
            MouseButton m_but;
            PressStatus m_status;
        };

        struct EventMouseChange
        {
            i32 m_x;
            i32 m_y;
        };

        struct EditorEvent
        {
            EditorEventType m_eventType;
            union
            {
                EventKeyPress m_key;
                EventMouseButton m_but;
                EventMouseChange m_val;
            };
        };

        void pushKeyEvent(i32 _key, PressStatus _status);
        void pushMouseButtonEvent(MouseButton _but, PressStatus _status);
        void pushMousePosEvent(i32 _x, i32 _y);
        void pushMouseWheelEvent(i32 _wheel, i32 _delta);
        void updateKeyboardForPlayer0();

        SDLInput m_sdlInput;
        i32 m_slotGamepad[JOY_MAX_COUNT];
        SafeArray<ListenerEntry> m_listeners;
        SafeArray<EditorEvent> m_eventPool;
        i32 m_lastWheelValue;
        i32 m_lastMouseX;
        i32 m_lastMouseY;
        bbool m_keys[KEY_COUNT];
        f32 m_keysReleaseTime[KEY_COUNT];
        bbool m_leftMBIsPressed;
        bbool m_rightMBIsPressed;
        bbool m_middleMBIsPressed;
        PressStatus m_keyStatus[KEY_COUNT];
        u32 m_keyPressTime[KEY_COUNT];
    };
}
#endif //_ITF_INPUTADAPTER_SDL3_H_
