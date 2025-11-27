
#pragma once

#if defined(ITF_SUPPORT_IMGUI) && defined(ITF_XBOX_SERIES)
#ifndef _ITF_IMGUIADAPTER_H_
#include "engine/AdaptersInterfaces/ImGui/ImGuiAdapter.h"
#endif //_ITF_IMGUIADAPTER_H_

namespace ITF {
    class ImGuiAdapter_xboxseries : public ImGuiAdapter
    {
    public:

        ImGuiAdapter_xboxseries() = default;
        ~ImGuiAdapter_xboxseries() = default;

    protected:
        bbool               initialize_internal() override;
        void                shutdown_internal() override;
        void                beginFrame_internal() override;
        void                render_internal() override;
        bbool               isToggleVisiblePressed() override;
    };
} // namespace ITF
#endif
