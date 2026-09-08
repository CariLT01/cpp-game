#include "mesh.hpp"
#include "gl_buffer.hpp"
#include <glad/gl.h>
#include <stdexcept>

Mesh::Mesh(const std::vector<float>& vertices, const std::vector<float>& normals, const std::vector<float>& UVs, const std::vector<unsigned int>& indices)
	: vertices(vertices), normals(normals), UVs(UVs), indices(indices) {
}

Mesh::Mesh() {

}

Mesh::~Mesh() {

	if (uploaded) {
		glDeleteVertexArrays(1, &vao);
	}
}

void Mesh::upload() {

	if (uploaded) {
		throw std::runtime_error("Attempted to upload twice!");
	}

	// Generate VAO
	glGenVertexArrays(1, &vao);
// 	glGenBuffers(1, &vbo);
//	glGenBuffers(1, &ebo);
    vbo = std::make_unique<GlBuffer>();
    ebo = std::make_unique<GlBuffer>();

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo->getBuffer());

    std::vector<float> data;

    for (int i = 0; i < vertices.size() / 3; i++) {
        data.push_back(vertices[i * 3 + 0]);
        data.push_back(vertices[i * 3 + 1] );
        data.push_back(vertices[i * 3 + 2] );

        data.push_back(normals[i * 3 + 0] );
        data.push_back(normals[i * 3 + 1] );
        data.push_back(normals[i * 3 + 2] );

        data.push_back(UVs[i * 2 + 0] );
        data.push_back(UVs[i * 2 + 1] );
    }

	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);

    logger->debug(std::format("data size: {}", data.size()));

	// Vertex ATTRs
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	uploaded = true;
}

void Mesh::render() {

	if (!uploaded) {
		throw std::runtime_error("Cannot render mesh before uploading it to the GPU");
	}

	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, indices.data());
	glBindVertexArray(0);
}