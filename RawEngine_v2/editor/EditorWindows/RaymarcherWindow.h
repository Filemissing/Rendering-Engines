// RaymarcherWindow.h
#pragma once
#include "EditorWindow.h"
#include <glm/vec3.hpp>

namespace core { class Raymarcher; }

namespace editor::editorWindows {
    class RaymarcherWindow : public EditorWindow {
        using EditorWindow::EditorWindow;

        core::Raymarcher* raymarcher = nullptr;

        int pendingResX = 128, pendingResY = 128, pendingResZ = 128;
        glm::vec3 pendingWorldMin = glm::vec3(-30.0f);
        glm::vec3 pendingWorldMax = glm::vec3(30.0f);

    public:
        void OnEnable() override;
        void OnGUI() override;
    };
}