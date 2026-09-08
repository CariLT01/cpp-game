#include "camera.hpp"
#include "shader_program.hpp"
#include <glad/gl.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

const glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

Camera::Camera()
    : cameraFront(glm::vec3(0.0f)), cameraRight(glm::vec3(0.0f)),
      cameraUp(glm::vec3(0.0f)), position(0.0f) {}

Camera::~Camera() {}

void Camera::update() {
    updateLookVectors();
    updateMatrices();

    /* logger->debug(std::format("Position: {} {} {}, Orientation {} {} {}",
                              position.x, position.y, position.z, pitch, yaw,
                              roll)); */
}

void Camera::updateLookVectors() {

    pitch = glm::clamp(pitch, -89.0f, 89.0f);

    cameraFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront.y = sin(glm::radians(pitch));
    cameraFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(cameraFront);

    cameraRight = glm::normalize(glm::cross(cameraFront, worldUp));
    cameraUp = glm::normalize(glm::cross(cameraRight, cameraFront));

    /* logger->debug(std::format("Front: {} {} {}, Right: {} {} {}, Up: {} {} {}",
                              cameraFront.x, cameraFront.y, cameraFront.z,
                              cameraRight.x, cameraRight.y, cameraRight.z,
                              cameraUp.x, cameraUp.y, cameraUp.z)); */
}

void Camera::updateMatrices() {
    viewMatrix = glm::lookAt(position, position + cameraFront, cameraUp);
    projectionMatrix =
        glm::perspective(glm::radians(fov), aspectRatio, near, far);
}

void Camera::apply(ShaderProgram *shaderProgram) {
    unsigned int program = shaderProgram->getProgram();

    int projectionLoc = shaderProgram->getUniformLocation("uProjectionMatrix");
    int viewLoc = shaderProgram->getUniformLocation("uViewMatrix");
    int viewPosLoc = shaderProgram->getUniformLocation("uViewPosition");

    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE,
                       glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
    glUniform3fv(viewPosLoc, 1, glm::value_ptr(position));
}