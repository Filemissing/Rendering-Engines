#pragma once

#include <string>
#include <vector>
#include <glm/ext/matrix_float4x4.hpp>
#include "mesh.h"

namespace core {
    class Model : public Asset {
    private:
        std::vector<Mesh> meshes;
        glm::mat4 modelMatrix;
    public:
        std::string assetPath;
        Model(std::vector<Mesh>&& meshes)
            : meshes(std::move(meshes)), modelMatrix(1) {}

        void render();

        void translate(glm::vec3 translation);
        void rotate(glm::vec3 axis, float radians);
        void scale(glm::vec3 scale);
        glm::mat4 getModelMatrix() const;
    };
}