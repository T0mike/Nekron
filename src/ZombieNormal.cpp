#include "../include/ZombieNormal.h"
#include <iostream>

ZombieNormal::ZombieNormal(const std::string& name, int hp, double speed, double x, double y, int damage)
    : Zombie(name, hp, speed, x, y, damage) {}

ZombieNormal::ZombieNormal(const ZombieNormal& z) : Zombie(z) {}

Zombie* ZombieNormal::clone() const {
    return new ZombieNormal(*this);
}

void ZombieNormal::attack() {
    std::cout << name << " musca pentru " << damage << " damage!\n";
}

void ZombieNormal::display(std::ostream& os) const {
    os << "[Normal] ";
    Zombie::display(os);
}