//
// Created by micha on 21/11/2025.
//

#include "Light.h"

#include "../GameObject.h"
#include "../Assets/Scene.h"

namespace core {
    void Light::Start() {
        gameObject->scene->lights.push_back(this);
    }

    nlohmann::json Light::Serialize() {
        nlohmann::json json;
        json["type"] = "Light";
        json["lightType"] = lightType;
        json["color"] = {};

        json["color"]["r"] = color.r;
        json["color"]["g"] = color.g;
        json["color"]["b"] = color.b;
        json["color"]["a"] = color.a;

        json["attenuation"] = attenuation;

        return json;
    }
    void Light::Deserialize(const nlohmann::json& json) {
        lightType = json["lightType"];

        color = glm::vec4(
            json["color"]["r"], json["color"]["g"],
            json["color"]["b"], json["color"]["a"]
            );

        attenuation = json["attenuation"];
    }
} // core