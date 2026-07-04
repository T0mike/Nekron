#ifndef ZOMBIE_H
#define ZOMBIE_H
#include <string>
#include <ostream>

class Zombie {
private:
    static int count;
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
    void increaseHp(int amount);

    double getX() const;
    double getY() const;
    double getSpeed() const;
    int getDamage() const;
    int getHp() const;
    void moveTowards(double tx, double ty, double dist);
    void translate(double dx, double dy);

    static int getCount();

    friend std::ostream& operator<<(std::ostream& os, const Zombie& z);
    friend void swap(Zombie& a, Zombie& b);
};
void swap(Zombie& a, Zombie& b);
#endif
