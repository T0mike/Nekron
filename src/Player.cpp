#include "../include/Player.h"
#include "../include/GameExceptions.h"
#include <algorithm>
#include <memory>

Player::Player(const std::string& name, int hp, double speed, double x, double y) {
    this->name = name;
    this->hp = hp;
    this->speed = speed;
    this->x = x;
    this->y = y;
}

std::ostream& operator<<(std::ostream& os, const Player& p) {
    os << "name: " << p.name << "\n";
    os << "hp: " << p.hp << "\n";
    os << "speed: " << p.speed << "\n";
    os << "x: " << p.x << " " << "y: " << p.y << "\n";
    os << p.weapons;
    return os;
}

void Player::addWeapon(const Weapon& w) {
    weapons.add(std::make_unique<Weapon>(w));
}

void Player::takeDamage(int damage) {
    this->hp -= damage;
    if (hp <= 0)
        throw GameOverException("Game over!");
}

void Player::heal(int amount, int maxHp) {
    hp = std::min(hp + amount, maxHp);
}

bool Player::isAlive() const {
    if (this->hp <= 0) return false;
    return true;
}

double Player::getX() const { return x; }
double Player::getY() const { return y; }
int Player::getHp() const { return hp; }
double Player::getSpeed() const { return speed; }

void Player::move(double dx, double dy) {
    x += dx;
    y += dy;
}

std::size_t Player::weaponCount() const { return weapons.size(); }
Weapon& Player::weaponAt(std::size_t index) { return weapons[index]; }
