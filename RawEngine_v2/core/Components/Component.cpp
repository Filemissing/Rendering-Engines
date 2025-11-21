//
// Created by micha on 17/10/2025.
//

#include "Component.h"
#include "../GameObject.h"

namespace core {
    Component::Component(GameObject* gameObject) {
        gameObject->AddComponent(this);
    }


    void Component::Start() {}
    void Component::Update() {}

    nlohmann::json Component::Serialize() {
        nlohmann::json json;
        json["type"] = "Component";
        return json;
    }
    void Component::Deserialize(const nlohmann::json& json) {}

    // component registry function
    std::unordered_map<std::string, std::function<Component*(GameObject*)>>& GetComponentRegistry() {
        static std::unordered_map<std::string, std::function<Component*(GameObject*)>> reg;
        return reg;
    }
}