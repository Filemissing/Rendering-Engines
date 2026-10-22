//
// Created by micha on 13/09/2026.
//

#ifndef RAWENGINE_COMPUTESHADER_H
#define RAWENGINE_COMPUTESHADER_H
#include <glm/glm.hpp>
#include <glad/glad.h>

#include "../Asset.h"

namespace core {
    class ComputeShader : public Asset {
        GLuint program;

        std::unordered_map<std::string, GLuint> textures2D;
        std::unordered_map<std::string, GLuint> textures3D;
        std::unordered_map<std::string, glm::vec3> vec3Uniforms;
        std::unordered_map<std::string, glm::vec4> vec4Uniforms;
        std::unordered_map<std::string, glm::mat4> mat4Uniforms;
        std::unordered_map<std::string, float> floatUniforms;
        std::unordered_map<std::string, int> intUniforms;

    public:
        explicit ComputeShader(const std::string& computeShaderPath);
        ~ComputeShader();

        void Bind() const; // glUseProgram only — no vertex/fragment concerns

        // uniform setters — mirror Material's, reuse the same pattern
        void SetTexture2D(const std::string& name, GLuint tex);
        void SetTexture3D(const std::string& name, GLuint tex);
        void SetVec3(const std::string& name, const glm::vec3& value);
        void SetVec4(const std::string& name, const glm::vec4& value);
        void SetMat4(const std::string& name, const glm::mat4& value);
        void SetFloat(const std::string& name, float value);
        void SetInt(const std::string& name, int value);

        void BindImage(GLuint unit, GLuint texture, GLenum access, GLenum format);
        // wraps glBindImageTexture — this is the compute-specific equivalent of Material's texture-unit binding loop

        void Dispatch(int groupsX, int groupsY, int groupsZ);
        // wraps glDispatchCompute + appropriate glMemoryBarrier

        GLuint GetProgram() const { return program; }
    };
} // core

#endif //RAWENGINE_COMPUTESHADER_H