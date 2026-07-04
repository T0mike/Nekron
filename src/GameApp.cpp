
#include "../include/GameApp.h"
#include "../include/GameExceptions.h"
#include "../include/GameStatistics.h"
#include "../include/SoundManager.h"
#include "../include/SpriteRects.h"
#include "../include/TextureManager.h"
#include "../include/ZombieFactory.h"
#include "../include/ZombieBoss.h"
#include "../include/ZombieNormal.h"
#include "../include/ZombieRunner.h"
#include "../include/ZombieSpitter.h"
#include "../include/ZombieTank.h"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <functional>
#include <string>

namespace {
constexpr unsigned int WINDOW_W = 1280;
constexpr unsigned int WINDOW_H = 720;
constexpr float PX_PER_SPEED = 34.f;
constexpr float BULLET_SPEED = 1150.f;
constexpr std::size_t MAX_DECALS = 500;
constexpr float RELOAD_TIME = 1.15f;
constexpr float PLAYER_RADIUS = 22.f;
constexpr float MELEE_COOLDOWN = 0.8f;
constexpr float WAVE_BREAK = 2.2f;
constexpr float DASH_TIME = 0.16f;
constexpr float DASH_SPEED = 1500.f;
constexpr float DASH_COOLDOWN = 1.2f;
constexpr float INVINCIBLE_TIME = 0.30f;
constexpr float DAY_CYCLE = 150.f;
constexpr float PICKUP_LIFETIME = 25.f;
constexpr float PICKUP_RADIUS = 36.f;

constexpr float PI = 3.14159265f;

constexpr int PICKUP_AMMO = 0;
constexpr int PICKUP_MEDKIT = 1;
constexpr int PICKUP_COIN = 2;
constexpr int PICKUP_GRENADE = 3;

constexpr int MAX_GRENADES = 3;
constexpr float GRENADE_RANGE = 420.f;
constexpr float GRENADE_FLIGHT = 0.55f;
constexpr float EXPLOSION_RADIUS = 160.f;
constexpr float MAGNET_RADIUS = 150.f;

const char* const kUpgradeNames[9] = {
    "+20% damage",
    "+15% move speed",
    "+25 max HP (heal 25)",
    "-20% reload time",
    "+15% fire rate",
    "+8% crit chance",
    "EVOLVE AK: fires 2 bullets",
    "EVOLVE SHOTGUN: 12 pellets",
    "EVOLVE PISTOL: piercing +50% dmg",
};

float length(sf::Vector2f v) { return std::sqrt(v.x * v.x + v.y * v.y); }

sf::Vector2f normalized(sf::Vector2f v) {
    const float len = length(v);
    return len > 0.0001f ? sf::Vector2f(v.x / len, v.y / len) : sf::Vector2f(1.f, 0.f);
}

struct ZombieLook {
    const char* texture;
    float scale;
    sf::Color tint;
    float halfWidth;
};

ZombieLook lookFor(const Zombie& z) {
    if (const auto* boss = dynamic_cast<const ZombieBoss*>(&z)) {
        if (boss->getPhase() >= 2)
            return {"zombieHard", 5.6f, sf::Color(255, 60, 60), 7.f};
        return {"zombieHard", 5.6f, sf::Color(255, 120, 120), 7.f};
    }
    if (dynamic_cast<const ZombieTank*>(&z))
        return {"zombieHard", 4.4f, sf::Color::White, 7.f};
    if (dynamic_cast<const ZombieRunner*>(&z))
        return {"zombie", 3.4f, sf::Color(185, 255, 175), 6.f};
    if (dynamic_cast<const ZombieSpitter*>(&z))
        return {"zombie", 3.8f, sf::Color(205, 145, 255), 6.f};
    return {"zombie", 4.f, sf::Color::White, 6.f};
}

float zombieRadius(const Zombie& z) {
    const ZombieLook look = lookFor(z);
    return look.halfWidth * look.scale;
}

sf::FloatRect zombieHitbox(const Zombie& z) {
    const ZombieLook look = lookFor(z);
    const auto x = static_cast<float>(z.getX());
    const auto y = static_cast<float>(z.getY());
    return {x - look.halfWidth * look.scale, y - 20.f * look.scale,
            look.halfWidth * 2.f * look.scale, 20.f * look.scale};
}

sf::FloatRect inflated(sf::FloatRect r, float by) {
    return {r.left - by, r.top - by, r.width + 2.f * by, r.height + 2.f * by};
}

int xpForKill(const Zombie& z) {
    if (dynamic_cast<const ZombieBoss*>(&z)) return 100;
    if (dynamic_cast<const ZombieTank*>(&z)) return 30;
    if (dynamic_cast<const ZombieSpitter*>(&z)) return 20;
    if (dynamic_cast<const ZombieRunner*>(&z)) return 15;
    return 10;
}
}

GameApp::GameApp()
    : window(sf::VideoMode(WINDOW_W, WINDOW_H), "Nekron",
             sf::Style::Titlebar | sf::Style::Close | sf::Style::Resize),
      camera(sf::FloatRect(0.f, 0.f, static_cast<float>(WINDOW_W), static_cast<float>(WINDOW_H))),
      uiView(sf::FloatRect(0.f, 0.f, static_cast<float>(WINDOW_W), static_cast<float>(WINDOW_H))),
      world(20260702u),
      rng(std::random_device{}()) {
    window.setFramerateLimit(60);
    window.setMouseCursorVisible(true);
    applyScreenLayout();

    auto& tm = TextureManager::instance();
    SoundManager::instance().startMusic();
    weaponSprite.setTexture(tm.get("weapons"));
    zombieSprite.setTexture(tm.get("zombie"));
    zombieShadow.setTexture(tm.get("shadow"));
    zombieShadow.setOrigin(8.f, 8.f);
    zombieShadow.setColor(sf::Color(255, 255, 255, 150));

    ammoPickupSprite.setTexture(tm.get("ui"));
    ammoPickupSprite.setTextureRect(sheet::UI_AMMO_PIP);
    ammoPickupSprite.setOrigin(4.f, 2.5f);
    ammoPickupSprite.setScale(3.f, 3.f);

    medkitShape.setSize({22.f, 22.f});
    medkitShape.setOrigin(11.f, 11.f);
    medkitShape.setFillColor(sf::Color(235, 235, 235));
    medkitShape.setOutlineThickness(2.f);
    medkitShape.setOutlineColor(sf::Color(60, 60, 60));
    medkitCross.setFillColor(sf::Color(200, 40, 40));

    coinShape.setRadius(8.f);
    coinShape.setOrigin(8.f, 8.f);
    coinShape.setFillColor(sf::Color(250, 210, 70));
    coinShape.setOutlineThickness(2.f);
    coinShape.setOutlineColor(sf::Color(150, 110, 20));

    spitShape.setRadius(7.f);
    spitShape.setOrigin(7.f, 7.f);
    spitShape.setFillColor(sf::Color(140, 230, 90));
    spitShape.setOutlineThickness(2.f);
    spitShape.setOutlineColor(sf::Color(60, 120, 40));

    grenadeSprite.setTexture(tm.get("grenade"));
    grenadeSprite.setOrigin(6.f, 6.5f);

    explosionShape.setRadius(1.f);
    explosionShape.setOrigin(1.f, 1.f);

    tracerShape.setSize({26.f, 8.f});
    tracerShape.setOrigin(13.f, 4.f);
    tracerShape.setFillColor(sf::Color(255, 232, 120));
    tracerShape.setOutlineThickness(2.f);
    tracerShape.setOutlineColor(sf::Color(180, 130, 40, 190));
    glowShape.setRadius(15.f);
    glowShape.setOrigin(15.f, 15.f);
    glowShape.setFillColor(sf::Color(255, 200, 70, 100));

    bloodShape.setRadius(1.f);
    bloodShape.setOrigin(1.f, 1.f);

    zombieHpBg.setFillColor(sf::Color(35, 35, 35, 190));
    zombieHpFill.setFillColor(sf::Color(215, 40, 40));

    nightOverlay.setFillColor(sf::Color(8, 10, 30, 0));
    minimapDot.setRadius(3.f);

    if (const sf::Font* font = hud.fontPtr()) {
        damageLabel.setFont(*font);
        damageLabel.setOutlineThickness(2.f);
        hasDamageFont = true;
    }

    std::uniform_int_distribution<int> style(1, 4);
    character.setStyles(style(rng), style(rng), style(rng));

    loadBest();
    resetGame();
}

