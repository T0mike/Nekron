#include "../include/Game.h"
#include <algorithm>
#include <iostream>

Game::Game(const std::string& playerName, int playerHp, double playerSpeed, double x, double y)
    : player(playerName, playerHp, playerSpeed, x, y), score(0), wave(1) {}

Player& Game::getPlayer() { return player; }
std::vector<std::unique_ptr<Zombie>>& Game::getZombies() { return zombies; }

void Game::addZombie(std::unique_ptr<Zombie> z) {
    zombies.push_back(std::move(z));
}

bool Game::isGameOver() const {
    return !player.isAlive();
}

void Game::removeDeadZombies() {
    zombies.erase(
        std::remove_if(zombies.begin(), zombies.end(),
            [](const std::unique_ptr<Zombie>& z) { return !z->isAlive(); }),
        zombies.end()
    );
}

std::ostream& operator<<(std::ostream& os, const Game& g) {
    os << g.player;
    for (const auto& z : g.zombies)
        os << *z;
    os << "score: " << g.score << "\n";
    os << "wave: " << g.wave << "\n";
    return os;
}

ZombieBoss* Game::findBoss() {
    for (auto& z : zombies) {
        ZombieBoss* boss = dynamic_cast<ZombieBoss*>(z.get());
        if (boss != nullptr) return boss;
    }
    return nullptr;
}

void Game::onWaveEnd() {
    wave++;
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
        }
    }
}

void Game::applyDamageToAll(int amount) {
    for (auto& z : zombies) {
        z->takeDamage(amount);
        if (!z->isAlive()) 
            std::cout << z->getName() << " a murit!\n";
    }
}