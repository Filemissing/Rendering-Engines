//
// Created by micha on 23/10/2025.
//

#include <glad/glad.h>

#include "Editor.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "EditorWindows/HierarchyWindow.h"
#include "EditorWindows/SceneLoader.h"

namespace editor {
    // static fields
    int Editor::g_width = 800;
    int Editor::g_height = 600;

    std::vector<editorWindows::EditorWindow*> Editor::windows;
    GLFWwindow* Editor::mainWindow = nullptr;

    core::Scene* Editor::activeScene = nullptr;
    core::GameObject* Editor::selectedObject = nullptr;

    glm::vec2 Editor::oldMousePos = glm::vec2();
    glm::vec2 Editor::deltaMouse = glm::vec2();
    float Editor::currentTime = 0.0;
    float Editor::deltaTime = 0.0;

    // private
    void Editor::FramebufferSizeCallback(GLFWwindow *window, int width, int height) {
        g_width = width;
        g_height = height;
        glViewport(0, 0, width, height);
    }
    bool Editor::InitGLFW(GLFWwindow*& outWindow) {
        glfwInit();
        glfwWindowHint(GLFW_SAMPLES, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

        GLFWwindow* window = glfwCreateWindow(g_width, g_height, "LearnOpenGL", nullptr, nullptr);
        if (window == nullptr) {
            printf("Failed to create GLFW window\n");
            glfwTerminate();
            return false;
        }
        glfwMakeContextCurrent(window);

        glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);

        if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
            printf("Failed to initialize GLAD\n");
            return false;
        }

        outWindow = window;
        return true;
    }
    void Editor::InitImGui() {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

        //Setup platforms
        ImGui_ImplGlfw_InitForOpenGL(mainWindow, true);
        ImGui_ImplOpenGL3_Init("#version 400");
    }
    void Editor::InitImGuiStyles() {
        ImGuiIO& io = ImGui::GetIO();
        ImGuiStyle& style = ImGui::GetStyle();

        io.Fonts->AddFontFromFileTTF("fonts/Roboto-Regular.ttf", 16.0f);
    }

    // public
    bool Editor::Init() {
        if (!InitGLFW(mainWindow)) {
            printf("Failed to create GLFW window\n");
            return false;
        }
        InitImGui();

        InitImGuiStyles();

        // add default windows here
        windows.push_back(new editorWindows::HierarchyWindow("Hierarchy"));
        windows.push_back(new editorWindows::SceneLoader("Scene Loader"));

        return true;
    }

    void Editor::Draw() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        for (auto window : windows) {
            window->Draw();
        }
    }
    void Editor::EndFrame() {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(mainWindow);
        glfwPollEvents();
    }

    glm::vec2 Editor::GetMainWindowSize() {
        return {g_width, g_height};
    }

    void Editor::Shutdown() {
        // close all windows
        for (auto window : windows) {
            delete window;
        }

        delete activeScene;

        // Terminate processes
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        glfwTerminate();
    }
} // Editor