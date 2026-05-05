#include "../include/Game.h"
#include <algorithm>

Game::Game(const std::string& playerName, int playerHp, double playerSpeed, double x, double y)
    : player(playerName, playerHp, playerSpeed, x, y), score(0), wave(1) {}

Player& Game::getPlayer() { return player; }
std::vector<Zombie>& Game::getZombies() { return zombies; }

void Game::addZombie(const Zombie& z) {
    zombies.push_back(z);
}

bool Game::isGameOver() {
    return !player.isAlive();
}

void Game::removeDeadZombies() {
    zombies.erase(
        std::remove_if(zombies.begin(), zombies.end(),
            [](const Zombie& z) { return !z.isAlive(); }),
        zombies.end()
    );
}

std::ostream& operator<<(std::ostream& os, const Game& g) {
    os << g.player;
    for (const auto& z : g.zombies)
        os << z;
    os << "score: " << g.score << "\n";
    os << "wave: " << g.wave << "\n";
    return os;
}
