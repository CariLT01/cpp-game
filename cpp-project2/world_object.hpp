#pragma once

#include "transform.hpp"
#include <glm/glm.hpp>

class Mesh;
class Material;

class WorldObject {
public:
	WorldObject(const ObjectTransform& transform, Material* material, Mesh* mesh);
	~WorldObject();

	void update();
	void render(bool applyMaterial = true);

    void setPosition(const glm::vec3& position);
    void setRotation(const glm::vec3& rotation);

	Material* getMaterial() const { return material; }

private:

	void updateModelMatrix();

	glm::mat4 modelMatrix;

	glm::vec3 position;
	glm::vec3 orientation;
	glm::vec3 scale;

	Material* material;
	Mesh* mesh;

};