GameApp::~GameApp() {
    saveBest();
}

void GameApp::applyScreenLayout() {
    const sf::Vector2u size = window.getSize();
    if (size.x == 0 || size.y == 0)
        return;
    const float windowRatio = static_cast<float>(size.x) / static_cast<float>(size.y);
    const float targetRatio = static_cast<float>(WINDOW_W) / static_cast<float>(WINDOW_H);
    sf::FloatRect viewport(0.f, 0.f, 1.f, 1.f);
    if (windowRatio > targetRatio) {
        viewport.width = targetRatio / windowRatio;
        viewport.left = (1.f - viewport.width) / 2.f;
    } else if (windowRatio < targetRatio) {
        viewport.height = windowRatio / targetRatio;
        viewport.top = (1.f - viewport.height) / 2.f;
    }
    camera.setViewport(viewport);
    uiView.setViewport(viewport);
}

void GameApp::toggleFullscreen() {
    fullscreen = !fullscreen;
    if (fullscreen)
        window.create(sf::VideoMode::getDesktopMode(), "Nekron", sf::Style::Fullscreen);
    else
        window.create(sf::VideoMode(WINDOW_W, WINDOW_H), "Nekron",
                      sf::Style::Titlebar | sf::Style::Close | sf::Style::Resize);
    window.setFramerateLimit(60);
    window.setMouseCursorVisible(true);
    applyScreenLayout();
}

const GameApp::WeaponDef& GameApp::weaponDef() const {
    static const WeaponDef defs[3] = {
        {sheet::SMG_RECT,    {8.f, 3.f}, {28.f, 2.f}, 0.050f, 1, true},
        {sheet::RIFLE_RECT,  {7.f, 4.f}, {28.f, 2.f}, 0.160f, 7, false},
        {sheet::PISTOL_RECT, {3.f, 3.f}, {14.f, 1.f}, 0.020f, 1, false},
    };
    return defs[currentWeapon];
}

Weapon& GameApp::weapon() {
    return game->getPlayer().weaponAt(currentWeapon);
}

sf::Vector2f GameApp::playerPos() const {
    return {static_cast<float>(game->getPlayer().getX()),
            static_cast<float>(game->getPlayer().getY())};
}

void GameApp::loadBest() {
    for (const std::string& path : {std::string("nekron_save.txt"),
                                    TextureManager::baseDir() + "nekron_save.txt"}) {
        std::ifstream in(path);
        if (in && (in >> bestScore))
            return;
    }
    bestScore = 0;
}

void GameApp::saveBest() {
    if (game)
        bestScore = std::max(bestScore, game->getScore());
    std::ofstream out(TextureManager::baseDir() + "nekron_save.txt");
    if (out)
        out << bestScore << "\n";
}

void GameApp::resetGame() {
    if (game)
        bestScore = std::max(bestScore, game->getScore());
    stats = PlayerStats{};
    game = std::make_unique<Game>("Nekron", stats.maxHp, 10.0, 0.0, 0.0);
    Player& p = game->getPlayer();
    p.addWeapon(Weapon("ak", 13, 10.0, 30, 30, 620));
    p.addWeapon(Weapon("shotgun", 11, 1.4, 6, 6, 400));
    p.addWeapon(Weapon("pistol", 25, 5.0, 12, 12, 750));

    bullets.clear();
    enemyShots.clear();
    grenades.clear();
    explosions.clear();
    bloodParticles.clear();
    bloodDecals.clear();
    scorchDecals.clear();
    damageTexts.clear();
    pickups.clear();
    zombieViews.clear();
    shake = 0.f;
    muzzleFlash = 0.f;
    critFlash = 0.f;
    hurtFlash = 0.f;
    hitStop = 0.f;
    fade = 1.f;
    zoom = 1.f;
    grenadeCount = 2;
    camera.setSize(static_cast<float>(WINDOW_W), static_cast<float>(WINDOW_H));
    currentWeapon = 0;
    fireCooldown = 0.f;
    reloading = false;
    xp = 0;
    xpNeeded = 80;
    level = 1;
    pendingUpgrades = 0;
    waveKills = 0;
    waveTotal = 1;
    betweenWaves = true;
    waveTimer = 1.2f;
    bannerTimer = 0.f;
    runKills = 0;
    dashTimer = 0.f;
    dashCooldown = 0.f;
    invincibleTimer = 0.f;
    cameraCenter = {0.f, 0.f};
    camera.setCenter(0.f, 0.f);
}

void GameApp::activatePauseOption() {
    SoundManager& sound = SoundManager::instance();
    switch (pauseMenuIndex) {
        case 0:
            state = GameState::Playing;
            break;
        case 1:
            resetGame();
            state = GameState::Playing;
            break;
        case 2:
            sound.setMusicEnabled(!sound.isMusicEnabled());
            break;
        case 3:
            saveBest();
            state = GameState::Menu;
            break;
        default:
            window.close();
            break;
    }
    sound.play("pickup", 60.f);
}

void GameApp::spawnWave() {
    const int wave = game->getWave();
    waveTotal = std::min(5 + wave * 3, 55);
    waveKills = 0;

    std::uniform_real_distribution<float> angle(0.f, 2.f * PI);
    std::uniform_real_distribution<float> radius(880.f, 1150.f);
    std::uniform_real_distribution<float> pick(0.f, 1.f);
    const sf::Vector2f center = playerPos();

    for (int i = 0; i < waveTotal; i++) {
        const float a = angle(rng);
        const float r = radius(rng);
        const double x = center.x + std::cos(a) * r;
        const double y = center.y + std::sin(a) * r;

        ZombieType type = ZombieType::Normal;
        if (wave % 5 == 0 && (i == 0 || (i == 1 && wave >= 15)))
            type = ZombieType::Boss;
        else if (wave >= 3 && pick(rng) < 0.22f)
            type = ZombieType::Tank;
        else if (wave >= 4 && pick(rng) < 0.18f)
            type = ZombieType::Spitter;
        else if (wave >= 2 && pick(rng) < 0.30f)
            type = ZombieType::Runner;

        auto zombie = ZombieFactory::create(
            type, "V" + std::to_string(wave) + "_" + std::to_string(i), x, y);
        zombie->increaseHp(20 + (wave - 1) * 18);
        game->addZombie(std::move(zombie));
    }
    bannerTimer = 2.f;
    grenadeCount = std::min(MAX_GRENADES, grenadeCount + 1);
    SoundManager::instance().play("wave", 70.f);
}

void GameApp::startReload() {
    if (!reloading && weapon().getAmmo() < weapon().getMaxAmmo()) {
        reloading = true;
        reloadDuration = RELOAD_TIME * stats.reloadMult;
        reloadTimer = reloadDuration;
        SoundManager::instance().play("reload", 80.f, 0.05f);
    }
}

void GameApp::tryDash() {
    if (dashCooldown > 0.f)
        return;
    sf::Vector2f dir(0.f, 0.f);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) dir.y -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) dir.y += 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) dir.x -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) dir.x += 1.f;
    dashDir = (dir.x != 0.f || dir.y != 0.f) ? normalized(dir) : aimDir;
    dashTimer = DASH_TIME;
    dashCooldown = DASH_COOLDOWN;
    invincibleTimer = INVINCIBLE_TIME;
    SoundManager::instance().play("dash", 70.f, 0.10f);
    addShake(0.08f);
}

void GameApp::addShake(float amount) {
    shake = std::min(1.f, shake + amount);
}

void GameApp::throwGrenade() {
    if (grenadeCount <= 0)
        return;
    grenadeCount--;
    const sf::Vector2f from = playerPos() + sf::Vector2f(0.f, sheet::HAND_OFFSET_Y * sheet::SCALE);
    const sf::Vector2f mouse = window.mapPixelToCoords(sf::Mouse::getPosition(window), camera);
    const sf::Vector2f toMouse = mouse - from;
    const float dist = std::min(length(toMouse), GRENADE_RANGE);
    const sf::Vector2f vel = normalized(toMouse) * (dist / GRENADE_FLIGHT);
    grenades.push_back({from, vel, GRENADE_FLIGHT, 0.f});
    SoundManager::instance().play("throw", 75.f, 0.10f);
}

