// RaymarcherWindow.h
#pragma once
#include "EditorWindow.h"
#include <glm/vec3.hpp>

namespace core { class Raymarcher; }

namespace editor::editorWindows {
    class RaymarcherWindow : public EditorWindow {
        using EditorWindow::EditorWindow;

        core::Raymarcher* raymarcher = nullptr;

        glm::ivec3 pendingRes = glm::ivec3(512);
        glm::vec3 pendingWorldMin = glm::vec3(-30.0f);
        glm::vec3 pendingWorldMax = glm::vec3(30.0f);
        glm::ivec3 pendingBrickPoolDim = glm::ivec3(32);

    public:
        void OnEnable() override;
        void OnGUI() override;
    };
}