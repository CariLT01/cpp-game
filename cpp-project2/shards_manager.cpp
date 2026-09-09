#include "shards_manager.hpp"
#include "asset_loader.hpp"
#include "glm/geometric.hpp"
#include "light_manager.hpp"
#include "material.hpp"
#include "mesh.hpp"
#include "player_controller.hpp"
#include "scene.hpp"
#include "transform.hpp"
#include "world_object.hpp"
#include "player_stats.hpp"
#include <cmath>
#include <iostream>

ShardsManager::ShardsManager(Scene *scene, AssetLoader *assetLoader,
                             LightManager *lightManager,
                             PlayerController *playerController, PlayerStats* playerStats)
    : scene(scene), assetLoader(assetLoader), lightManager(lightManager),
      playerController(playerController), playerStats(playerStats) {
    initialize();
}

ShardsManager::~ShardsManager() {}

void ShardsManager::initialize() {
    shardMaterial = std::make_unique<Material>(
        assetLoader, "basic", "assets/shard.png", "assets/normal_none.png");
    shardMesh = std::make_unique<Mesh>();
    assetLoader->loadMesh(shardMesh.get(), "assets/shard.obj");
    shardMesh->upload();
}

unsigned int ShardsManager::addShard(const glm::vec3 &position) {
    Shard *newShard = new Shard();
    newShard->position = position;

    unsigned int lightId = lightManager->addLight(
        {.position = position - glm::vec3(0.0f, 0.5f, 0.0f),
         .color = glm::vec3(1.0, 0.0f, 1.0f),
         .intensity = 5,
         .size = 5});

    unsigned int lightIdTop = lightManager->addLight(
        {.position = position + glm::vec3(0.0f, 0.5f, 0.0f),
         .color = glm::vec3(0.0f, 0.0f, 1.0f),
         .intensity = 5,
         .size = 5});

    WorldObject *shardObject =
        new WorldObject(ObjectTransform{.position = position,
                                        .orientation = glm::vec3(0, 0, 0),
                                        .scale = glm::vec3(0.5f)},
                        shardMaterial.get(), shardMesh.get());

    newShard->shardObject = shardObject;
    newShard->lightId = lightId;
    newShard->lightIdTop = lightIdTop;

    shards[shardCounter + 1] = std::unique_ptr<Shard>(newShard);
    scene->addObject(shardObject);

    shardCounter++;
    return shardCounter;
}

void ShardsManager::removeShard(unsigned int id) {
    auto &shard = shards[id];

    scene->removeObject(shard->shardObject);
    lightManager->removeLight(shard->lightId);
    lightManager->removeLight(shard->lightIdTop);

    shards.erase(id);

    playerStats->incrementShardCount();
}

void ShardsManager::update(float dt) {

    std::vector<unsigned int> indexesToRemove;
    glm::vec3 playerPosition = playerController->getPosition();
    // logger->debug("player pos: {} {} {}", playerPosition.x, playerPosition.y,
    // playerPosition.z);
    for (const auto &[id, shard] : shards) {
        glm::vec3 diff = playerPosition - shard->position;
        if (glm::length(diff) < 2) {
            logger->debug("KKKK found shard less than dist");
            indexesToRemove.push_back(id);
        } else {
            /* logger->debug("Not found: {} {} {} {} {} {} {}", diff.length(),
                          shard->position.x, shard->position.y,
                          shard->position.z, playerPosition.x, playerPosition.y,
                          playerPosition.z); */
        }
    }

    for (unsigned int i : indexesToRemove) {
        removeShard(i);
    }

    for (const auto &[_, shard] : shards) {
        shard->shardObject->setPosition(
            shard->position +
            glm::vec3(0.0f, sin(frame * 2.f * dt) * 0.4f, 0.0f));
    }

    frame++;
}