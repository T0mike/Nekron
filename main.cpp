#include <iostream>
#include <memory>
#include "include/Weapon.h"
#include "include/Player.h"
#include "include/Zombie.h"
#include "include/Game.h"
#include "include/ZombieNormal.h"
#include "include/ZombieTank.h"
#include "include/ZombieBoss.h"
#include "include/GameExceptions.h"
#include "include/ZombieRunner.h"
#include "include/ZombieFactory.h"
#include "include/GameStatistics.h"
#include "include/EntityPool.h"
#ifdef USE_SFML
#include "include/GameApp.h"

int main() {
    try {
        GameApp app;
        app.run();
    } catch (const GameException& e) {
        std::cerr << "Game error: " << e.what() << "\n";
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}

#else

int main() {
    ZombieNormal normal("Ion", 100, 5, 0, 0, 10);
    ZombieTank tank("Golem", 0, 0);
    ZombieBoss boss("Diablo", 0, 0);

    Zombie* z1 = &normal;
    Zombie* z2 = &tank;
    Zombie* z3 = &boss;
    std::cout << *z1 << *z2 << *z3;

    z1->attack();
    z2->attack();
    z3->attack();

    z1->takeDamage(15);
    z2->takeDamage(15);
    z3->takeDamage(15);

    Zombie* copie = z1->clone();
    std::cout << "Copie creata prin clone:\n" << *copie;
    delete copie;

    ZombieNormal altNormal("Vasile", 50, 3, 10, 10, 5);
    altNormal = normal;
    std::cout << "Dupa operator=:\n" << altNormal;

    std::cout << "Numar zombi vii: " << Zombie::getCount() << "\n";

    Game game("Player1", 100, 10, 0, 0);

    game.addZombie(ZombieFactory::create(ZombieType::Normal, "Ion2", 0, 0));
    game.addZombie(ZombieFactory::create(ZombieType::Tank, "Golem2", 0, 0));
    game.addZombie(ZombieFactory::create(ZombieType::Boss, "Diablo2", 0, 0));
    game.spawnWave(4);

    game.applyDamageToAll(50);
    game.removeDeadZombies();
    game.onWaveEnd();

    ZombieBoss* foundBoss = game.findBoss();
    if (foundBoss != nullptr) {
        std::cout << "Boss gasit prin dynamic_cast:\n" << *foundBoss;
        foundBoss->attack();
    }

    ZombieRunner* foundRunner = game.findRunner();
    if (foundRunner != nullptr) {
        std::cout << "Runner gasit prin dynamic_cast:\n" << *foundRunner;
        foundRunner->attack();
    }

    Weapon pistol("pistol", 10, 1, 5, 10, 20);
    std::cout << pistol;
    pistol.fire();
    pistol.reload();

    Player& p = game.getPlayer();
    p.addWeapon(pistol);
    std::cout << p;

    try {
        Weapon invalid("glitch", -10, 1, 5, 5, 10);
    } catch (const InvalidWeaponException& e) {
        std::cout << "Exceptie prinsa: " << e.what() << "\n";
    }

    try {
        Weapon gol("gol", 10, 1, 0, 10, 20);
        gol.fire();
    } catch (const OutOfAmmoException& e) {
        std::cout << "Exceptie prinsa: " << e.what() << "\n";
    }

    try {
        game.getPlayer().takeDamage(999);
    } catch (const GameException& e) {
        std::cout << "Exceptie prinsa: " << e.what() << "\n";
    }

    std::cout << "Game over? " << (game.isGameOver() ? "da" : "nu") << "\n";

    std::cout << game;

    std::cout << GameStatistics::instance();

    EntityPool<Zombie> horde;
    horde.add(ZombieFactory::create(ZombieType::Runner, "Speedy", 0, 0));
    horde.add(ZombieFactory::create(ZombieType::Tank, "Bulwark", 0, 0));
    horde.forEach([](Zombie& z) { z.attack(); });

    Player& erou = game.getPlayer();
    erou.heal(20, 100);
    std::cout << "Eroul are " << erou.weaponCount() << " arme\n";
    if (erou.weaponCount() > 0) {
        Weapon& arma = erou.weaponAt(0);
        arma.addAmmo(3);
        std::cout << "Arma: " << arma.getAmmo() << "/" << arma.getMaxAmmo()
                  << " gloante, fire rate " << arma.getFireRate()
                  << ", raza " << arma.getRange() << "\n";
    }

    game.addScore(50);
    std::cout << "Scor " << game.getScore() << " la valul " << game.getWave() << "\n";

    EntityPool<Zombie>& hoarda = game.getZombies();
    if (!hoarda.empty()) {
        Zombie& urmaritor = hoarda[0];
        urmaritor.moveTowards(erou.getX(), erou.getY(),
                              urmaritor.getSpeed() * 2.0);
        urmaritor.translate(1.0, -1.0);
        std::cout << urmaritor.getName() << " a ajuns la ("
                  << urmaritor.getX() << ", " << urmaritor.getY()
                  << ") cu " << urmaritor.getHp() << " hp si "
                  << urmaritor.getDamage() << " damage\n";
    }

    ZombieBoss diablo("Diablo2", 0, 0);
    diablo.takeDamage(300);
    if (diablo.getPhase() == 1)
        diablo.setPhase(2);
    std::cout << "Boss in faza " << diablo.getPhase() << ":\n";
    diablo.attack();

    GameStatistics& stats = GameStatistics::instance();
    std::cout << "Kills: " << stats.getTotalKills()
              << ", Score: " << stats.getTotalScore()
              << ", Wave max: " << stats.getHighestWave() << "\n";
    stats.reset();
    std::cout << "Dupa reset -> kills: " << stats.getTotalKills() << "\n";

    return 0;
}
#endif
