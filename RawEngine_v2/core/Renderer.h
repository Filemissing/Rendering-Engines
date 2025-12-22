//
// Created by micha on 02/12/2025.
//

#ifndef RAWENGINE_RENDERER_H
#define RAWENGINE_RENDERER_H
#include "assimpLoader.h"
#include "Assets/Material.h"
#include "Assets/model.h"
#include "Assets/Scene.h"

namespace core {
    struct PostProcessPass {
        PostProcessPass(Material* material) : material(material) {}
        Material* material;
    };
    struct PostProcessEffect {
        PostProcessEffect(const std::string& name, std::vector<PostProcessPass*> passes);

        std::string name;
        bool enabled = true;
        std::vector<PostProcessPass*> passes;
    };

    class Renderer {
    public:
        Model* quadModel;

        Renderer();
        ~Renderer();

        GLuint sceneFbo;
        GLuint sceneColor;
        GLuint sceneDepth;

        GLuint ppFbo[2];
        GLuint ppTex[2];
        GLuint ppDepth[2];

        std::vector<PostProcessEffect*> postProcessingEffects = std::vector<PostProcessEffect*>();
        void RenderScene(Scene*& scene, const GLuint& finalFbo, const GLuint& finalTexture, int width, int height);
        void EnsureFboSized(int width, int height);
        void DestroyFbo();
    };
} // core

#endif //RAWENGINE_RENDERER_H