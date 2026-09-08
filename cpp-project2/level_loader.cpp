#include "level_loader.hpp"
#include "asset_loader.hpp"
#include "collider.hpp"
#include "io.hpp"
#include "level_data_structs.hpp"
#include "light_manager.hpp"
#include "material.hpp"
#include "mesh.hpp"
#include "physics_engine.hpp"
#include "world_object.hpp"
#include <unordered_map>


LevelLoader::LevelLoader(AssetLoader *assetLoader, PhysicsEngine *physicsEngine)
    : assetLoader(assetLoader), physicsEngine(physicsEngine) {}

LevelLoader::~LevelLoader() {}

std::string LevelLoader::combineMaterialIdentifier(
    const std::vector<std::string> &textures) {
    std::string identifier;
    for (auto &text : textures) {
        identifier += text;
    }
    return identifier;
}

LevelData LevelLoader::loadLevel(const std::string &filename) {
    nlohmann::json json = nlohmann::json::parse(readFile(filename));
    level_data::Level levelData = json;

    // Find lists of materials and create them

    std::unordered_map<std::string, std::vector<unsigned int>> materials;
    std::unordered_map<std::string, std::vector<unsigned int>> meshes;

    unsigned int index = 0;
    for (level_data::Mesh &mesh : levelData.meshes) {
        std::string identifier = combineMaterialIdentifier(
            {mesh.material.albedo, mesh.material.normal, mesh.material.specular,
             mesh.material.name});
        auto mat = materials.find(identifier);

        logger->debug(std::format("mesh albedo: {}", mesh.material.albedo));

        logger->debug(std::format("identifier: {}", identifier));

        if (mat == materials.end()) {
            materials[identifier] = {index};
        } else {
            mat->second.push_back(index);
        }

        auto meshIt = meshes.find(mesh.mesh);
        if (meshIt == meshes.end()) {
            meshes[mesh.mesh] = {index};
        } else {
            meshIt->second.push_back(index);
        }

        index++;
    }

    logger->info(std::format("Found {} unique materials", materials.size()));
    logger->info(std::format("Found {} unique meshes", meshes.size()));

    // Create materials
    std::unordered_map<std::string, Material *> materialObjects;

    for (const auto &[identifier, meshIndices] : materials) {
        level_data::Mesh &meshObj = levelData.meshes[meshIndices[0]];

        logger->debug(std::format("material id: {}, material name: {}",
                                  identifier, meshObj.material.name));

        Material *newMaterial =
            new Material(assetLoader, meshObj.material.name,
                         meshObj.material.albedo, meshObj.material.normal);

        // TODO: assign textures!

        materialObjects[identifier] = (newMaterial);
    }

    // Create meshes
    std::unordered_map<std::string, Mesh *> meshObjectsMap;

    for (const auto &[meshName, indices] : meshes) {
        level_data::Mesh &mesh = levelData.meshes[indices[0]];

        Mesh *meshObject = new Mesh();

        assetLoader->loadMesh(meshObject, mesh.mesh);
        meshObjectsMap[mesh.mesh] = meshObject;
    }

    // Create game objects
    std::vector<WorldObject *> worldObjects;
    std::vector<Collider *> colliders;

    for (const auto &mesh : levelData.meshes) {

        std::string materialIdentifier = combineMaterialIdentifier(
            {mesh.material.albedo, mesh.material.normal, mesh.material.specular,
             mesh.material.name});

        Mesh *meshObject = meshObjectsMap[mesh.mesh];

        WorldObject *wobj = new WorldObject(
            ObjectTransform{
                .position = glm::vec3(mesh.position.x, mesh.position.y,
                                      mesh.position.z),
                .orientation = glm::vec3(mesh.rotation.x, mesh.rotation.y,
                                         mesh.rotation.z),
                .scale = glm::vec3(mesh.scale.x, mesh.scale.y, mesh.scale.z),
            },
            materialObjects[materialIdentifier], meshObjectsMap[mesh.mesh]);

        worldObjects.push_back(wobj);

        Collider *newCollider = physicsEngine->createMeshCollider(
            meshObject->getVertices(), meshObject->getIndices(),
            glm::vec3(mesh.position.x, mesh.position.y, mesh.position.z),
            glm::vec3(mesh.rotation.x, mesh.rotation.y, mesh.rotation.z),
            glm::vec3(mesh.scale.x, mesh.scale.y, mesh.scale.z));

        colliders.push_back(newCollider);
    }

    std::vector<Material *> materialsVec;
    for (const auto &[id, mat] : materialObjects) {
        materialsVec.push_back(mat);
    }

    std::vector<Mesh *> meshesVec;
    for (const auto &[id, mesh] : meshObjectsMap) {
        meshesVec.push_back(mesh);
    }

    std::vector<Light> lights;
    for (const level_data::Light &light : levelData.lights) {
        lights.push_back(Light{
            .position =
                glm::vec3(light.position.x, light.position.y, light.position.z),
            .color = glm::vec3(light.color.x, light.color.y, light.color.z),
            .intensity = light.intensity,
            .size = light.radius,
        });
    }

    return {
        .meshes = meshesVec,
        .materials = materialsVec,
        .worldObjects = worldObjects,
        .lights = lights,
        .colliders = colliders,
        .spawnPosition =
            glm::vec3(levelData.spawn.position.x, levelData.spawn.position.y,
                      levelData.spawn.position.z),
        
    };
}