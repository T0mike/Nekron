#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H
#include <SFML/Graphics/Texture.hpp>
#include <map>
#include <string>

class TextureManager {
    std::map<std::string, sf::Texture> textures;

    TextureManager();
public:
    TextureManager(const TextureManager&) = delete;
    TextureManager& operator=(const TextureManager&) = delete;

    static TextureManager& instance();
    static const std::string& baseDir();

    const sf::Texture& get(const std::string& id) const;
};

#endif
