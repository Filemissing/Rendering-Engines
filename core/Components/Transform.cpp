//
// Created by micha on 17/10/2025.
//

#include "./Transform.h"

#include <glm/ext/matrix_transform.hpp>

#include "imgui.h"
#include "../GameObject.h"


namespace core {
    // public
    Transform::Transform() {
        position = glm::vec3(0, 0, 0);
        rotation = glm::vec3(0, 0, 0);
        scale = glm::vec3(1, 1, 1);

        RecalculateMatrix();
    }
    Transform::Transform(GameObject* gameObject) : Transform() {
        gameObject->AddComponent(this);
    }


    void Transform::Translate(glm::vec3 translation) {
        position += translation;

        RecalculateMatrix();
    }
    void Transform::Rotate(glm::vec3 axis, float degrees) {
        rotation += axis * degrees;

        RecalculateMatrix();
    }
    void Transform::Scale(glm::vec3 scale) {
        this->scale += scale;

        RecalculateMatrix();
    }

    void Transform::SetPosition(glm::vec3 newPosition) {
        position = newPosition;

        RecalculateMatrix();
    }
    void Transform::SetRotation(glm::vec3 newRotation) {
        rotation = newRotation;

        RecalculateMatrix();
    }
    void Transform::SetScale(glm::vec3 newScale) {
        scale = newScale;

        RecalculateMatrix();
    }

    glm::mat4 Transform::GetMatrix() const {
        return matrix;
    }

    bool Transform::AddChild(Transform* child) {
        if (!child || child == this) return false;
        // reject if child is an ancestor (prevent cycles)
        for (Transform* p = this; p; p = p->parent) {
            if (p == child) return false;
        }
        // detach child from old parent
        child->DetachFromParent();

        child->parent = this;
        child->root = this->root;
        child->gameObject->scene = gameObject->scene;
        children.push_back(child);
        return true;
    }
    bool Transform::RemoveChild(Transform* child) {
        if (!child) return false;
        auto it = std::find(children.begin(), children.end(), child);
        if (it == children.end()) return false;
        (*it)->parent = nullptr;
        children.erase(it);
        return true;
    }
    void Transform::DetachFromParent() {
        if (parent) parent->RemoveChild(this);
        root = this;
    }

    // private
    void Transform::RecalculateMatrix() {
        auto newMatrix = glm::mat4(1.0f);

        // reverse order?
        newMatrix = glm::translate(newMatrix, glm::vec3(position.x, position.y, position.z));

        newMatrix = glm::rotate(newMatrix, glm::radians(rotation.y), glm::vec3(0, 1, 0));
        newMatrix = glm::rotate(newMatrix, glm::radians(rotation.x), glm::vec3(1, 0, 0));
        newMatrix = glm::rotate(newMatrix, glm::radians(rotation.z), glm::vec3(0, 0, 1));

        newMatrix = glm::scale(newMatrix, scale);

        if (parent != nullptr) {
            newMatrix = newMatrix * parent->GetMatrix();
        }

        matrix = newMatrix;

        RecalculateDirections();
    }
    void Transform::RecalculateDirections() {
        right = glm::normalize(matrix * glm::vec4(1, 0, 0, 0));
        up = glm::normalize(matrix * glm::vec4(0, 1, 0, 0));
        forward = glm::normalize(matrix * glm::vec4(0, 0, 1, 0));
    }

    void Transform::OnInspectorGUI() {
        glm::vec3 _position = this->position;
        glm::vec3 _rotation = this->rotation;
        glm::vec3 _scale = this->scale;

        ImGui::DragFloat3("Position", &_position.x, 0.1f);
        ImGui::DragFloat3("Rotation", &_rotation.x, 1.0f);
        ImGui::DragFloat3("Scale", &_scale.x, 0.01f);

        if (_position != this->position) this->SetPosition(_position);
        if (_rotation != this->rotation) this->SetRotation(_rotation);
        if (_scale != this->scale) this->SetScale(_scale);
    }

    nlohmann::json Transform::Serialize() {
        nlohmann::json json;
        json["type"] = "Transform";
        json["position"] = nlohmann::json::object();
        json["rotation"] = nlohmann::json::object();
        json["scale"] = nlohmann::json::object();

        json["position"]["x"] = position.x;
        json["position"]["y"] = position.y;
        json["position"]["z"] = position.z;

        json["rotation"]["x"] = rotation.x;
        json["rotation"]["y"] = rotation.y;
        json["rotation"]["z"] = rotation.z;

        json["scale"]["x"] = scale.x;
        json["scale"]["y"] = scale.y;
        json["scale"]["z"] = scale.z;

        json["children"] = nlohmann::json::array();
        for (auto child : children) {
            json["children"].push_back(child->gameObject->Serialize());
        }

        return json;
    }
    void Transform::Deserialize(const nlohmann::json &json) {
        position = glm::vec3(json["position"]["x"], json["position"]["y"], json["position"]["z"]);
        rotation = glm::vec3(json["rotation"]["x"], json["rotation"]["y"], json["rotation"]["z"]);
        scale = glm::vec3(json["scale"]["x"], json["scale"]["y"], json["scale"]["z"]);
        RecalculateMatrix();

        for (auto child : json["children"]) {
            GameObject* childGameObject = new GameObject(gameObject->scene);
            childGameObject->Deserialize(child);
            AddChild(&childGameObject->transform);
        }
    }
}
