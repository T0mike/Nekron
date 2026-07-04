#include "../include/ZombieFactory.h"
#include "../include/ZombieNormal.h"
#include "../include/ZombieTank.h"
#include "../include/ZombieBoss.h"
#include "../include/ZombieRunner.h"
#include "../include/ZombieSpitter.h"
#include "../include/GameExceptions.h"

std::unique_ptr<Zombie> ZombieFactory::create(ZombieType type, const std::string& name,
                                             double x, double y) {
    switch (type) {
        case ZombieType::Normal:
            return std::make_unique<ZombieNormal>(name, 100, 5.0, x, y, 10);
        case ZombieType::Tank:
            return std::make_unique<ZombieTank>(name, x, y);
        case ZombieType::Boss:
            return std::make_unique<ZombieBoss>(name, x, y);
        case ZombieType::Runner:
            return std::make_unique<ZombieRunner>(name, x, y);
        case ZombieType::Spitter:
            return std::make_unique<ZombieSpitter>(name, x, y);
    }
    throw GameException("Tip de zombie necunoscut in ZombieFactory!");
}
