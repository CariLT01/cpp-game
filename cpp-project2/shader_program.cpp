#include "shader_program.hpp"
#include <glad/gl.h>
#include <stdexcept>

ShaderProgram::ShaderProgram() : program(0) {

}

ShaderProgram::~ShaderProgram() {
	if (program != 0) {
		glDeleteProgram(program);
	}
}

void ShaderProgram::addShader(const int shaderType, const std::string& source) {
	unsigned int shader = glCreateShader(shaderType);

	const char* sourceCStr = source.c_str();

	glShaderSource(shader, 1, &sourceCStr, NULL);
	glCompileShader(shader);

	int success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		throw std::runtime_error("Shader compilation failed: " + std::string(infoLog));
	}

	shaders.push_back(shader);
}

void ShaderProgram::link() {
	program = glCreateProgram();
	for (unsigned int shader : shaders) {
		glAttachShader(program, shader);
	}

	glLinkProgram(program);

	int success;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetProgramInfoLog(program, 512, NULL, infoLog);
		throw std::runtime_error("Shader program linking failed: " + std::string(infoLog));
	}

	for (unsigned int shader : shaders) {
		glDeleteShader(shader);
	}

    logger->info("Shader program linked successfully");
}

void ShaderProgram::use() const {
	glUseProgram(program);
}

int ShaderProgram::getUniformLocation(const std::string& name) {
    auto loc = uniformLocationCache.find(name);
    if (loc == uniformLocationCache.end()) {
        int uniformLocation =  glGetUniformLocation(program, name.c_str());
        if (uniformLocation == -1) {
            logger->warn(std::format("Uniform '{}' does not exist in shader program {}", name, program));
        }
        logger->debug(std::format("Uniform {} cached location {}", name, uniformLocation));
        uniformLocationCache[name] = uniformLocation;
        return uniformLocation;
    } else {
        return loc->second;
    }
}