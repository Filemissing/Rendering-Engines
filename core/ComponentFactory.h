//
// Created by micha on 17/11/2025.
//

#ifndef RAWENGINE_COMPONENTFACTORY_H
#define RAWENGINE_COMPONENTFACTORY_H
#include "Components/Component.h"

namespace core {
    class ComponentFactory {
    public:
        static Component* Create(const std::string &, GameObject*);
    };
}

#endif //RAWENGINE_COMPONENTFACTORY_H