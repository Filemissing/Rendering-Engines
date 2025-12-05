//
// Created by micha on 17/10/2025.

#include "./MeshRenderer.h"
#include "../assimpLoader.h"
#include "Camera.h"
#include "../GameObject.h"

namespace core {
    MeshRenderer::MeshRenderer(Model* model, Material* material)
    : model(model), material(material) {}

    MeshRenderer::~MeshRenderer() {
        delete model;
        delete material;
    }

    void MeshRenderer::Render() {
        if (!material || !model || !Camera::GetMainCamera()) return;

        material->SetMat4("mvpMatrix",
            Camera::GetMainCamera()->GetProjection() *
                Camera::GetMainCamera()->GetView() *
                gameObject->transform.GetMatrix());

        material->SetMat4("modelMatrix",
            gameObject->transform.GetMatrix());

        material->SetMat4("viewMatrix",
            Camera::GetMainCamera()->GetView());

        material->SetMat4("projectionMatrix",
            Camera::GetMainCamera()->GetProjection());

        material->Bind();
        model->render();
        glBindVertexArray(0);
    }

    Model* MeshRenderer::GetModel() const {return model;}
    Material* MeshRenderer::GetMaterial() const {return material;}

    nlohmann::json MeshRenderer::Serialize() {
        nlohmann::json json;
        json["type"] = "MeshRenderer";
        json["model"] = model->assetPath;

        json["material"]["vertex"] = material->vertexShaderPath;
        json["material"]["fragment"] = material->fragmentShaderPath;

        json["material"]["smoothness"] = material->smoothness;
        json["material"]["metallic"] = material->metallic;

        return json;
    }

    void MeshRenderer::Deserialize(const nlohmann::json& json) {
        model = AssimpLoader::loadModel(json["model"]);

        material = new Material(
            json["material"]["vertex"],
            json["material"]["fragment"]
            );

        material->smoothness = json["material"]["smoothness"];
        material->metallic = json["material"]["metallic"];
    }
}
