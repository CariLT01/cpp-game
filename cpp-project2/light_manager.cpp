#include "light_manager.hpp"
#include "framebuffer.hpp"
#include "glad/gl.h"
#include "io.hpp"
#include "shader_program.hpp"

#include "gl_buffer.hpp"
#include "framebuffer.hpp"
#include "shader_program.hpp"
#include "camera.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

constexpr float vertices[] = {
    // Front
    -1,
    -1,
    1,
    1,
    -1,
    1,
    1,
    1,
    1,
    -1,
    -1,
    1,
    1,
    1,
    1,
    -1,
    1,
    1,

    // Back
    1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    1,
    -1,
    1,
    -1,
    -1,
    -1,
    1,
    -1,
    1,
    1,
    -1,

    // Left
    -1,
    -1,
    -1,
    -1,
    -1,
    1,
    -1,
    1,
    1,
    -1,
    -1,
    -1,
    -1,
    1,
    1,
    -1,
    1,
    -1,

    // Right
    1,
    -1,
    1,
    1,
    -1,
    -1,
    1,
    1,
    -1,
    1,
    -1,
    1,
    1,
    1,
    -1,
    1,
    1,
    1,

    // Top
    -1,
    1,
    1,
    1,
    1,
    1,
    1,
    1,
    -1,
    -1,
    1,
    1,
    1,
    1,
    -1,
    -1,
    1,
    -1,

    // Bottom
    -1,
    -1,
    -1,
    1,
    -1,
    -1,
    1,
    -1,
    1,
    -1,
    -1,
    -1,
    1,
    -1,
    1,
    -1,
    -1,
    1,
};

LightManager::LightManager(Framebuffer *gbuffer, Camera *camera, int width,
                           int height)
    : gbuffer(gbuffer), camera(camera), width(width), height(height) {

    init();
}

LightManager::~LightManager() {
    // TODO: proper cleanup
    glDeleteVertexArrays(1, &vao);
    glDeleteFramebuffers(1, &fbo);
}

void LightManager::init() {
    glGenVertexArrays(1, &vao);
    // glGenBuffers(1, &vbo);
    vbo = std::make_unique<GlBuffer>();

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo->getBuffer());
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glBindVertexArray(0);

    program = std::make_unique<ShaderProgram>();
    program->addShader(GL_VERTEX_SHADER,
                       readFile("shaders/lighting_vertex.glsl"));
    program->addShader(GL_FRAGMENT_SHADER,
                       readFile("shaders/lighting_fragment.glsl"));
    program->link();

    // Make framebuffer

    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // Make light texture
    // glGenTextures(1, &lightTexture);
    lightTexture = std::make_unique<Texture>();
    glBindTexture(GL_TEXTURE_2D, lightTexture->getTexture());

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB,
                 GL_FLOAT, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // Bind light texture to FBO

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           lightTexture->getTexture(), 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                              GL_RENDERBUFFER, gbuffer->getDepth());

    GLenum drawBuffers[] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, drawBuffers);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        logger->error(std::format("Incomplete framebuffer: {}",
                                  glCheckFramebufferStatus(GL_FRAMEBUFFER)));
        throw std::runtime_error("Incomplete framebuffer");
    }
}

void LightManager::runLightingPass() {

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    // We render the volume's BACK faces (cull front) so lighting still works
    // when the camera is inside the volume. Depth comes from the shared
    // gbuffer: accept the fragment only where scene geometry is in FRONT of
    // the volume's far side, i.e. fragment depth > scene depth (GL_GREATER).
    // GL_LEQUAL is inverted for back faces and rejects every fragment the
    // light should touch -> "black" light. Never write depth here so that
    // overlapping volumes can't occlude each other (blend stays additive).
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_GREATER);
    glDepthMask(GL_FALSE);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    glBindVertexArray(vao);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gbuffer->getPosition());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gbuffer->getNormal());
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, gbuffer->getAlbedo());

    program->use();
    camera->apply(program.get());

    int gPositionLoc = program->getUniformLocation("gPosition");
    int gNormalLoc = program->getUniformLocation("gNormal");
    int gAlbedoLoc = program->getUniformLocation("gAlbedo");

    glUniform1i(gPositionLoc, 0);
    glUniform1i(gNormalLoc, 1);
    glUniform1i(gAlbedoLoc, 2);

    for (auto &light : lights) {

        glm::mat4 modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, light.position);
        modelMatrix = glm::scale(modelMatrix, glm::vec3(light.size));

        // Apply uniform
        int modelMatrixUniform = program->getUniformLocation("uModelMatrix");
        glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE,
                           glm::value_ptr(modelMatrix));

        int lightPositionUniform = program->getUniformLocation("uLightPosition");
        int lightRadiusUniform = program->getUniformLocation("uLightRadius");
        int lightColorUniform = program->getUniformLocation("uLightColor");
        int lightIntensityUniform = program->getUniformLocation("uLightIntensity");

        // Apply light uniform
        glUniform3fv(lightPositionUniform, 1, glm::value_ptr(light.position));
        glUniform1f(lightRadiusUniform, light.size);
        glUniform3fv(lightColorUniform, 1, glm::value_ptr(light.color));
        glUniform1f(lightIntensityUniform, light.intensity);

        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Restore state for the gbuffer and final passes
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
    glCullFace(GL_BACK);
}

void LightManager::addLight(const Light& light) {
    lights.push_back(light);
    logger->debug(std::format("Add light: {} {} {} {}", light.intensity, light.position.x, light.position.y, light.position.z));
    logger->debug(std::format("Light data: {} {} {} {}", light.color.r, light.color.g, light.color.b, light.size));
}