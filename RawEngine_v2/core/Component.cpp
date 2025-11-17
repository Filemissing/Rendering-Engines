//
// Created by micha on 17/10/2025.
//

#include "Component.h"
#include "GameObject.h"

namespace core {
    void Component::Start() {}
    void Component::Update() {}

    nlohmann::json Component::Serialize() {
        nlohmann::json json;
        json["type"] = "Component";
        return json;
    }
    void Component::Deserialize(const nlohmann::json& json) {}

    Component::~Component() {
        gameObject = nullptr;
    }

    // component registry function
    std::unordered_map<std::string, std::function<Component*()>>& GetComponentRegistry() {
        static std::unordered_map<std::string, std::function<Component*()>> reg;
        return reg;
    }
}