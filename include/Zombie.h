#ifndef ZOMBIE_H
#define ZOMBIE_H
#include <string>
#include <ostream>

class Zombie {
protected:
    std::string name;
    int hp;
    double speed;
    double x, y;
    int damage;

    virtual void display(std::ostream& os) const;
public:
    Zombie(const std::string& name, int hp, double speed, double x, double y, int damage);
    Zombie(const Zombie& z);
    Zombie& operator=(const Zombie& other);
    virtual ~Zombie();

    virtual Zombie* clone() const = 0;
    virtual void attack() = 0;
    virtual void takeDamage(int amount);
    virtual bool isAlive() const;
    const std::string& getName() const;

    friend std::ostream& operator<<(std::ostream& os, const Zombie& z);
};

#endif
