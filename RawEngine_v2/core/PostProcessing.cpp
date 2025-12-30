//
// Created by micha on 30/12/2025.
//

#include "PostProcessing.h"

#include <glm/gtc/type_ptr.inl>

#include "imgui.h"

namespace core {
    PostProcessingParameter::PostProcessingParameter(
        const std::string &name,
        const type_info& typeId,
        float floatValue,
        glm::vec4 vec4Value,
        glm::mat4 mat4Value)
            : name(name)
    {
        if (typeId == typeid(float)) {
            this->floatValue = floatValue;
            typeName = "float";
        } else if (typeId == typeid(glm::vec4)) {
            this->vec4Value = vec4Value;
            typeName = "vec4";
        } else if (typeId == typeid(glm::mat4)) {
            this->mat4Value = mat4Value;
            typeName = "mat4";
        }
    }

    PostProcessEffect::PostProcessEffect(const std::string& name, std::vector<PostProcessPass*> passes) : name(name), passes(passes) {}
    void PostProcessEffect::OnGUI() {
        for (PostProcessPass* pass : passes) {
            for (PostProcessingParameter& parameter : pass->parameters) {
                if (parameter.typeName == "float") {
                    ImGui::DragFloat(parameter.name.c_str(), &parameter.floatValue);
                }
                else if (parameter.typeName == "vec4") {
                    ImGui::DragFloat4(parameter.name.c_str(), glm::value_ptr(parameter.vec4Value));
                }
            }
        }
    }

} // core