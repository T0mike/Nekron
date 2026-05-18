#include "../include/ZombieTank.h"
#include <iostream>

ZombieTank :: ZombieTank(const std::string& name, double x, double y)
    :Zombie(name, 300, 2.0, x, y, 20), armor(10){}

ZombieTank:: ZombieTank(const ZombieTank& z) : Zombie(z), armor(z.armor) {}

Zombie* ZombieTank::clone() const {
    return new ZombieTank(*this);
}

void ZombieTank::attack() {
    std::cout << name << " loveste pentru " << damage << " damage!\n";
}

void ZombieTank::takeDamage(int amount){
    int reduced = amount - armor;
    if(reduced > 0) hp -= reduced;
}

void ZombieTank::display(std::ostream& os) const {
    os << "[Tank] ";
    Zombie::display(os);
    os << "armor: " << armor << "\n";
}