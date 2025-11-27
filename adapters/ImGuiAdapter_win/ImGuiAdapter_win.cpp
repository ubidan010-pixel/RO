#include "precompiled_ImGuiAdapter_win.h"

#if ITF_SUPPORT_IMGUI
#include "adapters/ImGuiAdapter_win/ImGuiAdapter_win.h"

#ifndef _ITF_INPUTADAPTER_H_
#include "engine/AdaptersInterfaces/InputAdapter.h"
#endif //_ITF_INPUTADAPTER_H_

#define TOGGLE_IMGUI_KEY KEY_F10

#include "imgui.h"
#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_dx9.h"

#include <adapters/GFXAdapter_DirectX9/GFXAdapter_Directx9.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);

namespace ITF {
    static WNDPROC s_OldWndProc = nullptr;

    static LRESULT CALLBACK WndProcHook(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
            return 0;

        ImGuiIO& io = ImGui::GetIO();

        switch (msg)
        {
            // MOUSE
        case WM_MOUSEMOVE:
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
        case WM_MOUSEWHEEL:
        case WM_MOUSEHWHEEL:
            if (io.WantCaptureMouse)
                return 0;
            break;

            // KEYBOARD / TEXT
        case WM_KEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_CHAR:
            if (io.WantCaptureKeyboard && wParam != VK_ESCAPE)
                return 0;
            break;

        case WM_SYSCOMMAND:
            if ((wParam & 0xfff0) == SC_KEYMENU)
                return 0;
            break;
        }

        return CallWindowProc(s_OldWndProc, hWnd, msg, wParam, lParam);
    }

    bbool ImGuiAdapter_win::initialize_internal() {
        m_hwnd = reinterpret_cast<HWND>(SYSTEM_ADAPTER->getMainWindow());
        if (!m_hwnd)
            return false;

        auto* pDX = static_cast<GFXAdapter_Directx9*>(GFX_ADAPTER);
        if (!pDX)
            return false;

        m_device = reinterpret_cast<IDirect3DDevice9*>(pDX->getRenderingDevice());
        if (!m_device)
            return false;

        if (!ImGui_ImplWin32_Init(m_hwnd))
            return false;
        if (!ImGui_ImplDX9_Init(m_device))
        {
            ImGui_ImplWin32_Shutdown();
            return false;
        }

        s_OldWndProc = (WNDPROC)SetWindowLongPtr(m_hwnd, GWLP_WNDPROC, (LONG_PTR)WndProcHook);
        return true;
    }

    void ImGuiAdapter_win::shutdown_internal()
    {
        ImGui_ImplDX9_Shutdown();
        ImGui_ImplWin32_Shutdown();

        if (m_hwnd && s_OldWndProc)
        {
            SetWindowLongPtr(m_hwnd, GWLP_WNDPROC, (LONG_PTR)s_OldWndProc);
            s_OldWndProc = nullptr;
        }
        m_hwnd = nullptr;
        m_device = nullptr;
    }

    void ImGuiAdapter_win::beginFrame_internal()
    {
        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
    }

    void ImGuiAdapter_win::render_internal()
    {
        ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
    }

    void ImGuiAdapter_win::onDeviceLost_internal()
    {
        ImGui_ImplDX9_InvalidateDeviceObjects();
    }

    void ImGuiAdapter_win::onDeviceReset_internal()
    {
        ImGui_ImplDX9_CreateDeviceObjects();
    }

    bbool ImGuiAdapter_win::isToggleVisiblePressed()
    {
        return INPUT_ADAPTER->isKeyPressed(TOGGLE_IMGUI_KEY);
    }
} // namespace ITF
#endif
