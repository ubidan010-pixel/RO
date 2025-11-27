#include "precompiled_engine.h"

#ifdef ITF_SUPPORT_IMGUI

#ifndef _ITF_IMGUIADAPTER_H_
#include "engine/AdaptersInterfaces/ImGui/ImGuiAdapter.h"
#endif //_ITF_IMGUIADAPTER_H_

#ifndef _ITF_EDITOR_H_
#include "editor/Editor.h"
#endif //_ITF_EDITOR_H_

#ifndef _ITF_PLUGINGATEWAY_H_
#include "PluginGateway/PluginGateway.h"
#endif //_ITF_PLUGINGATEWAY_H_

#include "imgui/imgui.h"
#include "ImGuiEngineDebugTool.h"
#include "ImGuiMainItemMenu.h"
#include "ImGuiMenuPath.h"
#include "engine/imgui/ImGuiApplicationTool.h"
#include "engine/imgui/editor/ImGuiActorEditorTool.h"
#include <imgui_internal.h>

namespace ITF
{
    ImGuiAdapter::ImGuiAdapter() = default;

    ImGuiAdapter::~ImGuiAdapter()
    {
        shutdown();
    }

    bbool ImGuiAdapter::initialize()
    {
        if (m_initialized)
            return true;

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO & io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        ImGui::StyleColorsDark();

        if (!initialize_internal())
        {
            ImGui::DestroyContext();
            return false;
        }

        onAwake();

        m_initialized = true;
        return true;
    }

    void ImGuiAdapter::shutdown()
    {
        if (!m_initialized)
            return;

        shutdown_internal();

        ImGui::DestroyContext();
        m_initialized = false;
    }

    void ImGuiAdapter::newFrame()
    {
        if (!m_initialized || !m_visible)
            return;

        beginFrame_internal();

        ImGui::NewFrame();
    }

    void ImGuiAdapter::onAwake()
    {
        m_tools.clear();
        m_tools.push_back(std::make_unique<ImGuiApplicationTool>());
#if !defined(ITF_FINAL) && defined(ITF_SUPPORT_EDITOR)
        m_tools.push_back(std::make_unique<ImGuiActorEditorTool>());
#endif

        buildMenuPath();
        m_mainItemMenus.clear();
    }

    void ImGuiAdapter::buildMenuPath()
    {
        std::sort(m_tools.begin(), m_tools.end(),
            [](const std::unique_ptr<ImGuiEngineDebugTool>& a, const std::unique_ptr<ImGuiEngineDebugTool>& b)
        {
            return std::string(a->menuPath()) < std::string(b->menuPath());
        });

        m_rootMenu.clear();

        for (const auto& tool : m_tools)
        {
#ifdef ITF_FINAL
            if (!tool->isAvailableInFinal()) continue;
#endif

            const std::string& menuPath = tool->menuPath();
            const std::string& menuItem = tool->menuItemName();

            std::stringstream ss(menuPath);
            std::string segment;
            ImGuiMenuPath* current = nullptr;

            while (std::getline(ss, segment, '/'))
            {
                auto& container = current ? current->subPaths : m_rootMenu;

                auto it = std::find_if(container.begin(), container.end(),
                    [&](const ImGuiMenuPath& mp) { return mp.name == segment; });

                if (it == container.end())
                {
                    container.emplace_back(segment);
                    current = &container.back();
                }
                else
                {
                    current = &(*it);
                }
            }

            if (current)
                current->associatedTools.push_back(tool.get());
        }

        std::function<void(std::vector<ImGuiMenuPath>&)> sortTree = [&](std::vector<ImGuiMenuPath>& nodes)
        {
            std::sort(nodes.begin(), nodes.end(),
                [](const ImGuiMenuPath& a, const ImGuiMenuPath& b)
            {
                return a.name < b.name;
            });

            for (auto& n : nodes)
            {
                std::sort(n.associatedTools.begin(), n.associatedTools.end(),
                    [](const ImGuiEngineDebugTool* a, const ImGuiEngineDebugTool* b)
                {
                    return std::string(a->menuItemName()) < std::string(b->menuItemName());
                });

                sortTree(n.subPaths);
            }
        };

        sortTree(m_rootMenu);
    }

