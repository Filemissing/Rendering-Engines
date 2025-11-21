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
            Camera::GetMainCamera()->projection *
                Camera::GetMainCamera()->getView() *
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
        json["model"] = model->assetPath;
        json["material"]["vertex"] = material->vertexShaderPath;
        json["material"]["fragment"] = material->fragmentShaderPath;
        return json;
    }

    void MeshRenderer::Deserialize(const nlohmann::json& json) {
        model = AssimpLoader::loadModel(json["model"]);
        material = new Material(
            json["material"]["vertex"],
            json["material"]["fragment"]
            );
    }
}
