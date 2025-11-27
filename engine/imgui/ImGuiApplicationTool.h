#include "precompiled_engine.h"

#ifdef ITF_SUPPORT_IMGUI

#include <string>
#include <vector>
#include <sstream>

namespace ITF
{
    class ImGuiEngineDebugTool;

    class ImGuiApplicationTool final : public ImGuiEngineDebugTool
    {
    public:
        ImGuiApplicationTool();

        const char* menuPath() const override { return "Application"; }
        const char* menuItemName() const override { return "Info"; }

        void openTool() override;

    protected:
        void drawTool() override;

    private:
    };
}

#endif // ITF_SUPPORT_IMGUI
