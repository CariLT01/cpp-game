
#include <Jolt/Jolt.h>
#include "Jolt/Core/Reference.h"
#include "Jolt/Physics/Character/CharacterVirtual.h"
#include <glm/glm.hpp>

class PhysicsEngine;

class VirtualCharacter {
public:
    VirtualCharacter(PhysicsEngine* physicsEngine, const glm::vec3& startingPosition);
    ~VirtualCharacter();

    void update(const glm::vec2& move, bool jump, float dt);

    glm::vec3 getPosition();
    void setPosition(const glm::vec3& pos);
    void setYaw(float yaw);

private:

    void init(const glm::vec3& startingPosition);

    PhysicsEngine* physicsEngine;

    JPH::Ref<JPH::CharacterVirtual> character;
};