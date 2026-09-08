#include "text_renderer.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "glm/fwd.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "text_mesh.hpp"
#include "asset_loader.hpp"
#include "texture.hpp"
#include "glad/gl.h"
#include "io.hpp"
#include "shader_program.hpp"

TextRenderer::TextRenderer(AssetLoader* assetLoader, int width, int height) : assetLoader(assetLoader), windowWidth(width), windowHeight(height) {
    loadAtlas();
    loadShaders();
}

TextRenderer::~TextRenderer() {
    
}

Text* TextRenderer::createText(const std::string& text, int x, int y, float fontSize) {
    auto newTextObj = new Text();
    newTextObj->textMesh = std::make_unique<TextMesh>(logger.get(), text, glyphs, fontSize);
    newTextObj->x = x;
    newTextObj->y = y;
    newTextObj->fontSize = fontSize;

    /*
    {
        .textMesh = std::make_unique<TextMesh>(text, glyphs),
        .x = x,
        .y = y
    }
    */

    textObjects.push_back(std::unique_ptr<Text>(newTextObj));

    return newTextObj;

}

void TextRenderer::loadShaders() {
    textShader = std::make_unique<ShaderProgram>();
    textShader->addShader(GL_VERTEX_SHADER, readFile("shaders/text_rendering_vertex.glsl"));
    textShader->addShader(GL_FRAGMENT_SHADER, readFile("shaders/text_rendering_fragment.glsl"));
    textShader->link();
}

void TextRenderer::render() {

    glDisable(GL_DEPTH_TEST);

    textShader->use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, atlasTexture->getTexture());

    int loc = textShader->getUniformLocation("uTextAtlas");

    glUniform1i(loc, 0);

    for (auto& t : textObjects) {
        renderText(t.get());
    }

    glEnable(GL_DEPTH_TEST);

}

void TextRenderer::renderText(Text* text) {
    glm::mat4 projection = glm::ortho(
        0.0f,          // left
        (float)windowWidth,   // right
        (float)windowHeight,  // bottom
        0.0f,          // top
        -1.0f,         // near
        1.0f            // far
    );

    glm::mat4 model = glm::translate(
        glm::mat4(1.0f),
        glm::vec3(text->x, text->y, 0.0f)
    );

    glm::mat4 mvp = projection * glm::mat4(1.0f) * model;

    int projectionLoc = textShader->getUniformLocation("uProjection");

    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(mvp));

    text->textMesh->render();
}

void TextRenderer::loadAtlas() {
    atlasTexture = assetLoader->loadTextureDefault("assets/bitmap.png", true);

    const int cols = width;                  // number of glyphs per row (18)
    const int rows = atlasHeight / glyphHeight; // 64/9 ≈ 7

    for (int i = 0; i < 128; i++) {
        int row = i / cols;
        int col = i % cols;

        // Pixel coordinates in the atlas (top-left of glyph)
        int pixelX = col * glyphWidth;
        int pixelY = row * glyphHeight;

        // UV coordinates (v flipped because stbi_set_flip_vertically_on_load(true))
        float u0 = (float)pixelX / atlasWidth;
        float u1 = (float)(pixelX + glyphWidth) / atlasWidth;
        float v0 = (float)(atlasHeight - pixelY) / atlasHeight;          // top
        float v1 = (float)(atlasHeight - (pixelY + glyphHeight)) / atlasHeight; // bottom

        GlyphData data = {
            .u0 = u0, .v0 = v0,   // top-left
            .u1 = u1, .v1 = v0,   // top-right
            .u2 = u0, .v2 = v1,   // bottom-left
            .u3 = u1, .v3 = v1    // bottom-right
        };

        glyphs[static_cast<char>(i)] = data;
    }
}