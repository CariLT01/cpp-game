

#include <Jolt/jolt.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/RegisterTypes.h>
#include <glm/glm.hpp>

#include "Jolt/Core/TempAllocator.h"
#include "Jolt/Physics/Body/BodyInterface.h"
#include "Jolt/Physics/Collision/ObjectLayer.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include <memory>

#include <spdlog/spdlog.h>


using namespace JPH;

namespace Layers {
static constexpr JPH::ObjectLayer NON_MOVING = 0;
static constexpr JPH::ObjectLayer MOVING = 1;
static constexpr JPH::ObjectLayer NUM_LAYERS = 2; // Total count of layers used
} // namespace Layers

namespace BroadPhaseLayers {
static constexpr JPH::BroadPhaseLayer NON_MOVING(0);
static constexpr JPH::BroadPhaseLayer MOVING(1);
static constexpr JPH::uint NUM_LAYERS(2);
} // namespace BroadPhaseLayers

class Collider;
class BroadPhaseLayerInterfaceImpl;
class ObjectVsBroadPhaseLayerFilterImpl;
class ObjectLayerPairFilterImpl;

class PhysicsEngine {
  public:
    PhysicsEngine();
    ~PhysicsEngine();

    void update();
    Collider *createMeshCollider(const std::vector<float> &vertices,
                                 const std::vector<unsigned int> &indices,
                                 const glm::vec3 &position,
                                 const glm::vec3 &rotation,
                                 const glm::vec3 &scale);
    
    void addColider(Collider* collider);
    void removeCollider(Collider* collider);

    PhysicsSystem* getPhysicsSystem() { return physicsSystem.get(); }
    TempAllocatorImpl* getAllocator() { return allocator.get(); }

  private:
    void initialize();

    std::unique_ptr<PhysicsSystem> physicsSystem;
    std::unique_ptr<TempAllocatorImpl> allocator;
    std::unique_ptr<JobSystemThreadPool> jobSystemThreadPool;

    // Layers
    std::unique_ptr<BroadPhaseLayerInterfaceImpl> broadPhaseLayer;
    std::unique_ptr<ObjectLayerPairFilterImpl> objectVsObjectFilter;
    std::unique_ptr<ObjectVsBroadPhaseLayerFilterImpl> objectVsBroadPhaseLayerFilter;

    BodyInterface *bodyInterface;

    std::shared_ptr<spdlog::logger> logger = spdlog::stdout_color_mt("physics_engine");
};