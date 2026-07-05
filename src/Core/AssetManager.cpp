#include "AssetManager.h"
#include <iostream>

AssetManager::~AssetManager() {
    clear();
}

bool AssetManager::loadTexture(const std::string& id, const std::string& filePath) {
    // If texture is already loaded, return true
    if (textures.find(id) != textures.end()) {
        return true;
    }

    Texture2D tex = LoadTexture(filePath.c_str());
    if (tex.id == 0) {
        std::cerr << "Failed to load texture: " << filePath << std::endl;
        return false;
    }

    textures[id] = tex;
    return true;
}

Texture2D AssetManager::getTexture(const std::string& id) {
    auto it = textures.find(id);
    if (it != textures.end()) {
        return it->second;
    }
    // Return empty texture (id = 0)
    return Texture2D{ 0, 0, 0, 0, 0 };
}

void AssetManager::unloadTexture(const std::string& id) {
    auto it = textures.find(id);
    if (it != textures.end()) {
        UnloadTexture(it->second);
        textures.erase(it);
    }
}

void AssetManager::clear() {
    for (auto const& [id, tex] : textures) {
        UnloadTexture(tex);
    }
    textures.clear();
}
