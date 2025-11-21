//
// Created by micha on 13/10/2025.
//

#include "Camera.h"
#include "../GameObject.h"
#include "../../editor/Editor.h"

namespace core {
    Camera::Camera(int width, int height, GameObject* gameObject) {
        if (gameObject->scene->mainCamera == nullptr)
            gameObject->scene->mainCamera = this;

        this->width = width;
        this->height = height;

        projection = glm::perspective(glm::radians(45.0f),
            static_cast<float>(width) / static_cast<float>(height),
            0.1f, 100.0f);
    }

    Camera::~Camera() {
        if (GetMainCamera() == this)
            gameObject->scene->mainCamera = nullptr;
    }

    glm::mat4 Camera::getView() {
        return glm::inverse(gameObject->transform.GetMatrix());
    }

    Camera* Camera::GetMainCamera() {
        return editor::Editor::activeScene->mainCamera;
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
