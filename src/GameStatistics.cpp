#include "../include/GameStatistics.h"

GameStatistics::GameStatistics() : totalKills(0), totalScore(0), highestWave(1) {}

GameStatistics& GameStatistics::instance() {
    static GameStatistics inst;
    return inst;
}

void GameStatistics::recordKill(int points) {
    totalKills++;
    totalScore += points;
}

void GameStatistics::recordWave(int wave) {
    if (wave > highestWave)
        highestWave = wave;
}

void GameStatistics::reset() {
    totalKills = 0;
    totalScore = 0;
    highestWave = 1;
}

int GameStatistics::getTotalKills() const { return totalKills; }
int GameStatistics::getTotalScore() const { return totalScore; }
int GameStatistics::getHighestWave() const { return highestWave; }

std::ostream& operator<<(std::ostream& os, const GameStatistics& s) {
    os << "=== Game Statistics ===\n";
    os << "total kills: " << s.totalKills << "\n";
    os << "total score: " << s.totalScore << "\n";
    os << "highest wave: " << s.highestWave << "\n";
    return os;
}
