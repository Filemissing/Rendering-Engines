//
// Created by micha on 17/10/2025.
//

#include "GameObject.h"
#include "Component.h"
#include "ComponentFactory.h"

namespace core {
    GameObject::GameObject() {
        transform = Transform();
        transform.gameObject = this;

        components = std::vector<Component*>();
    }
    GameObject::~GameObject() {
        for (auto component : components) {
            delete component;
        }

        for (auto child : transform.children) {
            delete child->gameObject;
        }
    }

    Component* GameObject::AddComponent(core::Component* component) {
        components.push_back(component);
        component->gameObject = this;

        return component;
    }
    void GameObject::RemoveComponent(Component* component) {
        auto found = std::find(components.begin(), components.end(), component);
        if (found != components.end()) {
            delete *found;                  // Call component destructor
            components.erase(found);     // Remove pointer from vector
        }
    }

    void GameObject::Start() {
        transform.Start();
        for (auto component : components) {
            component->Start();
        }

        for (auto child : transform.children) {
            child->gameObject->Start();
        }
    }
    void GameObject::Update() {
        transform.Update();
        for (auto component: components) {
            component->Update();
        }

        for (auto child : transform.children) {
            child->gameObject->Update();
        }
    }

    nlohmann::json GameObject::Serialize() {
        nlohmann::json objJson = nlohmann::json::object();
        objJson["name"] = name;

        for (Component* component : components) {
            nlohmann::json compJson = component->Serialize();
            objJson["components"].push_back(compJson);
        }

        objJson["transform"] = transform.Serialize();

        return objJson;
    }
    void GameObject::Deserialize(nlohmann::json json) {
        name = json["name"];
        for (auto componentData: json["components"]) {
            auto comp = ComponentFactory::Create(componentData["type"]);
            comp->Deserialize(componentData);
        }
        transform.Deserialize(json["transform"]);
    }
}
