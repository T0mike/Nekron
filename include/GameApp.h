#ifndef GAME_APP_H
#define GAME_APP_H
#include <SFML/Graphics.hpp>
#include <array>
#include <memory>
#include <random>
#include <string>
#include <unordered_map>
#include <vector>
#include "Game.h"
#include "CharacterSprite.h"
#include "WorldRenderer.h"
#include "Hud.h"

enum class GameState { Menu, Playing, Paused, LevelUp, GameOver, PauseMenu };

class GameApp {
    struct WeaponDef {
        sf::IntRect rect;
        sf::Vector2f grip;
        sf::Vector2f muzzle;
        float spread;
        int pellets;
        bool automatic;
    };
    struct Bullet {
        sf::Vector2f pos;
        sf::Vector2f vel;
        float remaining;
        int damage;
        bool crit;
        int pierce;
        const Zombie* lastHit;
    };
    struct EnemyShot {
        sf::Vector2f pos;
        sf::Vector2f vel;
        float remaining;
        int damage;
    };
    struct Grenade {
        sf::Vector2f pos;
        sf::Vector2f vel;
        float fuse;
        float airTime;
    };
    struct BloodParticle {
        sf::Vector2f pos;
        sf::Vector2f vel;
        float life;
        float size;
    };
    struct BloodDecal {
        sf::Vector2f pos;
        float radius;
    };
    struct DamageText {
        sf::Vector2f pos;
        float life;
        int value;
        bool crit;
    };
    struct Pickup {
        int type;
        sf::Vector2f pos;
        float age;
    };
    struct PlayerStats {
        float damageMult = 1.f;
        float speedMult = 1.f;
        float reloadMult = 1.f;
        float fireRateMult = 1.f;
        float critChance = 0.10f;
        int maxHp = 100;
        bool akEvolved = false;
        bool shotgunEvolved = false;
        bool pistolEvolved = false;
    };
    struct ZombieViewState {
        float animTimer = 0.f;
        int frame = 0;
        bool facingLeft = false;
        float hitFlash = 0.f;
        float attackCooldown = 0.f;
        float spitCooldown = 1.2f;
        int maxHp = 1;
        bool seen = false;
    };

    sf::RenderWindow window;
    sf::View camera;
    sf::View uiView;
    bool fullscreen = false;
    std::unique_ptr<Game> game;
    CharacterSprite character;
    WorldRenderer world;
    Hud hud;

    sf::Sprite weaponSprite;
    sf::Sprite zombieSprite;
    sf::Sprite zombieShadow;
    sf::Sprite ammoPickupSprite;
    sf::RectangleShape tracerShape;
    sf::CircleShape glowShape;
    sf::RectangleShape zombieHpBg;
    sf::RectangleShape zombieHpFill;
    sf::CircleShape bloodShape;
    sf::RectangleShape medkitShape;
    sf::RectangleShape medkitCross;
    sf::CircleShape coinShape;
    sf::CircleShape spitShape;
    sf::Sprite grenadeSprite;
    sf::CircleShape explosionShape;
    sf::RectangleShape nightOverlay;
    sf::CircleShape minimapDot;
    sf::Text damageLabel;
    bool hasDamageFont = false;

    std::vector<Bullet> bullets;
    std::vector<EnemyShot> enemyShots;
    std::vector<Grenade> grenades;
    std::vector<BloodParticle> bloodParticles;
    std::vector<BloodDecal> bloodDecals;
    std::vector<BloodDecal> scorchDecals;
    std::vector<std::pair<sf::Vector2f, float>> explosions;
    std::vector<DamageText> damageTexts;
    std::vector<Pickup> pickups;
    std::vector<sf::FloatRect> colliderCache;
    std::unordered_map<const Zombie*, ZombieViewState> zombieViews;
    std::mt19937 rng;

    GameState state = GameState::Menu;
    int pauseMenuIndex = 0;
    PlayerStats stats;
    std::size_t currentWeapon = 0;
    float fireCooldown = 0.f;
    bool reloading = false;
    float reloadTimer = 0.f;
    float reloadDuration = 1.f;

    int xp = 0;
    int xpNeeded = 60;
    int level = 1;
    int pendingUpgrades = 0;
    std::array<int, 3> upgradeChoices{0, 1, 2};

    int waveKills = 0;
    int waveTotal = 1;
    bool betweenWaves = true;
    float waveTimer = 1.5f;
    float bannerTimer = 0.f;
    int runKills = 0;
    int bestScore = 0;

    sf::Vector2f aimDir{1.f, 0.f};
    bool playerMoving = false;
    float shake = 0.f;
    float muzzleFlash = 0.f;
    float muzzleFlashScale = 2.2f;
    float clickBuffer = 0.f;
    sf::Vector2f cameraCenter{0.f, 0.f};

    float dashTimer = 0.f;
    float dashCooldown = 0.f;
    float invincibleTimer = 0.f;
    sf::Vector2f dashDir{1.f, 0.f};
    float dayTime = 0.f;
    float effectTime = 0.f;
    float critFlash = 0.f;
    float hurtFlash = 0.f;
    float hitStop = 0.f;
    float fade = 1.f;
    float zoom = 1.f;
    int grenadeCount = 2;

    void resetGame();
    void applyScreenLayout();
    void toggleFullscreen();
    void activatePauseOption();
    void spawnWave();
    void startReload();
    void tryDash();
    void throwGrenade();
    void explodeGrenade(sf::Vector2f pos);
    void updateGrenades(float dt);
    void updateEnemyShots(float dt);
    void updateSpitter(Zombie& z, ZombieViewState& view, sf::Vector2f target, float dt);
    void handleEvents();
    void update(float dt);
    void updatePlayer(float dt);
    void updateWeapon();
    void tryFire(float dt);
    void updateZombies(float dt);
    void updateBullets(float dt);
    void updatePickups(float dt);
    void render();
    void drawZombie(const Zombie& z, const ZombieViewState& view);
    void drawPickups();
    void drawMinimap();
    sf::Vector2f resolveWorldCollisions(sf::Vector2f pos, float radius);
    void addShake(float amount);
    void spawnBlood(sf::Vector2f pos, sf::Vector2f dir, int count, float speedScale);
    void spawnDeathSplatter(sf::Vector2f pos, float scale);
    void spawnDamageText(sf::Vector2f pos, int value, bool crit);
    void updateBlood(float dt);
    void onZombieKilled(const Zombie& z, sf::Vector2f dir);
    void grantXp(int amount);
    void rollUpgrades();
    void applyUpgrade(std::size_t slot);
    void loadBest();
    void saveBest();
    float nightDarkness() const;
    sf::Vector2f playerPos() const;
    Weapon& weapon();
    const WeaponDef& weaponDef() const;

public:
    GameApp();
    ~GameApp();
    void run();
};

#endif
