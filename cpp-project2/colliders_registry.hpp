#include <memory>
#include <vector>

class Collider;

class CollidersRegistry {
public:
    CollidersRegistry();
    ~CollidersRegistry();

    void addColider(Collider* collider);

private:

    std::vector<std::unique_ptr<Collider>> colliders;
};