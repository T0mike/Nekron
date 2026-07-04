#include "../include/CharacterSprite.h"
#include "../include/TextureManager.h"
#include "../include/SpriteRects.h"
#include <string>

namespace {
constexpr float IDLE_FRAME_TIME = 0.22f;
constexpr float WALK_FRAME_TIME = 0.09f;

int wrapStyle(int idx) {
    return (idx - 1) % 4 + 1;
}
}

CharacterSprite::CharacterSprite() {
    auto& tm = TextureManager::instance();
    shadow.setTexture(tm.get("shadow"));
    shadow.setOrigin(8.f, 8.f);
    shadow.setScale(sheet::SCALE, sheet::SCALE);
    shadow.setColor(sf::Color(255, 255, 255, 160));

    body.setTexture(tm.get("body"));
    for (sf::Sprite* layer : {&body, &shirt, &moustache, &hair}) {
        layer->setOrigin(sheet::FEET_X, sheet::FEET_Y);
        layer->setScale(sheet::SCALE, sheet::SCALE);
    }
    applyStyles();
}

void CharacterSprite::applyStyles() {
    auto& tm = TextureManager::instance();
    shirt.setTexture(tm.get("shirt" + std::to_string(shirtStyle)));
    moustache.setTexture(tm.get("moustach" + std::to_string(moustacheStyle)));
    hair.setTexture(tm.get("hair" + std::to_string(hairStyle)));
}

void CharacterSprite::setStyles(int hairIdx, int shirtIdx, int moustacheIdx) {
    hairStyle = wrapStyle(hairIdx);
    shirtStyle = wrapStyle(shirtIdx);
    moustacheStyle = wrapStyle(moustacheIdx);
    applyStyles();
}

void CharacterSprite::cycleHair() { setStyles(hairStyle + 1, shirtStyle, moustacheStyle); }
void CharacterSprite::cycleShirt() { setStyles(hairStyle, shirtStyle + 1, moustacheStyle); }
void CharacterSprite::cycleMoustache() { setStyles(hairStyle, shirtStyle, moustacheStyle + 1); }

void CharacterSprite::update(float dt, bool isMoving, bool faceLeft) {
    facingLeft = faceLeft;
    if (isMoving != moving) {
        moving = isMoving;
        frame = 0;
        animTimer = 0.f;
    }
    const float frameTime = moving ? WALK_FRAME_TIME : IDLE_FRAME_TIME;
    const int count = moving ? sheet::WALK_COUNT : sheet::IDLE_COUNT;
    animTimer += dt;
    while (animTimer >= frameTime) {
        animTimer -= frameTime;
        frame = (frame + 1) % count;
    }

    const int sheetFrame = (moving ? sheet::WALK_FIRST : sheet::IDLE_FIRST) + frame;
    const sf::IntRect rect = sheet::frameRect(sheetFrame);
    const float sx = facingLeft ? -sheet::SCALE : sheet::SCALE;
    for (sf::Sprite* layer : {&body, &shirt, &moustache, &hair}) {
        layer->setTextureRect(rect);
        layer->setScale(sx, sheet::SCALE);
    }
}

void CharacterSprite::setPosition(sf::Vector2f feetPos) {
    for (sf::Sprite* layer : {&body, &shirt, &moustache, &hair})
        layer->setPosition(feetPos);
    shadow.setPosition(feetPos.x, feetPos.y - 2.f);
}

void CharacterSprite::drawShadow(sf::RenderTarget& target) const {
    target.draw(shadow);
}

void CharacterSprite::draw(sf::RenderTarget& target) const {
    target.draw(body);
    target.draw(shirt);
    target.draw(moustache);
    target.draw(hair);
}
