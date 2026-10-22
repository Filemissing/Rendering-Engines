//
// Created by micha on 25/10/2025.
//

#ifndef RAWENGINE_HIERARCHYNODE_H
#define RAWENGINE_HIERARCHYNODE_H

#include "../../core/GameObject.h"

namespace editor::UIElements {
    class HierarchyNode {
    public:
        core::GameObject* object = nullptr;
        bool isOpen = false;  // whether the node’s tree is expanded

        HierarchyNode() = default;
        HierarchyNode(core::GameObject* obj) : object(obj) {}
    };
} // editor

#endif //RAWENGINE_HIERARCHYNODE_H