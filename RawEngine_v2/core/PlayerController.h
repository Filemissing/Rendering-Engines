//
// Created by micha on 19/10/2025.
//

#ifndef RAWENGINE_PLAYERCONTROLLER_H
#define RAWENGINE_PLAYERCONTROLLER_H
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>
#include "Component.h"

namespace core {
    class PlayerController : public Component {
    private:
        bool isMoving = false;
    public:
        float moveSpeed = 5.0f;
        float rotateSpeed = 100.0f;

        void handleInputs(GLFWwindow* window, glm::vec2 mouseDelta, float deltaTime);
    };
}

#endif //RAWENGINE_PLAYERCONTROLLER_H