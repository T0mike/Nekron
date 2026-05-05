#ifndef ZOMBIE_H
#define ZOMBIE_H
#include <string>
#include <ostream>

class Zombie {
    std::string name;
    int hp;
    double speed;
    double x, y;
    int damage;
public:
    Zombie(const std::string& name, int hp, double speed, double x, double y, int damage);
    Zombie(const Zombie& z);
    Zombie& operator=(const Zombie& other);
    ~Zombie();

    friend std::ostream& operator<<(std::ostream& os, const Zombie& z);
    void takeDamage(int amount);
    bool isAlive() const;
    const std::string& getName() const;
};

#endif
