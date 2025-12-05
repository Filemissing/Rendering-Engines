//
// Created by micha on 17/11/2025.
//

#include "ComponentFactory.h"

namespace core {
    Component* ComponentFactory::Create(const std::string& name, GameObject* gameObject) {
        auto obj = GetComponentRegistry()[name](gameObject);
        obj->Start();
        return obj;
    }
}
