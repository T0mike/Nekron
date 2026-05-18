#ifndef ZOMBIE_NORMAL_H
#define ZOMBIE_NORMAL_H
#include "Zombie.h"

class ZombieNormal : public Zombie {
public:
    ZombieNormal(const std::string& name, int hp, double speed, double x, double y, int damage);
    ZombieNormal(const ZombieNormal& z);

    Zombie* clone() const override;
    void attack() override;

    ZombieNormal& operator=(ZombieNormal other);
    friend void swap(ZombieNormal& a, ZombieNormal& b);

protected:
    void display(std::ostream& os) const override;
};

#endif