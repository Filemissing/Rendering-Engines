//
// Created by micha on 26/03/2026.
//

#include "Primitive.h"

#include "imgui.h"
#include "../GameObject.h"
#include "../Assets/Scene.h"

namespace core {
    void Primitive::Start() {
        gameObject->scene->primitives.push_back(this);
    }

    Primitive::~Primitive() {
        auto it = std::find(gameObject->scene->primitives.begin(), gameObject->scene->primitives.end(), this);
        if (it != gameObject->scene->primitives.end()) {
            gameObject->scene->primitives.erase(it);
        }
    }

    void Primitive::OnInspectorGUI() {
        static const char* lightTypeLabels[] = {
            "Sphere",
            "Box",
            "Pyramid",
            "Capsule",
            "Cylinder",
            "Torus",
            "Cone",
            "Plane"
        };

        int value = static_cast<int>(type);

        if (ImGui::Combo(
            "primitiveType",
            &value,
            lightTypeLabels,
            IM_ARRAYSIZE(lightTypeLabels)))
        {
            type = static_cast<PrimitiveType>(value);
        }

        ImGui::SliderFloat3("color", glm::value_ptr(color), 0.0f, 1.0f);
        ImGui::DragFloat3("halfExtents", glm::value_ptr(halfExtents));
    }

    nlohmann::json Primitive::Serialize() {
        nlohmann::json json;
        json["type"] = "Primitive";
        json["primitiveType"] = type;
        json["color"] = {};

        json["color"]["r"] = color.r;
        json["color"]["g"] = color.g;
        json["color"]["b"] = color.b;

        json["halfExtents"] = {};
        json["halfExtents"]["x"] = halfExtents.x;
        json["halfExtents"]["y"] = halfExtents.y;
        json["halfExtents"]["z"] = halfExtents.z;

        return json;
    }
    void Primitive::Deserialize(const nlohmann::json& json) {
        type = json["primitiveType"];

        color = glm::vec3(
            json["color"]["r"],
            json["color"]["g"],
            json["color"]["b"]
            );

        halfExtents = glm::vec3(
            json["halfExtents"]["x"],
            json["halfExtents"]["y"],
            json["halfExtents"]["z"]
            );
    }
} // core