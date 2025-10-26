//
// Created by micha on 17/10/2025.
//

#include "GameObject.h"
#include "Component.h"

namespace core {
    GameObject::GameObject() {
        transform = Transform();
        transform.gameObject = this;

        components = std::vector<Component*>();
    }
    GameObject::GameObject(const std::string& name) : name(name) {
        transform = Transform();
        transform.gameObject = this;

        components = std::vector<Component*>();
    }
    GameObject::~GameObject() {
        for (auto component : components) {
            delete component;
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
}
