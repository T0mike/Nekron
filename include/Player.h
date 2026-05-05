#ifndef PLAYER_H
#define PLAYER_H
#include <string>
#include <vector>
#include <ostream>
#include "Weapon.h"

class Player {
    std::string name;
    int hp;
    double speed;
    double x, y;
    std::vector<Weapon> weapons;
public:
    Player(const std::string& name, int hp, double speed, double x, double y);
    friend std::ostream& operator<<(std::ostream& os, const Player& p);
    void addWeapon(const Weapon& w);
    void takeDamage(int damage);
    bool isAlive();
    std::vector<Weapon>& getWeapons();
};

#endif
