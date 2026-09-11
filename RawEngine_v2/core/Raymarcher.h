//
// Created by micha on 09/09/2026.
//

#ifndef RAWENGINE_RAYMARCHER_H
#define RAWENGINE_RAYMARCHER_H
#include "Assets/Material.h"
#include "Assets/model.h"
#include "Components/Camera.h"
#include "Components/Primitive.h"

namespace core {
    class Raymarcher {
        GLuint volumeTex;
        GLuint bakeFbo;
        Material* bakeMaterial;         // wraps your bake shader
        Material* marchMaterial;        // wraps march shader, samples volumeTex

        Model* quadModel;

        glm::ivec3 resolution;
        glm::vec3 worldMin, worldMax;   // volume bounds
        bool dirty = true;              // set true on any CSG param change

    public:
        int maxSteps = 96;
        float maxDist = 200.0f;
        float surfDist = 0.001f;

        Raymarcher();
        ~Raymarcher();

        void EnsureVolumeSized(glm::ivec3 newResolution);
        void DestroyFbo();
        void Bake(const std::vector<Primitive*>& primitives);
        void Render(GLuint targetFbo, GLuint sceneColorTex,
                                GLuint sceneDepthTex, Camera* cam);

        void SetWorldBounds(glm::vec3 min, glm::vec3 max);
        void MarkDirty() {dirty = true;}
        bool IsDirty() const { return dirty; }

        glm::ivec3 GetResolution() const { return resolution; }
        GLuint GetVolumeTexture() const { return volumeTex; }
    };
} // core

#endif //RAWENGINE_RAYMARCHER_H