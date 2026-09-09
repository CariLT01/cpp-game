#include "shards_manager.hpp"
#include "asset_loader.hpp"
#include "light_manager.hpp"
#include "material.hpp"
#include "mesh.hpp"
#include "transform.hpp"
#include "world_object.hpp"
#include "scene.hpp"
#include <cmath>

ShardsManager::ShardsManager(Scene* scene, AssetLoader* assetLoader, LightManager* lightManager) : scene(scene), assetLoader(assetLoader), lightManager(lightManager) {
    initialize();
}

ShardsManager::~ShardsManager() {

}

void ShardsManager::initialize() {
    shardMaterial = std::make_unique<Material>(assetLoader, "basic", "assets/shard.png", "assets/normal_none.png");
    shardMesh = std::make_unique<Mesh>();
    assetLoader->loadMesh(shardMesh.get(), "assets/shard.obj");
    shardMesh->upload();
}

void ShardsManager::addShard(const glm::vec3& position) {
    Shard* newShard = new Shard();
    newShard->position = position;

    unsigned int lightId = lightManager->addLight({
        .position = position - glm::vec3(0.0f, 0.5f, 0.0f),
        .color = glm::vec3(1.0, 0.0f, 1.0f),
        .intensity = 5,
        .size = 5
    });

    unsigned int lightIdTop = lightManager->addLight({
        .position = position + glm::vec3(0.0f, 0.5f, 0.0f),
        .color = glm::vec3(0.0f, 0.0f, 1.0f),
        .intensity = 5,
        .size = 5
    });

    WorldObject* shardObject = new WorldObject(ObjectTransform{
        .position = position,
        .orientation = glm::vec3(0, 0, 0),
        .scale = glm::vec3(0.5f)
    }, shardMaterial.get(), shardMesh.get());

    newShard->shardObject = shardObject;
    newShard->lightId = lightId;
    newShard->lightIdTop = lightIdTop;

    shards.push_back(std::unique_ptr<Shard>(newShard));

    scene->addObject(shardObject);
}

void ShardsManager::update(float dt) {
    for (const auto& shard : shards) {
        shard->shardObject->setPosition(shard->position + glm::vec3(0.0f, sin(frame * 2.f * dt) * 0.4f, 0.0f));
    }

    frame++;
}