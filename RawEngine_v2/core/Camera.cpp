//
// Created by micha on 13/10/2025.
//

#include "Camera.h"

#include "GameObject.h"

namespace core {
    Camera::Camera(int width, int height) {
        this->width = width;
        this->height = height;

        projection = glm::perspective(glm::radians(45.0f),
            static_cast<float>(width) / static_cast<float>(height),
            0.1f, 100.0f);
    }

    glm::mat4 Camera::getView() {
        return glm::inverse(gameObject->transform.GetMatrix());
    }
}
