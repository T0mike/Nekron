#ifndef GAME_H
#define GAME_H
#include <vector>
#include <ostream>
#include "Player.h"
#include "Zombie.h"

class Game {
    Player player;
    std::vector<Zombie> zombies;
    int score;
    int wave;
public:
    Game(const std::string& playerName, int playerHp, double playerSpeed, double x, double y);
    Player& getPlayer();
    std::vector<Zombie>& getZombies();
    void addZombie(const Zombie& z);
    bool isGameOver();
    void removeDeadZombies();
    friend std::ostream& operator<<(std::ostream& os, const Game& g);
};

#endif
