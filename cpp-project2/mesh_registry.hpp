#include <unordered_map>
#include "spdlog/sinks/stdout_color_sinks.h"
#include <string>
#include <memory>
#include <spdlog/spdlog.h>

class Mesh;

class MeshRegistry {
public:
	MeshRegistry();
	~MeshRegistry();

	void addMesh(Mesh* mesh, const std::string& name);
	Mesh* getMesh(const std::string& name);

private:
	std::unordered_map<std::string, std::unique_ptr<Mesh>> meshes;

    std::shared_ptr<spdlog::logger> logger = spdlog::stdout_color_mt("mesh_registry");
};