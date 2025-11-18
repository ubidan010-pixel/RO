#include "precompiled_engine.h"

#ifdef ITF_WINDOWS

#ifndef _ITF_WININPUTADAPTER_H_
#include "engine/AdaptersInterfaces/WinInputAdapter.h"
#endif //_ITF_WININPUTADAPTER_H_

#include <limits>

#ifndef ITF_SYSTEMADAPTER_WIN_H_
#include "adapters/SystemAdapter_win/SystemAdapter_win.h"
#endif

namespace ITF
{
    namespace
    {
        ITF_INLINE i32 TranslateVirtualKey(u32 vk)
        {
            switch (vk)
            {
            case VK_ESCAPE: return KEY_ESC;
            case VK_RETURN: return KEY_ENTER;
            case VK_CLEAR: return KEY_DEL;
            case VK_BACK: return KEY_BACKSPACE;
            case VK_TAB: return KEY_TAB;
            case VK_SPACE: return KEY_SPACE;
            case VK_PRIOR: return KEY_PAGEUP;
            case VK_NEXT: return KEY_PAGEDOWN;
            case VK_END: return KEY_END;
            case VK_HOME: return KEY_HOME;
            case VK_LEFT: return KEY_LEFT;
            case VK_UP: return KEY_UP;
            case VK_RIGHT: return KEY_RIGHT;
            case VK_DOWN: return KEY_DOWN;
            case VK_INSERT: return KEY_INSERT;
            case VK_DELETE: return KEY_DEL;
            case VK_NUMPAD0: return KEY_KP_0;
            case VK_NUMPAD1: return KEY_KP_1;
            case VK_NUMPAD2: return KEY_KP_2;
            case VK_NUMPAD3: return KEY_KP_3;
            case VK_NUMPAD4: return KEY_KP_4;
            case VK_NUMPAD5: return KEY_KP_5;
            case VK_NUMPAD6: return KEY_KP_6;
            case VK_NUMPAD7: return KEY_KP_7;
            case VK_NUMPAD8: return KEY_KP_8;
            case VK_NUMPAD9: return KEY_KP_9;
            case VK_MULTIPLY: return KEY_KP_MULTIPLY;
            case VK_ADD: return KEY_KP_ADD;
            case VK_SUBTRACT: return KEY_KP_SUBTRACT;
            case VK_DECIMAL: return KEY_KP_DECIMAL;
            case VK_DIVIDE: return KEY_KP_DIVIDE;
            case VK_F1: return KEY_F1;
            case VK_F2: return KEY_F2;
            case VK_F3: return KEY_F3;
            case VK_F4: return KEY_F4;
            case VK_F5: return KEY_F5;
            case VK_F6: return KEY_F6;
            case VK_F7: return KEY_F7;
            case VK_F8: return KEY_F8;
            case VK_F9: return KEY_F9;
            case VK_F10: return KEY_F10;
            case VK_F11: return KEY_F11;
            case VK_F12: return KEY_F12;
            case VK_F13: return KEY_F13;
            case VK_F14: return KEY_F14;
            case VK_F15: return KEY_F15;
            case VK_F16: return KEY_F16;
            case VK_F17: return KEY_F17;
            case VK_F18: return KEY_F18;
            case VK_F19: return KEY_F19;
            case VK_F20: return KEY_F20;
            case VK_F21: return KEY_F21;
            case VK_F22: return KEY_F22;
            case VK_F23: return KEY_F23;
            case VK_F24: return KEY_F24;
            default:
                return (vk < 256) ? static_cast<i32>(vk) : KEY_SPECIAL;
            }
        }
    }

    f64 WinInputAdapter::s_lastLeftMouseClick = 0.;

    WinInputAdapter::WinInputAdapter() :
        m_lastWheelValue(0),
        m_lastMouseX(0),
        m_lastMouseY(0),
        m_leftMBIsPressed(bfalse),
        m_rightMBIsPressed(bfalse),
        m_middleMBIsPressed(bfalse)
    {
        ITF_MemSet(m_keys, 0, sizeof(m_keys));
        ITF_MemSet(m_keysReleaseTime, 0, sizeof(m_keysReleaseTime));
        ITF_MemSet(m_keyStatus, 0, sizeof(m_keyStatus));
        ITF_MemSet(m_keyPressTime, 0, sizeof(m_keyPressTime));
    }

    WinInputAdapter::~WinInputAdapter()
    {
        m_listeners.clear();
        m_eventPool.clear();
    }

    void WinInputAdapter::addListener(Interface_InputListener* _listener, u32 _priority)
    {
        ITF_ASSERT(_listener);
        ListenerEntry entry;
        entry.m_listener = _listener;
        entry.m_priority = _priority;
        for (u32 i = 0; i < m_listeners.size(); i++)
        {
            if (m_listeners[i].m_priority > _priority)
            {
                m_listeners.insert(entry, i);
                return;
            }
        }
        m_listeners.push_back(entry);
    }

    void WinInputAdapter::removeListener(Interface_InputListener* _listener)
    {
        for (u32 i = 0; i < m_listeners.size(); i++)
        {
            if (m_listeners[i].m_listener == _listener)
            {
                m_listeners.eraseKeepOrder(i);
                return;
            }
        }
    }

