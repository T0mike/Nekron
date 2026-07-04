#include "../include/SoundManager.h"
#include "../include/TextureManager.h"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <functional>
#include <vector>

namespace {
constexpr unsigned int RATE = 44100;
constexpr float TAU = 6.2831853f;

std::vector<sf::Int16> synth(float duration, const std::function<float(float)>& wave) {
    const auto count = static_cast<std::size_t>(duration * static_cast<float>(RATE));
    std::vector<sf::Int16> samples(count);
    for (std::size_t i = 0; i < count; i++) {
        const float t = static_cast<float>(i) / static_cast<float>(RATE);
        const float v = std::clamp(wave(t), -1.f, 1.f);
        samples[i] = static_cast<sf::Int16>(v * 30000.f);
    }
    return samples;
}
}

SoundManager::SoundManager() : rng(1337u) {
    std::uniform_real_distribution<float> uni(-1.f, 1.f);
    auto noise = [&]() { return uni(rng); };

    auto add = [&](const std::string& id, float dur, const std::function<float(float)>& wave) {
        const auto samples = synth(dur, wave);
        buffers[id].loadFromSamples(samples.data(), samples.size(), 1, RATE);
    };

    add("shot_ak", 0.30f, [&, lp = 0.f, phase = 0.f](float t) mutable {
        const float n = noise();
        lp += 0.16f * (n - lp);
        const float freq = 55.f + 190.f * std::exp(-t * 22.f);
        phase += TAU * freq / static_cast<float>(RATE);
        const float crack = n * std::exp(-t * 120.f) * 1.3f;
        const float body = lp * std::exp(-t * 22.f) * 1.5f;
        const float kick = std::sin(phase) * std::exp(-t * 16.f) * 1.0f;
        return crack + body + kick;
    });
    add("shot_shotgun", 0.60f, [&, lp = 0.f, lp2 = 0.f, phase = 0.f](float t) mutable {
        const float n = noise();
        lp += 0.12f * (n - lp);
        lp2 += 0.04f * (n - lp2);
        const float freq = 42.f + 150.f * std::exp(-t * 14.f);
        phase += TAU * freq / static_cast<float>(RATE);
        const float crack = n * std::exp(-t * 70.f) * 1.2f;
        const float body = lp * std::exp(-t * 12.f) * 1.8f;
        const float boom = std::sin(phase) * std::exp(-t * 7.f) * 1.2f;
        const float tail = lp2 * std::exp(-t * 4.5f) * 0.9f;
        return crack + body + boom + tail;
    });
    add("shot_pistol", 0.24f, [&, lp = 0.f, phase = 0.f](float t) mutable {
        const float n = noise();
        lp += 0.22f * (n - lp);
        const float freq = 70.f + 240.f * std::exp(-t * 30.f);
        phase += TAU * freq / static_cast<float>(RATE);
        const float crack = n * std::exp(-t * 140.f) * 1.4f;
        const float body = lp * std::exp(-t * 30.f) * 1.1f;
        const float kick = std::sin(phase) * std::exp(-t * 24.f) * 0.8f;
        return crack + body + kick;
    });
    add("hit", 0.09f, [&](float t) {
        return std::sin(TAU * 95.f * t) * std::exp(-t * 40.f) * 0.9f
             + noise() * std::exp(-t * 60.f) * 0.35f;
    });
    add("death", 0.45f, [&](float t) {
        const float f = 160.f - 190.f * t;
        return std::sin(TAU * f * t + 2.5f * std::sin(TAU * 9.f * t)) * std::exp(-t * 6.f) * 0.8f
             + noise() * std::exp(-t * 12.f) * 0.3f;
    });
    add("reload", 0.16f, [&](float t) {
        const float a = t < 0.05f ? std::sin(TAU * 750.f * t) * std::exp(-t * 90.f) : 0.f;
        const float b = t > 0.09f ? std::sin(TAU * 950.f * (t - 0.09f)) * std::exp(-(t - 0.09f) * 90.f) : 0.f;
        return (a + b) * 0.7f;
    });
    add("hurt", 0.30f, [&](float t) {
        return std::sin(TAU * 65.f * t) * std::exp(-t * 10.f) * 0.95f
             + noise() * std::exp(-t * 25.f) * 0.25f;
    });
    add("wave", 0.9f, [&, lp = 0.f, ph1 = 0.f, ph2 = 0.f](float t) mutable {
        const float f1 = 38.f + 90.f * std::exp(-t * 18.f);
        ph1 += TAU * f1 / static_cast<float>(RATE);
        float drum = std::sin(ph1) * std::exp(-t * 10.f);
        const float t2 = t - 0.35f;
        if (t2 > 0.f) {
            const float f2 = 34.f + 80.f * std::exp(-t2 * 18.f);
            ph2 += TAU * f2 / static_cast<float>(RATE);
            drum += std::sin(ph2) * std::exp(-t2 * 8.f) * 1.1f;
        }
        const float n = noise();
        lp += 0.06f * (n - lp);
        return drum * 0.85f + lp * std::exp(-t * 3.f) * 0.5f;
    });
    add("dash", 0.22f, [&, lp = 0.f](float t) mutable {
        const float n = noise();
        lp += (0.05f + 0.5f * t / 0.22f) * (n - lp);
        const float env = std::sin(3.1415f * std::min(t / 0.22f, 1.f));
        return lp * env * 1.6f;
    });
    add("pickup", 0.18f, [&](float t) {
        const float a = t < 0.08f ? std::sin(TAU * 660.f * t) * std::exp(-t * 30.f) : 0.f;
        const float b = t > 0.07f ? std::sin(TAU * 990.f * (t - 0.07f)) * std::exp(-(t - 0.07f) * 30.f) : 0.f;
        return (a + b) * 0.55f;
    });
    add("spit", 0.18f, [&, lp = 0.f](float t) mutable {
        const float n = noise();
        lp += 0.30f * (n - lp);
        const float f = std::max(70.f, 320.f - 1100.f * t);
        return lp * std::exp(-t * 20.f) * 0.9f
             + std::sin(TAU * f * t) * std::exp(-t * 26.f) * 0.35f;
    });
    add("throw", 0.14f, [&, lp = 0.f](float t) mutable {
        const float n = noise();
        lp += (0.1f + 2.2f * t) * (n - lp);
        return lp * std::sin(3.1415f * std::min(t / 0.14f, 1.f)) * 1.1f;
    });
    add("explosion", 0.85f, [&, lp = 0.f, lp2 = 0.f, phase = 0.f](float t) mutable {
        const float n = noise();
        lp += 0.10f * (n - lp);
        lp2 += 0.03f * (n - lp2);
        const float f = 34.f + 130.f * std::exp(-t * 10.f);
        phase += TAU * f / static_cast<float>(RATE);
        const float crack = n * std::exp(-t * 50.f) * 1.1f;
        const float body = lp * std::exp(-t * 8.f) * 1.9f;
        const float boom = std::sin(phase) * std::exp(-t * 5.f) * 1.3f;
        const float rumble = lp2 * std::exp(-t * 3.f) * 1.0f;
        return crack + body + boom + rumble;
    });
    add("levelup", 0.55f, [&](float t) {
        const float a = std::sin(TAU * 523.f * t) * std::exp(-t * 8.f);
        const float b = t > 0.12f ? std::sin(TAU * 659.f * (t - 0.12f)) * std::exp(-(t - 0.12f) * 8.f) : 0.f;
        const float c = t > 0.24f ? std::sin(TAU * 784.f * (t - 0.24f)) * std::exp(-(t - 0.24f) * 6.f) : 0.f;
        return (a * 0.35f + b * 0.4f + c * 0.5f);
    });
}