void GameApp::explodeGrenade(sf::Vector2f pos) {
    SoundManager::instance().play("explosion", 100.f, 0.08f);
    addShake(0.75f);
    hitStop = std::max(hitStop, 0.05f);
    explosions.push_back({pos, 0.f});

    std::uniform_real_distribution<float> off(-40.f, 40.f);
    std::uniform_real_distribution<float> rad(6.f, 14.f);
    scorchDecals.push_back({pos, 42.f});
    for (int i = 0; i < 8; i++)
        scorchDecals.push_back({{pos.x + off(rng), pos.y + off(rng) * 0.6f}, rad(rng)});
    if (scorchDecals.size() > MAX_DECALS / 2)
        scorchDecals.erase(scorchDecals.begin(), scorchDecals.begin()
                           + static_cast<long long>(scorchDecals.size() - MAX_DECALS / 2));

    for (auto& zPtr : game->getZombies()) {
        Zombie& z = *zPtr;
        if (!z.isAlive())
            continue;
        const sf::Vector2f zp(static_cast<float>(z.getX()), static_cast<float>(z.getY()));
        const float dist = length(zp - pos);
        if (dist > EXPLOSION_RADIUS + zombieRadius(z))
            continue;
        const int dmg = std::max(25, static_cast<int>(std::lround(
            110.f * (1.f - dist / (EXPLOSION_RADIUS * 1.6f)))));
        const int hpBefore = z.getHp();
        z.takeDamage(dmg);
        const sf::Vector2f dir = dist > 1.f ? normalized(zp - pos) : sf::Vector2f(1.f, 0.f);
        spawnDamageText(zp + sf::Vector2f(0.f, -30.f), hpBefore - z.getHp(), false);
        zombieViews[&z].hitFlash = 0.15f;
        z.translate(dir.x * 40.f, dir.y * 40.f);
        spawnBlood(zp + sf::Vector2f(0.f, -20.f), dir, 10, 1.2f);
        if (!z.isAlive())
            onZombieKilled(z, dir);
    }
}

void GameApp::updateGrenades(float dt) {
    for (Grenade& g : grenades) {
        g.pos += g.vel * dt;
        g.fuse -= dt;
        g.airTime += dt;
    }
    for (const Grenade& g : grenades)
        if (g.fuse <= 0.f)
            explodeGrenade(g.pos);
    grenades.erase(std::remove_if(grenades.begin(), grenades.end(),
                                  [](const Grenade& g) { return g.fuse <= 0.f; }),
                   grenades.end());

    for (auto& fx : explosions)
        fx.second += dt;
    explosions.erase(std::remove_if(explosions.begin(), explosions.end(),
                                    [](const auto& fx) { return fx.second > 0.30f; }),
                     explosions.end());
}

void GameApp::updateEnemyShots(float dt) {
    const sf::Vector2f target = playerPos() + sf::Vector2f(0.f, -28.f);
    for (EnemyShot& shot : enemyShots) {
        const float speed = length(shot.vel);
        const sf::Vector2f dir = shot.vel / std::max(speed, 0.001f);
        float travel = speed * dt;
        while (travel > 0.f && shot.remaining > 0.f) {
            const float step = std::min(travel, 20.f);
            travel -= step;
            shot.pos += dir * step;
            shot.remaining -= step;
            if (invincibleTimer <= 0.f && length(shot.pos - target) < PLAYER_RADIUS + 8.f) {
                shot.remaining = 0.f;
                invincibleTimer = std::max(invincibleTimer, 0.35f);
                hurtFlash = 0.15f;
                SoundManager::instance().play("hurt", 85.f, 0.08f);
                addShake(0.30f);
                spawnBlood(target, shot.vel, 5, 0.6f);
                try {
                    game->getPlayer().takeDamage(shot.damage);
                } catch (const GameOverException&) {
                    state = GameState::GameOver;
                    bestScore = std::max(bestScore, game->getScore());
                    saveBest();
                    SoundManager::instance().play("death", 100.f);
                }
            }
        }
    }
    enemyShots.erase(std::remove_if(enemyShots.begin(), enemyShots.end(),
                                    [](const EnemyShot& s) { return s.remaining <= 0.f; }),
                     enemyShots.end());
}

void GameApp::updateSpitter(Zombie& z, ZombieViewState& view, sf::Vector2f target, float dt) {
    const sf::Vector2f pos(static_cast<float>(z.getX()), static_cast<float>(z.getY()));
    const sf::Vector2f toPlayer = target - pos;
    const float dist = length(toPlayer);
    view.spitCooldown = std::max(0.f, view.spitCooldown - dt);
    const auto step = static_cast<double>(static_cast<float>(z.getSpeed()) * PX_PER_SPEED * dt);

    if (dist > 480.f) {
        z.moveTowards(target.x, target.y, step);
    } else if (dist < 240.f) {
        const sf::Vector2f away = normalized(pos - target);
        z.translate(away.x * step * 0.7, away.y * step * 0.7);
    } else if (view.spitCooldown <= 0.f) {
        view.spitCooldown = 1.8f;
        z.attack();
        enemyShots.push_back({pos + sf::Vector2f(0.f, -40.f),
                              normalized(toPlayer) * 380.f, 700.f,
                              z.getDamage() + (game->getWave() - 1) * 2});
        SoundManager::instance().play("spit", 70.f, 0.15f);
    }
    const sf::Vector2f before(static_cast<float>(z.getX()), static_cast<float>(z.getY()));
    const sf::Vector2f after = resolveWorldCollisions(before, 10.f);
    z.translate(after.x - before.x, after.y - before.y);
}

void GameApp::spawnBlood(sf::Vector2f pos, sf::Vector2f dir, int count, float speedScale) {
    std::uniform_real_distribution<float> spread(-0.8f, 0.8f);
    std::uniform_real_distribution<float> speed(90.f, 340.f);
    std::uniform_real_distribution<float> life(0.18f, 0.45f);
    std::uniform_real_distribution<float> size(2.f, 5.f);
    const float baseAngle = std::atan2(dir.y, dir.x);
    for (int i = 0; i < count; i++) {
        const float a = baseAngle + spread(rng);
        const float s = speed(rng) * speedScale;
        bloodParticles.push_back({pos,
                                  {std::cos(a) * s, std::sin(a) * s},
                                  life(rng), size(rng)});
    }
}

void GameApp::spawnDeathSplatter(sf::Vector2f pos, float scale) {
    std::uniform_real_distribution<float> off(-30.f, 30.f);
    std::uniform_real_distribution<float> rad(4.f, 10.f);
    bloodDecals.push_back({pos, 16.f * scale / 4.f + 6.f});
    for (int i = 0; i < 6; i++)
        bloodDecals.push_back({{pos.x + off(rng), pos.y + off(rng) * 0.6f}, rad(rng)});
    if (bloodDecals.size() > MAX_DECALS)
        bloodDecals.erase(bloodDecals.begin(), bloodDecals.begin()
                          + static_cast<long long>(bloodDecals.size() - MAX_DECALS));
}

void GameApp::spawnDamageText(sf::Vector2f pos, int value, bool crit) {
    if (!hasDamageFont)
        return;
    std::uniform_real_distribution<float> offX(-16.f, 16.f);
    std::uniform_real_distribution<float> offY(-14.f, 0.f);
    damageTexts.push_back({{pos.x + offX(rng), pos.y - 24.f + offY(rng)}, 0.75f, value, crit});
}

void GameApp::updateBlood(float dt) {
    for (DamageText& d : damageTexts) {
        d.pos.y -= 60.f * dt;
        d.life -= dt;
    }
    damageTexts.erase(std::remove_if(damageTexts.begin(), damageTexts.end(),
                                     [](const DamageText& d) { return d.life <= 0.f; }),
                      damageTexts.end());

    for (BloodParticle& p : bloodParticles) {
        p.pos += p.vel * dt;
        p.vel *= std::max(0.f, 1.f - 5.f * dt);
        p.life -= dt;
        if (p.life <= 0.f && p.size > 3.f)
            bloodDecals.push_back({p.pos, p.size * 0.9f});
    }
    bloodParticles.erase(std::remove_if(bloodParticles.begin(), bloodParticles.end(),
                                        [](const BloodParticle& p) { return p.life <= 0.f; }),
                         bloodParticles.end());
    if (bloodDecals.size() > MAX_DECALS)
        bloodDecals.erase(bloodDecals.begin(), bloodDecals.begin()
                          + static_cast<long long>(bloodDecals.size() - MAX_DECALS));
}

