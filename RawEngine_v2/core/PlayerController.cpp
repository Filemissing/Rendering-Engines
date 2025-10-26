//
// Created by micha on 19/10/2025.
//

#include "PlayerController.h"
#include <iostream>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include "GameObject.h"

namespace core {
    void PlayerController::handleInputs(GLFWwindow *window, glm::vec2 deltaMouse, float deltaTime) {
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

            if (isMoving) {
                gameObject->transform.Rotate(glm::vec3(1, 0, 0), -deltaMouse.y * rotateSpeed * deltaTime);
                gameObject->transform.Rotate(glm::vec3(0, 1, 0), -deltaMouse.x * rotateSpeed * deltaTime);

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
}
