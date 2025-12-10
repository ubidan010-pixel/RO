#include "precompiled_engine.h"
#include "engine/video/videoHandle.h"

#ifdef ITF_SUPPORT_IMGUI
#include "engine/AdaptersInterfaces/ImGui/ImGuiAdapter.h"
#include "engine/imgui/ImGuiVideoDebugTool.h"
#include "imgui/imgui.h"
#endif // ITF_SUPPORT_IMGUI

namespace ITF
{

    void videoHandleBase::resetPlaybackClock()
    {
        m_playbackTimeSeconds = 0.0;
        m_useExternalClock = bfalse;
        m_externalClockMs = 0.0;
    }

    void videoHandleBase::setExternalClockMs(f64 ms)
    {
        m_useExternalClock = btrue;
        m_externalClockMs = ms;
        m_playbackTimeSeconds = ms / 1000.0;
    }

    void videoHandleBase::clearExternalClock()
    {
        m_useExternalClock = bfalse;
        m_externalClockMs = 0.0;
    }

    void videoHandleBase::tickPlaybackClock()
    {
        if (m_useExternalClock)
        {
            m_playbackTimeSeconds = m_externalClockMs / 1000.0;
            return;
        }

        if (m_isPaused || m_stopped)
            return;

        m_playbackTimeSeconds += APPLOOPDT;
    }

#ifdef ITF_SUPPORT_IMGUI
    void videoHandleBase::registerToImGui()
    {
        IMGUI_ADAPTER->registerEntity<ImGuiVideoDebugTool>(this);
    }

    void videoHandleBase::unRegisterToImGui()
    {
        IMGUI_ADAPTER->unRegisterEntity<ImGuiVideoDebugTool>(this);
    }

    void videoHandleBase::onVideoOpened(const String& _fullFilename)
    {
        m_lastOpenedFile = _fullFilename;
        m_openedAtSeconds = 0.0;
        m_lastDecodeTimeMs = 0.0f;
        m_avgDecodeTimeMs  = 0.0f;
        m_maxDecodeTimeMs  = 0.0f;
        m_decodeFrameCount = 0;
    }

    void videoHandleBase::onVideoClosed()
    {
        m_lastOpenedFile.clear();
        m_openedAtSeconds = 0.0;
        m_lastDecodeTimeMs = 0.0f;
        m_avgDecodeTimeMs  = 0.0f;
        m_maxDecodeTimeMs  = 0.0f;
        m_decodeFrameCount = 0;
    }

    void videoHandleBase::displayImGui()
    {
        ImGui::Text("videoHandle %p", this);
        ImGui::Separator();

        if (!m_lastOpenedFile.isEmpty())
        {
            ImGui::Text("File    : %s", m_lastOpenedFile.getCharCopy());
        }
        ImGui::Text("Duration: %.3f s", m_totalTimeInSeconds);

        ImGui::Separator();
        ImGui::Text("State:");
        ImGui::BulletText("Opened : %s", m_opened ? "true" : "false");
        ImGui::BulletText("Paused : %s", m_isPaused ? "true" : "false");
        ImGui::BulletText("Stopped: %s", m_stopped ? "true" : "false");
        ImGui::BulletText("Loop   : %s", m_loop ? "true" : "false");
        ImGui::BulletText("Alpha  : %.2f", m_alpha);

        ImGui::Separator();
        ImGui::Text("Decode stats:");
        ImGui::BulletText("Last   : %.3f ms", m_lastDecodeTimeMs);
        ImGui::BulletText("Avg    : %.3f ms", m_avgDecodeTimeMs);
        ImGui::BulletText("Max    : %.3f ms", m_maxDecodeTimeMs);
        ImGui::BulletText("Frames : %u", m_decodeFrameCount);
    }
#endif // ITF_SUPPORT_IMGUI
} // namespace ITF

