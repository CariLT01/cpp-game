#pragma once

#include "spdlog/sinks/stdout_color_sinks.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <spdlog/spdlog.h>

class ShaderProgram {
public:
	ShaderProgram();
	~ShaderProgram();

	void use() const;
	void addShader(const int shaderType, const std::string& source);
	void link();

	unsigned int getProgram() {
		return program;
	}

    int getUniformLocation(const std::string& name);

private:

	unsigned int program;

	std::vector<unsigned int> shaders;
    std::unordered_map<std::string, int> uniformLocationCache;

    std::shared_ptr<spdlog::logger> logger = spdlog::stdout_color_mt(std::format("shader_program_{}", (void*)this));
};