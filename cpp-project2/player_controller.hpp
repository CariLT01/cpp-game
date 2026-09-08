#pragma once

#include "spdlog/sinks/stdout_color_sinks.h"
#include <glm/glm.hpp>

class Camera;
class Window;
class VirtualCharacter;
class PhysicsEngine;

class PlayerController {
public:
    PlayerController(Camera* camera, Window* window, PhysicsEngine* physicsEngine);
    ~PlayerController();

    void update();
    void setPosition(const glm::vec3& position);

private:

    void updateLook();
    void updateMovement();

    Camera* camera;
    Window* window;

    std::unique_ptr<VirtualCharacter> virtualCharacter;

    double lastX = 0;
    double lastY = 0; 

    std::shared_ptr<spdlog::logger> logger = spdlog::stdout_color_mt("player_controller");

};