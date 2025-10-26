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
}

int main() {
    // Setup
    if (!editor::Editor::Init()) {
        printf("Editor Failed to Initialize\n");
        return -1;
    }
    RenderSettings::Init();

    //Scene
    auto scene1 = new core::Scene("Scene 1");
    editor::Editor::activeScene = scene1;

    // Camera
    auto* camera = new core::GameObject("Camera");
    camera->AddComponent(new core::Camera(g_width, g_height));
    camera->AddComponent(new core::PlayerController());
    camera->transform.Translate(glm::vec3(0, 0, 10));

    // Models
    auto* quadObject = new core::GameObject("CMGaTo");
    quadObject->AddComponent(new core::MeshRenderer(
        new core::Model( { core::Mesh::generateQuad() } ),
        new core::Material("shaders/modelVertex.vs", "shaders/texture.fs")
        ));
    quadObject->transform.Translate(glm::vec3(0,0,-2.5));
    quadObject->transform.Scale(glm::vec3(5, 5, 1));

    auto* suzanne = new core::GameObject("Suzanne");
    suzanne->AddComponent(new core::MeshRenderer(
        new core::Model(core::AssimpLoader::loadModel("models/nonormalmonkey.obj")),
        new core::Material("shaders/modelVertex.vs", "shaders/fragment.fs")
        ));
    core::Texture CMGaToTexture("textures/CMGaTo_crop.png");


    // Add objects to scene
    scene1->objects.push_back(camera);
    scene1->objects.push_back(quadObject);
    quadObject->transform.AddChild(&suzanne->transform);

    // global variables
    double currentTime = glfwGetTime();
    double finishFrameTime = 0.0;
    float deltaTime = 0.0f;
    float rotationStrength = 100.0f;

    double oldMouseX = 0.0f, oldMouseY = 0.0f;

    while (!glfwWindowShouldClose(editor::Editor::mainWindow)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        editor::Editor::Draw();

        double mouseXPos, mouseYPos;
        glfwGetCursorPos(editor::Editor::mainWindow, &mouseXPos, &mouseYPos);
        double deltaX = mouseXPos - oldMouseX, deltaY = mouseYPos - oldMouseY;
        glm::vec2 deltaMouse = glm::vec2(deltaX, deltaY);

        camera->GetComponent<core::PlayerController>()->handleInputs(editor::Editor::mainWindow, deltaMouse, deltaTime);
        processInput(editor::Editor::mainWindow);

        suzanne->transform.Rotate(glm::vec3(0.0f, 1.0f, 0.0f), rotationStrength * deltaTime);

        quadObject->GetComponent<core::MeshRenderer>()->GetMaterial()->SetTexture("text", CMGaToTexture.getId());
        quadObject->GetComponent<core::MeshRenderer>()->GetMaterial()->SetMat4(
            "mvpMatrix",
            camera->GetComponent<core::Camera>()->projection *
                camera->GetComponent<core::Camera>()->getView() *
                quadObject->transform.GetMatrix());

        suzanne->GetComponent<core::MeshRenderer>()->GetMaterial()->SetMat4(
            "mvpMatrix",
            camera->GetComponent<core::Camera>()->projection *
                camera->GetComponent<core::Camera>()->getView() *
                suzanne->transform.GetMatrix());

        scene1->Update();


        editor::Editor::EndFrame();

        // update global variables
        oldMouseX = mouseXPos; oldMouseY = mouseYPos; // keep track of mouse pos for delta mouse
        finishFrameTime = glfwGetTime();
        deltaTime = static_cast<float>(finishFrameTime - currentTime);
        currentTime = finishFrameTime;
    }

    editor::Editor::Shutdown();
    return 0;
}