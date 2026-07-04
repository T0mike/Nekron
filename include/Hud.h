#ifndef HUD_H
#define HUD_H
#include <SFML/Graphics.hpp>
#include <array>
#include <string>

struct HudState {
    int hp = 0;
    int maxHp = 1;
    float waveProgress = 0.f;
    int ammo = 0;
    int maxAmmo = 0;
    sf::IntRect weaponIcon;
    bool weaponEvolved = false;
    int grenades = 0;
    bool reloading = false;
    float reloadProgress = 0.f;
    int wave = 1;
    int score = 0;
    int kills = 0;
    std::string banner;
    float bannerAlpha = 0.f;
    bool gameOver = false;
    bool paused = false;
    bool menu = false;
    bool levelUp = false;
    bool pauseMenu = false;
    int menuIndex = 0;
    bool musicOn = true;
    int level = 1;
    int bestScore = 0;
    std::array<std::string, 3> upgrades;
};

class Hud {
    sf::Sprite barBgBig;
    sf::Sprite barRed;
    sf::Sprite barBgSmall;
    sf::Sprite barGreen;
    sf::Sprite ammoPip;
    sf::Sprite weaponIcon;
    sf::Sprite grenadePip;
    sf::Font font;
    bool hasFont = false;

    void drawText(sf::RenderTarget& target, const std::string& str, float x, float y,
                  unsigned int size, sf::Color color, bool centered = false);
public:
    Hud();
    void draw(sf::RenderWindow& window, const HudState& state, const sf::View& uiView);
    const sf::Font* fontPtr() const;
};

#endif
