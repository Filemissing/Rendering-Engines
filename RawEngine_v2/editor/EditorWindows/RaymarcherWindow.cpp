// RaymarcherWindow.cpp
#include "RaymarcherWindow.h"
#include "../../core/Raymarcher.h"
#include "imgui.h"
#include "../Editor.h"

namespace editor::editorWindows {
    void RaymarcherWindow::OnEnable() {
        raymarcher = Editor::viewPort->GetRenderer()->GetRaymarcher();
        pendingRes = raymarcher->GetResolution();
        pendingBrickPoolDim = raymarcher->GetBrickPoolDim();
    }

    void RaymarcherWindow::OnGUI() {
        if (!raymarcher) {
            ImGui::TextDisabled("No Raymarcher bound");
            return;
        }

        ImGui::SeparatorText("Volume");
        ImGui::DragInt3("Resolution", &pendingRes.x, 1.0f, 1);
        ImGui::DragFloat3("World Min", &pendingWorldMin.x, 0.1f);
        ImGui::DragFloat3("World Max", &pendingWorldMax.x, 0.1f);
        ImGui::DragInt3("Brick pool dimensions", &pendingBrickPoolDim.x, 0.1f);

        if (ImGui::Button("Apply Volume Settings")) {
            raymarcher->SetBrickPoolDim(pendingBrickPoolDim);
            raymarcher->SetWorldBounds(pendingWorldMin, pendingWorldMax);
            raymarcher->EnsureVolumeSized(pendingRes);
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
        ImGui::SeparatorText("Noise");
        ImGui::DragInt2("2D Resolution", &raymarcher->noise2DResolution.x, 1.0f, 1, 4096);
        ImGui::DragInt3("3D Resolution", &raymarcher->noise3DResolution.x, 1.0f, 1, 512);
        ImGui::DragFloat("2D Frequency", &raymarcher->noise2DFrequency, 0.1f, 0.01f, 100.0f);
        ImGui::DragFloat("3D Frequency", &raymarcher->noise3DFrequency, 0.1f, 0.01f, 100.0f);
        ImGui::InputScalar("Seed", ImGuiDataType_U32, &raymarcher->noiseSeed);
        if (ImGui::Button("Rebake Noise")) {
            raymarcher->BakeNoise();
            raymarcher->MarkDirty();
        }

        ImGui::Spacing();
        ImGui::SeparatorText("Terrain");
        ImGui::DragInt("Octaves", &raymarcher->octaves, 1.0f, 1, 12);
        ImGui::DragFloat("Lacunarity", &raymarcher->lacunarity, 0.01f, 1.0f, 4.0f);
        ImGui::DragFloat("Persistence", &raymarcher->persistence, 0.01f, 0.0f, 1.0f);
        ImGui::DragFloat("Warp Strength", &raymarcher->warpStrength, 0.01f, 0.0f, 10.0f);
        ImGui::DragFloat("Height Scale", &raymarcher->heightScale, 0.1f, 0.0f, 10.0f);

        ImGui::Spacing();
        ImGui::SeparatorText("Debug Visualization");
        ImGui::Checkbox("Keep Debug resources", &raymarcher->keepDebugResources);

        ImGui::Checkbox("Enable 3D texture view", &raymarcher->debug3D);

        if (raymarcher->debug3D) {
            const char* texNames[] = { "Sign Texture", "Seed Texture", "3D Noise Texture", "Brick active Texture", "Lookup Texture", "Reverse Lookup Texture", "BrickPool Texture" };
            int currentTex = static_cast<int>(raymarcher->debugTex3D);
            if (ImGui::Combo("Texture", &currentTex, texNames, IM_ARRAYSIZE(texNames))) {
                raymarcher->debugTex3D = static_cast<core::Raymarcher::DebugTexture3D>(currentTex);
            }

            ImGui::SliderInt("Slice Z", &raymarcher->sliceZ, 0, raymarcher->GetResolution().z - 1);
            ImGui::SliderInt("Mode", &raymarcher->mode, 0, 2);
            ImGui::DragFloat("Display Scale", &raymarcher->displayScale, 0.1f, 0.001f, 1000.0f);
        }

        ImGui::Checkbox("Enable 2D texture view", &raymarcher->debug2D);

        if (raymarcher->debug2D) {
            const char* texNames[] = { "2D Noise Texture" };
            int currentTex = static_cast<int>(raymarcher->debugTex2D);
            if (ImGui::Combo("Texture", &currentTex, texNames, IM_ARRAYSIZE(texNames))) {
                raymarcher->debugTex2D = static_cast<core::Raymarcher::DebugTexture2D>(currentTex);
            }

            ImGui::SliderInt("Mode", &raymarcher->mode, 0, 2);
            ImGui::DragFloat("Display Scale", &raymarcher->displayScale, 0.1f, 0.001f, 1000.0f);
        }
    }
}
