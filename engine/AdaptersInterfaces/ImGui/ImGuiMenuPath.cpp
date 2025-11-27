#include "precompiled_engine.h"

#ifdef ITF_SUPPORT_IMGUI
#include "ImGuiMenuPath.h"
#include "ImGuiEngineDebugTool.h"
#include "imgui/imgui.h"

namespace ITF
{
    void ImGuiMenuPath::DrawMenu()
    {
        if (ImGui::BeginMenu(name.c_str()))
        {
            for (auto& sub : subPaths)
                sub.DrawMenu();

            if (!subPaths.empty())
                ImGui::Separator();

            for (auto* tool : associatedTools)
            {
                if (!tool) continue;
                if (ImGui::MenuItem(tool->menuItemName()))
                    tool->openTool();
            }

            ImGui::EndMenu();
        }
    }
}

#endif // ITF_SUPPORT_IMGUI