    void WinInputAdapter::onMouseButton(InputAdapter::MouseButton _but, InputAdapter::PressStatus _status)
    {
        switch (_but)
        {
        case MB_Left:
            m_leftMBIsPressed = (_status != Released);
            break;
        case MB_Right:
            m_rightMBIsPressed = (_status == Pressed);
            break;
        case MB_Middle:
            m_middleMBIsPressed = (_status == Pressed);
            break;
        default:
            break;
        }
        pushMouseButtonEvent(_but, _status);
    }

    void WinInputAdapter::onMouseWheel(i32 _wheelValue)
    {
        const i32 delta = _wheelValue - m_lastWheelValue;
        pushMouseWheelEvent(_wheelValue, delta);
        m_lastWheelValue = _wheelValue;
    }

    void WinInputAdapter::flushKeys()
    {
        ITF_MemSet(m_keys, 0, sizeof(m_keys));
    }

    void WinInputAdapter::onKey(i32 _key, InputAdapter::PressStatus _status)
    {
        ITF_ASSERT((_key >= 0) && (_key < KEY_COUNT));

        switch (_status)
        {
        case Pressed:
            m_keys[_key] = btrue;
            break;
        case Released:
            m_keys[_key] = bfalse;
            m_keysReleaseTime[_key] = ELAPSEDTIME;
            break;
        default:
            ITF_ASSERT(0);
        }
        pushKeyEvent(_key, _status);
    }

    bbool WinInputAdapter::isKeyPressed(i32 _key) const
    {
        ITF_ASSERT((_key >= 0) && (_key < KEY_COUNT));
        return m_keys[_key];
    }

    void WinInputAdapter::onMousePos(i32 _x, i32 _y)
    {
        m_lastMouseX = _x;
        m_lastMouseY = _y;
        pushMousePosEvent(_x, _y);
    }

    void WinInputAdapter::getMousePos(i32& _x, i32& _y) const
    {
        _x = m_lastMouseX;
        _y = m_lastMouseY;
    }

    void WinInputAdapter::dispatchEventsToListeners()
    {
        const u32 count = m_eventPool.size();
        for (u32 evtIt = 0; evtIt < count; ++evtIt)
        {
            EditorEvent& editorEvent = m_eventPool[evtIt];
            switch (editorEvent.m_eventType)
            {
            case IA_EventKey:
                {
                    const i32 key = editorEvent.m_key.m_key;
                    const PressStatus status = editorEvent.m_key.m_status;
                    for (u32 it = 0; it < m_listeners.size(); it++)
                    {
                        if (!(m_listeners[it].m_listener->onKey(key, status)))
                            break;
                    }
                }
                break;
            case IA_EventMouseButton:
                {
                    const MouseButton but = editorEvent.m_but.m_but;
                    const PressStatus status = editorEvent.m_but.m_status;
                    for (u32 it = 0; it < m_listeners.size(); it++)
                    {
                        if (!(m_listeners[it].m_listener->onMouseButton(but, status)))
                            break;
                    }
                }
                break;
            case IA_EventMousePos:
                {
                    const i32 x = editorEvent.m_val.m_x;
                    const i32 y = editorEvent.m_val.m_y;
                    for (u32 it = 0; it < m_listeners.size(); it++)
                    {
                        if (!(m_listeners[it].m_listener->onMousePos(x, y)))
                            break;
                    }
                }
                break;
            case IA_EventMouseWheel:
                {
                    const i32 wheelValue = editorEvent.m_val.m_x;
                    const i32 delta = editorEvent.m_val.m_y;
                    for (u32 it = 0; it < m_listeners.size(); it++)
                    {
                        if (!(m_listeners[it].m_listener->onMouseWheel(wheelValue, delta)))
                            break;
                    }
                }
                break;
            }
        }
        m_eventPool.clear();
    }

    bbool WinInputAdapter::isMousePressed(MouseButton _but) const
    {
        switch (_but)
        {
        case MB_Left: return m_leftMBIsPressed;
        case MB_Right: return m_rightMBIsPressed;
        case MB_Middle: return m_middleMBIsPressed;
        default: break;
        }
        return bfalse;
    }

    void WinInputAdapter::platformUpdateKeyboardState()
    {
#if defined(ITF_FINAL) || ITF_ENABLE_EDITOR_KEYBOARD
        for (u32 keyIndex = 0; keyIndex < KEY_COUNT; ++keyIndex)
        {
            const int pressed = GetKeyState(keyIndex) & 0x80;
            m_keyPressTime[keyIndex] += 1;

            if (pressed)
            {
                switch (m_keyStatus[keyIndex])
                {
                case Released:
                case JustReleased:
                    m_keyStatus[keyIndex] = JustPressed;
                    m_keyPressTime[keyIndex] = 0;
                    break;
                case JustPressed:
                case Pressed:
                    m_keyStatus[keyIndex] = Pressed;
                    break;
                default: ;
                }
            }
            else
            {
                switch (m_keyStatus[keyIndex])
                {
                case Released:
                case JustReleased:
                    m_keyStatus[keyIndex] = Released;
                    break;
                case JustPressed:
                case Pressed:
                    m_keyStatus[keyIndex] = JustReleased;
                    break;
                default: ;
                }
            }
        }
#endif
    }

