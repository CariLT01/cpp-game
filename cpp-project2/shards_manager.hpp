#include "spdlog/sinks/stdout_color_sinks.h"
#include <memory>
#include <glm/glm.hpp>
#include <unordered_map>
#include <spdlog/spdlog.h>



class Scene;
class WorldObject;
class Mesh;
class Material;
class AssetLoader;
class LightManager;
class PlayerController;
class PlayerStats;

struct Shard {
    glm::vec3 position;
    WorldObject* shardObject;
    unsigned int lightId;
    unsigned int lightIdTop;
};

class ShardsManager {
public:
    ShardsManager(Scene* scene, AssetLoader* assetLoader, LightManager* lightManger, PlayerController* playerController, PlayerStats* playerStats);
    ~ShardsManager();

    unsigned int addShard(const glm::vec3& position);
    void removeShard(unsigned int shardId);

    void update(float dt);

private:

    void initialize();

    Scene* scene;
    AssetLoader* assetLoader;

    std::unordered_map<unsigned int, std::unique_ptr<Shard>> shards;
    std::unique_ptr<Mesh> shardMesh;
    std::unique_ptr<Material> shardMaterial;

    LightManager* lightManager;
    PlayerController* playerController;
    PlayerStats* playerStats;

    int frame = 0;
    unsigned int shardCounter;


    std::shared_ptr<spdlog::logger> logger = spdlog::stdout_color_mt("shards_manager");
};