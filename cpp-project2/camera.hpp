#pragma once

#include <glm/glm.hpp>
#include "spdlog/sinks/stdout_color_sinks.h"
#include <spdlog/spdlog.h>

class ShaderProgram;

class Camera {
public:
	Camera();
	~Camera();

	void update();
	void apply(ShaderProgram* shaderProgram);

    // Getters
    float getPitch() { return pitch; }
    float getYaw() { return yaw; }
    float getRoll() { return roll; }

    glm::vec3 getFront() { return cameraFront; }
    glm::vec3 getRight() { return cameraRight; }

    // Setters
    void setPitch(float v) { 
        pitch = glm::clamp(v, -89.0f, 89.0f);; 
    }
    void setYaw(float v) { yaw = v; }
    void setRoll(float v) { roll = v; }

    void setPosition(const glm::vec3& pos) { position = pos; }
    void addPosition(const glm::vec3& pos) { position += pos; }

private:

	void updateMatrices();
	void updateLookVectors();

	glm::vec3 position;
	
	float pitch = 0.0f;
	float yaw = 0.0f;
	float roll = 0.0f;
	float fov = 70.0f;
	float aspectRatio = 800.0f / 600.0f;
    
	float near = 0.1f;
	float far = 1000.0f;
	
	glm::vec3 cameraFront;
	glm::vec3 cameraUp;
	glm::vec3 cameraRight;

	glm::mat4 projectionMatrix;
	glm::mat4 viewMatrix;

    std::shared_ptr<spdlog::logger> logger = spdlog::stdout_color_mt("camera");
};