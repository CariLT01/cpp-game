#pragma once

#include "spdlog/sinks/stdout_color_sinks.h"
#include <spdlog/spdlog.h>

class GlBuffer {
public:
    GlBuffer();
    ~GlBuffer();

    unsigned int getBuffer() { return buffer; }

private:
    unsigned int buffer;

    std::shared_ptr<spdlog::logger> logger = spdlog::stdout_color_mt(std::format("gl_buffer_{}", (void*)this));
};