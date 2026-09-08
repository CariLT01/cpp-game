#include "nlohmann/detail/macro_scope.hpp"
#include <nlohmann/json.hpp>

namespace level_data {
enum MeshType { NORMAL, ENDPOINT };

NLOHMANN_JSON_SERIALIZE_ENUM(MeshType, {{MeshType::NORMAL, "NORMAL"},
                                        {MeshType::ENDPOINT, "ENDPOINT"}});

struct Vec3 {
    float x;
    float y;
    float z;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Vec3, x, y, z);
};

struct MeshBehavior {
    float damage;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(MeshBehavior, damage);
};

struct Material {
    std::string albedo;
    std::string normal;
    std::string specular;

    std::string name;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Material, albedo, normal, specular, name);
};

struct Mesh {

    std::string mesh;

    Vec3 position;
    Vec3 scale;
    Vec3 rotation;

    bool collidable;
    MeshType type;
    Material material;

    MeshBehavior behavior;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Mesh, mesh, position, scale, rotation,
                                   collidable, type, behavior, material);
};

struct Spawn {
    Vec3 position;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Spawn, position);
};

enum LightType { POINT, DIRECTIONAL };
NLOHMANN_JSON_SERIALIZE_ENUM(LightType,
                             {{LightType::POINT, "POINT"},
                              {LightType::DIRECTIONAL, "DIRECTIONAL"}});

struct Light {
    Vec3 position;
    Vec3 color;
    float radius;
    float intensity;

    LightType type;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Light, position, color, radius, intensity,
                                   type);
};

struct Level {
    std::vector<Mesh> meshes;
    Spawn spawn;
    std::vector<Light> lights;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Level, meshes, spawn, lights);
};
} // namespace level_data
