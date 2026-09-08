#include "material.hpp"
#include "io.hpp"
#include <glad/gl.h>
#include "shader_program.hpp"
#include "asset_loader.hpp"
#include "texture.hpp"

Material::Material(AssetLoader* assetLoader, const std::string& name, const std::string& albedoSpec, const std::string& normal) {
	// Vertex shader
	const std::string& vertexShaderSource = readFile("shaders/" + name + "_gbuffer_vertex.glsl");
	const std::string& fragmentShaderSource = readFile("shaders/" + name + "_gbuffer_fragment.glsl");

	shaderProgram = std::make_unique<ShaderProgram>();
	shaderProgram->addShader(GL_VERTEX_SHADER, vertexShaderSource);
	shaderProgram->addShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
	shaderProgram->link();

    albedoSpecTexture = std::unique_ptr<Texture>(assetLoader->loadTextureDefault(albedoSpec));
    normalTexture = std::unique_ptr<Texture>(assetLoader->loadTextureDefault(normal));
}

Material::~Material() {
    
}

void Material::use() {
	shaderProgram->use();

    // Bind textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, albedoSpecTexture->getTexture());

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normalTexture->getTexture());

    int albedoSpecTextureLoc = shaderProgram->getUniformLocation("uAlbedoSpecTexture");
    int normalTextureLoc = shaderProgram->getUniformLocation("uNormalTexture");

    glUniform1i(albedoSpecTextureLoc, 0);
    glUniform1i(normalTextureLoc, 1);
}

