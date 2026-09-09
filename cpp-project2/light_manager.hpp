#pragma once

// #include "framebuffer.hpp"
// #include "camera.hpp"
// #include "shader_program.hpp"
#include "texture.hpp"
// #include "gl_buffer.hpp"
#include "spdlog/sinks/stdout_color_sinks.h"
#include <glm/glm.hpp>
#include <spdlog/spdlog.h>

class Framebuffer;
class Camera;
class ShaderProgram;
class GlBuffer;

struct Light {
    glm::vec3 position;
    glm::vec3 color;
    float intensity;
    float size;
};

class LightManager {
public:

    LightManager(Framebuffer* gbuffer, Camera* camera, int width, int height);
    ~LightManager();

    void runLightingPass();

    unsigned int addLight(const Light& light);
    void removeLight(unsigned int lightId);

    unsigned int getLightTexture() { return lightTexture->getTexture(); }

private:

     void init();

    Framebuffer* gbuffer;
    Camera* camera;
    std::unique_ptr<ShaderProgram> program;

    unsigned int fbo;
    unsigned int lightCounter;
    // unsigned int lightTexture;
    std::unique_ptr<Texture> lightTexture;

    // unsigned int vbo;
    std::unique_ptr<GlBuffer> vbo;
    unsigned int vao;

    int width;
    int height;

    std::unordered_map<unsigned int, Light> lights;

    std::shared_ptr<spdlog::logger> logger = spdlog::stdout_color_mt("light_manager");
};