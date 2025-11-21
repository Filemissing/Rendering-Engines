#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <sstream>

#include "core/assimpLoader.h"
#include "core/GameObject.h"
#include "core/Assets/texture.h"
#include "core/Components/MeshRenderer.h"
#include "core/Assets/RenderSettings.h"
#include "core/Assets/Scene.h"
#include "editor/Editor.h"
#include "editor/Utils/SceneManager.h"

using namespace core;
using namespace editor;

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

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main() {
    // Setup
    if (!Editor::Init()) {
        printf("Editor Failed to Initialize\n");
        return -1;
    }
    RenderSettings::Init();

    //Scene
    auto* scene1 = SceneManager::LoadScene("Scene 1");
    Editor::activeScene = scene1;

    Texture CMGaToTexture("textures/CMGaTo_crop.png");
    scene1->FindGameObjectByName("CMGaTo")->GetComponent<core::MeshRenderer>()->GetMaterial()->SetTexture("text", CMGaToTexture.getId());

    while (!glfwWindowShouldClose(editor::Editor::mainWindow)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // calculate delta mouse
        double mouseXPos, mouseYPos;
        glfwGetCursorPos(Editor::mainWindow, &mouseXPos, &mouseYPos);
        double deltaX = mouseXPos - Editor::oldMousePos.x, deltaY = mouseYPos - Editor::oldMousePos.y;
        Editor::deltaMouse = glm::vec2(deltaX, deltaY);

        //  draw the editor
        Editor::Draw();

        processInput(Editor::mainWindow);

        //suzanne->transform.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), 100.0f * Editor::deltaTime);

        if (Editor::activeScene != nullptr)
            Editor::activeScene->Update();

        Editor::EndFrame();

        // update global variables
        Editor::oldMousePos = glm::vec2(mouseXPos, mouseYPos);
        float finishFrameTime = glfwGetTime();
        Editor::deltaTime = (finishFrameTime - Editor::currentTime);
        Editor::currentTime = finishFrameTime;
    }

    Editor::Shutdown();
    return 0;
}