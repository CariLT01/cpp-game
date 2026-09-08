
#include <Jolt/jolt.h>
#include "Jolt/Physics/Body/Body.h"

class PhysicsEngine;

class Collider {
public:
    Collider(PhysicsEngine* physicsEngine, JPH::Body* body);
    ~Collider();

    JPH::Body* getBody() { return body; }

private:
    JPH::Body* body;
    PhysicsEngine* physicsEngine;
};