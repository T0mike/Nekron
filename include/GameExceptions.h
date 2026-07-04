#ifndef GAME_EXCEPTIONS_H
#define GAME_EXCEPTIONS_H
#include <exception>
#include <string>

class GameException : public std::exception {
    std::string message;
public:
    explicit GameException(const std::string& msg) : message(msg) {}
    const char* what() const noexcept override { return message.c_str(); }
};

class GameOverException : public GameException {
public:
    explicit GameOverException(const std::string& msg) : GameException(msg) {}
};

class OutOfAmmoException : public GameException {
public:
    explicit OutOfAmmoException(const std::string& msg) : GameException(msg) {}
};

class InvalidWeaponException : public GameException {
public:
    explicit InvalidWeaponException(const std::string& msg) : GameException(msg) {}
};

class AssetLoadException : public GameException {
public:
    explicit AssetLoadException(const std::string& path)
        : GameException("Failed to load asset: " + path) {}
};

#endif