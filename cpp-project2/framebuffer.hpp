
#pragma once

#include "spdlog/sinks/stdout_color_sinks.h"
#include "texture.hpp"
#include <memory>
#include <spdlog/spdlog.h>

class Framebuffer {
public:
    Framebuffer(const int width, const int height);
    ~Framebuffer();

    unsigned int getAlbedo() { return gAlbedo->getTexture(); }
    unsigned int getNormal() { return gNormal->getTexture(); }
    unsigned int getPosition() { return gPosition->getTexture(); }
    unsigned int getDepth() { return gDepth; }

    void bind();
    void unbind();

private:

    unsigned int fbo;



    void initialize();

    void createGBuffers();

    std::unique_ptr<Texture> gPosition;
    std::unique_ptr<Texture> gNormal;
    std::unique_ptr<Texture> gAlbedo;

    /* unsigned int gPosition;
    unsigned int gNormal;
    unsigned int gAlbedo; */

    unsigned int gDepth; 

    int width;
    int height;

    std::shared_ptr<spdlog::logger> logger = spdlog::stdout_color_mt("framebuffer");
};