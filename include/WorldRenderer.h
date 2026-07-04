#ifndef WORLD_RENDERER_H
#define WORLD_RENDERER_H
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <vector>

class WorldRenderer {
public:
    struct Prop {
        const sf::Texture* texture;
        sf::IntRect rect;
        sf::Vector2f pos;
        float shadowScale;
        sf::FloatRect collider;
    };

    explicit WorldRenderer(unsigned int worldSeed);

    void drawGround(sf::RenderTarget& target, const sf::FloatRect& view);
    void collectProps(const sf::FloatRect& view, std::vector<Prop>& out);
    void collectColliders(const sf::FloatRect& area, std::vector<sf::FloatRect>& out);
    void drawLights(sf::RenderTarget& target, const sf::FloatRect& view);

private:
    struct Decor {
        sf::IntRect rect;
        sf::Vector2f pos;
        int textureId;
    };
    struct Chunk {
        std::vector<Decor> ground;
        std::vector<Prop> tall;
        std::vector<sf::Vector2f> lamps;
    };

    const Chunk& chunkAt(int cx, int cy);
    void generateChunk(int cx, int cy, Chunk& chunk);
    const sf::Texture* textureById(int id) const;

    std::unordered_map<long long, Chunk> chunks;
    sf::Sprite grassSprite;
    sf::Sprite decorSprite;
    sf::Sprite shadowSprite;
    sf::Sprite lightSprite;
    unsigned int seed;
};

#endif
