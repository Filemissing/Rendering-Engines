//
// Created by micha on 17/10/2025.
//

#include "Transform.h"

#include <glm/ext/matrix_transform.hpp>


namespace core {
    // public
    Transform::Transform() {
        position = glm::vec3(0, 0, 0);
        rotation = glm::vec3(0, 0, 0);
        scale = glm::vec3(1, 1, 1);

        RecalculateMatrix();
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
        if (child->parent) {
            child->parent->RemoveChild(child);
        }
        child->parent = this;
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

        matrix = newMatrix;

        RecalculateDirections();
    }
    void Transform::RecalculateDirections() {
        right = glm::normalize(matrix * glm::vec4(1, 0, 0, 0));
        up = glm::normalize(matrix * glm::vec4(0, 1, 0, 0));
        forward = glm::normalize(matrix * glm::vec4(0, 0, 1, 0));
    }
}