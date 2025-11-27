#include "precompiled_ImGuiAdapter_PS5.h"

#include "adapters/ImGuiAdapter_PS5/ImGuiAdapter_PS5.h"

#include "imgui/backends/prospero/imgui_impl_agc.h"
#include "imgui/backends/prospero/graphics/context.h"
#include <imgui/backends/prospero/input/input_bridge.h>

#ifndef _ITF_INPUTADAPTER_H_
#include "engine/AdaptersInterfaces/InputAdapter.h"
#endif //_ITF_INPUTADAPTER_H_

//#include "imgui.h"

namespace ITF {
    bbool ImGuiAdapter_PS5::initialize_internal() {
        sce::SampleUtil::Graphics::GraphicsContext* m_graphicsContext = new  sce::SampleUtil::Graphics::GraphicsContext(width, height,
            new sce::SampleUtil::Helper::AsyncAssetLoader(*new sce::SampleUtil::Helper::AsyncAssetLoaderOption()),
            new sce::SampleUtil::Graphics::GraphicsContextOption());

        if (!ImGui_ImplSampleUtil_Init(&m_graphicsContext->m_videoMemory, &m_graphicsContext->m_videoRingMemory, new sce::SampleUtil::System::UserIdManager()))
            return false;
        return btrue;
    }

    void ImGuiAdapter_PS5::shutdown_internal()
    {
        ImGui_ImplSampleUtil_Shutdown();
    }

    void ImGuiAdapter_PS5::beginFrame_internal(f32 _dt)
    {
        auto snap = makeSnapshotFromAdapter(width, height, _dt);
        ImGui_ImplSampleUtil_NewFrame(snap);
    }

    void ImGuiAdapter_PS5::render_internal()
    {
    }

    bbool ImGuiAdapter_PS5::isToggleVisiblePressed()
    {
        return bfalse;
    }

    static inline bool IsKey(int k) {
        return INPUT_ADAPTER->isKeyPressed(k);
    }
    static inline bool BtnDown(ITF::InputAdapter::PressStatus s) {
        return s == ITF::InputAdapter::Pressed || s == ITF::InputAdapter::JustPressed;
    }

    sce::SampleUtil::Input::Snapshot ImGuiAdapter_PS5::makeSnapshotFromAdapter(uint32_t displayWidth,
        uint32_t displayHeight,
        float delta_time) const
    {
        auto& ia = *INPUT_ADAPTER;
        //ia.updateAllInputState();

        sce::SampleUtil::Input::Snapshot s{};
        s.display_w = (float)displayWidth;
        s.display_h = (float)displayHeight;
        s.delta_time = (delta_time > 0.f) ? delta_time : (1.0f / 60.0f);

        // Mouse
        int mx = 0, my = 0;
        ia.getMousePos(mx, my);
        s.mouse_x = (float)mx;
        s.mouse_y = (float)my;
        s.mouse_left = ia.isMousePressed(InputAdapter::MB_Left);
        s.mouse_right = ia.isMousePressed(InputAdapter::MB_Right);
        s.mouse_middle = ia.isMousePressed(InputAdapter::MB_Middle);
        // If you have wheel delta somewhere, convert to +1/-1 and assign to s.mouse_wheel_y.

        // Modifiers
        s.key_ctrl = IsKey(ITF::KEY_LCTRL) || IsKey(ITF::KEY_RCTRL);
        s.key_shift = IsKey(ITF::KEY_LSHIFT) || IsKey(ITF::KEY_RSHIFT);
        s.key_alt = IsKey(ITF::KEY_LALT) || IsKey(ITF::KEY_RALT);
        s.key_super = false;

        // Common keys (expand as you need)
        s.keys[(int)ImGuiKey_Tab] = IsKey(ITF::KEY_TAB);
        s.keys[(int)ImGuiKey_LeftArrow] = IsKey(ITF::KEY_LEFT);
        s.keys[(int)ImGuiKey_RightArrow] = IsKey(ITF::KEY_RIGHT);
        s.keys[(int)ImGuiKey_UpArrow] = IsKey(ITF::KEY_UP);
        s.keys[(int)ImGuiKey_DownArrow] = IsKey(ITF::KEY_DOWN);
        s.keys[(int)ImGuiKey_PageUp] = IsKey(ITF::KEY_PAGEUP);
        s.keys[(int)ImGuiKey_PageDown] = IsKey(ITF::KEY_PAGEDOWN);
        s.keys[(int)ImGuiKey_Home] = IsKey(ITF::KEY_HOME);
        s.keys[(int)ImGuiKey_End] = IsKey(ITF::KEY_END);
        s.keys[(int)ImGuiKey_Insert] = IsKey(ITF::KEY_INSERT);
        s.keys[(int)ImGuiKey_Delete] = IsKey(ITF::KEY_DEL);
        s.keys[(int)ImGuiKey_Backspace] = IsKey(ITF::KEY_BACKSPACE);
        s.keys[(int)ImGuiKey_Space] = IsKey(ITF::KEY_SPACE);
        s.keys[(int)ImGuiKey_Enter] = IsKey(ITF::KEY_ENTER);
        s.keys[(int)ImGuiKey_Escape] = IsKey(ITF::KEY_ESC);

        s.has_gamepad = ia.isPadConnected(0);
        if (s.has_gamepad)
        {
            float axes[JOY_MAX_AXES] = {};
            ia.getGamePadPos(ITF::InputAdapter::EnvironmentEngine, 0, axes, JOY_MAX_AXES);
            s.lstick_x = axes[JOY_AXIS_LX];
            s.lstick_y = axes[JOY_AXIS_LY];
            s.rstick_x = axes[JOY_AXIS_RX];
            s.rstick_y = axes[JOY_AXIS_RY];
            s.l2 = (axes[JOY_AXIS_LT] + 1.0f) * 0.5f; // map [-1..1] -> [0..1]
            s.r2 = (axes[JOY_AXIS_RT] + 1.0f) * 0.5f;

            ITF::InputAdapter::PressStatus btn[JOY_MAX_BUT] = {};
            ia.getGamePadButtons(ITF::InputAdapter::EnvironmentEngine, 0, btn, JOY_MAX_BUT);

            s.face_down = BtnDown(btn[ITF::m_joyButton_A]);       // Cross
            s.face_right = BtnDown(btn[ITF::m_joyButton_B]);       // Circle
            s.face_left = BtnDown(btn[ITF::m_joyButton_X]);       // Square
            s.face_up = BtnDown(btn[ITF::m_joyButton_Y]);       // Triangle

            s.dpad_left = BtnDown(btn[ITF::m_joyButton_DPadL]);
            s.dpad_right = BtnDown(btn[ITF::m_joyButton_DPadR]);
            s.dpad_up = BtnDown(btn[ITF::m_joyButton_DPadU]);
            s.dpad_down = BtnDown(btn[ITF::m_joyButton_DPadD]);

            s.l1 = BtnDown(btn[ITF::m_joyButton_LB]);
            s.r1 = BtnDown(btn[ITF::m_joyButton_RB]);
            s.start_btn = BtnDown(btn[ITF::m_joyButton_Start]); // Options on PS5 mapping
            s.back_btn = BtnDown(btn[ITF::m_joyButton_Back]);  // TouchPad on PS5 mapping
        }

        // If you have a per-frame typed text buffer, assign it here:
        // s.text_utf8 = your_text_buffer;

        return s;
    }
} // namespace ITF
