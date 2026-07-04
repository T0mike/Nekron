#ifndef ZOMBIE_BOSS_H
#define ZOMBIE_BOSS_H
#include "Zombie.h"

class ZombieBoss : public Zombie {
    int phase;
public:
    ZombieBoss(const std::string& name, double x, double y);
    ZombieBoss(const ZombieBoss& z);

    Zombie* clone() const override;
    void attack() override;

    int getPhase() const;
    void setPhase(int p);

    ZombieBoss& operator=(ZombieBoss other);
    friend void swap(ZombieBoss& a, ZombieBoss& b);

protected:
    void display(std::ostream& os) const override;
};

#endif