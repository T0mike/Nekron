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
    void addAmmo(int amount);
    bool isOutOfAmmo() const;
    bool fire();

    const std::string& getName() const;
    int getDamage() const;
    double getFireRate() const;
    int getAmmo() const;
    int getMaxAmmo() const;
    int getRange() const;
    friend std::ostream& operator<<(std::ostream& os, const Weapon& w);
};

#endif
