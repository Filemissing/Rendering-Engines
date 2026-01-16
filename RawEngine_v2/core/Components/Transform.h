//
// Created by micha on 17/10/2025.
//

#ifndef RAWENGINE_TRANSFORM_H
#define RAWENGINE_TRANSFORM_H

#include <vector>
#include <glm/glm.hpp>
#include "Component.h"

namespace core {
    class Transform : public Component {
        glm::mat4 matrix;

        void RecalculateMatrix();
        void RecalculateDirections();

    public:
        // these are leading, matrix is calculated from these
        glm::vec3 position; // position in world-space
        glm::vec3 rotation; // rotation in world-space, euler angles, degrees
        glm::vec3 scale; // global scale

        // directional vectors
        glm::vec3 right;
        glm::vec3 up;
        glm::vec3 forward;

        Transform* parent = nullptr; // backreference-do not serialize
        Transform* root = nullptr;
        std::vector<Transform*> children;

        Transform();
        Transform(GameObject*);
        void Translate(glm::vec3 translation);
        void Rotate(glm::vec3 axis, float degrees);
        void Scale(glm::vec3 scale);

        void SetPosition(glm::vec3 newPosition);
        void SetRotation(glm::vec3 newRotation);
        void SetScale(glm::vec3 newScale);

        glm::mat4 GetMatrix() const;

        const std::vector<Transform*>& GetChildren() const { return children; }

        bool AddChild(Transform* child);
        bool RemoveChild(Transform* child);
        void DetachFromParent();

        const char* GetTypeName() override { return "Transform"; }

        void OnInspectorGUI() override;

        nlohmann::json Serialize() override;
        void Deserialize(const nlohmann::json &json) override;
    };
    REGISTER_COMPONENT(Transform)
}

#endif //RAWENGINE_TRANSFORM_H