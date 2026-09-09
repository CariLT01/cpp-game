#include "spdlog/sinks/stdout_color_sinks.h"
#include <vector>
#include <spdlog/spdlog.h>

class Camera;
class WorldObject;
class Material;

class Scene {
public:
	Scene();
	~Scene();

	void addObject(WorldObject* worldObject);
    void removeObject(WorldObject* worldObject);

	void render(Camera* camera);

private:

    bool worldObjectExists(WorldObject* worldObject);

	std::vector<std::unique_ptr<WorldObject>> worldObjects;

    std::shared_ptr<spdlog::logger> logger = spdlog::stdout_color_mt("scene");

    std::unordered_map<Material*, std::vector<WorldObject*>> materialRenderMap; 
    // TODO: use weak_ptr<Material> instead
};