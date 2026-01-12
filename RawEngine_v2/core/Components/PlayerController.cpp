//
// Created by micha on 19/10/2025.
//

#include "PlayerController.h"
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "../GameObject.h"
#include "imgui.h"
#include "../../editor/Editor.h"


namespace core {
    void PlayerController::Update() {
        GLFWwindow* window = editor::Editor::mainWindow;
        glm::vec2 deltaMouse = editor::Editor::deltaMouse;
        float deltaTime = editor::Editor::deltaTime;

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS && editor::Editor::viewPort->isHovered) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

            if (isMoving) {
                gameObject->transform.Rotate(glm::vec3(1, 0, 0), -deltaMouse.y * rotateSpeed * deltaTime / editor::Editor::GetViewPortSize().y);
                gameObject->transform.Rotate(glm::vec3(0, 1, 0), -deltaMouse.x * rotateSpeed * deltaTime / editor::Editor::GetViewPortSize().x);

                glm::vec3 movement(0.0f, 0.0f, 0.0f);
                float speed = 5.0f;
                if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) movement += -gameObject->transform.forward * deltaTime;
                if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) movement += gameObject->transform.forward * deltaTime;
                if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) movement += -gameObject->transform.right * deltaTime;
                if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) movement += gameObject->transform.right * deltaTime;
                if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) movement += -gameObject->transform.up * deltaTime;
                if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) movement += gameObject->transform.up * deltaTime;

                if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) speed *= 2.0f;

                gameObject->transform.Translate(movement * speed);
            }
            else isMoving = true;
        }
        else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            isMoving = false;
        }
    }

    nlohmann::json PlayerController::Serialize() {
        nlohmann::json json;
        json["type"] = "PlayerController";
        json["moveSpeed"] = moveSpeed;
        json["rotateSpeed"] = rotateSpeed;
        return json;
    }
    void PlayerController::Deserialize(const nlohmann::json& json) {
        moveSpeed = json["moveSpeed"];
        rotateSpeed = json["rotateSpeed"];
    }
}
