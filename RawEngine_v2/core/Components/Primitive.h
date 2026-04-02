//
// Created by micha on 26/03/2026.
//

#ifndef RAWENGINE_PRIMITIVE_H
#define RAWENGINE_PRIMITIVE_H
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "Component.h"

namespace core {
    class Primitive : public Component {
    public:
        using Component::Component; // inherit Constructors
        ~Primitive() override;

        enum PrimitiveType {
            Sphere,
            Box,
            Pyramid,
            Capsule,
            Cylinder,
            Torus,
            Cone,
            Plane
        };

        PrimitiveType type = Sphere;
        glm::vec3 color = glm::vec3(1);
        glm::vec3 halfExtents = glm::vec3(1, 1, 1);

        void Start() override;

        const char* GetTypeName() override { return "Primitive"; }

        void OnInspectorGUI() override;

        nlohmann::json Serialize() override;
        void Deserialize(const nlohmann::json& json) override;
    };

    REGISTER_COMPONENT(Primitive)
} // core


#endif //RAWENGINE_PRIMITIVE_H