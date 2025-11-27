#include "precompiled_engine.h"

#ifdef ITF_SUPPORT_IMGUI
#include "engine/AdaptersInterfaces/ImGui/ImGuiEngineDebugTool.h"
#include "ImGuiApplicationTool.h"

#include "imgui/imgui.h"
#include "core/versioning.h"

namespace ITF
{
    ImGuiApplicationTool::ImGuiApplicationTool() = default;

    void ImGuiApplicationTool::openTool()
    {
        ImGuiEngineDebugTool::openTool();
    }

    void ImGuiApplicationTool::drawTool()
    {
        std::stringstream sb;
        sb << "Engine Version: " << Versioning::getEngineVersion() << "\n";

        std::string content = sb.str();
        ImGui::TextUnformatted(content.c_str());

        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::Button("Quit"))
        {

        }
    }
}

#endif // ITF_SUPPORT_IMGUI
