#pragma once

#include "spdlog/logger.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include <spdlog/spdlog.h>
#include <string>

class Mesh;
class Texture;

class AssetLoader {
public:
	AssetLoader();
	~AssetLoader();

	void loadMesh(Mesh* mesh, const std::string& filename);
    Texture* loadTextureDefault(const std::string& filename, bool nearest = false);

private:
    std::shared_ptr<spdlog::logger> logger = spdlog::stderr_color_mt("asset_loader");
};