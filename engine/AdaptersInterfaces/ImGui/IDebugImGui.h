#pragma once

namespace ITF
{
    struct IDebugImGui
    {
        virtual ~IDebugImGui() {}
#ifdef ITF_SUPPORT_IMGUI
        virtual void registerToImGui() = 0;
        virtual void unRegisterToImGui() = 0;
        virtual void displayImGui() = 0;
#endif // ITF_SUPPORT_IMGUI
    };
}
