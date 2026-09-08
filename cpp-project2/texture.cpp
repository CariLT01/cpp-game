#include "texture.hpp"
#include <glad/gl.h>

Texture::Texture() {
    glGenTextures(1, &texture);
}

Texture::~Texture() {
    glDeleteTextures(1, &texture);

    logger->debug("Texture deleted");
}