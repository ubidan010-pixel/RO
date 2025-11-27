#include "precompiled_engine.h"

#if defined(ITF_SUPPORT_IMGUI) && !defined(ITF_FINAL) && defined(ITF_SUPPORT_EDITOR)
#ifndef _ITF_EDITOR_H_
#include "editor/editor.h"
#endif //_ITF_EDITOR_H_

#ifndef _ITF_PLUGINGATEWAY_H_
#include "PluginGateway/PluginGateway.h"
#endif //_ITF_PLUGINGATEWAY_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_CAMERACONTROLLERMANAGER_H_
#include "engine/actors/components/Camera/CameraControllerManager.h"
#endif // _ITF_CAMERACONTROLLERMANAGER_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#include "imgui/imgui.h"
#include "engine/AdaptersInterfaces/ImGui/ImGuiEngineDebugTool.h"
#include "ImGuiActorEditorTool.h"

namespace ITF
{
    ImGuiActorEditorTool::ImGuiActorEditorTool() = default;

    static inline bool IsBlank(const char* s) {
        if (!s) return true;
        for (const char* p = s; *p; ++p) if (!std::isspace((unsigned char)*p)) return false;
        return true;
    }
    static inline std::string lower(std::string v) { std::transform(v.begin(), v.end(), v.begin(), ::tolower); return v; }
    static inline bool icontains(const char* hay, const char* needle) {
        if (!needle || !*needle) return true;
        if (!hay) return false;
        std::string h = lower(hay), n = lower(needle);
        return h.find(n) != std::string::npos;
    }
    static bool IsEditorUtilityName(const char* name) {
        if (!name) return true;
        std::string s(name);
        std::transform(s.begin(), s.end(), s.begin(),
            [](unsigned char c) { return (char)std::tolower(c); });
        return s.find("_graph") != std::string::npos
            || s.find("_ld") != std::string::npos
            || s.find("_sound") != std::string::npos;
    }

    void ImGuiActorEditorTool::refreshScenes()
    {
        m_scenes.clear();
        m_selectedScene = -1;

        m_cachedWorld = static_cast<const World*>(GETOBJECT(WORLD_MANAGER->getCurrentWorld()));
        const World* w = m_cachedWorld;
        if (!w) return;

        for (u32 i = 0; i < w->getSceneCount(); ++i) {
            if (Scene* sc = w->getSceneAt(i)) {
                const char* nm = sc->getUniqueName().getCharCopy();
                if (!IsBlank(nm)) m_scenes.push_back(sc); // skip empty/whitespace names
            }
        }
        if (!m_scenes.empty()) m_selectedScene = 0;
    }

    void ImGuiActorEditorTool::refreshActorsForSelectedScene()
    {
        m_sceneActors.clear();
        m_selectedActor = -1;

        if (m_selectedScene < 0 || m_selectedScene >= (int)m_scenes.size()) return;
        Scene* sc = m_scenes[m_selectedScene];
        if (!sc) return;

        PickableList& list = sc->getActorsMutable();
        for (u32 i = 0; i < list.size(); ++i) {
            BaseObject* bo = list[i];
            if (!bo || bo->getObjectType() != BaseObject::eActor) continue;
            auto* a = static_cast<Actor*>(bo);
            //const char* nm = a->getUserFriendly().cStr();
            //if (IsEditorUtilityName(nm)) continue;
            m_sceneActors.push_back(a);
        }
        if (!m_sceneActors.empty()) m_selectedActor = 0;
    }

    void ImGuiActorEditorTool::camFocusOnTarget(Actor* _targetActor)
    {
        if (CAMERA->getMode() == Camera::Camera_Editor)
        {
            Vec3d target = _targetActor->getAABB().getCenter().to3d();
            AABB aabb = _targetActor->getAABB();

            f32 tanFocalBy2 = f32_Abs(tanf(CAMERA->getFocale() * 0.5f));
            f32 screenRatio = 0.5f;
            if (CAMERACONTROLLERMANAGER->m_screenRatio)
                screenRatio /= CAMERACONTROLLERMANAGER->m_screenRatio;

            target.m_z += Max(Max(aabb.getWidth() * screenRatio, aabb.getHeight() * 0.5f) / tanFocalBy2, 10.0f);

            CAMERA->forcePosition(target);
            CAMERA->cancelBiasMode();
        }
    }

    void ImGuiActorEditorTool::openTool()
    {
        ImGuiEngineDebugTool::openTool();
    }

