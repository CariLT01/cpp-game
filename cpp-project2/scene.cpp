#include "scene.hpp"
#include "camera.hpp"
#include "material.hpp"
#include "world_object.hpp"
#include <stdexcept>

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
        throw std::runtime_error(
            std::format("Material is nullptr or world object is nullptr: {} {}",
                        (void *)materialPtr, (void *)worldObject));
    }

    if (materialRenderMap.find(materialPtr) == materialRenderMap.end()) {
        materialRenderMap[materialPtr] = {worldObject};
    } else {
        materialRenderMap[materialPtr].push_back(worldObject);
    }

    logger->debug(std::format("Map size: {}", materialRenderMap.size()));
}

bool Scene::worldObjectExists(WorldObject *worldObject) {
    // TODO: more efficient implementation
    for (auto &obj : worldObjects) {
        if (obj.get() == worldObject) {
            return true;
        }
    }
    return false;
}

void Scene::removeObject(WorldObject *worldObject) {
    if (!worldObjectExists(worldObject)) {
        throw std::runtime_error("World object does not exist");
    }

    // Find index
    unsigned int index = 0;
    for (auto &obj : worldObjects) {
        if (obj.get() == worldObject) {
            break;
        }
        index++;
    }

    // Remove
    worldObjects.erase(worldObjects.begin() + index);

    // Search in map
    bool found = false;
    Material *key;
    int indexV = -1;

    for (const auto &[material, vec] : materialRenderMap) {
        if (found)
            break;

        key = material;
        int i = 0;

        for (auto &obj : vec) {
            if (obj == worldObject) {
                found = true;
                indexV = i;
                break;
            } else {
                i++;
            }
        }
    }

    materialRenderMap[key].erase(materialRenderMap[key].begin() + indexV);
}

void Scene::render(Camera *camera) {

    for (const auto &objects : materialRenderMap) {
    }

    for (const auto &[material, worldObjects] : materialRenderMap) {
        material->use();
        camera->apply(material->getShaderProgram());
        for (const auto &obj : worldObjects) {
            obj->render(false);
        }
    }
}