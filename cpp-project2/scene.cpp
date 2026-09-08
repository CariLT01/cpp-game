#include "scene.hpp"
#include "camera.hpp"
#include "world_object.hpp"
#include "material.hpp"

Scene::Scene() {}

Scene::~Scene() {}

void Scene::addObject(WorldObject *worldObject) {

    logger->debug("Add object called");

    if (worldObject == nullptr) {
        throw std::runtime_error("Attempt to add world object that is nullptr");
    }

    logger->debug("Add object to vec");

    worldObjects.push_back(std::unique_ptr<WorldObject>(worldObject));

    logger->debug("Object added to vec");

    Material *materialPtr = worldObject->getMaterial();

    if (materialPtr == nullptr || worldObject == nullptr) {
        throw std::runtime_error(std::format("Material is nullptr or world object is nullptr: {} {}", (void*)materialPtr, (void*)worldObject));
    }

    if (materialRenderMap.find(materialPtr) == materialRenderMap.end()) {
        materialRenderMap[materialPtr] = {worldObject};
    } else {
        materialRenderMap[materialPtr].push_back(worldObject);
    }

    logger->debug(std::format("Map size: {}", materialRenderMap.size()));
}

void Scene::render(Camera *camera) {

    for (const auto& objects : materialRenderMap) {
        
    }

    for (const auto &[material, worldObjects] : materialRenderMap) {
        material->use();
        camera->apply(material->getShaderProgram());
        for (const auto& obj : worldObjects) {
            obj->render(false);
        }
    }
}