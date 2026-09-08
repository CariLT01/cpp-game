#include "collider.hpp"
#include "physics_engine.hpp"

Collider::Collider(PhysicsEngine* physicsEngine, JPH::Body* body) : physicsEngine(physicsEngine), body(body) {

}

Collider::~Collider() {
    physicsEngine->removeCollider(this);
}