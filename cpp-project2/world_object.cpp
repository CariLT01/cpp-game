#include "world_object.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/gl.h>

#include "mesh.hpp"
#include "material.hpp"
#include "shader_program.hpp"

WorldObject::WorldObject(const ObjectTransform& transform, Material* material, Mesh* mesh) : position(transform.position), orientation(transform.orientation), scale(transform.scale), material(material), mesh(mesh) {
	updateModelMatrix();
}

WorldObject::~WorldObject() {
    
}

void WorldObject::updateModelMatrix() {
	modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, position);
	modelMatrix = glm::rotate(modelMatrix, glm::radians(orientation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(orientation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(orientation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	modelMatrix = glm::scale(modelMatrix, scale);
}

void WorldObject::update() {
	updateModelMatrix();
}

void WorldObject::render(bool applyMaterial) {
	if (applyMaterial) {
		material->use();
	}

	// Set uniform
	unsigned int program = material->getShaderProgram()->getProgram();
	unsigned int modelUniform = material->getShaderProgram()->getUniformLocation("uModelMatrix");

	glUniformMatrix4fv(modelUniform, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	// Render
	mesh->render();
}