#ifndef WEAPON_H
#define WEAPON_H
#include <string>
#include <ostream>

class Weapon {
    std::string name;
    int damage;
    double fireRate;
    int ammo;
    int maxAmmo;
    int range;
public:
    Weapon(const std::string& name, int damage, double fireRate, int ammo, int maxAmmo, int range);
    void reload();
    bool isOutOfAmmo();
    bool fire();
    friend std::ostream& operator<<(std::ostream& os, const Weapon& w);
};

#endif
