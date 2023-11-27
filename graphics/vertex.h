//
// Created by 김준용 on 2023-11-17.
//

#ifndef VERTEX_H
#define VERTEX_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>

class Vertex {
private:
    uint32_t vao = 0, vbo = 0, ebo = 0, tbo = 0;
    int count;

public:
    Vertex() = default;

    Vertex(const std::vector<float> &vertices, const std::vector<unsigned int> &indices,
           const std::vector<float> &textures) {
        count = (int) indices.size();

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) nullptr);
        glEnableVertexAttribArray(0);

        glGenBuffers(1, &tbo);
        glBindBuffer(GL_ARRAY_BUFFER, tbo);
        glBufferData(GL_ARRAY_BUFFER, textures.size() * sizeof(float), textures.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *) nullptr);
        glEnableVertexAttribArray(1);

        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void bind() {
        glBindVertexArray(vao);
        if (ebo > 0) glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    }

    void unbind() {
        if (ebo > 0) glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void draw() {
        if (ebo > 0) glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
        else glDrawArrays(GL_TRIANGLES, 0, count);
    }

    void render() {
        bind();
        draw();
    }
};

#endif // VERTEX_H

