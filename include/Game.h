#ifndef GAME_H
#define GAME_H
#include <vector>
#include <ostream>
#include <memory>
#include "Player.h"
#include "Zombie.h"
#include "ZombieBoss.h"
#include "ZombieNormal.h"
#include"ZombieTank.h"
#include "ZombieRunner.h"

class Game {
    Player player;
    std::vector<std::unique_ptr<Zombie>> zombies;
    int score;
    int wave;
public:
    Game(const std::string& playerName, int playerHp, double playerSpeed, double x, double y);
    Player& getPlayer();
    void addZombie(std::unique_ptr<Zombie> z);
    bool isGameOver() const;
    void removeDeadZombies();
    ZombieBoss* findBoss();
    void onWaveEnd();
    void applyDamageToAll(int amount);
    friend std::ostream& operator<<(std::ostream& os, const Game& g);
};

#endif
