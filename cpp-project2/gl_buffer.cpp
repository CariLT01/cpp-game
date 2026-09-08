#include "gl_buffer.hpp"
#include "glad/gl.h"

GlBuffer::GlBuffer() {
    glGenBuffers(1, &buffer);
}

GlBuffer::~GlBuffer() {
    glDeleteBuffers(1, &buffer);
    
    logger->debug("GL buffer deleted");
}