void GameApp::grantXp(int amount) {
    xp += amount;
    while (xp >= xpNeeded) {
        xp -= xpNeeded;
        level++;
        pendingUpgrades++;
    }
    if (pendingUpgrades > 0 && state == GameState::Playing) {
        xpNeeded = 50 + level * 30;
        rollUpgrades();
        state = GameState::LevelUp;
        SoundManager::instance().play("levelup", 85.f);
    }
}

void GameApp::rollUpgrades() {
    std::array<int, 6> pool{0, 1, 2, 3, 4, 5};
    std::shuffle(pool.begin(), pool.end(), rng);
    upgradeChoices = {pool[0], pool[1], pool[2]};
}

void GameApp::applyUpgrade(std::size_t slot) {
    switch (upgradeChoices[slot]) {
        case 0: stats.damageMult *= 1.20f; break;
        case 1: stats.speedMult *= 1.15f; break;
        case 2:
            stats.maxHp += 25;
            game->getPlayer().heal(25, stats.maxHp);
            break;
        case 3: stats.reloadMult *= 0.80f; break;
        case 4: stats.fireRateMult *= 1.15f; break;
        case 5: stats.critChance += 0.08f; break;
        case 6: stats.akEvolved = true; break;
        case 7: stats.shotgunEvolved = true; break;
        default: stats.pistolEvolved = true; break;
    }
    pendingUpgrades--;
    if (pendingUpgrades > 0) {
        rollUpgrades();
    } else {
        state = GameState::Playing;
    }
    SoundManager::instance().play("pickup", 80.f);
}

void GameApp::onZombieKilled(const Zombie& z, sf::Vector2f dir) {
    const ZombieLook look = lookFor(z);
    const sf::Vector2f zp(static_cast<float>(z.getX()), static_cast<float>(z.getY()));

    game->addScore(10);
    GameStatistics::instance().recordKill(10);
    waveKills++;
    runKills++;
    grantXp(xpForKill(z));

    spawnDeathSplatter(zp, look.scale);
    spawnBlood(zp + sf::Vector2f(0.f, -8.f * look.scale), dir, 14, 1.3f);
    SoundManager::instance().play("death", 80.f, 0.12f);
    addShake(0.18f);

    std::uniform_real_distribution<float> pick(0.f, 1.f);
    std::uniform_real_distribution<float> off(-24.f, 24.f);
    const bool isBoss = dynamic_cast<const ZombieBoss*>(&z) != nullptr;
    hitStop = std::max(hitStop, isBoss ? 0.20f : 0.03f);
    const int drops = isBoss ? 3 : (pick(rng) < 0.25f ? 1 : 0);
    for (int i = 0; i < drops; i++) {
        const float roll = pick(rng);
        int type = PICKUP_COIN;
        if (roll < 0.35f) type = PICKUP_AMMO;
        else if (roll < 0.60f) type = PICKUP_MEDKIT;
        else if (roll < 0.78f) type = PICKUP_GRENADE;
        pickups.push_back({type, {zp.x + off(rng), zp.y + off(rng)}, 0.f});
    }
}

void GameApp::handleEvents() {
    auto setWeapon = [this](std::size_t index) {
        if (index != currentWeapon) {
            currentWeapon = index;
            reloading = false;
            fireCooldown = std::max(fireCooldown, 0.15f);
            SoundManager::instance().play("reload", 55.f, 0.10f);
        }
    };

    sf::Event event{};
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();
        if (event.type == sf::Event::Resized)
            applyScreenLayout();
        if (event.type == sf::Event::LostFocus && state == GameState::Playing)
            state = GameState::Paused;
        if (state == GameState::Playing) {
            if (event.type == sf::Event::MouseButtonPressed
                && event.mouseButton.button == sf::Mouse::Left)
                clickBuffer = 0.2f;
            if (event.type == sf::Event::MouseWheelScrolled) {
                const std::size_t step = event.mouseWheelScroll.delta < 0.f ? 1 : 2;
                setWeapon((currentWeapon + step) % 3);
            }
        }
        if (event.type != sf::Event::KeyPressed)
            continue;
        const sf::Keyboard::Key key = event.key.code;
        if (key == sf::Keyboard::F11) {
            toggleFullscreen();
            continue;
        }
        if (key == sf::Keyboard::Escape) {
            switch (state) {
                case GameState::Menu:
                    window.close();
                    break;
                case GameState::PauseMenu:
                    state = GameState::Playing;
                    break;
                case GameState::Playing:
                case GameState::Paused:
                    pauseMenuIndex = 0;
                    state = GameState::PauseMenu;
                    break;
                case GameState::GameOver:
                    saveBest();
                    state = GameState::Menu;
                    break;
                default:
                    break;
            }
            continue;
        }
        switch (state) {
            case GameState::Menu:
            case GameState::GameOver:
                if (key == sf::Keyboard::Enter) {
                    resetGame();
                    state = GameState::Playing;
                }
                break;
            case GameState::Paused:
                if (key == sf::Keyboard::P)
                    state = GameState::Playing;
                break;
            case GameState::PauseMenu:
                if (key == sf::Keyboard::W || key == sf::Keyboard::Up)
                    pauseMenuIndex = (pauseMenuIndex + 4) % 5;
                else if (key == sf::Keyboard::S || key == sf::Keyboard::Down)
                    pauseMenuIndex = (pauseMenuIndex + 1) % 5;
                else if (key == sf::Keyboard::Enter)
                    activatePauseOption();
                break;
            case GameState::LevelUp:
                if (key == sf::Keyboard::Num1 || key == sf::Keyboard::Numpad1)
                    applyUpgrade(0);
                else if (key == sf::Keyboard::Num2 || key == sf::Keyboard::Numpad2)
                    applyUpgrade(1);
                else if (key == sf::Keyboard::Num3 || key == sf::Keyboard::Numpad3)
                    applyUpgrade(2);
                break;
            case GameState::Playing:
                switch (key) {
                    case sf::Keyboard::Num1:
                    case sf::Keyboard::Numpad1: setWeapon(0); break;
                    case sf::Keyboard::Num2:
                    case sf::Keyboard::Numpad2: setWeapon(1); break;
                    case sf::Keyboard::Num3:
                    case sf::Keyboard::Numpad3: setWeapon(2); break;
                    case sf::Keyboard::Q: setWeapon((currentWeapon + 1) % 3); break;
                    case sf::Keyboard::R: startReload(); break;
                    case sf::Keyboard::Space: tryDash(); break;
                    case sf::Keyboard::G: throwGrenade(); break;
                    case sf::Keyboard::P: state = GameState::Paused; break;
                    case sf::Keyboard::H: character.cycleHair(); break;
                    case sf::Keyboard::C: character.cycleShirt(); break;
                    case sf::Keyboard::M: character.cycleMoustache(); break;
                    default: break;
                }
                break;
        }
    }
}

void GameApp::updatePlayer(float dt) {
    dashTimer = std::max(0.f, dashTimer - dt);
    dashCooldown = std::max(0.f, dashCooldown - dt);
    invincibleTimer = std::max(0.f, invincibleTimer - dt);

    sf::Vector2f dir(0.f, 0.f);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) dir.y -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) dir.y += 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) dir.x -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) dir.x += 1.f;

    playerMoving = dir.x != 0.f || dir.y != 0.f;
    if (dashTimer > 0.f) {
        game->getPlayer().move(dashDir.x * DASH_SPEED * dt, dashDir.y * DASH_SPEED * dt);
        playerMoving = true;
    } else if (playerMoving) {
        dir = normalized(dir);
        const float speed = static_cast<float>(game->getPlayer().getSpeed())
                            * PX_PER_SPEED * stats.speedMult;
        game->getPlayer().move(dir.x * speed * dt, dir.y * speed * dt);
    }
    if (playerMoving) {
        const sf::Vector2f before = playerPos();
        const sf::Vector2f after = resolveWorldCollisions(before, 14.f);
        game->getPlayer().move(after.x - before.x, after.y - before.y);
    }

    const sf::Vector2f mouse = window.mapPixelToCoords(sf::Mouse::getPosition(window), camera);
    const sf::Vector2f hand = playerPos() + sf::Vector2f(0.f, sheet::HAND_OFFSET_Y * sheet::SCALE);
    aimDir = normalized(mouse - hand);

    character.update(dt, playerMoving, aimDir.x < 0.f);
    character.setPosition(playerPos());
    updateWeapon();
    tryFire(dt);
}

