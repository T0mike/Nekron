#ifndef ZOMBIE_TANK_H
#define ZOMBIE_TANK_H
#include "Zombie.h"

class ZombieTank : public Zombie {
    int armor;
public:
    ZombieTank(const std::string& name, double  x, double y);
    ZombieTank(const ZombieTank& z);

    Zombie* clone() const override;
    void attack() override;
    void takeDamage(int amount) override;

protected:
    void display(std::ostream& os) const override;
};

#endif