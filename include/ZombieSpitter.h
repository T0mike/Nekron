#ifndef ZOMBIE_SPITTER_H
#define ZOMBIE_SPITTER_H
#include "Zombie.h"

class ZombieSpitter : public Zombie {
    int venom;
public:
    ZombieSpitter(const std::string& name, double x, double y);
    ZombieSpitter(const ZombieSpitter& z);

    Zombie* clone() const override;
    void attack() override;

    ZombieSpitter& operator=(ZombieSpitter other);
    friend void swap(ZombieSpitter& a, ZombieSpitter& b);

protected:
    void display(std::ostream& os) const override;
};

#endif
