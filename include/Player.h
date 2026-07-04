#ifndef PLAYER_H
#define PLAYER_H
#include <string>
#include <ostream>
#include "Weapon.h"
#include "EntityPool.h"

class Player {
    std::string name;
    int hp;
    double speed;
    double x, y;
    EntityPool<Weapon> weapons;
public:
    Player(const std::string& name, int hp, double speed, double x, double y);
    friend std::ostream& operator<<(std::ostream& os, const Player& p);
    void addWeapon(const Weapon& w);
    void takeDamage(int damage);
    void heal(int amount, int maxHp);
    bool isAlive() const;

    double getX() const;
    double getY() const;
    int getHp() const;
    double getSpeed() const;
    void move(double dx, double dy);

    std::size_t weaponCount() const;
    Weapon& weaponAt(std::size_t index);
};

#endif
