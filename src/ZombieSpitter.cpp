#include "../include/ZombieSpitter.h"
#include <iostream>

ZombieSpitter::ZombieSpitter(const std::string& name, double x, double y)
    : Zombie(name, 70, 4.0, x, y, 12), venom(5) {}

ZombieSpitter::ZombieSpitter(const ZombieSpitter& z) : Zombie(z), venom(z.venom) {}

Zombie* ZombieSpitter::clone() const {
    return new ZombieSpitter(*this);
}

void ZombieSpitter::attack() {
    std::cout << name << " scuipa venin pentru " << damage << " damage!\n";
}

void ZombieSpitter::display(std::ostream& os) const {
    os << "[Spitter] ";
    Zombie::display(os);
    os << "venom: " << venom << "\n";
}

void swap(ZombieSpitter& a, ZombieSpitter& b) {
    swap(static_cast<Zombie&>(a), static_cast<Zombie&>(b));
    std::swap(a.venom, b.venom);
}

ZombieSpitter& ZombieSpitter::operator=(ZombieSpitter other) {
    swap(*this, other);
    return *this;
}
