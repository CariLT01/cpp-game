#include "material.hpp"
#include <memory>
#include <unordered_map>

class MaterialRegistry {
public:
	MaterialRegistry();
	~MaterialRegistry();

	void addMaterial(const std::string& name, Material* material);
	Material* getMaterial(const std::string& name);


private:

	std::unordered_map<std::string, std::unique_ptr<Material>> materials;
};