//
// Created by micha on 17/11/2025.
//

#ifndef RAWENGINE_COMPONENTFACTORY_H
#define RAWENGINE_COMPONENTFACTORY_H
#include "Component.h"

namespace core {
    class ComponentFactory {
    public:
        static Component* Create(const std::string &);
    };
}

#endif //RAWENGINE_COMPONENTFACTORY_H