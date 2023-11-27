//
// Created by 김준용 on 2023-11-24.
//

#pragma once

#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"

class Hint {
private:
    unsigned int vao = 0, vbo = 0, ebo = 0;

public:
    glm::vec4 color = glm::vec4(0.2f, 0.7f, 1.0f, 0.5f);
    glm::vec3 position = glm::vec3(100, 0, -0.075);

    bool show = false;

    explicit Hint(glm::vec4 color) : color(color) {
        float vertices[] = {
                // Positive X
                0.5f, 0.5f, -0.5f,
                0.5f, -0.5f, -0.5f,
                0.5f, -0.5f, 0.5f,
                0.5f, 0.5f, 0.5f,

                // Negative X
                -0.5f, 0.5f, -0.5f,
                -0.5f, -0.5f, -0.5f,
                -0.5f, -0.5f, 0.5f,
                -0.5f, 0.5f, 0.5f,

                // Positive Y
                -0.5f, 0.5f, 0.5f,
                -0.5f, 0.5f, -0.5f,
                0.5f, 0.5f, -0.5f,
                0.5f, 0.5f, 0.5f,

                // Negative Y
                -0.5f, -0.5f, 0.5f,
                -0.5f, -0.5f, -0.5f,
                0.5f, -0.5f, -0.5f,
                0.5f, -0.5f, 0.5f,

                // Positive Z
                -0.5f, 0.5f, 0.5f,
                -0.5f, -0.5f, 0.5f,
                0.5f, -0.5f, 0.5f,
                0.5f, 0.5f, 0.5f,

                // Negative Z
                -0.5f, 0.5f, -0.5f,
                -0.5f, -0.5f, -0.5f,
                0.5f, -0.5f, -0.5f,
                0.5f, 0.5f, -0.5f
        };

        unsigned int indices[] = {
                // Positive X
                0, 1, 3,
                3, 1, 2,

                // Negative X
                4, 5, 7,
                7, 5, 6,

                // Positive Y
                8, 9, 11,
                11, 9, 10,

                // Negative Y
                12, 13, 15,
                15, 13, 14,

                // Positive Z
                16, 17, 19,
                19, 17, 18,

                // Negative Z
                20, 21, 23,
                23, 21, 22
        };

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        glBindVertexArray(vao);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    ~Hint() = default;

    void draw() const {
        glBindVertexArray(vao);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
    }

    void clear() const {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ebo);
    }

    static void render(Shader &shader, std::vector<Hint> &hints) {
        shader.enable();
        for (auto &hint: hints) {
            shader.setUniform1i("render", hint.show);
            shader.setUniform4f("color", hint.color);
            glm::mat4 transform = glm::mat4(1.0);
            transform = glm::translate(transform, hint.position);
            transform = glm::scale(transform, glm::vec3(200, 0.001, 0.05));
            shader.setUniformMat4f("transform", transform);
            hint.draw();
        }
        shader.disable();
    }
};
