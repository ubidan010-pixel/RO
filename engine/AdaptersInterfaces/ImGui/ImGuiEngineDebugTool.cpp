#include "precompiled_engine.h"

#ifdef ITF_SUPPORT_IMGUI
#include "ImGuiEngineDebugTool.h"

namespace ITF
{
    ImGuiEngineDebugTool::ImGuiEngineDebugTool()
    {
        m_state.fullPath = fullPath();
    }

    std::string ImGuiEngineDebugTool::fullPath() const
    {
        const char* path = menuPath();
        const char* item = menuItemName();
        if (path && *path)
        {
            std::string s(path);
            s += '/';
            s += (item ? item : "");
            return s;
        }
        return item ? std::string(item) : std::string();
    }

    void ImGuiEngineDebugTool::openTool()
    {
        m_isActive = true;
        onOpen();
    }

    void ImGuiEngineDebugTool::closeTool()
    {
        m_isActive = false;
        onClosed();
    }

    void ImGuiEngineDebugTool::overrideStateInfo(const ImGuiWindowStateInfo& info)
    {
        m_forceStateInfo = true;
        m_state = info;
    }

    void ImGuiEngineDebugTool::onPreDraw()
    {
        if (m_forceStateInfo)
        {
            ImGui::SetNextWindowPos(m_state.position);
            ImGui::SetNextWindowSize(m_state.size);
            m_forceStateInfo = false;
        }

        bool openFlag = m_isActive;
        ImGui::Begin(menuItemName(), &openFlag, getGuiWindowFlags());
        m_isActive = openFlag;
    }

    void ImGuiEngineDebugTool::onPostDraw()
    {
        m_state.position = ImGui::GetWindowPos();
        m_state.size = ImGui::GetWindowSize();
        ImGui::End();
    }

    void ImGuiEngineDebugTool::draw()
    {
        onPreDraw();
        drawTool();
        onPostDraw();

        if (!m_isActive)
            onClosed();
    }

} // namespace ITF
#endif // ITF_SUPPORT_IMGUI
