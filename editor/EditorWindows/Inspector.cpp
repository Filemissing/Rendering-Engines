//
// Created by micha on 16/01/2026.
//

#include "Inspector.h"

#include "../Editor.h"
#include <misc/cpp/imgui_stdlib.h>
#include <misc/cpp/imgui_stdlib.cpp>

#include "../../core/ComponentFactory.h"

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

            ImGui::Separator();

            // add component button
            const char* label = "Add Component";

            ImVec2 size = ImGui::CalcTextSize(label);
            float padding = ImGui::GetStyle().FramePadding.x * 2.0f;
            float buttonWidth = size.x + padding;

            float avail = ImGui::GetContentRegionAvail().x;
            float offset = (avail - buttonWidth) * 0.5f;

            if (offset > 0.0f)
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset);

            if (ImGui::Button(label)) {
                ImGui::OpenPopup("Component Picker");
            }

            // component picker popup
            if (ImGui::BeginPopup("Component Picker")) {
                auto registry = core::GetComponentRegistry();

                for (auto component : registry) {
                    if (ImGui::Button(component.first.c_str())) {
                        core::ComponentFactory::Create(component.first, obj);
                    }
                }

                ImGui::EndPopup();
            }
        }
    }
} // editor