    void ImGuiActorEditorTool::drawTool()
    {
        {
            const World* cur = static_cast<const World*>(GETOBJECT(WORLD_MANAGER->getCurrentWorld()));
            if (cur != m_cachedWorld || m_scenes.empty()) {
                refreshScenes();
                refreshActorsForSelectedScene();
            }
        }

        // Toolbar
        if (ImGui::SmallButton("Refresh")) {
            refreshScenes();
            refreshActorsForSelectedScene();
        }
        ImGui::SameLine();
        if (ImGui::SmallButton("Clear")) {
            m_selectedScene = m_selectedActor = -1;
            m_scenes.clear();
            m_sceneActors.clear();
            m_currentActor = nullptr;
            m_cachedWorld = nullptr;
            m_sceneFilter[0] = m_actorFilter[0] = '\0';
        }

        ImGui::PushItemWidth(120.0f);
        ImGui::InputText("##SceneFilter", m_sceneFilter, IM_ARRAYSIZE(m_sceneFilter));
        ImGui::PopItemWidth();

        ImGui::SameLine();

        const char* sceneLabel = (m_selectedScene >= 0 && m_selectedScene < (int)m_scenes.size())
            ? m_scenes[m_selectedScene]->getUniqueName().getCharCopy() : "<none>";
        ImGui::SameLine();
        if (ImGui::BeginCombo("Scene", IsBlank(sceneLabel) ? "<unnamed>" : sceneLabel)) {
            for (int i = 0; i < (int)m_scenes.size(); ++i) {
                const char* nm = m_scenes[i]->getUniqueName().getCharCopy();
                if (IsBlank(nm)) continue;
                //if (!IsEditorUtilityName(nm)) continue;
                if (!icontains(nm, m_sceneFilter)) continue;
                bool sel = (i == m_selectedScene);
                if (ImGui::Selectable(nm, sel)) {
                    m_selectedScene = i;
                    refreshActorsForSelectedScene();
                    m_actorFilter[0] = '\0';
                }
                if (sel) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        ImGui::SameLine();
        if (ImGui::SmallButton("Copy##Scene"))
        {
            ImGui::SetClipboardText(sceneLabel);
        }

        ImGui::PushItemWidth(120.0f);
        ImGui::InputText("##ActorFilter", m_actorFilter, IM_ARRAYSIZE(m_actorFilter));
        ImGui::PopItemWidth();

        ImGui::SameLine();

        Actor* selectedActorPtr = nullptr;
        const char* actorLabel = "<none>";
        if (m_selectedActor >= 0 && m_selectedActor < (int)m_sceneActors.size()) {
            const char* nm = m_sceneActors[m_selectedActor]->getUserFriendly().cStr();
            actorLabel = (IsBlank(nm) ? "<unnamed>" : nm);
        }
        ImGui::SameLine();
        if (ImGui::BeginCombo("Actor", actorLabel)) {
            for (int i = 0; i < (int)m_sceneActors.size(); ++i) {
                Actor* a = m_sceneActors[i];
                const char* nm = a->getUserFriendly().cStr();
                if (!icontains(nm, m_actorFilter)) continue;
                bool sel = (i == m_selectedActor);
                if (ImGui::Selectable(nm, sel)) m_selectedActor = i;
                if (sel) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        if (m_selectedActor >= 0 && m_selectedActor < (int)m_sceneActors.size())
            selectedActorPtr = m_sceneActors[m_selectedActor];

        ImGui::SameLine();
        if (ImGui::SmallButton("Copy##Actor"))
        {
            ImGui::SetClipboardText(actorLabel);
        }

        ObjectRef target;
        if (selectedActorPtr)
            target = selectedActorPtr->getRef();

        BaseObject* obj = target.getObject();

        if (!obj || obj->getObjectType() != BaseObject::eActor)
        {
            ImGui::TextUnformatted("Please select an actor");
            return;
        }

        Actor* a = static_cast<Actor*>(obj);

        if (a != m_currentActor) {
            m_currentActor = a;
        }

        ImGui::Separator();
        if (ImGui::SmallButton("Focus")) {
            CAMERA->setMode(ITF::Camera::Mode::Camera_Editor);
            camFocusOnTarget(a);
        }

        ImGui::SameLine();
        if (ImGui::SmallButton("Save Changes")) {
            PLUGINGATEWAY->onObjectChanged(obj);
        }
        ImGui::Separator();

        // Title
        ImGui::Text("%s@%s", a->getUserFriendly().cStr(), a->getScene()->getUniqueName().cStr());
        ImGui::Separator();

        // Transform
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
        {
            {
                Vec3d pos = a->getPos();
                float p[3] = { (float)pos.m_x, (float)pos.m_y, (float)pos.m_z };
                if (ImGui::DragFloat3("Position", p, 0.1f))
                {
                    a->setPos(Vec3d(p[0], p[1], p[2]));
                    a->setWorldInitialPos(Vec3d(p[0], p[1], p[2]));
                }
            }
            {
                Vec2d sc = a->getScale();
                float s[2] = { (float)sc.m_x, (float)sc.m_y };
                if (ImGui::DragFloat2("Scale", s, 0.01f, 0.0001f, 1000.0f))
                    a->setScale(Vec2d(s[0], s[1]));
            }
        }

        auto comps = a->GetAllComponents();
        for (auto comp : comps)
        {
            if (IInspectable* insp = static_cast<IInspectable*>(comp))
            {
                if (!insp->canShow()) continue;

                if (ImGui::CollapsingHeader(insp->inspectableName(), ImGuiTreeNodeFlags_DefaultOpen))
                {
                    insp->displayProperties();
                }
            }
        }
    }
}

#endif // ITF_SUPPORT_IMGUI
