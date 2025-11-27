
#pragma once

#if defined(ITF_SUPPORT_IMGUI) && defined(ITF_PS5)
#ifndef _ITF_IMGUIADAPTER_H_
#include "engine/AdaptersInterfaces/ImGui/ImGuiAdapter.h"
#endif //_ITF_IMGUIADAPTER_H_

namespace sce::SampleUtil::Input {
    struct Snapshot;
}


namespace ITF {
    class ImGuiAdapter_PS5 : public ImGuiAdapter
    {
    public:

        ImGuiAdapter_PS5() = default;
        ~ImGuiAdapter_PS5() = default;

    protected:
        bbool               initialize_internal() override;
        void                shutdown_internal() override;
        void                beginFrame_internal(f32 _dt) override;
        void                render_internal() override;
        bbool               isToggleVisiblePressed() override;

    private:
        uint32_t width = 3840;
        uint32_t height = 2160;
        sce::SampleUtil::Input::Snapshot      makeSnapshotFromAdapter(uint32_t displayWidth, uint32_t displayHeight, float delta_time) const;
    };
} // namespace ITF
#endif
