#include <memory>
#include "spdlog/sinks/stdout_color_sinks.h"
#include <glad/gl.h>

#include <spdlog/spdlog.h>
#include <windows.h>

#undef near
#undef far

// FWD
class Window;
class Scene;
class MaterialRegistry;
class MeshRegistry;
class Camera;
class PlayerController;
class DeferredLightPass;
class Framebuffer;
class LightManager;
class AssetLoader;
class LevelLoader;
class PhysicsEngine;
class CollidersRegistry;
class TextRenderer;

class Engine {
public:
	Engine();
	~Engine();

	void run();

	void render();
	void tick();

    static void APIENTRY checkErrors(GLenum source, GLenum type, GLuint id, 
                                  GLenum severity, GLsizei length, 
                                  const GLchar* message, const void* userParam);

private:

	void initialize();

	std::unique_ptr<Window> window;
	std::unique_ptr<Scene> scene;
	std::unique_ptr<MaterialRegistry> materialRegistry;
	std::unique_ptr<MeshRegistry> meshRegistry;
	std::unique_ptr<Camera> camera;
    std::unique_ptr<PlayerController> playerController;
    std::unique_ptr<DeferredLightPass> deferredLightPass;
    std::unique_ptr<Framebuffer> framebuffer;
    std::unique_ptr<LightManager> lightManager;
    std::unique_ptr<AssetLoader> assetLoader;
    std::unique_ptr<LevelLoader> levelLoader;
    std::unique_ptr<PhysicsEngine> physicsEngine;
    std::unique_ptr<CollidersRegistry> collidersRegistry;
    std::unique_ptr<TextRenderer> textRenderer;

	bool running = false;

    std::shared_ptr<spdlog::logger> logger = spdlog::stderr_color_mt("engine");
};