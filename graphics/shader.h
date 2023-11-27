#ifndef SHADER_H
#define SHADER_H

#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>

class Shader {
public:
    unsigned int ID;

    Shader(const std::string &vertexPath, const std::string &fragmentPath) {
        // 1. retrieve the vertex/fragment source code from filePath
        std::string vertexCode, fragmentCode;
        std::ifstream vShaderFile, fShaderFile;

        // Ensure ifstream objects can throw exceptions:
        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        try {
            // Open files
            if (vertexPath.length() < 14 || vertexPath.substr(0, 15) != "assets/shaders/")
                vShaderFile.open("assets/shaders/" + vertexPath);
            else vShaderFile.open(vertexPath);
            if (fragmentPath.length() < 14 || fragmentPath.substr(0, 15) != "assets/shaders/")
                fShaderFile.open("assets/shaders/" + fragmentPath);
            else fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;

            // Read file's buffer contents into streams
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();

            // Close file handlers
            vShaderFile.close();
            fShaderFile.close();

            // Convert stream into string
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        } catch (std::ifstream::failure &e) {
            std::cout << "Shader file not successfully read" << std::endl;
        }

        const char *vShaderCode = vertexCode.c_str();
        const char *fShaderCode = fragmentCode.c_str();

        // Compile shaders
        unsigned int vertex, fragment;

        // Vertex shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, nullptr);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");

        // Fragment Shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, nullptr);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");

        // Shader Program
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM");

        // Delete the shaders as they're linked into our program now and no longer necessary
        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }

    // Activate the shader
    void enable() const {
        glUseProgram(ID);
    }

    void disable() const {
        glUseProgram(0);
    }

    // Utility uniform functions
    void setUniform1i(const std::string &name, int value) {
        glUniform1i(getUniform(name), value);
    }

    void setUniform1f(const std::string &name, float value) {
        glUniform1f(getUniform(name), value);
    }

    void setUniform2f(const std::string &name, float x, float y) {
        glUniform2f(getUniform(name), x, y);
    }

    void setUniform2f(const std::string &name, std::pair<float, float> value) {
        glUniform2f(getUniform(name), value.first, value.second);
    }

    void setUniform3f(const std::string &name, float x, float y, float z) {
        glUniform3f(getUniform(name), x, y, z);
    }

    void setUniform3f(const std::string &name, glm::vec3 &data) {
        glUniform3f(getUniform(name), data.x, data.y, data.z);
    }

    void setUniform4f(const std::string &name, float x, float y, float z, float w) {
        glUniform4f(getUniform(name), x, y, z, w);
    }

    void setUniform4f(const std::string &name, glm::vec4 &data) {
        glUniform4f(getUniform(name), data.x, data.y, data.z, data.w);
    }

    void setUniformMat4f(const std::string &name, const glm::mat4 &matrix) {
        glUniformMatrix4fv(getUniform(name), 1, GL_FALSE, &matrix[0][0]);
    }

private:
    // Utility function for checking shader compilation/linking errors.
    static void checkCompileErrors(unsigned int shader, const std::string &type) {
        int success;
        char infoLog[1024];

        if (type != "PROGRAM") {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
                std::cerr << "Shader compilation error of type: " << type << "\n" << infoLog
                          << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        } else {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
                std::cerr << "Program linking error of type: " << type << "\n" << infoLog
                          << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }

    std::map<std::string, int> cache;

    int getUniform(const std::string &name) {
        if (cache.find(name) != cache.end())
            return cache[name];

        int result = glGetUniformLocation(ID, name.c_str());

        if (result == -1) {
            std::cerr << "Could not find uniform variable \'" + name + "\'!" << std::endl;
        } else {
            cache.insert({name, result});
        }

        return result;
    }
};

#endif