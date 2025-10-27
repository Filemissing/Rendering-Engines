//
// Created by micha on 23/10/2025.
//

#include "HierarchyWindow.h"

#include "../Editor.h"
#include <stack>

#include "imgui_internal.h"

namespace editor::editorWindows {
    void HierarchyWindow::OnEnable() {

    }

    void HierarchyWindow::DrawHierarchyNode(UIElements::HierarchyNode& node) {
        auto* obj = node.object;
        bool hasChildren = !obj->transform.children.empty();

        ImGui::PushID(obj);

        // Arrow button
        if (hasChildren) {
            if (ImGui::ArrowButton("##arrow", node.isOpen ? ImGuiDir_Down : ImGuiDir_Right)) {
                node.isOpen = !node.isOpen;
            }
        } else {
            ImGui::Dummy(ImVec2(ImGui::GetFrameHeight() * 1.0f, 0)); // spacer for alignment
        }

        ImGui::SameLine();

        // Text area
        bool isSelected = (Editor::selectedObject == obj);
        if (ImGui::Selectable(obj->name.c_str(), isSelected, ImGuiSelectableFlags_SpanAvailWidth)) {
            Editor::selectedObject = obj;
        }

        // Draw children recursively
        if (hasChildren && node.isOpen) {
            ImGui::Indent(16.0f);
            for (auto child : obj->transform.children) {
                auto& childNode = hierarchyState[child->gameObject];
                if (!childNode.object)
                    childNode.object = child->gameObject;
                DrawHierarchyNode(childNode);
            }
            ImGui::Unindent(16.0f);
        }

        ImGui::PopID();
    }
    void HierarchyWindow::OnGUI() {
        if (ImGui::Begin("Hierarchy")) {
            for (auto* obj : Editor::activeScene->objects) {
                auto& node = hierarchyState[obj];
                if (!node.object)
                    node.object = obj;
                DrawHierarchyNode(node);
            }
        }
        ImGui::End();
    }
}
