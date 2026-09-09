#include <memory>
#include <glm/glm.hpp>
#include <vector>



class Scene;
class WorldObject;
class Mesh;
class Material;
class AssetLoader;
class LightManager;

struct Shard {
    glm::vec3 position;
    WorldObject* shardObject;
    unsigned int lightId;
    unsigned int lightIdTop;
};

class ShardsManager {
public:
    ShardsManager(Scene* scene, AssetLoader* assetLoader, LightManager* lightManger);
    ~ShardsManager();

    void addShard(const glm::vec3& position);
    void update(float dt);

private:

    void initialize();

    Scene* scene;
    AssetLoader* assetLoader;

    std::vector<std::unique_ptr<Shard>> shards;
    std::unique_ptr<Mesh> shardMesh;
    std::unique_ptr<Material> shardMaterial;

    LightManager* lightManager;

    int frame = 0;
};