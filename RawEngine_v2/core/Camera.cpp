//
// Created by micha on 13/10/2025.
//

#include "Camera.h"
#include "GameObject.h"

namespace core {
    Camera* Camera::mainCamera = nullptr;

    Camera::Camera(int width, int height) {
        if (mainCamera == nullptr)
            mainCamera = this;

        this->width = width;
        this->height = height;

        projection = glm::perspective(glm::radians(45.0f),
            static_cast<float>(width) / static_cast<float>(height),
            0.1f, 100.0f);
    }

    glm::mat4 Camera::getView() {
        return glm::inverse(gameObject->transform.GetMatrix());
    }

    nlohmann::json Camera::Serialize() {
        nlohmann::json json;
        json["type"] = "Camera";
        json["width"] = width;
        json["height"] = height;
        return json;
    }
    void Camera::Deserialize(const nlohmann::json& json) {
        width = json["width"];
        height = json["height"];
        // recalculate projection after height and width are changed
        projection = glm::perspective(glm::radians(45.0f),
            static_cast<float>(width) / static_cast<float>(height),
            0.1f, 100.0f);
    }
}
