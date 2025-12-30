//
// Created by micha on 30/12/2025.
//

#ifndef RAWENGINE_POSTPROCESSING_H
#define RAWENGINE_POSTPROCESSING_H
#include "Assets/Material.h"

namespace core {
    struct PostProcessingParameter {
        PostProcessingParameter(
            const std::string& name,
            const type_info& typeId,
            float floatValue = 0,
            glm::vec4 vec4Value = glm::vec4(0.0f),
            glm::mat4 mat4Value = glm::mat4(1.0f));

        std::string name;
        std::string typeName;

        float floatValue;
        glm::vec4 vec4Value;
        glm::mat4 mat4Value;
    };
    struct PostProcessPass {
        PostProcessPass(Material* material, const std::vector<PostProcessingParameter>& parameters = std::vector<PostProcessingParameter>()) : material(material), parameters(parameters) {}

        Material* material;
        std::vector<PostProcessingParameter> parameters;
    };
    struct PostProcessEffect {
        PostProcessEffect(const std::string& name, std::vector<PostProcessPass*> passes);

        std::string name;
        bool enabled = true;
        std::vector<PostProcessPass*> passes;

        void OnGUI();
    };

} // core

#endif //RAWENGINE_POSTPROCESSING_H