void GameApp::updateWeapon() {
    const WeaponDef& def = weaponDef();
    weaponSprite.setTextureRect(def.rect);
    weaponSprite.setOrigin(def.grip);
    weaponSprite.setPosition(playerPos() + sf::Vector2f(0.f, sheet::HAND_OFFSET_Y * sheet::SCALE));
    weaponSprite.setRotation(std::atan2(aimDir.y, aimDir.x) * 180.f / PI);
    weaponSprite.setScale(sheet::WEAPON_SCALE,
                          aimDir.x < 0.f ? -sheet::WEAPON_SCALE : sheet::WEAPON_SCALE);
}

void GameApp::tryFire(float dt) {
    fireCooldown -= dt;
    clickBuffer = std::max(0.f, clickBuffer - dt);
    if (reloading) {
        reloadTimer -= dt;
        if (reloadTimer <= 0.f) {
            weapon().reload();
            reloading = false;
            SoundManager::instance().play("reload", 90.f, 0.05f);
        }
        return;
    }
    const WeaponDef& def = weaponDef();
    const bool wantFire = def.automatic ? sf::Mouse::isButtonPressed(sf::Mouse::Left)
                                        : clickBuffer > 0.f;
    if (!wantFire || fireCooldown > 0.f)
        return;

    try {
        weapon().fire();
    } catch (const OutOfAmmoException&) {
        clickBuffer = 0.f;
        startReload();
        return;
    }
    clickBuffer = 0.f;
    fireCooldown = 1.f / (static_cast<float>(weapon().getFireRate()) * stats.fireRateMult);

    const sf::Vector2f muzzle = weaponSprite.getTransform().transformPoint(def.muzzle);
    std::uniform_real_distribution<float> jitter(-def.spread, def.spread);
    std::uniform_real_distribution<float> critRoll(0.f, 1.f);
    const float base = std::atan2(aimDir.y, aimDir.x);

    int shots = def.pellets;
    float damageMult = stats.damageMult;
    int pierce = 0;
    if (currentWeapon == 0 && stats.akEvolved)
        shots = 2;
    if (currentWeapon == 1 && stats.shotgunEvolved)
        shots = 12;
    if (currentWeapon == 2 && stats.pistolEvolved) {
        damageMult *= 1.5f;
        pierce = 2;
    }
    const int baseDamage = static_cast<int>(std::lround(
        static_cast<float>(weapon().getDamage()) * damageMult));
    for (int i = 0; i < shots; i++) {
        const float a = base + jitter(rng);
        const sf::Vector2f vel(std::cos(a) * BULLET_SPEED, std::sin(a) * BULLET_SPEED);
        const bool crit = critRoll(rng) < stats.critChance;
        bullets.push_back({muzzle, vel, static_cast<float>(weapon().getRange()),
                           crit ? baseDamage * 2 : baseDamage, crit, pierce, nullptr});
    }

    muzzleFlash = 0.06f;
    switch (currentWeapon) {
        case 0:
            muzzleFlashScale = 2.0f;
            SoundManager::instance().play("shot_ak", 75.f, 0.07f);
            addShake(0.12f);
            break;
        case 1:
            muzzleFlash = 0.09f;
            muzzleFlashScale = 3.4f;
            SoundManager::instance().play("shot_shotgun", 95.f, 0.05f);
            addShake(0.42f);
            break;
        default:
            muzzleFlashScale = 2.2f;
            SoundManager::instance().play("shot_pistol", 80.f, 0.08f);
            addShake(0.18f);
            break;
    }

    if (weapon().isOutOfAmmo())
        startReload();
}

sf::Vector2f GameApp::resolveWorldCollisions(sf::Vector2f pos, float radius) {
    colliderCache.clear();
    world.collectColliders({pos.x - 420.f, pos.y - 420.f, 840.f, 840.f}, colliderCache);
    for (const sf::FloatRect& box : colliderCache) {
        const float cx = std::clamp(pos.x, box.left, box.left + box.width);
        const float cy = std::clamp(pos.y, box.top, box.top + box.height);
        const sf::Vector2f d(pos.x - cx, pos.y - cy);
        const float dist = length(d);
        if (dist >= radius)
            continue;
        if (dist > 0.001f) {
            pos += normalized(d) * (radius - dist);
        } else {
            const float pushUp = pos.y - box.top + radius;
            const float pushDown = box.top + box.height - pos.y + radius;
            const float pushLeft = pos.x - box.left + radius;
            const float pushRight = box.left + box.width - pos.x + radius;
            const float m = std::min({pushUp, pushDown, pushLeft, pushRight});
            if (m == pushUp) pos.y -= pushUp;
            else if (m == pushDown) pos.y += pushDown;
            else if (m == pushLeft) pos.x -= pushLeft;
            else pos.x += pushRight;
        }
    }
    return pos;
}

void GameApp::updateZombies(float dt) {
    const sf::Vector2f target = playerPos();
    EntityPool<Zombie>& zombies = game->getZombies();

    for (auto& zPtr : zombies) {
        Zombie& z = *zPtr;
        ZombieViewState& view = zombieViews[&z];
        if (!view.seen) {
            view.seen = true;
            view.maxHp = std::max(z.getHp(), 1);
            std::uniform_real_distribution<float> phase(0.f, 0.5f);
            view.animTimer = phase(rng);
        }
        view.hitFlash = std::max(0.f, view.hitFlash - dt);
        view.attackCooldown = std::max(0.f, view.attackCooldown - dt);

        auto* boss = dynamic_cast<ZombieBoss*>(&z);
        if (boss != nullptr && boss->getPhase() == 1 && z.getHp() * 2 <= view.maxHp) {
            boss->setPhase(2);
            SoundManager::instance().play("hurt", 100.f, 0.20f);
            addShake(0.35f);
        }
        const bool enraged = boss != nullptr && boss->getPhase() >= 2;

        const sf::Vector2f pos(static_cast<float>(z.getX()), static_cast<float>(z.getY()));
        const sf::Vector2f toPlayer = target - pos;
        view.facingLeft = toPlayer.x < 0.f;

        if (dynamic_cast<ZombieSpitter*>(&z) != nullptr) {
            updateSpitter(z, view, target, dt);
            view.animTimer += dt;
            while (view.animTimer >= 0.10f) {
                view.animTimer -= 0.10f;
                view.frame = (view.frame + 1) % sheet::WALK_COUNT;
            }
            continue;
        }

        const float dist = length(toPlayer);
        const float reach = zombieRadius(z) + PLAYER_RADIUS;
        if (dist > reach * 0.9f) {
            const float waveSpeedMul = 1.f + std::min(0.5f,
                static_cast<float>(game->getWave() - 1) * 0.04f);
            const float speedMul = (enraged ? 1.5f : 1.f) * waveSpeedMul;
            z.moveTowards(target.x, target.y,
                          static_cast<double>(static_cast<float>(z.getSpeed())
                                              * PX_PER_SPEED * speedMul * dt));
            const sf::Vector2f before(static_cast<float>(z.getX()), static_cast<float>(z.getY()));
            const sf::Vector2f after = resolveWorldCollisions(before, 10.f);
            z.translate(after.x - before.x, after.y - before.y);
        } else if (view.attackCooldown <= 0.f && invincibleTimer <= 0.f) {
            view.attackCooldown = MELEE_COOLDOWN;
            z.attack();
            invincibleTimer = std::max(invincibleTimer, 0.35f);
            hurtFlash = 0.15f;
            SoundManager::instance().play("hurt", 90.f, 0.06f);
            addShake(0.45f);
            spawnBlood(target + sf::Vector2f(0.f, -30.f), toPlayer, 6, 0.7f);
            int meleeDamage = z.getDamage() + (game->getWave() - 1) * 2;
            if (enraged)
                meleeDamage *= 2;
            try {
                game->getPlayer().takeDamage(meleeDamage);
            } catch (const GameOverException&) {
                state = GameState::GameOver;
                bestScore = std::max(bestScore, game->getScore());
                saveBest();
                SoundManager::instance().play("death", 100.f);
            }
        }

        view.animTimer += dt;
        const float frameTime = enraged ? 0.07f : 0.10f;
        while (view.animTimer >= frameTime) {
            view.animTimer -= frameTime;
            view.frame = (view.frame + 1) % sheet::WALK_COUNT;
        }
    }

    std::vector<Zombie*> list;
    list.reserve(zombies.size());
    for (auto& zPtr : zombies)
        list.push_back(zPtr.get());
    for (std::size_t i = 0; i < list.size(); i++)
        for (std::size_t j = i + 1; j < list.size(); j++) {
            const float rr = zombieRadius(*list[i]) + zombieRadius(*list[j]);
            sf::Vector2f d(static_cast<float>(list[j]->getX() - list[i]->getX()),
                           static_cast<float>(list[j]->getY() - list[i]->getY()));
            const float dist = length(d);
            if (dist > 0.001f && dist < rr * 0.8f) {
                const sf::Vector2f push = normalized(d) * (rr * 0.8f - dist) * 0.5f;
                list[i]->translate(-push.x, -push.y);
                list[j]->translate(push.x, push.y);
            }
        }
}

