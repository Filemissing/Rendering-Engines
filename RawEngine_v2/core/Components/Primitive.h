//
// Created by micha on 26/03/2026.
//

#ifndef RAWENGINE_PRIMITIVE_H
#define RAWENGINE_PRIMITIVE_H
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "Component.h"

namespace core {
    struct GPUPrimitive {
        int type;
        glm::vec3  position;
        glm::vec3  rotation;
        glm::vec3  scale;
        glm::vec3  data;
        glm::vec3  color;
    };

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
            Plane,
            Terrain
        };

        PrimitiveType type = Sphere;
        glm::vec3 color = glm::vec3(1);
        glm::vec3 data = glm::vec3(1, 1, 1);

        GPUPrimitive GetGPUPrimitive() const;

        void Start() override;
        void OnInspectorGUI() override;

        nlohmann::json Serialize() override;
        void Deserialize(const nlohmann::json& json) override;
        const char* GetTypeName() override { return "Primitive"; }
    };

    REGISTER_COMPONENT(Primitive)
} // core


#endif //RAWENGINE_PRIMITIVE_H