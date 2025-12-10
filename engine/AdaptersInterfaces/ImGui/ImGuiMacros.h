#pragma once

#define IMGUI_TOOL_DECLARE_TYPE(ToolClass)                           \
public:                                                              \
    static ImGuiToolTypeId GetStaticTypeId()                         \
    {                                                                \
        return ImGuiEngineDebugTool::GetStaticTypeId<ToolClass>();   \
    }                                                                \
    virtual ImGuiToolTypeId getTypeId() const override               \
    {                                                                \
        return ToolClass::GetStaticTypeId();                         \
    }
