#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <sstream>

#include "core/assimpLoader.h"
#include "core/GameObject.h"
#include "core/Component.h"
#include "core/mesh.h"
#include "core/texture.h"
#include "core/Camera.h"
#include "core/MeshRenderer.h"
#include "core/PlayerController.h"
#include "core/RenderSettings.h"
#include "core/Scene.h"
#include "editor/Editor.h"
#include "editor/EditorWindows/TestWindow.h"
#include "editor/Utils/SceneManager.h"

#define MAC_CLION
//#define VSTUDIO

#ifdef MAC_CLION
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#endif

#ifdef VSTUDIO
// Note: install imgui with:
//     ./vcpkg.exe install imgui[glfw-binding,opengl3-binding]
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#endif

// TODO: remove and replace with GetMainWindowSize() method in Editor
int g_width = 800;
int g_height = 600;

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    // TODO: this calls every frame when held, not very nice
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        editor::SceneManager::SaveScene(editor::Editor::activeScene);
    }
}

int main() {
    // Setup
    if (!editor::Editor::Init()) {
        printf("Editor Failed to Initialize\n");
        return -1;
    }
    RenderSettings::Init();

    //Scene
    auto* scene1 = editor::SceneManager::LoadScene("Scene 1");
    editor::Editor::activeScene = scene1;

    core::Texture CMGaToTexture("textures/CMGaTo_crop.png");
    scene1->FindGameObjectByName("CMGaTo")->GetComponent<core::MeshRenderer>()->GetMaterial()->SetTexture("text", CMGaToTexture.getId());

    while (!glfwWindowShouldClose(editor::Editor::mainWindow)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // calculate delta mouse
        double mouseXPos, mouseYPos;
        glfwGetCursorPos(editor::Editor::mainWindow, &mouseXPos, &mouseYPos);
        double deltaX = mouseXPos - editor::Editor::oldMousePos.x, deltaY = mouseYPos - editor::Editor::oldMousePos.y;
        editor::Editor::deltaMouse = glm::vec2(deltaX, deltaY);

        //  draw the editor
        editor::Editor::Draw();

        processInput(editor::Editor::mainWindow);

        //suzanne->transform.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), 100.0f * editor::Editor::deltaTime);

        editor::Editor::activeScene->Update();

        editor::Editor::EndFrame();

        // update global variables
        editor::Editor::oldMousePos = glm::vec2(mouseXPos, mouseYPos);
        float finishFrameTime = glfwGetTime();
        editor::Editor::deltaTime = (finishFrameTime - editor::Editor::currentTime);
        editor::Editor::currentTime = finishFrameTime;
    }

    editor::Editor::Shutdown();
    return 0;
}