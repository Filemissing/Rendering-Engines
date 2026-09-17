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
    if (!raymarcher) {
        ImGui::TextDisabled("No Raymarcher bound");
        return;
    }

    ImGui::SeparatorText("Volume");
    ImGui::DragInt3("Resolution", &pendingResX, 1.0f, 1, 512);
    ImGui::DragFloat3("World Min", &pendingWorldMin.x, 0.1f);
    ImGui::DragFloat3("World Max", &pendingWorldMax.x, 0.1f);

    if (ImGui::Button("Apply Volume Settings")) {
        raymarcher->EnsureVolumeSized(glm::ivec3(pendingResX, pendingResY, pendingResZ));
        raymarcher->SetWorldBounds(pendingWorldMin, pendingWorldMax);
        raymarcher->MarkDirty();
    }

    ImGui::Spacing();
    ImGui::SeparatorText("Bake");
    ImGui::Text("State: %s", raymarcher->IsDirty() ? "Dirty" : "Up to date");
    ImGui::BeginDisabled(!raymarcher->IsDirty());
    if (ImGui::Button("Rebake Now")) raymarcher->Bake(Editor::activeScene->primitives);
    ImGui::EndDisabled();
    ImGui::SameLine();
    if (ImGui::Button("Force Rebake")) { raymarcher->MarkDirty(); raymarcher->Bake(Editor::activeScene->primitives); }

    ImGui::Spacing();
    ImGui::SeparatorText("Marching");
    ImGui::DragInt("Max Steps", &raymarcher->maxSteps, 1.0f, 1, 1000);
    ImGui::DragFloat("Max Distance", &raymarcher->maxDist, 1.0f, 0.1f, 10000.0f);
    ImGui::DragFloat("Surface Distance", &raymarcher->surfDist, 0.0001f, 0.00001f, 1.0f, "%.5f");

    ImGui::Spacing();
    ImGui::SeparatorText("Terrain");
    ImGui::DragInt("Octaves", &raymarcher->octaves, 1.0f, 1, 12);
    ImGui::DragFloat("Lacunarity", &raymarcher->lacunarity, 0.01f, 1.0f, 4.0f);
    ImGui::DragFloat("Persistence", &raymarcher->persistence, 0.01f, 0.0f, 1.0f);
    ImGui::DragFloat("Warp Strength", &raymarcher->warpStrength, 0.01f, 0.0f, 10.0f);
    ImGui::DragFloat("Height Scale", &raymarcher->heightScale, 0.1f, 0.0f, 10.0f);

    ImGui::Spacing();
    ImGui::SeparatorText("Debug Visualization");
    ImGui::Checkbox("Enable Debug View", &raymarcher->debug);

    if (raymarcher->debug) {
        const char* texNames[] = { "Sign Texture", "Seed Texture", "Volume Texture" };
        int currentTex = static_cast<int>(raymarcher->debugTex);
        if (ImGui::Combo("Texture", &currentTex, texNames, IM_ARRAYSIZE(texNames))) {
            raymarcher->debugTex = static_cast<core::Raymarcher::DebugTexture>(currentTex);
        }

        ImGui::SliderInt("Slice Z", &raymarcher->sliceZ, 0, raymarcher->GetResolution().z - 1);
        ImGui::SliderInt("Mode", &raymarcher->mode, 0, 2);
        ImGui::DragFloat("Display Scale", &raymarcher->displayScale, 0.1f, 0.001f, 1000.0f);
    }
}
}
