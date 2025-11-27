#include "precompiled_engine.h"

#if defined(ITF_SUPPORT_IMGUI) && !defined(ITF_FINAL) && defined(ITF_SUPPORT_EDITOR)

#include <string>
#include <vector>
#include <sstream>

namespace ITF
{
    class ImGuiEngineDebugTool;

    class ImGuiActorEditorTool final : public ImGuiEngineDebugTool
    {
    public:
        ImGuiActorEditorTool();

        const bbool isAvailableInFinal() const override { return bfalse; }
        const char* menuPath() const override { return "Editor"; }
        const char* menuItemName() const override { return "Actor"; }

        void openTool() override;

    protected:
        void drawTool() override;

    private:
        const Actor* m_currentActor = nullptr;

        int m_selectedScene = -1;
        int m_selectedActor = -1;
        std::vector<Scene*>  m_scenes;
        std::vector<Actor*>  m_sceneActors;
        const World* m_cachedWorld = nullptr;

        char m_sceneFilter[64] = {};
        char m_actorFilter[64] = {};

        void            refreshScenes();
        void            refreshActorsForSelectedScene();
        void            camFocusOnTarget(Actor* _targetActor);
    };
}

#endif // ITF_SUPPORT_IMGUI
