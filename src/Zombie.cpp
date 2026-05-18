#include "../include/Zombie.h"
#include <iostream>
#include <algorithm>

int Zombie::count = 0;

Zombie::Zombie(const std::string& name, int hp, double speed, double x, double y, int damage) {
    this->name = name;
    this->hp = hp;
    this->speed = speed;
    this->x = x;
    this->y = y;
    this->damage = damage;
    count++;
}

Zombie::Zombie(const Zombie& z) {
    this->name = z.name;
    this->hp = z.hp;
    this->speed = z.speed;
    this->x = z.x;
    this->y = z.y;
    this->damage = z.damage;
    count++;
}

Zombie& Zombie::operator=(const Zombie& other) {
    name = other.name;
    hp = other.hp;
    speed = other.speed;
    x = other.x;
    y = other.y;
    damage = other.damage;
    return *this;
}


Zombie::~Zombie() {
    std::cout << "Zombie " << name << " destroyed\n";
    count--;
}

std::ostream& operator<<(std::ostream& os, const Zombie& z) {
    z.display(os);
    return os;
}

void Zombie::display(std::ostream& os) const {
    os << "name: " << name << "\n";
    os << "hp: " << hp << "\n";
    os << "speed: " << speed << "\n";
    os << "x: " << x << " " << "y: " << y << "\n";
    os << "damage: " << damage << "\n";
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

int Zombie::getCount() {
    return count;
}

void Zombie::increaseHp(int amount) {
    hp += amount;
}

void swap(Zombie& a, Zombie& b) {
    std::swap(a.name, b.name);
    std::swap(a.hp, b.hp);
    std::swap(a.speed, b.speed);
    std::swap(a.x, b.x);
    std::swap(a.y, b.y);
    std::swap(a.damage, b.damage);
}