    void ImGuiAdapter::render()
    {
        if (!m_initialized || !m_visible)
            return;

        render_internal();
    }

    void ImGuiAdapter::update(f32 _dt)
    {
        if (!m_initialized)
            return;

        if (isToggleVisiblePressed())
            toggleVisible();

        if (!m_visible)
            return;

        newFrame();
        draw(_dt);
        //render();
    }

    void ImGuiAdapter::draw(f32 _dt)
    {
        if (!m_visible)
            return;

        ImGui::BeginMainMenuBar();

        forceFocusMenuBarIfNeeded();

        for (auto& menu : m_rootMenu)
            menu.DrawMenu();

        drawMainItemMenu();
        drawMenuSeperator();
		drawSingleItemMenu();
        drawFPS(_dt);

        ImGui::EndMainMenuBar();

        for (auto& tool : m_tools)
        {
            if (tool && tool->isActive())
                tool->draw();
        }
        ImGui::Render();
    }

    void ImGuiAdapter::forceFocusMenuBarIfNeeded()
    {
        ImGuiContext* ctx = ImGui::GetCurrentContext();
        if (ctx->NavWindow == nullptr)
        {
            ImGuiWindow* win = ctx->CurrentWindow;
            ctx->NavWindow = win;
            ctx->NavLayer = ImGuiNavLayer_Menu;
            ctx->NavInitRequest = true;
        }
    }

	void ImGuiAdapter::drawMainItemMenu()
	{
		for (const auto& item : m_mainItemMenus)
		{
			const std::string name = item.Name();
			if (ImGui::MenuItem(name.c_str()))
				item.onMenuItem();
		}
	}

    void ImGuiAdapter::drawSingleItemMenu()
    {
#if !defined(ITF_FINAL) && defined(ITF_SUPPORT_EDITOR)
        if (ImGui::MenuItem("Save"))
        {
            PLUGINGATEWAY->onSceneSave();
            EDITOR->saveCurrentWorld(bfalse, bfalse);
        }
        if (ImGui::MenuItem("Undo"))
        {
            UNDOREDO_MANAGER->requestUndo();
        }
        if (ImGui::MenuItem("Redo"))
        {
            UNDOREDO_MANAGER->requestRedo();
        }
#endif
    }

    void ImGuiAdapter::drawMenuSeperator()
    {
        ImVec2 p = ImGui::GetCursorScreenPos();
        float h = ImGui::GetFrameHeight();
        ImU32 col = ImGui::GetColorU32(ImGuiCol_Separator);
        ImGui::GetWindowDrawList()->AddLine(
            ImVec2(p.x, p.y + 2.0f),
            ImVec2(p.x, p.y + h - 2.0f),
            col);
        ImGui::Dummy(ImVec2(0.6f, 0));
        ImGui::SameLine();
    }

    void ImGuiAdapter::drawFPS(f32 _dt)
    {
        char buf[64];
        snprintf(buf, sizeof(buf), "%.0f %.0f", SINGLETONS.m_loopFPS, GFX_ADAPTER->getfPs());

        float text_w = ImGui::CalcTextSize(buf).x;
        float x_right = ImGui::GetWindowWidth() - text_w - ImGui::GetStyle().FramePadding.x;

        ImGui::SameLine(0.0f, 0.0f);
        ImGui::SetCursorPosX(x_right);
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
        ImGui::TextUnformatted(buf);
        ImGui::PopStyleColor();
    }

    void ImGuiAdapter::setVisible(bbool visible)
    {
        m_visible = visible;
    }

#if ITF_WINDOWS
    void ImGuiAdapter::onDeviceLost()
    {
        if (!m_initialized)
            return;

        onDeviceLost_internal();
    }

    void ImGuiAdapter::onDeviceReset()
    {
        if (!m_initialized)
            return;

        onDeviceReset_internal();
    }
#endif
};

#endif //ITF_SUPPORT_IMGUI
