#include "engine.hpp"
#include "asset_loader.hpp"
#include "player_controller.hpp"
#include "window.hpp"
#include "scene.hpp"
#include "mesh_registry.hpp"
#include "material_registry.hpp"
#include "colliders_registry.hpp"
#include "camera.hpp"
#include "mesh.hpp"
#include "player_controller.hpp"
#include "framebuffer.hpp"
#include "deferred_light_pass.hpp"
#include "light_manager.hpp"
#include "asset_loader.hpp"
#include "level_loader.hpp"
#include "physics_engine.hpp"
#include "text_renderer.hpp"
#include <format>
#include <glad/gl.h>

Engine::Engine()
    : meshRegistry(std::make_unique<MeshRegistry>()),
      materialRegistry(std::make_unique<MaterialRegistry>()),
      camera(std::make_unique<Camera>()) {
    logger->info("Engine instantiated");
    initialize();
}

Engine::~Engine() { logger->info("Engine destroying"); }

void Engine::initialize() {

    logger->debug("Instantiate window");

    window = std::make_unique<Window>(800, 600);

    logger->debug("Instantiating debugging context");

    // Make debug context
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glDebugMessageCallback(checkErrors, this);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    logger->debug("Create scene");

    scene = std::make_unique<Scene>();

    // Add some objects

    /* Material *basicMaterial = new Material("basic");

    Mesh *monkeyMesh = new Mesh();

    logger->debug("Instantiate asset loader"); */

    assetLoader = std::make_unique<AssetLoader>();
    physicsEngine = std::make_unique<PhysicsEngine>();
    levelLoader = std::make_unique<LevelLoader>(assetLoader.get(), physicsEngine.get());
    
    /* logger->debug("Loading mesh");

    assetLoader->loadMesh(monkeyMesh, "monkey.obj");

    logger->debug("Adding mesh");

    logger->debug("Monkey mesh is at: " +
                  std::format("{}", (void *)monkeyMesh));

    meshRegistry->addMesh(monkeyMesh, "monkey");

    monkeyMesh->upload();

    logger->debug("Add material");

    materialRegistry->addMaterial("basic", basicMaterial);

    logger->debug("Create world object");

    WorldObject *monkey = new WorldObject(
        ObjectTransform{.position = glm::vec3(0.0f, 0.0f, -2.0f),
                        .orientation = glm::vec3(0.0f, 0.0f, 0.0f),
                        .scale = glm::vec3(1.0f, 1.0f, 1.0f)},
        basicMaterial, monkeyMesh);

    logger->debug("Add object");

    scene->addObject(monkey); */

    logger->info("Initialization complete");

    playerController =
        std::make_unique<PlayerController>(camera.get(), window.get(), physicsEngine.get());

    framebuffer = std::make_unique<Framebuffer>(800, 600);
    deferredLightPass = std::make_unique<DeferredLightPass>(framebuffer.get());
    lightManager = std::make_unique<LightManager>(framebuffer.get(), camera.get(), 800, 600);
    collidersRegistry = std::make_unique<CollidersRegistry>();    
    textRenderer = std::make_unique<TextRenderer>(assetLoader.get(), 800, 600);
    
    textRenderer->createText("C++ Game showcase", 0, 0, 2.0f);

    // lightManager->addLight(Light{.position = glm::vec3(0.0f), .size = 5.0f});
    // lightManager->addLight(Light{.position = glm::vec3(0.0f, 1.0f, -2.0f), .size = 5.0f});

    LevelData data = levelLoader->loadLevel("level.json");


    // TODO: more informative names

    for (auto& mesh : data.meshes) {
        mesh->upload();
        meshRegistry->addMesh(mesh, std::format("mesh_{}", (void*)mesh));
    }

    for (auto& material : data.materials) {
        materialRegistry->addMaterial(std::format("material_{}", (void*)material), material);
    }

    for (auto& worldobject : data.worldObjects) {
        scene->addObject(worldobject);
    }

    for (auto& light : data.lights) {
        lightManager->addLight(light);
    }

    for (auto& collider : data.colliders) {
        collidersRegistry->addColider(collider);
        physicsEngine->addColider(collider);
    }

    playerController->setPosition(data.spawnPosition);

    window->lockMouse();
}

void APIENTRY Engine::checkErrors(GLenum source, GLenum type, GLuint id,
                                  GLenum severity, GLsizei length,
                                  const GLchar *message,
                                  const void *userParam) {

    if (userParam != nullptr) {
        Engine *currentInstance =
            static_cast<Engine *>(const_cast<void *>(userParam));
        currentInstance->logger->error(
            std::format("OpenGL error: {}, severity: {}, type: {}", message,
                        severity, type));
    }
}

void Engine::render() {

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    // glDepthMask(GL_FALSE);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // logger->debug("Render");

    framebuffer->bind();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    // glDepthMask(GL_FALSE);

    // Clear FBO
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // glDepthMask(GL_TRUE);
    
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);


    scene->render(camera.get());

    framebuffer->unbind();

    lightManager->runLightingPass();

    deferredLightPass->execute(lightManager->getLightTexture());

    textRenderer->render();
}

void Engine::tick() {

    // logger->debug("Tick!");

    playerController->update();
    physicsEngine->update();
    camera->update();
}

void Engine::run() {
    running = true;

    logger->debug("Begin main loop");

    while (running) {
        if (window->shouldClose()) {
            running = false;
        }

        tick();
        render();

        window->swapBuffers();
        window->pollEvents();
    }

    logger->debug("Main loop ended");
}