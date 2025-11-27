#ifdef ITF_SUPPORT_IMGUI

#include <string>
#include <vector>

namespace ITF
{
    class ImGuiEngineDebugTool;

    struct ImGuiMenuPath
    {
        std::string name;
        std::vector<ImGuiMenuPath> subPaths;
        std::vector<ImGuiEngineDebugTool*> associatedTools;

        explicit ImGuiMenuPath(std::string n) : name(std::move(n)) {}

        void DrawMenu();
    };
}

#endif // ITF_SUPPORT_IMGUI
