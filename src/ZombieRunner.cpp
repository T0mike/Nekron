#include "../include/ZombieRunner.h"
#include <iostream>

ZombieRunner::ZombieRunner(const std::string& name, double x, double y)
    : Zombie(name, 50, 8.0, x, y, 15), stamina(3){}

ZombieRunner::ZombieRunner(const ZombieRunner& z) : Zombie(z), stamina(z.stamina) {}

Zombie* ZombieRunner::clone() const {
    return new ZombieRunner(*this);
}

void ZombieRunner::attack() {
    if(stamina){
        std:: cout << name << " alearga si musca pentru " << damage * 2 << " damage!\n";
        stamina--;
    }else{
        std::cout << name << " musca obosit pentru " << damage << " damage!\n";
    }
}

void ZombieRunner::display(std::ostream& os) const {
    os << "[Runner] ";
    Zombie::display(os);
    os << "stamina: " << stamina << "\n";
}

void swap(ZombieRunner& a, ZombieRunner& b) {
    swap(static_cast<Zombie&>(a), static_cast<Zombie&>(b));
    std::swap(a.stamina, b.stamina);
}

ZombieRunner& ZombieRunner::operator=(ZombieRunner other) {
    swap(*this, other);
    return *this;
}