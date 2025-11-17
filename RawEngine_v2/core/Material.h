//
// Created by micha on 20/10/2025.
//

#ifndef RAWENGINE_MATERIAL_H
#define RAWENGINE_MATERIAL_H

#include <string>
#include <unordered_map>
#include <glm/glm.hpp>
#include <glad/glad.h>


namespace core {
    class Material {
        GLuint shaderProgram;
        std::unordered_map<std::string, GLuint> textures;
        std::unordered_map<std::string, glm::vec4> vec4Uniforms;
        std::unordered_map<std::string, glm::mat4> mat4Uniforms;
        std::unordered_map<std::string, float> floatUniforms;

    public:
        std::string vertexShaderPath, fragmentShaderPath;

        Material(GLuint shaderProgram) : shaderProgram(shaderProgram) {}
        Material(const std::string &vertexShaderPath, const std::string &fragmentShaderPath);
        ~Material();

        void SetTexture(const std::string& name, GLuint tex);

        void SetVec4(const std::string& name, const glm::vec4& value);

        void SetMat4(const std::string& name, const glm::mat4& value);

        void SetFloat(const std::string& name, float value);

        void Bind();

    private:
        static GLuint GenerateShader(const std::string& shaderPath, GLuint shaderType);

        static const unsigned int GenerateShaderProgram(const GLuint vertexShader, const GLuint fragmentShader);
    };
}

#endif //RAWENGINE_MATERIAL_H