void SoundManager::buildMusicLoop() {
    constexpr float BPM = 128.f;
    constexpr float SPB = 60.f / BPM;
    constexpr int BEATS = 16;
    constexpr float DUR = SPB * BEATS;
    const float noteTable[4] = {82.41f, 82.41f, 82.41f, 87.31f};
    const int gate[16] = {1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1, 0};

    std::uniform_real_distribution<float> uni(-1.f, 1.f);
    float sawPhase = 0.f;
    float kickPhase = 0.f;
    float lp = 0.f;
    float noiseLp = 0.f;

    const auto count = static_cast<std::size_t>(DUR * static_cast<float>(RATE));
    std::vector<sf::Int16> samples(count);
    for (std::size_t i = 0; i < count; i++) {
        const float t = static_cast<float>(i) / static_cast<float>(RATE);
        const float beat = t / SPB;
        const int bar = static_cast<int>(beat / 4.f) % 4;
        const float beatFrac = beat - std::floor(beat);
        const float sixteenth = beat * 4.f;
        const int sixIdx = static_cast<int>(sixteenth) % 16;
        const float sixFrac = sixteenth - std::floor(sixteenth);

        const float kickT = beatFrac * SPB;
        const float kickFreq = 40.f + 120.f * std::exp(-kickT * 30.f);
        kickPhase += TAU * kickFreq / static_cast<float>(RATE);
        const float kick = std::sin(kickPhase) * std::exp(-kickT * 9.f) * 0.9f;

        const float note = noteTable[bar] * (sixIdx % 8 == 7 ? 1.498f : 1.f);
        sawPhase += note / static_cast<float>(RATE);
        sawPhase -= std::floor(sawPhase);
        const float rawSaw = 2.f * sawPhase - 1.f;
        lp += 0.16f * (rawSaw - lp);
        const float chugEnv = gate[sixIdx] != 0 ? std::exp(-sixFrac * 9.f) : 0.f;
        const float chug = lp * chugEnv * 0.55f;

        const int beatInBar = static_cast<int>(beat) % 4;
        const float snareT = beatFrac * SPB;
        const float n = uni(rng);
        noiseLp += 0.35f * (n - noiseLp);
        const float snare = (beatInBar == 1 || beatInBar == 3)
            ? noiseLp * std::exp(-snareT * 22.f) * 0.5f : 0.f;

        const float sub = std::sin(TAU * 41.2f * t) * 0.10f;

        const float v = std::clamp((kick + chug + snare + sub) * 0.9f, -1.f, 1.f);
        samples[i] = static_cast<sf::Int16>(v * 27000.f);
    }
    musicBuffer.loadFromSamples(samples.data(), samples.size(), 1, RATE);
}

