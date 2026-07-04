#include "../include/WorldRenderer.h"
#include "../include/TextureManager.h"
#include "../include/SpriteRects.h"
#include <cmath>
#include <random>

namespace {
constexpr float CHUNK = 256.f;
constexpr float GRASS_BLOCK = 36.f * sheet::SCALE;
constexpr float PROP_MARGIN = 480.f;

long long chunkKey(int cx, int cy) {
    return (static_cast<long long>(cx) << 32) ^ static_cast<unsigned int>(cy);
}

unsigned long long chunkSeed(int cx, int cy, unsigned int worldSeed) {
    unsigned long long h = static_cast<unsigned long long>(static_cast<unsigned int>(cx));
    h = (h << 32) | static_cast<unsigned int>(cy);
    h ^= static_cast<unsigned long long>(worldSeed) * 0x9E3779B97F4A7C15ULL;
    h += 0x9E3779B97F4A7C15ULL;
    h = (h ^ (h >> 30)) * 0xBF58476D1CE4E5B9ULL;
    h = (h ^ (h >> 27)) * 0x94D049BB133111EBULL;
    return h ^ (h >> 31);
}
}

WorldRenderer::WorldRenderer(unsigned int worldSeed) : seed(worldSeed) {
    auto& tm = TextureManager::instance();
    grassSprite.setTexture(tm.get("grass"));
    grassSprite.setTextureRect(sheet::GRASS_TILE);
    grassSprite.setScale(sheet::SCALE, sheet::SCALE);

    shadowSprite.setTexture(tm.get("shadow"));
    shadowSprite.setOrigin(8.f, 8.f);
    shadowSprite.setColor(sf::Color(255, 255, 255, 140));

    lightSprite.setTexture(tm.get("props"));
    lightSprite.setTextureRect(sheet::LIGHT_CONE_SOFT);
    lightSprite.setOrigin(static_cast<float>(sheet::LIGHT_CONE_SOFT.width) / 2.f, 0.f);
    lightSprite.setScale(sheet::SCALE, sheet::SCALE);
    lightSprite.setColor(sf::Color(255, 255, 255, 70));
}

const sf::Texture* WorldRenderer::textureById(int id) const {
    auto& tm = TextureManager::instance();
    switch (id) {
        case 1: return &tm.get("trees");
        case 2: return &tm.get("props");
        default: return &tm.get("grass");
    }
}

const WorldRenderer::Chunk& WorldRenderer::chunkAt(int cx, int cy) {
    if (chunks.size() > 4096)
        chunks.clear();
    auto it = chunks.find(chunkKey(cx, cy));
    if (it == chunks.end()) {
        Chunk fresh;
        generateChunk(cx, cy, fresh);
        it = chunks.emplace(chunkKey(cx, cy), std::move(fresh)).first;
    }
    return it->second;
}

void WorldRenderer::generateChunk(int cx, int cy, Chunk& chunk) {
    std::mt19937_64 rng(chunkSeed(cx, cy, seed));
    std::uniform_real_distribution<float> unit(0.f, 1.f);
    auto spot = [&]() {
        std::uniform_real_distribution<float> d(30.f, CHUNK - 30.f);
        return sf::Vector2f(static_cast<float>(cx) * CHUNK + d(rng),
                            static_cast<float>(cy) * CHUNK + d(rng));
    };

    const int nSmall = 2 + static_cast<int>(unit(rng) * 4.f);
    for (int i = 0; i < nSmall; i++) {
        const sf::IntRect& r = sheet::DECOR_SMALL[static_cast<int>(unit(rng) * 5.99f)];
        chunk.ground.push_back({r, spot(), 0});
    }
    if (unit(rng) < 0.30f)
        chunk.ground.push_back({sheet::DECOR_PEBBLES, spot(), 0});
    if (unit(rng) < 0.10f)
        chunk.ground.push_back({sheet::ROCK_BIG, spot(), 1});
    if (unit(rng) < 0.12f)
        chunk.ground.push_back({sheet::ROCK_SMALL, spot(), 1});

    if (cx >= -1 && cx <= 1 && cy >= -1 && cy <= 1)
        return;

    const auto& tm = TextureManager::instance();
    const sf::Texture* trees = &tm.get("trees");
    const sf::Texture* props = &tm.get("props");

    const float roll = unit(rng);
    const sf::Vector2f p = spot();
    auto tall = [&chunk, p](const sf::Texture* tex, const sf::IntRect& rect,
                            float shadow, float halfW, float height) {
        chunk.tall.push_back({tex, rect, p, shadow,
                              {p.x - halfW, p.y - height, halfW * 2.f, height}});
    };
    if (roll < 0.11f) {
        tall(trees, sheet::TREE_ROUND, 2.6f, 18.f, 26.f);
    } else if (roll < 0.21f) {
        tall(trees, sheet::TREE_PINE, 2.2f, 16.f, 24.f);
    } else if (roll < 0.26f) {
        tall(trees, sheet::BERRY_BUSH, 0.f, 42.f, 34.f);
    } else if (roll < 0.30f) {
        tall(trees, sheet::DEAD_TREE, 1.6f, 20.f, 20.f);
    } else if (roll < 0.34f) {
        tall(props, sheet::SANDBAGS, 0.f, 90.f, 50.f);
    } else if (roll < 0.38f) {
        tall(props, sheet::CRATE, 1.4f, 30.f, 44.f);
    } else if (roll < 0.41f) {
        tall(props, sheet::LAMP_POST, 0.f, 12.f, 14.f);
        chunk.lamps.push_back(p);
    } else if (roll < 0.425f) {
        tall(props, sheet::HOUSE, 0.f, 158.f, 170.f);
    }
}

