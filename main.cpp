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
#include <SFML/Graphics.hpp>
#endif


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

    GameStatistics& stats = GameStatistics::instance();
    std::cout << "Kills: " << stats.getTotalKills()
              << ", Score: " << stats.getTotalScore()
              << ", Wave max: " << stats.getHighestWave() << "\n";
    stats.reset();
    std::cout << "Dupa reset -> kills: " << stats.getTotalKills() << "\n";

    return 0;

    // sf::RenderWindow window(sf::VideoMode(800, 600), "Nekron");
    // window.setFramerateLimit(60);

    // sf::Texture grassTexture;
    // grassTexture.loadFromFile("assets/Sprites/Objects&Tiles/TileMapGrass.png");

    // sf::Texture bodyTexture;
    // bodyTexture.loadFromFile("assets/Sprites/Character/Body/Body.png");

    // sf::Texture shirtTexture;
    // shirtTexture.loadFromFile("assets/Sprites/Character/Shirt/Shirt1.png");

    // sf::Texture hairTexture;
    // hairTexture.loadFromFile("assets/Sprites/Character/Hair/Hair1.png");

    // const int SCALE = 4;
    // const int TILE = 16;
    // const int CHAR_FRAME = 32;
    // const int GRASS_BLOCK = 48;

    // sf::Sprite grassTile(grassTexture);
    // grassTile.setTextureRect(sf::IntRect(0, 0, GRASS_BLOCK, GRASS_BLOCK));
    // grassTile.setScale(SCALE, SCALE);

    // sf::Sprite bodySprite(bodyTexture);
    // bodySprite.setTextureRect(sf::IntRect(0, 0, CHAR_FRAME, CHAR_FRAME));
    // bodySprite.setScale(SCALE, SCALE);

    // sf::Sprite shirtSprite(shirtTexture);
    // shirtSprite.setTextureRect(sf::IntRect(0, 0, CHAR_FRAME, CHAR_FRAME));
    // shirtSprite.setScale(SCALE, SCALE);

    // sf::Sprite hairSprite(hairTexture);
    // hairSprite.setTextureRect(sf::IntRect(0, 0, CHAR_FRAME, CHAR_FRAME));
    // hairSprite.setScale(SCALE, SCALE);

    // sf::Vector2f playerPos(0.f, 0.f);
    // const float SPEED = 4.f;
    // const int FRAME_COUNT = 8;
    // const float FRAME_TIME = 0.1f;

    // int currentFrame = 0;
    // float frameTimer = 0.f;
    // sf::Clock clock;

    // sf::View camera(sf::FloatRect(0, 0, 800, 600));

    // while (window.isOpen()) {
    //     sf::Event event;
    //     while (window.pollEvent(event))
    //         if (event.type == sf::Event::Closed)
    //             window.close();

    //     float dt = clock.restart().asSeconds();

    //     bool moving = false;
    //     if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) { playerPos.y -= SPEED; moving = true; }
    //     if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) { playerPos.y += SPEED; moving = true; }
    //     if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) { playerPos.x -= SPEED; moving = true; }
    //     if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) { playerPos.x += SPEED; moving = true; }

    //     if (moving) {
    //         frameTimer += dt;
    //         if (frameTimer >= FRAME_TIME) {
    //             frameTimer = 0.f;
    //             currentFrame = (currentFrame + 1) % FRAME_COUNT;
    //         }
    //     } else {
    //         currentFrame = 0;
    //         frameTimer = 0.f;
    //     }

    //     sf::IntRect frameRect(currentFrame * CHAR_FRAME, 0, CHAR_FRAME, CHAR_FRAME);
    //     bodySprite.setTextureRect(frameRect);
    //     shirtSprite.setTextureRect(frameRect);
    //     hairSprite.setTextureRect(frameRect);

    //     bodySprite.setPosition(playerPos);
    //     shirtSprite.setPosition(playerPos);
    //     hairSprite.setPosition(playerPos);

    //     camera.setCenter(playerPos.x + CHAR_FRAME * SCALE / 2.f, playerPos.y + CHAR_FRAME * SCALE / 2.f);
    //     window.setView(camera);

    //     window.clear();

    //     const int blockSize = GRASS_BLOCK * SCALE;
    //     int startX = static_cast<int>((camera.getCenter().x - 400) / blockSize) - 1;
    //     int endX   = startX + 800 / blockSize + 3;
    //     int startY = static_cast<int>((camera.getCenter().y - 300) / blockSize) - 1;
    //     int endY   = startY + 600 / blockSize + 3;

    //     for (int x = startX; x < endX; x++)
    //         for (int y = startY; y < endY; y++) {
    //             grassTile.setPosition(static_cast<float>(x * blockSize), static_cast<float>(y * blockSize));
    //             window.draw(grassTile);
    //         }

    //     window.draw(bodySprite);
    //     window.draw(shirtSprite);
    //     window.draw(hairSprite);

    //     window.display();
    // }
    // return 0;
}