void SoundManager::startMusic() {
    if (musicStarted)
        return;
    musicStarted = true;

    for (const std::string& path : {std::string("assets/music.ogg"),
                                    TextureManager::baseDir() + "assets/music.ogg"}) {
        if (std::ifstream(path).good() && musicFile.openFromFile(path)) {
            musicFile.setLoop(true);
            musicFile.setVolume(45.f);
            usingMusicFile = true;
            if (musicOn)
                musicFile.play();
            return;
        }
    }

    buildMusicLoop();
    musicVoice.setBuffer(musicBuffer);
    musicVoice.setLoop(true);
    musicVoice.setVolume(30.f);
    if (musicOn)
        musicVoice.play();
}

void SoundManager::setMusicEnabled(bool enabled) {
    musicOn = enabled;
    if (!musicStarted)
        return;
    if (usingMusicFile) {
        if (enabled) musicFile.play();
        else musicFile.pause();
    } else {
        if (enabled) musicVoice.play();
        else musicVoice.pause();
    }
}

bool SoundManager::isMusicEnabled() const {
    return musicOn;
}

SoundManager& SoundManager::instance() {
    static SoundManager mgr;
    return mgr;
}

void SoundManager::play(const std::string& id, float volume, float pitchJitter) {
    auto it = buffers.find(id);
    if (it == buffers.end())
        return;
    sf::Sound& voice = voices[nextVoice];
    nextVoice = (nextVoice + 1) % voices.size();
    voice.setBuffer(it->second);
    voice.setVolume(volume);
    if (pitchJitter > 0.f) {
        std::uniform_real_distribution<float> jitter(1.f - pitchJitter, 1.f + pitchJitter);
        voice.setPitch(jitter(rng));
    } else {
        voice.setPitch(1.f);
    }
    voice.play();
}