void GameApp::updateBullets(float dt) {
    EntityPool<Zombie>& zombies = game->getZombies();

    for (Bullet& b : bullets) {
        const sf::Vector2f dir = b.vel / BULLET_SPEED;
        float travel = BULLET_SPEED * dt;
        while (travel > 0.f && b.remaining > 0.f) {
            const float step = std::min(travel, 24.f);
            travel -= step;
            b.pos += dir * step;
            b.remaining -= step;

            for (auto& zPtr : zombies) {
                Zombie& z = *zPtr;
                if (!z.isAlive() || &z == b.lastHit)
                    continue;
                if (inflated(zombieHitbox(z), 6.f).contains(b.pos)) {
                    const int hpBefore = z.getHp();
                    z.takeDamage(b.damage);
                    spawnDamageText(b.pos, hpBefore - z.getHp(), b.crit);
                    zombieViews[&z].hitFlash = 0.12f;
                    if (b.crit)
                        critFlash = std::max(critFlash, 0.07f);
                    if (b.pierce > 0) {
                        b.pierce--;
                        b.lastHit = &z;
                    } else {
                        b.remaining = 0.f;
                    }
                    const sf::Vector2f hitDir = normalized(b.vel);
                    const float scale = lookFor(z).scale;
                    const float knockback = 14.f * 4.f / scale;
                    z.translate(hitDir.x * knockback, hitDir.y * knockback);
                    spawnBlood(b.pos, hitDir, 8, 1.f);
                    SoundManager::instance().play("hit", 55.f, 0.15f);
                    if (!z.isAlive())
                        onZombieKilled(z, hitDir);
                    break;
                }
            }
        }
    }

    bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
                                 [](const Bullet& b) { return b.remaining <= 0.f; }),
                  bullets.end());

    for (auto& zPtr : zombies)
        if (!zPtr->isAlive())
            zombieViews.erase(zPtr.get());
    game->removeDeadZombies();
}

void GameApp::updatePickups(float dt) {
    const sf::Vector2f p = playerPos();
    for (Pickup& item : pickups) {
        item.age += dt;
        const bool wanted =
            !(item.type == PICKUP_AMMO && weapon().getAmmo() >= weapon().getMaxAmmo())
            && !(item.type == PICKUP_MEDKIT && game->getPlayer().getHp() >= stats.maxHp)
            && !(item.type == PICKUP_GRENADE && grenadeCount >= MAX_GRENADES);
        if (!wanted)
            continue;
        const float dist = length(item.pos - p);
        if (dist < MAGNET_RADIUS && dist > 8.f)
            item.pos += normalized(p - item.pos) * 380.f * dt;
        if (dist >= PICKUP_RADIUS)
            continue;
        switch (item.type) {
            case PICKUP_AMMO:
                weapon().addAmmo(std::max(1, weapon().getMaxAmmo() / 2));
                break;
            case PICKUP_MEDKIT:
                game->getPlayer().heal(25, stats.maxHp);
                break;
            case PICKUP_GRENADE:
                grenadeCount++;
                break;
            default:
                game->addScore(25);
                grantXp(15);
                break;
        }
        SoundManager::instance().play("pickup", 75.f, 0.08f);
        item.age = PICKUP_LIFETIME + 1.f;
    }
    pickups.erase(std::remove_if(pickups.begin(), pickups.end(),
                                 [](const Pickup& item) { return item.age > PICKUP_LIFETIME; }),
                  pickups.end());
}

float GameApp::nightDarkness() const {
    const float phase = dayTime * 2.f * PI / DAY_CYCLE;
    return std::clamp((std::sin(phase - PI / 2.f) + 1.f) * 0.5f, 0.f, 1.f);
}

void GameApp::update(float dt) {
    effectTime += dt;
    fade = std::max(0.f, fade - 2.5f * dt);
    if (hitStop > 0.f) {
        hitStop -= dt;
        return;
    }
    critFlash = std::max(0.f, critFlash - dt);
    hurtFlash = std::max(0.f, hurtFlash - dt);
    if (state == GameState::Menu || state == GameState::Paused
        || state == GameState::LevelUp || state == GameState::PauseMenu)
        return;

    bannerTimer = std::max(0.f, bannerTimer - dt);
    shake = std::max(0.f, shake - 2.2f * dt);
    muzzleFlash = std::max(0.f, muzzleFlash - dt);
    updateBlood(dt);
    if (state == GameState::GameOver)
        return;

    dayTime += dt;
    updatePlayer(dt);
    updateZombies(dt);
    updateBullets(dt);
    updateEnemyShots(dt);
    updateGrenades(dt);
    updatePickups(dt);

    if (game->getZombies().empty()) {
        if (!betweenWaves) {
            betweenWaves = true;
            waveTimer = WAVE_BREAK;
            const int completedWave = game->getWave();
            game->onWaveEnd();
            if (completedWave % 5 == 0) {
                std::vector<int> choices;
                if (!stats.akEvolved) choices.push_back(6);
                if (!stats.shotgunEvolved) choices.push_back(7);
                if (!stats.pistolEvolved) choices.push_back(8);
                if (!choices.empty()) {
                    std::shuffle(choices.begin(), choices.end(), rng);
                    std::array<int, 6> normal{0, 1, 2, 3, 4, 5};
                    std::shuffle(normal.begin(), normal.end(), rng);
                    for (std::size_t i = 0; choices.size() < 3; i++)
                        choices.push_back(normal[i]);
                    upgradeChoices = {choices[0], choices[1], choices[2]};
                    pendingUpgrades++;
                    state = GameState::LevelUp;
                    SoundManager::instance().play("levelup", 90.f);
                }
            }
        } else {
            waveTimer -= dt;
            if (waveTimer <= 0.f) {
                betweenWaves = false;
                spawnWave();
            }
        }
    }

    const sf::Vector2f goal = playerPos();
    const float k = std::min(1.f, 8.f * dt);
    cameraCenter += (goal - cameraCenter) * k;
    sf::Vector2f next = cameraCenter;
    if (shake > 0.f) {
        std::uniform_real_distribution<float> off(-1.f, 1.f);
        next += sf::Vector2f(off(rng), off(rng)) * (shake * shake * 16.f);
    }
    camera.setCenter(std::floor(next.x), std::floor(next.y));

    int nearCount = 0;
    for (const auto& zPtr : game->getZombies())
        if (length(sf::Vector2f(static_cast<float>(zPtr->getX()),
                                static_cast<float>(zPtr->getY())) - goal) < 380.f)
            nearCount++;
    const float targetZoom = 1.f + std::min(0.15f, static_cast<float>(nearCount) * 0.012f);
    zoom += (targetZoom - zoom) * std::min(1.f, 4.f * dt);
    camera.setSize(static_cast<float>(WINDOW_W) * zoom,
                   static_cast<float>(WINDOW_H) * zoom);
}

