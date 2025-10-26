//
// Created by micha on 17/10/2025.

#include "MeshRenderer.h"
#include "assimpLoader.h"

namespace core {
    MeshRenderer::MeshRenderer(Model* model, Material* material)
    : model(model), material(material) {}

    void MeshRenderer::Render() {
        if (!material || !model) return;

        material->Bind();
        model->render();
        glBindVertexArray(0);
    }

    Model* MeshRenderer::GetModel() const {
        return model;
    }

    Material* MeshRenderer::GetMaterial() const {
        return material;
    }

    void MeshRenderer::Update() {
        Render();
    }
}
