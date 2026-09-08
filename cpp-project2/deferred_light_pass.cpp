#include "deferred_light_pass.hpp"
#include "glad/gl.h"
#include "shader_program.hpp"
#include "io.hpp"
#include "framebuffer.hpp"

DeferredLightPass::DeferredLightPass(Framebuffer* framebuffer) : framebuffer(framebuffer) {
    init();
}

DeferredLightPass::~DeferredLightPass() {
    // TODO: CLEANUP RESOURCES
}

void DeferredLightPass::init() {
    glGenVertexArrays(1, &vao);

    deferredProgram = std::make_unique<ShaderProgram>();

    deferredProgram->addShader(GL_VERTEX_SHADER, readFile("shaders/deferred_pass_vertex_shader.glsl"));
    deferredProgram->addShader(GL_FRAGMENT_SHADER, readFile("shaders/deferred_pass_fragment_shader.glsl"));
    deferredProgram->link();

    logger->debug(std::format("PROGRAM LOCATION: {}", deferredProgram->getProgram()));
}

void DeferredLightPass::execute(unsigned int lightTexture) {

    

    deferredProgram->use();

    int gPosLoc = deferredProgram->getUniformLocation("gPosition");
    int gNormalLoc = deferredProgram->getUniformLocation("gNormal");
    int gAlbedoLoc = deferredProgram->getUniformLocation("gAlbedo");
    int uLightTexture = deferredProgram->getUniformLocation("uLightTexture");

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, framebuffer->getPosition());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, framebuffer->getNormal());
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, framebuffer->getAlbedo());
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, lightTexture);

    glUniform1i(gPosLoc, 0);
    glUniform1i(gNormalLoc, 1);
    glUniform1i(gAlbedoLoc, 2);
    glUniform1i(uLightTexture, 3);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
}