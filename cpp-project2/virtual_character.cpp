#include "virtual_character.hpp"
#include "Jolt/Math/Real.h"
#include "Jolt/Math/Vec3.h"
#include "Jolt/Physics/Character/CharacterVirtual.h"
#include "Jolt/Physics/Collision/Shape/CapsuleShape.h"
#include "Jolt/Physics/Collision/Shape/Shape.h"
#include "Jolt/Physics/PhysicsSystem.h"
#include "physics_engine.hpp"
#include <glm/glm.hpp>

struct MovementSettings {
    float maxSpeed = 12.0f;     // Top speed (m/s)
    float acceleration = 60.0f; // Rate of acceleration (m/s^2)
    float deceleration =
        40.0f; // Rate of braking when no input is given (m/s^2)
    float airControl =
        0.3f; // Reduced control multiplier while airborne (0.0 to 1.0)
};

VirtualCharacter::VirtualCharacter(PhysicsEngine *physicsEngine,
                                   const glm::vec3 &startingPosition)
    : physicsEngine(physicsEngine) {
    init(startingPosition);
}

VirtualCharacter::~VirtualCharacter() { character = nullptr; }

void VirtualCharacter::init(const glm::vec3 &startingPosition) {
    JPH::RefConst<JPH::Shape> characterShape =
        new JPH::CapsuleShape(0.9f, 0.4f);

    JPH::CharacterVirtualSettings settings;
    settings.mShape = characterShape;
    settings.mSupportingVolume = JPH::Plane(
        JPH::Vec3::sAxisY(), -0.9f); // Volume to check ground support
    settings.mMaxSlopeAngle = JPH::DegreesToRadians(45.0f);
    settings.mMaxStrength =
        100.0f; // Max force applied to dynamic objects pushed by character
    settings.mCharacterPadding =
        0.02f; // Prevents precision stuckness against walls
    settings.mPenetrationRecoverySpeed = 1.0f;
    settings.mPredictiveContactDistance = 0.1f;

    character = new JPH::CharacterVirtual(
        &settings,
        JPH::RVec3(startingPosition.x, startingPosition.y, startingPosition.z),
        JPH::Quat::sIdentity(), physicsEngine->getPhysicsSystem());
}

JPH::Vec3 MoveTowards(const JPH::Vec3 &current, const JPH::Vec3 &target,
                      float maxDistanceDelta) {
    JPH::Vec3 delta = target - current;
    float lengthSq = delta.LengthSq();

    if (lengthSq == 0.0f || (maxDistanceDelta >= 0.0f &&
                             lengthSq <= maxDistanceDelta * maxDistanceDelta)) {
        return target;
    }

    float length = std::sqrt(lengthSq);
    return current + (delta / length) * maxDistanceDelta;
}

void VirtualCharacter::update(const glm::vec2 &move, bool jump, float dt) {
    JPH::Vec3 currentVelocity = character->GetLinearVelocity();
    bool isGrounded = (character->GetGroundState() ==
                       JPH::CharacterVirtual::EGroundState::OnGround);

    JPH::Vec3 horizontalVel(currentVelocity.GetX(), 0.0f,
                            currentVelocity.GetZ());
    float verticalVel = currentVelocity.GetY();

    JPH::Vec3 moveDir(move.x, 0.0f, move.y);
    MovementSettings settings;
    JPH::Vec3 targetVelocity = moveDir * settings.maxSpeed;

    bool hasInput = moveDir.LengthSq() > 0.001f;
    float rate = hasInput ? settings.acceleration : settings.deceleration;

    if (!isGrounded) {
        rate *= settings.airControl;
    }

    horizontalVel = MoveTowards(horizontalVel, targetVelocity, rate * dt);

    if (!isGrounded) {
        verticalVel +=
            physicsEngine->getPhysicsSystem()->GetGravity().GetY() * dt;
    } else if (verticalVel < 0.0f) {
        verticalVel = 0.0f;
    }

    if (jump && isGrounded) {
        verticalVel = 8.0f;
    }

    JPH::Vec3 newVelocity(horizontalVel.GetX(), verticalVel, horizontalVel.GetZ());
    character->SetLinearVelocity(newVelocity);

    JPH::CharacterVirtual::ExtendedUpdateSettings update_settings;

    update_settings.mStickToFloorStepDown = JPH::Vec3(0.0f, -0.5f, 0.0f);
    update_settings.mWalkStairsStepUp = JPH::Vec3(0.0f, 0.4f, 0.0f);

    PhysicsSystem *system = physicsEngine->getPhysicsSystem();

    character->ExtendedUpdate(
        dt, system->GetGravity(), update_settings,
        system->GetDefaultBroadPhaseLayerFilter(Layers::MOVING),
        system->GetDefaultLayerFilter(Layers::MOVING), {}, // BodyFilter
        {},                                                // ShapeFilter
        *(physicsEngine->getAllocator()));
}

glm::vec3 VirtualCharacter::getPosition() {
    auto pos = character->GetPosition();
    return glm::vec3(pos.GetX(), pos.GetY(), pos.GetZ());
}

void VirtualCharacter::setPosition(const glm::vec3 &pos) {
    character->SetPosition(JPH::RVec3Arg(pos.x, pos.y, pos.z));
}

void VirtualCharacter::setYaw(float yaw) {
    JPH::Quat targetRotation =
        JPH::Quat::sRotation(JPH::Vec3::sAxisY(), glm::radians(yaw));

    character->SetRotation(targetRotation);
}