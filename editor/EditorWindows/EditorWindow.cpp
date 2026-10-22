//
// Created by micha on 20/10/2025.
//

#include "EditorWindow.h"

#include "imgui.h"

namespace editor::editorWindows {
    void EditorWindow::Draw() {
        if (!isEnabled) return;

        if (ImGui::Begin(name.c_str(), &isEnabled)) {
            OnGUI();
        }
        ImGui::End();
    }
}