//
// Created by micha on 17/10/2025.
//

#ifndef RAWENGINE_GAMEOBJECT_H
#define RAWENGINE_GAMEOBJECT_H

#include <string>
#include <vector>
#include <typeinfo>
#include "Component.h"
#include "Transform.h"

namespace core {
    class GameObject {
    public:
        std::string name = "new GameObject";

        Transform transform;

        std::vector<Component*> components;

        GameObject();
        GameObject(const std::string& name);
        ~GameObject();

        Component* AddComponent(Component*);
        void RemoveComponent(Component*);

        template<typename T>
        T* GetComponent() {
            for (auto component: components) {
                if (auto match = dynamic_cast<T*>(component)) {
                    return match;
                }
            }

            printf_s("Could not find Component of type %s\n", typeid(T).name());
            return nullptr;
        }

        template<typename T>
        bool TryGetComponent(T*& pointer) {
            for (auto component: components) {
                if (auto match = dynamic_cast<T*>(component)) {
                    pointer = match;
                    return true;
                }
            }
            pointer = nullptr;
            return false;
        }

        void Start();

        void Update();
    };
}

#endif //RAWENGINE_GAMEOBJECT_H