    InputAdapter::PressStatus WinInputAdapter::getKeyboardStatus(i32 key) const
    {
        if (key < 0 || key >= KEY_COUNT)
            return Released;
        return m_keyStatus[key];
    }

    u32 WinInputAdapter::getKeyboardPressTime(i32 key) const
    {
        if (key < 0 || key >= KEY_COUNT)
            return std::numeric_limits<u32>::max();
        return m_keyPressTime[key];
    }

    void WinInputAdapter::pushKeyEvent(i32 _key, PressStatus _status)
    {
        EditorEvent evt;
        evt.m_eventType = IA_EventKey;
        evt.m_key.m_key = _key;
        evt.m_key.m_status = _status;
        m_eventPool.push_back(evt);
    }

    void WinInputAdapter::pushMouseButtonEvent(MouseButton _but, PressStatus _status)
    {
        EditorEvent evt;
        evt.m_eventType = IA_EventMouseButton;
        evt.m_but.m_but = _but;
        evt.m_but.m_status = _status;
        m_eventPool.push_back(evt);
    }

    void WinInputAdapter::pushMousePosEvent(i32 _x, i32 _y)
    {
        EditorEvent evt;
        evt.m_eventType = IA_EventMousePos;
        evt.m_val.m_x = _x;
        evt.m_val.m_y = _y;
        m_eventPool.push_back(evt);
    }

    void WinInputAdapter::pushMouseWheelEvent(i32 _wheel, i32 _delta)
    {
        EditorEvent evt;
        evt.m_eventType = IA_EventMouseWheel;
        evt.m_val.m_x = _wheel;
        evt.m_val.m_y = _delta;
        m_eventPool.push_back(evt);
    }

    void CALLBACK WinInputAdapter::KeyCB(u32 nChar, bool bKeyDown, bool bLAlt, bool bRAlt, bool bLCtrl, bool bRCtrl,
                                         bool bLShift, bool bRShift, void* pUserContext)
    {
        ITF_UNUSED(pUserContext);

        PressStatus status = bKeyDown ? Pressed : Released;
        auto* adapter = static_cast<WinInputAdapter*>(INPUT_ADAPTER);
        if (!adapter) return;

        if (bLCtrl)
        {
            adapter->onKey(KEY_LCTRL, (PressStatus)bLCtrl);
            return;
        }

        if (bRCtrl)
        {
            adapter->onKey(KEY_RCTRL, (PressStatus)bRCtrl);
            return;
        }

        if (bLShift)
        {
            adapter->onKey(KEY_LSHIFT, (PressStatus)bLShift);
            return;
        }

        if (bRShift)
        {
            adapter->onKey(KEY_RSHIFT, (PressStatus)bRShift);
            return;
        }

        if (bLAlt)
        {
            adapter->onKey(KEY_LALT, (PressStatus)bLAlt);
            adapter->onKey(KEY_RALT, (PressStatus)bRAlt);
            return;
        }

        if (bRAlt)
        {
            adapter->onKey(KEY_RALT, (PressStatus)bRAlt);
        }

        adapter->onKey(TranslateVirtualKey(nChar), status);
    }

    void CALLBACK WinInputAdapter::MousePosCB(i32 xPos, i32 yPos, void* pUserContext)
    {
        ITF_UNUSED(pUserContext);
        auto* adapter = static_cast<WinInputAdapter*>(INPUT_ADAPTER);
        if (adapter)
            adapter->onMousePos(xPos, yPos);
    }

    void CALLBACK WinInputAdapter::MouseWheelCB(i32 nMouseWheelDelta, void* pUserContext)
    {
        ITF_UNUSED(pUserContext);
        auto* adapter = static_cast<WinInputAdapter*>(INPUT_ADAPTER);
        if (adapter)
            adapter->onMouseWheel(nMouseWheelDelta);
    }

    void CALLBACK WinInputAdapter::MouseButtonCB(u32 _Button, u32 _action, void* pUserContext)
    {
        ITF_UNUSED(pUserContext);
        auto* adapter = static_cast<WinInputAdapter*>(INPUT_ADAPTER);
        if (!adapter)
            return;

        MouseButton but = static_cast<MouseButton>(_Button);
        PressStatus status = static_cast<PressStatus>(_action);
        if (but == MB_Left && status == Pressed)
        {
            f64 thisTime = SYSTEM_ADAPTER->getTime();
            f64 deltaTime = thisTime - s_lastLeftMouseClick;
            if (deltaTime >= 0. && deltaTime < fDoublePressMaxDuration)
            {
                status = Double_Press;
                s_lastLeftMouseClick = thisTime - fDoublePressMaxDuration - (f64)MTH_EPSILON;
            }
            else
            {
                s_lastLeftMouseClick = thisTime;
            }
        }
        adapter->onMouseButton(but, status);
    }
} // namespace ITF

#endif // ITF_WINDOWS

