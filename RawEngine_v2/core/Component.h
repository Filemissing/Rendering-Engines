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
        GameObject* gameObject = nullptr; // backreference - do not serialize

        virtual void Start();
        virtual void Update();

        virtual nlohmann::json Serialize();
        virtual void Deserialize(const nlohmann::json& json);

        virtual ~Component();
    };

    // component registry function
    std::unordered_map<std::string, std::function<Component*()>>& GetComponentRegistry();
}

#define REGISTER_COMPONENT(T)                                               \
namespace {                                                                 \
    struct T##AutoRegister {                                                \
        T##AutoRegister() {                                                 \
            core::GetComponentRegistry()[#T] = [](){ return new T(); };     \
        }                                                                   \
    };                                                                      \
    static T##AutoRegister global_##T##AutoRegister;                        \
}
#endif //RAWENGINE_COMPONENT_H