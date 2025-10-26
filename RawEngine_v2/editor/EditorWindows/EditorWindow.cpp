//
// Created by micha on 20/10/2025.
//

#include "EditorWindow.h"

#include "imgui.h"

namespace editor::editorWindows {
    EditorWindow::EditorWindow(const std::string& name) : name(name) {
        OnEnable();
    }

    void EditorWindow::OnEnable() {
        // inherited
    }

    void EditorWindow::Draw() {
        ImGui::Begin(name.c_str());
        OnGUI();
        ImGui::End();
    }

    void EditorWindow::OnGUI() {
        // inherited
    }

}