#ifndef CHARACTER_SPRITE_H
#define CHARACTER_SPRITE_H
#include <SFML/Graphics.hpp>

class CharacterSprite {
    sf::Sprite shadow;
    sf::Sprite body;
    sf::Sprite shirt;
    sf::Sprite moustache;
    sf::Sprite hair;

    int hairStyle = 1;
    int shirtStyle = 1;
    int moustacheStyle = 1;

    float animTimer = 0.f;
    int frame = 0;
    bool moving = false;
    bool facingLeft = false;

    void applyStyles();
public:
    CharacterSprite();

    void setStyles(int hairIdx, int shirtIdx, int moustacheIdx);
    void cycleHair();
    void cycleShirt();
    void cycleMoustache();

    void update(float dt, bool isMoving, bool faceLeft);
    void setPosition(sf::Vector2f feetPos);
    void draw(sf::RenderTarget& target) const;
    void drawShadow(sf::RenderTarget& target) const;
};

#endif