void GameApp::drawZombie(const Zombie& z, const ZombieViewState& view) {
    const ZombieLook look = lookFor(z);
    const sf::Vector2f pos(static_cast<float>(z.getX()), static_cast<float>(z.getY()));

    zombieShadow.setScale(look.scale, look.scale * 0.8f);
    zombieShadow.setPosition(pos.x, pos.y - 2.f);
    window.draw(zombieShadow);

    zombieSprite.setTexture(TextureManager::instance().get(look.texture));
    zombieSprite.setTextureRect(sheet::frameRect(sheet::WALK_FIRST + view.frame));
    zombieSprite.setOrigin(sheet::FEET_X, sheet::FEET_Y);
    zombieSprite.setScale(view.facingLeft ? -look.scale : look.scale, look.scale);
    zombieSprite.setPosition(pos);
    zombieSprite.setColor(view.hitFlash > 0.f ? sf::Color(255, 90, 90) : look.tint);
    window.draw(zombieSprite);

    if (z.getHp() < view.maxHp) {
        const float w = 12.f * look.scale;
        const float top = pos.y - 22.f * look.scale;
        const float ratio = std::clamp(static_cast<float>(z.getHp()) / static_cast<float>(view.maxHp), 0.f, 1.f);
        zombieHpBg.setSize({w, 5.f});
        zombieHpBg.setPosition(pos.x - w / 2.f, top);
        window.draw(zombieHpBg);
        zombieHpFill.setSize({(w - 2.f) * ratio, 3.f});
        zombieHpFill.setPosition(pos.x - w / 2.f + 1.f, top + 1.f);
        window.draw(zombieHpFill);
    }
}

void GameApp::drawPickups() {
    for (const Pickup& item : pickups) {
        const float bob = std::sin(item.age * 4.f) * 3.f;
        const sf::Vector2f pos(item.pos.x, item.pos.y + bob);
        const bool expiring = item.age > PICKUP_LIFETIME - 4.f;
        const auto alpha = static_cast<sf::Uint8>(
            expiring && std::fmod(item.age, 0.4f) < 0.2f ? 90 : 255);
        switch (item.type) {
            case PICKUP_AMMO:
                ammoPickupSprite.setColor(sf::Color(255, 255, 255, alpha));
                ammoPickupSprite.setPosition(pos);
                window.draw(ammoPickupSprite);
                break;
            case PICKUP_MEDKIT: {
                medkitShape.setFillColor(sf::Color(235, 235, 235, alpha));
                medkitShape.setPosition(pos);
                window.draw(medkitShape);
                medkitCross.setFillColor(sf::Color(200, 40, 40, alpha));
                medkitCross.setSize({14.f, 5.f});
                medkitCross.setOrigin(7.f, 2.5f);
                medkitCross.setPosition(pos);
                window.draw(medkitCross);
                medkitCross.setSize({5.f, 14.f});
                medkitCross.setOrigin(2.5f, 7.f);
                medkitCross.setPosition(pos);
                window.draw(medkitCross);
                break;
            }
            case PICKUP_GRENADE:
                grenadeSprite.setScale(1.7f, 1.7f);
                grenadeSprite.setRotation(0.f);
                grenadeSprite.setColor(sf::Color(255, 255, 255, alpha));
                grenadeSprite.setPosition(pos);
                window.draw(grenadeSprite);
                grenadeSprite.setColor(sf::Color::White);
                break;
            default:
                coinShape.setFillColor(sf::Color(250, 210, 70, alpha));
                coinShape.setPosition(pos);
                window.draw(coinShape);
                break;
        }
    }
}

void GameApp::drawMinimap() {
    const sf::View previous = window.getView();
    window.setView(uiView);

    const sf::Vector2f center(static_cast<float>(WINDOW_W) - 112.f, 112.f);
    const float radius = 90.f;
    const float mapScale = 1.f / 14.f;

    sf::CircleShape bg(radius);
    bg.setOrigin(radius, radius);
    bg.setPosition(center);
    bg.setFillColor(sf::Color(15, 20, 15, 170));
    bg.setOutlineThickness(2.f);
    bg.setOutlineColor(sf::Color(190, 190, 190, 120));
    window.draw(bg);

    const sf::Vector2f p = playerPos();
    auto plot = [&](sf::Vector2f worldPos, float dotR, sf::Color color) {
        sf::Vector2f d = (worldPos - p) * mapScale;
        const float len = length(d);
        if (len > radius - 8.f)
            d = normalized(d) * (radius - 8.f);
        minimapDot.setRadius(dotR);
        minimapDot.setOrigin(dotR, dotR);
        minimapDot.setFillColor(color);
        minimapDot.setPosition(center + d);
        window.draw(minimapDot);
    };

    for (const Pickup& item : pickups)
        plot(item.pos, 2.f, sf::Color(250, 210, 70, 220));
    for (const auto& zPtr : game->getZombies()) {
        const bool isBoss = dynamic_cast<const ZombieBoss*>(zPtr.get()) != nullptr;
        const bool isSpitter = dynamic_cast<const ZombieSpitter*>(zPtr.get()) != nullptr;
        sf::Color color = isBoss ? sf::Color(255, 60, 60)
                        : isSpitter ? sf::Color(205, 140, 255, 230)
                                    : sf::Color(220, 60, 60, 220);
        plot({static_cast<float>(zPtr->getX()), static_cast<float>(zPtr->getY())},
             isBoss ? 6.f : 3.f, color);
    }
    plot(p, 4.f, sf::Color::White);

    window.setView(previous);
}

