//
// Created by micha on 17/10/2025.
//

#ifndef RAWENGINE_COMPONENT_H
#define RAWENGINE_COMPONENT_H
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>

namespace core {
    class GameObject; // forward declaration to fix inclusion loop

    class Component {
    public:
        Component() = default;
        Component(GameObject* gameObject);
        virtual ~Component() = default;

        GameObject* gameObject = nullptr; // backreference - do not serialize

        virtual void Start();
        virtual void Update();

        virtual nlohmann::json Serialize();
        virtual void Deserialize(const nlohmann::json& json);
    };

    // component registry function
    std::unordered_map<std::string, std::function<Component*(GameObject*)>>& GetComponentRegistry();
}

#define REGISTER_COMPONENT(Type)                                            \
    namespace {                                                             \
        Component* Create_##Type(GameObject* go) { return new Type(go); }   \
        const bool registered_##Type = [](){                                \
        GetComponentRegistry()[#Type] = Create_##Type;                      \
        return true;                                                        \
    }();                                                                    \
}
#endif //RAWENGINE_COMPONENT_H