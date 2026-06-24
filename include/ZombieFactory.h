#ifndef ZOMBIE_FACTORY_H
#define ZOMBIE_FACTORY_H
#include <memory>
#include <string>
#include "Zombie.h"

enum class ZombieType { Normal, Tank, Boss, Runner };

class ZombieFactory {
public:
    static std::unique_ptr<Zombie> create(ZombieType type, const std::string& name,
                                          double x, double y);
};

#endif
