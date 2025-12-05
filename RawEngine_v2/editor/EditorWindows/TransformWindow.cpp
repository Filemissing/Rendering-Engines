//
// Created by micha on 01/12/2025.
//

#include "TransformWindow.h"

#include "../Editor.h"

namespace editor::editorWindows {
    void Slider(const std::string& name, float* value) {
        ImGui::SliderFloat("x", &Editor::selectedObject->transform.position.x, std::numeric_limits<float>::lowest(), std::numeric_limits<float>::max());
    }
    void TransformWindow::OnGUI() {
        if (Editor::selectedObject) {
            ImGui::Text("GameObject: %s", Editor::selectedObject->name.c_str());
            ImGui::Separator();

            auto& transform = Editor::selectedObject->transform;

            glm::vec3 position = transform.position;
            glm::vec3 rotation = transform.rotation;
            glm::vec3 scale = transform.scale;

            ImGui::DragFloat3("Position", &position.x, 0.1f);
            ImGui::DragFloat3("Rotation", &rotation.x, 1.0f);
            ImGui::DragFloat3("Scale", &scale.x, 0.01f);

            if (position != transform.position) transform.SetPosition(position);
            if (rotation != transform.rotation) transform.SetRotation(rotation);
            if (scale != transform.scale) transform.SetScale(scale);
        }
        else {
            ImGui::TextDisabled("No GameObject selected");
        }
    }
} // editor