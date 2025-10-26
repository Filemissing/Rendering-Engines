//
// Created by micha on 23/10/2025.
//

#ifndef RAWENGINE_EDITOR_H
#define RAWENGINE_EDITOR_H

#include "EditorWindows/EditorWindow.h"

#include <vector>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>

#include "../core/Scene.h"

namespace editor {
    class Editor {
        static int g_width;
        static int g_height;

        static void FramebufferSizeCallback(GLFWwindow *window, int width, int height);
        static bool InitGLFW(GLFWwindow*& outWindow);
        static void InitImGui();
        static void InitImGuiStyles();

    public:
        static std::vector<editorWindows::EditorWindow*> windows;
        static GLFWwindow* mainWindow;

        static core::Scene* activeScene;

        static bool Init();

        static void Draw();
        static void EndFrame();

        static glm::vec2 GetMainWindowSize();

        static void Shutdown();
    };
} // Editor

#endif //RAWENGINE_EDITOR_H