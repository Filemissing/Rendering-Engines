// RaymarcherWindow.cpp
#include "RaymarcherWindow.h"
#include "../../core/Raymarcher.h"
#include "imgui.h"
#include "../Editor.h"

namespace editor::editorWindows {
    void RaymarcherWindow::OnEnable() {
        raymarcher = Editor::viewPort->GetRenderer()->GetRaymarcher();
        pendingResX = raymarcher->GetResolution().x;
        pendingResY = raymarcher->GetResolution().y;
        pendingResZ = raymarcher->GetResolution().z;
    }

    void RaymarcherWindow::OnGUI() {
        if (raymarcher == nullptr) {
            ImGui::TextDisabled("No Raymarcher bound");
            return;
        }

        ImGui::SeparatorText("Settings");
        ImGui::DragInt("MaxSteps", &raymarcher->maxSteps, 1.0);
        ImGui::DragFloat("MaxDist", &raymarcher->maxDist, 1.0);
        ImGui::DragFloat("SurfDist", &raymarcher->surfDist, 0.001f);

        ImGui::SeparatorText("Volume");

        ImGui::DragInt3("Resolution", &pendingResX, 1.0f, 1, 512);
        ImGui::DragFloat3("World Min", &pendingWorldMin.x, 0.1f);
        ImGui::DragFloat3("World Max", &pendingWorldMax.x, 0.1f);

        if (ImGui::Button("Apply Volume Settings")) {
            raymarcher->EnsureVolumeSized(glm::ivec3(pendingResX, pendingResY, pendingResZ));
            raymarcher->SetWorldBounds(pendingWorldMin, pendingWorldMax);
        }

        ImGui::Spacing();
        ImGui::SeparatorText("Bake");

        ImGui::Text("State: %s", raymarcher->IsDirty() ? "Dirty" : "Up to date");

        ImGui::BeginDisabled(!raymarcher->IsDirty());
        if (ImGui::Button("Rebake Now")) {
            raymarcher->Bake(Editor::activeScene->primitives);
        }
        ImGui::EndDisabled();

        ImGui::SameLine();
        if (ImGui::Button("Force Rebake")) {
            raymarcher->MarkDirty();
            raymarcher->Bake(Editor::activeScene->primitives);
        }

        ImGui::Spacing();
        ImGui::SeparatorText("Info");

        glm::ivec3 res = raymarcher->GetResolution();
        ImGui::Text("Current resolution: %d x %d x %d", res.x, res.y, res.z);
        ImGui::Text("Voxel count: %d", res.x * res.y * res.z);
    }
}
