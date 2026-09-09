#pragma once

#include "spdlog/sinks/stdout_color_sinks.h"
// #include "world_object.hpp"
// #include "asset_loader.hpp"
// #include "light_manager.hpp"
#include <glm/glm.hpp>

class Mesh;
class Material;
class WorldObject;
class Light;
class AssetLoader;
class Collider;
class PhysicsEngine;


struct LevelData {
    std::vector<Mesh*> meshes;
    std::vector<Material*> materials;
    std::vector<WorldObject*> worldObjects;
    std::vector<Light> lights;
    std::vector<Collider*> colliders;
    std::vector<glm::vec3> shards;

    glm::vec3 spawnPosition;
};

class LevelLoader {
public:
    LevelLoader(AssetLoader* assetLoader, PhysicsEngine* physicsEngine);
    ~LevelLoader();

    LevelData loadLevel(const std::string& filename);

private:

    std::string combineMaterialIdentifier(const std::vector<std::string>& textures);

    std::shared_ptr<spdlog::logger> logger = spdlog::stdout_color_mt("level_loader");

    AssetLoader* assetLoader;
    PhysicsEngine* physicsEngine;

};