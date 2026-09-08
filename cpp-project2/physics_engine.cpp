#include "physics_engine.hpp"
#include "Jolt/Core/Factory.h"
#include "Jolt/Core/JobSystemThreadPool.h"
#include "Jolt/Core/Memory.h"
#include "Jolt/Core/TempAllocator.h"
#include "Jolt/Geometry/IndexedTriangle.h"
#include "Jolt/Math/Float3.h"
#include "Jolt/Math/MathTypes.h"
#include "Jolt/Math/Vec3.h"
#include "Jolt/Physics/Body/Body.h"
#include "Jolt/Physics/Body/BodyCreationSettings.h"
#include "Jolt/Physics/Body/MotionType.h"
#include "Jolt/Physics/Collision/Shape/MeshShape.h"
#include "Jolt/Physics/Collision/Shape/ScaledShape.h"
#include "Jolt/Physics/Collision/Shape/Shape.h"
#include "Jolt/Physics/EActivation.h"
#include "Jolt/Physics/PhysicsSettings.h"
#include "collider.hpp"

using namespace JPH;

class BroadPhaseLayerInterfaceImpl final
    : public JPH::BroadPhaseLayerInterface {
  public:
    BroadPhaseLayerInterfaceImpl() {
        // Map ObjectLayers to BroadPhaseLayers
        mObjectToBroadPhase[Layers::NON_MOVING] = BroadPhaseLayers::NON_MOVING;
        mObjectToBroadPhase[Layers::MOVING] = BroadPhaseLayers::MOVING;
    }

    virtual JPH::uint GetNumBroadPhaseLayers() const override {
        return BroadPhaseLayers::NUM_LAYERS;
    }

    virtual JPH::BroadPhaseLayer
    GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override {
        return mObjectToBroadPhase[inLayer];
    }

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
    // 3. Name of the layer (required for profiling builds)
    virtual const char *
    GetBroadPhaseLayerName(JPH::BroadPhaseLayer inLayer) const override {
        switch ((JPH::BroadPhaseLayer::Type)inLayer) {
        case (JPH::BroadPhaseLayer::Type)BroadPhaseLayers::NON_MOVING:
            return "NON_MOVING";
        case (JPH::BroadPhaseLayer::Type)BroadPhaseLayers::MOVING:
            return "MOVING";
        default:
            JPH_ASSERT(false);
            return "INVALID";
        }
    }
#endif

  private:
    JPH::BroadPhaseLayer mObjectToBroadPhase[Layers::NUM_LAYERS];
};

// 3. Concrete ObjectVsBroadPhaseLayerFilter Implementation
class ObjectVsBroadPhaseLayerFilterImpl
    : public JPH::ObjectVsBroadPhaseLayerFilter {
  public:
    virtual bool ShouldCollide(JPH::ObjectLayer inLayer1,
                               JPH::BroadPhaseLayer inLayer2) const override {
        switch (inLayer1) {
        case Layers::NON_MOVING:
            return inLayer2 ==
                   BroadPhaseLayers::MOVING; // Static objects only collide with
                                             // moving ones
        case Layers::MOVING:
            return true; // Moving objects collide with both static and moving
                         // layers
        default:
            return false;
        }
    }
};

class ObjectLayerPairFilterImpl : public ObjectLayerPairFilter {
  public:
    virtual bool ShouldCollide(ObjectLayer inObject1,
                               ObjectLayer inObject2) const override {
        switch (inObject1) {
        case Layers::NON_MOVING:
            return inObject2 == Layers::MOVING;
        case Layers::MOVING:
            return true;
        default:
            return false;
        }
    }
};

PhysicsEngine::PhysicsEngine() { initialize(); }

PhysicsEngine::~PhysicsEngine() {
    delete Factory::sInstance;
    Factory::sInstance = nullptr;
}

void PhysicsEngine::initialize() {

    logger->info("Initializing physics engine");

    RegisterDefaultAllocator();

    if (JPH::Factory::sInstance == nullptr) {
        JPH::Factory::sInstance = new JPH::Factory();
    }

    RegisterTypes();

    allocator = std::make_unique<TempAllocatorImpl>(10 * 1024 * 1024);
    jobSystemThreadPool = std::make_unique<JobSystemThreadPool>(
        cMaxPhysicsJobs, cMaxPhysicsBarriers,
        std::thread::hardware_concurrency() - 1);

    physicsSystem = std::make_unique<PhysicsSystem>();

    objectVsObjectFilter = std::make_unique<ObjectLayerPairFilterImpl>();
    broadPhaseLayer = std::make_unique<BroadPhaseLayerInterfaceImpl>();
    objectVsBroadPhaseLayerFilter = std::make_unique<ObjectVsBroadPhaseLayerFilterImpl>();


    bodyInterface = &physicsSystem->GetBodyInterface();

    physicsSystem->Init(1024, 0, 1024, 1024, *(broadPhaseLayer.get()),
                        *(objectVsBroadPhaseLayerFilter.get()), *(objectVsObjectFilter.get()));

    logger->info("Physics engine initialized");
}

void PhysicsEngine::update() {
    physicsSystem->Update(1.0f / 60.0f, 1, allocator.get(),
                          jobSystemThreadPool.get());
}

Collider *PhysicsEngine::createMeshCollider(
    const std::vector<float> &vertices,
    const std::vector<unsigned int> &indices, const glm::vec3 &position,
    const glm::vec3 &rotation, const glm::vec3 &scale) {

    logger->debug("Create static mesh collider");

    JPH::VertexList verticesJPH;
    for (int i = 0; i < vertices.size() / 3; i++) {
        verticesJPH.push_back(
            {vertices[i * 3 + 0], vertices[i * 3 + 1], vertices[i * 3 + 2]});
    }

    JPH::IndexedTriangleList triangles;
    for (int i = 0; i < indices.size() / 3; i++) {
        triangles.push_back(JPH::IndexedTriangle(
            indices[i * 3 + 0], indices[i * 3 + 1], indices[i * 3 + 2]));
    }

    JPH::MeshShapeSettings shapeSettings(verticesJPH, triangles);
    JPH::Shape::ShapeResult result = shapeSettings.Create();

    if (result.HasError()) {
        throw std::runtime_error(
            std::format("Mesh collider creation error: {}", result.GetError()));
    }

    JPH::ShapeRefC meshShapeRaw = result.Get();

    JPH::ScaledShapeSettings scaledSettings(
        meshShapeRaw, JPH::Vec3(scale.x, scale.y, scale.z));
    JPH::ShapeRefC meshShape = scaledSettings.Create().Get();

    JPH::BodyCreationSettings settings{
        meshShape, JPH::RVec3(position.x, position.y, position.z),
        JPH::Quat::sEulerAngles(
            JPH::Vec3Arg(rotation.x, rotation.y, rotation.z)),
        JPH::EMotionType::Static, Layers::NON_MOVING};

    JPH::Body *meshBody = bodyInterface->CreateBody(settings);

    Collider *newCollider = new Collider(this, meshBody);

    logger->debug("Static mesh collider created");

    return newCollider;
}

void PhysicsEngine::addColider(Collider *collider) {
    bodyInterface->AddBody(collider->getBody()->GetID(),
                           JPH::EActivation::Activate);
}

void PhysicsEngine::removeCollider(Collider *collider) {
    bodyInterface->RemoveBody(collider->getBody()->GetID());
}