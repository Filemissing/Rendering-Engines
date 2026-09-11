//
// Created by micha on 02/12/2025.
//

#ifndef RAWENGINE_RENDERER_H
#define RAWENGINE_RENDERER_H
#include "assimpLoader.h"
#include "Assets/Material.h"
#include "Assets/model.h"
#include "Assets/Scene.h"
#include "PostProcessing.h"
#include "Raymarcher.h"

namespace core {
    class Renderer {
        GLuint m_timeQuery = 0;
        bool m_queryInFlight = false;

        GLuint sceneFbo;
        GLuint sceneColor;
        GLuint sceneDepth;

        GLuint ppFbo[2];
        GLuint ppTex[2];
        GLuint ppDepth[2];

        Model* quadModel;

        Raymarcher* m_raymarcher = nullptr;

    public:
        Renderer();
        ~Renderer();

        std::vector<PostProcessEffect*> postProcessingEffects = std::vector<PostProcessEffect*>();
        void RenderScene(Scene*& scene, const GLuint& finalFbo, const GLuint& finalTexture, int width, int height);
        void EnsureFboSized(int width, int height);
        void DestroyFbo();

        Raymarcher* GetRaymarcher() const { return m_raymarcher; }
    };
} // core

#endif //RAWENGINE_RENDERER_H