void GameApp::render() {
    window.clear(sf::Color::Black);
    window.setView(camera);

    const sf::Vector2f c = camera.getCenter();
    const sf::Vector2f s = camera.getSize();
    const sf::FloatRect viewRect(c.x - s.x / 2.f, c.y - s.y / 2.f, s.x, s.y);

    world.drawGround(window, viewRect);

    bloodShape.setFillColor(sf::Color(32, 30, 26, 200));
    for (const BloodDecal& d : scorchDecals) {
        bloodShape.setScale(d.radius, d.radius * 0.65f);
        bloodShape.setPosition(d.pos);
        window.draw(bloodShape);
    }

    bloodShape.setFillColor(sf::Color(110, 12, 12, 185));
    for (const BloodDecal& d : bloodDecals) {
        bloodShape.setScale(d.radius, d.radius * 0.65f);
        bloodShape.setPosition(d.pos);
        window.draw(bloodShape);
    }

    drawPickups();
    character.drawShadow(window);

    struct Item {
        float y;
        std::function<void()> draw;
    };
    std::vector<Item> items;

    std::vector<WorldRenderer::Prop> props;
    world.collectProps(viewRect, props);
    items.reserve(props.size() + game->getZombies().size() + 1);
    for (const WorldRenderer::Prop& prop : props)
        items.push_back({prop.pos.y, [this, prop]() {
            sf::Sprite sprite(*prop.texture, prop.rect);
            sprite.setOrigin(static_cast<float>(prop.rect.width) / 2.f,
                             static_cast<float>(prop.rect.height));
            sprite.setScale(sheet::SCALE, sheet::SCALE);
            sprite.setPosition(prop.pos);
            window.draw(sprite);
        }});

    const sf::FloatRect cullRect = inflated(viewRect, 250.f);
    for (auto& zPtr : game->getZombies()) {
        const Zombie* z = zPtr.get();
        if (!cullRect.contains(static_cast<float>(z->getX()), static_cast<float>(z->getY())))
            continue;
        items.push_back({static_cast<float>(z->getY()),
                         [this, z]() { drawZombie(*z, zombieViews[z]); }});
    }

    if (state != GameState::Menu)
        items.push_back({playerPos().y, [this]() {
            character.draw(window);
            window.draw(weaponSprite);
        }});

    std::sort(items.begin(), items.end(),
              [](const Item& a, const Item& b) { return a.y < b.y; });
    for (const Item& item : items)
        item.draw();

    bloodShape.setFillColor(sf::Color(190, 25, 25, 230));
    for (const BloodParticle& p : bloodParticles) {
        bloodShape.setScale(p.size, p.size);
        bloodShape.setPosition(p.pos);
        window.draw(bloodShape);
    }

    for (const Bullet& b : bullets) {
        tracerShape.setPosition(b.pos);
        tracerShape.setRotation(std::atan2(b.vel.y, b.vel.x) * 180.f / PI);
        window.draw(tracerShape);
        glowShape.setPosition(b.pos);
        window.draw(glowShape, sf::RenderStates(sf::BlendAdd));
    }

    for (const EnemyShot& shot : enemyShots) {
        const float pulse = 1.f + 0.2f * std::sin(effectTime * 18.f);
        spitShape.setScale(pulse, pulse);
        spitShape.setPosition(shot.pos);
        window.draw(spitShape);
    }

    for (const Grenade& g : grenades) {
        const float t = g.airTime / (g.airTime + g.fuse);
        const float arc = std::sin(t * PI);
        zombieShadow.setScale(1.6f, 1.1f);
        zombieShadow.setPosition(g.pos);
        window.draw(zombieShadow);
        const float gScale = 1.7f * (1.f + arc * 0.5f);
        grenadeSprite.setScale(gScale, gScale);
        grenadeSprite.setRotation(g.airTime * 540.f);
        grenadeSprite.setPosition(g.pos.x, g.pos.y - arc * 46.f);
        window.draw(grenadeSprite);
    }

    for (const auto& fx : explosions) {
        const float t = fx.second / 0.30f;
        const float radius = 30.f + (EXPLOSION_RADIUS + 40.f) * t;
        const auto alpha = static_cast<sf::Uint8>(200.f * (1.f - t));
        explosionShape.setScale(radius, radius * 0.8f);
        explosionShape.setPosition(fx.first);
        explosionShape.setFillColor(sf::Color(255, 170, 60, alpha));
        window.draw(explosionShape, sf::RenderStates(sf::BlendAdd));
        explosionShape.setScale(radius * 0.55f, radius * 0.45f);
        explosionShape.setFillColor(sf::Color(255, 240, 180, alpha));
        window.draw(explosionShape, sf::RenderStates(sf::BlendAdd));
    }

    if (muzzleFlash > 0.f && state == GameState::Playing) {
        const sf::Vector2f muzzle = weaponSprite.getTransform().transformPoint(weaponDef().muzzle);
        const float pulse = std::min(1.f, muzzleFlash / 0.06f);
        glowShape.setScale(muzzleFlashScale * pulse, muzzleFlashScale * pulse);
        glowShape.setPosition(muzzle);
        window.draw(glowShape, sf::RenderStates(sf::BlendAdd));
        glowShape.setScale(1.f, 1.f);
    }

    const auto darknessAlpha = static_cast<sf::Uint8>(nightDarkness() * 115.f);
    if (darknessAlpha > 0) {
        nightOverlay.setPosition(viewRect.left, viewRect.top);
        nightOverlay.setSize({viewRect.width, viewRect.height});
        nightOverlay.setFillColor(sf::Color(8, 10, 30, darknessAlpha));
        window.draw(nightOverlay);
    }

    world.drawLights(window, viewRect);

    if (hasDamageFont) {
        for (const DamageText& d : damageTexts) {
            const auto alpha = static_cast<sf::Uint8>(255.f * std::clamp(d.life / 0.4f, 0.f, 1.f));
            damageLabel.setCharacterSize(d.crit ? 26 : 19);
            damageLabel.setString(std::to_string(d.value));
            const sf::FloatRect bounds = damageLabel.getLocalBounds();
            damageLabel.setOrigin(bounds.left + bounds.width / 2.f, bounds.top + bounds.height);
            damageLabel.setPosition(std::floor(d.pos.x), std::floor(d.pos.y));
            if (d.crit)
                damageLabel.setFillColor(sf::Color(255, 240, 80, alpha));
            else if (d.value > 0)
                damageLabel.setFillColor(sf::Color(255, 200, 90, alpha));
            else
                damageLabel.setFillColor(sf::Color(170, 170, 170, alpha));
            damageLabel.setOutlineColor(sf::Color(25, 20, 15, alpha));
            window.draw(damageLabel);
        }
    }

    window.setView(uiView);
    const sf::Vector2f screen(static_cast<float>(WINDOW_W), static_cast<float>(WINDOW_H));
    if (critFlash > 0.f) {
        sf::RectangleShape flash(screen);
        flash.setFillColor(sf::Color(255, 255, 255,
            static_cast<sf::Uint8>(45.f * std::min(1.f, critFlash / 0.07f))));
        window.draw(flash);
    }
    if (hurtFlash > 0.f) {
        sf::RectangleShape flash(screen);
        flash.setFillColor(sf::Color(200, 20, 20,
            static_cast<sf::Uint8>(60.f * std::min(1.f, hurtFlash / 0.15f))));
        window.draw(flash);
    }
    const float hpFrac = static_cast<float>(std::max(0, game->getPlayer().getHp()))
                         / static_cast<float>(stats.maxHp);
    if (state != GameState::Menu && hpFrac < 0.35f) {
        const float strength = (0.35f - hpFrac) / 0.35f;
        const auto va = static_cast<sf::Uint8>(
            (50.f + 28.f * std::sin(effectTime * 6.f)) * strength);
        const sf::Color outer(160, 10, 10, va);
        const sf::Color inner(160, 10, 10, 0);
        const float ix = screen.x * 0.24f;
        const float iy = screen.y * 0.24f;
        sf::VertexArray ring(sf::Triangles);
        auto quad = [&ring](sf::Vector2f a, sf::Vector2f b, sf::Vector2f c, sf::Vector2f d,
                            sf::Color ca, sf::Color cb, sf::Color cc, sf::Color cd) {
            ring.append({a, ca}); ring.append({b, cb}); ring.append({c, cc});
            ring.append({a, ca}); ring.append({c, cc}); ring.append({d, cd});
        };
        quad({0, 0}, {screen.x, 0}, {screen.x - ix, iy}, {ix, iy},
             outer, outer, inner, inner);
        quad({screen.x, 0}, {screen.x, screen.y}, {screen.x - ix, screen.y - iy},
             {screen.x - ix, iy}, outer, outer, inner, inner);
        quad({screen.x, screen.y}, {0, screen.y}, {ix, screen.y - iy},
             {screen.x - ix, screen.y - iy}, outer, outer, inner, inner);
        quad({0, screen.y}, {0, 0}, {ix, iy}, {ix, screen.y - iy},
             outer, outer, inner, inner);
        window.draw(ring);
    }
    window.setView(camera);

    HudState hudState;
    hudState.menu = state == GameState::Menu;
    hudState.hp = std::max(0, game->getPlayer().getHp());
    hudState.maxHp = stats.maxHp;
    hudState.waveProgress = xpNeeded > 0
        ? static_cast<float>(xp) / static_cast<float>(xpNeeded) : 0.f;
    hudState.ammo = weapon().getAmmo();
    hudState.maxAmmo = weapon().getMaxAmmo();
    hudState.weaponIcon = weaponDef().rect;
    hudState.weaponEvolved = (currentWeapon == 0 && stats.akEvolved)
                          || (currentWeapon == 1 && stats.shotgunEvolved)
                          || (currentWeapon == 2 && stats.pistolEvolved);
    hudState.grenades = grenadeCount;
    hudState.reloading = reloading;
    hudState.reloadProgress = reloadDuration > 0.f ? 1.f - reloadTimer / reloadDuration : 1.f;
    hudState.wave = game->getWave();
    hudState.score = game->getScore();
    hudState.kills = runKills;
    hudState.level = level;
    hudState.bestScore = std::max(bestScore, game->getScore());
    hudState.banner = (game->getWave() % 5 == 0 ? "BOSS WAVE " : "WAVE ")
                      + std::to_string(game->getWave());
    hudState.bannerAlpha = std::min(1.f, bannerTimer);
    hudState.paused = state == GameState::Paused;
    hudState.gameOver = state == GameState::GameOver;
    hudState.levelUp = state == GameState::LevelUp;
    hudState.pauseMenu = state == GameState::PauseMenu;
    hudState.menuIndex = pauseMenuIndex;
    hudState.musicOn = SoundManager::instance().isMusicEnabled();
    if (state == GameState::LevelUp)
        for (std::size_t i = 0; i < 3; i++)
            hudState.upgrades[i] = kUpgradeNames[upgradeChoices[i]];
    hud.draw(window, hudState, uiView);

    if (state != GameState::Menu)
        drawMinimap();

    if (fade > 0.f) {
        const sf::View previous = window.getView();
        window.setView(uiView);
        sf::RectangleShape black(screen);
        black.setFillColor(sf::Color(0, 0, 0,
            static_cast<sf::Uint8>(255.f * std::min(1.f, fade))));
        window.draw(black);
        window.setView(previous);
    }

    window.display();
}

void GameApp::run() {
    sf::Clock clock;
    while (window.isOpen()) {
        const float dt = std::min(clock.restart().asSeconds(), 0.1f);
        handleEvents();
        update(dt);
        render();
    }
}
