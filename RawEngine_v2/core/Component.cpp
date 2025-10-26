//
// Created by micha on 17/10/2025.
//

#include "Component.h"
#include "GameObject.h"

namespace core {
    void Component::Start() {}

    void Component::Update() {}

    Component::~Component() {
        gameObject = nullptr;
    }

}