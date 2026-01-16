//
// Created by micha on 16/01/2026.
//

#include "Inspector.h"

#include "../Editor.h"
#include <misc/cpp/imgui_stdlib.h>
#include <misc/cpp/imgui_stdlib.cpp>

namespace editor::editorWindows {
    void Inspector::OnGUI() {
        if (Editor::selectedObject) {
            auto* obj = Editor::selectedObject;
            ImGui::InputText("Name", &obj->name);

            if (ImGui::ArrowButton("Transform", obj->transform.collapseInspector ? ImGuiDir_Down : ImGuiDir_Right)) {
                obj->transform.collapseInspector = !obj->transform.collapseInspector;
            }
            ImGui::SameLine();
            ImGui::Text("Transform");

            if (obj->transform.collapseInspector) {
                obj->transform.OnInspectorGUI();
            }

            int i = 0;
            for (auto component : obj->components) {
                if (ImGui::ArrowButton(std::to_string(i).c_str(), component->collapseInspector ? ImGuiDir_Down : ImGuiDir_Right)) {
                    component->collapseInspector = !component->collapseInspector;
                }
                ImGui::SameLine();
                ImGui::Text(component->GetTypeName());

                if (component->collapseInspector) {
                    component->OnInspectorGUI();
                }

                i++;
            }
        }
    }

} // editor