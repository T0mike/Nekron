#include <iostream>
#include "include/Weapon.h"
#include "include/Player.h"
#include "include/Zombie.h"
#include "include/Game.h"
#include "include/ZombieNormal.h"
#include "include/ZombieTank.h"
#include "include/ZombieBoss.h"

int main() {
    ZombieNormal normal("Ion", 100, 5, 0, 0, 10);
    ZombieTank tank("Golem", 0, 0);
    ZombieBoss boss("Diablo", 0, 0);

    // test afisare prin pointer de baza (cerinta temei)
    Zombie* z1 = &normal;
    Zombie* z2 = &tank;
    Zombie* z3 = &boss;

    std::cout << *z1;
    std::cout << *z2;
    std::cout << *z3;

    // test attack virtual
    z1->attack();
    z2->attack();
    z3->attack();

    // test takeDamage - tank-ul primeste mai putin
    z1->takeDamage(15);
    z2->takeDamage(15); // tank: 15 - 10 armor = 5 damage real
    std::cout << *z1;
    std::cout << *z2;
    return 0;
}
