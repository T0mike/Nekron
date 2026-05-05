#include "../include/Zombie.h"
#include <iostream>

Zombie::Zombie(const std::string& name, int hp, double speed, double x, double y, int damage) {
    this->name = name;
    this->hp = hp;
    this->speed = speed;
    this->x = x;
    this->y = y;
    this->damage = damage;
}

Zombie::Zombie(const Zombie& z) {
    this->name = z.name;
    this->hp = z.hp;
    this->speed = z.speed;
    this->x = z.x;
    this->y = z.y;
    this->damage = z.damage;
}

Zombie& Zombie::operator=(const Zombie& other) {
    this->name = other.name;
    this->hp = other.hp;
    this->speed = other.speed;
    this->x = other.x;
    this->y = other.y;
    this->damage = other.damage;
    return *this;
}

Zombie::~Zombie() {
    std::cout << "Zombie " << name << " destroyed\n";
}

std::ostream& operator<<(std::ostream& os, const Zombie& z) {
    os << "name: " << z.name << "\n";
    os << "hp: " << z.hp << "\n";
    os << "speed: " << z.speed << "\n";
    os << "x: " << z.x << " " << "y: " << z.y << "\n";
    os << "damage: " << z.damage << "\n";
    return os;
}

void Zombie::takeDamage(int amount) {
    this->hp -= amount;
}

bool Zombie::isAlive() const {
    if (this->hp <= 0) return false;
    return true;
}

const std::string& Zombie::getName() const {
    return name;
}
