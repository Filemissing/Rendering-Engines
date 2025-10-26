//
// Created by micha on 17/10/2025.
//

#ifndef RAWENGINE_COMPONENT_H
#define RAWENGINE_COMPONENT_H

namespace core {
    class GameObject; // forward declaration to fix inclusion loop

    class Component {
    public:
        GameObject* gameObject = nullptr;

        virtual void Start();

        virtual void Update();

        virtual ~Component();
    };
}

#endif //RAWENGINE_COMPONENT_H