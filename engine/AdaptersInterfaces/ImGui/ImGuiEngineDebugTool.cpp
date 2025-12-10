#include "precompiled_engine.h"

#ifdef ITF_SUPPORT_IMGUI
#include "ImGuiEngineDebugTool.h"
#include "engine/AdaptersInterfaces/ImGui/IDebugImgui.h"

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

    void ImGuiEngineDebugTool::registerEntity(IDebugImGui* entity)
    {
        if (!entity)
            return;

        auto it = std::find(m_entities.begin(), m_entities.end(), entity);
        if (it == m_entities.end())
            m_entities.push_back(entity);

        if (m_activeDebugEntities.empty())
        {
            m_activeDebugEntities.push_back(entity);
        }
    }

    void ImGuiEngineDebugTool::unRegisterEntity(IDebugImGui* entity)
    {
        if (!entity)
            return;

        m_entities.erase(std::remove(m_entities.begin(), m_entities.end(), entity), m_entities.end());

        m_activeDebugEntities.erase(std::remove(m_activeDebugEntities.begin(), m_activeDebugEntities.end(), entity), m_activeDebugEntities.end());
    }

    void ImGuiEngineDebugTool::drawEntityPicker()
    {
        if (!useEntityPicker())
            return;

        if (ImGui::BeginMenuBar())
        {
            const int activeCount = (int)m_activeDebugEntities.size();

            std::string menuText = "Pick entity..";
            if (activeCount == 1)
            {
                menuText = "Selected: entity 0";
            }
            else if (activeCount > 1)
            {
                menuText = "Multi selection..";
            }

            if (ImGui::BeginMenu(menuText.c_str()))
            {
                const int count = (int)m_entities.size();
                if (count == 0)
                {
                    ImGui::MenuItem("No entities", nullptr, false, false);
                }
                else
                {
                    for (int i = 0; i < count; ++i)
                    {
                        IDebugImGui* e = m_entities[i];
                        if (!e)
                            continue;

                        char label[64];
                        std::snprintf(label, sizeof(label), "Entity %d (%p)", i, (void*)e);

                        const int idxActive = (int)std::distance(
                            m_activeDebugEntities.begin(),
                            std::find(m_activeDebugEntities.begin(), m_activeDebugEntities.end(), e));

                        bool isActive = (idxActive >= 0 && idxActive < (int)m_activeDebugEntities.size());

                        bool selected = ImGui::MenuItem(label, nullptr, isActive);
                        if (selected)
                        {
                            if (!isActive)
                            {
                                m_activeDebugEntities.push_back(e);
                            }
                            else
                            {
                                m_activeDebugEntities.erase(
                                    std::remove(m_activeDebugEntities.begin(), m_activeDebugEntities.end(), e),
                                    m_activeDebugEntities.end());
                            }

                            break;
                        }
                    }
                }

                ImGui::EndMenu();
            }

            if (!m_entities.empty())
            {
                if (ImGui::MenuItem("Clear selection"))
                {
                    m_activeDebugEntities.clear();
                }
            }

            ImGui::EndMenuBar();
        }
    }

    void ImGuiEngineDebugTool::drawActiveEntities()
    {
        if (m_activeDebugEntities.empty())
        {
            ImVec4 red{ 1.0f, 0.4f, 0.4f, 1.0f };
            ImGui::TextColored(red, "No entity selected");
            return;
        }

        if (m_activeDebugEntities.size() == 1)
        {
            IDebugImGui* e = m_activeDebugEntities[0];
            if (e)
                e->displayImGui();
            return;
        }

        IDebugImGui* toRemove = nullptr;

        for (IDebugImGui* e : m_activeDebugEntities)
        {
            if (!e)
                continue;

            ImGui::PushID((void*)e);

            bool headerOpen = ImGui::CollapsingHeader("Entity", ImGuiTreeNodeFlags_AllowItemOverlap);
            ImGui::SameLine();
            if (ImGui::Button("X"))
            {
                toRemove = e;
            }

            if (headerOpen)
            {
                e->displayImGui();
            }

            ImGui::PopID();
        }

        if (toRemove)
        {
            m_activeDebugEntities.erase(
                std::remove(m_activeDebugEntities.begin(), m_activeDebugEntities.end(), toRemove),
                m_activeDebugEntities.end());
        }
    }

    void ImGuiEngineDebugTool::drawTool()
    {
        drawEntityPicker();
        drawActiveEntities();

        drawToolContent();
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
