//
// Created by micha on 17/11/2025.
//

#include "SceneLoader.h"

#include "../Editor.h"
#include "../Utils/SceneManager.h"

namespace editor::editorWindows {
    void SceneLoader::OnEnable() {

    }

    void OnTextChanged() {

    }

    void SceneLoader::OnGUI() {
        // Text input field
        char buffer[256];
        memset(buffer, 0, sizeof(buffer));
        memcpy(buffer, sceneName.c_str(), sceneName.size());

        ImGui::Text("Scene Name");
        ImGui::SameLine();
        if (ImGui::InputText("##", buffer, sizeof(buffer)))
        {
            sceneName = buffer;
        }

        ImGui::Spacing();

        // Load button
        if (ImGui::Button("Load")) {
            if (!sceneName.empty()) {
                auto scene = SceneManager::LoadScene(sceneName);

                if (scene) SceneManager::SetActiveScene(scene);
                else ImGui::OpenPopup("Error");
            }
            else {
                ImGui::OpenPopup("Error");
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Save")) {
            if (!sceneName.empty()) {
                SceneManager::SaveScene(Editor::activeScene, sceneName);
            }
            else {
                ImGui::OpenPopup("Error");
            }
        }

        // Small popup for feedback
        if (ImGui::BeginPopupModal("Error", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Scene not found");
            if (ImGui::Button("OK"))
                ImGui::CloseCurrentPopup();
            ImGui::EndPopup();
        }
    }

} // editor