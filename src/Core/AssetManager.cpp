#include "AssetManager.h"
#include <iostream>

AssetManager::~AssetManager() {
    clear();
}

bool AssetManager::loadTexture(const std::string& id, const std::string& filePath, const std::vector<Color>& colorKeys) {
    if (textures.find(id) != textures.end()) {
        return true;
    }

    Image img = LoadImage(filePath.c_str());
    if (img.data == nullptr) {
        std::cerr << "Failed to load image: " << filePath << std::endl;
        return false;
    }

    for (Color c : colorKeys) {
        ImageColorReplace(&img, c, BLANK);
    }

    Texture2D tex = LoadTextureFromImage(img);
    UnloadImage(img);

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
