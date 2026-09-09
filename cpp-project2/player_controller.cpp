#include "player_controller.hpp"
#include "GLFW/glfw3.h"
#include "window.hpp"
#include "camera.hpp"
#include "virtual_character.hpp"

const double sensitivity = 0.1;
const float moveSpeed = 0.02f;

PlayerController::PlayerController(Camera *camera, Window *window, PhysicsEngine* physicsEngine)
    : camera(camera), window(window) {
        virtualCharacter = std::make_unique<VirtualCharacter>(physicsEngine, glm::vec3(0.0f));

    }

PlayerController::~PlayerController() {}

void PlayerController::updateLook() {
    Coord mouseCoord = window->getMousePosition();

    double xOffset = mouseCoord.x - lastX;
    double yOffset = lastY - mouseCoord.y;

    lastX = mouseCoord.x;
    lastY = mouseCoord.y;

    xOffset *= sensitivity;
    yOffset *= sensitivity;

    camera->setYaw(camera->getYaw() + xOffset);
    camera->setPitch(camera->getPitch() + yOffset);
}

void PlayerController::updateMovement() {
    float front = 0.0f;
    float right = 0.0f;

    if (window->checkKeyPressed(GLFW_KEY_W)) {
        front += 1.0f;
    }
    if (window->checkKeyPressed(GLFW_KEY_S)) {
        front -= 1.0f;
    }
    if (window->checkKeyPressed(GLFW_KEY_A)) {
        right -= 1.0f;
    }
    if (window->checkKeyPressed(GLFW_KEY_D)) {
        right += 1.0f;
    }

    glm::vec3 moveVector = glm::vec3(0.0f);

    glm::vec2 moveFront = glm::vec2(cos(glm::radians(camera->getYaw())), sin(glm::radians(camera->getYaw())));
    glm::vec2 moveRight = glm::vec2(cos(glm::radians(camera->getYaw() + 90)), sin(glm::radians(camera->getYaw() + 90)));



    moveVector += front * glm::vec3(moveFront.x, 0, moveFront.y);
    moveVector += right* glm::vec3(moveRight.x, 0, moveRight.y);

    /* logger->debug(
        std::format("{} {} {}", moveVector.x, moveVector.y, moveVector.z)); */

    if (moveVector.length() > 0 && (front != 0.0f || right != 0.0f)) {
        moveVector = glm::normalize(moveVector);

        if (glm::any(glm::isnan(moveVector))) {
            logger->warn("Ignoring NaN move vector");
            return;
        }

        // camera->addPosition(moveVector);
        
    }

    virtualCharacter->setYaw(camera->getYaw());
    virtualCharacter->update(glm::vec2(moveVector.x, moveVector.z), window->checkKeyPressed(GLFW_KEY_SPACE), 1 / 60.0f);
}

void PlayerController::update() {
    updateLook();



    updateMovement();

    camera->setPosition(virtualCharacter->getPosition());
}

void PlayerController::setPosition(const glm::vec3& position) {
    virtualCharacter->setPosition(position);
}