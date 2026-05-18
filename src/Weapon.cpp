#include "../include/Weapon.h"
#include "../include/GameExceptions.h"


Weapon::Weapon(const std::string& name, int damage, double fireRate, int ammo, int maxAmmo, int range) {
    if (damage < 0 || ammo < 0 || maxAmmo < 0 || range < 0)
        throw InvalidWeaponException("Weapon cu stats invalide!");
    this->name = name;
    this->damage = damage;
    this->fireRate = fireRate;
    this->ammo = ammo;
    this->maxAmmo = maxAmmo;
    this->range = range;
}

void Weapon::reload() {
    this->ammo = this->maxAmmo;
}

bool Weapon::isOutOfAmmo() const {
    return this->ammo == 0;
}

bool Weapon::fire() {
    if (isOutOfAmmo())
        throw OutOfAmmoException("Out of ammo!");
    this->ammo--;
    return true;
}

std::ostream& operator<<(std::ostream& os, const Weapon& w) {
    os << "name: " << w.name << "\n";
    os << "damage: " << w.damage << "\n";
    os << "fireRate: " << w.fireRate << "\n";
    os << "ammo: " << w.ammo << "\n";
    os << "maxAmmo: " << w.maxAmmo << "\n";
    os << "range: " << w.range << "\n";
    return os;
}
