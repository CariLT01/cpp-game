#include "framebuffer.hpp"
#include "glad/gl.h"
#include <stdexcept>
#include "texture.hpp"

Framebuffer::Framebuffer(const int width, const int height)
    : width(width), height(height) {
        initialize();
    }

Framebuffer::~Framebuffer() {
    // TODO: CLEANUP RESOURCES
    glDeleteRenderbuffers(1, &gDepth);
    
}

void Framebuffer::createGBuffers() {

    // position gbuffer

    // glGenTextures(1, &gPosition);
    gPosition = std::make_unique<Texture>();
    glBindTexture(GL_TEXTURE_2D, gPosition->getTexture());

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA,
                 GL_FLOAT, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           gPosition->getTexture(), 0);

    // gbuffer normals
    // glGenTextures(1, &gNormal);
    gNormal = std::make_unique<Texture>();
    glBindTexture(GL_TEXTURE_2D, gNormal->getTexture());

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA,
                 GL_FLOAT, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D,
                           gNormal->getTexture(), 0);

    // gbuffer albedo
    // glGenTextures(1, &gAlbedo);
    gAlbedo = std::make_unique<Texture>();
    glBindTexture(GL_TEXTURE_2D, gAlbedo->getTexture());

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D,
                           gAlbedo->getTexture(), 0);

}

void Framebuffer::initialize() {
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    createGBuffers();

    // create color attachments

    GLuint attachments[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1,
                            GL_COLOR_ATTACHMENT2};

    glDrawBuffers(3, attachments);

    // create depth buffer

    glGenRenderbuffers(1, &gDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, gDepth);

    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                              GL_RENDERBUFFER, gDepth);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        throw std::runtime_error("Incomplete deferred rendering pipeline framebuffer");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    logger->debug(std::format("Locations: FBO: {}, gAlbedo: {}, gPos: {}, gNormal: {}", fbo, gAlbedo->getTexture(), gPosition->getTexture(), gNormal->getTexture()));
}

void Framebuffer::bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}

void Framebuffer::unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}