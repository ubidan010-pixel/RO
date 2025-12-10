#include "precompiled_engine.h"

#ifdef ITF_SUPPORT_IMGUI

#include "ImGuiVideoDebugTool.h"
#include "imgui/imgui.h"
#include "engine/AdaptersInterfaces/VideoAdapter.h"

namespace ITF
{
    void ImGuiVideoDebugTool::drawToolContent()
    {
        ImGui::Separator();

        const char* samples[6] = {
            "sample",
            "cine_fin",
            "intro",
            "rolling_demo",
            "fresco",
            "fresco_rayman",
        };

        for (int i = 0; i < 6; ++i)
        {
            if (ImGui::Button(samples[i]))
            {
                VIDEO_ADAPTER->open(samples[i], bfalse);
            }   
        }
    }
} // namespace ITF

#endif