void WorldRenderer::drawGround(sf::RenderTarget& target, const sf::FloatRect& view) {
    const int x0 = static_cast<int>(std::floor(view.left / GRASS_BLOCK));
    const int x1 = static_cast<int>(std::floor((view.left + view.width) / GRASS_BLOCK));
    const int y0 = static_cast<int>(std::floor(view.top / GRASS_BLOCK));
    const int y1 = static_cast<int>(std::floor((view.top + view.height) / GRASS_BLOCK));
    for (int gx = x0; gx <= x1; gx++)
        for (int gy = y0; gy <= y1; gy++) {
            grassSprite.setPosition(static_cast<float>(gx) * GRASS_BLOCK,
                                    static_cast<float>(gy) * GRASS_BLOCK);
            target.draw(grassSprite);
        }

    const int cx0 = static_cast<int>(std::floor((view.left - PROP_MARGIN) / CHUNK));
    const int cx1 = static_cast<int>(std::floor((view.left + view.width + PROP_MARGIN) / CHUNK));
    const int cy0 = static_cast<int>(std::floor((view.top - PROP_MARGIN) / CHUNK));
    const int cy1 = static_cast<int>(std::floor((view.top + view.height + PROP_MARGIN) / CHUNK));
    for (int cx = cx0; cx <= cx1; cx++)
        for (int cy = cy0; cy <= cy1; cy++) {
            const Chunk& chunk = chunkAt(cx, cy);
            for (const Decor& d : chunk.ground) {
                decorSprite.setTexture(*textureById(d.textureId));
                decorSprite.setTextureRect(d.rect);
                decorSprite.setOrigin(static_cast<float>(d.rect.width) / 2.f,
                                      static_cast<float>(d.rect.height));
                decorSprite.setScale(sheet::SCALE, sheet::SCALE);
                decorSprite.setPosition(d.pos);
                target.draw(decorSprite);
            }
            for (const Prop& prop : chunk.tall) {
                if (prop.shadowScale <= 0.f)
                    continue;
                shadowSprite.setScale(sheet::SCALE * prop.shadowScale,
                                      sheet::SCALE * prop.shadowScale * 0.6f);
                shadowSprite.setPosition(prop.pos.x, prop.pos.y - 2.f);
                target.draw(shadowSprite);
            }
        }
}

void WorldRenderer::collectProps(const sf::FloatRect& view, std::vector<Prop>& out) {
    const int cx0 = static_cast<int>(std::floor((view.left - PROP_MARGIN) / CHUNK));
    const int cx1 = static_cast<int>(std::floor((view.left + view.width + PROP_MARGIN) / CHUNK));
    const int cy0 = static_cast<int>(std::floor((view.top - PROP_MARGIN) / CHUNK));
    const int cy1 = static_cast<int>(std::floor((view.top + view.height + PROP_MARGIN) / CHUNK));
    for (int cx = cx0; cx <= cx1; cx++)
        for (int cy = cy0; cy <= cy1; cy++) {
            const Chunk& chunk = chunkAt(cx, cy);
            out.insert(out.end(), chunk.tall.begin(), chunk.tall.end());
        }
}

void WorldRenderer::collectColliders(const sf::FloatRect& area, std::vector<sf::FloatRect>& out) {
    const int cx0 = static_cast<int>(std::floor(area.left / CHUNK));
    const int cx1 = static_cast<int>(std::floor((area.left + area.width) / CHUNK));
    const int cy0 = static_cast<int>(std::floor(area.top / CHUNK));
    const int cy1 = static_cast<int>(std::floor((area.top + area.height) / CHUNK));
    for (int cx = cx0; cx <= cx1; cx++)
        for (int cy = cy0; cy <= cy1; cy++)
            for (const Prop& prop : chunkAt(cx, cy).tall)
                out.push_back(prop.collider);
}

void WorldRenderer::drawLights(sf::RenderTarget& target, const sf::FloatRect& view) {
    const int cx0 = static_cast<int>(std::floor((view.left - PROP_MARGIN) / CHUNK));
    const int cx1 = static_cast<int>(std::floor((view.left + view.width + PROP_MARGIN) / CHUNK));
    const int cy0 = static_cast<int>(std::floor((view.top - PROP_MARGIN) / CHUNK));
    const int cy1 = static_cast<int>(std::floor((view.top + view.height + PROP_MARGIN) / CHUNK));
    for (int cx = cx0; cx <= cx1; cx++)
        for (int cy = cy0; cy <= cy1; cy++)
            for (const sf::Vector2f& lamp : chunkAt(cx, cy).lamps) {
                lightSprite.setPosition(lamp.x + 6.f * sheet::SCALE,
                                        lamp.y - 74.f * sheet::SCALE);
                target.draw(lightSprite, sf::RenderStates(sf::BlendAdd));
            }
}
