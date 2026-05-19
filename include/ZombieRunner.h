#ifndef ZOMBIE_RUNNER_H
#define ZOMBIE_RUNNER_H
#include "Zombie.h"

class ZombieRunner : public Zombie{
    int stamina;
public:
    ZombieRunner(const std::string& name, double x, double y);
    ZombieRunner(const ZombieRunner& z);

    Zombie* clone() const override;
    void attack() override;

    ZombieRunner& operator=(ZombieRunner other);
    friend void swap(ZombieRunner& a, ZombieRunner& b);

protected:
    void display(std::ostream& os) const override;
};

#endif