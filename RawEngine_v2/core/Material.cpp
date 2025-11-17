//
// Created by micha on 20/10/2025.
//
#include <fstream>
#include <sstream>
#include "Material.h"

#include <glm/gtc/type_ptr.hpp>

#include "Camera.h"


namespace core {
    Material::Material(const std::string &vertexShaderPath, const std::string &fragmentShaderPath)
        : vertexShaderPath(vertexShaderPath), fragmentShaderPath(fragmentShaderPath) {
        const GLuint vertexShader = GenerateShader(vertexShaderPath, GL_VERTEX_SHADER);
        const GLuint fragmentShader = GenerateShader(fragmentShaderPath, GL_FRAGMENT_SHADER);

        const unsigned int program = GenerateShaderProgram(vertexShader, fragmentShader);

        shaderProgram = program;
    }
    Material::~Material() {
        glDeleteProgram(shaderProgram);
    }

    void Material::SetTexture(const std::string& name, GLuint tex) {
        textures[name] = tex;
    }
    void Material::SetVec4(const std::string& name, const glm::vec4& value) {
        vec4Uniforms[name] = value;
    }
    void Material::SetMat4(const std::string& name, const glm::mat4& value) {
        mat4Uniforms[name] = value;
    }
    void Material::SetFloat(const std::string& name, float value) {
        floatUniforms[name] = value;
    }

    void Material::Bind() {
        glUseProgram(shaderProgram);

        // upload uniforms
        for (auto& [name, value] : vec4Uniforms) {
            GLint loc = glGetUniformLocation(shaderProgram, name.c_str());
            if (loc == -1) continue; // uniform not found
            glUniform4fv(loc, 1, glm::value_ptr(value));
        }
        for (auto& [name, value] : mat4Uniforms) {
            GLint loc = glGetUniformLocation(shaderProgram, name.c_str());
            if (loc == -1) continue; // uniform not found
            glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value));
        }
        for (auto& [name, value] : floatUniforms) {
            GLint loc = glGetUniformLocation(shaderProgram, name.c_str());
            if (loc == -1) continue; // uniform not found
            glUniform1f(loc, value);
        }

        // bind textures
        int i = 0;
        for (auto& [name, tex] : textures) {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, tex);
            glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), i);
            i++;
        }
    }

    // Private
    // helper function for reading shader files
    std::string ReadFileToString(const std::string &filePath) {
        std::ifstream fileStream(filePath, std::ios::in);
        if (!fileStream.is_open()) {
            printf("Could not open file: %s\n", filePath.c_str());
            return "";
        }
        std::stringstream buffer;
        buffer << fileStream.rdbuf();
        return buffer.str();
    }

    GLuint Material::GenerateShader(const std::string &shaderPath, GLuint shaderType) {
        printf("Loading shader: %s\n", shaderPath.c_str());
        const std::string shaderText = ReadFileToString(shaderPath);
        const GLuint shader = glCreateShader(shaderType);
        const char *s_str = shaderText.c_str();
        glShaderSource(shader, 1, &s_str, nullptr);
        glCompileShader(shader);
        GLint success = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetShaderInfoLog(shader, 512, nullptr, infoLog);
            printf("Error! Shader issue [%s]: %s\n", shaderPath.c_str(), infoLog);
        }
        return shader;
    }

    const unsigned int Material::GenerateShaderProgram(const GLuint vertexShader, const GLuint fragmentShader) {
        int success;
        char infoLog[512];
        const unsigned int modelShaderProgram = glCreateProgram();
        glAttachShader(modelShaderProgram, vertexShader);
        glAttachShader(modelShaderProgram, fragmentShader);
        glLinkProgram(modelShaderProgram);
        glGetProgramiv(modelShaderProgram, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(modelShaderProgram, 512, nullptr, infoLog);
            printf("Error! Making Shader Program: %s\n", infoLog);
        }

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        return modelShaderProgram;
    }
}
