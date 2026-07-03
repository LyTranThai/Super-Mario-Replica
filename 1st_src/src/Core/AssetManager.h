#ifndef ASSET_MANAGER_H
#define ASSET_MANAGER_H

#include "raylib.h"
#include <map>
#include <string>

class AssetManager {
private:
    std::map<std::string, Texture2D> textures;

    AssetManager() = default;

public:
    static AssetManager& getInstance() {
        static AssetManager instance;
        return instance;
    }

    AssetManager(const AssetManager&) = delete;
    AssetManager& operator=(const AssetManager&) = delete;

    ~AssetManager();

    bool loadTexture(const std::string& id, const std::string& filePath);
    Texture2D getTexture(const std::string& id);
    void unloadTexture(const std::string& id);
    void clear();
};

#endif // ASSET_MANAGER_H
