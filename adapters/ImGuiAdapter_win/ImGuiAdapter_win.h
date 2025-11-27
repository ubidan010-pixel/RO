
#pragma once

#if defined(ITF_SUPPORT_IMGUI) && defined(ITF_WINDOWS)

#ifndef _ITF_IMGUIADAPTER_H_
#include "engine/AdaptersInterfaces/ImGui/ImGuiAdapter.h"
#endif //_ITF_IMGUIADAPTER_H_

#include <d3d9.h>

#define TOGGLE_IMGUI_KEY KEY_F10

namespace ITF {
    class ImGuiAdapter_win : public ImGuiAdapter
    {
    public:

        ImGuiAdapter_win() = default;
        ~ImGuiAdapter_win() = default;

    protected:
        bbool               initialize_internal() override;
        void                shutdown_internal() override;
        void                beginFrame_internal() override;
        void                render_internal() override;
        bbool               isToggleVisiblePressed() override;
#if ITF_WINDOWS
        void                onDeviceLost_internal() override;
        void                onDeviceReset_internal() override;
#endif

    private:
        HWND                m_hwnd = nullptr;
        IDirect3DDevice9*   m_device = nullptr;
    };
} // namespace ITF

#endif
