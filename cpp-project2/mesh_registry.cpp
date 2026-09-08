#include "mesh_registry.hpp"
#include "mesh.hpp"

MeshRegistry::MeshRegistry() {

}

MeshRegistry::~MeshRegistry() {


}

void MeshRegistry::addMesh(Mesh* mesh, const std::string& name) {

    logger->debug("add mesh called");

    if (mesh == nullptr) {
        throw std::runtime_error("Attempt to add to mesh registry when mesh is nullptr");
    }

	meshes[name] = std::unique_ptr<Mesh>(mesh);
}

Mesh* MeshRegistry::getMesh(const std::string& name) {
	auto it = meshes.find(name);
	if (it != meshes.end()) {
		return it->second.get();
	}
	return nullptr;
}