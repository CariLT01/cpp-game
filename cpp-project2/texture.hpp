#pragma once

#include "spdlog/sinks/stdout_color_sinks.h"
#include <spdlog/spdlog.h>

class Texture {
public:
    Texture();
    ~Texture();

    unsigned int getTexture() { return texture; }

private:
    unsigned int texture;

    std::shared_ptr<spdlog::logger> logger = spdlog::stdout_color_mt(std::format("texture_{}", (void*)this));
};