#include "text_mesh.hpp"
#include "text_renderer.hpp"
#include "gl_buffer.hpp"
#include "glad/gl.h"
#include <unordered_map>
#include <vector>


TextMesh::TextMesh(spdlog::logger* logger, const std::string &text,
                   std::unordered_map<char, GlyphData> glyphhs, float fontSize)
    : text(text), glyphData(glyphhs), logger(logger), fontSize(fontSize) {
    createMesh();
}

TextMesh::~TextMesh() {
    if (vaoExists) {
        glDeleteVertexArrays(1, &vao);
        vaoExists = false;
    }
}

void TextMesh::render() {
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, count);
    glBindVertexArray(0);
}

void TextMesh::createMesh() {
    if (text.size() <= 0)
        return;

    if (vaoExists) {
        glDeleteVertexArrays(1, &vao);
        vaoExists = false;
    }

    std::vector<float> vertices;

    for (int i = 0; i < text.size(); i++) {
        char c = text[i];

        GlyphData data = glyphData[c - 32];

        int characterWidth = static_cast<int>(glyphWidth * fontSize);
        int characterHeight = static_cast<int>(glyphHeight * fontSize);

        float x0 = (float)i * characterWidth;
        float x1 = x0 + characterWidth;
        float y0 = 0.0f;
        float y1 = (float)characterHeight;

        // Triangle 1 (top-left, bottom-left, top-right)
        vertices.push_back(x0); vertices.push_back(y0);
        vertices.push_back(data.u0); vertices.push_back(data.v0);

        vertices.push_back(x0); vertices.push_back(y1);
        vertices.push_back(data.u2); vertices.push_back(data.v2);

        vertices.push_back(x1); vertices.push_back(y0);
        vertices.push_back(data.u1); vertices.push_back(data.v1);

        // Triangle 2 (bottom-left, bottom-right, top-right)
        vertices.push_back(x0); vertices.push_back(y1);
        vertices.push_back(data.u2); vertices.push_back(data.v2);

        vertices.push_back(x1); vertices.push_back(y1);
        vertices.push_back(data.u3); vertices.push_back(data.v3);

        vertices.push_back(x1); vertices.push_back(y0);
        vertices.push_back(data.u1); vertices.push_back(data.v1);
    }

    count = vertices.size() / 4; // 4 floats per vertex; count is number of vertices

    logger->debug("Number of vertices: {}", count);

    glGenVertexArrays(1, &vao);
    vbo = std::make_unique<GlBuffer>();

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo->getBuffer());

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4,
                          (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4,
                          (void *)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    vaoExists = true;
}