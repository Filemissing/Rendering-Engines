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
        MeshRenderer(Model* model, Material* material);

        Model* GetModel() const;

        Material* GetMaterial() const;

        virtual void Update();
    };
}

#endif //RAWENGINE_MESHRENDERER_H