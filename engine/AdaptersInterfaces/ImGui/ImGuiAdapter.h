#pragma once

#ifndef _ITF_IMGUIADAPTER_H_
#define _ITF_IMGUIADAPTER_H_

#ifdef ITF_SUPPORT_IMGUI

#ifndef _ITF_TEMPLATESINGLETON_H_
#include "core/templateSingleton.h"
#endif //_ITF_TEMPLATESINGLETON_H_

#include <algorithm>
#include <sstream>
#include "IDebugImGui.h"

namespace ITF
{
    class ImGuiEngineDebugTool;
    class ImGuiMainItemMenu;
    struct ImGuiMenuPath;

    class ImGuiAdapter :public TemplateSingleton<ImGuiAdapter>
    {
    public:
        ImGuiAdapter();
        virtual ~ImGuiAdapter();

        bbool initialize();
        void shutdown();
        void newFrame();
        void render();
        void update(f32 _dt);

        void setVisible(bbool visible);
        bool isVisible() const { return m_visible; }
        void toggleVisible() { setVisible(!m_visible); }

#if ITF_WINDOWS
        void onDeviceLost();
        void onDeviceReset();
#endif

        void draw(f32 _dt);
        void drawMainItemMenu();
        void drawSingleItemMenu();
        void drawMenuSeperator();
        void drawFPS(f32 _dt);

        void forceFocusMenuBarIfNeeded();

        template<typename T>
        void registerEntity(IDebugImGui* entity)
        {
            if (!entity)
                return;

            const ImGuiToolTypeId targetId = T::GetStaticTypeId();

            for (auto& tool : m_tools)
            {
                if (!tool)
                    continue;

                if (tool->getTypeId() == targetId)
                {
                    T* typed = static_cast<T*>(tool.get());
                    typed->registerEntity(entity);
                    return;
                }
            }
        }

        template<typename T>
        void unRegisterEntity(IDebugImGui* entity)
        {
            if (!entity)
                return;

            const ImGuiToolTypeId targetId = T::GetStaticTypeId();

            for (auto& tool : m_tools)
            {
                if (!tool)
                    continue;

                if (tool->getTypeId() == targetId)
                {
                    T* typed = static_cast<T*>(tool.get());
                    typed->unRegisterEntity(entity);
                    return;
                }
            }
        }

    protected:
        virtual bbool initialize_internal() { return bfalse; }
        virtual void shutdown_internal() {}
        virtual void beginFrame_internal() {}
        virtual void render_internal() {}
        virtual bbool isToggleVisiblePressed() { return bfalse; }
#if ITF_WINDOWS
        virtual void onDeviceLost_internal() {}
        virtual void onDeviceReset_internal() {}
#endif

    private:
        bbool m_initialized = false;
        bbool m_visible = true;

        std::vector<ImGuiMenuPath> m_rootMenu;
        std::vector<std::unique_ptr<ImGuiEngineDebugTool>> m_tools;
        std::vector<ImGuiMainItemMenu> m_mainItemMenus;

        void onAwake();
        void buildMenuPath();
    };
} // namespace ITF
#endif
#endif // _ITF_IMGUIADAPTER_H_
