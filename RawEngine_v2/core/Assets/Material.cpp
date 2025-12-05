//
// Created by micha on 20/10/2025.
//
#include <fstream>
#include <sstream>
#include "Material.h"

#include <glm/gtc/type_ptr.hpp>

#include "../../editor/Editor.h"
#include "../Components/Camera.h"
#include "../Components/Light.h"


namespace core {
    Texture* Material::defaultTexture = nullptr;

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

    void UploadLights(GLuint shader, const std::vector<Light*>& lights)
    {
        glUseProgram(shader);

        for (int i = 0; i < lights.size(); i++)
        {
            const Light* light = lights[i];

            // Build uniform name dynamically
            std::string base = "lights[" + std::to_string(i) + "].";

            glUniform1i(glGetUniformLocation(shader, (base + "type").c_str()), light->lightType);

            glUniform4fv(glGetUniformLocation(shader, (base + "color").c_str()),
                         1, glm::value_ptr(light->color));

            glUniform3fv(glGetUniformLocation(shader, (base + "position").c_str()),
                         1, glm::value_ptr(light->gameObject->transform.position));

            glUniform3fv(glGetUniformLocation(shader, (base + "rotation").c_str()),
                         1, glm::value_ptr(light->gameObject->transform.forward));

            glUniform1f(glGetUniformLocation(shader, (base + "attenuation").c_str()), light->attenuation);
        }

        GLint loc = glGetUniformLocation(shader, "lightsAmount");
        if (loc == -1) return;
        glUniform1i(loc, lights.size());
    }
    void Material::Bind() {
        glUseProgram(shaderProgram);

        if (textures.find("_MainTex") == textures.end()) {
            if (defaultTexture == nullptr) defaultTexture = new Texture("Assets/textures/white.png");
            this->SetTexture("_MainTex", defaultTexture->getId());
        }

        this->SetFloat("smoothness", this->smoothness);
        this->SetFloat("metallic", this->metallic);

        UploadLights(shaderProgram, editor::Editor::activeScene->lights);

        this->SetVec4("ambientColor", editor::Editor::activeScene->ambientColor);
        this->SetFloat("ambientIntensity", editor::Editor::activeScene->ambientIntensity);

        { // bind viewDirection
            GLint loc = glGetUniformLocation(shaderProgram, "cameraPos");
            if (loc != -1) {
                glUniform3fv(loc, 1, glm::value_ptr(Camera::GetMainCamera()->gameObject->transform.position));
            }
        }

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
