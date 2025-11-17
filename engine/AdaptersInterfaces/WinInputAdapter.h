#ifndef _ITF_WININPUTADAPTER_H_
#define _ITF_WININPUTADAPTER_H_

#ifdef ITF_WINDOWS

#ifndef _ITF_INPUTADAPTER_H_
#include "engine/AdaptersInterfaces/InputAdapter.h"
#endif //_ITF_INPUTADAPTER_H_

namespace ITF
{
    class WinInputAdapter : public InputAdapter
    {
    public:
        using ActionType = InputAdapter::ActionType;
        using MouseButton = InputAdapter::MouseButton;
        using PressStatus = InputAdapter::PressStatus;

        WinInputAdapter();
        ~WinInputAdapter() override = default;

        void flushKeys() override;
        void onMouseButton(MouseButton _but, PressStatus _status) override;
        void onMouseWheel(i32 _wheelValue) override;
        void onMousePos(i32 _x, i32 _y) override;
        void onKey(i32 _key, PressStatus _status) override;
        bbool isKeyPressed(i32 _key) const override;
        bbool isMousePressed(MouseButton _but) const override;
        void getMousePos(i32& _x, i32& _y) const override;
        void dispatchEventsToListeners() override;

        static void CALLBACK KeyCB(
            u32 nChar,
            bool bKeyDown,
            bool bLAlt,
            bool bRAlt,
            bool bLCtrl,
            bool bRCtrl,
            bool bLShift,
            bool bRShift,
            void* pUserContext);
        static void CALLBACK MousePosCB(i32 xPos, i32 yPos, void* pUserContext);
        static void CALLBACK MouseWheelCB(i32 nMouseWheelDelta, void* pUserContext);
        static void CALLBACK MouseButtonCB(u32 _Button, u32 _action, void* pUserContext);

    protected:
        void UpdateAdditionalInputs() override;
        PressStatus GetKeyboardStatusInternal(u32 key) const override;
        u32 GetKeyboardPressTimeInternal(u32 key) const override;
        void ApplyMenuKeyboardOverrides(u32 playerIndex) override;
        bbool ShouldProcessBubbleQuit(u32 playerIndex) const override;
        PressStatus GetMenuButtonOverride(u32 player, ActionType action, JoyButton_Common button) const override;

    private:
        enum EditorEventType : i32
        {
            IA_EventKey = 0,
            IA_EventMouseButton,
            IA_EventMousePos,
            IA_EventMouseWheel,
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
        void UpdateKeyboard();
        static i32 TranslateVirtualKey(u32 vk);

        i32 m_lastWheelValue;
        i32 m_lastMouseX;
        i32 m_lastMouseY;
        bbool m_keys[KEY_COUNT];
        f32 m_keysReleaseTime[KEY_COUNT];
        bbool m_leftMBIsPressed;
        bbool m_rightMBIsPressed;
        bbool m_middleMBIsPressed;
        SafeArray<EditorEvent> m_eventPool;
        PressStatus m_keyStatus[KEY_COUNT];
        u32 m_keyPressTime[KEY_COUNT];
        static f64 s_lastLeftMouseClick;
    };
} // namespace ITF

#endif // ITF_WINDOWS

#endif //_ITF_WININPUTADAPTER_H_
