//
// Created by 김준용 on 2023-11-20.
//

#ifndef LINE_H
#define LINE_H

#pragma once

#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"

class Line {
private:
    unsigned int vao = 0, vbo = 0;
    glm::vec3 start, end;
    glm::vec3 color;

public:
    Line() = default;

    Line(glm::vec3 start, glm::vec3 end, glm::vec3 color) : color(color), start(start), end(end) {
        float vertices[] = {
                start.x, start.y, start.z,
                end.x, end.y, end.z
        };

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);

        glBindVertexArray(vao);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    ~Line() = default;

    void draw() const {
        glBindVertexArray(vao);
        glDrawArrays(GL_LINES, 0, 2);
    }

    void clear() const {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
    }

    static void render(Shader &shader, std::vector<Line> &lines) {
        shader.enable();
        for (auto &line: lines) {
            shader.setUniform3f("color", line.color);
            line.draw();
        }
        shader.disable();
    }
};

#endif // LINE_H
