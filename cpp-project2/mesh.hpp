#pragma once

#include "spdlog/sinks/stdout_color_sinks.h"
#include <vector>
#include <spdlog/spdlog.h>

class GlBuffer;

class Mesh {
public:
	Mesh(const std::vector<float>& vertices, const std::vector<float>& normals, const std::vector<float>& UVs, const std::vector<unsigned int>& indices);
	Mesh();

	~Mesh();

	void render();
	void upload();

	// Setters
	void setVertices(const std::vector<float>& vertices) { this->vertices = vertices; }
	void setNormals(const std::vector<float>& normals) { this->normals = normals; }
	void setUVs(const std::vector<float>& UVs) { this->UVs = UVs; }
	void setIndices(const std::vector<unsigned int>& indices) { this->indices = indices; }

    // Getters
    std::vector<float> getVertices() { return vertices; }
    std::vector<unsigned int> getIndices() { return indices;} 


private:

	bool uploaded = false;

	

	std::vector<float> vertices;
	std::vector<float> normals;
	std::vector<float> UVs;
	std::vector<unsigned int> indices;

	unsigned int vao = 0;
	// unsigned int ebo = 0;
	// unsigned int vbo = 0;
    std::unique_ptr<GlBuffer> vbo;
    std::unique_ptr<GlBuffer> ebo;


    std::shared_ptr<spdlog::logger> logger = spdlog::stdout_color_mt(std::format("mesh_{}", (void*)this));
};