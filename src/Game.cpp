#include "../include/Game.h"
#include "../include/ZombieFactory.h"
#include "../include/GameStatistics.h"
#include <iostream>
#include <string>

Game::Game(const std::string& playerName, int playerHp, double playerSpeed, double x, double y)
    : player(playerName, playerHp, playerSpeed, x, y), score(0), wave(1) {}

Player& Game::getPlayer() { return player; }

void Game::addZombie(std::unique_ptr<Zombie> z) {
    zombies.add(std::move(z));
}

void Game::spawnWave(int count) {
    for (int i = 0; i < count; i++) {
        ZombieType type = static_cast<ZombieType>(i % 4);
        zombies.add(ZombieFactory::create(type, "Wave" + std::to_string(wave) + "_" + std::to_string(i),
                                          i * 10.0, 0.0));
    }
}

bool Game::isGameOver() const {
    return !player.isAlive();
}

void Game::removeDeadZombies() {
    zombies.removeDead();
}

std::ostream& operator<<(std::ostream& os, const Game& g) {
    os << g.player;
    os << g.zombies;
    os << "score: " << g.score << "\n";
    os << "wave: " << g.wave << "\n";
    return os;
}

ZombieBoss* Game::findBoss() {
    return findFirstOf<ZombieBoss>(zombies);
}

ZombieRunner* Game::findRunner() {
    return findFirstOf<ZombieRunner>(zombies);
}

void Game::onWaveEnd() {
    wave++;
    GameStatistics::instance().recordWave(wave);
    for (auto& z : zombies) {
        if (wave % 2 == 0) {
            ZombieNormal* normal = dynamic_cast<ZombieNormal*>(z.get());
            if (normal != nullptr)
                normal->increaseHp(25);
        } else {
            ZombieTank* tank = dynamic_cast<ZombieTank*>(z.get());
            if (tank != nullptr)
                tank->increaseHp(75);

            ZombieBoss* boss = dynamic_cast<ZombieBoss*>(z.get());
            if (boss != nullptr)
                boss->increaseHp(150);

            ZombieRunner* runner = dynamic_cast<ZombieRunner*>(z.get());
            if (runner != nullptr)
                runner->increaseHp(30);
        }
    }
}

void Game::applyDamageToAll(int amount) {
    for (auto& z : zombies) {
        z->takeDamage(amount);
        if (!z->isAlive()) {
            std::cout << z->getName() << " a murit!\n";
            score += 10;
            GameStatistics::instance().recordKill(10);
        }
    }
}
