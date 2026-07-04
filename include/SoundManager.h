#ifndef SOUND_MANAGER_H
#define SOUND_MANAGER_H
#include <SFML/Audio.hpp>
#include <array>
#include <map>
#include <random>
#include <string>

class SoundManager {
    std::map<std::string, sf::SoundBuffer> buffers;
    std::array<sf::Sound, 24> voices;
    std::size_t nextVoice = 0;
    std::mt19937 rng;
    sf::Music musicFile;
    sf::SoundBuffer musicBuffer;
    sf::Sound musicVoice;
    bool musicStarted = false;
    bool musicOn = true;
    bool usingMusicFile = false;

    SoundManager();
    void buildMusicLoop();
public:
    SoundManager(const SoundManager&) = delete;
    SoundManager& operator=(const SoundManager&) = delete;

    static SoundManager& instance();

    void play(const std::string& id, float volume = 100.f, float pitchJitter = 0.f);
    void startMusic();
    void setMusicEnabled(bool enabled);
    bool isMusicEnabled() const;
};

#endif
