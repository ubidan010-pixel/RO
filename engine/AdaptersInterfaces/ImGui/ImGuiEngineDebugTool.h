#ifdef ITF_SUPPORT_IMGUI
#include <string>
#include "imgui/imgui.h"

namespace ITF
{
    struct IDebugImGui;

    struct ImGuiWindowStateInfo
    {
        std::string fullPath;
        ImVec2      position{ 0.0f, 0.0f };
        ImVec2      size{ 0.0f, 0.0f };
    };

    class ImGuiEngineDebugTool
    {
    public:
        ImGuiEngineDebugTool();
        virtual ~ImGuiEngineDebugTool() = default;

        virtual const char*         menuPath() const { return ""; }
        virtual const char*         menuItemName() const { return ""; }
        virtual const bbool         isAvailableInFinal() const { return btrue; }

        std::string                 fullPath() const;

        virtual void                registerEntity(IDebugImGui* /*entity*/) {}
        virtual void                unRegisterEntity(IDebugImGui* /*entity*/) {}

        virtual void                openTool();
        virtual void                closeTool();

        bool                        isActive() const { return m_isActive; }
        bool                        isSavable() const { return true; }

        void                        overrideStateInfo(const ImGuiWindowStateInfo& info);

        void                        draw();

    protected:
        virtual ImGuiWindowFlags    getGuiWindowFlags() const { return ImGuiWindowFlags_MenuBar; }

        virtual void                onOpen() {}
        virtual void                onClosed() {}

        virtual void                drawTool() {}

        void                        onPreDraw();
        void                        onPostDraw();

        bool                        m_isActive = false;
        bool                        m_forceStateInfo = false;
        ImGuiWindowStateInfo        m_state;
    };

} // namespace ITF

#endif // ITF_SUPPORT_IMGUI
