#include "material_registry.hpp"

MaterialRegistry::MaterialRegistry() {

}

MaterialRegistry::~MaterialRegistry() {

}

void MaterialRegistry::addMaterial(const std::string& name, Material* material) {
	materials[name] = std::unique_ptr<Material>(material);
}

Material* MaterialRegistry::getMaterial(const std::string& name) {
	auto it = materials.find(name);
	if (it != materials.end()) {
		return it->second.get();
	}
	return nullptr;
}