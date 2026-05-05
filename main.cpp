#include <iostream>
#include "include/Weapon.h"
#include "include/Player.h"
#include "include/Zombie.h"
#include "include/Game.h"

int main() {
    Weapon pistol("pistol", 10, 1, 10, 10, 20);
    std::cout << pistol;
    Player Tomike("Tomike", 200, 20, 2.5, 3.5);
    Tomike.addWeapon(pistol);
    Tomike.getWeapons()[0].fire();
    Tomike.getWeapons()[0].reload();
    Zombie Mihai("Mihai", 10, 5, 2.5, 3.5, 5);
    std::cout << pistol;
    std::cout << Tomike;
    std::cout << Mihai;
    Game Test("Dummy", 100, 10, 0, 0);
    Test.addZombie(Mihai);
    Test.getZombies()[0].takeDamage(10);
    if (!Mihai.isAlive()) {
        std::cout << "Zombie " << Mihai.getName() << " a murit!\n";
    }
    Test.removeDeadZombies();
    Test.getPlayer().takeDamage(100);
    Test.isGameOver();
    std::cout << Test;
    return 0;
}
