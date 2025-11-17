//
// Created by micha on 23/10/2025.
//

#include "HierarchyWindow.h"

#include <algorithm>

#include "../Editor.h"

#include "imgui_internal.h"

namespace editor::editorWindows {
    void HierarchyWindow::OnEnable() {

    }

    void DefineContextMenu() {
        if (ImGui::BeginPopup("HierarchyContextMenu")) {
            core::GameObject* contextObject = Editor::selectedObject;
            if (!contextObject) ImGui::BeginDisabled();
            // object specific options
            if (ImGui::MenuItem("Rename")) {}
            if (ImGui::MenuItem("Delete")) {
                if (contextObject->transform.parent) {
                    contextObject->transform.DetachFromParent();
                    delete contextObject;
                } else {
                    Editor::activeScene->RemoveAndDeleteGameObject(contextObject); // only root objects are part of the scene
                }
            }
            if (!contextObject) ImGui::EndDisabled();
            // global options
            if (ImGui::MenuItem("Create Empty")) {
                if (contextObject) {
                    new core::GameObject(&contextObject->transform); // not a root object
                } else {
                    Editor::activeScene->AddGameObject(new core::GameObject()); // only root objects should be added to the scene
                }
            }

            ImGui::EndPopup();
        }
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
        }
        else {
            ImGui::Dummy(ImVec2(ImGui::GetFrameHeight() * 1.0f, 0)); // spacer for alignment
        }

        ImGui::SameLine();

        // Selectable text area
        bool isSelected = (Editor::selectedObject == obj);
        if (ImGui::Selectable(obj->name.c_str(), isSelected, ImGuiSelectableFlags_SpanAvailWidth)) {
            Editor::selectedObject = obj;
        }

        // Open context menu
        if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
            Editor::selectedObject = obj;
            ImGui::OpenPopup("HierarchyContextMenu");
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

        DefineContextMenu();

        ImGui::PopID();
    }
    void HierarchyWindow::OnGUI() {
        if (ImGui::IsWindowHovered()) {
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) Editor::selectedObject = nullptr;
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
                Editor::selectedObject = nullptr;
                ImGui::OpenPopup("HierarchyContextMenu");
            }
        }

        for (auto* obj : Editor::activeScene->objects) {
            auto& node = hierarchyState[obj];
            if (!node.object)
                node.object = obj;
            DrawHierarchyNode(node);
        }

        DefineContextMenu();
    }
}
