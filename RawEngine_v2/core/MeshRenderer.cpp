//
// Created by micha on 17/10/2025.

#include "MeshRenderer.h"
#include "assimpLoader.h"
#include "Camera.h"
#include "GameObject.h"

namespace core {
    MeshRenderer::MeshRenderer(Model* model, Material* material)
    : model(model), material(material) {}

    void MeshRenderer::Render() {
        if (!material || !model || !Camera::mainCamera) return;

        material->SetMat4("mvpMatrix",
            Camera::mainCamera->projection *
                Camera::mainCamera->getView() *
                gameObject->transform.GetMatrix());

        material->Bind();
        model->render();
        glBindVertexArray(0);
    }

    Model* MeshRenderer::GetModel() const {return model;}
    Material* MeshRenderer::GetMaterial() const {return material;}

    void MeshRenderer::Update() {
        Render();
    }

    nlohmann::json MeshRenderer::Serialize() {
        nlohmann::json json;
        json["type"] = "MeshRenderer";
        // ToDo: setup GUID's for serializing asset references (e.g. model, material)
        return json;
    }

    void MeshRenderer::Deserialize(const nlohmann::json& json) {

    }
}
