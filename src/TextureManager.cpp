#include "../include/TextureManager.h"
#include "../include/GameExceptions.h"
#include <utility>
#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

namespace {
const std::pair<const char*, const char*> kAssets[] = {
    {"body",      "assets/Sprites/Character/Body/Body.png"},
    {"shadow",    "assets/Sprites/Character/Shadow.png"},
    {"hair1",     "assets/Sprites/Character/Hair/Hair1.png"},
    {"hair2",     "assets/Sprites/Character/Hair/Hair2.png"},
    {"hair3",     "assets/Sprites/Character/Hair/Hair3.png"},
    {"hair4",     "assets/Sprites/Character/Hair/Hair4.png"},
    {"shirt1",    "assets/Sprites/Character/Shirt/Shirt1.png"},
    {"shirt2",    "assets/Sprites/Character/Shirt/Shirt2.png"},
    {"shirt3",    "assets/Sprites/Character/Shirt/Shirt3.png"},
    {"shirt4",    "assets/Sprites/Character/Shirt/Shirt4.png"},
    {"moustach1", "assets/Sprites/Character/Moustach/Moustach1.png"},
    {"moustach2", "assets/Sprites/Character/Moustach/Moustach2.png"},
    {"moustach3", "assets/Sprites/Character/Moustach/Moustach3.png"},
    {"moustach4", "assets/Sprites/Character/Moustach/Moustach4.png"},
    {"zombie",    "assets/Sprites/Zombie/Zombie.png"},
    {"zombieHard","assets/Sprites/Zombie/ZombieHard.png"},
    {"grass",     "assets/Sprites/Objects&Tiles/TileMapGrass.png"},
    {"trees",     "assets/Sprites/Objects&Tiles/Trees.png"},
    {"props",     "assets/Sprites/Objects&Tiles/House&Props.png"},
    {"ui",        "assets/Sprites/Objects&Tiles/UI.png"},
    {"weapons",   "assets/Sprites/Objects&Tiles/Weapons.png"},
    {"grenade",   "assets/Sprites/Objects&Tiles/Grenade.png"},
};

std::string exeDirectory() {
#ifdef _WIN32
    char buffer[MAX_PATH];
    const DWORD length = GetModuleFileNameA(nullptr, buffer, MAX_PATH);
    const std::string path(buffer, length);
    const std::size_t slash = path.find_last_of("\\/");
    return slash == std::string::npos ? std::string() : path.substr(0, slash + 1);
#else
    return {};
#endif
}
}

const std::string& TextureManager::baseDir() {
    static const std::string dir = exeDirectory();
    return dir;
}

TextureManager::TextureManager() {
    for (const auto& [id, path] : kAssets) {
        sf::Texture tex;
        if (!tex.loadFromFile(path) && !tex.loadFromFile(baseDir() + path))
            throw AssetLoadException(path);
        tex.setSmooth(false);
        textures.emplace(id, std::move(tex));
    }
}

TextureManager& TextureManager::instance() {
    static TextureManager mgr;
    return mgr;
}

const sf::Texture& TextureManager::get(const std::string& id) const {
    auto it = textures.find(id);
    if (it == textures.end())
        throw AssetLoadException("textura necunoscuta: " + id);
    return it->second;
}
