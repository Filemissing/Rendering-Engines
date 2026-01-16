//
// Created by micha on 21/11/2025.
//

#ifndef RAWENGINE_LIGHT_H
#define RAWENGINE_LIGHT_H
#include <glm/vec4.hpp>

#include "Component.h"

namespace core {
    class Light : public Component {
    public:
        using Component::Component; // inherit Constructors

        enum LightType {
            Directional,
            Point
        };

        LightType lightType = Point;
        glm::vec4 color = glm::vec4(1.0f);
        float attenuation = 1.0f;

        void Start() override;

        const char* GetTypeName() override { return "Light"; }

        void OnInspectorGUI() override;

        nlohmann::json Serialize() override;
        void Deserialize(const nlohmann::json& json) override;
    };

    REGISTER_COMPONENT(Light)
} // core

#endif //RAWENGINE_LIGHT_H