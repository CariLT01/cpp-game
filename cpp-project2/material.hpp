#pragma once

#include <string>
#include <memory>
#include <glm/glm.hpp>

class ShaderProgram;
class Texture;
class AssetLoader;

class Material {
public:
	Material(AssetLoader* assetLoader, const std::string& name, const std::string& albedoSpec, const std::string& normal);
	~Material();

	void use();

	ShaderProgram* getShaderProgram() {
		return shaderProgram.get();
	}

private:

	std::string materialName;
	std::unique_ptr<ShaderProgram> shaderProgram;
    
    std::unique_ptr<Texture> albedoSpecTexture;
    std::unique_ptr<Texture> normalTexture;
    

	void loadMaterial();

};