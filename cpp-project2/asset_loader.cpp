#include <stdexcept>
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include "asset_loader.hpp"
#include "mesh.hpp"
#include "texture.hpp"
#include <format>
#include <string>


#include <spdlog/spdlog.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glad/gl.h>

AssetLoader::AssetLoader() {}

AssetLoader::~AssetLoader() {}

void AssetLoader::loadMesh(Mesh *mesh, const std::string &filename) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err,
                                filename.c_str());

    if (!warn.empty()) {
        logger->warn("Asset " + filename + " loaded wtih warning: " + warn);
    }
    if (!err.empty()) {
        logger->error("Asset " + filename + " loaded wtih error: " + err);
    }
    if (!ret) {
        throw std::runtime_error("Asset loader failed to load: " + filename);
    }

    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<float> UVs;
    std::vector<unsigned int> indices;

    for (size_t s = 0; s < shapes.size(); s++) {
        size_t indexOffset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
            int fv = shapes[s].mesh.num_face_vertices[f];
            for (size_t v = 0; v < fv; v++) {
                tinyobj::index_t idx = shapes[s].mesh.indices[indexOffset + v];
                vertices.push_back(attrib.vertices[3 * idx.vertex_index + 0]);
                vertices.push_back(attrib.vertices[3 * idx.vertex_index + 1]);
                vertices.push_back(attrib.vertices[3 * idx.vertex_index + 2]);
                if (idx.normal_index >= 0) {
                    normals.push_back(attrib.normals[3 * idx.normal_index + 0]);
                    normals.push_back(attrib.normals[3 * idx.normal_index + 1]);
                    normals.push_back(attrib.normals[3 * idx.normal_index + 2]);
                }
                if (idx.texcoord_index >= 0) {
                    UVs.push_back(attrib.texcoords[2 * idx.texcoord_index + 0]);
                    UVs.push_back(attrib.texcoords[2 * idx.texcoord_index + 1]);
                }
                indices.push_back(static_cast<unsigned int>(indices.size()));
            }
            indexOffset += fv;
        }
    }

    mesh->setVertices(vertices);
    mesh->setNormals(normals);
    mesh->setIndices(indices);
    mesh->setUVs(UVs);

    logger->info(std::format("Stats: {} tris, {} normals, {} UVs, {} indices",
                             vertices.size(), normals.size(), UVs.size(),
                             indices.size()));
}

Texture *AssetLoader::loadTextureDefault(const std::string &filename,
                                         bool nearest) {
    Texture *newTexture = new Texture();
    glBindTexture(GL_TEXTURE_2D, newTexture->getTexture());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    if (!nearest) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                        GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                        GL_NEAREST_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

    stbi_set_flip_vertically_on_load(true);

    int width, height, nrChannels;
    unsigned char *data =
        stbi_load(filename.c_str(), &width, &height, &nrChannels, 4);

    if (data) {

        if (width <= 0 || height <= 0) {
            throw std::runtime_error(std::format(
                "Invalid dimensions for {}: {}x{}", filename, width, height));
        }

        if (data == nullptr) {
            throw std::runtime_error(
                std::format("Data is nullptr: {}", (void *)data));
        }

        logger->debug("Pixel dimensions: {}x{}", width, height);
        logger->debug("Number of channels: {}", nrChannels);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                     GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(data);
    } else {
        throw std::runtime_error(
            std::format("Texture load failed: {}", filename));
    }

    return newTexture;
}