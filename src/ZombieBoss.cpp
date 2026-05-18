#include "../include/ZombieBoss.h"
#include <iostream>

ZombieBoss::ZombieBoss(const std::string& name, double x, double y)
    :Zombie(name, 500, 3.0, x, y, 40), phase(1){}

ZombieBoss::ZombieBoss(const ZombieBoss& z) : Zombie(z), phase(z.phase) {}

Zombie* ZombieBoss::clone() const{
    return new ZombieBoss(*this);
}

void ZombieBoss::attack() {
    if(phase == 1){
        std::cout << name << " ataca normal pentru " << damage << " damage\n";
    }else{
        std::cout << name << " ataca dublu pentru " << damage*2 << " damage\n";
    }
}

void ZombieBoss::display(std::ostream& os) const {
    os << "[Boss] ";
    Zombie::display(os);
    os << "phase: " << phase << "\n";
}

void swap(ZombieBoss& a, ZombieBoss& b) {
    swap(static_cast<Zombie&>(a), static_cast<Zombie&>(b));
    std::swap(a.phase, b.phase);
}

ZombieBoss& ZombieBoss::operator=(ZombieBoss other) {
    swap(*this, other);
    return *this;
}