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
    }

    Camera::~Camera() {
        if (GetMainCamera() == this)
            gameObject->scene->mainCamera = nullptr;
    }

    glm::mat4 Camera::GetView() const {
        return glm::inverse(gameObject->transform.GetMatrix());
    }

    glm::mat4 Camera::GetProjection() const {
        return projection;
    }

    void Camera::RecalculateProjection(float width, float height) {
        projection = glm::perspective(glm::radians(fov),
            width / height,
            nearPlane, farPlane);
    }

    Camera* Camera::GetMainCamera() {
        return editor::Editor::activeScene->mainCamera;
    }

    void Camera::OnInspectorGUI() {
        ImGui::SliderFloat("FOV", &fov, 0.0f, 180.0f);

        ImGui::InputFloat("Near Plane", &nearPlane);
        ImGui::InputFloat("Far Plane", &farPlane);
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
    }
}
