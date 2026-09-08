#include "colliders_registry.hpp"
#include "collider.hpp"

CollidersRegistry::CollidersRegistry() {

}

CollidersRegistry::~CollidersRegistry() {

}

void CollidersRegistry::addColider(Collider* collider) {
    colliders.push_back(std::unique_ptr<Collider>(collider));
}