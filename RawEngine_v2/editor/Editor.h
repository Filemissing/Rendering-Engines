//
// Created by micha on 23/10/2025.
//

#ifndef RAWENGINE_EDITOR_H
#define RAWENGINE_EDITOR_H

#include "EditorWindows/EditorWindow.h"

#include <vector>
#include <Glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>

#include "../core/Assets/Scene.h"
#include "EditorWindows/ViewPort.h"

namespace editor {
    class Editor {
        static int g_width;
        static int g_height;

        static void FramebufferSizeCallback(GLFWwindow *window, int width, int height);
        static bool InitGLFW(GLFWwindow*& outWindow);
        static void InitImGui();
        static void DrawDockSpace();
        static void InitImGuiStyles();

    public:
        static float m_windowBorderSize;
        static float m_windowRoundingValue;
        static ImVec2 m_windowPadding;
        static ImGuiID m_dockspaceId;

        static editorWindows::ViewPort* viewPort;
        static std::vector<editorWindows::EditorWindow*> windows;
        static GLFWwindow* mainWindow;

        static core::Scene* activeScene;
        static core::GameObject* selectedObject;

        static glm::vec2 oldMousePos;
        static glm::vec2 deltaMouse;
        static float currentTime;
        static float deltaTime;

        static bool Init();

        static void Draw();
        static void DrawMainMenu();
        static void EndFrame();

        static glm::vec2 GetMainWindowSize();
        static glm::vec2 GetViewPortSize();

        static GLuint GetFrameBuffer();

        static void Shutdown();
    };
} // Editor

#endif //RAWENGINE_EDITOR_H