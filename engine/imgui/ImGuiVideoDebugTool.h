#include "precompiled_engine.h"

#ifdef ITF_SUPPORT_IMGUI

#include "engine/AdaptersInterfaces/ImGui/ImGuiEngineDebugTool.h"
#include "engine/AdaptersInterfaces/ImGui/IDebugImGui.h"

namespace ITF
{
    class ImGuiVideoDebugTool : public ImGuiEngineDebugTool
    {
        IMGUI_TOOL_DECLARE_TYPE(ImGuiVideoDebugTool);

    public:
        ImGuiVideoDebugTool() = default;
        virtual ~ImGuiVideoDebugTool() = default;

        virtual const char* menuPath() const override { return "Engine"; }
        virtual const char* menuItemName() const override { return "Video"; }

    protected:
        virtual bbool useEntityPicker() const override { return btrue; }
        virtual void drawToolContent() override;
    };

} // namespace ITF

#endif
