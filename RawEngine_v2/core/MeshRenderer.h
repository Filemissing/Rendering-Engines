//
// Created by micha on 17/10/2025.
//

#ifndef RAWENGINE_MESHRENDERER_H
#define RAWENGINE_MESHRENDERER_H

#include "Component.h"
#include "Material.h"
#include "model.h"

namespace core {
    class MeshRenderer : public Component {
        Model* model;
        Material* material;

        void Render();

    public:
        MeshRenderer() : model(nullptr), material(nullptr) {}
        MeshRenderer(Model* model, Material* material);

        Model* GetModel() const;
        Material* GetMaterial() const;

        void Update() override;

        nlohmann::json Serialize() override;
        void Deserialize(const nlohmann::json&) override;
    };
    REGISTER_COMPONENT(MeshRenderer)
}

#endif //RAWENGINE_MESHRENDERER_H